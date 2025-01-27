/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "udf_decl.h"
#include "fs.h"
#include "udf_fs.h"
#include "buffer_head.h"
#include "udf_fs_i.h"
#include "udf_sb.h"

#if defined CONFIG_FSYS_UDFFS_RW
static void extent_trunc(struct inode *inode, struct extent_position *epos,
                         struct kernel_lb_addr *eloc, int8_t etype, uint32_t elen,
                         uint32_t nelen)
{
    struct kernel_lb_addr neloc = {};
    int last_block = (elen + inode->i_sb->s_blocksize - 1) >>
                     inode->i_sb->s_blocksize_bits;
    int first_block = (nelen + inode->i_sb->s_blocksize - 1) >>
                      inode->i_sb->s_blocksize_bits;

    if (nelen)
    {
        if (etype == (EXT_NOT_RECORDED_ALLOCATED >> 30))
        {
            udf_free_blocks(inode->i_sb, inode, eloc, 0,
                            last_block);
            etype = (EXT_NOT_RECORDED_NOT_ALLOCATED >> 30);
        }
        else
        {
            neloc = *eloc;
        }
        nelen = (etype << 30) | nelen;
    }

    if (elen != nelen)
    {
        udf_write_aext(inode, epos, &neloc, nelen, 0);
        if (last_block - first_block > 0)
        {
            if (etype == (EXT_RECORDED_ALLOCATED >> 30))
            {
                mark_inode_dirty(inode);
            }

            if (etype != (EXT_NOT_RECORDED_NOT_ALLOCATED >> 30))
                udf_free_blocks(inode->i_sb, inode, eloc,
                                first_block,
                                last_block - first_block);
        }
    }
}

/*
 * Truncate the last extent to match i_size. This function assumes
 * that preallocation extent is already truncated.
 */
void udf_truncate_tail_extent(struct inode *inode)
{
    struct extent_position epos = {};
    struct kernel_lb_addr eloc;
    uint32_t elen, nelen;
    uint64_t lbcount = 0;
    int8_t etype = -1, netype;
    int adsize = 0;
    struct udf_inode_info *iinfo = UDF_I(inode);

    if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB ||
        inode->i_size == iinfo->i_lenExtents)
    {
        return;
    }
    /* Are we going to delete the file anyway? */
    if (inode->i_nlink == 0)
    {
        return;
    }

    if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
    {
        adsize = sizeof(struct short_ad);
    }
    else if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
    {
        adsize = sizeof(struct long_ad);
    }
    else
    {
        BUG();
    }

    /* Find the last extent in the file */
    while ((netype = udf_next_aext(inode, &epos, &eloc, &elen, 1)) != -1)
    {
        etype = netype;
        lbcount += elen;
        if (lbcount > inode->i_size)
        {
            if (lbcount - inode->i_size >= inode->i_sb->s_blocksize)
                udf_warning("udf_truncate_tail_extent(): Too long "
                            "extent after EOF in inode %u: i_size: "
                            "%Ld lbcount: %Ld extent %u+%u\n",
                            (unsigned)inode->i_ino,
                            (long long)inode->i_size,
                            (long long)lbcount,
                            (unsigned)eloc.logicalBlockNum,
                            (unsigned)elen);
            nelen = elen - (lbcount - inode->i_size);
            epos.offset -= adsize;
            extent_trunc(inode, &epos, &eloc, etype, elen, nelen);
            epos.offset += adsize;
            if (udf_next_aext(inode, &epos, &eloc, &elen, 1) != -1)
                udf_error("udf_truncate_tail_extent(): "
                          "Extent after EOF in inode %u.\n",
                          (unsigned)inode->i_ino);
            break;
        }
    }
    /* This inode entry is in-memory only and thus we don't have to mark
     * the inode dirty */
    iinfo->i_lenExtents = inode->i_size;
    brelse(epos.bh);
}

void udf_discard_prealloc(struct inode *inode)
{
    struct extent_position epos = { NULL, 0, {0, 0} };
    struct kernel_lb_addr eloc;
    uint32_t elen;
    uint64_t lbcount = 0;
    int8_t etype = -1, netype;
    int adsize;
    struct udf_inode_info *iinfo = UDF_I(inode);

    if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB ||
        inode->i_size == iinfo->i_lenExtents)
    {
        return;
    }

    if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
    {
        adsize = sizeof(struct short_ad);
    }
    else if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
    {
        adsize = sizeof(struct long_ad);
    }
    else
    {
        adsize = 0;
    }

    epos.block = iinfo->i_location;

    /* Find the last extent in the file */
    while ((netype = udf_next_aext(inode, &epos, &eloc, &elen, 1)) != -1)
    {
        etype = netype;
        lbcount += elen;
    }
    if (etype == (EXT_NOT_RECORDED_ALLOCATED >> 30))
    {
        epos.offset -= adsize;
        lbcount -= elen;
        extent_trunc(inode, &epos, &eloc, etype, elen, 0);
        if (!epos.bh)
        {
            iinfo->i_lenAlloc =
                epos.offset -
                udf_file_entry_alloc_offset(inode);
            mark_inode_dirty(inode);
        }
        else
        {
            struct allocExtDesc *aed =
                (struct allocExtDesc *)(epos.bh->b_data);
            aed->lengthAllocDescs =
                cpu_to_le32(epos.offset -
                            sizeof(struct allocExtDesc));
            if (!UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_STRICT) ||
                UDF_SB(inode->i_sb)->s_udfrev >= 0x0201)
            {
                udf_update_tag(epos.bh->b_data, epos.offset);
            }
            else
                udf_update_tag(epos.bh->b_data,
                               sizeof(struct allocExtDesc));
            mark_buffer_dirty_inode(epos.bh, inode);
        }
    }
    /* This inode entry is in-memory only and thus we don't have to mark
     * the inode dirty */
    iinfo->i_lenExtents = lbcount;
    brelse(epos.bh);
}

static void udf_update_alloc_ext_desc(struct inode *inode,
                                      struct extent_position *epos,
                                      u32 lenalloc)
{
    struct super_block *sb = inode->i_sb;
    struct udf_sb_info *sbi = UDF_SB(sb);

    struct allocExtDesc *aed = (struct allocExtDesc *)(epos->bh->b_data);
    int len = sizeof(struct allocExtDesc);

    aed->lengthAllocDescs = cpu_to_le32(lenalloc);
    if (!UDF_QUERY_FLAG(sb, UDF_FLAG_STRICT) || sbi->s_udfrev >= 0x0201)
    {
        len += lenalloc;
    }

    udf_update_tag(epos->bh->b_data, len);
    mark_buffer_dirty_inode(epos->bh, inode);
}

void udf_truncate_extents(struct inode *inode)
{
    struct extent_position epos;
    struct kernel_lb_addr eloc, neloc = {};
    uint32_t elen, nelen = 0, indirect_ext_len = 0, lenalloc;
    int8_t etype;
    struct super_block *sb = inode->i_sb;
    sector_t first_block = inode->i_size >> sb->s_blocksize_bits, offset;
    loff_t byte_offset;
    int adsize = 0;
    struct udf_inode_info *iinfo = UDF_I(inode);

    if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
    {
        adsize = sizeof(struct short_ad);
    }
    else if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
    {
        adsize = sizeof(struct long_ad);
    }
    else
    {
        BUG();
    }

    etype = inode_bmap(inode, first_block, &epos, &eloc, &elen, &offset);
    byte_offset = (offset << sb->s_blocksize_bits) +
                  (inode->i_size & (sb->s_blocksize - 1));
    if (etype != -1)
    {
        epos.offset -= adsize;
        extent_trunc(inode, &epos, &eloc, etype, elen, byte_offset);
        epos.offset += adsize;
        if (byte_offset)
        {
            lenalloc = epos.offset;
        }
        else
        {
            lenalloc = epos.offset - adsize;
        }

        if (!epos.bh)
        {
            lenalloc -= udf_file_entry_alloc_offset(inode);
        }
        else
        {
            lenalloc -= sizeof(struct allocExtDesc);
        }

        while ((etype = udf_current_aext(inode, &epos, &eloc,
                                         &elen, 0)) != -1)
        {
            if (etype == (EXT_NEXT_EXTENT_ALLOCDECS >> 30))
            {
                udf_write_aext(inode, &epos, &neloc, nelen, 0);
                if (indirect_ext_len)
                {
                    /* We managed to free all extents in the
                     * indirect extent - free it too */
                    BUG_ON(!epos.bh);
                    udf_free_blocks(sb, inode, &epos.block,
                                    0, indirect_ext_len);
                }
                else if (!epos.bh)
                {
                    iinfo->i_lenAlloc = lenalloc;
                    mark_inode_dirty(inode);
                }
                else
                    udf_update_alloc_ext_desc(inode,
                                              &epos, lenalloc);
                brelse(epos.bh);
                epos.offset = sizeof(struct allocExtDesc);
                epos.block = eloc;
                epos.bh = udf_tread(sb,
                                    udf_get_lb_pblock(sb, &eloc, 0));
                if (elen)
                    indirect_ext_len =
                        (elen + sb->s_blocksize - 1) >>
                        sb->s_blocksize_bits;
                else
                {
                    indirect_ext_len = 1;
                }
            }
            else
            {
                extent_trunc(inode, &epos, &eloc, etype,
                             elen, 0);
                epos.offset += adsize;
            }
        }

        if (indirect_ext_len)
        {
            BUG_ON(!epos.bh);
            udf_free_blocks(sb, inode, &epos.block, 0,
                            indirect_ext_len);
        }
        else if (!epos.bh)
        {
            iinfo->i_lenAlloc = lenalloc;
            mark_inode_dirty(inode);
        }
        else
        {
            udf_update_alloc_ext_desc(inode, &epos, lenalloc);
        }
    }
    else if (inode->i_size)
    {
        if (byte_offset)
        {
            struct kernel_long_ad extent;

            /*
             *  OK, there is not extent covering inode->i_size and
             *  no extent above inode->i_size => truncate is
             *  extending the file by 'offset' blocks.
             */
            if ((!epos.bh &&
                 epos.offset ==
                 udf_file_entry_alloc_offset(inode)) ||
                (epos.bh && epos.offset ==
                 sizeof(struct allocExtDesc)))
            {
                /* File has no extents at all or has empty last
                 * indirect extent! Create a fake extent... */
                extent.extLocation.logicalBlockNum = 0;
                extent.extLocation.partitionReferenceNum = 0;
                extent.extLength =
                    EXT_NOT_RECORDED_NOT_ALLOCATED;
            }
            else
            {
                epos.offset -= adsize;
                etype = udf_next_aext(inode, &epos,
                                      &extent.extLocation,
                                      &extent.extLength, 0);
                extent.extLength |= etype << 30;
            }
            udf_extend_file(inode, &epos, &extent,
                            offset +
                            ((inode->i_size &
                              (sb->s_blocksize - 1)) != 0));
        }
    }
    iinfo->i_lenExtents = inode->i_size;

    brelse(epos.bh);
}
#endif  /* FSYS_UDF_RW */
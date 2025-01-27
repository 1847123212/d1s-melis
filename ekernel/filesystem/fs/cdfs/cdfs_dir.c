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
#include "cdfs.h"
#include "page_pool.h"
#include "err.h"

int isofs_name_translate(struct iso_directory_record *de, char *new, struct inode *inode)
{
    char *old = de->name;
    int len = de->name_len[0];
    int i;

    for (i = 0; i < len; i++)
    {
        unsigned char c = old[i];
        if (!c)
        {
            break;
        }

        if (c >= 'A' && c <= 'Z')
        {
            c |= 0x20;    /* lower case */
        }

        /* Drop trailing '.;1' (ISO 9660:1988 7.5.1 requires period) */
        if (c == '.' && i == len - 3 && old[i + 1] == ';' && old[i + 2] == '1')
        {
            break;
        }

        /* Drop trailing ';1' */
        if (c == ';' && i == len - 2 && old[i + 1] == '1')
        {
            break;
        }

        /* Convert remaining ';' to '.' */
        /* Also '/' to '.' (broken Acorn-generated ISO9660 images) */
        if (c == ';' || c == '/')
        {
            c = '.';
        }

        new[i] = c;
    }
    return i;
}

/* Acorn extensions written by Matthew Wilcox <willy@bofh.ai> 1998 */
int get_acorn_filename(struct iso_directory_record *de,
                       char *retname, struct inode *inode)
{
    int std;
    unsigned char *chr;
    int retnamlen = isofs_name_translate(de, retname, inode);

    if (retnamlen == 0)
    {
        return 0;
    }
    std = sizeof(struct iso_directory_record) + de->name_len[0];
    if (std & 1)
    {
        std++;
    }
    if ((*((unsigned char *) de) - std) != 32)
    {
        return retnamlen;
    }
    chr = ((unsigned char *) de) + std;
    if (strncmp(chr, "ARCHIMEDES", 10))
    {
        return retnamlen;
    }
    if ((*retname == '_') && ((chr[19] & 1) == 1))
    {
        *retname = '!';
    }
    if (((de->flags[0] & 2) == 0) && (chr[13] == 0xff)
        && ((chr[12] & 0xf0) == 0xf0))
    {
        retname[retnamlen] = ',';
        sprintf(retname + retnamlen + 1, "%3.3x",
                ((chr[12] & 0xf) << 8) | chr[11]);
        retnamlen += 4;
    }
    return retnamlen;
}

/*
 * This should _really_ be cleaned up some day..
 */
static int do_isofs_readdir(struct inode *inode, struct file *filp,
                            void *dirent, filldir_t filldir,
                            char *tmpname, struct iso_directory_record *tmpde)
{
    unsigned long bufsize = ISOFS_BUFFER_SIZE(inode);
    unsigned char bufbits = ISOFS_BUFFER_BITS(inode);
    unsigned long block, offset, block_saved, offset_saved;
    unsigned long inode_number = 0; /* Quiet GCC */
    struct buffer_head *bh = NULL;
    int len;
    int map;
    int first_de = 1;
    char *p = NULL;     /* Quiet GCC */
    struct iso_directory_record *de;
    struct isofs_sb_info *sbi = ISOFS_SB(inode->i_sb);
    unsigned int dt_type = 0;

    offset = filp->f_pos & (bufsize - 1);
    block = filp->f_pos >> bufbits;

    while (filp->f_pos < inode->i_size)
    {
        int de_len;

        if (!bh)
        {
            bh = isofs_bread(inode, block);
            if (!bh)
            {
                return 0;
            }
        }

        de = (struct iso_directory_record *)(bh->b_data + offset);

        de_len = *(unsigned char *)de;

        /*
         * If the length byte is zero, we should move on to the next
         * CDROM sector.  If we are at the end of the directory, we
         * kick out of the while loop.
         */

        if (de_len == 0)
        {
            brelse(bh);
            bh = NULL;
            filp->f_pos = (filp->f_pos + ISOFS_BLOCK_SIZE) & ~(ISOFS_BLOCK_SIZE - 1);
            block = filp->f_pos >> bufbits;
            offset = 0;
            continue;
        }

        block_saved = block;
        offset_saved = offset;
        offset += de_len;

        /* Make sure we have a full directory entry */
        if (offset >= bufsize)
        {
            int slop = bufsize - offset + de_len;
            memcpy(tmpde, de, slop);
            offset &= bufsize - 1;
            block++;
            brelse(bh);
            bh = NULL;
            if (offset)
            {
                bh = isofs_bread(inode, block);
                if (!bh)
                {
                    return 0;
                }
                //              memcpy((void *)tmpde + slop, bh->b_data, offset);
                memcpy((char *)tmpde + slop, bh->b_data, offset);
            }
            de = tmpde;
        }

        if (first_de)
        {
            isofs_normalize_block_and_offset(de,
                                             &block_saved,
                                             &offset_saved);
            inode_number = isofs_get_ino(block_saved,
                                         offset_saved, bufbits);
        }

        if (de->flags[-sbi->s_high_sierra] & 0x80)
        {
            first_de = 0;
            filp->f_pos += de_len;
            continue;
        }
        first_de = 1;

        /* Handle the case of the '.' directory
         * NOTE : not support "." and ".." under epdk
         * just ignore it, By Sunny
         */
        if (de->name_len[0] == 1 && de->name[0] == 0)
        {
            //          if (filldir(dirent, ".", 1, filp->f_pos, inode->i_ino, DT_DIR) < 0)
            //              break;
            filp->f_pos += de_len;
            continue;
        }

        len = 0;

        /* Handle the case of the '..' directory
         * NOTE : not support "." and ".." under epdk
         * just ignore it, By Sunny
         */
        if (de->name_len[0] == 1 && de->name[0] == 1)
        {
            //          inode_number = parent_ino(filp->f_path.dentry);
            //          if (filldir(dirent, "..", 2, filp->f_pos, inode_number, DT_DIR) < 0)
            //              break;
            filp->f_pos += de_len;
            continue;
        }

        /* Handle everything else.  Do name translation if there
           is no Rock Ridge NM field. */

        /*
         * Do not report hidden files if so instructed, or associated
         * files unless instructed to do so
         */
        if ((sbi->s_hide == 'y' &&
             (de->flags[-sbi->s_high_sierra] & 1)) ||
            (sbi->s_showassoc == 'n' &&
             (de->flags[-sbi->s_high_sierra] & 4)))
        {
            filp->f_pos += de_len;
            continue;
        }

        map = 1;
        if (sbi->s_rock)
        {
            len = get_rock_ridge_filename(de, tmpname, inode);
            if (len != 0)       /* may be -1 */
            {
                p = tmpname;
                map = 0;
            }
        }
        if (map)
        {
#ifdef CONFIG_JOLIET
            if (sbi->s_joliet_level)
            {
                len = get_joliet_filename(de, tmpname, inode);
                p = tmpname;
            }
            else
#endif
                if (sbi->s_mapping == 'a')
                {
                    len = get_acorn_filename(de, tmpname, inode);
                    p = tmpname;
                }
                else if (sbi->s_mapping == 'n')
                {
                    len = isofs_name_translate(de, tmpname, inode);
                    p = tmpname;
                }
                else
                {
                    p = de->name;
                    len = de->name_len[0];
                }
        }
        if (len > 0)
        {
            if (de->flags[-sbi->s_high_sierra] & 2)
            {
                dt_type = FSYS_ATTR_DIRECTORY;
            }
            else
            {
                dt_type = 0;
            }
            if (filldir(dirent, p, len, 0, filp->f_pos,
                        inode_number, dt_type, /* size */0) < 0)
            {
                break;
            }
        }
        filp->f_pos += de_len;
        continue;
    }
    if (bh)
    {
        brelse(bh);
    }
    return 0;
}

/*
 * Handle allocation of temporary space for name translation and
 * handling split directory entries.. The real work is done by
 * "do_isofs_readdir()".
 */
static int isofs_readdir(struct file *filp,
                         void *dirent, filldir_t filldir)
{
    int result;
    char *tmpname;
    struct iso_directory_record *tmpde;
    struct inode *inode = filp->f_dentry->d_inode;

    /* maybe one page too bigger */
    tmpname = (char *)__get_free_page(GFP_KERNEL);
    if (tmpname == NULL)
    {
        return -ENOMEM;
    }

    //  lock_kernel();
    tmpde = (struct iso_directory_record *)(tmpname + 1024);

    result = do_isofs_readdir(inode, filp, dirent, filldir, tmpname, tmpde);

    free_page((unsigned long) tmpname);
    //  unlock_kernel();
    return result;
}

const struct file_operations isofs_dir_operations =
{
    //  .read = generic_read_dir,
    .readdir = isofs_readdir,
};

/*
 * directories can handle most operations...
 */
const struct inode_operations isofs_dir_inode_operations =
{
    .lookup = isofs_lookup,
};
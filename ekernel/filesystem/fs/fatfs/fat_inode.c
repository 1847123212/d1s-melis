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
#include "fatfs.h"
#include "buffer_head.h"
#include "endians.h"
#include "nls.h"
#include "errno.h"
#include "page_pool.h"
#include "slab.h"
#include "blk_dev.h"
#include "fsys_debug.h"
#include <string.h>

//#ifndef CONFIG_FAT_DEFAULT_IOCHARSET
///* if user don't select VFAT, this is undefined. */
//#define CONFIG_FAT_DEFAULT_IOCHARSET  "cp936"
//#endif

//#define CONFIG_FAT_DEFAULT_CODEPAGE 936
//static int fat_default_codepage = CONFIG_FAT_DEFAULT_CODEPAGE;
//static char fat_default_iocharset[] = CONFIG_FAT_DEFAULT_IOCHARSET;

extern struct inode_operations vfat_dir_inode_operations;
extern struct dentry_operations vfat_dentry_ops[4];

int __fat_get_block(struct inode *inode, int iblock,
                    unsigned int *max_blocks,
                    int *phys, int create)
{
    unsigned long mapped_blocks;
    struct super_block *sb = inode->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    unsigned int data_dclus, nr_newclus, sec_in_alloced_clus;
    unsigned int offset;
    struct msdos_inode_info *mi = MSDOS_I(inode);
    int sec_per_clus_bits = sbi->cluster_bits - sb->s_blocksize_bits;
    int err;

    debug_timerstart(bmaptime);
    offset = (unsigned long)iblock & (sbi->sec_per_clus - 1);

    err = fat_bmap(inode, iblock, phys, &mapped_blocks);
    if (err)
    {
        goto out;
    }
    if (*phys)
    {
        *max_blocks = min((unsigned int)mapped_blocks, *max_blocks);
        goto out;
    }

#if defined CONFIG_FSYS_FAT_RW
    if (!create)
    {
        goto out;
    }

    if (iblock != MSDOS_I(inode)->mmu_private >> sb->s_blocksize_bits)
    {
        fat_fs_panic(sb, "corrupted file size (i_pos %d, %d), iblock %d",
                     (int)MSDOS_I(inode)->i_pos, (int)MSDOS_I(inode)->mmu_private, iblock);
        err = -EIO;
        goto out;
    }

    sec_in_alloced_clus = offset ? min((sbi->sec_per_clus - offset), *max_blocks) : 0;
    nr_newclus = FSYS_ALIGN((*max_blocks - sec_in_alloced_clus), sbi->sec_per_clus)
                 >> sec_per_clus_bits;
    if (nr_newclus)
    {
        err = fat_add_cluster(inode, nr_newclus);
        if (err)
        {
            goto out;
        }
    }

    //  if (!offset) {
    //      /* TODO: multiple cluster allocation would be desirable. */
    //
    //        nr_newclus = *max_blocks >> sec_per_clus_bits;
    //      err = fat_add_cluster(inode, nr_newclus);
    //      if (err)
    //          goto out;
    //  }
    //  /* available blocks on this cluster */
    //  mapped_blocks = sbi->sec_per_clus - offset;
    //
    //  *max_blocks = min((unsigned int)mapped_blocks, *max_blocks);
    MSDOS_I(inode)->mmu_private += (sec_in_alloced_clus << sb->s_blocksize_bits) +
                                   (nr_newclus << sbi->cluster_bits);

    err = fat_bmap(inode, iblock, phys, &mapped_blocks);
    if (err)
    {
        goto out;
    }

    BUG_ON(!phys);

    /* mapped_blocks may be less than *max_blocks for added new clusters may not contiguous */
    *max_blocks = min((unsigned int)mapped_blocks, *max_blocks);

    //  set_buffer_new(bh_result);
    //  map_bh(bh_result, sb, phys);

    err = 0;
#endif  /* FSYS_FAT_RW */
out:
    debug_timerend(bmaptime);
    return err;
}

//static int fat_get_block(struct inode *inode, int iblock,
//           struct buffer_head *bh_result, int create)
//{
//  struct super_block *sb = inode->i_sb;
//  unsigned long max_blocks = bh_result->b_size >> inode->i_blkbits;
//  int err;
//
//  err = __fat_get_block(inode, iblock, &max_blocks, bh_result, create);
//  if (err)
//      return err;
//  bh_result->b_size = max_blocks << sb->s_blocksize_bits;
//  return 0;
//}
//
//static int fat_writepage(struct page *page, struct writeback_control *wbc)
//{
//  return block_write_full_page(page, fat_get_block, wbc);
//}
//
//static int fat_writepages(struct address_space *mapping,
//            struct writeback_control *wbc)
//{
//  return mpage_writepages(mapping, wbc, fat_get_block);
//}
//
//static int fat_readpage(struct file *file, struct page *page)
//{
//  return mpage_readpage(page, fat_get_block);
//}
//
//static int fat_readpages(struct file *file, struct address_space *mapping,
//           struct list_head *pages, unsigned nr_pages)
//{
//  return mpage_readpages(mapping, pages, nr_pages, fat_get_block);
//}
//
//static int fat_prepare_write(struct file *file, struct page *page,
//               unsigned from, unsigned to)
//{
//  return cont_prepare_write(page, from, to, fat_get_block,
//                &MSDOS_I(page->mapping->host)->mmu_private);
//}
//
//static int fat_commit_write(struct file *file, struct page *page,
//              unsigned from, unsigned to)
//{
//  struct inode *inode = page->mapping->host;
//  int err = generic_commit_write(file, page, from, to);
//  if (!err && !(MSDOS_I(inode)->i_attrs & ATTR_ARCH)) {
//      inode->i_mtime = inode->i_ctime = CURRENT_TIME_SEC;
//      MSDOS_I(inode)->i_attrs |= ATTR_ARCH;
//      mark_inode_dirty(inode);
//  }
//  return err;
//}
//
//static sunsigned int fat_direct_IO(int rw, struct kiocb *iocb,
//               const struct iovec *iov,
//               __s64 offset, unsigned long nr_segs)
//{
//  struct file *file = iocb->ki_filp;
//  struct inode *inode = file->f_mapping->host;
//
//  if (rw == WRITE) {
//      /*
//       * FIXME: blockdev_direct_IO() doesn't use ->prepare_write(),
//       * so we need to update the ->mmu_private to block boundary.
//       *
//       * But we must fill the remaining area or hole by nul for
//       * updating ->mmu_private.
//       *
//       * Return 0, and fallback to normal buffered write.
//       */
//      __s64 size = offset + iov_length(iov, nr_segs);
//      if (MSDOS_I(inode)->mmu_private < size)
//          return 0;
//  }
//
//  /*
//   * FAT need to use the DIO_LOCKING for avoiding the race
//   * condition of fat_get_block() and ->truncate().
//   */
//  return blockdev_direct_IO(rw, iocb, inode, inode->i_sb->s_bdev, iov,
//                offset, nr_segs, fat_get_block, NULL);
//}
//
//static int _fat_bmap(struct address_space *mapping, int block)
//{
//  return generic_block_bmap(mapping, block, fat_get_block);
//}

//static const struct address_space_operations fat_aops = {
//  .readpage   = fat_readpage,
//  .readpages  = fat_readpages,
//  .writepage  = fat_writepage,
//  .writepages = fat_writepages,
//  .sync_page  = block_sync_page,
//  .prepare_write  = fat_prepare_write,
//  .commit_write   = fat_commit_write,
//  .direct_IO  = fat_direct_IO,
//  .bmap       = _fat_bmap
//};

/*
 * New FAT inode stuff. We do the following:
 *  a) i_ino is constant and has nothing with on-disk location.
 *  b) FAT manages its own cache of directory entries.
 *  c) *This* cache is indexed by on-disk location.
 *  d) inode has an associated directory entry, all right, but
 *      it may be unhashed.
 *  e) currently entries are stored within struct inode. That should
 *      change.
 *  f) we deal with races in the following way:
 *      1. readdir() and lookup() do FAT-dir-cache lookup.
 *      2. rename() unhashes the F-d-c entry and rehashes it in
 *          a new place.
 *      3. unlink() and rmdir() unhash F-d-c entry.
 *      4. fat_write_inode() checks whether the thing is unhashed.
 *          If it is we silently return. If it isn't we do bread(),
 *          check if the location is still valid and retry if it
 *          isn't. Otherwise we do changes.
 *      5. Spinlock is used to protect hash/unhash/location check/lookup
 *      6. fat_clear_inode() unhashes the F-d-c entry.
 *      7. lookup() and readdir() do igrab() if they find a F-d-c entry
 *          and consider negative result as cache miss.
 */

static void fat_hash_init(struct super_block *sb)
{
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    int i;

    for (i = 0; i < FAT_HASH_SIZE; i++)
    {
        INIT_HLIST_HEAD(&sbi->inode_hashtable[i]);
    }
}

static   unsigned long fat_hash(struct super_block *sb, __s64 i_pos)
{
    unsigned long tmp = (unsigned long)i_pos | (unsigned long) sb;
    tmp = tmp + (tmp >> FAT_HASH_BITS) + (tmp >> FAT_HASH_BITS * 2);
    return tmp & FAT_HASH_MASK;
}

void fat_attach(struct inode *inode, __s64 i_pos)
{
    struct super_block *sb = inode->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);

    MSDOS_I(inode)->i_pos = i_pos;
    hlist_add_head(&MSDOS_I(inode)->i_fat_hash,
                   sbi->inode_hashtable + fat_hash(sb, i_pos));
}

struct inode *fat_iget(struct super_block *sb, __s64 i_pos)
{
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    struct hlist_head *head = sbi->inode_hashtable + fat_hash(sb, i_pos);
    struct hlist_node *_p;
    struct msdos_inode_info *i;
    struct inode *inode = NULL;

    hlist_for_each_entry(i, _p, head, i_fat_hash)
    {
        BUG_ON(i->vfs_inode.i_sb != sb);
        if (i->i_pos != i_pos)
        {
            continue;
        }
        inode = igrab(&i->vfs_inode);
        if (inode)
        {
            break;
        }
    }
    return inode;
}

static int is_exec(unsigned char *extension)
{
    unsigned char *exe_extensions = "EXECOMBAT", *walk;

    for (walk = exe_extensions; *walk; walk += 3)
        if (!strncmp(extension, walk, 3))
        {
            return 1;
        }
    return 0;
}

static int fat_calc_dir_size(struct inode *inode)
{
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
    int ret, fclus, dclus;

    inode->i_size = 0;
    if (MSDOS_I(inode)->i_start == 0)
    {
        return 0;
    }

    ret = fat_get_cluster(inode, FAT_ENT_EOF, &fclus, &dclus, NULL);
    if (ret < 0)
    {
        return ret;
    }
    inode->i_size = (fclus + 1) << sbi->cluster_bits;

    return 0;
}

/* doesn't deal with root inode */
static int fat_fill_inode(struct inode *inode, struct msdos_dir_entry *de)
{
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
    int error;

    MSDOS_I(inode)->i_pos = 0;
    inode->i_version++;
    inode->i_generation = 0;//get_seconds();

    if ((de->attr & ATTR_DIR) && !IS_FREE(de->name))
    {
        inode->i_generation &= ~1;
        inode->i_mode = MSDOS_MKMODE(de->attr,
                                     S_IRWXUGO & ~sbi->options.fs_dmask) | S_IFDIR;
        inode->i_op = sbi->dir_ops;
        inode->i_fop = &fat_dir_operations;

        MSDOS_I(inode)->i_start = le16_to_cpu(de->start);
        if (sbi->fat_bits == 32)
        {
            MSDOS_I(inode)->i_start |= (le16_to_cpu(de->starthi) << 16);
        }

        MSDOS_I(inode)->i_logstart = MSDOS_I(inode)->i_start;
        error = fat_calc_dir_size(inode);
        if (error < 0)
        {
            return error;
        }
        MSDOS_I(inode)->mmu_private = inode->i_size;
        MSDOS_I(inode)->dirent_search_start = 0;

        inode->i_nlink = fat_subdirs(inode);
    }
    else     /* not a directory */
    {
        inode->i_generation |= 1;
        inode->i_mode = MSDOS_MKMODE(de->attr,
                                     ((sbi->options.showexec &&
                                       !is_exec(de->ext))
                                      ? S_IRUGO | S_IWUGO : S_IRWXUGO)
                                     & ~sbi->options.fs_fmask) | S_IFREG;
        MSDOS_I(inode)->i_start = le16_to_cpu(de->start);
        if (sbi->fat_bits == 32)
        {
            MSDOS_I(inode)->i_start |= (le16_to_cpu(de->starthi) << 16);
        }

        MSDOS_I(inode)->i_logstart = MSDOS_I(inode)->i_start;
        inode->i_size = le32_to_cpu(de->size);
        inode->i_op = &fat_file_inode_operations;
        inode->i_fop = &fat_file_operations;
        MSDOS_I(inode)->mmu_private = inode->i_size;
    }
    if (de->attr & ATTR_SYS)
    {
        if (sbi->options.sys_immutable)
        {
            inode->i_flags |= S_IMMUTABLE;
        }
    }
    MSDOS_I(inode)->i_attrs = de->attr & ATTR_UNUSED;
    inode->i_blocks = ((inode->i_size + (sbi->cluster_size - 1))
                       & ~((__s64)sbi->cluster_size - 1)) >> 9;

    inode->i_mtime.tv_sec =
        date_dos2unix(le16_to_cpu(de->time), le16_to_cpu(de->date));
    inode->i_mtime.tv_nsec = 0;
    if (sbi->options.isvfat)
    {
        int secs = de->ctime_cs / 100;
        int csecs = de->ctime_cs % 100;
        inode->i_ctime.tv_sec  =
            date_dos2unix(le16_to_cpu(de->ctime),
                          le16_to_cpu(de->cdate)) + secs;
        inode->i_ctime.tv_nsec = csecs * 10000000;
        inode->i_atime.tv_sec =
            date_dos2unix(0, le16_to_cpu(de->adate));
        inode->i_atime.tv_nsec = 0;
    }
    else
    {
        inode->i_ctime = inode->i_atime = inode->i_mtime;
    }
    return 0;
}

struct inode *fat_build_inode(struct super_block *sb,
                              struct msdos_dir_entry *de, __s64 i_pos)
{
    struct inode *inode;
    int err;

    inode = fat_iget(sb, i_pos);
    if (inode)
    {
        goto out;
    }
    inode = new_inode(sb);
    if (!inode)
    {
        inode = ERR_PTR(-ENOMEM);
        goto out;
    }
    inode->i_ino = iunique(sb, MSDOS_ROOT_INO);
    inode->i_version = 1;
    err = fat_fill_inode(inode, de);
    if (err)
    {
        iput(inode);
        inode = ERR_PTR(err);
        goto out;
    }
    fat_attach(inode, i_pos);
    insert_inode_hash(inode);
out:
    return inode;
}

static void fat_clear_inode(struct inode *inode)
{
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);

    if (is_bad_inode(inode))
    {
        return;
    }
    fat_cache_inval_inode(inode);
    hlist_del_init(&MSDOS_I(inode)->i_fat_hash);
}

static void fat_put_super(struct super_block *sb)
{
    struct msdos_sb_info *sbi = MSDOS_SB(sb);

    sb->s_fs_info = NULL;
    free(sbi);
}

static kmem_cache_t *fat_inode_cachep;

static struct inode *fat_alloc_inode(struct super_block *sb)
{
    struct msdos_inode_info *ei;
    ei = kmem_cache_alloc(fat_inode_cachep, GFP_KERNEL);
    if (!ei)
    {
        return NULL;
    }
    return &ei->vfs_inode;
}


static void fat_destroy_inode(struct inode *inode)
{
    kmem_cache_free(fat_inode_cachep, MSDOS_I(inode));
}

static void init_once(void *foo, kmem_cache_t *cachep, unsigned long flags)
{
    struct msdos_inode_info *ei = (struct msdos_inode_info *)foo;

    ei->nr_caches = 0;
    ei->cache_valid_id = FAT_CACHE_VALID + 1;
    INIT_LIST_HEAD(&ei->cache_lru);
    INIT_HLIST_NODE(&ei->i_fat_hash);
    inode_init_once(&ei->vfs_inode);
}

int fat_init_inodecache(void)
{
    fat_inode_cachep = kmem_cache_create("fat_inode_cache",
                                         sizeof(struct msdos_inode_info),
                                         0, SLAB_HWCACHE_ALIGN,
                                         init_once, NULL);
    if (fat_inode_cachep == NULL)
    {
        return -ENOMEM;
    }
    return 0;
}

void fat_destroy_inodecache(void)
{
    kmem_cache_destroy(fat_inode_cachep);
}

static int fat_statfs(struct super_block *sb, struct kstatfs *buf, __u32 flags)
{
    struct msdos_sb_info *sbi = MSDOS_SB(sb);

    /* If the count of free cluster is still unknown,
     * counts it here, it is slowly.
     */
    if (flags & FSYS_KSTATUS_BFREE)
    {
        if ((int)sbi->free_clusters == -1)
        {
            int err = fat_count_free_clusters(sb);
            if (err)
            {
                return err;
            }
        }
    }

    buf->f_type = sb->s_magic;
    buf->f_bsize = sbi->cluster_size;
    buf->f_blocks = sbi->max_cluster - FAT_START_ENT;

    if (flags & FSYS_KSTATUS_BFREE)
    {
        buf->f_bfree = sbi->free_clusters;
    }
    else
    {
        buf->f_bfree = 0;
    }
    buf->f_namelen = sbi->options.isvfat ? 260 : 12;
    strncpy(buf->f_fsname, sb->s_fsname, MAX_FS_NAME_LEN);
    strncpy(buf->f_volname, sb->s_volname, MAX_FS_NAME_LEN);

    /* set fs attribute here */
    if (sb->s_flags & MS_RDONLY)
    {
        buf->f_attribute =  FS_ATTR_READONLY; /* readonly fs */
    }
    else
    {
        buf->f_attribute = 0;
    }
    return 0;
}

#if defined CONFIG_FSYS_FAT_RW
void fat_detach(struct inode *inode)
{
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
    MSDOS_I(inode)->i_pos = 0;
    hlist_del_init(&MSDOS_I(inode)->i_fat_hash);
}

int fat_add_cluster(struct inode *inode, int nr_newclus)
{
    int err, *cluster;

    /*
     * NOTE : if nr_newclus too large, kmalloc will fail,
     * the max valid size for kmalloc is 8K under epdk now.
     * so use system malloc to replace kmalloc.
     * By sunny  2010.1.29
     */
    //    cluster = kmalloc(nr_newclus<<2, 0);
    cluster = malloc(nr_newclus * sizeof(int));
    if (!cluster)
    {
        fs_log_error("allocate memory by system malloc failed\n");
        return -ENOMEM;
    }
    err = fat_alloc_clusters(inode, cluster, nr_newclus);
    if (err)
    {
        goto out;
    }

    /* FIXME: this cluster should be added after data of this
     * cluster is writed */
    err = fat_chain_add(inode, *cluster, nr_newclus);
    if (err)
    {
        fat_free_clusters(inode, *cluster);
    }

out:
    //    kfree(cluster);
    free(cluster);
    return err;
}

static __s32 fat_write_inode(struct inode *inode, __s32 wait)
{
    struct super_block *sb = inode->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    struct msdos_inode_info *mi = MSDOS_I(inode);
    struct buffer_head *bh;
    struct msdos_dir_entry *raw_entry;
    __s64 i_pos;
    int err = 0;

retry:
    i_pos = MSDOS_I(inode)->i_pos;
    if (inode->i_ino == MSDOS_ROOT_INO || !i_pos)
    {
        return 0;
    }

    bh = sb_bread(sb, i_pos >> sbi->dir_per_block_bits);
    if (!bh)
    {
        fs_log_error("FAT: unable to read inode block "
                     "for updating (i_pos %d)\n", (int)i_pos);
        err = -EIO;
        goto out;
    }
    if (i_pos != MSDOS_I(inode)->i_pos)
    {
        brelse(bh);
        goto retry;
    }

    raw_entry = &((struct msdos_dir_entry *)(bh->b_data))
                [i_pos & (sbi->dir_per_block - 1)];
    if (S_ISDIR(inode->i_mode))
    {
        raw_entry->size = 0;
    }
    else
    {
        raw_entry->size = cpu_to_le32(inode->i_size);
    }
    raw_entry->attr = fat_attr(inode);
    raw_entry->start = cpu_to_le16(MSDOS_I(inode)->i_logstart);
    raw_entry->starthi = cpu_to_le16(MSDOS_I(inode)->i_logstart >> 16);


    fat_date_unix2dos(inode->i_mtime.tv_sec, &raw_entry->time, &raw_entry->date);
    if (sbi->options.isvfat)
    {
        __le16 atime;
        fat_date_unix2dos(inode->i_ctime.tv_sec, &raw_entry->ctime, &raw_entry->cdate);
        fat_date_unix2dos(inode->i_atime.tv_sec, &atime, &raw_entry->adate);
        raw_entry->ctime_cs = (inode->i_ctime.tv_sec & 1) * 100 +
                              inode->i_ctime.tv_nsec / 10000000;
    }
    mark_buffer_dirty(bh);
    if (wait)
    {
        err = sync_dirty_buffer(bh);
    }
    brelse(bh);
out:
    return err;
}

static void fat_write_super(struct super_block *sb)
{
    sb->s_dirt = 0;

    if (!(sb->s_flags & MS_RDONLY))
    {
        fat_clusters_flush(sb);
    }
}


static void fat_delete_inode(struct inode *inode)
{
    //truncate_inode_pages(&inode->i_data, 0);

    if (!is_bad_inode(inode))
    {
        inode->i_size = 0;
        fat_truncate(inode);
    }
    clear_inode(inode);
}

int fat_sync_inode(struct inode *inode)
{
    return fat_write_inode(inode, 1);
}

/*
 * write data and metadata corresponding to i1 and i2.  The io is
 * started but we do not wait for any of it to finish.
 *
 * filemap_flush is used for the block device, so if there is a dirty
 * page for a block already in flight, we will not wait and start the
 * io over again
 */
int fat_flush_inodes(struct super_block *sb, struct inode *i1, struct inode *i2)
{
    int ret = 0;

    if (!MSDOS_SB(sb)->options.flush)
    {
        return 0;
    }
    if (i1)
    {
        ret = fat_write_inode(i1, 1);
    }
    if (!ret && i2)
    {
        ret = fat_write_inode(i2, 1);
    }

    return ret;
}
#endif

//static int fat_show_options(struct seq_file *m, struct vfsmount *mnt);
static const struct super_operations fat_sops =
{
    .alloc_inode    = fat_alloc_inode,
    .destroy_inode  = fat_destroy_inode,
    .put_super      = fat_put_super,
    .statfs         = fat_statfs,
    .clear_inode    = fat_clear_inode,
#if defined CONFIG_FSYS_FAT_RW
    .write_super    = fat_write_super,
    .write_inode    = fat_write_inode,
    .delete_inode   = fat_delete_inode,
#endif
};

///*
// * a FAT file handle with fhtype 3 is
// *  0/  i_ino - for fast, reliable lookup if still in the cache
// *  1/  i_generation - to see if i_ino is still valid
// *          bit 0 == 0 iff directory
// *  2/  i_pos(8-39) - if ino has changed, but still in cache
// *  3/  i_pos(4-7)|i_logstart - to semi-verify inode found at i_pos
// *  4/  i_pos(0-3)|parent->i_logstart - maybe used to hunt for the file on disc
// *
// * Hack for NFSv2: Maximum FAT entry number is 28bits and maximum
// * i_pos is 40bits (blocknr(32) + dir offset(8)), so two 4bits
// * of i_logstart is used to store the directory entry offset.
// */
//
//static struct dentry *
//fat_decode_fh(struct super_block *sb, __u32 *fh, int len, int fhtype,
//        int (*acceptable)(void *context, struct dentry *de),
//        void *context)
//{
//  if (fhtype != 3)
//      return ERR_PTR(-ESTALE);
//  if (len < 5)
//      return ERR_PTR(-ESTALE);
//
//  return sb->s_export_op->find_exported_dentry(sb, fh, NULL, acceptable, context);
//}
//
//static struct dentry *fat_get_dentry(struct super_block *sb, void *inump)
//{
//  struct inode *inode = NULL;
//  struct dentry *result;
//  __u32 *fh = inump;
//
//  inode = iget(sb, fh[0]);
//  if (!inode || is_bad_inode(inode) || inode->i_generation != fh[1]) {
//      if (inode)
//          iput(inode);
//      inode = NULL;
//  }
//  if (!inode) {
//      __s64 i_pos;
//      int i_logstart = fh[3] & 0x0fffffff;
//
//      i_pos = (__s64)fh[2] << 8;
//      i_pos |= ((fh[3] >> 24) & 0xf0) | (fh[4] >> 28);
//
//      /* try 2 - see if i_pos is in F-d-c
//       * require i_logstart to be the same
//       * Will fail if you truncate and then re-write
//       */
//
//      inode = fat_iget(sb, i_pos);
//      if (inode && MSDOS_I(inode)->i_logstart != i_logstart) {
//          iput(inode);
//          inode = NULL;
//      }
//  }
//  if (!inode) {
//      /* For now, do nothing
//       * What we could do is:
//       * follow the file starting at fh[4], and record
//       * the ".." entry, and the name of the fh[2] entry.
//       * The follow the ".." file finding the next step up.
//       * This way we build a path to the root of
//       * the tree. If this works, we lookup the path and so
//       * get this inode into the cache.
//       * Finally try the fat_iget lookup again
//       * If that fails, then weare totally out of luck
//       * But all that is for another day
//       */
//  }
//  if (!inode)
//      return ERR_PTR(-ESTALE);
//
//
//  /* now to find a dentry.
//   * If possible, get a well-connected one
//   */
//  result = d_alloc_anon(inode);
//  if (result == NULL) {
//      iput(inode);
//      return ERR_PTR(-ENOMEM);
//  }
//  result->d_op = sb->s_root->d_op;
//  return result;
//}
//
//static int
//fat_encode_fh(struct dentry *de, __u32 *fh, int *lenp, int connectable)
//{
//  int len = *lenp;
//  struct inode *inode =  de->d_inode;
//  __u32 ipos_h, ipos_m, ipos_l;
//
//  if (len < 5)
//      return 255; /* no room */
//
//  ipos_h = MSDOS_I(inode)->i_pos >> 8;
//  ipos_m = (MSDOS_I(inode)->i_pos & 0xf0) << 24;
//  ipos_l = (MSDOS_I(inode)->i_pos & 0x0f) << 28;
//  *lenp = 5;
//  fh[0] = inode->i_ino;
//  fh[1] = inode->i_generation;
//  fh[2] = ipos_h;
//  fh[3] = ipos_m | MSDOS_I(inode)->i_logstart;
//  spin_lock(&de->d_lock);
//  fh[4] = ipos_l | MSDOS_I(de->d_parent->d_inode)->i_logstart;
//  spin_unlock(&de->d_lock);
//  return 3;
//}
//
//static struct dentry *fat_get_parent(struct dentry *child)
//{
//  struct buffer_head *bh;
//  struct msdos_dir_entry *de;
//  __s64 i_pos;
//  struct dentry *parent;
//  struct inode *inode;
//  int err;
//
//  lock_kernel();
//
//  err = fat_get_dotdot_entry(child->d_inode, &bh, &de, &i_pos);
//  if (err) {
//      parent = ERR_PTR(err);
//      goto out;
//  }
//  inode = fat_build_inode(child->d_sb, de, i_pos);
//  brelse(bh);
//  if (IS_ERR(inode)) {
//      parent = ERR_PTR(PTR_ERR(inode));
//      goto out;
//  }
//  parent = d_alloc_anon(inode);
//  if (!parent) {
//      iput(inode);
//      parent = ERR_PTR(-ENOMEM);
//  }
//out:
//  unlock_kernel();
//
//  return parent;
//}
//
//static struct export_operations fat_export_ops = {
//  .decode_fh  = fat_decode_fh,
//  .encode_fh  = fat_encode_fh,
//  .get_dentry = fat_get_dentry,
//  .get_parent = fat_get_parent,
//};

//static int fat_show_options(struct seq_file *m, struct vfsmount *mnt)
//{
//  struct msdos_sb_info *sbi = MSDOS_SB(mnt->mnt_sb);
//  struct fat_mount_options *opts = &sbi->options;
//  int isvfat = opts->isvfat;
//
//  if (opts->fs_uid != 0)
//      seq_printf(m, ",uid=%u", opts->fs_uid);
//  if (opts->fs_gid != 0)
//      seq_printf(m, ",gid=%u", opts->fs_gid);
//  seq_printf(m, ",fmask=%04o", opts->fs_fmask);
//  seq_printf(m, ",dmask=%04o", opts->fs_dmask);
//  if (sbi->nls_disk)
//      seq_printf(m, ",codepage=%s", sbi->nls_disk->charset);
//  if (isvfat) {
//      if (sbi->nls_io)
//          seq_printf(m, ",iocharset=%s", sbi->nls_io->charset);
//
//      switch (opts->shortname) {
//      case VFAT_SFN_DISPLAY_WIN95 | VFAT_SFN_CREATE_WIN95:
//          seq_puts(m, ",shortname=win95");
//          break;
//      case VFAT_SFN_DISPLAY_WINNT | VFAT_SFN_CREATE_WINNT:
//          seq_puts(m, ",shortname=winnt");
//          break;
//      case VFAT_SFN_DISPLAY_WINNT | VFAT_SFN_CREATE_WIN95:
//          seq_puts(m, ",shortname=mixed");
//          break;
//      case VFAT_SFN_DISPLAY_LOWER | VFAT_SFN_CREATE_WIN95:
//          /* seq_puts(m, ",shortname=lower"); */
//          break;
//      default:
//          seq_puts(m, ",shortname=unknown");
//          break;
//      }
//  }
//  if (opts->name_check != 'n')
//      seq_printf(m, ",check=%c", opts->name_check);
//  if (opts->usefree)
//      seq_puts(m, ",usefree");
//  if (opts->quiet)
//      seq_puts(m, ",quiet");
//  if (opts->showexec)
//      seq_puts(m, ",showexec");
//  if (opts->sys_immutable)
//      seq_puts(m, ",sys_immutable");
//  if (!isvfat) {
//      if (opts->dotsOK)
//          seq_puts(m, ",dotsOK=yes");
//      if (opts->nocase)
//          seq_puts(m, ",nocase");
//  } else {
//      if (opts->utf8)
//          seq_puts(m, ",utf8");
//      if (opts->unicode_xlate)
//          seq_puts(m, ",uni_xlate");
//      if (!opts->numtail)
//          seq_puts(m, ",nonumtail");
//  }
//
//  return 0;
//}

//enum {
//  Opt_check_n, Opt_check_r, Opt_check_s, Opt_uid, Opt_gid,
//  Opt_umask, Opt_dmask, Opt_fmask, Opt_codepage, Opt_usefree, Opt_nocase,
//  Opt_quiet, Opt_showexec, Opt_debug, Opt_immutable,
//  Opt_dots, Opt_nodots,
//  Opt_charset, Opt_shortname_lower, Opt_shortname_win95,
//  Opt_shortname_winnt, Opt_shortname_mixed, Opt_utf8_no, Opt_utf8_yes,
//  Opt_uni_xl_no, Opt_uni_xl_yes, Opt_nonumtail_no, Opt_nonumtail_yes,
//  Opt_obsolate, Opt_flush, Opt_err,
//};
//
//static match_table_t fat_tokens = {
//  {Opt_check_r, "check=relaxed"},
//  {Opt_check_s, "check=strict"},
//  {Opt_check_n, "check=normal"},
//  {Opt_check_r, "check=r"},
//  {Opt_check_s, "check=s"},
//  {Opt_check_n, "check=n"},
//  {Opt_uid, "uid=%u"},
//  {Opt_gid, "gid=%u"},
//  {Opt_umask, "umask=%o"},
//  {Opt_dmask, "dmask=%o"},
//  {Opt_fmask, "fmask=%o"},
//  {Opt_codepage, "codepage=%u"},
//  {Opt_usefree, "usefree"},
//  {Opt_nocase, "nocase"},
//  {Opt_quiet, "quiet"},
//  {Opt_showexec, "showexec"},
//  {Opt_debug, "debug"},
//  {Opt_immutable, "sys_immutable"},
//  {Opt_obsolate, "conv=binary"},
//  {Opt_obsolate, "conv=text"},
//  {Opt_obsolate, "conv=auto"},
//  {Opt_obsolate, "conv=b"},
//  {Opt_obsolate, "conv=t"},
//  {Opt_obsolate, "conv=a"},
//  {Opt_obsolate, "fat=%u"},
//  {Opt_obsolate, "blocksize=%u"},
//  {Opt_obsolate, "cvf_format=%20s"},
//  {Opt_obsolate, "cvf_options=%100s"},
//  {Opt_obsolate, "posix"},
//  {Opt_flush, "flush"},
//  {Opt_err, NULL},
//};
//static match_table_t msdos_tokens = {
//  {Opt_nodots, "nodots"},
//  {Opt_nodots, "dotsOK=no"},
//  {Opt_dots, "dots"},
//  {Opt_dots, "dotsOK=yes"},
//  {Opt_err, NULL}
//};
//static match_table_t vfat_tokens = {
//  {Opt_charset, "iocharset=%s"},
//  {Opt_shortname_lower, "shortname=lower"},
//  {Opt_shortname_win95, "shortname=win95"},
//  {Opt_shortname_winnt, "shortname=winnt"},
//  {Opt_shortname_mixed, "shortname=mixed"},
//  {Opt_utf8_no, "utf8=0"},        /* 0 or no or false */
//  {Opt_utf8_no, "utf8=no"},
//  {Opt_utf8_no, "utf8=false"},
//  {Opt_utf8_yes, "utf8=1"},       /* empty or 1 or yes or true */
//  {Opt_utf8_yes, "utf8=yes"},
//  {Opt_utf8_yes, "utf8=true"},
//  {Opt_utf8_yes, "utf8"},
//  {Opt_uni_xl_no, "uni_xlate=0"},     /* 0 or no or false */
//  {Opt_uni_xl_no, "uni_xlate=no"},
//  {Opt_uni_xl_no, "uni_xlate=false"},
//  {Opt_uni_xl_yes, "uni_xlate=1"},    /* empty or 1 or yes or true */
//  {Opt_uni_xl_yes, "uni_xlate=yes"},
//  {Opt_uni_xl_yes, "uni_xlate=true"},
//  {Opt_uni_xl_yes, "uni_xlate"},
//  {Opt_nonumtail_no, "nonumtail=0"},  /* 0 or no or false */
//  {Opt_nonumtail_no, "nonumtail=no"},
//  {Opt_nonumtail_no, "nonumtail=false"},
//  {Opt_nonumtail_yes, "nonumtail=1"}, /* empty or 1 or yes or true */
//  {Opt_nonumtail_yes, "nonumtail=yes"},
//  {Opt_nonumtail_yes, "nonumtail=true"},
//  {Opt_nonumtail_yes, "nonumtail"},
//  {Opt_err, NULL}
//};
//
///**
// * strsep - Split a string into tokens
// * @s: The string to be searched
// * @ct: The characters to search for
// *
// * strsep() updates @s to point after the token, ready for the next call.
// *
// * It returns empty tokens, too, behaving exactly like the libc function
// * of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
// * Same semantics, slimmer shape. ;)
// */
//char *strsep(char **s, const char *ct)
//{
//  char *sbegin = *s;
//  char *end;
//
//  if (sbegin == NULL)
//      return NULL;
//
//  end = strpbrk(sbegin, ct);
//  if (end)
//      *end++ = '\0';
//  *s = end;
//  return sbegin;
//}
static int fat_parse_options(char *options, int is_vfat, int silent, int *debug,
                             struct fat_mount_options *opts)
{
    opts->isvfat = is_vfat;

    //opts->codepage = fat_default_codepage;
    //opts->iocharset = fat_default_iocharset;
    if (is_vfat)
    {
        opts->shortname = VFAT_SFN_DISPLAY_WINNT | VFAT_SFN_CREATE_WINNT;
    }
    else
    {
        opts->shortname = 0;
    }
    opts->name_check = 'n';
    opts->quiet = opts->showexec = opts->sys_immutable = opts->dotsOK =  0;
    opts->utf8 = opts->unicode_xlate = 0;
    opts->numtail = 1;
    opts->usefree = 1;
    opts->nocase = 0;
    opts->flush = 1;
    *debug = 0;

    return 0;
}

static int fat_read_root(struct inode *inode)
{
    struct super_block *sb = inode->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    int error;

    MSDOS_I(inode)->i_pos = 0;
    inode->i_version++;
    inode->i_generation = 0;
    inode->i_mode = (S_IRWXUGO & ~sbi->options.fs_dmask) | S_IFDIR;
    inode->i_op = sbi->dir_ops;
    inode->i_fop = &fat_dir_operations;
    if (sbi->fat_bits == 32)
    {
        MSDOS_I(inode)->i_start = sbi->root_cluster;
        error = fat_calc_dir_size(inode);
        if (error < 0)
        {
            return error;
        }
    }
    else
    {
        MSDOS_I(inode)->i_start = 0;
        inode->i_size = sbi->dir_entries * sizeof(struct msdos_dir_entry);
    }
    inode->i_blocks = ((inode->i_size + (sbi->cluster_size - 1))
                       & ~((__s64)sbi->cluster_size - 1)) >> 9;
    MSDOS_I(inode)->i_logstart = 0;
    MSDOS_I(inode)->mmu_private = inode->i_size;
    MSDOS_I(inode)->dirent_search_start = 0;

    MSDOS_I(inode)->i_attrs = ATTR_NONE;
    inode->i_mtime.tv_sec = inode->i_atime.tv_sec = inode->i_ctime.tv_sec = 0;
    inode->i_mtime.tv_nsec = inode->i_atime.tv_nsec = inode->i_ctime.tv_nsec = 0;
    inode->i_nlink = fat_subdirs(inode) + 2;

    return 0;
}

/*
 * Read the super block of an MS-DOS FS.
 */
__s32 fat_fill_super(struct super_block *sb, void *data, __s32 silent)
{
    int isvfat = 1;
    struct inode *root_inode = NULL;
    struct buffer_head *bh;
    struct fat_boot_sector *b;
    struct msdos_sb_info *sbi;
    __u16 logical_sector_size;
    __u32 total_sectors, total_clusters, fat_clusters, rootdir_sectors;
    int debug;
    unsigned int media;
    long error;
    char buf[50];

    sbi = calloc(sizeof(struct msdos_sb_info), 1);
    if (!sbi)
    {
        return -ENOMEM;
    }
    sb->s_fs_info = sbi;

    sb->s_flags |= MS_NODIRATIME | MS_SYNCHRONOUS;   //debug for MS_SYNCHRONOUS
    sb->s_magic = MSDOS_SUPER_MAGIC;
    sb->s_op = &fat_sops;
    //  sb->s_export_op = &fat_export_ops;
    sbi->dir_ops = &vfat_dir_inode_operations;

    error = fat_parse_options(data, isvfat, silent, &debug, &sbi->options);
    if (error)
    {
        goto out_fail;
    }

    error = -EIO;
    sb_min_blocksize(sb, 512);
    //    sb->s_blocksize_bits = 9;
    //    sb->s_blocksize = 512;
    bh = sb_bread(sb, 0);
    if (bh == NULL)
    {
        fs_log_error("FAT: unable to read boot sector\n");
        goto out_fail;
    }

    b = (struct fat_boot_sector *) bh->b_data;
    if (!b->reserved)
    {
        if (!silent)
        {
            fs_log_error("FAT: bogus number of reserved sectors\n");
        }
        brelse(bh);
        goto out_invalid;
    }
    if (!b->fats)
    {
        if (!silent)
        {
            fs_log_error("FAT: bogus number of FAT structure\n");
        }
        brelse(bh);
        goto out_invalid;
    }

    /*
     * Earlier we checked here that b->secs_track and b->head are nonzero,
     * but it turns out valid FAT filesystems can have zero there.
     */

    media = b->media;
    if (!FAT_VALID_MEDIA(media))
    {
        if (!silent)
            fs_log_error("FAT: invalid media value (0x%02x)\n",
                         media);
        brelse(bh);
        goto out_invalid;
    }
    logical_sector_size = b->sector_size[0];
    logical_sector_size |= b->sector_size[1] << 8;
    if (!is_power_of_2(logical_sector_size)
        || (logical_sector_size < 512)
        || (PAGE_CACHE_SIZE < logical_sector_size))
    {
        if (!silent)
            fs_log_error("FAT: bogus logical sector size %u\n",
                         logical_sector_size);
        brelse(bh);
        goto out_invalid;
    }
    sbi->sec_per_clus = b->sec_per_clus;
    if (!is_power_of_2(sbi->sec_per_clus))
    {
        if (!silent)
            fs_log_error("FAT: bogus sectors per cluster %u\n",
                         sbi->sec_per_clus);
        brelse(bh);
        goto out_invalid;
    }

    if (logical_sector_size < sb->s_blocksize)
    {
        fs_log_error("FAT: logical sector size too small for device"
                     " (logical sector size = %u)\n", logical_sector_size);
        brelse(bh);
        goto out_fail;
    }
    if (logical_sector_size > sb->s_blocksize)
    {
        brelse(bh);

        if (!sb_set_blocksize(sb, logical_sector_size))
        {
            fs_log_error("FAT: unable to set blocksize %u\n",
                         logical_sector_size);
            goto out_fail;
        }
        bh = sb_bread(sb, 0);
        if (bh == NULL)
        {
            fs_log_error("FAT: unable to read boot sector"
                         " (logical sector size = %lu)\n",
                         sb->s_blocksize);
            goto out_fail;
        }
        b = (struct fat_boot_sector *) bh->b_data;
    }

    sbi->cluster_size = sb->s_blocksize * sbi->sec_per_clus;
    sbi->cluster_bits = ffs(sbi->cluster_size) - 1;
    sbi->fats = b->fats;
    sbi->fat_bits = 0;      /* Don't know yet */
    sbi->fat_start = le16_to_cpu(b->reserved);
    sbi->fat_length = le16_to_cpu(b->fat_length);
    sbi->root_cluster = 0;
    sbi->free_clusters = -1;    /* Don't know yet */
    sbi->prev_free = FAT_START_ENT;

    if (!sbi->fat_length && b->fat32_length)
    {
        struct fat_boot_fsinfo *fsinfo;
        struct buffer_head *fsinfo_bh;

        /* Must be FAT32 */
        sbi->fat_bits = 32;
        sbi->fat_length = le32_to_cpu(b->fat32_length);
        sbi->root_cluster = le32_to_cpu(b->root_cluster);

        sb->s_maxbytes = 0xffffffff;

        /* MC - if info_sector is 0, don't multiply by 0 */
        sbi->fsinfo_sector = le16_to_cpu(b->info_sector);
        if (sbi->fsinfo_sector == 0)
        {
            sbi->fsinfo_sector = 1;
        }

        fsinfo_bh = sb_bread(sb, sbi->fsinfo_sector);
        if (fsinfo_bh == NULL)
        {
            fs_log_error("FAT: bread failed, FSINFO block"
                         " (sector = %lu)\n", sbi->fsinfo_sector);
            brelse(bh);
            goto out_fail;
        }

        fsinfo = (struct fat_boot_fsinfo *)fsinfo_bh->b_data;
        if (!IS_FSINFO(fsinfo))
        {
            fs_log_warning("FAT: Did not find valid FSINFO signature.\n"
                           "     Found signature1 0x%08x signature2 0x%08x"
                           " (sector = %lu)\n",
                           le32_to_cpu(fsinfo->signature1),
                           le32_to_cpu(fsinfo->signature2),
                           sbi->fsinfo_sector);
        }
        else
        {
            if (sbi->options.usefree)
            {
                sbi->free_clusters =
                    le32_to_cpu(fsinfo->free_clusters);
                if (((int)(sbi->free_clusters) <= 0) ||
                    ((sbi->free_clusters) == 0xffffffff))
                {
                    sbi->free_clusters = -1;
                }
            }
            sbi->prev_free =
                le32_to_cpu(fsinfo->next_cluster) == 0xffffffff ? FAT_START_ENT : le32_to_cpu(fsinfo->next_cluster);
        }

        brelse(fsinfo_bh);
    }

    sbi->dir_per_block = sb->s_blocksize / sizeof(struct msdos_dir_entry);
    sbi->dir_per_block_bits = ffs(sbi->dir_per_block) - 1;

    sbi->dir_start = sbi->fat_start + sbi->fats * sbi->fat_length;
    sbi->dir_entries = b->dir_entries[0];
    sbi->dir_entries |= b->dir_entries[1] << 8;
    if (sbi->dir_entries & (sbi->dir_per_block - 1))
    {
        if (!silent)
            fs_log_error("FAT: bogus directroy-entries per block"
                         " (%u)\n", sbi->dir_entries);
        brelse(bh);
        goto out_invalid;
    }

    rootdir_sectors = sbi->dir_entries
                      * sizeof(struct msdos_dir_entry) / sb->s_blocksize;
    sbi->data_start = sbi->dir_start + rootdir_sectors;
    total_sectors = b->sectors[0];
    total_sectors |= b->sectors[1] << 8;
    if (total_sectors == 0)
    {
        total_sectors = le32_to_cpu(b->total_sect);
    }

    total_clusters = (total_sectors - sbi->data_start) / sbi->sec_per_clus;

    if (sbi->fat_bits != 32)
    {
        sbi->fat_bits = (total_clusters > MAX_FAT12) ? 16 : 12;
    }
    melis_itoa(sbi->fat_bits, &sb->s_fsname[strlen(sb->s_fsname)]);

    /* check that FAT table does not overflow */
    fat_clusters = sbi->fat_length * sb->s_blocksize * 8 / sbi->fat_bits;
    total_clusters = min(total_clusters, fat_clusters - FAT_START_ENT);
    if (total_clusters > MAX_FAT(sb))
    {
        if (!silent)
            fs_log_error("FAT: count of clusters too big (%u)\n",
                         total_clusters);
        brelse(bh);
        goto out_invalid;
    }

    sbi->max_cluster = total_clusters + FAT_START_ENT;
    /* check the free_clusters, it's not necessarily correct */
    if (sbi->free_clusters != -1 && sbi->free_clusters > total_clusters)
    {
        sbi->free_clusters = -1;
    }
    /* check the prev_free, it's not necessarily correct */
    sbi->prev_free %= sbi->max_cluster;
    if (sbi->prev_free < FAT_START_ENT)
    {
        sbi->prev_free = FAT_START_ENT;
    }

    brelse(bh);

    /* set up enough so that it can read an inode */
    fat_hash_init(sb);
    fat_ent_access_init(sb);

    /*
     * The low byte of FAT's first entry must have same value with
     * media-field.  But in real world, too many devices is
     * writing wrong value.  So, removed that validity check.
     *
     * if (FAT_FIRST_ENT(sb, media) != first)
     */

    //error = -EINVAL;
    //sprintf(buf, "cp%d",sbi->options.codepage);
    //sbi->nls_disk = load_nls(buf);
    //if (!sbi->nls_disk) {
    //  fs_log_error("FAT: codepage %s not found\n", buf);
    //  goto out_fail;
    //}
    /* FIXME: utf8 is using iocharset for upper/lower conversion */
    //if (sbi->options.isvfat) {
    //  sbi->nls_io = load_nls(sbi->options.iocharset);
    //  if (!sbi->nls_io) {
    //      fs_log_error("FAT: IO charset %s not found\n",
    //             sbi->options.iocharset);
    //      goto out_fail;
    //  }
    //}

    error = -ENOMEM;
    root_inode = new_inode(sb);
    if (!root_inode)
    {
        goto out_fail;
    }
    root_inode->i_ino = MSDOS_ROOT_INO;
    root_inode->i_version = 1;
    error = fat_read_root(root_inode);
    if (error < 0)
    {
        goto out_fail;
    }
    error = -ENOMEM;
    insert_inode_hash(root_inode);
    sb->s_root = d_alloc_root(root_inode);
    if (!sb->s_root)
    {
        fs_log_error("FAT: get root inode failed\n");
        goto out_fail;
    }
    if (sbi->options.name_check != 's')
    {
        sb->s_root->d_op = &vfat_dentry_ops[0];
    }
    else
    {
        sb->s_root->d_op = &vfat_dentry_ops[2];
    }

    fat_get_label(sb);
    return 0;

out_invalid:
    error = -EINVAL;
    if (!silent)
        fs_log_info("VFS: Can't find a valid FAT filesystem"
                    " on disk %c.\n", sb->s_letter);

out_fail:
    if (root_inode)
    {
        iput(root_inode);
    }
    //if (sbi->nls_io)
    //  unload_nls(sbi->nls_io);
    //if (sbi->nls_disk)
    //  unload_nls(sbi->nls_disk);
    //if (sbi->options.iocharset != fat_default_iocharset)
    //  free(sbi->options.iocharset);
    sb->s_fs_info = NULL;
    free(sbi);
    return error;
}

///*
// * helper function for fat_flush_inodes.  This writes both the inode
// * and the file data blocks, waiting for in flight data blocks before
// * the start of the call.  It does not wait for any io started
// * during the call
// */
//static int writeback_inode(struct inode *inode)
//{
//
//  int ret;
//  struct address_space *mapping = inode->i_mapping;
//  struct writeback_control wbc = {
//         .sync_mode = WB_SYNC_NONE,
//        .nr_to_write = 0,
//  };
//  /* if we used WB_SYNC_ALL, sync_inode waits for the io for the
//  * inode to finish.  So WB_SYNC_NONE is sent down to sync_inode
//  * and filemap_fdatawrite is used for the data blocks
//  */
//  ret = sync_inode(inode, &wbc);
//  if (!ret)
//         ret = filemap_fdatawrite(mapping);
//  return ret;
//}
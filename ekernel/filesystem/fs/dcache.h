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
#ifndef __DCACHE_H__
#define __DCACHE_H__

#define __KERNEL__
#ifdef __KERNEL__

#include "list.h"
#include "atomic.h"

struct nameidata;

/*
 * linux/include/linux/dcache.h
 *
 * Dirent cache data structures
 *
 * (C) Copyright 1997 Thomas Schoebel-Theuer,
 * with heavy changes by Linus Torvalds
 */

#define IS_ROOT(x) ((x)->d_pphash_ == ((int)x))

/*
 * "quick string" -- eases parameter passing, but more importantly
 * saves "metadata" about the string (ie length and the hash).
 *
 * hash comes first so it snuggles against d_parent in the
 * dentry.
 */
struct qstr
{
    unsigned int hash;
    unsigned int len;
    const unsigned char *name;
};

struct dentry_stat_t
{
    int nr_dentry;
    int nr_unused;
    int age_limit;          /* age in seconds */
    int want_pages;         /* pages requested by system */
    int dummy[2];
};
extern struct dentry_stat_t dentry_stat;

/* Name hashing routines. Initial hash value */
/* Hash courtesy of the R5 hash in reiserfs modulo sign bits */
#define init_name_hash()        0

/* partial hash update function. Assume roughly 4 bits per character */
static inline unsigned long partial_name_hash(unsigned long c, unsigned long prevhash)
{
    return (prevhash + (c << 4) + (c >> 4)) * 11;
}

/*
 * Finally: cut down the number of bits to a int value (and try to avoid
 * losing bits)
 */
static inline unsigned long end_name_hash(unsigned long hash)
{
    return (unsigned int) hash;
}

/* Compute the hash for a name string. */
static inline unsigned int full_name_hash(const unsigned char *name, unsigned int len)
{
    unsigned long hash = init_name_hash();
    while (len--)
    {
        hash = partial_name_hash(*name++, hash);
    }
    return end_name_hash(hash);
}

//struct dcookie_struct;

#define DNAME_INLINE_LEN_MIN 36

struct dentry
{
    atomic_t d_count;
    unsigned int d_flags;   /* protected by d_lock */
    //  spinlock_t d_lock;      /* per dentry lock */
    struct inode *d_inode;  /* Where the name belongs to - NULL is
                             * negative */
    /*
     * The next three fields are touched by __d_lookup.  Place them here
     * so they all fit in a cache line.
     */
    struct hlist_node d_hash;   /* lookup hash list */
    // struct dentry *d_parent;    /* parent directory */
    unsigned int d_phash_;      /* parent directory hash value,
                                 * mainly for path walk lookup */
    unsigned int d_pphash_;     /* parent parent directory hash value,
                                 * mainly for path walk lookup */
    struct qstr  d_name;

    struct list_head d_lru;     /* LRU list */
    struct list_head d_sb_list; /* for free or super_block dentry list */
    //  /*
    //   * d_child and d_rcu can share memory
    //   */
    //  union {
    //      struct list_head d_child;/* child of parent list */
    //      struct rcu_head d_rcu;
    //  } d_u;
    //  struct list_head d_subdirs; /* our children */
    //  struct list_head d_alias;   /* inode alias list */
    unsigned long d_time;       /* used by d_revalidate */
    const struct dentry_operations *d_op;
    struct super_block *d_sb;   /* The root of the dentry tree */
    //  void *d_fsdata;             /* fs-specific data */
    //#ifdef CONFIG_PROFILING
    //  struct dcookie_struct *d_cookie; /* cookie, if any */
    //#endif
    //  int d_mounted;
    unsigned char d_iname[DNAME_INLINE_LEN_MIN];    /* small names */
};

/*
 * dentry->d_lock spinlock nesting subclasses:
 *
 * 0: normal
 * 1: nested
 */
enum dentry_d_lock_class
{
    DENTRY_D_LOCK_NORMAL, /* implicitly used by plain spin_lock() APIs. */
    DENTRY_D_LOCK_NESTED
};

struct dentry_operations
{
    int (*d_revalidate)(struct dentry *, struct nameidata *);
    int (*d_hash)(struct dentry *, struct qstr *);
    int (*d_compare)(struct dentry *, struct qstr *, struct qstr *);
    int (*d_delete)(struct dentry *);
    void (*d_release)(struct dentry *);
    void (*d_iput)(struct dentry *, struct inode *);
    char *(*d_dname)(struct dentry *, char *, int);
};

/* the dentry parameter passed to d_hash and d_compare is the parent
 * directory of the entries to be compared. It is used in case these
 * functions need any directory specific information for determining
 * equivalency classes.  Using the dentry itself might not work, as it
 * might be a negative dentry which has no information associated with
 * it */

/*
locking rules:
        big lock    dcache_lock d_lock   may block
d_revalidate:   no      no      no       yes
d_hash      no      no      no       yes
d_compare:  no      yes     yes      no
d_delete:   no      yes     no       no
d_release:  no      no      no       yes
d_iput:     no      no      no       yes
 */

/* d_flags entries */
#define DCACHE_AUTOFS_PENDING 0x0001    /* autofs: "under construction" */
#define DCACHE_NFSFS_RENAMED  0x0002    /* this dentry has been "silly
                     * renamed" and has to be
                     * deleted on the last dput()
                     */
#define DCACHE_DISCONNECTED 0x0004
/* This dentry is possibly not currently connected to the dcache tree,
 * in which case its parent will either be itself, or will have this
 * flag as well.  nfsd will not use a dentry with this bit set, but will
 * first endeavour to clear the bit either by discovering that it is
 * connected, or by performing lookup operations.   Any filesystem which
 * supports nfsd_operations MUST have a lookup function which, if it finds
 * a directory inode with a DCACHE_DISCONNECTED dentry, will d_move
 * that dentry into place and return that dentry rather than the passed one,
 * typically using d_splice_alias.
 */

#define DCACHE_REFERENCED   0x0008  /* Recently used, don't discard. */
#define DCACHE_UNHASHED     0x0010

#define DCACHE_INOTIFY_PARENT_WATCHED   0x0020 /* Parent inode is watched */

/**
 * d_drop - drop a dentry
 * @dentry: dentry to drop
 *
 * d_drop() unhashes the entry from the parent dentry hashes, so that it won't
 * be found through a VFS lookup any more. Note that this is different from
 * deleting the dentry - d_delete will try to mark the dentry negative if
 * possible, giving a successful _negative_ lookup, while d_drop will
 * just make the cache lookup fail.
 *
 * d_drop() is used mainly for stuff that wants to invalidate a dentry for some
 * reason (NFS timeouts or autofs deletes).
 *
 * __d_drop requires dentry->d_lock.
 */

static inline void __d_drop(struct dentry *dentry)
{
    if (!(dentry->d_flags & DCACHE_UNHASHED))
    {
        dentry->d_flags |= DCACHE_UNHASHED;
        hlist_del_rcu(&dentry->d_hash);
    }
}

static inline void d_drop(struct dentry *dentry)
{
    __d_drop(dentry);
}

static inline int dname_external(struct dentry *dentry)
{
    return dentry->d_name.name != dentry->d_iname;
}

/*
 * These are the low-level FS interfaces to the dcache..
 */
extern void d_instantiate(struct dentry *, struct inode *);
extern struct dentry *d_instantiate_unique(struct dentry *, struct inode *);
extern struct dentry *d_materialise_unique(struct dentry *, struct inode *);
extern void d_delete(struct dentry *);

/* allocate/de-allocate */
extern struct dentry *d_alloc(struct dentry *, const struct qstr *);
//extern struct dentry * d_alloc_anon(struct inode *);
extern struct dentry *d_splice_alias(struct inode *, struct dentry *);
//extern void shrink_dcache_sb(struct super_block *);
//extern void shrink_dcache_parent(struct dentry *);
extern int shrink_dcache_memory(int nr);
extern void shrink_dcache_for_umount(struct super_block *, int force);
extern int d_invalidate(struct dentry *);

/* only used at mount-time */
extern struct dentry *d_alloc_root(struct inode *);

/* <clickety>-<click> the ramfs-type tree */
//extern void d_genocide(struct dentry *);

//extern struct dentry *d_find_alias(struct inode *);
//extern void d_prune_aliases(struct inode *);

/* test whether we have any submounts in a subdir tree */
//extern int have_submounts(struct dentry *);

/*
 * This adds the entry to the hash queues.
 */
extern void d_rehash(struct dentry *);

extern void d_move(struct dentry *dentry, struct dentry *target);

/**
 * d_add - add dentry to hash queues
 * @entry: dentry to add
 * @inode: The inode to attach to this dentry
 *
 * This adds the entry to the hash queues and initializes @inode.
 * The entry was actually filled in earlier during d_alloc().
 */

static inline void d_add(struct dentry *entry, struct inode *inode)
{
    d_instantiate(entry, inode);
    d_rehash(entry);
}

///**
// * d_add_unique - add dentry to hash queues without aliasing
// * @entry: dentry to add
// * @inode: The inode to attach to this dentry
// *
// * This adds the entry to the hash queues and initializes @inode.
// * The entry was actually filled in earlier during d_alloc().
// */
//static inline struct dentry *d_add_unique(struct dentry *entry, struct inode *inode)
//{
//  struct dentry *res;
//
//  res = d_instantiate_unique(entry, inode);
//  d_rehash(res != NULL ? res : entry);
//  return res;
//}

/* used for rename() and baskets */
extern void d_move(struct dentry *, struct dentry *);

/* appendix may either be NULL or be used for transname suffixes */
extern struct dentry *__d_lookup(struct dentry *, struct qstr *);
extern struct dentry *d_hash_and_lookup(struct dentry *, struct qstr *);

/* validate "insecure" dentry pointer */
extern int d_validate(struct dentry *, struct dentry *);

/*
 * helper function for dentry_operations.d_dname() members
 */
extern char *dynamic_dname(struct dentry *, char *, int, const char *, ...);

/* Allocation counts.. */

/**
 *  dget, dget_locked   -   get a reference to a dentry
 *  @dentry: dentry to get a reference to
 *
 *  Given a dentry or %NULL pointer increment the reference count
 *  if appropriate and return the dentry. A dentry will not be
 *  destroyed when it has references. dget() should never be
 *  called for dentries with zero reference counter. For these cases
 *  (preferably none, functions in dcache.c are sufficient for normal
 *  needs and they take necessary precautions) you should hold dcache_lock
 *  and call dget_locked() instead of dget().
 */

static inline struct dentry *dget(struct dentry *dentry)
{
    if (dentry)
    {
        BUG_ON(!atomic_read(&dentry->d_count));
        atomic_inc(&dentry->d_count);
    }
    return dentry;
}

/**
 *  d_unhashed -    is dentry hashed
 *  @dentry: entry to check
 *
 *  Returns true if the dentry passed is not currently hashed.
 */

static inline int d_unhashed(struct dentry *dentry)
{
    return (dentry->d_flags & DCACHE_UNHASHED);
}

//static inline struct dentry *dget_parent(struct dentry *dentry)
//{
//  struct dentry *ret;
//
//  spin_lock(&dentry->d_lock);
//  ret = dget(dentry->d_parent);
//  spin_unlock(&dentry->d_lock);
//  return ret;
//}

extern void dput(struct dentry *);

//static inline int d_mountpoint(struct dentry *dentry)
//{
//  return dentry->d_mounted;
//}
//
extern struct vfsmount *lookup_mnt(struct vfsmount *, struct dentry *);
extern struct vfsmount *__lookup_mnt(struct vfsmount *, struct dentry *, int);
extern struct dentry *lookup_create(struct nameidata *nd, int is_dir);

//extern int sysctl_vfs_cache_pressure;

#endif /* __KERNEL__ */

#endif  /* __DCACHE_H__ */
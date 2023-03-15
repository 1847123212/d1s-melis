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
#ifndef __CACHE_HASH_H__
#define __CACHE_HASH_H__

struct cache_hash_element
{
    void *element;
};

typedef int (* cache_entry_compare_func)(void *element, const void *key);
typedef unsigned long (* cache_entry_hash_func)(void *element, const void *key, unsigned long range);

struct cache_hash_table
{
    struct cache_hash_element **table_list;
    int     next_element_offset;
    int     cnt_elements;
    int     flags;
    unsigned long table_size;
    cache_entry_compare_func compare_func;
    cache_entry_hash_func hash_func;
};

typedef struct cache_hash_table cache_hash_table;

#ifdef __cplusplus
extern "C"
{
#endif

struct cache_hash_table *cache_hash_init(unsigned long table_size,
        int next_element_offset,
        cache_entry_compare_func compare_func,
        cache_entry_hash_func hash_func);

int cache_hash_deinit(struct cache_hash_table *table);
int cache_hash_insert(struct cache_hash_table *table, void *element);
int cache_hash_remove(struct cache_hash_table *table, void *element);
void *cache_hash_lookup(struct cache_hash_table *table, const void *key);

#ifdef __cplusplus
}
#endif

#endif  /* __CACHE_HASH_H__ */

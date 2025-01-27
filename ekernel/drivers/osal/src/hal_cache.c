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
#include <hal_cache.h>
#include <kconfig.h>

#ifdef CONFIG_CACHE_ALIGN_CHECK
#include <sunxi_hal_common.h>

#ifdef CONFIG_DEBUG_BACKTRACE
/*#if 0*/
#include <backtrace.h>
#include <assert.h>
#define CACHELINE_CHECK(option) \
{ \
    if (!option) { \
        printf("[%s] cacheline check failed\n", __func__); \
        backtrace(NULL, NULL, 0, 0, printf); \
        assert(0); \
    } \
} while (0)

#else
#define CACHELINE_CHECK(option) \
{ \
    assert(option); \
} while (0)

#endif /* CONFIG_DEBUG_BACKTRACE */
#endif /* CACHE_ALIGN_CHECK */

void hal_dcache_clean(unsigned long vaddr_start, unsigned long size)
{
#ifdef CONFIG_CACHE_ALIGN_CHECK
    CACHELINE_CHECK(!(vaddr_start & (CACHELINE_LEN - 1)));
    /*CACHELINE_CHECK((size % CACHELINE_LEN) == 0);*/
#endif
    awos_arch_mems_clean_dcache_region(vaddr_start, size);
}

void hal_dcache_clean_invalidate(unsigned long vaddr_start, unsigned long size)
{
#ifdef CONFIG_CACHE_ALIGN_CHECK
    CACHELINE_CHECK(!(vaddr_start & (CACHELINE_LEN - 1)));
    /*CACHELINE_CHECK((size % CACHELINE_LEN) == 0);*/
#endif
    awos_arch_mems_clean_flush_dcache_region(vaddr_start, size);
}

void hal_dcache_invalidate(unsigned long vaddr_start, unsigned long size)
{
#ifdef CONFIG_CACHE_ALIGN_CHECK
    CACHELINE_CHECK(!(vaddr_start & (CACHELINE_LEN - 1)));
    /*CACHELINE_CHECK((size % CACHELINE_LEN) == 0);*/
#endif
    awos_arch_mems_flush_dcache_region(vaddr_start, size);
}

void hal_dcache_clean_all(void)
{
    awos_arch_clean_dcache();
}

void hal_dcache_invalidate_all(void)
{
    awos_arch_flush_dcache();
}

void hal_icache_invalidate_all(void)
{
    awos_arch_flush_icache_all();
}

void hal_icache_invalidate(unsigned long vaddr_start, unsigned long size)
{
    awos_arch_mems_flush_icache_region((void *)vaddr_start, size);
}

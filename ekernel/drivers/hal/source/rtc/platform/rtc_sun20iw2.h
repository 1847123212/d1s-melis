/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY��S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS��SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY��S TECHNOLOGY.


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

#ifndef __RTC_SUN20IW2_H__
#define __RTC_SUN20IW2_H__

#undef BIT
#define BIT(nr)     (1UL << (nr))

#define SUNXI_RTC_BASE      0x40051000

#include <prcm/platform_prcm.h>
#define SUNXI_RTC_DATA_BASE (PRCM_BASE + 0x200)

#define SUNXI_GIC_START 0
#define SUXNI_IRQ_RTC (SUNXI_GIC_START + 10)

#define SUNXI_RTC_YMD               0x0010

#define SUNXI_RTC_HMS               0x0014

#ifdef CONFIG_ALARM1_USED
#define SUNXI_ALRM_DHMS		      0x0040

#define SUNXI_ALRM_EN                 0x0044
#define SUNXI_ALARM_EN_CNT_EN         BIT(8)

#define SUNXI_ALRM_IRQ_EN             0x0048
#define SUNXI_ALARM_IRQ_EN_CNT_IRQ_EN BIT(0)

#define SUNXI_ALARM_IRQ_STA           0x004c
#define SUNXI_ALARM_IRQ_STA_CNT_IRQ_PEND BIT(0)

#define SUNXI_ALRM_WAKEUP_OUTPUT_EN   BIT(1)
#else
#define SUNXI_ALRM_DAY              0X0020
#define SUNXI_ALRM_COUNTER                      0x0020
#define SUNXI_ALRM_CURRENT                      0x0024
#define SUNXI_ALRM_HMS              0X0024

#define SUNXI_ALRM_EN                           0x0028
#define SUNXI_ALRM_EN_CNT_EN                    BIT(0)

#define SUNXI_ALRM_IRQ_EN                       0x002c
#define SUNXI_ALRM_IRQ_EN_CNT_IRQ_EN            BIT(0)

#define SUNXI_ALRM_IRQ_STA                      0x0030
#define SUNXI_ALRM_IRQ_STA_CNT_IRQ_PEND         BIT(0)

#define SUNXI_ALRM_WAKEUP_OUTPUT_EN             BIT(0)
#endif

#define SUNXI_LOSC_CTRL             0x0000
#define SUNXI_LOSC_CTRL_RTC_ALARM_ACC   BIT(2)
#define SUNXI_LOSC_CTRL_RTC_HMS_ACC     BIT(1)
#define SUNXI_LOSC_CTRL_RTC_YMD_ACC     BIT(0)
#define SUNXI_ALARM_CONFIG                      0x0050

#endif /* __RTC-SUN20IW2_H__ */

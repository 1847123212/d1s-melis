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
#include "gui_core_cc_private.h"

/*********************************************************************
*
*       Public code, LCD_FIXEDPALETTE == 222, 64 colors
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_222
*/
unsigned LCD_Color2Index_222(LCD_COLOR Color)
{
    int r, g, b;
    r = ((Color & 255)      + 0x2a) / 0x55;
    g = (((Color >> 8) & 255) + 0x2a) / 0x55;
    b = (((Color >> 16) & 255) + 0x2a) / 0x55;
    return r + (g << 2) + (b << 4);
}

/*********************************************************************
*
*       LCD_Index2Color_222
*/
LCD_COLOR LCD_Index2Color_222(int Index)
{
    U16P r, g, b;
    r = ((Index >> 0) & 3) * 0x55;
    g = ((Index >> 2) & 3) * 0x55;
    b = (Index >> 4)    * 0x55;
    return (((U32)b) << 16) | (g << 8) | r;
}

/*********************************************************************
*
*       LCD_GetIndexMask_222
*/
unsigned LCD_GetIndexMask_222(void)
{
    return 0x0003f;
}

/*********************************************************************
*
*       Public code, LCD_FIXEDPALETTE == 222, 64 colors, SWAP_RB
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_M222
*/
unsigned LCD_Color2Index_M222(LCD_COLOR Color)
{
    int r, g, b;
    r = ((Color & 255)      + 0x2a) / 0x55;
    g = (((Color >> 8) & 255) + 0x2a) / 0x55;
    b = (((Color >> 16) & 255) + 0x2a) / 0x55;
    return b + (g << 2) + (r << 4);
}

/*********************************************************************
*
*       LCD_Index2Color_M222
*/
LCD_COLOR LCD_Index2Color_M222(int Index)
{
    U16P r, g, b;
    r = ((Index >> 0) & 3) * 0x55;
    g = ((Index >> 2) & 3) * 0x55;
    b = (Index >> 4)    * 0x55;
    return (((U32)r) << 16) | (g << 8) | b;
}

/*********************************************************************
*
*       LCD_GetIndexMask_M222
*/
unsigned LCD_GetIndexMask_M222(void)
{
    return 0x003f;
}

/*************************** End of file ****************************/

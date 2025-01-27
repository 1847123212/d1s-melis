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
#include "gui_core_memdev_private.h"

#if GUI_SUPPORT_MEMDEV

/*********************************************************************
*
*       static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetPixelIndex
*
*/
static int _GetPixelIndex(const U8 *pData, int x, int y, int bpp, int BytesPerLine)
{
    if (bpp == 8)
    {
        return (int)(*(pData + (y * BytesPerLine) + x));
    }
    else
    {
        return (int)(*(const U16 *)(pData + (y * BytesPerLine) + (x << 1)));
    }
}

/*********************************************************************
*
*       _DrawHLineAlpha
*/
static void _DrawHLineAlpha(int x0, int y, int x1, int Intens)
{
    GUI_MEMDEV *pDev   = GUI_MEMDEV_H2P(GUI_Context.hDevData);
    GUI_USAGE_h hUsage = pDev->hUsage;
    LCD_COLOR Color1, Color2;
    int Len = x1 - x0 + 1;

    if (hUsage)
    {
        GUI_USAGE_AddHLine(GUI_USAGE_H2P(hUsage), x0, y, Len);
    }

    Color1 = LCD_Index2Color(LCD_COLORINDEX);

    if (pDev->BitsPerPixel == 8)
    {
        U8 *pData;
        pData  = (U8 *) GUI_MEMDEV__XY2PTR(x0, y);

        while (Len--)
        {
            Color2 = LCD_Index2Color(*(pData));
            Color2 = LCD_MixColors256(Color1, Color2, Intens);
            *(pData++) = LCD_Color2Index(Color2);
        }
    }
    else
    {
        U16 *pData;
        pData  = (U16 *) GUI_MEMDEV__XY2PTR(x0, y);

        while (Len--)
        {
            Color2 = LCD_Index2Color(*(pData));
            Color2 = LCD_MixColors256(Color1, Color2, Intens);
            *(pData++) = LCD_Color2Index(Color2);
        }
    }
}

/*********************************************************************
*
*       _DrawBitmapLineEx
*/
static void _DrawBitmapLineEx(int x0, int y0, int xOff, int yOff, int xSize, int xMag, int Alpha,
                              int bpp, int BytesPerLine, const U8 *pData)
{
    int x, xi, xAct, xStart, xMagAbs, xiMag, xMin, xMax, Cached;
    LCD_PIXELINDEX Index = 0, IndexPrev = 0;
    /* Use clipping rect */
    xMin = GUI_Context.ClipRect[GUI_Context.SelLayer].x0 - x0;
    xMax = GUI_Context.ClipRect[GUI_Context.SelLayer].x1 - x0;
    /* Init some values */
    xMagAbs = (xMag > 0) ? xMag : -xMag;
    x       = (xOff * xMagAbs) / 1000;
    xiMag   = (xOff * xMagAbs) % 1000;
    xStart  = -1;
    Cached  =  0;

    for (xi = 0; xi < xSize; xi++)
    {
        xiMag += xMagAbs;

        if (xiMag >= 1000)
        {
            if (x <= xMax)
            {
                xAct  = (xMag > 0) ? xi : xSize - xi - 1;
                Index = _GetPixelIndex(pData, xOff + xAct, yOff, bpp, BytesPerLine);

                if ((Index != IndexPrev) || (xStart == -1))
                {
                    if ((xStart != -1) && (x > xMin))
                    {
                        /* Clear cache */
                        if (xStart < xMin)
                        {
                            xStart = xMin;
                        }

                        LCD_SetColorIndex(IndexPrev);

                        if (Alpha == 256 || !GUI_Context.hDevData)
                        {
                            LCDDEV_L0_DrawHLine(x0 + xStart, y0, x0 + x - 1);
                        }
                        else
                        {
                            _DrawHLineAlpha(x0 + xStart, y0, x0 + x - 1, Alpha);
                        }
                    }

                    IndexPrev = Index;
                    xStart    = x;
                    Cached    = 1;
                }

                do
                {
                    x++;
                    xiMag -= 1000;
                } while (xiMag >= 1000);
            }
            else
            {
                x = xMax + 1;
                break;
            }
        }
    }

    if (Cached && (x > xMin))
    {
        /* Clear cache */
        if (xStart < xMin)
        {
            xStart = xMin;
        }

        LCD_SetColorIndex(Index);

        if (Alpha == 256 || !GUI_Context.hDevData)
        {
            LCDDEV_L0_DrawHLine(x0 + xStart, y0, x0 + x - 1);
        }
        else
        {
            _DrawHLineAlpha(x0 + xStart, y0, x0 + x - 1, Alpha);
        }
    }
}

/*********************************************************************
*
*       _DrawBitmapEx
*/
static void _DrawBitmapEx(int x0, int y0, int xOff, int yOff, int xSize, int ySize, int xMag, int yMag,
                          int Alpha, int BmpSizeX, int BmpSizeY, int bpp, int BytesPerLine, const U8 *pData)
{
    int y, yi, yMin, yMax, yEnd, yPrev;
    yMin = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
    yMax = GUI_Context.ClipRect[GUI_Context.SelLayer].y1;

    if (xMag < 0)
    {
        x0 -= (BmpSizeX - (xOff << 1) - xSize) * xMag / 1000;
    }

    if (yMag < 0)
    {
        y0 -= (BmpSizeY - 1) * yMag / 1000;
    }

    yEnd  = y0 + GUI__DivideRound32(((I32)(yOff) * yMag), 1000);
    yPrev = yEnd + 1;

    for (yi = yOff; yi < (yOff + ySize); yi++)
    {
        y = yEnd;
        yEnd = y0 + GUI__DivideRound32(((I32)(yi + 1) * yMag), 1000);

        if (y != yPrev)
        {
            yPrev = y;

            do
            {
                if ((y >= yMin) && (y <= yMax))
                {
                    _DrawBitmapLineEx(x0, y, xOff, yi, xSize, xMag, Alpha, bpp, BytesPerLine, pData);
                }
            } while ((yMag < 0) ? (--y > yEnd) : (++y < yEnd));
        }
    }
}

/*********************************************************************
*
*       _WriteExToActiveAt
*/
static void _WriteExToActiveAt(GUI_MEMDEV_Handle hMem, int x, int y, int xMag, int yMag, int Alpha)
{
    GUI_MEMDEV *pDev;
    GUI_USAGE_h hUsage;
    GUI_USAGE  *pUsage;
    U8 *pData;
    int yAct, yPrev, ySize, yi;
    int BytesPerLine, bpp;
    pDev         = GUI_MEMDEV_H2P(hMem);
    hUsage       = pDev->hUsage;
    ySize        = pDev->YSize;
    BytesPerLine = pDev->BytesPerLine;
    pData        = (U8 *)(pDev + 1);
    bpp          = pDev->BitsPerPixel;
    yPrev = y + 1;

    if (hUsage)
    {
        int xOff, xSize, y0;
        pUsage = GUI_USAGE_H2P(hUsage);

        for (yi = 0; yi < ySize; yi++)
        {
            yAct = y + GUI__DivideRound32(((I32)(yi) * yMag), 1000);

            if (yAct != yPrev)
            {
                xOff  = 0;
                xSize = GUI_USAGE_GetNextDirty(pUsage, &xOff, yi);

                if (xSize == pDev->XSize)
                {
                    /* If the entire line is affected, calculate the number of entire lines */
                    y0 = yi;

                    while (GUI_USAGE_GetNextDirty(pUsage, &xOff, yi + 1) == xSize)
                    {
                        yi++;
                    }

                    _DrawBitmapEx(x, y, 0, y0, pDev->XSize, yi - y0 + 1, xMag, yMag, Alpha,
                                  pDev->XSize, pDev->YSize, bpp, BytesPerLine, pData);
                    yPrev = y + GUI__DivideRound32(((I32)(yi) * yMag), 1000);
                }
                else
                {
                    /* Draw the partial line which needs to be drawn */
                    while (xSize)
                    {
                        _DrawBitmapEx(x, y, xOff, yi, xSize, 1, xMag, yMag, Alpha,
                                      pDev->XSize, pDev->YSize, bpp, BytesPerLine, pData);
                        xOff += xSize;
                        xSize = GUI_USAGE_GetNextDirty(pUsage, &xOff, yi);
                    }

                    yPrev = yAct;
                }
            }
        }
    }
    else
    {
        _DrawBitmapEx(x, y, 0, 0, pDev->XSize, ySize, xMag, yMag, Alpha,
                      pDev->XSize, pDev->YSize, bpp, BytesPerLine, pData);
    }
}

/*********************************************************************
*
*        public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_WriteExAt
*/
void GUI_MEMDEV_WriteExAt(GUI_MEMDEV_Handle hMem, int x, int y, int xMag, int yMag, int Alpha)
{
    if (hMem)
    {
        GUI_MEMDEV *pDevData;
        GUI_RECT ClipRectPrev;
        GUI_LOCK();
        pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem);

        if (x == GUI_POS_AUTO)
        {
            x = pDevData->x0;
            y = pDevData->y0;
        }

        ClipRectPrev = GUI_Context.ClipRect[GUI_Context.SelLayer];
        LCD_SetClipRectMax();
        _WriteExToActiveAt(hMem, x, y, xMag, yMag, Alpha);
        GUI_Context.ClipRect[GUI_Context.SelLayer] = ClipRectPrev;
        GUI_UNLOCK();
    }
}

/*********************************************************************
*
*       GUI_MEMDEV_WriteEx
*/
void GUI_MEMDEV_WriteEx(GUI_MEMDEV_Handle hMem, int xMag, int yMag, int Alpha)
{
    GUI_MEMDEV_WriteExAt(hMem, GUI_POS_AUTO, GUI_POS_AUTO, xMag, yMag, Alpha);
}

#else

void GUIDEV_WriteEx_C(void) {}

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/

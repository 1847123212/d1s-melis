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
#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define GUI_TIMER_H2P(h) (GUI_TIMER_Obj*)GUI_ALLOC_h2p(h)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct
{
    GUI_TIMER_CALLBACK *cb;
    GUI_TIMER_HANDLE hNext;
    int Flags;
    U32 Context;
    GUI_TIMER_TIME t0;
    GUI_TIMER_TIME Period;
} GUI_TIMER_Obj;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

GUI_TIMER_HANDLE hFirstTimer;
GUI_TIMER_HANDLE _hActiveTimer;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Unlink
*/
static void _Unlink(GUI_TIMER_HANDLE hTimer)
{
    GUI_TIMER_Obj *pTimer = GUI_TIMER_H2P(hTimer);
    GUI_TIMER_HANDLE hi;
    GUI_TIMER_Obj   *pi;

    /* Check if it is the first element */
    if (hFirstTimer == hTimer)
    {
        hFirstTimer = pTimer->hNext;
        return;
    }

    hi = hFirstTimer;

    /* Try to find it in the list ... */
    while (hi)
    {
        /* GUI_ASSERT(hi<1000,0); */
        pi = GUI_TIMER_H2P(hi);

        if (pi->hNext == hTimer)
        {
            pi->hNext = pTimer->hNext;
            break;
        }

        hi = pi->hNext;
    }
}

/*********************************************************************
*
*       _Link
*
* Purpose:
*   This routine inserts the new timer (referenced by its handle) into
*     the linked list. The linked list is sorted according to timestamps.
*     The first element is the timer which expires first.
*/
static void _Link(GUI_TIMER_HANDLE hNew)
{
    GUI_TIMER_Obj   *pNew        = GUI_TIMER_H2P(hNew);
    GUI_TIMER_Obj   *pTimer;
    GUI_TIMER_Obj   *pNext;
    GUI_TIMER_HANDLE hNext;

    if (hFirstTimer == 0)  /* List is empty, make it the only element */
    {
        hFirstTimer = hNew;
        pNew->hNext = 0;
    }
    else
    {
        GUI_TIMER_Obj *pFirstTimer      = GUI_TIMER_H2P(hFirstTimer);

        /* Check if we have to make it the first element */
        if ((pNew->t0 - pFirstTimer->t0) <= 0)
        {
            pNew->hNext = hFirstTimer;
            hFirstTimer = hNew;
            return;
        }
        else
        {
            GUI_TIMER_HANDLE hTimer = hFirstTimer;

            /* Put it into the list */
            do
            {
                pTimer       = GUI_TIMER_H2P(hTimer);
                hNext        = pTimer->hNext;

                if (hNext == 0)
                {
                    goto Append;
                }

                pNext      = GUI_TIMER_H2P(hNext);

                if ((pNew->t0 - pNext->t0) <= 0)
                {
                    pNew->hNext  = hNext;
                    pTimer->hNext = hNew;
                    return;
                }
            } while (1);

            /* Put it at the end of the list */
Append:
            pNew->hNext  = hNext;
            pTimer->hNext = hNew;
            return;
        }
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_TIMER_Exec
*/
int GUI_TIMER_Exec(void)
{
    int r = 0;
    GUI_TIMER_TIME t = GUI_GetTime();
    GUI_LOCK();
    {
        while (hFirstTimer)
        {
            GUI_TIMER_Obj *pTimer = GUI_TIMER_H2P(hFirstTimer);

            if ((pTimer->t0 - t) <= 0)
            {
                GUI_TIMER_MESSAGE tm;
                tm.Time = t;
                tm.Context = pTimer->Context;
                _hActiveTimer = hFirstTimer;
                hFirstTimer = pTimer->hNext;
#if GUI_EPOS_CB
                esKRNL_CallBack((__pCBK_t)(pTimer->cb), &tm);
#else
                (pTimer->cb)(&tm);
#endif
                r = 1;
            }
            else
            {
                break;
            }
        }

        /*
            GUI_TIMER_Obj* pObj = GUI_TIMER_H2P(hObj);
        pObj->t0 = Time;
        */
    }
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_TIMER_Create
*/
GUI_TIMER_HANDLE GUI_TIMER_Create(GUI_TIMER_CALLBACK *cb, int Time, U32 Context, int Flags)
{
    GUI_TIMER_HANDLE hObj;
    GUI_TIMER_Obj *pObj;
    GUI_LOCK();
    GUI_USE_PARA(Flags);
    GUI_USE_PARA(Time);
    GUI_pfTimerExec = GUI_TIMER_Exec;
    {
        /* Alloc memory for obj */
        hObj = GUI_ALLOC_AllocZero(sizeof(GUI_TIMER_Obj));
        pObj = GUI_TIMER_H2P(hObj);
        /* init member variables */
#if GUI_EPOS_CB
        pObj->cb = (GUI_TIMER_CALLBACK *)esKRNL_GetCallBack((__pCBK_t)cb);
#else
        pObj->cb = cb;
#endif
        pObj->Context = Context;
        pObj->t0 = Time;    //houhh 20061018...
        /* Link it */
        _Link(hObj);
    }
    GUI_UNLOCK();
    return hObj;
}

/*********************************************************************
*
*       GUI_TIMER_Delete
*/
void GUI_TIMER_Delete(GUI_TIMER_HANDLE hObj)
{
    /* Unlink Timer */
    GUI_LOCK();
    _Unlink(hObj);
    GUI_ALLOC_Free(hObj);
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_TIMER_SetPeriod
*/
void GUI_TIMER_SetPeriod(GUI_TIMER_HANDLE hObj, GUI_TIMER_TIME Period)
{
    GUI_LOCK();
    {
        GUI_TIMER_Obj *pObj = GUI_TIMER_H2P(hObj);
        pObj->Period = Period;
    }
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_TIMER_SetTime
*/
void GUI_TIMER_SetTime(GUI_TIMER_HANDLE hObj, GUI_TIMER_TIME Time)
{
    GUI_LOCK();
    {
        GUI_TIMER_Obj *pObj = GUI_TIMER_H2P(hObj);
        pObj->t0 = Time;
    }
    GUI_UNLOCK();
}

//////
void GUI_TIMER_Context(GUI_TIMER_HANDLE hObj, U32 Context)
{
    GUI_LOCK();
    {
        GUI_TIMER_Obj *pObj = GUI_TIMER_H2P(hObj);
        pObj->Context = Context;
    }
    GUI_UNLOCK();
}
//////

/*********************************************************************
*
*       GUI_TIMER_SetDelay
*/
void GUI_TIMER_SetDelay(GUI_TIMER_HANDLE hObj, GUI_TIMER_TIME Delay)
{
    GUI_LOCK();
    {
        GUI_TIMER_Obj *pObj = GUI_TIMER_H2P(hObj);
        pObj->t0 = Delay;
        _Unlink(hObj);
        _Link(hObj);
    }
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_TIMER_Restart
*/
void GUI_TIMER_Restart(GUI_TIMER_HANDLE hObj)
{
    GUI_TIMER_Obj *pObj;
    GUI_LOCK();
    {
        if (hObj == 0)
        {
            hObj = _hActiveTimer;
        }

        pObj = GUI_TIMER_H2P(hObj);
        pObj->t0 = GUI_GetTime() + pObj->Period;
        _Unlink(hObj);
        _Link(hObj);
    }
    GUI_UNLOCK();
}

/*************************** End of file ****************************/

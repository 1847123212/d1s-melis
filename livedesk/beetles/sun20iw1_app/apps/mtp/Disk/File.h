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
#ifndef _FILE_H
#define _FILE_H

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

#include "aw_list.h"
#include "Dir.h"
#include "elibs_stdio.h"

typedef mode_t FileType;
typedef uint64_t FileSize;
enum
{
    FILE_TYPE_SOCK = 0x140000,
    FILE_TYPE_LINK = 0x120000,
    FILE_TYPE_REGULAR = 0x100000,
    FILE_TYPE_BLK = 0x60000,
    FILE_TYPE_DIR = 0x40000,
    FILE_TYPE_CHR = 0x20000,
    FILE_TYPE_FIFO = 0x10000,

};

struct File
{
    struct list_head list;
    char *path;
    char *fileName;

    FileType fileType;
    FileSize fileSize;
    ES_TIMESTAMP fileAccessTime;
    ES_TIMESTAMP fileModifyTime;

    struct Dir *parentDir;
    void *object;

    bool (*renameTo)(const char *newPath, struct File *dir);
};

struct File *FileInit(const char *path, struct Dir *parentDir);
void FileRelease(struct File *file);
void updateFileInfo(struct File *file);
void deleteFileInfo(struct File *file);
#endif

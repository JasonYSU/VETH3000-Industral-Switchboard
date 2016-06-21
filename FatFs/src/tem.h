#ifndef _TEM_H__
#define _TEM_H__


#include "ff_gen_drv.h"

DSTATUS norInitialize (void);
DSTATUS norStatus (void);
DRESULT norRead (BYTE*, DWORD, BYTE);
#if _USE_WRITE == 1
DRESULT norWrite (const BYTE*, DWORD, BYTE);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
DRESULT norIoctl (BYTE, void*);
#endif  /* _USE_IOCTL == 1 */


#endif
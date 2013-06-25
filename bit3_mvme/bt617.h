// ****************************************************************************
// *     Filename:  bt617.h
// *  Description:  Bit3 interface according to mvme
// *   Created by:  Nam Tran (nam at kuno-g.phys.sci.osaka-u.ac.jp)
// *     Time tag:  2013-03-05 18:10:18
// ****************************************************************************
#ifndef BT617_TR8QHYYP
#define BT617_TR8QHYYP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "vmedrv.h"
#include <stdint.h>
// Midas general interface
#include "mvmestd.h"

#ifdef __cplusplus
extern "C" {
#endif

	void show_mvme(MVME_INTERFACE *vme);
	vmedrv_access_mode_t bt617_GetAccessMode(MVME_INTERFACE *vme);
	void bt617_SetAccessMode(MVME_INTERFACE *vme);
	void showbits(uint32_t data);
	void bt617_SetBLT(MVME_INTERFACE *vme, bool blt);

#ifdef __cplusplus
}
#endif
#endif /* end of include guard: BT617_TR8QHYYP */

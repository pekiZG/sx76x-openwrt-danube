/* sys_drv_delaylib_vxworks.h - self-calibrating hard delay routines header file */

/*
modification history
--------------------
27Mar96,espin written.

$Id: sys_drv_delaylib_vxworks.h,v 1.2 2004/02/05 15:05:46 martin Exp $

*/

#ifndef __SYS_DRV_DELAYLIB_VXWORKS
#define __SYS_DRV_DELAYLIB_VXWORKS

#if defined(__STDC__) || defined(__cplusplus)
extern void delayUsec (unsigned int u);
extern void delayMsec (unsigned int m);
#else
extern void delayUsec ();
extern void delayMsec ();
#endif /* __STDC__ || __cplusplus */

#endif /* __SYS_DRV_DELAYLIB_VXWORKS */



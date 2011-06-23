#ifndef _sys_drv_debug_h
#define _sys_drv_debug_h
/****************************************************************************
       Copyright (c) 2000, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
 ****************************************************************************
   Module      : $RCSfile: sys_drv_debug.h,v $
   Date        : $Date: 2004/10/07 03:53:32 $
   Description : system debug interface

   $Log: sys_drv_debug.h,v $
   Revision 1.2  2004/10/07 03:53:32  pliu
   integration with kernel

   Revision 1.14  2004/07/26 17:24:55  rah
   changes for Windows

   Revision 1.13  2004/07/20 12:38:39  neumann
   Renamed dgbResult to dbgResult

   Revision 1.12  2004/07/15 15:18:20  rah
   changed file name for VxWorks header with capital letters

   Revision 1.11  2004/07/08 15:31:52  martin
   UINT32 changed to native types (for change to ifx_types)

   Revision 1.10  2004/03/17 16:00:21  tauche
   Macros with variable number of arguments are GNU-specific. For other preprocessors (e.g. PC-Lint) other definitions have to be used

   Revision 1.9  2004/03/11 21:53:47  tauche
   added PC-Lint flags to suppress warning because PC-Lint is not able to deal with macro with variable args (gcc only)

   Revision 1.8  2004/03/04 17:58:05  tauche
   no message

   Revision 1.7  2004/02/27 16:06:44  martin
   Macros changed:
   need explicit ; at the end

   Revision 1.6  2004/02/19 11:15:52  tauche
   added missing vxworks.h include

   Revision 1.5  2004/02/19 09:13:45  tauche
   change DRV_LOG macro to support logMsg with variable number of args

   Revision 1.4  2004/02/12 13:12:57  wachendorf
   cosmetics

   Revision 1.3  2004/01/29 18:03:28  langer
   - moved sys_debug.c to sys_drv_debug.c
   - updated name inside

   Revision 1.2  2004/01/27 13:13:19  kamdem
   * sys_drv_noOS.h : new defines added for _IOW and _IOR
   * sys_drv_debug.h :  DRV_LOG (x) calls printf
   * sys_drv_noOSLib.c : error corrected

   Revision 1.1  2004/01/23 14:13:49  martin
   file renamed: sys_debug.h -> sys_drv_debug.h because of conflict with vos/sys_debug.h when include paths are omitted within code files

   Revision 1.22  2003/12/19 13:52:43  tauche
   VXWORKS PRINTK function using logMsg with variable number of arguments (printf compatible)
   temporary OS-less section added

   Revision 1.21  2003/12/19 10:22:32  rah
   Added windows types and system calls for simulation
   customer module as template for new OS's, still not finished -> serves as discussion basis for moving to OS_... calls

   Revision 1.16  2003/06/27 13:01:18  rutkowski
   - Change the formation of the text to be easier to read.


 ***************************************************************************/

/* ============================= */
/* Global Macro Definitions      */
/* ============================= */

/* ============================= */
/* Group=VOS - debug interface   */
/* ============================= */

#define DBG_LEVEL_OFF 		4
#define DBG_LEVEL_HIGH		3
#define DBG_LEVEL_NORMAL	2
#define DBG_LEVEL_LOW 		1

/* FIXME: test for GNU compiler */
/*lint -emacro(2,__TRACE) -emacro(10,__TRACE)*/
/* This inhibits PC-Lint from reporting errors 2 and 10 for the __TRACE macro */
/* since PC-Lints preprocessor is not able to deal with macros with variable  */
/* number of arguments */
#ifdef VXWORKS
/* enable logMsg with variable number of arguments */
   #ifndef __PROTOTYPE_5_0
   #define __PROTOTYPE_5_0
   #endif

   #include "vxWorks.h"
   #include "logLib.h"

   #ifdef __GNUC__
	   #define __TRACE(fmt, args...) logMsg(fmt, ##args)
   #endif

   #define PRINTF	printf
   #ifdef __GNUC__
      #define PRINTK __TRACE
   #else
      #define PRINTK logMsg
   #endif

   #ifdef __GNUC__
      #define DRV_LOG __TRACE
   #else
      #define DRV_LOG logMsg
   #endif
#endif /* VXWORKS */

#ifdef LINUX

/*
to see the debug output on console
use following line:
   echo 8 > /proc/sys/kernel/printk
*/
#ifdef __KERNEL__
   #define __TRACE(fmt,args...) printk(KERN_DEBUG fmt "\r", ##args)
   #define PRINTF __TRACE
   #define DRV_LOG __TRACE
#else
   #define PRINTF printf
   #define DRV_LOG printf
#endif /* __KERNEL__ */
#endif /* LINUX */

#ifdef OSE
   #ifdef USE_DEBUG_PRINTF
   #include "dbgprintf.h"
   #define PRINTF dbgprintf
   #define DRV_LOG(x) dbgprintf( "%s", x)
   #else
   #define PRINTF printf
   #define DRV_LOG(x) printf( "%s", x)
   #endif/* USE_DEBUG_PRINTF*/
#endif /* OSE */

#ifdef PSOS
   #define PRINTF	printf
   #define DRV_LOG(x)
#endif /* PSOS */

#ifdef NO_OS
   #define PRINTF	printf
   #define PRINTK printf
   #define DRV_LOG printf
#endif /* NO_OS */

#ifdef WIN32
   #ifdef TRACE
      #undef TRACE
   #endif /* TRACE */
    #define PRINTF print
    #define DRV_LOG print
#endif /* WIN32 */


#ifdef ENABLE_TRACE
/****************************************************************************
Description:
   Prototype for a trace group.
Arguments:
   name - Name of the trace group
Return Value:
   None.
Remarks:
   None.
Example:
   DECLARE_TRACE_GROUP(LLC) - declares a logical trace group named LLC.
****************************************************************************/
#define DECLARE_TRACE_GROUP(name) extern unsigned int G_nTraceGroup##name


/****************************************************************************
Description:
   Create a trace group.
Arguments:
   name - Name of the trace group
Return Value:
   None.
Remarks:
   This has to be done once in the project. We do it in the file prj_debug.c.
   The default level of this trace group is DBG_LEVEL_HIGH.
Example:
   CREATE_TRACE_GROUP(LLC) - creates a logical trace group named LLC.
****************************************************************************/
#define CREATE_TRACE_GROUP(name) unsigned int G_nTraceGroup##name = DBG_LEVEL_HIGH


/****************************************************************************
Description:
   Prints a trace message.
Arguments:
   name - Name of the trace group
   level - level of this message
   message - a printf compatible formated string + opt. arguments
Return Value:
   None.
Remarks:
   The string will be redirected via printf if level is higher or equal to the
   actual level for this trace group ( please see SetTraceLevel ).
Example:
   TRACE(LLC,DBG_LEVEL_NORMAL,("LLC> State:%d\n", nState));
****************************************************************************/
#define TRACE(name,level,message) do {if(level >= G_nTraceGroup##name) \
      { PRINTF message ; } } while(0)


/****************************************************************************
Description:
   Set the actual level of a trace group.
Arguments:
   name - Name of the trace group
   new_level - new trace level
Return Value:
   None.
Remarks:
   None.
****************************************************************************/
#define SetTraceLevel(name, new_level) {G_nTraceGroup##name = new_level;}

/****************************************************************************
Description:
   Get the actual level of a trace group.
Arguments:
   name - Name of the trace group
   new_level - new trace level
Return Value:
   None.
Remarks:
   None.
****************************************************************************/
#define GetTraceLevel(name) G_nTraceGroup##name

#else /* ENABLE_TRACE */
   #define DECLARE_TRACE_GROUP(name)
   #define CREATE_TRACE_GROUP(name)
   #define TRACE(name,level,message) {}
   #define SetTraceLevel(name, new_level) {}
   #define GetTraceLevel(name) 0
#endif /* ENABLE_TRACE */


#ifdef ENABLE_LOG
/****************************************************************************
Description:
   Prototype for a log group.
Arguments:
   name - Name of the log group
Return Value:
   None.
Remarks:
   None.
Example:
   DECLARE_LOG_GROUP(LLC) - declares a logical log group named LLC.
****************************************************************************/
#define DECLARE_LOG_GROUP(diag_group) extern unsigned int G_nLogGroup##diag_group


/****************************************************************************
Description:
   Create a log group.
Arguments:
   name - Name of the log group
Return Value:
   None.
Remarks:
   This has to be done once in the project. We do it in the file prj_debug.c.
   The default level of this log group is DBG_LEVEL_HIGH.
Example:
   CREATE_LOG_GROUP(LLC) - creates a logical log group named LLC.
****************************************************************************/
#define CREATE_LOG_GROUP(diag_group) unsigned int G_nLogGroup##diag_group = DBG_LEVEL_HIGH


/****************************************************************************
Description:
   Prints a log message.
Arguments:
   name - Name of the log group
   level - level of this message
   message - a c-string
Return Value:
   None.
Remarks:
   The string will be redirected via VOS_Log if level is higher or equal to the
   actual level for this log group ( please see SetLogLevel ).
Example:
   LOG(LLC,DBG_LEVEL_NORMAL,("LLC> State:%d\n", nState));
****************************************************************************/
/* lint -emacro(155,LOG)*/
/* lint -emacro(26,LOG)*/
/* PC Lint is not able to deal with structures like {commands}; */
#define LOG(diag_group,level,message) do {if(level >= G_nLogGroup##diag_group) \
   { DRV_LOG message; }else{}} while(0)

/****************************************************************************
Description:
   Set the actual level of a log group.
Arguments:
   name - Name of the log group
   new_level - new log level
Return Value:
   None.
Remarks:
   None.
****************************************************************************/
#define SetLogLevel(diag_group, new_level) {G_nLogGroup##diag_group = new_level;}

/****************************************************************************
Description:
   Get the actual level of a log group.
Arguments:
   name - Name of the log group
   new_level - new log level
Return Value:
   None.
Remarks:
   None.
****************************************************************************/
#define GetLogLevel(diag_group) G_nLogGroup##diag_group

#else /* ENABLE_LOG */
   #define DECLARE_LOG_GROUP(diag_group)
   #define CREATE_LOG_GROUP(diag_group)
   #define LOG(diag_group,level,message) {}
   #define SetLogLevel(diag_group, new_level) {}
   #define GetLogLevel(diag_group) 0
#endif /* ENABLE_LOG */

#ifdef DEBUG
extern void * sys_dbgMalloc(int size, int line, const char* sFile);
extern void sys_dbgFree (void *pBuf, int line, const char* sFile);
extern void dbgResult(void);
#endif /* DEBUG */

#endif /* _sys_drv_debug_h */


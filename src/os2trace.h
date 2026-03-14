/******************************************************************************/
/*                                                                            */
/* File name    : OS2TRACE.H                                                  */
/*                                                                            */
/* Title        : OS/2 API trace include file                                 */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.00 12Jun95 Created (supported LX format and 32-bit Dos */
/*                                APIs only)                                  */
/*                2.30.01 09Jul95 Issued warning if trace DLL not on LIBPATH  */
/*                2.30.02 18Jul95 Added optional "NO" prefix to API groups    */
/*                2.30.03 20Jul95 Added 32-bit Win APIs                       */
/*                2.30.04 28Jul95 Fixed integer buffer logging                */
/*                2.30.05 04Aug95 Added 32-bit Gpi APIs                       */
/*                2.30.06 09Aug95 Fixed Dos32DevIOCtl bug                     */
/*                2.30.07 11Aug95 Changed to new build structure              */
/*                2.30.08 11Aug95 Added 32-bit Ddf APIs                       */
/*                2.30.09 12Aug95 Added 32-bit Dev APIs                       */
/*                2.30.10 13Aug95 Added 32-bit Drg APIs                       */
/*                2.30.11 14Aug95 Added 32-bit Prf APIs                       */
/*                2.30.12 25Aug95 Fixed Dos32QueryMessageCP bug               */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.30.14 29Aug95 Added 32-bit Prt and Spl APIs               */
/*                2.30.15 29Aug95 Added 32-bit Pic APIs                       */
/*                2.30.16 22Sep95 Enhanced -B option                          */
/*                2.30.17 22Sep95 Added -F option                             */
/*                2.30.18 24Sep95 Added -T option                             */
/*                2.30.19 02Nov95 Fixed Drg32DragFiles bug                    */
/*                2.30.20 27Nov95 Added support for  Dos APIs  in  MSG,  NLS, */
/*                                QUECALLS, and SESMGR exported by DOSCALLS   */
/*                2.30.21 13Dec95 Forwarded private entry table ordinals      */
/*                2.30.22 19Dec95 Added new OS/2 3.00 non-Uni Dos APIs        */
/*                2.30.23 27Mar96 Fixed PMWIN bugs                            */
/*                2.30.24 29Mar96 Added PM interface (supported customization */
/*                                and help only)                              */
/*                2.30.25 15Apr96 Verified DOS header new header file address */
/*                2.30.26 23May96 Issued more specific enablement messages    */
/*                2.30.27 13Jun96 Logged FEA2 structure EA value              */
/*                2.30.28 14Jun96 Changed "\r\n" to "\n" in output            */
/*                2.30.29 21Jun96 Fixed Dos32UnwindException bug              */
/*                2.30.30 26Jun94 Added OS/2 2.00 PM debugger APIs            */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.01 27Sep96 Fixed customization information display bug */
/*                2.40.02 11Mar97 Removed  loading/unloading  trace DLLs from */
/*                                .EXE                                        */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.04 12Mar97 Fixed OS/2 for PowerPC DLL support          */
/*                2.40.05 12Mar97 Added enablement support to PM interface    */
/*                2.40.06 12Mar97 Fixed Win32EnumObjectClasses bug            */
/*                2.40.07 13Mar97 Added customization cancel confirmation     */
/*                2.40.08 13Mar97 Added summarization support to PM interface */
/*                2.40.09 14Mar97 Removed  summarization   API  exit  without */
/*                                entry error                                 */
/*                2.40.10 14Mar97 Fixed Dev32StdOpen bug                      */
/*                2.40.11 19Mar97 Added  save  window  position  option to PM */
/*                                interface                                   */
/*                2.40.12 19Mar97 Added support for NE format                 */
/*                2.40.13 20Mar97 Added  launch   .EXE  files  option  to  PM */
/*                                interface                                   */
/*                2.40.14 25Mar97 Fixed DENA2 buffer logging                  */
/*                2.40.15 01Apr97 Fixed Dos32FindFirst bug                    */
/*                2.40.16 15Apr97 Removed OS/2 for PowerPC from PM help       */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.18 30Apr97 Added 16-bit Kbd APIs                       */
/*                2.40.19 01May97 Added 16-bit Mou APIs                       */
/*                2.40.20 05May97 Added 16-bit Vio APIs                       */
/*                2.40.21 20May97 Fixed/minimized 16-bit stack usage          */
/*                2.40.22 22May97 Added FAQ                                   */
/*                2.40.23 27May97 Linked .EXEs with proper threaded C library */
/*                2.40.24 03Jun97 Added FAQ to PM interface help submenu      */
/*                2.40.25 04Jun97 Fixed 16-bit registers structure init bug   */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
/*                2.40.27 22Jul97 Added 16-bit Gpi APIs                       */
/*                2.40.28 06Aug97 Added 16-bit Dev APIs                       */
/*                2.40.29 07Aug97 Added 16-bit Drg APIs                       */
/*                2.40.30 08Aug97 Added 16-bit Pic APIs                       */
/*                2.40.31 08Aug97 Logged user-defined buffer initial bytes    */
/*                2.40.32 11Aug97 Added 16-bit Prf APIs                       */
/*                2.40.33 11Aug97 Added 16-bit Prt APIs                       */
/*                2.40.34 13Aug97 Added 16-bit DosPrint and Spl APIs          */
/*                2.40.35 13Aug97 Fixed Spl32EnumDevice/Spl32EnumQueue bug    */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.37 15Aug97 Fixed Vio16GetPhysBuf bug                   */
/*                2.40.38 15Aug97 Added TRACE-IT batch file                   */
/*                2.40.39 17Sep97 Fixed 16-bit DosProtect APIs bug            */
/*                2.40.40 12Jan98 Enhanced performance                        */
/*                2.40.41 12Jan98 Fixed Dos16GetInfoSeg bug                   */
/*                2.40.42 23Jan98 Added PRQINFO8 structure                    */
/*                2.40.43 23Jan98 Fixed Dos16PrintQSetInfo/Spl32SetQueue bug  */
/*                2.40.44 27Jan98 Fixed returned MRESULT thunk bug            */
/*                2.40.45 28Jan98 Added indeterminate API return codes        */
/*                2.40.46 28Jan98 Fixed summarization alphabetical API bug    */
/*                2.40.47 29Jan98 Added Win16DefAVioWindowProc                */
/*                2.40.48 30Jan98 Fixed Win16Catch bug                        */
/*                2.40.49 04Feb98 Added OS2TRACE.API text file                */
/*                2.40.50 06Feb98 Fixed CX and DX  corruption by  16-bit Kbd, */
/*                                Mou, and Vio APIs                           */
/*                2.40.51 18Feb98 Verified saved  window  position  origin on */
/*                                desktop                                     */
/*                2.40.52 16Mar98 Fixed file dialog root directory bug        */
/*                2.40.53 16Mar98 Fixed file dialog clean up bug              */
/*                2.40.54 16Mar98 Changed main window identifier to 1         */
/*                2.40.55 02Jun98 Fixed DevQueryHardcopyCaps bug              */
/*                2.40.56 02Jun98 Changed default selection  for cancellation */
/*                                dialog to "No"                              */
/*                2.40.57 25Jun98 Added trace-enabled executable checker      */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.59 06Jul98 Added bitmaps of dialogs to PM help         */
/*                2.40.60 08Jul98 Added  title/action  bar only  option to PM */
/*                                interface                                   */
/*                2.40.61 17Aug98 Added SMP Dos APIs                          */
/*                2.40.62 18Aug98 Removed  unnecesary  semaphore  nesting  in */
/*                                DosFindFirst/DosFindClose                   */
/*                2.40.63 19Aug98 Fixed -F option                             */
/*                2.40.64 21Aug98 Added bitmap of logo to PM help             */
/*                2.40.65 28Aug98 Added monitor trace options to PM interface */
/*                2.40.66 09Sep98 Added private profile Dos APIs              */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.68 12Oct98 Fixed/enhanced helping information          */
/*                2.40.69 21Oct98 Forwarded removed entry table ordinals      */
/*                2.40.70 21Oct98 Added miscellaneous Dos APIs                */
/*                2.40.71 22Oct98 Added PM AVIO Vio APIs                      */
/*                2.40.72 22Oct98 Moved Prt/Spl APIs loading to OS/2 2.30     */
/*                2.40.73 26Oct98 Fixed   Dos32GetMessage/Dos32QueryMessageCP */
/*                                bug                                         */
/*                2.40.74 27Oct98 Added WinSetErrorInfo                       */
/*                2.40.75 28Oct98 Added Gpi/Win APIs exported from PMBIDI     */
/*                2.40.76 28Oct98 Issued more specific trace DLL load errors  */
/*                2.40.77 28Oct98 Added Win32MessageBox2                      */
/*                2.40.78 16Nov98 Added miscellaneous Dos APIs                */
/*                2.40.79 24Nov98 Fixed Dos32QueryModFromEIP usage bug        */
/*                2.40.80 02Dec98 Added undocumented -DUMPSTACK option        */
/*                2.40.81 02Dec98 Added -E option                             */
/*                2.40.82 07Dec98 Fixed stack dump at DosExit bug             */
/*                2.40.83 12Jan99 Enhanced trace monitoring                   */
/*                2.40.84 20Jan99 Fixed monitor trace options dialogs bugs    */
/*                2.40.85 21Jan99 Added -P option                             */
/*                2.40.86 03Mar99 Added support  for executables  without DOS */
/*                                header                                      */
/*                2.40.87 23Mar99 Fixed 16-bit SMP Dos APIs loading bug       */
/*                2.45.00 15Apr99 Added new OS/2 2.45 APIs                    */
/*                2.45.01 02May99 Added  support for  Gpi/Win APIs  in PMGPI, */
/*                                PMSHAPI, and PMWIN exported by PMMERGE      */
/*                2.45.02 02May99 Fixed WinQueryWindowText bug                */
/*                2.45.03 02May99 Added   support  for  Drg  APIs  in  PMDRAG */
/*                                exported by PMCTLS                          */
/*                2.45.04 05May99 Added -Q option                             */
/*                2.45.05 09May99 Added support for  K and M  suffixes to  -B */
/*                                and -F options                              */
/*                2.45.06 16May99 Added support for  KB and MB  values to  PM */
/*                                interface                                   */
/*                2.45.07 21May99 Fixed Dos32Debug bug                        */
/*                2.45.08 11Jun99 Added support for .COM files                */
/*                2.45.09 11Jun99 Logged Dos16DevIOCtl(2) output buffers      */
/*                2.45.10 02Aug99 Moved common strings into T_COMMON          */
/*                2.45.11 03Aug99 Logged name of module  containing API being */
/*                                intercepted                                 */
/*                2.45.12 01Feb00 Displayed customization options on new line */
/*                2.45.13 01Feb00 Logged PID in errors and warnings           */
/*                2.45.14 02Feb00 Saved OS2.INI information only when changed */
/*                2.45.15 08Feb00 Standardized output                         */
/*                2.45.16 18Feb00 Added -C, -PAUSE, and -RESUME options       */
/*                2.45.17 21Feb00 Added restore window  position option to PM */
/*                                interface                                   */
/*                2.45.18 28Feb00 Removed import by name restriction          */
/*                2.45.19 29Feb00 Fixed  trace monitoring without  background */
/*                                font                                        */
/*                2.45.20 29Feb00 Fixed control semaphore IPE under OS/2 2.00 */
/*                2.45.21 29Feb00 Fixed resizing main window with zero height */
/*                                twice during trace monitoring               */
/*                2.45.22 01Mar00 Removed  drawing  background font if bitmap */
/*                                calculation too long                        */
/*                2.45.23 24May00 Handled running out of disk space           */
/*                2.45.24 01Jun00 Added -A option                             */
/*                2.45.25 03Jun00 Added trace information file API stripper   */
/*                2.45.26 10Jan01 Handled OS/2 2.45 APIs removed by WSeB CP   */
/*                2.45.27 05Jun01 Fixed Dos32CreateThread bug                 */
/*                2.45.28 14Jun01 Fixed Dos16(Protect)QFileInfo bug           */
/*                2.45.29 19Jun01 Added enter key support to PM interface     */
/*                2.45.30 22Jun01 Fixed Dos32UnwindException bug              */
/*                2.45.31 26Jun01 Added  capability of building  32-bit  only */
/*                                trace DLLs                                  */
/*                2.45.32 28Jun01 Added -U option                             */
/*                2.45.33 29Jun01 Added Win APIs exported from PMSHAPI not in */
/*                                public include file                         */
/*                2.45.34 10Jul01 Moved C run-time functions into T_COMMON    */
/*                2.45.35 02Dec03 Fixed Dos16CreateThread bug                 */
/*                2.45.36 03Dec03 Fixed WinSetErrorInfo user hook bug         */
/*                2.45.37 30Apr09 Fixed error-to-string bugs                  */
/*                2.45.38 04May09 Added Dos32DumpProcess errors               */
/*                2.45.39 10Oct10 Preserved FS in T_GetTID                    */
/*                2.45.40 25Oct10 Fixed PEAOP/PEAOP2 bug                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#define ERROR         1                          /* Abnormal termination code */
#define KILOBYTE      1024                       /* Kilobyte                  */
#define MEGABYTE      1048576                    /* Megabyte                  */
#define SUPPORTED_DLL 22                         /* Number supported DLLs     */
#define SYS_SEMAPHORE "\\SEM32\\OS2TRACE.SEM"    /* System semaphore name     */
#define SYS_SEMPAUSE  "\\SEM32\\OS2TRACE\\PAUSE.SEM"
                                                 /* Trace pause sem name      */
#define SYS_SEMRESUME "\\SEM32\\OS2TRACE\\RESUME.SEM"
                                                 /* Trace resume sem name     */
#define SYS_QUEUECTL  "\\QUEUES\\OS2TRACE\\CONTROL.QUE"
                                                 /* Trace control queue name  */
#define SYS_QUEUEMON  "\\QUEUES\\OS2TRACE\\MONITOR.QUE"
                                                 /* Trace monitor queue name  */
#define UNKNOWN_DLL   ((ULONG)-1)                /* Unknown DLL indicator     */

#define fAOPTION      0x00000001                 /* -A option indicator       */
#define fBOPTION      0x00000002                 /* -B option indicator       */
#define fCHANGED      0x00000004                 /* Changed state indicator   */
#define fCONTROL      0x00000008                 /* Trace control options     */
#define fCOPTION      0x00000010                 /* -C option indicator       */
#define fCUSTOM       0x00000020                 /* Trace customization opts  */
#define fDLL          0x00000040                 /* DLL indicator             */
#define fELF_FMT      0x00000080                 /* ELF executable format     */
#define fENABLE       0x00000100                 /* Trace enablement options  */
#define fEOPTION      0x00000200                 /* -E option indicator       */
#define fFOPTION      0x00000400                 /* -F option indicator       */
#define fIOPTION      0x00000800                 /* -I option indicator       */
#define fLAUNCH       0x00001000                 /* Launch exe indicator      */
#define fLX_FMT       0x00002000                 /* LX executable format      */
#define fMONAPI_OFF   0x00004000                 /* Monitor APIs indicator    */
#define fMONLEN_OFF   0x00008000                 /* Monitor length indicator  */
#define fNE_FMT       0x00010000                 /* NE executable format      */
#define fPAUSEOPT     0x00020000                 /* -PAUSE option indicator   */
#define fPOPTION      0x00040000                 /* -P option indicator       */
#define fQOPTION      0x00080000                 /* -Q option indicator       */
#define fRESUMEOPT    0x00100000                 /* -RESUME option indicator  */
#define fSUMMARY      0x00200000                 /* Trace summarization opts  */
#define fTOPTION      0x00400000                 /* -T option indicator       */
#define fTRACEDLL     0x00800000                 /* Trace DLL indicator       */
#define fTRACE_OFF    0x01000000                 /* Disable trace indicator   */
#define fTRACE_ON     0x02000000                 /* Enable trace indicator    */
#define fUNDOCOPT     0x04000000                 /* Undoc option indicator    */
#define fUOPTION      0x08000000                 /* -U option indicator       */

#define ALTDIR_KEY    "Alternative Directory"    /* Alternative directory key */
#define DEF_ALTDIR    ""                         /* Def alternative directory */

#define BUFLEN_KEY    "Maximum Buffer Length"    /* Buffer length key         */
#define DEF_BUFLEN    "0x00000100"               /* Default buffer length     */
#define DEF_BUFLENInt 256                        /* Default buffer length     */
#define INC_BUFLEN    16                         /* Buffer length increment   */
#define MAX_BUFLEN    65536                      /* Maximum buffer length     */
#define MIN_BUFLEN    16                         /* Minimum buffer length     */

#define CTLFLG_KEY    "Trace Control Flags"      /* Trace control flags key   */
#define DEF_CTLFLG    "0x00000000"               /* Def trace control flags   */
#define DEF_CTLFLGInt 0x00000000                 /* Def trace control flags   */
#define C_PAUSE       0x00000001                 /* Pause tracing             */

#define DOSGRP_KEY    "DOSCALLS APIs Groups"     /* DOSCALLS groups key       */
#define DEF_DOSGRP    "0x000FFFFF"               /* Default DOSCALLS groups   */
#define DEF_DOSGRPInt D_ALL                      /* Default DOSCALLS groups   */
#define D_ALL         0x000FFFFF                 /* All DOSCALLS groups       */
#define D_DEV         0x00000001                 /* DOSCALLS device group     */
#define D_FILE        0x00000002                 /* DOSCALLS file group       */
#define D_MEM         0x00000004                 /* DOSCALLS memory group     */
#define D_MISC        0x00000008                 /* DOSCALLS misc group       */
#define D_MOD         0x00000010                 /* DOSCALLS module group     */
#define D_MVDM        0x00000020                 /* DOSCALLS MVDM group       */
#define D_NLS         0x00000040                 /* DOSCALLS NLS group        */
#define D_PIPE        0x00000080                 /* DOSCALLS pipe group       */
#define D_PRF         0x00000100                 /* DOSCALLS profile group    */
#define D_PROC        0x00000200                 /* DOSCALLS process group    */
#define D_RES         0x00000400                 /* DOSCALLS resource group   */
#define D_SEM         0x00000800                 /* DOSCALLS semaphore group  */
#define D_SES         0x00001000                 /* DOSCALLS session group    */
#define D_TIME        0x00002000                 /* DOSCALLS time group       */
#define D_XCPT        0x00004000                 /* DOSCALLS exception group  */
#define D_MSG         0x00008000                 /* DOSCALLS message group    */
#define D_INFO        0x00010000                 /* DOSCALLS infoseg group    */
#define D_SIG         0x00020000                 /* DOSCALLS signals group    */
#define D_SMP         0x00040000                 /* DOSCALLS SMP group        */
#define D_PROF        0x00080000                 /* DOSCALLS DosProfile API   */

#define FILLEN_KEY    "File Wrapping Length"     /* File wrapping length key  */
#define DEF_FILLEN    "0x00000000"               /* Default file wrapping len */
#define DEF_FILLENInt 0                          /* Default file wrapping len */
#define INC_FILLEN    4096                       /* File length increment     */
#define MAX_FILLEN    67108864                   /* Maximum file wrapping len */
#define MIN_FILLEN    4096                       /* Minimum file wrapping len */

#define GPIGRP_KEY    "PMGPI APIs Groups"        /* PMGPI groups key          */
#define DEF_GPIGRP    "0x0000FFFF"               /* Default PMGPI groups      */
#define DEF_GPIGRPInt G_ALL                      /* Default PMGPI groups      */
#define G_ALL         0x0000FFFF                 /* All PMGPI groups          */
#define G_BIT         0x00000001                 /* PMGPI bitmap group        */
#define G_CORR        0x00000002                 /* PMGPI correlation group   */
#define G_CTRL        0x00000004                 /* PMGPI control group       */
#define G_DEF         0x00000008                 /* PMGPI defaults group      */
#define G_EDIT        0x00000010                 /* PMGPI segment edit group  */
#define G_LCID        0x00000020                 /* PMGPI LCID group          */
#define G_LCT         0x00000040                 /* PMGPI log color tbl group */
#define G_META        0x00000080                 /* PMGPI metafile group      */
#define G_PATH        0x00000100                 /* PMGPI path group          */
#define G_POLY        0x00000200                 /* PMGPI polygon group       */
#define G_PRIM        0x00000400                 /* PMGPI primitive group     */
#define G_RGN         0x00000800                 /* PMGPI region group        */
#define G_SEG         0x00001000                 /* PMGPI segment group       */
#define G_TRAN        0x00002000                 /* PMGPI transform group     */
#define G_DEV         0x00004000                 /* PMGPI device group        */
#define G_INK         0x00008000                 /* PMGPI ink group           */

#define TRCFLG_KEY    "Trace Flags"              /* Trace flags key           */
#define DEF_TRCFLG    "0x00000000"               /* Default trace flags       */
#define DEF_TRCFLGInt 0x00000000                 /* Default trace flags       */
#define F_TIMESTAMP   0x00000001                 /* Time stamp API entry/exit */
#define F_INTERCEPT   0x00000002                 /* Intercept dynamic API     */
#define F_DUMPSTACK   0x00000004                 /* Dump stack at DosExit     */
#define F_EBCDIC      0x00000008                 /* Log EBCDIC translation    */
#define F_CONTROL     0x00000010                 /* Control tracing           */

#define TRCLVL_KEY    "Trace Information Level"  /* Trace level key           */
#define DEF_TRCLVL    "0x00000001"               /* Default trace level       */
#define DEF_TRCLVLInt L_ENTEXT                   /* Default trace level       */
#define L_ENTEXT      1                          /* API entry/exit trace lvl  */
#define L_PARMS       2                          /* API parameters trace lvl  */
#define L_CONTENTS    3                          /* API parm contents trc lvl */
#define MAX_TRCLVL    L_CONTENTS                 /* Maximum trace level       */
#define MIN_TRCLVL    L_ENTEXT                   /* Minimum trace level       */

#ifdef T_USERHOOK
#define USERHK_KEY    "User Hook"                /* User hook key             */
#define DEF_USERHK    ""                         /* Default user hook         */
#define U_INIT        0                          /* Hook initialization flag  */
#define U_TERM        1                          /* Hook termination flag     */
#define U_ENTRY       2                          /* Hook API entry flag       */
#define U_EXIT        3                          /* Hook API exit flag        */
#endif

#define WINGRP_KEY    "PMWIN APIs Groups"        /* PMWIN groups key          */
#define DEF_WINGRP    "0x0FFFFFFF"               /* Default PMWIN groups      */
#define DEF_WINGRPInt W_ALL                      /* Default PMWIN groups      */
#define W_ALL         0x0FFFFFFF                 /* All PMWIN groups          */
#define W_ACCL        0x00000001                 /* PMWIN accelerator group   */
#define W_ATOM        0x00000002                 /* PMWIN atom group          */
#define W_CLIP        0x00000004                 /* PMWIN clipboard group     */
#define W_CTRY        0x00000008                 /* PMWIN country group       */
#define W_CUR         0x00000010                 /* PMWIN cursor group        */
#define W_DDE         0x00000020                 /* PMWIN DDE group           */
#define W_DESK        0x00000040                 /* PMWIN desktop group       */
#define W_DLG         0x00000080                 /* PMWIN dialog group        */
#define W_DWIN        0x00000100                 /* PMWIN default win group   */
#define W_ERR         0x00000200                 /* PMWIN error group         */
#define W_FRAM        0x00000400                 /* PMWIN frame group         */
#define W_HOOK        0x00000800                 /* PMWIN hook group          */
#define W_INPT        0x00001000                 /* PMWIN input group         */
#define W_LOAD        0x00002000                 /* PMWIN load group          */
#define W_MENU        0x00004000                 /* PMWIN menu group          */
#define W_MSG         0x00008000                 /* PMWIN message group       */
#define W_MSGL        0x00010000                 /* PMWIN message loop group  */
#define W_PAL         0x00020000                 /* PMWIN palette group       */
#define W_PTR         0x00040000                 /* PMWIN pointer group       */
#define W_RECT        0x00080000                 /* PMWIN rectangle group     */
#define W_SYS         0x00100000                 /* PMWIN system group        */
#define W_THK         0x00200000                 /* PMWIN thunk group         */
#define W_TIME        0x00400000                 /* PMWIN time group          */
#define W_TREC        0x00800000                 /* PMWIN track rect group    */
#define W_WIN         0x01000000                 /* PMWIN window group        */
#define W_ENV         0x02000000                 /* PMWIN environment group   */
#define W_HEAP        0x04000000                 /* PMWIN heap group          */
#define W_SEI         0x08000000                 /* PMWIN set err info group  */

#define MONAPI_KEY    "Number APIs Monitoring"   /* Number APIs key           */
#define DEF_MONAPI    "0x00000064"               /* Default number APIs       */
#define DEF_MONAPIInt 100                        /* Default number APIs       */
#define INC_MONAPI    25                         /* Number APIs increment     */
#define MAX_MONAPI    1000000                    /* Maximum number APIs       */
#define MIN_MONAPI    25                         /* Minimum number APIs       */

#define MONLEN_KEY    "File Length Monitoring"   /* Log file length key       */
#define DEF_MONLEN    "0x00010000"               /* Default log file length   */
#define DEF_MONLENInt 65536                      /* Default log file length   */
#define INC_MONLEN    4096                       /* Log file length increment */
#define MAX_MONLEN    67108864                   /* Maximum log file length   */
#define MIN_MONLEN    4096                       /* Minimum log file length   */

#define APIEXT_PASS   1                          /* Passed API invocation     */
#define APIEXT_FAIL   2                          /* Failed API invocation     */
#define APIEXT_IND    3                          /* Indeterminate invocation  */

#define LEN_FILENAME  CCHMAXPATH                 /* File name length          */
#define LEN_IOLINE    256                        /* Input/output line length  */
#define MAX_APINAME   112                        /* Maximum API name length   */
#define MAX_THREAD    0x0FFF                     /* Maximum threads           */
#define MAX_UNSNRRNAM 35                         /* Max unsupported NRRNAMs   */

#define TRCMON_PRCINI 1                          /* Initialize monitoring proc*/
#define TRCMON_NUMAPI 2                          /* Update number APIs        */
#define TRCMON_FILLEN 3                          /* Update file length        */
#define TRCMON_PRCTRM 4                          /* Terminate monitoring proc */
#define TRCMON_QUIT   5                          /* Quit monitoring           */

#define TRCCTL_PAUSE  1                          /* Pause tracing             */
#define TRCCTL_RESUME 2                          /* Resume tracing            */
#define TRCCTL_QUIT   3                          /* Quit tracing              */

#define VER_FULLNAME  "OS/2 API Trace"           /* API trc enabler full name */
#define LEN_FULLNAME  14                         /* Enabler full name length  */
#define VER_NAME      "OS2TRACE"                 /* API trace enabler name    */
#define VER_DATE      "25 Oct 2010"              /* Version date              */
#define VER_MAJOR     2                          /* Major version             */
#define VER_MINOR     45                         /* Minor version             */
#define VER_REV       40                         /* Revision version          */

/******************************************************************************/
/*                                                                            */
/* Macros                                                                     */
/*                                                                            */
/******************************************************************************/

#define CLRFLAG(f,m)  f &= ~(m)

#define FLAGCLR(f,m)  (!(f & (m)))

#define FLAGSET(f,m)  (f & (m))

#define SETFLAG(f,m)  f |= (m)

/******************************************************************************/
/*                                                                            */
/* Structure declarations                                                     */
/*                                                                            */
/******************************************************************************/

typedef struct _apiinfo                          /* API information           */
{
   struct _apiinfo *pNext;                       /* Next API information      */
   CHAR             szName[ MAX_APINAME ];       /* API name                  */
   ULONG            ulCall;                      /* API invocations           */
   ULONG            ulPass;                      /* Passed API invocations    */
   ULONG            ulFail;                      /* Failed API invocations    */
   ULONG            ulIndeterminate;             /* Indeterminate invocations */
}  APIINFO, *PAPIINFO;

typedef struct _dllinfo                          /* DLL information           */
{
   PCHAR szName;                                 /* Name                      */
   ULONG ulMinOrd386;                            /* Minimum Intel ordinal     */
   ULONG ulMaxOrd386;                            /* Maximum Intel ordinal     */
   PCHAR szTraceDLLName;                         /* Trace DLL name            */
   BOOL  bEnable;                                /* Trace on/off enablement   */
   ULONG ulFlags;                                /* Flags                     */
}  DLLINFO, *PDLLINFO;

typedef struct _moninitinfo                      /* Trace monitor init info   */
{
   ULONG ulFlags;                                /* Flags                     */
   CHAR  szExeName[ LEN_FILENAME ];              /* Executable file name      */
   CHAR  szLogName[ LEN_FILENAME ];              /* Log file name             */
}  MONINITINFO, *PMONINITINFO;

typedef struct _unsNRRNAM                        /* Unsupported NRRNAM info   */
{
   PCHAR szDLL;                                  /* DLL name                  */
   ULONG ulNumAPI;                               /* Unsupported APIs          */
   PCHAR szAPI[ MAX_UNSNRRNAM ];                 /* API names                 */
}  UNSNRRNAM, *PUNSNRRNAM;

typedef CONTEXTRECORD                CTXREC;     /* Exception context and     */
typedef CONTEXTRECORD               *PCTXREC;    /*   corresponding pointer   */
typedef EXCEPTIONREGISTRATIONRECORD  REGREC;     /* Registration record and   */
typedef EXCEPTIONREGISTRATIONRECORD *PREGREC;    /*   corresponding pointer   */
typedef EXCEPTIONREPORTRECORD        REPREC;     /* Exception information and */
typedef EXCEPTIONREPORTRECORD       *PREPREC;    /*   corresponding pointer   */

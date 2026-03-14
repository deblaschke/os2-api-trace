/******************************************************************************/
/*                                                                            */
/* File name    : T_DOS___.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit DOSCALLS routines      */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from DOSCALLS.DLL.   */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.00 12Jun95 Created (supported LX format and 32-bit Dos */
/*                                APIs only)                                  */
/*                2.30.04 28Jul95 Fixed integer buffer logging                */
/*                2.30.06 09Aug95 Fixed Dos32DevIOCtl bug                     */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.30.22 19Dec95 Added new OS/2 3.00 non-Uni Dos APIs        */
/*                2.30.29 21Jun96 Fixed Dos32UnwindException bug              */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.21 20May97 Fixed/minimized 16-bit stack usage          */
/*                2.40.25 04Jun97 Fixed 16-bit registers structure init bug   */
/*                2.40.31 08Aug97 Logged user-defined buffer initial bytes    */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.39 17Sep97 Fixed 16-bit DosProtect APIs bug            */
/*                2.40.41 12Jan98 Fixed Dos16GetInfoSeg bug                   */
/*                2.40.50 06Feb98 Fixed CX and DX  corruption by  16-bit Kbd, */
/*                                Mou, and Vio APIs                           */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.61 17Aug98 Added SMP Dos APIs                          */
/*                2.40.62 18Aug98 Removed  unnecesary  semaphore  nesting  in */
/*                                DosFindFirst/DosFindClose                   */
/*                2.40.66 09Sep98 Added private profile Dos APIs              */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.70 21Oct98 Added miscellaneous Dos APIs                */
/*                2.40.76 28Oct98 Issued more specific trace DLL load errors  */
/*                2.40.78 16Nov98 Added miscellaneous Dos APIs                */
/*                2.40.80 02Dec98 Added undocumented -DUMPSTACK option        */
/*                2.40.87 23Mar99 Fixed 16-bit SMP Dos APIs loading bug       */
/*                2.45.00 15Apr99 Added new OS/2 2.45 APIs                    */
/*                2.45.09 11Jun99 Logged Dos16DevIOCtl(2) output buffers      */
/*                2.45.10 02Aug99 Moved common strings into T_COMMON          */
/*                2.45.26 10Jan01 Handled OS/2 2.45 APIs removed by WSeB CP   */
/*                2.45.27 05Jun01 Fixed Dos32CreateThread bug                 */
/*                2.45.28 14Jun01 Fixed Dos16(Protect)QFileInfo bug           */
/*                2.45.31 26Jun01 Added  capability of building  32-bit  only */
/*                                trace DLLs                                  */
/*                2.45.32 28Jun01 Added -U option                             */
/*                2.45.34 10Jul01 Moved C run-time functions into T_COMMON    */
/*                2.45.35 02Dec03 Fixed Dos16CreateThread bug                 */
/*                2.45.40 25Oct10 Fixed PEAOP/PEAOP2 bug                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#define  INCL_BASE                               /* Include base functions    */
#include <os2.h>                                 /* OS/2 information          */
#include "os2trace.h"                            /* OS2TRACE information      */
#include "t_common.h"                            /* T_COMMON information      */

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#pragma stack16( 0 )
#endif

/******************************************************************************/
/*                                                                            */
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#define MAX_DFFINFO                     512      /* Maximum outstanding calls */

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#define ORD_DOS16ACQUIRESPINLOCK        450      /* OS/2 SMP proc ordinal     */
#define ORD_DOS16CREATESPINLOCK         449      /* OS/2 SMP proc ordinal     */
#define ORD_DOS16FREESPINLOCK           452      /* OS/2 SMP proc ordinal     */
#define ORD_DOS16RELEASESPINLOCK        451      /* OS/2 SMP proc ordinal     */
#endif
#define ORD_DOS32ACQUIRESPINLOCK        558      /* OS/2 SMP proc ordinal     */
#define ORD_DOS32CREATESPINLOCK         557      /* OS/2 SMP proc ordinal     */
#define ORD_DOS32FREESPINLOCK           560      /* OS/2 SMP proc ordinal     */
#define ORD_DOS32GETPROCESSORSTATUS     447      /* OS/2 SMP proc ordinal     */
#define ORD_DOS32RELEASESPINLOCK        559      /* OS/2 SMP proc ordinal     */
#define ORD_DOS32SETPROCESSORSTATUS     448      /* OS/2 SMP proc ordinal     */
#define ORD_DOS32TESTPSD                453      /* OS/2 SMP proc ordinal     */
#if ( OS2VER >= 210 )
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#define ORD_DOS16QUERYMODFROMCS         359      /* OS/2 2.10 proc ordinal    */
#endif
#define ORD_DOS32DUMPPROCESS            113      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTCLOSE           638      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTENUMATTRIBUTE   636      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTOPEN            637      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTQUERYFHSTATE    645      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTQUERYFILEINFO   646      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTREAD            641      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTSETFHSTATE      644      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTSETFILEINFO     643      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTSETFILELOCKS    639      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTSETFILEPTR      621      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTSETFILESIZE     640      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32PROTECTWRITE           642      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32QUERYABIOSSUPPORT      371      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32QUERYMODFROMEIP        360      /* OS/2 2.10 proc ordinal    */
#define ORD_DOS32SUPPRESSPOPUPS         114      /* OS/2 2.10 proc ordinal    */
#endif
#if ( OS2VER >= 230 )
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#define ORD_DOS16PROTECTCHGFILEPTR      634      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTCLOSE           622      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTENUMATTRIBUTE   635      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTFILEIO          623      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTFILELOCKS       624      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTNEWSIZE         627      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTOPEN            628      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTQFHANDSTATE     629      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTQFILEINFO       631      /* OS/2 2.30 proc ordinal    */
#if FALSE
#define ORD_DOS16PROTECTREAD            ???      /* OS/2 2.30 proc ordinal    */
#endif
#define ORD_DOS16PROTECTSETFHANDSTATE   630      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16PROTECTSETFILEINFO     632      /* OS/2 2.30 proc ordinal    */
#if FALSE
#define ORD_DOS16PROTECTWRITE           ???      /* OS/2 2.30 proc ordinal    */
#endif
#define ORD_DOS16QUERYEXTLIBPATH        999      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS16SETEXTLIBPATH          998      /* OS/2 2.30 proc ordinal    */
#endif
#define ORD_DOS32ALLOCTHREADLOCALMEMORY 454      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS32FREETHREADLOCALMEMORY  455      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS32PMPOSTEVENTSEM         590      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS32PMREQUESTMUTEXSEM      593      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS32PMWAITEVENTSEM         591      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS32PMWAITMUXWAITSEM       595      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS32QUERYEXTLIBPATH        874      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS32SETEXTLIBPATH          873      /* OS/2 2.30 proc ordinal    */
#define ORD_DOS32VERIFYPIDTID           460      /* OS/2 2.30 proc ordinal    */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
#define ORD_DOS32PERFSYSCALL            976      /* OS/2 2.40 proc ordinal    */
#define ORD_DOS32QUERYTHREADCONTEXT     877      /* OS/2 2.40 proc ordinal    */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
#ifdef T_16BIT
#define ORD_DOS16FSRAMSEMCLEAR2         992      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS16FSRAMSEMREQUEST2       991      /* OS/2 2.45 proc ordinal    */
#endif
#define ORD_DOS32CANCELLOCKREQUESTL     987      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32CREATETHREAD2          997      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32LISTIO                 996      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32LISTIOL                990      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32OPENL                  981      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32PMR3POSTEVENTSEM       562      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32PMR3WAITEVENTSEM       561      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32PROTECTOPENL           982      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32PROTECTSETFILELOCKSL   983      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32PROTECTSETFILEPTRL     984      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32PROTECTSETFILESIZEL    985      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32QUERYTHREADAFFINITY    563      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32SETFILELOCKSL          986      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32SETFILEPTRL            988      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32SETFILESIZEL           989      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32SETTHREADAFFINITY      564      /* OS/2 2.45 proc ordinal    */
#define ORD_DOS32SYSTEMSERVICE          995      /* OS/2 2.45 proc ordinal    */
#endif
#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
#define ORD_DOS32CLOSERAMSEM            9005     /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32CREATERAMSEM           9004     /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32FINDFROMNAME           9018     /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32GETLOCALEMODULE        430      /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32OPENRAMSEM             9006     /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32QUERYPROCESSINFO       9010     /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32QUERYTHREADINFO        9011     /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32RELEASERAMSEM          9008     /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32REQUESTRAMSEM          9007     /* OS/2 3.00 proc ordinal    */
#define ORD_TEMP32WRITE                 900      /* OS/2 3.00 proc ordinal    */
#endif

/******************************************************************************/
/*                                                                            */
/* Structure declaration                                                      */
/*                                                                            */
/******************************************************************************/

typedef struct _DFFInfo                          /* DosFindFirst information  */
{
   HDIR  hDir;                                   /* Directory handle          */
   ULONG ulLevel;                                /* Information level         */
}  DFFINFO, *PDFFINFO;

/******************************************************************************/
/*                                                                            */
/* OS/2 function declarations                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
APIRET16 APIENTRY16 DOSGETPROCADDR( HMODULE16, PSZ16, PPFN16 );
#endif
APIRET   APIENTRY   DOS32FREEMODULE( HMODULE );
APIRET   APIENTRY   DOS32GETDATETIME( PDATETIME );
APIRET   APIENTRY   DOS32LOADMODULE( PSZ, ULONG, PSZ, PHMODULE );
APIRET   APIENTRY   DOS32QUERYPROCADDR( HMODULE, ULONG, PSZ, PFN * );

/******************************************************************************/
/*                                                                            */
/* C run-time function declarations                                           */
/*                                                                            */
/******************************************************************************/

#ifndef T_COMRUNTIME
void _fpreset( void );
int  _CRT_init( void );
void _CRT_term( void );
#ifdef INTEL
void __ctordtorInit( void );
void __ctordtorTerm( void );
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Function declarations                                                      */
/*                                                                            */
/******************************************************************************/

VOID  AddDFFLevel( HDIR, ULONG );
VOID  DelDFFLevel( HDIR );
ULONG GetDFFLevel( HDIR );

/******************************************************************************/
/*                                                                            */
/* Variable definitions                                                       */
/*                                                                            */
/******************************************************************************/

DFFINFO  aDFFInfo[ MAX_DFFINFO ];                /* DosFindFirst information  */
ULONG    ulDFFInfo;                              /* Outstanding DosFF calls   */
HMODULE  hmodDLL;                                /* DLL module handle         */
HMODULE  hmodDOSCALL1;                           /* DOSCALL1 module handle    */
HMODULE  hmodTraceDLL;                           /* Trace DLL module handle   */
PCHAR    szACPMsg =                              /* WSeB CP error message     */
                       "Returning ERROR_INVALID_FUNCTION for API (%s) removed by WSeB Convenience Package";
PCHAR    szDLL = "DOSCALLS";                     /* DLL module name           */
PCHAR    szDOSCALL1 = "DOSCALL1";                /* DOSCALL1 module name      */
PCHAR    szNotSMPMsg =                           /* Not SMP error message     */
                       "Returning ERROR_INVALID_FUNCTION for API (%s) not present on this version of OS/2";
PCHAR    szTraceDLL = "T_DOS___";                /* Trace DLL module name     */
ULONG    ulOS2TRACEVer =                         /* OS2TRACE version number   */
                         ( VER_MAJOR << 16 ) +
                         ( VER_MINOR <<  8 ) +
                         ( VER_REV         ) ;

#ifdef INTEL
#ifdef T_16BIT
APIRET16 (* _Seg16 APIENTRY16 pfnDOSACQUIRESPINLOCK)( HSPINLOCK );
                                                 /* OS/2 SMP procedure        */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSCREATESPINLOCK)( PHSPINLOCK16 );
                                                 /* OS/2 SMP procedure        */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSFREESPINLOCK)( HSPINLOCK );
                                                 /* OS/2 SMP procedure        */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSRELEASESPINLOCK)( HSPINLOCK );
                                                 /* OS/2 SMP procedure        */
#endif
APIRET   (* APIENTRY          pfnDOS32ACQUIRESPINLOCK)( HSPINLOCK );
                                                 /* OS/2 SMP procedure        */
APIRET   (* APIENTRY          pfnDOS32CREATESPINLOCK)( PHSPINLOCK );
                                                 /* OS/2 SMP procedure        */
APIRET   (* APIENTRY          pfnDOS32FREESPINLOCK)( HSPINLOCK );
                                                 /* OS/2 SMP procedure        */
APIRET   (* APIENTRY          pfnDOS32GETPROCESSORSTATUS)( ULONG, PULONG );
                                                 /* OS/2 SMP procedure        */
APIRET   (* APIENTRY          pfnDOS32RELEASESPINLOCK)( HSPINLOCK );
                                                 /* OS/2 SMP procedure        */
APIRET   (* APIENTRY          pfnDOS32SETPROCESSORSTATUS)( ULONG, ULONG );
                                                 /* OS/2 SMP procedure        */
APIRET   (* APIENTRY          pfnDOS32TESTPSD)( PSZ );
                                                 /* OS/2 SMP procedure        */
#endif
#if ( OS2VER >= 210 )
#ifdef INTEL
#ifdef T_16BIT
APIRET16 (* _Seg16 APIENTRY16 pfnDOSQUERYMODFROMCS)( SEL, PQMRESULT16 );
                                                 /* OS/2 2.10 procedure       */
#endif
APIRET   (* APIENTRY          pfnDOS32DUMPPROCESS)( ULONG, ULONG, PID );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTCLOSE)( HFILE, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTENUMATTRIBUTE)( ULONG, PVOID, ULONG, PVOID, ULONG, PULONG, ULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTOPEN)( PSZ, PHFILE, PULONG, ULONG, ULONG, ULONG, ULONG, PEAOP2, PFHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTQUERYFHSTATE)( HFILE, PULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTQUERYFILEINFO)( HFILE, ULONG, PVOID, ULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTREAD)( HFILE, PVOID, ULONG, PULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTSETFHSTATE)( HFILE, ULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTSETFILEINFO)( HFILE, ULONG, PVOID, ULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTSETFILELOCKS)( HFILE, PFILELOCK, PFILELOCK, ULONG, ULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTSETFILEPTR)( HFILE, LONG, ULONG, PULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTSETFILESIZE)( HFILE, ULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTWRITE)( HFILE, PVOID, ULONG, PULONG, FHLOCK );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32QUERYABIOSSUPPORT)( ULONG );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32QUERYMODFROMEIP)( PHMODULE, PULONG, ULONG, PCHAR, PULONG, ULONG );
                                                 /* OS/2 2.10 procedure       */
APIRET   (* APIENTRY          pfnDOS32SUPPRESSPOPUPS)( ULONG, ULONG );
                                                 /* OS/2 2.10 procedure       */
#else
PFN      pfnDOS32PROTECTCLOSE;                   /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTENUMATTRIBUTE;           /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTOPEN;                    /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTQUERYFHSTATE;            /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTQUERYFILEINFO;           /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTREAD;                    /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTSETFHSTATE;              /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTSETFILEINFO;             /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTSETFILELOCKS;            /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTSETFILEPTR;              /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTSETFILESIZE;             /* OS/2 2.10 procedure       */
PFN      pfnDOS32PROTECTWRITE;                   /* OS/2 2.10 procedure       */
PFN      pfnDOS32QUERYMODFROMEIP;                /* OS/2 2.10 procedure       */
#endif
#endif
#if ( OS2VER >= 230 )
#ifdef INTEL
#ifdef T_16BIT
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTCHGFILEPTR)( HFILE16, LONG, USHORT, PULONG16, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTCLOSE)( HFILE16, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTENUMATTRIBUTE)( USHORT, PVOID16, ULONG, PVOID16, ULONG, PULONG16, ULONG, FHLOCK, ULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTFILEIO)( HFILE16, PVOID16, USHORT, PUSHORT16, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTFILELOCKS)( HFILE16, PFILELOCK16, PFILELOCK16, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTNEWSIZE)( HFILE16, ULONG, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTOPEN)( PSZ16, PHFILE16, PUSHORT16, ULONG, USHORT, USHORT, ULONG, PEAOP16, PFHLOCK16, ULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTQFHANDSTATE)( HFILE16, PUSHORT16, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTQFILEINFO)( HFILE16, USHORT, PVOID16, USHORT, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
#if FALSE
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTREAD)( HFILE16, PVOID16, USHORT, PUSHORT16, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
#endif
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTSETFHANDSTATE)( HFILE16, USHORT, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTSETFILEINFO)( HFILE16, USHORT, PVOID16, USHORT, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
#if FALSE
APIRET16 (* _Seg16 APIENTRY16 pfnDOSPROTECTWRITE)( HFILE16, PVOID16, USHORT, PUSHORT16, FHLOCK );
                                                 /* OS/2 2.30 procedure       */
#endif
APIRET16 (* _Seg16 APIENTRY16 pfnDOSQUERYEXTLIBPATH)( PSZ16, ULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSSETEXTLIBPATH)( PSZ16, ULONG );
                                                 /* OS/2 2.30 procedure       */
#endif
APIRET   (* APIENTRY          pfnDOS32ALLOCTHREADLOCALMEMORY)( ULONG, PULONG * );
                                                 /* OS/2 2.30 procedure       */
APIRET   (* APIENTRY          pfnDOS32FREETHREADLOCALMEMORY)( PVOID );
                                                 /* OS/2 2.30 procedure       */
APIRET   (* APIENTRY          pfnDOS32PMPOSTEVENTSEM)( HEV );
                                                 /* OS/2 2.30 procedure       */
APIRET   (* APIENTRY          pfnDOS32PMREQUESTMUTEXSEM)( HEV, HMTX, ULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET   (* APIENTRY          pfnDOS32PMWAITEVENTSEM)( HEV, HEV, ULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET   (* APIENTRY          pfnDOS32PMWAITMUXWAITSEM)( HEV, HMUX, ULONG, PULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET   (* APIENTRY          pfnDOS32QUERYEXTLIBPATH)( PSZ, ULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET   (* APIENTRY          pfnDOS32SETEXTLIBPATH)( PSZ, ULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET   (* APIENTRY          pfnDOS32VERIFYPIDTID)( PID, TID );
                                                 /* OS/2 2.30 procedure       */
#else
PFN      pfnDOS32QUERYEXTLIBPATH;                /* OS/2 2.30 procedure       */
PFN      pfnDOS32SETEXTLIBPATH;                  /* OS/2 2.30 procedure       */
#endif
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
APIRET   (* APIENTRY          pfnDOS32PERFSYSCALL)( ULONG, ULONG, ULONG, ULONG );
                                                 /* OS/2 2.40 procedure       */
APIRET   (* APIENTRY          pfnDOS32QUERYTHREADCONTEXT)( TID, ULONG, PCONTEXTRECORD );
                                                 /* OS/2 2.40 procedure       */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
#ifdef T_16BIT
APIRET16 (* _Seg16 APIENTRY16 pfnDOSFSRAMSEMCLEAR2)( PFSRSEM216 );
                                                 /* OS/2 2.45 procedure       */
APIRET16 (* _Seg16 APIENTRY16 pfnDOSFSRAMSEMREQUEST2)( PFSRSEM216, LONG );
                                                 /* OS/2 2.45 procedure       */
#endif
APIRET   (* APIENTRY          pfnDOS32CANCELLOCKREQUESTL)( HFILE, PFILELOCKL );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32CREATETHREAD2)( PTHREADCREATE );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32LISTIO)( ULONG, ULONG, PLISTIO );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32LISTIOL)( LONG, LONG, VOID * );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32OPENL)( PSZ, PHFILE, PULONG, ULONG, ULONG, ULONG, ULONG, ULONG, PEAOP2 );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32PMR3POSTEVENTSEM)( GRESEMT * );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32PMR3WAITEVENTSEM)( GRESEMT *, ULONG, ULONG );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTOPENL)( PSZ, PHFILE, PULONG, ULONG, ULONG, ULONG, ULONG, ULONG, PEAOP2, PFHLOCK );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTSETFILELOCKSL)( HFILE, PFILELOCKL, PFILELOCKL, ULONG, ULONG, FHLOCK );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTSETFILEPTRL)( HFILE, ULONG, ULONG, ULONG, PLONGLONG, FHLOCK );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32PROTECTSETFILESIZEL)( HFILE, ULONG, ULONG, FHLOCK );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32QUERYTHREADAFFINITY)( ULONG, PMPAFFINITY );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32SETFILELOCKSL)( HFILE, PFILELOCKL, PFILELOCKL, ULONG, ULONG );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32SETFILEPTRL)( HFILE, ULONG, ULONG, ULONG, PLONGLONG );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32SETFILESIZEL)( HFILE, ULONG, ULONG );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32SETTHREADAFFINITY)( PMPAFFINITY );
                                                 /* OS/2 2.45 procedure       */
APIRET   (* APIENTRY          pfnDOS32SYSTEMSERVICE)( ULONG, PVOID, PVOID );
                                                 /* OS/2 2.45 procedure       */
#endif
#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
PFN      pfnDOS32CLOSERAMSEM;                    /* OS/2 3.00 procedure       */
PFN      pfnDOS32CREATERAMSEM;                   /* OS/2 3.00 procedure       */
PFN      pfnDOS32FINDFROMNAME;                   /* OS/2 3.00 procedure       */
PFN      pfnDOS32GETLOCALEMODULE;                /* OS/2 3.00 procedure       */
PFN      pfnDOS32OPENRAMSEM;                     /* OS/2 3.00 procedure       */
PFN      pfnDOS32QUERYPROCESSINFO;               /* OS/2 3.00 procedure       */
PFN      pfnDOS32QUERYTHREADINFO;                /* OS/2 3.00 procedure       */
PFN      pfnDOS32RELEASERAMSEM;                  /* OS/2 3.00 procedure       */
PFN      pfnDOS32REQUESTRAMSEM;                  /* OS/2 3.00 procedure       */
PFN      pfnTEMP32WRITE;                         /* OS/2 3.00 procedure       */
#endif

#ifdef POWERPC
#pragma code( ".initterm" );
#endif
/******************************************************************************/
/*                                                                            */
/* _DLL_InitTerm()                                                            */
/*                                                                            */
/******************************************************************************/

ULONG APIENTRY _DLL_InitTerm( ULONG ulModule     /* Module handle             */
                            , ULONG ulFlag       /* Init/term flag            */
                            )
{

   CHAR     szObjName[ LEN_FILENAME ];           /* Failing object name       */
   ULONG    ulOrdinal;                           /* Procedure ordinal         */
   APIRET16 usRC;                                /* API return code           */

   /*
    * Library initialization required if process attaching to T_DOS___
    */
   if ( ulFlag == 0L )
   {
#ifndef T_COMRUNTIME
      /*
       * Reset floating-point unit to default state (required by C Set/2)
       */
      _fpreset( );

      /*
       * Initialize C Set/2 run-time environment, terminate if unsuccessful
       */
      if ( _CRT_init( ) == -1 )
      {
         return 0L;
      }

#ifdef INTEL
      /*
       * Initialize C Set++ run-time environment
       */
      __ctordtorInit( );
#endif
#endif

#ifndef PRIVATE
      /*
       * Log error and terminate if T_DOS___ version does not match T_COMMON
       * version
       */
      if ( ulOS2TRACEVer != T_OS2TRACEVer )
      {
         T_Error( TRUE, EM_VERMATCH, szTraceDLL,
                  HIWORD( ulOS2TRACEVer ), HIBYTE( LOWORD( ulOS2TRACEVer ) ), LOBYTE( LOWORD( ulOS2TRACEVer ) ),
                  HIWORD( T_OS2TRACEVer ), HIBYTE( LOWORD( T_OS2TRACEVer ) ), LOBYTE( LOWORD( T_OS2TRACEVer ) )
                );
      }
#endif

      /*
       * Load DOSCALLS library, log error and terminate if unsuccessful
       */
      if ( DOS32LOADMODULE( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load DOSCALL1 library, log error and terminate if unsuccessful
       */
      if ( DOS32LOADMODULE( szObjName, sizeof( szObjName ), szDOSCALL1, &hmodDOSCALL1 ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_DOS___ library, log error and terminate if unsuccessful
       */
      if ( ( DOS32LOADMODULE( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                     ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register DOSCALLS trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

      /*
       * Register DOSCALL1 trace DLL
       */
      T_RegisterDLL( hmodDOSCALL1, hmodTraceDLL );

#ifdef INTEL
      /*
       * Obtain OS/2 SMP procedure addresses if tracing SMP API group
       */
      if ( LOGDOSAPI( D_SMP ) )
      {
         /*
          * Obtain OS/2 SMP procedure addresses, indicate API not present if
          * unsuccessful
          */
#ifdef T_16BIT
         if ( DOSGETPROCADDR( hmodDLL, (PSZ16)ORD_DOS16ACQUIRESPINLOCK, (PPFN16)&pfnDOSACQUIRESPINLOCK           ) ) pfnDOSACQUIRESPINLOCK      = NULL;
         if ( DOSGETPROCADDR( hmodDLL, (PSZ16)ORD_DOS16CREATESPINLOCK , (PPFN16)&pfnDOSCREATESPINLOCK            ) ) pfnDOSCREATESPINLOCK       = NULL;
         if ( DOSGETPROCADDR( hmodDLL, (PSZ16)ORD_DOS16FREESPINLOCK   , (PPFN16)&pfnDOSFREESPINLOCK              ) ) pfnDOSFREESPINLOCK         = NULL;
         if ( DOSGETPROCADDR( hmodDLL, (PSZ16)ORD_DOS16RELEASESPINLOCK, (PPFN16)&pfnDOSRELEASESPINLOCK           ) ) pfnDOSRELEASESPINLOCK      = NULL;
#endif
         if ( DOS32QUERYPROCADDR( hmodDLL, ORD_DOS32ACQUIRESPINLOCK   , NULL, (PFN *)&pfnDOS32ACQUIRESPINLOCK    ) ) pfnDOS32ACQUIRESPINLOCK    = NULL;
         if ( DOS32QUERYPROCADDR( hmodDLL, ORD_DOS32CREATESPINLOCK    , NULL, (PFN *)&pfnDOS32CREATESPINLOCK     ) ) pfnDOS32CREATESPINLOCK     = NULL;
         if ( DOS32QUERYPROCADDR( hmodDLL, ORD_DOS32FREESPINLOCK      , NULL, (PFN *)&pfnDOS32FREESPINLOCK       ) ) pfnDOS32FREESPINLOCK       = NULL;
         if ( DOS32QUERYPROCADDR( hmodDLL, ORD_DOS32GETPROCESSORSTATUS, NULL, (PFN *)&pfnDOS32GETPROCESSORSTATUS ) ) pfnDOS32GETPROCESSORSTATUS = NULL;
         if ( DOS32QUERYPROCADDR( hmodDLL, ORD_DOS32RELEASESPINLOCK   , NULL, (PFN *)&pfnDOS32RELEASESPINLOCK    ) ) pfnDOS32RELEASESPINLOCK    = NULL;
         if ( DOS32QUERYPROCADDR( hmodDLL, ORD_DOS32SETPROCESSORSTATUS, NULL, (PFN *)&pfnDOS32SETPROCESSORSTATUS ) ) pfnDOS32SETPROCESSORSTATUS = NULL;
         if ( DOS32QUERYPROCADDR( hmodDLL, ORD_DOS32TESTPSD           , NULL, (PFN *)&pfnDOS32TESTPSD            ) ) pfnDOS32TESTPSD            = NULL;
      }
#endif

#if ( OS2VER >= 210 )
      /*
       * Obtain OS/2 2.10 procedure addresses if OS/2 version number greater
       * than or equal to 2.10
       */
      if ( T_OS2Ver >= MAKEVER( 2, 10 ) )
      {
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
         /*
          * Obtain 16-bit OS/2 2.10 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16QUERYMODFROMCS ), (PPFN16)&pfnDOSQUERYMODFROMCS ) )
         {
            T_Error( TRUE, EM_GETPROC, 16, 2, 10, szDLL, ulOrdinal );
         }
#endif
         /*
          * Obtain 32-bit OS/2 2.10 procedure addresses, log error and
          * terminate if unsuccessful
          */
#ifdef INTEL
         if ( ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32DUMPPROCESS         , NULL, (PFN *)&pfnDOS32DUMPPROCESS          ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTCLOSE        , NULL, (PFN *)&pfnDOS32PROTECTCLOSE         ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTENUMATTRIBUTE, NULL, (PFN *)&pfnDOS32PROTECTENUMATTRIBUTE ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTOPEN         , NULL, (PFN *)&pfnDOS32PROTECTOPEN          ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTQUERYFHSTATE , NULL, (PFN *)&pfnDOS32PROTECTQUERYFHSTATE  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTQUERYFILEINFO, NULL, (PFN *)&pfnDOS32PROTECTQUERYFILEINFO ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTREAD         , NULL, (PFN *)&pfnDOS32PROTECTREAD          ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFHSTATE   , NULL, (PFN *)&pfnDOS32PROTECTSETFHSTATE    ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILEINFO  , NULL, (PFN *)&pfnDOS32PROTECTSETFILEINFO   ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILELOCKS , NULL, (PFN *)&pfnDOS32PROTECTSETFILELOCKS  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILEPTR   , NULL, (PFN *)&pfnDOS32PROTECTSETFILEPTR    ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILESIZE  , NULL, (PFN *)&pfnDOS32PROTECTSETFILESIZE   ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTWRITE        , NULL, (PFN *)&pfnDOS32PROTECTWRITE         ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYABIOSSUPPORT   , NULL, (PFN *)&pfnDOS32QUERYABIOSSUPPORT    ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYMODFROMEIP     , NULL, (PFN *)&pfnDOS32QUERYMODFROMEIP      ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32SUPPRESSPOPUPS      , NULL, (PFN *)&pfnDOS32SUPPRESSPOPUPS       ) ) )
#else
         if ( ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTCLOSE        , NULL, &pfnDOS32PROTECTCLOSE         ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTENUMATTRIBUTE, NULL, &pfnDOS32PROTECTENUMATTRIBUTE ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTOPEN         , NULL, &pfnDOS32PROTECTOPEN          ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTQUERYFHSTATE , NULL, &pfnDOS32PROTECTQUERYFHSTATE  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTQUERYFILEINFO, NULL, &pfnDOS32PROTECTQUERYFILEINFO ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTREAD         , NULL, &pfnDOS32PROTECTREAD          ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFHSTATE   , NULL, &pfnDOS32PROTECTSETFHSTATE    ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILEINFO  , NULL, &pfnDOS32PROTECTSETFILEINFO   ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILELOCKS , NULL, &pfnDOS32PROTECTSETFILELOCKS  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILEPTR   , NULL, &pfnDOS32PROTECTSETFILEPTR    ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILESIZE  , NULL, &pfnDOS32PROTECTSETFILESIZE   ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTWRITE        , NULL, &pfnDOS32PROTECTWRITE         ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYMODFROMEIP     , NULL, &pfnDOS32QUERYMODFROMEIP      ) ) )
#endif
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 10, szDLL, ulOrdinal );
         }
      }
#endif

#if ( OS2VER >= 230 )
      /*
       * Obtain OS/2 2.30 procedure addresses if OS/2 version number greater
       * than or equal to 2.30
       */
      if ( T_OS2Ver >= MAKEVER( 2, 30 ) )
      {
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
         /*
          * Obtain 16-bit OS/2 2.30 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTCHGFILEPTR    ), (PPFN16)&pfnDOSPROTECTCHGFILEPTR    ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTCLOSE         ), (PPFN16)&pfnDOSPROTECTCLOSE         ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTENUMATTRIBUTE ), (PPFN16)&pfnDOSPROTECTENUMATTRIBUTE ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTFILEIO        ), (PPFN16)&pfnDOSPROTECTFILEIO        ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTFILELOCKS     ), (PPFN16)&pfnDOSPROTECTFILELOCKS     ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTNEWSIZE       ), (PPFN16)&pfnDOSPROTECTNEWSIZE       ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTOPEN          ), (PPFN16)&pfnDOSPROTECTOPEN          ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTQFHANDSTATE   ), (PPFN16)&pfnDOSPROTECTQFHANDSTATE   ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTQFILEINFO     ), (PPFN16)&pfnDOSPROTECTQFILEINFO     ) ) ||
#if FALSE
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTREAD          ), (PPFN16)&pfnDOSPROTECTREAD          ) ) ||
#endif
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTSETFHANDSTATE ), (PPFN16)&pfnDOSPROTECTSETFHANDSTATE ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTSETFILEINFO   ), (PPFN16)&pfnDOSPROTECTSETFILEINFO   ) ) ||
#if FALSE
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16PROTECTWRITE         ), (PPFN16)&pfnDOSPROTECTWRITE         ) ) ||
#endif
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16QUERYEXTLIBPATH      ), (PPFN16)&pfnDOSQUERYEXTLIBPATH      ) ) ||
              ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16SETEXTLIBPATH        ), (PPFN16)&pfnDOSSETEXTLIBPATH        ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 16, 2, 30, szDLL, ulOrdinal );
         }
#endif
         /*
          * Obtain 32-bit OS/2 2.30 procedure addresses, log error and
          * terminate if unsuccessful
          */
#ifdef INTEL
         if ( ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32ALLOCTHREADLOCALMEMORY, NULL, (PFN *)&pfnDOS32ALLOCTHREADLOCALMEMORY ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32FREETHREADLOCALMEMORY , NULL, (PFN *)&pfnDOS32FREETHREADLOCALMEMORY  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PMPOSTEVENTSEM        , NULL, (PFN *)&pfnDOS32PMPOSTEVENTSEM         ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PMREQUESTMUTEXSEM     , NULL, (PFN *)&pfnDOS32PMREQUESTMUTEXSEM      ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PMWAITEVENTSEM        , NULL, (PFN *)&pfnDOS32PMWAITEVENTSEM         ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PMWAITMUXWAITSEM      , NULL, (PFN *)&pfnDOS32PMWAITMUXWAITSEM       ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYEXTLIBPATH       , NULL, (PFN *)&pfnDOS32QUERYEXTLIBPATH        ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32SETEXTLIBPATH         , NULL, (PFN *)&pfnDOS32SETEXTLIBPATH          ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32VERIFYPIDTID          , NULL, (PFN *)&pfnDOS32VERIFYPIDTID           ) ) )
#else
         if ( ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYEXTLIBPATH       , NULL, &pfnDOS32QUERYEXTLIBPATH        ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32SETEXTLIBPATH         , NULL, &pfnDOS32SETEXTLIBPATH          ) ) )
#endif
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 30, szDLL, ulOrdinal );
         }
      }
#endif

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
      /*
       * Obtain OS/2 2.40 procedure addresses if OS/2 version number greater
       * than or equal to 2.40
       */
      if ( T_OS2Ver >= MAKEVER( 2, 40 ) )
      {
         /*
          * Obtain OS/2 2.40 procedure addresses, log error and terminate if
          * unsuccessful
          */
         if ( ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PERFSYSCALL       , NULL, (PFN *)&pfnDOS32PERFSYSCALL        ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYTHREADCONTEXT, NULL, (PFN *)&pfnDOS32QUERYTHREADCONTEXT ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 40, szDLL, ulOrdinal );
         }
      }
#endif

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
      /*
       * Obtain OS/2 2.45 procedure addresses if OS/2 version number greater
       * than or equal to 2.45
       */
      if ( T_OS2Ver >= MAKEVER( 2, 45 ) )
      {
#ifdef T_16BIT
         /*
          * Obtain 16-bit OS/2 2.45 procedure addresses, indicate API not
          * present if unsuccessful
          */
         if ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16FSRAMSEMCLEAR2 ), (PPFN16)&pfnDOSFSRAMSEMCLEAR2 ) ) pfnDOSFSRAMSEMCLEAR2 = NULL;
         if ( DOSGETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DOS16FSRAMSEMREQUEST2 ), (PPFN16)&pfnDOSFSRAMSEMREQUEST2 ) ) pfnDOSFSRAMSEMREQUEST2 = NULL;
#endif

         /*
          * Obtain 32-bit OS/2 2.45 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32CANCELLOCKREQUESTL  , NULL, (PFN *)&pfnDOS32CANCELLOCKREQUESTL   ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32CREATETHREAD2       , NULL, (PFN *)&pfnDOS32CREATETHREAD2        ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32LISTIO              , NULL, (PFN *)&pfnDOS32LISTIO               ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32LISTIOL             , NULL, (PFN *)&pfnDOS32LISTIOL              ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32OPENL               , NULL, (PFN *)&pfnDOS32OPENL                ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PMR3POSTEVENTSEM    , NULL, (PFN *)&pfnDOS32PMR3POSTEVENTSEM     ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PMR3WAITEVENTSEM    , NULL, (PFN *)&pfnDOS32PMR3WAITEVENTSEM     ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTOPENL        , NULL, (PFN *)&pfnDOS32PROTECTOPENL         ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILELOCKSL, NULL, (PFN *)&pfnDOS32PROTECTSETFILELOCKSL ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILEPTRL  , NULL, (PFN *)&pfnDOS32PROTECTSETFILEPTRL   ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32PROTECTSETFILESIZEL , NULL, (PFN *)&pfnDOS32PROTECTSETFILESIZEL  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYTHREADAFFINITY , NULL, (PFN *)&pfnDOS32QUERYTHREADAFFINITY  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32SETFILELOCKSL       , NULL, (PFN *)&pfnDOS32SETFILELOCKSL        ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32SETFILEPTRL         , NULL, (PFN *)&pfnDOS32SETFILEPTRL          ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32SETFILESIZEL        , NULL, (PFN *)&pfnDOS32SETFILESIZEL         ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32SETTHREADAFFINITY   , NULL, (PFN *)&pfnDOS32SETTHREADAFFINITY    ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32SYSTEMSERVICE       , NULL, (PFN *)&pfnDOS32SYSTEMSERVICE        ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 45, szDLL, ulOrdinal );
         }
      }
#endif

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
      /*
       * Obtain OS/2 3.00 procedure addresses if OS/2 version number greater
       * than or equal to 3.00
       */
      if ( T_OS2Ver >= MAKEVER( 3, 0 ) )
      {
         /*
          * Obtain OS/2 3.00 procedure addresses, log error and terminate if
          * unsuccessful
          */
         if ( ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32CLOSERAMSEM      , NULL, &pfnDOS32CLOSERAMSEM      ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32CREATERAMSEM     , NULL, &pfnDOS32CREATERAMSEM     ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32FINDFROMNAME     , NULL, &pfnDOS32FINDFROMNAME     ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32GETLOCALEMODULE  , NULL, &pfnDOS32GETLOCALEMODULE  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32OPENRAMSEM       , NULL, &pfnDOS32OPENRAMSEM       ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYPROCESSINFO , NULL, &pfnDOS32QUERYPROCESSINFO ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32QUERYTHREADINFO  , NULL, &pfnDOS32QUERYTHREADINFO  ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32RELEASERAMSEM    , NULL, &pfnDOS32RELEASERAMSEM    ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_DOS32REQUESTRAMSEM    , NULL, &pfnDOS32REQUESTRAMSEM    ) ) ||
              ( DOS32QUERYPROCADDR( hmodDLL, ulOrdinal = ORD_TEMP32WRITE           , NULL, &pfnTEMP32WRITE           ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 3, 0, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_DOS___
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload DOSCALL1 library if library loaded
       */
      if ( hmodDOSCALL1 )
      {
         DOS32FREEMODULE( hmodDOSCALL1 );
      }

      /*
       * Unload DOSCALLS library if library loaded
       */
      if ( hmodDLL )
      {
         DOS32FREEMODULE( hmodDLL );
      }

#ifndef T_COMRUNTIME
#ifndef INTEL
      /*
       * Terminate C Set++ run-time environment
       */
      __ctordtorTerm( );
#endif

      /*
       * Terminate C Set/2 run-time environment
       */
      _CRT_term( );
#endif
   }

   /*
    * Indicate success
    */
   return 1L;

}  /* _DLL_InitTerm() */
#ifdef POWERPC
#pragma code( );
#endif

/******************************************************************************/
/*                                                                            */
/* AddDFFLevel()                                                              */
/*                                                                            */
/******************************************************************************/

VOID AddDFFLevel( HDIR  hDir                     /* Directory handle          */
                , ULONG ulLevel                  /* Information level         */
                )
{

   int i;                                        /* Loop variable             */

   /*
    * Search through outstanding DosFindFirst calls for matching directory
    * handle
    */
   for ( i = 0; i < ulDFFInfo; i++ )
   {
      /*
       * Matching directory handle found, update information level and return
       */
      if ( aDFFInfo[ i ].hDir == hDir )
      {
         aDFFInfo[ i ].ulLevel = ulLevel;
         return;
      }
   }

   /*
    * Issue error and terminate if too many outstanding DosFindFirst calls
    */
   if ( ulDFFInfo >= MAX_DFFINFO )
   {
      T_Error( TRUE, EM_DFFLIMIT, MAX_DFFINFO );
   }

   /*
    * Initialize next available outstanding DosFindFirst call entry
    */
   aDFFInfo[ ulDFFInfo ].hDir = hDir;
   aDFFInfo[ ulDFFInfo ].ulLevel = ulLevel;

   /*
    * Increment number of outstanding DosFindFirst calls
    */
   ulDFFInfo++;

}  /* AddDFFLevel() */

/******************************************************************************/
/*                                                                            */
/* C_PostDos16CreateThread()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PCHAR szDos16CreateThread = "Dos16CreateThread";

VOID APIENTRY16 C_PostDos16CreateThread( APIRET16  *pusRC
                                       , PTID16     Parm2
                                       , PUSHORT16  Parms
                                       )
{

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16CreateThread );
      T_LogCPReturnCode( *pusRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PTID );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16CreateThread, Parms, pusRC );
      T_FreeLog( );
   }

}  /* C_PostDos16CreateThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* C_PreDos16CreateThread()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
VOID APIENTRY16 C_PreDos16CreateThread( PVOID16     APICaller
                                      , PFNTHREAD16 Parm1
                                      , PTID16      Parm2
                                      , PBYTE16     Parm3
                                      , PUSHORT16   Parms
                                      )
{

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntryWithCaller16( szDos16CreateThread, APICaller );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PFNTHREAD );
         T_LogPFN16( (PFN16)Parm1 );
         T_LogParameter( 2L, DT_PTID );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PBYTE );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16CreateThread, Parms );
      T_FreeLog( );
   }

}  /* C_PreDos16CreateThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* DelDFFLevel()                                                              */
/*                                                                            */
/******************************************************************************/

VOID DelDFFLevel( HDIR hDir )                    /* Directory handle          */
{

   int i;                                        /* Loop variable             */

   /*
    * Search through outstanding DosFindFirst calls for matching directory
    * handle
    */
   for ( i = 0; i < ulDFFInfo; i++ )
   {
      /*
       * Matching directory handle found, delete outstanding DosFindFirst call
       * entry
       */
      if ( aDFFInfo[ i ].hDir == hDir )
      {
         /*
          * Slide remaining outstanding DosFindFirst call entries down
          */
         for ( ; i < ulDFFInfo; i++ )
         {
            aDFFInfo[ i ].hDir = aDFFInfo[ i+1 ].hDir;
            aDFFInfo[ i ].ulLevel = aDFFInfo[ i+1 ].ulLevel;
         }

         /*
          * Decrement number of outstanding DosFindFirst calls
          */
         ulDFFInfo--;

         break;
      }
   }

}  /* DelDFFLevel() */

/******************************************************************************/
/*                                                                            */
/* GetDFFLevel()                                                              */
/*                                                                            */
/******************************************************************************/

ULONG GetDFFLevel( HDIR hDir )                   /* Directory handle          */
{

   int i;                                        /* Loop variable             */

   /*
    * Search through outstanding DosFindFirst calls for matching directory
    * handle
    */
   for ( i = 0; i < ulDFFInfo; i++ )
   {
      /*
       * Matching directory handle found, return information level
       */
      if ( aDFFInfo[ i ].hDir == hDir )
      {
         return aDFFInfo[ i ].ulLevel;
      }
   }

   /*
    * Return zero if matching directory handle not found
    */
   return 0L;

}  /* GetDFFLevel() */

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AcknowledgeSignalException()                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ACKNOWLEDGESIGNALEXCEPTION( ULONG );
PCHAR szDos32AcknowledgeSignalException = "Dos32AcknowledgeSignalException";

APIRET APIENTRY Trc_Dos32AcknowledgeSignalException( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AcknowledgeSignalException );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32AcknowledgeSignalException, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ACKNOWLEDGESIGNALEXCEPTION( Parm1 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AcknowledgeSignalException );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32AcknowledgeSignalException, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AcknowledgeSignalException() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16AcquireSpinLock()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSACQUIRESPINLOCK( HSPINLOCK );
PCHAR szDos16AcquireSpinLock = "Dos16AcquireSpinLock";

APIRET16 APIENTRY16 Trc_Dos16AcquireSpinLock( HSPINLOCK Parm1 )
{

   APIRET16 usRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos16AcquireSpinLock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSPINLOCK );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szDos16AcquireSpinLock, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOSACQUIRESPINLOCK )
   {
      usRC = ( *pfnDOSACQUIRESPINLOCK )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos16AcquireSpinLock );
         T_FreeLog( );
      }

      usRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16AcquireSpinLock );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16AcquireSpinLock, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Dos16AcquireSpinLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AcquireSpinLock()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32ACQUIRESPINLOCK( HSPINLOCK );
PCHAR szDos32AcquireSpinLock = "Dos32AcquireSpinLock";

APIRET APIENTRY Trc_Dos32AcquireSpinLock( HSPINLOCK Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AcquireSpinLock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSPINLOCK );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32AcquireSpinLock, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOS32ACQUIRESPINLOCK )
   {
      ulRC = ( *pfnDOS32ACQUIRESPINLOCK )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos32AcquireSpinLock );
         T_FreeLog( );
      }

      ulRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AcquireSpinLock );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32AcquireSpinLock, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AcquireSpinLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AddMuxWaitSem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ADDMUXWAITSEM( HMUX, PSEMRECORD );
PCHAR szDos32AddMuxWaitSem = "Dos32AddMuxWaitSem";

APIRET APIENTRY Trc_Dos32AddMuxWaitSem( HMUX       Parm1
                                      , PSEMRECORD Parm2
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AddMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMUX );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSEMRECORD );
         T_LogPSEMRECORD( Parm2 );
      }
      HOOKENTRY( szDos32AddMuxWaitSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ADDMUXWAITSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AddMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32AddMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AddMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AliasMem()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32ALIASMEM( PVOID, ULONG, PPVOID, ULONG );
PCHAR szDos32AliasMem = "Dos32AliasMem";

APIRET APIENTRY Trc_Dos32AliasMem( PVOID  Parm1
                                 , ULONG  Parm2
                                 , PPVOID Parm3
                                 , ULONG  Parm4
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AliasMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPVOID );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32AliasMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ALIASMEM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AliasMem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PPVOID );
         T_LogPULONG( (PULONG)Parm3 );
      }
      HOOKEXIT( szDos32AliasMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AliasMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AliasPerfCtrs()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32ALIASPERFCTRS( ULONG, ULONG, PBYTE *, ULONG * );
PCHAR szDos32AliasPerfCtrs = "Dos32AliasPerfCtrs";

APIRET APIENTRY Trc_Dos32AliasPerfCtrs( ULONG  Parm1
                                      , ULONG  Parm2
                                      , PBYTE *Parm3
                                      , ULONG *Parm4
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AliasPerfCtrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PBYTESTAR );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONGSTAR );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32AliasPerfCtrs, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ALIASPERFCTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AliasPerfCtrs );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PBYTESTAR );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_ULONGSTAR );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32AliasPerfCtrs, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AliasPerfCtrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16AllocHuge()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSALLOCHUGE( USHORT, USHORT, PSEL16, USHORT, USHORT );
#define DOSALLOCHUGE_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PSEL16 ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16AllocHuge = "Dos16AllocHuge";

APIRET16 APIENTRY16 Trc_Dos16AllocHuge( USHORT Parm1
                                      , USHORT Parm2
                                      , PSEL16 Parm3
                                      , USHORT Parm4
                                      , USHORT Parm5
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSALLOCHUGE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16AllocHuge, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PSEL );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szDos16AllocHuge, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSALLOCHUGE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16AllocHuge );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSEL );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16AllocHuge, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16AllocHuge() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AllocMem()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ALLOCMEM( PPVOID, ULONG, ULONG );
PCHAR szDos32AllocMem = "Dos32AllocMem";

APIRET APIENTRY Trc_Dos32AllocMem( PPVOID Parm1
                                 , ULONG  Parm2
                                 , ULONG  Parm3
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AllocMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PPVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32AllocMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ALLOCMEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AllocMem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PPVOID );
         T_LogPULONG( (PULONG)Parm1 );
      }
      HOOKEXIT( szDos32AllocMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AllocMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16AllocSeg()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSALLOCSEG( USHORT, PSEL16, USHORT );
#define DOSALLOCSEG_PARMBYTES ( sizeof( USHORT ) + sizeof( PSEL16 ) + sizeof( USHORT ) )
PCHAR szDos16AllocSeg = "Dos16AllocSeg";

APIRET16 APIENTRY16 Trc_Dos16AllocSeg( USHORT Parm1
                                     , PSEL16 Parm2
                                     , USHORT Parm3
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSALLOCSEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16AllocSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PSEL );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16AllocSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSALLOCSEG( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16AllocSeg );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSEL );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16AllocSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16AllocSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AllocSharedMem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ALLOCSHAREDMEM( PPVOID, PSZ, ULONG, ULONG );
PCHAR szDos32AllocSharedMem = "Dos32AllocSharedMem";

APIRET APIENTRY Trc_Dos32AllocSharedMem( PPVOID Parm1
                                       , PSZ    Parm2
                                       , ULONG  Parm3
                                       , ULONG  Parm4
                                       )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AllocSharedMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PPVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32AllocSharedMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ALLOCSHAREDMEM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AllocSharedMem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PPVOID );
         T_LogPULONG( (PULONG)Parm1 );
      }
      HOOKEXIT( szDos32AllocSharedMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AllocSharedMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16AllocShrSeg()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSALLOCSHRSEG( USHORT, PSZ16, PSEL16 );
#define DOSALLOCSHRSEG_PARMBYTES ( sizeof( USHORT ) + sizeof( PSZ16 ) + sizeof( PSEL16 ) )
PCHAR szDos16AllocShrSeg = "Dos16AllocShrSeg";

APIRET16 APIENTRY16 Trc_Dos16AllocShrSeg( USHORT Parm1
                                        , PSZ16  Parm2
                                        , PSEL16 Parm3
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSALLOCSHRSEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16AllocShrSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PSEL );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16AllocShrSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSALLOCSHRSEG( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16AllocShrSeg );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSEL );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16AllocShrSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16AllocShrSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AllocThreadLocalMemory()                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
APIRET APIENTRY DOS32ALLOCTHREADLOCALMEMORY( ULONG, PULONG * );
PCHAR szDos32AllocThreadLocalMemory = "Dos32AllocThreadLocalMemory";

APIRET APIENTRY Trc_Dos32AllocThreadLocalMemory( ULONG   Parm1
                                               , PULONG *Parm2
                                               )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AllocThreadLocalMemory );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONGSTAR );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32AllocThreadLocalMemory, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32ALLOCTHREADLOCALMEMORY )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AllocThreadLocalMemory );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONGSTAR );
         T_LogPULONG( (PULONG)Parm2 );
      }
      HOOKEXIT( szDos32AllocThreadLocalMemory, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AllocThreadLocalMemory() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32AsyncTimer()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ASYNCTIMER( ULONG, HSEM, PHTIMER );
PCHAR szDos32AsyncTimer = "Dos32AsyncTimer";

APIRET APIENTRY Trc_Dos32AsyncTimer( ULONG   Parm1
                                   , HSEM    Parm2
                                   , PHTIMER Parm3
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32AsyncTimer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HSEM );
         T_LogULONG( (ULONG)Parm2 );
         T_LogParameter( 3L, DT_PHTIMER );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32AsyncTimer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ASYNCTIMER( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32AsyncTimer );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PHTIMER );
         T_LogPULONG( (PHTIMER)Parm3 );
      }
      HOOKEXIT( szDos32AsyncTimer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32AsyncTimer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Beep()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSBEEP( USHORT, USHORT );
#define DOSBEEP_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16Beep = "Dos16Beep";

APIRET16 APIENTRY16 Trc_Dos16Beep( USHORT Parm1
                                 , USHORT Parm2
                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSBEEP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Beep, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szDos16Beep, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSBEEP( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Beep );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16Beep, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Beep() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Beep()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32BEEP( ULONG, ULONG );
PCHAR szDos32Beep = "Dos32Beep";

APIRET APIENTRY Trc_Dos32Beep( ULONG Parm1
                             , ULONG Parm2
                             )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Beep );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32Beep, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32BEEP( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Beep );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32Beep, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Beep() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16BufReset()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSBUFRESET( HFILE16 );
#define DOSBUFRESET_PARMBYTES ( sizeof( HFILE16 ) )
PCHAR szDos16BufReset = "Dos16BufReset";

APIRET16 APIENTRY16 Trc_Dos16BufReset( HFILE16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSBUFRESET_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16BufReset, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16BufReset, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSBUFRESET( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16BufReset );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16BufReset, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16BufReset() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Callback()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
VOID APIENTRY16 DOSCALLBACK( PFN16 );
#define DOSCALLBACK_PARMBYTES ( sizeof( PFN16 ) )
PCHAR szDos16Callback = "Dos16Callback";

VOID APIENTRY16 Trc_Dos16Callback( PFN16 Parm1 )
{

   REGS16 sRegs16;                               /* 16-bit registers          */

   sRegs16.usParmBytes = DOSCALLBACK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Callback, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PFN );
         T_LogPFN16( Parm1 );
      }
      HOOKENTRY16( szDos16Callback, &Parm1 );
      T_FreeLog( );
   }

   DOSCALLBACK( Parm1 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Callback );
      HOOKEXIT16( szDos16Callback, &Parm1, NULL );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

}  /* Trc_Dos16Callback() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CallNPipe()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CALLNPIPE( PSZ, PVOID, ULONG, PVOID, ULONG, PULONG, ULONG );
PCHAR szDos32CallNPipe = "Dos32CallNPipe";

APIRET APIENTRY Trc_Dos32CallNPipe( PSZ    Parm1
                                  , PVOID  Parm2
                                  , ULONG  Parm3
                                  , PVOID  Parm4
                                  , ULONG  Parm5
                                  , PULONG Parm6
                                  , ULONG  Parm7
                                  )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CallNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogCHARBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szDos32CallNPipe, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32CALLNPIPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32CallNPipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( ( Parm6 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer( Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      HOOKEXIT( szDos32CallNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CallNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CancelLockRequest()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CANCELLOCKREQUEST( HFILE, PFILELOCK );
PCHAR szDos32CancelLockRequest = "Dos32CancelLockRequest";

APIRET APIENTRY Trc_Dos32CancelLockRequest( HFILE     Parm1
                                          , PFILELOCK Parm2
                                          )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CancelLockRequest );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFILELOCK );
         T_LogPFILELOCK( Parm2 );
      }
      HOOKENTRY( szDos32CancelLockRequest, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CANCELLOCKREQUEST( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CancelLockRequest );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32CancelLockRequest, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CancelLockRequest() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CancelLockRequestL()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32CANCELLOCKREQUESTL( HFILE, PFILELOCKL );
PCHAR szDos32CancelLockRequestL = "Dos32CancelLockRequestL";

APIRET APIENTRY Trc_Dos32CancelLockRequestL( HFILE      Parm1
                                           , PFILELOCKL Parm2
                                           )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CancelLockRequestL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFILELOCKL );
         T_LogPFILELOCKL( Parm2 );
      }
      HOOKENTRY( szDos32CancelLockRequestL, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32CANCELLOCKREQUESTL )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CancelLockRequestL );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32CancelLockRequestL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CancelLockRequestL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ChDir()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCHDIR( PSZ16, ULONG );
#define DOSCHDIR_PARMBYTES ( sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szDos16ChDir = "Dos16ChDir";

APIRET16 APIENTRY16 Trc_Dos16ChDir( PSZ16 Parm1
                                  , ULONG Parm2
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCHDIR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ChDir, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16ChDir, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCHDIR( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ChDir );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ChDir, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ChDir() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ChgFilePtr()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCHGFILEPTR( HFILE16, LONG, USHORT, PULONG16 );
#define DOSCHGFILEPTR_PARMBYTES ( sizeof( HFILE16 ) + sizeof( LONG ) + sizeof( USHORT ) + sizeof( PULONG16 ) )
PCHAR szDos16ChgFilePtr = "Dos16ChgFilePtr";

APIRET16 APIENTRY16 Trc_Dos16ChgFilePtr( HFILE16  Parm1
                                       , LONG     Parm2
                                       , USHORT   Parm3
                                       , PULONG16 Parm4
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCHGFILEPTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ChgFilePtr, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16ChgFilePtr, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCHGFILEPTR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ChgFilePtr );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG16( Parm4 );
      }
      HOOKEXIT16( szDos16ChgFilePtr, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ChgFilePtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16CLIAccess()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCLIACCESS( VOID );
#define DOSCLIACCESS_PARMBYTES ( 0 )
PCHAR szDos16CLIAccess = "Dos16CLIAccess";

APIRET16 APIENTRY16 Trc_Dos16CLIAccess( VOID )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCLIACCESS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16CLIAccess, sRegs16.ulCSIP );
      HOOKENTRY16( szDos16CLIAccess, NULL );
      T_FreeLog( );
   }

   usRC = DOSCLIACCESS( );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16CLIAccess );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16CLIAccess, NULL, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CLIAccess() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Close()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCLOSE( HFILE16 );
#define DOSCLOSE_PARMBYTES ( sizeof( HFILE16 ) )
PCHAR szDos16Close = "Dos16Close";

APIRET16 APIENTRY16 Trc_Dos16Close( HFILE16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCLOSE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Close, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16Close, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCLOSE( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Close );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16Close, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Close() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Close()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CLOSE( HFILE );
PCHAR szDos32Close = "Dos32Close";

APIRET APIENTRY Trc_Dos32Close( HFILE Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Close );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32Close, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CLOSE( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Close );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32Close, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Close() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CloseEventSem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CLOSEEVENTSEM( HEV );
PCHAR szDos32CloseEventSem = "Dos32CloseEventSem";

APIRET APIENTRY Trc_Dos32CloseEventSem( HEV Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CloseEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32CloseEventSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CLOSEEVENTSEM( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CloseEventSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32CloseEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CloseEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CloseMutexSem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CLOSEMUTEXSEM( HMTX );
PCHAR szDos32CloseMutexSem = "Dos32CloseMutexSem";

APIRET APIENTRY Trc_Dos32CloseMutexSem( HMTX Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CloseMutexSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMTX );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32CloseMutexSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CLOSEMUTEXSEM( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CloseMutexSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32CloseMutexSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CloseMutexSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CloseMuxWaitSem()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CLOSEMUXWAITSEM( HMUX );
PCHAR szDos32CloseMuxWaitSem = "Dos32CloseMuxWaitSem";

APIRET APIENTRY Trc_Dos32CloseMuxWaitSem( HMUX Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CloseMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMUX );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32CloseMuxWaitSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CLOSEMUXWAITSEM( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CloseMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32CloseMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CloseMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CloseRamSem()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET EXPENTRY DOS32CLOSERAMSEM( PRAMSEM );
PCHAR szDos32CloseRamSem = "Dos32CloseRamSem";

APIRET EXPENTRY Trc_Dos32CloseRamSem( PRAMSEM Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CloseRamSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         T_LogPRAMSEM( Parm1 );
      }
      HOOKENTRY( szDos32CloseRamSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32CLOSERAMSEM )( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CloseRamSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         if ( !ulRC ) T_LogPRAMSEM( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32CloseRamSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CloseRamSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16CloseSem()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCLOSESEM( HSEM );
#define DOSCLOSESEM_PARMBYTES ( sizeof( HSEM ) )
PCHAR szDos16CloseSem = "Dos16CloseSem";

APIRET16 APIENTRY16 Trc_Dos16CloseSem( HSEM Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCLOSESEM_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16CloseSem, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
      }
      HOOKENTRY16( szDos16CloseSem, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCLOSESEM( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16CloseSem );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16CloseSem, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CloseSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16CloseVDD()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCLOSEVDD( HVDD );
#define DOSCLOSEVDD_PARMBYTES ( sizeof( HVDD ) )
PCHAR szDos16CloseVDD = "Dos16CloseVDD";

APIRET16 APIENTRY16 Trc_Dos16CloseVDD( HVDD Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCLOSEVDD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16CloseVDD, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HVDD );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szDos16CloseVDD, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCLOSEVDD( Parm1 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16CloseVDD );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16CloseVDD, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CloseVDD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CloseVDD()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CLOSEVDD( HVDD );
PCHAR szDos32CloseVDD = "Dos32CloseVDD";

APIRET APIENTRY Trc_Dos32CloseVDD( HVDD Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CloseVDD );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HVDD );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32CloseVDD, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CLOSEVDD( Parm1 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CloseVDD );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32CloseVDD, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CloseVDD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ConfigurePerf()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32CONFIGUREPERF( ULONG, ULONG, ULONG, ULONG, PSZ, BOOL32 );
PCHAR szDos32ConfigurePerf = "Dos32ConfigurePerf";

APIRET APIENTRY Trc_Dos32ConfigurePerf( ULONG  Parm1
                                      , ULONG  Parm2
                                      , ULONG  Parm3
                                      , ULONG  Parm4
                                      , PSZ    Parm5
                                      , BOOL32 Parm6
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ConfigurePerf );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZ( Parm5 );
         T_LogParameter( 6L, DT_BOOL32 );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szDos32ConfigurePerf, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CONFIGUREPERF( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ConfigurePerf );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ConfigurePerf, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ConfigurePerf() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ConnectNPipe()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CONNECTNPIPE( HPIPE );
PCHAR szDos32ConnectNPipe = "Dos32ConnectNPipe";

APIRET APIENTRY Trc_Dos32ConnectNPipe( HPIPE Parm1 )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ConnectNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPIPE );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32ConnectNPipe, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32CONNECTNPIPE( Parm1 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32ConnectNPipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32ConnectNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ConnectNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Copy()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCOPY( PSZ16, PSZ16, USHORT, ULONG );
#define DOSCOPY_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16Copy = "Dos16Copy";

APIRET16 APIENTRY16 Trc_Dos16Copy( PSZ16  Parm1
                                 , PSZ16  Parm2
                                 , USHORT Parm3
                                 , ULONG  Parm4
                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCOPY_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Copy, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szDos16Copy, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCOPY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Copy );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16Copy, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos32Copy() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Copy()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32COPY( PSZ, PSZ, ULONG );
PCHAR szDos32Copy = "Dos32Copy";

APIRET APIENTRY Trc_Dos32Copy( PSZ   Parm1
                             , PSZ   Parm2
                             , ULONG Parm3
                             )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Copy );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32Copy, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32COPY( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Copy );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32Copy, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Copy() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16CreateCSAlias()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCREATECSALIAS( SEL, PSEL16 );
#define DOSCREATECSALIAS_PARMBYTES ( sizeof( SEL ) + sizeof( PSEL16 ) )
PCHAR szDos16CreateCSAlias = "Dos16CreateCSAlias";

APIRET16 APIENTRY16 Trc_Dos16CreateCSAlias( SEL    Parm1
                                          , PSEL16 Parm2
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCREATECSALIAS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16CreateCSAlias, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PSEL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16CreateCSAlias, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCREATECSALIAS( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16CreateCSAlias );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSEL );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16CreateCSAlias, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CreateCSAlias() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateDir()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CREATEDIR( PSZ, PEAOP2 );
PCHAR szDos32CreateDir = "Dos32CreateDir";

APIRET APIENTRY Trc_Dos32CreateDir( PSZ    Parm1
                                  , PEAOP2 Parm2
                                  )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateDir );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PEAOP2 );
         T_LogPVOID( Parm2 );
         T_LogPEAOP2( Parm2, PEAOP_FEA );
      }
      HOOKENTRY( szDos32CreateDir, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CREATEDIR( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateDir );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PEAOP2 );
         T_LogPVOID( Parm2 );
         if ( ulRC ) T_LogPEAOP2( Parm2, PEAOP_ERR );
      }
      HOOKEXIT( szDos32CreateDir, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateDir() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateEventSem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CREATEEVENTSEM( PSZ, PHEV, ULONG, BOOL32 );
PCHAR szDos32CreateEventSem = "Dos32CreateEventSem";

APIRET APIENTRY Trc_Dos32CreateEventSem( PSZ    Parm1
                                       , PHEV   Parm2
                                       , ULONG  Parm3
                                       , BOOL32 Parm4
                                       )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHEV );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_BOOL32 );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32CreateEventSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CREATEEVENTSEM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHEV );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32CreateEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateMutexSem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CREATEMUTEXSEM( PSZ, PHMTX, ULONG, BOOL32 );
PCHAR szDos32CreateMutexSem = "Dos32CreateMutexSem";

APIRET APIENTRY Trc_Dos32CreateMutexSem( PSZ    Parm1
                                       , PHMTX  Parm2
                                       , ULONG  Parm3
                                       , BOOL32 Parm4
                                       )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateMutexSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHMTX );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_BOOL32 );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32CreateMutexSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CREATEMUTEXSEM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateMutexSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHMTX );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32CreateMutexSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateMutexSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateMuxWaitSem()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CREATEMUXWAITSEM( PSZ, PHMUX, ULONG, PSEMRECORD, ULONG );
PCHAR szDos32CreateMuxWaitSem = "Dos32CreateMuxWaitSem";

APIRET APIENTRY Trc_Dos32CreateMuxWaitSem( PSZ        Parm1
                                         , PHMUX      Parm2
                                         , ULONG      Parm3
                                         , PSEMRECORD Parm4
                                         , ULONG      Parm5
                                         )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHMUX );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSEMRECORD );
         T_LogSEMRECORDBuffer( Parm4, Parm3 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32CreateMuxWaitSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CREATEMUXWAITSEM( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHMUX );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32CreateMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateNPipe()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CREATENPIPE( PSZ, PHPIPE, ULONG, ULONG, ULONG, ULONG, ULONG );
PCHAR szDos32CreateNPipe = "Dos32CreateNPipe";

APIRET APIENTRY Trc_Dos32CreateNPipe( PSZ    Parm1
                                    , PHPIPE Parm2
                                    , ULONG  Parm3
                                    , ULONG  Parm4
                                    , ULONG  Parm5
                                    , ULONG  Parm6
                                    , ULONG  Parm7
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHPIPE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szDos32CreateNPipe, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CREATENPIPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateNPipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHPIPE );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32CreateNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreatePipe()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CREATEPIPE( PHFILE, PHFILE, ULONG );
PCHAR szDos32CreatePipe = "Dos32CreatePipe";

APIRET APIENTRY Trc_Dos32CreatePipe( PHFILE Parm1
                                   , PHFILE Parm2
                                   , ULONG  Parm3
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreatePipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHFILE );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32CreatePipe, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CREATEPIPE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreatePipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHFILE );
         T_LogPULONG( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32CreatePipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreatePipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateRamSem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET EXPENTRY DOS32CREATERAMSEM( PRAMSEM );
PCHAR szDos32CreateRamSem = "Dos32CreateRamSem";

APIRET EXPENTRY Trc_Dos32CreateRamSem( PRAMSEM Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateRamSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32CreateRamSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32CREATERAMSEM )( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateRamSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         if ( !ulRC ) T_LogPRAMSEM( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32CreateRamSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateRamSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16CreateSem()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCREATESEM( USHORT, PHSYSSEM16, PSZ16 );
#define DOSCREATESEM_PARMBYTES ( sizeof( USHORT ) + sizeof( PHSYSSEM16 ) + sizeof( PSZ16 ) )
PCHAR szDos16CreateSem = "Dos16CreateSem";

APIRET16 APIENTRY16 Trc_Dos16CreateSem( USHORT     Parm1
                                      , PHSYSSEM16 Parm2
                                      , PSZ16      Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCREATESEM_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16CreateSem, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PHSYSSEM );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
      }
      HOOKENTRY16( szDos16CreateSem, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCREATESEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16CreateSem );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHSYSSEM );
         T_LogPULONG16( Parm2 );
      }
      HOOKEXIT16( szDos16CreateSem, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CreateSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16CreateSpinLock()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCREATESPINLOCK( PHSPINLOCK16 );
PCHAR szDos16CreateSpinLock = "Dos16CreateSpinLock";

APIRET16 APIENTRY16 Trc_Dos16CreateSpinLock( PHSPINLOCK16 Parm1 )
{

   APIRET16 usRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos16CreateSpinLock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHSPINLOCK );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16CreateSpinLock, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOSCREATESPINLOCK )
   {
      usRC = ( *pfnDOSCREATESPINLOCK )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos16CreateSpinLock );
         T_FreeLog( );
      }

      usRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16CreateSpinLock );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHSPINLOCK );
         T_LogPULONG16( Parm1 );
      }
      HOOKEXIT16( szDos16CreateSpinLock, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Dos16CreateSpinLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateSpinLock()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32CREATESPINLOCK( PHSPINLOCK );
PCHAR szDos32CreateSpinLock = "Dos32CreateSpinLock";

APIRET APIENTRY Trc_Dos32CreateSpinLock( PHSPINLOCK Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateSpinLock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHSPINLOCK );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32CreateSpinLock, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOS32CREATESPINLOCK )
   {
      ulRC = ( *pfnDOS32CREATESPINLOCK )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos32CreateSpinLock );
         T_FreeLog( );
      }

      ulRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateSpinLock );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHSPINLOCK );
         T_LogPULONG( Parm1 );
      }
      HOOKEXIT( szDos32CreateSpinLock, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateSpinLock() */
#endif

#if 0 /* Moved to t_dos386.asm */
/******************************************************************************/
/*                                                                            */
/* Trc_Dos16CreateThread()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCREATETHREAD( PFNTHREAD16, PTID16, PBYTE16 );
#define DOSCREATETHREAD_PARMBYTES ( sizeof( PFNTHREAD16 ) + sizeof( PTID16 ) + sizeof( PBYTE16 ) )
PCHAR szDos16CreateThread = "Dos16CreateThread";

APIRET16 APIENTRY16 Trc_Dos16CreateThread( PFNTHREAD16 Parm1
                                         , PTID16      Parm2
                                         , PBYTE16     Parm3
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCREATETHREAD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16CreateThread, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PFNTHREAD );
         T_LogPFN16( (PFN16)Parm1 );
         T_LogParameter( 2L, DT_PTID );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PBYTE );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16CreateThread, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSCREATETHREAD( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16CreateThread );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PTID );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16CreateThread, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CreateThread() */
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateThread()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CREATETHREAD( PTID, PFNTHREAD, ULONG, ULONG, ULONG );
PCHAR szDos32CreateThread = "Dos32CreateThread";

APIRET APIENTRY Trc_Dos32CreateThread( PTID      Parm1
                                     , PFNTHREAD Parm2
                                     , ULONG     Parm3
                                     , ULONG     Parm4
                                     , ULONG     Parm5
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateThread );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PTID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PFNTHREAD );
         T_LogPFN( (PFN)Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32CreateThread, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32CREATETHREAD( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateThread );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PTID );
         T_LogPULONG( Parm1 );
      }
      HOOKEXIT( szDos32CreateThread, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateThread2()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32CREATETHREAD2( PTHREADCREATE );
PCHAR szDos32CreateThread2 = "Dos32CreateThread2";

APIRET APIENTRY Trc_Dos32CreateThread2( PTHREADCREATE Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32CreateThread2 );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PTHREADCREATE );
         T_LogPTHREADCREATE( Parm1 );
      }
      HOOKENTRY( szDos32CreateThread2, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32CREATETHREAD2 )( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32CreateThread2 );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PTHREADCREATE );
         if ( !ulRC ) T_LogPTHREADCREATE( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32CreateThread2, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32CreateThread2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Cwait()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCWAIT( USHORT, USHORT, PRESULTCODES16, PPID16, PID16 );
#define DOSCWAIT_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PRESULTCODES16 ) + sizeof( PPID16 ) + sizeof( PID16 ) )
PCHAR szDos16Cwait = "Dos16Cwait";

APIRET16 APIENTRY16 Trc_Dos16Cwait( USHORT         Parm1
                                  , USHORT         Parm2
                                  , PRESULTCODES16 Parm3
                                  , PPID16         Parm4
                                  , PID16          Parm5
                                  )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCWAIT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Cwait, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PRESULTCODES );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_PPID );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_PID );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szDos16Cwait, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSCWAIT( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16Cwait );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm2 == DCWW_WAIT ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 3L, DT_PRESULTCODES );
         if ( !usRC ) T_LogPRESULTCODES16( Parm3 );
         else T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_PPID );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16Cwait, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Cwait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Debug()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32DEBUG( PVOID );
PCHAR szDos32Debug = "Dos32Debug";

APIRET APIENTRY Trc_Dos32Debug( PVOID Parm1 )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Debug );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogpuDB_t( Parm1 );
      }
      HOOKENTRY( szDos32Debug, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32DEBUG( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32Debug );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         if ( !ulRC ) T_LogpuDB_t( Parm1 );
      }
      HOOKEXIT( szDos32Debug, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Debug() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DeconPerf()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32DECONPERF( VOID );
PCHAR szDos32DeconPerf = "Dos32DeconPerf";

APIRET APIENTRY Trc_Dos32DeconPerf( VOID )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DeconPerf );
      HOOKENTRY( szDos32DeconPerf, NULL );
      T_FreeLog( );
   }

   ulRC = DOS32DECONPERF( );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DeconPerf );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32DeconPerf, NULL, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DeconPerf() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Delete()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSDELETE( PSZ16, ULONG );
#define DOSDELETE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szDos16Delete = "Dos16Delete";

APIRET16 APIENTRY16 Trc_Dos16Delete( PSZ16 Parm1
                                   , ULONG Parm2
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSDELETE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Delete, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16Delete, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSDELETE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Delete );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16Delete, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Delete() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Delete()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32DELETE( PSZ );
PCHAR szDos32Delete = "Dos32Delete";

APIRET APIENTRY Trc_Dos32Delete( PSZ Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Delete );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
      }
      HOOKENTRY( szDos32Delete, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32DELETE( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Delete );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32Delete, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Delete() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DeleteDir()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32DELETEDIR( PSZ );
PCHAR szDos32DeleteDir = "Dos32DeleteDir";

APIRET APIENTRY Trc_Dos32DeleteDir( PSZ Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DeleteDir );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
      }
      HOOKENTRY( szDos32DeleteDir, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32DELETEDIR( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DeleteDir );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32DeleteDir, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DeleteDir() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DeleteMuxWaitSem()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32DELETEMUXWAITSEM( HMUX, HSEM );
PCHAR szDos32DeleteMuxWaitSem = "Dos32DeleteMuxWaitSem";

APIRET APIENTRY Trc_Dos32DeleteMuxWaitSem( HMUX Parm1
                                         , HSEM Parm2
                                         )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DeleteMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMUX );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HSEM );
         T_LogULONG( (ULONG)Parm2 );
      }
      HOOKENTRY( szDos32DeleteMuxWaitSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32DELETEMUXWAITSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DeleteMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32DeleteMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DeleteMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16DevConfig()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSDEVCONFIG( PVOID16, USHORT, USHORT );
#define DOSDEVCONFIG_PARMBYTES ( sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16DevConfig = "Dos16DevConfig";

APIRET16 APIENTRY16 Trc_Dos16DevConfig( PVOID16 Parm1
                                      , USHORT  Parm2
                                      , USHORT  Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSDEVCONFIG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16DevConfig, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16DevConfig, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSDEVCONFIG( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16DevConfig );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPCHAR16( Parm1 );
      }
      HOOKEXIT16( szDos16DevConfig, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16DevConfig() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DevConfig()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32DEVCONFIG( PVOID, ULONG );
PCHAR szDos32DevConfig = "Dos32DevConfig";

APIRET APIENTRY Trc_Dos32DevConfig( PVOID Parm1
                                  , ULONG Parm2
                                  )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DevConfig );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32DevConfig, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32DEVCONFIG( Parm1, Parm2 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DevConfig );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPCHAR( Parm1 );
      }
      HOOKEXIT( szDos32DevConfig, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DevConfig() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16DevIOCtl()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSDEVIOCTL( PVOID16, PVOID16, USHORT, USHORT, HFILE16 );
#define DOSDEVIOCTL_PARMBYTES ( sizeof( PVOID16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HFILE16 ) )
PCHAR szDos16DevIOCtl = "Dos16DevIOCtl";

APIRET16 APIENTRY16 Trc_Dos16DevIOCtl( PVOID16 Parm1
                                     , PVOID16 Parm2
                                     , USHORT  Parm3
                                     , USHORT  Parm4
                                     , HFILE16 Parm5
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSDEVIOCTL_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16DevIOCtl, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID16( Parm1 );
         T_LogCHARBufferUnformatted16( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogCHARBufferUnformatted16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_HFILE );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szDos16DevIOCtl, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSDEVIOCTL( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16DevIOCtl );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID16( Parm1 );
         if ( !usRC ) T_LogCHARBufferUnformatted16( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         if ( !usRC ) T_LogCHARBufferUnformatted16( Parm2 );
      }
      HOOKEXIT16( szDos16DevIOCtl, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16DevIOCtl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16DevIOCtl2()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSDEVIOCTL2( PVOID16, USHORT, PVOID16, USHORT, USHORT, USHORT, HFILE16 );
#define DOSDEVIOCTL2_PARMBYTES ( sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HFILE16 ) )
PCHAR szDos16DevIOCtl2 = "Dos16DevIOCtl2";

APIRET16 APIENTRY16 Trc_Dos16DevIOCtl2( PVOID16 Parm1
                                      , USHORT  Parm2
                                      , PVOID16 Parm3
                                      , USHORT  Parm4
                                      , USHORT  Parm5
                                      , USHORT  Parm6
                                      , HFILE16 Parm7
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSDEVIOCTL2_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16DevIOCtl2, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID16( Parm1 );
         T_LogCHARBuffer16( Parm1, Parm2 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogCHARBuffer16( Parm3, Parm4 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_USHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_HFILE );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szDos16DevIOCtl2, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSDEVIOCTL2( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16DevIOCtl2 );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID16( Parm1 );
         if ( !usRC ) T_LogCHARBuffer16( Parm1, Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( !usRC ) T_LogCHARBuffer16( Parm3, Parm4 );
      }
      HOOKEXIT16( szDos16DevIOCtl2, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16DevIOCtl2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DevIOCtl()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32DEVIOCTL( HFILE, ULONG, ULONG, PVOID, ULONG, PULONG, PVOID, ULONG, PULONG );
PCHAR szDos32DevIOCtl = "Dos32DevIOCtl";

APIRET APIENTRY Trc_Dos32DevIOCtl( HFILE  Parm1
                                 , ULONG  Parm2
                                 , ULONG  Parm3
                                 , PVOID  Parm4
                                 , ULONG  Parm5
                                 , PULONG Parm6
                                 , PVOID  Parm7
                                 , ULONG  Parm8
                                 , PULONG Parm9
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DevIOCtl );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( T_IsValidMem( Parm6, sizeof( ULONG ) ) ) T_LogCHARBuffer( Parm4, *Parm6 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
         T_LogParameter( 7L, DT_PVOID );
         T_LogPVOID( Parm7 );
         if ( T_IsValidMem( Parm9, sizeof( ULONG ) ) ) T_LogCHARBuffer( Parm7, *Parm9 );
         T_LogParameter( 8L, DT_ULONG );
         T_LogULONG( Parm8 );
         T_LogParameter( 9L, DT_PULONG );
         T_LogPULONG( Parm9 );
      }
      HOOKENTRY( szDos32DevIOCtl, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32DEVIOCTL( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DevIOCtl );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( ( Parm6 ) && ( !ulRC ) ) T_LogCHARBuffer( Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
         T_LogParameter( 7L, DT_PVOID );
         T_LogPVOID( Parm7 );
         if ( ( Parm9 ) && ( !ulRC ) ) T_LogCHARBuffer( Parm7, *Parm9 );
         T_LogParameter( 9L, DT_PULONG );
         T_LogPULONG( Parm9 );
      }
      HOOKEXIT( szDos32DevIOCtl, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DevIOCtl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DisConnectNPipe()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32DISCONNECTNPIPE( HPIPE );
PCHAR szDos32DisConnectNPipe = "Dos32DisConnectNPipe";

APIRET APIENTRY Trc_Dos32DisConnectNPipe( HPIPE Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DisConnectNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPIPE );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32DisConnectNPipe, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32DISCONNECTNPIPE( Parm1 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DisConnectNPipe );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32DisConnectNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DisConnectNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DumpProcess()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 210 ) )
APIRET APIENTRY DOS32DUMPPROCESS( ULONG, ULONG, PID );
PCHAR szDos32DumpProcess = "Dos32DumpProcess";

APIRET APIENTRY Trc_Dos32DumpProcess( ULONG Parm1
                                    , ULONG Parm2
                                    , PID   Parm3
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DumpProcess );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PID );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32DumpProcess, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32DUMPPROCESS )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DumpProcess );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32DumpProcess, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DumpProcess() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16DupHandle()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSDUPHANDLE( HFILE16, PHFILE16 );
#define DOSDUPHANDLE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PHFILE16 ) )
PCHAR szDos16DupHandle = "Dos16DupHandle";

APIRET16 APIENTRY16 Trc_Dos16DupHandle( HFILE16  Parm1
                                      , PHFILE16 Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSDUPHANDLE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16DupHandle, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKENTRY16( szDos16DupHandle, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSDUPHANDLE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16DupHandle );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16DupHandle, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16DupHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DupHandle()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32DUPHANDLE( HFILE, PHFILE );
PCHAR szDos32DupHandle = "Dos32DupHandle";

APIRET APIENTRY Trc_Dos32DupHandle( HFILE  Parm1
                                  , PHFILE Parm2
                                  )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DupHandle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPULONG( Parm2 );
      }
      HOOKENTRY( szDos32DupHandle, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32DUPHANDLE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DupHandle );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32DupHandle, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DupHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16DynamicTrace()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSDYNAMICTRACE( USHORT, PBYTE16, PBYTE16 );
#define DOSDYNAMICTRACE_PARMBYTES ( sizeof( USHORT ) + sizeof( PBYTE16 ) + sizeof( PBYTE16 ) )
PCHAR szDos16DynamicTrace = "Dos16DynamicTrace";

APIRET16 APIENTRY16 Trc_Dos16DynamicTrace( USHORT  Parm1
                                         , PBYTE16 Parm2
                                         , PBYTE16 Parm3
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSDYNAMICTRACE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16DynamicTrace, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID16( Parm2 );
         T_LogD16DT_Parm2( Parm2, Parm1 );
         T_LogParameter( 3L, DT_PBYTE );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16DynamicTrace, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSDYNAMICTRACE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16DynamicTrace );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16DynamicTrace, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16DynamicTrace() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32DynamicTrace()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32DYNAMICTRACE( ULONG, PBYTE, PBYTE );
PCHAR szDos32DynamicTrace = "Dos32DynamicTrace";

APIRET APIENTRY Trc_Dos32DynamicTrace( ULONG Parm1
                                     , PBYTE Parm2
                                     , PBYTE Parm3
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32DynamicTrace );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID( Parm2 );
         T_LogD32DT_Parm2( Parm2, Parm1, TRUE );
         T_LogParameter( 3L, DT_PBYTE );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32DynamicTrace, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32DYNAMICTRACE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32DynamicTrace );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID( Parm2 );
         if ( !ulRC ) T_LogD32DT_Parm2( Parm2, Parm1, FALSE );
         T_LogParameter( 3L, DT_PBYTE );
         T_LogPVOID( Parm3 );
         if ( !ulRC ) T_LogD32DT_Parm3( Parm3, Parm1, Parm2 );
      }
      HOOKEXIT( szDos32DynamicTrace, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32DynamicTrace() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16EditName()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSEDITNAME( USHORT, PSZ16, PSZ16, PBYTE16, USHORT );
#define DOSEDITNAME_PARMBYTES ( sizeof( USHORT ) + sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( PBYTE16 ) + sizeof( USHORT ) )
PCHAR szDos16EditName = "Dos16EditName";

APIRET16 APIENTRY16 Trc_Dos16EditName( USHORT  Parm1
                                     , PSZ16   Parm2
                                     , PSZ16   Parm3
                                     , PBYTE16 Parm4
                                     , USHORT  Parm5
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSEDITNAME_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16EditName, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szDos16EditName, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSEDITNAME( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16EditName );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         if ( !usRC ) T_LogPSZ16( (PSZ16)Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szDos16EditName, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16EditName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32EditName()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32EDITNAME( ULONG, PSZ, PSZ, PBYTE, ULONG );
PCHAR szDos32EditName = "Dos32EditName";

APIRET APIENTRY Trc_Dos32EditName( ULONG Parm1
                                 , PSZ   Parm2
                                 , PSZ   Parm3
                                 , PBYTE Parm4
                                 , ULONG Parm5
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32EditName );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32EditName, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32EDITNAME( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32EditName );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         if ( !ulRC ) T_LogPSZ( (PSZ)Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szDos32EditName, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32EditName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16EnterCritSec()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSENTERCRITSEC( VOID );
#define DOSENTERCRITSEC_PARMBYTES ( 0 )
PCHAR szDos16EnterCritSec = "Dos16EnterCritSec";

APIRET16 APIENTRY16 Trc_Dos16EnterCritSec( VOID )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSENTERCRITSEC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16EnterCritSec, sRegs16.ulCSIP );
      HOOKENTRY16( szDos16EnterCritSec, NULL );
      T_FreeLog( );
   }

   usRC = DOSENTERCRITSEC( );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16EnterCritSec );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16EnterCritSec, NULL, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16EnterCritSec() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32EnterCritSec()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ENTERCRITSEC( VOID );
PCHAR szDos32EnterCritSec = "Dos32EnterCritSec";

APIRET APIENTRY Trc_Dos32EnterCritSec( VOID )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32EnterCritSec );
      HOOKENTRY( szDos32EnterCritSec, NULL );
      T_FreeLog( );
   }

   ulRC = DOS32ENTERCRITSEC( );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32EnterCritSec );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32EnterCritSec, NULL, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32EnterCritSec() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32EnterMustComplete()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ENTERMUSTCOMPLETE( PULONG );
PCHAR szDos32EnterMustComplete = "Dos32EnterMustComplete";

APIRET APIENTRY Trc_Dos32EnterMustComplete( PULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32EnterMustComplete );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32EnterMustComplete, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ENTERMUSTCOMPLETE( Parm1 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32EnterMustComplete );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPULONG( Parm1 );
      }
      HOOKEXIT( szDos32EnterMustComplete, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32EnterMustComplete() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16EnumAttribute()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSENUMATTRIBUTE( USHORT, PVOID16, ULONG, PVOID16, ULONG, PULONG16, ULONG, ULONG );
#define DOSENUMATTRIBUTE_PARMBYTES ( sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( ULONG ) + sizeof( PVOID16 ) + sizeof( ULONG ) + sizeof( PULONG16 ) + sizeof( ULONG ) + sizeof( ULONG ) )
PCHAR szDos16EnumAttribute = "Dos16EnumAttribute";

APIRET16 APIENTRY16 Trc_Dos16EnumAttribute( USHORT   Parm1
                                          , PVOID16  Parm2
                                          , ULONG    Parm3
                                          , PVOID16  Parm4
                                          , ULONG    Parm5
                                          , PULONG16 Parm6
                                          , ULONG    Parm7
                                          , ULONG    Parm8
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSENUMATTRIBUTE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16EnumAttribute, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         if ( Parm1 == ENUMEA_REFTYPE_FHANDLE ) T_LogPUSHORT16( Parm2 );
         else if ( Parm1 == ENUMEA_REFTYPE_PATH ) T_LogPSZ16( Parm2 );
         else T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG16( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_ULONG );
         T_LogULONG( Parm8 );
      }
      HOOKENTRY16( szDos16EnumAttribute, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSENUMATTRIBUTE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16EnumAttribute );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogDENA1Buffer( Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG16( Parm6 );
      }
      HOOKEXIT16( szDos16EnumAttribute, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16EnumAttribute() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32EnumAttribute()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ENUMATTRIBUTE( ULONG, PVOID, ULONG, PVOID, ULONG, PULONG, ULONG );
PCHAR szDos32EnumAttribute = "Dos32EnumAttribute";

APIRET APIENTRY Trc_Dos32EnumAttribute( ULONG  Parm1
                                      , PVOID  Parm2
                                      , ULONG  Parm3
                                      , PVOID  Parm4
                                      , ULONG  Parm5
                                      , PULONG Parm6
                                      , ULONG  Parm7
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32EnumAttribute );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         if ( Parm1 == ENUMEA_REFTYPE_FHANDLE ) T_LogPULONG( (PULONG)Parm2 );
         else if ( Parm1 == ENUMEA_REFTYPE_PATH ) T_LogPSZ( (PSZ)Parm2 );
         else T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szDos32EnumAttribute, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ENUMATTRIBUTE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32EnumAttribute );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( ( Parm6 ) && ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogDENA2Buffer( (PDENA2)Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      HOOKEXIT( szDos32EnumAttribute, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32EnumAttribute() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ErrClass()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSERRCLASS( USHORT, PUSHORT16, PUSHORT16, PUSHORT16 );
#define DOSERRCLASS_PARMBYTES ( sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( PUSHORT16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16ErrClass = "Dos16ErrClass";

APIRET16 APIENTRY16 Trc_Dos16ErrClass( USHORT    Parm1
                                     , PUSHORT16 Parm2
                                     , PUSHORT16 Parm3
                                     , PUSHORT16 Parm4
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSERRCLASS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ErrClass, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16ErrClass, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSERRCLASS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ErrClass );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16ErrClass, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ErrClass() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ErrClass()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ERRCLASS( ULONG, PULONG, PULONG, PULONG );
PCHAR szDos32ErrClass = "Dos32ErrClass";

APIRET APIENTRY Trc_Dos32ErrClass( ULONG  Parm1
                                 , PULONG Parm2
                                 , PULONG Parm3
                                 , PULONG Parm4
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ErrClass );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32ErrClass, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ERRCLASS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ErrClass );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32ErrClass, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ErrClass() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Error()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSERROR( USHORT );
#define DOSERROR_PARMBYTES ( sizeof( USHORT ) )
PCHAR szDos16Error = "Dos16Error";

APIRET16 APIENTRY16 Trc_Dos16Error( USHORT Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSERROR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Error, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16Error, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSERROR( Parm1 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Error );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16Error, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Error() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Error()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32ERROR( ULONG );
PCHAR szDos32Error = "Dos32Error";

APIRET APIENTRY Trc_Dos32Error( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Error );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32Error, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32ERROR( Parm1 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Error );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32Error, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Error() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ExecPgm()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSEXECPGM( PCHAR16, SHORT, USHORT, PSZ16, PSZ16, PRESULTCODES16, PSZ16 );
#define DOSEXECPGM_PARMBYTES ( sizeof( PCHAR16 ) + sizeof( SHORT ) + sizeof( USHORT ) + sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( PRESULTCODES16 ) + sizeof( PSZ16 ) )
PCHAR szDos16ExecPgm = "Dos16ExecPgm";

APIRET16 APIENTRY16 Trc_Dos16ExecPgm( PCHAR16        Parm1
                                    , SHORT          Parm2
                                    , USHORT         Parm3
                                    , PSZ16          Parm4
                                    , PSZ16          Parm5
                                    , PRESULTCODES16 Parm6
                                    , PSZ16          Parm7
                                    )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSEXECPGM_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ExecPgm, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCHAR );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZs16( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZs16( Parm5 );
         T_LogParameter( 6L, DT_PRESULTCODES );
         T_LogPVOID16( Parm6 );
         T_LogParameter( 7L, DT_PSZ );
         T_LogPSZ16( Parm7 );
      }
      HOOKENTRY16( szDos16ExecPgm, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSEXECPGM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16ExecPgm );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm3 == EXEC_SYNC ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PCHAR );
         T_LogPVOID16( Parm1 );
         if ( usRC ) T_LogCHARBuffer16( Parm1, Parm2 );
         T_LogParameter( 6L, DT_PRESULTCODES );
         if ( !usRC ) T_LogPRESULTCODES16( Parm6 );
         else T_LogPVOID16( Parm6 );
      }
      HOOKEXIT16( szDos16ExecPgm, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ExecPgm() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ExecPgm()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32EXECPGM( PCHAR, LONG, ULONG, PSZ, PSZ, PRESULTCODES, PSZ );
PCHAR szDos32ExecPgm = "Dos32ExecPgm";

APIRET APIENTRY Trc_Dos32ExecPgm( PCHAR        Parm1
                                , LONG         Parm2
                                , ULONG        Parm3
                                , PSZ          Parm4
                                , PSZ          Parm5
                                , PRESULTCODES Parm6
                                , PSZ          Parm7
                                )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ExecPgm );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCHAR );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZs( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZs( Parm5 );
         T_LogParameter( 6L, DT_PRESULTCODES );
         T_LogPVOID( Parm6 );
         T_LogParameter( 7L, DT_PSZ );
         T_LogPSZ( Parm7 );
      }
      HOOKENTRY( szDos32ExecPgm, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32EXECPGM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32ExecPgm );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm3 == EXEC_SYNC ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PCHAR );
         T_LogPVOID( Parm1 );
         if ( ulRC ) T_LogCHARBuffer( Parm1, Parm2 );
         T_LogParameter( 6L, DT_PRESULTCODES );
         if ( !ulRC ) T_LogPRESULTCODES( Parm6 );
         else T_LogPVOID( Parm6 );
      }
      HOOKEXIT( szDos32ExecPgm, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ExecPgm() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Exit()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
VOID APIENTRY16 DOSEXIT( USHORT, USHORT );
#define DOSEXIT_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16Exit = "Dos16Exit";

VOID APIENTRY16 Trc_Dos16Exit( USHORT Parm1
                             , USHORT Parm2
                             )
{

   REGS16 sRegs16;                               /* 16-bit registers          */

   sRegs16.usParmBytes = DOSEXIT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Exit, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      T_LogStackDump16( );
      HOOKENTRY16( szDos16Exit, &Parm1 );
      T_FreeLog( );
   }

   DOSEXIT( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Exit );
      HOOKEXIT16( szDos16Exit, &Parm1, NULL );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

}  /* Trc_Dos16Exit() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Exit()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
VOID APIENTRY DOS32EXIT( ULONG, ULONG );
PCHAR szDos32Exit = "Dos32Exit";

VOID APIENTRY Trc_Dos32Exit( ULONG Parm1
                           , ULONG Parm2
                           )
{

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Exit );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
#ifdef INTEL
      T_LogStackDump( );
#endif
      HOOKENTRY( szDos32Exit, &Parm1 );
      T_FreeLog( );
   }

   DOS32EXIT( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Exit );
      HOOKEXIT( szDos32Exit, &Parm1, NULL );
      T_FreeLog( );
   }

}  /* Trc_Dos32Exit() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ExitCritSec()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSEXITCRITSEC( VOID );
#define DOSEXITCRITSEC_PARMBYTES ( 0 )
PCHAR szDos16ExitCritSec = "Dos16ExitCritSec";

APIRET16 APIENTRY16 Trc_Dos16ExitCritSec( VOID )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSEXITCRITSEC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ExitCritSec, sRegs16.ulCSIP );
      HOOKENTRY16( szDos16ExitCritSec, NULL );
      T_FreeLog( );
   }

   usRC = DOSEXITCRITSEC( );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ExitCritSec );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ExitCritSec, NULL, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos32ExitCritSec() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ExitCritSec()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32EXITCRITSEC( VOID );
PCHAR szDos32ExitCritSec = "Dos32ExitCritSec";

APIRET APIENTRY Trc_Dos32ExitCritSec( VOID )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ExitCritSec );
      HOOKENTRY( szDos32ExitCritSec, NULL );
      T_FreeLog( );
   }

   ulRC = DOS32EXITCRITSEC( );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ExitCritSec );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ExitCritSec, NULL, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ExitCritSec() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ExitList()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSEXITLIST( USHORT, PFNEXITLIST16 );
#define DOSEXITLIST_PARMBYTES ( sizeof( USHORT ) + sizeof( PFNEXITLIST16 ) )
PCHAR szDos16ExitList = "Dos16ExitList";

APIRET16 APIENTRY16 Trc_Dos16ExitList( USHORT        Parm1
                                     , PFNEXITLIST16 Parm2
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSEXITLIST_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ExitList, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PFNEXITLIST );
         T_LogPFN16( (PFN16)Parm2 );
      }
      HOOKENTRY16( szDos16ExitList, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSEXITLIST( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ExitList );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ExitList, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ExitList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ExitList()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32EXITLIST( ULONG, PFNEXITLIST );
PCHAR szDos32ExitList = "Dos32ExitList";

APIRET APIENTRY Trc_Dos32ExitList( ULONG       Parm1
                                 , PFNEXITLIST Parm2
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ExitList );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFNEXITLIST );
         T_LogPFN( (PFN)Parm2 );
      }
      HOOKENTRY( szDos32ExitList, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32EXITLIST( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ExitList );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ExitList, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ExitList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ExitMustComplete()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32EXITMUSTCOMPLETE( PULONG );
PCHAR szDos32ExitMustComplete = "Dos32ExitMustComplete";

APIRET APIENTRY Trc_Dos32ExitMustComplete( PULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ExitMustComplete );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32ExitMustComplete, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32EXITMUSTCOMPLETE( Parm1 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ExitMustComplete );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPULONG( Parm1 );
      }
      HOOKEXIT( szDos32ExitMustComplete, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ExitMustComplete() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FileIO()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFILEIO( HFILE16, PVOID16, USHORT, PUSHORT16 );
#define DOSFILEIO_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16FileIO = "Dos16FileIO";

APIRET16 APIENTRY16 Trc_Dos16FileIO( HFILE16   Parm1
                                   , PVOID16   Parm2
                                   , USHORT    Parm3
                                   , PUSHORT16 Parm4
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFILEIO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FileIO, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogD16FIO_Parm2( Parm2, Parm3 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16FileIO, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFILEIO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FileIO );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16FileIO, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FileIO() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FileLocks()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFILELOCKS( HFILE16, PFILELOCK16, PFILELOCK16 );
#define DOSFILELOCKS_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PFILELOCK16 ) + sizeof( PFILELOCK16 ) )
PCHAR szDos16FileLocks = "Dos16FileLocks";

APIRET16 APIENTRY16 Trc_Dos16FileLocks( HFILE16     Parm1
                                      , PFILELOCK16 Parm2
                                      , PFILELOCK16 Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFILELOCKS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FileLocks, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PFILELOCK );
         T_LogPFILELOCK16( Parm2 );
         T_LogParameter( 3L, DT_PFILELOCK );
         T_LogPFILELOCK16( Parm3 );
      }
      HOOKENTRY16( szDos16FileLocks, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFILELOCKS( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FileLocks );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16FileLocks, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FileLocks() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FindClose()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFINDCLOSE( HDIR16 );
#define DOSFINDCLOSE_PARMBYTES ( sizeof( HDIR16 ) )
PCHAR szDos16FindClose = "Dos16FindClose";

APIRET16 APIENTRY16 Trc_Dos16FindClose( HDIR16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFINDCLOSE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FindClose, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDIR );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16FindClose, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFINDCLOSE( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FindClose );
      T_LogCPReturnCode( usRC );
      if ( !usRC ) DelDFFLevel( Parm1 );
      HOOKEXIT16( szDos16FindClose, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FindClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FindClose()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FINDCLOSE( HDIR );
PCHAR szDos32FindClose = "Dos32FindClose";

APIRET APIENTRY Trc_Dos32FindClose( HDIR Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FindClose );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDIR );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32FindClose, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FINDCLOSE( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FindClose );
      T_LogCPReturnCode( ulRC );
      if ( !ulRC ) DelDFFLevel( Parm1 );
      HOOKEXIT( szDos32FindClose, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FindClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FindFirst()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFINDFIRST( PSZ16, PHDIR16, USHORT, PFILEFINDBUF16, USHORT, PUSHORT, ULONG );
#define DOSFINDFIRST_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHDIR16 ) + sizeof( USHORT ) + sizeof( PFILEFINDBUF16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( ULONG ) )
PCHAR szDos16FindFirst = "Dos16FindFirst";

APIRET16 APIENTRY16 Trc_Dos16FindFirst( PSZ16          Parm1
                                      , PHDIR16        Parm2
                                      , USHORT         Parm3
                                      , PFILEFINDBUF16 Parm4
                                      , USHORT         Parm5
                                      , PUSHORT16      Parm6
                                      , ULONG          Parm7
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFINDFIRST_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FindFirst, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PHDIR );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PFILEFINDBUF );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPUSHORT16( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY16( szDos16FindFirst, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFINDFIRST( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FindFirst );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHDIR );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 4L, DT_PFILEFINDBUF );
         T_LogPVOID16( Parm4 );
         if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogD16FF_Parm4( Parm4, *Parm6, 1 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPUSHORT16( Parm6 );
      }
      if ( ( Parm2 ) && ( !usRC ) ) AddDFFLevel( *Parm2, 1L );
      HOOKEXIT16( szDos16FindFirst, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FindFirst() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FindFirst2()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFINDFIRST2( PSZ16, PHDIR16, USHORT, PVOID16, USHORT, PUSHORT, USHORT, ULONG );
#define DOSFINDFIRST2_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHDIR16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16FindFirst2 = "Dos16FindFirst2";

APIRET16 APIENTRY16 Trc_Dos16FindFirst2( PSZ16     Parm1
                                       , PHDIR16   Parm2
                                       , USHORT    Parm3
                                       , PVOID16   Parm4
                                       , USHORT    Parm5
                                       , PUSHORT16 Parm6
                                       , USHORT    Parm7
                                       , ULONG     Parm8
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFINDFIRST2_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FindFirst2, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PHDIR );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         if ( Parm7 == FIL_QUERYEASFROMLIST ) T_LogPEAOP( Parm4, PEAOP_GEA );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPUSHORT16( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
         T_LogParameter( 8L, DT_ULONG );
         T_LogULONG( Parm8 );
      }
      HOOKENTRY16( szDos16FindFirst2, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFINDFIRST2( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FindFirst2 );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHDIR );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogD16FF_Parm4( Parm4, *Parm6, Parm7 );
         else if ( usRC == ERROR_EAS_DIDNT_FIT ) T_LogD16FF_Parm4( Parm4, 1, FIL_QUERYEASIZE );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPUSHORT16( Parm6 );
      }
      if ( ( Parm2 ) && ( !usRC ) ) AddDFFLevel( *Parm2, Parm7 );
      HOOKEXIT16( szDos16FindFirst2, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FindFirst2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FindFirst()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FINDFIRST( PSZ, PHDIR, ULONG, PVOID, ULONG, PULONG, ULONG );
PCHAR szDos32FindFirst = "Dos32FindFirst";

APIRET APIENTRY Trc_Dos32FindFirst( PSZ    Parm1
                                  , PHDIR  Parm2
                                  , ULONG  Parm3
                                  , PVOID  Parm4
                                  , ULONG  Parm5
                                  , PULONG Parm6
                                  , ULONG  Parm7
                                  )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FindFirst );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHDIR );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( Parm7 == FIL_QUERYEASFROMLIST ) T_LogPEAOP2( Parm4, PEAOP_GEA );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szDos32FindFirst, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FINDFIRST( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FindFirst );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHDIR );
         T_LogPULONG( Parm2 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( ( Parm6 ) && ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogD32FF_Parm4( Parm4, *Parm6, Parm7 );
         else if ( ulRC == ERROR_EAS_DIDNT_FIT ) T_LogD32FF_Parm4( Parm4, 1L, FIL_QUERYEASIZE );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      if ( ( Parm2 ) && ( !ulRC ) ) AddDFFLevel( *Parm2, Parm7 );
      HOOKEXIT( szDos32FindFirst, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FindFirst() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FindFromName()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET APIENTRY DOS32FINDFROMNAME( HDIR, PVOID, ULONG, PULONG, ULONG, PVOID );
PCHAR szDos32FindFromName = "Dos32FindFromName";

APIRET APIENTRY Trc_Dos32FindFromName( HDIR   Parm1
                                     , PVOID  Parm2
                                     , ULONG  Parm3
                                     , PULONG Parm4
                                     , ULONG  Parm5
                                     , PVOID  Parm6
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FindFromName );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDIR );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( GetDFFLevel( Parm1 ) == FIL_QUERYEASFROMLIST ) T_LogPEAOP2( Parm2, PEAOP_GEA );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PVOID );
         if ( GetDFFLevel( Parm1 ) ) T_LogPSZ( Parm6 );
         else T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szDos32FindFromName, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32FINDFROMNAME )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FindFromName );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( ( Parm4 ) && ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogD32FF_Parm4( Parm2, *Parm4, GetDFFLevel( Parm1 ) );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32FindFromName, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FindFromName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FindNext()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFINDNEXT( HDIR16, PFILEFINDBUF16, USHORT, PUSHORT16 );
#define DOSFINDNEXT_PARMBYTES ( sizeof( HDIR16 ) + sizeof( PFILEFINDBUF16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16FindNext = "Dos16FindNext";

APIRET16 APIENTRY16 Trc_Dos16FindNext( HDIR16         Parm1
                                     , PFILEFINDBUF16 Parm2
                                     , USHORT         Parm3
                                     , PUSHORT16      Parm4
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFINDNEXT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FindNext, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDIR );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PFILEFINDBUF );
         T_LogPVOID16( Parm2 );
         if ( GetDFFLevel( Parm1 ) == FIL_QUERYEASFROMLIST ) T_LogPEAOP( (PEAOP16)Parm2, PEAOP_GEA );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKENTRY16( szDos16FindNext, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFINDNEXT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FindNext );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PFILEFINDBUF );
         T_LogPVOID16( Parm2 );
         if ( ( Parm4 ) && ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogD16FF_Parm4( Parm2, *Parm4, GetDFFLevel( Parm1 ) );
         else if ( usRC == ERROR_EAS_DIDNT_FIT ) T_LogD16FF_Parm4( Parm2, 1, FIL_QUERYEASIZE );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16FindNext, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FindNext() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FindNext()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FINDNEXT( HDIR, PVOID, ULONG, PULONG );
PCHAR szDos32FindNext = "Dos32FindNext";

APIRET APIENTRY Trc_Dos32FindNext( HDIR   Parm1
                                 , PVOID  Parm2
                                 , ULONG  Parm3
                                 , PULONG Parm4
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FindNext );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDIR );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( GetDFFLevel( Parm1 ) == FIL_QUERYEASFROMLIST ) T_LogPEAOP2( Parm2, PEAOP_GEA );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKENTRY( szDos32FindNext, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FINDNEXT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FindNext );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( ( Parm4 ) && ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogD32FF_Parm4( Parm2, *Parm4, GetDFFLevel( Parm1 ) );
         else if ( ulRC == ERROR_EAS_DIDNT_FIT ) T_LogD32FF_Parm4( Parm2, 1L, FIL_QUERYEASIZE );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32FindNext, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FindNext() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FlagProcess()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFLAGPROCESS( PID16, USHORT, USHORT, USHORT );
#define DOSFLAGPROCESS_PARMBYTES ( sizeof( PID16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16FlagProcess = "Dos16FlagProcess";

APIRET16 APIENTRY16 Trc_Dos16FlagProcess( PID16  Parm1
                                        , USHORT Parm2
                                        , USHORT Parm3
                                        , USHORT Parm4
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFLAGPROCESS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SIG ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FlagProcess, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PID );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szDos16FlagProcess, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFLAGPROCESS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_SIG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FlagProcess );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16FlagProcess, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FlagProcess() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ForceDelete()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFORCEDELETE( PSZ16, ULONG );
#define DOSFORCEDELETE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szDos16ForceDelete = "Dos16ForceDelete";

APIRET16 APIENTRY16 Trc_Dos16ForceDelete( PSZ16 Parm1
                                        , ULONG Parm2
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFORCEDELETE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ForceDelete, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16ForceDelete, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFORCEDELETE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ForceDelete );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ForceDelete, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ForceDelete() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ForceDelete()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FORCEDELETE( PSZ );
PCHAR szDos32ForceDelete = "Dos32ForceDelete";

APIRET APIENTRY Trc_Dos32ForceDelete( PSZ Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ForceDelete );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
      }
      HOOKENTRY( szDos32ForceDelete, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FORCEDELETE( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ForceDelete );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ForceDelete, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ForceDelete() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FreeMem()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FREEMEM( PVOID );
PCHAR szDos32FreeMem = "Dos32FreeMem";

APIRET APIENTRY Trc_Dos32FreeMem( PVOID Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FreeMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32FreeMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FREEMEM( Parm1 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FreeMem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32FreeMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FreeMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FreeModule()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFREEMODULE( HMODULE16 );
#define DOSFREEMODULE_PARMBYTES ( sizeof( HMODULE16 ) )
PCHAR szDos16FreeModule = "Dos16FreeModule";

APIRET16 APIENTRY16 Trc_Dos16FreeModule( HMODULE16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFREEMODULE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FreeModule, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16FreeModule, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFREEMODULE( Parm1 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FreeModule );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16FreeModule, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FreeModule() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FreeModule()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FREEMODULE( HMODULE );
PCHAR szDos32FreeModule = "Dos32FreeModule";

APIRET APIENTRY Trc_Dos32FreeModule( HMODULE Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FreeModule );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32FreeModule, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FREEMODULE( Parm1 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FreeModule );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32FreeModule, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FreeModule() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FreeResource()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFREERESOURCE( PVOID16 );
#define DOSFREERESOURCE_PARMBYTES ( sizeof( PVOID16 ) )
PCHAR szDos16FreeResource = "Dos16FreeResource";

APIRET16 APIENTRY16 Trc_Dos16FreeResource( PVOID16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFREERESOURCE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FreeResource, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16FreeResource, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFREERESOURCE( Parm1 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FreeResource );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16FreeResource, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FreeResource() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FreeResource()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FREERESOURCE( PVOID );
PCHAR szDos32FreeResource = "Dos32FreeResource";

APIRET APIENTRY Trc_Dos32FreeResource( PVOID Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FreeResource );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32FreeResource, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FREERESOURCE( Parm1 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FreeResource );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32FreeResource, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FreeResource() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FreeSeg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFREESEG( SEL );
#define DOSFREESEG_PARMBYTES ( sizeof( SEL ) )
PCHAR szDos16FreeSeg = "Dos16FreeSeg";

APIRET16 APIENTRY16 Trc_Dos16FreeSeg( SEL Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFREESEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FreeSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16FreeSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFREESEG( Parm1 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FreeSeg );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16FreeSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FreeSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FreeSpinLock()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFREESPINLOCK( HSPINLOCK );
PCHAR szDos16FreeSpinLock = "Dos16FreeSpinLock";

APIRET16 APIENTRY16 Trc_Dos16FreeSpinLock( HSPINLOCK Parm1 )
{

   APIRET16 usRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos16FreeSpinLock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSPINLOCK );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szDos16FreeSpinLock, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOSFREESPINLOCK )
   {
      usRC = ( *pfnDOSFREESPINLOCK )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos16FreeSpinLock );
         T_FreeLog( );
      }

      usRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FreeSpinLock );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16FreeSpinLock, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Dos16FreeSpinLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FreeSpinLock()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32FREESPINLOCK( HSPINLOCK );
PCHAR szDos32FreeSpinLock = "Dos32FreeSpinLock";

APIRET APIENTRY Trc_Dos32FreeSpinLock( HSPINLOCK Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FreeSpinLock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSPINLOCK );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32FreeSpinLock, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOS32FREESPINLOCK )
   {
      ulRC = ( *pfnDOS32FREESPINLOCK )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos32FreeSpinLock );
         T_FreeLog( );
      }

      ulRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FreeSpinLock );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32FreeSpinLock, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FreeSpinLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FreeThreadLocalMemory()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
APIRET APIENTRY DOS32FREETHREADLOCALMEMORY( PVOID );
PCHAR szDos32FreeThreadLocalMemory = "Dos32FreeThreadLocalMemory";

APIRET APIENTRY Trc_Dos32FreeThreadLocalMemory( PVOID Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FreeThreadLocalMemory );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32FreeThreadLocalMemory, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32FREETHREADLOCALMEMORY )( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FreeThreadLocalMemory );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32FreeThreadLocalMemory, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FreeThreadLocalMemory() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FSAttach()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFSATTACH( PSZ16, PSZ16, PVOID16, USHORT, USHORT, ULONG );
#define DOSFSATTACH_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16FSAttach = "Dos16FSAttach";

APIRET16 APIENTRY16 Trc_Dos16FSAttach( PSZ16   Parm1
                                     , PSZ16   Parm2
                                     , PVOID16 Parm3
                                     , USHORT  Parm4
                                     , USHORT  Parm5
                                     , ULONG   Parm6
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFSATTACH_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FSAttach, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogD16FSA_Parm3( Parm3, Parm4, Parm5 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY16( szDos16FSAttach, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFSATTACH( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FSAttach );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16FSAttach, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FSAttach() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FSAttach()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FSATTACH( PSZ, PSZ, PVOID, ULONG, ULONG );
PCHAR szDos32FSAttach = "Dos32FSAttach";

APIRET APIENTRY Trc_Dos32FSAttach( PSZ   Parm1
                                 , PSZ   Parm2
                                 , PVOID Parm3
                                 , ULONG Parm4
                                 , ULONG Parm5
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FSAttach );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogD32FSA_Parm3( Parm3, Parm4, Parm5 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32FSAttach, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FSATTACH( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FSAttach );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32FSAttach, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FSAttach() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FSCtl()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFSCTL( PVOID16, USHORT, PUSHORT16, PVOID16, USHORT, PUSHORT16, USHORT, PSZ16, HFILE16, USHORT, ULONG );
#define DOSFSCTL_PARMBYTES ( sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) + sizeof( PSZ16 ) + sizeof( HFILE16 ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16FSCtl = "Dos16FSCtl";

APIRET16 APIENTRY16 Trc_Dos16FSCtl( PVOID16   Parm1
                                  , USHORT    Parm2
                                  , PUSHORT16 Parm3
                                  , PVOID16   Parm4
                                  , USHORT    Parm5
                                  , PUSHORT16 Parm6
                                  , USHORT    Parm7
                                  , PSZ16     Parm8
                                  , HFILE16   Parm9
                                  , USHORT    Parm10
                                  , ULONG     Parm11
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFSCTL_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FSCtl, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID16( Parm1 );
         if ( T_IsValidMem16( Parm3, sizeof( USHORT ) ) ) T_LogCHARBuffer16( Parm1, *Parm3 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         if ( T_IsValidMem16( Parm6, sizeof( USHORT ) ) ) T_LogCHARBuffer16( Parm4, *Parm6 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPUSHORT16( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
         T_LogParameter( 8L, DT_PSZ );
         T_LogPSZ16( Parm8 );
         T_LogParameter( 9L, DT_HFILE );
         T_LogUSHORT( Parm9 );
         T_LogParameter( 10L, DT_USHORT );
         T_LogUSHORT( Parm10 );
         T_LogParameter( 11L, DT_ULONG );
         T_LogULONG( Parm11 );
      }
      HOOKENTRY16( szDos16FSCtl, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFSCTL( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FSCtl );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID16( Parm1 );
         if ( ( Parm3 ) && ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogCHARBuffer16( Parm1, *Parm3 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogCHARBuffer16( Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPUSHORT16( Parm6 );
      }
      HOOKEXIT16( szDos16FSCtl, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FSCtl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32FSCtl()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32FSCTL( PVOID, ULONG, PULONG, PVOID, ULONG, PULONG, ULONG, PSZ, HFILE, ULONG );
PCHAR szDos32FSCtl = "Dos32FSCtl";

APIRET APIENTRY Trc_Dos32FSCtl( PVOID  Parm1
                              , ULONG  Parm2
                              , PULONG Parm3
                              , PVOID  Parm4
                              , ULONG  Parm5
                              , PULONG Parm6
                              , ULONG  Parm7
                              , PSZ    Parm8
                              , HFILE  Parm9
                              , ULONG  Parm10
                              )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32FSCtl );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         if ( T_IsValidMem( Parm3, sizeof( ULONG ) ) ) T_LogCHARBuffer( Parm1, *Parm3 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( T_IsValidMem( Parm6, sizeof( ULONG ) ) ) T_LogCHARBuffer( Parm4, *Parm6 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PSZ );
         T_LogPSZ( Parm8 );
         T_LogParameter( 9L, DT_HFILE );
         T_LogULONG( Parm9 );
         T_LogParameter( 10L, DT_ULONG );
         T_LogULONG( Parm10 );
      }
      HOOKENTRY( szDos32FSCtl, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32FSCTL( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32FSCtl );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         if ( ( Parm3 ) && ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogCHARBuffer( Parm1, *Parm3 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( ( Parm6 ) && ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogCHARBuffer( Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      HOOKEXIT( szDos32FSCtl, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32FSCtl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FSRamSemClear()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFSRAMSEMCLEAR( PDOSFSRSEM16 );
#define DOSFSRAMSEMCLEAR_PARMBYTES ( sizeof( PDOSFSRSEM16 ) )
PCHAR szDos16FSRamSemClear = "Dos16FSRamSemClear";

APIRET16 APIENTRY16 Trc_Dos16FSRamSemClear( PDOSFSRSEM16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFSRAMSEMCLEAR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FSRamSemClear, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDOSFSRSEM );
         T_LogPDOSFSRSEM( Parm1 );
      }
      HOOKENTRY16( szDos16FSRamSemClear, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSFSRAMSEMCLEAR( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FSRamSemClear );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDOSFSRSEM );
         T_LogPDOSFSRSEM( Parm1 );
      }
      HOOKEXIT16( szDos16FSRamSemClear, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FSRamSemClear() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FSRamSemClear2()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 245 ) )
APIRET16 APIENTRY16 DOSFSRAMSEMCLEAR2( PFSRSEM216 );
#define DOSFSRAMSEMCLEAR2_PARMBYTES ( sizeof( PFSRSEM216 ) )
PCHAR szDos16FSRamSemClear2 = "Dos16FSRamSemClear2";

APIRET16 APIENTRY16 Trc_Dos16FSRamSemClear2( PFSRSEM216 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFSRAMSEMCLEAR2_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FSRamSemClear2, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PFSRSEM2 );
         T_LogPFSRSEM2( Parm1 );
      }
      HOOKENTRY16( szDos16FSRamSemClear2, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOSFSRAMSEMCLEAR2 )
   {
      usRC = ( *pfnDOSFSRAMSEMCLEAR2 )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SEM ) )
      {
         T_WaitLog( );
         T_LogMessage( szACPMsg, szDos16FSRamSemClear2 );
         T_FreeLog( );
      }

      usRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16FSRamSemClear2 );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PFSRSEM2 );
         T_LogPFSRSEM2( Parm1 );
      }
      HOOKEXIT16( szDos16FSRamSemClear2, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FSRamSemClear2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FSRamSemRequest()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSFSRAMSEMREQUEST( PDOSFSRSEM16, LONG );
#define DOSFSRAMSEMREQUEST_PARMBYTES ( sizeof( PDOSFSRSEM16 ) + sizeof( LONG ) )
PCHAR szDos16FSRamSemRequest = "Dos16FSRamSemRequest";

APIRET16 APIENTRY16 Trc_Dos16FSRamSemRequest( PDOSFSRSEM16 Parm1
                                            , LONG         Parm2
                                            )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFSRAMSEMREQUEST_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FSRamSemRequest, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDOSFSRSEM );
         T_LogPDOSFSRSEM( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16FSRamSemRequest, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSFSRAMSEMREQUEST( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16FSRamSemRequest );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PDOSFSRSEM );
         T_LogPDOSFSRSEM( Parm1 );
      }
      HOOKEXIT16( szDos16FSRamSemRequest, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FSRamSemRequest() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16FSRamSemRequest2()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 245 ) )
APIRET16 APIENTRY16 DOSFSRAMSEMREQUEST2( PFSRSEM216, LONG );
#define DOSFSRAMSEMREQUEST2_PARMBYTES ( sizeof( PFSRSEM216 ) + sizeof( LONG ) )
PCHAR szDos16FSRamSemRequest2 = "Dos16FSRamSemRequest2";

APIRET16 APIENTRY16 Trc_Dos16FSRamSemRequest2( PFSRSEM216 Parm1
                                             , LONG       Parm2
                                             )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSFSRAMSEMREQUEST2_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16FSRamSemRequest2, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PFSRSEM2 );
         T_LogPFSRSEM2( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16FSRamSemRequest2, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   if ( pfnDOSFSRAMSEMREQUEST2 )
   {
      usRC = ( *pfnDOSFSRAMSEMREQUEST2 )( Parm1, Parm2 );
   }
   else
   {
      if ( LOGDOSAPI( D_SEM ) )
      {
         T_WaitLog( );
         T_LogMessage( szACPMsg, szDos16FSRamSemRequest2 );
         T_FreeLog( );
      }

      usRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16FSRamSemRequest2 );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PFSRSEM2 );
         T_LogPFSRSEM2( Parm1 );
      }
      HOOKEXIT16( szDos16FSRamSemRequest2, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16FSRamSemRequest2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETCP( USHORT, PUSHORT16, PUSHORT16 );
#define DOSGETCP_PARMBYTES ( sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16GetCp = "Dos16GetCp";

APIRET16 APIENTRY16 Trc_Dos16GetCp( USHORT    Parm1
                                  , PUSHORT16 Parm2
                                  , PUSHORT16 Parm3
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETCP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetCp, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16GetCp, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETCP( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetCp );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
         if ( ( Parm3 ) && ( ( !usRC ) || ( usRC == ERROR_CPLIST_TOO_SMALL ) ) ) T_LogUSHORTBuffer16( Parm2, *Parm3 / sizeof( USHORT ) );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16GetCp, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetDateTime()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETDATETIME( PDATETIME16 );
#define DOSGETDATETIME_PARMBYTES ( sizeof( PDATETIME16 ) )
PCHAR szDos16GetDateTime = "Dos16GetDateTime";

APIRET16 APIENTRY16 Trc_Dos16GetDateTime( PDATETIME16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETDATETIME_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetDateTime, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDATETIME );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16GetDateTime, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETDATETIME( Parm1 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetDateTime );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDATETIME );
         if ( !usRC ) T_LogPDATETIME16( Parm1 );
         else T_LogPVOID16( Parm1 );
      }
      HOOKEXIT16( szDos16GetDateTime, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetDateTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GetDateTime()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32GETDATETIME( PDATETIME );
PCHAR szDos32GetDateTime = "Dos32GetDateTime";

APIRET APIENTRY Trc_Dos32GetDateTime( PDATETIME Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GetDateTime );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDATETIME );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32GetDateTime, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32GETDATETIME( Parm1 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GetDateTime );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDATETIME );
         if ( !ulRC ) T_LogPDATETIME( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32GetDateTime, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GetDateTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetEnv()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETENV( PSEL16, PUSHORT16 );
#define DOSGETENV_PARMBYTES ( sizeof( PSEL16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16GetEnv = "Dos16GetEnv";

APIRET16 APIENTRY16 Trc_Dos16GetEnv( PSEL16    Parm1
                                   , PUSHORT16 Parm2
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETENV_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetEnv, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSEL );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16GetEnv, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETENV( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetEnv );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSEL );
         T_LogPUSHORT16( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16GetEnv, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetEnv() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetHugeShift()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETHUGESHIFT( PUSHORT16 );
#define DOSGETHUGESHIFT_PARMBYTES ( sizeof( PUSHORT16 ) )
PCHAR szDos16GetHugeShift = "Dos16GetHugeShift";

APIRET16 APIENTRY16 Trc_Dos16GetHugeShift( PUSHORT16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETHUGESHIFT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetHugeShift, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16GetHugeShift, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETHUGESHIFT( Parm1 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetHugeShift );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPUSHORT16( Parm1 );
      }
      HOOKEXIT16( szDos16GetHugeShift, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetHugeShift() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GetInfoBlocks()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32GETINFOBLOCKS( PTIB *, PPIB * );
PCHAR szDos32GetInfoBlocks = "Dos32GetInfoBlocks";

APIRET APIENTRY Trc_Dos32GetInfoBlocks( PTIB * Parm1
                                      , PPIB * Parm2
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GetInfoBlocks );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PTIBSTAR );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PPIBSTAR );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32GetInfoBlocks, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32GETINFOBLOCKS( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GetInfoBlocks );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PTIBSTAR );
         T_LogPULONG( (PULONG)Parm1 );
         if ( ( Parm1 ) && ( !ulRC ) ) T_LogPTIB( *Parm1 );
         T_LogParameter( 2L, DT_PPIBSTAR );
         T_LogPULONG( (PULONG)Parm2 );
         if ( ( Parm2 ) && ( !ulRC ) ) T_LogPPIB( *Parm2 );
      }
      HOOKEXIT( szDos32GetInfoBlocks, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GetInfoBlocks() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetInfoSeg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETINFOSEG( PSEL16, PSEL16 );
#define DOSGETINFOSEG_PARMBYTES ( sizeof( PSEL16 ) + sizeof( PSEL16 ) )
PCHAR szDos16GetInfoSeg = "Dos16GetInfoSeg";

APIRET16 APIENTRY16 Trc_Dos16GetInfoSeg( PSEL16 Parm1
                                       , PSEL16 Parm2
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETINFOSEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_INFO ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetInfoSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSEL );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_PSEL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16GetInfoSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETINFOSEG( Parm1, Parm2 );

   if ( LOGDOSAPI( D_INFO ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetInfoSeg );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSEL );
         T_LogPUSHORT16( Parm1 );
         if ( !usRC ) T_LogPGINFOSEG( MAKEP( *Parm1, 0 ) );
         T_LogParameter( 2L, DT_PSEL );
         T_LogPUSHORT16( Parm2 );
         if ( !usRC ) T_LogPLINFOSEG( MAKEP( *Parm2, 0 ) );
      }
      HOOKEXIT16( szDos16GetInfoSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetInfoSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GetLocaleModule()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET APIENTRY DOS32GETLOCALEMODULE( PHMODULE, PSZ, PSZ );
PCHAR szDos32GetLocaleModule = "Dos32GetLocaleModule";

APIRET APIENTRY Trc_Dos32GetLocaleModule( PHMODULE Parm1
                                        , PSZ      Parm2
                                        , PSZ      Parm3
                                        )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GetLocaleModule );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHMODULE );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
      }
      HOOKENTRY( szDos32GetLocaleModule, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32GETLOCALEMODULE )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GetLocaleModule );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHMODULE );
         T_LogPULONG( Parm1 );
      }
      HOOKEXIT( szDos32GetLocaleModule, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GetLocaleModule() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetMachineMode()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETMACHINEMODE( PBYTE16 );
#define DOSGETMACHINEMODE_PARMBYTES ( sizeof( PBYTE16 ) )
PCHAR szDos16GetMachineMode = "Dos16GetMachineMode";

APIRET16 APIENTRY16 Trc_Dos16GetMachineMode( PBYTE16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETMACHINEMODE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetMachineMode, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PBYTE );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16GetMachineMode, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETMACHINEMODE( Parm1 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetMachineMode );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PBYTE );
         T_LogPCHAR16( (PCHAR16)Parm1 );
      }
      HOOKEXIT16( szDos16GetMachineMode, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetMachineMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetModHandle()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETMODHANDLE( PSZ16, PHMODULE16 );
#define DOSGETMODHANDLE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHMODULE16 ) )
PCHAR szDos16GetModHandle = "Dos16GetModHandle";

APIRET16 APIENTRY16 Trc_Dos16GetModHandle( PSZ16      Parm1
                                         , PHMODULE16 Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETMODHANDLE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetModHandle, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PHMODULE );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16GetModHandle, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETMODHANDLE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetModHandle );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHMODULE );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16GetModHandle, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetModHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetModName()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETMODNAME( HMODULE16, USHORT, PCHAR16 );
#define DOSGETMODNAME_PARMBYTES ( sizeof( HMODULE16 ) + sizeof( USHORT ) + sizeof( PCHAR16 ) )
PCHAR szDos16GetModName = "Dos16GetModName";

APIRET16 APIENTRY16 Trc_Dos16GetModName( HMODULE16 Parm1
                                       , USHORT    Parm2
                                       , PCHAR16   Parm3
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETMODNAME_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetModName, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PCHAR );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16GetModName, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETMODNAME( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetModName );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCHAR );
         T_LogPVOID16( Parm3 );
         if ( !usRC ) T_LogCHARBuffer16( Parm3, Parm2 );
      }
      HOOKEXIT16( szDos16GetModName, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetModName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GetNamedSharedMem()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32GETNAMEDSHAREDMEM( PPVOID, PSZ, ULONG );
PCHAR szDos32GetNamedSharedMem = "Dos32GetNamedSharedMem";

APIRET APIENTRY Trc_Dos32GetNamedSharedMem( PPVOID Parm1
                                          , PSZ    Parm2
                                          , ULONG  Parm3
                                          )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GetNamedSharedMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PPVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32GetNamedSharedMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32GETNAMEDSHAREDMEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GetNamedSharedMem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PPVOID );
         T_LogPULONG( (PULONG)Parm1 );
      }
      HOOKEXIT( szDos32GetNamedSharedMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GetNamedSharedMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetPID()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETPID( PPIDINFO16 );
#define DOSGETPID_PARMBYTES ( sizeof( PPIDINFO16 ) )
PCHAR szDos16GetPID = "Dos16GetPID";

APIRET16 APIENTRY16 Trc_Dos16GetPID( PPIDINFO16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETPID_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetPID, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PPIDINFO );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16GetPID, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETPID( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetPID );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PPIDINFO );
         T_LogPVOID16( Parm1 );
         if ( !usRC ) T_LogPPIDINFO( Parm1 );
      }
      HOOKEXIT16( szDos16GetPID, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetPID() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetPPID()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETPPID( USHORT, PUSHORT16 );
#define DOSGETPPID_PARMBYTES ( sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16GetPPID = "Dos16GetPPID";

APIRET16 APIENTRY16 Trc_Dos16GetPPID( USHORT    Parm1
                                    , PUSHORT16 Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETPPID_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetPPID, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16GetPPID, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETPPID( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetPPID );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16GetPPID, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetPPID() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetProcAddr()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETPROCADDR( HMODULE16, PSZ16, PPFN16 );
#define DOSGETPROCADDR_PARMBYTES ( sizeof( HMODULE16 ) + sizeof( PSZ16 ) + sizeof( PPFN16 ) )
PCHAR szDos16GetProcAddr = "Dos16GetProcAddr";

APIRET16 APIENTRY16 Trc_Dos16GetProcAddr( HMODULE16 Parm1
                                        , PSZ16     Parm2
                                        , PPFN16    Parm3
                                        )
{

   PFN16    pfnTrace;                            /* Trace API                 */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETPROCADDR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetProcAddr, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PFNFARSTAR );
         T_LogPVOID16( (PVOID16)Parm3 );
      }
      HOOKENTRY16( szDos16GetProcAddr, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETPROCADDR( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetProcAddr );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PFNFARSTAR );
         T_LogPPFN16( Parm3 );
      }
      if ( ( FLAGSET( T_TraceFlg, F_INTERCEPT )                                               ) &&
           ( usRC == NO_ERROR                                                                 ) &&
           ( DOSGETPROCADDR( T_DLLToTraceDLL( Parm1 ), Parm2, (PPFN16)&pfnTrace ) == NO_ERROR ) )
      {
         T_LogAPIInterception16( Parm1, Parm2 );
         *Parm3 = pfnTrace;
      }
      HOOKEXIT16( szDos16GetProcAddr, &Parm1, &usRC );
      T_FreeLog( );
   }

   else if ( FLAGSET( T_TraceFlg, F_INTERCEPT ) )
   {
      T_WaitLog( );
      if ( ( usRC == NO_ERROR                                                                 ) &&
           ( DOSGETPROCADDR( T_DLLToTraceDLL( Parm1 ), Parm2, (PPFN16)&pfnTrace ) == NO_ERROR ) )
      {
         T_LogAPIInterception16( Parm1, Parm2 );
         *Parm3 = pfnTrace;
      }
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetProcAddr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GetProcessorStatus()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32GETPROCESSORSTATUS( ULONG, PULONG );
PCHAR szDos32GetProcessorStatus = "Dos32GetProcessorStatus";

APIRET APIENTRY Trc_Dos32GetProcessorStatus( ULONG  Parm1
                                           , PULONG Parm2
                                           )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GetProcessorStatus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32GetProcessorStatus, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOS32GETPROCESSORSTATUS )
   {
      ulRC = ( *pfnDOS32GETPROCESSORSTATUS )( Parm1, Parm2 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos32GetProcessorStatus );
         T_FreeLog( );
      }

      ulRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GetProcessorStatus );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32GetProcessorStatus, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GetProcessorStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetPrty()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETPRTY( USHORT, PUSHORT16, USHORT );
#define DOSGETPRTY_PARMBYTES ( sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) )
PCHAR szDos16GetPrty = "Dos16GetPrty";

APIRET16 APIENTRY16 Trc_Dos16GetPrty( USHORT    Parm1
                                    , PUSHORT16 Parm2
                                    , USHORT    Parm3
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETPRTY_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetPrty, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16GetPrty, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETPRTY( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetPrty );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16GetPrty, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetPrty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetResource()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETRESOURCE( HMODULE16, USHORT, USHORT, PSEL16 );
#define DOSGETRESOURCE_PARMBYTES ( sizeof( HMODULE16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PSEL16 ) )
PCHAR szDos16GetResource = "Dos16GetResource";

APIRET16 APIENTRY16 Trc_Dos16GetResource( HMODULE16 Parm1
                                        , USHORT    Parm2
                                        , USHORT    Parm3
                                        , PSEL16    Parm4
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETRESOURCE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetResource, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSEL );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16GetResource, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETRESOURCE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetResource );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSEL );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16GetResource, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetResource() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetResource2()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETRESOURCE2( HMODULE16, USHORT, USHORT, PPVOID16 );
#define DOSGETRESOURCE2_PARMBYTES ( sizeof( HMODULE16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PPVOID16 ) )
PCHAR szDos16GetResource2 = "Dos16GetResource2";

APIRET16 APIENTRY16 Trc_Dos16GetResource2( HMODULE16 Parm1
                                         , USHORT    Parm2
                                         , USHORT    Parm3
                                         , PPVOID16  Parm4
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETRESOURCE2_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetResource2, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PVOIDFARSTAR );
         T_LogPVOID16( (PVOID16)Parm4 );
      }
      HOOKENTRY16( szDos16GetResource2, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETRESOURCE2( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetResource2 );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOIDFARSTAR );
         T_LogPULONG16( (PULONG16)Parm4 );
      }
      HOOKEXIT16( szDos16GetResource2, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetResource2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GetResource()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32GETRESOURCE( HMODULE, ULONG, ULONG, PPVOID );
PCHAR szDos32GetResource = "Dos32GetResource";

APIRET APIENTRY Trc_Dos32GetResource( HMODULE Parm1
                                    , ULONG   Parm2
                                    , ULONG   Parm3
                                    , PPVOID  Parm4
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GetResource );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPVOID );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32GetResource, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32GETRESOURCE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GetResource );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PPVOID );
         T_LogPULONG( (PULONG)Parm4 );
      }
      HOOKEXIT( szDos32GetResource, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GetResource() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetSeg()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETSEG( SEL );
#define DOSGETSEG_PARMBYTES ( sizeof( SEL ) )
PCHAR szDos16GetSeg = "Dos16GetSeg";

APIRET16 APIENTRY16 Trc_Dos16GetSeg( SEL Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETSEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16GetSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETSEG( Parm1 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetSeg );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16GetSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GetSharedMem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32GETSHAREDMEM( PVOID, ULONG );
PCHAR szDos32GetSharedMem = "Dos32GetSharedMem";

APIRET APIENTRY Trc_Dos32GetSharedMem( PVOID Parm1
                                     , ULONG Parm2
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GetSharedMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32GetSharedMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32GETSHAREDMEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GetSharedMem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32GetSharedMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GetSharedMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetShrSeg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETSHRSEG( PSZ16, PSEL16 );
#define DOSGETSHRSEG_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PSEL16 ) )
PCHAR szDos16GetShrSeg = "Dos16GetShrSeg";

APIRET16 APIENTRY16 Trc_Dos16GetShrSeg( PSZ16  Parm1
                                      , PSEL16 Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETSHRSEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetShrSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PSEL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16GetShrSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETSHRSEG( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetShrSeg );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSEL );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16GetShrSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetShrSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetVersion()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETVERSION( PUSHORT16 );
#define DOSGETVERSION_PARMBYTES ( sizeof( PUSHORT16 ) )
PCHAR szDos16GetVersion = "Dos16GetVersion";

APIRET16 APIENTRY16 Trc_Dos16GetVersion( PUSHORT16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETVERSION_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GetVersion, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16GetVersion, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGETVERSION( Parm1 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GetVersion );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPUSHORT16( Parm1 );
      }
      HOOKEXIT16( szDos16GetVersion, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetVersion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GiveSeg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGIVESEG( SEL, PID16, PSEL16 );
#define DOSGIVESEG_PARMBYTES ( sizeof( SEL ) + sizeof( PID16 ) + sizeof( PSEL16 ) )
PCHAR szDos16GiveSeg = "Dos16GiveSeg";

APIRET16 APIENTRY16 Trc_Dos16GiveSeg( SEL    Parm1
                                    , PID16  Parm2
                                    , PSEL16 Parm3
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGIVESEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16GiveSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PSEL );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16GiveSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSGIVESEG( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16GiveSeg );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSEL );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16GiveSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GiveSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GiveSharedMem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32GIVESHAREDMEM( PVOID, PID, ULONG );
PCHAR szDos32GiveSharedMem = "Dos32GiveSharedMem";

APIRET APIENTRY Trc_Dos32GiveSharedMem( PVOID Parm1
                                      , PID   Parm2
                                      , ULONG Parm3
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GiveSharedMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32GiveSharedMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32GIVESHAREDMEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GiveSharedMem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32GiveSharedMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GiveSharedMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16HoldSignal()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSHOLDSIGNAL( USHORT );
#define DOSHOLDSIGNAL_PARMBYTES ( sizeof( USHORT ) )
PCHAR szDos16HoldSignal = "Dos16HoldSignal";

APIRET16 APIENTRY16 Trc_Dos16HoldSignal( USHORT Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSHOLDSIGNAL_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SIG ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16HoldSignal, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16HoldSignal, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSHOLDSIGNAL( Parm1 );

   if ( LOGDOSAPI( D_SIG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16HoldSignal );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16HoldSignal, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16HoldSignal() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16KillProcess()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSKILLPROCESS( USHORT, PID16 );
#define DOSKILLPROCESS_PARMBYTES ( sizeof( USHORT ) + sizeof( PID16 ) )
PCHAR szDos16KillProcess = "Dos16KillProcess";

APIRET16 APIENTRY16 Trc_Dos16KillProcess( USHORT Parm1
                                        , PID16  Parm2
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSKILLPROCESS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16KillProcess, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szDos16KillProcess, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSKILLPROCESS( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16KillProcess );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16KillProcess, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16KillProcess() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32KillProcess()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32KILLPROCESS( ULONG, PID );
PCHAR szDos32KillProcess = "Dos32KillProcess";

APIRET APIENTRY Trc_Dos32KillProcess( ULONG Parm1
                                    , PID   Parm2
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32KillProcess );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32KillProcess, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32KILLPROCESS( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32KillProcess );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32KillProcess, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32KillProcess() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32KillThread()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32KILLTHREAD( TID );
PCHAR szDos32KillThread = "Dos32KillThread";

APIRET APIENTRY Trc_Dos32KillThread( TID Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32KillThread );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_TID );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32KillThread, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32KILLTHREAD( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32KillThread );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32KillThread, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32KillThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ListIO()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32LISTIO( ULONG, ULONG, PLISTIO );
PCHAR szDos32ListIO = "Dos32ListIO";

APIRET APIENTRY Trc_Dos32ListIO( ULONG   Parm1
                               , ULONG   Parm2
                               , PLISTIO Parm3
                               )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ListIO );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLISTIO );
         T_LogPLISTIO( Parm3, Parm2, Parm1, TRUE );
      }
      HOOKENTRY( szDos32ListIO, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32LISTIO )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ListIO );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLISTIO );
         T_LogPLISTIO( Parm3, Parm2, Parm1, FALSE );
      }
      HOOKEXIT( szDos32ListIO, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ListIO() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ListIOL()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32LISTIOL( ULONG, ULONG, VOID * );
PCHAR szDos32ListIOL = "Dos32ListIOL";

APIRET APIENTRY Trc_Dos32ListIOL( ULONG  Parm1
                                , ULONG  Parm2
                                , VOID  *Parm3
                                )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ListIOL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_VOIDSTAR );
         T_LogPLISTIOL( Parm3, Parm2, Parm1, TRUE );
      }
      HOOKENTRY( szDos32ListIOL, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32LISTIOL )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ListIOL );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_VOIDSTAR );
         T_LogPLISTIOL( Parm3, Parm2, Parm1, FALSE );
      }
      HOOKEXIT( szDos32ListIOL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ListIOL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16LoadModule()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSLOADMODULE( PSZ16, USHORT, PSZ16, PHMODULE16 );
#define DOSLOADMODULE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( PSZ16 ) + sizeof( PHMODULE16 ) )
PCHAR szDos16LoadModule = "Dos16LoadModule";

APIRET16 APIENTRY16 Trc_Dos16LoadModule( PSZ16      Parm1
                                       , USHORT     Parm2
                                       , PSZ16      Parm3
                                       , PHMODULE16 Parm4
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSLOADMODULE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16LoadModule, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_PHMODULE );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16LoadModule, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSLOADMODULE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16LoadModule );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPVOID16( Parm1 );
         if ( usRC ) T_LogCHARBuffer16( Parm1, Parm2 );
         T_LogParameter( 4L, DT_PHMODULE );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16LoadModule, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16LoadModule() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32LoadModule()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32LOADMODULE( PSZ, ULONG, PSZ, PHMODULE );
PCHAR szDos32LoadModule = "Dos32LoadModule";

APIRET APIENTRY Trc_Dos32LoadModule( PSZ      Parm1
                                   , ULONG    Parm2
                                   , PSZ      Parm3
                                   , PHMODULE Parm4
                                   )
{

   PSZ    pszDLL;                                /* Trace DLL                 */
   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32LoadModule );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PHMODULE );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32LoadModule, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32LOADMODULE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32LoadModule );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPVOID( Parm1 );
         if ( ulRC ) T_LogCHARBuffer( Parm1, Parm2 );
         T_LogParameter( 4L, DT_PHMODULE );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32LoadModule, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32LoadModule() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16LockSeg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSLOCKSEG( SEL );
#define DOSLOCKSEG_PARMBYTES ( sizeof( SEL ) )
PCHAR szDos16LockSeg = "Dos16LockSeg";

APIRET16 APIENTRY16 Trc_Dos16LockSeg( SEL Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSLOCKSEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16LockSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16LockSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSLOCKSEG( Parm1 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16LockSeg );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16LockSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16LockSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MakePipe()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMAKEPIPE( PHFILE16, PHFILE16, USHORT );
#define DOSMAKEPIPE_PARMBYTES ( sizeof( PHFILE16 ) + sizeof( PHFILE16 ) + sizeof( USHORT ) )
PCHAR szDos16MakePipe = "Dos16MakePipe";

APIRET16 APIENTRY16 Trc_Dos16MakePipe( PHFILE16 Parm1
                                     , PHFILE16 Parm2
                                     , USHORT   Parm3
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMAKEPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16MakePipe, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHFILE );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16MakePipe, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSMAKEPIPE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16MakePipe );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHFILE );
         T_LogPUSHORT16( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16MakePipe, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MakePipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MemAvail()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMEMAVAIL( PULONG16 );
#define DOSMEMAVAIL_PARMBYTES ( sizeof( PULONG16 ) )
PCHAR szDos16MemAvail = "Dos16MemAvail";

APIRET16 APIENTRY16 Trc_Dos16MemAvail( PULONG16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMEMAVAIL_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16MemAvail, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16MemAvail, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSMEMAVAIL( Parm1 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16MemAvail );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPULONG16( Parm1 );
      }
      HOOKEXIT16( szDos16MemAvail, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MemAvail() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MkDir()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMKDIR( PSZ16, ULONG );
#define DOSMKDIR_PARMBYTES ( sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szDos16MkDir = "Dos16MkDir";

APIRET16 APIENTRY16 Trc_Dos16MkDir( PSZ16 Parm1
                                  , ULONG Parm2
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMKDIR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16MkDir, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16MkDir, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSMKDIR( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16MkDir );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16MkDir, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MkDir() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MkDir2()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMKDIR2( PSZ16, PEAOP16, ULONG );
#define DOSMKDIR2_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PEAOP16 ) + sizeof( ULONG ) )
PCHAR szDos16MkDir2 = "Dos16MkDir2";

APIRET16 APIENTRY16 Trc_Dos16MkDir2( PSZ16   Parm1
                                   , PEAOP16 Parm2
                                   , ULONG   Parm3
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMKDIR2_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16MkDir2, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PEAOP );
         T_LogPVOID16( Parm2 );
         T_LogPEAOP( Parm2, PEAOP_FEA );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szDos16MkDir2, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSMKDIR2( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16MkDir2 );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PEAOP );
         T_LogPVOID16( Parm2 );
         if ( usRC ) T_LogPEAOP( Parm2, PEAOP_ERR );
      }
      HOOKEXIT16( szDos16MkDir2, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MkDir2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Move()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMOVE( PSZ16, PSZ16 );
#define DOSMOVE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PSZ16 ) )
PCHAR szDos16Move = "Dos16Move";

APIRET16 APIENTRY16 Trc_Dos16Move( PSZ16 Parm1
                                 , PSZ16 Parm2
                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMOVE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Move, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szDos16Move, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSMOVE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Move );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16Move, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Move() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Move()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32MOVE( PSZ, PSZ );
PCHAR szDos32Move = "Dos32Move";

APIRET APIENTRY Trc_Dos32Move( PSZ Parm1
                             , PSZ Parm2
                             )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Move );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szDos32Move, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32MOVE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Move );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32Move, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Move() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MuxSemWait()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMUXSEMWAIT( PUSHORT16, PVOID16, LONG );
#define DOSMUXSEMWAIT_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( PVOID16 ) + sizeof( LONG ) )
PCHAR szDos16MuxSemWait = "Dos16MuxSemWait";

APIRET16 APIENTRY16 Trc_Dos16MuxSemWait( PUSHORT16 Parm1
                                       , PVOID16   Parm2
                                       , LONG      Parm3
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMUXSEMWAIT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16MuxSemWait, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogPMUXSEMLIST( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szDos16MuxSemWait, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSMUXSEMWAIT( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16MuxSemWait );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPUSHORT16( Parm1 );
      }
      HOOKEXIT16( szDos16MuxSemWait, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MuxSemWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16NewSize()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSNEWSIZE( HFILE16, ULONG );
#define DOSNEWSIZE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( ULONG ) )
PCHAR szDos16NewSize = "Dos16NewSize";

APIRET16 APIENTRY16 Trc_Dos16NewSize( HFILE16 Parm1
                                    , ULONG   Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSNEWSIZE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16NewSize, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16NewSize, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSNEWSIZE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16NewSize );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16NewSize, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16NewSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Open()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSOPEN( PSZ16, PHFILE16, PUSHORT16, ULONG, USHORT, USHORT, USHORT, ULONG );
#define DOSOPEN_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHFILE16 ) + sizeof( PUSHORT16 ) + sizeof( ULONG ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16Open = "Dos16Open";

APIRET16 APIENTRY16 Trc_Dos16Open( PSZ16     Parm1
                                 , PHFILE16  Parm2
                                 , PUSHORT16 Parm3
                                 , ULONG     Parm4
                                 , USHORT    Parm5
                                 , USHORT    Parm6
                                 , USHORT    Parm7
                                 , ULONG     Parm8
                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSOPEN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Open, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_USHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
         T_LogParameter( 8L, DT_ULONG );
         T_LogULONG( Parm8 );
      }
      HOOKENTRY16( szDos16Open, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSOPEN( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Open );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16Open, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Open() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Open2()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSOPEN2( PSZ16, PHFILE16, PUSHORT16, ULONG, USHORT, USHORT, ULONG, PEAOP16, ULONG );
#define DOSOPEN2_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHFILE16 ) + sizeof( PUSHORT16 ) + sizeof( ULONG ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( ULONG ) + sizeof( PEAOP16 ) + sizeof( ULONG ) )
PCHAR szDos16Open2 = "Dos16Open2";

APIRET16 APIENTRY16 Trc_Dos16Open2( PSZ16     Parm1
                                  , PHFILE16  Parm2
                                  , PUSHORT16 Parm3
                                  , ULONG     Parm4
                                  , USHORT    Parm5
                                  , USHORT    Parm6
                                  , ULONG     Parm7
                                  , PEAOP16   Parm8
                                  , ULONG     Parm9
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSOPEN2_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Open2, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_USHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PEAOP );
         T_LogPVOID16( Parm8 );
         if ( Parm8 ) T_LogPEAOP( Parm8, PEAOP_FEA );
         T_LogParameter( 9L, DT_ULONG );
         T_LogULONG( Parm9 );
      }
      HOOKENTRY16( szDos16Open2, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSOPEN2( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Open2 );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
         T_LogParameter( 8L, DT_PEAOP );
         T_LogPVOID16( Parm8 );
         if ( ( usRC ) && ( Parm8 ) ) T_LogPEAOP( Parm8, PEAOP_ERR );
      }
      HOOKEXIT16( szDos16Open2, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Open2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Open()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32OPEN( PSZ, PHFILE, PULONG, ULONG, ULONG, ULONG, ULONG, PEAOP2 );
PCHAR szDos32Open = "Dos32Open";

APIRET APIENTRY Trc_Dos32Open( PSZ    Parm1
                             , PHFILE Parm2
                             , PULONG Parm3
                             , ULONG  Parm4
                             , ULONG  Parm5
                             , ULONG  Parm6
                             , ULONG  Parm7
                             , PEAOP2 Parm8
                             )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Open );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PEAOP2 );
         T_LogPVOID( Parm8 );
         if ( Parm8 ) T_LogPEAOP2( Parm8, PEAOP_FEA );
      }
      HOOKENTRY( szDos32Open, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32OPEN( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Open );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
         T_LogParameter( 8L, DT_PEAOP2 );
         T_LogPVOID( Parm8 );
         if ( ( ulRC ) && ( Parm8 ) ) T_LogPEAOP2( Parm8, PEAOP_ERR );
      }
      HOOKEXIT( szDos32Open, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Open() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32OpenL()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32OPENL( PSZ, PHFILE, PULONG, ULONG, ULONG, ULONG, ULONG, ULONG, PEAOP2 );
PCHAR szDos32OpenL = "Dos32OpenL";

APIRET APIENTRY Trc_Dos32OpenL( PSZ    Parm1
                              , PHFILE Parm2
                              , PULONG Parm3
                              , ULONG  Parm4Lo
                              , ULONG  Parm4Hi
                              , ULONG  Parm5
                              , ULONG  Parm6
                              , ULONG  Parm7
                              , PEAOP2 Parm8
                              )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32OpenL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_LONGLONG );
         T_LogLONGLONG( Parm4Hi, Parm4Lo );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PEAOP2 );
         T_LogPVOID( Parm8 );
         if ( Parm8 ) T_LogPEAOP2( Parm8, PEAOP_FEA );
      }
      HOOKENTRY( szDos32OpenL, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32OPENL )( Parm1, Parm2, Parm3, Parm4Lo, Parm4Hi, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32OpenL );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
         T_LogParameter( 8L, DT_PEAOP2 );
         T_LogPVOID( Parm8 );
         if ( ( ulRC ) && ( Parm8 ) ) T_LogPEAOP2( Parm8, PEAOP_ERR );
      }
      HOOKEXIT( szDos32OpenL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32OpenL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32OpenEventSem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32OPENEVENTSEM( PSZ, PHEV );
PCHAR szDos32OpenEventSem = "Dos32OpenEventSem";

APIRET APIENTRY Trc_Dos32OpenEventSem( PSZ  Parm1
                                     , PHEV Parm2
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32OpenEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHEV );
         T_LogPULONG( Parm2 );
      }
      HOOKENTRY( szDos32OpenEventSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32OPENEVENTSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32OpenEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHEV );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32OpenEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32OpenEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32OpenMutexSem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32OPENMUTEXSEM( PSZ, PHMTX );
PCHAR szDos32OpenMutexSem = "Dos32OpenMutexSem";

APIRET APIENTRY Trc_Dos32OpenMutexSem( PSZ   Parm1
                                     , PHMTX Parm2
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32OpenMutexSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHMTX );
         T_LogPULONG( Parm2 );
      }
      HOOKENTRY( szDos32OpenMutexSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32OPENMUTEXSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32OpenMutexSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHMTX );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32OpenMutexSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32OpenMutexSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32OpenMuxWaitSem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32OPENMUXWAITSEM( PSZ, PHMUX );
PCHAR szDos32OpenMuxWaitSem = "Dos32OpenMuxWaitSem";

APIRET APIENTRY Trc_Dos32OpenMuxWaitSem( PSZ   Parm1
                                       , PHMUX Parm2
                                       )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32OpenMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHMUX );
         T_LogPULONG( Parm2 );
      }
      HOOKENTRY( szDos32OpenMuxWaitSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32OPENMUXWAITSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32OpenMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHMUX );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32OpenMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32OpenMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32OpenRamSem()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET EXPENTRY DOS32OPENRAMSEM( PRAMSEM );
PCHAR szDos32OpenRamSem = "Dos32OpenRamSem";

APIRET EXPENTRY Trc_Dos32OpenRamSem( PRAMSEM Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32OpenRamSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         T_LogPRAMSEM( Parm1 );
      }
      HOOKENTRY( szDos32OpenRamSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32OPENRAMSEM )( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32OpenRamSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         if ( !ulRC ) T_LogPRAMSEM( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32OpenRamSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32OpenRamSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16OpenSem()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSOPENSEM( PHSEM16, PSZ16 );
#define DOSOPENSEM_PARMBYTES ( sizeof( PHSEM16 ) + sizeof( PSZ16 ) )
PCHAR szDos16OpenSem = "Dos16OpenSem";

APIRET16 APIENTRY16 Trc_Dos16OpenSem( PHSEM16 Parm1
                                    , PSZ16   Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSOPENSEM_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16OpenSem, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHSEM );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szDos16OpenSem, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSOPENSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16OpenSem );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHSEM );
         T_LogPULONG16( Parm1 );
      }
      HOOKEXIT16( szDos16OpenSem, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16OpenSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16OpenVDD()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSOPENVDD( PSZ16, PHVDD16 );
#define DOSOPENVDD_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHVDD16 ) )
PCHAR szDos16OpenVDD = "Dos16OpenVDD";

APIRET16 APIENTRY16 Trc_Dos16OpenVDD( PSZ16   Parm1
                                    , PHVDD16 Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSOPENVDD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16OpenVDD, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PHVDD );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16OpenVDD, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSOPENVDD( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16OpenVDD );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHVDD );
         T_LogPULONG16( Parm2 );
      }
      HOOKEXIT16( szDos16OpenVDD, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16OpenVDD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32OpenVDD()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32OPENVDD( PSZ, PHVDD );
PCHAR szDos32OpenVDD = "Dos32OpenVDD";

APIRET APIENTRY Trc_Dos32OpenVDD( PSZ   Parm1
                                , PHVDD Parm2
                                )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32OpenVDD );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHVDD );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32OpenVDD, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32OPENVDD( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32OpenVDD );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHVDD );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32OpenVDD, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32OpenVDD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PeekNPipe()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32PEEKNPIPE( HPIPE, PVOID, ULONG, PULONG, PAVAILDATA, PULONG );
PCHAR szDos32PeekNPipe = "Dos32PeekNPipe";

APIRET APIENTRY Trc_Dos32PeekNPipe( HPIPE      Parm1
                                  , PVOID      Parm2
                                  , ULONG      Parm3
                                  , PULONG     Parm4
                                  , PAVAILDATA Parm5
                                  , PULONG     Parm6
                                  )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PeekNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPIPE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PAVAILDATA );
         T_LogPVOID( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szDos32PeekNPipe, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32PEEKNPIPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32PeekNPipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( ( Parm4 ) && ( !ulRC ) ) T_LogCHARBuffer( Parm2, *Parm4 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
         T_LogParameter( 5L, DT_PAVAILDATA );
         if ( !ulRC ) T_LogPAVAILDATA( Parm5 );
         else T_LogPVOID( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      HOOKEXIT( szDos32PeekNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PeekNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PerfSysCall()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
APIRET APIENTRY DOS32PERFSYSCALL( ULONG, ULONG, ULONG, ULONG );
PCHAR szDos32PerfSysCall = "Dos32PerfSysCall";

APIRET APIENTRY Trc_Dos32PerfSysCall( ULONG Parm1
                                    , ULONG Parm2
                                    , ULONG Parm3
                                    , ULONG Parm4
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PerfSysCall );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32PerfSysCall, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PERFSYSCALL )( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32PerfSysCall );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32PerfSysCall, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PerfSysCall() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PhysicalDisk()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSPHYSICALDISK( USHORT, PVOID16, USHORT, PVOID16, USHORT );
#define DOSPHYSICALDISK_PARMBYTES ( sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) )
PCHAR szDos16PhysicalDisk = "Dos16PhysicalDisk";

APIRET16 APIENTRY16 Trc_Dos16PhysicalDisk( USHORT  Parm1
                                         , PVOID16 Parm2
                                         , USHORT  Parm3
                                         , PVOID16 Parm4
                                         , USHORT  Parm5
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPHYSICALDISK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16PhysicalDisk, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( Parm4, Parm5 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szDos16PhysicalDisk, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSPHYSICALDISK( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16PhysicalDisk );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         if ( !usRC ) T_LogCHARBuffer16( Parm2, Parm3 );
      }
      HOOKEXIT16( szDos16PhysicalDisk, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16PhysicalDisk() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PhysicalDisk()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32PHYSICALDISK( ULONG, PVOID, ULONG, PVOID, ULONG );
PCHAR szDos32PhysicalDisk = "Dos32PhysicalDisk";

APIRET APIENTRY Trc_Dos32PhysicalDisk( ULONG Parm1
                                     , PVOID Parm2
                                     , ULONG Parm3
                                     , PVOID Parm4
                                     , ULONG Parm5
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PhysicalDisk );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogCHARBuffer( Parm4, Parm5 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32PhysicalDisk, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32PHYSICALDISK( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32PhysicalDisk );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( !ulRC ) T_LogCHARBuffer( Parm2, Parm3 );
      }
      HOOKEXIT( szDos32PhysicalDisk, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PhysicalDisk() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PMPostEventSem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
APIRET APIENTRY DOS32PMPOSTEVENTSEM( HEV );
PCHAR szDos32PMPostEventSem = "Dos32PMPostEventSem";

APIRET APIENTRY Trc_Dos32PMPostEventSem( HEV Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PMPostEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32PMPostEventSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PMPOSTEVENTSEM )( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32PMPostEventSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32PMPostEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PMPostEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PMR3PostEventSem()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32PMR3POSTEVENTSEM( GRESEMT * );
PCHAR szDos32PMR3PostEventSem = "Dos32PMR3PostEventSem";

APIRET APIENTRY Trc_Dos32PMR3PostEventSem( GRESEMT *Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PMR3PostEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_GRESEMTSTAR );
         T_LogPGRESEMT( Parm1 );
      }
      HOOKENTRY( szDos32PMR3PostEventSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PMR3POSTEVENTSEM )( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32PMR3PostEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_GRESEMTSTAR );
         T_LogPGRESEMT( Parm1 );
      }
      HOOKEXIT( szDos32PMR3PostEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PMR3PostEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PMR3WaitEventSem()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32PMR3WAITEVENTSEM( GRESEMT *, ULONG, ULONG );
PCHAR szDos32PMR3WaitEventSem = "Dos32PMR3WaitEventSem";

APIRET APIENTRY Trc_Dos32PMR3WaitEventSem( GRESEMT *Parm1
                                         , ULONG    Parm2
                                         , ULONG    Parm3
                                         )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PMR3WaitEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_GRESEMTSTAR );
         T_LogPGRESEMT( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32PMR3WaitEventSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = ( *pfnDOS32PMR3WAITEVENTSEM )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32PMR3WaitEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_GRESEMTSTAR );
         T_LogPGRESEMT( Parm1 );
      }
      HOOKEXIT( szDos32PMR3WaitEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PMR3WaitEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PMRequestMutexSem()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
APIRET APIENTRY DOS32PMREQUESTMUTEXSEM( HEV, HMTX, ULONG );
PCHAR szDos32PMRequestMutexSem = "Dos32PMRequestMutexSem";

APIRET APIENTRY Trc_Dos32PMRequestMutexSem( HEV   Parm1
                                          , HMTX  Parm2
                                          , ULONG Parm3
                                          )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PMRequestMutexSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMTX );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32PMRequestMutexSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = ( *pfnDOS32PMREQUESTMUTEXSEM )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32PMRequestMutexSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32PMRequestMutexSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PMRequestMutexSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PMWaitEventSem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
APIRET APIENTRY DOS32PMWAITEVENTSEM( HEV, HEV, ULONG );
PCHAR szDos32PMWaitEventSem = "Dos32PMWaitEventSem";

APIRET APIENTRY Trc_Dos32PMWaitEventSem( HEV   Parm1
                                       , HEV   Parm2
                                       , ULONG Parm3
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PMWaitEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HEV );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32PMWaitEventSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = ( *pfnDOS32PMWAITEVENTSEM )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32PMWaitEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32PMWaitEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PMWaitEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PMWaitMuxWaitSem()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
APIRET APIENTRY DOS32PMWAITMUXWAITSEM( HEV, HMUX, ULONG, PULONG );
PCHAR szDos32PMWaitMuxWaitSem = "Dos32PMWaitMuxWaitSem";

APIRET APIENTRY Trc_Dos32PMWaitMuxWaitSem( HEV    Parm1
                                         , HMUX   Parm2
                                         , ULONG  Parm3
                                         , PULONG Parm4
                                         )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PMWaitMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMUX );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32PMWaitMuxWaitSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = ( *pfnDOS32PMWAITMUXWAITSEM )( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32PMWaitMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32PMWaitMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PMWaitMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PortAccess()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSPORTACCESS( USHORT, USHORT, USHORT, USHORT );
#define DOSPORTACCESS_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16PortAccess = "Dos16PortAccess";

APIRET16 APIENTRY16 Trc_Dos16PortAccess( USHORT Parm1
                                       , USHORT Parm2
                                       , USHORT Parm3
                                       , USHORT Parm4
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPORTACCESS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16PortAccess, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szDos16PortAccess, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSPORTACCESS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16PortAccess );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16PortAccess, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16PortAccess() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PostEventSem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32POSTEVENTSEM( HEV );
PCHAR szDos32PostEventSem = "Dos32PostEventSem";

APIRET APIENTRY Trc_Dos32PostEventSem( HEV Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32PostEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32PostEventSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32POSTEVENTSEM( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32PostEventSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32PostEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32PostEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Profile()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
#define PROF_CREATE   0
#define PROF_START    1
#define PROF_STOP     2
#undef  PROF_CLEAR
#define PROF_CLEAR    3
#define PROF_DUMP     4
#define PROF_DESTROY  5
#define PROF_INT_TEST 6
APIRET APIENTRY DOS32PROFILE( ULONG, PID, PIR *, PDR * );
PCHAR szDos32Profile = "Dos32Profile";

APIRET APIENTRY Trc_Dos32Profile( ULONG Parm1
                                , PID   Parm2
                                , PIR  *Parm3
                                , PDR  *Parm4
                                )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Profile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PIRSTAR );
         T_LogPVOID( Parm3 );
         if ( Parm1 == PROF_CREATE ) T_LogPPIR( Parm3 );
         T_LogParameter( 4L, DT_PDRSTAR );
         T_LogPVOID( Parm4 );
         if ( Parm1 == PROF_DUMP ) T_LogPPDR( Parm4, TRUE );
      }
      HOOKENTRY( szDos32Profile, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32PROFILE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_PROF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Profile );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PDRSTAR );
         T_LogPVOID( Parm4 );
         if ( ( !ulRC ) && ( Parm1 == PROF_DUMP ) && ( Parm4 ) ) T_LogPPDR( Parm4, FALSE );
      }
      HOOKEXIT( szDos32Profile, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Profile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectChgFilePtr()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTCHGFILEPTR( HFILE16, LONG, USHORT, PULONG16, FHLOCK );
#define DOSPROTECTCHGFILEPTR_PARMBYTES ( sizeof( HFILE16 ) + sizeof( LONG ) + sizeof( USHORT ) + sizeof( PULONG16 ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectChgFilePtr = "Dos16ProtectChgFilePtr";

APIRET16 APIENTRY16 Trc_Dos16ProtectChgFilePtr( HFILE16  Parm1
                                              , LONG     Parm2
                                              , USHORT   Parm3
                                              , PULONG16 Parm4
                                              , FHLOCK   Parm5
                                              )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTCHGFILEPTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectChgFilePtr, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szDos16ProtectChgFilePtr, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTCHGFILEPTR )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectChgFilePtr );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG16( Parm4 );
      }
      HOOKEXIT16( szDos16ProtectChgFilePtr, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectChgFilePtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectClose()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTCLOSE( HFILE16, FHLOCK );
#define DOSPROTECTCLOSE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectClose = "Dos16ProtectClose";

APIRET16 APIENTRY16 Trc_Dos16ProtectClose( HFILE16 Parm1
                                         , FHLOCK  Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTCLOSE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectClose, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_FHLOCK );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16ProtectClose, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTCLOSE )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectClose );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ProtectClose, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectClose()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTCLOSE( HFILE, FHLOCK );
PCHAR szDos32ProtectClose = "Dos32ProtectClose";

APIRET APIENTRY Trc_Dos32ProtectClose( HFILE  Parm1
                                     , FHLOCK Parm2
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectClose );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_FHLOCK );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32ProtectClose, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTCLOSE )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectClose );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ProtectClose, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectEnumAttribute()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTENUMATTRIBUTE( USHORT, PVOID16, ULONG, PVOID16, ULONG, PULONG16, ULONG, FHLOCK, ULONG );
#define DOSPROTECTENUMATTRIBUTE_PARMBYTES ( sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( ULONG ) + sizeof( PVOID16 ) + sizeof( ULONG ) + sizeof( PULONG16 ) + sizeof( ULONG ) + sizeof( FHLOCK ) + sizeof( ULONG ) )
PCHAR szDos16ProtectEnumAttribute = "Dos16ProtectEnumAttribute";

APIRET16 APIENTRY16 Trc_Dos16ProtectEnumAttribute( USHORT   Parm1
                                                 , PVOID16  Parm2
                                                 , ULONG    Parm3
                                                 , PVOID16  Parm4
                                                 , ULONG    Parm5
                                                 , PULONG16 Parm6
                                                 , ULONG    Parm7
                                                 , FHLOCK   Parm8
                                                 , ULONG    Parm9
                                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTENUMATTRIBUTE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectEnumAttribute, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         if ( Parm1 == ENUMEA_REFTYPE_FHANDLE ) T_LogPUSHORT16( Parm2 );
         else if ( Parm1 == ENUMEA_REFTYPE_PATH ) T_LogPSZ16( Parm2 );
         else T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG16( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_FHLOCK );
         T_LogULONG( Parm8 );
         T_LogParameter( 9L, DT_ULONG );
         T_LogULONG( Parm9 );
      }
      HOOKENTRY16( szDos16ProtectEnumAttribute, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTENUMATTRIBUTE )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectEnumAttribute );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogDENA1Buffer( Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG16( Parm6 );
      }
      HOOKEXIT16( szDos16ProtectEnumAttribute, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectEnumAttribute() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectEnumAttribute()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTENUMATTRIBUTE( ULONG, PVOID, ULONG, PVOID, ULONG, PULONG, ULONG, FHLOCK );
PCHAR szDos32ProtectEnumAttribute = "Dos32ProtectEnumAttribute";

APIRET APIENTRY Trc_Dos32ProtectEnumAttribute( ULONG  Parm1
                                             , PVOID  Parm2
                                             , ULONG  Parm3
                                             , PVOID  Parm4
                                             , ULONG  Parm5
                                             , PULONG Parm6
                                             , ULONG  Parm7
                                             , FHLOCK Parm8
                                             )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectEnumAttribute );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         if ( Parm1 == ENUMEA_REFTYPE_FHANDLE ) T_LogPULONG( (PULONG)Parm2 );
         else if ( Parm1 == ENUMEA_REFTYPE_PATH ) T_LogPSZ( (PSZ)Parm2 );
         else T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_FHLOCK );
         T_LogULONG( Parm8 );
      }
      HOOKENTRY( szDos32ProtectEnumAttribute, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTENUMATTRIBUTE )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectEnumAttribute );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( ( Parm6 ) && ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogDENA2Buffer( (PDENA2)Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      HOOKEXIT( szDos32ProtectEnumAttribute, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectEnumAttribute() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectFileIO()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTFILEIO( HFILE16, PVOID16, USHORT, PUSHORT16, FHLOCK );
#define DOSPROTECTFILEIO_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectFileIO = "Dos16ProtectFileIO";

APIRET16 APIENTRY16 Trc_Dos16ProtectFileIO( HFILE16   Parm1
                                          , PVOID16   Parm2
                                          , USHORT    Parm3
                                          , PUSHORT16 Parm4
                                          , FHLOCK    Parm5
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTFILEIO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectFileIO, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogD16FIO_Parm2( Parm2, Parm3 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szDos16ProtectFileIO, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTFILEIO )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectFileIO );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16ProtectFileIO, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectFileIO() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectFileLocks()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTFILELOCKS( HFILE16, PFILELOCK16, PFILELOCK16, FHLOCK );
#define DOSPROTECTFILELOCKS_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PFILELOCK16 ) + sizeof( PFILELOCK16 ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectFileLocks = "Dos16ProtectFileLocks";

APIRET16 APIENTRY16 Trc_Dos16ProtectFileLocks( HFILE16     Parm1
                                             , PFILELOCK16 Parm2
                                             , PFILELOCK16 Parm3
                                             , FHLOCK      Parm4
                                             )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTFILELOCKS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectFileLocks, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PFILELOCK );
         T_LogPFILELOCK16( Parm2 );
         T_LogParameter( 3L, DT_PFILELOCK );
         T_LogPFILELOCK16( Parm3 );
         T_LogParameter( 4L, DT_FHLOCK );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szDos16ProtectFileLocks, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTFILELOCKS )( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectFileLocks );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ProtectFileLocks, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectFileLocks() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectNewSize()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTNEWSIZE( HFILE16, ULONG, FHLOCK );
#define DOSPROTECTNEWSIZE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( ULONG ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectNewSize = "Dos16ProtectNewSize";

APIRET16 APIENTRY16 Trc_Dos16ProtectNewSize( HFILE16 Parm1
                                           , ULONG   Parm2
                                           , FHLOCK  Parm3
                                           )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTNEWSIZE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectNewSize, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_FHLOCK );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szDos16ProtectNewSize, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTNEWSIZE )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectNewSize );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ProtectNewSize, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectNewSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectOpen()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTOPEN( PSZ16, PHFILE16, PUSHORT16, ULONG, USHORT, USHORT, ULONG, PEAOP16, PFHLOCK16, ULONG );
#define DOSPROTECTOPEN_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHFILE16 ) + sizeof( PUSHORT16 ) + sizeof( ULONG ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( ULONG ) + sizeof( PEAOP16 ) + sizeof( PFHLOCK16 ) + sizeof( ULONG ) )
PCHAR szDos16ProtectOpen = "Dos16ProtectOpen";

APIRET16 APIENTRY16 Trc_Dos16ProtectOpen( PSZ16     Parm1
                                        , PHFILE16  Parm2
                                        , PUSHORT16 Parm3
                                        , ULONG     Parm4
                                        , USHORT    Parm5
                                        , USHORT    Parm6
                                        , ULONG     Parm7
                                        , PEAOP16   Parm8
                                        , PFHLOCK16 Parm9
                                        , ULONG     Parm10
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTOPEN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectOpen, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_USHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PEAOP );
         T_LogPVOID16( Parm8 );
         if ( Parm8 ) T_LogPEAOP( Parm8, PEAOP_FEA );
         T_LogParameter( 9L, DT_PFHLOCK );
         T_LogPVOID16( Parm9 );
         T_LogParameter( 10L, DT_ULONG );
         T_LogULONG( Parm10 );
      }
      HOOKENTRY16( szDos16ProtectOpen, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTOPEN )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectOpen );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
         T_LogParameter( 8L, DT_PEAOP );
         T_LogPVOID16( Parm8 );
         if ( ( usRC ) && ( Parm8 ) ) T_LogPEAOP( Parm8, PEAOP_ERR );
         T_LogParameter( 9L, DT_PFHLOCK );
         T_LogPULONG16( Parm9 );
      }
      HOOKEXIT16( szDos16ProtectOpen, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectOpen()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTOPEN( PSZ, PHFILE, PULONG, ULONG, ULONG, ULONG, ULONG, PEAOP2, PFHLOCK );
PCHAR szDos32ProtectOpen = "Dos32ProtectOpen";

APIRET APIENTRY Trc_Dos32ProtectOpen( PSZ     Parm1
                                    , PHFILE  Parm2
                                    , PULONG  Parm3
                                    , ULONG   Parm4
                                    , ULONG   Parm5
                                    , ULONG   Parm6
                                    , ULONG   Parm7
                                    , PEAOP2  Parm8
                                    , PFHLOCK Parm9
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectOpen );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PEAOP2 );
         T_LogPVOID( Parm8 );
         if ( Parm8 ) T_LogPEAOP2( Parm8, PEAOP_FEA );
         T_LogParameter( 9L, DT_PFHLOCK );
         T_LogPULONG( Parm9 );
      }
      HOOKENTRY( szDos32ProtectOpen, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTOPEN )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectOpen );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
         T_LogParameter( 8L, DT_PEAOP2 );
         T_LogPVOID( Parm8 );
         if ( ( ulRC ) && ( Parm8 ) ) T_LogPEAOP2( Parm8, PEAOP_ERR );
         T_LogParameter( 9L, DT_PFHLOCK );
         T_LogPULONG( Parm9 );
      }
      HOOKEXIT( szDos32ProtectOpen, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectOpenL()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32PROTECTOPENL( PSZ, PHFILE, PULONG, ULONG, ULONG, ULONG, ULONG, ULONG, PEAOP2, PFHLOCK );
PCHAR szDos32ProtectOpenL = "Dos32ProtectOpenL";

APIRET APIENTRY Trc_Dos32ProtectOpenL( PSZ     Parm1
                                     , PHFILE  Parm2
                                     , PULONG  Parm3
                                     , ULONG   Parm4Lo
                                     , ULONG   Parm4Hi
                                     , ULONG   Parm5
                                     , ULONG   Parm6
                                     , ULONG   Parm7
                                     , PEAOP2  Parm8
                                     , PFHLOCK Parm9
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectOpenL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_LONGLONG );
         T_LogLONGLONG( Parm4Hi, Parm4Lo );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PEAOP2 );
         T_LogPVOID( Parm8 );
         if ( Parm8 ) T_LogPEAOP2( Parm8, PEAOP_FEA );
         T_LogParameter( 9L, DT_PFHLOCK );
         T_LogPULONG( Parm9 );
      }
      HOOKENTRY( szDos32ProtectOpenL, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTOPENL )( Parm1, Parm2, Parm3, Parm4Lo, Parm4Hi, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectOpenL );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHFILE );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
         T_LogParameter( 8L, DT_PEAOP2 );
         T_LogPVOID( Parm8 );
         if ( ( ulRC ) && ( Parm8 ) ) T_LogPEAOP2( Parm8, PEAOP_ERR );
         T_LogParameter( 9L, DT_PFHLOCK );
         T_LogPULONG( Parm9 );
      }
      HOOKEXIT( szDos32ProtectOpenL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectOpenL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectQFHandState()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTQFHANDSTATE( HFILE16, PUSHORT16, FHLOCK );
#define DOSPROTECTQFHANDSTATE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PUSHORT16 ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectQFHandState = "Dos16ProtectQFHandState";

APIRET16 APIENTRY16 Trc_Dos16ProtectQFHandState( HFILE16   Parm1
                                               , PUSHORT16 Parm2
                                               , FHLOCK    Parm3
                                               )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTQFHANDSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectQFHandState, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_FHLOCK );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szDos16ProtectQFHandState, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTQFHANDSTATE )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectQFHandState );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16ProtectQFHandState, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectQFHandState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectQFileInfo()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTQFILEINFO( HFILE16, USHORT, PVOID16, USHORT, FHLOCK );
#define DOSPROTECTQFILEINFO_PARMBYTES ( sizeof( HFILE16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectQFileInfo = "Dos16ProtectQFileInfo";

APIRET16 APIENTRY16 Trc_Dos16ProtectQFileInfo( HFILE16 Parm1
                                             , USHORT  Parm2
                                             , PVOID16 Parm3
                                             , USHORT  Parm4
                                             , FHLOCK  Parm5
                                             )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTQFILEINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectQFileInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( Parm2 == FIL_QUERYEASFROMLIST ) T_LogPEAOP( Parm3, PEAOP_GEA );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szDos16ProtectQFileInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTQFILEINFO )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectQFileInfo );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD16QFI_Parm3( Parm3, Parm2 );
      }
      HOOKEXIT16( szDos16ProtectQFileInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectQFileInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectQueryFHState()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTQUERYFHSTATE( HFILE, PULONG, FHLOCK );
PCHAR szDos32ProtectQueryFHState = "Dos32ProtectQueryFHState";

APIRET APIENTRY Trc_Dos32ProtectQueryFHState( HFILE  Parm1
                                            , PULONG Parm2
                                            , FHLOCK Parm3
                                            )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectQueryFHState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_FHLOCK );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32ProtectQueryFHState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTQUERYFHSTATE )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectQueryFHState );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32ProtectQueryFHState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectQueryFHState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectQueryFileInfo()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTQUERYFILEINFO( HFILE, ULONG, PVOID, ULONG, FHLOCK );
PCHAR szDos32ProtectQueryFileInfo = "Dos32ProtectQueryFileInfo";

APIRET APIENTRY Trc_Dos32ProtectQueryFileInfo( HFILE  Parm1
                                             , ULONG  Parm2
                                             , PVOID  Parm3
                                             , ULONG  Parm4
                                             , FHLOCK Parm5
                                             )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectQueryFileInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( Parm2 == FIL_QUERYEASFROMLIST ) T_LogPEAOP2( Parm3, PEAOP_GEA );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32ProtectQueryFileInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTQUERYFILEINFO )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectQueryFileInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD32QFI_Parm3( Parm3, Parm2 );
      }
      HOOKEXIT( szDos32ProtectQueryFileInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectQueryFileInfo() */
#endif

#if FALSE /* OS/2 BUG 192126 - Dos16ProtectRead does not have ordinal         */
/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectRead()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTREAD( HFILE16, PVOID16, USHORT, PUSHORT16, FHLOCK );
#define DOSPROTECTREAD_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectRead = "Dos16ProtectRead";

APIRET16 APIENTRY16 Trc_Dos16ProtectRead( HFILE16   Parm1
                                        , PVOID16   Parm2
                                        , USHORT    Parm3
                                        , PUSHORT16 Parm4
                                        , FHLOCK    Parm5
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTREAD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectRead, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szDos16ProtectRead, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTREAD )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectRead );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         if ( ( Parm4 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer16( Parm2, *Parm4 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16ProtectRead, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectRead() */
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectRead()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTREAD( HFILE, PVOID, ULONG, PULONG, FHLOCK );
PCHAR szDos32ProtectRead = "Dos32ProtectRead";

APIRET APIENTRY Trc_Dos32ProtectRead( HFILE  Parm1
                                    , PVOID  Parm2
                                    , ULONG  Parm3
                                    , PULONG Parm4
                                    , FHLOCK Parm5
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectRead );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32ProtectRead, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTREAD )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectRead );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( ( Parm4 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer( Parm2, *Parm4 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32ProtectRead, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectRead() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectSetFHandState()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTSETFHANDSTATE( HFILE16, USHORT, FHLOCK );
#define DOSPROTECTSETFHANDSTATE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( USHORT ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectSetFHandState = "Dos16ProtectSetFHandState";

APIRET16 APIENTRY16 Trc_Dos16ProtectSetFHandState( HFILE16 Parm1
                                                 , USHORT Parm2
                                                 , FHLOCK Parm3
                                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTSETFHANDSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectSetFHandState, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_FHLOCK );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szDos16ProtectSetFHandState, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTSETFHANDSTATE )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectSetFHandState );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ProtectSetFHandState, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectSetFHandState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectSetFHState()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTSETFHSTATE( HFILE, ULONG, FHLOCK );
PCHAR szDos32ProtectSetFHState = "Dos32ProtectSetFHState";

APIRET APIENTRY Trc_Dos32ProtectSetFHState( HFILE  Parm1
                                          , ULONG  Parm2
                                          , FHLOCK Parm3
                                          )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectSetFHState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_FHLOCK );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32ProtectSetFHState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTSETFHSTATE )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectSetFHState );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ProtectSetFHState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectSetFHState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectSetFileInfo()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTSETFILEINFO( HFILE16, USHORT, PVOID16, USHORT, FHLOCK );
#define DOSPROTECTSETFILEINFO_PARMBYTES ( sizeof( HFILE16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectSetFileInfo = "Dos16ProtectSetFileInfo";

APIRET16 APIENTRY16 Trc_Dos16ProtectSetFileInfo( HFILE16 Parm1
                                               , USHORT  Parm2
                                               , PVOID16 Parm3
                                               , USHORT  Parm4
                                               , FHLOCK  Parm5
                                               )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTSETFILEINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectSetFileInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogD16SFI_Parm3( Parm3, Parm2 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szDos16ProtectSetFileInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTSETFILEINFO )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectSetFileInfo );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( ( usRC ) && ( Parm2 == FIL_QUERYEASIZE ) ) T_LogPEAOP( Parm3, PEAOP_ERR );
      }
      HOOKEXIT16( szDos16ProtectSetFileInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectSetFileInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectSetFileInfo()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTSETFILEINFO( HFILE, ULONG, PVOID, ULONG, FHLOCK );
PCHAR szDos32ProtectSetFileInfo = "Dos32ProtectSetFileInfo";

APIRET APIENTRY Trc_Dos32ProtectSetFileInfo( HFILE  Parm1
                                           , ULONG  Parm2
                                           , PVOID  Parm3
                                           , ULONG  Parm4
                                           , FHLOCK Parm5
                                           )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectSetFileInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogD32SFI_Parm3( Parm3, Parm2 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32ProtectSetFileInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTSETFILEINFO )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectSetFileInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( ( ulRC ) && ( Parm2 == FIL_QUERYEASIZE ) ) T_LogPEAOP2( Parm3, PEAOP_ERR );
      }
      HOOKEXIT( szDos32ProtectSetFileInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectSetFileInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectSetFileLocks()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTSETFILELOCKS( HFILE, PFILELOCK, PFILELOCK, ULONG, ULONG, FHLOCK );
PCHAR szDos32ProtectSetFileLocks = "Dos32ProtectSetFileLocks";

APIRET APIENTRY Trc_Dos32ProtectSetFileLocks( HFILE     Parm1
                                            , PFILELOCK Parm2
                                            , PFILELOCK Parm3
                                            , ULONG     Parm4
                                            , ULONG     Parm5
                                            , FHLOCK    Parm6
                                            )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectSetFileLocks );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFILELOCK );
         T_LogPFILELOCK( Parm2 );
         T_LogParameter( 3L, DT_PFILELOCK );
         T_LogPFILELOCK( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_FHLOCK );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szDos32ProtectSetFileLocks, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = ( *pfnDOS32PROTECTSETFILELOCKS )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectSetFileLocks );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32ProtectSetFileLocks, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectSetFileLocks() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectSetFileLocksL()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32PROTECTSETFILELOCKSL( HFILE, PFILELOCKL, PFILELOCKL, ULONG, ULONG, FHLOCK );
PCHAR szDos32ProtectSetFileLocksL = "Dos32ProtectSetFileLocksL";

APIRET APIENTRY Trc_Dos32ProtectSetFileLocksL( HFILE      Parm1
                                             , PFILELOCKL Parm2
                                             , PFILELOCKL Parm3
                                             , ULONG      Parm4
                                             , ULONG      Parm5
                                             , FHLOCK     Parm6
                                             )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectSetFileLocksL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFILELOCKL );
         T_LogPFILELOCKL( Parm2 );
         T_LogParameter( 3L, DT_PFILELOCKL );
         T_LogPFILELOCKL( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_FHLOCK );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szDos32ProtectSetFileLocksL, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = ( *pfnDOS32PROTECTSETFILELOCKSL )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectSetFileLocksL );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32ProtectSetFileLocksL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectSetFileLocksL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectSetFilePtr()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTSETFILEPTR( HFILE, LONG, ULONG, PULONG, FHLOCK );
PCHAR szDos32ProtectSetFilePtr = "Dos32ProtectSetFilePtr";

APIRET APIENTRY Trc_Dos32ProtectSetFilePtr( HFILE  Parm1
                                          , LONG   Parm2
                                          , ULONG  Parm3
                                          , PULONG Parm4
                                          , FHLOCK Parm5
                                          )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectSetFilePtr );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32ProtectSetFilePtr, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTSETFILEPTR )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectSetFilePtr );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32ProtectSetFilePtr, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectSetFilePtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectSetFilePtrL()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32PROTECTSETFILEPTRL( HFILE, ULONG, ULONG, ULONG, PLONGLONG, FHLOCK );
PCHAR szDos32ProtectSetFilePtrL = "Dos32ProtectSetFilePtrL";

APIRET APIENTRY Trc_Dos32ProtectSetFilePtrL( HFILE     Parm1
                                           , ULONG     Parm2Lo
                                           , ULONG     Parm2Hi
                                           , ULONG     Parm3
                                           , PLONGLONG Parm4
                                           , FHLOCK    Parm5
                                           )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectSetFilePtrL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONGLONG );
         T_LogLONGLONG( Parm2Hi, Parm2Lo );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONGLONG );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32ProtectSetFilePtrL, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTSETFILEPTRL )( Parm1, Parm2Lo, Parm2Hi, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectSetFilePtrL );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PLONGLONG );
         T_LogPLONGLONG( Parm4 );
      }
      HOOKEXIT( szDos32ProtectSetFilePtrL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectSetFilePtrL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectSetFileSize()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTSETFILESIZE( HFILE, ULONG, FHLOCK );
PCHAR szDos32ProtectSetFileSize = "Dos32ProtectSetFileSize";

APIRET APIENTRY Trc_Dos32ProtectSetFileSize( HFILE  Parm1
                                           , ULONG  Parm2
                                           , FHLOCK Parm3
                                           )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectSetFileSize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_FHLOCK );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32ProtectSetFileSize, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTSETFILESIZE )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectSetFileSize );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ProtectSetFileSize, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectSetFileSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectSetFileSizeL()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32PROTECTSETFILESIZEL( HFILE, ULONG, ULONG, FHLOCK );
PCHAR szDos32ProtectSetFileSizeL = "Dos32ProtectSetFileSizeL";

APIRET APIENTRY Trc_Dos32ProtectSetFileSizeL( HFILE  Parm1
                                            , ULONG  Parm2Lo
                                            , ULONG  Parm2Hi
                                            , FHLOCK Parm3
                                            )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectSetFileSizeL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONGLONG );
         T_LogLONGLONG( Parm2Hi, Parm2Lo );
         T_LogParameter( 3L, DT_FHLOCK );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32ProtectSetFileSizeL, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTSETFILESIZEL )( Parm1, Parm2Lo, Parm2Hi, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectSetFileSizeL );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ProtectSetFileSizeL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectSetFileSizeL() */
#endif

#if FALSE /* OS/2 BUG 192126 - Dos16ProtectWrite does not have ordinal        */
/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ProtectWrite()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSPROTECTWRITE( HFILE16, PVOID16, USHORT, PUSHORT16, FHLOCK );
#define DOSPROTECTWRITE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( FHLOCK ) )
PCHAR szDos16ProtectWrite = "Dos16ProtectWrite";

APIRET16 APIENTRY16 Trc_Dos16ProtectWrite( HFILE16   Parm1
                                         , PVOID16   Parm2
                                         , USHORT    Parm3
                                         , PUSHORT16 Parm4
                                         , FHLOCK    Parm5
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPROTECTWRITE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ProtectWrite, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogCHARBuffer16( Parm2, Parm3 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szDos16ProtectWrite, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSPROTECTWRITE )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ProtectWrite );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16ProtectWrite, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ProtectWrite() */
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ProtectWrite()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32PROTECTWRITE( HFILE, PVOID, ULONG, PULONG, FHLOCK );
PCHAR szDos32ProtectWrite = "Dos32ProtectWrite";

APIRET APIENTRY Trc_Dos32ProtectWrite( HFILE  Parm1
                                     , PVOID  Parm2
                                     , ULONG  Parm3
                                     , PULONG Parm4
                                     , FHLOCK Parm5
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ProtectWrite );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogCHARBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_FHLOCK );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32ProtectWrite, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32PROTECTWRITE )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ProtectWrite );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32ProtectWrite, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ProtectWrite() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Ptrace()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSPTRACE( PBYTE16 );
#define DOSPTRACE_PARMBYTES ( sizeof( PBYTE16 ) )
PCHAR szDos16Ptrace = "Dos16Ptrace";

APIRET16 APIENTRY16 Trc_Dos16Ptrace( PBYTE16 Parm1 )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPTRACE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Ptrace, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PBYTE );
         T_LogPVOID16( Parm1 );
         T_LogPPtrace_B( (PPtrace_B16)Parm1 );
      }
      HOOKENTRY16( szDos16Ptrace, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSPTRACE( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16Ptrace );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PBYTE );
         T_LogPVOID16( Parm1 );
         if ( !usRC ) T_LogPPtrace_B( (PPtrace_B16)Parm1 );
      }
      HOOKEXIT16( szDos16Ptrace, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Ptrace() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QAppType()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQAPPTYPE( PSZ16, PUSHORT16 );
#define DOSQAPPTYPE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16QAppType = "Dos16QAppType";

APIRET16 APIENTRY16 Trc_Dos16QAppType( PSZ16     Parm1
                                     , PUSHORT16 Parm2
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQAPPTYPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SES ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QAppType, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16QAppType, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQAPPTYPE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QAppType );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16QAppType, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QAppType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QCurDir()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQCURDIR( USHORT, PBYTE16, PUSHORT16 );
#define DOSQCURDIR_PARMBYTES ( sizeof( USHORT ) + sizeof( PBYTE16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16QCurDir = "Dos16QCurDir";

APIRET16 APIENTRY16 Trc_Dos16QCurDir( USHORT    Parm1
                                    , PBYTE16   Parm2
                                    , PUSHORT16 Parm3
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQCURDIR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QCurDir, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKENTRY16( szDos16QCurDir, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQCURDIR( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QCurDir );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBYTE );
         if ( !usRC ) T_LogPSZ16( (PSZ16)Parm2 );
         else T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16QCurDir, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QCurDir() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QCurDisk()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQCURDISK( PUSHORT16, PULONG16 );
#define DOSQCURDISK_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( PULONG16 ) )
PCHAR szDos16QCurDisk = "Dos16QCurDisk";

APIRET16 APIENTRY16 Trc_Dos16QCurDisk( PUSHORT16 Parm1
                                     , PULONG16  Parm2
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQCURDISK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QCurDisk, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16QCurDisk, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQCURDISK( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QCurDisk );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPUSHORT16( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG16( Parm2 );
      }
      HOOKEXIT16( szDos16QCurDisk, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QCurDisk() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QFHandState()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQFHANDSTATE( HFILE16, PUSHORT16 );
#define DOSQFHANDSTATE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16QFHandState = "Dos16QFHandState";

APIRET16 APIENTRY16 Trc_Dos16QFHandState( HFILE16   Parm1
                                        , PUSHORT16 Parm2
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQFHANDSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QFHandState, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16QFHandState, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQFHANDSTATE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QFHandState );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16QFHandState, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QFHandState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QFileInfo()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQFILEINFO( HFILE16, USHORT, PVOID16, USHORT );
#define DOSQFILEINFO_PARMBYTES ( sizeof( HFILE16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) )
PCHAR szDos16QFileInfo = "Dos16QFileInfo";

APIRET16 APIENTRY16 Trc_Dos16QFileInfo( HFILE16 Parm1
                                      , USHORT  Parm2
                                      , PVOID16 Parm3
                                      , USHORT  Parm4
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQFILEINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QFileInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( Parm2 == FIL_QUERYEASFROMLIST ) T_LogPEAOP( Parm3, PEAOP_GEA );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szDos16QFileInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQFILEINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QFileInfo );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD16QFI_Parm3( Parm3, Parm2 );
      }
      HOOKEXIT16( szDos16QFileInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QFileInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QFileMode()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQFILEMODE( PSZ16, PUSHORT16, ULONG );
#define DOSQFILEMODE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PUSHORT16 ) + sizeof( ULONG ) )
PCHAR szDos16QFileMode = "Dos16QFileMode";

APIRET16 APIENTRY16 Trc_Dos16QFileMode( PSZ16     Parm1
                                      , PUSHORT16 Parm2
                                      , ULONG     Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQFILEMODE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QFileMode, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szDos16QFileMode, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQFILEMODE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QFileMode );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16QFileMode, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QFileMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QFSAttach()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQFSATTACH( PSZ16, USHORT, USHORT, PVOID16, PUSHORT16, ULONG );
#define DOSQFSATTACH_PARMBYTES ( sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( PUSHORT16 ) + sizeof( ULONG ) )
PCHAR szDos16QFSAttach = "Dos16QFSAttach";

APIRET16 APIENTRY16 Trc_Dos16QFSAttach( PSZ16     Parm1
                                      , USHORT    Parm2
                                      , USHORT    Parm3
                                      , PVOID16   Parm4
                                      , PUSHORT16 Parm5
                                      , ULONG     Parm6
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQFSATTACH_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QFSAttach, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_PUSHORT );
         T_LogPUSHORT16( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY16( szDos16QFSAttach, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQFSATTACH( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QFSAttach );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         if ( ( Parm5 ) && ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogPFSQBUFFER( Parm4, *Parm5 );
         T_LogParameter( 5L, DT_PUSHORT );
         T_LogPUSHORT16( Parm5 );
      }
      HOOKEXIT16( szDos16QFSAttach, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QFSAttach() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QFSInfo()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQFSINFO( USHORT, USHORT, PVOID16, USHORT );
#define DOSQFSINFO_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) )
PCHAR szDos16QFSInfo = "Dos16QFSInfo";

APIRET16 APIENTRY16 Trc_Dos16QFSInfo( USHORT  Parm1
                                    , USHORT  Parm2
                                    , PVOID16 Parm3
                                    , USHORT  Parm4
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQFSINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QFSInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szDos16QFSInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQFSINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QFSInfo );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD16QFSI_Parm3( Parm3, Parm4, Parm2 );
      }
      HOOKEXIT16( szDos16QFSInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QFSInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QHandType()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQHANDTYPE( HFILE16, PUSHORT16, PUSHORT16 );
#define DOSQHANDTYPE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PUSHORT16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16QHandType = "Dos16QHandType";

APIRET16 APIENTRY16 Trc_Dos16QHandType( HFILE16   Parm1
                                      , PUSHORT16 Parm2
                                      , PUSHORT16 Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                  /* API return code           */

   sRegs16.usParmBytes = DOSQHANDTYPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QHandType, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16QHandType, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQHANDTYPE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QHandType );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16QHandType, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QHandType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QPathInfo()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQPATHINFO( PSZ16, USHORT, PVOID16, USHORT, ULONG );
#define DOSQPATHINFO_PARMBYTES ( sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16QPathInfo = "Dos16QPathInfo";

APIRET16 APIENTRY16 Trc_Dos16QPathInfo( PSZ16   Parm1
                                      , USHORT  Parm2
                                      , PVOID16 Parm3
                                      , USHORT  Parm4
                                      , ULONG   Parm5
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQPATHINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QPathInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( Parm2 == FIL_QUERYEASFROMLIST ) T_LogPEAOP( Parm3, PEAOP_GEA );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szDos16QPathInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQPATHINFO( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QPathInfo );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD16QPI_Parm3( Parm3, Parm2 );
      }
      HOOKEXIT16( szDos16QPathInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QPathInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QSysInfo()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQSYSINFO( USHORT, PVOID16, USHORT );
#define DOSQSYSINFO_PARMBYTES ( sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) )
PCHAR szDos16QSysInfo = "Dos16QSysInfo";

APIRET16 APIENTRY16 Trc_Dos16QSysInfo( USHORT  Parm1
                                     , PVOID16 Parm2
                                     , USHORT  Parm3
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQSYSINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QSysInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16QSysInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQSYSINFO( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QSysInfo );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         if ( Parm1 == 0 /* Q_MAX_PATH_LENGTH */ ) T_LogPUSHORT16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szDos16QSysInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QSysInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryABIOSSupport()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 210 ) )
APIRET APIENTRY DOS32QUERYABIOSSUPPORT( ULONG );
PCHAR szDos32QueryABIOSSupport = "Dos32QueryABIOSSupport";

APIRET APIENTRY Trc_Dos32QueryABIOSSupport( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryABIOSSupport );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32QueryABIOSSupport, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32QUERYABIOSSUPPORT )( Parm1 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryABIOSSupport );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32QueryABIOSSupport, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryABIOSSupport() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryAppType()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYAPPTYPE( PSZ, PULONG );
PCHAR szDos32QueryAppType = "Dos32QueryAppType";

APIRET APIENTRY Trc_Dos32QueryAppType( PSZ    Parm1
                                     , PULONG Parm2
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SES ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryAppType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32QueryAppType, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYAPPTYPE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryAppType );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32QueryAppType, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryAppType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryCp()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32QUERYCP( ULONG, PULONG, PULONG );
PCHAR szDos32QueryCp = "Dos32QueryCp";

APIRET APIENTRY Trc_Dos32QueryCp( ULONG  Parm1
                                , PULONG Parm2
                                , PULONG Parm3
                                )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32QueryCp, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYCP( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryCp );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
         if ( ( Parm3 ) && ( ( !ulRC ) || ( ulRC == ERROR_CPLIST_TOO_SMALL ) ) ) T_LogULONGBuffer( Parm2, *Parm3 / sizeof( ULONG ) );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szDos32QueryCp, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryCurrentDir()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYCURRENTDIR( ULONG, PBYTE, PULONG );
PCHAR szDos32QueryCurrentDir = "Dos32QueryCurrentDir";

APIRET APIENTRY Trc_Dos32QueryCurrentDir( ULONG  Parm1
                                        , PBYTE  Parm2
                                        , PULONG Parm3
                                        )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryCurrentDir );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKENTRY( szDos32QueryCurrentDir, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYCURRENTDIR( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryCurrentDir );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBYTE );
         if ( !ulRC ) T_LogPSZ( (PSZ)Parm2 );
         else T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szDos32QueryCurrentDir, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryCurrentDir() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryCurrentDisk()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYCURRENTDISK( PULONG, PULONG );
PCHAR szDos32QueryCurrentDisk = "Dos32QueryCurrentDisk";

APIRET APIENTRY Trc_Dos32QueryCurrentDisk( PULONG Parm1
                                         , PULONG Parm2
                                         )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryCurrentDisk );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32QueryCurrentDisk, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYCURRENTDISK( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryCurrentDisk );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32QueryCurrentDisk, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryCurrentDisk() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QueryDOSProperty()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQUERYDOSPROPERTY( SGID16, PSZ16, USHORT, PSZ16 );
#define DOSQUERYDOSPROPERTY_PARMBYTES ( sizeof( SGID16 ) + sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( PSZ16 ) )
PCHAR szDos16QueryDOSProperty = "Dos16QueryDOSProperty";

APIRET16 APIENTRY16 Trc_Dos16QueryDOSProperty( SGID16 Parm1
                                             , PSZ16  Parm2
                                             , USHORT Parm3
                                             , PSZ16  Parm4
                                             )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQUERYDOSPROPERTY_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QueryDOSProperty, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SGID );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16QueryDOSProperty, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQUERYDOSPROPERTY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QueryDOSProperty );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID16( Parm4 );
         if ( !usRC ) T_LogCHARBuffer16( Parm4, Parm3 );
      }
      HOOKEXIT16( szDos16QueryDOSProperty, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QueryDOSProperty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryDOSProperty()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYDOSPROPERTY( SGID, PSZ, ULONG, PSZ );
PCHAR szDos32QueryDOSProperty = "Dos32QueryDOSProperty";

APIRET APIENTRY Trc_Dos32QueryDOSProperty( SGID  Parm1
                                         , PSZ   Parm2
                                         , ULONG Parm3
                                         , PSZ   Parm4
                                         )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryDOSProperty );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SGID );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32QueryDOSProperty, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYDOSPROPERTY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryDOSProperty );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID( Parm4 );
         if ( !ulRC ) T_LogCHARBuffer( Parm4, Parm3 );
      }
      HOOKEXIT( szDos32QueryDOSProperty, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryDOSProperty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryEventSem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYEVENTSEM( HEV, PULONG );
PCHAR szDos32QueryEventSem = "Dos32QueryEventSem";

APIRET APIENTRY Trc_Dos32QueryEventSem( HEV    Parm1
                                      , PULONG Parm2
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32QueryEventSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYEVENTSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32QueryEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QueryExtLIBPATH()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSQUERYEXTLIBPATH( PSZ16, ULONG );
#define DOSQUERYEXTLIBPATH_PARMBYTES ( sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szDos16QueryExtLIBPATH = "Dos16QueryExtLIBPATH";

APIRET16 APIENTRY16 Trc_Dos16QueryExtLIBPATH( PSZ16 Parm1
                                            , ULONG Parm2
                                            )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQUERYEXTLIBPATH_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QueryExtLIBPATH, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16QueryExtLIBPATH, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSQUERYEXTLIBPATH )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QueryExtLIBPATH );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         if ( !usRC ) T_LogPSZ16( Parm1 );
         else T_LogPVOID16( Parm1 );
      }
      HOOKEXIT16( szDos16QueryExtLIBPATH, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QueryExtLIBPATH() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryExtLIBPATH()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
APIRET APIENTRY DOS32QUERYEXTLIBPATH( PSZ, ULONG );
PCHAR szDos32QueryExtLIBPATH = "Dos32QueryExtLIBPATH";

APIRET APIENTRY Trc_Dos32QueryExtLIBPATH( PSZ   Parm1
                                        , ULONG Parm2
                                        )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryExtLIBPATH );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32QueryExtLIBPATH, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32QUERYEXTLIBPATH )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryExtLIBPATH );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         if ( !ulRC ) T_LogPSZ( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32QueryExtLIBPATH, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryExtLIBPATH() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryFHState()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYFHSTATE( HFILE, PULONG );
PCHAR szDos32QueryFHState = "Dos32QueryFHState";

APIRET APIENTRY Trc_Dos32QueryFHState( HFILE  Parm1
                                     , PULONG Parm2
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryFHState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32QueryFHState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYFHSTATE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryFHState );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32QueryFHState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryFHState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryFileInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYFILEINFO( HFILE, ULONG, PVOID, ULONG );
PCHAR szDos32QueryFileInfo = "Dos32QueryFileInfo";

APIRET APIENTRY Trc_Dos32QueryFileInfo( HFILE Parm1
                                      , ULONG Parm2
                                      , PVOID Parm3
                                      , ULONG Parm4
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryFileInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( Parm2 == FIL_QUERYEASFROMLIST ) T_LogPEAOP2( Parm3, PEAOP_GEA );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32QueryFileInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYFILEINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryFileInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD32QFI_Parm3( Parm3, Parm2 );
      }
      HOOKEXIT( szDos32QueryFileInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryFileInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryFSAttach()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYFSATTACH( PSZ, ULONG, ULONG, PFSQBUFFER2, PULONG );
PCHAR szDos32QueryFSAttach = "Dos32QueryFSAttach";

APIRET APIENTRY Trc_Dos32QueryFSAttach( PSZ         Parm1
                                      , ULONG       Parm2
                                      , ULONG       Parm3
                                      , PFSQBUFFER2 Parm4
                                      , PULONG      Parm5
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryFSAttach );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PFSQBUFFER2 );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPULONG( Parm5 );
      }
      HOOKENTRY( szDos32QueryFSAttach, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYFSATTACH( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryFSAttach );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PFSQBUFFER2 );
         T_LogPVOID( Parm4 );
         if ( ( Parm5 ) && ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) ) T_LogPFSQBUFFER2( Parm4, *Parm5 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPULONG( Parm5 );
      }
      HOOKEXIT( szDos32QueryFSAttach, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryFSAttach() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryFSInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYFSINFO( ULONG, ULONG, PVOID, ULONG );
PCHAR szDos32QueryFSInfo = "Dos32QueryFSInfo";

APIRET APIENTRY Trc_Dos32QueryFSInfo( ULONG Parm1
                                    , ULONG Parm2
                                    , PVOID Parm3
                                    , ULONG Parm4
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryFSInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32QueryFSInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYFSINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryFSInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD32QFSI_Parm3( Parm3, Parm4, Parm2 );
      }
      HOOKEXIT( szDos32QueryFSInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryFSInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryHType()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYHTYPE( HFILE, PULONG, PULONG );
PCHAR szDos32QueryHType = "Dos32QueryHType";

APIRET APIENTRY Trc_Dos32QueryHType( HFILE  Parm1
                                   , PULONG Parm2
                                   , PULONG Parm3
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryHType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32QueryHType, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYHTYPE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryHType );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szDos32QueryHType, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryHType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryMem()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYMEM( PVOID, PULONG, PULONG );
PCHAR szDos32QueryMem = "Dos32QueryMem";

APIRET APIENTRY Trc_Dos32QueryMem( PVOID  Parm1
                                 , PULONG Parm2
                                 , PULONG Parm3
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32QueryMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYMEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryMem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szDos32QueryMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryMemState()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32QUERYMEMSTATE( PVOID, PULONG, PULONG );
PCHAR szDos32QueryMemState = "Dos32QueryMemState";

APIRET APIENTRY Trc_Dos32QueryMemState( PVOID  Parm1
                                      , PULONG Parm2
                                      , PULONG Parm3
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryMemState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32QueryMemState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYMEMSTATE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryMemState );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szDos32QueryMemState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryMemState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QueryModFromCS()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 210 ) )
APIRET16 APIENTRY16 DOSQUERYMODFROMCS( SEL, PQMRESULT16 );
#define DOSQUERYMODFROMCS_PARMBYTES ( sizeof( SEL ) + sizeof( PQMRESULT16 ) )
PCHAR szDos16QueryModFromCS = "Dos16QueryModFromCS";

APIRET16 APIENTRY16 Trc_Dos16QueryModFromCS( SEL         Parm1
                                           , PQMRESULT16 Parm2
                                           )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQUERYMODFROMCS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QueryModFromCS, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PQMRESULT );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16QueryModFromCS, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSQUERYMODFROMCS )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QueryModFromCS );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PQMRESULT );
         T_LogPQMRESULT( Parm2 );
      }
      HOOKEXIT16( szDos16QueryModFromCS, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QueryModFromCS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryModFromEIP()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
APIRET APIENTRY DOS32QUERYMODFROMEIP( PHMODULE, PULONG, ULONG, PCHAR, PULONG, ULONG );
#ifdef INTEL
PCHAR szDos32QueryModFromEIP = "Dos32QueryModFromEIP";
#else
PCHAR szDos32QueryModFromEIP = "Dos32QueryModFromAddr";
#endif

APIRET APIENTRY Trc_Dos32QueryModFromEIP( PHMODULE Parm1
                                        , PULONG   Parm2
                                        , ULONG    Parm3
                                        , PCHAR    Parm4
                                        , PULONG   Parm5
                                        , ULONG    Parm6
                                        )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryModFromEIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHMODULE );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PCHAR );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPVOID( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szDos32QueryModFromEIP, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32QUERYMODFROMEIP )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryModFromEIP );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PHMODULE );
         T_LogPULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 4L, DT_PCHAR );
         T_LogPVOID( Parm4 );
         if ( !ulRC ) T_LogCHARBuffer( Parm4, Parm3 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPULONG( Parm5 );
      }
      HOOKEXIT( szDos32QueryModFromEIP, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryModFromEIP() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryModuleHandle()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYMODULEHANDLE( PSZ, PHMODULE );
PCHAR szDos32QueryModuleHandle = "Dos32QueryModuleHandle";

APIRET APIENTRY Trc_Dos32QueryModuleHandle( PSZ      Parm1
                                          , PHMODULE Parm2
                                          )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryModuleHandle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PHMODULE );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32QueryModuleHandle, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYMODULEHANDLE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryModuleHandle );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHMODULE );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32QueryModuleHandle, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryModuleHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryModuleName()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYMODULENAME( HMODULE, ULONG, PCHAR );
PCHAR szDos32QueryModuleName = "Dos32QueryModuleName";

APIRET APIENTRY Trc_Dos32QueryModuleName( HMODULE Parm1
                                        , ULONG   Parm2
                                        , PCHAR   Parm3
                                        )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryModuleName );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCHAR );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32QueryModuleName, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYMODULENAME( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryModuleName );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCHAR );
         T_LogPVOID( Parm3 );
         if ( !ulRC ) T_LogCHARBuffer( Parm3, Parm2 );
      }
      HOOKEXIT( szDos32QueryModuleName, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryModuleName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryMutexSem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYMUTEXSEM( HMTX, PID *, TID *, PULONG );
PCHAR szDos32QueryMutexSem = "Dos32QueryMutexSem";

APIRET APIENTRY Trc_Dos32QueryMutexSem( HMTX    Parm1
                                      , PID    *Parm2
                                      , TID    *Parm3
                                      , PULONG  Parm4
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryMutexSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMTX );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PIDSTAR );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_TIDSTAR );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32QueryMutexSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYMUTEXSEM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryMutexSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PIDSTAR );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_TIDSTAR );
         T_LogPULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32QueryMutexSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryMutexSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryMuxWaitSem()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYMUXWAITSEM( HMUX, PULONG, PSEMRECORD, PULONG );
PCHAR szDos32QueryMuxWaitSem = "Dos32QueryMuxWaitSem";

APIRET APIENTRY Trc_Dos32QueryMuxWaitSem( HMUX       Parm1
                                        , PULONG     Parm2
                                        , PSEMRECORD Parm3
                                        , PULONG     Parm4
                                        )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMUX );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PSEMRECORD );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32QueryMuxWaitSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYMUXWAITSEM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PSEMRECORD );
         if ( ( Parm2 ) && ( !ulRC ) ) T_LogSEMRECORDBuffer( Parm3, *Parm2 );
         else T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32QueryMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryNPHState()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYNPHSTATE( HPIPE, PULONG );
PCHAR szDos32QueryNPHState = "Dos32QueryNPHState";

APIRET APIENTRY Trc_Dos32QueryNPHState( HPIPE  Parm1
                                      , PULONG Parm2
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryNPHState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPIPE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32QueryNPHState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYNPHSTATE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryNPHState );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32QueryNPHState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryNPHState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryNPipeInfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYNPIPEINFO( HPIPE, ULONG, PVOID, ULONG );
PCHAR szDos32QueryNPipeInfo = "Dos32QueryNPipeInfo";

APIRET APIENTRY Trc_Dos32QueryNPipeInfo( HPIPE Parm1
                                       , ULONG Parm2
                                       , PVOID Parm3
                                       , ULONG Parm4
                                       )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryNPipeInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPIPE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32QueryNPipeInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYNPIPEINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryNPipeInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD32QNPI_Parm3( Parm3, Parm4, Parm2 );
      }
      HOOKEXIT( szDos32QueryNPipeInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryNPipeInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryNPipeSemState()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYNPIPESEMSTATE( HSEM, PPIPESEMSTATE, ULONG );
PCHAR szDos32QueryNPipeSemState = "Dos32QueryNPipeSemState";

APIRET APIENTRY Trc_Dos32QueryNPipeSemState( HSEM          Parm1
                                           , PPIPESEMSTATE Parm2
                                           , ULONG         Parm3
                                           )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryNPipeSemState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
         T_LogParameter( 2L, DT_PPIPESEMSTATE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32QueryNPipeSemState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYNPIPESEMSTATE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryNPipeSemState );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPIPESEMSTATE );
         T_LogPVOID( Parm2 );
         if ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) T_LogPPIPESEMSTATE( Parm2, Parm3 );
      }
      HOOKEXIT( szDos32QueryNPipeSemState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryNPipeSemState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryPathInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYPATHINFO( PSZ, ULONG, PVOID, ULONG );
PCHAR szDos32QueryPathInfo = "Dos32QueryPathInfo";

APIRET APIENTRY Trc_Dos32QueryPathInfo( PSZ   Parm1
                                      , ULONG Parm2
                                      , PVOID Parm3
                                      , ULONG Parm4
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryPathInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( Parm2 == FIL_QUERYEASFROMLIST ) T_LogPEAOP2( Parm3, PEAOP_GEA );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32QueryPathInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYPATHINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryPathInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( ( !ulRC ) || ( ulRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD32QPI_Parm3( Parm3, Parm2 );
      }
      HOOKEXIT( szDos32QueryPathInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryPathInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryProcAddr()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYPROCADDR( HMODULE, ULONG, PSZ, PFN* );
PCHAR szDos32QueryProcAddr = "Dos32QueryProcAddr";

APIRET APIENTRY Trc_Dos32QueryProcAddr( HMODULE   Parm1
                                      , ULONG     Parm2
                                      , PSZ       Parm3
                                      , PFN      *Parm4
                                      )
{

   PFN    pfnTrace;                              /* Trace API                 */
   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryProcAddr );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PFNSTAR );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32QueryProcAddr, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYPROCADDR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryProcAddr );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PFNSTAR );
         T_LogPPFN( Parm4 );
      }
      if ( ( FLAGSET( T_TraceFlg, F_INTERCEPT )                                                  ) &&
           ( ulRC == NO_ERROR                                                                    ) &&
           ( DOS32QUERYPROCADDR( T_DLLToTraceDLL( Parm1 ), Parm2, Parm3, &pfnTrace ) == NO_ERROR ) )
      {
         T_LogAPIInterception( Parm1, Parm2, Parm3 );
         *Parm4 = pfnTrace;
      }
      HOOKEXIT( szDos32QueryProcAddr, &Parm1, &ulRC );
      T_FreeLog( );
   }

   else if ( FLAGSET( T_TraceFlg, F_INTERCEPT ) )
   {
      T_WaitLog( );
      if ( ( ulRC == NO_ERROR                                                                    ) &&
           ( DOS32QUERYPROCADDR( T_DLLToTraceDLL( Parm1 ), Parm2, Parm3, &pfnTrace ) == NO_ERROR ) )
      {
         T_LogAPIInterception( Parm1, Parm2, Parm3 );
         *Parm4 = pfnTrace;
      }
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryProcAddr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryProcessInfo()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET APIENTRY DOS32QUERYPROCESSINFO( ULONG, ULONG, ULONG, PVOID, PULONG, PULONG );
PCHAR szDos32QueryProcessInfo = "Dos32QueryProcessInfo";

APIRET APIENTRY Trc_Dos32QueryProcessInfo( ULONG  Parm1
                                         , ULONG  Parm2
                                         , ULONG  Parm3
                                         , PVOID  Parm4
                                         , PULONG Parm5
                                         , PULONG Parm6
                                         )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryProcessInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szDos32QueryProcessInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32QUERYPROCESSINFO )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryProcessInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         if ( ( Parm5 ) && ( Parm6 ) && ( !ulRC ) ) T_LogQPROCINFOBuffer( Parm4, *Parm5, *Parm6, Parm3 );
         else T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      HOOKEXIT( szDos32QueryProcessInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryProcessInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QueryProcType()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQUERYPROCTYPE( HMODULE16, ULONG, PSZ16, PULONG16 );
#define DOSQUERYPROCTYPE_PARMBYTES ( sizeof( HMODULE16 ) + sizeof( ULONG ) + sizeof( PSZ16 ) + sizeof( PULONG16 ) )
PCHAR szDos16QueryProcType = "Dos16QueryProcType";

APIRET16 APIENTRY16 Trc_Dos16QueryProcType( HMODULE16 Parm1
                                          , ULONG     Parm2
                                          , PSZ16     Parm3
                                          , PULONG16  Parm4
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQUERYPROCTYPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QueryProcType, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16QueryProcType, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQUERYPROCTYPE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QueryProcType );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG16( Parm4 );
      }
      HOOKEXIT16( szDos16QueryProcType, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Dos16QueryProcType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryProcType()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYPROCTYPE( HMODULE, ULONG, PSZ, PULONG );
PCHAR szDos32QueryProcType = "Dos32QueryProcType";

APIRET APIENTRY Trc_Dos32QueryProcType( HMODULE Parm1
                                      , ULONG   Parm2
                                      , PSZ     Parm3
                                      , PULONG  Parm4
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryProcType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32QueryProcType, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYPROCTYPE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryProcType );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32QueryProcType, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryProcType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryRASInfo()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32QUERYRASINFO( ULONG, PPVOID );
PCHAR szDos32QueryRASInfo = "Dos32QueryRASInfo";

APIRET APIENTRY Trc_Dos32QueryRASInfo( ULONG  Parm1
                                     , PPVOID Parm2
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryRASInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPVOID );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32QueryRASInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYRASINFO( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryRASInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPVOID );
         T_LogPULONG( (PULONG)Parm2 );
      }
      HOOKEXIT( szDos32QueryRASInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryRASInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QueryResourceSize()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQUERYRESOURCESIZE( HMODULE16, ULONG, ULONG, PULONG16 );
#define DOSQUERYRESOURCESIZE_PARMBYTES ( sizeof( HMODULE16 ) + sizeof( ULONG ) + sizeof( ULONG ) + sizeof( PULONG16 ) )
PCHAR szDos16QueryResourceSize = "Dos16QueryResourceSize";

APIRET16 APIENTRY16 Trc_Dos16QueryResourceSize( HMODULE16 Parm1
                                              , ULONG     Parm2
                                              , ULONG     Parm3
                                              , PULONG16  Parm4
                                              )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQUERYRESOURCESIZE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QueryResourceSize, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16QueryResourceSize, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQUERYRESOURCESIZE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QueryResourceSize );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG16( Parm4 );
      }
      HOOKEXIT16( szDos16QueryResourceSize, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Dos16QueryResourceSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryResourceSize()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYRESOURCESIZE( HMODULE, ULONG, ULONG, PULONG );
PCHAR szDos32QueryResourceSize = "Dos32QueryResourceSize";

APIRET APIENTRY Trc_Dos32QueryResourceSize( HMODULE Parm1
                                          , ULONG   Parm2
                                          , ULONG   Parm3
                                          , PULONG  Parm4
                                          )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryResourceSize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMODULE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32QueryResourceSize, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYRESOURCESIZE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_RES ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryResourceSize );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32QueryResourceSize, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryResourceSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QuerySysInfo()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYSYSINFO( ULONG, ULONG, PVOID, ULONG );
PCHAR szDos32QuerySysInfo = "Dos32QuerySysInfo";

APIRET APIENTRY Trc_Dos32QuerySysInfo( ULONG Parm1
                                     , ULONG Parm2
                                     , PVOID Parm3
                                     , ULONG Parm4
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QuerySysInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32QuerySysInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYSYSINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QuerySysInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( !ulRC ) T_LogULONGBuffer( Parm3, ( Parm2 - Parm1 + 1 ) );
         else if ( ulRC == ERROR_BUFFER_OVERFLOW ) T_LogULONGBuffer( Parm3, Parm4 / sizeof( ULONG ) );
      }
      HOOKEXIT( szDos32QuerySysInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QuerySysInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QuerySysState()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32QUERYSYSSTATE( ULONG, ULONG, PID, TID, PVOID, ULONG );
PCHAR szDos32QuerySysState = "Dos32QuerySysState";

APIRET APIENTRY Trc_Dos32QuerySysState( ULONG Parm1
                                      , ULONG Parm2
                                      , PID   Parm3
                                      , TID   Parm4
                                      , PVOID Parm5
                                      , ULONG Parm6
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QuerySysState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PID );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_TID );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PVOID );
         T_LogPVOID( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szDos32QuerySysState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYSYSSTATE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QuerySysState );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PVOID );
         T_LogPVOID( Parm5 );
         if ( !ulRC ) T_LogCHARBuffer( Parm5, Parm6 );
      }
      HOOKEXIT( szDos32QuerySysState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QuerySysState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryThreadAffinity()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32QUERYTHREADAFFINITY( ULONG, PMPAFFINITY );
PCHAR szDos32QueryThreadAffinity = "Dos32QueryThreadAffinity";

APIRET APIENTRY Trc_Dos32QueryThreadAffinity( ULONG       Parm1
                                            , PMPAFFINITY Parm2
                                            )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryThreadAffinity );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMPAFFINITY );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32QueryThreadAffinity, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32QUERYTHREADAFFINITY )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryThreadAffinity );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMPAFFINITY );
         if ( !ulRC ) T_LogPMPAFFINITY( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szDos32QueryThreadAffinity, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryThreadAffinity() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryThreadContext()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
APIRET APIENTRY DOS32QUERYTHREADCONTEXT( TID, ULONG, PCONTEXTRECORD );
PCHAR szDos32QueryThreadContext = "Dos32QueryThreadContext";

APIRET APIENTRY Trc_Dos32QueryThreadContext( TID            Parm1
                                           , ULONG          Parm2
                                           , PCONTEXTRECORD Parm3
                                           )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryThreadContext );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_TID );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCONTEXTRECORD );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32QueryThreadContext, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32QUERYTHREADCONTEXT )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryThreadContext );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCONTEXTRECORD );
         if ( !ulRC ) T_LogPCONTEXTRECORD( Parm3, Parm2 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szDos32QueryThreadContext, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryThreadContext() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryThreadInfo()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET APIENTRY DOS32QUERYTHREADINFO( ULONG, ULONG, ULONG, PVOID, PULONG, PULONG );
PCHAR szDos32QueryThreadInfo = "Dos32QueryThreadInfo";

APIRET APIENTRY Trc_Dos32QueryThreadInfo( ULONG  Parm1
                                        , ULONG  Parm2
                                        , ULONG  Parm3
                                        , PVOID  Parm4
                                        , PULONG Parm5
                                        , PULONG Parm6
                                        )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryThreadInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szDos32QueryThreadInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32QUERYTHREADINFO )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryThreadInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         if ( ( Parm5 ) && ( Parm6 ) && ( !ulRC ) ) T_LogQTHREADINFOBuffer( Parm4, *Parm5, *Parm6, Parm3 );
         else T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      HOOKEXIT( szDos32QueryThreadInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryThreadInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryVerify()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYVERIFY( PBOOL32 );
PCHAR szDos32QueryVerify = "Dos32QueryVerify";

APIRET APIENTRY Trc_Dos32QueryVerify( PBOOL32 Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryVerify );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PBOOL32 );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32QueryVerify, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYVERIFY( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryVerify );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PBOOL32 );
         T_LogPULONG( Parm1 );
      }
      HOOKEXIT( szDos32QueryVerify, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryVerify() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QVerify()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQVERIFY( PUSHORT16 );
#define DOSQVERIFY_PARMBYTES ( sizeof( PUSHORT16 ) )
PCHAR szDos16QVerify = "Dos16QVerify";

APIRET16 APIENTRY16 Trc_Dos16QVerify( PUSHORT16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQVERIFY_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16QVerify, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16QVerify, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSQVERIFY( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16QVerify );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPUSHORT16( Parm1 );
      }
      HOOKEXIT16( szDos16QVerify, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QVerify() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16R2StackRealloc()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSR2STACKREALLOC( USHORT );
#define DOSR2STACKREALLOC_PARMBYTES ( sizeof( USHORT ) )
PCHAR szDos16R2StackRealloc = "Dos16R2StackRealloc";

APIRET16 APIENTRY16 Trc_Dos16R2StackRealloc( USHORT Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSR2STACKREALLOC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16R2StackRealloc, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16R2StackRealloc, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSR2STACKREALLOC( Parm1 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16R2StackRealloc );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16R2StackRealloc, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16R2StackRealloc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32RaiseException()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32RAISEEXCEPTION( PEXCEPTIONREPORTRECORD );
PCHAR szDos32RaiseException = "Dos32RaiseException";

APIRET APIENTRY Trc_Dos32RaiseException( PEXCEPTIONREPORTRECORD Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32RaiseException );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PEXCEPTIONREPORTRECORD );
         T_LogPEXCEPTIONREPORTRECORD( Parm1 );
      }
      HOOKENTRY( szDos32RaiseException, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32RAISEEXCEPTION( Parm1 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32RaiseException );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32RaiseException, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32RaiseException() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32RawReadNPipe()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32RAWREADNPIPE( HFILE, PVOID, ULONG, PULONG );
PCHAR szDos32RawReadNPipe = "Dos32RawReadNPipe";

APIRET APIENTRY Trc_Dos32RawReadNPipe( HFILE  Parm1
                                     , PVOID  Parm2
                                     , ULONG  Parm3
                                     , PULONG Parm4
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32RawReadNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32RawReadNPipe, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32RAWREADNPIPE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32RawReadNPipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( ( Parm4 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer( Parm2, *Parm4 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32RawReadNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32RawReadNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32RawWriteNPipe()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32RAWWRITENPIPE( HFILE, PVOID, ULONG, PULONG );
PCHAR szDos32RawWriteNPipe = "Dos32RawWriteNPipe";

APIRET APIENTRY Trc_Dos32RawWriteNPipe( HFILE  Parm1
                                      , PVOID  Parm2
                                      , ULONG  Parm3
                                      , PULONG Parm4
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32RawWriteNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogCHARBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32RawWriteNPipe, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32RAWWRITENPIPE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32RawWriteNPipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32RawWriteNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32RawWriteNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Read()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSREAD( HFILE16, PVOID16, USHORT, PUSHORT16 );
#define DOSREAD_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16Read = "Dos16Read";

APIRET16 APIENTRY16 Trc_Dos16Read( HFILE16   Parm1
                                 , PVOID16   Parm2
                                 , USHORT    Parm3
                                 , PUSHORT16 Parm4
                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSREAD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Read, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16Read, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSREAD( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Read );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         if ( ( Parm4 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer16( Parm2, *Parm4 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16Read, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Read() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ReadAsync()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSREADASYNC( HFILE16, PULONG16, PUSHORT16, PVOID16, USHORT, PUSHORT16 );
#define DOSREADASYNC_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PULONG16 ) + sizeof( PUSHORT16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16ReadAsync = "Dos16ReadAsync";

APIRET16 APIENTRY16 Trc_Dos16ReadAsync( HFILE16   Parm1
                                      , PULONG16  Parm2
                                      , PUSHORT16 Parm3
                                      , PVOID16   Parm4
                                      , USHORT    Parm5
                                      , PUSHORT16 Parm6
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSREADASYNC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ReadAsync, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szDos16ReadAsync, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSREADASYNC( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ReadAsync );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer16( Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPUSHORT16( Parm6 );
      }
      HOOKEXIT16( szDos16ReadAsync, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ReadAsync() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Read()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32READ( HFILE, PVOID, ULONG, PULONG );
PCHAR szDos32Read = "Dos32Read";

APIRET APIENTRY Trc_Dos32Read( HFILE  Parm1
                             , PVOID  Parm2
                             , ULONG  Parm3
                             , PULONG Parm4
                             )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Read );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32Read, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32READ( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Read );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( ( Parm4 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer( Parm2, *Parm4 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32Read, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Read() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ReallocHuge()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSREALLOCHUGE( USHORT, USHORT, SEL );
#define DOSREALLOCHUGE_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( SEL ) )
PCHAR szDos16ReallocHuge = "Dos16ReallocHuge";

APIRET16 APIENTRY16 Trc_Dos16ReallocHuge( USHORT Parm1
                                        , USHORT Parm2
                                        , SEL    Parm3
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSREALLOCHUGE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ReallocHuge, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SEL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16ReallocHuge, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSREALLOCHUGE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ReallocHuge );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ReallocHuge, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ReallocHuge() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ReallocSeg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSREALLOCSEG( USHORT, SEL );
#define DOSREALLOCSEG_PARMBYTES ( sizeof( USHORT ) + sizeof( SEL ) )
PCHAR szDos16ReallocSeg = "Dos16ReallocSeg";

APIRET16 APIENTRY16 Trc_Dos16ReallocSeg( USHORT Parm1
                                       , SEL    Parm2
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSREALLOCSEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ReallocSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_SEL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szDos16ReallocSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSREALLOCSEG( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ReallocSeg );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ReallocSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ReallocSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16RegisterPerfCtrs()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSREGISTERPERFCTRS( PBYTE16, PBYTE16, ULONG );
#define DOSREGISTERPERFCTRS_PARMBYTES ( sizeof( PBYTE16 ) + sizeof( PBYTE16 ) + sizeof( ULONG ) )
PCHAR szDos16RegisterPerfCtrs = "Dos16RegisterPerfCtrs";

APIRET16 APIENTRY16 Trc_Dos16RegisterPerfCtrs( PBYTE16 Parm1
                                             , PBYTE16 Parm2
                                             , ULONG   Parm3
                                             )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSREGISTERPERFCTRS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16RegisterPerfCtrs, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PBYTE );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szDos16RegisterPerfCtrs, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSREGISTERPERFCTRS( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16RegisterPerfCtrs );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16RegisterPerfCtrs, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16RegisterPerfCtrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32RegisterPerfCtrs()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32REGISTERPERFCTRS( PBYTE, PBYTE, ULONG );
PCHAR szDos32RegisterPerfCtrs = "Dos32RegisterPerfCtrs";

APIRET APIENTRY Trc_Dos32RegisterPerfCtrs( PBYTE Parm1
                                         , PBYTE Parm2
                                         , ULONG Parm3
                                         )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32RegisterPerfCtrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PBYTE );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32RegisterPerfCtrs, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32REGISTERPERFCTRS( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32RegisterPerfCtrs );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32RegisterPerfCtrs, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32RegisterPerfCtrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ReleaseMutexSem()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32RELEASEMUTEXSEM( HMTX );
PCHAR szDos32ReleaseMutexSem = "Dos32ReleaseMutexSem";

APIRET APIENTRY Trc_Dos32ReleaseMutexSem( HMTX Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ReleaseMutexSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMTX );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32ReleaseMutexSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32RELEASEMUTEXSEM( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ReleaseMutexSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ReleaseMutexSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ReleaseMutexSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ReleaseRamSem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET EXPENTRY DOS32RELEASERAMSEM( PRAMSEM, ULONG );
PCHAR szDos32ReleaseRamSem = "Dos32ReleaseRamSem";

APIRET EXPENTRY Trc_Dos32ReleaseRamSem( PRAMSEM Parm1
                                      , ULONG   Parm2
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ReleaseRamSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         T_LogPRAMSEM( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32ReleaseRamSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32RELEASERAMSEM )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ReleaseRamSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         if ( !ulRC ) T_LogPRAMSEM( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32ReleaseRamSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ReleaseRamSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ReleaseSpinLock()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSRELEASESPINLOCK( HSPINLOCK );
PCHAR szDos16ReleaseSpinLock = "Dos16ReleaseSpinLock";

APIRET16 APIENTRY16 Trc_Dos16ReleaseSpinLock( HSPINLOCK Parm1 )
{

   APIRET16 usRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos16ReleaseSpinLock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSPINLOCK );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szDos16ReleaseSpinLock, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOSRELEASESPINLOCK )
   {
      usRC = ( *pfnDOSRELEASESPINLOCK )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos16ReleaseSpinLock );
         T_FreeLog( );
      }

      usRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ReleaseSpinLock );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ReleaseSpinLock, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Dos16ReleaseSpinLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ReleaseSpinLock()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32RELEASESPINLOCK( HSPINLOCK );
PCHAR szDos32ReleaseSpinLock = "Dos32ReleaseSpinLock";

APIRET APIENTRY Trc_Dos32ReleaseSpinLock( HSPINLOCK Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ReleaseSpinLock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSPINLOCK );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32ReleaseSpinLock, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOS32RELEASESPINLOCK )
   {
      ulRC = ( *pfnDOS32RELEASESPINLOCK )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos32ReleaseSpinLock );
         T_FreeLog( );
      }

      ulRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ReleaseSpinLock );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ReleaseSpinLock, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ReleaseSpinLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ReplaceModule()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32REPLACEMODULE( PSZ, PSZ, PSZ );
PCHAR szDos32ReplaceModule = "Dos32ReplaceModule";

APIRET APIENTRY Trc_Dos32ReplaceModule( PSZ Parm1
                                      , PSZ Parm2
                                      , PSZ Parm3
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ReplaceModule );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
      }
      HOOKENTRY( szDos32ReplaceModule, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32REPLACEMODULE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MOD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ReplaceModule );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ReplaceModule, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ReplaceModule() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32RequestMutexSem()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32REQUESTMUTEXSEM( HMTX, ULONG );
PCHAR szDos32RequestMutexSem = "Dos32RequestMutexSem";

APIRET APIENTRY Trc_Dos32RequestMutexSem( HMTX  Parm1
                                        , ULONG Parm2
                                        )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32RequestMutexSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMTX );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32RequestMutexSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32REQUESTMUTEXSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32RequestMutexSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32RequestMutexSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32RequestMutexSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32RequestRamSem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET EXPENTRY DOS32REQUESTRAMSEM( PRAMSEM, ULONG );
PCHAR szDos32RequestRamSem = "Dos32RequestRamSem";

APIRET EXPENTRY Trc_Dos32RequestRamSem( PRAMSEM Parm1
                                      , ULONG   Parm2
                                      )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32RequestRamSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRAMSEM );
         T_LogPRAMSEM( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32RequestRamSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = ( *pfnDOS32REQUESTRAMSEM )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32RequestRamSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PRAMSEM );
         if ( !ulRC ) T_LogPRAMSEM( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32RequestRamSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32RequestRamSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16RequestVDD()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSREQUESTVDD( HVDD, SGID16, USHORT, ULONG, PVOID16, ULONG, PVOID16 );
#define DOSREQUESTVDD_PARMBYTES ( sizeof( HVDD ) + sizeof( SGID16 ) + sizeof( USHORT ) + sizeof( ULONG ) + sizeof( PVOID16 ) + sizeof( ULONG ) + sizeof( PVOID16 ) )
PCHAR szDos16RequestVDD = "Dos16RequestVDD";

APIRET16 APIENTRY16 Trc_Dos16RequestVDD( HVDD    Parm1
                                       , SGID16  Parm2
                                       , USHORT  Parm3
                                       , ULONG   Parm4
                                       , PVOID16 Parm5
                                       , ULONG   Parm6
                                       , PVOID16 Parm7
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSREQUESTVDD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16RequestVDD, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HVDD );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SGID );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PVOID );
         T_LogPVOID16( Parm5 );
         T_LogCHARBuffer16( Parm5, Parm4 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PVOID );
         T_LogPVOID16( Parm7 );
      }
      HOOKENTRY16( szDos16RequestVDD, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSREQUESTVDD( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16RequestVDD );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PVOID );
         T_LogPVOID16( Parm7 );
         if ( !usRC ) T_LogCHARBuffer16( Parm7, Parm6 );
      }
      HOOKEXIT16( szDos16RequestVDD, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16RequestVDD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32RequestVDD()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32REQUESTVDD( HVDD, SGID, ULONG, ULONG, PVOID, ULONG, PVOID );
PCHAR szDos32RequestVDD = "Dos32RequestVDD";

APIRET APIENTRY Trc_Dos32RequestVDD( HVDD  Parm1
                                   , SGID  Parm2
                                   , ULONG Parm3
                                   , ULONG Parm4
                                   , PVOID Parm5
                                   , ULONG Parm6
                                   , PVOID Parm7
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32RequestVDD );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HVDD );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SGID );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PVOID );
         T_LogPVOID( Parm5 );
         T_LogCHARBuffer( Parm5, Parm4 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PVOID );
         T_LogPVOID( Parm7 );
      }
      HOOKENTRY( szDos32RequestVDD, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32REQUESTVDD( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32RequestVDD );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PVOID );
         T_LogPVOID( Parm7 );
         if ( !ulRC ) T_LogCHARBuffer( Parm7, Parm6 );
      }
      HOOKEXIT( szDos32RequestVDD, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32RequestVDD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ResetBuffer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32RESETBUFFER( HFILE );
PCHAR szDos32ResetBuffer = "Dos32ResetBuffer";

APIRET APIENTRY Trc_Dos32ResetBuffer( HFILE Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ResetBuffer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32ResetBuffer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32RESETBUFFER( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ResetBuffer );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ResetBuffer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ResetBuffer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ResetEventSem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32RESETEVENTSEM( HEV, PULONG );
PCHAR szDos32ResetEventSem = "Dos32ResetEventSem";

APIRET APIENTRY Trc_Dos32ResetEventSem( HEV    Parm1
                                      , PULONG Parm2
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ResetEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32ResetEventSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32RESETEVENTSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ResetEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32ResetEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ResetEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ResumeThread()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSRESUMETHREAD( TID16 );
#define DOSRESUMETHREAD_PARMBYTES ( sizeof( TID16 ) )
PCHAR szDos16ResumeThread = "Dos16ResumeThread";

APIRET16 APIENTRY16 Trc_Dos16ResumeThread( TID16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSRESUMETHREAD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ResumeThread, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_TID );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16ResumeThread, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSRESUMETHREAD( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ResumeThread );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16ResumeThread, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ResumeThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ResumeThread()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32RESUMETHREAD( TID );
PCHAR szDos32ResumeThread = "Dos32ResumeThread";

APIRET APIENTRY Trc_Dos32ResumeThread( TID Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ResumeThread );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_TID );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32ResumeThread, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32RESUMETHREAD( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ResumeThread );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32ResumeThread, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ResumeThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16RmDir()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSRMDIR( PSZ16, ULONG );
#define DOSRMDIR_PARMBYTES ( sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szDos16RmDir = "Dos16RmDir";

APIRET16 APIENTRY16 Trc_Dos16RmDir( PSZ16 Parm1
                                  , ULONG Parm2
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSRMDIR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16RmDir, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16RmDir, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSRMDIR( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16RmDir );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16RmDir, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16RmDir() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ScanEnv()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSCANENV( PSZ16, PPSZ16 );
#define DOSSCANENV_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PPSZ16 ) )
PCHAR szDos16ScanEnv = "Dos16ScanEnv";

APIRET16 APIENTRY16 Trc_Dos16ScanEnv( PSZ16  Parm1
                                    , PPSZ16 Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSCANENV_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16ScanEnv, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PSZFARSTAR );
         T_LogPVOID16( (PVOID16)Parm2 );
      }
      HOOKENTRY16( szDos16ScanEnv, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSCANENV( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16ScanEnv );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSZFARSTAR );
         if ( !usRC ) T_LogPPSZ16( Parm2 );
         else T_LogPULONG16( (PULONG16)Parm2 );
      }
      HOOKEXIT16( szDos16ScanEnv, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ScanEnv() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ScanEnv()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SCANENV( PSZ, PSZ * );
PCHAR szDos32ScanEnv = "Dos32ScanEnv";

APIRET APIENTRY Trc_Dos32ScanEnv( PSZ  Parm1
                                , PSZ *Parm2
                                )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32ScanEnv );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PSZSTAR );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32ScanEnv, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SCANENV( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32ScanEnv );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSZSTAR );
         if ( !ulRC ) T_LogPPSZ( Parm2 );
         else T_LogPULONG( (PULONG)Parm2 );
      }
      HOOKEXIT( szDos32ScanEnv, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32ScanEnv() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SearchPath()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSEARCHPATH( USHORT, PSZ16, PSZ16, PBYTE16, USHORT );
#define DOSSEARCHPATH_PARMBYTES ( sizeof( USHORT ) + sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( PBYTE16 ) + sizeof( USHORT ) )
PCHAR szDos16SearchPath = "Dos16SearchPath";

APIRET16 APIENTRY16 Trc_Dos16SearchPath( USHORT  Parm1
                                       , PSZ16   Parm2
                                       , PSZ16   Parm3
                                       , PBYTE16 Parm4
                                       , USHORT  Parm5
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSEARCHPATH_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SearchPath, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szDos16SearchPath, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSEARCHPATH( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SearchPath );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         if ( !usRC ) T_LogCHARBuffer16( (PCHAR16)Parm4, Parm5 );
      }
      HOOKEXIT16( szDos16SearchPath, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SearchPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SearchPath()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SEARCHPATH( ULONG, PSZ, PSZ, PBYTE, ULONG );
PCHAR szDos32SearchPath = "Dos32SearchPath";

APIRET APIENTRY Trc_Dos32SearchPath( ULONG Parm1
                                   , PSZ   Parm2
                                   , PSZ   Parm3
                                   , PBYTE Parm4
                                   , ULONG Parm5
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SearchPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32SearchPath, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SEARCHPATH( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SearchPath );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         if ( !ulRC ) T_LogCHARBuffer( (PCHAR)Parm4, Parm5 );
      }
      HOOKEXIT( szDos32SearchPath, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SearchPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SelectDisk()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSELECTDISK( USHORT );
#define DOSSELECTDISK_PARMBYTES ( sizeof( USHORT ) )
PCHAR szDos16SelectDisk = "Dos16SelectDisk";

APIRET16 APIENTRY16 Trc_Dos16SelectDisk( USHORT Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSELECTDISK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SelectDisk, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16SelectDisk, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSELECTDISK( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SelectDisk );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SelectDisk, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SelectDisk() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SemClear()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSEMCLEAR( HSEM );
#define DOSSEMCLEAR_PARMBYTES ( sizeof( HSEM ) )
PCHAR szDos16SemClear = "Dos16SemClear";

APIRET16 APIENTRY16 Trc_Dos16SemClear( HSEM Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSEMCLEAR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SemClear, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
      }
      HOOKENTRY16( szDos16SemClear, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSEMCLEAR( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SemClear );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SemClear, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SemClear() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SemRequest()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSEMREQUEST( HSEM, LONG );
#define DOSSEMREQUEST_PARMBYTES ( sizeof( HSEM ) + sizeof( LONG ) )
PCHAR szDos16SemRequest = "Dos16SemRequest";

APIRET16 APIENTRY16 Trc_Dos16SemRequest( HSEM Parm1
                                       , LONG Parm2
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSEMREQUEST_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SemRequest, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16SemRequest, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSSEMREQUEST( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16SemRequest );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT16( szDos16SemRequest, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SemRequest() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SemSet()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSEMSET( HSEM );
#define DOSSEMSET_PARMBYTES ( sizeof( HSEM ) )
PCHAR szDos16SemSet = "Dos16SemSet";

APIRET16 APIENTRY16 Trc_Dos16SemSet( HSEM Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSEMSET_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SemSet, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
      }
      HOOKENTRY16( szDos16SemSet, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSEMSET( Parm1 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SemSet );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SemSet, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SemSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SemSetWait()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSEMSETWAIT( HSEM, LONG );
#define DOSSEMSETWAIT_PARMBYTES ( sizeof( HSEM ) + sizeof( LONG ) )
PCHAR szDos16SemSetWait = "Dos16SemSetWait";

APIRET16 APIENTRY16 Trc_Dos16SemSetWait( HSEM Parm1
                                       , LONG Parm2
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSEMSETWAIT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SemSetWait, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16SemSetWait, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSSEMSETWAIT( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16SemSetWait );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT16( szDos16SemSetWait, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SemSetWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SemWait()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSEMWAIT( HSEM, LONG );
#define DOSSEMWAIT_PARMBYTES ( sizeof( HSEM ) + sizeof( LONG ) )
PCHAR szDos16SemWait = "Dos16SemWait";

APIRET16 APIENTRY16 Trc_Dos16SemWait( HSEM Parm1
                                    , LONG Parm2
                                    )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSEMWAIT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SemWait, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16SemWait, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSSEMWAIT( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16SemWait );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT16( szDos16SemWait, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SemWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SendSignal()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSENDSIGNAL( USHORT, USHORT );
#define DOSSENDSIGNAL_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16SendSignal = "Dos16SendSignal";

APIRET16 APIENTRY16 Trc_Dos16SendSignal( USHORT Parm1
                                       , USHORT Parm2
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSENDSIGNAL_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SIG ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SendSignal, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szDos16SendSignal, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSENDSIGNAL( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SIG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SendSignal );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SendSignal, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SendSignal() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SendSignalException()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SENDSIGNALEXCEPTION( PID, ULONG );
PCHAR szDos32SendSignalException = "Dos32SendSignalException";

APIRET APIENTRY Trc_Dos32SendSignalException( PID   Parm1
                                            , ULONG Parm2
                                            )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SendSignalException );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PID );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32SendSignalException, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SENDSIGNALEXCEPTION( Parm1, Parm2 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SendSignalException );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SendSignalException, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SendSignalException() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETCP( USHORT, USHORT );
#define DOSSETCP_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16SetCp = "Dos16SetCp";

APIRET16 APIENTRY16 Trc_Dos16SetCp( USHORT Parm1
                                  , USHORT Parm2
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETCP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetCp, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szDos16SetCp, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETCP( Parm1, Parm2 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetCp );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetCp, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32SETCP( ULONG );
PCHAR szDos32SetCp = "Dos32SetCp";

APIRET APIENTRY Trc_Dos32SetCp( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32SetCp, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETCP( Parm1 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetCp );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetCp, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetCurrentDir()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETCURRENTDIR( PSZ );
PCHAR szDos32SetCurrentDir = "Dos32SetCurrentDir";

APIRET APIENTRY Trc_Dos32SetCurrentDir( PSZ Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetCurrentDir );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
      }
      HOOKENTRY( szDos32SetCurrentDir, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETCURRENTDIR( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetCurrentDir );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetCurrentDir, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetCurrentDir() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetDateTime()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETDATETIME( PDATETIME16 );
#define DOSSETDATETIME_PARMBYTES ( sizeof( PDATETIME16 ) )
PCHAR szDos16SetDateTime = "Dos16SetDateTime";

APIRET16 APIENTRY16 Trc_Dos16SetDateTime( PDATETIME16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETDATETIME_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetDateTime, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDATETIME );
         T_LogPDATETIME16( Parm1 );
      }
      HOOKENTRY16( szDos16SetDateTime, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETDATETIME( Parm1 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetDateTime );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetDateTime, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetDateTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetDateTime()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETDATETIME( PDATETIME );
PCHAR szDos32SetDateTime = "Dos32SetDateTime";

APIRET APIENTRY Trc_Dos32SetDateTime( PDATETIME Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetDateTime );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PDATETIME );
         T_LogPDATETIME( Parm1 );
      }
      HOOKENTRY( szDos32SetDateTime, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETDATETIME( Parm1 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetDateTime );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetDateTime, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetDateTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetDefaultDisk()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETDEFAULTDISK( ULONG );
PCHAR szDos32SetDefaultDisk = "Dos32SetDefaultDisk";

APIRET APIENTRY Trc_Dos32SetDefaultDisk( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetDefaultDisk );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32SetDefaultDisk, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETDEFAULTDISK( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetDefaultDisk );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetDefaultDisk, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetDefaultDisk() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetDOSProperty()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETDOSPROPERTY( SGID16, PSZ16, USHORT, PSZ16 );
#define DOSSETDOSPROPERTY_PARMBYTES ( sizeof( SGID16 ) + sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( PSZ16 ) )
PCHAR szDos16SetDOSProperty = "Dos16SetDOSProperty";

APIRET16 APIENTRY16 Trc_Dos16SetDOSProperty( SGID16 Parm1
                                           , PSZ16  Parm2
                                           , USHORT Parm3
                                           , PSZ16  Parm4
                                           )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETDOSPROPERTY_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetDOSProperty, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SGID );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( Parm4, Parm3 );
      }
      HOOKENTRY16( szDos16SetDOSProperty, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETDOSPROPERTY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetDOSProperty );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetDOSProperty, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetDOSProperty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetDOSProperty()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETDOSPROPERTY( SGID, PSZ, ULONG, PSZ );
PCHAR szDos32SetDOSProperty = "Dos32SetDOSProperty";

APIRET APIENTRY Trc_Dos32SetDOSProperty( SGID  Parm1
                                       , PSZ   Parm2
                                       , ULONG Parm3
                                       , PSZ   Parm4
                                       )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetDOSProperty );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SGID );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID( Parm4 );
         T_LogCHARBuffer( Parm4, Parm3 );
      }
      HOOKENTRY( szDos32SetDOSProperty, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETDOSPROPERTY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MVDM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetDOSProperty );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetDOSProperty, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetDOSProperty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetExceptionHandler()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETEXCEPTIONHANDLER( PEXCEPTIONREGISTRATIONRECORD );
PCHAR szDos32SetExceptionHandler = "Dos32SetExceptionHandler";

APIRET APIENTRY Trc_Dos32SetExceptionHandler( PEXCEPTIONREGISTRATIONRECORD Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetExceptionHandler );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PEXCEPTIONREGISTRATIONRECORD );
         T_LogPEXCEPTIONREGISTRATIONRECORD( Parm1 );
      }
      HOOKENTRY( szDos32SetExceptionHandler, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETEXCEPTIONHANDLER( Parm1 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetExceptionHandler );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetExceptionHandler, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetExceptionHandler() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetExtLIBPATH()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
APIRET16 APIENTRY16 DOSSETEXTLIBPATH( PSZ16, ULONG );
#define DOSSETEXTLIBPATH_PARMBYTES ( sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szDos16SetExtLIBPATH = "Dos16SetExtLIBPATH";

APIRET16 APIENTRY16 Trc_Dos16SetExtLIBPATH( PSZ16 Parm1
                                          , ULONG Parm2
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETEXTLIBPATH_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetExtLIBPATH, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szDos16SetExtLIBPATH, &Parm1 );
      T_FreeLog( );
   }

   usRC = ( *pfnDOSSETEXTLIBPATH )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetExtLIBPATH );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetExtLIBPATH, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetExtLIBPATH() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetExtLIBPATH()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
APIRET APIENTRY DOS32SETEXTLIBPATH( PSZ, ULONG );
PCHAR szDos32SetExtLIBPATH = "Dos32SetExtLIBPATH";

APIRET APIENTRY Trc_Dos32SetExtLIBPATH( PSZ   Parm1
                                      , ULONG Parm2
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetExtLIBPATH );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32SetExtLIBPATH, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32SETEXTLIBPATH )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetExtLIBPATH );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetExtLIBPATH, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetExtLIBPATH() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetFHandState()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETFHANDSTATE( HFILE16, USHORT );
#define DOSSETFHANDSTATE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( USHORT ) )
PCHAR szDos16SetFHandState = "Dos16SetFHandState";

APIRET16 APIENTRY16 Trc_Dos16SetFHandState( HFILE16 Parm1
                                          , USHORT Parm2
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETFHANDSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetFHandState, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szDos16SetFHandState, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETFHANDSTATE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetFHandState );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetFHandState, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetFHandState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFHState()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETFHSTATE( HFILE, ULONG );
PCHAR szDos32SetFHState = "Dos32SetFHState";

APIRET APIENTRY Trc_Dos32SetFHState( HFILE Parm1
                                   , ULONG Parm2
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFHState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32SetFHState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETFHSTATE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFHState );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetFHState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFHState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetFileInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETFILEINFO( HFILE16, USHORT, PVOID16, USHORT );
#define DOSSETFILEINFO_PARMBYTES ( sizeof( HFILE16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) )
PCHAR szDos16SetFileInfo = "Dos16SetFileInfo";

APIRET16 APIENTRY16 Trc_Dos16SetFileInfo( HFILE16 Parm1
                                        , USHORT  Parm2
                                        , PVOID16 Parm3
                                        , USHORT  Parm4
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETFILEINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetFileInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogD16SFI_Parm3( Parm3, Parm2 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szDos16SetFileInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETFILEINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetFileInfo );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( ( usRC ) && ( Parm2 == FIL_QUERYEASIZE ) ) T_LogPEAOP( Parm3, PEAOP_ERR );
      }
      HOOKEXIT16( szDos16SetFileInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetFileInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFileInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETFILEINFO( HFILE, ULONG, PVOID, ULONG );
PCHAR szDos32SetFileInfo = "Dos32SetFileInfo";

APIRET APIENTRY Trc_Dos32SetFileInfo( HFILE Parm1
                                    , ULONG Parm2
                                    , PVOID Parm3
                                    , ULONG Parm4
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFileInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogD32SFI_Parm3( Parm3, Parm2 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32SetFileInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETFILEINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFileInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( ( ulRC ) && ( Parm2 == FIL_QUERYEASIZE ) ) T_LogPEAOP2( Parm3, PEAOP_ERR );
      }
      HOOKEXIT( szDos32SetFileInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFileInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFileLocks()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETFILELOCKS( HFILE, PFILELOCK, PFILELOCK, ULONG, ULONG );
PCHAR szDos32SetFileLocks = "Dos32SetFileLocks";

APIRET APIENTRY Trc_Dos32SetFileLocks( HFILE     Parm1
                                     , PFILELOCK Parm2
                                     , PFILELOCK Parm3
                                     , ULONG     Parm4
                                     , ULONG     Parm5
                                     )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFileLocks );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFILELOCK );
         T_LogPFILELOCK( Parm2 );
         T_LogParameter( 3L, DT_PFILELOCK );
         T_LogPFILELOCK( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32SetFileLocks, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32SETFILELOCKS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFileLocks );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32SetFileLocks, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFileLocks() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFileLocksL()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32SETFILELOCKSL( HFILE, PFILELOCKL, PFILELOCKL, ULONG, ULONG );
PCHAR szDos32SetFileLocksL = "Dos32SetFileLocksL";

APIRET APIENTRY Trc_Dos32SetFileLocksL( HFILE      Parm1
                                      , PFILELOCKL Parm2
                                      , PFILELOCKL Parm3
                                      , ULONG      Parm4
                                      , ULONG      Parm5
                                      )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFileLocksL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFILELOCKL );
         T_LogPFILELOCKL( Parm2 );
         T_LogParameter( 3L, DT_PFILELOCKL );
         T_LogPFILELOCKL( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32SetFileLocksL, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = ( *pfnDOS32SETFILELOCKSL )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFileLocksL );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32SetFileLocksL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFileLocksL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetFileMode()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETFILEMODE( PSZ16, USHORT, ULONG );
#define DOSSETFILEMODE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16SetFileMode = "Dos16SetFileMode";

APIRET16 APIENTRY16 Trc_Dos16SetFileMode( PSZ16  Parm1
                                        , USHORT Parm2
                                        , ULONG  Parm3
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETFILEMODE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetFileMode, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szDos16SetFileMode, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETFILEMODE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetFileMode );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetFileMode, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetFileMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFilePtr()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETFILEPTR( HFILE, LONG, ULONG, PULONG );
PCHAR szDos32SetFilePtr = "Dos32SetFilePtr";

APIRET APIENTRY Trc_Dos32SetFilePtr( HFILE  Parm1
                                   , LONG   Parm2
                                   , ULONG  Parm3
                                   , PULONG Parm4
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFilePtr );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32SetFilePtr, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETFILEPTR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFilePtr );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32SetFilePtr, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFilePtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFilePtrL()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32SETFILEPTRL( HFILE, ULONG, ULONG, ULONG, PLONGLONG );
PCHAR szDos32SetFilePtrL = "Dos32SetFilePtrL";

APIRET APIENTRY Trc_Dos32SetFilePtrL( HFILE     Parm1
                                    , ULONG     Parm2Lo
                                    , ULONG     Parm2Hi
                                    , ULONG     Parm3
                                    , PLONGLONG Parm4
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFilePtrL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONGLONG );
         T_LogLONGLONG( Parm2Hi, Parm2Lo );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONGLONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32SetFilePtrL, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32SETFILEPTRL )( Parm1, Parm2Lo, Parm2Hi, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFilePtrL );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PLONGLONG );
         T_LogPLONGLONG( Parm4 );
      }
      HOOKEXIT( szDos32SetFilePtrL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFilePtrL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFileSize()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETFILESIZE( HFILE, ULONG );
PCHAR szDos32SetFileSize = "Dos32SetFileSize";

APIRET APIENTRY Trc_Dos32SetFileSize( HFILE Parm1
                                    , ULONG Parm2
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFileSize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32SetFileSize, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETFILESIZE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFileSize );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetFileSize, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFileSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFileSizeL()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32SETFILESIZEL( HFILE, ULONG, ULONG );
PCHAR szDos32SetFileSizeL = "Dos32SetFileSizeL";

APIRET APIENTRY Trc_Dos32SetFileSizeL( HFILE Parm1
                                     , ULONG Parm2Lo
                                     , ULONG Parm2Hi
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFileSizeL );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONGLONG );
         T_LogLONGLONG( Parm2Hi, Parm2Lo );
      }
      HOOKENTRY( szDos32SetFileSizeL, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32SETFILESIZEL )( Parm1, Parm2Lo, Parm2Hi );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFileSizeL );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetFileSizeL, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFileSizeL() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetFSInfo()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETFSINFO( USHORT, USHORT, PVOID16, USHORT );
#define DOSSETFSINFO_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) )
PCHAR szDos16SetFSInfo = "Dos16SetFSInfo";

APIRET16 APIENTRY16 Trc_Dos16SetFSInfo( USHORT  Parm1
                                      , USHORT  Parm2
                                      , PVOID16 Parm3
                                      , USHORT  Parm4
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETFSINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetFSInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogCHARBuffer16( Parm3, Parm4 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szDos16SetFSInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETFSINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetFSInfo );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetFSInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetFSInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetFSInfo()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETFSINFO( ULONG, ULONG, PVOID, ULONG );
PCHAR szDos32SetFSInfo = "Dos32SetFSInfo";

APIRET APIENTRY Trc_Dos32SetFSInfo( ULONG Parm1
                                  , ULONG Parm2
                                  , PVOID Parm3
                                  , ULONG Parm4
                                  )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetFSInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogCHARBuffer( Parm3, Parm4 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32SetFSInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETFSINFO( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetFSInfo );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetFSInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetFSInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetMaxFH()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETMAXFH( USHORT );
#define DOSSETMAXFH_PARMBYTES ( sizeof( USHORT ) )
PCHAR szDos16SetMaxFH = "Dos16SetMaxFH";

APIRET16 APIENTRY16 Trc_Dos16SetMaxFH( USHORT Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETMAXFH_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetMaxFH, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16SetMaxFH, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETMAXFH( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetMaxFH );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetMaxFH, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetMaxFH() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetMaxFH()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETMAXFH( ULONG );
PCHAR szDos32SetMaxFH = "Dos32SetMaxFH";

APIRET APIENTRY Trc_Dos32SetMaxFH( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetMaxFH );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32SetMaxFH, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETMAXFH( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetMaxFH );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetMaxFH, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetMaxFH() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetMem()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETMEM( PVOID, ULONG, ULONG );
PCHAR szDos32SetMem = "Dos32SetMem";

APIRET APIENTRY Trc_Dos32SetMem( PVOID Parm1
                               , ULONG Parm2
                               , ULONG Parm3
                               )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32SetMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETMEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetMem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetNPHState()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETNPHSTATE( HPIPE, ULONG );
PCHAR szDos32SetNPHState = "Dos32SetNPHState";

APIRET APIENTRY Trc_Dos32SetNPHState( HPIPE Parm1
                                    , ULONG Parm2
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetNPHState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPIPE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32SetNPHState, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETNPHSTATE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetNPHState );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetNPHState, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetNPHState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetNPipeSem()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETNPIPESEM( HPIPE, HSEM, ULONG );
PCHAR szDos32SetNPipeSem = "Dos32SetNPipeSem";

APIRET APIENTRY Trc_Dos32SetNPipeSem( HPIPE Parm1
                                    , HSEM  Parm2
                                    , ULONG Parm3
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetNPipeSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPIPE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HSEM );
         T_LogULONG( (ULONG)Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32SetNPipeSem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETNPIPESEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetNPipeSem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetNPipeSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetNPipeSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetPathInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETPATHINFO( PSZ16, USHORT, PVOID16, USHORT, USHORT, ULONG );
#define DOSSETPATHINFO_PARMBYTES ( sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16SetPathInfo = "Dos16SetPathInfo";

APIRET16 APIENTRY16 Trc_Dos16SetPathInfo( PSZ16   Parm1
                                        , USHORT  Parm2
                                        , PVOID16 Parm3
                                        , USHORT  Parm4
                                        , USHORT  Parm5
                                        , ULONG   Parm6
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETPATHINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetPathInfo, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogD16SFI_Parm3( Parm3, Parm2 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY16( szDos16SetPathInfo, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETPATHINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetPathInfo );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( ( usRC ) && ( Parm2 == FIL_QUERYEASIZE ) ) T_LogPEAOP( Parm3, PEAOP_ERR );
      }
      HOOKEXIT16( szDos16SetPathInfo, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetPathInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetPathInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETPATHINFO( PSZ, ULONG, PVOID, ULONG, ULONG );
PCHAR szDos32SetPathInfo = "Dos32SetPathInfo";

APIRET APIENTRY Trc_Dos32SetPathInfo( PSZ   Parm1
                                    , ULONG Parm2
                                    , PVOID Parm3
                                    , ULONG Parm4
                                    , ULONG Parm5
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetPathInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogD32SFI_Parm3( Parm3, Parm2 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32SetPathInfo, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETPATHINFO( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetPathInfo );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( ( ulRC ) && ( Parm2 == FIL_QUERYEASIZE ) ) T_LogPEAOP2( Parm3, PEAOP_ERR );
      }
      HOOKEXIT( szDos32SetPathInfo, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetPathInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetPriority()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETPRIORITY( ULONG, ULONG, LONG, ULONG );
PCHAR szDos32SetPriority = "Dos32SetPriority";

APIRET APIENTRY Trc_Dos32SetPriority( ULONG Parm1
                                    , ULONG Parm2
                                    , LONG  Parm3
                                    , ULONG Parm4
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetPriority );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szDos32SetPriority, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETPRIORITY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetPriority );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetPriority, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetPriority() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetProcCp()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETPROCCP( USHORT, USHORT );
#define DOSSETPROCCP_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16SetProcCp = "Dos16SetProcCp";

APIRET16 APIENTRY16 Trc_Dos16SetProcCp( USHORT Parm1
                                      , USHORT Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETPROCCP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetProcCp, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szDos16SetProcCp, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETPROCCP( Parm1, Parm2 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetProcCp );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetProcCp, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetProcCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetProcessCp()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETPROCESSCP( ULONG );
PCHAR szDos32SetProcessCp = "Dos32SetProcessCp";

APIRET APIENTRY Trc_Dos32SetProcessCp( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetProcessCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32SetProcessCp, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETPROCESSCP( Parm1 );

   if ( LOGDOSAPI( D_NLS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetProcessCp );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetProcessCp, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetProcessCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetProcessorStatus()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32SETPROCESSORSTATUS( ULONG, ULONG );
PCHAR szDos32SetProcessorStatus = "Dos32SetProcessorStatus";

APIRET APIENTRY Trc_Dos32SetProcessorStatus( ULONG Parm1
                                           , ULONG Parm2
                                           )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetProcessorStatus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32SetProcessorStatus, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOS32SETPROCESSORSTATUS )
   {
      ulRC = ( *pfnDOS32SETPROCESSORSTATUS )( Parm1, Parm2 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos32SetProcessorStatus );
         T_FreeLog( );
      }

      ulRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetProcessorStatus );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetProcessorStatus, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetProcessorStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetPrty()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETPRTY( USHORT, USHORT, SHORT, USHORT );
#define DOSSETPRTY_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( SHORT ) + sizeof( USHORT ) )
PCHAR szDos16SetPrty = "Dos16SetPrty";

APIRET16 APIENTRY16 Trc_Dos16SetPrty( USHORT Parm1
                                    , USHORT Parm2
                                    , SHORT  Parm3
                                    , USHORT Parm4
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETPRTY_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetPrty, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szDos16SetPrty, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETPRTY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetPrty );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetPrty, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetPrty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetRelMaxFH()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETRELMAXFH( PLONG, PULONG );
PCHAR szDos32SetRelMaxFH = "Dos32SetRelMaxFH";

APIRET APIENTRY Trc_Dos32SetRelMaxFH( PLONG  Parm1
                                    , PULONG Parm2
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetRelMaxFH );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32SetRelMaxFH, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETRELMAXFH( Parm1, Parm2 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetRelMaxFH );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32SetRelMaxFH, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetRelMaxFH() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetSigHandler()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETSIGHANDLER( PFNSIGHANDLER16, PPFNSIGHANDLER16, PUSHORT16, USHORT, USHORT );
#define DOSSETSIGHANDLER_PARMBYTES ( sizeof( PFNSIGHANDLER16 ) + sizeof( PPFNSIGHANDLER16 ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16SetSigHandler = "Dos16SetSigHandler";

APIRET16 APIENTRY16 Trc_Dos16SetSigHandler( PFNSIGHANDLER16  Parm1
                                          , PPFNSIGHANDLER16 Parm2
                                          , PUSHORT16        Parm3
                                          , USHORT           Parm4
                                          , USHORT           Parm5
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETSIGHANDLER_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_SIG ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetSigHandler, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PFNSIGHANDLER );
         T_LogPFN16( (PFN16)Parm1 );
         T_LogParameter( 2L, DT_PFNSIGHANDLERFARSTAR );
         T_LogPVOID16( (PVOID16)Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szDos16SetSigHandler, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETSIGHANDLER( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_SIG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetSigHandler );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PFNSIGHANDLERFARSTAR );
         T_LogPPFN16( (PPFN16)Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16SetSigHandler, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetSigHandler() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetSignalExceptionFocus()                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETSIGNALEXCEPTIONFOCUS( BOOL32, PULONG );
PCHAR szDos32SetSignalExceptionFocus = "Dos32SetSignalExceptionFocus";

APIRET APIENTRY Trc_Dos32SetSignalExceptionFocus( BOOL32 Parm1
                                                , PULONG Parm2
                                                )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetSignalExceptionFocus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_BOOL32 );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szDos32SetSignalExceptionFocus, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETSIGNALEXCEPTIONFOCUS( Parm1, Parm2 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetSignalExceptionFocus );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
      }
      HOOKEXIT( szDos32SetSignalExceptionFocus, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetSignalExceptionFocus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetThreadAffinity()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32SETTHREADAFFINITY( PMPAFFINITY );
PCHAR szDos32SetThreadAffinity = "Dos32SetThreadAffinity";

APIRET APIENTRY Trc_Dos32SetThreadAffinity( PMPAFFINITY Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetThreadAffinity );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PMPAFFINITY );
         T_LogPMPAFFINITY( Parm1 );
      }
      HOOKENTRY( szDos32SetThreadAffinity, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32SETTHREADAFFINITY )( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetThreadAffinity );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetThreadAffinity, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetThreadAffinity() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetVec()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETVEC( USHORT, PFN16, PPFN16 );
#define DOSSETVEC_PARMBYTES ( sizeof( USHORT ) + sizeof( PFN16 ) + sizeof( PPFN16 ) )
PCHAR szDos16SetVec = "Dos16SetVec";

APIRET16 APIENTRY16 Trc_Dos16SetVec( USHORT  Parm1
                                   , PFN16   Parm2
                                   , PPFN16  Parm3
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETVEC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetVec, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PFN );
         T_LogPFN16( Parm2 );
         T_LogParameter( 3L, DT_PFNFARSTAR );
         T_LogPVOID16( (PVOID16)Parm3 );
      }
      HOOKENTRY16( szDos16SetVec, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETVEC( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetVec );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PFNFARSTAR );
         T_LogPPFN16( Parm3 );
      }
      HOOKEXIT16( szDos16SetVec, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetVec() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetVerify()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETVERIFY( USHORT );
#define DOSSETVERIFY_PARMBYTES ( sizeof( USHORT ) )
PCHAR szDos16SetVerify = "Dos16SetVerify";

APIRET16 APIENTRY16 Trc_Dos16SetVerify( USHORT Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETVERIFY_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SetVerify, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16SetVerify, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSETVERIFY( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SetVerify );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SetVerify, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetVerify() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetVerify()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETVERIFY( BOOL32 );
PCHAR szDos32SetVerify = "Dos32SetVerify";

APIRET APIENTRY Trc_Dos32SetVerify( BOOL32 Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetVerify );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_BOOL32 );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32SetVerify, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SETVERIFY( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SetVerify );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetVerify, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SetVerify() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Shutdown()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSHUTDOWN( ULONG );
#define DOSSHUTDOWN_PARMBYTES ( sizeof( ULONG ) )
PCHAR szDos16Shutdown = "Dos16Shutdown";

APIRET16 APIENTRY16 Trc_Dos16Shutdown( ULONG Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSHUTDOWN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Shutdown, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szDos16Shutdown, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSHUTDOWN( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Shutdown );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16Shutdown, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Shutdown() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Shutdown()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SHUTDOWN( ULONG );
PCHAR szDos32Shutdown = "Dos32Shutdown";

APIRET APIENTRY Trc_Dos32Shutdown( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Shutdown );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32Shutdown, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SHUTDOWN( Parm1 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Shutdown );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32Shutdown, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Shutdown() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SizeSeg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSIZESEG( SEL, PULONG16 );
#define DOSSIZESEG_PARMBYTES ( sizeof( SEL ) + sizeof( PULONG16 ) )
PCHAR szDos16SizeSeg = "Dos16SizeSeg";

APIRET16 APIENTRY16 Trc_Dos16SizeSeg( SEL      Parm1
                                    , PULONG16 Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSIZESEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SizeSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szDos16SizeSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSIZESEG( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SizeSeg );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG16( Parm2 );
      }
      HOOKEXIT16( szDos16SizeSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SizeSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Sleep()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSLEEP( ULONG );
#define DOSSLEEP_PARMBYTES ( sizeof( ULONG ) )
PCHAR szDos16Sleep = "Dos16Sleep";

APIRET16 APIENTRY16 Trc_Dos16Sleep( ULONG Parm1 )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSLEEP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Sleep, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szDos16Sleep, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   usRC = DOSSLEEP( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos16Sleep );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT16( szDos16Sleep, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos32Sleep() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Sleep()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SLEEP( ULONG );
PCHAR szDos32Sleep = "Dos32Sleep";

APIRET APIENTRY Trc_Dos32Sleep( ULONG Parm1 )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Sleep );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32Sleep, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32SLEEP( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32Sleep );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32Sleep, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Sleep() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32StartTimer()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32STARTTIMER( ULONG, HSEM, PHTIMER );
PCHAR szDos32StartTimer = "Dos32StartTimer";

APIRET APIENTRY Trc_Dos32StartTimer( ULONG   Parm1
                                   , HSEM    Parm2
                                   , PHTIMER Parm3
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32StartTimer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HSEM );
         T_LogULONG( (ULONG)Parm2 );
         T_LogParameter( 3L, DT_PHTIMER );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32StartTimer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32STARTTIMER( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32StartTimer );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PHTIMER );
         T_LogPULONG( (PHTIMER)Parm3 );
      }
      HOOKEXIT( szDos32StartTimer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32StartTimer */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32StopTimer()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32STOPTIMER( HTIMER );
PCHAR szDos32StopTimer = "Dos32StopTimer";

APIRET APIENTRY Trc_Dos32StopTimer( HTIMER Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32StopTimer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HTIMER );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32StopTimer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32STOPTIMER( Parm1 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32StopTimer );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32StopTimer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32StopTimer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SubAlloc()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOS16SUBALLOC( SEL, PUSHORT16, USHORT );
#define DOSSUBALLOC_PARMBYTES ( sizeof( SEL ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) )
PCHAR szDos16SubAlloc = "Dos16SubAlloc";

APIRET16 APIENTRY16 Trc_Dos16SubAlloc( SEL       Parm1
                                     , PUSHORT16 Parm2
                                     , USHORT    Parm3
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSUBALLOC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SubAlloc, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16SubAlloc, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOS16SUBALLOC( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SubAlloc );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PUSHORT );
         T_LogPUSHORT16( Parm2 );
      }
      HOOKEXIT16( szDos16SubAlloc, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SubAlloc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SubAllocMem()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SUBALLOCMEM( PVOID, PPVOID, ULONG );
PCHAR szDos32SubAllocMem = "Dos32SubAllocMem";

APIRET APIENTRY Trc_Dos32SubAllocMem( PVOID  Parm1
                                    , PPVOID Parm2
                                    , ULONG  Parm3
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SubAllocMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PPVOID );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32SubAllocMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SUBALLOCMEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SubAllocMem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPVOID );
         T_LogPULONG( (PULONG)Parm2 );
      }
      HOOKEXIT( szDos32SubAllocMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SubAllocMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SubFree()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOS16SUBFREE( SEL, USHORT, USHORT );
#define DOSSUBFREE_PARMBYTES ( sizeof( SEL ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16SubFree = "Dos16SubFree";

APIRET16 APIENTRY16 Trc_Dos16SubFree( SEL    Parm1
                                    , USHORT Parm2
                                    , USHORT Parm3
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSUBFREE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SubFree, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16SubFree, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOS16SUBFREE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SubFree );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SubFree, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SubFree() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SubFreeMem()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SUBFREEMEM( PVOID, PVOID, ULONG );
PCHAR szDos32SubFreeMem = "Dos32SubFreeMem";

APIRET APIENTRY Trc_Dos32SubFreeMem( PVOID Parm1
                                   , PVOID Parm2
                                   , ULONG Parm3
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SubFreeMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32SubFreeMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SUBFREEMEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SubFreeMem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SubFreeMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SubFreeMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SubSet()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOS16SUBSET( SEL, USHORT, USHORT );
#define DOSSUBSET_PARMBYTES ( sizeof( SEL ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16SubSet = "Dos16SubSet";

APIRET16 APIENTRY16 Trc_Dos16SubSet( SEL    Parm1
                                   , USHORT Parm2
                                   , USHORT Parm3
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSUBSET_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SubSet, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szDos16SubSet, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOS16SUBSET( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SubSet );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SubSet, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SubSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SubSetMem()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SUBSETMEM( PVOID, ULONG, ULONG );
PCHAR szDos32SubSetMem = "Dos32SubSetMem";

APIRET APIENTRY Trc_Dos32SubSetMem( PVOID Parm1
                                  , ULONG Parm2
                                  , ULONG Parm3
                                  )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SubSetMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szDos32SubSetMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SUBSETMEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SubSetMem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SubSetMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SubSetMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SubUnsetMem()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SUBUNSETMEM( PVOID );
PCHAR szDos32SubUnsetMem = "Dos32SubUnsetMem";

APIRET APIENTRY Trc_Dos32SubUnsetMem( PVOID Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SubUnsetMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PVOID );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32SubUnsetMem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SUBUNSETMEM( Parm1 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SubUnsetMem );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SubUnsetMem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SubUnsetMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SuppressPopUps()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 210 ) )
APIRET APIENTRY DOS32SUPPRESSPOPUPS( ULONG, ULONG );
PCHAR szDos32SuppressPopUps = "Dos32SuppressPopUps";

APIRET APIENTRY Trc_Dos32SuppressPopUps( ULONG Parm1
                                       , ULONG Parm2
                                       )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SuppressPopUps );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32SuppressPopUps, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32SUPPRESSPOPUPS )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SuppressPopUps );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SuppressPopUps, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SuppressPopUps() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SuspendThread()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSUSPENDTHREAD( TID16 );
#define DOSSUSPENDTHREAD_PARMBYTES ( sizeof( TID16 ) )
PCHAR szDos16SuspendThread = "Dos16SuspendThread";

APIRET16 APIENTRY16 Trc_Dos16SuspendThread( TID16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSUSPENDTHREAD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SuspendThread, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_TID );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16SuspendThread, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSUSPENDTHREAD( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SuspendThread );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SuspendThread, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SuspendThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SuspendThread()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SUSPENDTHREAD( TID );
PCHAR szDos32SuspendThread = "Dos32SuspendThread";

APIRET APIENTRY Trc_Dos32SuspendThread( TID Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SuspendThread );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_TID );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32SuspendThread, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32SUSPENDTHREAD( Parm1 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SuspendThread );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SuspendThread, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SuspendThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SystemService()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSYSTEMSERVICE( USHORT, PVOID16, PVOID16 );
#define DOSSYSTEMSERVICE_PARMBYTES ( sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( PVOID16 ) )
PCHAR szDos16SystemService = "Dos16SystemService";

APIRET16 APIENTRY16 Trc_Dos16SystemService( USHORT  Parm1
                                          , PVOID16 Parm2
                                          , PVOID16 Parm3
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSYSTEMSERVICE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SystemService, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogCHARBufferUnformatted16( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogCHARBufferUnformatted16( Parm3 );
      }
      HOOKENTRY16( szDos16SystemService, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSYSTEMSERVICE( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SystemService );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         if ( !usRC ) T_LogCHARBufferUnformatted16( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         if ( !usRC ) T_LogCHARBufferUnformatted16( Parm3 );
      }
      HOOKEXIT16( szDos16SystemService, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SystemService() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SystemService()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
APIRET APIENTRY DOS32SYSTEMSERVICE( ULONG, PVOID, PVOID );
PCHAR szDos32SystemService = "Dos32SystemService";

APIRET APIENTRY Trc_Dos32SystemService( ULONG Parm1
                                      , PVOID Parm2
                                      , PVOID Parm3
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32SystemService );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogCHARBufferUnformatted( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogCHARBufferUnformatted( Parm3 );
      }
      HOOKENTRY( szDos32SystemService, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32SYSTEMSERVICE )( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32SystemService );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         if ( !ulRC ) T_LogCHARBufferUnformatted( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         if ( !ulRC ) T_LogCHARBufferUnformatted( Parm3 );
      }
      HOOKEXIT( szDos32SystemService, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32SystemService() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SysTrace()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSYSTRACE( USHORT, USHORT, USHORT, PCHAR16 );
#define DOSSYSTRACE_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PCHAR16 ) )
PCHAR szDos16SysTrace = "Dos16SysTrace";

APIRET16 APIENTRY16 Trc_Dos16SysTrace( USHORT  Parm1
                                     , USHORT  Parm2
                                     , USHORT  Parm3
                                     , PCHAR16 Parm4
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSYSTRACE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16SysTrace, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PCHAR );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( Parm4, Parm2 );
      }
      HOOKENTRY16( szDos16SysTrace, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSSYSTRACE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16SysTrace );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16SysTrace, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SysTrace() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32TestPSD()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32TESTPSD( PSZ );
PCHAR szDos32TestPSD = "Dos32TestPSD";

APIRET APIENTRY Trc_Dos32TestPSD( PSZ Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32TestPSD );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
      }
      HOOKENTRY( szDos32TestPSD, &Parm1 );
      T_FreeLog( );
   }

   if ( pfnDOS32TESTPSD )
   {
      ulRC = ( *pfnDOS32TESTPSD )( Parm1 );
   }
   else
   {
      if ( LOGDOSAPI( D_SMP ) )
      {
         T_WaitLog( );
         T_LogMessage( szNotSMPMsg, szDos32TestPSD );
         T_FreeLog( );
      }

      ulRC = ERROR_INVALID_FUNCTION;
   }

   if ( LOGDOSAPI( D_SMP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32TestPSD );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32TestPSD, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32TestPSD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16TimerAsync()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSTIMERASYNC( ULONG, HSEM, PHTIMER16 );
#define DOSTIMERASYNC_PARMBYTES ( sizeof( ULONG ) + sizeof( HSEM ) + sizeof( PHTIMER16 ) )
PCHAR szDos16TimerAsync = "Dos16TimerAsync";

APIRET16 APIENTRY16 Trc_Dos16TimerAsync( ULONG     Parm1
                                       , HSEM      Parm2
                                       , PHTIMER16 Parm3
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSTIMERASYNC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16TimerAsync, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HSEM );
         T_LogULONG( (ULONG)Parm2 );
         T_LogParameter( 3L, DT_PHTIMER );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16TimerAsync, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSTIMERASYNC( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16TimerAsync );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PHTIMER );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16TimerAsync, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16TimerAsync() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16TimerStart()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSTIMERSTART( ULONG, HSEM, PHTIMER16 );
#define DOSTIMERSTART_PARMBYTES ( sizeof( ULONG ) + sizeof( HSEM ) + sizeof( PHTIMER16 ) )
PCHAR szDos16TimerStart = "Dos16TimerStart";

APIRET16 APIENTRY16 Trc_Dos16TimerStart( ULONG     Parm1
                                       , HSEM      Parm2
                                       , PHTIMER16 Parm3
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSTIMERSTART_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16TimerStart, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HSEM );
         T_LogULONG( (ULONG)Parm2 );
         T_LogParameter( 3L, DT_PHTIMER );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szDos16TimerStart, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSTIMERSTART( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16TimerStart );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PHTIMER );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szDos16TimerStart, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16TimerStart() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16TimerStop()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSTIMERSTOP( HTIMER16 );
#define DOSTIMERSTOP_PARMBYTES ( sizeof( HTIMER16 ) )
PCHAR szDos16TimerStop = "Dos16TimerStop";

APIRET16 APIENTRY16 Trc_Dos16TimerStop( HTIMER16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSTIMERSTOP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16TimerStop, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HTIMER );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16TimerStop, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSTIMERSTOP( Parm1 );

   if ( LOGDOSAPI( D_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16TimerStop );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16TimerStop, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16TimerStop() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16TmrQueryFreq()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSTMRQUERYFREQ( PULONG16 );
#define DOSTMRQUERYFREQ_PARMBYTES ( sizeof( PULONG16 ) )
PCHAR szDos16TmrQueryFreq = "Dos16TmrQueryFreq";

APIRET16 APIENTRY16 Trc_Dos16TmrQueryFreq( PULONG16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSTMRQUERYFREQ_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16TmrQueryFreq, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16TmrQueryFreq, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSTMRQUERYFREQ( Parm1 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16TmrQueryFreq );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPULONG16( Parm1 );
      }
      HOOKEXIT16( szDos16TmrQueryFreq, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16TmrQueryFreq() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32TmrQueryFreq()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32TMRQUERYFREQ( PULONG );
PCHAR szDos32TmrQueryFreq = "Dos32TmrQueryFreq";

APIRET APIENTRY Trc_Dos32TmrQueryFreq( PULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32TmrQueryFreq );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32TmrQueryFreq, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32TMRQUERYFREQ( Parm1 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32TmrQueryFreq );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PULONG );
         T_LogPULONG( Parm1 );
      }
      HOOKEXIT( szDos32TmrQueryFreq, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32TmrQueryFreq() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16TmrQueryTime()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSTMRQUERYTIME( PQWORD16 );
#define DOSTMRQUERYTIME_PARMBYTES ( sizeof( PQWORD16 ) )
PCHAR szDos16TmrQueryTime = "Dos16TmrQueryTime";

APIRET16 APIENTRY16 Trc_Dos16TmrQueryTime( PQWORD16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSTMRQUERYTIME_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16TmrQueryTime, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PQWORD );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szDos16TmrQueryTime, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSTMRQUERYTIME( Parm1 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16TmrQueryTime );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PQWORD );
         if ( !usRC ) T_LogPQWORD16( Parm1 );
         else T_LogPVOID16( Parm1 );
      }
      HOOKEXIT16( szDos16TmrQueryTime, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16TmrQueryTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32TmrQueryTime()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32TMRQUERYTIME( PQWORD );
PCHAR szDos32TmrQueryTime = "Dos32TmrQueryTime";

APIRET APIENTRY Trc_Dos32TmrQueryTime( PQWORD Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32TmrQueryTime );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PQWORD );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szDos32TmrQueryTime, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32TMRQUERYTIME( Parm1 );

   if ( LOGDOSAPI( D_PRF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32TmrQueryTime );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PQWORD );
         if ( !ulRC ) T_LogPQWORD( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szDos32TmrQueryTime, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32TmrQueryTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32TransactNPipe()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32TRANSACTNPIPE( HPIPE, PVOID, ULONG, PVOID, ULONG, PULONG );
PCHAR szDos32TransactNPipe = "Dos32TransactNPipe";

APIRET APIENTRY Trc_Dos32TransactNPipe( HPIPE  Parm1
                                      , PVOID  Parm2
                                      , ULONG  Parm3
                                      , PVOID  Parm4
                                      , ULONG  Parm5
                                      , PULONG Parm6
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32TransactNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPIPE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogCHARBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szDos32TransactNPipe, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32TRANSACTNPIPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32TransactNPipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         if ( ( Parm6 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer( Parm4, *Parm6 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPULONG( Parm6 );
      }
      HOOKEXIT( szDos32TransactNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32TransactNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16UnlockSeg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSUNLOCKSEG( SEL );
#define DOSUNLOCKSEG_PARMBYTES ( sizeof( SEL ) )
PCHAR szDos16UnlockSeg = "Dos16UnlockSeg";

APIRET16 APIENTRY16 Trc_Dos16UnlockSeg( SEL Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSUNLOCKSEG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16UnlockSeg, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szDos16UnlockSeg, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSUNLOCKSEG( Parm1 );

   if ( LOGDOSAPI( D_MEM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16UnlockSeg );
      T_LogCPReturnCode( usRC );
      HOOKEXIT16( szDos16UnlockSeg, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16UnlockSeg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32UnsetExceptionHandler()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32UNSETEXCEPTIONHANDLER( PEXCEPTIONREGISTRATIONRECORD );
PCHAR szDos32UnsetExceptionHandler = "Dos32UnsetExceptionHandler";

APIRET APIENTRY Trc_Dos32UnsetExceptionHandler( PEXCEPTIONREGISTRATIONRECORD Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32UnsetExceptionHandler );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PEXCEPTIONREGISTRATIONRECORD );
         T_LogPEXCEPTIONREGISTRATIONRECORD( Parm1 );
      }
      HOOKENTRY( szDos32UnsetExceptionHandler, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32UNSETEXCEPTIONHANDLER( Parm1 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32UnsetExceptionHandler );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32UnsetExceptionHandler, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32UnsetExceptionHandler() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32UnwindException()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET APIENTRY DOS32UNWINDEXCEPTION( PEXCEPTIONREGISTRATIONRECORD, PVOID, PEXCEPTIONREPORTRECORD );
PCHAR szDos32UnwindException = "Dos32UnwindException";

APIRET APIENTRY Trc_Dos32UnwindException( PEXCEPTIONREGISTRATIONRECORD Parm1
                                        , PVOID                        Parm2
                                        , PEXCEPTIONREPORTRECORD       Parm3
                                        )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32UnwindException );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PEXCEPTIONREGISTRATIONRECORD );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPFN( (PFN)Parm2 );
         T_LogParameter( 3L, DT_PEXCEPTIONREPORTRECORD );
         T_LogPEXCEPTIONREPORTRECORD( Parm3 );
      }
      HOOKENTRY( szDos32UnwindException, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32UNWINDEXCEPTION( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_XCPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32UnwindException );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32UnwindException, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32UnwindException() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32VerifyPidTid()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
APIRET APIENTRY DOS32VERIFYPIDTID( PID, TID );
PCHAR szDos32VerifyPidTid = "Dos32VerifyPidTid";

APIRET APIENTRY Trc_Dos32VerifyPidTid( PID Parm1
                                     , TID Parm2
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32VerifyPidTid );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PID );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_TID );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32VerifyPidTid, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnDOS32VERIFYPIDTID )( Parm1, Parm2 );

   if ( LOGDOSAPI( D_MISC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32VerifyPidTid );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32VerifyPidTid, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32VerifyPidTid() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32WaitChild()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32WAITCHILD( ULONG, ULONG, PRESULTCODES, PPID, PID );
PCHAR szDos32WaitChild = "Dos32WaitChild";

APIRET APIENTRY Trc_Dos32WaitChild( ULONG        Parm1
                                  , ULONG        Parm2
                                  , PRESULTCODES Parm3
                                  , PPID         Parm4
                                  , PID          Parm5
                                  )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32WaitChild );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRESULTCODES );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_PPID );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PID );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szDos32WaitChild, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32WAITCHILD( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32WaitChild );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm2 == DCWW_WAIT ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 3L, DT_PRESULTCODES );
         if ( !ulRC ) T_LogPRESULTCODES( Parm3 );
         else T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_PPID );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32WaitChild, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32WaitChild() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32WaitEventSem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32WAITEVENTSEM( HEV, ULONG );
PCHAR szDos32WaitEventSem = "Dos32WaitEventSem";

APIRET APIENTRY Trc_Dos32WaitEventSem( HEV   Parm1
                                     , ULONG Parm2
                                     )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32WaitEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32WaitEventSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32WAITEVENTSEM( Parm1, Parm2 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32WaitEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32WaitEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32WaitEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32WaitMuxWaitSem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32WAITMUXWAITSEM( HMUX, ULONG, PULONG );
PCHAR szDos32WaitMuxWaitSem = "Dos32WaitMuxWaitSem";

APIRET APIENTRY Trc_Dos32WaitMuxWaitSem( HMUX   Parm1
                                       , ULONG  Parm2
                                       , PULONG Parm3
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_SEM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32WaitMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMUX );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32WaitMuxWaitSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32WAITMUXWAITSEM( Parm1, Parm2, Parm3 );

   if ( LOGDOSAPI( D_SEM ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32WaitMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szDos32WaitMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32WaitMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32WaitNPipe()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32WAITNPIPE( PSZ, ULONG );
PCHAR szDos32WaitNPipe = "Dos32WaitNPipe";

APIRET APIENTRY Trc_Dos32WaitNPipe( PSZ   Parm1
                                  , ULONG Parm2
                                  )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_PIPE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32WaitNPipe );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32WaitNPipe, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32WAITNPIPE( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PIPE ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32WaitNPipe );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szDos32WaitNPipe, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32WaitNPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32WaitThread()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32WAITTHREAD( PTID, ULONG );
PCHAR szDos32WaitThread = "Dos32WaitThread";

APIRET APIENTRY Trc_Dos32WaitThread( PTID  Parm1
                                   , ULONG Parm2
                                   )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGDOSAPI( D_PROC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32WaitThread );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PTID );
         T_LogPULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32WaitThread, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtEntry );
   }

   ulRC = DOS32WAITTHREAD( Parm1, Parm2 );

   if ( LOGDOSAPI( D_PROC ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DOS32GETDATETIME( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szDos32WaitThread );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm2 == DCWW_WAIT ) && ( !ulRC ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PTID );
         T_LogPULONG( Parm1 );
      }
      HOOKEXIT( szDos32WaitThread, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32WaitThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16Write()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSWRITE( HFILE16, PVOID16, USHORT, PUSHORT16 );
#define DOSWRITE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16Write = "Dos16Write";

APIRET16 APIENTRY16 Trc_Dos16Write( HFILE16   Parm1
                                  , PVOID16   Parm2
                                  , USHORT    Parm3
                                  , PUSHORT16 Parm4
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSWRITE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16Write, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogCHARBuffer16( Parm2, Parm3 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDos16Write, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSWRITE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16Write );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PUSHORT );
         T_LogPUSHORT16( Parm4 );
      }
      HOOKEXIT16( szDos16Write, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16Write() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32Write()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32WRITE( HFILE, PVOID, ULONG, PULONG );
PCHAR szDos32Write = "Dos32Write";

APIRET APIENTRY Trc_Dos32Write( HFILE  Parm1
                              , PVOID  Parm2
                              , ULONG  Parm3
                              , PULONG Parm4
                              )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32Write );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogCHARBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32Write, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32WRITE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32Write );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32Write, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32Write() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16WriteAsync()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSWRITEASYNC( HFILE16, PULONG16, PUSHORT16, PVOID16, USHORT, PUSHORT16 );
#define DOSWRITEASYNC_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PULONG16 ) + sizeof( PUSHORT16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16WriteAsync = "Dos16WriteAsync";

APIRET16 APIENTRY16 Trc_Dos16WriteAsync( HFILE16   Parm1
                                       , PULONG16  Parm2
                                       , PUSHORT16 Parm3
                                       , PVOID16   Parm4
                                       , USHORT    Parm5
                                       , PUSHORT16 Parm6
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSWRITEASYNC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogBaseAPIEntry16( szDos16WriteAsync, sRegs16.ulCSIP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG16( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( Parm4, Parm5 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szDos16WriteAsync, &Parm1 );
      T_FreeLog( );
   }

   usRC = DOSWRITEASYNC( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos16WriteAsync );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
         T_LogParameter( 6L, DT_PUSHORT );
         T_LogPUSHORT16( Parm6 );
      }
      HOOKEXIT16( szDos16WriteAsync, &Parm1, &usRC );
      T_FreeLog( );
   }

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16WriteAsync() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Temp32Write()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET APIENTRY TEMP32WRITE( ULONG, PCHAR, ULONG, PULONG );
PCHAR szTemp32Write = "Temp32Write";

APIRET APIENTRY Trc_Temp32Write( ULONG  Parm1
                               , PCHAR  Parm2
                               , ULONG  Parm3
                               , PULONG Parm4
                               )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szTemp32Write );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PCHAR );
         T_LogPVOID( Parm2 );
         T_LogCHARBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szTemp32Write, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnTEMP32WRITE )( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_FILE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szTemp32Write );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szTemp32Write, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Temp32Write() */
#endif

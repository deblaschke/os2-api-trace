/******************************************************************************/
/*                                                                            */
/* File name    : T_CTL_.C                                                    */
/*                                                                            */
/* Title        : OS/2 API trace DLL 32-bit PMCTLS routines                   */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 32-bit APIs imported from PMCTLS.DLL.                */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.03 20Jul95 Added 32-bit Win APIs                       */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.45 28Jan98 Added indeterminate API return codes        */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.76 28Oct98 Issued more specific trace DLL load errors  */
/*                2.45.10 02Aug99 Moved common strings into T_COMMON          */
/*                2.45.32 28Jun01 Added -U option                             */
/*                2.45.34 10Jul01 Moved C run-time functions into T_COMMON    */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#define  INCL_BASE                               /* Include base functions    */
#define  INCL_PM                                 /* Include PM functions      */
#include <os2.h>                                 /* OS/2 information          */
#include "os2trace.h"                            /* OS2TRACE information      */
#include "t_common.h"                            /* T_COMMON information      */

/******************************************************************************/
/*                                                                            */
/* Constant                                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
#define ORD_WINREGISTERCIRCULARSLIDER 24         /* OS/2 2.30 proc ordinal    */
#endif
#if FALSE
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
#define ORD_WINCOLORDLG               294        /* OS/2 2.40 proc ordinal    */
#define ORD_WINDEFCOLORDLGPROC        293        /* OS/2 2.40 proc ordinal    */
#define ORD_WINDEFFINDDLGPROC         297        /* OS/2 2.40 proc ordinal    */
#define ORD_WINDEFPRINTDLGPROC        301        /* OS/2 2.40 proc ordinal    */
#define ORD_WINDEFREPLACEDLGPROC      299        /* OS/2 2.40 proc ordinal    */
#define ORD_WINFINDDLG                296        /* OS/2 2.40 proc ordinal    */
#define ORD_WINPRINTDLG               300        /* OS/2 2.40 proc ordinal    */
#define ORD_WINREPLACEDLG             298        /* OS/2 2.40 proc ordinal    */
#endif
#endif

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
/* Variable definitions                                                       */
/*                                                                            */
/******************************************************************************/

HMODULE hmodDLL;                                 /* DLL module handle         */
HMODULE hmodTraceDLL;                            /* Trace DLL module handle   */
PCHAR   szDLL = "PMCTLS";                        /* DLL module name           */
PCHAR   szTraceDLL = "T_CTL_";                   /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( OS2VER >= 230 )
BOOL (* APIENTRY    pfnWINREGISTERCIRCULARSLIDER)( VOID );
                                                 /* OS/2 2.30 procedure       */
#endif
#if FALSE
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG   (* APIENTRY pfnWINCOLORDLG)( HWND, HWND, PCHOOSECOLOR );
                                                 /* OS/2 2.40 procedure       */
MRESULT (* APIENTRY pfnWINDEFCOLORDLGPROC)( HWND, ULONG, MPARAM, MPARAM );
                                                 /* OS/2 2.40 procedure       */
MRESULT (* APIENTRY pfnWINDEFFINDDLGPROC)( HWND, ULONG, MPARAM, MPARAM );
                                                 /* OS/2 2.40 procedure       */
MRESULT (* APIENTRY pfnWINDEFPRINTDLGPROC)( HWND, ULONG, MPARAM, MPARAM );
                                                 /* OS/2 2.40 procedure       */
MRESULT (* APIENTRY pfnWINDEFREPLACEDLGPROC)( HWND, ULONG, MPARAM, MPARAM );
                                                 /* OS/2 2.40 procedure       */
HWND    (* APIENTRY pfnWINFINDDLG)( HWND, HWND, PFINDDLG );
                                                 /* OS/2 2.40 procedure       */
HWND    (* APIENTRY pfnWINPRINTDLG)( HWND, HWND, PPRINTDLG );
                                                 /* OS/2 2.40 procedure       */
HWND    (* APIENTRY pfnWINREPLACEDLG)( HWND, HWND, PFINDDLG );
                                                 /* OS/2 2.40 procedure       */
#endif
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

   CHAR  szObjName[ LEN_FILENAME ];              /* Failing object name       */
   ULONG ulOrdinal;                              /* Procedure ordinal         */

   /*
    * Library initialization required if process attaching to T_CTL_
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
       * Log error and terminate if T_CTL_ version does not match T_COMMON
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
       * Load PMCTLS library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_CTL_ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMCTLS trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

#if ( OS2VER >= 230 )
      /*
       * Obtain OS/2 2.30+ procedure address if OS/2 version number greater
       * than or equal to 2.30
       */
      if ( T_OS2Ver >= MAKEVER( 2, 30 ) )
      {
         /*
          * Obtain OS/2 2.30 procedure address, log error and terminate if
          * unsuccessful
          */
         if ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINREGISTERCIRCULARSLIDER, NULL, (PFN *)&pfnWINREGISTERCIRCULARSLIDER ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 30, szDLL, ulOrdinal );
         }
      }
#endif

#if FALSE
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
      /*
       * Obtain OS/2 2.40+ procedure addresses if OS/2 version number greater
       * than or equal to 2.40
       */
      if ( T_OS2Ver >= MAKEVER( 2, 40 ) )
      {
         /*
          * Obtain OS/2 2.40 procedure addresses, log error and terminate if
          * unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINCOLORDLG         , NULL, (PFN *)&pfnWINCOLORDLG          ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINDEFCOLORDLGPROC  , NULL, (PFN *)&pfnWINDEFCOLORDLGPROC   ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINDEFFINDDLGPROC   , NULL, (PFN *)&pfnWINDEFFINDDLGPROC    ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINDEFPRINTDLGPROC  , NULL, (PFN *)&pfnWINDEFPRINTDLGPROC   ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINDEFREPLACEDLGPROC, NULL, (PFN *)&pfnWINDEFREPLACEDLGPROC ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINFINDDLG          , NULL, (PFN *)&pfnWINFINDDLG           ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINPRINTDLG         , NULL, (PFN *)&pfnWINPRINTDLG          ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINREPLACEDLG       , NULL, (PFN *)&pfnWINREPLACEDLG        ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 40, szDLL, ulOrdinal );
         }
      }
#endif
#endif
   }

   /*
    * Library termination required if process detaching from T_CTL_
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMCTLS library if library loaded
       */
      if ( hmodDLL )
      {
         DosFreeModule( hmodDLL );
      }

#ifndef T_COMRUNTIME
#ifdef INTEL
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

#if FALSE /* Changed from public to private API                               */
/******************************************************************************/
/*                                                                            */
/* Trc_Win32DefColorDlgProc()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
MRESULT APIENTRY WINDEFCOLORDLGPROC( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32DefColorDlgProc = "Win32ColorileDlgProc";

MRESULT APIENTRY Trc_Win32DefColorDlgProc( HWND   Parm1
                                         , ULONG  Parm2
                                         , MPARAM Parm3
                                         , MPARAM Parm4
                                         )
{

   MRESULT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DefColorDlgProc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPMMessage( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY( szWin32DefColorDlgProc, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINDEFCOLORDLGPROC )( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DefColorDlgProc );
   T_LogPMReturnCode( DT_MRESULT, (ULONG)ulRC, (BOOL)ulRC );
   HOOKEXIT( szWin32DefColorDlgProc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32DefColorDlgProc() */
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DefFileDlgProc()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY WINDEFFILEDLGPROC( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32DefFileDlgProc = "Win32DefFileDlgProc";

MRESULT APIENTRY Trc_Win32DefFileDlgProc( HWND   Parm1
                                        , ULONG  Parm2
                                        , MPARAM Parm3
                                        , MPARAM Parm4
                                        )
{

   MRESULT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DefFileDlgProc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPMMessage( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY( szWin32DefFileDlgProc, &Parm1 );
   T_FreeLog( );

   ulRC = WINDEFFILEDLGPROC( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DefFileDlgProc );
   T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
   HOOKEXIT( szWin32DefFileDlgProc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32DefFileDlgProc() */
#endif

#if FALSE /* Changed from public to private API                               */
/******************************************************************************/
/*                                                                            */
/* Trc_Win32DefFindDlgProc()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
MRESULT APIENTRY WINDEFFINDDLGPROC( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32DefFindDlgProc = "Win32DefFindDlgProc";

MRESULT APIENTRY Trc_Win32DefFindDlgProc( HWND   Parm1
                                        , ULONG  Parm2
                                        , MPARAM Parm3
                                        , MPARAM Parm4
                                        )
{

   MRESULT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DefFindDlgProc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPMMessage( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY( szWin32DefFindDlgProc, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINDEFFINDDLGPROC )( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DefFindDlgProc );
   T_LogPMReturnCode( DT_MRESULT, (ULONG)ulRC, (BOOL)ulRC );
   HOOKEXIT( szWin32DefFindDlgProc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32DefFindDlgProc() */
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DefFontDlgProc()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY WINDEFFONTDLGPROC( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32DefFontDlgProc = "Win32DefFontDlgProc";

MRESULT APIENTRY Trc_Win32DefFontDlgProc( HWND   Parm1
                                        , ULONG  Parm2
                                        , MPARAM Parm3
                                        , MPARAM Parm4
                                        )
{

   MRESULT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DefFontDlgProc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPMMessage( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY( szWin32DefFontDlgProc, &Parm1 );
   T_FreeLog( );

   ulRC = WINDEFFONTDLGPROC( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DefFontDlgProc );
   T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
   HOOKEXIT( szWin32DefFontDlgProc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32DefFontDlgProc() */
#endif

#if FALSE /* Changed from public to private API                               */
/******************************************************************************/
/*                                                                            */
/* Trc_Win32DefPrintDlgProc()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
MRESULT APIENTRY WINDEFPRINTDLGPROC( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32DefPrintDlgProc = "Win32DefPrintDlgProc";

MRESULT APIENTRY Trc_Win32DefPrintDlgProc( HWND   Parm1
                                         , ULONG  Parm2
                                         , MPARAM Parm3
                                         , MPARAM Parm4
                                         )
{

   MRESULT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DefPrintDlgProc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPMMessage( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY( szWin32DefPrintDlgProc, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINDEFPRINTDLGPROC )( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DefPrintDlgProc );
   T_LogPMReturnCode( DT_MRESULT, (ULONG)ulRC, (BOOL)ulRC );
   HOOKEXIT( szWin32DefPrintDlgProc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32DefPrintDlgProc() */
#endif
#endif

#if FALSE /* Changed from public to private API                               */
/******************************************************************************/
/*                                                                            */
/* Trc_Win32DefReplaceDlgProc()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
MRESULT APIENTRY WINDEFREPLACEDLGPROC( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32DefReplaceDlgProc = "Win32DefReplaceDlgProc";

MRESULT APIENTRY Trc_Win32DefReplaceDlgProc( HWND   Parm1
                                         , ULONG  Parm2
                                         , MPARAM Parm3
                                         , MPARAM Parm4
                                         )
{

   MRESULT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DefReplaceDlgProc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPMMessage( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY( szWin32DefReplaceDlgProc, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINDEFREPLACEDLGPROC )( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DefReplaceDlgProc );
   T_LogPMReturnCode( DT_MRESULT, (ULONG)ulRC, (BOOL)ulRC );
   HOOKEXIT( szWin32DefReplaceDlgProc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32DefReplaceDlgProc() */
#endif
#endif

#if FALSE /* Changed from public to private API                               */
/******************************************************************************/
/*                                                                            */
/* Trc_Win32ColorDlg()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY WINCOLORDLG( HWND, HWND, PCHOOSECOLOR );
PCHAR szWin32ColorDlg = "Win32ColorDlg";

ULONG APIENTRY Trc_Win32ColorDlg( HWND         Parm1
                                , HWND         Parm2
                                , PCHOOSECOLOR Parm3
                                )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32ColorDlg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PCHOOSECOLOR );
      T_LogPCHOOSECOLOR( Parm3 );
   }
   HOOKENTRY( szWin32ColorDlg, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINCOLORDLG )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32ColorDlg );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHOOSECOLOR );
      if ( ulRC ) T_LogPCHOOSECOLOR( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szWin32ColorDlg, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32ColorDlg() */
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FileDlg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WINFILEDLG( HWND, HWND, PFILEDLG );
PCHAR szWin32FileDlg = "Win32FileDlg";

HWND APIENTRY Trc_Win32FileDlg( HWND     Parm1
                              , HWND     Parm2
                              , PFILEDLG Parm3
                              )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32FileDlg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PFILEDLG );
      T_LogPFILEDLG( Parm3 );
   }
   HOOKENTRY( szWin32FileDlg, &Parm1 );
   T_FreeLog( );

   ulRC = WINFILEDLG( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32FileDlg );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PFILEDLG );
      if ( ulRC ) T_LogPFILEDLG( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szWin32FileDlg, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32FileDlg() */
#endif

#if FALSE /* Changed from public to private API                               */
/******************************************************************************/
/*                                                                            */
/* Trc_Win32FindDlg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
HWND APIENTRY WINFINDDLG( HWND, HWND, PFINDDLG );
PCHAR szWin32FindDlg = "Win32FindDlg";

HWND APIENTRY Trc_Win32FindDlg( HWND     Parm1
                              , HWND     Parm2
                              , PFINDDLG Parm3
                              )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32FindDlg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PFINDDLG );
      T_LogPFINDDLG( Parm3 );
   }
   HOOKENTRY( szWin32FindDlg, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINFINDDLG )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32FindDlg );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PFINDDLG );
      if ( ulRC ) T_LogPFINDDLG( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szWin32FindDlg, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32FindDlg() */
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FontDlg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WINFONTDLG( HWND, HWND, PFONTDLG );
PCHAR szWin32FontDlg = "Win32FontDlg";

HWND APIENTRY Trc_Win32FontDlg( HWND     Parm1
                              , HWND     Parm2
                              , PFONTDLG Parm3
                              )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32FontDlg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PFONTDLG );
      T_LogPFONTDLG( Parm3 );
   }
   HOOKENTRY( szWin32FontDlg, &Parm1 );
   T_FreeLog( );

   ulRC = WINFONTDLG( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32FontDlg );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PFONTDLG );
      if ( ulRC ) T_LogPFONTDLG( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szWin32FontDlg, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32FontDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FreeFileDlgList()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINFREEFILEDLGLIST( PAPSZ );
PCHAR szWin32FreeFileDlgList = "Win32FreeFileDlgList";

BOOL APIENTRY Trc_Win32FreeFileDlgList( PAPSZ Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32FreeFileDlgList );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PAPSZ );
      T_LogPAPSZ( (PSZ *)Parm1 );
   }
   HOOKENTRY( szWin32FreeFileDlgList, &Parm1 );
   T_FreeLog( );

   bRC = WINFREEFILEDLGLIST( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32FreeFileDlgList );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32FreeFileDlgList, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32FreeFileDlgList() */
#endif

#if FALSE /* Changed from public to private API                               */
/******************************************************************************/
/*                                                                            */
/* Trc_Win32PrintDlg()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
HWND APIENTRY WINPRINTDLG( HWND, HWND, PPRINTDLG );
PCHAR szWin32PrintDlg = "Win32PrintDlg";

HWND APIENTRY Trc_Win32PrintDlg( HWND      Parm1
                               , HWND      Parm2
                               , PPRINTDLG Parm3
                               )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32PrintDlg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPRINTDLG );
      T_LogPPRINTDLG( Parm3 );
   }
   HOOKENTRY( szWin32PrintDlg, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINPRINTDLG )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32PrintDlg );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PPRINTDLG );
      if ( ulRC ) T_LogPPRINTDLG( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szWin32PrintDlg, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32PrintDlg() */
#endif
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RegisterCircularSlider()                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY WINREGISTERCIRCULARSLIDER( VOID );
PCHAR szWin32RegisterCircularSlider = "Win32RegisterCircularSlider";

BOOL APIENTRY Trc_Win32RegisterCircularSlider( VOID )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32RegisterCircularSlider );
   HOOKENTRY( szWin32RegisterCircularSlider, NULL );
   T_FreeLog( );

   bRC = ( *pfnWINREGISTERCIRCULARSLIDER )( );

   T_WaitLog( );
   T_LogAPIExit( szWin32RegisterCircularSlider );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32RegisterCircularSlider, NULL, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32RegisterCircularSlider() */
#endif

#if FALSE /* Changed from public to private API                               */
/******************************************************************************/
/*                                                                            */
/* Trc_Win32ReplaceDlg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
HWND APIENTRY WINREPLACEDLG( HWND, HWND, PFINDDLG );
PCHAR szWin32ReplaceDlg = "Win32ReplaceDlg";

HWND APIENTRY Trc_Win32ReplaceDlg( HWND     Parm1
                                 , HWND     Parm2
                                 , PFINDDLG Parm3
                                 )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32ReplaceDlg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PFINDDLG );
      T_LogPFINDDLG( Parm3 );
   }
   HOOKENTRY( szWin32ReplaceDlg, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINREPLACEDLG )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32ReplaceDlg );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PFINDDLG );
      if ( ulRC ) T_LogPFINDDLG( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szWin32ReplaceDlg, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32ReplaceDlg() */
#endif
#endif

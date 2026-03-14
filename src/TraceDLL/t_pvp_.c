/******************************************************************************/
/*                                                                            */
/* File name    : T_PVP_.C                                                    */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit PMVIOP routines                   */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit APIs imported from PMVIOP.DLL.                */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.47 29Jan98 Added Win16DefAVioWindowProc                */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.45.10 02Aug99 Moved common strings into T_COMMON          */
/*                2.45.31 26Jun01 Added  capability of building  32-bit  only */
/*                                trace DLLs                                  */
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

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#pragma stack16( 0 )
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
PCHAR   szDLL = "PMVIOP";                        /* DLL module name           */
PCHAR   szTraceDLL = "T_PVP_";                   /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

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

   CHAR szObjName[ LEN_FILENAME ];               /* Failing object name       */

   /*
    * Library initialization required if process attaching to T_PVP_
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
       * Log error and terminate if T_PVP_ version does not match T_COMMON
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
       * Load PMVIOP library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_PVP_ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMVIOP trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_PVP_
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMVIOP library if library loaded
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

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DefAVioWindowProc()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
MRESULT16 APIENTRY16 WINDEFAVIOWINDOWPROC( HWND, USHORT, MPARAM, MPARAM );
PCHAR szWin16DefAVioWindowProc = "Win16DefAVioWindowProc";

MRESULT16 APIENTRY16 Trc_Win16DefAVioWindowProc( HWND   Parm1
                                               , USHORT Parm2
                                               , MPARAM Parm3
                                               , MPARAM Parm4
                                               )
{

   MRESULT16 ulRC;                               /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16DefAVioWindowProc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogPMMessage16( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY16( szWin16DefAVioWindowProc, &Parm1 );
   T_FreeLog( );

   ulRC = WINDEFAVIOWINDOWPROC( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin16DefAVioWindowProc );
   T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
   HOOKEXIT16( szWin16DefAVioWindowProc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16DefAVioWindowProc() */
#endif

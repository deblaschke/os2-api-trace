/******************************************************************************/
/*                                                                            */
/* File name    : T_MRG__.C                                                   */
/*                                                                            */
/* Title        : OS/2 API trace DLL 32-bit PMMERGE routines                  */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 32-bit APIs imported from PMMERGE.DLL.               */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.76 28Oct98 Issued more specific trace DLL load errors  */
/*                2.45.00 15Apr99 Added new OS/2 2.45 APIs                    */
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
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 240 )
#define ORD_WIN32QUERYCONTROLCOLORS 5470         /* OS/2 2.40 proc ordinal    */
#define ORD_WIN32SETCONTROLCOLORS   5471         /* OS/2 2.40 proc ordinal    */
#endif
#if ( OS2VER >= 245 )
#define ORD_WIN32QUERYCPTYPE        5480         /* OS/2 2.45 proc ordinal    */
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
PCHAR   szDLL = "PMMERGE";                       /* DLL module name           */
PCHAR   szTraceDLL = "T_MRG__";                  /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG  (* APIENTRY pfnWIN32QUERYCONTROLCOLORS)( HWND, LONG, ULONG, ULONG, PCTLCOLOR );
                                                 /* OS/2 2.40 procedure       */
BOOL  (* APIENTRY pfnWIN32SETCONTROLCOLORS)( HWND, LONG, ULONG, ULONG, PCTLCOLOR );
                                                 /* OS/2 2.40 procedure       */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
ULONG (* APIENTRY pfnWIN32QUERYCPTYPE)( HMQ );
                                                 /* OS/2 2.45 procedure       */
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
    * Library initialization required if process attaching to T_MRG__
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
       * Log error and terminate if T_MRG__ version does not match T_COMMON
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
       * Load PMMERGE library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_MRG__ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMMERGE trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

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
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32QUERYCONTROLCOLORS, NULL, (PFN *)&pfnWIN32QUERYCONTROLCOLORS ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32SETCONTROLCOLORS  , NULL, (PFN *)&pfnWIN32SETCONTROLCOLORS   ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 40, szDLL, ulOrdinal );
         }
      }
#endif

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
      /*
       * Obtain OS/2 2.45 procedure address if OS/2 version number greater than
       * or equal to 2.45
       */
      if ( T_OS2Ver >= MAKEVER( 2, 45 ) )
      {
         /*
          * Obtain OS/2 2.45 procedure address, log error and terminate if
          * unsuccessful
          */
         if ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32QUERYCPTYPE, NULL, (PFN *)&pfnWIN32QUERYCPTYPE ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 45, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_MRG__
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMMERGE library if library loaded
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
/* Trc_Win32QueryControlColors()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG APIENTRY WINQUERYCONTROLCOLORS( HWND, LONG, ULONG, ULONG, PCTLCOLOR );
PCHAR szWin32QueryControlColors = "Win32QueryControlColors";

#define CCF_GLOBAL      0x0000
#define CCF_APPLICATION 0x0001
#define CCF_COUNTCOLORS 0x0010
#define CCF_ALLCOLORS   0x0020

LONG APIENTRY Trc_Win32QueryControlColors( HWND      Parm1
                                         , LONG      Parm2
                                         , ULONG     Parm3
                                         , ULONG     Parm4
                                         , PCTLCOLOR Parm5
                                         )
{

   LONG lRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryControlColors );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PCTLCOLOR );
      T_LogPVOID( Parm5 );
      if ( FLAGCLR( Parm3, CCF_COUNTCOLORS | CCF_ALLCOLORS ) ) T_LogCTLCOLORBuffer( Parm5, Parm4 );
   }
   HOOKENTRY( szWin32QueryControlColors, &Parm1 );
   T_FreeLog( );

   lRC = ( *pfnWIN32QUERYCONTROLCOLORS )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryControlColors );
   T_LogPMReturnCode( DT_LONG, lRC, lRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PCTLCOLOR );
      T_LogPVOID( Parm5 );
      if ( FLAGCLR( Parm3, CCF_COUNTCOLORS ) ) T_LogCTLCOLORBuffer( Parm5, lRC );
   }
   HOOKEXIT( szWin32QueryControlColors, &Parm1, &lRC );
   T_FreeLog( );

   return lRC;

}  /* Trc_Win32QueryControlColors() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryCpType()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
ULONG APIENTRY WINQUERYCPTYPE( HMQ );
PCHAR szWin32QueryCpType = "Win32QueryCpType";

ULONG APIENTRY Trc_Win32QueryCpType( HMQ Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryCpType );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HMQ );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32QueryCpType, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN32QUERYCPTYPE )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryCpType );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szWin32QueryCpType, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QueryCpType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetControlColors()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
BOOL APIENTRY WINSETCONTROLCOLORS( HWND, LONG, ULONG, ULONG, PCTLCOLOR );
PCHAR szWin32SetControlColors = "Win32SetControlColors";

BOOL APIENTRY Trc_Win32SetControlColors( HWND      Parm1
                                       , LONG      Parm2
                                       , ULONG     Parm3
                                       , ULONG     Parm4
                                       , PCTLCOLOR Parm5
                                       )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SetControlColors );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PCTLCOLOR );
      T_LogPVOID( Parm5 );
      T_LogCTLCOLORBuffer( Parm5, Parm4 );
   }
   HOOKENTRY( szWin32SetControlColors, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnWIN32SETCONTROLCOLORS )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SetControlColors );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32SetControlColors, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32SetControlColors() */
#endif

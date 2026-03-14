/******************************************************************************/
/*                                                                            */
/* File name    : T_MON___.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit MONCALLS routines                 */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit APIs imported from MONCALLS.DLL.              */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.21 20May97 Fixed/minimized 16-bit stack usage          */
/*                2.40.25 04Jun97 Fixed 16-bit registers structure init bug   */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.50 06Feb98 Fixed CX and DX  corruption by  16-bit Kbd, */
/*                                Mou, and Vio APIs                           */
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
PCHAR   szDLL = "MONCALLS";                      /* DLL module name           */
PCHAR   szTraceDLL = "T_MON___";                 /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_MON___
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
       * Log error and terminate if T_MON___ version does not match T_COMMON
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
       * Load MONCALLS library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_MON___ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register MONCALLS trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_MON___
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload MONCALLS library if library loaded
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
/* Trc_Dos16MonClose()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMONCLOSE( HMONITOR16 );
#define DOSMONCLOSE_PARMBYTES ( sizeof( HMONITOR16 ) )
PCHAR szDos16MonClose = "Dos16MonClose";

APIRET16 APIENTRY16 Trc_Dos16MonClose( HMONITOR16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMONCLOSE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16MonClose, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HMONITOR );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szDos16MonClose, &Parm1 );
   T_FreeLog( );

   usRC = DOSMONCLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos16MonClose );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16MonClose, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MonClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MonOpen()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMONOPEN( PSZ16, PHMONITOR16 );
#define DOSMONOPEN_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHMONITOR16 ) )
PCHAR szDos16MonOpen = "Dos16MonOpen";

APIRET16 APIENTRY16 Trc_Dos16MonOpen( PSZ16       Parm1
                                    , PHMONITOR16 Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMONOPEN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16MonOpen, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PHMONITOR );
      T_LogPVOID16( Parm2 );
   }
   HOOKENTRY16( szDos16MonOpen, &Parm1 );
   T_FreeLog( );

   usRC = DOSMONOPEN( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16MonOpen );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PHMONITOR );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szDos16MonOpen, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MonOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MonRead()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMONREAD( PBYTE16, USHORT, PBYTE16, PUSHORT16 );
#define DOSMONREAD_PARMBYTES ( sizeof( PBYTE16 ) + sizeof( USHORT ) + sizeof( PBYTE16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16MonRead = "Dos16MonRead";

APIRET16 APIENTRY16 Trc_Dos16MonRead( PBYTE16   Parm1
                                    , USHORT    Parm2
                                    , PBYTE16   Parm3
                                    , PUSHORT16 Parm4
                                    )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMONREAD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16MonRead, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PBYTE );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
   }
   HOOKENTRY16( szDos16MonRead, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = DOSMONREAD( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szDos16MonRead );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm2 == DCWW_WAIT ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      if ( ( Parm4 ) && ( !usRC ) ) T_LogCHARBuffer16( (PCHAR16)Parm3, *Parm4 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
   }
   HOOKEXIT16( szDos16MonRead, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MonRead() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MonReg()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMONREG( HMONITOR16, PBYTE16, PBYTE16, USHORT, USHORT );
#define DOSMONREG_PARMBYTES ( sizeof( HMONITOR16 ) + sizeof( PBYTE16 ) + sizeof( PBYTE16 ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szDos16MonReg = "Dos16MonReg";

APIRET16 APIENTRY16 Trc_Dos16MonReg( HMONITOR16 Parm1
                                   , PBYTE16    Parm2
                                   , PBYTE16    Parm3
                                   , USHORT     Parm4
                                   , USHORT     Parm5
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMONREG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16MonReg, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HMONITOR );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PBYTE );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szDos16MonReg, &Parm1 );
   T_FreeLog( );

   usRC = DOSMONREG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szDos16MonReg );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16MonReg, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MonReg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MonWrite()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMONWRITE( PBYTE16, PBYTE16, USHORT );
#define DOSMONWRITE_PARMBYTES ( sizeof( PBYTE16 ) + sizeof( PBYTE16 ) + sizeof( USHORT ) )
PCHAR szDos16MonWrite = "Dos16MonWrite";

APIRET16 APIENTRY16 Trc_Dos16MonWrite( PBYTE16 Parm1
                                     , PBYTE16 Parm2
                                     , USHORT  Parm3
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMONWRITE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16MonWrite, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PBYTE );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PBYTE );
      T_LogPVOID16( Parm2 );
      T_LogCHARBuffer16( (PCHAR16)Parm2, Parm3 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szDos16MonWrite, &Parm1 );
   T_FreeLog( );

   usRC = DOSMONWRITE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16MonWrite );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16MonWrite, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MonWrite() */
#endif

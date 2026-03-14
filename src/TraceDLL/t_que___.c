/******************************************************************************/
/*                                                                            */
/* File name    : T_QUE___.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit QUECALLS routines      */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from QUECALLS.DLL.   */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.00 12Jun95 Created (supported LX format and 32-bit Dos */
/*                                APIs only)                                  */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
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
PCHAR   szDLL = "QUECALLS";                      /* DLL module name           */
PCHAR   szTraceDLL = "T_QUE___";                 /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_QUE___
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
       * Log error and terminate if T_QUE___ version does not match T_COMMON
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
       * Load QUECALLS library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_QUE___ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register QUECALLS trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_QUE___
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload QUECALLS library if library loaded
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
/* Trc_Dos16CloseQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCLOSEQUEUE( HQUEUE16 );
#define DOSCLOSEQUEUE_PARMBYTES ( sizeof( HQUEUE16 ) )
PCHAR szDos16CloseQueue = "Dos16CloseQueue";

APIRET16 APIENTRY16 Trc_Dos16CloseQueue( HQUEUE16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCLOSEQUEUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16CloseQueue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szDos16CloseQueue, &Parm1 );
   T_FreeLog( );

   usRC = DOSCLOSEQUEUE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos16CloseQueue );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16CloseQueue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CloseQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CloseQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CLOSEQUEUE( HQUEUE );
PCHAR szDos32CloseQueue = "Dos32CloseQueue";

APIRET APIENTRY Trc_Dos32CloseQueue( HQUEUE Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32CloseQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDos32CloseQueue, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32CLOSEQUEUE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos32CloseQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szDos32CloseQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32CloseQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16CreateQueue()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCREATEQUEUE( PHQUEUE16, USHORT, PSZ16 );
#define DOSCREATEQUEUE_PARMBYTES ( sizeof( PHQUEUE16 ) + sizeof( USHORT ) + sizeof( PSZ16 ) )
PCHAR szDos16CreateQueue = "Dos16CreateQueue";

APIRET16 APIENTRY16 Trc_Dos16CreateQueue( PHQUEUE16 Parm1
                                        , USHORT    Parm2
                                        , PSZ16     Parm3
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCREATEQUEUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16CreateQueue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PHQUEUE );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
   }
   HOOKENTRY16( szDos16CreateQueue, &Parm1 );
   T_FreeLog( );

   usRC = DOSCREATEQUEUE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16CreateQueue );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PHQUEUE );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szDos16CreateQueue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CreateQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32CreateQueue()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32CREATEQUEUE( PHQUEUE, ULONG, PSZ );
PCHAR szDos32CreateQueue = "Dos32CreateQueue";

APIRET APIENTRY Trc_Dos32CreateQueue( PHQUEUE Parm1
                                    , ULONG   Parm2
                                    , PSZ     Parm3
                                    )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32CreateQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PHQUEUE );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
   }
   HOOKENTRY( szDos32CreateQueue, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32CREATEQUEUE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos32CreateQueue );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PHQUEUE );
      T_LogPULONG( Parm1 );
   }
   HOOKEXIT( szDos32CreateQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32CreateQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16OpenQueue()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSOPENQUEUE( PUSHORT16, PHQUEUE16, PSZ16 );
#define DOSOPENQUEUE_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( PHQUEUE16 ) + sizeof( PSZ16 ) )
PCHAR szDos16OpenQueue = "Dos16OpenQueue";

APIRET16 APIENTRY16 Trc_Dos16OpenQueue( PUSHORT16 Parm1
                                      , PHQUEUE16 Parm2
                                      , PSZ16     Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSOPENQUEUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16OpenQueue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PHQUEUE );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
   }
   HOOKENTRY16( szDos16OpenQueue, &Parm1 );
   T_FreeLog( );

   usRC = DOSOPENQUEUE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16OpenQueue );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
      T_LogParameter( 2L, DT_PHQUEUE );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szDos16OpenQueue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16OpenQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32OpenQueue()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32OPENQUEUE( PPID, PHQUEUE, PSZ );
PCHAR szDos32OpenQueue = "Dos32OpenQueue";

APIRET APIENTRY Trc_Dos32OpenQueue( PPID    Parm1
                                  , PHQUEUE Parm2
                                  , PSZ     Parm3
                                  )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32OpenQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PPID );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_PHQUEUE );
      T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
   }
   HOOKENTRY( szDos32OpenQueue, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32OPENQUEUE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos32OpenQueue );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PPID );
      T_LogPULONG( Parm1 );
      T_LogParameter( 2L, DT_PHQUEUE );
      T_LogPULONG( Parm2 );
   }
   HOOKEXIT( szDos32OpenQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32OpenQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PeekQueue()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSPEEKQUEUE( HQUEUE16, PULONG16, PUSHORT16, PPVOID16, PUSHORT16, UCHAR, PBYTE16, ULONG );
#define DOSPEEKQUEUE_PARMBYTES ( sizeof( HQUEUE16 ) + sizeof( PULONG16 ) + sizeof( PUSHORT16 ) + sizeof( PPVOID16 ) + sizeof( PUSHORT16 ) + sizeof( /* UCHAR */ USHORT ) + sizeof( PBYTE16 ) + sizeof( ULONG ) )
PCHAR szDos16PeekQueue = "Dos16PeekQueue";

APIRET16 APIENTRY16 Trc_Dos16PeekQueue( HQUEUE16  Parm1
                                      , PULONG16  Parm2
                                      , PUSHORT16 Parm3
                                      , PPVOID16  Parm4
                                      , PUSHORT16 Parm5
                                      , UCHAR     Parm6
                                      , PBYTE16   Parm7
                                      , ULONG     Parm8
                                      )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPEEKQUEUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16PeekQueue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PULONG );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_PUSHORT );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_PPVOID );
      T_LogPVOID16( (PVOID16)Parm4 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
      T_LogParameter( 6L, DT_UCHAR );
      T_LogUCHAR( Parm6 );
      T_LogParameter( 7L, DT_PBYTE );
      T_LogPVOID16( Parm7 );
      T_LogParameter( 8L, DT_ULONG );
      T_LogULONG( Parm8 );
   }
   HOOKENTRY16( szDos16PeekQueue, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = DOSPEEKQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szDos16PeekQueue );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm6 == DCWW_WAIT ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 2L, DT_PULONG );
      T_LogPULONG16( Parm2 );
      T_LogParameter( 3L, DT_PUSHORT );
      T_LogPUSHORT16( Parm3 );
      T_LogParameter( 4L, DT_PPVOID );
      T_LogPULONG16( (PULONG16)Parm4 );
      if ( ( Parm3 ) && ( Parm4 ) && ( !usRC ) ) T_LogCHARBuffer16( (PCHAR16)*Parm4, *Parm3 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
      T_LogParameter( 7L, DT_PBYTE );
      T_LogPCHAR16( (PCHAR16)Parm7 );
   }
   HOOKEXIT16( szDos16PeekQueue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos32PeekQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PeekQueue()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32PEEKQUEUE( HQUEUE, PREQUESTDATA, PULONG, PPVOID, PULONG, BOOL32, PBYTE, HEV );
PCHAR szDos32PeekQueue = "Dos32PeekQueue";

APIRET APIENTRY Trc_Dos32PeekQueue( HQUEUE       Parm1
                                  , PREQUESTDATA Parm2
                                  , PULONG       Parm3
                                  , PPVOID       Parm4
                                  , PULONG       Parm5
                                  , BOOL32       Parm6
                                  , PBYTE        Parm7
                                  , HEV          Parm8
                                  )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32PeekQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PREQUESTDATA );
      T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_PULONG );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_PPVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
      T_LogParameter( 6L, DT_BOOL32 );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_PBYTE );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_HEV );
      T_LogULONG( Parm8 );
   }
   HOOKENTRY( szDos32PeekQueue, &Parm1 );
   T_FreeLog( );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   ulRC = DOS32PEEKQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );

   T_WaitLog( );
   T_LogAPIExit( szDos32PeekQueue );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm6 == DCWW_WAIT ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 2L, DT_PREQUESTDATA );
      if ( !ulRC ) T_LogPREQUESTDATA( Parm2 );
      else T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_PULONG );
      T_LogPULONG( Parm3 );
      T_LogParameter( 4L, DT_PPVOID );
      T_LogPULONG( (PULONG)Parm4 );
      if ( ( Parm3 ) && ( Parm4 ) && ( !ulRC ) ) T_LogCHARBuffer( (PCHAR)*Parm4, *Parm3 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
      T_LogParameter( 7L, DT_PBYTE );
      T_LogPCHAR( (PCHAR)Parm7 );
   }
   HOOKEXIT( szDos32PeekQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32PeekQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PurgeQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSPURGEQUEUE( HQUEUE16 );
#define DOSPURGEQUEUE_PARMBYTES ( sizeof( HQUEUE16 ) )
PCHAR szDos16PurgeQueue = "Dos16PurgeQueue";

APIRET16 APIENTRY16 Trc_Dos16PurgeQueue( HQUEUE16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPURGEQUEUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16PurgeQueue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szDos16PurgeQueue, &Parm1 );
   T_FreeLog( );

   usRC = DOSPURGEQUEUE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PurgeQueue );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PurgeQueue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

} /* Trc_Dos16PurgeQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PurgeQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32PURGEQUEUE( HQUEUE );
PCHAR szDos32PurgeQueue = "Dos32PurgeQueue";

APIRET APIENTRY Trc_Dos32PurgeQueue( HQUEUE Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32PurgeQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDos32PurgeQueue, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32PURGEQUEUE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos32PurgeQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szDos32PurgeQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

} /* Trc_Dos32PurgeQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QueryQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQUERYQUEUE( HQUEUE16, PUSHORT16 );
#define DOSQUERYQUEUE_PARMBYTES ( sizeof( HQUEUE16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16QueryQueue = "Dos16QueryQueue";

APIRET16 APIENTRY16 Trc_Dos16QueryQueue( HQUEUE16  Parm1
                                       , PUSHORT16 Parm2
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQUERYQUEUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16QueryQueue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
   }
   HOOKENTRY16( szDos16QueryQueue, &Parm1 );
   T_FreeLog( );

   usRC = DOSQUERYQUEUE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16QueryQueue );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szDos16QueryQueue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QueryQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYQUEUE( HQUEUE, PULONG );
PCHAR szDos32QueryQueue = "Dos32QueryQueue";

APIRET APIENTRY Trc_Dos32QueryQueue( HQUEUE Parm1
                                   , PULONG Parm2
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32QueryQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PULONG );
      T_LogPVOID( Parm2 );
   }
   HOOKENTRY( szDos32QueryQueue, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32QUERYQUEUE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos32QueryQueue );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PULONG );
      T_LogPULONG( Parm2 );
   }
   HOOKEXIT( szDos32QueryQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32QueryQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ReadQueue()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSREADQUEUE( HQUEUE16, PULONG16, PUSHORT16, PPVOID16, USHORT, UCHAR, PBYTE16, HSEM );
#define DOSREADQUEUE_PARMBYTES ( sizeof( HQUEUE16 ) + sizeof( PULONG16 ) + sizeof( PUSHORT16 ) + sizeof( PPVOID16 ) + sizeof( USHORT ) + sizeof( /* UCHAR */ USHORT ) + sizeof( PBYTE16 ) + sizeof( HSEM ) )
PCHAR szDos16ReadQueue = "Dos16ReadQueue";

APIRET16 APIENTRY16 Trc_Dos16ReadQueue( HQUEUE16  Parm1
                                      , PULONG16  Parm2
                                      , PUSHORT16 Parm3
                                      , PPVOID16  Parm4
                                      , USHORT    Parm5
                                      , UCHAR     Parm6
                                      , PBYTE16   Parm7
                                      , HSEM      Parm8
                                      )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSREADQUEUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16ReadQueue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PULONG );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_PUSHORT );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_PPVOID );
      T_LogPVOID16( (PVOID16)Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_UCHAR );
      T_LogUCHAR( Parm6 );
      T_LogParameter( 7L, DT_PBYTE );
      T_LogPVOID16( Parm7 );
      T_LogParameter( 8L, DT_HSEM );
      T_LogULONG( (ULONG)Parm8 );
   }
   HOOKENTRY16( szDos16ReadQueue, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = DOSREADQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szDos16ReadQueue );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm6 == DCWW_WAIT ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 2L, DT_PULONG );
      T_LogPULONG16( Parm2 );
      T_LogParameter( 3L, DT_PUSHORT );
      T_LogPUSHORT16( Parm3 );
      T_LogParameter( 4L, DT_PPVOID );
      T_LogPULONG16( (PULONG16)Parm4 );
      if ( ( Parm3 ) && ( Parm4 ) && ( !usRC ) ) T_LogCHARBuffer16( (PCHAR16)*Parm4, *Parm3 );
      T_LogParameter( 7L, DT_PBYTE );
      T_LogPCHAR16( (PCHAR16)Parm7 );
   }
   HOOKEXIT16( szDos16ReadQueue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ReadQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32ReadQueue()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32READQUEUE( HQUEUE, PREQUESTDATA, PULONG, PPVOID, ULONG, BOOL32, PBYTE, HEV );
PCHAR szDos32ReadQueue = "Dos32ReadQueue";

APIRET APIENTRY Trc_Dos32ReadQueue( HQUEUE       Parm1
                                  , PREQUESTDATA Parm2
                                  , PULONG       Parm3
                                  , PPVOID       Parm4
                                  , ULONG        Parm5
                                  , BOOL32       Parm6
                                  , PBYTE        Parm7
                                  , HEV          Parm8
                                  )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32ReadQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PREQUESTDATA );
      T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_PULONG );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_PPVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_BOOL32 );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_PBYTE );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_HEV );
      T_LogULONG( Parm8 );
   }
   HOOKENTRY( szDos32ReadQueue, &Parm1 );
   T_FreeLog( );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   ulRC = DOS32READQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );

   T_WaitLog( );
   T_LogAPIExit( szDos32ReadQueue );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm6 == DCWW_WAIT ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 2L, DT_PREQUESTDATA );
      if ( !ulRC ) T_LogPREQUESTDATA( Parm2 );
      else T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_PULONG );
      T_LogPULONG( Parm3 );
      T_LogParameter( 4L, DT_PPVOID );
      T_LogPULONG( (PULONG)Parm4 );
      if ( ( Parm3 ) && ( Parm4 ) && ( !ulRC ) ) T_LogCHARBuffer( (PCHAR)*Parm4, *Parm3 );
      T_LogParameter( 7L, DT_PBYTE );
      T_LogPCHAR( (PCHAR)Parm7 );
   }
   HOOKEXIT( szDos32ReadQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32ReadQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16WriteQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSWRITEQUEUE( HQUEUE16, USHORT, USHORT, PVOID16, UCHAR );
#define DOSWRITEQUEUE_PARMBYTES ( sizeof( HQUEUE16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( /* UCHAR */ USHORT ) )
PCHAR szDos16WriteQueue = "Dos16WriteQueue";

APIRET16 APIENTRY16 Trc_Dos16WriteQueue( HQUEUE16 Parm1
                                       , USHORT   Parm2
                                       , USHORT   Parm3
                                       , PVOID16  Parm4
                                       , UCHAR    Parm5
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSWRITEQUEUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16WriteQueue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      T_LogCHARBuffer16( (PCHAR16)Parm4, Parm3 );
      T_LogParameter( 5L, DT_UCHAR );
      T_LogUCHAR( Parm5 );
   }
   HOOKENTRY16( szDos16WriteQueue, &Parm1 );
   T_FreeLog( );

   usRC = DOSWRITEQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szDos16WriteQueue );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16WriteQueue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16WriteQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32WriteQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32WRITEQUEUE( HQUEUE, ULONG, ULONG, PVOID, ULONG );
PCHAR szDos32WriteQueue = "Dos32WriteQueue";

APIRET APIENTRY Trc_Dos32WriteQueue( HQUEUE Parm1
                                   , ULONG  Parm2
                                   , ULONG  Parm3
                                   , PVOID  Parm4
                                   , ULONG  Parm5
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32WriteQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HQUEUE );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogCHARBuffer( (PCHAR)Parm4, Parm3 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY( szDos32WriteQueue, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32WRITEQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szDos32WriteQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szDos32WriteQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32WriteQueue() */
#endif

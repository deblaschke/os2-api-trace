/******************************************************************************/
/*                                                                            */
/* File name    : T_PIP___.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit NAMPIPES routines                 */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit APIs imported from NAMPIPES.DLL.              */
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
PCHAR   szDLL = "NAMPIPES";                      /* DLL module name           */
PCHAR   szTraceDLL = "T_PIP___";                 /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_PIP___
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
       * Log error and terminate if T_PIP___ version does not match T_COMMON
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
       * Load NAMPIPES library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_PIP___ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register NAMPIPES trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_PIP___
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload NAMPIPES library if library loaded
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
/* Trc_Dos16CallNmPipe()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCALLNMPIPE( PSZ16, PVOID16, USHORT, PVOID16, USHORT, PUSHORT16, ULONG );
#define DOSCALLNMPIPE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( ULONG ) )
PCHAR szDos16CallNmPipe = "Dos16CallNmPipe";

APIRET16 APIENTRY16 Trc_Dos16CallNmPipe( PSZ16     Parm1
                                       , PVOID16   Parm2
                                       , USHORT    Parm3
                                       , PVOID16   Parm4
                                       , USHORT    Parm5
                                       , PUSHORT16 Parm6
                                       , ULONG     Parm7
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCALLNMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16CallNmPipe, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID16( Parm2 );
      T_LogCHARBuffer16( Parm2, Parm3 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
      T_LogParameter( 7L, DT_ULONG );
      T_LogULONG( Parm7 );
   }
   HOOKENTRY16( szDos16CallNmPipe, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = DOSCALLNMPIPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szDos16CallNmPipe );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer16( Parm4, *Parm6 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16CallNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CallNmPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16ConnectNmPipe()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCONNECTNMPIPE( HPIPE16 );
#define DOSCONNECTNMPIPE_PARMBYTES ( sizeof( HPIPE16 ) )
PCHAR szDos16ConnectNmPipe = "Dos16ConnectNmPipe";

APIRET16 APIENTRY16 Trc_Dos16ConnectNmPipe( HPIPE16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCONNECTNMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16ConnectNmPipe, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPIPE );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szDos16ConnectNmPipe, &Parm1 );
   T_FreeLog( );

   usRC = DOSCONNECTNMPIPE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos16ConnectNmPipe );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16ConnectNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16ConnectNmPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16DisConnectNmPipe()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSDISCONNECTNMPIPE( HPIPE16 );
#define DOSDISCONNECTNMPIPE_PARMBYTES ( sizeof( HPIPE16 ) )
PCHAR szDos16DisConnectNmPipe = "Dos16DisConnectNmPipe";

APIRET16 APIENTRY16 Trc_Dos16DisConnectNmPipe( HPIPE16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSDISCONNECTNMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16DisConnectNmPipe, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPIPE );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szDos16DisConnectNmPipe, &Parm1 );
   T_FreeLog( );

   usRC = DOSDISCONNECTNMPIPE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos16DisConnectNmPipe );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16DisConnectNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16DisConnectNmPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16MakeNmPipe()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSMAKENMPIPE( PSZ16, PHPIPE16, USHORT, USHORT, USHORT, USHORT, ULONG );
#define DOSMAKENMPIPE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHPIPE16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16MakeNmPipe = "Dos16MakeNmPipe";

APIRET16 APIENTRY16 Trc_Dos16MakeNmPipe( PSZ16    Parm1
                                       , PHPIPE16 Parm2
                                       , USHORT   Parm3
                                       , USHORT   Parm4
                                       , USHORT   Parm5
                                       , USHORT   Parm6
                                       , ULONG    Parm7
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSMAKENMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16MakeNmPipe, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PHPIPE );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_USHORT );
      T_LogUSHORT( Parm6 );
      T_LogParameter( 7L, DT_ULONG );
      T_LogULONG( Parm7 );
   }
   HOOKENTRY16( szDos16MakeNmPipe, &Parm1 );
   T_FreeLog( );

   usRC = DOSMAKENMPIPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szDos16MakeNmPipe );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PHPIPE );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szDos16MakeNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16MakeNmPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PeekNmPipe()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSPEEKNMPIPE( HPIPE16, PVOID16, USHORT, PUSHORT16, PAVAILDATA16, PUSHORT16 );
#define DOSPEEKNMPIPE_PARMBYTES ( sizeof( HPIPE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( PAVAILDATA16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16PeekNmPipe = "Dos16PeekNmPipe";

APIRET16 APIENTRY16 Trc_Dos16PeekNmPipe( HPIPE16      Parm1
                                       , PVOID16      Parm2
                                       , USHORT       Parm3
                                       , PUSHORT16    Parm4
                                       , PAVAILDATA16 Parm5
                                       , PUSHORT16    Parm6
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPEEKNMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16PeekNmPipe, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPIPE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_PAVAILDATA );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PeekNmPipe, &Parm1 );
   T_FreeLog( );

   usRC = DOSPEEKNMPIPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PeekNmPipe );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID16( Parm2 );
      if ( ( Parm4 ) && ( !usRC ) ) T_LogCHARBuffer16( Parm2, *Parm4 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
      T_LogParameter( 5L, DT_PAVAILDATA );
      if ( !usRC ) T_LogPAVAILDATA16( Parm5 );
      else T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16PeekNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16PeekNmPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QNmPHandState()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQNMPHANDSTATE( HPIPE16, PUSHORT16 );
#define DOSQNMPHANDSTATE_PARMBYTES ( sizeof( HPIPE16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16QNmPHandState = "Dos16QNmPHandState";

APIRET16 APIENTRY16 Trc_Dos16QNmPHandState( HPIPE16   Parm1
                                          , PUSHORT16 Parm2
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQNMPHANDSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16QNmPHandState, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPIPE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
   }
   HOOKENTRY16( szDos16QNmPHandState, &Parm1 );
   T_FreeLog( );

   usRC = DOSQNMPHANDSTATE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16QNmPHandState );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szDos16QNmPHandState, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QNmPHandState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QNmPipeInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQNMPIPEINFO( HPIPE16, USHORT, PVOID16, USHORT );
#define DOSQNMPIPEINFO_PARMBYTES ( sizeof( HPIPE16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) )
PCHAR szDos16QNmPipeInfo = "Dos16QNmPipeInfo";

APIRET16 APIENTRY16 Trc_Dos16QNmPipeInfo( HPIPE16 Parm1
                                        , USHORT  Parm2
                                        , PVOID16 Parm3
                                        , USHORT  Parm4
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQNMPIPEINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16QNmPipeInfo, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPIPE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szDos16QNmPipeInfo, &Parm1 );
   T_FreeLog( );

   usRC = DOSQNMPIPEINFO( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos16QNmPipeInfo );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID16( Parm3 );
      if ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) T_LogD16QNPI_Parm3( Parm3, Parm4, Parm2 );
   }
   HOOKEXIT16( szDos16QNmPipeInfo, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QNmPipeInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16QNmPipeSemState()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSQNMPIPESEMSTATE( HSEM, PPIPESEMSTATE16, USHORT );
#define DOSQNMPIPESEMSTATE_PARMBYTES ( sizeof( HSEM ) + sizeof( PPIPESEMSTATE16 ) + sizeof( USHORT ) )
PCHAR szDos16QNmPipeSemState = "Dos16QNmPipeSemState";

APIRET16 APIENTRY16 Trc_Dos16QNmPipeSemState( HSEM            Parm1
                                            , PPIPESEMSTATE16 Parm2
                                            , USHORT          Parm3
                                            )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSQNMPIPESEMSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16QNmPipeSemState, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSEM );
      T_LogULONG( (ULONG)Parm1 );
      T_LogParameter( 2L, DT_PPIPESEMSTATE );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szDos16QNmPipeSemState, &Parm1 );
   T_FreeLog( );

   usRC = DOSQNMPIPESEMSTATE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16QNmPipeSemState );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PPIPESEMSTATE );
      T_LogPVOID16( Parm2 );
      if ( ( !usRC ) || ( usRC == ERROR_BUFFER_OVERFLOW ) ) T_LogPPIPESEMSTATE( (PPIPESEMSTATE)Parm2, Parm3 );
   }
   HOOKEXIT16( szDos16QNmPipeSemState, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16QNmPipeSemState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16RawReadNmPipe()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSRAWREADNMPIPE( HFILE16, PVOID16, USHORT, PUSHORT16 );
#define DOSRAWREADNMPIPE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16RawReadNmPipe = "Dos16RawReadNmPipe";

APIRET16 APIENTRY16 Trc_Dos16RawReadNmPipe( HFILE16   Parm1
                                          , PVOID16   Parm2
                                          , USHORT    Parm3
                                          , PUSHORT16 Parm4
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSRAWREADNMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16RawReadNmPipe, sRegs16.ulCSIP );
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
   HOOKENTRY16( szDos16RawReadNmPipe, &Parm1 );
   T_FreeLog( );

   usRC = DOSRAWREADNMPIPE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos16RawReadNmPipe );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID16( Parm2 );
      if ( ( Parm4 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer16( Parm2, *Parm4 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
   }
   HOOKEXIT16( szDos16RawReadNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16RawReadNmPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16RawWriteNmPipe()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSRAWWRITENMPIPE( HFILE16, PVOID16, USHORT, PUSHORT16 );
#define DOSRAWWRITENMPIPE_PARMBYTES ( sizeof( HFILE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16RawWriteNmPipe = "Dos16RawWriteNmPipe";

APIRET16 APIENTRY16 Trc_Dos16RawWriteNmPipe( HFILE16   Parm1
                                           , PVOID16   Parm2
                                           , USHORT    Parm3
                                           , PUSHORT16 Parm4
                                           )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSRAWWRITENMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16RawWriteNmPipe, sRegs16.ulCSIP );
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
   HOOKENTRY16( szDos16RawWriteNmPipe, &Parm1 );
   T_FreeLog( );

   usRC = DOSRAWWRITENMPIPE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos16RawWriteNmPipe );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
   }
   HOOKEXIT16( szDos16RawWriteNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16RawWriteNmPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetNmPHandState()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETNMPHANDSTATE( HPIPE16, USHORT );
#define DOSSETNMPHANDSTATE_PARMBYTES (sizeof( HPIPE16 ) + sizeof( USHORT ) )
PCHAR szDos16SetNmPHandState = "Dos16SetNmPHandState";

APIRET16 APIENTRY16 Trc_Dos16SetNmPHandState( HPIPE16 Parm1
                                            , USHORT  Parm2
                                            )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETNMPHANDSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16SetNmPHandState, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPIPE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szDos16SetNmPHandState, &Parm1 );
   T_FreeLog( );

   usRC = DOSSETNMPHANDSTATE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16SetNmPHandState );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16SetNmPHandState, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetNmPHandState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetNmPipeSem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETNMPIPESEM( HPIPE16, HSEM, USHORT );
#define DOSSETNMPIPESEM_PARMBYTES ( sizeof( HPIPE16 ) + sizeof( HSEM ) + sizeof( USHORT ) )
PCHAR szDos16SetNmPipeSem = "Dos16SetNmPipeSem";

APIRET16 APIENTRY16 Trc_Dos16SetNmPipeSem( HPIPE16 Parm1
                                         , HSEM    Parm2
                                         , USHORT  Parm3
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETNMPIPESEM_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16SetNmPipeSem, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPIPE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_HSEM );
      T_LogULONG( (ULONG)Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szDos16SetNmPipeSem, &Parm1 );
   T_FreeLog( );

   usRC = DOSSETNMPIPESEM( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16SetNmPipeSem );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16SetNmPipeSem, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetNmPipeSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16TransactNmPipe()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSTRANSACTNMPIPE( HPIPE16, PVOID16, USHORT, PVOID16, USHORT, PUSHORT16 );
#define DOSTRANSACTNMPIPE_PARMBYTES ( sizeof( HPIPE16 ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PVOID16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16TransactNmPipe = "Dos16TransactNmPipe";

APIRET16 APIENTRY16 Trc_Dos16TransactNmPipe( HPIPE16   Parm1
                                           , PVOID16   Parm2
                                           , USHORT    Parm3
                                           , PVOID16   Parm4
                                           , USHORT    Parm5
                                           , PUSHORT16 Parm6
                                           )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSTRANSACTNMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16TransactNmPipe, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPIPE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID16( Parm2 );
      T_LogCHARBuffer16( Parm2, Parm3 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16TransactNmPipe, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = DOSTRANSACTNMPIPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szDos16TransactNmPipe );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer16( Parm4, *Parm6 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16TransactNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16TransactNmPipe() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16WaitNmPipe()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSWAITNMPIPE( PSZ16, ULONG );
#define DOSWAITNMPIPE_PARMBYTES ( sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szDos16WaitNmPipe = "Dos16WaitNmPipe";

APIRET16 APIENTRY16 Trc_Dos16WaitNmPipe( PSZ16 Parm1
                                       , ULONG Parm2
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSWAITNMPIPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16WaitNmPipe, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szDos16WaitNmPipe, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = DOSWAITNMPIPE( Parm1, Parm2 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szDos16WaitNmPipe );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
   HOOKEXIT16( szDos16WaitNmPipe, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16WaitNmPipe() */
#endif

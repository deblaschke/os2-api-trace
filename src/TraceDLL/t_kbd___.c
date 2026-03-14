/******************************************************************************/
/*                                                                            */
/* File name    : T_KBD___.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit KBDCALLS routines                 */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit APIs imported from KBDCALLS.DLL.              */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.18 30Apr97 Added 16-bit Kbd APIs                       */
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
#define  INCL_KBD                                /* Include Kbd functions     */
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
PCHAR   szDLL = "KBDCALLS";                      /* DLL module name           */
PCHAR   szTraceDLL = "T_KBD___";                 /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_KBD___
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
       * Log error and terminate if T_KBD___ version does not match T_COMMON
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
       * Load KBDCALLS library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_KBD___ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register KBDCALLS trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_KBD___
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload KBDCALLS library if library loaded
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
/* Trc_Kbd16CharIn()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDCHARIN( PKBDKEYINFO16, USHORT, HKBD16 );
#define KBDCHARIN_PARMBYTES ( sizeof( PKBDKEYINFO16 ) + sizeof( USHORT ) + sizeof( HKBD16 ) )
PCHAR szKbd16CharIn = "Kbd16CharIn";

APIRET16 APIENTRY16 Trc_Kbd16CharIn( PKBDKEYINFO16 Parm1
                                   , USHORT        Parm2
                                   , HKBD16        Parm3
                                   )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDCHARIN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16CharIn, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDKEYINFO );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_HKBD );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szKbd16CharIn, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = KBDCHARIN( Parm1, Parm2, Parm3 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szKbd16CharIn );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( !Parm2 ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 1L, DT_PKBDKEYINFO );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPKBDKEYINFO( Parm1 );
   }
   HOOKEXIT16( szKbd16CharIn, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16CharIn() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16Close()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDCLOSE( HKBD16 );
#define KBDCLOSE_PARMBYTES ( sizeof( HKBD16 ) )
PCHAR szKbd16Close = "Kbd16Close";

APIRET16 APIENTRY16 Trc_Kbd16Close( HKBD16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDCLOSE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16Close, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HKBD );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szKbd16Close, &Parm1 );
   T_FreeLog( );

   usRC = KBDCLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16Close );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16Close, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16Close() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16DeRegister()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDDEREGISTER( VOID );
#define KBDDEREGISTER_PARMBYTES ( 0 )
PCHAR szKbd16DeRegister = "Kbd16DeRegister";

APIRET16 APIENTRY16 Trc_Kbd16DeRegister( VOID )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDDEREGISTER_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16DeRegister, sRegs16.ulCSIP );
   HOOKENTRY16( szKbd16DeRegister, NULL );
   T_FreeLog( );

   usRC = KBDDEREGISTER( );

   T_WaitLog( );
   T_LogAPIExit( szKbd16DeRegister );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16DeRegister, NULL, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16DeRegister() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16FlushBuffer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDFLUSHBUFFER( HKBD16 );
#define KBDFLUSHBUFFER_PARMBYTES ( sizeof( HKBD16 ) )
PCHAR szKbd16FlushBuffer = "Kbd16FlushBuffer";

APIRET16 APIENTRY16 Trc_Kbd16FlushBuffer( HKBD16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDFLUSHBUFFER_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16FlushBuffer, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HKBD );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szKbd16FlushBuffer, &Parm1 );
   T_FreeLog( );

   usRC = KBDFLUSHBUFFER( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16FlushBuffer );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16FlushBuffer, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16FlushBuffer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16FreeFocus()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDFREEFOCUS( HKBD16 );
#define KBDFREEFOCUS_PARMBYTES ( sizeof( HKBD16 ) )
PCHAR szKbd16FreeFocus = "Kbd16FreeFocus";

APIRET16 APIENTRY16 Trc_Kbd16FreeFocus( HKBD16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDFREEFOCUS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16FreeFocus, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HKBD );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szKbd16FreeFocus, &Parm1 );
   T_FreeLog( );

   usRC = KBDFREEFOCUS( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16FreeFocus );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16FreeFocus, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16FreeFocus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16GetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDGETCP( ULONG, PUSHORT16, HKBD16 );
#define KBDGETCP_PARMBYTES ( sizeof( ULONG ) + sizeof( PUSHORT16 ) + sizeof( HKBD16 ) )
PCHAR szKbd16GetCp = "Kbd16GetCp";

APIRET16 APIENTRY16 Trc_Kbd16GetCp( ULONG     Parm1
                                  , PUSHORT16 Parm2
                                  , HKBD16    Parm3
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDGETCP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16GetCp, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HKBD );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szKbd16GetCp, &Parm1 );
   T_FreeLog( );

   usRC = KBDGETCP( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16GetCp );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szKbd16GetCp, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16GetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16GetFocus()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDGETFOCUS( USHORT, HKBD16 );
#define KBDGETFOCUS_PARMBYTES ( sizeof( USHORT ) + sizeof( HKBD16 ) )
PCHAR szKbd16GetFocus = "Kbd16GetFocus";

APIRET16 APIENTRY16 Trc_Kbd16GetFocus( USHORT Parm1
                                     , HKBD16 Parm2
                                     )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDGETFOCUS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16GetFocus, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_HKBD );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szKbd16GetFocus, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = KBDGETFOCUS( Parm1, Parm2 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szKbd16GetFocus );
   T_LogCPReturnCode( usRC );
   if ( ( LOGLEVEL( L_CONTENTS ) ) && ( !Parm1 ) ) T_LogTimes( &dtEntry, &dtExit );
   HOOKEXIT16( szKbd16GetFocus, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16GetFocus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16GetHWId()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDGETHWID( PKBDHWID16, HKBD16 );
#define KBDGETHWID_PARMBYTES ( sizeof( PKBDHWID16 ) + sizeof( HKBD16 ) )
PCHAR szKbd16GetHWId = "Kbd16GetHWId";

APIRET16 APIENTRY16 Trc_Kbd16GetHWId( PKBDHWID16 Parm1
                                    , HKBD16     Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDGETHWID_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16GetHWId, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDHWID );
      T_LogPVOID16( Parm1 );
      T_LogPKBDHWID( Parm1 );
      T_LogParameter( 2L, DT_HKBD );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szKbd16GetHWId, &Parm1 );
   T_FreeLog( );

   usRC = KBDGETHWID( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16GetHWId );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDHWID );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPKBDHWID( Parm1 );
   }
   HOOKEXIT16( szKbd16GetHWId, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16GetHWId() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16GetStatus()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDGETSTATUS( PKBDINFO16, HKBD16 );
#define KBDGETSTATUS_PARMBYTES ( sizeof( PKBDINFO16 ) + sizeof( HKBD16 ) )
PCHAR szKbd16GetStatus = "Kbd16GetStatus";

APIRET16 APIENTRY16 Trc_Kbd16GetStatus( PKBDINFO16 Parm1
                                      , HKBD16 Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDGETSTATUS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16GetStatus, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDINFO );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HKBD );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szKbd16GetStatus, &Parm1 );
   T_FreeLog( );

   usRC = KBDGETSTATUS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16GetStatus );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDINFO );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPKBDINFO( Parm1 );
   }
   HOOKEXIT16( szKbd16GetStatus, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16GetStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16Open()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDOPEN( PHKBD16 );
#define KBDOPEN_PARMBYTES ( sizeof( PHKBD16 ) )
PCHAR szKbd16Open = "Kbd16Open";

APIRET16 APIENTRY16 Trc_Kbd16Open( PHKBD16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDOPEN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16Open, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PHKBD );
      T_LogPVOID16( Parm1 );
   }
   HOOKENTRY16( szKbd16Open, &Parm1 );
   T_FreeLog( );

   usRC = KBDOPEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16Open );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PHKBD );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szKbd16Open, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16Open() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16Peek()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDPEEK( PKBDKEYINFO16, HKBD16 );
#define KBDPEEK_PARMBYTES ( sizeof( PKBDKEYINFO16 ) + sizeof( HKBD16 ) )
PCHAR szKbd16Peek = "Kbd16Peek";

APIRET16 APIENTRY16 Trc_Kbd16Peek( PKBDKEYINFO16 Parm1
                                 , HKBD16        Parm2
                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDPEEK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16Peek, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDKEYINFO );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HKBD );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szKbd16Peek, &Parm1 );
   T_FreeLog( );

   usRC = KBDPEEK( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16Peek );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDKEYINFO );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPKBDKEYINFO( Parm1 );
   }
   HOOKEXIT16( szKbd16Peek, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16Peek() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16Register()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDREGISTER( PSZ16, PSZ16, ULONG );
#define KBDREGISTER_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szKbd16Register = "Kbd16Register";

APIRET16 APIENTRY16 Trc_Kbd16Register( PSZ16 Parm1
                                     , PSZ16 Parm2
                                     , ULONG Parm3
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDREGISTER_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16Register, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY16( szKbd16Register, &Parm1 );
   T_FreeLog( );

   usRC = KBDREGISTER( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16Register );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16Register, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16Register() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16SetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDSETCP( USHORT, USHORT, HKBD16 );
#define KBDSETCP_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HKBD16 ) )
PCHAR szKbd16SetCp = "Kbd16SetCp";

APIRET16 APIENTRY16 Trc_Kbd16SetCp( USHORT Parm1
                                  , USHORT Parm2
                                  , HKBD16 Parm3
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDSETCP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16SetCp, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_HKBD );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szKbd16SetCp, &Parm1 );
   T_FreeLog( );

   usRC = KBDSETCP( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16SetCp );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16SetCp, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16SetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16SetCustXt()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDSETCUSTXT( PUSHORT16, HKBD16 );
#define KBDSETCUSTXT_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HKBD16 ) )
PCHAR szKbd16SetCustXt = "Kbd16SetCustXt";

APIRET16 APIENTRY16 Trc_Kbd16SetCustXt( PUSHORT16 Parm1
                                      , HKBD16    Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDSETCUSTXT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16SetCustXt, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HKBD );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szKbd16SetCustXt, &Parm1 );
   T_FreeLog( );

   usRC = KBDSETCUSTXT( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16SetCustXt );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16SetCustXt, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16SetCustXt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16SetFgnd()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDSETFGND( VOID );
#define KBDSETFGND_PARMBYTES ( 0 )
PCHAR szKbd16SetFgnd = "Kbd16SetFgnd";

APIRET16 APIENTRY16 Trc_Kbd16SetFgnd( VOID )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDSETFGND_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16SetFgnd, sRegs16.ulCSIP );
   HOOKENTRY16( szKbd16SetFgnd, NULL );
   T_FreeLog( );

   usRC = KBDSETFGND( );

   T_WaitLog( );
   T_LogAPIExit( szKbd16SetFgnd );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16SetFgnd, NULL, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16SetFgnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16SetHWId()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDSETHWID( PKBDHWID16, HKBD16 );
#define KBDSETHWID_PARMBYTES ( sizeof( PKBDHWID16 ) + sizeof( HKBD16 ) )
PCHAR szKbd16SetHWId = "Kbd16SetHWId";

APIRET16 APIENTRY16 Trc_Kbd16SetHWId( PKBDHWID16 Parm1
                                    , HKBD16     Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDSETHWID_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16SetHWId, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDHWID );
      T_LogPVOID16( Parm1 );
      T_LogPKBDHWID( Parm1 );
      T_LogParameter( 2L, DT_HKBD );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szKbd16SetHWId, &Parm1 );
   T_FreeLog( );

   usRC = KBDSETHWID( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16SetHWId );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16SetHWId, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16SetHWId() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16SetStatus()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDSETSTATUS( PKBDINFO16, HKBD16 );
#define KBDSETSTATUS_PARMBYTES ( sizeof( PKBDINFO16 ) + sizeof( HKBD16 ) )
PCHAR szKbd16SetStatus = "Kbd16SetStatus";

APIRET16 APIENTRY16 Trc_Kbd16SetStatus( PKBDINFO16 Parm1
                                      , HKBD16 Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDSETSTATUS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16SetStatus, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDINFO );
      T_LogPVOID16( Parm1 );
      T_LogPKBDINFO( Parm1 );
      T_LogParameter( 2L, DT_HKBD );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szKbd16SetStatus, &Parm1 );
   T_FreeLog( );

   usRC = KBDSETSTATUS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16SetStatus );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16SetStatus, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16SetStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16StringIn()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDSTRINGIN( PCH16, PSTRINGINBUF16, USHORT, HKBD16 );
#define KBDSTRINGIN_PARMBYTES ( sizeof( PCH16 ) + sizeof( PSTRINGINBUF16 ) + sizeof( USHORT ) + sizeof( HKBD16 ) )
PCHAR szKbd16StringIn = "Kbd16StringIn";

APIRET16 APIENTRY16 Trc_Kbd16StringIn( PCH16          Parm1
                                     , PSTRINGINBUF16 Parm2
                                     , USHORT         Parm3
                                     , HKBD16         Parm4
                                     )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDSTRINGIN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16StringIn, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PSTRINGINBUF );
      T_LogPVOID16( Parm2 );
      T_LogPSTRINGINBUF( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_HKBD );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szKbd16StringIn, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = KBDSTRINGIN( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szKbd16StringIn );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( !Parm3 ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      if ( ( Parm2 ) && ( !usRC ) ) T_LogCHARBuffer16( Parm1, LOWORD( *((PULONG16)Parm2) ) );
      T_LogParameter( 2L, DT_PSTRINGINBUF );
      T_LogPVOID16( Parm2 );
      if ( !usRC ) T_LogPSTRINGINBUF( Parm2 );
   }
   HOOKEXIT16( szKbd16StringIn, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16StringIn() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16Synch()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDSYNCH( USHORT );
#define KBDSYNCH_PARMBYTES ( sizeof( USHORT ) )
PCHAR szKbd16Synch = "Kbd16Synch";

APIRET16 APIENTRY16 Trc_Kbd16Synch( USHORT Parm1 )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDSYNCH_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16Synch, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szKbd16Synch, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = KBDSYNCH( Parm1 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szKbd16Synch );
   T_LogCPReturnCode( usRC );
   if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm1 == 1 ) ) T_LogTimes( &dtEntry, &dtExit );
   HOOKEXIT16( szKbd16Synch, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16Synch() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Kbd16Xlate()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 KBDXLATE( PKBDTRANS16, HKBD16 );
#define KBDXLATE_PARMBYTES ( sizeof( PKBDTRANS16 ) + sizeof( HKBD16 ) )
PCHAR szKbd16Xlate = "Kbd16Xlate";

APIRET16 APIENTRY16 Trc_Kbd16Xlate( PKBDTRANS16 Parm1
                                  , HKBD16      Parm2
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = KBDXLATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szKbd16Xlate, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PKBDTRANS );
      T_LogPKBDTRANS( Parm1 );
      T_LogParameter( 2L, DT_HKBD );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szKbd16Xlate, &Parm1 );
   T_FreeLog( );

   usRC = KBDXLATE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szKbd16Xlate );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szKbd16Xlate, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Kbd16Xlate() */
#endif

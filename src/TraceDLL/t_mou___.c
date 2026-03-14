/******************************************************************************/
/*                                                                            */
/* File name    : T_MOU___.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit MOUCALLS routines                 */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit APIs imported from MOUCALLS.DLL.              */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.19 01May97 Added 16-bit Mou APIs                       */
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
#define  INCL_MOU                                /* Include Mou functions     */
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
PCHAR   szDLL = "MOUCALLS";                      /* DLL module name           */
PCHAR   szTraceDLL = "T_MOU___";                 /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_MOU___
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
       * Log error and terminate if T_MOU___ version does not match T_COMMON
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
       * Load MOUCALLS library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_MOU___ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register MOUCALLS trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_MOU___
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload MOUCALLS library if library loaded
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
/* Trc_Mou16Close()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUCLOSE( HMOU16 );
#define MOUCLOSE_PARMBYTES ( sizeof( HMOU16 ) )
PCHAR szMou16Close = "Mou16Close";

APIRET16 APIENTRY16 Trc_Mou16Close( HMOU16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUCLOSE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16Close, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HMOU );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szMou16Close, &Parm1 );
   T_FreeLog( );

   usRC = MOUCLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szMou16Close );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16Close, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16Close() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16DeRegister()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUDEREGISTER( VOID );
#define MOUDEREGISTER_PARMBYTES ( 0 )
PCHAR szMou16DeRegister = "Mou16DeRegister";

APIRET16 APIENTRY16 Trc_Mou16DeRegister( VOID )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUDEREGISTER_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16DeRegister, sRegs16.ulCSIP );
   HOOKENTRY16( szMou16DeRegister, NULL );
   T_FreeLog( );

   usRC = MOUDEREGISTER( );

   T_WaitLog( );
   T_LogAPIExit( szMou16DeRegister );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16DeRegister, NULL, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16DeRegister() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16DrawPtr()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUDRAWPTR( HMOU16 );
#define MOUDRAWPTR_PARMBYTES ( sizeof( HMOU16 ) )
PCHAR szMou16DrawPtr = "Mou16DrawPtr";

APIRET16 APIENTRY16 Trc_Mou16DrawPtr( HMOU16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUDRAWPTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16DrawPtr, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HMOU );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szMou16DrawPtr, &Parm1 );
   T_FreeLog( );

   usRC = MOUDRAWPTR( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szMou16DrawPtr );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16DrawPtr, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16DrawPtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16FlushQue()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUFLUSHQUE( HMOU16 );
#define MOUFLUSHQUE_PARMBYTES ( sizeof( HMOU16 ) )
PCHAR szMou16FlushQue = "Mou16FlushQue";

APIRET16 APIENTRY16 Trc_Mou16FlushQue( HMOU16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUFLUSHQUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16FlushQue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HMOU );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szMou16FlushQue, &Parm1 );
   T_FreeLog( );

   usRC = MOUFLUSHQUE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szMou16FlushQue );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16FlushQue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16FlushQue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetDevStatus()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETDEVSTATUS( PUSHORT16, HMOU16 );
#define MOUGETDEVSTATUS_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetDevStatus = "Mou16GetDevStatus";

APIRET16 APIENTRY16 Trc_Mou16GetDevStatus( PUSHORT16 Parm1
                                         , HMOU16    Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETDEVSTATUS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetDevStatus, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16GetDevStatus, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETDEVSTATUS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetDevStatus );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szMou16GetDevStatus, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetDevStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetEventMask()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETEVENTMASK( PUSHORT16, HMOU16 );
#define MOUGETEVENTMASK_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetEventMask = "Mou16GetEventMask";

APIRET16 APIENTRY16 Trc_Mou16GetEventMask( PUSHORT16 Parm1
                                         , HMOU16    Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETEVENTMASK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetEventMask, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16GetEventMask, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETEVENTMASK( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetEventMask );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szMou16GetEventMask, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetEventMask() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetNumButtons()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETNUMBUTTONS( PUSHORT16, HMOU16 );
#define MOUGETNUMBUTTONS_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetNumButtons = "Mou16GetNumButtons";

APIRET16 APIENTRY16 Trc_Mou16GetNumButtons( PUSHORT16 Parm1
                                          , HMOU16    Parm2
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETNUMBUTTONS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetNumButtons, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16GetNumButtons, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETNUMBUTTONS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetNumButtons );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szMou16GetNumButtons, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetNumButtons() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetNumMickeys()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETNUMMICKEYS( PUSHORT16, HMOU16 );
#define MOUGETNUMMICKEYS_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetNumMickeys = "Mou16GetNumMickeys";

APIRET16 APIENTRY16 Trc_Mou16GetNumMickeys( PUSHORT16 Parm1
                                          , HMOU16    Parm2
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETNUMMICKEYS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetNumMickeys, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16GetNumMickeys, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETNUMMICKEYS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetNumMickeys );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szMou16GetNumMickeys, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetNumMickeys() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetNumQueEl()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETNUMQUEEL( PMOUQUEINFO16, HMOU16 );
#define MOUGETNUMQUEEL_PARMBYTES ( sizeof( PMOUQUEINFO16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetNumQueEl = "Mou16GetNumQueEl";

APIRET16 APIENTRY16 Trc_Mou16GetNumQueEl( PMOUQUEINFO16 Parm1
                                        , HMOU16        Parm2
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETNUMQUEEL_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetNumQueEl, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PMOUQUEINFO );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16GetNumQueEl, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETNUMQUEEL( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetNumQueEl );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PMOUQUEINFO );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPMOUQUEINFO( Parm1 );
   }
   HOOKEXIT16( szMou16GetNumQueEl, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetNumQueEl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetPtrPos()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETPTRPOS( PPTRLOC16, HMOU16 );
#define MOUGETPTRPOS_PARMBYTES ( sizeof( PPTRLOC16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetPtrPos = "Mou16GetPtrPos";

APIRET16 APIENTRY16 Trc_Mou16GetPtrPos( PPTRLOC16 Parm1
                                      , HMOU16    Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETPTRPOS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetPtrPos, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PPTRLOC );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16GetPtrPos, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETPTRPOS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetPtrPos );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PPTRLOC );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPPTRLOC( Parm1 );
   }
   HOOKEXIT16( szMou16GetPtrPos, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetPtrPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetPtrShape()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETPTRSHAPE( PBYTE16, PPTRSHAPE16, HMOU16 );
#define MOUGETPTRSHAPE_PARMBYTES ( sizeof( PBYTE16 ) + sizeof( PPTRSHAPE16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetPtrShape = "Mou16GetPtrShape";

APIRET16 APIENTRY16 Trc_Mou16GetPtrShape( PBYTE16     Parm1
                                        , PPTRSHAPE16 Parm2
                                        , HMOU16      Parm3
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETPTRSHAPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetPtrShape, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PBYTE );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PPTRSHAPE );
      T_LogPVOID16( Parm2 );
      T_LogPPTRSHAPE( Parm2 );
      T_LogParameter( 3L, DT_HMOU );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szMou16GetPtrShape, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETPTRSHAPE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetPtrShape );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PBYTE );
      T_LogPVOID16( Parm1 );
      if ( ( Parm2 ) && ( !usRC ) ) T_LogCHARBuffer16( (PCHAR16)Parm1, Parm2->cb );
      T_LogParameter( 2L, DT_PPTRSHAPE );
      T_LogPVOID16( Parm2 );
      if ( ( !usRC ) || ( usRC == ERROR_MOUSE_INV_PARMS ) ) T_LogPPTRSHAPE( Parm2 );
   }
   HOOKEXIT16( szMou16GetPtrShape, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetPtrShape() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetScaleFact()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETSCALEFACT( PSCALEFACT16, HMOU16 );
#define MOUGETSCALEFACT_PARMBYTES ( sizeof( PSCALEFACT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetScaleFact = "Mou16GetScaleFact";

APIRET16 APIENTRY16 Trc_Mou16GetScaleFact( PSCALEFACT16 Parm1
                                         , HMOU16       Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETSCALEFACT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetScaleFact, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSCALEFACT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16GetScaleFact, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETSCALEFACT( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetScaleFact );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSCALEFACT );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPSCALEFACT( Parm1 );
   }
   HOOKEXIT16( szMou16GetScaleFact, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetScaleFact() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16GetThreshold()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUGETTHRESHOLD( PTHRESHOLD16, HMOU16 );
#define MOUGETTHRESHOLD_PARMBYTES ( sizeof( PTHRESHOLD16 ) + sizeof( HMOU16 ) )
PCHAR szMou16GetThreshold = "Mou16GetThreshold";

APIRET16 APIENTRY16 Trc_Mou16GetThreshold( PTHRESHOLD16 Parm1
                                         , HMOU16       Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUGETTHRESHOLD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16GetThreshold, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PTHRESHOLD );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16GetThreshold, &Parm1 );
   T_FreeLog( );

   usRC = MOUGETTHRESHOLD( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16GetThreshold );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PTHRESHOLD );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPTHRESHOLD( Parm1 );
   }
   HOOKEXIT16( szMou16GetThreshold, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16GetThreshold() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16InitReal()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUINITREAL( PSZ16 );
#define MOUINITREAL_PARMBYTES ( sizeof( PSZ16 ) )
PCHAR szMou16InitReal = "Mou16InitReal";

APIRET16 APIENTRY16 Trc_Mou16InitReal( PSZ16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUINITREAL_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16InitReal, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
   }
   HOOKENTRY16( szMou16InitReal, &Parm1 );
   T_FreeLog( );

   usRC = MOUINITREAL( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szMou16InitReal );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16InitReal, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16InitReal() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16Open()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUOPEN( PSZ16, PHMOU16 );
#define MOUOPEN_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PHMOU16 ) )
PCHAR szMou16Open = "Mou16Open";

APIRET16 APIENTRY16 Trc_Mou16Open( PSZ16   Parm1
                                 , PHMOU16 Parm2
                                 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUOPEN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16Open, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PHMOU );
      T_LogPVOID16( Parm2 );
   }
   HOOKENTRY16( szMou16Open, &Parm1 );
   T_FreeLog( );

   usRC = MOUOPEN( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16Open );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PHMOU );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szMou16Open, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16Open() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16ReadEventQue()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUREADEVENTQUE( PMOUEVENTINFO16, PUSHORT16, HMOU16 );
#define MOUREADEVENTQUE_PARMBYTES ( sizeof( PMOUEVENTINFO16 ) + sizeof( PUSHORT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16ReadEventQue = "Mou16ReadEventQue";

APIRET16 APIENTRY16 Trc_Mou16ReadEventQue( PMOUEVENTINFO16 Parm1
                                         , PUSHORT16       Parm2
                                         , HMOU16          Parm3
                                         )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUREADEVENTQUE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16ReadEventQue, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PMOUEVENTINFO );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
      T_LogParameter( 3L, DT_HMOU );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szMou16ReadEventQue, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = MOUREADEVENTQUE( Parm1, Parm2, Parm3 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szMou16ReadEventQue );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( T_IsValidMem16( Parm2, sizeof( USHORT ) ) ) && ( *Parm2 == MOU_WAIT ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 1L, DT_PMOUEVENTINFO );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPMOUEVENTINFO( Parm1 );
   }
   HOOKEXIT16( szMou16ReadEventQue, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16ReadEventQue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16Register()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUREGISTER( PSZ16, PSZ16, ULONG );
#define MOUREGISTER_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( ULONG ) )
PCHAR szMou16Register = "Mou16Register";

APIRET16 APIENTRY16 Trc_Mou16Register( PSZ16 Parm1
                                     , PSZ16 Parm2
                                     , ULONG Parm3
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUREGISTER_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16Register, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY16( szMou16Register, &Parm1 );
   T_FreeLog( );

   usRC = MOUREGISTER( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szMou16Register );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16Register, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16Register() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16RemovePtr()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUREMOVEPTR( PNOPTRRECT16, HMOU16 );
#define MOUREMOVEPTR_PARMBYTES ( sizeof( PNOPTRRECT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16RemovePtr = "Mou16RemovePtr";

APIRET16 APIENTRY16 Trc_Mou16RemovePtr( PNOPTRRECT16 Parm1
                                      , HMOU16       Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUREMOVEPTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16RemovePtr, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PNOPTRRECT );
      T_LogPNOPTRRECT( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16RemovePtr, &Parm1 );
   T_FreeLog( );

   usRC = MOUREMOVEPTR( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16RemovePtr );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16RemovePtr, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16RemovePtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16SetDevStatus()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUSETDEVSTATUS( PUSHORT16, HMOU16 );
#define MOUSETDEVSTATUS_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16SetDevStatus = "Mou16SetDevStatus";

APIRET16 APIENTRY16 Trc_Mou16SetDevStatus( PUSHORT16 Parm1
                                         , HMOU16    Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUSETDEVSTATUS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16SetDevStatus, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16SetDevStatus, &Parm1 );
   T_FreeLog( );

   usRC = MOUSETDEVSTATUS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16SetDevStatus );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16SetDevStatus, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16SetDevStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16SetEventMask()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUSETEVENTMASK( PUSHORT16, HMOU16 );
#define MOUSETEVENTMASK_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16SetEventMask = "Mou16SetEventMask";

APIRET16 APIENTRY16 Trc_Mou16SetEventMask( PUSHORT16 Parm1
                                         , HMOU16    Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUSETEVENTMASK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16SetEventMask, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16SetEventMask, &Parm1 );
   T_FreeLog( );

   usRC = MOUSETEVENTMASK( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16SetEventMask );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16SetEventMask, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16SetEventMask() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16SetPtrPos()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUSETPTRPOS( PPTRLOC16, HMOU16 );
#define MOUSETPTRPOS_PARMBYTES ( sizeof( PPTRLOC16 ) + sizeof( HMOU16 ) )
PCHAR szMou16SetPtrPos = "Mou16SetPtrPos";

APIRET16 APIENTRY16 Trc_Mou16SetPtrPos( PPTRLOC16 Parm1
                                      , HMOU16    Parm2
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUSETPTRPOS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16SetPtrPos, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PPTRLOC );
      T_LogPVOID16( Parm1 );
      T_LogPPTRLOC( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16SetPtrPos, &Parm1 );
   T_FreeLog( );

   usRC = MOUSETPTRPOS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16SetPtrPos );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16SetPtrPos, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16SetPtrPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16SetPtrShape()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUSETPTRSHAPE( PBYTE16, PPTRSHAPE16, HMOU16 );
#define MOUSETPTRSHAPE_PARMBYTES ( sizeof( PBYTE16 ) + sizeof( PPTRSHAPE16 ) + sizeof( HMOU16 ) )
PCHAR szMou16SetPtrShape = "Mou16SetPtrShape";

APIRET16 APIENTRY16 Trc_Mou16SetPtrShape( PBYTE16     Parm1
                                        , PPTRSHAPE16 Parm2
                                        , HMOU16      Parm3
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUSETPTRSHAPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16SetPtrShape, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PBYTE );
      T_LogPVOID16( Parm1 );
      if ( T_IsValidMem16( Parm2, sizeof( PTRSHAPE ) ) ) T_LogCHARBuffer16( (PCHAR16)Parm1, Parm2->cb );
      T_LogParameter( 2L, DT_PPTRSHAPE );
      T_LogPVOID16( Parm2 );
      T_LogPPTRSHAPE( Parm2 );
      T_LogParameter( 3L, DT_HMOU );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szMou16SetPtrShape, &Parm1 );
   T_FreeLog( );

   usRC = MOUSETPTRSHAPE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szMou16SetPtrShape );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16SetPtrShape, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16SetPtrShape() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16SetScaleFact()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUSETSCALEFACT( PSCALEFACT16, HMOU16 );
#define MOUSETSCALEFACT_PARMBYTES ( sizeof( PSCALEFACT16 ) + sizeof( HMOU16 ) )
PCHAR szMou16SetScaleFact = "Mou16SetScaleFact";

APIRET16 APIENTRY16 Trc_Mou16SetScaleFact( PSCALEFACT16 Parm1
                                         , HMOU16       Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUSETSCALEFACT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16SetScaleFact, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSCALEFACT );
      T_LogPVOID16( Parm1 );
      T_LogPSCALEFACT( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16SetScaleFact, &Parm1 );
   T_FreeLog( );

   usRC = MOUSETSCALEFACT( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16SetScaleFact );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16SetScaleFact, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16SetScaleFact() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16SetThreshold()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUSETTHRESHOLD( PTHRESHOLD16, HMOU16 );
#define MOUSETTHRESHOLD_PARMBYTES ( sizeof( PTHRESHOLD16 ) + sizeof( HMOU16 ) )
PCHAR szMou16SetThreshold = "Mou16SetThreshold";

APIRET16 APIENTRY16 Trc_Mou16SetThreshold( PTHRESHOLD16 Parm1
                                         , HMOU16       Parm2
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUSETTHRESHOLD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16SetThreshold, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PTHRESHOLD );
      T_LogPVOID16( Parm1 );
      T_LogPTHRESHOLD( Parm1 );
      T_LogParameter( 2L, DT_HMOU );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szMou16SetThreshold, &Parm1 );
   T_FreeLog( );

   usRC = MOUSETTHRESHOLD( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szMou16SetThreshold );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szMou16SetThreshold, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16SetThreshold() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Mou16Synch()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 MOUSYNCH( USHORT );
#define MOUSYNCH_PARMBYTES ( sizeof( USHORT ) )
PCHAR szMou16Synch = "Mou16Synch";

APIRET16 APIENTRY16 Trc_Mou16Synch( USHORT Parm1 )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = MOUSYNCH_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szMou16Synch, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szMou16Synch, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = MOUSYNCH( Parm1 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szMou16Synch );
   T_LogCPReturnCode( usRC );
   if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm1 == 1 ) ) T_LogTimes( &dtEntry, &dtExit );
   HOOKEXIT16( szMou16Synch, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Mou16Synch() */
#endif

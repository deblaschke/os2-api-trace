/******************************************************************************/
/*                                                                            */
/* File name    : T_SES_.C                                                    */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit SESMGR routines        */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from SESMGR.DLL.     */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.00 12Jun95 Created (supported LX format and 32-bit Dos */
/*                                APIs only)                                  */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
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
PCHAR   szDLL = "SESMGR";                        /* DLL module name           */
PCHAR   szTraceDLL = "T_SES_";                   /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_SES_
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
       * Log error and terminate if T_SES_ version does not match T_COMMON
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
       * Load SESMGR library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_SES_ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register SESMGR trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_SES_
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload SESMGR library if library loaded
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
/* Trc_Dos16SelectSession()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSELECTSESSION( USHORT, ULONG );
#define DOSSELECTSESSION_PARMBYTES ( sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16SelectSession = "Dos16SelectSession";

APIRET16 APIENTRY16 Trc_Dos16SelectSession( USHORT Parm1
                                          , ULONG  Parm2
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSELECTSESSION_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16SelectSession, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szDos16SelectSession, &Parm1 );
   T_FreeLog( );

   usRC = DOSSELECTSESSION( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16SelectSession );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16SelectSession, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SelectSession() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SelectSession()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SELECTSESSION( ULONG );
PCHAR szDos32SelectSession = "Dos32SelectSession";

APIRET APIENTRY Trc_Dos32SelectSession( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

#ifdef POWERPC
   if ( LOGDOSAPI( D_SES ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIEntry( szDos32SelectSession );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDos32SelectSession, &Parm1 );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   ulRC = DOS32SELECTSESSION( Parm1 );

#ifdef POWERPC
   if ( LOGDOSAPI( D_SES ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIExit( szDos32SelectSession );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SelectSession, &Parm1, &ulRC );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   return ulRC;

}  /* Trc_Dos32SelectSession() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SetSession()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSETSESSION( USHORT, PSTATUSDATA16 );
#define DOSSETSESSION_PARMBYTES ( sizeof( USHORT ) + sizeof( PSTATUSDATA16 ) )
PCHAR szDos16SetSession = "Dos16SetSession";

APIRET16 APIENTRY16 Trc_Dos16SetSession( USHORT        Parm1
                                       , PSTATUSDATA16 Parm2
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSETSESSION_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16SetSession, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PSTATUSDATA );
      T_LogPSTATUSDATA16( Parm2 );
   }
   HOOKENTRY16( szDos16SetSession, &Parm1 );
   T_FreeLog( );

   usRC = DOSSETSESSION( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16SetSession );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16SetSession, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SetSession() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetSession()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32SETSESSION( ULONG, PSTATUSDATA );
PCHAR szDos32SetSession = "Dos32SetSession";

APIRET APIENTRY Trc_Dos32SetSession( ULONG       Parm1
                                   , PSTATUSDATA Parm2
                                   )
{

   APIRET ulRC;                                  /* API return code           */

#ifdef POWERPC
   if ( LOGDOSAPI( D_SES ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIEntry( szDos32SetSession );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSTATUSDATA );
         T_LogPSTATUSDATA( Parm2 );
      }
      HOOKENTRY( szDos32SetSession, &Parm1 );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   ulRC = DOS32SETSESSION( Parm1, Parm2 );

#ifdef POWERPC
   if ( LOGDOSAPI( D_SES ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIExit( szDos32SetSession );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32SetSession, &Parm1, &ulRC );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   return ulRC;

}  /* Trc_Dos32SetSession() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16SMRegisterDD()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSMREGISTERDD( PREGISTERDATA16 );
#define DOSSMREGISTERDD_PARMBYTES ( sizeof( PREGISTERDATA16 ) )
PCHAR szDos16SMRegisterDD = "Dos16SMRegisterDD";

APIRET16 APIENTRY16 Trc_Dos16SMRegisterDD( PREGISTERDATA16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSMREGISTERDD_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16SMRegisterDD, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PREGISTERDATA );
      T_LogPREGISTERDATA( Parm1 );
   }
   HOOKENTRY16( szDos16SMRegisterDD, &Parm1 );
   T_FreeLog( );

   usRC = DOSSMREGISTERDD( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos16SMRegisterDD );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16SMRegisterDD, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16SMRegisterDD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16StartSession()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSTARTSESSION( PSTARTDATA16, PUSHORT16, PUSHORT16 );
#define DOSSTARTSESSION_PARMBYTES ( sizeof( PSTARTDATA16 ) + sizeof( PUSHORT16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16StartSession = "Dos16StartSession";

APIRET16 APIENTRY16 Trc_Dos16StartSession( PSTARTDATA16 Parm1
                                         , PUSHORT16    Parm2
                                         , PUSHORT16    Parm3
                                         )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSTARTSESSION_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16StartSession, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSTARTDATA );
      T_LogPSTARTDATA16( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_PUSHORT );
      T_LogPVOID16( Parm3 );
   }
   HOOKENTRY16( szDos16StartSession, &Parm1 );
   T_FreeLog( );

   usRC = DOSSTARTSESSION( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16StartSession );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
      T_LogParameter( 3L, DT_PUSHORT );
      T_LogPUSHORT16( Parm3 );
   }
   HOOKEXIT16( szDos16StartSession, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16StartSession() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32StartSession()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32STARTSESSION( PSTARTDATA, PULONG, PPID );
PCHAR szDos32StartSession = "Dos32StartSession";

APIRET APIENTRY Trc_Dos32StartSession( PSTARTDATA Parm1
                                     , PULONG     Parm2
                                     , PPID       Parm3
                                     )
{

   APIRET ulRC;                                  /* API return code           */

#ifdef POWERPC
   if ( LOGDOSAPI( D_SES ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIEntry( szDos32StartSession );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSTARTDATA );
         T_LogPSTARTDATA( Parm1 );
         T_LogParameter( 2L, DT_PULONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PPID );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szDos32StartSession, &Parm1 );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   ulRC = DOS32STARTSESSION( Parm1, Parm2, Parm3 );

#ifdef POWERPC
   if ( LOGDOSAPI( D_SES ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIExit( szDos32StartSession );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PULONG );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PPID );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szDos32StartSession, &Parm1, &ulRC );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   return ulRC;

}  /* Trc_Dos32StartSession() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16StopSession()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSSTOPSESSION( USHORT, USHORT, ULONG );
#define DOSSTOPSESSION_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( ULONG ) )
PCHAR szDos16StopSession = "Dos16StopSession";

APIRET16 APIENTRY16 Trc_Dos16StopSession( USHORT Parm1
                                        , USHORT Parm2
                                        , ULONG  Parm3
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSSTOPSESSION_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16StopSession, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY16( szDos16StopSession, &Parm1 );
   T_FreeLog( );

   usRC = DOSSTOPSESSION( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16StopSession );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16StopSession, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16StopSession() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32StopSession()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32STOPSESSION( ULONG, ULONG );
PCHAR szDos32StopSession = "Dos32StopSession";

APIRET APIENTRY Trc_Dos32StopSession( ULONG Parm1
                                    , ULONG Parm2
                                    )
{

   APIRET ulRC;                                  /* API return code           */

#ifdef POWERPC
   if ( LOGDOSAPI( D_SES ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIEntry( szDos32StopSession );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szDos32StopSession, &Parm1 );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   ulRC = DOS32STOPSESSION( Parm1, Parm2 );

#ifdef POWERPC
   if ( LOGDOSAPI( D_SES ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIExit( szDos32StopSession );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32StopSession, &Parm1, &ulRC );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   return ulRC;

}  /* Trc_Dos32StopSession() */
#endif

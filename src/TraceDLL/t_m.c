/******************************************************************************/
/*                                                                            */
/* File name    : T_M.C                                                       */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit MSG routines           */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from MSG.DLL.        */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.00 12Jun95 Created (supported LX format and 32-bit Dos */
/*                                APIs only)                                  */
/*                2.30.12 25Aug95 Fixed Dos32QueryMessageCP bug               */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.21 20May97 Fixed/minimized 16-bit stack usage          */
/*                2.40.25 04Jun97 Fixed 16-bit registers structure init bug   */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.50 06Feb98 Fixed CX and DX  corruption by  16-bit Kbd, */
/*                                Mou, and Vio APIs                           */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.73 26Oct98 Fixed   Dos32GetMessage/Dos32QueryMessageCP */
/*                                bug                                         */
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
PCHAR   szDLL = "MSG";                           /* DLL module name           */
PCHAR   szTraceDLL = "T_M";                      /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_M
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
       * Log error and terminate if T_M version does not match T_COMMON version
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
       * Load MSG library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_M library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register MSG trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_M
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload MSG library if library loaded
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
/* C_Dos32GetMessage()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY A_DOS32TRUEGETMESSAGE( PVOID, PCHAR *, ULONG, PCHAR, ULONG, ULONG, PSZ, PULONG );
PCHAR szDos32GetMessage = "Dos32GetMessage";

APIRET APIENTRY C_Dos32GetMessage( ULONG   APICaller
                                 , PVOID   MsgSeg
                                 , PCHAR  *Parm1
                                 , ULONG   Parm2
                                 , PCHAR   Parm3
                                 , ULONG   Parm4
                                 , ULONG   Parm5
                                 , PSZ     Parm6
                                 , PULONG  Parm7
                                 )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntryWithCaller( szDos32GetMessage, APICaller );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCHARSTAR );
      T_LogPVOID( Parm1 );
      if ( Parm2 ) T_LogPSZBuffer( Parm1, Parm2 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PSZ );
      T_LogPSZ( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
   }
   HOOKENTRY( szDos32GetMessage, &Parm1 );
   T_FreeLog( );

   ulRC = A_DOS32TRUEGETMESSAGE( MsgSeg, Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szDos32GetMessage );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
      if ( ( Parm7 ) && ( ( !ulRC ) || ( ulRC == ERROR_MR_MSG_TOO_LONG ) ) ) T_LogCHARBuffer( Parm3, *Parm7 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szDos32GetMessage, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* C_Dos32GetMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* C_Dos32QueryMessageCP()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY A_DOS32IQUERYMESSAGECP( PCHAR, ULONG, PSZ, PULONG, PVOID );
PCHAR szDos32QueryMessageCP = "Dos32QueryMessageCP";

APIRET APIENTRY C_Dos32QueryMessageCP( ULONG  APICaller
                                     , PCHAR  Parm1
                                     , ULONG  Parm2
                                     , PSZ    Parm3
                                     , PULONG Parm4
                                     , PVOID  MsgSeg
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntryWithCaller( szDos32QueryMessageCP, APICaller );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCHAR );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPVOID( Parm4 );
   }
   HOOKENTRY( szDos32QueryMessageCP, &Parm1 );
   T_FreeLog( );

   ulRC = A_DOS32IQUERYMESSAGECP( Parm1, Parm2, Parm3, Parm4, MsgSeg );

   T_WaitLog( );
   T_LogAPIExit( szDos32QueryMessageCP );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCHAR );
      T_LogPVOID( Parm1 );
      if ( ( Parm4 ) && ( !ulRC ) ) T_LogD32QMCP_Parm1( (PSHORT)Parm1, *Parm4 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPULONG( Parm4 );
   }
   HOOKEXIT( szDos32QueryMessageCP, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* C_Dos32QueryMessageCP() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetMessage()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSTRUEGETMESSAGE( PPCHAR16, USHORT, PCHAR16, USHORT, USHORT, PSZ16, PUSHORT, PVOID16 );
#define DOSTRUEGETMESSAGE_PARMBYTES ( sizeof( PPCHAR16 ) + sizeof( USHORT ) + sizeof( PCHAR16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PSZ16 ) + sizeof( PUSHORT16 ) + sizeof( PVOID16 ) )
PCHAR szDos16GetMessage = "Dos16GetMessage";

APIRET16 APIENTRY16 Trc_Dos16GetMessage( PPCHAR16  Parm1
                                       , USHORT    Parm2
                                       , PCHAR16   Parm3
                                       , USHORT    Parm4
                                       , USHORT    Parm5
                                       , PSZ16     Parm6
                                       , PUSHORT16 Parm7
                                       , PVOID16   MsgSeg
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSTRUEGETMESSAGE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16GetMessage, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCHARFARSTAR );
      T_LogPVOID16( (PVOID16)Parm1 );
      if ( Parm2 ) T_LogPSZBuffer16( Parm1, Parm2 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PSZ );
      T_LogPSZ16( Parm6 );
      T_LogParameter( 7L, DT_PUSHORT );
      T_LogPVOID16( Parm7 );
   }
   HOOKENTRY16( szDos16GetMessage, &Parm1 );
   T_FreeLog( );

   usRC = DOSTRUEGETMESSAGE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, MsgSeg );

   T_WaitLog( );
   T_LogAPIExit( szDos16GetMessage );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
      if ( ( Parm7 ) && ( ( !usRC ) || ( usRC == ERROR_MR_MSG_TOO_LONG ) ) ) T_LogCHARBuffer16( Parm3, *Parm7 );
      T_LogParameter( 7L, DT_PUSHORT );
      T_LogPUSHORT16( Parm7 );
   }
   HOOKEXIT16( szDos16GetMessage, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32GetMessage()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET APIENTRY DOS32GETMESSAGE( PCHAR *, ULONG, PCHAR, ULONG, ULONG, PSZ, PULONG );
PCHAR szDos32GetMessage = "Dos32GetMessage";

APIRET APIENTRY Trc_Dos32GetMessage( PCHAR  *Parm1
                                   , ULONG   Parm2
                                   , PCHAR   Parm3
                                   , ULONG   Parm4
                                   , ULONG   Parm5
                                   , PSZ     Parm6
                                   , PULONG  Parm7
                                   )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32GetMessage );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCHARSTAR );
         T_LogPVOID( Parm1 );
         if ( Parm2 ) T_LogPSZBuffer( Parm1, Parm2 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCHAR );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PSZ );
         T_LogPSZ( Parm6 );
         T_LogParameter( 7L, DT_PULONG );
         T_LogPVOID( Parm7 );
      }
      HOOKENTRY( szDos32GetMessage, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32GETMESSAGE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGDOSAPI( D_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32GetMessage );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCHAR );
         T_LogPVOID( Parm3 );
         if ( ( Parm7 ) && ( ( !ulRC ) || ( ulRC == ERROR_MR_MSG_TOO_LONG ) ) ) T_LogCHARBuffer( Parm3, *Parm7 );
         T_LogParameter( 7L, DT_PULONG );
         T_LogPULONG( Parm7 );
      }
      HOOKEXIT( szDos32GetMessage, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32GetMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16InsMessage()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSINSMESSAGE( PPCHAR16, USHORT, PSZ16, USHORT, PCHAR16, USHORT, PUSHORT16 );
#define DOSINSMESSAGE_PARMBYTES ( sizeof( PPCHAR16 ) + sizeof( USHORT ) + sizeof( PSZ16 ) + sizeof( USHORT ) + sizeof( PCHAR16 ) + sizeof( USHORT ) + sizeof( PUSHORT16 ) )
PCHAR szDos16InsMessage = "Dos16InsMessage";

APIRET16 APIENTRY16 Trc_Dos16InsMessage( PPCHAR16  Parm1
                                       , USHORT    Parm2
                                       , PSZ16     Parm3
                                       , USHORT    Parm4
                                       , PCHAR16   Parm5
                                       , USHORT    Parm6
                                       , PUSHORT16 Parm7
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                  /* API return code           */

   sRegs16.usParmBytes = DOSINSMESSAGE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16InsMessage, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCHARFARSTAR );
      T_LogPVOID16( (PVOID16)Parm1 );
      if ( Parm2 ) T_LogPSZBuffer16( Parm1, Parm2 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_PCHAR );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_USHORT );
      T_LogUSHORT( Parm6 );
      T_LogParameter( 7L, DT_PUSHORT );
      T_LogPVOID16( Parm7 );
   }
   HOOKENTRY16( szDos16InsMessage, &Parm1 );
   T_FreeLog( );

   usRC = DOSINSMESSAGE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szDos16InsMessage );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PCHAR );
      T_LogPVOID16( Parm5 );
      if ( ( Parm7 ) && ( ( !usRC ) || ( usRC == ERROR_MR_MSG_TOO_LONG ) ) ) T_LogCHARBuffer16( Parm5, *Parm7 );
      T_LogParameter( 7L, DT_PUSHORT );
      T_LogPUSHORT16( Parm7 );
   }
   HOOKEXIT16( szDos16InsMessage, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16InsMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32InsertMessage()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32INSERTMESSAGE( PCHAR *, ULONG, PSZ, ULONG, PCHAR, ULONG, PULONG );
PCHAR szDos32InsertMessage = "Dos32InsertMessage";

APIRET APIENTRY Trc_Dos32InsertMessage( PCHAR *Parm1
                                      , ULONG  Parm2
                                      , PSZ    Parm3
                                      , ULONG  Parm4
                                      , PCHAR  Parm5
                                      , ULONG  Parm6
                                      , PULONG Parm7
                                      )
{

   APIRET ulRC;                                  /* API return code           */

#ifdef POWERPC
   if ( LOGDOSAPI( D_MSG ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIEntry( szDos32InsertMessage );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCHARSTAR );
         T_LogPVOID( Parm1 );
         if ( Parm2 ) T_LogPSZBuffer( Parm1, Parm2 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PCHAR );
         T_LogPVOID( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PULONG );
         T_LogPVOID( Parm7 );
      }
      HOOKENTRY( szDos32InsertMessage, &Parm1 );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   ulRC = DOS32INSERTMESSAGE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

#ifdef POWERPC
   if ( LOGDOSAPI( D_MSG ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIExit( szDos32InsertMessage );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PCHAR );
         T_LogPVOID( Parm5 );
         if ( ( Parm7 ) && ( ( !ulRC ) || ( ulRC == ERROR_MR_MSG_TOO_LONG ) ) ) T_LogCHARBuffer( Parm5, *Parm7 );
         T_LogParameter( 7L, DT_PULONG );
         T_LogPULONG( Parm7 );
      }
      HOOKEXIT( szDos32InsertMessage, &Parm1, &ulRC );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   return ulRC;

}  /* Trc_Dos32InsertMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PutMessage()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSPUTMESSAGE( USHORT, USHORT, PCHAR16 );
#define DOSPUTMESSAGE_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PCHAR16 ) )
PCHAR szDos16PutMessage = "Dos16PutMessage";

APIRET16 APIENTRY16 Trc_Dos16PutMessage( USHORT  Parm1
                                       , USHORT  Parm2
                                       , PCHAR16 Parm3
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSPUTMESSAGE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16PutMessage, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
      T_LogCHARBuffer16( Parm3, Parm2 );
   }
   HOOKENTRY16( szDos16PutMessage, &Parm1 );
   T_FreeLog( );

   usRC = DOSPUTMESSAGE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PutMessage );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PutMessage, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16PutMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32PutMessage()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32PUTMESSAGE( HFILE, ULONG, PCHAR );
PCHAR szDos32PutMessage = "Dos32PutMessage";

APIRET APIENTRY Trc_Dos32PutMessage( HFILE Parm1
                                   , ULONG Parm2
                                   , PCHAR Parm3
                                   )
{

   APIRET ulRC;                                  /* API return code           */

#ifdef POWERPC
   if ( LOGDOSAPI( D_MSG ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIEntry( szDos32PutMessage );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HFILE );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCHAR );
         T_LogPVOID( Parm3 );
         T_LogCHARBuffer( Parm3, Parm2 );
      }
      HOOKENTRY( szDos32PutMessage, &Parm1 );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   ulRC = DOS32PUTMESSAGE( Parm1, Parm2, Parm3 );

#ifdef POWERPC
   if ( LOGDOSAPI( D_MSG ) )
   {
#endif
      T_WaitLog( );
      T_LogAPIExit( szDos32PutMessage );
      T_LogCPReturnCode( ulRC );
      HOOKEXIT( szDos32PutMessage, &Parm1, &ulRC );
      T_FreeLog( );
#ifdef POWERPC
   }
#endif

   return ulRC;

}  /* Trc_Dos32PutMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryMessageCP()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
APIRET APIENTRY DOS32QUERYMESSAGECP( PCHAR, ULONG, PSZ, PULONG );
PCHAR szDos32QueryMessageCP = "Dos32QueryMessageCP";

APIRET APIENTRY Trc_Dos32QueryMessageCP( PCHAR  Parm1
                                       , ULONG  Parm2
                                       , PSZ    Parm3
                                       , PULONG Parm4
                                       )
{

   APIRET ulRC;                                  /* API return code           */

   if ( LOGDOSAPI( D_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDos32QueryMessageCP );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCHAR );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDos32QueryMessageCP, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DOS32QUERYMESSAGECP( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGDOSAPI( D_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDos32QueryMessageCP );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCHAR );
         T_LogPVOID( Parm1 );
         if ( ( Parm4 ) && ( !ulRC ) ) T_LogD32QMCP_Parm1( (PSHORT)Parm1, *Parm4 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
      }
      HOOKEXIT( szDos32QueryMessageCP, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dos32QueryMessageCP() */
#endif

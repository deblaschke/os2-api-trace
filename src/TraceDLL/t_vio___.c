/******************************************************************************/
/*                                                                            */
/* File name    : T_VIO___.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit VIOCALLS routines                 */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit APIs imported from VIOCALLS.DLL.              */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.20 05May97 Added 16-bit Vio APIs                       */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.50 06Feb98 Fixed CX and DX  corruption by  16-bit Kbd, */
/*                                Mou, and Vio APIs                           */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.71 22Oct98 Added PM AVIO Vio APIs                      */
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
#define  INCL_VIO                                /* Include Vio functions     */
#include <os2.h>                                 /* OS/2 information          */
#include "os2trace.h"                            /* OS2TRACE information      */
#include "t_common.h"                            /* T_COMMON information      */

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#pragma stack16( 0 )

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
PCHAR   szDLL = "VIOCALLS";                      /* DLL module name           */
PCHAR   szTraceDLL = "T_VIO___";                 /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_VIO___
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
       * Log error and terminate if T_VIO___ version does not match T_COMMON
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
       * Load VIOCALLS library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_VIO___ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register VIOCALLS trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_VIO___
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload VIOCALLS library if library loaded
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
/* Trc_Vio16Associate()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOASSOCIATE( HDC, HVPS16 );
#define VIOASSOCIATE_PARMBYTES ( sizeof( HDC ) + sizeof( HVPS16 ) )
PCHAR szVio16Associate = "Vio16Associate";

USHORT APIENTRY16 Trc_Vio16Associate( HDC    Parm1
                                    , HVPS16 Parm2
                                    )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOASSOCIATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16Associate, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HVPS );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16Associate, &Parm1 );
   T_FreeLog( );

   usRC = VIOASSOCIATE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16Associate );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16Associate, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16Associate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16CheckCharType()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOCHECKCHARTYPE( PUSHORT16, USHORT, USHORT, HVIO16 );
#define VIOCHECKCHARTYPE_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16CheckCharType = "Vio16CheckCharType";

APIRET16 APIENTRY16 Trc_Vio16CheckCharType( PUSHORT16 Parm1
                                          , USHORT    Parm2
                                          , USHORT    Parm3
                                          , HVIO16    Parm4
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOCHECKCHARTYPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16CheckCharType, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_HVIO );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szVio16CheckCharType, &Parm1 );
   T_FreeLog( );

   usRC = VIOCHECKCHARTYPE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szVio16CheckCharType );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szVio16CheckCharType, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16CheckCharType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16CreateLogFont()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOCREATELOGFONT( PFATTRS16, LONG, PSTR816, HVPS16 );
#define VIOCREATELOGFONT_PARMBYTES ( sizeof( PFATTRS16 ) + sizeof( LONG ) + sizeof( PSTR816 ) + sizeof( HVPS16 ) )
PCHAR szVio16CreateLogFont = "Vio16CreateLogFont";

USHORT APIENTRY16 Trc_Vio16CreateLogFont( PFATTRS16 Parm1
                                        , LONG      Parm2
                                        , PSTR816   Parm3
                                        , HVPS16    Parm4
                                        )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOCREATELOGFONT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16CreateLogFont, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PFATTRS );
      T_LogPFATTRS16( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PSTR8 );
      T_LogPVOID16( Parm3 );
      T_LogCHARBuffer16( (PCHAR16)Parm3, sizeof( STR8 ) );
      T_LogParameter( 4L, DT_HVPS );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szVio16CreateLogFont, &Parm1 );
   T_FreeLog( );

   usRC = VIOCREATELOGFONT( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szVio16CreateLogFont );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16CreateLogFont, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16CreateLogFont() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16CreatePS()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOCREATEPS( PHVPS16, SHORT, SHORT, SHORT, SHORT, HVPS16 );
#define VIOCREATEPS_PARMBYTES ( sizeof( PHVPS16 ) + sizeof( SHORT ) + sizeof( SHORT ) + sizeof( SHORT ) + sizeof( SHORT ) + sizeof( HVPS16 ) )
PCHAR szVio16CreatePS = "Vio16CreatePS";

USHORT APIENTRY16 Trc_Vio16CreatePS( PHVPS16 Parm1
                                   , SHORT   Parm2
                                   , SHORT   Parm3
                                   , SHORT   Parm4
                                   , SHORT   Parm5
                                   , HVPS16  Parm6
                                   )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOCREATEPS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16CreatePS, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PHVPS );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_SHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_SHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_SHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_SHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_HVPS );
      T_LogUSHORT( Parm6 );
   }
   HOOKENTRY16( szVio16CreatePS, &Parm1 );
   T_FreeLog( );

   usRC = VIOCREATEPS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szVio16CreatePS );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PHVPS );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szVio16CreatePS, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16CreatePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16DeleteSetId()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIODELETESETID( LONG, HVPS16 );
#define VIODELETESETID_PARMBYTES ( sizeof( LONG ) + sizeof( HVPS16 ) )
PCHAR szVio16DeleteSetId = "Vio16DeleteSetId";

USHORT APIENTRY16 Trc_Vio16DeleteSetId( LONG   Parm1
                                      , HVPS16 Parm2
                                      )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIODELETESETID_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16DeleteSetId, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_LONG );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HVPS );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16DeleteSetId, &Parm1 );
   T_FreeLog( );

   usRC = VIODELETESETID( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16DeleteSetId );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16DeleteSetId, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16DeleteSetId() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16DeRegister()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIODEREGISTER( VOID );
#define VIODEREGISTER_PARMBYTES ( 0 )
PCHAR szVio16DeRegister = "Vio16DeRegister";

APIRET16 APIENTRY16 Trc_Vio16DeRegister( VOID )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIODEREGISTER_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16DeRegister, sRegs16.ulCSIP );
   HOOKENTRY16( szVio16DeRegister, NULL );
   T_FreeLog( );

   usRC = VIODEREGISTER( );

   T_WaitLog( );
   T_LogAPIExit( szVio16DeRegister );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16DeRegister, NULL, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16DeRegister() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16DestroyPS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIODESTROYPS( HVPS16 );
#define VIODESTROYPS_PARMBYTES ( sizeof( HVPS16 ) )
PCHAR szVio16DestroyPS = "Vio16DestroyPS";

USHORT APIENTRY16 Trc_Vio16DestroyPS( HVPS16 Parm1 )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIODESTROYPS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16DestroyPS, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HVPS );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szVio16DestroyPS, &Parm1 );
   T_FreeLog( );

   usRC = VIODESTROYPS( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szVio16DestroyPS );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16DestroyPS, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16DestroyPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16EndPopUp()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOENDPOPUP( HVIO16 );
#define VIOENDPOPUP_PARMBYTES ( sizeof( HVIO16 ) )
PCHAR szVio16EndPopUp = "Vio16EndPopUp";

APIRET16 APIENTRY16 Trc_Vio16EndPopUp( HVIO16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOENDPOPUP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16EndPopUp, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HVIO );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szVio16EndPopUp, &Parm1 );
   T_FreeLog( );

   usRC = VIOENDPOPUP( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szVio16EndPopUp );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16EndPopUp, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16EndPopUp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetAnsi()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETANSI( PUSHORT16, HVIO16 );
#define VIOGETANSI_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetAnsi = "Vio16GetAnsi";

APIRET16 APIENTRY16 Trc_Vio16GetAnsi( PUSHORT16 Parm1
                                    , HVIO16    Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETANSI_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetAnsi, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16GetAnsi, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETANSI( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetAnsi );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
   }
   HOOKEXIT16( szVio16GetAnsi, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetAnsi() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetBuf()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETBUF( PULONG16, PUSHORT16, HVIO16 );
#define VIOGETBUF_PARMBYTES ( sizeof( PULONG16 ) + sizeof( PUSHORT16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetBuf = "Vio16GetBuf";

APIRET16 APIENTRY16 Trc_Vio16GetBuf( PULONG16  Parm1
                                   , PUSHORT16 Parm2
                                   , HVIO16    Parm3
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETBUF_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetBuf, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PULONG );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16GetBuf, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETBUF( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetBuf );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PULONG );
      T_LogPULONG16( Parm1 );
      if ( ( Parm2 ) && ( !usRC ) ) T_LogCHARBuffer16( (PCHAR16)*Parm1, *Parm2 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szVio16GetBuf, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetBuf() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetConfig()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETCONFIG( USHORT, PVIOCONFIGINFO16, HVIO16 );
#define VIOGETCONFIG_PARMBYTES ( sizeof( USHORT ) + sizeof( PVIOCONFIGINFO16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetConfig = "Vio16GetConfig";

APIRET16 APIENTRY16 Trc_Vio16GetConfig( USHORT           Parm1
                                      , PVIOCONFIGINFO16 Parm2
                                      , HVIO16           Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETCONFIG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetConfig, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PVIOCONFIGINFO );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16GetConfig, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETCONFIG( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetConfig );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PVIOCONFIGINFO );
      T_LogPVOID16( Parm2 );
      if ( !usRC ) T_LogPVIOCONFIGINFO( Parm2 );
   }
   HOOKEXIT16( szVio16GetConfig, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetConfig() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETCP( USHORT, PUSHORT16, HVIO16 );
#define VIOGETCP_PARMBYTES ( sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetCp = "Vio16GetCp";

APIRET16 APIENTRY16 Trc_Vio16GetCp( USHORT    Parm1
                                  , PUSHORT16 Parm2
                                  , HVIO16    Parm3
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETCP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetCp, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16GetCp, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETCP( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetCp );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szVio16GetCp, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetCurPos()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETCURPOS( PUSHORT16, PUSHORT16, HVIO16 );
#define VIOGETCURPOS_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( PUSHORT16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetCurPos = "Vio16GetCurPos";

APIRET16 APIENTRY16 Trc_Vio16GetCurPos( PUSHORT16 Parm1
                                      , PUSHORT16 Parm2
                                      , HVIO16    Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETCURPOS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetCurPos, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16GetCurPos, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETCURPOS( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetCurPos );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szVio16GetCurPos, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetCurPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetCurType()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETCURTYPE( PVIOCURSORINFO16, HVIO16 );
#define VIOGETCURTYPE_PARMBYTES ( sizeof( PVIOCURSORINFO16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetCurType = "Vio16GetCurType";

APIRET16 APIENTRY16 Trc_Vio16GetCurType( PVIOCURSORINFO16 Parm1
                                       , HVIO16           Parm2
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETCURTYPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetCurType, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOCURSORINFO );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16GetCurType, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETCURTYPE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetCurType );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOCURSORINFO );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPVIOCURSORINFO( Parm1 );
   }
   HOOKEXIT16( szVio16GetCurType, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetCurType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetDeviceCellSize()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOGETDEVICECELLSIZE( PSHORT16, PSHORT16, HVPS16 );
#define VIOGETDEVICECELLSIZE_PARMBYTES ( sizeof( PSHORT16 ) + sizeof( PSHORT16 ) + sizeof( HVPS16 ) )
PCHAR szVio16GetDeviceCellSize = "Vio16GetDeviceCellSize";

USHORT APIENTRY16 Trc_Vio16GetDeviceCellSize( PSHORT16 Parm1
                                            , PSHORT16 Parm2
                                            , HVPS16   Parm3
                                            )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOGETDEVICECELLSIZE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetDeviceCellSize, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HVPS );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16GetDeviceCellSize, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETDEVICECELLSIZE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetDeviceCellSize );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSHORT );
      T_LogPUSHORT16( (PUSHORT16)Parm1 );
      T_LogParameter( 2L, DT_PSHORT );
      T_LogPUSHORT16( (PUSHORT16)Parm2 );
   }
   HOOKEXIT16( szVio16GetDeviceCellSize, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetDeviceCellSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetFont()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETFONT( PVIOFONTINFO16, HVIO16 );
#define VIOGETFONT_PARMBYTES ( sizeof( PVIOFONTINFO16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetFont = "Vio16GetFont";

APIRET16 APIENTRY16 Trc_Vio16GetFont( PVIOFONTINFO16 Parm1
                                    , HVIO16         Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETFONT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetFont, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOFONTINFO );
      T_LogPVOID16( Parm1 );
      T_LogPVIOFONTINFO( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16GetFont, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETFONT( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetFont );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOFONTINFO );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPVIOFONTINFO( Parm1 );
   }
   HOOKEXIT16( szVio16GetFont, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetFont() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetMode()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETMODE( PVIOMODEINFO16, HVIO16 );
#define VIOGETMODE_PARMBYTES ( sizeof( PVIOMODEINFO16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetMode = "Vio16GetMode";

APIRET16 APIENTRY16 Trc_Vio16GetMode( PVIOMODEINFO16 Parm1
                                    , HVIO16         Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETMODE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetMode, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOMODEINFO );
      T_LogPVOID16( Parm1 );
      T_LogPVIOMODEINFO( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16GetMode, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETMODE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetMode );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOMODEINFO );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPVIOMODEINFO( Parm1 );
   }
   HOOKEXIT16( szVio16GetMode, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetOrg()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOGETORG( PSHORT16, PSHORT16, HVPS16 );
#define VIOGETORG_PARMBYTES ( sizeof( PSHORT16 ) + sizeof( PSHORT16 ) + sizeof( HVPS16 ) )
PCHAR szVio16GetOrg = "Vio16GetOrg";

USHORT APIENTRY16 Trc_Vio16GetOrg( PSHORT16 Parm1
                                 , PSHORT16 Parm2
                                 , HVPS16   Parm3
                                 )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOGETORG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetOrg, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HVPS );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16GetOrg, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETORG( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetOrg );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSHORT );
      T_LogPUSHORT16( (PUSHORT16)Parm1 );
      T_LogParameter( 2L, DT_PSHORT );
      T_LogPUSHORT16( (PUSHORT16)Parm2 );
   }
   HOOKEXIT16( szVio16GetOrg, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetOrg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetPhysBuf()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETPHYSBUF( PVIOPHYSBUF16, USHORT );
#define VIOGETPHYSBUF_PARMBYTES ( sizeof( PVIOPHYSBUF16 ) + sizeof( USHORT ) )
PCHAR szVio16GetPhysBuf = "Vio16GetPhysBuf";

APIRET16 APIENTRY16 Trc_Vio16GetPhysBuf( PVIOPHYSBUF16 Parm1
                                       , USHORT        Parm2
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETPHYSBUF_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetPhysBuf, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOPHYSBUF );
      T_LogPVOID16( Parm1 );
      T_LogPVIOPHYSBUF( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16GetPhysBuf, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETPHYSBUF( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetPhysBuf );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOPHYSBUF );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogPVIOPHYSBUF( Parm1 );
   }
   HOOKEXIT16( szVio16GetPhysBuf, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetPhysBuf() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GetState()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGETSTATE( PVOID16, HVIO16 );
#define VIOGETSTATE_PARMBYTES ( sizeof( PVOID16 ) + sizeof( HVIO16 ) )
PCHAR szVio16GetState = "Vio16GetState";

APIRET16 APIENTRY16 Trc_Vio16GetState( PVOID16 Parm1
                                     , HVIO16  Parm2
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGETSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GetState, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVOID );
      T_LogPVOID16( Parm1 );
      T_LogV16GS_Parm1( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16GetState, &Parm1 );
   T_FreeLog( );

   usRC = VIOGETSTATE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GetState );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVOID );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogV16GS_Parm1( Parm1 );
   }
   HOOKEXIT16( szVio16GetState, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GetState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16GlobalReg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOGLOBALREG( PSZ16, PSZ16, ULONG, ULONG, LONG );
#define VIOGLOBALREG_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( ULONG ) + sizeof( ULONG ) + sizeof( LONG ) )
PCHAR szVio16GlobalReg = "Vio16GlobalReg";

APIRET16 APIENTRY16 Trc_Vio16GlobalReg( PSZ16 Parm1
                                      , PSZ16 Parm2
                                      , ULONG Parm3
                                      , ULONG Parm4
                                      , LONG  Parm5
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOGLOBALREG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16GlobalReg, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_LONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY16( szVio16GlobalReg, &Parm1 );
   T_FreeLog( );

   usRC = VIOGLOBALREG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16GlobalReg );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16GlobalReg, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16GlobalReg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ModeUndo()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOMODEUNDO( USHORT, USHORT, USHORT );
#define VIOMODEUNDO_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szVio16ModeUndo = "Vio16ModeUndo";

APIRET16 APIENTRY16 Trc_Vio16ModeUndo( USHORT Parm1
                                     , USHORT Parm2
                                     , USHORT Parm3
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOMODEUNDO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ModeUndo, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16ModeUndo, &Parm1 );
   T_FreeLog( );

   usRC = VIOMODEUNDO( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ModeUndo );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16ModeUndo, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ModeUndo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ModeWait()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOMODEWAIT( USHORT, PUSHORT16, USHORT );
#define VIOMODEWAIT_PARMBYTES ( sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) )
PCHAR szVio16ModeWait = "Vio16ModeWait";

APIRET16 APIENTRY16 Trc_Vio16ModeWait( USHORT    Parm1
                                     , PUSHORT16 Parm2
                                     , USHORT    Parm3
                                     )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOMODEWAIT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ModeWait, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16ModeWait, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = VIOMODEWAIT( Parm1, Parm2, Parm3 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szVio16ModeWait );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
   if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szVio16ModeWait, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ModeWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16PopUp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOPOPUP( PUSHORT16, HVIO16 );
#define VIOPOPUP_PARMBYTES ( sizeof( PUSHORT16 ) + sizeof( HVIO16 ) )
PCHAR szVio16PopUp = "Vio16PopUp";

APIRET16 APIENTRY16 Trc_Vio16PopUp( PUSHORT16 Parm1
                                  , HVIO16    Parm2
                                  )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOPOPUP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16PopUp, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PUSHORT );
      T_LogPUSHORT16( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16PopUp, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = VIOPOPUP( Parm1, Parm2 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szVio16PopUp );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
   HOOKEXIT16( szVio16PopUp, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16PopUp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16PrtSc()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOPRTSC( HVIO16 );
#define VIOPRTSC_PARMBYTES ( sizeof( HVIO16 ) )
PCHAR szVio16PrtSc = "Vio16PrtSc";

APIRET16 APIENTRY16 Trc_Vio16PrtSc( HVIO16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOPRTSC_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16PrtSc, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HVIO );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szVio16PrtSc, &Parm1 );
   T_FreeLog( );

   usRC = VIOPRTSC( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szVio16PrtSc );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16PrtSc, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16PrtSc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16PrtScToggle()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOPRTSCTOGGLE( HVIO16 );
#define VIOPRTSCTOGGLE_PARMBYTES ( sizeof( HVIO16 ) )
PCHAR szVio16PrtScToggle = "Vio16PrtScToggle";

APIRET16 APIENTRY16 Trc_Vio16PrtScToggle( HVIO16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOPRTSCTOGGLE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16PrtScToggle, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HVIO );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szVio16PrtScToggle, &Parm1 );
   T_FreeLog( );

   usRC = VIOPRTSCTOGGLE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szVio16PrtScToggle );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16PrtScToggle, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16PrtScToggle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16QueryFonts()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOQUERYFONTS( PLONG16, PFONTMETRICS16, LONG, PLONG16, PSZ16, ULONG, HVPS16 );
#define VIOQUERYFONTS_PARMBYTES ( sizeof( PLONG16 ) + sizeof( PFONTMETRICS16 ) + sizeof( LONG ) + sizeof( PLONG16 ) + sizeof( PSZ16 ) + sizeof( ULONG ) + sizeof( HVPS16 ) )
PCHAR szVio16QueryFonts = "Vio16QueryFonts";

USHORT APIENTRY16 Trc_Vio16QueryFonts( PLONG16        Parm1
                                     , PFONTMETRICS16 Parm2
                                     , LONG           Parm3
                                     , PLONG16        Parm4
                                     , PSZ16          Parm5
                                     , ULONG          Parm6
                                     , HVPS16         Parm7
                                     )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOQUERYFONTS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16QueryFonts, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PLONG );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PFONTMETRICS );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_LONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PLONG );
      T_LogPULONG16( (PULONG16)Parm4 );
      T_LogParameter( 5L, DT_PSZ );
      T_LogPSZ16( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_HVPS );
      T_LogUSHORT( Parm7 );
   }
   HOOKENTRY16( szVio16QueryFonts, &Parm1 );
   T_FreeLog( );

   usRC = VIOQUERYFONTS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szVio16QueryFonts );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PLONG );
      T_LogPULONG16( (PULONG16)Parm1 );
      T_LogParameter( 2L, DT_PFONTMETRICS );
      if ( ( Parm4 ) && ( !usRC ) ) T_LogFONTMETRICSBuffer16( Parm2, Parm3, *Parm4 );
      else T_LogPVOID16( Parm2 );
      T_LogParameter( 4L, DT_PLONG );
      T_LogPULONG16( (PULONG16)Parm4 );
   }
   HOOKEXIT16( szVio16QueryFonts, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16QueryFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16QuerySetIds()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOQUERYSETIDS( PLONG16, PSTR816, PLONG16, LONG, HVPS16 );
#define VIOQUERYSETIDS_PARMBYTES ( sizeof( PLONG16 ) + sizeof( PSTR816 ) + sizeof( PLONG16 ) + sizeof( LONG ) + sizeof( HVPS16 ) )
PCHAR szVio16QuerySetIds = "Vio16QuerySetIds";

USHORT APIENTRY16 Trc_Vio16QuerySetIds( PLONG16 Parm1
                                      , PSTR816 Parm2
                                      , PLONG16 Parm3
                                      , LONG    Parm4
                                      , HVPS16  Parm5
                                      )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOQUERYSETIDS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16QuerySetIds, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PLONG );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PSTR8 );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_PLONG );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_LONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_HVPS );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szVio16QuerySetIds, &Parm1 );
   T_FreeLog( );

   usRC = VIOQUERYSETIDS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16QuerySetIds );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PLONG );
      T_LogPVOID16( Parm1 );
      if ( !usRC ) T_LogULONGBuffer16( (PULONG16)Parm1, Parm4 );
      T_LogParameter( 2L, DT_PSTR8 );
      T_LogPVOID16( Parm2 );
      if ( !usRC ) T_LogCHARBuffer16( (PCHAR16)Parm2, Parm4 * sizeof( STR8 ) );
      T_LogParameter( 3L, DT_PLONG );
      T_LogPVOID16( Parm3 );
      if ( !usRC ) T_LogULONGBuffer16( (PULONG16)Parm3, Parm4 );
   }
   HOOKEXIT16( szVio16QuerySetIds, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16QuerySetIds() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ReadCellStr()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOREADCELLSTR( PCH16, PUSHORT16, USHORT, USHORT, HVIO16 );
#define VIOREADCELLSTR_PARMBYTES ( sizeof( PCH16 ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16ReadCellStr = "Vio16ReadCellStr";

APIRET16 APIENTRY16 Trc_Vio16ReadCellStr( PCH16     Parm1
                                        , PUSHORT16 Parm2
                                        , USHORT    Parm3
                                        , USHORT    Parm4
                                        , HVIO16    Parm5
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOREADCELLSTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ReadCellStr, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_HVIO );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szVio16ReadCellStr, &Parm1 );
   T_FreeLog( );

   usRC = VIOREADCELLSTR( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ReadCellStr );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      if ( ( Parm2 ) && ( !usRC ) ) T_LogCHARBuffer16( Parm1, *Parm2 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szVio16ReadCellStr, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ReadCellStr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ReadCharStr()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOREADCHARSTR( PCH16, PUSHORT16, USHORT, USHORT, HVIO16 );
#define VIOREADCHARSTR_PARMBYTES ( sizeof( PCH16 ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16ReadCharStr = "Vio16ReadCharStr";

APIRET16 APIENTRY16 Trc_Vio16ReadCharStr( PCH16     Parm1
                                        , PUSHORT16 Parm2
                                        , USHORT    Parm3
                                        , USHORT    Parm4
                                        , HVIO16    Parm5
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOREADCHARSTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ReadCharStr, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_HVIO );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szVio16ReadCharStr, &Parm1 );
   T_FreeLog( );

   usRC = VIOREADCHARSTR( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ReadCharStr );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      if ( ( Parm2 ) && ( !usRC ) ) T_LogCHARBuffer16( Parm1, *Parm2 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szVio16ReadCharStr, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ReadCharStr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16RedrawSize()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOREDRAWSIZE( PULONG16 );
#define VIOREDRAWSIZE_PARMBYTES ( sizeof( PULONG16 ) )
PCHAR szVio16RedrawSize = "Vio16RedrawSize";

APIRET16 APIENTRY16 Trc_Vio16RedrawSize( PULONG16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOREDRAWSIZE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16RedrawSize, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PULONG );
      T_LogPVOID16( Parm1 );
   }
   HOOKENTRY16( szVio16RedrawSize, &Parm1 );
   T_FreeLog( );

   usRC = VIOREDRAWSIZE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szVio16RedrawSize );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PULONG );
      T_LogPULONG16( Parm1 );
   }
   HOOKEXIT16( szVio16RedrawSize, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16RedrawSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16Register()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOREGISTER( PSZ16, PSZ16, ULONG, ULONG );
#define VIOREGISTER_PARMBYTES ( sizeof( PSZ16 ) + sizeof( PSZ16 ) + sizeof( ULONG ) + sizeof( ULONG ) )
PCHAR szVio16Register = "Vio16Register";

APIRET16 APIENTRY16 Trc_Vio16Register( PSZ16 Parm1
                                     , PSZ16 Parm2
                                     , ULONG Parm3
                                     , ULONG Parm4
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOREGISTER_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16Register, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY16( szVio16Register, &Parm1 );
   T_FreeLog( );

   usRC = VIOREGISTER( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szVio16Register );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16Register, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16Register() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SavRedrawUndo()                                                   */
/*                                                                            */
/******************************************************************************/

APIRET16 APIENTRY16 VIOSAVREDRAWUNDO( USHORT, USHORT, USHORT );
#define VIOSAVREDRAWUNDO_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) )
PCHAR szVio16SavRedrawUndo = "Vio16SavRedrawUndo";

APIRET16 APIENTRY16 Trc_Vio16SavRedrawUndo( USHORT Parm1
                                          , USHORT Parm2
                                          , USHORT Parm3
                                          )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSAVREDRAWUNDO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SavRedrawUndo, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16SavRedrawUndo, &Parm1 );
   T_FreeLog( );

   usRC = VIOSAVREDRAWUNDO( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SavRedrawUndo );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SavRedrawUndo, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SavRedrawUndo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SavRedrawWait()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSAVREDRAWWAIT( USHORT, PUSHORT16, USHORT );
#define VIOSAVREDRAWWAIT_PARMBYTES ( sizeof( USHORT ) + sizeof( PUSHORT16 ) + sizeof( USHORT ) )
PCHAR szVio16SavRedrawWait = "Vio16SavRedrawWait";

APIRET16 APIENTRY16 Trc_Vio16SavRedrawWait( USHORT    Parm1
                                          , PUSHORT16 Parm2
                                          , USHORT    Parm3
                                          )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSAVREDRAWWAIT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SavRedrawWait, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16SavRedrawWait, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = VIOSAVREDRAWWAIT( Parm1, Parm2, Parm3 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szVio16SavRedrawWait );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
   if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 2L, DT_PUSHORT );
      T_LogPUSHORT16( Parm2 );
   }
   HOOKEXIT16( szVio16SavRedrawWait, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SavRedrawWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ScrLock()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSCRLOCK( USHORT, PUCHAR16, HVIO16 );
#define VIOSCRLOCK_PARMBYTES ( sizeof( USHORT ) + sizeof( PUCHAR16 ) + sizeof( HVIO16 ) )
PCHAR szVio16ScrLock = "Vio16ScrLock";

APIRET16 APIENTRY16 Trc_Vio16ScrLock( USHORT   Parm1
                                    , PUCHAR16 Parm2
                                    , HVIO16   Parm3
                                    )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSCRLOCK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ScrLock, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PUCHAR );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16ScrLock, &Parm1 );
   T_FreeLog( );
   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );

   usRC = VIOSCRLOCK( Parm1, Parm2, Parm3 );

   if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
   T_WaitLog( );
   T_LogAPIExit( szVio16ScrLock );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ( LOGLEVEL( L_CONTENTS ) ) && ( Parm1 == 1 ) ) T_LogTimes( &dtEntry, &dtExit );
      T_LogParameter( 2L, DT_PUCHAR );
      T_LogPCHAR16( (PCHAR16)Parm2 );
   }
   HOOKEXIT16( szVio16ScrLock, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ScrLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ScrollDn()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSCROLLDN( USHORT, USHORT, USHORT, USHORT, USHORT, PBYTE16, HVIO16 );
#define VIOSCROLLDN_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PBYTE16 ) + sizeof( HVIO16 ) )
PCHAR szVio16ScrollDn = "Vio16ScrollDn";

APIRET16 APIENTRY16 Trc_Vio16ScrollDn( USHORT  Parm1
                                     , USHORT  Parm2
                                     , USHORT  Parm3
                                     , USHORT  Parm4
                                     , USHORT  Parm5
                                     , PBYTE16 Parm6
                                     , HVIO16  Parm7
                                     )
{

   REGS16        sRegs16;                        /* 16-bit registers          */
   VIOMODEINFO16 sVIOMODEINFO;                   /* VIOMODEINFO               */
   APIRET16      usRC;                           /* API return code           */

   sRegs16.usParmBytes = VIOSCROLLDN_PARMBYTES;
   T_SaveRegs16( &sRegs16 );


   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ScrollDn, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PBYTE );
      T_LogPVOID16( Parm6 );
      if ( ( sVIOMODEINFO.cb = sizeof( VIOMODEINFO ) ) && ( !VIOGETMODE( &sVIOMODEINFO, 0 ) ) ) T_LogCHARBuffer16( (PCHAR16)Parm6, sizeof( CHAR ) + sVIOMODEINFO.attrib );
      T_LogParameter( 7L, DT_HVIO );
      T_LogUSHORT( Parm7 );
   }
   HOOKENTRY16( szVio16ScrollDn, &Parm1 );
   T_FreeLog( );

   usRC = VIOSCROLLDN( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ScrollDn );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16ScrollDn, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ScrollDn() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ScrollLf()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSCROLLLF( USHORT, USHORT, USHORT, USHORT, USHORT, PBYTE16, HVIO16 );
#define VIOSCROLLLF_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PBYTE16 ) + sizeof( HVIO16 ) )
PCHAR szVio16ScrollLf = "Vio16ScrollLf";

APIRET16 APIENTRY16 Trc_Vio16ScrollLf( USHORT  Parm1
                                     , USHORT  Parm2
                                     , USHORT  Parm3
                                     , USHORT  Parm4
                                     , USHORT  Parm5
                                     , PBYTE16 Parm6
                                     , HVIO16  Parm7
                                     )
{

   REGS16        sRegs16;                        /* 16-bit registers          */
   VIOMODEINFO16 sVIOMODEINFO;                   /* VIOMODEINFO               */
   APIRET16      usRC;                           /* API return code           */

   sRegs16.usParmBytes = VIOSCROLLLF_PARMBYTES;
   T_SaveRegs16( &sRegs16 );


   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ScrollLf, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PBYTE );
      T_LogPVOID16( Parm6 );
      if ( ( sVIOMODEINFO.cb = sizeof( VIOMODEINFO ) ) && ( !VIOGETMODE( &sVIOMODEINFO, 0 ) ) )T_LogCHARBuffer16( (PCHAR16)Parm6, sizeof( CHAR ) + sVIOMODEINFO.attrib );
      T_LogParameter( 7L, DT_HVIO );
      T_LogUSHORT( Parm7 );
   }
   HOOKENTRY16( szVio16ScrollLf, &Parm1 );
   T_FreeLog( );

   usRC = VIOSCROLLLF( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ScrollLf );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16ScrollLf, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ScrollLf() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ScrollRt()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSCROLLRT( USHORT, USHORT, USHORT, USHORT, USHORT, PBYTE16, HVIO16 );
#define VIOSCROLLRT_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PBYTE16 ) + sizeof( HVIO16 ) )
PCHAR szVio16ScrollRt = "Vio16ScrollRt";

APIRET16 APIENTRY16 Trc_Vio16ScrollRt( USHORT  Parm1
                                     , USHORT  Parm2
                                     , USHORT  Parm3
                                     , USHORT  Parm4
                                     , USHORT  Parm5
                                     , PBYTE16 Parm6
                                     , HVIO16  Parm7
                                     )
{

   REGS16        sRegs16;                        /* 16-bit registers          */
   VIOMODEINFO16 sVIOMODEINFO;                   /* VIOMODEINFO               */
   APIRET16      usRC;                           /* API return code           */

   sRegs16.usParmBytes = VIOSCROLLRT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );


   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ScrollRt, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PBYTE );
      T_LogPVOID16( Parm6 );
      if ( ( sVIOMODEINFO.cb = sizeof( VIOMODEINFO ) ) && ( !VIOGETMODE( &sVIOMODEINFO, 0 ) ) )T_LogCHARBuffer16( (PCHAR16)Parm6, sizeof( CHAR ) + sVIOMODEINFO.attrib );
      T_LogParameter( 7L, DT_HVIO );
      T_LogUSHORT( Parm7 );
   }
   HOOKENTRY16( szVio16ScrollRt, &Parm1 );
   T_FreeLog( );

   usRC = VIOSCROLLRT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ScrollRt );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16ScrollRt, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ScrollRt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ScrollUp()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSCROLLUP( USHORT, USHORT, USHORT, USHORT, USHORT, PBYTE16, HVIO16 );
#define VIOSCROLLUP_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PBYTE16 ) + sizeof( HVIO16 ) )
PCHAR szVio16ScrollUp = "Vio16ScrollUp";

APIRET16 APIENTRY16 Trc_Vio16ScrollUp( USHORT  Parm1
                                     , USHORT  Parm2
                                     , USHORT  Parm3
                                     , USHORT  Parm4
                                     , USHORT  Parm5
                                     , PBYTE16 Parm6
                                     , HVIO16  Parm7
                                     )
{

   REGS16        sRegs16;                        /* 16-bit registers          */
   VIOMODEINFO16 sVIOMODEINFO;                   /* VIOMODEINFO               */
   APIRET16      usRC;                           /* API return code           */

   sRegs16.usParmBytes = VIOSCROLLUP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );


   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ScrollUp, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PBYTE );
      T_LogPVOID16( Parm6 );
      if ( ( sVIOMODEINFO.cb = sizeof( VIOMODEINFO ) ) && ( !VIOGETMODE( &sVIOMODEINFO, 0 ) ) )T_LogCHARBuffer16( (PCHAR16)Parm6, sizeof( CHAR ) + sVIOMODEINFO.attrib );
      T_LogParameter( 7L, DT_HVIO );
      T_LogUSHORT( Parm7 );
   }
   HOOKENTRY16( szVio16ScrollUp, &Parm1 );
   T_FreeLog( );

   usRC = VIOSCROLLUP( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ScrollUp );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16ScrollUp, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ScrollUp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ScrUnLock()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSCRUNLOCK( HVIO16 );
#define VIOSCRUNLOCK_PARMBYTES ( sizeof( HVIO16 ) )
PCHAR szVio16ScrUnLock = "Vio16ScrUnLock";

APIRET16 APIENTRY16 Trc_Vio16ScrUnLock( HVIO16 Parm1 )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSCRUNLOCK_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ScrUnLock, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HVIO );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szVio16ScrUnLock, &Parm1 );
   T_FreeLog( );

   usRC = VIOSCRUNLOCK( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ScrUnLock );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16ScrUnLock, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ScrUnLock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetAnsi()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSETANSI( USHORT, HVIO16 );
#define VIOSETANSI_PARMBYTES ( sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16SetAnsi = "Vio16SetAnsi";

APIRET16 APIENTRY16 Trc_Vio16SetAnsi( USHORT Parm1
                                    , HVIO16 Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSETANSI_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetAnsi, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16SetAnsi, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETANSI( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetAnsi );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SetAnsi, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetAnsi() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSETCP( USHORT, USHORT, HVIO16 );
#define VIOSETCP_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16SetCp = "Vio16SetCp";

APIRET16 APIENTRY16 Trc_Vio16SetCp( USHORT Parm1
                                  , USHORT Parm2
                                  , HVIO16 Parm3
                                  )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSETCP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetCp, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16SetCp, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETCP( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetCp );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SetCp, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetCurPos()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSETCURPOS( USHORT, USHORT, HVIO16 );
#define VIOSETCURPOS_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16SetCurPos = "Vio16SetCurPos";

APIRET16 APIENTRY16 Trc_Vio16SetCurPos( USHORT Parm1
                                      , USHORT Parm2
                                      , HVIO16 Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSETCURPOS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetCurPos, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16SetCurPos, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETCURPOS( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetCurPos );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SetCurPos, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetCurPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetCurType()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSETCURTYPE( PVIOCURSORINFO16, HVIO16 );
#define VIOSETCURTYPE_PARMBYTES ( sizeof( PVIOCURSORINFO16 ) + sizeof( HVIO16 ) )
PCHAR szVio16SetCurType = "Vio16SetCurType";

APIRET16 APIENTRY16 Trc_Vio16SetCurType( PVIOCURSORINFO16 Parm1
                                       , HVIO16           Parm2
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSETCURTYPE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetCurType, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOCURSORINFO );
      T_LogPVOID16( Parm1 );
      T_LogPVIOCURSORINFO( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16SetCurType, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETCURTYPE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetCurType );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SetCurType, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetCurType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetDeviceCellSize()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOSETDEVICECELLSIZE( PSHORT16, PSHORT16, HVPS16 );
#define VIOSETDEVICECELLSIZE_PARMBYTES ( sizeof( PSHORT16 ) + sizeof( PSHORT16 ) + sizeof( HVPS16 ) )
PCHAR szVio16SetDeviceCellSize = "Vio16SetDeviceCellSize";

USHORT APIENTRY16 Trc_Vio16SetDeviceCellSize( PSHORT16 Parm1
                                            , PSHORT16 Parm2
                                            , HVPS16   Parm3
                                            )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOSETDEVICECELLSIZE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetDeviceCellSize, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSHORT );
      T_LogPVOID16( Parm1 );
      T_LogParameter( 2L, DT_PSHORT );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_HVPS );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16SetDeviceCellSize, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETDEVICECELLSIZE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetDeviceCellSize );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSHORT );
      T_LogPUSHORT16( (PUSHORT16)Parm1 );
      T_LogParameter( 2L, DT_PSHORT );
      T_LogPUSHORT16( (PUSHORT16)Parm2 );
   }
   HOOKEXIT16( szVio16SetDeviceCellSize, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetDeviceCellSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetFont()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSETFONT( PVIOFONTINFO16, HVIO16 );
#define VIOSETFONT_PARMBYTES ( sizeof( PVIOFONTINFO16 ) + sizeof( HVIO16 ) )
PCHAR szVio16SetFont = "Vio16SetFont";

APIRET16 APIENTRY16 Trc_Vio16SetFont( PVIOFONTINFO16 Parm1
                                    , HVIO16         Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSETFONT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetFont, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOFONTINFO );
      T_LogPVOID16( Parm1 );
      T_LogPVIOFONTINFO( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16SetFont, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETFONT( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetFont );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SetFont, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetFont() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetMode()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSETMODE( PVIOMODEINFO16, HVIO16 );
#define VIOSETMODE_PARMBYTES ( sizeof( PVIOMODEINFO16 ) + sizeof( HVIO16 ) )
PCHAR szVio16SetMode = "Vio16SetMode";

APIRET16 APIENTRY16 Trc_Vio16SetMode( PVIOMODEINFO16 Parm1
                                    , HVIO16         Parm2
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSETMODE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetMode, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVIOMODEINFO );
      T_LogPVOID16( Parm1 );
      T_LogPVIOMODEINFO( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16SetMode, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETMODE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetMode );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SetMode, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetOrg()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOSETORG( SHORT, SHORT, HVPS16 );
#define VIOSETORG_PARMBYTES ( sizeof( SHORT ) + sizeof( SHORT ) + sizeof( HVPS16 ) )
PCHAR szVio16SetOrg = "Vio16SetOrg";

USHORT APIENTRY16 Trc_Vio16SetOrg( SHORT  Parm1
                                 , SHORT  Parm2
                                 , HVPS16 Parm3
                                 )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOSETORG_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetOrg, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_SHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_SHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_HVPS );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16SetOrg, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETORG( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetOrg );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SetOrg, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetOrg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16SetState()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSETSTATE( PVOID16, HVIO16 );
#define VIOSETSTATE_PARMBYTES ( sizeof( PVOID16 ) + sizeof( HVIO16 ) )
PCHAR szVio16SetState = "Vio16SetState";

APIRET16 APIENTRY16 Trc_Vio16SetState( PVOID16 Parm1
                                     , HVIO16  Parm2
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSETSTATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16SetState, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVOID );
      T_LogPVOID16( Parm1 );
      T_LogV16GS_Parm1( Parm1 );
      T_LogParameter( 2L, DT_HVIO );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szVio16SetState, &Parm1 );
   T_FreeLog( );

   usRC = VIOSETSTATE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szVio16SetState );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16SetState, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16SetState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ShowBuf()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOSHOWBUF( USHORT, USHORT, HVIO16 );
#define VIOSHOWBUF_PARMBYTES ( sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16ShowBuf = "Vio16ShowBuf";

APIRET16 APIENTRY16 Trc_Vio16ShowBuf( USHORT Parm1
                                    , USHORT Parm2
                                    , HVIO16 Parm3
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOSHOWBUF_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ShowBuf, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16ShowBuf, &Parm1 );
   T_FreeLog( );

   usRC = VIOSHOWBUF( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ShowBuf );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16ShowBuf, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ShowBuf() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16ShowPS()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 VIOSHOWPS( SHORT, SHORT, SHORT, HVPS16 );
#define VIOSHOWPS_PARMBYTES ( sizeof( SHORT ) + sizeof( SHORT ) + sizeof( SHORT ) + sizeof( HVPS16 ) )
PCHAR szVio16ShowPS = "Vio16ShowPS";

USHORT APIENTRY16 Trc_Vio16ShowPS( SHORT  Parm1
                                 , SHORT  Parm2
                                 , SHORT  Parm3
                                 , HVPS16 Parm4
                                 )
{

   REGS16 sRegs16;                               /* 16-bit registers          */
   USHORT usRC;                                  /* API return code           */

   sRegs16.usParmBytes = VIOSHOWPS_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16ShowPS, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_SHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_SHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_SHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_HVPS );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szVio16ShowPS, &Parm1 );
   T_FreeLog( );

   usRC = VIOSHOWPS( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szVio16ShowPS );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16ShowPS, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16ShowPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16WrtCellStr()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOWRTCELLSTR( PCH16, USHORT, USHORT, USHORT, HVIO16 );
#define VIOWRTCELLSTR_PARMBYTES ( sizeof( PCH16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16WrtCellStr = "Vio16WrtCellStr";

APIRET16 APIENTRY16 Trc_Vio16WrtCellStr( PCH16  Parm1
                                       , USHORT Parm2
                                       , USHORT Parm3
                                       , USHORT Parm4
                                       , HVIO16 Parm5
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOWRTCELLSTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16WrtCellStr, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      T_LogCHARBuffer16( Parm1, Parm2 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_HVIO );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szVio16WrtCellStr, &Parm1 );
   T_FreeLog( );

   usRC = VIOWRTCELLSTR( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16WrtCellStr );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16WrtCellStr, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16WrtCellStr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16WrtCharStr()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOWRTCHARSTR( PCH16, USHORT, USHORT, USHORT, HVIO16 );
#define VIOWRTCHARSTR_PARMBYTES ( sizeof( PCH16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16WrtCharStr = "Vio16WrtCharStr";

APIRET16 APIENTRY16 Trc_Vio16WrtCharStr( PCH16  Parm1
                                       , USHORT Parm2
                                       , USHORT Parm3
                                       , USHORT Parm4
                                       , HVIO16 Parm5
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOWRTCHARSTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16WrtCharStr, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      T_LogCHARBuffer16( Parm1, Parm2 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_HVIO );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szVio16WrtCharStr, &Parm1 );
   T_FreeLog( );

   usRC = VIOWRTCHARSTR( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16WrtCharStr );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16WrtCharStr, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16WrtCharStr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16WrtCharStrAtt()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOWRTCHARSTRATT( PCH16, USHORT, USHORT, USHORT, PBYTE16, HVIO16 );
#define VIOWRTCHARSTRATT_PARMBYTES ( sizeof( PCH16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( PBYTE16 ) + sizeof( HVIO16 ) )
PCHAR szVio16WrtCharStrAtt = "Vio16WrtCharStrAtt";

APIRET16 APIENTRY16 Trc_Vio16WrtCharStrAtt( PCH16   Parm1
                                          , USHORT  Parm2
                                          , USHORT  Parm3
                                          , USHORT  Parm4
                                          , PBYTE16 Parm5
                                          , HVIO16  Parm6
                                          )
{

   REGS16        sRegs16;                        /* 16-bit registers          */
   VIOMODEINFO16 sVIOMODEINFO;                   /* VIOMODEINFO               */
   APIRET16      usRC;                           /* API return code           */

   sRegs16.usParmBytes = VIOWRTCHARSTRATT_PARMBYTES;
   T_SaveRegs16( &sRegs16 );


   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16WrtCharStrAtt, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      T_LogCHARBuffer16( Parm1, Parm2 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_PBYTE );
      T_LogPVOID16( Parm5 );
      if ( ( sVIOMODEINFO.cb = sizeof( VIOMODEINFO ) ) && ( !VIOGETMODE( &sVIOMODEINFO, 0 ) ) )T_LogCHARBuffer16( (PCHAR16)Parm5, sVIOMODEINFO.attrib * Parm2 );
      T_LogParameter( 6L, DT_HVIO );
      T_LogUSHORT( Parm6 );
   }
   HOOKENTRY16( szVio16WrtCharStrAtt, &Parm1 );
   T_FreeLog( );

   usRC = VIOWRTCHARSTRATT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szVio16WrtCharStrAtt );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16WrtCharStrAtt, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16WrtCharStrAtt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16WrtNAttr()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOWRTNATTR( PBYTE16, USHORT, USHORT, USHORT, HVIO16 );
#define VIOWRTNATTR_PARMBYTES ( sizeof( PBYTE16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16WrtNAttr = "Vio16WrtNAttr";

APIRET16 APIENTRY16 Trc_Vio16WrtNAttr( PBYTE16 Parm1
                                     , USHORT  Parm2
                                     , USHORT  Parm3
                                     , USHORT  Parm4
                                     , HVIO16  Parm5
                                     )
{

   REGS16        sRegs16;                        /* 16-bit registers          */
   VIOMODEINFO16 sVIOMODEINFO;                   /* VIOMODEINFO               */
   APIRET16      usRC;                           /* API return code           */

   sRegs16.usParmBytes = VIOWRTNATTR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );


   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16WrtNAttr, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PBYTE );
      T_LogPVOID16( Parm1 );
      if ( ( sVIOMODEINFO.cb = sizeof( VIOMODEINFO ) ) && ( !VIOGETMODE( &sVIOMODEINFO, 0 ) ) )T_LogCHARBuffer16( (PCHAR16)Parm1, sVIOMODEINFO.attrib );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_HVIO );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szVio16WrtNAttr, &Parm1 );
   T_FreeLog( );

   usRC = VIOWRTNATTR( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16WrtNAttr );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16WrtNAttr, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16WrtNAttr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16WrtNCell()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOWRTNCELL( PBYTE16, USHORT, USHORT, USHORT, HVIO16 );
#define VIOWRTNCELL_PARMBYTES ( sizeof( PBYTE16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16WrtNCell = "Vio16WrtNCell";

APIRET16 APIENTRY16 Trc_Vio16WrtNCell( PBYTE16 Parm1
                                     , USHORT  Parm2
                                     , USHORT  Parm3
                                     , USHORT  Parm4
                                     , HVIO16  Parm5
                                     )
{

   REGS16        sRegs16;                        /* 16-bit registers          */
   VIOMODEINFO16 sVIOMODEINFO;                   /* VIOMODEINFO               */
   APIRET16      usRC;                           /* API return code           */

   sRegs16.usParmBytes = VIOWRTNCELL_PARMBYTES;
   T_SaveRegs16( &sRegs16 );


   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16WrtNCell, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PBYTE );
      T_LogPVOID16( Parm1 );
      if ( ( sVIOMODEINFO.cb = sizeof( VIOMODEINFO ) ) && ( !VIOGETMODE( &sVIOMODEINFO, 0 ) ) )T_LogCHARBuffer16( (PCHAR16)Parm1, sizeof( CHAR ) + sVIOMODEINFO.attrib );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_HVIO );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szVio16WrtNCell, &Parm1 );
   T_FreeLog( );

   usRC = VIOWRTNCELL( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16WrtNCell );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16WrtNCell, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16WrtNCell() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16WrtNChar()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOWRTNCHAR( PCH16, USHORT, USHORT, USHORT, HVIO16 );
#define VIOWRTNCHAR_PARMBYTES ( sizeof( PCH16 ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16WrtNChar = "Vio16WrtNChar";

APIRET16 APIENTRY16 Trc_Vio16WrtNChar( PCH16  Parm1
                                     , USHORT Parm2
                                     , USHORT Parm3
                                     , USHORT Parm4
                                     , HVIO16 Parm5
                                     )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOWRTNCHAR_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16WrtNChar, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPCHAR16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_HVIO );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szVio16WrtNChar, &Parm1 );
   T_FreeLog( );

   usRC = VIOWRTNCHAR( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szVio16WrtNChar );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16WrtNChar, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16WrtNChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Vio16WrtTTY()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 VIOWRTTTY( PCH16, USHORT, HVIO16 );
#define VIOWRTTTY_PARMBYTES ( sizeof( PCH16 ) + sizeof( USHORT ) + sizeof( HVIO16 ) )
PCHAR szVio16WrtTTY = "Vio16WrtTTY";

APIRET16 APIENTRY16 Trc_Vio16WrtTTY( PCH16  Parm1
                                   , USHORT Parm2
                                   , HVIO16 Parm3
                                   )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = VIOWRTTTY_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szVio16WrtTTY, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PCH );
      T_LogPVOID16( Parm1 );
      T_LogCHARBuffer16( Parm1, Parm2 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_HVIO );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szVio16WrtTTY, &Parm1 );
   T_FreeLog( );

   usRC = VIOWRTTTY( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szVio16WrtTTY );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szVio16WrtTTY, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Vio16WrtTTY() */
#endif

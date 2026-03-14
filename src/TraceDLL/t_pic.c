/******************************************************************************/
/*                                                                            */
/* File name    : T_PIC.C                                                     */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit PMPIC routines         */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from PMPIC.DLL.      */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.15 29Aug95 Added 32-bit Pic APIs                       */
/*                2.40.30 08Aug97 Added 16-bit Pic APIs                       */
/*                2.40.36 14Aug97 Added -I option                             */
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
PCHAR   szDLL = "PMPIC";                         /* DLL module name           */
PCHAR   szTraceDLL = "T_PIC";                    /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_PIC
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
       * Log error and terminate if T_PIC version does not match T_COMMON
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
       * Load PMPIC library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_PIC library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMPIC trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_PIC
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMPIC library if library loaded
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
/* Trc_Pic16Ichg()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PICICHG( HAB, PSZ16, PSZ16, LONG );
PCHAR szPic16Ichg = "Pic16Ichg";

BOOL16 APIENTRY16 Trc_Pic16Ichg( HAB   Parm1
                               , PSZ16 Parm2
                               , PSZ16 Parm3
                               , LONG  Parm4
                               )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPic16Ichg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_LONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY16( szPic16Ichg, &Parm1 );
   T_FreeLog( );

   bRC = PICICHG( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPic16Ichg );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPic16Ichg, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Pic16Ichg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Pic32Ichg()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PIC32ICHG( HAB, PSZ, PSZ, LONG );
PCHAR szPic32Ichg = "Pic32Ichg";

BOOL APIENTRY Trc_Pic32Ichg( HAB  Parm1
                           , PSZ  Parm2
                           , PSZ  Parm3
                           , LONG Parm4
                           )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPic32Ichg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_LONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szPic32Ichg, &Parm1 );
   T_FreeLog( );

   bRC = PIC32ICHG( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPic32Ichg );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPic32Ichg, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Pic32Ichg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Pic16Print()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PICPRINT( HAB, PSZ16, LONG, PSZ16 );
PCHAR szPic16Print = "Pic16Print";

BOOL16 APIENTRY16 Trc_Pic16Print( HAB   Parm1
                                , PSZ16 Parm2
                                , LONG  Parm3
                                , PSZ16 Parm4
                                )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPic16Print );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_LONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ16( Parm4 );
   }
   HOOKENTRY16( szPic16Print, &Parm1 );
   T_FreeLog( );

   bRC = PICPRINT( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPic16Print );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPic16Print, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Pic16Print() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Pic32Print()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PIC32PRINT( HAB, PSZ, LONG, PSZ );
PCHAR szPic32Print = "Pic32Print";

BOOL APIENTRY Trc_Pic32Print( HAB  Parm1
                            , PSZ  Parm2
                            , LONG Parm3
                            , PSZ  Parm4
                            )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPic32Print );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_LONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ( Parm4 );
   }
   HOOKENTRY( szPic32Print, &Parm1 );
   T_FreeLog( );

   bRC = PIC32PRINT( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPic32Print );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPic32Print, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Pic32Print() */
#endif

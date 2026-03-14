/******************************************************************************/
/*                                                                            */
/* File name    : T_SHL__.C                                                   */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit PMSHAPI routines       */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from PMSHAPI.DLL.    */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.03 20Jul95 Added 32-bit Win APIs                       */
/*                2.30.11 14Aug95 Added 32-bit Prf APIs                       */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.45.10 02Aug99 Moved common strings into T_COMMON          */
/*                2.45.31 26Jun01 Added  capability of building  32-bit  only */
/*                                trace DLLs                                  */
/*                2.45.32 28Jun01 Added -U option                             */
/*                2.45.33 29Jun01 Added Win APIs exported from PMSHAPI not in */
/*                                public include file                         */
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
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
#define ORD_WIN16SETTITLEANDHWNDICON  99         /* OS/2 2.30 proc ordinal    */
#endif

/******************************************************************************/
/*                                                                            */
/* OS/2 function declaration                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
APIRET16 APIENTRY16 DOS16GETPROCADDR( HMODULE16, PSZ16, PPFN16 );
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
PCHAR   szDLL = "PMSHAPI";                       /* DLL module name           */
PCHAR   szTraceDLL = "T_SHL__";                  /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
BOOL16 (* _Seg16 APIENTRY16 pfnWIN16SETTITLEANDHWNDICON)( PSZ16, HWND );
                                                 /* OS/2 2.30 procedure       */
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
    * Library initialization required if process attaching to T_SHL__
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
       * Log error and terminate if T_SHL__ version does not match T_COMMON
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
       * Load PMSHAPI library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_SHL__ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMSHAPI trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
      /*
       * Obtain OS/2 2.30 procedure addresses if OS/2 version number greater
       * than or equal to 2.30
       */
      if ( T_OS2Ver >= MAKEVER( 2, 30 ) )
      {
         /*
          * Obtain 16-bit OS/2 2.30 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16SETTITLEANDHWNDICON ), (PPFN16)&pfnWIN16SETTITLEANDHWNDICON ) )
         {
            T_Error( TRUE, EM_GETPROC, 16, 2, 30, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_SHL__
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMSHAPI library if library loaded
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
/* Trc_Prf16AddProgram()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPROGRAM APIENTRY16 PRFADDPROGRAM( HINI, PPROGDETAILS16, HPROGRAM );
PCHAR szPrf16AddProgram = "Prf16AddProgram";

HPROGRAM APIENTRY16 Trc_Prf16AddProgram( HINI           Parm1
                                       , PPROGDETAILS16 Parm2
                                       , HPROGRAM       Parm3
                                       )
{

   HPROGRAM ulRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16AddProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPROGDETAILS );
      T_LogPPROGDETAILS16( Parm2 );
      T_LogParameter( 3L, DT_HPROGRAM );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY16( szPrf16AddProgram, &Parm1 );
   T_FreeLog( );

   ulRC = PRFADDPROGRAM( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16AddProgram );
   T_LogPMReturnCode( DT_HPROGRAM, ulRC, ulRC );
   HOOKEXIT16( szPrf16AddProgram, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf16AddProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32AddProgram()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPROGRAM APIENTRY PRF32ADDPROGRAM( HINI, PPROGDETAILS, HPROGRAM );
PCHAR szPrf32AddProgram = "Prf32AddProgram";

HPROGRAM APIENTRY Trc_Prf32AddProgram( HINI         Parm1
                                     , PPROGDETAILS Parm2
                                     , HPROGRAM     Parm3
                                     )
{

   HPROGRAM ulRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32AddProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPROGDETAILS );
      T_LogPPROGDETAILS( Parm2 );
      T_LogParameter( 3L, DT_HPROGRAM );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szPrf32AddProgram, &Parm1 );
   T_FreeLog( );

   ulRC = PRF32ADDPROGRAM( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32AddProgram );
   T_LogPMReturnCode( DT_HPROGRAM, ulRC, ulRC );
   HOOKEXIT( szPrf32AddProgram, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf32AddProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16ChangeProgram()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFCHANGEPROGRAM( HINI, HPROGRAM, PPROGDETAILS16 );
PCHAR szPrf16ChangeProgram = "Prf16ChangeProgram";

BOOL16 APIENTRY16 Trc_Prf16ChangeProgram( HINI           Parm1
                                        , HPROGRAM       Parm2
                                        , PPROGDETAILS16 Parm3
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16ChangeProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPROGDETAILS );
      T_LogPPROGDETAILS16( Parm3 );
   }
   HOOKENTRY16( szPrf16ChangeProgram, &Parm1 );
   T_FreeLog( );

   bRC = PRFCHANGEPROGRAM( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16ChangeProgram );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPrf16ChangeProgram, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16ChangeProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32ChangeProgram()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32CHANGEPROGRAM( HINI, HPROGRAM, PPROGDETAILS );
PCHAR szPrf32ChangeProgram = "Prf32ChangeProgram";

BOOL APIENTRY Trc_Prf32ChangeProgram( HINI         Parm1
                                    , HPROGRAM     Parm2
                                    , PPROGDETAILS Parm3
                                    )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32ChangeProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPROGDETAILS );
      T_LogPPROGDETAILS( Parm3 );
   }
   HOOKENTRY( szPrf32ChangeProgram, &Parm1 );
   T_FreeLog( );

   bRC = PRF32CHANGEPROGRAM( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32ChangeProgram );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPrf32ChangeProgram, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32ChangeProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16CloseProfile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFCLOSEPROFILE( HINI );
PCHAR szPrf16CloseProfile = "Prf16CloseProfile";

BOOL16 APIENTRY16 Trc_Prf16CloseProfile( HINI Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16CloseProfile );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szPrf16CloseProfile, &Parm1 );
   T_FreeLog( );

   bRC = PRFCLOSEPROFILE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16CloseProfile );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPrf16CloseProfile, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16CloseProfile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32CloseProfile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32CLOSEPROFILE( HINI );
PCHAR szPrf32CloseProfile = "Prf32CloseProfile";

BOOL APIENTRY Trc_Prf32CloseProfile( HINI Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32CloseProfile );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szPrf32CloseProfile, &Parm1 );
   T_FreeLog( );

   bRC = PRF32CLOSEPROFILE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32CloseProfile );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPrf32CloseProfile, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32CloseProfile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16CreateGroup()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPROGRAM APIENTRY16 PRFCREATEGROUP( HINI, PSZ16, UCHAR );
PCHAR szPrf16CreateGroup = "Prf16CreateGroup";

HPROGRAM APIENTRY16 Trc_Prf16CreateGroup( HINI  Parm1
                                        , PSZ16 Parm2
                                        , UCHAR Parm3
                                        )
{

   HPROGRAM ulRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16CreateGroup );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_UCHAR );
      T_LogUCHAR( Parm3 );
   }
   HOOKENTRY16( szPrf16CreateGroup, &Parm1 );
   T_FreeLog( );

   ulRC = PRFCREATEGROUP( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16CreateGroup );
   T_LogPMReturnCode( DT_HPROGRAM, ulRC, ulRC );
   HOOKEXIT16( szPrf16CreateGroup, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf16CreateGroup() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16DestroyGroup()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFDESTROYGROUP( HINI, HPROGRAM );
PCHAR szPrf16DestroyGroup = "Prf16DestroyGroup";

BOOL16 APIENTRY16 Trc_Prf16DestroyGroup( HINI     Parm1
                                       , HPROGRAM Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16DestroyGroup );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szPrf16DestroyGroup, &Parm1 );
   T_FreeLog( );

   bRC = PRFDESTROYGROUP( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16DestroyGroup );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPrf16DestroyGroup, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16DestroyGroup() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32DestroyGroup()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32DESTROYGROUP( HINI, HPROGRAM );
PCHAR szPrf32DestroyGroup = "Prf32DestroyGroup";

BOOL APIENTRY Trc_Prf32DestroyGroup( HINI     Parm1
                                   , HPROGRAM Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32DestroyGroup );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szPrf32DestroyGroup, &Parm1 );
   T_FreeLog( );

   bRC = PRF32DESTROYGROUP( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32DestroyGroup );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPrf32DestroyGroup, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32DestroyGroup() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16OpenProfile()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HINI APIENTRY16 PRFOPENPROFILE( HAB, PSZ16 );
PCHAR szPrf16OpenProfile = "Prf16OpenProfile";

HINI APIENTRY16 Trc_Prf16OpenProfile( HAB   Parm1
                                    , PSZ16 Parm2
                                    )
{

   HINI ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16OpenProfile );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szPrf16OpenProfile, &Parm1 );
   T_FreeLog( );

   ulRC = PRFOPENPROFILE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16OpenProfile );
   T_LogPMReturnCode( DT_HINI, ulRC, ulRC );
   HOOKEXIT16( szPrf16OpenProfile, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf16OpenProfile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32OpenProfile()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HINI APIENTRY PRF32OPENPROFILE( HAB, PSZ );
PCHAR szPrf32OpenProfile = "Prf32OpenProfile";

HINI APIENTRY Trc_Prf32OpenProfile( HAB Parm1
                                  , PSZ Parm2
                                  )
{

   HINI ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32OpenProfile );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szPrf32OpenProfile, &Parm1 );
   T_FreeLog( );

   ulRC = PRF32OPENPROFILE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32OpenProfile );
   T_LogPMReturnCode( DT_HINI, ulRC, ulRC );
   HOOKEXIT( szPrf32OpenProfile, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf32OpenProfile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryDefinition()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 PRFQUERYDEFINITION( HINI, HPROGRAM, PPROGDETAILS16, ULONG );
PCHAR szPrf16QueryDefinition = "Prf16QueryDefinition";

ULONG APIENTRY16 Trc_Prf16QueryDefinition( HINI           Parm1
                                         , HPROGRAM       Parm2
                                         , PPROGDETAILS16 Parm3
                                         , ULONG          Parm4
                                         )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryDefinition );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPROGDETAILS );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY16( szPrf16QueryDefinition, &Parm1 );
   T_FreeLog( );

   ulRC = PRFQUERYDEFINITION( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryDefinition );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PPROGDETAILS );
      if ( ( ulRC ) && ( Parm4 ) ) T_LogPPROGDETAILS16( Parm3 );
      else T_LogPVOID16( Parm3 );
   }
   HOOKEXIT16( szPrf16QueryDefinition, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf16QueryDefinition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32QueryDefinition()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY PRF32QUERYDEFINITION( HINI, HPROGRAM, PPROGDETAILS, ULONG );
PCHAR szPrf32QueryDefinition = "Prf32QueryDefinition";

ULONG APIENTRY Trc_Prf32QueryDefinition( HINI         Parm1
                                       , HPROGRAM     Parm2
                                       , PPROGDETAILS Parm3
                                       , ULONG        Parm4
                                       )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32QueryDefinition );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPROGDETAILS );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szPrf32QueryDefinition, &Parm1 );
   T_FreeLog( );

   ulRC = PRF32QUERYDEFINITION( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32QueryDefinition );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PPROGDETAILS );
      if ( ( ulRC ) && ( Parm4 ) ) T_LogPPROGDETAILS( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szPrf32QueryDefinition, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf32QueryDefinition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryProfile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFQUERYPROFILE( HAB, PPRFPROFILE16 );
PCHAR szPrf16QueryProfile = "Prf16QueryProfile";

BOOL16 APIENTRY16 Trc_Prf16QueryProfile( HAB           Parm1
                                       , PPRFPROFILE16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryProfile );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPRFPROFILE );
      T_LogPPRFPROFILE16( Parm2 );
   }
   HOOKENTRY16( szPrf16QueryProfile, &Parm1 );
   T_FreeLog( );

   bRC = PRFQUERYPROFILE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryProfile );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PPRFPROFILE );
      if ( bRC ) T_LogPPRFPROFILE16( Parm2 );
      else T_LogPVOID16( Parm2 );
   }
   HOOKEXIT16( szPrf16QueryProfile, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16QueryProfile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32QueryProfile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32QUERYPROFILE( HAB, PPRFPROFILE );
PCHAR szPrf32QueryProfile = "Prf32QueryProfile";

BOOL APIENTRY Trc_Prf32QueryProfile( HAB         Parm1
                                   , PPRFPROFILE Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32QueryProfile );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPRFPROFILE );
      T_LogPPRFPROFILE( Parm2 );
   }
   HOOKENTRY( szPrf32QueryProfile, &Parm1 );
   T_FreeLog( );

   bRC = PRF32QUERYPROFILE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32QueryProfile );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PPRFPROFILE );
      if ( bRC ) T_LogPPRFPROFILE( Parm2 );
      else T_LogPVOID( Parm2 );
   }
   HOOKEXIT( szPrf32QueryProfile, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32QueryProfile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryProfileData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFQUERYPROFILEDATA( HINI, PSZ16, PSZ16, PVOID16, PULONG16 );
PCHAR szPrf16QueryProfileData = "Prf16QueryProfileData";

BOOL16 APIENTRY16 Trc_Prf16QueryProfileData( HINI     Parm1
                                           , PSZ16    Parm2
                                           , PSZ16    Parm3
                                           , PVOID16  Parm4
                                           , PULONG16 Parm5
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryProfileData );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG16( Parm5 );
   }
   HOOKENTRY16( szPrf16QueryProfileData, &Parm1 );
   T_FreeLog( );

   bRC = PRFQUERYPROFILEDATA( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryProfileData );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      if ( ( Parm5 ) && ( bRC ) ) T_LogCHARBuffer16( Parm4, *Parm5 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG16( Parm5 );
   }
   HOOKEXIT16( szPrf16QueryProfileData, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16QueryProfileData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32QueryProfileData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32QUERYPROFILEDATA( HINI, PSZ, PSZ, PVOID, PULONG );
PCHAR szPrf32QueryProfileData = "Prf32QueryProfileData";

BOOL APIENTRY Trc_Prf32QueryProfileData( HINI   Parm1
                                       , PSZ    Parm2
                                       , PSZ    Parm3
                                       , PVOID  Parm4
                                       , PULONG Parm5
                                       )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32QueryProfileData );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
   }
   HOOKENTRY( szPrf32QueryProfileData, &Parm1 );
   T_FreeLog( );

   bRC = PRF32QUERYPROFILEDATA( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32QueryProfileData );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( Parm5 ) && ( bRC ) ) T_LogCHARBuffer( Parm4, *Parm5 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
   }
   HOOKEXIT( szPrf32QueryProfileData, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32QueryProfileData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryProfileInt()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 PRFQUERYPROFILEINT( HINI, PSZ16, PSZ16, SHORT );
PCHAR szPrf16QueryProfileInt = "Prf16QueryProfileInt";

SHORT APIENTRY16 Trc_Prf16QueryProfileInt( HINI  Parm1
                                         , PSZ16 Parm2
                                         , PSZ16 Parm3
                                         , SHORT Parm4
                                         )
{

   SHORT sRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryProfileInt );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_SHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szPrf16QueryProfileInt, &Parm1 );
   T_FreeLog( );

   sRC = PRFQUERYPROFILEINT( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryProfileInt );
   T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
   HOOKEXIT16( szPrf16QueryProfileInt, &Parm1, &sRC );
   T_FreeLog( );

   return sRC;

}  /* Trc_Prf16QueryProfileInt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32QueryProfileInt()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY PRF32QUERYPROFILEINT( HINI, PSZ, PSZ, LONG );
PCHAR szPrf32QueryProfileInt = "Prf32QueryProfileInt";

LONG APIENTRY Trc_Prf32QueryProfileInt( HINI Parm1
                                      , PSZ  Parm2
                                      , PSZ  Parm3
                                      , LONG Parm4
                                      )
{

   LONG lRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32QueryProfileInt );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_LONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szPrf32QueryProfileInt, &Parm1 );
   T_FreeLog( );

   lRC = PRF32QUERYPROFILEINT( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32QueryProfileInt );
   T_LogPMReturnCode( DT_LONG, lRC, lRC );
   HOOKEXIT( szPrf32QueryProfileInt, &Parm1, &lRC );
   T_FreeLog( );

   return lRC;

}  /* Trc_Prf32QueryProfileInt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryProfileSize()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFQUERYPROFILESIZE( HINI, PSZ16, PSZ16, PULONG16 );
PCHAR szPrf16QueryProfileSize = "Prf16QueryProfileSize";

BOOL16 APIENTRY16 Trc_Prf16QueryProfileSize( HINI     Parm1
                                           , PSZ16    Parm2
                                           , PSZ16    Parm3
                                           , PULONG16 Parm4
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryProfileSize );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPVOID16( Parm4 );
   }
   HOOKENTRY16( szPrf16QueryProfileSize, &Parm1 );
   T_FreeLog( );

   bRC = PRFQUERYPROFILESIZE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryProfileSize );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PULONG );
      T_LogPULONG16( Parm4 );
   }
   HOOKEXIT16( szPrf16QueryProfileSize, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16QueryProfileSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32QueryProfileSize()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32QUERYPROFILESIZE( HINI, PSZ, PSZ, PULONG );
PCHAR szPrf32QueryProfileSize = "Prf32QueryProfileSize";

BOOL APIENTRY Trc_Prf32QueryProfileSize( HINI   Parm1
                                       , PSZ    Parm2
                                       , PSZ    Parm3
                                       , PULONG Parm4
                                       )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32QueryProfileSize );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPVOID( Parm4 );
   }
   HOOKENTRY( szPrf32QueryProfileSize, &Parm1 );
   T_FreeLog( );

   bRC = PRF32QUERYPROFILESIZE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32QueryProfileSize );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PULONG );
      T_LogPULONG( Parm4 );
   }
   HOOKEXIT( szPrf32QueryProfileSize, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32QueryProfileSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryProfileString()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 PRFQUERYPROFILESTRING( HINI, PSZ16, PSZ16, PSZ16, PVOID16, ULONG );
PCHAR szPrf16QueryProfileString = "Prf16QueryProfileString";

ULONG APIENTRY16 Trc_Prf16QueryProfileString( HINI    Parm1
                                            , PSZ16   Parm2
                                            , PSZ16   Parm3
                                            , PSZ16   Parm4
                                            , PVOID16 Parm5
                                            , ULONG   Parm6
                                            )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryProfileString );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ16( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY16( szPrf16QueryProfileString, &Parm1 );
   T_FreeLog( );

   ulRC = PRFQUERYPROFILESTRING( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryProfileString );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID16( Parm5 );
      if ( ulRC ) T_LogCHARBuffer16( Parm5, ulRC );
   }
   HOOKEXIT16( szPrf16QueryProfileString, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf16QueryProfileString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32QueryProfileString()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY PRF32QUERYPROFILESTRING( HINI, PSZ, PSZ, PSZ, PVOID, ULONG );
PCHAR szPrf32QueryProfileString = "Prf32QueryProfileString";

ULONG APIENTRY Trc_Prf32QueryProfileString( HINI  Parm1
                                          , PSZ   Parm2
                                          , PSZ   Parm3
                                          , PSZ   Parm4
                                          , PVOID Parm5
                                          , ULONG Parm6
                                          )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32QueryProfileString );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szPrf32QueryProfileString, &Parm1 );
   T_FreeLog( );

   ulRC = PRF32QUERYPROFILESTRING( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32QueryProfileString );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      if ( ulRC ) T_LogCHARBuffer( Parm5, ulRC );
   }
   HOOKEXIT( szPrf32QueryProfileString, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf32QueryProfileString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryProgramCategory()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PROGCATEGORY16 APIENTRY16 PRFQUERYPROGRAMCATEGORY( HINI, PSZ16 );
PCHAR szPrf16QueryProgramCategory = "Prf16QueryProgramCategory";

PROGCATEGORY16 APIENTRY16 Trc_Prf16QueryProgramCategory( HINI  Parm1
                                                       , PSZ16 Parm2
                                                       )
{

   PROGCATEGORY16 chRC;                          /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryProgramCategory );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szPrf16QueryProgramCategory, &Parm1 );
   T_FreeLog( );

   chRC = PRFQUERYPROGRAMCATEGORY( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryProgramCategory );
   T_LogPMReturnCodeUCHAR( DT_PROGCATEGORY, chRC, TRUE );
   HOOKEXIT16( szPrf16QueryProgramCategory, &Parm1, &chRC );
   T_FreeLog( );

   return chRC;

}  /* Trc_Prf16QueryProgramCategory() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryProgramHandle()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 PRFQUERYPROGRAMHANDLE( HINI, PSZ16, PHPROGARRAY16, ULONG, PULONG16 );
PCHAR szPrf16QueryProgramHandle = "Prf16QueryProgramHandle";

ULONG APIENTRY16 Trc_Prf16QueryProgramHandle( HINI          Parm1
                                            , PSZ16         Parm2
                                            , PHPROGARRAY16 Parm3
                                            , ULONG         Parm4
                                            , PULONG16      Parm5
                                            )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryProgramHandle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PHPROGARRAY );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID16( Parm5 );
   }
   HOOKENTRY16( szPrf16QueryProgramHandle, &Parm1 );
   T_FreeLog( );

   ulRC = PRFQUERYPROGRAMHANDLE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryProgramHandle );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PHPROGARRAY );
      if ( ( Parm5 ) && ( ulRC ) ) T_LogPHPROGARRAY( Parm3, *Parm5 );
      else T_LogPVOID16( Parm3 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG16( Parm5 );
   }
   HOOKEXIT16( szPrf16QueryProgramHandle, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf16QueryProgramHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16QueryProgramTitles()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 PRFQUERYPROGRAMTITLES( HINI, HPROGRAM, PPROGTITLE16, ULONG, PULONG16 );
PCHAR szPrf16QueryProgramTitles = "Prf16QueryProgramTitles";

ULONG APIENTRY16 Trc_Prf16QueryProgramTitles( HINI         Parm1
                                            , HPROGRAM     Parm2
                                            , PPROGTITLE16 Parm3
                                            , ULONG        Parm4
                                            , PULONG16     Parm5
                                            )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16QueryProgramTitles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPROGTITLE );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID16( Parm5 );
   }
   HOOKENTRY16( szPrf16QueryProgramTitles, &Parm1 );
   T_FreeLog( );

   ulRC = PRFQUERYPROGRAMTITLES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16QueryProgramTitles );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PPROGTITLE );
      if ( ( Parm4 ) && ( Parm5 ) && ( ulRC ) ) T_LogPROGTITLEBuffer16( Parm3, *Parm5 );
      else T_LogPVOID16( Parm3 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG16( Parm5 );
   }
   HOOKEXIT16( szPrf16QueryProgramTitles, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf16QueryProgramTitles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32QueryProgramTitles()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY PRF32QUERYPROGRAMTITLES( HINI, HPROGRAM, PPROGTITLE, ULONG, PULONG );
PCHAR szPrf32QueryProgramTitles = "Prf32QueryProgramTitles";

ULONG APIENTRY Trc_Prf32QueryProgramTitles( HINI       Parm1
                                          , HPROGRAM   Parm2
                                          , PPROGTITLE Parm3
                                          , ULONG      Parm4
                                          , PULONG     Parm5
                                          )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32QueryProgramTitles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPROGTITLE );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID( Parm5 );
   }
   HOOKENTRY( szPrf32QueryProgramTitles, &Parm1 );
   T_FreeLog( );

   ulRC = PRF32QUERYPROGRAMTITLES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32QueryProgramTitles );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PPROGTITLE );
      if ( ( Parm4 ) && ( Parm5 ) && ( ulRC ) ) T_LogPROGTITLEBuffer( Parm3, *Parm5 );
      else T_LogPVOID( Parm3 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
   }
   HOOKEXIT( szPrf32QueryProgramTitles, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prf32QueryProgramTitles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16RemoveProgram()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFREMOVEPROGRAM( HINI, HPROGRAM );
PCHAR szPrf16RemoveProgram = "Prf16RemoveProgram";

BOOL16 APIENTRY16 Trc_Prf16RemoveProgram( HINI     Parm1
                                        , HPROGRAM Parm2
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16RemoveProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szPrf16RemoveProgram, &Parm1 );
   T_FreeLog( );

   bRC = PRFREMOVEPROGRAM( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16RemoveProgram );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPrf16RemoveProgram, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16RemoveProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32RemoveProgram()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32REMOVEPROGRAM( HINI, HPROGRAM );
PCHAR szPrf32RemoveProgram = "Prf32RemoveProgram";

BOOL APIENTRY Trc_Prf32RemoveProgram( HINI     Parm1
                                    , HPROGRAM Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32RemoveProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szPrf32RemoveProgram, &Parm1 );
   T_FreeLog( );

   bRC = PRF32REMOVEPROGRAM( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32RemoveProgram );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPrf32RemoveProgram, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32RemoveProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16Reset()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFRESET( HAB, PPRFPROFILE16 );
PCHAR szPrf16Reset = "Prf16Reset";

BOOL16 APIENTRY16 Trc_Prf16Reset( HAB           Parm1
                                , PPRFPROFILE16 Parm2
                                )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16Reset );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPRFPROFILE );
      T_LogPPRFPROFILE16( Parm2 );
   }
   HOOKENTRY16( szPrf16Reset, &Parm1 );
   T_FreeLog( );

   bRC = PRFRESET( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16Reset );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPrf16Reset, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16Reset() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32Reset()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32RESET( HAB, PPRFPROFILE );
PCHAR szPrf32Reset = "Prf32Reset";

BOOL APIENTRY Trc_Prf32Reset( HAB         Parm1
                            , PPRFPROFILE Parm2
                            )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32Reset );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPRFPROFILE );
      T_LogPPRFPROFILE( Parm2 );
   }
   HOOKENTRY( szPrf32Reset, &Parm1 );
   T_FreeLog( );

   bRC = PRF32RESET( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32Reset );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPrf32Reset, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32Reset() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16WriteProfileData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFWRITEPROFILEDATA( HINI, PSZ16, PSZ16, PVOID16, ULONG );
PCHAR szPrf16WriteProfileData = "Prf16WriteProfileData";

BOOL16 APIENTRY16 Trc_Prf16WriteProfileData( HINI    Parm1
                                           , PSZ16   Parm2
                                           , PSZ16   Parm3
                                           , PVOID16 Parm4
                                           , ULONG   Parm5
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16WriteProfileData );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      T_LogCHARBuffer16( Parm4, Parm5 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY16( szPrf16WriteProfileData, &Parm1 );
   T_FreeLog( );

   bRC = PRFWRITEPROFILEDATA( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16WriteProfileData );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPrf16WriteProfileData, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16WriteProfileData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32WriteProfileData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32WRITEPROFILEDATA( HINI, PSZ, PSZ, PVOID, ULONG );
PCHAR szPrf32WriteProfileData = "Prf32WriteProfileData";

BOOL APIENTRY Trc_Prf32WriteProfileData( HINI  Parm1
                                       , PSZ   Parm2
                                       , PSZ   Parm3
                                       , PVOID Parm4
                                       , ULONG Parm5
                                       )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32WriteProfileData );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogCHARBuffer( Parm4, Parm5 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY( szPrf32WriteProfileData, &Parm1 );
   T_FreeLog( );

   bRC = PRF32WRITEPROFILEDATA( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32WriteProfileData );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPrf32WriteProfileData, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32WriteProfileData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf16WriteProfileString()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 PRFWRITEPROFILESTRING( HINI, PSZ16, PSZ16, PSZ16 );
PCHAR szPrf16WriteProfileString = "Prf16WriteProfileString";

BOOL16 APIENTRY16 Trc_Prf16WriteProfileString( HINI  Parm1
                                             , PSZ16 Parm2
                                             , PSZ16 Parm3
                                             , PSZ16 Parm4
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrf16WriteProfileString );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ16( Parm4 );
   }
   HOOKENTRY16( szPrf16WriteProfileString, &Parm1 );
   T_FreeLog( );

   bRC = PRFWRITEPROFILESTRING( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrf16WriteProfileString );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szPrf16WriteProfileString, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf16WriteProfileString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prf32WriteProfileString()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY PRF32WRITEPROFILESTRING( HINI, PSZ, PSZ, PSZ );
PCHAR szPrf32WriteProfileString = "Prf32WriteProfileString";

BOOL APIENTRY Trc_Prf32WriteProfileString( HINI Parm1
                                         , PSZ  Parm2
                                         , PSZ  Parm3
                                         , PSZ  Parm4
                                         )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrf32WriteProfileString );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ( Parm4 );
   }
   HOOKENTRY( szPrf32WriteProfileString, &Parm1 );
   T_FreeLog( );

   bRC = PRF32WRITEPROFILESTRING( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrf32WriteProfileString );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szPrf32WriteProfileString, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Prf32WriteProfileString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16AddProgram()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPROGRAM APIENTRY16 WINADDPROGRAM( HAB, PPIBSTRUCT16, HPROGRAM );
PCHAR szWin16AddProgram = "Win16AddProgram";

HPROGRAM APIENTRY16 Trc_Win16AddProgram( HAB          Parm1
                                       , PPIBSTRUCT16 Parm2
                                       , HPROGRAM     Parm3
                                       )
{

   HPROGRAM ulRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16AddProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPIBSTRUCT );
      T_LogPVOID16( Parm2 );
      T_LogPPIBSTRUCT( Parm2, sizeof( PIBSTRUCT16 ) );
      T_LogParameter( 3L, DT_HPROGRAM );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY16( szWin16AddProgram, &Parm1 );
   T_FreeLog( );

   ulRC = WINADDPROGRAM( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin16AddProgram );
   T_LogPMReturnCode( DT_HPROGRAM, ulRC, ulRC );
   HOOKEXIT16( szWin16AddProgram, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16AddProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16AddSwitchEntry()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HSWITCH APIENTRY16 WINADDSWITCHENTRY( PSWCNTRL16 );
PCHAR szWin16AddSwitchEntry = "Win16AddSwitchEntry";

HSWITCH APIENTRY16 Trc_Win16AddSwitchEntry( PSWCNTRL16 Parm1 )
{

   HSWITCH ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16AddSwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSWCNTRL );
      T_LogPSWCNTRL16( Parm1 );
   }
   HOOKENTRY16( szWin16AddSwitchEntry, &Parm1 );
   T_FreeLog( );

   ulRC = WINADDSWITCHENTRY( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16AddSwitchEntry );
   T_LogPMReturnCode( DT_HSWITCH, ulRC, ulRC );
   HOOKEXIT16( szWin16AddSwitchEntry, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16AddSwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32AddSwitchEntry()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HSWITCH APIENTRY WIN32ADDSWITCHENTRY( PSWCNTRL );
PCHAR szWin32AddSwitchEntry = "Win32AddSwitchEntry";

HSWITCH APIENTRY Trc_Win32AddSwitchEntry( PSWCNTRL Parm1 )
{

   HSWITCH ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32AddSwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSWCNTRL );
      T_LogPSWCNTRL( Parm1 );
   }
   HOOKENTRY( szWin32AddSwitchEntry, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32ADDSWITCHENTRY( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32AddSwitchEntry );
   T_LogPMReturnCode( DT_HSWITCH, ulRC, ulRC );
   HOOKEXIT( szWin32AddSwitchEntry, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32AddSwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16AddToGroup()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINADDTOGROUP( HPROGRAM, HPROGRAM );
PCHAR szWin16AddToGroup = "Win16AddToGroup";

USHORT APIENTRY16 Trc_Win16AddToGroup( HPROGRAM Parm1
                                     , HPROGRAM Parm2
                                     )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16AddToGroup );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPROGRAM );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szWin16AddToGroup, &Parm1 );
   T_FreeLog( );

   usRC = WINADDTOGROUP( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16AddToGroup );
   T_LogPMReturnCode( DT_USHORT, usRC, usRC );
   HOOKEXIT16( szWin16AddToGroup, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16AddToGroup() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ChangeProgram()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCHANGEPROGRAM( HPROGRAM, PPIBSTRUCT16 );
PCHAR szWin16ChangeProgram = "Win16ChangeProgram";

BOOL16 APIENTRY16 Trc_Win16ChangeProgram( HPROGRAM     Parm1
                                        , PPIBSTRUCT16 Parm2
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16ChangeProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPROGRAM );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPIBSTRUCT );
      T_LogPVOID16( Parm2 );
      T_LogPPIBSTRUCT( Parm2, sizeof( PIBSTRUCT16 ) );
   }
   HOOKENTRY16( szWin16ChangeProgram, &Parm1 );
   T_FreeLog( );

   bRC = WINCHANGEPROGRAM( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16ChangeProgram );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16ChangeProgram, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16ChangeProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ChangeSwitchEntry()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINCHANGESWITCHENTRY( HSWITCH, PSWCNTRL16 );
PCHAR szWin16ChangeSwitchEntry = "Win16ChangeSwitchEntry";

USHORT APIENTRY16 Trc_Win16ChangeSwitchEntry( HSWITCH    Parm1
                                            , PSWCNTRL16 Parm2
                                            )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16ChangeSwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSWCNTRL );
      T_LogPSWCNTRL16( Parm2 );
   }
   HOOKENTRY16( szWin16ChangeSwitchEntry, &Parm1 );
   T_FreeLog( );

   usRC = WINCHANGESWITCHENTRY( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16ChangeSwitchEntry );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   HOOKEXIT16( szWin16ChangeSwitchEntry, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16ChangeSwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ChangeSwitchEntry()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32CHANGESWITCHENTRY( HSWITCH, PSWCNTRL );
PCHAR szWin32ChangeSwitchEntry = "Win32ChangeSwitchEntry";

ULONG APIENTRY Trc_Win32ChangeSwitchEntry( HSWITCH  Parm1
                                         , PSWCNTRL Parm2
                                         )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32ChangeSwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSWCNTRL );
      T_LogPSWCNTRL( Parm2 );
   }
   HOOKENTRY( szWin32ChangeSwitchEntry, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32CHANGESWITCHENTRY( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32ChangeSwitchEntry );
   T_LogPMReturnCode( DT_ULONG, ulRC, !ulRC );
   HOOKEXIT( szWin32ChangeSwitchEntry, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32ChangeSwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateGroup()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPROGRAM APIENTRY16 WINCREATEGROUP( HAB, PSZ16, UCHAR, ULONG, ULONG );
PCHAR szWin16CreateGroup = "Win16CreateGroup";

HPROGRAM APIENTRY16 Trc_Win16CreateGroup( HAB   Parm1
                                        , PSZ16 Parm2
                                        , UCHAR Parm3
                                        , ULONG Parm4
                                        , ULONG Parm5
                                        )
{

   HPROGRAM ulRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16CreateGroup );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_UCHAR );
      T_LogUCHAR( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY16( szWin16CreateGroup, &Parm1 );
   T_FreeLog( );

   ulRC = WINCREATEGROUP( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin16CreateGroup );
   T_LogPMReturnCode( DT_HPROGRAM, ulRC, ulRC );
   HOOKEXIT16( szWin16CreateGroup, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16CreateGroup() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateSwitchEntry()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HSWITCH APIENTRY16 WINCREATESWITCHENTRY( HAB, PSWCNTRL16 );
PCHAR szWin16CreateSwitchEntry = "Win16CreateSwitchEntry";

HSWITCH APIENTRY16 Trc_Win16CreateSwitchEntry( HAB        Parm1
                                             , PSWCNTRL16 Parm2
                                             )
{

   HSWITCH ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16CreateSwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSWCNTRL );
      T_LogPSWCNTRL16( Parm2 );
   }
   HOOKENTRY16( szWin16CreateSwitchEntry, &Parm1 );
   T_FreeLog( );

   ulRC = WINCREATESWITCHENTRY( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16CreateSwitchEntry );
   T_LogPMReturnCode( DT_HSWITCH, ulRC, ulRC );
   HOOKEXIT16( szWin16CreateSwitchEntry, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16CreateSwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateSwitchEntry()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HSWITCH APIENTRY WIN32CREATESWITCHENTRY( HAB, PSWCNTRL );
PCHAR szWin32CreateSwitchEntry = "Win32CreateSwitchEntry";

HSWITCH APIENTRY Trc_Win32CreateSwitchEntry( HAB      Parm1
                                           , PSWCNTRL Parm2
                                           )
{

   HSWITCH ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32CreateSwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSWCNTRL );
      T_LogPSWCNTRL( Parm2 );
   }
   HOOKENTRY( szWin32CreateSwitchEntry, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32CREATESWITCHENTRY( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32CreateSwitchEntry );
   T_LogPMReturnCode( DT_HSWITCH, ulRC, ulRC );
   HOOKEXIT( szWin32CreateSwitchEntry, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32CreateSwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyGroup()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDESTROYGROUP( HPROGRAM );
PCHAR szWin16DestroyGroup = "Win16DestroyGroup";

BOOL16 APIENTRY16 Trc_Win16DestroyGroup( HPROGRAM Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16DestroyGroup );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPROGRAM );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16DestroyGroup, &Parm1 );
   T_FreeLog( );

   bRC = WINDESTROYGROUP( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16DestroyGroup );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16DestroyGroup, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16DestroyGroup() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EndProgram()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINENDPROGRAM( HSWITCH, USHORT );
PCHAR szWin16EndProgram = "Win16EndProgram";

USHORT APIENTRY16 Trc_Win16EndProgram( HSWITCH Parm1
                                     , USHORT  Parm2
                                     )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16EndProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szWin16EndProgram, &Parm1 );
   T_FreeLog( );

   usRC = WINENDPROGRAM( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16EndProgram );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   HOOKEXIT16( szWin16EndProgram, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16EndProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16HAPPfromPID()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HAPP APIENTRY16 WINHAPPFROMPID( PID16 );
PCHAR szWin16HAPPfromPID = "Win16HAPPfromPID";

HAPP APIENTRY16 Trc_Win16HAPPfromPID( PID16 Parm1 )
{

   HAPP ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16HAPPfromPID );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PID );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szWin16HAPPfromPID, &Parm1 );
   T_FreeLog( );

   ulRC = WINHAPPFROMPID( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16HAPPfromPID );
   T_LogPMReturnCode( DT_HAPP, ulRC, ulRC );
   HOOKEXIT16( szWin16HAPPfromPID, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16HAPPfromPID() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16HSWITCHfromHAPP()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HSWITCH APIENTRY16 WINHSWITCHFROMHAPP( PID16 );
PCHAR szWin16HSWITCHfromHAPP = "Win16HSWITCHfromHAPP";

HSWITCH APIENTRY16 Trc_Win16HSWITCHfromHAPP( HAPP Parm1 )
{

   HSWITCH ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16HSWITCHfromHAPP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAPP );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16HSWITCHfromHAPP, &Parm1 );
   T_FreeLog( );

   ulRC = WINHSWITCHFROMHAPP( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16HSWITCHfromHAPP );
   T_LogPMReturnCode( DT_HAPP, ulRC, ulRC );
   HOOKEXIT16( szWin16HSWITCHfromHAPP, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16HSWITCHfromHAPP() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16InstStartApp()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HAPP APIENTRY16 WININSTSTARTAPP( HINI, HWND, USHORT, PPSZ16, PSZ16, PVOID16, USHORT );
PCHAR szWin16InstStartApp = "Win16InstStartApp";

HAPP APIENTRY16 Trc_Win16InstStartApp( HINI         Parm1
                                     , HWND         Parm2
                                     , USHORT       Parm3
                                     , PPSZ16       Parm4
                                     , PSZ16        Parm5
                                     , PVOID16      Parm6
                                     , USHORT       Parm7
                                     )
{

   HAPP ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16InstStartApp );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HINI );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PSZFARSTAR );
      T_LogPSZBuffer16( Parm4, Parm3 );
      T_LogParameter( 5L, DT_PSZ );
      T_LogPSZ16( Parm5 );
      T_LogParameter( 6L, DT_PVOID );
      T_LogPVOID16( Parm6 );
      T_LogParameter( 7L, DT_USHORT );
      T_LogUSHORT( Parm7 );
   }
   HOOKENTRY16( szWin16InstStartApp, &Parm1 );
   T_FreeLog( );

   ulRC = WININSTSTARTAPP( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szWin16InstStartApp );
   T_LogPMReturnCode( DT_HAPP, ulRC, ulRC );
   HOOKEXIT16( szWin16InstStartApp, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16InstStartApp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16NoShutdown()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINNOSHUTDOWN( USHORT, BOOL16 );
PCHAR szWin16NoShutdown = "Win16NoShutdown";

BOOL16 APIENTRY16 Trc_Win16NoShutdown( USHORT Parm1
                                     , BOOL16 Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16NoShutdown );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_BOOL );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szWin16NoShutdown, &Parm1 );
   T_FreeLog( );

   bRC = WINNOSHUTDOWN( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16NoShutdown );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16NoShutdown, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16NoShutdown() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryDefinition()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYDEFINITION( HAB, HPROGRAM, PPIBSTRUCT16, USHORT );
PCHAR szWin16QueryDefinition = "Win16QueryDefinition";

USHORT APIENTRY16 Trc_Win16QueryDefinition( HAB          Parm1
                                          , HPROGRAM     Parm2
                                          , PPIBSTRUCT16 Parm3
                                          , USHORT       Parm4
                                          )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryDefinition );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPIBSTRUCT );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szWin16QueryDefinition, &Parm1 );
   T_FreeLog( );

   usRC = WINQUERYDEFINITION( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryDefinition );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PPIBSTRUCT );
      if ( ( usRC ) && ( Parm4 ) ) T_LogPPIBSTRUCT( Parm3, Parm4 );
      else T_LogPVOID16( Parm3 );
   }
   HOOKEXIT16( szWin16QueryDefinition, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16QueryDefinition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryProfileData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYPROFILEDATA( HAB, PSZ16, PSZ16, PVOID16, PUSHORT16 );
PCHAR szWin16QueryProfileData = "Win16QueryProfileData";

BOOL16 APIENTRY16 Trc_Win16QueryProfileData( HAB       Parm1
                                           , PSZ16     Parm2
                                           , PSZ16     Parm3
                                           , PVOID16   Parm4
                                           , PUSHORT16 Parm5
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryProfileData );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
   }
   HOOKENTRY16( szWin16QueryProfileData, &Parm1 );
   T_FreeLog( );

   bRC = WINQUERYPROFILEDATA( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryProfileData );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      if ( ( Parm5 ) && ( bRC ) ) T_LogCHARBuffer16( Parm4, *Parm5 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
   }
   HOOKEXIT16( szWin16QueryProfileData, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16QueryProfileData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryProfileInt()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINQUERYPROFILEINT( HAB, PSZ16, PSZ16, SHORT );
PCHAR szWin16QueryProfileInt = "Win16QueryProfileInt";

SHORT APIENTRY16 Trc_Win16QueryProfileInt( HAB   Parm1
                                         , PSZ16 Parm2
                                         , PSZ16 Parm3
                                         , SHORT Parm4
                                         )
{

   SHORT sRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryProfileInt );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_SHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szWin16QueryProfileInt, &Parm1 );
   T_FreeLog( );

   sRC = WINQUERYPROFILEINT( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryProfileInt );
   T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
   HOOKEXIT16( szWin16QueryProfileInt, &Parm1, &sRC );
   T_FreeLog( );

   return sRC;

}  /* Trc_Win16QueryProfileInt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryProfileSize()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYPROFILESIZE( HAB, PSZ16, PSZ16, PUSHORT16 );
PCHAR szWin16QueryProfileSize = "Win16QueryProfileSize";

USHORT APIENTRY16 Trc_Win16QueryProfileSize( HAB       Parm1
                                           , PSZ16     Parm2
                                           , PSZ16     Parm3
                                           , PUSHORT16 Parm4
                                           )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryProfileSize );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPVOID16( Parm4 );
   }
   HOOKENTRY16( szWin16QueryProfileSize, &Parm1 );
   T_FreeLog( );

   usRC = WINQUERYPROFILESIZE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryProfileSize );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
   }
   HOOKEXIT16( szWin16QueryProfileSize, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16QueryProfileSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryProfileString()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYPROFILESTRING( HAB, PSZ16, PSZ16, PSZ16, PVOID16, USHORT );
PCHAR szWin16QueryProfileString = "Win16QueryProfileString";

USHORT APIENTRY16 Trc_Win16QueryProfileString( HAB     Parm1
                                             , PSZ16   Parm2
                                             , PSZ16   Parm3
                                             , PSZ16   Parm4
                                             , PVOID16 Parm5
                                             , USHORT  Parm6
                                             )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryProfileString );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ16( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_USHORT );
      T_LogUSHORT( Parm6 );
   }
   HOOKENTRY16( szWin16QueryProfileString, &Parm1 );
   T_FreeLog( );

   usRC = WINQUERYPROFILESTRING( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryProfileString );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID16( Parm5 );
      if ( usRC ) T_LogCHARBuffer16( Parm5, usRC );
   }
   HOOKEXIT16( szWin16QueryProfileString, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16QueryProfileString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryProgramTitles()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYPROGRAMTITLES( HAB, HPROGRAM, PPROGRAMENTRY16, USHORT, PUSHORT16 );
PCHAR szWin16QueryProgramTitles = "Win16QueryProgramTitles";

BOOL16 APIENTRY16 Trc_Win16QueryProgramTitles( HAB             Parm1
                                             , HPROGRAM        Parm2
                                             , PPROGRAMENTRY16 Parm3
                                             , USHORT          Parm4
                                             , PUSHORT16       Parm5
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryProgramTitles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPROGRAMENTRY );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPVOID16( Parm5 );
   }
   HOOKENTRY16( szWin16QueryProgramTitles, &Parm1 );
   T_FreeLog( );

   bRC = WINQUERYPROGRAMTITLES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryProgramTitles );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PPROGRAMENTRY );
      if ( ( Parm4 ) && ( Parm5 ) && ( bRC ) ) T_LogPROGRAMENTRYBuffer( Parm3, *Parm5 );
      else T_LogPVOID16( Parm3 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
   }
   HOOKEXIT16( szWin16QueryProgramTitles, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16QueryProgramTitles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryProgramType()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PROGCATEGORY16 APIENTRY16 WINQUERYPROGRAMTYPE( PSZ16 );
PCHAR szWin16QueryProgramType = "Win16QueryProgramType";

PROGCATEGORY16 APIENTRY16 Trc_Win16QueryProgramType( PSZ16 Parm1 )
{

   PROGCATEGORY16 chRC;                          /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryProgramType );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
   }
   HOOKENTRY16( szWin16QueryProgramType, &Parm1 );
   T_FreeLog( );

   chRC = WINQUERYPROGRAMTYPE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryProgramType );
   T_LogPMReturnCodeUCHAR( DT_PROGCATEGORY, chRC, TRUE );
   HOOKEXIT16( szWin16QueryProgramType, &Parm1, &chRC );
   T_FreeLog( );

   return chRC;

}  /* Trc_Win16QueryProgramType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryProgramUse()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYPROGRAMUSE( PSZ16 );
PCHAR szWin16QueryProgramUse = "Win16QueryProgramUse";

BOOL16 APIENTRY16 Trc_Win16QueryProgramUse( PSZ16 Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryProgramUse );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
   }
   HOOKENTRY16( szWin16QueryProgramUse, &Parm1 );
   T_FreeLog( );

   bRC = WINQUERYPROGRAMUSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryProgramUse );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16QueryProgramUse, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16QueryProgramUse() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySessionTitle()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYSESSIONTITLE( HAB, USHORT, PSZ16, USHORT );
PCHAR szWin16QuerySessionTitle = "Win16QuerySessionTitle";

USHORT APIENTRY16 Trc_Win16QuerySessionTitle( HAB    Parm1
                                            , USHORT Parm2
                                            , PSZ16  Parm3
                                            , USHORT Parm4
                                            )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QuerySessionTitle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szWin16QuerySessionTitle, &Parm1 );
   T_FreeLog( );

   usRC = WINQUERYSESSIONTITLE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QuerySessionTitle );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PSZ );
      if ( !usRC ) T_LogPSZ16( Parm3 );
      else T_LogPVOID16( Parm3 );
   }
   HOOKEXIT16( szWin16QuerySessionTitle, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16QuerySessionTitle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySessionTitle()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYSESSIONTITLE( HAB, ULONG, PSZ, ULONG );
PCHAR szWin32QuerySessionTitle = "Win32QuerySessionTitle";

ULONG APIENTRY Trc_Win32QuerySessionTitle( HAB   Parm1
                                         , ULONG Parm2
                                         , PSZ   Parm3
                                         , ULONG Parm4
                                         )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QuerySessionTitle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szWin32QuerySessionTitle, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32QUERYSESSIONTITLE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QuerySessionTitle );
   T_LogPMReturnCode( DT_ULONG, ulRC, !ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PSZ );
      if ( !ulRC ) T_LogPSZ( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szWin32QuerySessionTitle, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QuerySessionTitle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySwitchEntry()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYSWITCHENTRY( HSWITCH, PSWCNTRL16 );
PCHAR szWin16QuerySwitchEntry = "Win16QuerySwitchEntry";

USHORT APIENTRY16 Trc_Win16QuerySwitchEntry( HSWITCH    Parm1
                                           , PSWCNTRL16 Parm2
                                           )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QuerySwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSWCNTRL );
      T_LogPVOID16( Parm2 );
   }
   HOOKENTRY16( szWin16QuerySwitchEntry, &Parm1 );
   T_FreeLog( );

   usRC = WINQUERYSWITCHENTRY( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QuerySwitchEntry );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PSWCNTRL );
      if ( !usRC ) T_LogPSWCNTRL16( Parm2 );
      else T_LogPVOID16( Parm2 );
   }
   HOOKEXIT16( szWin16QuerySwitchEntry, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16QuerySwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySwitchEntry()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYSWITCHENTRY( HSWITCH, PSWCNTRL );
PCHAR szWin32QuerySwitchEntry = "Win32QuerySwitchEntry";

ULONG APIENTRY Trc_Win32QuerySwitchEntry( HSWITCH  Parm1
                                        , PSWCNTRL Parm2
                                        )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QuerySwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSWCNTRL );
      T_LogPVOID( Parm2 );
   }
   HOOKENTRY( szWin32QuerySwitchEntry, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32QUERYSWITCHENTRY( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QuerySwitchEntry );
   T_LogPMReturnCode( DT_ULONG, ulRC, !ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PSWCNTRL );
      if ( !ulRC ) T_LogPSWCNTRL( Parm2 );
      else T_LogPVOID( Parm2 );
   }
   HOOKEXIT( szWin32QuerySwitchEntry, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QuerySwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySwitchHandle()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HSWITCH APIENTRY16 WINQUERYSWITCHHANDLE( HWND, PID16 );
PCHAR szWin16QuerySwitchHandle = "Win16QuerySwitchHandle";

HSWITCH APIENTRY16 Trc_Win16QuerySwitchHandle( HWND  Parm1
                                             , PID16 Parm2
                                             )
{

   HSWITCH ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QuerySwitchHandle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PID );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szWin16QuerySwitchHandle, &Parm1 );
   T_FreeLog( );

   ulRC = WINQUERYSWITCHHANDLE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QuerySwitchHandle );
   T_LogPMReturnCode( DT_HSWITCH, ulRC, ulRC );
   HOOKEXIT16( szWin16QuerySwitchHandle, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16QuerySwitchHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySwitchHandle()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HSWITCH APIENTRY WIN32QUERYSWITCHHANDLE( HWND, PID );
PCHAR szWin32QuerySwitchHandle = "Win32QuerySwitchHandle";

HSWITCH APIENTRY Trc_Win32QuerySwitchHandle( HWND Parm1
                                           , PID  Parm2
                                           )
{

   HSWITCH ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QuerySwitchHandle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PID );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szWin32QuerySwitchHandle, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32QUERYSWITCHHANDLE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QuerySwitchHandle );
   T_LogPMReturnCode( DT_HSWITCH, ulRC, ulRC );
   HOOKEXIT( szWin32QuerySwitchHandle, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QuerySwitchHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySwitchList()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYSWITCHLIST( HAB, PSWBLOCK16, USHORT );
PCHAR szWin16QuerySwitchList = "Win16QuerySwitchList";

USHORT APIENTRY16 Trc_Win16QuerySwitchList( HAB        Parm1
                                          , PSWBLOCK16 Parm2
                                          , USHORT     Parm3
                                          )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QuerySwitchList );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSWBLOCK );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szWin16QuerySwitchList, &Parm1 );
   T_FreeLog( );

   usRC = WINQUERYSWITCHLIST( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QuerySwitchList );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PSWBLOCK );
      if ( ( usRC ) && ( Parm2 ) ) T_LogPSWBLOCK16( Parm2 );
      else T_LogPVOID16( Parm2 );
   }
   HOOKEXIT16( szWin16QuerySwitchList, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16QuerySwitchList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySwitchList()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYSWITCHLIST( HAB, PSWBLOCK, ULONG );
PCHAR szWin32QuerySwitchList = "Win32QuerySwitchList";

ULONG APIENTRY Trc_Win32QuerySwitchList( HAB      Parm1
                                       , PSWBLOCK Parm2
                                       , ULONG    Parm3
                                       )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QuerySwitchList );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSWBLOCK );
      T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32QuerySwitchList, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32QUERYSWITCHLIST( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QuerySwitchList );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PSWBLOCK );
      if ( ( ulRC ) && ( Parm2 ) ) T_LogPSWBLOCK( Parm2 );
      else T_LogPVOID( Parm2 );
   }
   HOOKEXIT( szWin32QuerySwitchList, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QuerySwitchList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryTaskSizePos()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYTASKSIZEPOS( HAB, USHORT, PSWP16 );
PCHAR szWin16QueryTaskSizePos = "Win16QueryTaskSizePos";

USHORT APIENTRY16 Trc_Win16QueryTaskSizePos( HAB    Parm1
                                           , USHORT Parm2
                                           , PSWP16 Parm3
                                           )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryTaskSizePos );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PSWP );
      T_LogPVOID16( Parm3 );
   }
   HOOKENTRY16( szWin16QueryTaskSizePos, &Parm1 );
   T_FreeLog( );

   usRC = WINQUERYTASKSIZEPOS( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryTaskSizePos );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PSWP );
      if ( !usRC ) T_LogPSWP16( Parm3 );
      else T_LogPVOID16( Parm3 );
   }
   HOOKEXIT16( szWin16QueryTaskSizePos, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16QueryTaskSizePos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryTaskSizePos()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYTASKSIZEPOS( HAB, ULONG, PSWP );
PCHAR szWin32QueryTaskSizePos = "Win32QueryTaskSizePos";

ULONG APIENTRY Trc_Win32QueryTaskSizePos( HAB   Parm1
                                        , ULONG Parm2
                                        , PSWP  Parm3
                                        )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryTaskSizePos );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PSWP );
      T_LogPVOID( Parm3 );
   }
   HOOKENTRY( szWin32QueryTaskSizePos, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32QUERYTASKSIZEPOS( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryTaskSizePos );
   T_LogPMReturnCode( DT_ULONG, ulRC, !ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PSWP );
      if ( !ulRC ) T_LogPSWP( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szWin32QueryTaskSizePos, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QueryTaskSizePos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryTaskTitle()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYTASKTITLE( USHORT, PSZ16, USHORT );
PCHAR szWin16QueryTaskTitle = "Win16QueryTaskTitle";

USHORT APIENTRY16 Trc_Win16QueryTaskTitle( USHORT Parm1
                                         , PSZ16  Parm2
                                         , USHORT Parm3
                                         )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryTaskTitle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szWin16QueryTaskTitle, &Parm1 );
   T_FreeLog( );

   usRC = WINQUERYTASKTITLE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryTaskTitle );
   T_LogPMReturnCode( DT_USHORT, usRC, !usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PSZ );
      if ( !usRC ) T_LogPSZ16( Parm2 );
      else T_LogPVOID16( Parm2 );
   }
   HOOKEXIT16( szWin16QueryTaskTitle, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16QueryTaskTitle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryTaskTitle()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYTASKTITLE( ULONG, PSZ, ULONG );
PCHAR szWin32QueryTaskTitle = "Win32QueryTaskTitle";

ULONG APIENTRY Trc_Win32QueryTaskTitle( ULONG Parm1
                                      , PSZ   Parm2
                                      , ULONG Parm3
                                      )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryTaskTitle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32QueryTaskTitle, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32QUERYTASKTITLE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryTaskTitle );
   T_LogPMReturnCode( DT_ULONG, ulRC, !ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PSZ );
      if ( !ulRC ) T_LogPSZ( Parm2 );
      else T_LogPVOID( Parm2 );
   }
   HOOKEXIT( szWin32QueryTaskTitle, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QueryTaskTitle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RemoveFromGroup()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINREMOVEFROMGROUP( HPROGRAM, HPROGRAM );
PCHAR szWin16RemoveFromGroup = "Win16RemoveFromGroup";

USHORT APIENTRY16 Trc_Win16RemoveFromGroup( HPROGRAM Parm1
                                          , HPROGRAM Parm2
                                          )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16RemoveFromGroup );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPROGRAM );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HPROGRAM );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szWin16RemoveFromGroup, &Parm1 );
   T_FreeLog( );

   usRC = WINREMOVEFROMGROUP( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16RemoveFromGroup );
   T_LogPMReturnCode( DT_USHORT, usRC, usRC );
   HOOKEXIT16( szWin16RemoveFromGroup, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16RemoveFromGroup() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RemoveProgram()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINREMOVEPROGRAM( HPROGRAM );
PCHAR szWin16RemoveProgram = "Win16RemoveProgram";

BOOL16 APIENTRY16 Trc_Win16RemoveProgram( HPROGRAM Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16RemoveProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPROGRAM );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16RemoveProgram, &Parm1 );
   T_FreeLog( );

   bRC = WINREMOVEPROGRAM( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16RemoveProgram );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16RemoveProgram, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16RemoveProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RemoveSwitchEntry()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINREMOVESWITCHENTRY( HSWITCH );
PCHAR szWin16RemoveSwitchEntry = "Win16RemoveSwitchEntry";

USHORT APIENTRY16 Trc_Win16RemoveSwitchEntry( HSWITCH Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16RemoveSwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16RemoveSwitchEntry, &Parm1 );
   T_FreeLog( );

   usRC = WINREMOVESWITCHENTRY( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16RemoveSwitchEntry );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   HOOKEXIT16( szWin16RemoveSwitchEntry, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16RemoveSwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RemoveSwitchEntry()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32REMOVESWITCHENTRY( HSWITCH );
PCHAR szWin32RemoveSwitchEntry = "Win32RemoveSwitchEntry";

ULONG APIENTRY Trc_Win32RemoveSwitchEntry( HSWITCH Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32RemoveSwitchEntry );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32RemoveSwitchEntry, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32REMOVESWITCHENTRY( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32RemoveSwitchEntry );
   T_LogPMReturnCode( DT_ULONG, ulRC, !ulRC );
   HOOKEXIT( szWin32RemoveSwitchEntry, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32RemoveSwitchEntry() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetFgndWindow()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINSETFGNDWINDOW( HWND, USHORT );
PCHAR szWin16SetFgndWindow = "Win16SetFgndWindow";

USHORT APIENTRY16 Trc_Win16SetFgndWindow( HWND   Parm1
                                        , USHORT Parm2
                                        )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SetFgndWindow );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szWin16SetFgndWindow, &Parm1 );
   T_FreeLog( );

   usRC = WINSETFGNDWINDOW( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SetFgndWindow );
   T_LogPMReturnCode( DT_USHORT, usRC, usRC );
   HOOKEXIT16( szWin16SetFgndWindow, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16SetFgndWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetTitle()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINSETTITLE( PSZ16 );
PCHAR szWin16SetTitle = "Win16SetTitle";

USHORT APIENTRY16 Trc_Win16SetTitle( PSZ16 Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SetTitle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
   }
   HOOKENTRY16( szWin16SetTitle, &Parm1 );
   T_FreeLog( );

   usRC = WINSETTITLE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SetTitle );
   T_LogPMReturnCodeUSHORT( DT_BOOL, usRC, !usRC );
   HOOKEXIT16( szWin16SetTitle, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16SetTitle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetTitleAndHwndIcon()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
BOOL16 APIENTRY16 WINSETTITLEANDHWNDICON( PSZ16, HWND );
PCHAR szWin16SetTitleAndHwndIcon = "Win16SetTitleAndHwndIcon";

BOOL16 APIENTRY16 Trc_Win16SetTitleAndHwndIcon( PSZ16 Parm1
                                              , HWND  Parm2
                                              )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SetTitleAndHwndIcon );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szWin16SetTitleAndHwndIcon, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnWIN16SETTITLEANDHWNDICON )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SetTitleAndHwndIcon );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16SetTitleAndHwndIcon, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16SetTitleAndHwndIcon() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetTitleAndIcon()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETTITLEANDICON( PSZ16, PSZ16 );
PCHAR szWin16SetTitleAndIcon = "Win16SetTitleAndIcon";

BOOL16 APIENTRY16 Trc_Win16SetTitleAndIcon( PSZ16 Parm1
                                          , PSZ16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SetTitleAndIcon );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szWin16SetTitleAndIcon, &Parm1 );
   T_FreeLog( );

   bRC = WINSETTITLEANDICON( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SetTitleAndIcon );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16SetTitleAndIcon, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16SetTitleAndIcon() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16StartApp()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HAPP APIENTRY16 WINSTARTAPP( HWND, PPROGDETAILS16, PSZ16, PVOID16, ULONG );
PCHAR szWin16StartApp = "Win16StartApp";

HAPP APIENTRY16 Trc_Win16StartApp( HWND           Parm1
                                 , PPROGDETAILS16 Parm2
                                 , PSZ16          Parm3
                                 , PVOID16        Parm4
                                 , ULONG          Parm5
                                 )
{

   HAPP ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16StartApp );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPROGDETAILS );
      T_LogPPROGDETAILS16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY16( szWin16StartApp, &Parm1 );
   T_FreeLog( );

   ulRC = WINSTARTAPP( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin16StartApp );
   T_LogPMReturnCode( DT_HAPP, ulRC, ulRC );
   HOOKEXIT16( szWin16StartApp, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16StartApp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32StartApp()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HAPP APIENTRY WIN32STARTAPP( HWND, PPROGDETAILS, PSZ, PVOID, ULONG );
PCHAR szWin32StartApp = "Win32StartApp";

HAPP APIENTRY Trc_Win32StartApp( HWND         Parm1
                               , PPROGDETAILS Parm2
                               , PSZ          Parm3
                               , PVOID        Parm4
                               , ULONG        Parm5
                               )
{

   HAPP ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32StartApp );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PPROGDETAILS );
      T_LogPPROGDETAILS( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY( szWin32StartApp, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32STARTAPP( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin32StartApp );
   T_LogPMReturnCode( DT_HAPP, ulRC, ulRC );
   HOOKEXIT( szWin32StartApp, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32StartApp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16StopProgram()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINSTOPPROGRAM( HSWITCH, USHORT );
PCHAR szWin16StopProgram = "Win16StopProgram";

USHORT APIENTRY16 Trc_Win16StopProgram( HSWITCH Parm1
                                      , USHORT  Parm2
                                      )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16StopProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szWin16StopProgram, &Parm1 );
   T_FreeLog( );

   usRC = WINSTOPPROGRAM( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16StopProgram );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   HOOKEXIT16( szWin16StopProgram, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16StopProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SwitchProgramRegister()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINSWITCHPROGRAMREGISTER( HWND, HMQ, PFNWP16 );
PCHAR szWin16SwitchProgramRegister = "Win16SwitchProgramRegister";

USHORT APIENTRY16 Trc_Win16SwitchProgramRegister( HWND    Parm1
                                                , HMQ     Parm2
                                                , PFNWP16 Parm3
                                                )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SwitchProgramRegister );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HMQ );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PFNWP );
      T_LogPFN16( (PFN16)Parm3 );
   }
   HOOKENTRY16( szWin16SwitchProgramRegister, &Parm1 );
   T_FreeLog( );

   usRC = WINSWITCHPROGRAMREGISTER( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SwitchProgramRegister );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   HOOKEXIT16( szWin16SwitchProgramRegister, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16SwitchProgramRegister() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SwitchProgramRegister()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32SWITCHPROGRAMREGISTER( HWND, HMQ );
PCHAR szWin32SwitchProgramRegister = "Win32SwitchProgramRegister";

ULONG APIENTRY Trc_Win32SwitchProgramRegister( HWND Parm1
                                             , HMQ  Parm2
                                             )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SwitchProgramRegister );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HMQ );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szWin32SwitchProgramRegister, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32SWITCHPROGRAMREGISTER( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SwitchProgramRegister );
   T_LogPMReturnCode( DT_ULONG, ulRC, !ulRC );
   HOOKEXIT( szWin32SwitchProgramRegister, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32SwitchProgramRegister() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SwitchToProgram()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINSWITCHTOPROGRAM( HSWITCH );
PCHAR szWin16SwitchToProgram = "Win16SwitchToProgram";

USHORT APIENTRY16 Trc_Win16SwitchToProgram( HSWITCH Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SwitchToProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16SwitchToProgram, &Parm1 );
   T_FreeLog( );

   usRC = WINSWITCHTOPROGRAM( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SwitchToProgram );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, !usRC );
   HOOKEXIT16( szWin16SwitchToProgram, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Win16SwitchToProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SwitchToProgram()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32SWITCHTOPROGRAM( HSWITCH );
PCHAR szWin32SwitchToProgram = "Win32SwitchToProgram";

ULONG APIENTRY Trc_Win32SwitchToProgram( HSWITCH Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SwitchToProgram );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSWITCH );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32SwitchToProgram, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32SWITCHTOPROGRAM( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SwitchToProgram );
   T_LogPMReturnCode( DT_ULONG, ulRC, !ulRC );
   HOOKEXIT( szWin32SwitchToProgram, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32SwitchToProgram() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16TerminateApp()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINTERMINATEAPP( HAPP );
PCHAR szWin16TerminateApp = "Win16TerminateApp";

BOOL16 APIENTRY16 Trc_Win16TerminateApp( HAPP Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16TerminateApp );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAPP );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16TerminateApp, &Parm1 );
   T_FreeLog( );

   bRC = WINTERMINATEAPP( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16TerminateApp );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16TerminateApp, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16TerminateApp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32TerminateApp()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32TERMINATEAPP( HAPP );
PCHAR szWin32TerminateApp = "Win32TerminateApp";

BOOL APIENTRY Trc_Win32TerminateApp( HAPP Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32TerminateApp );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAPP );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32TerminateApp, &Parm1 );
   T_FreeLog( );

   bRC = WIN32TERMINATEAPP( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32TerminateApp );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32TerminateApp, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32TerminateApp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16WriteProfileData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINWRITEPROFILEDATA( HAB, PSZ16, PSZ16, PVOID16, USHORT );
PCHAR szWin16WriteProfileData = "Win16WriteProfileData";

BOOL16 APIENTRY16 Trc_Win16WriteProfileData( HAB     Parm1
                                           , PSZ16   Parm2
                                           , PSZ16   Parm3
                                           , PVOID16 Parm4
                                           , USHORT  Parm5
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16WriteProfileData );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      T_LogCHARBuffer16( Parm4, Parm5 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szWin16WriteProfileData, &Parm1 );
   T_FreeLog( );

   bRC = WINWRITEPROFILEDATA( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin16WriteProfileData );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16WriteProfileData, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16WriteProfileData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16WriteProfileString()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINWRITEPROFILESTRING( HAB, PSZ16, PSZ16, PSZ16 );
PCHAR szWin16WriteProfileString = "Win16WriteProfileString";

BOOL16 APIENTRY16 Trc_Win16WriteProfileString( HAB   Parm1
                                             , PSZ16 Parm2
                                             , PSZ16 Parm3
                                             , PSZ16 Parm4
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16WriteProfileString );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ16( Parm4 );
   }
   HOOKENTRY16( szWin16WriteProfileString, &Parm1 );
   T_FreeLog( );

   bRC = WINWRITEPROFILESTRING( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin16WriteProfileString );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16WriteProfileString, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16WriteProfileString() */
#endif

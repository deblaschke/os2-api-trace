/******************************************************************************/
/*                                                                            */
/* File name    : T_GPI.C                                                     */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit PMGPI routines         */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from PMGPI.DLL.      */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.05 04Aug95 Added 32-bit Gpi APIs                       */
/*                2.30.09 12Aug95 Added 32-bit Dev APIs                       */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.10 14Mar97 Fixed Dev32StdOpen bug                      */
/*                2.40.27 22Jul97 Added 16-bit Gpi APIs                       */
/*                2.40.28 06Aug97 Added 16-bit Dev APIs                       */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.76 28Oct98 Issued more specific trace DLL load errors  */
/*                2.45.00 15Apr99 Added new OS/2 2.45 APIs                    */
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
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#define ORD_DEV16STDOPEN                  244    /* OS/2 2.10 proc ordinal    */
#endif
#define ORD_DEV32STDOPEN                  625    /* OS/2 2.10 proc ordinal    */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
#define ORD_DEV32POSTESCAPE               729    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32BEGININKPATH             670    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32CREATEELLIPTICREGION     662    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32CREATEPOLYGONREGION      663    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32CREATEROUNDRECTREGION    661    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32ENDINKPATH               671    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32QUERYFONTMAPPINGFLAGS    669    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32QUERYNEARESTPALETTEINDEX 667    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32QUERYRASTERCAPS          660    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32QUERYTABBEDTEXTEXTENT    658    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32RESIZEPALETTE            666    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32SETFONTMAPPINGFLAGS      668    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32STROKEINKPATH            672    /* OS/2 2.40 proc ordinal    */
#define ORD_GPI32TABBEDCHARSTRINGAT       659    /* OS/2 2.40 proc ordinal    */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
#define ORD_GPI32OVAL                     730    /* OS/2 2.45 proc ordinal    */
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
PCHAR   szDLL = "PMGPI";                         /* DLL module name           */
PCHAR   szTraceDLL = "T_GPI";                    /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( OS2VER >= 210 )
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
BOOL16 (* _Seg16 APIENTRY16 pfnDEVSTDOPEN)( HDC );
                                                 /* OS/2 2.10 procedure       */
#endif
BOOL (* APIENTRY            pfnDEV32STDOPEN)( HDC );
                                                 /* OS/2 2.10 procedure       */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG (* APIENTRY            pfnDEV32POSTESCAPE)( PSZ, PSZ, PSZ, PSZ, ULONG, ULONG, PBYTE, ULONG, PBYTE );
                                                 /* OS/2 2.40 procedure       */
BOOL (* APIENTRY            pfnGPI32BEGININKPATH)( HPS, LONG, ULONG );
                                                 /* OS/2 2.40 procedure       */
HRGN (* APIENTRY            pfnGPI32CREATEELLIPTICREGION)( HPS, PRECTL );
                                                 /* OS/2 2.40 procedure       */
HRGN (* APIENTRY            pfnGPI32CREATEPOLYGONREGION)( HPS, ULONG, PPOLYGON, ULONG );
                                                 /* OS/2 2.40 procedure       */
HRGN (* APIENTRY            pfnGPI32CREATEROUNDRECTREGION)( HPS, PPOINTL, LONG, LONG );
                                                 /* OS/2 2.40 procedure       */
BOOL (* APIENTRY            pfnGPI32ENDINKPATH)( HPS, ULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY           pfnGPI32QUERYFONTMAPPINGFLAGS)( HPS );
                                                 /* OS/2 2.40 procedure       */
LONG (* APIENTRY            pfnGPI32QUERYNEARESTPALETTEINDEX)( HPAL, ULONG );
                                                 /* OS/2 2.40 procedure       */
BOOL (* APIENTRY            pfnGPI32QUERYRASTERIZERCAPS)( PRASTERIZERCAPS );
                                                 /* OS/2 2.40 procedure       */
LONG (* APIENTRY            pfnGPI32QUERYTABBEDTEXTEXTENT)( HPS, LONG, PCH, LONG, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY           pfnGPI32RESIZEPALETTE)( HPAL, ULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY           pfnGPI32SETFONTMAPPINGFLAGS)( HPS, ULONG );
                                                 /* OS/2 2.40 procedure       */
LONG (* APIENTRY            pfnGPI32STROKEINKPATH)( HPS, LONG, LONG, PPOINTL, ULONG );
                                                 /* OS/2 2.40 procedure       */
LONG (* APIENTRY            pfnGPI32TABBEDCHARSTRINGAT)( HPS, PPOINTL, PRECTL, ULONG, LONG, PCH, LONG, PULONG, LONG );
                                                 /* OS/2 2.40 procedure       */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
LONG (* APIENTRY            pfnGPI32OVAL)( HPS, PPOINTL, FIXED, FIXED, ULONG );
                                                 /* OS/2 2.45 procedure       */
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
    * Library initialization required if process attaching to T_GPI
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
       * Log error and terminate if T_GPI version does not match T_COMMON
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
       * Load PMGPI library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_GPI library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMGPI trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

#if ( OS2VER >= 210 )
      /*
       * Obtain OS/2 2.10 procedure addresses if OS/2 version number greater
       * than or equal to 2.10
       */
      if ( T_OS2Ver >= MAKEVER( 2, 10 ) )
      {
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
         /*
          * Obtain 16-bit OS/2 2.10 procedure address, log error and
          * terminate if unsuccessful
          */
         if ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_DEV16STDOPEN ), (PPFN16)&pfnDEVSTDOPEN ) )
         {
            T_Error( TRUE, EM_GETPROC, 16, 2, 10, szDLL, ulOrdinal );
         }
#endif

         /*
          * Obtain 32-bit OS/2 2.10 procedure address, log error and
          * terminate if unsuccessful
          */
         if ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DEV32STDOPEN, NULL, (PFN *)&pfnDEV32STDOPEN ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 10, szDLL, ulOrdinal );
         }
      }
#endif

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
      /*
       * Obtain OS/2 2.40 procedure addresses if OS/2 version number greater
       * than or equal to 2.40
       */
      if ( T_OS2Ver >= MAKEVER( 2, 40 ) )
      {
         /*
          * Obtain OS/2 2.40 procedure addresses, log error and terminate if
          * unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DEV32POSTESCAPE              , NULL, (PFN *)&pfnDEV32POSTESCAPE               ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32BEGININKPATH            , NULL, (PFN *)&pfnGPI32BEGININKPATH             ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32CREATEELLIPTICREGION    , NULL, (PFN *)&pfnGPI32CREATEELLIPTICREGION     ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32CREATEPOLYGONREGION     , NULL, (PFN *)&pfnGPI32CREATEPOLYGONREGION      ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32CREATEROUNDRECTREGION   , NULL, (PFN *)&pfnGPI32CREATEROUNDRECTREGION    ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32ENDINKPATH              , NULL, (PFN *)&pfnGPI32ENDINKPATH               ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32QUERYFONTMAPPINGFLAGS   , NULL, (PFN *)&pfnGPI32QUERYFONTMAPPINGFLAGS    ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32QUERYNEARESTPALETTEINDEX, NULL, (PFN *)&pfnGPI32QUERYNEARESTPALETTEINDEX ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32QUERYRASTERCAPS         , NULL, (PFN *)&pfnGPI32QUERYRASTERIZERCAPS      ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32QUERYTABBEDTEXTEXTENT   , NULL, (PFN *)&pfnGPI32QUERYTABBEDTEXTEXTENT    ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32RESIZEPALETTE           , NULL, (PFN *)&pfnGPI32RESIZEPALETTE            ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32SETFONTMAPPINGFLAGS     , NULL, (PFN *)&pfnGPI32SETFONTMAPPINGFLAGS      ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32STROKEINKPATH           , NULL, (PFN *)&pfnGPI32STROKEINKPATH            ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32TABBEDCHARSTRINGAT      , NULL, (PFN *)&pfnGPI32TABBEDCHARSTRINGAT       ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 40, szDLL, ulOrdinal );
         }
      }
#endif

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
      /*
       * Obtain OS/2 2.45 procedure address if OS/2 version number greater than
       * or equal to 2.45
       */
      if ( T_OS2Ver >= MAKEVER( 2, 45 ) )
      {
         /*
          * Obtain OS/2 2.45 procedure address, log error and terminate if
          * unsuccessful
          */
         if ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32OVAL, NULL, (PFN *)&pfnGPI32OVAL ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 45, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_GPI
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMGPI library if library loaded
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
/* Trc_Dev16CloseDC()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HMF APIENTRY16 DEVCLOSEDC( HDC );
PCHAR szDev16CloseDC = "Dev16CloseDC";

HMF APIENTRY16 Trc_Dev16CloseDC( HDC Parm1 )
{

   HMF ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szDev16CloseDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szDev16CloseDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DEVCLOSEDC( Parm1 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev16CloseDC );
      T_LogPMReturnCode( DT_HMF, ulRC, ulRC != DEV_ERROR );
      HOOKEXIT16( szDev16CloseDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dev16CloseDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32CloseDC()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HMF APIENTRY DEV32CLOSEDC( HDC );
PCHAR szDev32CloseDC = "Dev32CloseDC";

HMF APIENTRY Trc_Dev32CloseDC( HDC Parm1 )
{

   HMF ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32CloseDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDev32CloseDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DEV32CLOSEDC( Parm1 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32CloseDC );
      T_LogPMReturnCode( DT_HMF, ulRC, ulRC != DEV_ERROR );
      HOOKEXIT( szDev32CloseDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dev32CloseDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev16Escape()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 DEVESCAPE( HDC, LONG, LONG, PBYTE16, PLONG16, PBYTE16 );
PCHAR szDev16Escape = "Dev16Escape";

LONG APIENTRY16 Trc_Dev16Escape( HDC     Parm1
                               , LONG    Parm2
                               , LONG    Parm3
                               , PBYTE16 Parm4
                               , PLONG16 Parm5
                               , PBYTE16 Parm6
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szDev16Escape );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( (PCHAR16)Parm4, Parm3 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm5 );
         T_LogParameter( 6L, DT_PBYTE );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szDev16Escape, &Parm1 );
      T_FreeLog( );
   }

   lRC = DEVESCAPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev16Escape );
      T_LogPMReturnCode( DT_LONG, lRC, lRC > DEVESC_NOTIMPLEMENTED );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm5 );
         T_LogParameter( 6L, DT_PBYTE );
         T_LogPVOID16( Parm6 );
         if ( ( Parm5 ) && ( lRC > DEVESC_NOTIMPLEMENTED ) ) T_LogCHARBuffer16( (PCHAR16)Parm6, *Parm5 );
      }
      HOOKEXIT16( szDev16Escape, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Dev16Escape() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32Escape()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY DEV32ESCAPE( HDC, LONG, LONG, PBYTE, PLONG, PBYTE );
PCHAR szDev32Escape = "Dev32Escape";

LONG APIENTRY Trc_Dev32Escape( HDC   Parm1
                             , LONG  Parm2
                             , LONG  Parm3
                             , PBYTE Parm4
                             , PLONG Parm5
                             , PBYTE Parm6
                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32Escape );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         T_LogCHARBuffer( (PCHAR)Parm4, Parm3 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm5 );
         T_LogParameter( 6L, DT_PBYTE );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szDev32Escape, &Parm1 );
      T_FreeLog( );
   }

   lRC = DEV32ESCAPE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32Escape );
      T_LogPMReturnCode( DT_LONG, lRC, lRC > DEVESC_NOTIMPLEMENTED );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm5 );
         T_LogParameter( 6L, DT_PBYTE );
         T_LogPVOID( Parm6 );
         if ( ( Parm5 ) && ( lRC > DEVESC_NOTIMPLEMENTED ) ) T_LogCHARBuffer( (PCHAR)Parm6, *Parm5 );
      }
      HOOKEXIT( szDev32Escape, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Dev32Escape() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev16OpenDC()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HDC APIENTRY16 DEVOPENDC( HAB, LONG, PSZ16, LONG, PDEVOPENDATA16, HDC );
PCHAR szDev16OpenDC = "Dev16OpenDC";

HDC APIENTRY16 Trc_Dev16OpenDC( HAB            Parm1
                              , LONG           Parm2
                              , PSZ16          Parm3
                              , LONG           Parm4
                              , PDEVOPENDATA16 Parm5
                              , HDC            Parm6
                              )
{

   HDC ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szDev16OpenDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PDEVOPENDATA );
         T_LogPDEVOPENSTRUC16( (PDEVOPENSTRUC16)Parm5, Parm4 );
         T_LogParameter( 6L, DT_HDC );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY16( szDev16OpenDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DEVOPENDC( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev16OpenDC );
      T_LogPMReturnCode( DT_HDC, ulRC, ulRC != DEV_ERROR );
      HOOKEXIT16( szDev16OpenDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dev16OpenDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32OpenDC()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HDC APIENTRY DEV32OPENDC( HAB, LONG, PSZ, LONG, PDEVOPENDATA, HDC );
PCHAR szDev32OpenDC = "Dev32OpenDC";

HDC APIENTRY Trc_Dev32OpenDC( HAB          Parm1
                            , LONG         Parm2
                            , PSZ          Parm3
                            , LONG         Parm4
                            , PDEVOPENDATA Parm5
                            , HDC          Parm6
                            )
{

   HDC ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32OpenDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PDEVOPENDATA );
         T_LogPDEVOPENSTRUC( (PDEVOPENSTRUC)Parm5, Parm4 );
         T_LogParameter( 6L, DT_HDC );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szDev32OpenDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = DEV32OPENDC( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32OpenDC );
      T_LogPMReturnCode( DT_HDC, ulRC, ulRC != DEV_ERROR );
      HOOKEXIT( szDev32OpenDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Dev32OpenDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev16PostDeviceModes()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 DEVPOSTDEVICEMODES( HAB, PDRIVDATA16, PSZ16, PSZ16, PSZ16, ULONG );
PCHAR szDev16PostDeviceModes = "Dev16PostDeviceModes";

LONG APIENTRY16 Trc_Dev16PostDeviceModes( HAB         Parm1
                                        , PDRIVDATA16 Parm2
                                        , PSZ16       Parm3
                                        , PSZ16       Parm4
                                        , PSZ16       Parm5
                                        , ULONG       Parm6
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szDev16PostDeviceModes );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PDRIVDATA );
         T_LogPDRIVDATA16( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ16( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZ16( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY16( szDev16PostDeviceModes, &Parm1 );
      T_FreeLog( );
   }

   lRC = DEVPOSTDEVICEMODES( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev16PostDeviceModes );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != DPDM_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PDRIVDATA );
         if ( lRC != DPDM_ERROR ) T_LogPDRIVDATA16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szDev16PostDeviceModes, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Dev16PostDeviceModes() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32PostDeviceModes()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY DEV32POSTDEVICEMODES( HAB, PDRIVDATA, PSZ, PSZ, PSZ, ULONG );
PCHAR szDev32PostDeviceModes = "Dev32PostDeviceModes";

LONG APIENTRY Trc_Dev32PostDeviceModes( HAB       Parm1
                                      , PDRIVDATA Parm2
                                      , PSZ       Parm3
                                      , PSZ       Parm4
                                      , PSZ       Parm5
                                      , ULONG     Parm6
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32PostDeviceModes );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PDRIVDATA );
         T_LogPDRIVDATA( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZ( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szDev32PostDeviceModes, &Parm1 );
      T_FreeLog( );
   }

   lRC = DEV32POSTDEVICEMODES( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32PostDeviceModes );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != DPDM_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PDRIVDATA );
         if ( lRC != DPDM_ERROR ) T_LogPDRIVDATA( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szDev32PostDeviceModes, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Dev32PostDeviceModes() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32PostEscape()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG APIENTRY DEVPOSTESCAPE( PSZ, PSZ, PSZ, PSZ, ULONG, ULONG, PBYTE, ULONG, PBYTE );
PCHAR szDev32PostEscape = "Dev32PostEscape";

#define DEVPE_ERROR -1L

LONG APIENTRY Trc_Dev32PostEscape( PSZ   Parm1
                                 , PSZ   Parm2
                                 , PSZ   Parm3
                                 , PSZ   Parm4
                                 , ULONG Parm5
                                 , ULONG Parm6
                                 , PBYTE Parm7
                                 , ULONG Parm8
                                 , PBYTE Parm9
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32PostEscape );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PBYTE );
         T_LogPVOID( Parm7 );
         T_LogCHARBuffer( (PCHAR)Parm7, Parm6 );
         T_LogParameter( 8L, DT_ULONG );
         T_LogULONG( Parm8 );
         T_LogParameter( 9L, DT_PBYTE );
         T_LogPVOID( Parm9 );
         T_LogCHARBuffer( (PCHAR)Parm9, Parm8 );
      }
      HOOKENTRY( szDev32PostEscape, &Parm1 );
      T_FreeLog( );
   }

   lRC = ( *pfnDEV32POSTESCAPE )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32PostEscape );
      T_LogPMReturnCode( DT_LONG, lRC, lRC > DEVPE_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PBYTE );
         T_LogPVOID( Parm7 );
         if ( lRC > DEVPE_ERROR ) T_LogCHARBuffer( (PCHAR)Parm7, Parm6 );
         T_LogParameter( 9L, DT_PBYTE );
         T_LogPVOID( Parm9 );
         if ( lRC > DEVPE_ERROR ) T_LogCHARBuffer( (PCHAR)Parm9, Parm8 );
      }
      HOOKEXIT( szDev32PostEscape, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Dev32PostEscape() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev16QueryCaps()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DEVQUERYCAPS( HDC, LONG, LONG, PLONG16 );
PCHAR szDev16QueryCaps = "Dev16QueryCaps";

BOOL16 APIENTRY16 Trc_Dev16QueryCaps( HDC     Parm1
                                    , LONG    Parm2
                                    , LONG    Parm3
                                    , PLONG16 Parm4
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szDev16QueryCaps );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDev16QueryCaps, &Parm1 );
      T_FreeLog( );
   }

   bRC = DEVQUERYCAPS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev16QueryCaps );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID16( Parm4 );
         if ( bRC ) T_LogULONGBuffer16( (PULONG16)Parm4, Parm3 );
      }
      HOOKEXIT16( szDev16QueryCaps, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Dev16QueryCaps() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32QueryCaps()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DEV32QUERYCAPS( HDC, LONG, LONG, PLONG );
PCHAR szDev32QueryCaps = "Dev32QueryCaps";

BOOL APIENTRY Trc_Dev32QueryCaps( HDC   Parm1
                                , LONG  Parm2
                                , LONG  Parm3
                                , PLONG Parm4
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32QueryCaps );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDev32QueryCaps, &Parm1 );
      T_FreeLog( );
   }

   bRC = DEV32QUERYCAPS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32QueryCaps );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID( Parm4 );
         if ( bRC ) T_LogULONGBuffer( (PULONG)Parm4, Parm3 );
      }
      HOOKEXIT( szDev32QueryCaps, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Dev32QueryCaps() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev16QueryDeviceNames()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DEVQUERYDEVICENAMES( HAB, PSZ16, PLONG16, PSTR3216, PSTR6416, PLONG16, PSTR1616 );
PCHAR szDev16QueryDeviceNames = "Dev16QueryDeviceNames";

BOOL16 APIENTRY16 Trc_Dev16QueryDeviceNames( HAB      Parm1
                                           , PSZ16    Parm2
                                           , PLONG16  Parm3
                                           , PSTR3216 Parm4
                                           , PSTR6416 Parm5
                                           , PLONG16  Parm6
                                           , PSTR1616 Parm7
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szDev16QueryDeviceNames );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
         T_LogParameter( 4L, DT_PSTR32 );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_PSTR64 );
         T_LogPVOID16( Parm5 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm6 );
         T_LogParameter( 7L, DT_PSTR16 );
         T_LogPVOID16( Parm7 );
      }
      HOOKENTRY16( szDev16QueryDeviceNames, &Parm1 );
      T_FreeLog( );
   }

   bRC = DEVQUERYDEVICENAMES( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev16QueryDeviceNames );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
         T_LogParameter( 4L, DT_PSTR32 );
         T_LogPVOID16( Parm4 );
         if ( ( Parm3 ) && ( bRC ) ) T_LogCHARBuffer16( (PCHAR16)Parm4, *Parm3 * sizeof( STR32 ) );
         T_LogParameter( 5L, DT_PSTR64 );
         T_LogPVOID16( Parm5 );
         if ( ( Parm3 ) && ( bRC ) ) T_LogCHARBuffer16( (PCHAR16)Parm4, *Parm3 * sizeof( STR64 ) );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm6 );
         T_LogParameter( 7L, DT_PSTR16 );
         T_LogPVOID16( Parm7 );
         if ( ( Parm6 ) && ( bRC ) ) T_LogCHARBuffer16( (PCHAR16)Parm7, *Parm6 * sizeof( STR16 ) );
      }
      HOOKEXIT16( szDev16QueryDeviceNames, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Dev16QueryDeviceNames() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32QueryDeviceNames()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DEV32QUERYDEVICENAMES( HAB, PSZ, PLONG, PSTR32, PSTR64, PLONG, PSTR16 );
PCHAR szDev32QueryDeviceNames = "Dev32QueryDeviceNames";

BOOL APIENTRY Trc_Dev32QueryDeviceNames( HAB    Parm1
                                       , PSZ    Parm2
                                       , PLONG  Parm3
                                       , PSTR32 Parm4
                                       , PSTR64 Parm5
                                       , PLONG  Parm6
                                       , PSTR16 Parm7
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32QueryDeviceNames );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_PSTR32 );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PSTR64 );
         T_LogPVOID( Parm5 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm6 );
         T_LogParameter( 7L, DT_PSTR16 );
         T_LogPVOID( Parm7 );
      }
      HOOKENTRY( szDev32QueryDeviceNames, &Parm1 );
      T_FreeLog( );
   }

   bRC = DEV32QUERYDEVICENAMES( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32QueryDeviceNames );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_PSTR32 );
         T_LogPVOID( Parm4 );
         if ( ( Parm3 ) && ( bRC ) ) T_LogCHARBuffer( (PCHAR)Parm4, *Parm3 * sizeof( STR32 ) );
         T_LogParameter( 5L, DT_PSTR64 );
         T_LogPVOID( Parm5 );
         if ( ( Parm3 ) && ( bRC ) ) T_LogCHARBuffer( (PCHAR)Parm4, *Parm3 * sizeof( STR64 ) );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm6 );
         T_LogParameter( 7L, DT_PSTR16 );
         T_LogPVOID( Parm7 );
         if ( ( Parm6 ) && ( bRC ) ) T_LogCHARBuffer( (PCHAR)Parm7, *Parm6 * sizeof( STR16 ) );
      }
      HOOKEXIT( szDev32QueryDeviceNames, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Dev32QueryDeviceNames() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev16QueryHardcopyCaps()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 DEVQUERYHARDCOPYCAPS( HDC, LONG, LONG, PHCINFO16 );
PCHAR szDev16QueryHardcopyCaps = "Dev16QueryHardcopyCaps";

LONG APIENTRY16 Trc_Dev16QueryHardcopyCaps( HDC       Parm1
                                          , LONG      Parm2
                                          , LONG      Parm3
                                          , PHCINFO16 Parm4
                                          )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szDev16QueryHardcopyCaps );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PHCINFO );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szDev16QueryHardcopyCaps, &Parm1 );
      T_FreeLog( );
   }

   lRC = DEVQUERYHARDCOPYCAPS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev16QueryHardcopyCaps );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != DQHC_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PHCINFO );
         if ( ( Parm3 ) && ( lRC != DQHC_ERROR ) ) T_LogPHCINFO16( Parm4, lRC );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szDev16QueryHardcopyCaps, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Dev16QueryHardcopyCaps() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32QueryHardcopyCaps()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY DEV32QUERYHARDCOPYCAPS( HDC, LONG, LONG, PHCINFO );
PCHAR szDev32QueryHardcopyCaps = "Dev32QueryHardcopyCaps";

LONG APIENTRY Trc_Dev32QueryHardcopyCaps( HDC     Parm1
                                        , LONG    Parm2
                                        , LONG    Parm3
                                        , PHCINFO Parm4
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32QueryHardcopyCaps );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PHCINFO );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szDev32QueryHardcopyCaps, &Parm1 );
      T_FreeLog( );
   }

   lRC = DEV32QUERYHARDCOPYCAPS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32QueryHardcopyCaps );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != DQHC_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PHCINFO );
         if ( ( Parm3 ) && ( lRC != DQHC_ERROR ) ) T_LogPHCINFO( Parm4, lRC );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szDev32QueryHardcopyCaps, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Dev32QueryHardcopyCaps() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev16StdOpen()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 210 ) )
BOOL16 APIENTRY16 DEVSTDOPEN( HDC );
PCHAR szDev16StdOpen = "Dev16StdOpen";

BOOL16 APIENTRY16 Trc_Dev16StdOpen( HDC Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szDev16StdOpen );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szDev16StdOpen, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnDEVSTDOPEN )( Parm1 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev16StdOpen );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szDev16StdOpen, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Dev16StdOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dev32StdOpen()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
BOOL APIENTRY DEV32STDOPEN( HDC );
PCHAR szDev32StdOpen = "Dev32StdOpen";

BOOL APIENTRY Trc_Dev32StdOpen( HDC Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szDev32StdOpen );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szDev32StdOpen, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnDEV32STDOPEN )( Parm1 );

   if ( LOGGPIAPI( G_DEV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szDev32StdOpen );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szDev32StdOpen, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Dev32StdOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16AnimatePalette()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIANIMATEPALETTE( HPAL, ULONG, ULONG, ULONG, PULONG16 );
PCHAR szGpi16AnimatePalette = "Gpi16AnimatePalette";

LONG APIENTRY16 Trc_Gpi16AnimatePalette( HPAL     Parm1
                                       , ULONG    Parm2
                                       , ULONG    Parm3
                                       , ULONG    Parm4
                                       , PULONG16 Parm5
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16AnimatePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPVOID16( Parm5 );
         T_LogULONGBuffer16( Parm5, Parm4 );
      }
      HOOKENTRY16( szGpi16AnimatePalette, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIANIMATEPALETTE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16AnimatePalette );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PAL_ERROR );
      HOOKEXIT16( szGpi16AnimatePalette, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16AnimatePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32AnimatePalette()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32ANIMATEPALETTE( HPAL, ULONG, ULONG, ULONG, PULONG );
PCHAR szGpi32AnimatePalette = "Gpi32AnimatePalette";

LONG APIENTRY Trc_Gpi32AnimatePalette( HPAL   Parm1
                                     , ULONG  Parm2
                                     , ULONG  Parm3
                                     , ULONG  Parm4
                                     , PULONG Parm5
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32AnimatePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPVOID( Parm5 );
         T_LogULONGBuffer( Parm5, Parm4 );
      }
      HOOKENTRY( szGpi32AnimatePalette, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32ANIMATEPALETTE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32AnimatePalette );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PAL_ERROR );
      HOOKEXIT( szGpi32AnimatePalette, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32AnimatePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Associate()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIASSOCIATE( HPS, HDC );
PCHAR szGpi16Associate = "Gpi16Associate";

BOOL16 APIENTRY16 Trc_Gpi16Associate( HPS Parm1
                                    , HDC Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Associate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HDC );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16Associate, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIASSOCIATE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Associate );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16Associate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Associate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Associate()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32ASSOCIATE( HPS, HDC );
PCHAR szGpi32Associate = "Gpi32Associate";

BOOL APIENTRY Trc_Gpi32Associate( HPS Parm1
                                , HDC Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Associate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HDC );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32Associate, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32ASSOCIATE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Associate );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32Associate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Associate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16BeginArea()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIBEGINAREA( HPS, ULONG );
PCHAR szGpi16BeginArea = "Gpi16BeginArea";

BOOL16 APIENTRY16 Trc_Gpi16BeginArea( HPS   Parm1
                                    , ULONG Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16BeginArea );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16BeginArea, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIBEGINAREA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16BeginArea );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16BeginArea, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16BeginArea() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32BeginArea()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32BEGINAREA( HPS, ULONG );
PCHAR szGpi32BeginArea = "Gpi32BeginArea";

BOOL APIENTRY Trc_Gpi32BeginArea( HPS   Parm1
                                , ULONG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32BeginArea );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32BeginArea, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32BEGINAREA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32BeginArea );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32BeginArea, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32BeginArea() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16BeginElement()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIBEGINELEMENT( HPS, LONG, PSZ16 );
PCHAR szGpi16BeginElement = "Gpi16BeginElement";

BOOL16 APIENTRY16 Trc_Gpi16BeginElement( HPS   Parm1
                                       , LONG  Parm2
                                       , PSZ16 Parm3
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16BeginElement );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
      }
      HOOKENTRY16( szGpi16BeginElement, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIBEGINELEMENT( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16BeginElement );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16BeginElement, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16BeginElement() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32BeginElement()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32BEGINELEMENT( HPS, LONG, PSZ );
PCHAR szGpi32BeginElement = "Gpi32BeginElement";

BOOL APIENTRY Trc_Gpi32BeginElement( HPS  Parm1
                                   , LONG Parm2
                                   , PSZ  Parm3
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32BeginElement );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
      }
      HOOKENTRY( szGpi32BeginElement, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32BEGINELEMENT( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32BeginElement );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32BeginElement, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32BeginElement() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32BeginInkPath()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
BOOL APIENTRY GPIBEGININKPATH( HPS, LONG, ULONG );
PCHAR szGpi32BeginInkPath = "Gpi32BeginInkPath";

BOOL APIENTRY Trc_Gpi32BeginInkPath( HPS   Parm1
                                   , LONG  Parm2
                                   , ULONG Parm3
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_INK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32BeginInkPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32BeginInkPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnGPI32BEGININKPATH )( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_INK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32BeginInkPath );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32BeginInkPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32BeginInkPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16BeginPath()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIBEGINPATH( HPS, LONG );
PCHAR szGpi16BeginPath = "Gpi16BeginPath";

BOOL16 APIENTRY16 Trc_Gpi16BeginPath( HPS  Parm1
                                    , LONG Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16BeginPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16BeginPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIBEGINPATH( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16BeginPath );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16BeginPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16BeginPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32BeginPath()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32BEGINPATH( HPS, LONG );
PCHAR szGpi32BeginPath = "Gpi32BeginPath";

BOOL APIENTRY Trc_Gpi32BeginPath( HPS  Parm1
                                , LONG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32BeginPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32BeginPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32BEGINPATH( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32BeginPath );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32BeginPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32BeginPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16BitBlt()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIBITBLT( HPS, HPS, LONG, PPOINTL16, LONG, ULONG );
PCHAR szGpi16BitBlt = "Gpi16BitBlt";

LONG APIENTRY16 Trc_Gpi16BitBlt( HPS       Parm1
                               , HPS       Parm2
                               , LONG      Parm3
                               , PPOINTL16 Parm4
                               , LONG      Parm5
                               , ULONG     Parm6
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16BitBlt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm4, Parm3 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY16( szGpi16BitBlt, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIBITBLT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16BitBlt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16BitBlt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16BitBlt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32BitBlt()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32BITBLT( HPS, HPS, LONG, PPOINTL, LONG, ULONG );
PCHAR szGpi32BitBlt = "Gpi32BitBlt";

LONG APIENTRY Trc_Gpi32BitBlt( HPS     Parm1
                             , HPS     Parm2
                             , LONG    Parm3
                             , PPOINTL Parm4
                             , LONG    Parm5
                             , ULONG   Parm6
                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32BitBlt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm4, Parm3 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szGpi32BitBlt, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32BITBLT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32BitBlt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32BitBlt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32BitBlt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Box()                                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIBOX( HPS, LONG, PPOINTL16, LONG, LONG );
PCHAR szGpi16Box = "Gpi16Box";

LONG APIENTRY16 Trc_Gpi16Box( HPS       Parm1
                            , LONG      Parm2
                            , PPOINTL16 Parm3
                            , LONG      Parm4
                            , LONG      Parm5
                            )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Box );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szGpi16Box, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIBOX( Parm1,  Parm2,  Parm3,  Parm4,  Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Box );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16Box, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16Box() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Box()                                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32BOX( HPS, LONG, PPOINTL, LONG, LONG );
PCHAR szGpi32Box = "Gpi32Box";

LONG APIENTRY Trc_Gpi32Box( HPS     Parm1
                          , LONG    Parm2
                          , PPOINTL Parm3
                          , LONG    Parm4
                          , LONG    Parm5
                          )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Box );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32Box, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32BOX( Parm1,  Parm2,  Parm3,  Parm4,  Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Box );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32Box, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32Box() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CallSegmentMatrix()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICALLSEGMENTMATRIX( HPS, LONG, LONG, PMATRIXLF16, LONG );
PCHAR szGpi16CallSegmentMatrix = "Gpi16CallSegmentMatrix";

LONG APIENTRY16 Trc_Gpi16CallSegmentMatrix( HPS         Parm1
                                          , LONG        Parm2
                                          , LONG        Parm3
                                          , PMATRIXLF16 Parm4
                                          , LONG        Parm5
                                          )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CallSegmentMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szGpi16CallSegmentMatrix, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICALLSEGMENTMATRIX( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CallSegmentMatrix );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CallSegmentMatrix, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CallSegmentMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CallSegmentMatrix()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CALLSEGMENTMATRIX( HPS, LONG, LONG, PMATRIXLF, LONG );
PCHAR szGpi32CallSegmentMatrix = "Gpi32CallSegmentMatrix";

LONG APIENTRY Trc_Gpi32CallSegmentMatrix( HPS       Parm1
                                        , LONG      Parm2
                                        , LONG      Parm3
                                        , PMATRIXLF Parm4
                                        , LONG      Parm5
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CallSegmentMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32CallSegmentMatrix, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CALLSEGMENTMATRIX( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CallSegmentMatrix );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32CallSegmentMatrix, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CallSegmentMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CharString()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICHARSTRING( HPS, LONG, PCH16 );
PCHAR szGpi16CharString = "Gpi16CharString";

LONG APIENTRY16 Trc_Gpi16CharString( HPS   Parm1
                                   , LONG  Parm2
                                   , PCH16 Parm3
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CharString );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID16( Parm3 );
         T_LogCHARBuffer16( Parm3, Parm2 );
      }
      HOOKENTRY16( szGpi16CharString, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICHARSTRING( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CharString );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CharString, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CharString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CharString()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CHARSTRING( HPS, LONG, PCH );
PCHAR szGpi32CharString = "Gpi32CharString";

LONG APIENTRY Trc_Gpi32CharString( HPS  Parm1
                                 , LONG Parm2
                                 , PCH  Parm3
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CharString );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID( Parm3 );
         T_LogCHARBuffer( Parm3, Parm2 );
      }
      HOOKENTRY( szGpi32CharString, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CHARSTRING( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CharString );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32CharString, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CharString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CharStringAt()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICHARSTRINGAT( HPS, PPOINTL16, LONG, PCH16 );
PCHAR szGpi16CharStringAt = "Gpi16CharStringAt";

LONG APIENTRY16 Trc_Gpi16CharStringAt( HPS       Parm1
                                     , PPOINTL16 Parm2
                                     , LONG      Parm3
                                     , PCH16     Parm4
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CharStringAt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PCH );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( Parm4, Parm3 );
      }
      HOOKENTRY16( szGpi16CharStringAt, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICHARSTRINGAT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CharStringAt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CharStringAt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CharStringAt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CharStringAt()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CHARSTRINGAT( HPS, PPOINTL, LONG, PCH );
PCHAR szGpi32CharStringAt = "Gpi32CharStringAt";

LONG APIENTRY Trc_Gpi32CharStringAt( HPS     Parm1
                                   , PPOINTL Parm2
                                   , LONG    Parm3
                                   , PCH     Parm4
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CharStringAt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PCH );
         T_LogPVOID( Parm4 );
         T_LogCHARBuffer( Parm4, Parm3 );
      }
      HOOKENTRY( szGpi32CharStringAt, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CHARSTRINGAT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CharStringAt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32CharStringAt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CharStringAt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CharStringPos()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICHARSTRINGPOS( HPS, PRECTL16, ULONG, LONG, PCH16, PLONG16 );
PCHAR szGpi16CharStringPos = "Gpi16CharStringPos";

LONG APIENTRY16 Trc_Gpi16CharStringPos( HPS      Parm1
                                      , PRECTL16 Parm2
                                      , ULONG    Parm3
                                      , LONG     Parm4
                                      , PCH16    Parm5
                                      , PLONG16  Parm6
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CharStringPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         if ( FLAGSET( Parm3, CHS_OPAQUE | CHS_CLIP ) ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PCH );
         T_LogPVOID16( Parm5 );
         T_LogCHARBuffer16( Parm5, Parm4 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID16( Parm6 );
         if ( FLAGSET( Parm3, CHS_VECTOR ) ) T_LogULONGBuffer16( (PULONG16)Parm6, Parm4 );
      }
      HOOKENTRY16( szGpi16CharStringPos, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICHARSTRINGPOS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CharStringPos );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CharStringPos, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CharStringPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CharStringPos()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CHARSTRINGPOS( HPS, PRECTL, ULONG, LONG, PCH, PLONG );
PCHAR szGpi32CharStringPos = "Gpi32CharStringPos";

LONG APIENTRY Trc_Gpi32CharStringPos( HPS    Parm1
                                    , PRECTL Parm2
                                    , ULONG  Parm3
                                    , LONG   Parm4
                                    , PCH    Parm5
                                    , PLONG  Parm6
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CharStringPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         if ( FLAGSET( Parm3, CHS_OPAQUE | CHS_CLIP ) ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PCH );
         T_LogPVOID( Parm5 );
         T_LogCHARBuffer( Parm5, Parm4 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID( Parm6 );
         if ( FLAGSET( Parm3, CHS_VECTOR ) ) T_LogULONGBuffer( (PULONG)Parm6, Parm4 );
      }
      HOOKENTRY( szGpi32CharStringPos, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CHARSTRINGPOS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CharStringPos );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32CharStringPos, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CharStringPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CharStringPosAt()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICHARSTRINGPOSAT( HPS, PPOINTL16, PRECTL16, ULONG, LONG, PCH16, PLONG16 );
PCHAR szGpi16CharStringPosAt = "Gpi16CharStringPosAt";

LONG APIENTRY16 Trc_Gpi16CharStringPosAt( HPS       Parm1
                                        , PPOINTL16 Parm2
                                        , PRECTL16  Parm3
                                        , ULONG     Parm4
                                        , LONG      Parm5
                                        , PCH16     Parm6
                                        , PLONG16   Parm7
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CharStringPosAt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         if ( FLAGSET( Parm4, CHS_OPAQUE | CHS_CLIP ) ) T_LogPRECTL16( Parm3 );
         else T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PCH );
         T_LogPVOID16( Parm6 );
         T_LogCHARBuffer16( Parm6, Parm5 );
         T_LogParameter( 7L, DT_PLONG );
         T_LogPVOID16( Parm7 );
         if ( FLAGSET( Parm4, CHS_VECTOR ) ) T_LogULONGBuffer16( (PULONG)Parm7, Parm5 );
      }
      HOOKENTRY16( szGpi16CharStringPosAt, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICHARSTRINGPOSAT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CharStringPosAt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CharStringPosAt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CharStringPosAt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CharStringPosAt()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CHARSTRINGPOSAT( HPS, PPOINTL, PRECTL, ULONG, LONG, PCH, PLONG );
PCHAR szGpi32CharStringPosAt = "Gpi32CharStringPosAt";

LONG APIENTRY Trc_Gpi32CharStringPosAt( HPS     Parm1
                                      , PPOINTL Parm2
                                      , PRECTL  Parm3
                                      , ULONG   Parm4
                                      , LONG    Parm5
                                      , PCH     Parm6
                                      , PLONG   Parm7
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CharStringPosAt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         if ( FLAGSET( Parm4, CHS_OPAQUE | CHS_CLIP ) ) T_LogPRECTL( Parm3 );
         else T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PCH );
         T_LogPVOID( Parm6 );
         T_LogCHARBuffer( Parm6, Parm5 );
         T_LogParameter( 7L, DT_PLONG );
         T_LogPVOID( Parm7 );
         if ( FLAGSET( Parm4, CHS_VECTOR ) ) T_LogULONGBuffer( (PULONG)Parm7, Parm5 );
      }
      HOOKENTRY( szGpi32CharStringPosAt, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CHARSTRINGPOSAT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CharStringPosAt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32CharStringPosAt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CharStringPosAt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CloseFigure()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPICLOSEFIGURE( HPS );
PCHAR szGpi16CloseFigure = "Gpi16CloseFigure";

BOOL16 APIENTRY16 Trc_Gpi16CloseFigure( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CloseFigure );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16CloseFigure, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPICLOSEFIGURE( Parm1 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CloseFigure );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16CloseFigure, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16CloseFigure() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CloseFigure()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32CLOSEFIGURE( HPS );
PCHAR szGpi32CloseFigure = "Gpi32CloseFigure";

BOOL APIENTRY Trc_Gpi32CloseFigure( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CloseFigure );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32CloseFigure, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32CLOSEFIGURE( Parm1 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CloseFigure );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32CloseFigure, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32CloseFigure() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CloseSegment()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPICLOSESEGMENT( HPS );
PCHAR szGpi16CloseSegment = "Gpi16CloseSegment";

BOOL16 APIENTRY16 Trc_Gpi16CloseSegment( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CloseSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16CloseSegment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPICLOSESEGMENT( Parm1 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CloseSegment );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16CloseSegment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16CloseSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CloseSegment()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32CLOSESEGMENT( HPS );
PCHAR szGpi32CloseSegment = "Gpi32CloseSegment";

BOOL APIENTRY Trc_Gpi32CloseSegment( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CloseSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32CloseSegment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32CLOSESEGMENT( Parm1 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CloseSegment );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32CloseSegment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32CloseSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CombineRegion()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICOMBINEREGION( HPS, HRGN, HRGN, HRGN, LONG );
PCHAR szGpi16CombineRegion = "Gpi16CombineRegion";

LONG APIENTRY16 Trc_Gpi16CombineRegion( HPS  Parm1
                                      , HRGN Parm2
                                      , HRGN Parm3
                                      , HRGN Parm4
                                      , LONG Parm5
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CombineRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HRGN );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_HRGN );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szGpi16CombineRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICOMBINEREGION( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CombineRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT16( szGpi16CombineRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CombineRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CombineRegion()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32COMBINEREGION( HPS, HRGN, HRGN, HRGN, LONG );
PCHAR szGpi32CombineRegion = "Gpi32CombineRegion";

LONG APIENTRY Trc_Gpi32CombineRegion( HPS  Parm1
                                    , HRGN Parm2
                                    , HRGN Parm3
                                    , HRGN Parm4
                                    , LONG Parm5
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CombineRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HRGN );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_HRGN );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32CombineRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32COMBINEREGION( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CombineRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT( szGpi32CombineRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CombineRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Comment()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPICOMMENT( HPS, LONG, PBYTE16 );
PCHAR szGpi16Comment = "Gpi16Comment";

BOOL16 APIENTRY16 Trc_Gpi16Comment( HPS     Parm1
                                  , LONG    Parm2
                                  , PBYTE16 Parm3
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Comment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PBYTE );
         T_LogPVOID16( Parm3 );
         T_LogCHARBuffer16( (PCHAR)Parm3, Parm2 );
      }
      HOOKENTRY16( szGpi16Comment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPICOMMENT( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Comment );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16Comment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Comment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Comment()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32COMMENT( HPS, LONG, PBYTE );
PCHAR szGpi32Comment = "Gpi32Comment";

BOOL APIENTRY Trc_Gpi32Comment( HPS   Parm1
                              , LONG  Parm2
                              , PBYTE Parm3
                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Comment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PBYTE );
         T_LogPVOID( Parm3 );
         T_LogCHARBuffer( (PCHAR)Parm3, Parm2 );
      }
      HOOKENTRY( szGpi32Comment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32COMMENT( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Comment );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32Comment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Comment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Convert()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPICONVERT( HPS, LONG, LONG, LONG, PPOINTL16 );
PCHAR szGpi16Convert = "Gpi16Convert";

BOOL16 APIENTRY16 Trc_Gpi16Convert( HPS       Parm1
                                  , LONG      Parm2
                                  , LONG      Parm3
                                  , LONG      Parm4
                                  , PPOINTL16 Parm5
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Convert );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm5, Parm4 );
      }
      HOOKENTRY16( szGpi16Convert, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPICONVERT( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Convert );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer16( Parm5, Parm4 );
         else T_LogPVOID16( Parm5 );
      }
      HOOKEXIT16( szGpi16Convert, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Convert() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Convert()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32CONVERT( HPS, LONG, LONG, LONG, PPOINTL );
PCHAR szGpi32Convert = "Gpi32Convert";

BOOL APIENTRY Trc_Gpi32Convert( HPS     Parm1
                              , LONG    Parm2
                              , LONG    Parm3
                              , LONG    Parm4
                              , PPOINTL Parm5
                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Convert );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm5, Parm4 );
      }
      HOOKENTRY( szGpi32Convert, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32CONVERT( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Convert );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer( Parm5, Parm4 );
         else T_LogPVOID( Parm5 );
      }
      HOOKEXIT( szGpi32Convert, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Convert() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16ConvertWithMatrix()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPICONVERTWITHMATRIX( HPS, LONG, PPOINTL16, LONG, PMATRIXLF16 );
PCHAR szGpi16ConvertWithMatrix = "Gpi16ConvertWithMatrix";

BOOL16 APIENTRY16 Trc_Gpi16ConvertWithMatrix( HPS         Parm1
                                            , LONG        Parm2
                                            , PPOINTL16   Parm3
                                            , LONG        Parm4
                                            , PMATRIXLF16 Parm5
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16ConvertWithMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm3, Parm2 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm5 );
      }
      HOOKENTRY16( szGpi16ConvertWithMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPICONVERTWITHMATRIX( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16ConvertWithMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer16( Parm3, Parm2 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szGpi16ConvertWithMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16ConvertWithMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32ConvertWithMatrix()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32CONVERTWITHMATRIX( HPS, LONG, PPOINTL, LONG, PMATRIXLF );
PCHAR szGpi32ConvertWithMatrix = "Gpi32ConvertWithMatrix";

BOOL APIENTRY Trc_Gpi32ConvertWithMatrix( HPS       Parm1
                                        , LONG      Parm2
                                        , PPOINTL   Parm3
                                        , LONG      Parm4
                                        , PMATRIXLF Parm5
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32ConvertWithMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm3, Parm2 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm5 );
      }
      HOOKENTRY( szGpi32ConvertWithMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32CONVERTWITHMATRIX( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32ConvertWithMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer( Parm3, Parm2 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szGpi32ConvertWithMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32ConvertWithMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CopyMetaFile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HMF APIENTRY16 GPICOPYMETAFILE( HMF );
PCHAR szGpi16CopyMetaFile = "Gpi16CopyMetaFile";

HMF APIENTRY16 Trc_Gpi16CopyMetaFile( HMF Parm1 )
{

   HMF ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CopyMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16CopyMetaFile, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPICOPYMETAFILE( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CopyMetaFile );
      T_LogPMReturnCode( DT_HMF, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CopyMetaFile, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16CopyMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CopyMetaFile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HMF APIENTRY GPI32COPYMETAFILE( HMF );
PCHAR szGpi32CopyMetaFile = "Gpi32CopyMetaFile";

HMF APIENTRY Trc_Gpi32CopyMetaFile( HMF Parm1 )
{

   HMF ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CopyMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32CopyMetaFile, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32COPYMETAFILE( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CopyMetaFile );
      T_LogPMReturnCode( DT_HMF, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT( szGpi32CopyMetaFile, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32CopyMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CorrelateChain()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICORRELATECHAIN( HPS, LONG, PPOINTL16, LONG, LONG, PLONG16 );
PCHAR szGpi16CorrelateChain = "Gpi16CorrelateChain";

LONG APIENTRY16 Trc_Gpi16CorrelateChain( HPS       Parm1
                                       , LONG      Parm2
                                       , PPOINTL16 Parm3
                                       , LONG      Parm4
                                       , LONG      Parm5
                                       , PLONG16   Parm6
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CorrelateChain );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szGpi16CorrelateChain, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICORRELATECHAIN( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CorrelateChain );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID16( Parm6 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer16( (PULONG16)Parm6, lRC * Parm5 * 2 );
      }
      HOOKEXIT16( szGpi16CorrelateChain, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CorrelateChain() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CorrelateChain()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CORRELATECHAIN( HPS, LONG, PPOINTL, LONG, LONG, PLONG );
PCHAR szGpi32CorrelateChain = "Gpi32CorrelateChain";

LONG APIENTRY Trc_Gpi32CorrelateChain( HPS     Parm1
                                     , LONG    Parm2
                                     , PPOINTL Parm3
                                     , LONG    Parm4
                                     , LONG    Parm5
                                     , PLONG   Parm6
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CorrelateChain );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szGpi32CorrelateChain, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CORRELATECHAIN( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CorrelateChain );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID( Parm6 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer( (PULONG)Parm6, lRC * Parm5 * 2 );
      }
      HOOKEXIT( szGpi32CorrelateChain, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CorrelateChain() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CorrelateFrom()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICORRELATEFROM( HPS, LONG, LONG, LONG, PPOINTL16, LONG, LONG, PLONG16 );
PCHAR szGpi16CorrelateFrom = "Gpi16CorrelateFrom";

LONG APIENTRY16 Trc_Gpi16CorrelateFrom( HPS       Parm1
                                      , LONG      Parm2
                                      , LONG      Parm3
                                      , LONG      Parm4
                                      , PPOINTL16 Parm5
                                      , LONG      Parm6
                                      , LONG      Parm7
                                      , PLONG16   Parm8
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CorrelateFrom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPPOINTL16( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_LONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PLONG );
         T_LogPVOID16( Parm8 );
      }
      HOOKENTRY16( szGpi16CorrelateFrom, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICORRELATEFROM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CorrelateFrom );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 8L, DT_PLONG );
         T_LogPVOID16( Parm8 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer16( (PULONG16)Parm8, lRC * Parm7 * 2 );
      }
      HOOKEXIT16( szGpi16CorrelateFrom, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CorrelateFrom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CorrelateFrom()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CORRELATEFROM( HPS, LONG, LONG, LONG, PPOINTL, LONG, LONG, PLONG );
PCHAR szGpi32CorrelateFrom = "Gpi32CorrelateFrom";

LONG APIENTRY Trc_Gpi32CorrelateFrom( HPS     Parm1
                                    , LONG    Parm2
                                    , LONG    Parm3
                                    , LONG    Parm4
                                    , PPOINTL Parm5
                                    , LONG    Parm6
                                    , LONG    Parm7
                                    , PLONG   Parm8
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CorrelateFrom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPPOINTL( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_LONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PLONG );
         T_LogPVOID( Parm8 );
      }
      HOOKENTRY( szGpi32CorrelateFrom, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CORRELATEFROM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CorrelateFrom );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 8L, DT_PLONG );
         T_LogPVOID( Parm8 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer( (PULONG)Parm8, lRC * Parm7 * 2 );
      }
      HOOKEXIT( szGpi32CorrelateFrom, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CorrelateFrom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CorrelateSegment()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICORRELATESEGMENT( HPS, LONG, LONG, PPOINTL16, LONG, LONG, PLONG16 );
PCHAR szGpi16CorrelateSegment = "Gpi16CorrelateSegment";

LONG APIENTRY16 Trc_Gpi16CorrelateSegment( HPS       Parm1
                                         , LONG      Parm2
                                         , LONG      Parm3
                                         , PPOINTL16 Parm4
                                         , LONG      Parm5
                                         , LONG      Parm6
                                         , PLONG16   Parm7
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CorrelateSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPPOINTL16( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PLONG );
         T_LogPVOID16( Parm7 );
      }
      HOOKENTRY16( szGpi16CorrelateSegment, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICORRELATESEGMENT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CorrelateSegment );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PLONG );
         T_LogPVOID16( Parm7 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer16( (PULONG16)Parm7, lRC * Parm6 * 2 );
      }
      HOOKEXIT16( szGpi16CorrelateSegment, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CorrelateSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CorrelateSegment()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CORRELATESEGMENT( HPS, LONG, LONG, PPOINTL, LONG, LONG, PLONG );
PCHAR szGpi32CorrelateSegment = "Gpi32CorrelateSegment";

LONG APIENTRY Trc_Gpi32CorrelateSegment( HPS     Parm1
                                       , LONG    Parm2
                                       , LONG    Parm3
                                       , PPOINTL Parm4
                                       , LONG    Parm5
                                       , LONG    Parm6
                                       , PLONG   Parm7
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CorrelateSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPPOINTL( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PLONG );
         T_LogPVOID( Parm7 );
      }
      HOOKENTRY( szGpi32CorrelateSegment, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CORRELATESEGMENT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CorrelateSegment );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PLONG );
         T_LogPVOID( Parm7 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer( (PULONG)Parm7, lRC * Parm6 * 2 );
      }
      HOOKEXIT( szGpi32CorrelateSegment, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CorrelateSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CreateBitmap()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HBITMAP APIENTRY16 GPICREATEBITMAP( HPS, PBITMAPINFOHEADER216, ULONG, PBYTE16, PBITMAPINFO216 );
PCHAR szGpi16CreateBitmap = "Gpi16CreateBitmap";

HBITMAP APIENTRY16 Trc_Gpi16CreateBitmap( HPS                  Parm1
                                        , PBITMAPINFOHEADER216 Parm2
                                        , ULONG                Parm3
                                        , PBYTE16              Parm4
                                        , PBITMAPINFO216       Parm5
                                        )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CreateBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER2 );
         T_LogPBITMAPINFOHEADER216( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         if ( FLAGSET( Parm3, CBM_INIT ) ) T_LogG16CB_Parm4( Parm4, Parm5 );
         T_LogParameter( 5L, DT_PBITMAPINFO2 );
         if ( FLAGSET( Parm3, CBM_INIT ) ) T_LogPBITMAPINFO216( Parm5 );
         else T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szGpi16CreateBitmap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPICREATEBITMAP( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CreateBitmap );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CreateBitmap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16CreateBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreateBitmap()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HBITMAP APIENTRY GPI32CREATEBITMAP( HPS, PBITMAPINFOHEADER2, ULONG, PBYTE, PBITMAPINFO2 );
PCHAR szGpi32CreateBitmap = "Gpi32CreateBitmap";

HBITMAP APIENTRY Trc_Gpi32CreateBitmap( HPS                Parm1
                                      , PBITMAPINFOHEADER2 Parm2
                                      , ULONG              Parm3
                                      , PBYTE              Parm4
                                      , PBITMAPINFO2       Parm5
                                      )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreateBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER2 );
         T_LogPBITMAPINFOHEADER2( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         if ( FLAGSET( Parm3, CBM_INIT ) ) T_LogG32CB_Parm4( Parm4, Parm5 );
         T_LogParameter( 5L, DT_PBITMAPINFO2 );
         if ( FLAGSET( Parm3, CBM_INIT ) ) T_LogPBITMAPINFO2( Parm5 );
         else T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szGpi32CreateBitmap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32CREATEBITMAP( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreateBitmap );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT( szGpi32CreateBitmap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32CreateBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreateEllipticRegion()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
HRGN APIENTRY GPI32CREATEELLIPTICREGION( HPS, PRECTL );
PCHAR szGpi32CreateEllipticRegion = "Gpi32CreateEllipticRegion";

HRGN APIENTRY Trc_Gpi32CreateEllipticRegion( HPS    Parm1
                                           , PRECTL Parm2
                                           )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreateEllipticRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szGpi32CreateEllipticRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnGPI32CREATEELLIPTICREGION )( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreateEllipticRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != RGN_ERROR );
      HOOKEXIT( szGpi32CreateEllipticRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32CreateEllipticRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CreateLogColorTable()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPICREATELOGCOLORTABLE( HPS, ULONG, LONG, LONG, LONG, PLONG16 );
PCHAR szGpi16CreateLogColorTable = "Gpi16CreateLogColorTable";

BOOL16 APIENTRY16 Trc_Gpi16CreateLogColorTable( HPS     Parm1
                                              , ULONG   Parm2
                                              , LONG    Parm3
                                              , LONG    Parm4
                                              , LONG    Parm5
                                              , PLONG16 Parm6
                                              )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CreateLogColorTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID16( Parm6 );
         T_LogULONGBuffer16( (PULONG16)Parm6, Parm5 );
      }
      HOOKENTRY16( szGpi16CreateLogColorTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPICREATELOGCOLORTABLE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CreateLogColorTable );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16CreateLogColorTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16CreateLogColorTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreateLogColorTable()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32CREATELOGCOLORTABLE( HPS, ULONG, LONG, LONG, LONG, PLONG );
PCHAR szGpi32CreateLogColorTable = "Gpi32CreateLogColorTable";

BOOL APIENTRY Trc_Gpi32CreateLogColorTable( HPS   Parm1
                                          , ULONG Parm2
                                          , LONG  Parm3
                                          , LONG  Parm4
                                          , LONG  Parm5
                                          , PLONG Parm6
                                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreateLogColorTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID( Parm6 );
         T_LogULONGBuffer( (PULONG)Parm6, Parm5 );
      }
      HOOKENTRY( szGpi32CreateLogColorTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32CREATELOGCOLORTABLE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreateLogColorTable );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32CreateLogColorTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32CreateLogColorTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CreateLogFont()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPICREATELOGFONT( HPS, PSTR816, LONG, PFATTRS16 );
PCHAR szGpi16CreateLogFont = "Gpi16CreateLogFont";

LONG APIENTRY16 Trc_Gpi16CreateLogFont( HPS       Parm1
                                      , PSTR816   Parm2
                                      , LONG      Parm3
                                      , PFATTRS16 Parm4
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CreateLogFont );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSTR8 );
         T_LogPVOID16( Parm2 );
         T_LogCHARBuffer16( (PCHAR16)Parm2, sizeof( STR8 ) );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PFATTRS );
         T_LogPFATTRS16( Parm4 );
      }
      HOOKENTRY16( szGpi16CreateLogFont, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPICREATELOGFONT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CreateLogFont );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CreateLogFont, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16CreateLogFont() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreateLogFont()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32CREATELOGFONT( HPS, PSTR8, LONG, PFATTRS );
PCHAR szGpi32CreateLogFont = "Gpi32CreateLogFont";

LONG APIENTRY Trc_Gpi32CreateLogFont( HPS     Parm1
                                    , PSTR8   Parm2
                                    , LONG    Parm3
                                    , PFATTRS Parm4
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreateLogFont );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSTR8 );
         T_LogPVOID( Parm2 );
         T_LogCHARBuffer( (PCHAR)Parm2, sizeof( STR8 ) );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PFATTRS );
         T_LogPFATTRS( Parm4 );
      }
      HOOKENTRY( szGpi32CreateLogFont, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32CREATELOGFONT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreateLogFont );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32CreateLogFont, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32CreateLogFont() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CreatePalette()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPAL APIENTRY16 GPICREATEPALETTE( HAB, ULONG, ULONG, ULONG, PULONG16 );
PCHAR szGpi16CreatePalette = "Gpi16CreatePalette";

HPAL APIENTRY16 Trc_Gpi16CreatePalette( HAB      Parm1
                                      , ULONG    Parm2
                                      , ULONG    Parm3
                                      , ULONG    Parm4
                                      , PULONG16 Parm5
                                      )
{

   HPAL ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CreatePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPVOID16( Parm5 );
         T_LogULONGBuffer16( Parm5, Parm4 );
      }
      HOOKENTRY16( szGpi16CreatePalette, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPICREATEPALETTE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CreatePalette );
      T_LogPMReturnCode( DT_HPAL, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CreatePalette, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16CreatePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreatePalette()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPAL APIENTRY GPI32CREATEPALETTE( HAB, ULONG, ULONG, ULONG, PULONG );
PCHAR szGpi32CreatePalette = "Gpi32CreatePalette";

HPAL APIENTRY Trc_Gpi32CreatePalette( HAB    Parm1
                                    , ULONG  Parm2
                                    , ULONG  Parm3
                                    , ULONG  Parm4
                                    , PULONG Parm5
                                    )
{

   HPAL ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreatePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPVOID( Parm5 );
         T_LogULONGBuffer( Parm5, Parm4 );
      }
      HOOKENTRY( szGpi32CreatePalette, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32CREATEPALETTE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreatePalette );
      T_LogPMReturnCode( DT_HPAL, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT( szGpi32CreatePalette, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32CreatePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreatePolygonRegion()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
HRGN APIENTRY GPI32CREATEPOLYGONREGION( HPS, ULONG, PPOLYGON, ULONG );
PCHAR szGpi32CreatePolygonRegion = "Gpi32CreatePolygonRegion";

HRGN APIENTRY Trc_Gpi32CreatePolygonRegion( HPS      Parm1
                                          , ULONG    Parm2
                                          , PPOLYGON Parm3
                                          , ULONG    Parm4
                                          )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_POLY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreatePolygonRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOLYGON );
         T_LogPOLYGONBuffer( Parm3, Parm2 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szGpi32CreatePolygonRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnGPI32CREATEPOLYGONREGION )( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_POLY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreatePolygonRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != RGN_ERROR );
      HOOKEXIT( szGpi32CreatePolygonRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32CreatePolygonRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CreatePS()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPS APIENTRY16 GPICREATEPS( HAB, HDC, PSIZEL16, ULONG );
PCHAR szGpi16CreatePS = "Gpi16CreatePS";

HPS APIENTRY16 Trc_Gpi16CreatePS( HAB      Parm1
                                , HDC      Parm2
                                , PSIZEL16 Parm3
                                , ULONG    Parm4
                                )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CreatePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HDC );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSIZEL );
         T_LogPSIZEL16( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szGpi16CreatePS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPICREATEPS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CreatePS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT16( szGpi16CreatePS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16CreatePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreatePS()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPS APIENTRY GPI32CREATEPS( HAB, HDC, PSIZEL, ULONG );
PCHAR szGpi32CreatePS = "Gpi32CreatePS";

HPS APIENTRY Trc_Gpi32CreatePS( HAB    Parm1
                              , HDC    Parm2
                              , PSIZEL Parm3
                              , ULONG  Parm4
                              )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreatePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HDC );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSIZEL );
         T_LogPSIZEL( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szGpi32CreatePS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32CREATEPS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreatePS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT( szGpi32CreatePS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32CreatePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16CreateRegion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HRGN APIENTRY16 GPICREATEREGION( HPS, LONG, PRECTL16 );
PCHAR szGpi16CreateRegion = "Gpi16CreateRegion";

HRGN APIENTRY16 Trc_Gpi16CreateRegion( HPS      Parm1
                                     , LONG     Parm2
                                     , PRECTL16 Parm3
                                     )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16CreateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogRECTLBuffer16( Parm3, Parm2 );
      }
      HOOKENTRY16( szGpi16CreateRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPICREATEREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16CreateRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != RGN_ERROR );
      HOOKEXIT16( szGpi16CreateRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16CreateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreateRegion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HRGN APIENTRY GPI32CREATEREGION( HPS, LONG, PRECTL );
PCHAR szGpi32CreateRegion = "Gpi32CreateRegion";

HRGN APIENTRY Trc_Gpi32CreateRegion( HPS    Parm1
                                   , LONG   Parm2
                                   , PRECTL Parm3
                                   )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogRECTLBuffer( Parm3, Parm2 );
      }
      HOOKENTRY( szGpi32CreateRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32CREATEREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreateRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != RGN_ERROR );
      HOOKEXIT( szGpi32CreateRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32CreateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32CreateRoundRectRegion()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
HRGN APIENTRY GPI32CREATEROUNDRECTREGION( HPS, PPOINTL, LONG, LONG );
PCHAR szGpi32CreateRoundRectRegion = "Gpi32CreateRoundRectRegion";

HRGN APIENTRY Trc_Gpi32CreateRoundRectRegion( HPS     Parm1
                                            , PPOINTL Parm2
                                            , LONG    Parm3
                                            , LONG    Parm4
                                            )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32CreateRoundRectRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szGpi32CreateRoundRectRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnGPI32CREATEROUNDRECTREGION )( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32CreateRoundRectRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != RGN_ERROR );
      HOOKEXIT( szGpi32CreateRoundRectRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32CreateRoundRectRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeleteBitmap()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETEBITMAP( HBITMAP );
PCHAR szGpi16DeleteBitmap = "Gpi16DeleteBitmap";

BOOL16 APIENTRY16 Trc_Gpi16DeleteBitmap( HBITMAP Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeleteBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16DeleteBitmap, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETEBITMAP( Parm1 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeleteBitmap );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeleteBitmap, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeleteBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeleteBitmap()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETEBITMAP( HBITMAP );
PCHAR szGpi32DeleteBitmap = "Gpi32DeleteBitmap";

BOOL APIENTRY Trc_Gpi32DeleteBitmap( HBITMAP Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeleteBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32DeleteBitmap, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETEBITMAP( Parm1 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeleteBitmap );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeleteBitmap, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeleteBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeleteElement()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETEELEMENT( HPS );
PCHAR szGpi16DeleteElement = "Gpi16DeleteElement";

BOOL16 APIENTRY16 Trc_Gpi16DeleteElement( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeleteElement );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16DeleteElement, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETEELEMENT( Parm1 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeleteElement );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeleteElement, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeleteElement() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeleteElement()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETEELEMENT( HPS );
PCHAR szGpi32DeleteElement = "Gpi32DeleteElement";

BOOL APIENTRY Trc_Gpi32DeleteElement( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeleteElement );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32DeleteElement, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETEELEMENT( Parm1 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeleteElement );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeleteElement, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeleteElement() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeleteElementRange()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETEELEMENTRANGE( HPS, LONG, LONG );
PCHAR szGpi16DeleteElementRange = "Gpi16DeleteElementRange";

BOOL16 APIENTRY16 Trc_Gpi16DeleteElementRange( HPS  Parm1
                                             , LONG Parm2
                                             , LONG Parm3
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeleteElementRange );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16DeleteElementRange, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETEELEMENTRANGE( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeleteElementRange );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeleteElementRange, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeleteElementRange() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeleteElementRange()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETEELEMENTRANGE( HPS, LONG, LONG );
PCHAR szGpi32DeleteElementRange = "Gpi32DeleteElementRange";

BOOL APIENTRY Trc_Gpi32DeleteElementRange( HPS  Parm1
                                         , LONG Parm2
                                         , LONG Parm3
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeleteElementRange );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32DeleteElementRange, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETEELEMENTRANGE( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeleteElementRange );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeleteElementRange, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeleteElementRange() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeleteElementsBetweenLabels()                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETEELEMENTSBETWEENLABELS( HPS, LONG, LONG );
PCHAR szGpi16DeleteElementsBetweenLabels = "Gpi16DeleteElementsBetweenLabels";

BOOL16 APIENTRY16 Trc_Gpi16DeleteElementsBetweenLabels( HPS  Parm1
                                                      , LONG Parm2
                                                      , LONG Parm3
                                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeleteElementsBetweenLabels );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16DeleteElementsBetweenLabels, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETEELEMENTSBETWEENLABELS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeleteElementsBetweenLabels );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeleteElementsBetweenLabels, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeleteElementsBetweenLabels() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeleteElementsBetweenLabels()                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETEELEMENTSBETWEENLABELS( HPS, LONG, LONG );
PCHAR szGpi32DeleteElementsBetweenLabels = "Gpi32DeleteElementsBetweenLabels";

BOOL APIENTRY Trc_Gpi32DeleteElementsBetweenLabels( HPS  Parm1
                                                  , LONG Parm2
                                                  , LONG Parm3
                                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeleteElementsBetweenLabels );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32DeleteElementsBetweenLabels, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETEELEMENTSBETWEENLABELS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeleteElementsBetweenLabels );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeleteElementsBetweenLabels, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeleteElementsBetweenLabels() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeleteMetaFile()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETEMETAFILE( HMF );
PCHAR szGpi16DeleteMetaFile = "Gpi16DeleteMetaFile";

BOOL16 APIENTRY16 Trc_Gpi16DeleteMetaFile( HMF Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeleteMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16DeleteMetaFile, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETEMETAFILE( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeleteMetaFile );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeleteMetaFile, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeleteMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeleteMetaFile()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETEMETAFILE( HMF );
PCHAR szGpi32DeleteMetaFile = "Gpi32DeleteMetaFile";

BOOL APIENTRY Trc_Gpi32DeleteMetaFile( HMF Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeleteMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32DeleteMetaFile, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETEMETAFILE( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeleteMetaFile );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeleteMetaFile, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeleteMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeletePalette()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETEPALETTE( HPAL );
PCHAR szGpi16DeletePalette = "Gpi16DeletePalette";

BOOL16 APIENTRY16 Trc_Gpi16DeletePalette( HPAL Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeletePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16DeletePalette, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETEPALETTE( Parm1 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeletePalette );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeletePalette, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeletePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeletePalette()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETEPALETTE( HPAL );
PCHAR szGpi32DeletePalette = "Gpi32DeletePalette";

BOOL APIENTRY Trc_Gpi32DeletePalette( HPAL Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeletePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32DeletePalette, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETEPALETTE( Parm1 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeletePalette );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeletePalette, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeletePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeleteSegment()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETESEGMENT( HPS, LONG );
PCHAR szGpi16DeleteSegment = "Gpi16DeleteSegment";

BOOL16 APIENTRY16 Trc_Gpi16DeleteSegment( HPS  Parm1
                                        , LONG Parm2
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeleteSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16DeleteSegment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETESEGMENT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeleteSegment );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeleteSegment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeleteSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeleteSegment()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETESEGMENT( HPS, LONG );
PCHAR szGpi32DeleteSegment = "Gpi32DeleteSegment";

BOOL APIENTRY Trc_Gpi32DeleteSegment( HPS  Parm1
                                    , LONG Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeleteSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32DeleteSegment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETESEGMENT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeleteSegment );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeleteSegment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeleteSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeleteSegments()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETESEGMENTS( HPS, LONG, LONG );
PCHAR szGpi16DeleteSegments = "Gpi16DeleteSegments";

BOOL16 APIENTRY16 Trc_Gpi16DeleteSegments( HPS  Parm1
                                         , LONG Parm2
                                         , LONG Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeleteSegments );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16DeleteSegments, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETESEGMENTS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeleteSegments );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeleteSegments, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeleteSegments() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeleteSegments()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETESEGMENTS( HPS, LONG, LONG );
PCHAR szGpi32DeleteSegments = "Gpi32DeleteSegments";

BOOL APIENTRY Trc_Gpi32DeleteSegments( HPS  Parm1
                                     , LONG Parm2
                                     , LONG Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeleteSegments );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32DeleteSegments, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETESEGMENTS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeleteSegments );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeleteSegments, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeleteSegments() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DeleteSetId()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDELETESETID( HPS, LONG );
PCHAR szGpi16DeleteSetId = "Gpi16DeleteSetId";

BOOL16 APIENTRY16 Trc_Gpi16DeleteSetId( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DeleteSetId );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16DeleteSetId, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDELETESETID( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DeleteSetId );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DeleteSetId, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DeleteSetId() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DeleteSetId()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DELETESETID( HPS, LONG );
PCHAR szGpi32DeleteSetId = "Gpi32DeleteSetId";

BOOL APIENTRY Trc_Gpi32DeleteSetId( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DeleteSetId );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32DeleteSetId, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DELETESETID( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DeleteSetId );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DeleteSetId, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DeleteSetId() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DestroyPS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDESTROYPS( HPS );
PCHAR szGpi16DestroyPS = "Gpi16DestroyPS";

BOOL16 APIENTRY16 Trc_Gpi16DestroyPS( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DestroyPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16DestroyPS, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDESTROYPS( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DestroyPS );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DestroyPS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DestroyPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DestroyPS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DESTROYPS( HPS );
PCHAR szGpi32DestroyPS = "Gpi32DestroyPS";

BOOL APIENTRY Trc_Gpi32DestroyPS( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DestroyPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32DestroyPS, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DESTROYPS( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DestroyPS );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DestroyPS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DestroyPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DestroyRegion()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDESTROYREGION( HPS, HRGN );
PCHAR szGpi16DestroyRegion = "Gpi16DestroyRegion";

BOOL16 APIENTRY16 Trc_Gpi16DestroyRegion( HPS  Parm1
                                        , HRGN Parm2
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DestroyRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16DestroyRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDESTROYREGION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DestroyRegion );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DestroyRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DestroyRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DestroyRegion()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DESTROYREGION( HPS, HRGN );
PCHAR szGpi32DestroyRegion = "Gpi32DestroyRegion";

BOOL APIENTRY Trc_Gpi32DestroyRegion( HPS  Parm1
                                    , HRGN Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DestroyRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32DestroyRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DESTROYREGION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DestroyRegion );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DestroyRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DestroyRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DrawBits()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIDRAWBITS( HPS, PVOID16, PBITMAPINFO216, LONG, PPOINTL16, LONG, ULONG );
PCHAR szGpi16DrawBits = "Gpi16DrawBits";

LONG APIENTRY16 Trc_Gpi16DrawBits( HPS            Parm1
                                 , PVOID16        Parm2
                                 , PBITMAPINFO216 Parm3
                                 , LONG           Parm4
                                 , PPOINTL16      Parm5
                                 , LONG           Parm6
                                 , ULONG          Parm7
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DrawBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogG16DB_Parm2( Parm2, Parm3, Parm5 );
         T_LogParameter( 3L, DT_PBITMAPINFO2 );
         T_LogPBITMAPINFO216( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm5, Parm4 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY16( szGpi16DrawBits, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIDRAWBITS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DrawBits );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16DrawBits, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16DrawBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DrawBits()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32DRAWBITS( HPS, PVOID, PBITMAPINFO2, LONG, PPOINTL, LONG, ULONG );
PCHAR szGpi32DrawBits = "Gpi32DrawBits";

LONG APIENTRY Trc_Gpi32DrawBits( HPS          Parm1
                               , PVOID        Parm2
                               , PBITMAPINFO2 Parm3
                               , LONG         Parm4
                               , PPOINTL      Parm5
                               , LONG         Parm6
                               , ULONG        Parm7
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DrawBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogG32DB_Parm2( Parm2, Parm3, Parm5 );
         T_LogParameter( 3L, DT_PBITMAPINFO2 );
         T_LogPBITMAPINFO2( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm5, Parm4 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szGpi32DrawBits, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32DRAWBITS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DrawBits );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32DrawBits, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32DrawBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DrawChain()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDRAWCHAIN( HPS );
PCHAR szGpi16DrawChain = "Gpi16DrawChain";

BOOL16 APIENTRY16 Trc_Gpi16DrawChain( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DrawChain );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16DrawChain, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDRAWCHAIN( Parm1 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DrawChain );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DrawChain, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DrawChain() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DrawChain()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DRAWCHAIN( HPS );
PCHAR szGpi32DrawChain = "Gpi32DrawChain";

BOOL APIENTRY Trc_Gpi32DrawChain( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DrawChain );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32DrawChain, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DRAWCHAIN( Parm1 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DrawChain );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DrawChain, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DrawChain() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DrawDynamics()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDRAWDYNAMICS( HPS );
PCHAR szGpi16DrawDynamics = "Gpi16DrawDynamics";

BOOL16 APIENTRY16 Trc_Gpi16DrawDynamics( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DrawDynamics );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16DrawDynamics, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDRAWDYNAMICS( Parm1 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DrawDynamics );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DrawDynamics, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DrawDynamics() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DrawDynamics()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DRAWDYNAMICS( HPS );
PCHAR szGpi32DrawDynamics = "Gpi32DrawDynamics";

BOOL APIENTRY Trc_Gpi32DrawDynamics( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DrawDynamics );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32DrawDynamics, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DRAWDYNAMICS( Parm1 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DrawDynamics );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DrawDynamics, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DrawDynamics() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DrawFrom()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDRAWFROM( HPS, LONG, LONG );
PCHAR szGpi16DrawFrom = "Gpi16DrawFrom";

BOOL16 APIENTRY16 Trc_Gpi16DrawFrom( HPS  Parm1
                                   , LONG Parm2
                                   , LONG Parm3
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DrawFrom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16DrawFrom, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDRAWFROM( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DrawFrom );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DrawFrom, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DrawFrom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DrawFrom()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DRAWFROM( HPS, LONG, LONG );
PCHAR szGpi32DrawFrom = "Gpi32DrawFrom";

BOOL APIENTRY Trc_Gpi32DrawFrom( HPS  Parm1
                               , LONG Parm2
                               , LONG Parm3
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DrawFrom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32DrawFrom, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DRAWFROM( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DrawFrom );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DrawFrom, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DrawFrom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16DrawSegment()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIDRAWSEGMENT( HPS, LONG );
PCHAR szGpi16DrawSegment = "Gpi16DrawSegment";

BOOL16 APIENTRY16 Trc_Gpi16DrawSegment( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16DrawSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16DrawSegment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIDRAWSEGMENT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16DrawSegment );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16DrawSegment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16DrawSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32DrawSegment()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32DRAWSEGMENT( HPS, LONG );
PCHAR szGpi32DrawSegment = "Gpi32DrawSegment";

BOOL APIENTRY Trc_Gpi32DrawSegment( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32DrawSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32DrawSegment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32DRAWSEGMENT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32DrawSegment );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32DrawSegment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32DrawSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Element()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIELEMENT( HPS, LONG, PSZ16, LONG, PBYTE16 );
PCHAR szGpi16Element = "Gpi16Element";

LONG APIENTRY16 Trc_Gpi16Element( HPS     Parm1
                                , LONG    Parm2
                                , PSZ16   Parm3
                                , LONG    Parm4
                                , PBYTE16 Parm5
                                )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Element );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PBYTE );
         T_LogPVOID16( Parm5 );
         T_LogCHARBuffer16( (PCHAR16)Parm5, Parm4 );
      }
      HOOKENTRY16( szGpi16Element, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIELEMENT( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Element );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16Element, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16Element() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Element()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32ELEMENT( HPS, LONG, PSZ, LONG, PBYTE );
PCHAR szGpi32Element = "Gpi32Element";

LONG APIENTRY Trc_Gpi32Element( HPS   Parm1
                              , LONG  Parm2
                              , PSZ   Parm3
                              , LONG  Parm4
                              , PBYTE Parm5
                              )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Element );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PBYTE );
         T_LogPVOID( Parm5 );
         T_LogCHARBuffer( (PCHAR)Parm5, Parm4 );
      }
      HOOKENTRY( szGpi32Element, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32ELEMENT( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Element );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32Element, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32Element() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16EndArea()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIENDAREA( HPS );
PCHAR szGpi16EndArea = "Gpi16EndArea";

LONG APIENTRY16 Trc_Gpi16EndArea( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16EndArea );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16EndArea, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIENDAREA( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16EndArea );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16EndArea, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16EndArea() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32EndArea()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32ENDAREA( HPS );
PCHAR szGpi32EndArea = "Gpi32EndArea";

LONG APIENTRY Trc_Gpi32EndArea( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32EndArea );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32EndArea, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32ENDAREA( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32EndArea );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32EndArea, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32EndArea() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16EndElement()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIENDELEMENT( HPS );
PCHAR szGpi16EndElement = "Gpi16EndElement";

BOOL16 APIENTRY16 Trc_Gpi16EndElement( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16EndElement );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16EndElement, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIENDELEMENT( Parm1 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16EndElement );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16EndElement, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16EndElement() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32EndElement()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32ENDELEMENT( HPS );
PCHAR szGpi32EndElement = "Gpi32EndElement";

BOOL APIENTRY Trc_Gpi32EndElement( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32EndElement );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32EndElement, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32ENDELEMENT( Parm1 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32EndElement );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32EndElement, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32EndElement() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32EndInkPath()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
BOOL APIENTRY GPIENDINKPATH( HPS, ULONG );
PCHAR szGpi32EndInkPath = "Gpi32EndInkPath";

BOOL APIENTRY Trc_Gpi32EndInkPath( HPS   Parm1
                                 , ULONG Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_INK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32EndInkPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32EndInkPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnGPI32ENDINKPATH )( Parm1, Parm2 );

   if ( LOGGPIAPI( G_INK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32EndInkPath );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32EndInkPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32EndInkPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16EndPath()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIENDPATH( HPS );
PCHAR szGpi16EndPath = "Gpi16EndPath";

BOOL16 APIENTRY16 Trc_Gpi16EndPath( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16EndPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16EndPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIENDPATH( Parm1 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16EndPath );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16EndPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16EndPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32EndPath()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32ENDPATH( HPS );
PCHAR szGpi32EndPath = "Gpi32EndPath";

BOOL APIENTRY Trc_Gpi32EndPath( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32EndPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32EndPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32ENDPATH( Parm1 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32EndPath );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32EndPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32EndPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16EqualRegion()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIEQUALREGION( HPS, HRGN, HRGN );
PCHAR szGpi16EqualRegion = "Gpi16EqualRegion";

LONG APIENTRY16 Trc_Gpi16EqualRegion( HPS  Parm1
                                    , HRGN Parm2
                                    , HRGN Parm3
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16EqualRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HRGN );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16EqualRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIEQUALREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16EqualRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != EQRGN_ERROR );
      HOOKEXIT16( szGpi16EqualRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16EqualRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32EqualRegion()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32EQUALREGION( HPS, HRGN, HRGN );
PCHAR szGpi32EqualRegion = "Gpi32EqualRegion";

LONG APIENTRY Trc_Gpi32EqualRegion( HPS  Parm1
                                  , HRGN Parm2
                                  , HRGN Parm3
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32EqualRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HRGN );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32EqualRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32EQUALREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32EqualRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != EQRGN_ERROR );
      HOOKEXIT( szGpi32EqualRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32EqualRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Erase()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIERASE( HPS );
PCHAR szGpi16Erase = "Gpi16Erase";

BOOL16 APIENTRY16 Trc_Gpi16Erase( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Erase );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16Erase, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIERASE( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Erase );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16Erase, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Erase() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Erase()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32ERASE( HPS );
PCHAR szGpi32Erase = "Gpi32Erase";

BOOL APIENTRY Trc_Gpi32Erase( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Erase );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32Erase, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32ERASE( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Erase );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32Erase, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Erase() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16ErrorSegmentData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIERRORSEGMENTDATA( HPS, PLONG, PLONG );
PCHAR szGpi16ErrorSegmentData = "Gpi16ErrorSegmentData";

LONG APIENTRY16 Trc_Gpi16ErrorSegmentData( HPS   Parm1
                                         , PLONG Parm2
                                         , PLONG Parm3
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16ErrorSegmentData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16ErrorSegmentData, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIERRORSEGMENTDATA( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16ErrorSegmentData );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
      }
      HOOKEXIT16( szGpi16ErrorSegmentData, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16ErrorSegmentData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32ErrorSegmentData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32ERRORSEGMENTDATA( HPS, PLONG, PLONG );
PCHAR szGpi32ErrorSegmentData = "Gpi32ErrorSegmentData";

LONG APIENTRY Trc_Gpi32ErrorSegmentData( HPS   Parm1
                                       , PLONG Parm2
                                       , PLONG Parm3
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32ErrorSegmentData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32ErrorSegmentData, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32ERRORSEGMENTDATA( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32ErrorSegmentData );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
      }
      HOOKEXIT( szGpi32ErrorSegmentData, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32ErrorSegmentData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16ExcludeClipRectangle()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIEXCLUDECLIPRECTANGLE( HPS, PRECTL );
PCHAR szGpi16ExcludeClipRectangle = "Gpi16ExcludeClipRectangle";

LONG APIENTRY16 Trc_Gpi16ExcludeClipRectangle( HPS      Parm1
                                             , PRECTL16 Parm2
                                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16ExcludeClipRectangle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16ExcludeClipRectangle, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIEXCLUDECLIPRECTANGLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16ExcludeClipRectangle );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT16( szGpi16ExcludeClipRectangle, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16ExcludeClipRectangle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32ExcludeClipRectangle()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32EXCLUDECLIPRECTANGLE( HPS, PRECTL );
PCHAR szGpi32ExcludeClipRectangle = "Gpi32ExcludeClipRectangle";

LONG APIENTRY Trc_Gpi32ExcludeClipRectangle( HPS    Parm1
                                           , PRECTL Parm2
                                           )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32ExcludeClipRectangle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szGpi32ExcludeClipRectangle, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32EXCLUDECLIPRECTANGLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32ExcludeClipRectangle );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT( szGpi32ExcludeClipRectangle, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32ExcludeClipRectangle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16FillPath()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIFILLPATH( HPS, LONG, LONG );
PCHAR szGpi16FillPath = "Gpi16FillPath";

LONG APIENTRY16 Trc_Gpi16FillPath( HPS  Parm1
                                 , LONG Parm2
                                 , LONG Parm3
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16FillPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16FillPath, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIFILLPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16FillPath );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16FillPath, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16FillPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32FillPath()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32FILLPATH( HPS, LONG, LONG );
PCHAR szGpi32FillPath = "Gpi32FillPath";

LONG APIENTRY Trc_Gpi32FillPath( HPS  Parm1
                               , LONG Parm2
                               , LONG Parm3
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32FillPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32FillPath, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32FILLPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32FillPath );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32FillPath, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32FillPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16FloodFill()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIFLOODFILL( HPS, LONG, LONG );
PCHAR szGpi16FloodFill = "Gpi16FloodFill";

LONG APIENTRY16 Trc_Gpi16FloodFill( HPS  Parm1
                                  , LONG Parm2
                                  , LONG Parm3
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16FloodFill );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16FloodFill, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIFLOODFILL( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16FloodFill );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16FloodFill, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16FloodFill() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32FloodFill()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32FLOODFILL( HPS, LONG, LONG );
PCHAR szGpi32FloodFill = "Gpi32FloodFill";

LONG APIENTRY Trc_Gpi32FloodFill( HPS  Parm1
                                , LONG Parm2
                                , LONG Parm3
                                )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32FloodFill );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32FloodFill, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32FLOODFILL( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32FloodFill );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32FloodFill, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32FloodFill() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16FrameRegion()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIFRAMEREGION( HPS, HRGN, PSIZEL16 );
PCHAR szGpi16FrameRegion = "Gpi16FrameRegion";

LONG APIENTRY16 Trc_Gpi16FrameRegion( HPS      Parm1
                                    , HRGN     Parm2
                                    , PSIZEL16 Parm3
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16FrameRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSIZEL );
         T_LogPSIZEL16( Parm3 );
      }
      HOOKENTRY16( szGpi16FrameRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIFRAMEREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16FrameRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16FrameRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16FrameRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32FrameRegion()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32FRAMEREGION( HPS, HRGN, PSIZEL );
PCHAR szGpi32FrameRegion = "Gpi32FrameRegion";

LONG APIENTRY Trc_Gpi32FrameRegion( HPS    Parm1
                                  , HRGN   Parm2
                                  , PSIZEL Parm3
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32FrameRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSIZEL );
         T_LogPSIZEL( Parm3 );
      }
      HOOKENTRY( szGpi32FrameRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32FRAMEREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32FrameRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32FrameRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32FrameRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16FullArc()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIFULLARC( HPS, LONG, FIXED );
PCHAR szGpi16FullArc = "Gpi16FullArc";

LONG APIENTRY16 Trc_Gpi16FullArc( HPS   Parm1
                                , LONG  Parm2
                                , FIXED Parm3
                                )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16FullArc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_FIXED );
         T_LogFIXED( Parm3 );
      }
      HOOKENTRY16( szGpi16FullArc, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIFULLARC( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16FullArc );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16FullArc, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16FullArc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32FullArc()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32FULLARC( HPS, LONG, FIXED );
PCHAR szGpi32FullArc = "Gpi32FullArc";

LONG APIENTRY Trc_Gpi32FullArc( HPS   Parm1
                              , LONG  Parm2
                              , FIXED Parm3
                              )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32FullArc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_FIXED );
         T_LogFIXED( Parm3 );
      }
      HOOKENTRY( szGpi32FullArc, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32FULLARC( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32FullArc );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32FullArc, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32FullArc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16GetData()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIGETDATA( HPS, LONG, PLONG16, LONG, LONG, PBYTE16 );
PCHAR szGpi16GetData = "Gpi16GetData";

LONG APIENTRY16 Trc_Gpi16GetData( HPS     Parm1
                                , LONG    Parm2
                                , PLONG16 Parm3
                                , LONG    Parm4
                                , LONG    Parm5
                                , PBYTE16 Parm6
                                )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16GetData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PBYTE );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szGpi16GetData, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIGETDATA( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16GetData );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
         T_LogParameter( 6L, DT_PBYTE );
         T_LogPVOID16( Parm6 );
         if ( lRC != GPI_ALTERROR ) T_LogCHARBuffer16( (PCHAR16)Parm6, lRC );
      }
      HOOKEXIT16( szGpi16GetData, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16GetData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32GetData()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32GETDATA( HPS, LONG, PLONG, LONG, LONG, PBYTE );
PCHAR szGpi32GetData = "Gpi32GetData";

LONG APIENTRY Trc_Gpi32GetData( HPS   Parm1
                              , LONG  Parm2
                              , PLONG Parm3
                              , LONG  Parm4
                              , LONG  Parm5
                              , PBYTE Parm6
                              )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32GetData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PBYTE );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szGpi32GetData, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32GETDATA( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32GetData );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 6L, DT_PBYTE );
         T_LogPVOID( Parm6 );
         if ( lRC != GPI_ALTERROR ) T_LogCHARBuffer( (PCHAR)Parm6, lRC );
      }
      HOOKEXIT( szGpi32GetData, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32GetData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Image()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIIMAGE( HPS, LONG, PSIZEL16, LONG, PBYTE16 );
PCHAR szGpi16Image = "Gpi16Image";

LONG APIENTRY16 Trc_Gpi16Image( HPS      Parm1
                              , LONG     Parm2
                              , PSIZEL16 Parm3
                              , LONG     Parm4
                              , PBYTE16  Parm5
                              )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Image );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSIZEL );
         T_LogPSIZEL16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PBYTE );
         T_LogPVOID16( Parm5 );
         T_LogCHARBuffer16( (PCHAR16)Parm5, Parm4 );
      }
      HOOKENTRY16( szGpi16Image, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIIMAGE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Image );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16Image, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16Image() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Image()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32IMAGE( HPS, LONG, PSIZEL, LONG, PBYTE );
PCHAR szGpi32Image = "Gpi32Image";

LONG APIENTRY Trc_Gpi32Image( HPS    Parm1
                            , LONG   Parm2
                            , PSIZEL Parm3
                            , LONG   Parm4
                            , PBYTE  Parm5
                            )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Image );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSIZEL );
         T_LogPSIZEL( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PBYTE );
         T_LogPVOID( Parm5 );
         T_LogCHARBuffer( (PCHAR)Parm5, Parm4 );
      }
      HOOKENTRY( szGpi32Image, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32IMAGE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Image );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32Image, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32Image() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16IntersectClipRectangle()                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIINTERSECTCLIPRECTANGLE( HPS, PRECTL16 );
PCHAR szGpi16IntersectClipRectangle = "Gpi16IntersectClipRectangle";

LONG APIENTRY16 Trc_Gpi16IntersectClipRectangle( HPS      Parm1
                                               , PRECTL16 Parm2
                                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16IntersectClipRectangle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16IntersectClipRectangle, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIINTERSECTCLIPRECTANGLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16IntersectClipRectangle );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT16( szGpi16IntersectClipRectangle, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16IntersectClipRectangle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32IntersectClipRectangle()                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32INTERSECTCLIPRECTANGLE( HPS, PRECTL );
PCHAR szGpi32IntersectClipRectangle = "Gpi32IntersectClipRectangle";

LONG APIENTRY Trc_Gpi32IntersectClipRectangle( HPS    Parm1
                                             , PRECTL Parm2
                                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32IntersectClipRectangle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szGpi32IntersectClipRectangle, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32INTERSECTCLIPRECTANGLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32IntersectClipRectangle );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT( szGpi32IntersectClipRectangle, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32IntersectClipRectangle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Label()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPILABEL( HPS, LONG );
PCHAR szGpi16Label = "Gpi16Label";

BOOL16 APIENTRY16 Trc_Gpi16Label( HPS  Parm1
                                , LONG Parm2
                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Label );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16Label, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPILABEL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Label );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16Label, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Label() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Label()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32LABEL( HPS, LONG );
PCHAR szGpi32Label = "Gpi32Label";

BOOL APIENTRY Trc_Gpi32Label( HPS  Parm1
                            , LONG Parm2
                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Label );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32Label, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32LABEL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Label );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32Label, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Label() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Line()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPILINE( HPS, PPOINTL16 );
PCHAR szGpi16Line = "Gpi16Line";

LONG APIENTRY16 Trc_Gpi16Line( HPS       Parm1
                             , PPOINTL16 Parm2
                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Line );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16Line, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPILINE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Line );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16Line, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16Line() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Line()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32LINE( HPS, PPOINTL );
PCHAR szGpi32Line = "Gpi32Line";

LONG APIENTRY Trc_Gpi32Line( HPS     Parm1
                           , PPOINTL Parm2
                           )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Line );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32Line, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32LINE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Line );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32Line, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32Line() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32LoadBitmap()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HBITMAP APIENTRY GPI32LOADBITMAP( HPS, HMODULE, ULONG, LONG, LONG );
PCHAR szGpi32LoadBitmap = "Gpi32LoadBitmap";

HBITMAP APIENTRY Trc_Gpi32LoadBitmap( HPS     Parm1
                                    , HMODULE Parm2
                                    , ULONG   Parm3
                                    , LONG    Parm4
                                    , LONG    Parm5
                                    )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32LoadBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32LoadBitmap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32LOADBITMAP( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32LoadBitmap );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT( szGpi32LoadBitmap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32LoadBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16LoadFonts()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPILOADFONTS( HAB, PSZ16 );
PCHAR szGpi16LoadFonts = "Gpi16LoadFonts";

BOOL16 APIENTRY16 Trc_Gpi16LoadFonts( HAB   Parm1
                                    , PSZ16 Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16LoadFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szGpi16LoadFonts, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPILOADFONTS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16LoadFonts );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16LoadFonts, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16LoadFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32LoadFonts()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32LOADFONTS( HAB, PSZ );
PCHAR szGpi32LoadFonts = "Gpi32LoadFonts";

BOOL APIENTRY Trc_Gpi32LoadFonts( HAB Parm1
                                , PSZ Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32LoadFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szGpi32LoadFonts, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32LOADFONTS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32LoadFonts );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32LoadFonts, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32LoadFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16LoadMetaFile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HMF APIENTRY16 GPILOADMETAFILE( HAB, PSZ16 );
PCHAR szGpi16LoadMetaFile = "Gpi16LoadMetaFile";

HMF APIENTRY16 Trc_Gpi16LoadMetaFile( HAB   Parm1
                                    , PSZ16 Parm2
                                    )
{

   HMF ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16LoadMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szGpi16LoadMetaFile, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPILOADMETAFILE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16LoadMetaFile );
      T_LogPMReturnCode( DT_HMF, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT16( szGpi16LoadMetaFile, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16LoadMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32LoadMetaFile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HMF APIENTRY GPI32LOADMETAFILE( HAB, PSZ );
PCHAR szGpi32LoadMetaFile = "Gpi32LoadMetaFile";

HMF APIENTRY Trc_Gpi32LoadMetaFile( HAB Parm1
                                  , PSZ Parm2
                                  )
{

   HMF ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32LoadMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szGpi32LoadMetaFile, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32LOADMETAFILE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32LoadMetaFile );
      T_LogPMReturnCode( DT_HMF, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT( szGpi32LoadMetaFile, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32LoadMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16LoadPublicFonts()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPILOADPUBLICFONTS( HAB, PSZ16 );
PCHAR szGpi16LoadPublicFonts = "Gpi16LoadPublicFonts";

BOOL16 APIENTRY16 Trc_Gpi16LoadPublicFonts( HAB   Parm1
                                          , PSZ16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16LoadPublicFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szGpi16LoadPublicFonts, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPILOADPUBLICFONTS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16LoadPublicFonts );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16LoadPublicFonts, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16LoadPublicFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32LoadPublicFonts()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32LOADPUBLICFONTS( HAB, PSZ );
PCHAR szGpi32LoadPublicFonts = "Gpi32LoadPublicFonts";

BOOL APIENTRY Trc_Gpi32LoadPublicFonts( HAB Parm1
                                      , PSZ Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32LoadPublicFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szGpi32LoadPublicFonts, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32LOADPUBLICFONTS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32LoadPublicFonts );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32LoadPublicFonts, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32LoadPublicFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Marker()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIMARKER( HPS, PPOINTL16 );
PCHAR szGpi16Marker = "Gpi16Marker";

LONG APIENTRY16 Trc_Gpi16Marker( HPS       Parm1
                               , PPOINTL16 Parm2
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Marker );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16Marker, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIMARKER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Marker );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16Marker, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16Marker() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Marker()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32MARKER( HPS, PPOINTL );
PCHAR szGpi32Marker = "Gpi32Marker";

LONG APIENTRY Trc_Gpi32Marker( HPS     Parm1
                             , PPOINTL Parm2
                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Marker );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32Marker, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32MARKER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Marker );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32Marker, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32Marker() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16ModifyPath()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIMODIFYPATH( HPS, LONG, LONG );
PCHAR szGpi16ModifyPath = "Gpi16ModifyPath";

BOOL16 APIENTRY16 Trc_Gpi16ModifyPath( HPS  Parm1
                                     , LONG Parm2
                                     , LONG Parm3
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16ModifyPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16ModifyPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIMODIFYPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16ModifyPath );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16ModifyPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16ModifyPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32ModifyPath()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32MODIFYPATH( HPS, LONG, LONG );
PCHAR szGpi32ModifyPath = "Gpi32ModifyPath";

BOOL APIENTRY Trc_Gpi32ModifyPath( HPS  Parm1
                                 , LONG Parm2
                                 , LONG Parm3
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32ModifyPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32ModifyPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32MODIFYPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32ModifyPath );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32ModifyPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32ModifyPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Move()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIMOVE( HPS, PPOINTL16 );
PCHAR szGpi16Move = "Gpi16Move";

BOOL16 APIENTRY16 Trc_Gpi16Move( HPS       Parm1
                               , PPOINTL16 Parm2
                               )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Move );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16Move, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIMOVE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Move );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16Move, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Move() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Move()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32MOVE( HPS, PPOINTL );
PCHAR szGpi32Move = "Gpi32Move";

BOOL APIENTRY Trc_Gpi32Move( HPS     Parm1
                           , PPOINTL Parm2
                           )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Move );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32Move, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32MOVE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Move );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32Move, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Move() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16OffsetClipRegion()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIOFFSETCLIPREGION( HPS, PPOINTL16 );
PCHAR szGpi16OffsetClipRegion = "Gpi16OffsetClipRegion";

LONG APIENTRY16 Trc_Gpi16OffsetClipRegion( HPS       Parm1
                                         , PPOINTL16 Parm2
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16OffsetClipRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16OffsetClipRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIOFFSETCLIPREGION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16OffsetClipRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT16( szGpi16OffsetClipRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16OffsetClipRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32OffsetClipRegion()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32OFFSETCLIPREGION( HPS, PPOINTL );
PCHAR szGpi32OffsetClipRegion = "Gpi32OffsetClipRegion";

LONG APIENTRY Trc_Gpi32OffsetClipRegion( HPS     Parm1
                                       , PPOINTL Parm2
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32OffsetClipRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32OffsetClipRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32OFFSETCLIPREGION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32OffsetClipRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT( szGpi32OffsetClipRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32OffsetClipRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16OffsetElementPointer()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIOFFSETELEMENTPOINTER( HPS, LONG );
PCHAR szGpi16OffsetElementPointer = "Gpi16OffsetElementPointer";

BOOL16 APIENTRY16 Trc_Gpi16OffsetElementPointer( HPS  Parm1
                                               , LONG Parm2
                                               )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16OffsetElementPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16OffsetElementPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIOFFSETELEMENTPOINTER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16OffsetElementPointer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16OffsetElementPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16OffsetElementPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32OffsetElementPointer()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32OFFSETELEMENTPOINTER( HPS, LONG );
PCHAR szGpi32OffsetElementPointer = "Gpi32OffsetElementPointer";

BOOL APIENTRY Trc_Gpi32OffsetElementPointer( HPS  Parm1
                                           , LONG Parm2
                                           )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32OffsetElementPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32OffsetElementPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32OFFSETELEMENTPOINTER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32OffsetElementPointer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32OffsetElementPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32OffsetElementPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16OffsetRegion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIOFFSETREGION( HPS, HRGN, PPOINTL16 );
PCHAR szGpi16OffsetRegion = "Gpi16OffsetRegion";

BOOL16 APIENTRY16 Trc_Gpi16OffsetRegion( HPS       Parm1
                                       , HRGN      Parm2
                                       , PPOINTL16 Parm3
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16OffsetRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL16( Parm3 );
      }
      HOOKENTRY16( szGpi16OffsetRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIOFFSETREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16OffsetRegion );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16OffsetRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16OffsetRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32OffsetRegion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32OFFSETREGION( HPS, HRGN, PPOINTL );
PCHAR szGpi32OffsetRegion = "Gpi32OffsetRegion";

BOOL APIENTRY Trc_Gpi32OffsetRegion( HPS     Parm1
                                   , HRGN    Parm2
                                   , PPOINTL Parm3
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32OffsetRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL( Parm3 );
      }
      HOOKENTRY( szGpi32OffsetRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32OFFSETREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32OffsetRegion );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32OffsetRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32OffsetRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16OpenSegment()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIOPENSEGMENT( HPS, LONG );
PCHAR szGpi16OpenSegment = "Gpi16OpenSegment";

BOOL16 APIENTRY16 Trc_Gpi16OpenSegment( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16OpenSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16OpenSegment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIOPENSEGMENT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16OpenSegment );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16OpenSegment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16OpenSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32OpenSegment()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32OPENSEGMENT( HPS, LONG );
PCHAR szGpi32OpenSegment = "Gpi32OpenSegment";

BOOL APIENTRY Trc_Gpi32OpenSegment( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32OpenSegment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32OpenSegment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32OPENSEGMENT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32OpenSegment );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32OpenSegment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32OpenSegment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16OutlinePath()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIOUTLINEPATH( HPS, LONG, LONG );
PCHAR szGpi16OutlinePath = "Gpi16OutlinePath";

LONG APIENTRY16 Trc_Gpi16OutlinePath( HPS  Parm1
                                    , LONG Parm2
                                    , LONG Parm3
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16OutlinePath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16OutlinePath, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIOUTLINEPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16OutlinePath );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16OutlinePath, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16OutlinePath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32OutlinePath()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32OUTLINEPATH( HPS, LONG, LONG );
PCHAR szGpi32OutlinePath = "Gpi32OutlinePath";

LONG APIENTRY Trc_Gpi32OutlinePath( HPS  Parm1
                                  , LONG Parm2
                                  , LONG Parm3
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32OutlinePath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32OutlinePath, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32OUTLINEPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32OutlinePath );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32OutlinePath, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32OutlinePath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Oval()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 245 ) )
LONG APIENTRY GPI32OVAL( HPS, PPOINTL, FIXED, FIXED, ULONG );
PCHAR szGpi32Oval = "Gpi32Oval";

LONG APIENTRY Trc_Gpi32Oval( HPS     Parm1
                           , PPOINTL Parm2
                           , FIXED   Parm3
                           , FIXED   Parm4
                           , ULONG   Parm5
                           )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Oval );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm2, 2 );
         T_LogParameter( 3L, DT_FIXED );
         T_LogFIXED( Parm3 );
         T_LogParameter( 4L, DT_FIXED );
         T_LogFIXED( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32Oval, &Parm1 );
      T_FreeLog( );
   }

   lRC = ( *pfnGPI32OVAL )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Oval );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32Oval, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32Oval() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PaintRegion()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPAINTREGION( HPS, HRGN );
PCHAR szGpi16PaintRegion = "Gpi16PaintRegion";

LONG APIENTRY16 Trc_Gpi16PaintRegion( HPS  Parm1
                                    , HRGN Parm2
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PaintRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16PaintRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPAINTREGION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PaintRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PaintRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PaintRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PaintRegion()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32PAINTREGION( HPS, HRGN );
PCHAR szGpi32PaintRegion = "Gpi32PaintRegion";

LONG APIENTRY Trc_Gpi32PaintRegion( HPS  Parm1
                                  , HRGN Parm2
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PaintRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32PaintRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32PAINTREGION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PaintRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PaintRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PaintRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PartialArc()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPARTIALARC( HPS, PPOINTL16, FIXED, FIXED, FIXED );
PCHAR szGpi16PartialArc = "Gpi16PartialArc";

LONG APIENTRY16 Trc_Gpi16PartialArc( HPS       Parm1
                                   , PPOINTL16 Parm2
                                   , FIXED     Parm3
                                   , FIXED     Parm4
                                   , FIXED     Parm5
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PartialArc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
         T_LogParameter( 3L, DT_FIXED );
         T_LogFIXED( Parm3 );
         T_LogParameter( 4L, DT_FIXED );
         T_LogFIXED( Parm4 );
         T_LogParameter( 5L, DT_FIXED );
         T_LogFIXED( Parm5 );
      }
      HOOKENTRY16( szGpi16PartialArc, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPARTIALARC( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PartialArc );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PartialArc, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PartialArc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PartialArc()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32PARTIALARC( HPS, PPOINTL, FIXED, FIXED, FIXED );
PCHAR szGpi32PartialArc = "Gpi32PartialArc";

LONG APIENTRY Trc_Gpi32PartialArc( HPS     Parm1
                                 , PPOINTL Parm2
                                 , FIXED   Parm3
                                 , FIXED   Parm4
                                 , FIXED   Parm5
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PartialArc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
         T_LogParameter( 3L, DT_FIXED );
         T_LogFIXED( Parm3 );
         T_LogParameter( 4L, DT_FIXED );
         T_LogFIXED( Parm4 );
         T_LogParameter( 5L, DT_FIXED );
         T_LogFIXED( Parm5 );
      }
      HOOKENTRY( szGpi32PartialArc, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32PARTIALARC( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PartialArc );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PartialArc, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PartialArc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PathToRegion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HRGN APIENTRY16 GPIPATHTOREGION( HPS, LONG, LONG );
PCHAR szGpi16PathToRegion = "Gpi16PathToRegion";

HRGN APIENTRY16 Trc_Gpi16PathToRegion( HPS  Parm1
                                     , LONG Parm2
                                     , LONG Parm3
                                     )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PathToRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16PathToRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPIPATHTOREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PathToRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != RGN_ERROR );
      HOOKEXIT16( szGpi16PathToRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16PathToRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PathToRegion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HRGN APIENTRY GPI32PATHTOREGION( HPS, LONG, LONG );
PCHAR szGpi32PathToRegion = "Gpi32PathToRegion";

HRGN APIENTRY Trc_Gpi32PathToRegion( HPS  Parm1
                                   , LONG Parm2
                                   , LONG Parm3
                                   )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PathToRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32PathToRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32PATHTOREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PathToRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != RGN_ERROR );
      HOOKEXIT( szGpi32PathToRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32PathToRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PlayMetaFile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPLAYMETAFILE( HPS, HMF, LONG, PLONG16, PLONG16, LONG, PSZ16 );
PCHAR szGpi16PlayMetaFile = "Gpi16PlayMetaFile";

LONG APIENTRY16 Trc_Gpi16PlayMetaFile( HPS     Parm1
                                     , HMF     Parm2
                                     , LONG    Parm3
                                     , PLONG16 Parm4
                                     , PLONG16 Parm5
                                     , LONG    Parm6
                                     , PSZ16   Parm7
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PlayMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMF );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID16( Parm4 );
         T_LogULONGBuffer16( (PULONG16)Parm4, Parm3 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PSZ );
         T_LogPVOID16( Parm7 );
      }
      HOOKENTRY16( szGpi16PlayMetaFile, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPLAYMETAFILE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PlayMetaFile );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm5 );
         T_LogParameter( 7L, DT_PSZ );
         if ( lRC != GPI_ERROR ) T_LogPSZ16( Parm7 );
         else T_LogPVOID16( Parm7 );
      }
      HOOKEXIT16( szGpi16PlayMetaFile, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PlayMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PlayMetaFile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32PLAYMETAFILE( HPS, HMF, LONG, PLONG, PLONG, LONG, PSZ );
PCHAR szGpi32PlayMetaFile = "Gpi32PlayMetaFile";

LONG APIENTRY Trc_Gpi32PlayMetaFile( HPS   Parm1
                                   , HMF   Parm2
                                   , LONG  Parm3
                                   , PLONG Parm4
                                   , PLONG Parm5
                                   , LONG  Parm6
                                   , PSZ   Parm7
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PlayMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMF );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID( Parm4 );
         T_LogULONGBuffer( (PULONG)Parm4, Parm3 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PSZ );
         T_LogPVOID( Parm7 );
      }
      HOOKENTRY( szGpi32PlayMetaFile, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32PLAYMETAFILE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PlayMetaFile );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm5 );
         T_LogParameter( 7L, DT_PSZ );
         if ( lRC != GPI_ERROR ) T_LogPSZ( Parm7 );
         else T_LogPVOID( Parm7 );
      }
      HOOKEXIT( szGpi32PlayMetaFile, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PlayMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PointArc()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPOINTARC( HPS, PPOINTL16 );
PCHAR szGpi16PointArc = "Gpi16PointArc";

LONG APIENTRY16 Trc_Gpi16PointArc( HPS       Parm1
                                 , PPOINTL16 Parm2
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PointArc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm2, 2 );
      }
      HOOKENTRY16( szGpi16PointArc, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPOINTARC( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PointArc );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PointArc, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PointArc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PointArc()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32POINTARC( HPS, PPOINTL );
PCHAR szGpi32PointArc = "Gpi32PointArc";

LONG APIENTRY Trc_Gpi32PointArc( HPS     Parm1
                               , PPOINTL Parm2
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PointArc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm2, 2 );
      }
      HOOKENTRY( szGpi32PointArc, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32POINTARC( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PointArc );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PointArc, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PointArc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PolyFillet()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPOLYFILLET( HPS, LONG, PPOINTL16 );
PCHAR szGpi16PolyFillet = "Gpi16PolyFillet";

LONG APIENTRY16 Trc_Gpi16PolyFillet( HPS       Parm1
                                   , LONG      Parm2
                                   , PPOINTL16 Parm3
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PolyFillet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm3, Parm2 );
      }
      HOOKENTRY16( szGpi16PolyFillet, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPOLYFILLET( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PolyFillet );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PolyFillet, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PolyFillet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PolyFillet()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32POLYFILLET( HPS, LONG, PPOINTL );
PCHAR szGpi32PolyFillet = "Gpi32PolyFillet";

LONG APIENTRY Trc_Gpi32PolyFillet( HPS     Parm1
                                 , LONG    Parm2
                                 , PPOINTL Parm3
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PolyFillet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm3, Parm2 );
      }
      HOOKENTRY( szGpi32PolyFillet, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32POLYFILLET( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PolyFillet );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PolyFillet, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PolyFillet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PolyFilletSharp()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPOLYFILLETSHARP( HPS, LONG, PPOINTL16, PFIXED16 );
PCHAR szGpi16PolyFilletSharp = "Gpi16PolyFilletSharp";

LONG APIENTRY16 Trc_Gpi16PolyFilletSharp( HPS       Parm1
                                        , LONG      Parm2
                                        , PPOINTL16 Parm3
                                        , PFIXED16  Parm4
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PolyFilletSharp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm3, Parm2 );
         T_LogParameter( 4L, DT_PFIXED );
         T_LogPVOID16( Parm4 );
         T_LogFIXEDBuffer16( Parm4, Parm2 / 2 );
      }
      HOOKENTRY16( szGpi16PolyFilletSharp, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPOLYFILLETSHARP( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PolyFilletSharp );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PolyFilletSharp, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PolyFilletSharp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PolyFilletSharp()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32POLYFILLETSHARP( HPS, LONG, PPOINTL, PFIXED );
PCHAR szGpi32PolyFilletSharp = "Gpi32PolyFilletSharp";

LONG APIENTRY Trc_Gpi32PolyFilletSharp( HPS     Parm1
                                      , LONG    Parm2
                                      , PPOINTL Parm3
                                      , PFIXED  Parm4
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PolyFilletSharp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm3, Parm2 );
         T_LogParameter( 4L, DT_PFIXED );
         T_LogPVOID( Parm4 );
         T_LogFIXEDBuffer( Parm4, Parm2 / 2 );
      }
      HOOKENTRY( szGpi32PolyFilletSharp, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32POLYFILLETSHARP( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PolyFilletSharp );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PolyFilletSharp, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PolyFilletSharp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Polygons()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32POLYGONS( HPS, ULONG, PPOLYGON, ULONG, ULONG );
PCHAR szGpi32Polygons = "Gpi32Polygons";

LONG APIENTRY Trc_Gpi32Polygons( HPS      Parm1
                               , ULONG    Parm2
                               , PPOLYGON Parm3
                               , ULONG    Parm4
                               , ULONG    Parm5
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_POLY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Polygons );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOLYGON );
         T_LogPOLYGONBuffer( Parm3, Parm2 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32Polygons, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32POLYGONS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_POLY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Polygons );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32Polygons, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32Polygons() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PolyLine()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPOLYLINE( HPS, LONG, PPOINTL16 );
PCHAR szGpi16PolyLine = "Gpi16PolyLine";

LONG APIENTRY16 Trc_Gpi16PolyLine( HPS       Parm1
                                 , LONG      Parm2
                                 , PPOINTL16 Parm3
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PolyLine );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm3, Parm2 );
      }
      HOOKENTRY16( szGpi16PolyLine, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPOLYLINE( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PolyLine );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PolyLine, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PolyLine() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PolyLine()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32POLYLINE( HPS, LONG, PPOINTL );
PCHAR szGpi32PolyLine = "Gpi32PolyLine";

LONG APIENTRY Trc_Gpi32PolyLine( HPS     Parm1
                               , LONG    Parm2
                               , PPOINTL Parm3
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PolyLine );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm3, Parm2 );
      }
      HOOKENTRY( szGpi32PolyLine, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32POLYLINE( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PolyLine );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PolyLine, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PolyLine() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PolyLineDisjoint()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPOLYLINEDISJOINT( HPS, LONG, PPOINTL16 );
PCHAR szGpi16PolyLineDisjoint = "Gpi16PolyLineDisjoint";

LONG APIENTRY16 Trc_Gpi16PolyLineDisjoint( HPS       Parm1
                                         , LONG      Parm2
                                         , PPOINTL16 Parm3
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PolyLineDisjoint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm3, Parm2 );
      }
      HOOKENTRY16( szGpi16PolyLineDisjoint, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPOLYLINEDISJOINT( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PolyLineDisjoint );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PolyLineDisjoint, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PolyLineDisjoint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PolyLineDisjoint()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32POLYLINEDISJOINT( HPS, LONG, PPOINTL );
PCHAR szGpi32PolyLineDisjoint = "Gpi32PolyLineDisjoint";

LONG APIENTRY Trc_Gpi32PolyLineDisjoint( HPS     Parm1
                                       , LONG    Parm2
                                       , PPOINTL Parm3
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PolyLineDisjoint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm3, Parm2 );
      }
      HOOKENTRY( szGpi32PolyLineDisjoint, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32POLYLINEDISJOINT( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PolyLineDisjoint );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PolyLineDisjoint, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PolyLineDisjoint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PolyMarker()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPOLYMARKER( HPS, LONG, PPOINTL16 );
PCHAR szGpi16PolyMarker = "Gpi16PolyMarker";

LONG APIENTRY16 Trc_Gpi16PolyMarker( HPS       Parm1
                                   , LONG      Parm2
                                   , PPOINTL16 Parm3
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PolyMarker );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm3, Parm2 );
      }
      HOOKENTRY16( szGpi16PolyMarker, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPOLYMARKER( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PolyMarker );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PolyMarker, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PolyMarker() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PolyMarker()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32POLYMARKER( HPS, LONG, PPOINTL );
PCHAR szGpi32PolyMarker = "Gpi32PolyMarker";

LONG APIENTRY Trc_Gpi32PolyMarker( HPS     Parm1
                                 , LONG    Parm2
                                 , PPOINTL Parm3
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PolyMarker );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm3, Parm2 );
      }
      HOOKENTRY( szGpi32PolyMarker, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32POLYMARKER( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PolyMarker );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PolyMarker, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PolyMarker() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PolySpline()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPOLYSPLINE( HPS, LONG, PPOINTL16 );
PCHAR szGpi16PolySpline = "Gpi16PolySpline";

LONG APIENTRY16 Trc_Gpi16PolySpline( HPS       Parm1
                                   , LONG      Parm2
                                   , PPOINTL16 Parm3
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PolySpline );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm3, Parm2 );
      }
      HOOKENTRY16( szGpi16PolySpline, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPOLYSPLINE( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PolySpline );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16PolySpline, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PolySpline() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PolySpline()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32POLYSPLINE( HPS, LONG, PPOINTL );
PCHAR szGpi32PolySpline = "Gpi32PolySpline";

LONG APIENTRY Trc_Gpi32PolySpline( HPS     Parm1
                                 , LONG    Parm2
                                 , PPOINTL Parm3
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PolySpline );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm3, Parm2 );
      }
      HOOKENTRY( szGpi32PolySpline, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32POLYSPLINE( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PolySpline );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32PolySpline, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PolySpline() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Pop()                                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIPOP( HPS, LONG );
PCHAR szGpi16Pop = "Gpi16Pop";

BOOL16 APIENTRY16 Trc_Gpi16Pop( HPS  Parm1
                              , LONG Parm2
                              )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Pop );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16Pop, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIPOP( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Pop );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16Pop, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Pop() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Pop()                                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32POP( HPS, LONG );
PCHAR szGpi32Pop = "Gpi32Pop";

BOOL APIENTRY Trc_Gpi32Pop( HPS  Parm1
                          , LONG Parm2
                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Pop );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32Pop, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32POP( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Pop );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32Pop, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Pop() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PtInRegion()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPTINREGION( HPS, HRGN, PPOINTL16 );
PCHAR szGpi16PtInRegion = "Gpi16PtInRegion";

LONG APIENTRY16 Trc_Gpi16PtInRegion( HPS       Parm1
                                   , HRGN      Parm2
                                   , PPOINTL16 Parm3
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PtInRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL16( Parm3 );
      }
      HOOKENTRY16( szGpi16PtInRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPTINREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PtInRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PRGN_ERROR );
      HOOKEXIT16( szGpi16PtInRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PtInRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PtInRegion()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32PTINREGION( HPS, HRGN, PPOINTL );
PCHAR szGpi32PtInRegion = "Gpi32PtInRegion";

LONG APIENTRY Trc_Gpi32PtInRegion( HPS     Parm1
                                 , HRGN    Parm2
                                 , PPOINTL Parm3
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PtInRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL( Parm3 );
      }
      HOOKENTRY( szGpi32PtInRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32PTINREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PtInRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PRGN_ERROR );
      HOOKEXIT( szGpi32PtInRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PtInRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PtVisible()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPTVISIBLE( HPS, PPOINTL16 );
PCHAR szGpi16PtVisible = "Gpi16PtVisible";

LONG APIENTRY16 Trc_Gpi16PtVisible( HPS       Parm1
                                  , PPOINTL16 Parm2
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PtVisible );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16PtVisible, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPTVISIBLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PtVisible );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PVIS_ERROR );
      HOOKEXIT16( szGpi16PtVisible, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PtVisible() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PtVisible()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32PTVISIBLE( HPS, PPOINTL );
PCHAR szGpi32PtVisible = "Gpi32PtVisible";

LONG APIENTRY Trc_Gpi32PtVisible( HPS     Parm1
                                , PPOINTL Parm2
                                )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PtVisible );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32PtVisible, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32PTVISIBLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PtVisible );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PVIS_ERROR );
      HOOKEXIT( szGpi32PtVisible, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PtVisible() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16PutData()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIPUTDATA( HPS, LONG, PLONG16, PBYTE16 );
PCHAR szGpi16PutData = "Gpi16PutData";

LONG APIENTRY16 Trc_Gpi16PutData( HPS     Parm1
                                , LONG    Parm2
                                , PLONG16 Parm3
                                , PBYTE16 Parm4
                                )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16PutData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         if ( T_IsValidMem16( Parm3, sizeof( LONG ) ) ) T_LogCHARBuffer16( (PCHAR16)Parm4, *Parm3 );
      }
      HOOKENTRY16( szGpi16PutData, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIPUTDATA( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16PutData );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
      }
      HOOKEXIT16( szGpi16PutData, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16PutData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32PutData()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32PUTDATA( HPS, LONG, PLONG, PBYTE );
PCHAR szGpi32PutData = "Gpi32PutData";

LONG APIENTRY Trc_Gpi32PutData( HPS   Parm1
                              , LONG  Parm2
                              , PLONG Parm3
                              , PBYTE Parm4
                              )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32PutData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         if ( T_IsValidMem( Parm3, sizeof( LONG ) ) ) T_LogCHARBuffer( (PCHAR)Parm4, *Parm3 );
      }
      HOOKENTRY( szGpi32PutData, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32PUTDATA( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32PutData );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
      }
      HOOKEXIT( szGpi32PutData, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32PutData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryArcParams()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYARCPARAMS( HPS, PARCPARAMS16 );
PCHAR szGpi16QueryArcParams = "Gpi16QueryArcParams";

BOOL16 APIENTRY16 Trc_Gpi16QueryArcParams( HPS          Parm1
                                         , PARCPARAMS16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryArcParams );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PARCPARAMS );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryArcParams, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYARCPARAMS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryArcParams );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PARCPARAMS );
         if ( bRC ) T_LogPARCPARAMS16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryArcParams, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryArcParams() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryArcParams()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYARCPARAMS( HPS, PARCPARAMS );
PCHAR szGpi32QueryArcParams = "Gpi32QueryArcParams";

BOOL APIENTRY Trc_Gpi32QueryArcParams( HPS        Parm1
                                     , PARCPARAMS Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryArcParams );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PARCPARAMS );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryArcParams, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYARCPARAMS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryArcParams );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PARCPARAMS );
         if ( bRC ) T_LogPARCPARAMS( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryArcParams, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryArcParams() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryAttrMode()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYATTRMODE( HPS );
PCHAR szGpi16QueryAttrMode = "Gpi16QueryAttrMode";

LONG APIENTRY16 Trc_Gpi16QueryAttrMode( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryAttrMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryAttrMode, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYATTRMODE( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryAttrMode );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != AM_ERROR );
      HOOKEXIT16( szGpi16QueryAttrMode, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryAttrMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryAttrMode()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYATTRMODE( HPS );
PCHAR szGpi32QueryAttrMode = "Gpi32QueryAttrMode";

LONG APIENTRY Trc_Gpi32QueryAttrMode( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryAttrMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryAttrMode, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYATTRMODE( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryAttrMode );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != AM_ERROR );
      HOOKEXIT( szGpi32QueryAttrMode, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryAttrMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryAttrs()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYATTRS( HPS, LONG, ULONG, PBUNDLE16 );
PCHAR szGpi16QueryAttrs = "Gpi16QueryAttrs";

LONG APIENTRY16 Trc_Gpi16QueryAttrs( HPS       Parm1
                                   , LONG      Parm2
                                   , ULONG     Parm3
                                   , PBUNDLE16 Parm4
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBUNDLE );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szGpi16QueryAttrs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYATTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryAttrs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBUNDLE );
         if ( lRC != GPI_ALTERROR ) T_LogPBUNDLE16( Parm4, Parm2 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szGpi16QueryAttrs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryAttrs()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYATTRS( HPS, LONG, ULONG, PBUNDLE );
PCHAR szGpi32QueryAttrs = "Gpi32QueryAttrs";

LONG APIENTRY Trc_Gpi32QueryAttrs( HPS     Parm1
                                 , LONG    Parm2
                                 , ULONG   Parm3
                                 , PBUNDLE Parm4
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBUNDLE );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szGpi32QueryAttrs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYATTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryAttrs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBUNDLE );
         if ( lRC != GPI_ALTERROR ) T_LogPBUNDLE( Parm4, Parm2 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szGpi32QueryAttrs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryBackColor()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYBACKCOLOR( HPS );
PCHAR szGpi16QueryBackColor = "Gpi16QueryBackColor";

LONG APIENTRY16 Trc_Gpi16QueryBackColor( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryBackColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryBackColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYBACKCOLOR( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryBackColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != CLR_ERROR );
      HOOKEXIT16( szGpi16QueryBackColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryBackColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBackColor()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYBACKCOLOR( HPS );
PCHAR szGpi32QueryBackColor = "Gpi32QueryBackColor";

LONG APIENTRY Trc_Gpi32QueryBackColor( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryBackColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryBackColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYBACKCOLOR( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryBackColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != CLR_ERROR );
      HOOKEXIT( szGpi32QueryBackColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryBackColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryBackMix()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYBACKMIX( HPS );
PCHAR szGpi16QueryBackMix = "Gpi16QueryBackMix";

LONG APIENTRY16 Trc_Gpi16QueryBackMix( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryBackMix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryBackMix, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYBACKMIX( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryBackMix );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != BM_ERROR );
      HOOKEXIT16( szGpi16QueryBackMix, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryBackMix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBackMix()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYBACKMIX( HPS );
PCHAR szGpi32QueryBackMix = "Gpi32QueryBackMix";

LONG APIENTRY Trc_Gpi32QueryBackMix( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryBackMix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryBackMix, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYBACKMIX( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryBackMix );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != BM_ERROR );
      HOOKEXIT( szGpi32QueryBackMix, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryBackMix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryBitmapBits()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYBITMAPBITS( HPS, LONG, LONG, PBYTE16, PBITMAPINFO216 );
PCHAR szGpi16QueryBitmapBits = "Gpi16QueryBitmapBits";

LONG APIENTRY16 Trc_Gpi16QueryBitmapBits( HPS            Parm1
                                        , LONG           Parm2
                                        , LONG           Parm3
                                        , PBYTE16        Parm4
                                        , PBITMAPINFO216 Parm5
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryBitmapBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_PBITMAPINFO2 );
         T_LogPBITMAPINFO216( Parm5 );
      }
      HOOKENTRY16( szGpi16QueryBitmapBits, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYBITMAPBITS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryBitmapBits );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         if ( lRC != GPI_ALTERROR ) T_LogG16QBB_Parm4( Parm4, Parm5, lRC );
         T_LogParameter( 5L, DT_PBITMAPINFO2 );
         if ( lRC != GPI_ALTERROR ) T_LogPBITMAPINFO216( Parm5 );
         else T_LogPVOID16( Parm5 );
      }
      HOOKEXIT16( szGpi16QueryBitmapBits, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryBitmapBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBitmapBits()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYBITMAPBITS( HPS, LONG, LONG, PBYTE, PBITMAPINFO2 );
PCHAR szGpi32QueryBitmapBits = "Gpi32QueryBitmapBits";

LONG APIENTRY Trc_Gpi32QueryBitmapBits( HPS          Parm1
                                      , LONG         Parm2
                                      , LONG         Parm3
                                      , PBYTE        Parm4
                                      , PBITMAPINFO2 Parm5
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryBitmapBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PBITMAPINFO2 );
         T_LogPBITMAPINFO2( Parm5 );
      }
      HOOKENTRY( szGpi32QueryBitmapBits, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYBITMAPBITS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryBitmapBits );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         if ( lRC != GPI_ALTERROR ) T_LogG32QBB_Parm4( Parm4, Parm5, lRC );
         T_LogParameter( 5L, DT_PBITMAPINFO2 );
         if ( lRC != GPI_ALTERROR ) T_LogPBITMAPINFO2( Parm5 );
         else T_LogPVOID( Parm5 );
      }
      HOOKEXIT( szGpi32QueryBitmapBits, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryBitmapBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryBitmapDimension()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYBITMAPDIMENSION( HBITMAP, PSIZEL16 );
PCHAR szGpi16QueryBitmapDimension = "Gpi16QueryBitmapDimension";

BOOL16 APIENTRY16 Trc_Gpi16QueryBitmapDimension( HBITMAP  Parm1
                                               , PSIZEL16 Parm2
                                               )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryBitmapDimension );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryBitmapDimension, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYBITMAPDIMENSION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryBitmapDimension );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEL );
         if ( bRC ) T_LogPSIZEL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryBitmapDimension, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryBitmapDimension() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBitmapDimension()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYBITMAPDIMENSION( HBITMAP, PSIZEL );
PCHAR szGpi32QueryBitmapDimension = "Gpi32QueryBitmapDimension";

BOOL APIENTRY Trc_Gpi32QueryBitmapDimension( HBITMAP Parm1
                                           , PSIZEL  Parm2
                                           )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryBitmapDimension );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryBitmapDimension, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYBITMAPDIMENSION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryBitmapDimension );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEL );
         if ( bRC ) T_LogPSIZEL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryBitmapDimension, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryBitmapDimension() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryBitmapHandle()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HBITMAP APIENTRY16 GPIQUERYBITMAPHANDLE( HPS, LONG );
PCHAR szGpi16QueryBitmapHandle = "Gpi16QueryBitmapHandle";

HBITMAP APIENTRY16 Trc_Gpi16QueryBitmapHandle( HPS  Parm1
                                             , LONG Parm2
                                             )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryBitmapHandle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryBitmapHandle, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPIQUERYBITMAPHANDLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryBitmapHandle );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT16( szGpi16QueryBitmapHandle, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16QueryBitmapHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBitmapHandle()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HBITMAP APIENTRY GPI32QUERYBITMAPHANDLE( HPS, LONG );
PCHAR szGpi32QueryBitmapHandle = "Gpi32QueryBitmapHandle";

HBITMAP APIENTRY Trc_Gpi32QueryBitmapHandle( HPS  Parm1
                                           , LONG Parm2
                                           )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryBitmapHandle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32QueryBitmapHandle, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32QUERYBITMAPHANDLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryBitmapHandle );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT( szGpi32QueryBitmapHandle, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryBitmapHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryBitmapInfoHeader()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYBITMAPINFOHEADER( HBITMAP, PBITMAPINFOHEADER216 );
PCHAR szGpi16QueryBitmapInfoHeader = "Gpi16QueryBitmapInfoHeader";

BOOL16 APIENTRY16 Trc_Gpi16QueryBitmapInfoHeader( HBITMAP              Parm1
                                                , PBITMAPINFOHEADER216 Parm2
                                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryBitmapInfoHeader );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER2 );
         T_LogPBITMAPINFOHEADER216( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryBitmapInfoHeader, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYBITMAPINFOHEADER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryBitmapInfoHeader );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER2 );
         if ( bRC ) T_LogPBITMAPINFOHEADER216( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryBitmapInfoHeader, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryBitmapInfoHeader() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBitmapInfoHeader()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYBITMAPINFOHEADER( HBITMAP, PBITMAPINFOHEADER2 );
PCHAR szGpi32QueryBitmapInfoHeader = "Gpi32QueryBitmapInfoHeader";

BOOL APIENTRY Trc_Gpi32QueryBitmapInfoHeader( HBITMAP            Parm1
                                            , PBITMAPINFOHEADER2 Parm2
                                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryBitmapInfoHeader );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER2 );
         T_LogPBITMAPINFOHEADER2( Parm2 );
      }
      HOOKENTRY( szGpi32QueryBitmapInfoHeader, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYBITMAPINFOHEADER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryBitmapInfoHeader );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER2 );
         if ( bRC ) T_LogPBITMAPINFOHEADER2( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryBitmapInfoHeader, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryBitmapInfoHeader() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryBitmapParameters()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYBITMAPPARAMETERS( HBITMAP, PBITMAPINFOHEADER16 );
PCHAR szGpi16QueryBitmapParameters = "Gpi16QueryBitmapParameters";

BOOL16 APIENTRY16 Trc_Gpi16QueryBitmapParameters( HBITMAP             Parm1
                                                , PBITMAPINFOHEADER16 Parm2
                                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryBitmapParameters );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER );
         T_LogPBITMAPINFOHEADER216( (PBITMAPINFOHEADER216)Parm2 );
      }
      HOOKENTRY16( szGpi16QueryBitmapParameters, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYBITMAPPARAMETERS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryBitmapParameters );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER );
         if ( bRC ) T_LogPBITMAPINFOHEADER216( (PBITMAPINFOHEADER216)Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryBitmapParameters, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryBitmapParameters() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBitmapParameters()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYBITMAPPARAMETERS( HBITMAP, PBITMAPINFOHEADER );
PCHAR szGpi32QueryBitmapParameters = "Gpi32QueryBitmapParameters";

BOOL APIENTRY Trc_Gpi32QueryBitmapParameters( HBITMAP           Parm1
                                            , PBITMAPINFOHEADER Parm2
                                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryBitmapParameters );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER );
         T_LogPBITMAPINFOHEADER2( (PBITMAPINFOHEADER2)Parm2 );
      }
      HOOKENTRY( szGpi32QueryBitmapParameters, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYBITMAPPARAMETERS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryBitmapParameters );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBITMAPINFOHEADER );
         if ( bRC ) T_LogPBITMAPINFOHEADER2( (PBITMAPINFOHEADER2)Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryBitmapParameters, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryBitmapParameters() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryBoundaryData()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYBOUNDARYDATA( HPS, PRECTL16 );
PCHAR szGpi16QueryBoundaryData = "Gpi16QueryBoundaryData";

BOOL16 APIENTRY16 Trc_Gpi16QueryBoundaryData( HPS      Parm1
                                            , PRECTL16 Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryBoundaryData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryBoundaryData, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYBOUNDARYDATA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryBoundaryData );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryBoundaryData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryBoundaryData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBoundaryData()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYBOUNDARYDATA( HPS, PRECTL );
PCHAR szGpi32QueryBoundaryData = "Gpi32QueryBoundaryData";

BOOL APIENTRY Trc_Gpi32QueryBoundaryData( HPS    Parm1
                                        , PRECTL Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryBoundaryData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryBoundaryData, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYBOUNDARYDATA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryBoundaryData );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryBoundaryData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryBoundaryData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharAngle()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCHARANGLE( HPS, PGRADIENTL16 );
PCHAR szGpi16QueryCharAngle = "Gpi16QueryCharAngle";

BOOL16 APIENTRY16 Trc_Gpi16QueryCharAngle( HPS          Parm1
                                         , PGRADIENTL16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharAngle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PGRADIENTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryCharAngle, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCHARANGLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharAngle );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PGRADIENTL );
         if ( bRC ) T_LogPGRADIENTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryCharAngle, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryCharAngle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharAngle()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCHARANGLE( HPS, PGRADIENTL );
PCHAR szGpi32QueryCharAngle = "Gpi32QueryCharAngle";

BOOL APIENTRY Trc_Gpi32QueryCharAngle( HPS        Parm1
                                     , PGRADIENTL Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharAngle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PGRADIENTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryCharAngle, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCHARANGLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharAngle );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PGRADIENTL );
         if ( bRC ) T_LogPGRADIENTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryCharAngle, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryCharAngle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharBox()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCHARBOX( HPS, PSIZEF16 );
PCHAR szGpi16QueryCharBox = "Gpi16QueryCharBox";

BOOL16 APIENTRY16 Trc_Gpi16QueryCharBox( HPS      Parm1
                                       , PSIZEF16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEF );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryCharBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCHARBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharBox );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEF );
         if ( bRC ) T_LogPSIZEF16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryCharBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryCharBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharBox()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCHARBOX( HPS, PSIZEF );
PCHAR szGpi32QueryCharBox = "Gpi32QueryCharBox";

BOOL APIENTRY Trc_Gpi32QueryCharBox( HPS    Parm1
                                   , PSIZEF Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEF );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryCharBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCHARBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharBox );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEF );
         if ( bRC ) T_LogPSIZEF( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryCharBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryCharBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharBreakExtra()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCHARBREAKEXTRA( HPS, PFIXED16 );
PCHAR szGpi16QueryCharBreakExtra = "Gpi16QueryCharBreakExtra";

BOOL16 APIENTRY16 Trc_Gpi16QueryCharBreakExtra( HPS      Parm1
                                              , PFIXED16 Parm2
                                              )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharBreakExtra );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFIXED );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryCharBreakExtra, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCHARBREAKEXTRA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharBreakExtra );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PFIXED );
         if ( bRC ) T_LogPFIXED16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryCharBreakExtra, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryCharBreakExtra() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharBreakExtra()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCHARBREAKEXTRA( HPS, PFIXED );
PCHAR szGpi32QueryCharBreakExtra = "Gpi32QueryCharBreakExtra";

BOOL APIENTRY Trc_Gpi32QueryCharBreakExtra( HPS    Parm1
                                          , PFIXED Parm2
                                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharBreakExtra );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFIXED );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryCharBreakExtra, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCHARBREAKEXTRA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharBreakExtra );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PFIXED );
         if ( bRC ) T_LogPFIXED( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryCharBreakExtra, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryCharBreakExtra() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharDirection()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYCHARDIRECTION( HPS );
PCHAR szGpi16QueryCharDirection = "Gpi16QueryCharDirection";

LONG APIENTRY16 Trc_Gpi16QueryCharDirection( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharDirection );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryCharDirection, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYCHARDIRECTION( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharDirection );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != CHDIRN_ERROR );
      HOOKEXIT16( szGpi16QueryCharDirection, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryCharDirection() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharDirection()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYCHARDIRECTION( HPS );
PCHAR szGpi32QueryCharDirection = "Gpi32QueryCharDirection";

LONG APIENTRY Trc_Gpi32QueryCharDirection( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharDirection );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryCharDirection, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYCHARDIRECTION( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharDirection );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != CHDIRN_ERROR );
      HOOKEXIT( szGpi32QueryCharDirection, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryCharDirection() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharExtra()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCHAREXTRA( HPS, PFIXED16 );
PCHAR szGpi16QueryCharExtra = "Gpi16QueryCharExtra";

BOOL16 APIENTRY16 Trc_Gpi16QueryCharExtra( HPS      Parm1
                                         , PFIXED16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharExtra );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFIXED );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryCharExtra, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCHAREXTRA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharExtra );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PFIXED );
         if ( bRC ) T_LogPFIXED16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryCharExtra, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryCharExtra() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharExtra()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCHAREXTRA( HPS, PFIXED );
PCHAR szGpi32QueryCharExtra = "Gpi32QueryCharExtra";

BOOL APIENTRY Trc_Gpi32QueryCharExtra( HPS    Parm1
                                     , PFIXED Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharExtra );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFIXED );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryCharExtra, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCHAREXTRA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharExtra );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PFIXED );
         if ( bRC ) T_LogPFIXED( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryCharExtra, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryCharExtra() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharMode()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYCHARMODE( HPS );
PCHAR szGpi16QueryCharMode = "Gpi16QueryCharMode";

LONG APIENTRY16 Trc_Gpi16QueryCharMode( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryCharMode, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYCHARMODE( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharMode );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != CM_ERROR );
      HOOKEXIT16( szGpi16QueryCharMode, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryCharMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharMode()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYCHARMODE( HPS );
PCHAR szGpi32QueryCharMode = "Gpi32QueryCharMode";

LONG APIENTRY Trc_Gpi32QueryCharMode( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryCharMode, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYCHARMODE( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharMode );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != CM_ERROR );
      HOOKEXIT( szGpi32QueryCharMode, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryCharMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharSet()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYCHARSET( HPS );
PCHAR szGpi16QueryCharSet = "Gpi16QueryCharSet";

LONG APIENTRY16 Trc_Gpi16QueryCharSet( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryCharSet, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYCHARSET( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharSet );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LCID_ERROR );
      HOOKEXIT16( szGpi16QueryCharSet, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryCharSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharSet()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYCHARSET( HPS );
PCHAR szGpi32QueryCharSet = "Gpi32QueryCharSet";

LONG APIENTRY Trc_Gpi32QueryCharSet( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryCharSet, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYCHARSET( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharSet );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LCID_ERROR );
      HOOKEXIT( szGpi32QueryCharSet, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryCharSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharShear()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCHARSHEAR( HPS, PPOINTL16 );
PCHAR szGpi16QueryCharShear = "Gpi16QueryCharShear";

BOOL16 APIENTRY16 Trc_Gpi16QueryCharShear( HPS       Parm1
                                         , PPOINTL16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharShear );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryCharShear, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCHARSHEAR( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharShear );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryCharShear, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryCharShear() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharShear()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCHARSHEAR( HPS, PPOINTL );
PCHAR szGpi32QueryCharShear = "Gpi32QueryCharShear";

BOOL APIENTRY Trc_Gpi32QueryCharShear( HPS     Parm1
                                     , PPOINTL Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharShear );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryCharShear, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCHARSHEAR( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharShear );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryCharShear, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryCharShear() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharStringPos()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCHARSTRINGPOS( HPS, ULONG, LONG, PCH16, PLONG16, PPOINTL16 );
PCHAR szGpi16QueryCharStringPos = "Gpi16QueryCharStringPos";

BOOL16 APIENTRY16 Trc_Gpi16QueryCharStringPos( HPS       Parm1
                                             , ULONG     Parm2
                                             , LONG      Parm3
                                             , PCH16     Parm4
                                             , PLONG16   Parm5
                                             , PPOINTL16 Parm6
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharStringPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PCH );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( Parm4, Parm3 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
         if ( FLAGSET( Parm2, CHS_VECTOR ) ) T_LogULONGBuffer16( (PULONG16)Parm5, Parm3 );
         T_LogParameter( 6L, DT_PPOINTL );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szGpi16QueryCharStringPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCHARSTRINGPOS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharStringPos );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 6L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer16( Parm6, Parm3 );
         else T_LogPVOID16( Parm6 );
      }
      HOOKEXIT16( szGpi16QueryCharStringPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryCharStringPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharStringPos()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCHARSTRINGPOS( HPS, ULONG, LONG, PCH, PLONG, PPOINTL );
PCHAR szGpi32QueryCharStringPos = "Gpi32QueryCharStringPos";

BOOL APIENTRY Trc_Gpi32QueryCharStringPos( HPS     Parm1
                                         , ULONG   Parm2
                                         , LONG    Parm3
                                         , PCH     Parm4
                                         , PLONG   Parm5
                                         , PPOINTL Parm6
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharStringPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PCH );
         T_LogPVOID( Parm4 );
         T_LogCHARBuffer( Parm4, Parm3 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
         if ( FLAGSET( Parm2, CHS_VECTOR ) ) T_LogULONGBuffer( (PULONG)Parm5, Parm3 );
         T_LogParameter( 6L, DT_PPOINTL );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szGpi32QueryCharStringPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCHARSTRINGPOS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharStringPos );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 6L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer( Parm6, Parm3 );
         else T_LogPVOID( Parm6 );
      }
      HOOKEXIT( szGpi32QueryCharStringPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryCharStringPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCharStringPosAt()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCHARSTRINGPOSAT( HPS, PPOINTL16, ULONG, LONG, PCH16, PLONG16, PPOINTL16 );
PCHAR szGpi16QueryCharStringPosAt = "Gpi16QueryCharStringPosAt";

BOOL16 APIENTRY16 Trc_Gpi16QueryCharStringPosAt( HPS       Parm1
                                               , PPOINTL16 Parm2
                                               , ULONG     Parm3
                                               , LONG      Parm4
                                               , PCH16     Parm5
                                               , PLONG16   Parm6
                                               , PPOINTL16 Parm7
                                               )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCharStringPosAt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PCH );
         T_LogPVOID16( Parm5 );
         T_LogCHARBuffer16( Parm5, Parm4 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID16( Parm6 );
         if ( FLAGSET( Parm3, CHS_VECTOR ) ) T_LogULONGBuffer16( (PULONG16)Parm6, Parm4 );
         T_LogParameter( 7L, DT_PPOINTL );
         T_LogPVOID16( Parm7 );
      }
      HOOKENTRY16( szGpi16QueryCharStringPosAt, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCHARSTRINGPOSAT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCharStringPosAt );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer16( Parm7, Parm4 );
         else T_LogPVOID16( Parm7 );
      }
      HOOKEXIT16( szGpi16QueryCharStringPosAt, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryCharStringPosAt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCharStringPosAt()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCHARSTRINGPOSAT( HPS, PPOINTL, ULONG, LONG, PCH, PLONG, PPOINTL );
PCHAR szGpi32QueryCharStringPosAt = "Gpi32QueryCharStringPosAt";

BOOL APIENTRY Trc_Gpi32QueryCharStringPosAt( HPS     Parm1
                                           , PPOINTL Parm2
                                           , ULONG   Parm3
                                           , LONG    Parm4
                                           , PCH     Parm5
                                           , PLONG   Parm6
                                           , PPOINTL Parm7
                                           )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCharStringPosAt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PCH );
         T_LogPVOID( Parm5 );
         T_LogCHARBuffer( Parm5, Parm4 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID( Parm6 );
         if ( FLAGSET( Parm3, CHS_VECTOR ) ) T_LogULONGBuffer( (PULONG)Parm6, Parm4 );
         T_LogParameter( 7L, DT_PPOINTL );
         T_LogPVOID( Parm7 );
      }
      HOOKENTRY( szGpi32QueryCharStringPosAt, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCHARSTRINGPOSAT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCharStringPosAt );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer( Parm7, Parm4 );
         else T_LogPVOID( Parm7 );
      }
      HOOKEXIT( szGpi32QueryCharStringPosAt, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryCharStringPosAt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryClipBox()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYCLIPBOX( HPS, PRECTL16 );
PCHAR szGpi16QueryClipBox = "Gpi16QueryClipBox";

LONG APIENTRY16 Trc_Gpi16QueryClipBox( HPS      Parm1
                                     , PRECTL16 Parm2
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryClipBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryClipBox, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYCLIPBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryClipBox );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( lRC != RGN_ERROR ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryClipBox, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryClipBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryClipBox()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYCLIPBOX( HPS, PRECTL );
PCHAR szGpi32QueryClipBox = "Gpi32QueryClipBox";

LONG APIENTRY Trc_Gpi32QueryClipBox( HPS    Parm1
                                   , PRECTL Parm2
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryClipBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryClipBox, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYCLIPBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryClipBox );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( lRC != RGN_ERROR ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryClipBox, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryClipBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryClipRegion()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HRGN APIENTRY16 GPIQUERYCLIPREGION( HPS );
PCHAR szGpi16QueryClipRegion = "Gpi16QueryClipRegion";

HRGN APIENTRY16 Trc_Gpi16QueryClipRegion( HPS Parm1 )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryClipRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryClipRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPIQUERYCLIPREGION( Parm1 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryClipRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != HRGN_ERROR );
      HOOKEXIT16( szGpi16QueryClipRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16QueryClipRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryClipRegion()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HRGN APIENTRY GPI32QUERYCLIPREGION( HPS );
PCHAR szGpi32QueryClipRegion = "Gpi32QueryClipRegion";

HRGN APIENTRY Trc_Gpi32QueryClipRegion( HPS Parm1 )
{

   HRGN ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryClipRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryClipRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32QUERYCLIPREGION( Parm1 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryClipRegion );
      T_LogPMReturnCode( DT_HRGN, ulRC, ulRC != HRGN_ERROR );
      HOOKEXIT( szGpi32QueryClipRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryClipRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryColor()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYCOLOR( HPS );
PCHAR szGpi16QueryColor = "Gpi16QueryColor";

LONG APIENTRY16 Trc_Gpi16QueryColor( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYCOLOR( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != CLR_ERROR );
      HOOKEXIT16( szGpi16QueryColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryColor()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYCOLOR( HPS );
PCHAR szGpi32QueryColor = "Gpi32QueryColor";

LONG APIENTRY Trc_Gpi32QueryColor( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYCOLOR( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != CLR_ERROR );
      HOOKEXIT( szGpi32QueryColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryColorData()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCOLORDATA( HPS, LONG, PLONG16 );
PCHAR szGpi16QueryColorData = "Gpi16QueryColorData";

BOOL16 APIENTRY16 Trc_Gpi16QueryColorData( HPS     Parm1
                                         , LONG    Parm2
                                         , PLONG16 Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryColorData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryColorData, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCOLORDATA( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryColorData );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID16( Parm3 );
         if ( bRC ) T_LogULONGBuffer16( (PULONG16)Parm3, Parm2 );
      }
      HOOKEXIT16( szGpi16QueryColorData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryColorData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryColorData()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCOLORDATA( HPS, LONG, PLONG );
PCHAR szGpi32QueryColorData = "Gpi32QueryColorData";

BOOL APIENTRY Trc_Gpi32QueryColorData( HPS   Parm1
                                     , LONG  Parm2
                                     , PLONG Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryColorData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryColorData, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCOLORDATA( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryColorData );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID( Parm3 );
         if ( bRC ) T_LogULONGBuffer( (PULONG)Parm3, Parm2 );
      }
      HOOKEXIT( szGpi32QueryColorData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryColorData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryColorIndex()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYCOLORINDEX( HPS, ULONG, LONG );
PCHAR szGpi16QueryColorIndex = "Gpi16QueryColorIndex";

LONG APIENTRY16 Trc_Gpi16QueryColorIndex( HPS   Parm1
                                        , ULONG Parm2
                                        , LONG  Parm3
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryColorIndex );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryColorIndex, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYCOLORINDEX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryColorIndex );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16QueryColorIndex, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryColorIndex() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryColorIndex()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYCOLORINDEX( HPS, ULONG, LONG );
PCHAR szGpi32QueryColorIndex = "Gpi32QueryColorIndex";

LONG APIENTRY Trc_Gpi32QueryColorIndex( HPS   Parm1
                                      , ULONG Parm2
                                      , LONG  Parm3
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryColorIndex );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32QueryColorIndex, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYCOLORINDEX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryColorIndex );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryColorIndex, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryColorIndex() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCp()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 GPIQUERYCP( HPS );
PCHAR szGpi16QueryCp = "Gpi16QueryCp";

USHORT APIENTRY16 Trc_Gpi16QueryCp( HPS Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryCp, &Parm1 );
      T_FreeLog( );
   }

   usRC = GPIQUERYCP( Parm1 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCp );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC != GPI_ERROR );
      HOOKEXIT16( szGpi16QueryCp, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Gpi16QueryCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCp()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY GPI32QUERYCP( HPS );
PCHAR szGpi32QueryCp = "Gpi32QueryCp";

ULONG APIENTRY Trc_Gpi32QueryCp( HPS Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryCp, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32QUERYCP( Parm1 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCp );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT( szGpi32QueryCp, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryCurrentPosition()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYCURRENTPOSITION( HPS, PPOINTL16 );
PCHAR szGpi16QueryCurrentPosition = "Gpi16QueryCurrentPosition";

BOOL16 APIENTRY16 Trc_Gpi16QueryCurrentPosition( HPS       Parm1
                                               , PPOINTL16 Parm2
                                               )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryCurrentPosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryCurrentPosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYCURRENTPOSITION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryCurrentPosition );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryCurrentPosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryCurrentPosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryCurrentPosition()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYCURRENTPOSITION( HPS, PPOINTL );
PCHAR szGpi32QueryCurrentPosition = "Gpi32QueryCurrentPosition";

BOOL APIENTRY Trc_Gpi32QueryCurrentPosition( HPS     Parm1
                                           , PPOINTL Parm2
                                           )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryCurrentPosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryCurrentPosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYCURRENTPOSITION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryCurrentPosition );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryCurrentPosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryCurrentPosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDefArcParams()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYDEFARCPARAMS( HPS, PARCPARAMS16 );
PCHAR szGpi16QueryDefArcParams = "Gpi16QueryDefArcParams";

BOOL16 APIENTRY16 Trc_Gpi16QueryDefArcParams( HPS          Parm1
                                            , PARCPARAMS16 Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDefArcParams );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PARCPARAMS );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryDefArcParams, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYDEFARCPARAMS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDefArcParams );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PARCPARAMS );
         if ( bRC ) T_LogPARCPARAMS16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryDefArcParams, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryDefArcParams() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDefArcParams()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYDEFARCPARAMS( HPS, PARCPARAMS );
PCHAR szGpi32QueryDefArcParams = "Gpi32QueryDefArcParams";

BOOL APIENTRY Trc_Gpi32QueryDefArcParams( HPS        Parm1
                                        , PARCPARAMS Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDefArcParams );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PARCPARAMS );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryDefArcParams, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYDEFARCPARAMS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDefArcParams );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PARCPARAMS );
         if ( bRC ) T_LogPARCPARAMS( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryDefArcParams, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryDefArcParams() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDefAttrs()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYDEFATTRS( HPS, LONG, ULONG, PBUNDLE16 );
PCHAR szGpi16QueryDefAttrs = "Gpi16QueryDefAttrs";

BOOL16 APIENTRY16 Trc_Gpi16QueryDefAttrs( HPS       Parm1
                                        , LONG      Parm2
                                        , ULONG     Parm3
                                        , PBUNDLE16 Parm4
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDefAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBUNDLE );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szGpi16QueryDefAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYDEFATTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDefAttrs );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBUNDLE );
         if ( bRC ) T_LogPBUNDLE16( Parm4, Parm2 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szGpi16QueryDefAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryDefAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDefAttrs()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYDEFATTRS( HPS, LONG, ULONG, PBUNDLE );
PCHAR szGpi32QueryDefAttrs = "Gpi32QueryDefAttrs";

BOOL APIENTRY Trc_Gpi32QueryDefAttrs( HPS     Parm1
                                    , LONG    Parm2
                                    , ULONG   Parm3
                                    , PBUNDLE Parm4
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDefAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBUNDLE );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szGpi32QueryDefAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYDEFATTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDefAttrs );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBUNDLE );
         if ( bRC ) T_LogPBUNDLE( Parm4, Parm2 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szGpi32QueryDefAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryDefAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDefaultViewMatrix()                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYDEFAULTVIEWMATRIX( HPS, LONG, PMATRIXLF16 );
PCHAR szGpi16QueryDefaultViewMatrix = "Gpi16QueryDefaultViewMatrix";

BOOL16 APIENTRY16 Trc_Gpi16QueryDefaultViewMatrix( HPS         Parm1
                                                 , LONG        Parm2
                                                 , PMATRIXLF16 Parm3
                                                 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDefaultViewMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryDefaultViewMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYDEFAULTVIEWMATRIX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDefaultViewMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF16( Parm3 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szGpi16QueryDefaultViewMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryDefaultViewMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDefaultViewMatrix()                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYDEFAULTVIEWMATRIX( HPS, LONG, PMATRIXLF );
PCHAR szGpi32QueryDefaultViewMatrix = "Gpi32QueryDefaultViewMatrix";

BOOL APIENTRY Trc_Gpi32QueryDefaultViewMatrix( HPS       Parm1
                                             , LONG      Parm2
                                             , PMATRIXLF Parm3
                                             )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDefaultViewMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryDefaultViewMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYDEFAULTVIEWMATRIX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDefaultViewMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF( Parm3 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szGpi32QueryDefaultViewMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryDefaultViewMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDefCharBox()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYDEFCHARBOX( HPS, PSIZEL16 );
PCHAR szGpi16QueryDefCharBox = "Gpi16QueryDefCharBox";

BOOL16 APIENTRY16 Trc_Gpi16QueryDefCharBox( HPS      Parm1
                                          , PSIZEL16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDefCharBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryDefCharBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYDEFCHARBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDefCharBox );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEL );
         if ( bRC ) T_LogPSIZEL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryDefCharBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryDefCharBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDefCharBox()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYDEFCHARBOX( HPS, PSIZEL );
PCHAR szGpi32QueryDefCharBox = "Gpi32QueryDefCharBox";

BOOL APIENTRY Trc_Gpi32QueryDefCharBox( HPS    Parm1
                                      , PSIZEL Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDefCharBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryDefCharBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYDEFCHARBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDefCharBox );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEL );
         if ( bRC ) T_LogPSIZEL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryDefCharBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryDefCharBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDefTag()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYDEFTAG( HPS, PLONG16 );
PCHAR szGpi16QueryDefTag = "Gpi16QueryDefTag";

BOOL16 APIENTRY16 Trc_Gpi16QueryDefTag( HPS     Parm1
                                      , PLONG16 Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDefTag );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryDefTag, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYDEFTAG( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDefTag );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm2 );
      }
      HOOKEXIT16( szGpi16QueryDefTag, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryDefTag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDefTag()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYDEFTAG( HPS, PLONG );
PCHAR szGpi32QueryDefTag = "Gpi32QueryDefTag";

BOOL APIENTRY Trc_Gpi32QueryDefTag( HPS   Parm1
                                  , PLONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDefTag );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryDefTag, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYDEFTAG( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDefTag );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm2 );
      }
      HOOKEXIT( szGpi32QueryDefTag, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryDefTag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDefViewingLimits()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYDEFVIEWINGLIMITS( HPS, PRECTL16 );
PCHAR szGpi16QueryDefViewingLimits = "Gpi16QueryDefViewingLimits";

BOOL16 APIENTRY16 Trc_Gpi16QueryDefViewingLimits( HPS      Parm1
                                                , PRECTL16 Parm2
                                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDefViewingLimits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryDefViewingLimits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYDEFVIEWINGLIMITS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDefViewingLimits );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryDefViewingLimits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryDefViewingLimits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDefViewingLimits()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYDEFVIEWINGLIMITS( HPS, PRECTL );
PCHAR szGpi32QueryDefViewingLimits = "Gpi32QueryDefViewingLimits";

BOOL APIENTRY Trc_Gpi32QueryDefViewingLimits( HPS    Parm1
                                            , PRECTL Parm2
                                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDefViewingLimits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryDefViewingLimits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYDEFVIEWINGLIMITS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDefViewingLimits );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryDefViewingLimits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryDefViewingLimits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDevice()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HDC APIENTRY16 GPIQUERYDEVICE( HPS );
PCHAR szGpi16QueryDevice = "Gpi16QueryDevice";

HDC APIENTRY16 Trc_Gpi16QueryDevice( HPS Parm1 )
{

   HDC ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDevice );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryDevice, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPIQUERYDEVICE( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDevice );
      T_LogPMReturnCode( DT_HDC, ulRC, ulRC != HDC_ERROR );
      HOOKEXIT16( szGpi16QueryDevice, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16QueryDevice() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDevice()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HDC APIENTRY GPI32QUERYDEVICE( HPS );
PCHAR szGpi32QueryDevice = "Gpi32QueryDevice";

HDC APIENTRY Trc_Gpi32QueryDevice( HPS Parm1 )
{

   HDC ulRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDevice );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryDevice, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32QUERYDEVICE( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDevice );
      T_LogPMReturnCode( DT_HDC, ulRC, ulRC != HDC_ERROR );
      HOOKEXIT( szGpi32QueryDevice, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryDevice() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDeviceBitmapFormats()                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYDEVICEBITMAPFORMATS( HPS, LONG, PLONG16 );
PCHAR szGpi16QueryDeviceBitmapFormats = "Gpi16QueryDeviceBitmapFormats";

BOOL16 APIENTRY16 Trc_Gpi16QueryDeviceBitmapFormats( HPS     Parm1
                                                   , LONG    Parm2
                                                   , PLONG16 Parm3
                                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDeviceBitmapFormats );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryDeviceBitmapFormats, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYDEVICEBITMAPFORMATS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDeviceBitmapFormats );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID16( Parm3 );
         if ( bRC ) T_LogULONGBuffer16( (PULONG16)Parm3, Parm2 );
      }
      HOOKEXIT16( szGpi16QueryDeviceBitmapFormats, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryDeviceBitmapFormats() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDeviceBitmapFormats()                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYDEVICEBITMAPFORMATS( HPS, LONG, PLONG );
PCHAR szGpi32QueryDeviceBitmapFormats = "Gpi32QueryDeviceBitmapFormats";

BOOL APIENTRY Trc_Gpi32QueryDeviceBitmapFormats( HPS   Parm1
                                               , LONG  Parm2
                                               , PLONG Parm3
                                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDeviceBitmapFormats );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryDeviceBitmapFormats, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYDEVICEBITMAPFORMATS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDeviceBitmapFormats );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID( Parm3 );
         if ( bRC ) T_LogULONGBuffer( (PULONG)Parm3, Parm2 );
      }
      HOOKEXIT( szGpi32QueryDeviceBitmapFormats, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryDeviceBitmapFormats() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDrawControl()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYDRAWCONTROL( HPS, LONG );
PCHAR szGpi16QueryDrawControl = "Gpi16QueryDrawControl";

LONG APIENTRY16 Trc_Gpi16QueryDrawControl( HPS  Parm1
                                         , LONG Parm2
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDrawControl );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryDrawControl, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYDRAWCONTROL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDrawControl );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != DCTL_ERROR );
      HOOKEXIT16( szGpi16QueryDrawControl, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryDrawControl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDrawControl()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYDRAWCONTROL( HPS, LONG );
PCHAR szGpi32QueryDrawControl = "Gpi32QueryDrawControl";

LONG APIENTRY Trc_Gpi32QueryDrawControl( HPS  Parm1
                                       , LONG Parm2
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDrawControl );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32QueryDrawControl, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYDRAWCONTROL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDrawControl );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != DCTL_ERROR );
      HOOKEXIT( szGpi32QueryDrawControl, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryDrawControl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryDrawingMode()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYDRAWINGMODE( HPS );
PCHAR szGpi16QueryDrawingMode = "Gpi16QueryDrawingMode";

LONG APIENTRY16 Trc_Gpi16QueryDrawingMode( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryDrawingMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryDrawingMode, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYDRAWINGMODE( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryDrawingMode );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != DM_ERROR );
      HOOKEXIT16( szGpi16QueryDrawingMode, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryDrawingMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryDrawingMode()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYDRAWINGMODE( HPS );
PCHAR szGpi32QueryDrawingMode = "Gpi32QueryDrawingMode";

LONG APIENTRY Trc_Gpi32QueryDrawingMode( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryDrawingMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryDrawingMode, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYDRAWINGMODE( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryDrawingMode );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != DM_ERROR );
      HOOKEXIT( szGpi32QueryDrawingMode, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryDrawingMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryEditMode()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYEDITMODE( HPS );
PCHAR szGpi16QueryEditMode = "Gpi16QueryEditMode";

LONG APIENTRY16 Trc_Gpi16QueryEditMode( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryEditMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryEditMode, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYEDITMODE( Parm1 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryEditMode );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != SEGEM_ERROR );
      HOOKEXIT16( szGpi16QueryEditMode, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryEditMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryEditMode()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYEDITMODE( HPS );
PCHAR szGpi32QueryEditMode = "Gpi32QueryEditMode";

LONG APIENTRY Trc_Gpi32QueryEditMode( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryEditMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryEditMode, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYEDITMODE( Parm1 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryEditMode );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != SEGEM_ERROR );
      HOOKEXIT( szGpi32QueryEditMode, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryEditMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryElement()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYELEMENT( HPS, LONG, LONG, PBYTE16 );
PCHAR szGpi16QueryElement = "Gpi16QueryElement";

LONG APIENTRY16 Trc_Gpi16QueryElement( HPS     Parm1
                                     , LONG    Parm2
                                     , LONG    Parm3
                                     , PBYTE16 Parm4
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryElement );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szGpi16QueryElement, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYELEMENT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryElement );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         if ( lRC != GPI_ALTERROR ) T_LogCHARBuffer16( (PCHAR16)Parm4, lRC );
      }
      HOOKEXIT16( szGpi16QueryElement, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryElement() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryElement()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYELEMENT( HPS, LONG, LONG, PBYTE );
PCHAR szGpi32QueryElement = "Gpi32QueryElement";

LONG APIENTRY Trc_Gpi32QueryElement( HPS   Parm1
                                   , LONG  Parm2
                                   , LONG  Parm3
                                   , PBYTE Parm4
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryElement );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szGpi32QueryElement, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYELEMENT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryElement );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         if ( lRC != GPI_ALTERROR ) T_LogCHARBuffer( (PCHAR)Parm4, lRC );
      }
      HOOKEXIT( szGpi32QueryElement, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryElement() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryElementPointer()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYELEMENTPOINTER( HPS );
PCHAR szGpi16QueryElementPointer = "Gpi16QueryElementPointer";

LONG APIENTRY16 Trc_Gpi16QueryElementPointer( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryElementPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryElementPointer, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYELEMENTPOINTER( Parm1 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryElementPointer );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16QueryElementPointer, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryElementPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryElementPointer()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYELEMENTPOINTER( HPS );
PCHAR szGpi32QueryElementPointer = "Gpi32QueryElementPointer";

LONG APIENTRY Trc_Gpi32QueryElementPointer( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryElementPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryElementPointer, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYELEMENTPOINTER( Parm1 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryElementPointer );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryElementPointer, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryElementPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryElementType()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYELEMENTTYPE( HPS, PLONG16, LONG, PSZ16 );
PCHAR szGpi16QueryElementType = "Gpi16QueryElementType";

LONG APIENTRY16 Trc_Gpi16QueryElementType( HPS     Parm1
                                         , PLONG16 Parm2
                                         , LONG    Parm3
                                         , PSZ16   Parm4
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryElementType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szGpi16QueryElementType, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYELEMENTTYPE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryElementType );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm2 );
         T_LogParameter( 4L, DT_PSZ );
         if ( lRC != GPI_ALTERROR ) T_LogPSZ16( Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szGpi16QueryElementType, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryElementType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryElementType()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYELEMENTTYPE( HPS, PLONG, LONG, PSZ );
PCHAR szGpi32QueryElementType = "Gpi32QueryElementType";

LONG APIENTRY Trc_Gpi32QueryElementType( HPS   Parm1
                                       , PLONG Parm2
                                       , LONG  Parm3
                                       , PSZ   Parm4
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryElementType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szGpi32QueryElementType, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYELEMENTTYPE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryElementType );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm2 );
         T_LogParameter( 4L, DT_PSZ );
         if ( lRC != GPI_ALTERROR ) T_LogPSZ( Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szGpi32QueryElementType, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryElementType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryFaceString()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 GPIQUERYFACESTRING( HPS, PSZ16, PFACENAMEDESC16, LONG, PSZ16 );
PCHAR szGpi16QueryFaceString = "Gpi16QueryFaceString";

ULONG APIENTRY16 Trc_Gpi16QueryFaceString( HPS             Parm1
                                         , PSZ16           Parm2
                                         , PFACENAMEDESC16 Parm3
                                         , LONG            Parm4
                                         , PSZ16           Parm5
                                         )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryFaceString );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PFACENAMEDESC );
         T_LogPFACENAMEDESC16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szGpi16QueryFaceString, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPIQUERYFACESTRING( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryFaceString );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != GPI_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PSZ );
         T_LogPVOID16( Parm5 );
         if ( ( ulRC != GPI_ERROR ) && ( Parm4 ) ) T_LogCHARBuffer16( Parm5, ulRC );
      }
      HOOKEXIT16( szGpi16QueryFaceString, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16QueryFaceString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryFaceString()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY GPI32QUERYFACESTRING( HPS, PSZ, PFACENAMEDESC, LONG, PSZ );
PCHAR szGpi32QueryFaceString = "Gpi32QueryFaceString";

ULONG APIENTRY Trc_Gpi32QueryFaceString( HPS           Parm1
                                       , PSZ           Parm2
                                       , PFACENAMEDESC Parm3
                                       , LONG          Parm4
                                       , PSZ           Parm5
                                       )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryFaceString );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PFACENAMEDESC );
         T_LogPFACENAMEDESC( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szGpi32QueryFaceString, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32QUERYFACESTRING( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryFaceString );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != GPI_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PSZ );
         T_LogPVOID( Parm5 );
         if ( ( ulRC != GPI_ERROR ) && ( Parm4 ) ) T_LogCHARBuffer( Parm5, ulRC );
      }
      HOOKEXIT( szGpi32QueryFaceString, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryFaceString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryFontAction()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 GPIQUERYFONTACTION( HAB, ULONG );
PCHAR szGpi16QueryFontAction = "Gpi16QueryFontAction";

ULONG APIENTRY16 Trc_Gpi16QueryFontAction( HAB   Parm1
                                         , ULONG Parm2
                                         )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryFontAction );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryFontAction, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPIQUERYFONTACTION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryFontAction );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != QFA_ERROR );
      HOOKEXIT16( szGpi16QueryFontAction, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16QueryFontAction() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryFontAction()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY GPI32QUERYFONTACTION( HAB, ULONG );
PCHAR szGpi32QueryFontAction = "Gpi32QueryFontAction";

ULONG APIENTRY Trc_Gpi32QueryFontAction( HAB   Parm1
                                       , ULONG Parm2
                                       )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryFontAction );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32QueryFontAction, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32QUERYFONTACTION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryFontAction );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != QFA_ERROR );
      HOOKEXIT( szGpi32QueryFontAction, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryFontAction() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryFontFileDescriptions()                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYFONTFILEDESCRIPTIONS( HAB, PSZ16, PLONG16, PFFDESCS16 );
PCHAR szGpi16QueryFontFileDescriptions = "Gpi16QueryFontFileDescriptions";

LONG APIENTRY16 Trc_Gpi16QueryFontFileDescriptions( HAB        Parm1
                                                  , PSZ16      Parm2
                                                  , PLONG16    Parm3
                                                  , PFFDESCS16 Parm4
                                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryFontFileDescriptions );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
         T_LogParameter( 4L, DT_PFFDESCS );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szGpi16QueryFontFileDescriptions, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYFONTFILEDESCRIPTIONS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryFontFileDescriptions );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm3 );
         T_LogParameter( 4L, DT_PFFDESCS );
         if ( ( Parm3 ) && ( lRC != GPI_ALTERROR ) ) T_LogFFDESCSBuffer16( Parm4, *Parm3 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szGpi16QueryFontFileDescriptions, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryFontFileDescriptions() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryFontFileDescriptions()                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYFONTFILEDESCRIPTIONS( HAB, PSZ, PLONG, PFFDESCS );
PCHAR szGpi32QueryFontFileDescriptions = "Gpi32QueryFontFileDescriptions";

LONG APIENTRY Trc_Gpi32QueryFontFileDescriptions( HAB      Parm1
                                                , PSZ      Parm2
                                                , PLONG    Parm3
                                                , PFFDESCS Parm4
                                                )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryFontFileDescriptions );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_PFFDESCS );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szGpi32QueryFontFileDescriptions, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYFONTFILEDESCRIPTIONS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryFontFileDescriptions );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_PFFDESCS );
         if ( ( Parm3 ) && ( lRC != GPI_ALTERROR ) ) T_LogFFDESCSBuffer( Parm4, *Parm3 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szGpi32QueryFontFileDescriptions, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryFontFileDescriptions() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryFontMappingFlags()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY GPI32QUERYFONTMAPPINGFLAGS( HPS );
PCHAR szGpi32QueryFontMappingFlags = "Gpi32QueryFontMappingFlags";

ULONG APIENTRY Trc_Gpi32QueryFontMappingFlags( HPS Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryFontMappingFlags );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryFontMappingFlags, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnGPI32QUERYFONTMAPPINGFLAGS )( Parm1 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryFontMappingFlags );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryFontMappingFlags, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryFontMappingFlags() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryFontMetrics()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYFONTMETRICS( HPS, LONG, PFONTMETRICS16 );
PCHAR szGpi16QueryFontMetrics = "Gpi16QueryFontMetrics";

BOOL16 APIENTRY16 Trc_Gpi16QueryFontMetrics( HPS            Parm1
                                           , LONG           Parm2
                                           , PFONTMETRICS16 Parm3
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryFontMetrics );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PFONTMETRICS );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryFontMetrics, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYFONTMETRICS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryFontMetrics );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PFONTMETRICS );
         if ( bRC ) T_LogPFONTMETRICS16( Parm3, Parm2 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szGpi16QueryFontMetrics, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryFontMetrics() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryFontMetrics()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYFONTMETRICS( HPS, LONG, PFONTMETRICS );
PCHAR szGpi32QueryFontMetrics = "Gpi32QueryFontMetrics";

BOOL APIENTRY Trc_Gpi32QueryFontMetrics( HPS          Parm1
                                       , LONG         Parm2
                                       , PFONTMETRICS Parm3
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryFontMetrics );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PFONTMETRICS );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryFontMetrics, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYFONTMETRICS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryFontMetrics );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PFONTMETRICS );
         if ( bRC ) T_LogPFONTMETRICS( Parm3, Parm2 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szGpi32QueryFontMetrics, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryFontMetrics() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryFonts()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYFONTS( HPS, ULONG, PSZ16, PLONG16, LONG, PFONTMETRICS16 );
PCHAR szGpi16QueryFonts = "Gpi16QueryFonts";

LONG APIENTRY16 Trc_Gpi16QueryFonts( HPS            Parm1
                                   , ULONG          Parm2
                                   , PSZ16          Parm3
                                   , PLONG16        Parm4
                                   , LONG           Parm5
                                   , PFONTMETRICS16 Parm6
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PFONTMETRICS );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szGpi16QueryFonts, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYFONTS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryFonts );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm4 );
         T_LogParameter( 6L, DT_PFONTMETRICS );
         if ( ( Parm4 ) && ( lRC != GPI_ALTERROR ) ) T_LogFONTMETRICSBuffer16( Parm6, Parm5, *Parm4 );
         else T_LogPVOID16( Parm6 );
      }
      HOOKEXIT16( szGpi16QueryFonts, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryFonts()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYFONTS( HPS, ULONG, PSZ, PLONG, LONG, PFONTMETRICS );
PCHAR szGpi32QueryFonts = "Gpi32QueryFonts";

LONG APIENTRY Trc_Gpi32QueryFonts( HPS          Parm1
                                 , ULONG        Parm2
                                 , PSZ          Parm3
                                 , PLONG        Parm4
                                 , LONG         Parm5
                                 , PFONTMETRICS Parm6
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PFONTMETRICS );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szGpi32QueryFonts, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYFONTS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryFonts );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm4 );
         T_LogParameter( 6L, DT_PFONTMETRICS );
         if ( ( Parm4 ) && ( lRC != GPI_ALTERROR ) ) T_LogFONTMETRICSBuffer( Parm6, Parm5, *Parm4 );
         else T_LogPVOID( Parm6 );
      }
      HOOKEXIT( szGpi32QueryFonts, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryFullFontFileDescs()                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYFULLFONTFILEDESCS( HAB, PSZ, PLONG, PVOID, PLONG );
PCHAR szGpi32QueryFullFontFileDescs = "Gpi32QueryFullFontFileDescs";

LONG APIENTRY Trc_Gpi32QueryFullFontFileDescs( HAB   Parm1
                                             , PSZ   Parm2
                                             , PLONG Parm3
                                             , PVOID Parm4
                                             , PLONG Parm5
                                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryFullFontFileDescs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm5 );
      }
      HOOKENTRY( szGpi32QueryFullFontFileDescs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYFULLFONTFILEDESCS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryFullFontFileDescs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         if ( ( Parm3 ) && ( lRC != GPI_ALTERROR ) ) T_LogFFDESCS2Buffer( Parm4, *Parm3 );
         else T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm5 );
      }
      HOOKEXIT( szGpi32QueryFullFontFileDescs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryFullFontFileDescs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryGraphicsField()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYGRAPHICSFIELD( HPS, PRECTL16 );
PCHAR szGpi16QueryGraphicsField = "Gpi16QueryGraphicsField";

BOOL16 APIENTRY16 Trc_Gpi16QueryGraphicsField( HPS      Parm1
                                             , PRECTL16 Parm2
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryGraphicsField );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryGraphicsField, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYGRAPHICSFIELD( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryGraphicsField );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryGraphicsField, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryGraphicsField() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryGraphicsField()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYGRAPHICSFIELD( HPS, PRECTL );
PCHAR szGpi32QueryGraphicsField = "Gpi32QueryGraphicsField";

BOOL APIENTRY Trc_Gpi32QueryGraphicsField( HPS    Parm1
                                         , PRECTL Parm2
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryGraphicsField );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryGraphicsField, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYGRAPHICSFIELD( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryGraphicsField );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryGraphicsField, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryGraphicsField() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryInitialSegmentAttrs()                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYINITIALSEGMENTATTRS( HPS, LONG );
PCHAR szGpi16QueryInitialSegmentAttrs = "Gpi16QueryInitialSegmentAttrs";

LONG APIENTRY16 Trc_Gpi16QueryInitialSegmentAttrs( HPS  Parm1
                                                 , LONG Parm2
                                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryInitialSegmentAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryInitialSegmentAttrs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYINITIALSEGMENTATTRS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryInitialSegmentAttrs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != ATTR_ERROR );
      HOOKEXIT16( szGpi16QueryInitialSegmentAttrs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryInitialSegmentAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryInitialSegmentAttrs()                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYINITIALSEGMENTATTRS( HPS, LONG );
PCHAR szGpi32QueryInitialSegmentAttrs = "Gpi32QueryInitialSegmentAttrs";

LONG APIENTRY Trc_Gpi32QueryInitialSegmentAttrs( HPS  Parm1
                                               , LONG Parm2
                                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryInitialSegmentAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32QueryInitialSegmentAttrs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYINITIALSEGMENTATTRS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryInitialSegmentAttrs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != ATTR_ERROR );
      HOOKEXIT( szGpi32QueryInitialSegmentAttrs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryInitialSegmentAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryKerningPairs()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYKERNINGPAIRS( HPS, LONG, PKERNINGPAIRS16 );
PCHAR szGpi16QueryKerningPairs = "Gpi16QueryKerningPairs";

LONG APIENTRY16 Trc_Gpi16QueryKerningPairs( HPS             Parm1
                                          , LONG            Parm2
                                          , PKERNINGPAIRS16 Parm3
                                          )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryKerningPairs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PKERNINGPAIRS );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryKerningPairs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYKERNINGPAIRS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryKerningPairs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PKERNINGPAIRS );
         if ( lRC != GPI_ALTERROR ) T_LogKERNINGPAIRSBuffer16( Parm3, lRC );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szGpi16QueryKerningPairs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryKerningPairs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryKerningPairs()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYKERNINGPAIRS( HPS, LONG, PKERNINGPAIRS );
PCHAR szGpi32QueryKerningPairs = "Gpi32QueryKerningPairs";

LONG APIENTRY Trc_Gpi32QueryKerningPairs( HPS           Parm1
                                        , LONG          Parm2
                                        , PKERNINGPAIRS Parm3
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryKerningPairs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PKERNINGPAIRS );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryKerningPairs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYKERNINGPAIRS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryKerningPairs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PKERNINGPAIRS );
         if ( lRC != GPI_ALTERROR ) T_LogKERNINGPAIRSBuffer( Parm3, lRC );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szGpi32QueryKerningPairs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryKerningPairs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryLineEnd()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYLINEEND( HPS );
PCHAR szGpi16QueryLineEnd = "Gpi16QueryLineEnd";

LONG APIENTRY16 Trc_Gpi16QueryLineEnd( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryLineEnd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryLineEnd, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYLINEEND( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryLineEnd );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LINEEND_ERROR );
      HOOKEXIT16( szGpi16QueryLineEnd, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryLineEnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryLineEnd()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYLINEEND( HPS );
PCHAR szGpi32QueryLineEnd = "Gpi32QueryLineEnd";

LONG APIENTRY Trc_Gpi32QueryLineEnd( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryLineEnd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryLineEnd, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYLINEEND( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryLineEnd );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LINEEND_ERROR );
      HOOKEXIT( szGpi32QueryLineEnd, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryLineEnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryLineJoin()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYLINEJOIN( HPS );
PCHAR szGpi16QueryLineJoin = "Gpi16QueryLineJoin";

LONG APIENTRY16 Trc_Gpi16QueryLineJoin( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryLineJoin );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryLineJoin, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYLINEJOIN( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryLineJoin );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LINEJOIN_ERROR );
      HOOKEXIT16( szGpi16QueryLineJoin, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryLineJoin() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryLineJoin()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYLINEJOIN( HPS );
PCHAR szGpi32QueryLineJoin = "Gpi32QueryLineJoin";

LONG APIENTRY Trc_Gpi32QueryLineJoin( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryLineJoin );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryLineJoin, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYLINEJOIN( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryLineJoin );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LINEJOIN_ERROR );
      HOOKEXIT( szGpi32QueryLineJoin, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryLineJoin() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryLineType()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYLINETYPE( HPS );
PCHAR szGpi16QueryLineType = "Gpi16QueryLineType";

LONG APIENTRY16 Trc_Gpi16QueryLineType( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryLineType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryLineType, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYLINETYPE( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryLineType );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LINETYPE_ERROR );
      HOOKEXIT16( szGpi16QueryLineType, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryLineType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryLineType()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYLINETYPE( HPS );
PCHAR szGpi32QueryLineType = "Gpi32QueryLineType";

LONG APIENTRY Trc_Gpi32QueryLineType( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryLineType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryLineType, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYLINETYPE( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryLineType );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LINETYPE_ERROR );
      HOOKEXIT( szGpi32QueryLineType, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryLineType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryLineWidth()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
FIXED APIENTRY16 GPIQUERYLINEWIDTH( HPS );
PCHAR szGpi16QueryLineWidth = "Gpi16QueryLineWidth";

FIXED APIENTRY16 Trc_Gpi16QueryLineWidth( HPS Parm1 )
{

   FIXED lRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryLineWidth );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryLineWidth, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYLINEWIDTH( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryLineWidth );
      T_LogPMReturnCode( DT_FIXED, lRC, lRC != LINEWIDTH_ERROR );
      HOOKEXIT16( szGpi16QueryLineWidth, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryLineWidth() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryLineWidth()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
FIXED APIENTRY GPI32QUERYLINEWIDTH( HPS );
PCHAR szGpi32QueryLineWidth = "Gpi32QueryLineWidth";

FIXED APIENTRY Trc_Gpi32QueryLineWidth( HPS Parm1 )
{

   FIXED lRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryLineWidth );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryLineWidth, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYLINEWIDTH( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryLineWidth );
      T_LogPMReturnCode( DT_FIXED, lRC, lRC != LINEWIDTH_ERROR );
      HOOKEXIT( szGpi32QueryLineWidth, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryLineWidth() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryLineWidthGeom()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYLINEWIDTHGEOM( HPS );
PCHAR szGpi16QueryLineWidthGeom = "Gpi16QueryLineWidthGeom";

LONG APIENTRY16 Trc_Gpi16QueryLineWidthGeom( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryLineWidthGeom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryLineWidthGeom, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYLINEWIDTHGEOM( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryLineWidthGeom );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LINEWIDTHGEOM_ERROR );
      HOOKEXIT16( szGpi16QueryLineWidthGeom, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryLineWidthGeom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryLineWidthGeom()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYLINEWIDTHGEOM( HPS );
PCHAR szGpi32QueryLineWidthGeom = "Gpi32QueryLineWidthGeom";

LONG APIENTRY Trc_Gpi32QueryLineWidthGeom( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryLineWidthGeom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryLineWidthGeom, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYLINEWIDTHGEOM( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryLineWidthGeom );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LINEWIDTHGEOM_ERROR );
      HOOKEXIT( szGpi32QueryLineWidthGeom, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryLineWidthGeom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryLogColorTable()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYLOGCOLORTABLE( HPS, ULONG, LONG, LONG, PLONG16 );
PCHAR szGpi16QueryLogColorTable = "Gpi16QueryLogColorTable";

LONG APIENTRY16 Trc_Gpi16QueryLogColorTable( HPS     Parm1
                                           , ULONG   Parm2
                                           , LONG    Parm3
                                           , LONG    Parm4
                                           , PLONG16 Parm5
                                           )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryLogColorTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szGpi16QueryLogColorTable, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYLOGCOLORTABLE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryLogColorTable );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != QLCT_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
         if ( ( lRC != QLCT_ERROR ) && ( lRC != QLCT_RGB ) ) T_LogULONGBuffer16( (PULONG16)Parm5, lRC );
      }
      HOOKEXIT16( szGpi16QueryLogColorTable, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryLogColorTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryLogColorTable()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYLOGCOLORTABLE( HPS, ULONG, LONG, LONG, PLONG );
PCHAR szGpi32QueryLogColorTable = "Gpi32QueryLogColorTable";

LONG APIENTRY Trc_Gpi32QueryLogColorTable( HPS   Parm1
                                         , ULONG Parm2
                                         , LONG  Parm3
                                         , LONG  Parm4
                                         , PLONG Parm5
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryLogColorTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szGpi32QueryLogColorTable, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYLOGCOLORTABLE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryLogColorTable );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != QLCT_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
         if ( ( lRC != QLCT_ERROR ) && ( lRC != QLCT_RGB ) ) T_LogULONGBuffer( (PULONG)Parm5, lRC );
      }
      HOOKEXIT( szGpi32QueryLogColorTable, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryLogColorTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryLogicalFont()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYLOGICALFONT( HPS, LONG, PSTR816, PFATTRS16, LONG );
PCHAR szGpi16QueryLogicalFont = "Gpi16QueryLogicalFont";

BOOL16 APIENTRY16 Trc_Gpi16QueryLogicalFont( HPS       Parm1
                                           , LONG      Parm2
                                           , PSTR816   Parm3
                                           , PFATTRS16 Parm4
                                           , LONG      Parm5
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryLogicalFont );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSTR8 );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_PFATTRS );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szGpi16QueryLogicalFont, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYLOGICALFONT( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryLogicalFont );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSTR8 );
         T_LogPVOID16( Parm3 );
         if ( bRC ) T_LogCHARBuffer16( (PCHAR16)Parm3, sizeof( STR8 ) );
         T_LogParameter( 4L, DT_PFATTRS );
         if ( bRC ) T_LogPFATTRS16( Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szGpi16QueryLogicalFont, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryLogicalFont() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryLogicalFont()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYLOGICALFONT( HPS, LONG, PSTR8, PFATTRS, LONG );
PCHAR szGpi32QueryLogicalFont = "Gpi32QueryLogicalFont";

BOOL APIENTRY Trc_Gpi32QueryLogicalFont( HPS     Parm1
                                       , LONG    Parm2
                                       , PSTR8   Parm3
                                       , PFATTRS Parm4
                                       , LONG    Parm5
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryLogicalFont );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSTR8 );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_PFATTRS );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32QueryLogicalFont, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYLOGICALFONT( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryLogicalFont );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSTR8 );
         T_LogPVOID( Parm3 );
         if ( bRC ) T_LogCHARBuffer( (PCHAR)Parm3, sizeof( STR8 ) );
         T_LogParameter( 4L, DT_PFATTRS );
         if ( bRC ) T_LogPFATTRS( Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szGpi32QueryLogicalFont, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryLogicalFont() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryMarker()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYMARKER( HPS );
PCHAR szGpi16QueryMarker = "Gpi16QueryMarker";

LONG APIENTRY16 Trc_Gpi16QueryMarker( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryMarker );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryMarker, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYMARKER( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryMarker );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != MARKSYM_ERROR );
      HOOKEXIT16( szGpi16QueryMarker, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryMarker() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryMarker()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYMARKER( HPS );
PCHAR szGpi32QueryMarker = "Gpi32QueryMarker";

LONG APIENTRY Trc_Gpi32QueryMarker( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryMarker );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryMarker, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYMARKER( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryMarker );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != MARKSYM_ERROR );
      HOOKEXIT( szGpi32QueryMarker, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryMarker() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryMarkerBox()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYMARKERBOX( HPS, PSIZEF16 );
PCHAR szGpi16QueryMarkerBox = "Gpi16QueryMarkerBox";

BOOL16 APIENTRY16 Trc_Gpi16QueryMarkerBox( HPS      Parm1
                                         , PSIZEF16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryMarkerBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEF );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryMarkerBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYMARKERBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryMarkerBox );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEF );
         if ( bRC ) T_LogPSIZEF16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryMarkerBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryMarkerBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryMarkerBox()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYMARKERBOX( HPS, PSIZEF );
PCHAR szGpi32QueryMarkerBox = "Gpi32QueryMarkerBox";

BOOL APIENTRY Trc_Gpi32QueryMarkerBox( HPS    Parm1
                                     , PSIZEF Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryMarkerBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEF );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryMarkerBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYMARKERBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryMarkerBox );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEF );
         if ( bRC ) T_LogPSIZEF( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryMarkerBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryMarkerBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryMarkerSet()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYMARKERSET( HPS );
PCHAR szGpi16QueryMarkerSet = "Gpi16QueryMarkerSet";

LONG APIENTRY16 Trc_Gpi16QueryMarkerSet( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryMarkerSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryMarkerSet, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYMARKERSET( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryMarkerSet );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LCID_ERROR );
      HOOKEXIT16( szGpi16QueryMarkerSet, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryMarkerSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryMarkerSet()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYMARKERSET( HPS );
PCHAR szGpi32QueryMarkerSet = "Gpi32QueryMarkerSet";

LONG APIENTRY Trc_Gpi32QueryMarkerSet( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryMarkerSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryMarkerSet, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYMARKERSET( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryMarkerSet );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LCID_ERROR );
      HOOKEXIT( szGpi32QueryMarkerSet, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryMarkerSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryMetaFileBits()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYMETAFILEBITS( HMF, LONG, LONG, PBYTE16 );
PCHAR szGpi16QueryMetaFileBits = "Gpi16QueryMetaFileBits";

BOOL16 APIENTRY16 Trc_Gpi16QueryMetaFileBits( HMF     Parm1
                                            , LONG    Parm2
                                            , LONG    Parm3
                                            , PBYTE16 Parm4
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryMetaFileBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szGpi16QueryMetaFileBits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYMETAFILEBITS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryMetaFileBits );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         if ( bRC ) T_LogCHARBuffer16( (PCHAR16)Parm4, Parm3 );
      }
      HOOKEXIT16( szGpi16QueryMetaFileBits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryMetaFileBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryMetaFileBits()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYMETAFILEBITS( HMF, LONG, LONG, PBYTE );
PCHAR szGpi32QueryMetaFileBits = "Gpi32QueryMetaFileBits";

BOOL APIENTRY Trc_Gpi32QueryMetaFileBits( HMF   Parm1
                                        , LONG  Parm2
                                        , LONG  Parm3
                                        , PBYTE Parm4
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryMetaFileBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szGpi32QueryMetaFileBits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYMETAFILEBITS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryMetaFileBits );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         if ( bRC ) T_LogCHARBuffer( (PCHAR)Parm4, Parm3 );
      }
      HOOKEXIT( szGpi32QueryMetaFileBits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryMetaFileBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryMetaFileLength()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYMETAFILELENGTH( HMF );
PCHAR szGpi16QueryMetaFileLength = "Gpi16QueryMetaFileLength";

LONG APIENTRY16 Trc_Gpi16QueryMetaFileLength( HMF Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryMetaFileLength );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryMetaFileLength, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYMETAFILELENGTH( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryMetaFileLength );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16QueryMetaFileLength, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryMetaFileLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryMetaFileLength()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYMETAFILELENGTH( HMF );
PCHAR szGpi32QueryMetaFileLength = "Gpi32QueryMetaFileLength";

LONG APIENTRY Trc_Gpi32QueryMetaFileLength( HMF Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryMetaFileLength );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryMetaFileLength, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYMETAFILELENGTH( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryMetaFileLength );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryMetaFileLength, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryMetaFileLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryMix()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYMIX( HPS );
PCHAR szGpi16QueryMix = "Gpi16QueryMix";

LONG APIENTRY16 Trc_Gpi16QueryMix( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryMix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryMix, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYMIX( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryMix );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != FM_ERROR );
      HOOKEXIT16( szGpi16QueryMix, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryMix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryMix()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYMIX( HPS );
PCHAR szGpi32QueryMix = "Gpi32QueryMix";

LONG APIENTRY Trc_Gpi32QueryMix( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryMix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryMix, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYMIX( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryMix );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != FM_ERROR );
      HOOKEXIT( szGpi32QueryMix, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryMix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryModelTransformMatrix()                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYMODELTRANSFORMMATRIX( HPS, LONG, PMATRIXLF16 );
PCHAR szGpi16QueryModelTransformMatrix = "Gpi16QueryModelTransformMatrix";

BOOL16 APIENTRY16 Trc_Gpi16QueryModelTransformMatrix( HPS         Parm1
                                                    , LONG        Parm2
                                                    , PMATRIXLF16 Parm3
                                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryModelTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryModelTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYMODELTRANSFORMMATRIX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryModelTransformMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF16( Parm3 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szGpi16QueryModelTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryModelTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryModelTransformMatrix()                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYMODELTRANSFORMMATRIX( HPS, LONG, PMATRIXLF );
PCHAR szGpi32QueryModelTransformMatrix = "Gpi32QueryModelTransformMatrix";

BOOL APIENTRY Trc_Gpi32QueryModelTransformMatrix( HPS       Parm1
                                                , LONG      Parm2
                                                , PMATRIXLF Parm3
                                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryModelTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryModelTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYMODELTRANSFORMMATRIX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryModelTransformMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF( Parm3 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szGpi32QueryModelTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryModelTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryNearestColor()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYNEARESTCOLOR( HPS, ULONG, LONG );
PCHAR szGpi16QueryNearestColor = "Gpi16QueryNearestColor";

LONG APIENTRY16 Trc_Gpi16QueryNearestColor( HPS   Parm1
                                          , ULONG Parm2
                                          , LONG  Parm3
                                          )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryNearestColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryNearestColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYNEARESTCOLOR( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryNearestColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16QueryNearestColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryNearestColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryNearestColor()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYNEARESTCOLOR( HPS, ULONG, LONG );
PCHAR szGpi32QueryNearestColor = "Gpi32QueryNearestColor";

LONG APIENTRY Trc_Gpi32QueryNearestColor( HPS   Parm1
                                        , ULONG Parm2
                                        , LONG  Parm3
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryNearestColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32QueryNearestColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYNEARESTCOLOR( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryNearestColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryNearestColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryNearestColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryNearestPaletteIndex()                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG APIENTRY GPI32QUERYNEARESTPALETTEINDEX( HPAL, ULONG );
PCHAR szGpi32QueryNearestPaletteIndex = "Gpi32QueryNearestPaletteIndex";

LONG APIENTRY Trc_Gpi32QueryNearestPaletteIndex( HPAL  Parm1
                                               , ULONG Parm2
                                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryNearestPaletteIndex );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32QueryNearestPaletteIndex, &Parm1 );
      T_FreeLog( );
   }

   lRC = ( *pfnGPI32QUERYNEARESTPALETTEINDEX )( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryNearestPaletteIndex );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryNearestPaletteIndex, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryNearestPaletteIndex() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryNumberSetIds()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYNUMBERSETIDS( HPS );
PCHAR szGpi16QueryNumberSetIds = "Gpi16QueryNumberSetIds";

LONG APIENTRY16 Trc_Gpi16QueryNumberSetIds( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryNumberSetIds );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryNumberSetIds, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYNUMBERSETIDS( Parm1 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryNumberSetIds );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16QueryNumberSetIds, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryNumberSetIds() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryNumberSetIds()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYNUMBERSETIDS( HPS );
PCHAR szGpi32QueryNumberSetIds = "Gpi32QueryNumberSetIds";

LONG APIENTRY Trc_Gpi32QueryNumberSetIds( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryNumberSetIds );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryNumberSetIds, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYNUMBERSETIDS( Parm1 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryNumberSetIds );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryNumberSetIds, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryNumberSetIds() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPageViewport()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYPAGEVIEWPORT( HPS, PRECTL16 );
PCHAR szGpi16QueryPageViewport = "Gpi16QueryPageViewport";

BOOL16 APIENTRY16 Trc_Gpi16QueryPageViewport( HPS      Parm1
                                            , PRECTL16 Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPageViewport );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryPageViewport, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYPAGEVIEWPORT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPageViewport );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryPageViewport, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryPageViewport() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPageViewport()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYPAGEVIEWPORT( HPS, PRECTL );
PCHAR szGpi32QueryPageViewport = "Gpi32QueryPageViewport";

BOOL APIENTRY Trc_Gpi32QueryPageViewport( HPS    Parm1
                                        , PRECTL Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPageViewport );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryPageViewport, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYPAGEVIEWPORT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPageViewport );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryPageViewport, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryPageViewport() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPalette()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPAL APIENTRY16 GPIQUERYPALETTE( HPS );
PCHAR szGpi16QueryPalette = "Gpi16QueryPalette";

HPAL APIENTRY16 Trc_Gpi16QueryPalette( HPS Parm1 )
{

   HPAL ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryPalette, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPIQUERYPALETTE( Parm1 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPalette );
      T_LogPMReturnCode( DT_HPAL, ulRC, ulRC != PAL_ERROR );
      HOOKEXIT16( szGpi16QueryPalette, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16QueryPalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPalette()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPAL APIENTRY GPI32QUERYPALETTE( HPS );
PCHAR szGpi32QueryPalette = "Gpi32QueryPalette";

HPAL APIENTRY Trc_Gpi32QueryPalette( HPS Parm1 )
{

   HPAL ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryPalette, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32QUERYPALETTE( Parm1 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPalette );
      T_LogPMReturnCode( DT_HPAL, ulRC, ulRC != PAL_ERROR );
      HOOKEXIT( szGpi32QueryPalette, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryPalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPaletteInfo()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYPALETTEINFO( HPAL, HPS, ULONG, ULONG, ULONG, PULONG16 );
PCHAR szGpi16QueryPaletteInfo = "Gpi16QueryPaletteInfo";

LONG APIENTRY16 Trc_Gpi16QueryPaletteInfo( HPAL     Parm1
                                         , HPS      Parm2
                                         , ULONG    Parm3
                                         , ULONG    Parm4
                                         , ULONG    Parm5
                                         , PULONG16 Parm6
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPaletteInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szGpi16QueryPaletteInfo, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYPALETTEINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPaletteInfo );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PAL_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID16( Parm6 );
         if ( lRC != PAL_ERROR ) T_LogULONGBuffer16( Parm6, lRC );
      }
      HOOKEXIT16( szGpi16QueryPaletteInfo, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryPaletteInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPaletteInfo()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYPALETTEINFO( HPAL, HPS, ULONG, ULONG, ULONG, PULONG );
PCHAR szGpi32QueryPaletteInfo = "Gpi32QueryPaletteInfo";

LONG APIENTRY Trc_Gpi32QueryPaletteInfo( HPAL   Parm1
                                       , HPS    Parm2
                                       , ULONG  Parm3
                                       , ULONG  Parm4
                                       , ULONG  Parm5
                                       , PULONG Parm6
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPaletteInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szGpi32QueryPaletteInfo, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYPALETTEINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPaletteInfo );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PAL_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 6L, DT_PULONG );
         T_LogPVOID( Parm6 );
         if ( lRC != PAL_ERROR ) T_LogULONGBuffer( Parm6, lRC );
      }
      HOOKEXIT( szGpi32QueryPaletteInfo, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryPaletteInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPattern()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYPATTERN( HPS );
PCHAR szGpi16QueryPattern = "Gpi16QueryPattern";

LONG APIENTRY16 Trc_Gpi16QueryPattern( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPattern );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryPattern, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYPATTERN( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPattern );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PATSYM_ERROR );
      HOOKEXIT16( szGpi16QueryPattern, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryPattern() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPattern()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYPATTERN( HPS );
PCHAR szGpi32QueryPattern = "Gpi32QueryPattern";

LONG APIENTRY Trc_Gpi32QueryPattern( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPattern );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryPattern, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYPATTERN( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPattern );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PATSYM_ERROR );
      HOOKEXIT( szGpi32QueryPattern, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryPattern() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPatternRefPoint()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYPATTERNREFPOINT( HPS, PPOINTL16 );
PCHAR szGpi16QueryPatternRefPoint = "Gpi16QueryPatternRefPoint";

BOOL16 APIENTRY16 Trc_Gpi16QueryPatternRefPoint( HPS       Parm1
                                               , PPOINTL16 Parm2
                                               )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPatternRefPoint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryPatternRefPoint, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYPATTERNREFPOINT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPatternRefPoint );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryPatternRefPoint, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryPatternRefPoint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPatternRefPoint()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYPATTERNREFPOINT( HPS, PPOINTL );
PCHAR szGpi32QueryPatternRefPoint = "Gpi32QueryPatternRefPoint";

BOOL APIENTRY Trc_Gpi32QueryPatternRefPoint( HPS     Parm1
                                           , PPOINTL Parm2
                                           )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPatternRefPoint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryPatternRefPoint, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYPATTERNREFPOINT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPatternRefPoint );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryPatternRefPoint, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryPatternRefPoint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPatternSet()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYPATTERNSET( HPS );
PCHAR szGpi16QueryPatternSet = "Gpi16QueryPatternSet";

LONG APIENTRY16 Trc_Gpi16QueryPatternSet( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPatternSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryPatternSet, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYPATTERNSET( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPatternSet );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LCID_ERROR );
      HOOKEXIT16( szGpi16QueryPatternSet, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryPatternSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPatternSet()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYPATTERNSET( HPS );
PCHAR szGpi32QueryPatternSet = "Gpi32QueryPatternSet";

LONG APIENTRY Trc_Gpi32QueryPatternSet( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPatternSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryPatternSet, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYPATTERNSET( Parm1 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPatternSet );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != LCID_ERROR );
      HOOKEXIT( szGpi32QueryPatternSet, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryPatternSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPel()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYPEL( HPS, PPOINTL16 );
PCHAR szGpi16QueryPel = "Gpi16QueryPel";

LONG APIENTRY16 Trc_Gpi16QueryPel( HPS       Parm1
                                 , PPOINTL16 Parm2
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryPel, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYPEL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPel );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16QueryPel, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryPel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPel()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYPEL( HPS, PPOINTL );
PCHAR szGpi32QueryPel = "Gpi32QueryPel";

LONG APIENTRY Trc_Gpi32QueryPel( HPS     Parm1
                               , PPOINTL Parm2
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32QueryPel, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYPEL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPel );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryPel, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryPel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPickAperturePosition()                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYPICKAPERTUREPOSITION( HPS, PPOINTL16 );
PCHAR szGpi16QueryPickAperturePosition = "Gpi16QueryPickAperturePosition";

BOOL16 APIENTRY16 Trc_Gpi16QueryPickAperturePosition( HPS       Parm1
                                                    , PPOINTL16 Parm2
                                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPickAperturePosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryPickAperturePosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYPICKAPERTUREPOSITION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPickAperturePosition );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryPickAperturePosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryPickAperturePosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPickAperturePosition()                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYPICKAPERTUREPOSITION( HPS, PPOINTL );
PCHAR szGpi32QueryPickAperturePosition = "Gpi32QueryPickAperturePosition";

BOOL APIENTRY Trc_Gpi32QueryPickAperturePosition( HPS     Parm1
                                                , PPOINTL Parm2
                                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPickAperturePosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryPickAperturePosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYPICKAPERTUREPOSITION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPickAperturePosition );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryPickAperturePosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryPickAperturePosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPickApertureSize()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYPICKAPERTURESIZE( HPS, PSIZEL16 );
PCHAR szGpi16QueryPickApertureSize = "Gpi16QueryPickApertureSize";

BOOL16 APIENTRY16 Trc_Gpi16QueryPickApertureSize( HPS      Parm1
                                                , PSIZEL16 Parm2
                                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPickApertureSize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryPickApertureSize, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYPICKAPERTURESIZE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPickApertureSize );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEL );
         if ( bRC ) T_LogPSIZEL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryPickApertureSize, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryPickApertureSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPickApertureSize()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYPICKAPERTURESIZE( HPS, PSIZEL );
PCHAR szGpi32QueryPickApertureSize = "Gpi32QueryPickApertureSize";

BOOL APIENTRY Trc_Gpi32QueryPickApertureSize( HPS    Parm1
                                            , PSIZEL Parm2
                                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPickApertureSize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryPickApertureSize, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYPICKAPERTURESIZE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPickApertureSize );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEL );
         if ( bRC ) T_LogPSIZEL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryPickApertureSize, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryPickApertureSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryPS()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 GPIQUERYPS( HPS, PSIZEL16 );
PCHAR szGpi16QueryPS = "Gpi16QueryPS";

ULONG APIENTRY16 Trc_Gpi16QueryPS( HPS      Parm1
                                 , PSIZEL16 Parm2
                                 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryPS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPIQUERYPS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryPS );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEL );
         if ( ulRC ) T_LogPSIZEL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryPS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16QueryPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryPS()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY GPI32QUERYPS( HPS, PSIZEL );
PCHAR szGpi32QueryPS = "Gpi32QueryPS";

ULONG APIENTRY Trc_Gpi32QueryPS( HPS    Parm1
                               , PSIZEL Parm2
                               )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryPS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32QUERYPS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryPS );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSIZEL );
         if ( ulRC ) T_LogPSIZEL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryPS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32QueryPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryRasterizerCaps()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
BOOL APIENTRY GPI32QUERYRASTERIZERCAPS( PRASTERIZERCAPS );
PCHAR szGpi32QueryRasterizerCaps = "Gpi32QueryRasterizerCaps";

BOOL APIENTRY Trc_Gpi32QueryRasterizerCaps( PRASTERIZERCAPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryRasterizerCaps );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRASTERIZERCAPS );
         T_LogPVOID( Parm1 );
      }
      HOOKENTRY( szGpi32QueryRasterizerCaps, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnGPI32QUERYRASTERIZERCAPS )( Parm1 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryRasterizerCaps );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PRASTERIZERCAPS );
         if ( bRC ) T_LogPRASTERIZERCAPS( Parm1 );
         else T_LogPVOID( Parm1 );
      }
      HOOKEXIT( szGpi32QueryRasterizerCaps, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryRasterizerCaps() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryRealColors                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYREALCOLORS( HPS, ULONG, LONG, LONG, PLONG16 );
PCHAR szGpi16QueryRealColors = "Gpi16QueryRealColors";

LONG APIENTRY16 Trc_Gpi16QueryRealColors( HPS     Parm1
                                        , ULONG   Parm2
                                        , LONG    Parm3
                                        , LONG    Parm4
                                        , PLONG16 Parm5
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryRealColors );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szGpi16QueryRealColors, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYREALCOLORS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryRealColors );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer16( (PULONG16)Parm5, lRC );
      }
      HOOKEXIT16( szGpi16QueryRealColors, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryRealColors() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryRealColors                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYREALCOLORS( HPS, ULONG, LONG, LONG, PLONG );
PCHAR szGpi32QueryRealColors = "Gpi32QueryRealColors";

LONG APIENTRY Trc_Gpi32QueryRealColors( HPS   Parm1
                                      , ULONG Parm2
                                      , LONG  Parm3
                                      , LONG  Parm4
                                      , PLONG Parm5
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryRealColors );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szGpi32QueryRealColors, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYREALCOLORS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryRealColors );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer( (PULONG)Parm5, lRC );
      }
      HOOKEXIT( szGpi32QueryRealColors, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryRealColors() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryRegionBox()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYREGIONBOX( HPS, HRGN, PRECTL16 );
PCHAR szGpi16QueryRegionBox = "Gpi16QueryRegionBox";

LONG APIENTRY16 Trc_Gpi16QueryRegionBox( HPS      Parm1
                                       , HRGN     Parm2
                                       , PRECTL16 Parm3
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryRegionBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryRegionBox, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYREGIONBOX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryRegionBox );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PRECTL );
         if ( lRC != RGN_ERROR ) T_LogPRECTL16( Parm3 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szGpi16QueryRegionBox, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryRegionBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryRegionBox()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYREGIONBOX( HPS, HRGN, PRECTL );
PCHAR szGpi32QueryRegionBox = "Gpi32QueryRegionBox";

LONG APIENTRY Trc_Gpi32QueryRegionBox( HPS    Parm1
                                     , HRGN   Parm2
                                     , PRECTL Parm3
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryRegionBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryRegionBox, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYREGIONBOX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryRegionBox );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PRECTL );
         if ( lRC != RGN_ERROR ) T_LogPRECTL( Parm3 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szGpi32QueryRegionBox, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryRegionBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryRegionRects()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYREGIONRECTS( HPS, HRGN, PRECTL16, PRGNRECT16, PRECTL16 );
PCHAR szGpi16QueryRegionRects = "Gpi16QueryRegionRects";

BOOL16 APIENTRY16 Trc_Gpi16QueryRegionRects( HPS        Parm1
                                           , HRGN       Parm2
                                           , PRECTL16   Parm3
                                           , PRGNRECT16 Parm4
                                           , PRECTL16   Parm5
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryRegionRects );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
         T_LogParameter( 4L, DT_PRGNRECT );
         T_LogPRGNRECT16( Parm4 );
         T_LogParameter( 5L, DT_PRECTL );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szGpi16QueryRegionRects, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYREGIONRECTS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryRegionRects );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PRGNRECT );
         if ( bRC ) T_LogPRGNRECT16( Parm4 );
         else T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_PRECTL );
         if ( bRC ) T_LogRECTLBuffer16( Parm5, Parm4->crcReturned );
         else T_LogPVOID16( Parm5 );
      }
      HOOKEXIT16( szGpi16QueryRegionRects, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryRegionRects() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryRegionRects()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYREGIONRECTS( HPS, HRGN, PRECTL, PRGNRECT, PRECTL );
PCHAR szGpi32QueryRegionRects = "Gpi32QueryRegionRects";

BOOL APIENTRY Trc_Gpi32QueryRegionRects( HPS      Parm1
                                       , HRGN     Parm2
                                       , PRECTL   Parm3
                                       , PRGNRECT Parm4
                                       , PRECTL   Parm5
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryRegionRects );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
         T_LogParameter( 4L, DT_PRGNRECT );
         T_LogPRGNRECT( Parm4 );
         T_LogParameter( 5L, DT_PRECTL );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szGpi32QueryRegionRects, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYREGIONRECTS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryRegionRects );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PRGNRECT );
         if ( bRC ) T_LogPRGNRECT( Parm4 );
         else T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PRECTL );
         if ( bRC ) T_LogRECTLBuffer( Parm5, Parm4->crcReturned );
         else T_LogPVOID( Parm5 );
      }
      HOOKEXIT( szGpi32QueryRegionRects, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryRegionRects() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryRGBColor()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYRGBCOLOR( HPS, ULONG, LONG );
PCHAR szGpi16QueryRGBColor = "Gpi16QueryRGBColor";

LONG APIENTRY16 Trc_Gpi16QueryRGBColor( HPS   Parm1
                                      , ULONG Parm2
                                      , LONG  Parm3
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryRGBColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryRGBColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYRGBCOLOR( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryRGBColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16QueryRGBColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryRGBColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryRGBColor()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYRGBCOLOR( HPS, ULONG, LONG );
PCHAR szGpi32QueryRGBColor = "Gpi32QueryRGBColor";

LONG APIENTRY Trc_Gpi32QueryRGBColor( HPS   Parm1
                                    , ULONG Parm2
                                    , LONG  Parm3
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryRGBColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32QueryRGBColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYRGBCOLOR( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryRGBColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QueryRGBColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryRGBColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QuerySegmentAttrs()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYSEGMENTATTRS( HPS, LONG, LONG );
PCHAR szGpi16QuerySegmentAttrs = "Gpi16QuerySegmentAttrs";

LONG APIENTRY16 Trc_Gpi16QuerySegmentAttrs( HPS  Parm1
                                          , LONG Parm2
                                          , LONG Parm3
                                          )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QuerySegmentAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16QuerySegmentAttrs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYSEGMENTATTRS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QuerySegmentAttrs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != ATTR_ERROR );
      HOOKEXIT16( szGpi16QuerySegmentAttrs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QuerySegmentAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QuerySegmentAttrs()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYSEGMENTATTRS( HPS, LONG, LONG );
PCHAR szGpi32QuerySegmentAttrs = "Gpi32QuerySegmentAttrs";

LONG APIENTRY Trc_Gpi32QuerySegmentAttrs( HPS  Parm1
                                        , LONG Parm2
                                        , LONG Parm3
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QuerySegmentAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32QuerySegmentAttrs, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYSEGMENTATTRS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QuerySegmentAttrs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != ATTR_ERROR );
      HOOKEXIT( szGpi32QuerySegmentAttrs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QuerySegmentAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QuerySegmentNames()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYSEGMENTNAMES( HPS, LONG, LONG, LONG, PLONG16 );
PCHAR szGpi16QuerySegmentNames = "Gpi16QuerySegmentNames";

LONG APIENTRY16 Trc_Gpi16QuerySegmentNames( HPS     Parm1
                                          , LONG    Parm2
                                          , LONG    Parm3
                                          , LONG    Parm4
                                          , PLONG16 Parm5
                                          )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QuerySegmentNames );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szGpi16QuerySegmentNames, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYSEGMENTNAMES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QuerySegmentNames );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer16( (PULONG16)Parm5, lRC );
      }
      HOOKEXIT16( szGpi16QuerySegmentNames, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QuerySegmentNames() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QuerySegmentNames()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYSEGMENTNAMES( HPS, LONG, LONG, LONG, PLONG );
PCHAR szGpi32QuerySegmentNames = "Gpi32QuerySegmentNames";

LONG APIENTRY Trc_Gpi32QuerySegmentNames( HPS   Parm1
                                        , LONG  Parm2
                                        , LONG  Parm3
                                        , LONG  Parm4
                                        , PLONG Parm5
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QuerySegmentNames );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szGpi32QuerySegmentNames, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYSEGMENTNAMES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QuerySegmentNames );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
         if ( lRC != GPI_ALTERROR ) T_LogULONGBuffer( (PULONG)Parm5, lRC );
      }
      HOOKEXIT( szGpi32QuerySegmentNames, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QuerySegmentNames() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QuerySegmentPriority()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYSEGMENTPRIORITY( HPS, LONG, LONG );
PCHAR szGpi16QuerySegmentPriority = "Gpi16QuerySegmentPriority";

LONG APIENTRY16 Trc_Gpi16QuerySegmentPriority( HPS  Parm1
                                             , LONG Parm2
                                             , LONG Parm3
                                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QuerySegmentPriority );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16QuerySegmentPriority, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYSEGMENTPRIORITY( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QuerySegmentPriority );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16QuerySegmentPriority, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QuerySegmentPriority() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QuerySegmentPriority()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYSEGMENTPRIORITY( HPS, LONG, LONG );
PCHAR szGpi32QuerySegmentPriority = "Gpi32QuerySegmentPriority";

LONG APIENTRY Trc_Gpi32QuerySegmentPriority( HPS  Parm1
                                           , LONG Parm2
                                           , LONG Parm3
                                           )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QuerySegmentPriority );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32QuerySegmentPriority, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYSEGMENTPRIORITY( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QuerySegmentPriority );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32QuerySegmentPriority, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QuerySegmentPriority() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QuerySegmentTransformMatrix()                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYSEGMENTTRANSFORMMATRIX( HPS, LONG, LONG, PMATRIXLF16 );
PCHAR szGpi16QuerySegmentTransformMatrix = "Gpi16QuerySegmentTransformMatrix";

BOOL16 APIENTRY16 Trc_Gpi16QuerySegmentTransformMatrix( HPS         Parm1
                                                      , LONG        Parm2
                                                      , LONG        Parm3
                                                      , PMATRIXLF16 Parm4
                                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QuerySegmentTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PMATRIXLF );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szGpi16QuerySegmentTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYSEGMENTTRANSFORMMATRIX( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QuerySegmentTransformMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF16( Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szGpi16QuerySegmentTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QuerySegmentTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QuerySegmentTransformMatrix()                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYSEGMENTTRANSFORMMATRIX( HPS, LONG, LONG, PMATRIXLF );
PCHAR szGpi32QuerySegmentTransformMatrix = "Gpi32QuerySegmentTransformMatrix";

BOOL APIENTRY Trc_Gpi32QuerySegmentTransformMatrix( HPS       Parm1
                                                  , LONG      Parm2
                                                  , LONG      Parm3
                                                  , PMATRIXLF Parm4
                                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QuerySegmentTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PMATRIXLF );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szGpi32QuerySegmentTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYSEGMENTTRANSFORMMATRIX( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QuerySegmentTransformMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF( Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szGpi32QuerySegmentTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QuerySegmentTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QuerySetIds()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYSETIDS( HPS, LONG, PLONG16, PSTR816, PLONG16 );
PCHAR szGpi16QuerySetIds = "Gpi16QuerySetIds";

BOOL16 APIENTRY16 Trc_Gpi16QuerySetIds( HPS     Parm1
                                      , LONG    Parm2
                                      , PLONG16 Parm3
                                      , PSTR816 Parm4
                                      , PLONG16 Parm5
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QuerySetIds );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_PSTR8 );
         T_LogPVOID16( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szGpi16QuerySetIds, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYSETIDS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QuerySetIds );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID16( Parm3 );
         if ( bRC ) T_LogULONGBuffer16( (PULONG16)Parm3, Parm2 );
         T_LogParameter( 4L, DT_PSTR8 );
         T_LogPVOID16( Parm4 );
         if ( bRC ) T_LogCHARBuffer16( (PCHAR16)Parm4, Parm2 * sizeof( STR8 ) );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID16( Parm5 );
         if ( bRC ) T_LogULONGBuffer16( (PULONG16)Parm5, Parm2 );
      }
      HOOKEXIT16( szGpi16QuerySetIds, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QuerySetIds() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QuerySetIds()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYSETIDS( HPS, LONG, PLONG, PSTR8, PLONG );
PCHAR szGpi32QuerySetIds = "Gpi32QuerySetIds";

BOOL APIENTRY Trc_Gpi32QuerySetIds( HPS   Parm1
                                  , LONG  Parm2
                                  , PLONG Parm3
                                  , PSTR8 Parm4
                                  , PLONG Parm5
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QuerySetIds );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_PSTR8 );
         T_LogPVOID( Parm4 );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szGpi32QuerySetIds, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYSETIDS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QuerySetIds );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID( Parm3 );
         if ( bRC ) T_LogULONGBuffer( (PULONG)Parm3, Parm2 );
         T_LogParameter( 4L, DT_PSTR8 );
         T_LogPVOID( Parm4 );
         if ( bRC ) T_LogCHARBuffer( (PCHAR)Parm4, Parm2 * sizeof( STR8 ) );
         T_LogParameter( 5L, DT_PLONG );
         T_LogPVOID( Parm5 );
         if ( bRC ) T_LogULONGBuffer( (PULONG)Parm5, Parm2 );
      }
      HOOKEXIT( szGpi32QuerySetIds, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QuerySetIds() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryStopDraw()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIQUERYSTOPDRAW( HPS );
PCHAR szGpi16QueryStopDraw = "Gpi16QueryStopDraw";

LONG APIENTRY16 Trc_Gpi16QueryStopDraw( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryStopDraw );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16QueryStopDraw, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIQUERYSTOPDRAW( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryStopDraw );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != SDW_ERROR );
      HOOKEXIT16( szGpi16QueryStopDraw, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16QueryStopDraw() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryStopDraw()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32QUERYSTOPDRAW( HPS );
PCHAR szGpi32QueryStopDraw = "Gpi32QueryStopDraw";

LONG APIENTRY Trc_Gpi32QueryStopDraw( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryStopDraw );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32QueryStopDraw, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32QUERYSTOPDRAW( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryStopDraw );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != SDW_ERROR );
      HOOKEXIT( szGpi32QueryStopDraw, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryStopDraw() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryTabbedTextExtent()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG APIENTRY GPI32QUERYTABBEDTEXTEXTENT( HPS, LONG, PCH, LONG, PULONG );
PCHAR szGpi32QueryTabbedTextExtent = "Gpi32QueryTabbedTextExtent";

LONG APIENTRY Trc_Gpi32QueryTabbedTextExtent( HPS    Parm1
                                            , LONG   Parm2
                                            , PCH    Parm3
                                            , LONG   Parm4
                                            , PULONG Parm5
                                            )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryTabbedTextExtent );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogCHARBuffer( Parm3, Parm2 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogULONGBuffer( Parm5, Parm4 );
      }
      HOOKENTRY( szGpi32QueryTabbedTextExtent, &Parm1 );
      T_FreeLog( );
   }

   lRC = ( *pfnGPI32QUERYTABBEDTEXTEXTENT )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryTabbedTextExtent );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32QueryTabbedTextExtent, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32QueryTabbedTextExtent() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryTag()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYTAG( HPS, PLONG16 );
PCHAR szGpi16QueryTag = "Gpi16QueryTag";

BOOL16 APIENTRY16 Trc_Gpi16QueryTag( HPS     Parm1
                                   , PLONG16 Parm2
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryTag );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryTag, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYTAG( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryTag );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG16( (PULONG16)Parm2 );
      }
      HOOKEXIT16( szGpi16QueryTag, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryTag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryTag()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYTAG( HPS, PLONG );
PCHAR szGpi32QueryTag = "Gpi32QueryTag";

BOOL APIENTRY Trc_Gpi32QueryTag( HPS   Parm1
                               , PLONG Parm2
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryTag );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryTag, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYTAG( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryTag );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm2 );
      }
      HOOKEXIT( szGpi32QueryTag, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryTag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryTextAlignment()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYTEXTALIGNMENT( HPS, PLONG, PLONG );
PCHAR szGpi32QueryTextAlignment = "Gpi32QueryTextAlignment";

BOOL APIENTRY Trc_Gpi32QueryTextAlignment( HPS   Parm1
                                         , PLONG Parm2
                                         , PLONG Parm3
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryTextAlignment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PLONG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryTextAlignment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYTEXTALIGNMENT( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryTextAlignment );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm2 );
         T_LogParameter( 3L, DT_PLONG );
         T_LogPULONG( (PULONG)Parm3 );
      }
      HOOKEXIT( szGpi32QueryTextAlignment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryTextAlignment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryTextBox()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYTEXTBOX( HPS, LONG, PCH16, LONG, PPOINTL16 );
PCHAR szGpi16QueryTextBox = "Gpi16QueryTextBox";

BOOL16 APIENTRY16 Trc_Gpi16QueryTextBox( HPS       Parm1
                                       , LONG      Parm2
                                       , PCH16     Parm3
                                       , LONG      Parm4
                                       , PPOINTL16 Parm5
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryTextBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID16( Parm3 );
         T_LogCHARBuffer16( Parm3, Parm2 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szGpi16QueryTextBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYTEXTBOX( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryTextBox );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer16( Parm5, Parm4 );
         else T_LogPVOID16( Parm5 );
      }
      HOOKEXIT16( szGpi16QueryTextBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryTextBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryTextBox()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYTEXTBOX( HPS, LONG, PCH, LONG, PPOINTL );
PCHAR szGpi32QueryTextBox = "Gpi32QueryTextBox";

BOOL APIENTRY Trc_Gpi32QueryTextBox( HPS     Parm1
                                   , LONG    Parm2
                                   , PCH     Parm3
                                   , LONG    Parm4
                                   , PPOINTL Parm5
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryTextBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID( Parm3 );
         T_LogCHARBuffer( Parm3, Parm2 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szGpi32QueryTextBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYTEXTBOX( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryTextBox );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer( Parm5, Parm4 );
         else T_LogPVOID( Parm5 );
      }
      HOOKEXIT( szGpi32QueryTextBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryTextBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryViewingLimits()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYVIEWINGLIMITS( HPS, PRECTL16 );
PCHAR szGpi16QueryViewingLimits = "Gpi16QueryViewingLimits";

BOOL16 APIENTRY16 Trc_Gpi16QueryViewingLimits( HPS      Parm1
                                             , PRECTL16 Parm2
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryViewingLimits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szGpi16QueryViewingLimits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYVIEWINGLIMITS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryViewingLimits );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16QueryViewingLimits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryViewingLimits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryViewingLimits()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYVIEWINGLIMITS( HPS, PRECTL );
PCHAR szGpi32QueryViewingLimits = "Gpi32QueryViewingLimits";

BOOL APIENTRY Trc_Gpi32QueryViewingLimits( HPS    Parm1
                                         , PRECTL Parm2
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryViewingLimits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szGpi32QueryViewingLimits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYVIEWINGLIMITS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryViewingLimits );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32QueryViewingLimits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryViewingLimits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryViewingTransformMatrix()                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYVIEWINGTRANSFORMMATRIX( HPS, LONG, PMATRIXLF16 );
PCHAR szGpi16QueryViewingTransformMatrix = "Gpi16QueryViewingTransformMatrix";

BOOL16 APIENTRY16 Trc_Gpi16QueryViewingTransformMatrix( HPS         Parm1
                                                      , LONG        Parm2
                                                      , PMATRIXLF16 Parm3
                                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryViewingTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16QueryViewingTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYVIEWINGTRANSFORMMATRIX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryViewingTransformMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF16( Parm3 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szGpi16QueryViewingTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryViewingTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryViewingTransformMatrix()                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYVIEWINGTRANSFORMMATRIX( HPS, LONG, PMATRIXLF );
PCHAR szGpi32QueryViewingTransformMatrix = "Gpi32QueryViewingTransformMatrix";

BOOL APIENTRY Trc_Gpi32QueryViewingTransformMatrix( HPS       Parm1
                                                  , LONG      Parm2
                                                  , PMATRIXLF Parm3
                                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryViewingTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32QueryViewingTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYVIEWINGTRANSFORMMATRIX( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryViewingTransformMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF( Parm3 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szGpi32QueryViewingTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryViewingTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16QueryWidthTable()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIQUERYWIDTHTABLE( HPS, LONG, LONG, PLONG16 );
PCHAR szGpi16QueryWidthTable = "Gpi16QueryWidthTable";

BOOL16 APIENTRY16 Trc_Gpi16QueryWidthTable( HPS     Parm1
                                          , LONG    Parm2
                                          , LONG    Parm3
                                          , PLONG16 Parm4
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16QueryWidthTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szGpi16QueryWidthTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIQUERYWIDTHTABLE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16QueryWidthTable );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID16( Parm4 );
         if ( bRC ) T_LogULONGBuffer16( (PULONG16)Parm4, Parm3 );
      }
      HOOKEXIT16( szGpi16QueryWidthTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16QueryWidthTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryWidthTable()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32QUERYWIDTHTABLE( HPS, LONG, LONG, PLONG );
PCHAR szGpi32QueryWidthTable = "Gpi32QueryWidthTable";

BOOL APIENTRY Trc_Gpi32QueryWidthTable( HPS   Parm1
                                      , LONG  Parm2
                                      , LONG  Parm3
                                      , PLONG Parm4
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32QueryWidthTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szGpi32QueryWidthTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32QUERYWIDTHTABLE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32QueryWidthTable );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID( Parm4 );
         if ( bRC ) T_LogULONGBuffer( (PULONG)Parm4, Parm3 );
      }
      HOOKEXIT( szGpi32QueryWidthTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32QueryWidthTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16RealizeColorTable()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIREALIZECOLORTABLE( HPS );
PCHAR szGpi16RealizeColorTable = "Gpi16RealizeColorTable";

BOOL16 APIENTRY16 Trc_Gpi16RealizeColorTable( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16RealizeColorTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16RealizeColorTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIREALIZECOLORTABLE( Parm1 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16RealizeColorTable );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16RealizeColorTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16RealizeColorTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16RectInRegion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIRECTINREGION( HPS, HRGN, PRECTL16 );
PCHAR szGpi16RectInRegion = "Gpi16RectInRegion";

LONG APIENTRY16 Trc_Gpi16RectInRegion( HPS      Parm1
                                     , HRGN     Parm2
                                     , PRECTL16 Parm3
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16RectInRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
      }
      HOOKENTRY16( szGpi16RectInRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIRECTINREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16RectInRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RRGN_ERROR );
      HOOKEXIT16( szGpi16RectInRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16RectInRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32RectInRegion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32RECTINREGION( HPS, HRGN, PRECTL );
PCHAR szGpi32RectInRegion = "Gpi32RectInRegion";

LONG APIENTRY Trc_Gpi32RectInRegion( HPS    Parm1
                                   , HRGN   Parm2
                                   , PRECTL Parm3
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32RectInRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
      }
      HOOKENTRY( szGpi32RectInRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32RECTINREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32RectInRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RRGN_ERROR );
      HOOKEXIT( szGpi32RectInRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32RectInRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16RectVisible()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIRECTVISIBLE( HPS, PRECTL16 );
PCHAR szGpi16RectVisible = "Gpi16RectVisible";

LONG APIENTRY16 Trc_Gpi16RectVisible( HPS      Parm1
                                    , PRECTL16 Parm2
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16RectVisible );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16RectVisible, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIRECTVISIBLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16RectVisible );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RVIS_ERROR );
      HOOKEXIT16( szGpi16RectVisible, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16RectVisible() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32RectVisible()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32RECTVISIBLE( HPS, PRECTL );
PCHAR szGpi32RectVisible = "Gpi32RectVisible";

LONG APIENTRY Trc_Gpi32RectVisible( HPS    Parm1
                                  , PRECTL Parm2
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32RectVisible );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szGpi32RectVisible, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32RECTVISIBLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32RectVisible );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RVIS_ERROR );
      HOOKEXIT( szGpi32RectVisible, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32RectVisible() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16RemoveDynamics()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIREMOVEDYNAMICS( HPS, LONG, LONG );
PCHAR szGpi16RemoveDynamics = "Gpi16RemoveDynamics";

BOOL16 APIENTRY16 Trc_Gpi16RemoveDynamics( HPS  Parm1
                                         , LONG Parm2
                                         , LONG Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16RemoveDynamics );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16RemoveDynamics, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIREMOVEDYNAMICS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16RemoveDynamics );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16RemoveDynamics, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16RemoveDynamics() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32RemoveDynamics()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32REMOVEDYNAMICS( HPS, LONG, LONG );
PCHAR szGpi32RemoveDynamics = "Gpi32RemoveDynamics";

BOOL APIENTRY Trc_Gpi32RemoveDynamics( HPS  Parm1
                                     , LONG Parm2
                                     , LONG Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32RemoveDynamics );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32RemoveDynamics, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32REMOVEDYNAMICS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32RemoveDynamics );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32RemoveDynamics, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32RemoveDynamics() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16ResetBoundaryData()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIRESETBOUNDARYDATA( HPS );
PCHAR szGpi16ResetBoundaryData = "Gpi16ResetBoundaryData";

BOOL16 APIENTRY16 Trc_Gpi16ResetBoundaryData( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16ResetBoundaryData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16ResetBoundaryData, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIRESETBOUNDARYDATA( Parm1 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16ResetBoundaryData );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16ResetBoundaryData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16ResetBoundaryData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32ResetBoundaryData()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32RESETBOUNDARYDATA( HPS );
PCHAR szGpi32ResetBoundaryData = "Gpi32ResetBoundaryData";

BOOL APIENTRY Trc_Gpi32ResetBoundaryData( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32ResetBoundaryData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32ResetBoundaryData, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32RESETBOUNDARYDATA( Parm1 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32ResetBoundaryData );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32ResetBoundaryData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32ResetBoundaryData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16ResetPS()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIRESETPS( HPS, ULONG );
PCHAR szGpi16ResetPS = "Gpi16ResetPS";

BOOL16 APIENTRY16 Trc_Gpi16ResetPS( HPS   Parm1
                                  , ULONG Parm2
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16ResetPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16ResetPS, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIRESETPS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16ResetPS );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16ResetPS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16ResetPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32ResetPS()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32RESETPS( HPS, ULONG );
PCHAR szGpi32ResetPS = "Gpi32ResetPS";

BOOL APIENTRY Trc_Gpi32ResetPS( HPS   Parm1
                              , ULONG Parm2
                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32ResetPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32ResetPS, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32RESETPS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32ResetPS );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32ResetPS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32ResetPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32ResizePalette()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY GPI32RESIZEPALETTE( HPAL, ULONG );
PCHAR szGpi32ResizePalette = "Gpi32ResizePalette";

ULONG APIENTRY Trc_Gpi32ResizePalette( HPAL  Parm1
                                     , ULONG Parm2
                                     )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32ResizePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32ResizePalette, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnGPI32RESIZEPALETTE )( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32ResizePalette );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szGpi32ResizePalette, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32ResizePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16RestorePS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIRESTOREPS( HPS, LONG );
PCHAR szGpi16RestorePS = "Gpi16RestorePS";

BOOL16 APIENTRY16 Trc_Gpi16RestorePS( HPS  Parm1
                                    , LONG Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16RestorePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16RestorePS, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIRESTOREPS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16RestorePS );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16RestorePS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16RestorePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32RestorePS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32RESTOREPS( HPS, LONG );
PCHAR szGpi32RestorePS = "Gpi32RestorePS";

BOOL APIENTRY Trc_Gpi32RestorePS( HPS  Parm1
                                , LONG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32RestorePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32RestorePS, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32RESTOREPS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32RestorePS );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32RestorePS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32RestorePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16ResumePlay()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIRESUMEPLAY( HPS );
PCHAR szGpi16ResumePlay = "Gpi16ResumePlay";

BOOL16 APIENTRY16 Trc_Gpi16ResumePlay( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16ResumePlay );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16ResumePlay, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIRESUMEPLAY( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16ResumePlay );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16ResumePlay, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16ResumePlay() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32ResumePlay()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32RESUMEPLAY( HPS );
PCHAR szGpi32ResumePlay = "Gpi32ResumePlay";

BOOL APIENTRY Trc_Gpi32ResumePlay( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32ResumePlay );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32ResumePlay, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32RESUMEPLAY( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32ResumePlay );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32ResumePlay, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32ResumePlay() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Rotate()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIROTATE( HPS, PMATRIXLF16, LONG, FIXED, PPOINTL16 );
PCHAR szGpi16Rotate = "Gpi16Rotate";

BOOL16 APIENTRY16 Trc_Gpi16Rotate( HPS         Parm1
                                 , PMATRIXLF16 Parm2
                                 , LONG        Parm3
                                 , FIXED       Parm4
                                 , PPOINTL16   Parm5
                                 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Rotate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_FIXED );
         T_LogFIXED( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPPOINTL16( Parm5 );
      }
      HOOKENTRY16( szGpi16Rotate, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIROTATE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Rotate );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16Rotate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Rotate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Rotate()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32ROTATE( HPS, PMATRIXLF, LONG, FIXED, PPOINTL );
PCHAR szGpi32Rotate = "Gpi32Rotate";

BOOL APIENTRY Trc_Gpi32Rotate( HPS       Parm1
                             , PMATRIXLF Parm2
                             , LONG      Parm3
                             , FIXED     Parm4
                             , PPOINTL   Parm5
                             )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Rotate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_FIXED );
         T_LogFIXED( Parm4 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPPOINTL( Parm5 );
      }
      HOOKENTRY( szGpi32Rotate, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32ROTATE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Rotate );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32Rotate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Rotate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SaveMetaFile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISAVEMETAFILE( HMF, PSZ16 );
PCHAR szGpi16SaveMetaFile = "Gpi16SaveMetaFile";

BOOL16 APIENTRY16 Trc_Gpi16SaveMetaFile( HMF   Parm1
                                       , PSZ16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SaveMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szGpi16SaveMetaFile, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISAVEMETAFILE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SaveMetaFile );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SaveMetaFile, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SaveMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SaveMetaFile()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SAVEMETAFILE( HMF, PSZ );
PCHAR szGpi32SaveMetaFile = "Gpi32SaveMetaFile";

BOOL APIENTRY Trc_Gpi32SaveMetaFile( HMF Parm1
                                   , PSZ Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SaveMetaFile );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szGpi32SaveMetaFile, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SAVEMETAFILE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SaveMetaFile );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SaveMetaFile, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SaveMetaFile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SavePS()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPISAVEPS( HPS );
PCHAR szGpi16SavePS = "Gpi16SavePS";

LONG APIENTRY16 Trc_Gpi16SavePS( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SavePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16SavePS, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPISAVEPS( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SavePS );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16SavePS, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16SavePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SavePS()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32SAVEPS( HPS );
PCHAR szGpi32SavePS = "Gpi32SavePS";

LONG APIENTRY Trc_Gpi32SavePS( HPS Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SavePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32SavePS, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32SAVEPS( Parm1 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SavePS );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32SavePS, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32SavePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Scale()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISCALE( HPS, PMATRIXLF16, LONG, PFIXED16, PPOINTL16 );
PCHAR szGpi16Scale = "Gpi16Scale";

BOOL16 APIENTRY16 Trc_Gpi16Scale( HPS         Parm1
                                , PMATRIXLF16 Parm2
                                , LONG        Parm3
                                , PFIXED16    Parm4
                                , PPOINTL16   Parm5
                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Scale );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PFIXED );
         T_LogFIXEDBuffer16( Parm4, 2 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPPOINTL16( Parm5 );
      }
      HOOKENTRY16( szGpi16Scale, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISCALE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Scale );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16Scale, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Scale() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Scale()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SCALE( HPS, PMATRIXLF, LONG, PFIXED, PPOINTL );
PCHAR szGpi32Scale = "Gpi32Scale";

BOOL APIENTRY Trc_Gpi32Scale( HPS       Parm1
                            , PMATRIXLF Parm2
                            , LONG      Parm3
                            , PFIXED    Parm4
                            , PPOINTL   Parm5
                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Scale );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PFIXED );
         T_LogFIXEDBuffer( Parm4, 2 );
         T_LogParameter( 5L, DT_PPOINTL );
         T_LogPPOINTL( Parm5 );
      }
      HOOKENTRY( szGpi32Scale, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SCALE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Scale );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32Scale, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Scale() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SelectPalette()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPAL APIENTRY16 GPISELECTPALETTE( HPS, HPAL );
PCHAR szGpi16SelectPalette = "Gpi16SelectPalette";

HPAL APIENTRY16 Trc_Gpi16SelectPalette( HPS  Parm1
                                      , HPAL Parm2
                                      )
{

   HPAL ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SelectPalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPAL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SelectPalette, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPISELECTPALETTE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SelectPalette );
      T_LogPMReturnCode( DT_HPAL, ulRC, ulRC != PAL_ERROR );
      HOOKEXIT16( szGpi16SelectPalette, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16SelectPalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SelectPalette()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPAL APIENTRY GPI32SELECTPALETTE( HPS, HPAL );
PCHAR szGpi32SelectPalette = "Gpi32SelectPalette";

HPAL APIENTRY Trc_Gpi32SelectPalette( HPS  Parm1
                                    , HPAL Parm2
                                    )
{

   HPAL ulRC;                                    /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SelectPalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPAL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SelectPalette, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32SELECTPALETTE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SelectPalette );
      T_LogPMReturnCode( DT_HPAL, ulRC, ulRC != PAL_ERROR );
      HOOKEXIT( szGpi32SelectPalette, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32SelectPalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetArcParams()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETARCPARAMS( HPS, PARCPARAMS16 );
PCHAR szGpi16SetArcParams = "Gpi16SetArcParams";

BOOL16 APIENTRY16 Trc_Gpi16SetArcParams( HPS          Parm1
                                       , PARCPARAMS16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetArcParams );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PARCPARAMS );
         T_LogPARCPARAMS16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetArcParams, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETARCPARAMS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetArcParams );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetArcParams, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetArcParams() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetArcParams()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETARCPARAMS( HPS, PARCPARAMS );
PCHAR szGpi32SetArcParams = "Gpi32SetArcParams";

BOOL APIENTRY Trc_Gpi32SetArcParams( HPS        Parm1
                                   , PARCPARAMS Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetArcParams );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PARCPARAMS );
         T_LogPARCPARAMS( Parm2 );
      }
      HOOKENTRY( szGpi32SetArcParams, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETARCPARAMS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetArcParams );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetArcParams, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetArcParams() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetAttrMode()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETATTRMODE( HPS, LONG );
PCHAR szGpi16SetAttrMode = "Gpi16SetAttrMode";

BOOL16 APIENTRY16 Trc_Gpi16SetAttrMode( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetAttrMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetAttrMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETATTRMODE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetAttrMode );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetAttrMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetAttrMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetAttrMode()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETATTRMODE( HPS, LONG );
PCHAR szGpi32SetAttrMode = "Gpi32SetAttrMode";

BOOL APIENTRY Trc_Gpi32SetAttrMode( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetAttrMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetAttrMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETATTRMODE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetAttrMode );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetAttrMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetAttrMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetAttrs()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETATTRS( HPS, LONG, ULONG, ULONG, PBUNDLE16 );
PCHAR szGpi16SetAttrs = "Gpi16SetAttrs";

BOOL16 APIENTRY16 Trc_Gpi16SetAttrs( HPS       Parm1
                                   , LONG      Parm2
                                   , ULONG     Parm3
                                   , ULONG     Parm4
                                   , PBUNDLE16 Parm5
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PBUNDLE );
         T_LogPBUNDLE16( Parm5, Parm2 );
      }
      HOOKENTRY16( szGpi16SetAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETATTRS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetAttrs );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC  );
      HOOKEXIT16( szGpi16SetAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetAttrs()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETATTRS( HPS, LONG, ULONG, ULONG, PBUNDLE );
PCHAR szGpi32SetAttrs = "Gpi32SetAttrs";

BOOL APIENTRY Trc_Gpi32SetAttrs( HPS     Parm1
                               , LONG    Parm2
                               , ULONG   Parm3
                               , ULONG   Parm4
                               , PBUNDLE Parm5
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PBUNDLE );
         T_LogPBUNDLE( Parm5, Parm2 );
      }
      HOOKENTRY( szGpi32SetAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETATTRS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetAttrs );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC  );
      HOOKEXIT( szGpi32SetAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetBackColor()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETBACKCOLOR( HPS, LONG );
PCHAR szGpi16SetBackColor = "Gpi16SetBackColor";

BOOL16 APIENTRY16 Trc_Gpi16SetBackColor( HPS  Parm1
                                       , LONG Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetBackColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetBackColor, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETBACKCOLOR( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetBackColor );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetBackColor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetBackColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetBackColor()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETBACKCOLOR( HPS, LONG );
PCHAR szGpi32SetBackColor = "Gpi32SetBackColor";

BOOL APIENTRY Trc_Gpi32SetBackColor( HPS  Parm1
                                   , LONG Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetBackColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetBackColor, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETBACKCOLOR( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetBackColor );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetBackColor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetBackColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetBackMix()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETBACKMIX( HPS, LONG );
PCHAR szGpi16SetBackMix = "Gpi16SetBackMix";

BOOL16 APIENTRY16 Trc_Gpi16SetBackMix( HPS  Parm1
                                     , LONG Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetBackMix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetBackMix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETBACKMIX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetBackMix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetBackMix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetBackMix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetBackMix()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETBACKMIX( HPS, LONG );
PCHAR szGpi32SetBackMix = "Gpi32SetBackMix";

BOOL APIENTRY Trc_Gpi32SetBackMix( HPS  Parm1
                                 , LONG Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetBackMix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetBackMix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETBACKMIX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetBackMix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetBackMix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetBackMix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetBitmap()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HBITMAP APIENTRY16 GPISETBITMAP( HPS, HBITMAP );
PCHAR szGpi16SetBitmap = "Gpi16SetBitmap";

HBITMAP APIENTRY16 Trc_Gpi16SetBitmap( HPS     Parm1
                                     , HBITMAP Parm2
                                     )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetBitmap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPISETBITMAP( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetBitmap );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC != HBM_ERROR );
      HOOKEXIT16( szGpi16SetBitmap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16SetBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetBitmap()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HBITMAP APIENTRY GPI32SETBITMAP( HPS, HBITMAP );
PCHAR szGpi32SetBitmap = "Gpi32SetBitmap";

HBITMAP APIENTRY Trc_Gpi32SetBitmap( HPS     Parm1
                                   , HBITMAP Parm2
                                   )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetBitmap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPI32SETBITMAP( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetBitmap );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC != HBM_ERROR );
      HOOKEXIT( szGpi32SetBitmap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32SetBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetBitmapBits()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPISETBITMAPBITS( HPS, LONG, LONG, PBYTE16, PBITMAPINFO216 );
PCHAR szGpi16SetBitmapBits = "Gpi16SetBitmapBits";

LONG APIENTRY16 Trc_Gpi16SetBitmapBits( HPS            Parm1
                                      , LONG           Parm2
                                      , LONG           Parm3
                                      , PBYTE16        Parm4
                                      , PBITMAPINFO216 Parm5
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetBitmapBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         T_LogG16SBB_Parm4( Parm4, Parm5 );
         T_LogParameter( 5L, DT_PBITMAPINFO2 );
         T_LogPBITMAPINFO216( Parm5 );
      }
      HOOKENTRY16( szGpi16SetBitmapBits, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPISETBITMAPBITS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetBitmapBits );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT16( szGpi16SetBitmapBits, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16SetBitmapBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetBitmapBits()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32SETBITMAPBITS( HPS, LONG, LONG, PBYTE, PBITMAPINFO2 );
PCHAR szGpi32SetBitmapBits = "Gpi32SetBitmapBits";

LONG APIENTRY Trc_Gpi32SetBitmapBits( HPS          Parm1
                                    , LONG         Parm2
                                    , LONG         Parm3
                                    , PBYTE        Parm4
                                    , PBITMAPINFO2 Parm5
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetBitmapBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         T_LogG32SBB_Parm4( Parm4, Parm5 );
         T_LogParameter( 5L, DT_PBITMAPINFO2 );
         T_LogPBITMAPINFO2( Parm5 );
      }
      HOOKENTRY( szGpi32SetBitmapBits, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32SETBITMAPBITS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetBitmapBits );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32SetBitmapBits, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32SetBitmapBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetBitmapDimension()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETBITMAPDIMENSION( HBITMAP, PSIZEL16 );
PCHAR szGpi16SetBitmapDimension = "Gpi16SetBitmapDimension";

BOOL16 APIENTRY16 Trc_Gpi16SetBitmapDimension( HBITMAP  Parm1
                                             , PSIZEL16 Parm2
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetBitmapDimension );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPSIZEL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetBitmapDimension, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETBITMAPDIMENSION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetBitmapDimension );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetBitmapDimension, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetBitmapDimension() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetBitmapDimension()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETBITMAPDIMENSION( HBITMAP, PSIZEL );
PCHAR szGpi32SetBitmapDimension = "Gpi32SetBitmapDimension";

BOOL APIENTRY Trc_Gpi32SetBitmapDimension( HBITMAP Parm1
                                         , PSIZEL  Parm2
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetBitmapDimension );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HBITMAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPSIZEL( Parm2 );
      }
      HOOKENTRY( szGpi32SetBitmapDimension, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETBITMAPDIMENSION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetBitmapDimension );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetBitmapDimension, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetBitmapDimension() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetBitmapId()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETBITMAPID( HPS, HBITMAP, LONG );
PCHAR szGpi16SetBitmapId = "Gpi16SetBitmapId";

BOOL16 APIENTRY16 Trc_Gpi16SetBitmapId( HPS     Parm1
                                      , HBITMAP Parm2
                                      , LONG    Parm3
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetBitmapId );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16SetBitmapId, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETBITMAPID( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetBitmapId );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetBitmapId, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetBitmapId() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetBitmapId()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETBITMAPID( HPS, HBITMAP, LONG );
PCHAR szGpi32SetBitmapId = "Gpi32SetBitmapId";

BOOL APIENTRY Trc_Gpi32SetBitmapId( HPS     Parm1
                                  , HBITMAP Parm2
                                  , LONG    Parm3
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetBitmapId );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32SetBitmapId, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETBITMAPID( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetBitmapId );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetBitmapId, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetBitmapId() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCharAngle()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCHARANGLE( HPS, PGRADIENTL16 );
PCHAR szGpi16SetCharAngle = "Gpi16SetCharAngle";

BOOL16 APIENTRY16 Trc_Gpi16SetCharAngle( HPS          Parm1
                                       , PGRADIENTL16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCharAngle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PGRADIENTL );
         T_LogPGRADIENTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCharAngle, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCHARANGLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCharAngle );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCharAngle, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCharAngle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCharAngle()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCHARANGLE( HPS, PGRADIENTL );
PCHAR szGpi32SetCharAngle = "Gpi32SetCharAngle";

BOOL APIENTRY Trc_Gpi32SetCharAngle( HPS        Parm1
                                   , PGRADIENTL Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCharAngle );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PGRADIENTL );
         T_LogPGRADIENTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetCharAngle, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCHARANGLE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCharAngle );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCharAngle, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCharAngle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCharBox()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCHARBOX( HPS, PSIZEF16 );
PCHAR szGpi16SetCharBox = "Gpi16SetCharBox";

BOOL16 APIENTRY16 Trc_Gpi16SetCharBox( HPS      Parm1
                                     , PSIZEF16 Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCharBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEF );
         T_LogPSIZEF16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCharBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCHARBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCharBox );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCharBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCharBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCharBox()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCHARBOX( HPS, PSIZEF );
PCHAR szGpi32SetCharBox = "Gpi32SetCharBox";

BOOL APIENTRY Trc_Gpi32SetCharBox( HPS    Parm1
                                 , PSIZEF Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCharBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEF );
         T_LogPSIZEF( Parm2 );
      }
      HOOKENTRY( szGpi32SetCharBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCHARBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCharBox );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCharBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCharBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCharBreakExtra()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCHARBREAKEXTRA( HPS, FIXED );
PCHAR szGpi16SetCharBreakExtra = "Gpi16SetCharBreakExtra";

BOOL16 APIENTRY16 Trc_Gpi16SetCharBreakExtra( HPS   Parm1
                                            , FIXED Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCharBreakExtra );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_FIXED );
         T_LogFIXED( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCharBreakExtra, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCHARBREAKEXTRA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCharBreakExtra );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCharBreakExtra, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCharBreakExtra() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCharBreakExtra()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCHARBREAKEXTRA( HPS, FIXED );
PCHAR szGpi32SetCharBreakExtra = "Gpi32SetCharBreakExtra";

BOOL APIENTRY Trc_Gpi32SetCharBreakExtra( HPS   Parm1
                                        , FIXED Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCharBreakExtra );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_FIXED );
         T_LogFIXED( Parm2 );
      }
      HOOKENTRY( szGpi32SetCharBreakExtra, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCHARBREAKEXTRA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCharBreakExtra );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCharBreakExtra, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCharBreakExtra() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCharDirection()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCHARDIRECTION( HPS, LONG );
PCHAR szGpi16SetCharDirection = "Gpi16SetCharDirection";

BOOL16 APIENTRY16 Trc_Gpi16SetCharDirection( HPS  Parm1
                                           , LONG Parm2
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCharDirection );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCharDirection, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCHARDIRECTION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCharDirection );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCharDirection, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCharDirection() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCharDirection()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCHARDIRECTION( HPS, LONG );
PCHAR szGpi32SetCharDirection = "Gpi32SetCharDirection";

BOOL APIENTRY Trc_Gpi32SetCharDirection( HPS  Parm1
                                       , LONG Parm2
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCharDirection );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetCharDirection, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCHARDIRECTION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCharDirection );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCharDirection, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCharDirection() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCharExtra()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCHAREXTRA( HPS, FIXED );
PCHAR szGpi16SetCharExtra = "Gpi16SetCharExtra";

BOOL16 APIENTRY16 Trc_Gpi16SetCharExtra( HPS   Parm1
                                       , FIXED Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCharExtra );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_FIXED );
         T_LogFIXED( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCharExtra, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCHAREXTRA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCharExtra );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCharExtra, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCharExtra() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCharExtra()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCHAREXTRA( HPS, FIXED );
PCHAR szGpi32SetCharExtra = "Gpi32SetCharExtra";

BOOL APIENTRY Trc_Gpi32SetCharExtra( HPS   Parm1
                                   , FIXED Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCharExtra );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_FIXED );
         T_LogFIXED( Parm2 );
      }
      HOOKENTRY( szGpi32SetCharExtra, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCHAREXTRA( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCharExtra );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCharExtra, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCharExtra() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCharMode()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCHARMODE( HPS, LONG );
PCHAR szGpi16SetCharMode = "Gpi16SetCharMode";

BOOL16 APIENTRY16 Trc_Gpi16SetCharMode( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCharMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCharMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCHARMODE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCharMode );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCharMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCharMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCharMode()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCHARMODE( HPS, LONG );
PCHAR szGpi32SetCharMode = "Gpi32SetCharMode";

BOOL APIENTRY Trc_Gpi32SetCharMode( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCharMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetCharMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCHARMODE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCharMode );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCharMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCharMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCharSet()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCHARSET( HPS, LONG );
PCHAR szGpi16SetCharSet = "Gpi16SetCharSet";

BOOL16 APIENTRY16 Trc_Gpi16SetCharSet( HPS  Parm1
                                     , LONG Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCharSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCharSet, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCHARSET( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCharSet );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCharSet, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCharSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCharSet()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCHARSET( HPS, LONG );
PCHAR szGpi32SetCharSet = "Gpi32SetCharSet";

BOOL APIENTRY Trc_Gpi32SetCharSet( HPS  Parm1
                                 , LONG Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCharSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetCharSet, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCHARSET( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCharSet );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCharSet, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCharSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCharShear()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCHARSHEAR( HPS, PPOINTL16 );
PCHAR szGpi16SetCharShear = "Gpi16SetCharShear";

BOOL16 APIENTRY16 Trc_Gpi16SetCharShear( HPS       Parm1
                                       , PPOINTL16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCharShear );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCharShear, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCHARSHEAR( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCharShear );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCharShear, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCharShear() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCharShear()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCHARSHEAR( HPS, PPOINTL );
PCHAR szGpi32SetCharShear = "Gpi32SetCharShear";

BOOL APIENTRY Trc_Gpi32SetCharShear( HPS     Parm1
                                   , PPOINTL Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCharShear );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetCharShear, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCHARSHEAR( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCharShear );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCharShear, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCharShear() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetClipPath()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCLIPPATH( HPS, LONG, LONG );
PCHAR szGpi16SetClipPath = "Gpi16SetClipPath";

BOOL16 APIENTRY16 Trc_Gpi16SetClipPath( HPS  Parm1
                                      , LONG Parm2
                                      , LONG Parm3
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetClipPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16SetClipPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCLIPPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetClipPath );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetClipPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetClipPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetClipPath()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCLIPPATH( HPS, LONG, LONG );
PCHAR szGpi32SetClipPath = "Gpi32SetClipPath";

BOOL APIENTRY Trc_Gpi32SetClipPath( HPS  Parm1
                                  , LONG Parm2
                                  , LONG Parm3
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetClipPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32SetClipPath, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCLIPPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetClipPath );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetClipPath, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetClipPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetClipRegion()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPISETCLIPREGION( HPS, HRGN, PHRGN16 );
PCHAR szGpi16SetClipRegion = "Gpi16SetClipRegion";

LONG APIENTRY16 Trc_Gpi16SetClipRegion( HPS     Parm1
                                      , HRGN    Parm2
                                      , PHRGN16 Parm3
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetClipRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PHRGN );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szGpi16SetClipRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPISETCLIPREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetClipRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PHRGN );
         T_LogPULONG16( Parm3 );
      }
      HOOKEXIT16( szGpi16SetClipRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16SetClipRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetClipRegion()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32SETCLIPREGION( HPS, HRGN, PHRGN );
PCHAR szGpi32SetClipRegion = "Gpi32SetClipRegion";

LONG APIENTRY Trc_Gpi32SetClipRegion( HPS   Parm1
                                    , HRGN  Parm2
                                    , PHRGN Parm3
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetClipRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PHRGN );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szGpi32SetClipRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32SETCLIPREGION( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetClipRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PHRGN );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szGpi32SetClipRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32SetClipRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetColor()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCOLOR( HPS, LONG );
PCHAR szGpi16SetColor = "Gpi16SetColor";

BOOL16 APIENTRY16 Trc_Gpi16SetColor( HPS  Parm1
                                   , LONG Parm2
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetColor, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCOLOR( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetColor );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetColor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetColor()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCOLOR( HPS, LONG );
PCHAR szGpi32SetColor = "Gpi32SetColor";

BOOL APIENTRY Trc_Gpi32SetColor( HPS  Parm1
                               , LONG Parm2
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetColor, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCOLOR( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetColor );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetColor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCP( HPS, USHORT );
PCHAR szGpi16SetCp = "Gpi16SetCp";

BOOL16 APIENTRY16 Trc_Gpi16SetCp( HPS    Parm1
                                , USHORT Parm2
                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCp, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCP( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCp );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCp, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCP( HPS, ULONG );
PCHAR szGpi32SetCp = "Gpi32SetCp";

BOOL APIENTRY Trc_Gpi32SetCp( HPS   Parm1
                            , ULONG Parm2
                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetCp, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCP( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCp );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCp, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetCurrentPosition()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETCURRENTPOSITION( HPS, PPOINTL16 );
PCHAR szGpi16SetCurrentPosition = "Gpi16SetCurrentPosition";

BOOL16 APIENTRY16 Trc_Gpi16SetCurrentPosition( HPS       Parm1
                                             , PPOINTL16 Parm2
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetCurrentPosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetCurrentPosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETCURRENTPOSITION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetCurrentPosition );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetCurrentPosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetCurrentPosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetCurrentPosition()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETCURRENTPOSITION( HPS, PPOINTL );
PCHAR szGpi32SetCurrentPosition = "Gpi32SetCurrentPosition";

BOOL APIENTRY Trc_Gpi32SetCurrentPosition( HPS     Parm1
                                         , PPOINTL Parm2
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetCurrentPosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetCurrentPosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETCURRENTPOSITION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetCurrentPosition );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetCurrentPosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetCurrentPosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetDefArcParams()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETDEFARCPARAMS( HPS, PARCPARAMS16 );
PCHAR szGpi16SetDefArcParams = "Gpi16SetDefArcParams";

BOOL16 APIENTRY16 Trc_Gpi16SetDefArcParams( HPS          Parm1
                                          , PARCPARAMS16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetDefArcParams );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PARCPARAMS );
         T_LogPARCPARAMS16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetDefArcParams, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETDEFARCPARAMS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetDefArcParams );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetDefArcParams, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetDefArcParams() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetDefArcParams()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETDEFARCPARAMS( HPS, PARCPARAMS );
PCHAR szGpi32SetDefArcParams = "Gpi32SetDefArcParams";

BOOL APIENTRY Trc_Gpi32SetDefArcParams( HPS        Parm1
                                      , PARCPARAMS Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetDefArcParams );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PARCPARAMS );
         T_LogPARCPARAMS( Parm2 );
      }
      HOOKENTRY( szGpi32SetDefArcParams, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETDEFARCPARAMS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetDefArcParams );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetDefArcParams, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetDefArcParams() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetDefAttrs()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETDEFATTRS( HPS, LONG, ULONG, PBUNDLE16 );
PCHAR szGpi16SetDefAttrs = "Gpi16SetDefAttrs";

BOOL16 APIENTRY16 Trc_Gpi16SetDefAttrs( HPS       Parm1
                                      , LONG      Parm2
                                      , ULONG     Parm3
                                      , PBUNDLE16 Parm4
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetDefAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBUNDLE );
         T_LogPBUNDLE16( Parm4, Parm2 );
      }
      HOOKENTRY16( szGpi16SetDefAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETDEFATTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetDefAttrs );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetDefAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetDefAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetDefAttrs()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETDEFATTRS( HPS, LONG, ULONG, PBUNDLE );
PCHAR szGpi32SetDefAttrs = "Gpi32SetDefAttrs";

BOOL APIENTRY Trc_Gpi32SetDefAttrs( HPS     Parm1
                                  , LONG    Parm2
                                  , ULONG   Parm3
                                  , PBUNDLE Parm4
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetDefAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBUNDLE );
         T_LogPBUNDLE( Parm4, Parm2 );
      }
      HOOKENTRY( szGpi32SetDefAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETDEFATTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetDefAttrs );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetDefAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetDefAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetDefaultViewMatrix()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETDEFAULTVIEWMATRIX( HPS, LONG, PMATRIXLF16, LONG );
PCHAR szGpi16SetDefaultViewMatrix = "Gpi16SetDefaultViewMatrix";

BOOL16 APIENTRY16 Trc_Gpi16SetDefaultViewMatrix( HPS         Parm1
                                               , LONG        Parm2
                                               , PMATRIXLF16 Parm3
                                               , LONG        Parm4
                                               )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetDefaultViewMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szGpi16SetDefaultViewMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETDEFAULTVIEWMATRIX( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetDefaultViewMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetDefaultViewMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetDefaultViewMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetDefaultViewMatrix()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETDEFAULTVIEWMATRIX( HPS, LONG, PMATRIXLF, LONG );
PCHAR szGpi32SetDefaultViewMatrix = "Gpi32SetDefaultViewMatrix";

BOOL APIENTRY Trc_Gpi32SetDefaultViewMatrix( HPS       Parm1
                                           , LONG      Parm2
                                           , PMATRIXLF Parm3
                                           , LONG      Parm4
                                           )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetDefaultViewMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szGpi32SetDefaultViewMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETDEFAULTVIEWMATRIX( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetDefaultViewMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetDefaultViewMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetDefaultViewMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetDefTag()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETDEFTAG( HPS, LONG );
PCHAR szGpi16SetDefTag = "Gpi16SetDefTag";

BOOL16 APIENTRY16 Trc_Gpi16SetDefTag( HPS  Parm1
                                    , LONG Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetDefTag );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetDefTag, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETDEFTAG( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetDefTag );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetDefTag, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetDefTag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetDefTag()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETDEFTAG( HPS, LONG );
PCHAR szGpi32SetDefTag = "Gpi32SetDefTag";

BOOL APIENTRY Trc_Gpi32SetDefTag( HPS  Parm1
                                , LONG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetDefTag );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetDefTag, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETDEFTAG( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetDefTag );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetDefTag, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetDefTag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetDefViewingLimits()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETDEFVIEWINGLIMITS( HPS, PRECTL16 );
PCHAR szGpi16SetDefViewingLimits = "Gpi16SetDefViewingLimits";

BOOL16 APIENTRY16 Trc_Gpi16SetDefViewingLimits( HPS      Parm1
                                              , PRECTL16 Parm2
                                              )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetDefViewingLimits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetDefViewingLimits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETDEFVIEWINGLIMITS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetDefViewingLimits );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetDefViewingLimits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetDefViewingLimits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetDefViewingLimits()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETDEFVIEWINGLIMITS( HPS, PRECTL );
PCHAR szGpi32SetDefViewingLimits = "Gpi32SetDefViewingLimits";

BOOL APIENTRY Trc_Gpi32SetDefViewingLimits( HPS    Parm1
                                          , PRECTL Parm2
                                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetDefViewingLimits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetDefViewingLimits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETDEFVIEWINGLIMITS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_DEF ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetDefViewingLimits );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetDefViewingLimits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetDefViewingLimits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetDrawControl()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETDRAWCONTROL( HPS, LONG, LONG );
PCHAR szGpi16SetDrawControl = "Gpi16SetDrawControl";

BOOL16 APIENTRY16 Trc_Gpi16SetDrawControl( HPS  Parm1
                                         , LONG Parm2
                                         , LONG Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetDrawControl );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16SetDrawControl, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETDRAWCONTROL( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetDrawControl );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetDrawControl, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetDrawControl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetDrawControl()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETDRAWCONTROL( HPS, LONG, LONG );
PCHAR szGpi32SetDrawControl = "Gpi32SetDrawControl";

BOOL APIENTRY Trc_Gpi32SetDrawControl( HPS  Parm1
                                     , LONG Parm2
                                     , LONG Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetDrawControl );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32SetDrawControl, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETDRAWCONTROL( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetDrawControl );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetDrawControl, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetDrawControl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetDrawingMode()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETDRAWINGMODE( HPS, LONG );
PCHAR szGpi16SetDrawingMode = "Gpi16SetDrawingMode";

BOOL16 APIENTRY16 Trc_Gpi16SetDrawingMode( HPS  Parm1
                                         , LONG Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetDrawingMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetDrawingMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETDRAWINGMODE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetDrawingMode );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetDrawingMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetDrawingMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetDrawingMode()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETDRAWINGMODE( HPS, LONG );
PCHAR szGpi32SetDrawingMode = "Gpi32SetDrawingMode";

BOOL APIENTRY Trc_Gpi32SetDrawingMode( HPS  Parm1
                                     , LONG Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetDrawingMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetDrawingMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETDRAWINGMODE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetDrawingMode );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetDrawingMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetDrawingMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetEditMode()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETEDITMODE( HPS, LONG );
PCHAR szGpi16SetEditMode = "Gpi16SetEditMode";

BOOL16 APIENTRY16 Trc_Gpi16SetEditMode( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetEditMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetEditMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETEDITMODE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetEditMode );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetEditMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetEditMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetEditMode()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETEDITMODE( HPS, LONG );
PCHAR szGpi32SetEditMode = "Gpi32SetEditMode";

BOOL APIENTRY Trc_Gpi32SetEditMode( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetEditMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetEditMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETEDITMODE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetEditMode );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetEditMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetEditMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetElementPointer()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETELEMENTPOINTER( HPS, LONG );
PCHAR szGpi16SetElementPointer = "Gpi16SetElementPointer";

BOOL16 APIENTRY16 Trc_Gpi16SetElementPointer( HPS  Parm1
                                            , LONG Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetElementPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetElementPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETELEMENTPOINTER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetElementPointer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetElementPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetElementPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetElementPointer()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETELEMENTPOINTER( HPS, LONG );
PCHAR szGpi32SetElementPointer = "Gpi32SetElementPointer";

BOOL APIENTRY Trc_Gpi32SetElementPointer( HPS  Parm1
                                        , LONG Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetElementPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetElementPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETELEMENTPOINTER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetElementPointer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetElementPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetElementPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetElementPointerAtLabel()                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETELEMENTPOINTERATLABEL( HPS, LONG );
PCHAR szGpi16SetElementPointerAtLabel = "Gpi16SetElementPointerAtLabel";

BOOL16 APIENTRY16 Trc_Gpi16SetElementPointerAtLabel( HPS  Parm1
                                                   , LONG Parm2
                                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetElementPointerAtLabel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetElementPointerAtLabel, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETELEMENTPOINTERATLABEL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetElementPointerAtLabel );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetElementPointerAtLabel, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetElementPointerAtLabel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetElementPointerAtLabel()                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETELEMENTPOINTERATLABEL( HPS, LONG );
PCHAR szGpi32SetElementPointerAtLabel = "Gpi32SetElementPointerAtLabel";

BOOL APIENTRY Trc_Gpi32SetElementPointerAtLabel( HPS  Parm1
                                               , LONG Parm2
                                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetElementPointerAtLabel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetElementPointerAtLabel, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETELEMENTPOINTERATLABEL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_EDIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetElementPointerAtLabel );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetElementPointerAtLabel, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetElementPointerAtLabel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetFontMappingFlags()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY GPI32SETFONTMAPPINGFLAGS( HPS, ULONG );
PCHAR szGpi32SetFontMappingFlags = "Gpi32SetFontMappingFlags";

ULONG APIENTRY Trc_Gpi32SetFontMappingFlags( HPS   Parm1
                                           , ULONG Parm2
                                           )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetFontMappingFlags );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetFontMappingFlags, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnGPI32SETFONTMAPPINGFLAGS )( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetFontMappingFlags );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != GPI_ALTERROR );
      HOOKEXIT( szGpi32SetFontMappingFlags, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi32SetFontMappingFlags() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetGraphicsField()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETGRAPHICSFIELD( HPS, PRECTL16 );
PCHAR szGpi16SetGraphicsField = "Gpi16SetGraphicsField";

BOOL16 APIENTRY16 Trc_Gpi16SetGraphicsField( HPS      Parm1
                                           , PRECTL16 Parm2
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetGraphicsField );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetGraphicsField, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETGRAPHICSFIELD( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetGraphicsField );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetGraphicsField, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetGraphicsField() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetGraphicsField()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETGRAPHICSFIELD( HPS, PRECTL );
PCHAR szGpi32SetGraphicsField = "Gpi32SetGraphicsField";

BOOL APIENTRY Trc_Gpi32SetGraphicsField( HPS    Parm1
                                       , PRECTL Parm2
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetGraphicsField );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetGraphicsField, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETGRAPHICSFIELD( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetGraphicsField );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetGraphicsField, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetGraphicsField() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetInitialSegmentAttrs()                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETINITIALSEGMENTATTRS( HPS, LONG, LONG );
PCHAR szGpi16SetInitialSegmentAttrs = "Gpi16SetInitialSegmentAttrs";

BOOL16 APIENTRY16 Trc_Gpi16SetInitialSegmentAttrs( HPS  Parm1
                                                 , LONG Parm2
                                                 , LONG Parm3
                                                 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetInitialSegmentAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16SetInitialSegmentAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETINITIALSEGMENTATTRS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetInitialSegmentAttrs );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetInitialSegmentAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetInitialSegmentAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetInitialSegmentAttrs()                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETINITIALSEGMENTATTRS( HPS, LONG, LONG );
PCHAR szGpi32SetInitialSegmentAttrs = "Gpi32SetInitialSegmentAttrs";

BOOL APIENTRY Trc_Gpi32SetInitialSegmentAttrs( HPS  Parm1
                                             , LONG Parm2
                                             , LONG Parm3
                                             )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetInitialSegmentAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32SetInitialSegmentAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETINITIALSEGMENTATTRS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetInitialSegmentAttrs );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetInitialSegmentAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetInitialSegmentAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetLineEnd()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETLINEEND( HPS, LONG );
PCHAR szGpi16SetLineEnd = "Gpi16SetLineEnd";

BOOL16 APIENTRY16 Trc_Gpi16SetLineEnd( HPS  Parm1
                                     , LONG Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetLineEnd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetLineEnd, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETLINEEND( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetLineEnd );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetLineEnd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetLineEnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetLineEnd()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETLINEEND( HPS, LONG );
PCHAR szGpi32SetLineEnd = "Gpi32SetLineEnd";

BOOL APIENTRY Trc_Gpi32SetLineEnd( HPS  Parm1
                                 , LONG Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetLineEnd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetLineEnd, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETLINEEND( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetLineEnd );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetLineEnd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetLineEnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetLineJoin()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETLINEJOIN( HPS, LONG );
PCHAR szGpi16SetLineJoin = "Gpi16SetLineJoin";

BOOL16 APIENTRY16 Trc_Gpi16SetLineJoin( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetLineJoin );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetLineJoin, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETLINEJOIN( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetLineJoin );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetLineJoin, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetLineJoin() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetLineJoin()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETLINEJOIN( HPS, LONG );
PCHAR szGpi32SetLineJoin = "Gpi32SetLineJoin";

BOOL APIENTRY Trc_Gpi32SetLineJoin( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetLineJoin );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetLineJoin, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETLINEJOIN( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetLineJoin );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetLineJoin, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetLineJoin() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetLineType()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETLINETYPE( HPS, LONG );
PCHAR szGpi16SetLineType = "Gpi16SetLineType";

BOOL16 APIENTRY16 Trc_Gpi16SetLineType( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetLineType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetLineType, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETLINETYPE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetLineType );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetLineType, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetLineType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetLineType()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETLINETYPE( HPS, LONG );
PCHAR szGpi32SetLineType = "Gpi32SetLineType";

BOOL APIENTRY Trc_Gpi32SetLineType( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetLineType );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetLineType, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETLINETYPE( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetLineType );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetLineType, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetLineType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetLineWidth()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETLINEWIDTH( HPS, FIXED );
PCHAR szGpi16SetLineWidth = "Gpi16SetLineWidth";

BOOL16 APIENTRY16 Trc_Gpi16SetLineWidth( HPS   Parm1
                                       , FIXED Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetLineWidth );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_FIXED );
         T_LogFIXED( Parm2 );
      }
      HOOKENTRY16( szGpi16SetLineWidth, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETLINEWIDTH( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetLineWidth );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetLineWidth, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetLineWidth() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetLineWidth()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETLINEWIDTH( HPS, FIXED );
PCHAR szGpi32SetLineWidth = "Gpi32SetLineWidth";

BOOL APIENTRY Trc_Gpi32SetLineWidth( HPS   Parm1
                                   , FIXED Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetLineWidth );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_FIXED );
         T_LogFIXED( Parm2 );
      }
      HOOKENTRY( szGpi32SetLineWidth, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETLINEWIDTH( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetLineWidth );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetLineWidth, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetLineWidth() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetLineWidthGeom()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETLINEWIDTHGEOM( HPS, LONG );
PCHAR szGpi16SetLineWidthGeom = "Gpi16SetLineWidthGeom";

BOOL16 APIENTRY16 Trc_Gpi16SetLineWidthGeom( HPS  Parm1
                                           , LONG Parm2
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetLineWidthGeom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetLineWidthGeom, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETLINEWIDTHGEOM( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetLineWidthGeom );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetLineWidthGeom, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetLineWidthGeom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetLineWidthGeom()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETLINEWIDTHGEOM( HPS, LONG );
PCHAR szGpi32SetLineWidthGeom = "Gpi32SetLineWidthGeom";

BOOL APIENTRY Trc_Gpi32SetLineWidthGeom( HPS  Parm1
                                       , LONG Parm2
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetLineWidthGeom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetLineWidthGeom, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETLINEWIDTHGEOM( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetLineWidthGeom );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetLineWidthGeom, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetLineWidthGeom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetMarker()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETMARKER( HPS, LONG );
PCHAR szGpi16SetMarker = "Gpi16SetMarker";

BOOL16 APIENTRY16 Trc_Gpi16SetMarker( HPS  Parm1
                                    , LONG Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetMarker );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetMarker, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETMARKER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetMarker );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetMarker, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetMarker() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetMarker()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETMARKER( HPS, LONG );
PCHAR szGpi32SetMarker = "Gpi32SetMarker";

BOOL APIENTRY Trc_Gpi32SetMarker( HPS  Parm1
                                , LONG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetMarker );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetMarker, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETMARKER( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetMarker );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetMarker, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetMarker() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetMarkerBox()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETMARKERBOX( HPS, PSIZEF16 );
PCHAR szGpi16SetMarkerBox = "Gpi16SetMarkerBox";

BOOL16 APIENTRY16 Trc_Gpi16SetMarkerBox( HPS      Parm1
                                       , PSIZEF16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetMarkerBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEF );
         T_LogPSIZEF16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetMarkerBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETMARKERBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetMarkerBox );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetMarkerBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetMarkerBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetMarkerBox()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETMARKERBOX( HPS, PSIZEF );
PCHAR szGpi32SetMarkerBox = "Gpi32SetMarkerBox";

BOOL APIENTRY Trc_Gpi32SetMarkerBox( HPS    Parm1
                                   , PSIZEF Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetMarkerBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEF );
         T_LogPSIZEF( Parm2 );
      }
      HOOKENTRY( szGpi32SetMarkerBox, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETMARKERBOX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetMarkerBox );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetMarkerBox, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetMarkerBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetMarkerSet()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETMARKERSET( HPS, LONG );
PCHAR szGpi16SetMarkerSet = "Gpi16SetMarkerSet";

BOOL16 APIENTRY16 Trc_Gpi16SetMarkerSet( HPS  Parm1
                                       , LONG Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetMarkerSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetMarkerSet, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETMARKERSET( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetMarkerSet );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetMarkerSet, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetMarkerSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetMarkerSet()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETMARKERSET( HPS, LONG );
PCHAR szGpi32SetMarkerSet = "Gpi32SetMarkerSet";

BOOL APIENTRY Trc_Gpi32SetMarkerSet( HPS  Parm1
                                   , LONG Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetMarkerSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetMarkerSet, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETMARKERSET( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetMarkerSet );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetMarkerSet, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetMarkerSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetMetaFileBits()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETMETAFILEBITS( HMF, LONG, LONG, PBYTE16 );
PCHAR szGpi16SetMetaFileBits = "Gpi16SetMetaFileBits";

BOOL16 APIENTRY16 Trc_Gpi16SetMetaFileBits( HMF     Parm1
                                          , LONG    Parm2
                                          , LONG    Parm3
                                          , PBYTE16 Parm4
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetMetaFileBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( (PCHAR16)Parm4, Parm3 );
      }
      HOOKENTRY16( szGpi16SetMetaFileBits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETMETAFILEBITS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetMetaFileBits );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetMetaFileBits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetMetaFileBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetMetaFileBits()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETMETAFILEBITS( HMF, LONG, LONG, PBYTE );
PCHAR szGpi32SetMetaFileBits = "Gpi32SetMetaFileBits";

BOOL APIENTRY Trc_Gpi32SetMetaFileBits( HMF   Parm1
                                      , LONG  Parm2
                                      , LONG  Parm3
                                      , PBYTE Parm4
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetMetaFileBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMF );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PBYTE );
         T_LogPVOID( Parm4 );
         T_LogCHARBuffer( (PCHAR)Parm4, Parm3 );
      }
      HOOKENTRY( szGpi32SetMetaFileBits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETMETAFILEBITS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetMetaFileBits );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetMetaFileBits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetMetaFileBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetMix()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETMIX( HPS, LONG );
PCHAR szGpi16SetMix = "Gpi16SetMix";

BOOL16 APIENTRY16 Trc_Gpi16SetMix( HPS  Parm1
                                 , LONG Parm2
                                 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetMix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetMix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETMIX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetMix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetMix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetMix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetMix()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETMIX( HPS, LONG );
PCHAR szGpi32SetMix = "Gpi32SetMix";

BOOL APIENTRY Trc_Gpi32SetMix( HPS  Parm1
                             , LONG Parm2
                             )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetMix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetMix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETMIX( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetMix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetMix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetMix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetModelTransformMatrix()                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETMODELTRANSFORMMATRIX( HPS, LONG, PMATRIXLF16, LONG );
PCHAR szGpi16SetModelTransformMatrix = "Gpi16SetModelTransformMatrix";

BOOL16 APIENTRY16 Trc_Gpi16SetModelTransformMatrix( HPS         Parm1
                                                  , LONG        Parm2
                                                  , PMATRIXLF16 Parm3
                                                  , LONG        Parm4
                                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetModelTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szGpi16SetModelTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETMODELTRANSFORMMATRIX( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetModelTransformMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetModelTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetModelTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetModelTransformMatrix()                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETMODELTRANSFORMMATRIX( HPS, LONG, PMATRIXLF, LONG );
PCHAR szGpi32SetModelTransformMatrix = "Gpi32SetModelTransformMatrix";

BOOL APIENTRY Trc_Gpi32SetModelTransformMatrix( HPS       Parm1
                                              , LONG      Parm2
                                              , PMATRIXLF Parm3
                                              , LONG      Parm4
                                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetModelTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szGpi32SetModelTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETMODELTRANSFORMMATRIX( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetModelTransformMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetModelTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetModelTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPageViewport()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETPAGEVIEWPORT( HPS, PRECTL16 );
PCHAR szGpi16SetPageViewport = "Gpi16SetPageViewport";

BOOL16 APIENTRY16 Trc_Gpi16SetPageViewport( HPS      Parm1
                                          , PRECTL16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPageViewport );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetPageViewport, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETPAGEVIEWPORT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPageViewport );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetPageViewport, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetPageViewport() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPageViewport()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETPAGEVIEWPORT( HPS, PRECTL );
PCHAR szGpi32SetPageViewport = "Gpi32SetPageViewport";

BOOL APIENTRY Trc_Gpi32SetPageViewport( HPS    Parm1
                                      , PRECTL Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPageViewport );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetPageViewport, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETPAGEVIEWPORT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPageViewport );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetPageViewport, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetPageViewport() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPaletteEntries()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETPALETTEENTRIES( HPAL, ULONG, ULONG, ULONG, PULONG16 );
PCHAR szGpi16SetPaletteEntries = "Gpi16SetPaletteEntries";

BOOL16 APIENTRY16 Trc_Gpi16SetPaletteEntries( HPAL     Parm1
                                            , ULONG    Parm2
                                            , ULONG    Parm3
                                            , ULONG    Parm4
                                            , PULONG16 Parm5
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPaletteEntries );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPVOID16( Parm5 );
         T_LogULONGBuffer16( Parm5, Parm4 );
      }
      HOOKENTRY16( szGpi16SetPaletteEntries, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETPALETTEENTRIES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPaletteEntries );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetPaletteEntries, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetPaletteEntries() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPaletteEntries()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETPALETTEENTRIES( HPAL, ULONG, ULONG, ULONG, PULONG );
PCHAR szGpi32SetPaletteEntries = "Gpi32SetPaletteEntries";

BOOL APIENTRY Trc_Gpi32SetPaletteEntries( HPAL   Parm1
                                        , ULONG  Parm2
                                        , ULONG  Parm3
                                        , ULONG  Parm4
                                        , PULONG Parm5
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPaletteEntries );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPAL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PULONG );
         T_LogPVOID( Parm5 );
         T_LogULONGBuffer( Parm5, Parm4 );
      }
      HOOKENTRY( szGpi32SetPaletteEntries, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETPALETTEENTRIES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPaletteEntries );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetPaletteEntries, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetPaletteEntries() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPattern()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETPATTERN( HPS, LONG );
PCHAR szGpi16SetPattern = "Gpi16SetPattern";

BOOL16 APIENTRY16 Trc_Gpi16SetPattern( HPS  Parm1
                                     , LONG Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPattern );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetPattern, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETPATTERN( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPattern );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetPattern, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetPattern() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPattern()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETPATTERN( HPS, LONG );
PCHAR szGpi32SetPattern = "Gpi32SetPattern";

BOOL APIENTRY Trc_Gpi32SetPattern( HPS  Parm1
                                 , LONG Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPattern );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetPattern, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETPATTERN( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPattern );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetPattern, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetPattern() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPatternRefPoint()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETPATTERNREFPOINT( HPS, PPOINTL16 );
PCHAR szGpi16SetPatternRefPoint = "Gpi16SetPatternRefPoint";

BOOL16 APIENTRY16 Trc_Gpi16SetPatternRefPoint( HPS       Parm1
                                             , PPOINTL16 Parm2
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPatternRefPoint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetPatternRefPoint, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETPATTERNREFPOINT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPatternRefPoint );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetPatternRefPoint, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetPatternRefPoint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPatternRefPoint()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETPATTERNREFPOINT( HPS, PPOINTL );
PCHAR szGpi32SetPatternRefPoint = "Gpi32SetPatternRefPoint";

BOOL APIENTRY Trc_Gpi32SetPatternRefPoint( HPS     Parm1
                                         , PPOINTL Parm2
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPatternRefPoint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetPatternRefPoint, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETPATTERNREFPOINT( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPatternRefPoint );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetPatternRefPoint, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetPatternRefPoint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPatternSet()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETPATTERNSET( HPS, LONG );
PCHAR szGpi16SetPatternSet = "Gpi16SetPatternSet";

BOOL16 APIENTRY16 Trc_Gpi16SetPatternSet( HPS  Parm1
                                        , LONG Parm2
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPatternSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetPatternSet, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETPATTERNSET( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPatternSet );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetPatternSet, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetPatternSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPatternSet()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETPATTERNSET( HPS, LONG );
PCHAR szGpi32SetPatternSet = "Gpi32SetPatternSet";

BOOL APIENTRY Trc_Gpi32SetPatternSet( HPS  Parm1
                                    , LONG Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPatternSet );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetPatternSet, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETPATTERNSET( Parm1, Parm2 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPatternSet );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetPatternSet, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetPatternSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPel()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPISETPEL( HPS, PPOINTL16 );
PCHAR szGpi16SetPel = "Gpi16SetPel";

LONG APIENTRY16 Trc_Gpi16SetPel( HPS       Parm1
                               , PPOINTL16 Parm2
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetPel, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPISETPEL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPel );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16SetPel, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16SetPel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPel()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32SETPEL( HPS, PPOINTL );
PCHAR szGpi32SetPel = "Gpi32SetPel";

LONG APIENTRY Trc_Gpi32SetPel( HPS     Parm1
                             , PPOINTL Parm2
                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetPel, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32SETPEL( Parm1, Parm2 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPel );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32SetPel, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32SetPel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPickAperturePosition()                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETPICKAPERTUREPOSITION( HPS, PPOINTL16 );
PCHAR szGpi16SetPickAperturePosition = "Gpi16SetPickAperturePosition";

BOOL16 APIENTRY16 Trc_Gpi16SetPickAperturePosition( HPS       Parm1
                                                  , PPOINTL16 Parm2
                                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPickAperturePosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetPickAperturePosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETPICKAPERTUREPOSITION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPickAperturePosition );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetPickAperturePosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetPickAperturePosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPickAperturePosition()                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETPICKAPERTUREPOSITION( HPS, PPOINTL );
PCHAR szGpi32SetPickAperturePosition = "Gpi32SetPickAperturePosition";

BOOL APIENTRY Trc_Gpi32SetPickAperturePosition( HPS     Parm1
                                              , PPOINTL Parm2
                                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPickAperturePosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetPickAperturePosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETPICKAPERTUREPOSITION( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPickAperturePosition );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetPickAperturePosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetPickAperturePosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPickApertureSize()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETPICKAPERTURESIZE( HPS, LONG, PSIZEL16 );
PCHAR szGpi16SetPickApertureSize = "Gpi16SetPickApertureSize";

BOOL16 APIENTRY16 Trc_Gpi16SetPickApertureSize( HPS      Parm1
                                              , LONG     Parm2
                                              , PSIZEL16 Parm3
                                              )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPickApertureSize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSIZEL );
         T_LogPSIZEL16( Parm3 );
      }
      HOOKENTRY16( szGpi16SetPickApertureSize, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETPICKAPERTURESIZE( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPickApertureSize );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetPickApertureSize, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetPickApertureSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPickApertureSize()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETPICKAPERTURESIZE( HPS, LONG, PSIZEL );
PCHAR szGpi32SetPickApertureSize = "Gpi32SetPickApertureSize";

BOOL APIENTRY Trc_Gpi32SetPickApertureSize( HPS    Parm1
                                          , LONG   Parm2
                                          , PSIZEL Parm3
                                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPickApertureSize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSIZEL );
         T_LogPSIZEL( Parm3 );
      }
      HOOKENTRY( szGpi32SetPickApertureSize, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETPICKAPERTURESIZE( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPickApertureSize );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetPickApertureSize, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetPickApertureSize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetPS()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETPS( HPS, PSIZEL16, ULONG );
PCHAR szGpi16SetPS = "Gpi16SetPS";

BOOL16 APIENTRY16 Trc_Gpi16SetPS( HPS      Parm1
                                , PSIZEL16 Parm2
                                , ULONG    Parm3
                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPSIZEL16( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16SetPS, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETPS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetPS );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetPS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetPS()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETPS( HPS, PSIZEL, ULONG );
PCHAR szGpi32SetPS = "Gpi32SetPS";

BOOL APIENTRY Trc_Gpi32SetPS( HPS    Parm1
                            , PSIZEL Parm2
                            , ULONG  Parm3
                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSIZEL );
         T_LogPSIZEL( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32SetPS, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETPS( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetPS );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetPS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetRegion()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETREGION( HPS, HRGN, LONG, PRECTL16 );
PCHAR szGpi16SetRegion = "Gpi16SetRegion";

BOOL16 APIENTRY16 Trc_Gpi16SetRegion( HPS      Parm1
                                    , HRGN     Parm2
                                    , LONG     Parm3
                                    , PRECTL16 Parm4
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogRECTLBuffer16( Parm4, Parm3 );
      }
      HOOKENTRY16( szGpi16SetRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETREGION( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetRegion );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetRegion()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETREGION( HPS, HRGN, LONG, PRECTL );
PCHAR szGpi32SetRegion = "Gpi32SetRegion";

BOOL APIENTRY Trc_Gpi32SetRegion( HPS    Parm1
                                , HRGN   Parm2
                                , LONG   Parm3
                                , PRECTL Parm4
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogRECTLBuffer( Parm4, Parm3 );
      }
      HOOKENTRY( szGpi32SetRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETREGION( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_RGN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetRegion );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetSegmentAttrs()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETSEGMENTATTRS( HPS, LONG, LONG, LONG );
PCHAR szGpi16SetSegmentAttrs = "Gpi16SetSegmentAttrs";

BOOL16 APIENTRY16 Trc_Gpi16SetSegmentAttrs( HPS  Parm1
                                          , LONG Parm2
                                          , LONG Parm3
                                          , LONG Parm4
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetSegmentAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szGpi16SetSegmentAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETSEGMENTATTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetSegmentAttrs );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetSegmentAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetSegmentAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetSegmentAttrs()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETSEGMENTATTRS( HPS, LONG, LONG, LONG );
PCHAR szGpi32SetSegmentAttrs = "Gpi32SetSegmentAttrs";

BOOL APIENTRY Trc_Gpi32SetSegmentAttrs( HPS  Parm1
                                      , LONG Parm2
                                      , LONG Parm3
                                      , LONG Parm4
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetSegmentAttrs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szGpi32SetSegmentAttrs, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETSEGMENTATTRS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetSegmentAttrs );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetSegmentAttrs, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetSegmentAttrs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetSegmentPriority()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETSEGMENTPRIORITY( HPS, LONG, LONG, LONG );
PCHAR szGpi16SetSegmentPriority = "Gpi16SetSegmentPriority";

BOOL16 APIENTRY16 Trc_Gpi16SetSegmentPriority( HPS  Parm1
                                             , LONG Parm2
                                             , LONG Parm3
                                             , LONG Parm4
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetSegmentPriority );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szGpi16SetSegmentPriority, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETSEGMENTPRIORITY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetSegmentPriority );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetSegmentPriority, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetSegmentPriority() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetSegmentPriority()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETSEGMENTPRIORITY( HPS, LONG, LONG, LONG );
PCHAR szGpi32SetSegmentPriority = "Gpi32SetSegmentPriority";

BOOL APIENTRY Trc_Gpi32SetSegmentPriority( HPS  Parm1
                                         , LONG Parm2
                                         , LONG Parm3
                                         , LONG Parm4
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetSegmentPriority );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szGpi32SetSegmentPriority, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETSEGMENTPRIORITY( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_SEG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetSegmentPriority );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetSegmentPriority, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetSegmentPriority() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetSegmentTransformMatrix()                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETSEGMENTTRANSFORMMATRIX( HPS, LONG, LONG, PMATRIXLF16, LONG );
PCHAR szGpi16SetSegmentTransformMatrix = "Gpi16SetSegmentTransformMatrix";

BOOL16 APIENTRY16 Trc_Gpi16SetSegmentTransformMatrix( HPS         Parm1
                                                    , LONG        Parm2
                                                    , LONG        Parm3
                                                    , PMATRIXLF16 Parm4
                                                    , LONG        Parm5
                                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetSegmentTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szGpi16SetSegmentTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETSEGMENTTRANSFORMMATRIX( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetSegmentTransformMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetSegmentTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetSegmentTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetSegmentTransformMatrix()                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETSEGMENTTRANSFORMMATRIX( HPS, LONG, LONG, PMATRIXLF, LONG );
PCHAR szGpi32SetSegmentTransformMatrix = "Gpi32SetSegmentTransformMatrix";

BOOL APIENTRY Trc_Gpi32SetSegmentTransformMatrix( HPS       Parm1
                                                , LONG      Parm2
                                                , LONG      Parm3
                                                , PMATRIXLF Parm4
                                                , LONG      Parm5
                                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetSegmentTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32SetSegmentTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETSEGMENTTRANSFORMMATRIX( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetSegmentTransformMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetSegmentTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetSegmentTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetStopDraw()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETSTOPDRAW( HPS, LONG );
PCHAR szGpi16SetStopDraw = "Gpi16SetStopDraw";

BOOL16 APIENTRY16 Trc_Gpi16SetStopDraw( HPS  Parm1
                                      , LONG Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetStopDraw );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetStopDraw, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETSTOPDRAW( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetStopDraw );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetStopDraw, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetStopDraw() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetStopDraw()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETSTOPDRAW( HPS, LONG );
PCHAR szGpi32SetStopDraw = "Gpi32SetStopDraw";

BOOL APIENTRY Trc_Gpi32SetStopDraw( HPS  Parm1
                                  , LONG Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetStopDraw );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetStopDraw, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETSTOPDRAW( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CTRL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetStopDraw );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetStopDraw, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetStopDraw() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetTag()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETTAG( HPS, LONG );
PCHAR szGpi16SetTag = "Gpi16SetTag";

BOOL16 APIENTRY16 Trc_Gpi16SetTag( HPS  Parm1
                                 , LONG Parm2
                                 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetTag );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szGpi16SetTag, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETTAG( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetTag );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetTag, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetTag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetTag()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETTAG( HPS, LONG );
PCHAR szGpi32SetTag = "Gpi32SetTag";

BOOL APIENTRY Trc_Gpi32SetTag( HPS  Parm1
                             , LONG Parm2
                             )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetTag );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szGpi32SetTag, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETTAG( Parm1, Parm2 );

   if ( LOGGPIAPI( G_CORR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetTag );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetTag, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetTag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetTextAlignment()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETTEXTALIGNMENT( HPS, LONG, LONG );
PCHAR szGpi32SetTextAlignment = "Gpi32SetTextAlignment";

BOOL APIENTRY Trc_Gpi32SetTextAlignment( HPS  Parm1
                                       , LONG Parm2
                                       , LONG Parm3
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetTextAlignment );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32SetTextAlignment, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETTEXTALIGNMENT( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetTextAlignment );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetTextAlignment, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetTextAlignment() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetViewingLimits()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETVIEWINGLIMITS( HPS, PRECTL16 );
PCHAR szGpi16SetViewingLimits = "Gpi16SetViewingLimits";

BOOL16 APIENTRY16 Trc_Gpi16SetViewingLimits( HPS      Parm1
                                           , PRECTL16 Parm2
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetViewingLimits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szGpi16SetViewingLimits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETVIEWINGLIMITS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetViewingLimits );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetViewingLimits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetViewingLimits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetViewingLimits()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETVIEWINGLIMITS( HPS, PRECTL );
PCHAR szGpi32SetViewingLimits = "Gpi32SetViewingLimits";

BOOL APIENTRY Trc_Gpi32SetViewingLimits( HPS    Parm1
                                       , PRECTL Parm2
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetViewingLimits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szGpi32SetViewingLimits, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETVIEWINGLIMITS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetViewingLimits );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetViewingLimits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetViewingLimits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetViewingTransformMatrix()                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISETVIEWINGTRANSFORMMATRIX( HPS, LONG, PMATRIXLF16, LONG );
PCHAR szGpi16SetViewingTransformMatrix = "Gpi16SetViewingTransformMatrix";

BOOL16 APIENTRY16 Trc_Gpi16SetViewingTransformMatrix( HPS         Parm1
                                                    , LONG        Parm2
                                                    , PMATRIXLF16 Parm3
                                                    , LONG        Parm4
                                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SetViewingTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szGpi16SetViewingTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISETVIEWINGTRANSFORMMATRIX( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SetViewingTransformMatrix );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SetViewingTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SetViewingTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetViewingTransformMatrix()                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SETVIEWINGTRANSFORMMATRIX( HPS, LONG, PMATRIXLF, LONG );
PCHAR szGpi32SetViewingTransformMatrix = "Gpi32SetViewingTransformMatrix";

BOOL APIENTRY Trc_Gpi32SetViewingTransformMatrix( HPS       Parm1
                                                , LONG      Parm2
                                                , PMATRIXLF Parm3
                                                , LONG      Parm4
                                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SetViewingTransformMatrix );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szGpi32SetViewingTransformMatrix, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SETVIEWINGTRANSFORMMATRIX( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SetViewingTransformMatrix );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SetViewingTransformMatrix, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SetViewingTransformMatrix() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32StrokeInkPath()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG APIENTRY GPISTROKEINKPATH( HPS, LONG, LONG, PPOINTL, ULONG );
PCHAR szGpi32StrokeInkPath = "Gpi32StrokeInkPath";

LONG APIENTRY Trc_Gpi32StrokeInkPath( HPS     Parm1
                                    , LONG    Parm2
                                    , LONG    Parm3
                                    , PPOINTL Parm4
                                    , ULONG   Parm5
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_INK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32StrokeInkPath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm4, Parm3 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szGpi32StrokeInkPath, &Parm1 );
      T_FreeLog( );
   }

   lRC = ( *pfnGPI32STROKEINKPATH )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_INK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32StrokeInkPath );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32StrokeInkPath, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32StrokeInkPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16StrokePath()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPISTROKEPATH( HPS, LONG, ULONG );
PCHAR szGpi16StrokePath = "Gpi16StrokePath";

LONG APIENTRY16 Trc_Gpi16StrokePath( HPS   Parm1
                                   , LONG  Parm2
                                   , ULONG Parm3
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16StrokePath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szGpi16StrokePath, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPISTROKEPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16StrokePath );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16StrokePath, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16StrokePath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32StrokePath()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32STROKEPATH( HPS, LONG, ULONG );
PCHAR szGpi32StrokePath = "Gpi32StrokePath";

LONG APIENTRY Trc_Gpi32StrokePath( HPS   Parm1
                                 , LONG  Parm2
                                 , ULONG Parm3
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32StrokePath );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szGpi32StrokePath, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32STROKEPATH( Parm1, Parm2, Parm3 );

   if ( LOGGPIAPI( G_PATH ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32StrokePath );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32StrokePath, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32StrokePath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SuspendPlay()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPISUSPENDPLAY( HPS );
PCHAR szGpi16SuspendPlay = "Gpi16SuspendPlay";

BOOL16 APIENTRY16 Trc_Gpi16SuspendPlay( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16SuspendPlay );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16SuspendPlay, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPISUSPENDPLAY( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16SuspendPlay );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16SuspendPlay, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16SuspendPlay() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SuspendPlay()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32SUSPENDPLAY( HPS );
PCHAR szGpi32SuspendPlay = "Gpi32SuspendPlay";

BOOL APIENTRY Trc_Gpi32SuspendPlay( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32SuspendPlay );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szGpi32SuspendPlay, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32SUSPENDPLAY( Parm1 );

   if ( LOGGPIAPI( G_META ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32SuspendPlay );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32SuspendPlay, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32SuspendPlay() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32TabbedCharStringAt()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG APIENTRY GPI32TABBEDCHARSTRINGAT( HPS, PPOINTL, PRECTL, ULONG, LONG, PCH, LONG, PULONG, LONG );
PCHAR szGpi32TabbedCharStringAt = "Gpi32TabbedCharStringAt";

LONG APIENTRY Trc_Gpi32TabbedCharStringAt( HPS     Parm1
                                         , PPOINTL Parm2
                                         , PRECTL  Parm3
                                         , ULONG   Parm4
                                         , LONG    Parm5
                                         , PCH     Parm6
                                         , LONG    Parm7
                                         , PULONG  Parm8
                                         , LONG    Parm9
                                         )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32TabbedCharStringAt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         if ( FLAGSET( Parm4, CHS_OPAQUE | CHS_CLIP ) ) T_LogPRECTL( Parm3 );
         else T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PCH );
         T_LogCHARBuffer( Parm6, Parm5 );
         T_LogParameter( 7L, DT_LONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_PULONG );
         T_LogULONGBuffer( Parm8, Parm7 );
         T_LogParameter( 9L, DT_LONG );
         T_LogULONG( Parm9 );
      }
      HOOKENTRY( szGpi32TabbedCharStringAt, &Parm1 );
      T_FreeLog( );
   }

   lRC = ( *pfnGPI32TABBEDCHARSTRINGAT )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGGPIAPI( G_PRIM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32TabbedCharStringAt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32TabbedCharStringAt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32TabbedCharStringAt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16Translate()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPITRANSLATE( HPS, PMATRIXLF16, LONG, PPOINTL16 );
PCHAR szGpi16Translate = "Gpi16Translate";

BOOL16 APIENTRY16 Trc_Gpi16Translate( HPS         Parm1
                                    , PMATRIXLF16 Parm2
                                    , LONG        Parm3
                                    , PPOINTL16   Parm4
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16Translate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMATRIXLF );
         T_LogPMATRIXLF16( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPPOINTL16( Parm4 );
      }
      HOOKENTRY16( szGpi16Translate, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPITRANSLATE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16Translate );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szGpi16Translate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16Translate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32Translate()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32TRANSLATE( HPS, PMATRIXLF, LONG, PPOINTL );
PCHAR szGpi32Translate = "Gpi32Translate";

BOOL APIENTRY Trc_Gpi32Translate( HPS       Parm1
                                , PMATRIXLF Parm2
                                , LONG      Parm3
                                , PPOINTL   Parm4
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32Translate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMATRIXLF );
         T_LogPMATRIXLF( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPPOINTL( Parm4 );
      }
      HOOKENTRY( szGpi32Translate, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32TRANSLATE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGGPIAPI( G_TRAN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32Translate );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMATRIXLF );
         if ( bRC ) T_LogPMATRIXLF( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szGpi32Translate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32Translate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16UnloadFonts()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIUNLOADFONTS( HAB, PSZ16 );
PCHAR szGpi16UnloadFonts = "Gpi16UnloadFonts";

BOOL16 APIENTRY16 Trc_Gpi16UnloadFonts( HAB   Parm1
                                      , PSZ16 Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16UnloadFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szGpi16UnloadFonts, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIUNLOADFONTS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16UnloadFonts );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16UnloadFonts, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16UnloadFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32UnloadFonts()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32UNLOADFONTS( HAB, PSZ );
PCHAR szGpi32UnloadFonts = "Gpi32UnloadFonts";

BOOL APIENTRY Trc_Gpi32UnloadFonts( HAB Parm1
                                  , PSZ Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32UnloadFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szGpi32UnloadFonts, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32UNLOADFONTS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32UnloadFonts );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32UnloadFonts, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32UnloadFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16UnloadPublicFonts()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIUNLOADPUBLICFONTS( HAB, PSZ16 );
PCHAR szGpi16UnloadPublicFonts = "Gpi16UnloadPublicFonts";

BOOL16 APIENTRY16 Trc_Gpi16UnloadPublicFonts( HAB   Parm1
                                            , PSZ16 Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16UnloadPublicFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szGpi16UnloadPublicFonts, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIUNLOADPUBLICFONTS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16UnloadPublicFonts );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16UnloadPublicFonts, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16UnloadPublicFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32UnloadPublicFonts()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY GPI32UNLOADPUBLICFONTS( HAB, PSZ );
PCHAR szGpi32UnloadPublicFonts = "Gpi32UnloadPublicFonts";

BOOL APIENTRY Trc_Gpi32UnloadPublicFonts( HAB Parm1
                                        , PSZ Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32UnloadPublicFonts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szGpi32UnloadPublicFonts, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPI32UNLOADPUBLICFONTS( Parm1, Parm2 );

   if ( LOGGPIAPI( G_LCID ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32UnloadPublicFonts );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szGpi32UnloadPublicFonts, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi32UnloadPublicFonts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16UnrealizeColorTable()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 GPIUNREALIZECOLORTABLE( HPS );
PCHAR szGpi16UnrealizeColorTable = "Gpi16UnrealizeColorTable";

BOOL16 APIENTRY16 Trc_Gpi16UnrealizeColorTable( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16UnrealizeColorTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szGpi16UnrealizeColorTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = GPIUNREALIZECOLORTABLE( Parm1 );

   if ( LOGGPIAPI( G_LCT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16UnrealizeColorTable );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szGpi16UnrealizeColorTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Gpi16UnrealizeColorTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16WCBitBlt()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 GPIWCBITBLT( HPS, HBITMAP, LONG, PPOINTL16, LONG, ULONG );
PCHAR szGpi16WCBitBlt = "Gpi16WCBitBlt";

LONG APIENTRY16 Trc_Gpi16WCBitBlt( HPS       Parm1
                                 , HBITMAP   Parm2
                                 , LONG      Parm3
                                 , PPOINTL16 Parm4
                                 , LONG      Parm5
                                 , ULONG     Parm6
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16WCBitBlt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm4, Parm3 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY16( szGpi16WCBitBlt, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPIWCBITBLT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16WCBitBlt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT16( szGpi16WCBitBlt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi16WCBitBlt() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32WCBitBlt()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY GPI32WCBITBLT( HPS, HBITMAP, LONG, PPOINTL, LONG, ULONG );
PCHAR szGpi32WCBitBlt = "Gpi32WCBitBlt";

LONG APIENTRY Trc_Gpi32WCBitBlt( HPS     Parm1
                               , HBITMAP Parm2
                               , LONG    Parm3
                               , PPOINTL Parm4
                               , LONG    Parm5
                               , ULONG   Parm6
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szGpi32WCBitBlt );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm4, Parm3 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szGpi32WCBitBlt, &Parm1 );
      T_FreeLog( );
   }

   lRC = GPI32WCBITBLT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi32WCBitBlt );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != GPI_ERROR );
      HOOKEXIT( szGpi32WCBitBlt, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Gpi32WCBitBlt() */
#endif

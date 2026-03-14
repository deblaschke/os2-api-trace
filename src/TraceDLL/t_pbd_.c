/******************************************************************************/
/*                                                                            */
/* File name    : T_PBD_.C                                                    */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit PMBIDI routines        */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from PMBIDI.DLL.     */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.75 28Oct98 Added Gpi/Win APIs exported from PMBIDI     */
/*                2.40.76 28Oct98 Issued more specific trace DLL load errors  */
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

#if ( OS2VER >= 230 )
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#define ORD_GPI16QUERYBIDIATTR        41         /* OS/2 2.30 proc ordinal    */
#define ORD_GPI16SETBIDIATTR          40         /* OS/2 2.30 proc ordinal    */
#endif
#define ORD_GPI32QUERYBIDIATTR        51         /* OS/2 2.30 proc ordinal    */
#define ORD_GPI32SETBIDIATTR          50         /* OS/2 2.30 proc ordinal    */
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
#define ORD_WIN16QUERYKBDLAYER        13         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN16QUERYLANGINFO        11         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN16QUERYLANGVIEWER      15         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN16SETKBDLAYER          12         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN16SETLANGINFO          10         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN16SETLANGVIEWER        14         /* OS/2 2.30 proc ordinal    */
#endif
#define ORD_WIN32QUERYKBDLAYER        23         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN32QUERYLANGINFO        21         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN32QUERYLANGVIEWER      25         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN32SETKBDLAYER          22         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN32SETLANGINFO          20         /* OS/2 2.30 proc ordinal    */
#define ORD_WIN32SETLANGVIEWER        24         /* OS/2 2.30 proc ordinal    */
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
PCHAR   szDLL = "PMBIDI";                        /* DLL module name           */
PCHAR   szEmpty = "";                            /* Empty string              */
PCHAR   szTraceDLL = "T_PBD_";                   /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
#ifdef T_16BIT
ULONG (* _Seg16 APIENTRY16 pfnGPI16QUERYBIDIATTR)( HPS );
                                                 /* OS/2 2.30 procedure       */
ULONG (* _Seg16 APIENTRY16 pfnGPI16SETBIDIATTR)( HPS, ULONG );
                                                 /* OS/2 2.30 procedure       */
ULONG (* _Seg16 APIENTRY16 pfnWIN16QUERYKBDLAYER)( HWND );
                                                 /* OS/2 2.30 procedure       */
ULONG (* _Seg16 APIENTRY16 pfnWIN16QUERYLANGINFO)( HWND, ULONG, ULONG, ULONG );
                                                 /* OS/2 2.30 procedure       */
HWND  (* _Seg16 APIENTRY16 pfnWIN16QUERYLANGVIEWER)( HAB, ULONG );
                                                 /* OS/2 2.30 procedure       */
ULONG (* _Seg16 APIENTRY16 pfnWIN16SETKBDLAYER)( HWND, ULONG, ULONG );
                                                 /* OS/2 2.30 procedure       */
ULONG (* _Seg16 APIENTRY16 pfnWIN16SETLANGINFO)( HWND, ULONG, ULONG, ULONG, ULONG, ULONG );
                                                 /* OS/2 2.30 procedure       */
HWND  (* _Seg16 APIENTRY16 pfnWIN16SETLANGVIEWER)( HAB, HWND, ULONG );
                                                 /* OS/2 2.30 procedure       */
#endif
ULONG (* APIENTRY          pfnGPI32QUERYBIDIATTR)( HPS );
                                                 /* OS/2 2.30 procedure       */
ULONG (* APIENTRY          pfnGPI32SETBIDIATTR)( HPS, ULONG );
                                                 /* OS/2 2.30 procedure       */
ULONG (* APIENTRY          pfnWIN32QUERYKBDLAYER)( HWND );
                                                 /* OS/2 2.30 procedure       */
ULONG (* APIENTRY          pfnWIN32QUERYLANGINFO)( HWND, ULONG, ULONG, ULONG );
                                                 /* OS/2 2.30 procedure       */
HWND (* APIENTRY           pfnWIN32QUERYLANGVIEWER)( HAB, ULONG );
                                                 /* OS/2 2.30 procedure       */
ULONG (* APIENTRY          pfnWIN32SETKBDLAYER)( HWND, ULONG, ULONG );
                                                 /* OS/2 2.30 procedure       */
ULONG (* APIENTRY          pfnWIN32SETLANGINFO)( HWND, ULONG, ULONG, ULONG, ULONG, ULONG );
                                                 /* OS/2 2.30 procedure       */
HWND (* APIENTRY           pfnWIN32SETLANGVIEWER)( HAB, HWND, ULONG );
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
    * Library initialization required if process attaching to T_PBD_
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
       * Log error and terminate if T_PBD_ version does not match T_COMMON
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
       * Load PMBIDI library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_PBD_ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMBIDI trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
      /*
       * Obtain OS/2 2.30 procedure addresses if OS/2 version number greater
       * than or equal to 2.30
       */
      if ( T_OS2Ver >= MAKEVER( 2, 30 ) )
      {
#ifdef T_16BIT
         /*
          * Obtain 16-bit OS/2 2.30 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_GPI16QUERYBIDIATTR   ), (PPFN16)&pfnGPI16QUERYBIDIATTR   ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_GPI16SETBIDIATTR     ), (PPFN16)&pfnGPI16SETBIDIATTR     ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16QUERYKBDLAYER   ), (PPFN16)&pfnWIN16QUERYKBDLAYER   ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16QUERYLANGINFO   ), (PPFN16)&pfnWIN16QUERYLANGINFO   ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16QUERYLANGVIEWER ), (PPFN16)&pfnWIN16QUERYLANGVIEWER ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16SETKBDLAYER     ), (PPFN16)&pfnWIN16SETKBDLAYER     ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16SETLANGINFO     ), (PPFN16)&pfnWIN16SETLANGINFO     ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16SETLANGVIEWER   ), (PPFN16)&pfnWIN16SETLANGVIEWER   ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 16, 2, 30, szDLL, ulOrdinal );
         }
#endif

         /*
          * Obtain 32-bit OS/2 2.30 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32QUERYBIDIATTR  , NULL, (PFN *)&pfnGPI32QUERYBIDIATTR   ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_GPI32SETBIDIATTR    , NULL, (PFN *)&pfnGPI32SETBIDIATTR     ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32QUERYKBDLAYER  , NULL, (PFN *)&pfnWIN32QUERYKBDLAYER   ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32QUERYLANGINFO  , NULL, (PFN *)&pfnWIN32QUERYLANGINFO   ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32QUERYLANGVIEWER, NULL, (PFN *)&pfnWIN32QUERYLANGVIEWER ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32SETKBDLAYER    , NULL, (PFN *)&pfnWIN32SETKBDLAYER     ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32SETLANGINFO    , NULL, (PFN *)&pfnWIN32SETLANGINFO     ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32SETLANGVIEWER  , NULL, (PFN *)&pfnWIN32SETLANGVIEWER   ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 30, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_PBD_
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMBIDI library if library loaded
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
/* Trc_Gpi16QueryBidiAttr()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY16 GPI16QUERYBIDIATTR( HPS );
PCHAR szGpi16QueryBidiAttr = "Gpi16QueryBidiAttr";

ULONG APIENTRY16 Trc_Gpi16QueryBidiAttr( HPS Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szGpi16QueryBidiAttr );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPS );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szGpi16QueryBidiAttr, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnGPI16QUERYBIDIATTR )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szGpi16QueryBidiAttr );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ulRC ) { T_LogLineHeader( szEmpty ); T_LogPULONG16( (PULONG16)ulRC ); }
   }
   HOOKEXIT16( szGpi16QueryBidiAttr, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Gpi16QueryBidiAttr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32QueryBidiAttr()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY GPI32QUERYBIDIATTR( HPS );
PCHAR szGpi32QueryBidiAttr = "Gpi32QueryBidiAttr";

ULONG APIENTRY Trc_Gpi32QueryBidiAttr( HPS Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szGpi32QueryBidiAttr );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPS );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szGpi32QueryBidiAttr, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnGPI32QUERYBIDIATTR )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szGpi32QueryBidiAttr );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( ulRC ) { T_LogLineHeader( szEmpty ); T_LogPULONG( (PULONG)ulRC ); }
   }
   HOOKEXIT( szGpi32QueryBidiAttr, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Gpi32QueryBidiAttr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16SetBidiAttr()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY16 GPI16SETBIDIATTR( HPS, ULONG );
PCHAR szGpi16SetBidiAttr = "Gpi16SetBidiAttr";

ULONG APIENTRY16 Trc_Gpi16SetBidiAttr( HPS   Parm1
                                     , ULONG Parm2
                                     )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szGpi16SetBidiAttr );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPS );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPULONG16( (PULONG16)Parm2 );
   }
   HOOKENTRY16( szGpi16SetBidiAttr, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnGPI16SETBIDIATTR )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szGpi16SetBidiAttr );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT16( szGpi16SetBidiAttr, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Gpi16SetBidiAttr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi32SetBidiAttr()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY GPI32SETBIDIATTR( HPS, ULONG );
PCHAR szGpi32SetBidiAttr = "Gpi32SetBidiAttr";

ULONG APIENTRY Trc_Gpi32SetBidiAttr( HPS   Parm1
                                   , ULONG Parm2
                                   )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szGpi32SetBidiAttr );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPS );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPULONG( (PULONG)Parm2 );
   }
   HOOKENTRY( szGpi32SetBidiAttr, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnGPI32SETBIDIATTR )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szGpi32SetBidiAttr );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szGpi32SetBidiAttr, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Gpi32SetBidiAttr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryKbdLayer()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY16 WIN16QUERYKBDLAYER( HWND );
PCHAR szWin16QueryKbdLayer = "Win16QueryKbdLayer";

ULONG APIENTRY16 Trc_Win16QueryKbdLayer( HWND Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryKbdLayer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16QueryKbdLayer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN16QUERYKBDLAYER )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryKbdLayer );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT16( szWin16QueryKbdLayer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16QueryKbdLayer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryKbdLayer()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY WIN32QUERYKBDLAYER( HWND );
PCHAR szWin32QueryKbdLayer = "Win32QueryKbdLayer";

ULONG APIENTRY Trc_Win32QueryKbdLayer( HWND Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryKbdLayer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32QueryKbdLayer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN32QUERYKBDLAYER )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryKbdLayer );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szWin32QueryKbdLayer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QueryKbdLayer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryLangInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY16 WIN16QUERYLANGINFO( HWND, ULONG, ULONG, ULONG );
PCHAR szWin16QueryLangInfo = "Win16QueryLangInfo";

ULONG APIENTRY16 Trc_Win16QueryLangInfo( HAB   Parm1
                                       , ULONG Parm2
                                       , ULONG Parm3
                                       , ULONG Parm4
                                       )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryLangInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY16( szWin16QueryLangInfo, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN16QUERYLANGINFO )( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryLangInfo );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT16( szWin16QueryLangInfo, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16QueryLangInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryLangInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY WIN32QUERYLANGINFO( HWND, ULONG, ULONG, ULONG );
PCHAR szWin32QueryLangInfo = "Win32QueryLangInfo";

ULONG APIENTRY Trc_Win32QueryLangInfo( HAB   Parm1
                                     , ULONG Parm2
                                     , ULONG Parm3
                                     , ULONG Parm4
                                     )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryLangInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szWin32QueryLangInfo, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN32QUERYLANGINFO )( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryLangInfo );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szWin32QueryLangInfo, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QueryLangInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryLangViewer()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
HWND APIENTRY16 WIN16QUERYLANGVIEWER( HAB, ULONG );
PCHAR szWin16QueryLangViewer = "Win16QueryLangViewer";

HWND APIENTRY16 Trc_Win16QueryLangViewer( HAB   Parm1
                                        , ULONG Parm2
                                        )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryLangViewer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szWin16QueryLangViewer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN16QUERYLANGVIEWER )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryLangViewer );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   HOOKEXIT16( szWin16QueryLangViewer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16QueryLangViewer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryLangViewer()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
HWND APIENTRY WIN32QUERYLANGVIEWER( HAB, ULONG );
PCHAR szWin32QueryLangViewer = "Win32QueryLangViewer";

HWND APIENTRY Trc_Win32QueryLangViewer( HAB   Parm1
                                      , ULONG Parm2
                                      )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryLangViewer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szWin32QueryLangViewer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN32QUERYLANGVIEWER )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryLangViewer );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   HOOKEXIT( szWin32QueryLangViewer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QueryLangViewer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetKbdLayer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY16 WIN16SETKBDLAYER( HWND, ULONG, ULONG );
PCHAR szWin16SetKbdLayer = "Win16SetKbdLayer";

ULONG APIENTRY16 Trc_Win16SetKbdLayer( HWND  Parm1
                                     , ULONG Parm2
                                     , ULONG Parm3
                                     )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SetKbdLayer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY16( szWin16SetKbdLayer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN16SETKBDLAYER )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SetKbdLayer );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT16( szWin16SetKbdLayer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16SetKbdLayer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetKbdLayer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY WIN32SETKBDLAYER( HWND, ULONG, ULONG );
PCHAR szWin32SetKbdLayer = "Win32SetKbdLayer";

ULONG APIENTRY Trc_Win32SetKbdLayer( HWND  Parm1
                                   , ULONG Parm2
                                   , ULONG Parm3
                                   )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SetKbdLayer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32SetKbdLayer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN32SETKBDLAYER )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SetKbdLayer );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szWin32SetKbdLayer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32SetKbdLayer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetLangInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY16 WIN16SETLANGINFO( HWND, ULONG, ULONG, ULONG, ULONG, ULONG );
PCHAR szWin16SetLangInfo = "Win16SetLangInfo";

ULONG APIENTRY16 Trc_Win16SetLangInfo( HAB   Parm1
                                     , ULONG Parm2
                                     , ULONG Parm3
                                     , ULONG Parm4
                                     , ULONG Parm5
                                     , ULONG Parm6
                                     )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SetLangInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY16( szWin16SetLangInfo, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN16SETLANGINFO )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SetLangInfo );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT16( szWin16SetLangInfo, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16SetLangInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetLangInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY WIN32SETLANGINFO( HWND, ULONG, ULONG, ULONG, ULONG, ULONG );
PCHAR szWin32SetLangInfo = "Win32SetLangInfo";

ULONG APIENTRY Trc_Win32SetLangInfo( HAB   Parm1
                                   , ULONG Parm2
                                   , ULONG Parm3
                                   , ULONG Parm4
                                   , ULONG Parm5
                                   , ULONG Parm6
                                   )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SetLangInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szWin32SetLangInfo, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN32SETLANGINFO )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SetLangInfo );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szWin32SetLangInfo, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32SetLangInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetLangViewer()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
HWND APIENTRY16 WIN16SETLANGVIEWER( HAB, HWND, ULONG );
PCHAR szWin16SetLangViewer = "Win16SetLangViewer";

HWND APIENTRY16 Trc_Win16SetLangViewer( HAB   Parm1
                                      , HWND  Parm2
                                      , ULONG Parm3
                                      )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16SetLangViewer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY16( szWin16SetLangViewer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN16SETLANGVIEWER )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin16SetLangViewer );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   HOOKEXIT16( szWin16SetLangViewer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16SetLangViewer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetLangViewer()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
HWND APIENTRY WIN32SETLANGVIEWER( HAB, HWND, ULONG );
PCHAR szWin32SetLangViewer = "Win32SetLangViewer";

HWND APIENTRY Trc_Win32SetLangViewer( HAB   Parm1
                                    , HWND  Parm2
                                    , ULONG Parm3
                                    )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SetLangViewer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32SetLangViewer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWIN32SETLANGVIEWER )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SetLangViewer );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   HOOKEXIT( szWin32SetLangViewer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32SetLangViewer() */
#endif

/******************************************************************************/
/*                                                                            */
/* File name    : T_HLP__.C                                                   */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit HELPMGR routines       */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from HELPMGR.DLL.    */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.03 20Jul95 Added 32-bit Win APIs                       */
/*                2.30.08 11Aug95 Added 32-bit Ddf APIs                       */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
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
#define  INCL_DDF                                /* Include PM DDF functions  */
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
PCHAR   szDLL = "HELPMGR";                       /* DLL module name           */
PCHAR   szTraceDLL = "T_HLP__";                  /* Trace DLL module name     */
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
    * Library initialization required if process attaching to T_HLP__
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
       * Log error and terminate if T_HLP__ version does not match T_COMMON
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
       * Load HELPMGR library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_HLP__ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register HELPMGR trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );
   }

   /*
    * Library termination required if process detaching from T_HLP__
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload HELPMGR library if library loaded
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
/* Trc_Ddf32BeginList()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFBEGINLIST( HDDF, ULONG, ULONG, ULONG );
PCHAR szDdf32BeginList = "Ddf32BeginList";

BOOL APIENTRY Trc_Ddf32BeginList( HDDF  Parm1
                                , ULONG Parm2
                                , ULONG Parm3
                                , ULONG Parm4
                                )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32BeginList );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szDdf32BeginList, &Parm1 );
   T_FreeLog( );

   bRC = DDFBEGINLIST( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32BeginList );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32BeginList, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32BeginList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32Bitmap()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFBITMAP( HDDF, HBITMAP, ULONG );
PCHAR szDdf32Bitmap = "Ddf32Bitmap";

BOOL APIENTRY Trc_Ddf32Bitmap( HDDF    Parm1
                             , HBITMAP Parm2
                             , ULONG   Parm3
                             )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32Bitmap );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_HBITMAP );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szDdf32Bitmap, &Parm1 );
   T_FreeLog( );

   bRC = DDFBITMAP( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32Bitmap );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32Bitmap, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32Bitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32EndList()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFENDLIST( HDDF );
PCHAR szDdf32EndList = "Ddf32EndList";

BOOL APIENTRY Trc_Ddf32EndList( HDDF Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32EndList );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
   }
   HOOKENTRY( szDdf32EndList, &Parm1 );
   T_FreeLog( );

   bRC = DDFENDLIST( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32EndList );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32EndList, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32EndList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32HyperText()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFHYPERTEXT( HDDF, PSZ, PSZ, ULONG );
PCHAR szDdf32HyperText = "Ddf32HyperText";

BOOL APIENTRY Trc_Ddf32HyperText( HDDF  Parm1
                                , PSZ   Parm2
                                , PSZ   Parm3
                                , ULONG Parm4
                                )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32HyperText );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szDdf32HyperText, &Parm1 );
   T_FreeLog( );

   bRC = DDFHYPERTEXT( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32HyperText );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32HyperText, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32HyperText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32Inform()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFINFORM( HDDF, PSZ, ULONG );
PCHAR szDdf32Inform = "Ddf32Inform";

BOOL APIENTRY Trc_Ddf32Inform( HDDF  Parm1
                             , PSZ   Parm2
                             , ULONG Parm3
                             )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32Inform );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szDdf32Inform, &Parm1 );
   T_FreeLog( );

   bRC = DDFINFORM( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32Inform );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32Inform, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32Inform() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32Initialize()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HDDF APIENTRY DDFINITIALIZE( HWND, ULONG, ULONG );
PCHAR szDdf32Initialize = "Ddf32Initialize";

HDDF APIENTRY Trc_Ddf32Initialize( HWND  Parm1
                                 , ULONG Parm2
                                 , ULONG Parm3
                                 )
{

   HDDF ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32Initialize );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szDdf32Initialize, &Parm1 );
   T_FreeLog( );

   ulRC = DDFINITIALIZE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32Initialize );
   T_LogPMReturnCode( DT_HDDF, (ULONG)ulRC, (BOOL)ulRC );
   HOOKEXIT( szDdf32Initialize, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Ddf32Initialize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32ListItem()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFLISTITEM( HDDF, PSZ, PSZ );
PCHAR szDdf32ListItem = "Ddf32ListItem";

BOOL APIENTRY Trc_Ddf32ListItem( HDDF Parm1
                               , PSZ  Parm2
                               , PSZ  Parm3
                               )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32ListItem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
   }
   HOOKENTRY( szDdf32ListItem, &Parm1 );
   T_FreeLog( );

   bRC = DDFLISTITEM( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32ListItem );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32ListItem, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32ListItem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32Metafile()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFMETAFILE( HDDF, HMF, PRECTL );
PCHAR szDdf32Metafile = "Ddf32Metafile";

BOOL APIENTRY Trc_Ddf32Metafile( HDDF   Parm1
                               , HMF    Parm2
                               , PRECTL Parm3
                               )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32Metafile );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_HMF );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PRECTL );
      T_LogPRECTL( Parm3 );
   }
   HOOKENTRY( szDdf32Metafile, &Parm1 );
   T_FreeLog( );

   bRC = DDFMETAFILE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32Metafile );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32Metafile, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32Metafile() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32Para()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFPARA( HDDF );
PCHAR szDdf32Para = "Ddf32Para";

BOOL APIENTRY Trc_Ddf32Para( HDDF Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32Para );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
   }
   HOOKENTRY( szDdf32Para, &Parm1 );
   T_FreeLog( );

   bRC = DDFPARA( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32Para );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32Para, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32Para() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32SetColor()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFSETCOLOR( HDDF, COLOR, COLOR );
PCHAR szDdf32SetColor = "Ddf32SetColor";

BOOL APIENTRY Trc_Ddf32SetColor( HDDF  Parm1
                               , COLOR Parm2
                               , COLOR Parm3
                               )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32SetColor );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_COLOR );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_COLOR );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szDdf32SetColor, &Parm1 );
   T_FreeLog( );

   bRC = DDFSETCOLOR( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32SetColor );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32SetColor, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32SetColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32SetFont()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFSETFONT( HDDF, PSZ, ULONG, ULONG );
PCHAR szDdf32SetFont = "Ddf32SetFont";

BOOL APIENTRY Trc_Ddf32SetFont( HDDF  Parm1
                              , PSZ   Parm2
                              , ULONG Parm3
                              , ULONG Parm4
                              )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32SetFont );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szDdf32SetFont, &Parm1 );
   T_FreeLog( );

   bRC = DDFSETFONT( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32SetFont );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32SetFont, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32SetFont() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32SetFontStyle()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFSETFONTSTYLE( HDDF, ULONG );
PCHAR szDdf32SetFontStyle = "Ddf32SetFontStyle";

BOOL APIENTRY Trc_Ddf32SetFontStyle( HDDF  Parm1
                                   , ULONG Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32SetFontStyle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szDdf32SetFontStyle, &Parm1 );
   T_FreeLog( );

   bRC = DDFSETFONTSTYLE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32SetFontStyle );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32SetFontStyle, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32SetFontStyle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32SetFormat()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFSETFORMAT( HDDF, ULONG );
PCHAR szDdf32SetFormat = "Ddf32SetFormat";

BOOL APIENTRY Trc_Ddf32SetFormat( HDDF  Parm1
                                , ULONG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32SetFormat );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szDdf32SetFormat, &Parm1 );
   T_FreeLog( );

   bRC = DDFSETFORMAT( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32SetFormat );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32SetFormat, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32SetFormat() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32SetTextAlign()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFSETTEXTALIGN( HDDF, ULONG );
PCHAR szDdf32SetTextAlign = "Ddf32SetTextAlign";

BOOL APIENTRY Trc_Ddf32SetTextAlign( HDDF  Parm1
                                   , ULONG Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32SetTextAlign );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szDdf32SetTextAlign, &Parm1 );
   T_FreeLog( );

   bRC = DDFSETTEXTALIGN( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32SetTextAlign );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32SetTextAlign, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32SetTextAlign() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Ddf32Text()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DDFTEXT( HDDF, PSZ );
PCHAR szDdf32Text = "Ddf32Text";

BOOL APIENTRY Trc_Ddf32Text( HDDF Parm1
                           , PSZ  Parm2
                           )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDdf32Text );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDDF );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szDdf32Text, &Parm1 );
   T_FreeLog( );

   bRC = DDFTEXT( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDdf32Text );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDdf32Text, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Ddf32Text() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16AssociateHelpInstance()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINASSOCIATEHELPINSTANCE( HWND, HWND );
PCHAR szWin16AssociateHelpInstance = "Win16AssociateHelpInstance";

BOOL16 APIENTRY16 Trc_Win16AssociateHelpInstance( HWND Parm1
                                                , HWND Parm2
                                                )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16AssociateHelpInstance );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szWin16AssociateHelpInstance, &Parm1 );
   T_FreeLog( );

   bRC = WINASSOCIATEHELPINSTANCE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16AssociateHelpInstance );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16AssociateHelpInstance, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16AssociateHelpInstance() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32AssociateHelpInstance()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ASSOCIATEHELPINSTANCE( HWND, HWND );
PCHAR szWin32AssociateHelpInstance = "Win32AssociateHelpInstance";

BOOL APIENTRY Trc_Win32AssociateHelpInstance( HWND Parm1
                                            , HWND Parm2
                                            )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32AssociateHelpInstance );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szWin32AssociateHelpInstance, &Parm1 );
   T_FreeLog( );

   bRC = WIN32ASSOCIATEHELPINSTANCE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32AssociateHelpInstance );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32AssociateHelpInstance, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32AssociateHelpInstance() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateHelpInstance()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINCREATEHELPINSTANCE( HAB, PHELPINIT16 );
PCHAR szWin16CreateHelpInstance = "Win16CreateHelpInstance";

HWND APIENTRY16 Trc_Win16CreateHelpInstance( HAB         Parm1
                                           , PHELPINIT16 Parm2
                                           )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16CreateHelpInstance );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PHELPINIT );
      T_LogPHELPINIT16( Parm2 );
   }
   HOOKENTRY16( szWin16CreateHelpInstance, &Parm1 );
   T_FreeLog( );

   ulRC = WINCREATEHELPINSTANCE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16CreateHelpInstance );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PHELPINIT );
      if ( !ulRC ) T_LogPHELPINIT16( Parm2 );
      else T_LogPVOID16( Parm2 );
   }
   HOOKEXIT16( szWin16CreateHelpInstance, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16CreateHelpInstance() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateHelpInstance()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32CREATEHELPINSTANCE( HAB, PHELPINIT );
PCHAR szWin32CreateHelpInstance = "Win32CreateHelpInstance";

HWND APIENTRY Trc_Win32CreateHelpInstance( HAB       Parm1
                                         , PHELPINIT Parm2
                                         )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32CreateHelpInstance );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PHELPINIT );
      T_LogPHELPINIT( Parm2 );
   }
   HOOKENTRY( szWin32CreateHelpInstance, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32CREATEHELPINSTANCE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32CreateHelpInstance );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PHELPINIT );
      if ( !ulRC ) T_LogPHELPINIT( Parm2 );
      else T_LogPVOID( Parm2 );
   }
   HOOKEXIT( szWin32CreateHelpInstance, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32CreateHelpInstance() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateHelpTable()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCREATEHELPTABLE( HWND, PHELPTABLE16 );
PCHAR szWin16CreateHelpTable = "Win16CreateHelpTable";

BOOL16 APIENTRY16 Trc_Win16CreateHelpTable( HWND         Parm1
                                          , PHELPTABLE16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16CreateHelpTable );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PHELPTABLE );
      T_LogPHELPTABLE16( Parm2 );
   }
   HOOKENTRY16( szWin16CreateHelpTable, &Parm1 );
   T_FreeLog( );

   bRC = WINCREATEHELPTABLE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin16CreateHelpTable );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16CreateHelpTable, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16CreateHelpTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateHelpTable()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32CREATEHELPTABLE( HWND, PHELPTABLE );
PCHAR szWin32CreateHelpTable = "Win32CreateHelpTable";

BOOL APIENTRY Trc_Win32CreateHelpTable( HWND       Parm1
                                      , PHELPTABLE Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32CreateHelpTable );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PHELPTABLE );
      T_LogPHELPTABLE( Parm2 );
   }
   HOOKENTRY( szWin32CreateHelpTable, &Parm1 );
   T_FreeLog( );

   bRC = WIN32CREATEHELPTABLE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32CreateHelpTable );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32CreateHelpTable, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32CreateHelpTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyHelpInstance()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDESTROYHELPINSTANCE( HWND );
PCHAR szWin16DestroyHelpInstance = "Win16DestroyHelpInstance";

BOOL16 APIENTRY16 Trc_Win16DestroyHelpInstance( HWND Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16DestroyHelpInstance );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16DestroyHelpInstance, &Parm1 );
   T_FreeLog( );

   bRC = WINDESTROYHELPINSTANCE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16DestroyHelpInstance );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16DestroyHelpInstance, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16DestroyHelpInstance() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DestroyHelpInstance()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DESTROYHELPINSTANCE( HWND );
PCHAR szWin32DestroyHelpInstance = "Win32DestroyHelpInstance";

BOOL APIENTRY Trc_Win32DestroyHelpInstance( HWND Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DestroyHelpInstance );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32DestroyHelpInstance, &Parm1 );
   T_FreeLog( );

   bRC = WIN32DESTROYHELPINSTANCE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DestroyHelpInstance );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32DestroyHelpInstance, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32DestroyHelpInstance() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadHelpTable()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINLOADHELPTABLE( HWND, USHORT, HMODULE16 );
PCHAR szWin16LoadHelpTable = "Win16LoadHelpTable";

BOOL16 APIENTRY16 Trc_Win16LoadHelpTable( HWND      Parm1
                                        , USHORT    Parm2
                                        , HMODULE16 Parm3
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16LoadHelpTable );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_HMODULE );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szWin16LoadHelpTable, &Parm1 );
   T_FreeLog( );

   bRC = WINLOADHELPTABLE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin16LoadHelpTable );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szWin16LoadHelpTable, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win16LoadHelpTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadHelpTable()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32LOADHELPTABLE( HWND, ULONG, HMODULE );
PCHAR szWin32LoadHelpTable = "Win32LoadHelpTable";

BOOL APIENTRY Trc_Win32LoadHelpTable( HWND    Parm1
                                    , ULONG   Parm2
                                    , HMODULE Parm3
                                    )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32LoadHelpTable );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_HMODULE );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32LoadHelpTable, &Parm1 );
   T_FreeLog( );

   bRC = WIN32LOADHELPTABLE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32LoadHelpTable );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32LoadHelpTable, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32LoadHelpTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryHelpInstance()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYHELPINSTANCE( HWND );
PCHAR szWin16QueryHelpInstance = "Win16QueryHelpInstance";

HWND APIENTRY16 Trc_Win16QueryHelpInstance( HWND Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szWin16QueryHelpInstance );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szWin16QueryHelpInstance, &Parm1 );
   T_FreeLog( );

   ulRC = WINQUERYHELPINSTANCE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin16QueryHelpInstance );
   T_LogPMReturnCode( DT_HWND, ulRC, TRUE );
   HOOKEXIT16( szWin16QueryHelpInstance, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win16QueryHelpInstance() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryHelpInstance()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYHELPINSTANCE( HWND );
PCHAR szWin32QueryHelpInstance = "Win32QueryHelpInstance";

HWND APIENTRY Trc_Win32QueryHelpInstance( HWND Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryHelpInstance );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32QueryHelpInstance, &Parm1 );
   T_FreeLog( );

   ulRC = WIN32QUERYHELPINSTANCE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryHelpInstance );
   T_LogPMReturnCode( DT_HWND, ulRC, TRUE );
   HOOKEXIT( szWin32QueryHelpInstance, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QueryHelpInstance() */
#endif

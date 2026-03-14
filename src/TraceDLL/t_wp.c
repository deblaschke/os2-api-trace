/******************************************************************************/
/*                                                                            */
/* File name    : T_WP.C                                                      */
/*                                                                            */
/* Title        : OS/2 API trace DLL 32-bit PMWP routines                     */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 32-bit APIs imported from PMWP.DLL.                  */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.03 20Jul95 Added 32-bit Win APIs                       */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.06 12Mar97 Fixed Win32EnumObjectClasses bug            */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.76 28Oct98 Issued more specific trace DLL load errors  */
/*                2.45.10 02Aug99 Moved common strings into T_COMMON          */
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

/******************************************************************************/
/*                                                                            */
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
#define ORD_WINLOCKUPSYSTEM               283    /* OS/2 2.10 proc ordinal    */
#define ORD_WINUNLOCKSYSTEM               282    /* OS/2 2.10 proc ordinal    */
#endif
#if ( OS2VER >= 230 )
#define ORD_WINCOPYOBJECT                 288    /* OS/2 2.30 proc ordinal    */
#define ORD_WINCREATESHADOW               289    /* OS/2 2.30 proc ordinal    */
#define ORD_WINISSOMDDREADY               480    /* OS/2 2.30 proc ordinal    */
#define ORD_WINISWPDSERVERREADY           465    /* OS/2 2.30 proc ordinal    */
#define ORD_WINMOVEOBJECT                 287    /* OS/2 2.30 proc ordinal    */
#define ORD_WINOPENOBJECT                 286    /* OS/2 2.30 proc ordinal    */
#define ORD_WINQUERYACTIVEDESKTOPPATHNAME 262    /* OS/2 2.30 proc ordinal    */
#define ORD_WINQUERYOBJECTPATH            263    /* OS/2 2.30 proc ordinal    */
#define ORD_WINRESTARTSOMDD               464    /* OS/2 2.30 proc ordinal    */
#define ORD_WINRESTARTWPDSERVER           463    /* OS/2 2.30 proc ordinal    */
#define ORD_WINSAVEOBJECT                 285    /* OS/2 2.30 proc ordinal    */
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
PCHAR   szDLL = "PMWP";                          /* DLL module name           */
PCHAR   szTraceDLL = "T_WP";                     /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( OS2VER >= 210 )
BOOL (* APIENTRY    pfnWINLOCKUPSYSTEM)( HAB );
                                                 /* OS/2 2.10 procedure       */
BOOL (* APIENTRY    pfnWINUNLOCKSYSTEM)( HAB, PSZ );
                                                 /* OS/2 2.10 procedure       */
#endif
#if ( OS2VER >= 230 )
HOBJECT (* APIENTRY pfnWINCOPYOBJECT)( HOBJECT, HOBJECT, ULONG );
                                                 /* OS/2 2.30 procedure       */
HOBJECT (* APIENTRY pfnWINCREATESHADOW)( HOBJECT, HOBJECT, ULONG );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY    pfnWINISSOMDDREADY)( VOID );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY    pfnWINISWPDSERVERREADY)( VOID );
                                                 /* OS/2 2.30 procedure       */
HOBJECT (* APIENTRY pfnWINMOVEOBJECT)( HOBJECT, HOBJECT, ULONG );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY    pfnWINOPENOBJECT)( HOBJECT, ULONG, BOOL );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY    pfnWINQUERYACTIVEDESKTOPPATHNAME)( PSZ, ULONG );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY    pfnWINQUERYOBJECTPATH)( HOBJECT, PSZ, ULONG );
                                                 /* OS/2 2.30 procedure       */
APIRET (* APIENTRY  pfnWINRESTARTSOMDD)( BOOL );
                                                 /* OS/2 2.30 procedure       */
APIRET (* APIENTRY  pfnWINRESTARTWPDSERVER)( BOOL );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY    pfnWINSAVEOBJECT)( HOBJECT, BOOL );
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
    * Library initialization required if process attaching to T_WP
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
       * Log error and terminate if T_WP version does not match T_COMMON
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
       * Load PMWP library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_WP library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMWP trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

#if ( OS2VER >= 210 )
      /*
       * Obtain OS/2 2.10 procedure addresses if OS/2 version number greater
       * than or equal to 2.10
       */
      if ( T_OS2Ver >= MAKEVER( 2, 10 ) )
      {
         /*
          * Obtain OS/2 2.10 procedure addresses, log error and terminate if
          * unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINLOCKUPSYSTEM, NULL, (PFN *)&pfnWINLOCKUPSYSTEM ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINUNLOCKSYSTEM, NULL, (PFN *)&pfnWINUNLOCKSYSTEM ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 10, szDLL, ulOrdinal );
         }
      }
#endif

#if ( OS2VER >= 230 )
      /*
       * Obtain OS/2 2.30 procedure addresses if OS/2 version number greater
       * than or equal to 2.30
       */
      if ( T_OS2Ver >= MAKEVER( 2, 30 ) )
      {
         /*
          * Obtain OS/2 2.30 procedure addresses, log error and terminate if
          * unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINCOPYOBJECT                , NULL, (PFN *)&pfnWINCOPYOBJECT                 ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINCREATESHADOW              , NULL, (PFN *)&pfnWINCREATESHADOW               ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINISSOMDDREADY              , NULL, (PFN *)&pfnWINISSOMDDREADY               ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINISWPDSERVERREADY          , NULL, (PFN *)&pfnWINISWPDSERVERREADY           ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINMOVEOBJECT                , NULL, (PFN *)&pfnWINMOVEOBJECT                 ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINOPENOBJECT                , NULL, (PFN *)&pfnWINOPENOBJECT                 ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINQUERYACTIVEDESKTOPPATHNAME, NULL, (PFN *)&pfnWINQUERYACTIVEDESKTOPPATHNAME ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINQUERYOBJECTPATH           , NULL, (PFN *)&pfnWINQUERYOBJECTPATH            ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINRESTARTSOMDD              , NULL, (PFN *)&pfnWINRESTARTSOMDD               ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINRESTARTWPDSERVER          , NULL, (PFN *)&pfnWINRESTARTWPDSERVER           ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WINSAVEOBJECT                , NULL, (PFN *)&pfnWINSAVEOBJECT                 ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 30, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_WP
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMWP library if library loaded
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
/* Trc_Win32CopyObject()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
HOBJECT APIENTRY WINCOPYOBJECT( HOBJECT, HOBJECT, ULONG );
PCHAR szWin32CopyObject = "Win32CopyObject";

HOBJECT APIENTRY Trc_Win32CopyObject( HOBJECT Parm1
                                    , HOBJECT Parm2
                                    , ULONG   Parm3
                                    )
{

   HOBJECT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32CopyObject );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HOBJECT );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HOBJECT );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32CopyObject, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINCOPYOBJECT )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32CopyObject );
   T_LogPMReturnCode( DT_HOBJECT, ulRC, ulRC );
   HOOKEXIT( szWin32CopyObject, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32CopyObject() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateObject()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HOBJECT APIENTRY WINCREATEOBJECT( PSZ, PSZ, PSZ, PSZ, ULONG );
PCHAR szWin32CreateObject = "Win32CreateObject";

HOBJECT APIENTRY Trc_Win32CreateObject( PSZ   Parm1
                                      , PSZ   Parm2
                                      , PSZ   Parm3
                                      , PSZ   Parm4
                                      , ULONG Parm5
                                      )
{

   HOBJECT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32CreateObject );
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
   }
   HOOKENTRY( szWin32CreateObject, &Parm1 );
   T_FreeLog( );

   ulRC = WINCREATEOBJECT( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szWin32CreateObject );
   T_LogPMReturnCode( DT_HOBJECT,ulRC, ulRC );
   HOOKEXIT( szWin32CreateObject, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32CreateObject() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateShadow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
HOBJECT APIENTRY WINCREATESHADOW( HOBJECT, HOBJECT, ULONG );
PCHAR szWin32CreateShadow = "Win32CreateShadow";

HOBJECT APIENTRY Trc_Win32CreateShadow( HOBJECT Parm1
                                      , HOBJECT Parm2
                                      , ULONG   Parm3
                                      )
{

   HOBJECT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32CreateShadow );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HOBJECT );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HOBJECT );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32CreateShadow, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINCREATESHADOW )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32CreateShadow );
   T_LogPMReturnCode( DT_HOBJECT,ulRC, ulRC );
   HOOKEXIT( szWin32CreateShadow, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32CreateShadow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DeregisterObjectClass()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINDEREGISTEROBJECTCLASS( PSZ );
PCHAR szWin32DeregisterObjectClass = "Win32DeregisterObjectClass";

BOOL APIENTRY Trc_Win32DeregisterObjectClass( PSZ Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DeregisterObjectClass );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
   }
   HOOKENTRY( szWin32DeregisterObjectClass, &Parm1 );
   T_FreeLog( );

   bRC = WINDEREGISTEROBJECTCLASS( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DeregisterObjectClass );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32DeregisterObjectClass, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32DeregisterObjectClass() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DestroyObject()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINDESTROYOBJECT( HOBJECT );
PCHAR szWin32DestroyObject = "Win32DestroyObject";

BOOL APIENTRY Trc_Win32DestroyObject( HOBJECT Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32DestroyObject );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HOBJECT );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32DestroyObject, &Parm1 );
   T_FreeLog( );

   bRC = WINDESTROYOBJECT( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32DestroyObject );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32DestroyObject, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32DestroyObject() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EnumObjectClasses()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINENUMOBJECTCLASSES( POBJCLASS, PULONG );
PCHAR szWin32EnumObjectClasses = "Win32EnumObjectClasses";

BOOL APIENTRY Trc_Win32EnumObjectClasses( POBJCLASS Parm1
                                        , PULONG    Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32EnumObjectClasses );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_POBJCLASS );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_PULONG );
      T_LogPULONG( Parm2 );
   }
   HOOKENTRY( szWin32EnumObjectClasses, &Parm1 );
   T_FreeLog( );

   bRC = WINENUMOBJECTCLASSES( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32EnumObjectClasses );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_POBJCLASS );
      if ( bRC ) T_LogPOBJCLASS( Parm1 );
      else T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_PULONG );
      T_LogPULONG( Parm2 );
   }
   HOOKEXIT( szWin32EnumObjectClasses, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32EnumObjectClasses() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FreeFileIcon()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINFREEFILEICON( HPOINTER );
PCHAR szWin32FreeFileIcon = "Win32FreeFileIcon";

BOOL APIENTRY Trc_Win32FreeFileIcon( HPOINTER Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32FreeFileIcon );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPOINTER );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32FreeFileIcon, &Parm1 );
   T_FreeLog( );

   bRC = WINFREEFILEICON( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32FreeFileIcon );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32FreeFileIcon, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32FreeFileIcon() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsSOMDDReady()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY WINISSOMDDREADY( VOID );
PCHAR szWin32IsSOMDDReady = "Win32IsSOMDDReady";

BOOL APIENTRY Trc_Win32IsSOMDDReady( VOID )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32IsSOMDDReady );
   HOOKENTRY( szWin32IsSOMDDReady, NULL );
   T_FreeLog( );

   bRC = ( *pfnWINISSOMDDREADY )( );

   T_WaitLog( );
   T_LogAPIExit( szWin32IsSOMDDReady );
   T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
   HOOKEXIT( szWin32IsSOMDDReady, NULL, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32IsSOMDDReady() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsWPDServerReady()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY WINISWPDSERVERREADY( VOID );
PCHAR szWin32IsWPDServerReady = "Win32IsWPDServerReady";

BOOL APIENTRY Trc_Win32IsWPDServerReady( VOID )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32IsWPDServerReady );
   HOOKENTRY( szWin32IsWPDServerReady, NULL );
   T_FreeLog( );

   bRC = ( *pfnWINISWPDSERVERREADY )( );

   T_WaitLog( );
   T_LogAPIExit( szWin32IsWPDServerReady );
   T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
   HOOKEXIT( szWin32IsWPDServerReady, NULL, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32IsWPDServerReady() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadFileIcon()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPOINTER APIENTRY WINLOADFILEICON( PSZ, BOOL );
PCHAR szWin32LoadFileIcon = "Win32LoadFileIcon";

HPOINTER APIENTRY Trc_Win32LoadFileIcon( PSZ  Parm1
                                       , BOOL Parm2
                                       )
{

   HPOINTER ulRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32LoadFileIcon );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2, DT_BOOL );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szWin32LoadFileIcon, &Parm1 );
   T_FreeLog( );

   ulRC = WINLOADFILEICON( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32LoadFileIcon );
   T_LogPMReturnCode( DT_HPOINTER,ulRC, ulRC );
   HOOKEXIT( szWin32LoadFileIcon, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32LoadFileIcon() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LockupSystem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
BOOL EXPENTRY WINLOCKUPSYSTEM( HAB );
PCHAR szWin32LockupSystem = "Win32LockupSystem";

BOOL EXPENTRY Trc_Win32LockupSystem( HAB Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32LockupSystem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32LockupSystem, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnWINLOCKUPSYSTEM )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32LockupSystem );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32LockupSystem, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32LockupSystem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MoveObject()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
HOBJECT APIENTRY WINMOVEOBJECT( HOBJECT, HOBJECT, ULONG );
PCHAR szWin32MoveObject = "Win32MoveObject";

HOBJECT APIENTRY Trc_Win32MoveObject( HOBJECT Parm1
                                    , HOBJECT Parm2
                                    , ULONG   Parm3
                                    )
{

   HOBJECT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32MoveObject );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HOBJECT );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HOBJECT );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32MoveObject, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINMOVEOBJECT )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32MoveObject );
   T_LogPMReturnCode( DT_HOBJECT, ulRC, ulRC );
   HOOKEXIT( szWin32MoveObject, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32MoveObject() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32OpenObject()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY WINOPENOBJECT( HOBJECT, ULONG, BOOL );
PCHAR szWin32OpenObject = "Win32OpenObject";

BOOL APIENTRY Trc_Win32OpenObject( HOBJECT Parm1
                                 , ULONG   Parm2
                                 , BOOL    Parm3
                                 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32OpenObject );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HOBJECT );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_BOOL );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32OpenObject, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnWINOPENOBJECT )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32OpenObject );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32OpenObject, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32OpenObject() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryActiveDesktopPathname()                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY WINQUERYACTIVEDESKTOPPATHNAME( PSZ, ULONG );
PCHAR szWin32QueryActiveDesktopPathname = "Win32QueryActiveDesktopPathname";

BOOL APIENTRY Trc_Win32QueryActiveDesktopPathname( PSZ   Parm1
                                                 , ULONG Parm2
                                                 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryActiveDesktopPathname );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPVOID( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szWin32QueryActiveDesktopPathname, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnWINQUERYACTIVEDESKTOPPATHNAME )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryActiveDesktopPathname );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      if ( bRC ) T_LogPSZ( Parm1 );
      else T_LogPVOID( Parm1 );
   }
   HOOKEXIT( szWin32QueryActiveDesktopPathname, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32QueryActiveDesktopPathname() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryObject()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HOBJECT APIENTRY WINQUERYOBJECT( PSZ );
PCHAR szWin32QueryObject = "Win32QueryObject";

HOBJECT APIENTRY Trc_Win32QueryObject( PSZ Parm1 )
{

   HOBJECT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryObject );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
   }
   HOOKENTRY( szWin32QueryObject, &Parm1 );
   T_FreeLog( );

   ulRC = WINQUERYOBJECT( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryObject );
   T_LogPMReturnCode( DT_HOBJECT,ulRC, ulRC );
   HOOKEXIT( szWin32QueryObject, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32QueryObject() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryObjectPath()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY WINQUERYOBJECTPATH( HOBJECT, PSZ, ULONG );
PCHAR szWin32QueryObjectPath = "Win32QueryObjectPath";

BOOL APIENTRY Trc_Win32QueryObjectPath( HOBJECT Parm1
                                      , PSZ     Parm2
                                      , ULONG   Parm3
                                      )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32QueryObjectPath );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HOBJECT );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32QueryObjectPath, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnWINQUERYOBJECTPATH )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32QueryObjectPath );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PSZ );
      if ( bRC ) T_LogPSZ( Parm2 );
      else T_LogPVOID( Parm2 );
   }
   HOOKEXIT( szWin32QueryObjectPath, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32QueryObjectPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RegisterObjectClass()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINREGISTEROBJECTCLASS( PSZ, PSZ );
PCHAR szWin32RegisterObjectClass = "Win32RegisterObjectClass";

BOOL APIENTRY Trc_Win32RegisterObjectClass( PSZ Parm1
                                          , PSZ Parm2
                                          )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32RegisterObjectClass );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szWin32RegisterObjectClass, &Parm1 );
   T_FreeLog( );

   bRC = WINREGISTEROBJECTCLASS( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32RegisterObjectClass );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32RegisterObjectClass, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32RegisterObjectClass() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ReplaceObjectClass()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINREPLACEOBJECTCLASS( PSZ, PSZ, BOOL );
PCHAR szWin32ReplaceObjectClass = "Win32ReplaceObjectClass";

BOOL APIENTRY Trc_Win32ReplaceObjectClass( PSZ  Parm1
                                         , PSZ  Parm2
                                         , BOOL Parm3
                                         )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32ReplaceObjectClass );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_BOOL );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32ReplaceObjectClass, &Parm1 );
   T_FreeLog( );

   bRC = WINREPLACEOBJECTCLASS( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32ReplaceObjectClass );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32ReplaceObjectClass, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32ReplaceObjectClass() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RestartSOMDD()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
APIRET APIENTRY WINRESTARTSOMDD( BOOL );
PCHAR szWin32RestartSOMDD = "Win32RestartSOMDD";

APIRET APIENTRY Trc_Win32RestartSOMDD( BOOL Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32RestartSOMDD );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1, DT_BOOL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32RestartSOMDD, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINRESTARTSOMDD )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32RestartSOMDD );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szWin32RestartSOMDD, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32RestartSOMDD() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RestartWPDServer()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
APIRET APIENTRY WINRESTARTWPDSERVER( BOOL );
PCHAR szWin32RestartWPDServer = "Win32RestartWPDServer";

APIRET APIENTRY Trc_Win32RestartWPDServer( BOOL Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32RestartWPDServer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1, DT_BOOL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szWin32RestartWPDServer, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnWINRESTARTWPDSERVER )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szWin32RestartWPDServer );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szWin32RestartWPDServer, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Win32RestartWPDServer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RestoreWindowPos()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINRESTOREWINDOWPOS( PSZ, PSZ, HWND );
PCHAR szWin32RestoreWindowPos = "Win32RestoreWindowPos";

BOOL APIENTRY Trc_Win32RestoreWindowPos( PSZ  Parm1
                                       , PSZ  Parm2
                                       , HWND Parm3
                                       )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32RestoreWindowPos );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_HWND );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32RestoreWindowPos, &Parm1 );
   T_FreeLog( );

   bRC = WINRESTOREWINDOWPOS( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32RestoreWindowPos );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32RestoreWindowPos, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32RestoreWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SaveObject()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY WINSAVEOBJECT( HOBJECT, BOOL );
PCHAR szWin32SaveObject = "Win32SaveObject";

BOOL APIENTRY Trc_Win32SaveObject( HOBJECT Parm1
                                 , BOOL    Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SaveObject );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HOBJECT );
      T_LogULONG( Parm1 );
      T_LogParameter( 2, DT_BOOL );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szWin32SaveObject, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnWINSAVEOBJECT )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SaveObject );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32SaveObject, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32SaveObject() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetFileIcon()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINSETFILEICON( PSZ, PICONINFO );
PCHAR szWin32SetFileIcon = "Win32SetFileIcon";

BOOL APIENTRY Trc_Win32SetFileIcon( PSZ       Parm1
                                  , PICONINFO Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SetFileIcon );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PICONINFO );
      T_LogPICONINFO( Parm2 );
   }
   HOOKENTRY( szWin32SetFileIcon, &Parm1 );
   T_FreeLog( );

   bRC = WINSETFILEICON( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SetFileIcon );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32SetFileIcon, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32SetFileIcon() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetObjectData()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINSETOBJECTDATA( HOBJECT, PSZ );
PCHAR szWin32SetObjectData = "Win32SetObjectData";

BOOL APIENTRY Trc_Win32SetObjectData( HOBJECT Parm1
                                    , PSZ     Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32SetObjectData );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HOBJECT );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szWin32SetObjectData, &Parm1 );
   T_FreeLog( );

   bRC = WINSETOBJECTDATA( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32SetObjectData );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32SetObjectData, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32SetObjectData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ShutdownSystem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINSHUTDOWNSYSTEM( HAB, HMQ );
PCHAR szWin32ShutdownSystem = "Win32ShutdownSystem";

BOOL APIENTRY Trc_Win32ShutdownSystem( HAB Parm1
                                     , HMQ Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32ShutdownSystem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_HMQ );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szWin32ShutdownSystem, &Parm1 );
   T_FreeLog( );

   bRC = WINSHUTDOWNSYSTEM( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32ShutdownSystem );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32ShutdownSystem, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32ShutdownSystem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32StoreWindowPos()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WINSTOREWINDOWPOS( PSZ, PSZ, HWND );
PCHAR szWin32StoreWindowPos = "Win32StoreWindowPos";

BOOL APIENTRY Trc_Win32StoreWindowPos( PSZ  Parm1
                                     , PSZ  Parm2
                                     , HWND Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32StoreWindowPos );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_HWND );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szWin32StoreWindowPos, &Parm1 );
   T_FreeLog( );

   bRC = WINSTOREWINDOWPOS( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szWin32StoreWindowPos );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32StoreWindowPos, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32StoreWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32UnlockSystem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
BOOL EXPENTRY WINUNLOCKSYSTEM( HAB, PSZ );
PCHAR szWin32UnlockSystem = "Win32UnlockSystem";

BOOL EXPENTRY Trc_Win32UnlockSystem( HAB Parm1
                                   , PSZ Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szWin32UnlockSystem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HAB );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szWin32UnlockSystem, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnWINUNLOCKSYSTEM )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szWin32UnlockSystem );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szWin32UnlockSystem, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Win32UnlockSystem() */
#endif

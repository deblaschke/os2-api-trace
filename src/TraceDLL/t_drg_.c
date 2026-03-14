/******************************************************************************/
/*                                                                            */
/* File name    : T_DRG_.C                                                    */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit PMDRAG routines        */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from PMDRAG.DLL.     */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.10 13Aug95 Added 32-bit Drg APIs                       */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.30.19 02Nov95 Fixed Drg32DragFiles bug                    */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.29 07Aug97 Added 16-bit Drg APIs                       */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.44 27Jan98 Fixed returned MRESULT thunk bug            */
/*                2.40.45 28Jan98 Added indeterminate API return codes        */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
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
#define ORD_DRG32CANCELLAZYDRAG               68 /* OS/2 2.30 proc ordinal    */
#define ORD_DRG32LAZYDRAG                     67 /* OS/2 2.30 proc ordinal    */
#define ORD_DRG32LAZYDROP                     69 /* OS/2 2.30 proc ordinal    */
#define ORD_DRG32QUERYDRAGINFOPTR             70 /* OS/2 2.30 proc ordinal    */
#define ORD_DRG32QUERYDRAGINFOPTRFROMDRAGITEM 72 /* OS/2 2.30 proc ordinal    */
#define ORD_DRG32QUERYDRAGINFOPTRFROMHWND     71 /* OS/2 2.30 proc ordinal    */
#define ORD_DRG32QUERYDRAGSTATUS              73 /* OS/2 2.30 proc ordinal    */
#define ORD_DRG32REALLOCDRAGINFO              74 /* OS/2 2.30 proc ordinal    */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
#define ORD_DRG32QUERYFORMAT                  75 /* OS/2 2.40 proc ordinal    */
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
PCHAR   szDLL = "PMDRAG";                        /* DLL module name           */
PCHAR   szTraceDLL = "T_DRG_";                   /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( OS2VER >= 230 )
BOOL (* APIENTRY      pfnDRG32CANCELLAZYDRAG)( VOID );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY      pfnDRG32LAZYDRAG)( HWND, PDRAGINFO, PDRAGIMAGE, ULONG, PVOID );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY      pfnDRG32LAZYDROP)( HWND, ULONG, PPOINTL );
                                                 /* OS/2 2.30 procedure       */
PDRAGINFO (* APIENTRY pfnDRG32QUERYDRAGINFOPTR)( PDRAGINFO );
                                                 /* OS/2 2.30 procedure       */
PDRAGINFO (* APIENTRY pfnDRG32QUERYDRAGINFOPTRFROMDRAGITEM)( PDRAGITEM );
                                                 /* OS/2 2.30 procedure       */
PDRAGINFO (* APIENTRY pfnDRG32QUERYDRAGINFOPTRFROMHWND)( HWND );
                                                 /* OS/2 2.30 procedure       */
ULONG (* APIENTRY     pfnDRG32QUERYDRAGSTATUS)( VOID );
                                                 /* OS/2 2.30 procedure       */
PDRAGINFO (* APIENTRY pfnDRG32REALLOCDRAGINFO)( PDRAGINFO, ULONG );
                                                 /* OS/2 2.30 procedure       */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG (* APIENTRY     pfnDRG32QUERYFORMAT)( PDRAGITEM , PSZ, ULONG, PSZ, ULONG );
                                                 /* OS/2 2.40 procedure       */
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
    * Library initialization required if process attaching to T_DRG_
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
       * Log error and terminate if T_DRG_ version does not match T_COMMON
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
       * Load PMDRAG library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_DRG_ library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMDRAG trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

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
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32CANCELLAZYDRAG              , NULL, (PFN *)&pfnDRG32CANCELLAZYDRAG               ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32LAZYDRAG                    , NULL, (PFN *)&pfnDRG32LAZYDRAG                     ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32LAZYDROP                    , NULL, (PFN *)&pfnDRG32LAZYDROP                     ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32QUERYDRAGINFOPTR            , NULL, (PFN *)&pfnDRG32QUERYDRAGINFOPTR             ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32QUERYDRAGINFOPTRFROMDRAGITEM, NULL, (PFN *)&pfnDRG32QUERYDRAGINFOPTRFROMDRAGITEM ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32QUERYDRAGINFOPTRFROMHWND    , NULL, (PFN *)&pfnDRG32QUERYDRAGINFOPTRFROMHWND     ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32QUERYDRAGSTATUS             , NULL, (PFN *)&pfnDRG32QUERYDRAGSTATUS              ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32REALLOCDRAGINFO             , NULL, (PFN *)&pfnDRG32REALLOCDRAGINFO              ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 30, szDLL, ulOrdinal );
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
         if ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DRG32QUERYFORMAT, NULL, (PFN *)&pfnDRG32QUERYFORMAT ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 40, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_DRAG_
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMDRAG library if library loaded
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
/* Trc_Drg16AcceptDroppedFiles()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16ACCEPTDROPPEDFILES( HWND, PSZ16, PSZ16, USHORT, USHORT );
PCHAR szDrg16AcceptDroppedFiles = "Drg16AcceptDroppedFiles";

BOOL16 APIENTRY16 Trc_Drg16AcceptDroppedFiles( HWND   Parm1
                                             , PSZ16  Parm2
                                             , PSZ16  Parm3
                                             , USHORT Parm4
                                             , USHORT Parm5
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16AcceptDroppedFiles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szDrg16AcceptDroppedFiles, &Parm1 );
   T_FreeLog( );

   bRC = DRG16ACCEPTDROPPEDFILES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16AcceptDroppedFiles );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16AcceptDroppedFiles, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16AcceptDroppedFiles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32AcceptDroppedFiles()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGACCEPTDROPPEDFILES( HWND, PSZ, PSZ, ULONG, ULONG );
PCHAR szDrg32AcceptDroppedFiles = "Drg32AcceptDroppedFiles";

BOOL APIENTRY Trc_Drg32AcceptDroppedFiles( HWND  Parm1
                                         , PSZ   Parm2
                                         , PSZ   Parm3
                                         , ULONG Parm4
                                         , ULONG Parm5
                                         )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32AcceptDroppedFiles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY( szDrg32AcceptDroppedFiles, &Parm1 );
   T_FreeLog( );

   bRC = DRGACCEPTDROPPEDFILES( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32AcceptDroppedFiles );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32AcceptDroppedFiles, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32AcceptDroppedFiles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16AccessDraginfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16ACCESSDRAGINFO( PDRAGINFO16 );
PCHAR szDrg16AccessDraginfo = "Drg16AccessDraginfo";

BOOL16 APIENTRY16 Trc_Drg16AccessDraginfo( PDRAGINFO16 Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16AccessDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
   }
   HOOKENTRY16( szDrg16AccessDraginfo, &Parm1 );
   T_FreeLog( );

   bRC = DRG16ACCESSDRAGINFO( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16AccessDraginfo );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16AccessDraginfo, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16AccessDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32AccessDraginfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGACCESSDRAGINFO( PDRAGINFO );
PCHAR szDrg32AccessDraginfo = "Drg32AccessDraginfo";

BOOL APIENTRY Trc_Drg32AccessDraginfo( PDRAGINFO Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32AccessDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
   }
   HOOKENTRY( szDrg32AccessDraginfo, &Parm1 );
   T_FreeLog( );

   bRC = DRGACCESSDRAGINFO( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32AccessDraginfo );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32AccessDraginfo, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32AccessDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16AddStrHandle()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HSTR APIENTRY16 DRG16ADDSTRHANDLE( PSZ16 );
PCHAR szDrg16AddStrHandle = "Drg16AddStrHandle";

HSTR APIENTRY16 Trc_Drg16AddStrHandle( PSZ16 Parm1 )
{

   HSTR ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16AddStrHandle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
   }
   HOOKENTRY16( szDrg16AddStrHandle, &Parm1 );
   T_FreeLog( );

   ulRC = DRG16ADDSTRHANDLE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16AddStrHandle );
   T_LogPMReturnCode( DT_HSTR, ulRC, ulRC );
   HOOKEXIT16( szDrg16AddStrHandle, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg16AddStrHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32AddStrHandle()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HSTR APIENTRY DRGADDSTRHANDLE( PSZ );
PCHAR szDrg32AddStrHandle = "Drg32AddStrHandle";

HSTR APIENTRY Trc_Drg32AddStrHandle( PSZ Parm1 )
{

   HSTR ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32AddStrHandle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
   }
   HOOKENTRY( szDrg32AddStrHandle, &Parm1 );
   T_FreeLog( );

   ulRC = DRGADDSTRHANDLE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32AddStrHandle );
   T_LogPMReturnCode( DT_HSTR, ulRC, ulRC );
   HOOKEXIT( szDrg32AddStrHandle, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32AddStrHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16AllocDraginfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PDRAGINFO16 APIENTRY16 DRG16ALLOCDRAGINFO( USHORT );
PCHAR szDrg16AllocDraginfo = "Drg16AllocDraginfo";

PDRAGINFO16 APIENTRY16 Trc_Drg16AllocDraginfo( USHORT Parm1 )
{

   PDRAGINFO16 pRC;                              /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16AllocDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szDrg16AllocDraginfo, &Parm1 );
   T_FreeLog( );

   pRC = DRG16ALLOCDRAGINFO( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16AllocDraginfo );
   T_LogPMReturnCodePtr16( DT_PDRAGINFO, pRC, (BOOL)pRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( pRC ) T_LogPDRAGINFO16( pRC );
   }
   HOOKEXIT16( szDrg16AllocDraginfo, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg16AllocDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32AllocDraginfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PDRAGINFO APIENTRY DRGALLOCDRAGINFO( ULONG );
PCHAR szDrg32AllocDraginfo = "Drg32AllocDraginfo";

PDRAGINFO APIENTRY Trc_Drg32AllocDraginfo( ULONG Parm1 )
{

   PDRAGINFO pRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32AllocDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDrg32AllocDraginfo, &Parm1 );
   T_FreeLog( );

   pRC = DRGALLOCDRAGINFO( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32AllocDraginfo );
   T_LogPMReturnCode( DT_PDRAGINFO, (ULONG)pRC, (BOOL)pRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( pRC ) T_LogPDRAGINFO( pRC );
   }
   HOOKEXIT( szDrg32AllocDraginfo, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg32AllocDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16AllocDragtransfer()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PDRAGTRANSFER16 APIENTRY16 DRG16ALLOCDRAGTRANSFER( USHORT );
PCHAR szDrg16AllocDragtransfer = "Drg16AllocDragtransfer";

PDRAGTRANSFER16 APIENTRY16 Trc_Drg16AllocDragtransfer( USHORT Parm1 )
{

   PDRAGTRANSFER16 pRC;                          /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16AllocDragtransfer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szDrg16AllocDragtransfer, &Parm1 );
   T_FreeLog( );

   pRC = DRG16ALLOCDRAGTRANSFER( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16AllocDragtransfer );
   T_LogPMReturnCodePtr16( DT_PDRAGTRANSFER, pRC, (BOOL)pRC );
   HOOKEXIT16( szDrg16AllocDragtransfer, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg16AllocDragtransfer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32AllocDragtransfer()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PDRAGTRANSFER APIENTRY DRGALLOCDRAGTRANSFER( ULONG );
PCHAR szDrg32AllocDragtransfer = "Drg32AllocDragtransfer";

PDRAGTRANSFER APIENTRY Trc_Drg32AllocDragtransfer( ULONG Parm1 )
{

   PDRAGTRANSFER pRC;                            /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32AllocDragtransfer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDrg32AllocDragtransfer, &Parm1 );
   T_FreeLog( );

   pRC = DRGALLOCDRAGTRANSFER( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32AllocDragtransfer );
   T_LogPMReturnCode( DT_PDRAGTRANSFER, (ULONG)pRC, (BOOL)pRC );
   HOOKEXIT( szDrg32AllocDragtransfer, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg32AllocDragtransfer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32CancelLazyDrag()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY DRGCANCELLAZYDRAG( VOID );
PCHAR szDrg32CancelLazyDrag = "Drg32CancelLazyDrag";

BOOL APIENTRY Trc_Drg32CancelLazyDrag( VOID )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32CancelLazyDrag );
   HOOKENTRY( szDrg32CancelLazyDrag, NULL );
   T_FreeLog( );

   bRC = ( *pfnDRG32CANCELLAZYDRAG )( );

   T_WaitLog( );
   T_LogAPIExit( szDrg32CancelLazyDrag );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32CancelLazyDrag, NULL, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32CancelLazyDrag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16DeleteDraginfoStrHandles()                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16DELETEDRAGINFOSTRHANDLES( PDRAGINFO16 );
PCHAR szDrg16DeleteDraginfoStrHandles = "Drg16DeleteDraginfoStrHandles";

BOOL16 APIENTRY16 Trc_Drg16DeleteDraginfoStrHandles( PDRAGINFO16 Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16DeleteDraginfoStrHandles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
   }
   HOOKENTRY16( szDrg16DeleteDraginfoStrHandles, &Parm1 );
   T_FreeLog( );

   bRC = DRG16DELETEDRAGINFOSTRHANDLES( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16DeleteDraginfoStrHandles );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16DeleteDraginfoStrHandles, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16DeleteDraginfoStrHandles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32DeleteDraginfoStrHandles()                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGDELETEDRAGINFOSTRHANDLES( PDRAGINFO );
PCHAR szDrg32DeleteDraginfoStrHandles = "Drg32DeleteDraginfoStrHandles";

BOOL APIENTRY Trc_Drg32DeleteDraginfoStrHandles( PDRAGINFO Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32DeleteDraginfoStrHandles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
   }
   HOOKENTRY( szDrg32DeleteDraginfoStrHandles, &Parm1 );
   T_FreeLog( );

   bRC = DRGDELETEDRAGINFOSTRHANDLES( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32DeleteDraginfoStrHandles );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32DeleteDraginfoStrHandles, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32DeleteDraginfoStrHandles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16DeleteStrHandle()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16DELETESTRHANDLE( HSTR );
PCHAR szDrg16DeleteStrHandle = "Drg16DeleteStrHandle";

BOOL16 APIENTRY16 Trc_Drg16DeleteStrHandle( HSTR Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16DeleteStrHandle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTR );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szDrg16DeleteStrHandle, &Parm1 );
   T_FreeLog( );

   bRC = DRG16DELETESTRHANDLE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16DeleteStrHandle );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16DeleteStrHandle, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16DeleteStrHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32DeleteStrHandle()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGDELETESTRHANDLE( HSTR );
PCHAR szDrg32DeleteStrHandle = "Drg32DeleteStrHandle";

BOOL APIENTRY Trc_Drg32DeleteStrHandle( HSTR Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32DeleteStrHandle );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTR );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDrg32DeleteStrHandle, &Parm1 );
   T_FreeLog( );

   bRC = DRGDELETESTRHANDLE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32DeleteStrHandle );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32DeleteStrHandle, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32DeleteStrHandle() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16Drag()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 DRG16DRAG( HWND, PDRAGINFO16, PDRAGIMAGE16, USHORT, SHORT, PVOID16 );
PCHAR szDrg16Drag = "Drg16Drag";

HWND APIENTRY16 Trc_Drg16Drag( HWND         Parm1
                             , PDRAGINFO16  Parm2
                             , PDRAGIMAGE16 Parm3
                             , USHORT       Parm4
                             , SHORT        Parm5
                             , PVOID16      Parm6
                             )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16Drag );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PDRAGINFO );
      T_LogPVOID16( Parm2 );
      T_LogPDRAGINFO16( Parm2 );
      T_LogParameter( 3L, DT_PDRAGIMAGE );
      T_LogDRAGIMAGEBuffer16( Parm3, Parm4 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_SHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PVOID );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDrg16Drag, &Parm1 );
   T_FreeLog( );

   ulRC = DRG16DRAG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16Drag );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PDRAGINFO );
      T_LogPVOID16( Parm2 );
      if ( ulRC ) T_LogPDRAGINFO16( Parm2 );
   }
   HOOKEXIT16( szDrg16Drag, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg16Drag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32Drag()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY DRGDRAG( HWND, PDRAGINFO, PDRAGIMAGE, ULONG, LONG, PVOID );
PCHAR szDrg32Drag = "Drg32Drag";

HWND APIENTRY Trc_Drg32Drag( HWND       Parm1
                           , PDRAGINFO  Parm2
                           , PDRAGIMAGE Parm3
                           , ULONG      Parm4
                           , LONG       Parm5
                           , PVOID      Parm6
                           )
{

   HWND ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32Drag );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PDRAGINFO );
      T_LogPVOID( Parm2 );
      T_LogPDRAGINFO( Parm2 );
      T_LogParameter( 3L, DT_PDRAGIMAGE );
      T_LogDRAGIMAGEBuffer( Parm3, Parm4 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_LONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PVOID );
      T_LogPVOID( Parm6 );
   }
   HOOKENTRY( szDrg32Drag, &Parm1 );
   T_FreeLog( );

   ulRC = DRGDRAG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32Drag );
   T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PDRAGINFO );
      T_LogPVOID( Parm2 );
      if ( ulRC ) T_LogPDRAGINFO( Parm2 );
   }
   HOOKEXIT( szDrg32Drag, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32Drag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16DragFiles()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16DRAGFILES( HWND, PSZ16 * _Seg16, PSZ16 * _Seg16, PSZ16 * _Seg16, USHORT, HPOINTER, USHORT, BOOL16, ULONG );
PCHAR szDrg16DragFiles = "Drg16DragFiles";

BOOL16 APIENTRY16 Trc_Drg16DragFiles( HWND           Parm1
                                    , PSZ16 * _Seg16 Parm2
                                    , PSZ16 * _Seg16 Parm3
                                    , PSZ16 * _Seg16 Parm4
                                    , USHORT         Parm5
                                    , HPOINTER       Parm6
                                    , USHORT         Parm7
                                    , BOOL16         Parm8
                                    , ULONG          Parm9
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16DragFiles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZSTAR );
      T_LogPSZBuffer16( Parm2, Parm5 );
      T_LogParameter( 3L, DT_PSZSTAR );
      T_LogPSZBuffer16( Parm3, Parm5 );
      T_LogParameter( 4L, DT_PSZSTAR );
      T_LogPSZBuffer16( Parm4, Parm5 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_HPOINTER );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_USHORT );
      T_LogUSHORT( Parm7 );
      T_LogParameter( 8L, DT_BOOL );
      T_LogUSHORT( Parm8 );
      T_LogParameter( 9L, DT_ULONG );
      T_LogULONG( Parm9 );
   }
   HOOKENTRY16( szDrg16DragFiles, &Parm1 );
   T_FreeLog( );

   bRC = DRG16DRAGFILES( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16DragFiles );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16DragFiles, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16DragFiles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32DragFiles()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGDRAGFILES( HWND, PSZ *, PSZ *, PSZ *, ULONG, HPOINTER, ULONG, BOOL, ULONG );
PCHAR szDrg32DragFiles = "Drg32DragFiles";

BOOL APIENTRY Trc_Drg32DragFiles( HWND      Parm1
                                , PSZ      *Parm2
                                , PSZ      *Parm3
                                , PSZ      *Parm4
                                , ULONG     Parm5
                                , HPOINTER  Parm6
                                , ULONG     Parm7
                                , BOOL      Parm8
                                , ULONG     Parm9
                                )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32DragFiles );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZSTAR );
      T_LogPSZBuffer( Parm2, Parm5 );
      T_LogParameter( 3L, DT_PSZSTAR );
      T_LogPSZBuffer( Parm3, Parm5 );
      T_LogParameter( 4L, DT_PSZSTAR );
      T_LogPSZBuffer( Parm4, Parm5 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_HPOINTER );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_ULONG );
      T_LogULONG( Parm7 );
      T_LogParameter( 8L, DT_BOOL );
      T_LogULONG( Parm8 );
      T_LogParameter( 9L, DT_ULONG );
      T_LogULONG( Parm9 );
   }
   HOOKENTRY( szDrg32DragFiles, &Parm1 );
   T_FreeLog( );

   bRC = DRGDRAGFILES( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32DragFiles );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32DragFiles, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32DragFiles() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16FreeDraginfo()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16FREEDRAGINFO( PDRAGINFO16 );
PCHAR szDrg16FreeDraginfo = "Drg16FreeDraginfo";

BOOL16 APIENTRY16 Trc_Drg16FreeDraginfo( PDRAGINFO16 Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16FreeDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
   }
   HOOKENTRY16( szDrg16FreeDraginfo, &Parm1 );
   T_FreeLog( );

   bRC = DRG16FREEDRAGINFO( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16FreeDraginfo );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16FreeDraginfo, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16FreeDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32FreeDraginfo()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGFREEDRAGINFO( PDRAGINFO );
PCHAR szDrg32FreeDraginfo = "Drg32FreeDraginfo";

BOOL APIENTRY Trc_Drg32FreeDraginfo( PDRAGINFO Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32FreeDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
   }
   HOOKENTRY( szDrg32FreeDraginfo, &Parm1 );
   T_FreeLog( );

   bRC = DRGFREEDRAGINFO( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32FreeDraginfo );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32FreeDraginfo, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32FreeDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16FreeDragtransfer()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16FREEDRAGTRANSFER( PDRAGTRANSFER16 );
PCHAR szDrg16FreeDragtransfer = "Drg16FreeDragtransfer";

BOOL16 APIENTRY16 Trc_Drg16FreeDragtransfer( PDRAGTRANSFER16 Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16FreeDragtransfer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGTRANSFER );
      T_LogPDRAGTRANSFER16( Parm1 );
   }
   HOOKENTRY16( szDrg16FreeDragtransfer, &Parm1 );
   T_FreeLog( );

   bRC = DRG16FREEDRAGTRANSFER( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16FreeDragtransfer );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16FreeDragtransfer, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16FreeDragtransfer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32FreeDragtransfer()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGFREEDRAGTRANSFER( PDRAGTRANSFER );
PCHAR szDrg32FreeDragtransfer = "Drg32FreeDragtransfer";

BOOL APIENTRY Trc_Drg32FreeDragtransfer( PDRAGTRANSFER Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32FreeDragtransfer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGTRANSFER );
      T_LogPDRAGTRANSFER( Parm1 );
   }
   HOOKENTRY( szDrg32FreeDragtransfer, &Parm1 );
   T_FreeLog( );

   bRC = DRGFREEDRAGTRANSFER( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32FreeDragtransfer );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32FreeDragtransfer, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32FreeDragtransfer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16GetPS()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPS APIENTRY16 DRG16GETPS( HWND );
PCHAR szDrg16GetPS = "Drg16GetPS";

HPS APIENTRY16 Trc_Drg16GetPS( HWND Parm1 )
{

   HPS ulRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16GetPS );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szDrg16GetPS, &Parm1 );
   T_FreeLog( );

   ulRC = DRG16GETPS( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16GetPS );
   T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
   HOOKEXIT16( szDrg16GetPS, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg16GetPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32GetPS()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPS APIENTRY DRGGETPS( HWND );
PCHAR szDrg32GetPS = "Drg32GetPS";

HPS APIENTRY Trc_Drg32GetPS( HWND Parm1 )
{

   HPS ulRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32GetPS );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDrg32GetPS, &Parm1 );
   T_FreeLog( );

   ulRC = DRGGETPS( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32GetPS );
   T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
   HOOKEXIT( szDrg32GetPS, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32GetPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32LazyDrag()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY DRGLAZYDRAG( HWND, PDRAGINFO, PDRAGIMAGE, ULONG, PVOID );
PCHAR szDrg32LazyDrag = "Drg32LazyDrag";

BOOL APIENTRY Trc_Drg32LazyDrag( HWND       Parm1
                               , PDRAGINFO  Parm2
                               , PDRAGIMAGE Parm3
                               , ULONG      Parm4
                               , PVOID      Parm5
                               )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32LazyDrag );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PDRAGINFO );
      T_LogPVOID( Parm2 );
      T_LogPDRAGINFO( Parm2 );
      T_LogParameter( 3L, DT_PDRAGIMAGE );
      T_LogDRAGIMAGEBuffer( Parm3, Parm4 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
   }
   HOOKENTRY( szDrg32LazyDrag, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnDRG32LAZYDRAG )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32LazyDrag );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32LazyDrag, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32LazyDrag() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32LazyDrop()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
BOOL APIENTRY DRGLAZYDROP( HWND, ULONG, PPOINTL );
PCHAR szDrg32LazyDrop = "Drg32LazyDrop";

BOOL APIENTRY Trc_Drg32LazyDrop( HWND    Parm1
                               , ULONG   Parm2
                               , PPOINTL Parm3
                               )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32LazyDrop );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PPOINTL );
      T_LogPPOINTL( Parm3 );
   }
   HOOKENTRY( szDrg32LazyDrop, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnDRG32LAZYDROP )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32LazyDrop );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32LazyDrop, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32LazyDrop() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16PostTransferMsg()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16POSTTRANSFERMSG( HWND, USHORT, PDRAGTRANSFER16, USHORT, USHORT, BOOL16 );
PCHAR szDrg16PostTransferMsg = "Drg16PostTransferMsg";

BOOL16 APIENTRY16 Trc_Drg16PostTransferMsg( HWND            Parm1
                                          , USHORT          Parm2
                                          , PDRAGTRANSFER16 Parm3
                                          , USHORT          Parm4
                                          , USHORT          Parm5
                                          , BOOL16          Parm6
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16PostTransferMsg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PDRAGTRANSFER );
      T_LogPDRAGTRANSFER16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_BOOL );
      T_LogUSHORT( Parm6 );
   }
   HOOKENTRY16( szDrg16PostTransferMsg, &Parm1 );
   T_FreeLog( );

   bRC = DRG16POSTTRANSFERMSG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16PostTransferMsg );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16PostTransferMsg, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16PostTransferMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32PostTransferMsg()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGPOSTTRANSFERMSG( HWND, ULONG, PDRAGTRANSFER, ULONG, ULONG, BOOL );
PCHAR szDrg32PostTransferMsg = "Drg32PostTransferMsg";

BOOL APIENTRY Trc_Drg32PostTransferMsg( HWND          Parm1
                                      , ULONG         Parm2
                                      , PDRAGTRANSFER Parm3
                                      , ULONG         Parm4
                                      , ULONG         Parm5
                                      , BOOL          Parm6
                                      )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32PostTransferMsg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PDRAGTRANSFER );
      T_LogPDRAGTRANSFER( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_BOOL );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szDrg32PostTransferMsg, &Parm1 );
   T_FreeLog( );

   bRC = DRGPOSTTRANSFERMSG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32PostTransferMsg );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32PostTransferMsg, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32PostTransferMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16PushDraginfo()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16PUSHDRAGINFO( PDRAGINFO16, HWND );
PCHAR szDrg16PushDraginfo = "Drg16PushDraginfo";

BOOL16 APIENTRY16 Trc_Drg16PushDraginfo( PDRAGINFO16 Parm1
                                       , HWND        Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16PushDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szDrg16PushDraginfo, &Parm1 );
   T_FreeLog( );

   bRC = DRG16PUSHDRAGINFO( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16PushDraginfo );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16PushDraginfo, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16PushDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32PushDraginfo()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGPUSHDRAGINFO( PDRAGINFO, HWND );
PCHAR szDrg32PushDraginfo = "Drg32PushDraginfo";

BOOL APIENTRY Trc_Drg32PushDraginfo( PDRAGINFO Parm1
                                   , HWND      Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32PushDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
      T_LogParameter( 2L, DT_HWND );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szDrg32PushDraginfo, &Parm1 );
   T_FreeLog( );

   bRC = DRGPUSHDRAGINFO( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32PushDraginfo );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32PushDraginfo, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32PushDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryDraginfoPtr()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
PDRAGINFO APIENTRY DRGQUERYDRAGINFOPTR( PDRAGINFO );
PCHAR szDrg32QueryDraginfoPtr = "Drg32QueryDraginfoPtr";

PDRAGINFO APIENTRY Trc_Drg32QueryDraginfoPtr( PDRAGINFO Parm1 )
{

   PDRAGINFO pRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryDraginfoPtr );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
   }
   HOOKENTRY( szDrg32QueryDraginfoPtr, &Parm1 );
   T_FreeLog( );

   pRC = (PDRAGINFO)( *pfnDRG32QUERYDRAGINFOPTR )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryDraginfoPtr );
   T_LogPMReturnCode( DT_PDRAGINFO, (ULONG)pRC, (BOOL)pRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( pRC ) T_LogPDRAGINFO( pRC );
   }
   HOOKEXIT( szDrg32QueryDraginfoPtr, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg32QueryDraginfoPtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryDraginfoPtrFromDragitem()                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
PDRAGINFO APIENTRY DRGQUERYDRAGINFOPTRFROMDRAGITEM( PDRAGITEM );
PCHAR szDrg32QueryDraginfoPtrFromDragitem = "Drg32QueryDraginfoPtrFromDragitem";

PDRAGINFO APIENTRY Trc_Drg32QueryDraginfoPtrFromDragitem( PDRAGITEM Parm1 )
{

   PDRAGINFO pRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryDraginfoPtrFromDragitem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
   }
   HOOKENTRY( szDrg32QueryDraginfoPtrFromDragitem, &Parm1 );
   T_FreeLog( );

   pRC = (PDRAGINFO)( *pfnDRG32QUERYDRAGINFOPTRFROMDRAGITEM )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryDraginfoPtrFromDragitem );
   T_LogPMReturnCode( DT_PDRAGINFO, (ULONG)pRC, (BOOL)pRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( pRC ) T_LogPDRAGINFO( pRC );
   }
   HOOKEXIT( szDrg32QueryDraginfoPtrFromDragitem, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg32QueryDraginfoPtrFromDragitem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryDraginfoPtrFromHwnd()                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
PDRAGINFO APIENTRY DRGQUERYDRAGINFOPTRFROMHWND( HWND );
PCHAR szDrg32QueryDraginfoPtrFromHwnd = "Drg32QueryDraginfoPtrFromHwnd";

PDRAGINFO APIENTRY Trc_Drg32QueryDraginfoPtrFromHwnd( HWND Parm1 )
{

   PDRAGINFO pRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryDraginfoPtrFromHwnd );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDrg32QueryDraginfoPtrFromHwnd, &Parm1 );
   T_FreeLog( );

   pRC = (PDRAGINFO)( *pfnDRG32QUERYDRAGINFOPTRFROMHWND )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryDraginfoPtrFromHwnd );
   T_LogPMReturnCode( DT_PDRAGINFO, (ULONG)pRC, (BOOL)pRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( pRC ) T_LogPDRAGINFO( pRC );
   }
   HOOKEXIT( szDrg32QueryDraginfoPtrFromHwnd, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg32QueryDraginfoPtrFromHwnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryDragitem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16QUERYDRAGITEM( PDRAGINFO16, USHORT, PDRAGITEM16, USHORT );
PCHAR szDrg16QueryDragitem = "Drg16QueryDragitem";

BOOL16 APIENTRY16 Trc_Drg16QueryDragitem( PDRAGINFO16 Parm1
                                        , USHORT      Parm2
                                        , PDRAGITEM16 Parm3
                                        , USHORT      Parm4
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryDragitem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PDRAGITEM );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szDrg16QueryDragitem, &Parm1 );
   T_FreeLog( );

   bRC = DRG16QUERYDRAGITEM( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryDragitem );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PDRAGITEM );
      T_LogPVOID16( Parm3 );
      if ( bRC ) T_LogPDRAGITEM16( Parm3 );
   }
   HOOKEXIT16( szDrg16QueryDragitem, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16QueryDragitem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryDragitem()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGQUERYDRAGITEM( PDRAGINFO, ULONG, PDRAGITEM, ULONG );
PCHAR szDrg32QueryDragitem = "Drg32QueryDragitem";

BOOL APIENTRY Trc_Drg32QueryDragitem( PDRAGINFO Parm1
                                    , ULONG     Parm2
                                    , PDRAGITEM Parm3
                                    , ULONG     Parm4
                                    )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryDragitem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PDRAGITEM );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szDrg32QueryDragitem, &Parm1 );
   T_FreeLog( );

   bRC = DRGQUERYDRAGITEM( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryDragitem );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PDRAGITEM );
      T_LogPVOID( Parm3 );
      if ( bRC ) T_LogPDRAGITEM( Parm3 );
   }
   HOOKEXIT( szDrg32QueryDragitem, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32QueryDragitem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryDragitemCount()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 DRG16QUERYDRAGITEMCOUNT( PDRAGINFO16 );
PCHAR szDrg16QueryDragitemCount = "Drg16QueryDragitemCount";

USHORT APIENTRY16 Trc_Drg16QueryDragitemCount( PDRAGINFO16 Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryDragitemCount );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
   }
   HOOKENTRY16( szDrg16QueryDragitemCount, &Parm1 );
   T_FreeLog( );

   usRC = DRG16QUERYDRAGITEMCOUNT( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryDragitemCount );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   HOOKEXIT16( szDrg16QueryDragitemCount, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Drg16QueryDragitemCount() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryDragitemCount()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY DRGQUERYDRAGITEMCOUNT( PDRAGINFO );
PCHAR szDrg32QueryDragitemCount = "Drg32QueryDragitemCount";

ULONG APIENTRY Trc_Drg32QueryDragitemCount( PDRAGINFO Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryDragitemCount );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
   }
   HOOKENTRY( szDrg32QueryDragitemCount, &Parm1 );
   T_FreeLog( );

   ulRC = DRGQUERYDRAGITEMCOUNT( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryDragitemCount );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szDrg32QueryDragitemCount, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32QueryDragitemCount() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryDragitemPtr()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PDRAGITEM16 APIENTRY16 DRG16QUERYDRAGITEMPTR( PDRAGINFO16, USHORT );
PCHAR szDrg16QueryDragitemPtr = "Drg16QueryDragitemPtr";

PDRAGITEM16 APIENTRY16 Trc_Drg16QueryDragitemPtr( PDRAGINFO16 Parm1
                                                , USHORT      Parm2
                                                )
{

   PDRAGITEM16 pRC;                              /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryDragitemPtr );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szDrg16QueryDragitemPtr, &Parm1 );
   T_FreeLog( );

   pRC = DRG16QUERYDRAGITEMPTR( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryDragitemPtr );
   T_LogPMReturnCodePtr16( DT_PDRAGITEM, pRC, (BOOL)pRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( pRC ) T_LogPDRAGITEM16( pRC );
   }
   HOOKEXIT16( szDrg16QueryDragitemPtr, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg16QueryDragitemPtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryDragitemPtr()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PDRAGITEM APIENTRY DRGQUERYDRAGITEMPTR( PDRAGINFO, ULONG );
PCHAR szDrg32QueryDragitemPtr = "Drg32QueryDragitemPtr";

PDRAGITEM APIENTRY Trc_Drg32QueryDragitemPtr( PDRAGINFO Parm1
                                            , ULONG     Parm2
                                            )
{

   PDRAGITEM pRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryDragitemPtr );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szDrg32QueryDragitemPtr, &Parm1 );
   T_FreeLog( );

   pRC = DRGQUERYDRAGITEMPTR( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryDragitemPtr );
   T_LogPMReturnCode( DT_PDRAGITEM, (ULONG)pRC, (BOOL)pRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( pRC ) T_LogPDRAGITEM( pRC );
   }
   HOOKEXIT( szDrg32QueryDragitemPtr, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg32QueryDragitemPtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryDragStatus()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
ULONG APIENTRY DRGQUERYDRAGSTATUS( VOID );
PCHAR szDrg32QueryDragStatus = "Drg32QueryDragStatus";

ULONG APIENTRY Trc_Drg32QueryDragStatus( VOID )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryDragStatus );
   HOOKENTRY( szDrg32QueryDragStatus, NULL );
   T_FreeLog( );

   ulRC = ( *pfnDRG32QUERYDRAGSTATUS )( );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryDragStatus );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szDrg32QueryDragStatus, NULL, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32QueryDragStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryFormat()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY DRGQUERYFORMAT( PDRAGITEM , PSZ, ULONG, PSZ, ULONG );
PCHAR szDrg32QueryFormat = "Drg32QueryFormat";

ULONG APIENTRY Trc_Drg32QueryFormat( PDRAGITEM Parm1
                                   , PSZ       Parm2
                                   , ULONG     Parm3
                                   , PSZ       Parm4
                                   , ULONG     Parm5
                                   )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryFormat );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY( szDrg32QueryFormat, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnDRG32QUERYFORMAT )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryFormat );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PSZ );
      if ( ulRC ) T_LogPSZ( Parm4 );
      else T_LogPVOID( Parm4 );
   }
   HOOKEXIT( szDrg32QueryFormat, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32QueryFormat() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryNativeRMF()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16QUERYNATIVERMF( PDRAGITEM16, USHORT, PCHAR16 );
PCHAR szDrg16QueryNativeRMF = "Drg16QueryNativeRMF";

BOOL16 APIENTRY16 Trc_Drg16QueryNativeRMF( PDRAGITEM16 Parm1
                                         , USHORT      Parm2
                                         , PCHAR16     Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryNativeRMF );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
   }
   HOOKENTRY16( szDrg16QueryNativeRMF, &Parm1 );
   T_FreeLog( );

   bRC = DRG16QUERYNATIVERMF( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryNativeRMF );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      if ( bRC ) T_LogPSZ16( Parm3 );
      else T_LogPVOID16( Parm3 );
   }
   HOOKEXIT16( szDrg16QueryNativeRMF, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16QueryNativeRMF() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryNativeRMF()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGQUERYNATIVERMF( PDRAGITEM, ULONG, PCHAR );
PCHAR szDrg32QueryNativeRMF = "Drg32QueryNativeRMF";

BOOL APIENTRY Trc_Drg32QueryNativeRMF( PDRAGITEM Parm1
                                     , ULONG     Parm2
                                     , PCHAR     Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryNativeRMF );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
   }
   HOOKENTRY( szDrg32QueryNativeRMF, &Parm1 );
   T_FreeLog( );

   bRC = DRGQUERYNATIVERMF( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryNativeRMF );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      if ( bRC ) T_LogPSZ( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szDrg32QueryNativeRMF, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32QueryNativeRMF() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryNativeRMFLen()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 DRG16QUERYNATIVERMFLEN( PDRAGITEM16 );
PCHAR szDrg16QueryNativeRMFLen = "Drg16QueryNativeRMFLen";

USHORT APIENTRY16 Trc_Drg16QueryNativeRMFLen( PDRAGITEM16 Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryNativeRMFLen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
   }
   HOOKENTRY16( szDrg16QueryNativeRMFLen, &Parm1 );
   T_FreeLog( );

   usRC = DRG16QUERYNATIVERMFLEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryNativeRMFLen );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   HOOKEXIT16( szDrg16QueryNativeRMFLen, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Drg16QueryNativeRMFLen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryNativeRMFLen()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY DRGQUERYNATIVERMFLEN( PDRAGITEM );
PCHAR szDrg32QueryNativeRMFLen = "Drg32QueryNativeRMFLen";

ULONG APIENTRY Trc_Drg32QueryNativeRMFLen( PDRAGITEM Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryNativeRMFLen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
   }
   HOOKENTRY( szDrg32QueryNativeRMFLen, &Parm1 );
   T_FreeLog( );

   ulRC = DRGQUERYNATIVERMFLEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryNativeRMFLen );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szDrg32QueryNativeRMFLen, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32QueryNativeRMFLen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryStrName()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 DRG16QUERYSTRNAME( HSTR, USHORT, PSZ16 );
PCHAR szDrg16QueryStrName = "Drg16QueryStrName";

USHORT APIENTRY16 Trc_Drg16QueryStrName( HSTR   Parm1
                                       , USHORT Parm2
                                       , PSZ16  Parm3
                                       )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryStrName );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTR );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPVOID16( Parm3 );
   }
   HOOKENTRY16( szDrg16QueryStrName, &Parm1 );
   T_FreeLog( );

   usRC = DRG16QUERYSTRNAME( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryStrName );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PSZ );
      if ( usRC ) T_LogPSZ16( Parm3 );
      else T_LogPVOID16( Parm3 );
   }
   HOOKEXIT16( szDrg16QueryStrName, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Drg16QueryStrName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryStrName()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY DRGQUERYSTRNAME( HSTR, ULONG, PSZ );
PCHAR szDrg32QueryStrName = "Drg32QueryStrName";

ULONG APIENTRY Trc_Drg32QueryStrName( HSTR  Parm1
                                    , ULONG Parm2
                                    , PSZ   Parm3
                                    )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryStrName );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTR );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPVOID( Parm3 );
   }
   HOOKENTRY( szDrg32QueryStrName, &Parm1 );
   T_FreeLog( );

   ulRC = DRGQUERYSTRNAME( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryStrName );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PSZ );
      if ( ulRC ) T_LogPSZ( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szDrg32QueryStrName, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32QueryStrName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryStrNameLen()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 DRG16QUERYSTRNAMELEN( HSTR );
PCHAR szDrg16QueryStrNameLen = "Drg16QueryStrNameLen";

USHORT APIENTRY16 Trc_Drg16QueryStrNameLen( HSTR Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryStrNameLen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTR );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szDrg16QueryStrNameLen, &Parm1 );
   T_FreeLog( );

   usRC = DRG16QUERYSTRNAMELEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryStrNameLen );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   HOOKEXIT16( szDrg16QueryStrNameLen, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Drg16QueryStrNameLen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryStrNameLen()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY DRGQUERYSTRNAMELEN( HSTR );
PCHAR szDrg32QueryStrNameLen = "Drg32QueryStrNameLen";

ULONG APIENTRY Trc_Drg32QueryStrNameLen( HSTR Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryStrNameLen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTR );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDrg32QueryStrNameLen, &Parm1 );
   T_FreeLog( );

   ulRC = DRGQUERYSTRNAMELEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryStrNameLen );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szDrg32QueryStrNameLen, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32QueryStrNameLen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryTrueType()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16QUERYTRUETYPE( PDRAGITEM16, USHORT, PSZ16 );
PCHAR szDrg16QueryTrueType = "Drg16QueryTrueType";

BOOL16 APIENTRY16 Trc_Drg16QueryTrueType( PDRAGITEM16 Parm1
                                        , USHORT      Parm2
                                        , PSZ16       Parm3
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryTrueType );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPVOID16( Parm3 );
   }
   HOOKENTRY16( szDrg16QueryTrueType, &Parm1 );
   T_FreeLog( );

   bRC = DRG16QUERYTRUETYPE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryTrueType );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PSZ );
      if ( bRC ) T_LogPSZ16( Parm3 );
      else T_LogPVOID16( Parm3 );
   }
   HOOKEXIT16( szDrg16QueryTrueType, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16QueryTrueType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryTrueType()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGQUERYTRUETYPE( PDRAGITEM, ULONG, PSZ );
PCHAR szDrg32QueryTrueType = "Drg32QueryTrueType";

BOOL APIENTRY Trc_Drg32QueryTrueType( PDRAGITEM Parm1
                                    , ULONG     Parm2
                                    , PSZ       Parm3
                                    )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryTrueType );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPVOID( Parm3 );
   }
   HOOKENTRY( szDrg32QueryTrueType, &Parm1 );
   T_FreeLog( );

   bRC = DRGQUERYTRUETYPE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryTrueType );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PSZ );
      if ( bRC ) T_LogPSZ( Parm3 );
      else T_LogPVOID( Parm3 );
   }
   HOOKEXIT( szDrg32QueryTrueType, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32QueryTrueType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16QueryTrueTypeLen()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 DRG16QUERYTRUETYPELEN( PDRAGITEM16 );
PCHAR szDrg16QueryTrueTypeLen = "Drg16QueryTrueTypeLen";

USHORT APIENTRY16 Trc_Drg16QueryTrueTypeLen( PDRAGITEM16 Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16QueryTrueTypeLen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
   }
   HOOKENTRY16( szDrg16QueryTrueTypeLen, &Parm1 );
   T_FreeLog( );

   usRC = DRG16QUERYTRUETYPELEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16QueryTrueTypeLen );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
   HOOKEXIT16( szDrg16QueryTrueTypeLen, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Drg16QueryTrueTypeLen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32QueryTrueTypeLen()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY DRGQUERYTRUETYPELEN( PDRAGITEM );
PCHAR szDrg32QueryTrueTypeLen = "Drg32QueryTrueTypeLen";

ULONG APIENTRY Trc_Drg32QueryTrueTypeLen( PDRAGITEM Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32QueryTrueTypeLen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
   }
   HOOKENTRY( szDrg32QueryTrueTypeLen, &Parm1 );
   T_FreeLog( );

   ulRC = DRGQUERYTRUETYPELEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32QueryTrueTypeLen );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   HOOKEXIT( szDrg32QueryTrueTypeLen, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32QueryTrueTypeLen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32ReallocDraginfo()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 230 )
PDRAGINFO APIENTRY DRGREALLOCDRAGINFO( PDRAGINFO, ULONG );
PCHAR szDrg32ReallocDraginfo = "Drg32ReallocDraginfo";

PDRAGINFO APIENTRY Trc_Drg32ReallocDraginfo( PDRAGINFO Parm1
                                           , ULONG     Parm2
                                           )
{

   PDRAGINFO pRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32ReallocDraginfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szDrg32ReallocDraginfo, &Parm1 );
   T_FreeLog( );

   pRC = (PDRAGINFO)( *pfnDRG32REALLOCDRAGINFO )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32ReallocDraginfo );
   T_LogPMReturnCode( DT_PDRAGINFO, (ULONG)pRC, (BOOL)pRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      if ( pRC ) T_LogPDRAGINFO( pRC );
   }
   HOOKEXIT( szDrg32ReallocDraginfo, &Parm1, &pRC );
   T_FreeLog( );

   return pRC;

}  /* Trc_Drg32ReallocDraginfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16ReleasePS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16RELEASEPS( HPS );
PCHAR szDrg16ReleasePS = "Drg16ReleasePS";

BOOL16 APIENTRY16 Trc_Drg16ReleasePS( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16ReleasePS );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPS );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szDrg16ReleasePS, &Parm1 );
   T_FreeLog( );

   bRC = DRG16RELEASEPS( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16ReleasePS );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16ReleasePS, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16ReleasePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32ReleasePS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGRELEASEPS( HPS );
PCHAR szDrg32ReleasePS = "Drg32ReleasePS";

BOOL APIENTRY Trc_Drg32ReleasePS( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32ReleasePS );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HPS );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDrg32ReleasePS, &Parm1 );
   T_FreeLog( );

   bRC = DRGRELEASEPS( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32ReleasePS );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32ReleasePS, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32ReleasePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16SendTransferMsg()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
MRESULT16 APIENTRY16 DRG16SENDTRANSFERMSG( HWND, USHORT, MPARAM, MPARAM );
PCHAR szDrg16SendTransferMsg = "Drg16SendTransferMsg";

MRESULT16 APIENTRY16 Trc_Drg16SendTransferMsg( HWND   Parm1
                                             , USHORT Parm2
                                             , MPARAM Parm3
                                             , MPARAM Parm4
                                             )
{

   MRESULT16 ulRC;                               /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16SendTransferMsg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogPMMessage16( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY16( szDrg16SendTransferMsg, &Parm1 );
   T_FreeLog( );

   ulRC = DRG16SENDTRANSFERMSG( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16SendTransferMsg );
   T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
   HOOKEXIT16( szDrg16SendTransferMsg, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg16SendTransferMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32SendTransferMsg()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY DRGSENDTRANSFERMSG( HWND, ULONG, MPARAM, MPARAM );
PCHAR szDrg32SendTransferMsg = "Drg32SendTransferMsg";

MRESULT APIENTRY Trc_Drg32SendTransferMsg( HWND   Parm1
                                         , ULONG  Parm2
                                         , MPARAM Parm3
                                         , MPARAM Parm4
                                         )
{

   MRESULT ulRC;                                 /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32SendTransferMsg );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HWND );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogPMMessage( Parm2 );
      T_LogParameter( 3L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm3 );
      T_LogParameter( 4L, DT_MPARAM );
      T_LogULONG( (ULONG)Parm4 );
   }
   HOOKENTRY( szDrg32SendTransferMsg, &Parm1 );
   T_FreeLog( );

   ulRC = DRGSENDTRANSFERMSG( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32SendTransferMsg );
   T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
   HOOKEXIT( szDrg32SendTransferMsg, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Drg32SendTransferMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16SetDragImage()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16SETDRAGIMAGE( PDRAGINFO16, PDRAGIMAGE16, USHORT, PVOID16 );
PCHAR szDrg16SetDragImage = "Drg16SetDragImage";

BOOL16 APIENTRY16 Trc_Drg16SetDragImage( PDRAGINFO16  Parm1
                                       , PDRAGIMAGE16 Parm2
                                       , USHORT       Parm3
                                       , PVOID16      Parm4
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16SetDragImage );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
      T_LogParameter( 2L, DT_PDRAGIMAGE );
      T_LogDRAGIMAGEBuffer16( Parm2, Parm3 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
   }
   HOOKENTRY16( szDrg16SetDragImage, &Parm1 );
   T_FreeLog( );

   bRC = DRG16SETDRAGIMAGE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16SetDragImage );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16SetDragImage, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16SetDragImage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32SetDragImage()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGSETDRAGIMAGE( PDRAGINFO, PDRAGIMAGE, ULONG, PVOID );
PCHAR szDrg32SetDragImage = "Drg32SetDragImage";

BOOL APIENTRY Trc_Drg32SetDragImage( PDRAGINFO  Parm1
                                   , PDRAGIMAGE Parm2
                                   , ULONG      Parm3
                                   , PVOID      Parm4
                                   )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32SetDragImage );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
      T_LogParameter( 2L, DT_PDRAGIMAGE );
      T_LogDRAGIMAGEBuffer( Parm2, Parm3 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
   }
   HOOKENTRY( szDrg32SetDragImage, &Parm1 );
   T_FreeLog( );

   bRC = DRGSETDRAGIMAGE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32SetDragImage );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32SetDragImage, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32SetDragImage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16SetDragitem()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16SETDRAGITEM( PDRAGINFO16, PDRAGITEM16, USHORT, USHORT );
PCHAR szDrg16SetDragitem = "Drg16SetDragitem";

BOOL16 APIENTRY16 Trc_Drg16SetDragitem( PDRAGINFO16 Parm1
                                      , PDRAGITEM16 Parm2
                                      , USHORT      Parm3
                                      , USHORT      Parm4
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16SetDragitem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
      T_LogParameter( 2L, DT_PDRAGITEM );
      T_LogPVOID16( Parm2 );
      T_LogPDRAGITEM16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szDrg16SetDragitem, &Parm1 );
   T_FreeLog( );

   bRC = DRG16SETDRAGITEM( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16SetDragitem );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16SetDragitem, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16SetDragitem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32SetDragitem()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGSETDRAGITEM( PDRAGINFO, PDRAGITEM, ULONG, ULONG );
PCHAR szDrg32SetDragitem = "Drg32SetDragitem";

BOOL APIENTRY Trc_Drg32SetDragitem( PDRAGINFO Parm1
                                  , PDRAGITEM Parm2
                                  , ULONG     Parm3
                                  , ULONG     Parm4
                                  )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32SetDragitem );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
      T_LogParameter( 2L, DT_PDRAGITEM );
      T_LogPVOID( Parm2 );
      T_LogPDRAGITEM( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szDrg32SetDragitem, &Parm1 );
   T_FreeLog( );

   bRC = DRGSETDRAGITEM( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32SetDragitem );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32SetDragitem, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32SetDragitem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16SetDragPointer()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16SETDRAGPOINTER( PDRAGINFO16, HPOINTER );
PCHAR szDrg16SetDragPointer = "Drg16SetDragPointer";

BOOL16 APIENTRY16 Trc_Drg16SetDragPointer( PDRAGINFO16 Parm1
                                         , HPOINTER    Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16SetDragPointer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGINFO16( Parm1 );
      T_LogParameter( 2L, DT_HPOINTER );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY16( szDrg16SetDragPointer, &Parm1 );
   T_FreeLog( );

   bRC = DRG16SETDRAGPOINTER( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16SetDragPointer );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16SetDragPointer, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16SetDragPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32SetDragPointer()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGSETDRAGPOINTER( PDRAGINFO, HPOINTER );
PCHAR szDrg32SetDragPointer = "Drg32SetDragPointer";

BOOL APIENTRY Trc_Drg32SetDragPointer( PDRAGINFO Parm1
                                     , HPOINTER  Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32SetDragPointer );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGINFO );
      T_LogPVOID( Parm1 );
      T_LogPDRAGINFO( Parm1 );
      T_LogParameter( 2L, DT_HPOINTER );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szDrg32SetDragPointer, &Parm1 );
   T_FreeLog( );

   bRC = DRGSETDRAGPOINTER( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32SetDragPointer );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32SetDragPointer, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32SetDragPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16VerifyNativeRMF()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16VERIFYNATIVERMF( PDRAGITEM16, PSZ16 );
PCHAR szDrg16VerifyNativeRMF = "Drg16VerifyNativeRMF";

BOOL16 APIENTRY16 Trc_Drg16VerifyNativeRMF( PDRAGITEM16 Parm1
                                          , PSZ16       Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16VerifyNativeRMF );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szDrg16VerifyNativeRMF, &Parm1 );
   T_FreeLog( );

   bRC = DRG16VERIFYNATIVERMF( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16VerifyNativeRMF );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16VerifyNativeRMF, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16VerifyNativeRMF() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32VerifyNativeRMF()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGVERIFYNATIVERMF( PDRAGITEM, PSZ );
PCHAR szDrg32VerifyNativeRMF = "Drg32VerifyNativeRMF";

BOOL APIENTRY Trc_Drg32VerifyNativeRMF( PDRAGITEM Parm1
                                      , PSZ       Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32VerifyNativeRMF );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szDrg32VerifyNativeRMF, &Parm1 );
   T_FreeLog( );

   bRC = DRGVERIFYNATIVERMF( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32VerifyNativeRMF );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32VerifyNativeRMF, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32VerifyNativeRMF() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16VerifyRMF()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16VERIFYRMF( PDRAGITEM16, PSZ16, PSZ16 );
PCHAR szDrg16VerifyRMF = "Drg16VerifyRMF";

BOOL16 APIENTRY16 Trc_Drg16VerifyRMF( PDRAGITEM16 Parm1
                                    , PSZ16       Parm2
                                    , PSZ16       Parm3
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16VerifyRMF );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ16( Parm3 );
   }
   HOOKENTRY16( szDrg16VerifyRMF, &Parm1 );
   T_FreeLog( );

   bRC = DRG16VERIFYRMF( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16VerifyRMF );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16VerifyRMF, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16VerifyRMF() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32VerifyRMF()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGVERIFYRMF( PDRAGITEM, PSZ, PSZ );
PCHAR szDrg32VerifyRMF = "Drg32VerifyRMF";

BOOL APIENTRY Trc_Drg32VerifyRMF( PDRAGITEM Parm1
                                , PSZ       Parm2
                                , PSZ       Parm3
                                )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32VerifyRMF );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
   }
   HOOKENTRY( szDrg32VerifyRMF, &Parm1 );
   T_FreeLog( );

   bRC = DRGVERIFYRMF( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32VerifyRMF );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32VerifyRMF, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32VerifyRMF() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16VerifyTrueType()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16VERIFYTRUETYPE( PDRAGITEM16, PSZ16 );
PCHAR szDrg16VerifyTrueType = "Drg16VerifyTrueType";

BOOL16 APIENTRY16 Trc_Drg16VerifyTrueType( PDRAGITEM16 Parm1
                                         , PSZ16       Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16VerifyTrueType );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szDrg16VerifyTrueType, &Parm1 );
   T_FreeLog( );

   bRC = DRG16VERIFYTRUETYPE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16VerifyTrueType );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16VerifyTrueType, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16VerifyTrueType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32VerifyTrueType()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGVERIFYTRUETYPE( PDRAGITEM, PSZ );
PCHAR szDrg32VerifyTrueType = "Drg32VerifyTrueType";

BOOL APIENTRY Trc_Drg32VerifyTrueType( PDRAGITEM Parm1
                                     , PSZ       Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32VerifyTrueType );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szDrg32VerifyTrueType, &Parm1 );
   T_FreeLog( );

   bRC = DRGVERIFYTRUETYPE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32VerifyTrueType );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32VerifyTrueType, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32VerifyTrueType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16VerifyType()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16VERIFYTYPE( PDRAGITEM16, PSZ16 );
PCHAR szDrg16VerifyType = "Drg16VerifyType";

BOOL16 APIENTRY16 Trc_Drg16VerifyType( PDRAGITEM16 Parm1
                                     , PSZ16       Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16VerifyType );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szDrg16VerifyType, &Parm1 );
   T_FreeLog( );

   bRC = DRG16VERIFYTYPE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16VerifyType );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szDrg16VerifyType, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16VerifyType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32VerifyType()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGVERIFYTYPE( PDRAGITEM, PSZ );
PCHAR szDrg32VerifyType = "Drg32VerifyType";

BOOL APIENTRY Trc_Drg32VerifyType( PDRAGITEM Parm1
                                 , PSZ       Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32VerifyType );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szDrg32VerifyType, &Parm1 );
   T_FreeLog( );

   bRC = DRGVERIFYTYPE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32VerifyType );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szDrg32VerifyType, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32VerifyType() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg16VerifyTypeSet()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 DRG16VERIFYTYPESET( PDRAGITEM16, PSZ16, USHORT, PSZ16 );
PCHAR szDrg16VerifyTypeSet = "Drg16VerifyTypeSet";

BOOL16 APIENTRY16 Trc_Drg16VerifyTypeSet( PDRAGITEM16 Parm1
                                        , PSZ16       Parm2
                                        , USHORT      Parm3
                                        , PSZ16       Parm4
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDrg16VerifyTypeSet );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID16( Parm1 );
      T_LogPDRAGITEM16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPVOID16( Parm4 );
   }
   HOOKENTRY16( szDrg16VerifyTypeSet, &Parm1 );
   T_FreeLog( );

   bRC = DRG16VERIFYTYPESET( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg16VerifyTypeSet );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PSZ );
      if ( bRC ) T_LogPSZ16( Parm4 );
      else T_LogPVOID16( Parm4 );
   }
   HOOKEXIT16( szDrg16VerifyTypeSet, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg16VerifyTypeSet() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Drg32VerifyTypeSet()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY DRGVERIFYTYPESET( PDRAGITEM, PSZ, ULONG, PSZ );
PCHAR szDrg32VerifyTypeSet = "Drg32VerifyTypeSet";

BOOL APIENTRY Trc_Drg32VerifyTypeSet( PDRAGITEM Parm1
                                    , PSZ       Parm2
                                    , ULONG     Parm3
                                    , PSZ       Parm4
                                    )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDrg32VerifyTypeSet );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PDRAGITEM );
      T_LogPVOID( Parm1 );
      T_LogPDRAGITEM( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPVOID( Parm4 );
   }
   HOOKENTRY( szDrg32VerifyTypeSet, &Parm1 );
   T_FreeLog( );

   bRC = DRGVERIFYTYPESET( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDrg32VerifyTypeSet );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PSZ );
      if ( bRC ) T_LogPSZ( Parm4 );
      else T_LogPVOID( Parm4 );
   }
   HOOKEXIT( szDrg32VerifyTypeSet, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Drg32VerifyTypeSet() */
#endif

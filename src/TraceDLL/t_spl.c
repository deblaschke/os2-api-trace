/******************************************************************************/
/*                                                                            */
/* File name    : T_SPL.C                                                     */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit PMSPL routines         */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from PMSPL.DLL.      */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.14 29Aug95 Added 32-bit Prt and Spl APIs               */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.31 08Aug97 Logged user-defined buffer initial bytes    */
/*                2.40.33 11Aug97 Added 16-bit Prt APIs                       */
/*                2.40.34 13Aug97 Added 16-bit DosPrint and Spl APIs          */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.42 23Jan98 Added PRQINFO8 structure                    */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.72 22Oct98 Moved Prt/Spl APIs loading to OS/2 2.30     */
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
#define  INCL_SPLBIDI                            /* Include spool bidi        */
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

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
#ifdef T_16BIT
#define ORD_PRT16ABORTDOC             78         /* OS/2 2.30 proc ordinal    */
#define ORD_PRT16RESETABORT           79         /* OS/2 2.30 proc ordinal    */
#endif
#define ORD_PRT32ABORTDOC             601        /* OS/2 2.30 proc ordinal    */
#define ORD_PRT32RESETABORT           600        /* OS/2 2.30 proc ordinal    */
#define ORD_SPL32QMGETJOBID           608        /* OS/2 2.30 proc ordinal    */
#define ORD_SPL32QMNEWPAGE            607        /* OS/2 2.30 proc ordinal    */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
#ifdef T_16BIT
#define ORD_PRT16QUERY                76         /* OS/2 2.40 proc ordinal    */
#define ORD_PRT16SET                  77         /* OS/2 2.40 proc ordinal    */
#endif
#define ORD_PRT32NEWPAGE              602        /* OS/2 2.40 proc ordinal    */
#define ORD_PRT32QUERY                603        /* OS/2 2.40 proc ordinal    */
#define ORD_PRT32SET                  604        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32CREATEPORT           413        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32DELETEPORT           414        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32DISABLE              610        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32DISPLAYCONTROLPANEL  616        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32ENABLE               611        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32GETCONTROLPANELLIST  618        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32GETPORTFROMQ         617        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32PROTSENDCMD          605        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32PROTXLATECMD         606        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32QUERYCONTROLPANEL    615        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32QUERYPATH            412        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32QUERYPORT            410        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32REGISTER             612        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32REGISTERCONTROLPANEL 614        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32SETPORT              411        /* OS/2 2.40 proc ordinal    */
#define ORD_SPL32UNREGISTER           613        /* OS/2 2.40 proc ordinal    */
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
PCHAR   szDLL = "PMSPL";                         /* DLL module name           */
PCHAR   szTraceDLL = "T_SPL";                    /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
#ifdef T_16BIT
ULONG (* _Seg16 APIENTRY16 pfnPRT16ABORTDOC)( HFILE16, PVOID16, ULONG, ULONG );
                                                 /* OS/2 2.30 procedure       */
ULONG (* _Seg16 APIENTRY16 pfnPRT16RESETABORT)( HFILE );
                                                 /* OS/2 2.30 procedure       */
#endif
ULONG (* APIENTRY          pfnPRT32ABORTDOC)( HFILE, PVOID, ULONG, ULONG );
                                                 /* OS/2 2.30 procedure       */
ULONG (* APIENTRY          pfnPRT32RESETABORT)( HFILE );
                                                 /* OS/2 2.30 procedure       */
ULONG (* APIENTRY          pfnSPL32QMGETJOBID)( HSPL, ULONG, PVOID, ULONG, PULONG );
                                                 /* OS/2 2.30 procedure       */
BOOL (* APIENTRY           pfnSPL32QMNEWPAGE)( HSPL, ULONG );
                                                 /* OS/2 2.30 procedure       */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
#ifdef T_16BIT
ULONG (* _Seg16 APIENTRY16 pfnPRT16QUERY)( PSZ16, PSZ16, ULONG, ULONG, PVOID16, ULONG, PVOID16, PULONG16 );
                                                 /* OS/2 2.40 procedure       */
ULONG (* _Seg16 APIENTRY16 pfnPRT16SET)( PSZ16, PSZ16, ULONG, ULONG, PVOID16, ULONG );
                                                 /* OS/2 2.40 procedure       */
#endif
ULONG (* APIENTRY          pfnPRT32NEWPAGE)( HFILE, ULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnPRT32QUERY)( PSZ, PSZ, ULONG, ULONG, PVOID, ULONG, PVOID, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnPRT32SET)( PSZ, PSZ, ULONG, ULONG, PVOID, ULONG );
                                                 /* OS/2 2.40 procedure       */
SPLERR (* APIENTRY         pfnSPL32CREATEPORT)( PSZ, PSZ, PSZ, ULONG, PVOID, ULONG );
                                                 /* OS/2 2.40 procedure       */
SPLERR (* APIENTRY         pfnSPL32DELETEPORT)( PSZ, PSZ );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32DISABLE)( PSZ, PVOID );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32DISPLAYCONTROLPANEL)( PSZ, PSZ, PSZ, PSZ, HAB, ULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32ENABLE)( PSZ, PSZ, PVOID );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32GETCONTROLPANELLIST)( PSZ, PSZ, PSZ, ULONG, PSZ, ULONG, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32GETPORTFROMQ)( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32PROTSENDCMD)( PSZ, ULONG, ULONG, PFN, PFN, PVOID, ULONG, PVOID, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32PROTXLATECMD)( PSZ, PFN, PVOID, ULONG, PVOID, PVOID, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32QUERYCONTROLPANEL)( PSZ, PSZ, PSZ, PSZ, ULONG, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32QUERYPATH)( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32QUERYPORT)( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32REGISTER)( PSZ, PSZ, HWND, ULONG, ULONG, PULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32REGISTERCONTROLPANEL)( PSZ, PSZ, ULONG, PSZ, ULONG );
                                                 /* OS/2 2.40 procedure       */
SPLERR (* APIENTRY         pfnSPL32SETPORT)( PSZ, PSZ, ULONG, PVOID, ULONG, ULONG );
                                                 /* OS/2 2.40 procedure       */
ULONG (* APIENTRY          pfnSPL32UNREGISTER)( PSZ, PSZ, ULONG, ULONG, ULONG );
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
    * Library initialization required if process attaching to T_SPL
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
       * Log error and terminate if T_SPL version does not match T_COMMON
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
       * Load PMSPL library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_SPL library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMSPL trace DLL
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
         if ( ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_PRT16ABORTDOC   ), (PPFN16)&pfnPRT16ABORTDOC   ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_PRT16RESETABORT ), (PPFN16)&pfnPRT16RESETABORT ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 16, 2, 30, szDLL, ulOrdinal );
         }
#endif

         /*
          * Obtain 32-bit OS/2 2.30 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_PRT32ABORTDOC  , NULL, (PFN *)&pfnPRT32ABORTDOC   ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_PRT32RESETABORT, NULL, (PFN *)&pfnPRT32RESETABORT ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32QMGETJOBID, NULL, (PFN *)&pfnSPL32QMGETJOBID ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32QMNEWPAGE , NULL, (PFN *)&pfnSPL32QMNEWPAGE  ) ) )
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
#ifdef T_16BIT
         /*
          * Obtain 16-bit OS/2 2.40 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_PRT16QUERY ), (PPFN16)&pfnPRT16QUERY ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_PRT16SET   ), (PPFN16)&pfnPRT16SET   ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 16, 2, 40, szDLL, ulOrdinal );
         }
#endif

         /*
          * Obtain 32-bit OS/2 2.40 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_PRT32NEWPAGE             , NULL, (PFN *)&pfnPRT32NEWPAGE              ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_PRT32QUERY               , NULL, (PFN *)&pfnPRT32QUERY                ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_PRT32SET                 , NULL, (PFN *)&pfnPRT32SET                  ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32CREATEPORT          , NULL, (PFN *)&pfnSPL32CREATEPORT           ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32DELETEPORT          , NULL, (PFN *)&pfnSPL32DELETEPORT           ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32DISABLE             , NULL, (PFN *)&pfnSPL32DISABLE              ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32DISPLAYCONTROLPANEL , NULL, (PFN *)&pfnSPL32DISPLAYCONTROLPANEL  ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32ENABLE              , NULL, (PFN *)&pfnSPL32ENABLE               ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32GETCONTROLPANELLIST , NULL, (PFN *)&pfnSPL32GETCONTROLPANELLIST  ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32GETPORTFROMQ        , NULL, (PFN *)&pfnSPL32GETPORTFROMQ         ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32PROTSENDCMD         , NULL, (PFN *)&pfnSPL32PROTSENDCMD          ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32PROTXLATECMD        , NULL, (PFN *)&pfnSPL32PROTXLATECMD         ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32QUERYCONTROLPANEL   , NULL, (PFN *)&pfnSPL32QUERYCONTROLPANEL    ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32QUERYPATH           , NULL, (PFN *)&pfnSPL32QUERYPATH            ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32QUERYPORT           , NULL, (PFN *)&pfnSPL32QUERYPORT            ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32REGISTER            , NULL, (PFN *)&pfnSPL32REGISTER             ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32REGISTERCONTROLPANEL, NULL, (PFN *)&pfnSPL32REGISTERCONTROLPANEL ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32SETPORT             , NULL, (PFN *)&pfnSPL32SETPORT              ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_SPL32UNREGISTER          , NULL, (PFN *)&pfnSPL32UNREGISTER           ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 40, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_SPL
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMSPL library if library loaded
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
/* Trc_Dos16PrintDestAdd()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTDESTADD( PSZ16, USHORT, PBYTE16, USHORT );
PCHAR szDos16PrintDestAdd = "Dos16PrintDestAdd";

SPLERR16 APIENTRY16 Trc_Dos16PrintDestAdd( PSZ16   Parm1
                                         , USHORT  Parm2
                                         , PBYTE16 Parm3
                                         , USHORT  Parm4
                                         )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintDestAdd );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogPPRDINFO16( Parm3, Parm4, Parm2 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szDos16PrintDestAdd, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTDESTADD( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintDestAdd );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintDestAdd, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintDestAdd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintDestControl()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTDESTCONTROL( PSZ16, PSZ16, USHORT );
PCHAR szDos16PrintDestControl = "Dos16PrintDestControl";

SPLERR16 APIENTRY16 Trc_Dos16PrintDestControl( PSZ16  Parm1
                                             , PSZ16  Parm2
                                             , USHORT Parm3
                                             )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintDestControl );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szDos16PrintDestControl, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTDESTCONTROL( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintDestControl );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintDestControl, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintDestControl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintDestDel()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTDESTDEL( PSZ16, PSZ16 );
PCHAR szDos16PrintDestDel = "Dos16PrintDestDel";

SPLERR16 APIENTRY16 Trc_Dos16PrintDestDel( PSZ16  Parm1
                                         , PSZ16  Parm2
                                         )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintDestDel );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szDos16PrintDestDel, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTDESTDEL( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintDestDel );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintDestDel, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintDestDel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintDestEnum()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTDESTENUM( PSZ16, USHORT, PBYTE16, USHORT, PUSHORT16, PUSHORT16 );
PCHAR szDos16PrintDestEnum = "Dos16PrintDestEnum";

SPLERR16 APIENTRY16 Trc_Dos16PrintDestEnum( PSZ16     Parm1
                                          , USHORT    Parm2
                                          , PBYTE16   Parm3
                                          , USHORT    Parm4
                                          , PUSHORT16 Parm5
                                          , PUSHORT16 Parm6
                                          )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintDestEnum );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PrintDestEnum, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTDESTENUM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintDestEnum );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      if ( ( Parm5 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogD16PDeE_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16PrintDestEnum, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintDestEnum() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintDestGetInfo()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTDESTGETINFO( PSZ16, PSZ16, USHORT, PBYTE16, USHORT, PUSHORT16 );
PCHAR szDos16PrintDestGetInfo = "Dos16PrintDestGetInfo";

SPLERR16 APIENTRY16 Trc_Dos16PrintDestGetInfo( PSZ16     Parm1
                                             , PSZ16     Parm2
                                             , USHORT    Parm3
                                             , PBYTE16   Parm4
                                             , USHORT    Parm5
                                             , PUSHORT16 Parm6
                                             )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintDestGetInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PrintDestGetInfo, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTDESTGETINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintDestGetInfo );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      if ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) T_LogD16PDGI_Parm4( Parm4, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16PrintDestGetInfo, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintDestGetInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintDestSetInfo()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTDESTSETINFO( PSZ16, PSZ16, USHORT, PBYTE16, USHORT, USHORT );
PCHAR szDos16PrintDestSetInfo = "Dos16PrintDestSetInfo";

SPLERR16 APIENTRY16 Trc_Dos16PrintDestSetInfo( PSZ16   Parm1
                                             , PSZ16   Parm2
                                             , USHORT  Parm3
                                             , PBYTE16 Parm4
                                             , USHORT  Parm5
                                             , USHORT  Parm6
                                             )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintDestSetInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      if ( Parm3 == 3 ) T_LogD16PDSI_Parm4( Parm4, Parm5, Parm6 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_USHORT );
      T_LogUSHORT( Parm6 );
   }
   HOOKENTRY16( szDos16PrintDestSetInfo, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTDESTSETINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintDestSetInfo );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintDestSetInfo, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintDestSetInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintDriverEnum()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTDRIVERENUM( PSZ16, USHORT, PBYTE16, USHORT, PUSHORT16, PUSHORT16 );
PCHAR szDos16PrintDriverEnum = "Dos16PrintDriverEnum";

SPLERR16 APIENTRY16 Trc_Dos16PrintDriverEnum( PSZ16     Parm1
                                            , USHORT    Parm2
                                            , PBYTE16   Parm3
                                            , USHORT    Parm4
                                            , PUSHORT16 Parm5
                                            , PUSHORT16 Parm6
                                            )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintDriverEnum );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PrintDriverEnum, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTDRIVERENUM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintDriverEnum );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      if ( ( Parm5 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogD16PDrE_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16PrintDriverEnum, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintDriverEnum() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintJobContinue()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTJOBCONTINUE( PSZ16, USHORT );
PCHAR szDos16PrintJobContinue = "Dos16PrintJobContinue";

SPLERR16 APIENTRY16 Trc_Dos16PrintJobContinue( PSZ16  Parm1
                                             , USHORT Parm2
                                             )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintJobContinue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szDos16PrintJobContinue, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTJOBCONTINUE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintJobContinue );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintJobContinue, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintJobContinue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintJobDel()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTJOBDEL( PSZ16, USHORT );
PCHAR szDos16PrintJobDel = "Dos16PrintJobDel";

SPLERR16 APIENTRY16 Trc_Dos16PrintJobDel( PSZ16  Parm1
                                        , USHORT Parm2
                                        )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintJobDel );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szDos16PrintJobDel, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTJOBDEL( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintJobDel );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintJobDel, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintJobDel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintJobEnum()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTJOBENUM( PSZ16, PSZ16, USHORT, PBYTE16, USHORT, PUSHORT16, PUSHORT16 );
PCHAR szDos16PrintJobEnum = "Dos16PrintJobEnum";

SPLERR16 APIENTRY16 Trc_Dos16PrintJobEnum( PSZ16     Parm1
                                         , PSZ16     Parm2
                                         , USHORT    Parm3
                                         , PBYTE16   Parm4
                                         , USHORT    Parm5
                                         , PUSHORT16 Parm6
                                         , PUSHORT16 Parm7
                                         )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintJobEnum );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
      T_LogParameter( 7L, DT_PUSHORT );
      T_LogPVOID16( Parm7 );
   }
   HOOKENTRY16( szDos16PrintJobEnum, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTJOBENUM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintJobEnum );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      if ( ( Parm6 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogD16PJE_Parm4( Parm4, *Parm6, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
      T_LogParameter( 7L, DT_PUSHORT );
      T_LogPUSHORT16( Parm7 );
   }
   HOOKEXIT16( szDos16PrintJobEnum, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintJobEnum() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintJobPause()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTJOBPAUSE( PSZ16, USHORT );
PCHAR szDos16PrintJobPause = "Dos16PrintJobPause";

SPLERR16 APIENTRY16 Trc_Dos16PrintJobPause( PSZ16  Parm1
                                          , USHORT Parm2
                                          )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintJobPause );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
   }
   HOOKENTRY16( szDos16PrintJobPause, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTJOBPAUSE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintJobPause );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintJobPause, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintJobPause() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintJobGetInfo()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTJOBGETINFO( PSZ16, USHORT, USHORT, PBYTE16, USHORT, PUSHORT16 );
PCHAR szDos16PrintJobGetInfo = "Dos16PrintJobGetInfo";

SPLERR16 APIENTRY16 Trc_Dos16PrintJobGetInfo( PSZ16     Parm1
                                            , USHORT    Parm2
                                            , USHORT    Parm3
                                            , PBYTE16   Parm4
                                            , USHORT    Parm5
                                            , PUSHORT16 Parm6
                                            )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintJobGetInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PrintJobGetInfo, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTJOBGETINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintJobGetInfo );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      if ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) T_LogD16PJGI_Parm4( Parm4, Parm5, Parm3 );
   }
   HOOKEXIT16( szDos16PrintJobGetInfo, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintJobGetInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintJobGetId()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTJOBGETID( HFILE16, PPRIDINFO16, USHORT );
PCHAR szDos16PrintJobGetId = "Dos16PrintJobGetId";

SPLERR16 APIENTRY16 Trc_Dos16PrintJobGetId( HFILE16     Parm1
                                          , PPRIDINFO16 Parm2
                                          , USHORT      Parm3
                                          )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintJobGetId );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PPRIDINFO );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
   }
   HOOKENTRY16( szDos16PrintJobGetId, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTJOBGETID( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintJobGetId );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PPRIDINFO );
      T_LogPVOID16( Parm2 );
      if ( usRC ) T_LogPPRIDINFO( Parm2, Parm3 );
   }
   HOOKEXIT16( szDos16PrintJobGetId, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintJobGetId() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintJobSetInfo()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTJOBSETINFO( PSZ16, USHORT, USHORT, PBYTE16, USHORT, USHORT );
PCHAR szDos16PrintJobSetInfo = "Dos16PrintJobSetInfo";

SPLERR16 APIENTRY16 Trc_Dos16PrintJobSetInfo( PSZ16   Parm1
                                            , USHORT  Parm2
                                            , USHORT  Parm3
                                            , PBYTE16 Parm4
                                            , USHORT  Parm5
                                            , USHORT  Parm6
                                            )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintJobSetInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      if ( ( Parm3 == 1 ) || ( Parm3 == 3 ) || ( Parm3 == 4 ) ) T_LogD16PJSI_Parm4( Parm4, Parm5, Parm3, Parm6 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_USHORT );
      T_LogUSHORT( Parm6 );
   }
   HOOKENTRY16( szDos16PrintJobSetInfo, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTJOBSETINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintJobSetInfo );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintJobSetInfo, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintJobSetInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintPortEnum()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTPORTENUM( PSZ16, USHORT, PBYTE16, USHORT, PUSHORT16, PUSHORT16 );
PCHAR szDos16PrintPortEnum = "Dos16PrintPortEnum";

SPLERR16 APIENTRY16 Trc_Dos16PrintPortEnum( PSZ16     Parm1
                                          , USHORT    Parm2
                                          , PBYTE16   Parm3
                                          , USHORT    Parm4
                                          , PUSHORT16 Parm5
                                          , PUSHORT16 Parm6
                                          )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintPortEnum );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PrintPortEnum, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTPORTENUM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintPortEnum );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      if ( ( Parm5 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogD16PPE_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16PrintPortEnum, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintPortEnum() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQAdd()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQADD( PSZ16, USHORT, PBYTE16, USHORT );
PCHAR szDos16PrintQAdd = "Dos16PrintQAdd";

SPLERR16 APIENTRY16 Trc_Dos16PrintQAdd( PSZ16   Parm1
                                      , USHORT  Parm2
                                      , PBYTE16 Parm3
                                      , USHORT  Parm4
                                      )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQAdd );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogPPRQINFO16( Parm3, Parm4, Parm2 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
   }
   HOOKENTRY16( szDos16PrintQAdd, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQADD( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQAdd );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintQAdd, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQAdd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQContinue()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQCONTINUE( PSZ16, PSZ16 );
PCHAR szDos16PrintQContinue = "Dos16PrintQContinue";

SPLERR16 APIENTRY16 Trc_Dos16PrintQContinue( PSZ16 Parm1
                                           , PSZ16 Parm2
                                           )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQContinue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szDos16PrintQContinue, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQCONTINUE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQContinue );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintQContinue, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQContinue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQDel()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQDEL( PSZ16, PSZ16 );
PCHAR szDos16PrintQDel = "Dos16PrintQDel";

SPLERR16 APIENTRY16 Trc_Dos16PrintQDel( PSZ16 Parm1
                                      , PSZ16 Parm2
                                      )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQDel );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szDos16PrintQDel, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQDEL( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQDel );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintQDel, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQDel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQEnum()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQENUM( PSZ, USHORT, PBYTE16, USHORT, PUSHORT16, PUSHORT16 );
PCHAR szDos16PrintQEnum = "Dos16PrintQEnum";

SPLERR16 APIENTRY16 Trc_Dos16PrintQEnum( PSZ16     Parm1
                                       , USHORT    Parm2
                                       , PBYTE16   Parm3
                                       , USHORT    Parm4
                                       , PUSHORT16 Parm5
                                       , PUSHORT16 Parm6
                                       )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQEnum );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PrintQEnum, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQENUM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQEnum );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      if ( ( Parm5 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogD16PQE_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16PrintQEnum, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQEnum() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQGetInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQGETINFO( PSZ16, PSZ16, USHORT, PBYTE16, USHORT, PUSHORT16 );
PCHAR szDos16PrintQGetInfo = "Dos16PrintQGetInfo";

SPLERR16 APIENTRY16 Trc_Dos16PrintQGetInfo( PSZ16     Parm1
                                          , PSZ16     Parm2
                                          , USHORT    Parm3
                                          , PBYTE16   Parm4
                                          , USHORT    Parm5
                                          , PUSHORT16 Parm6
                                          )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQGetInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PrintQGetInfo, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQGETINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQGetInfo );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID16( Parm4 );
      if ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) T_LogD16PQGI_Parm4( Parm4, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16PrintQGetInfo, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQGetInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQPause()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQPAUSE( PSZ16, PSZ16 );
PCHAR szDos16PrintQPause = "Dos16PrintQPause";

SPLERR16 APIENTRY16 Trc_Dos16PrintQPause( PSZ16 Parm1
                                        , PSZ16 Parm2
                                        )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQPause );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szDos16PrintQPause, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQPAUSE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQPause );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintQPause, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQPause() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQProcessorEnum()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQPROCESSORENUM( PSZ16, USHORT, PBYTE16, USHORT, PUSHORT16, PUSHORT16 );
PCHAR szDos16PrintQProcessorEnum = "Dos16PrintQProcessorEnum";

SPLERR16 APIENTRY16 Trc_Dos16PrintQProcessorEnum( PSZ16     Parm1
                                                , USHORT    Parm2
                                                , PBYTE16   Parm3
                                                , USHORT    Parm4
                                                , PUSHORT16 Parm5
                                                , PUSHORT16 Parm6
                                                )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQProcessorEnum );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPVOID16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPVOID16( Parm6 );
   }
   HOOKENTRY16( szDos16PrintQProcessorEnum, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQPROCESSORENUM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQProcessorEnum );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      if ( ( Parm5 ) && ( ( !usRC ) || ( usRC == ERROR_MORE_DATA ) ) ) T_LogD16PQPE_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PUSHORT );
      T_LogPUSHORT16( Parm5 );
      T_LogParameter( 6L, DT_PUSHORT );
      T_LogPUSHORT16( Parm6 );
   }
   HOOKEXIT16( szDos16PrintQProcessorEnum, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQProcessorEnum() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQPurge()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQPURGE( PSZ16, PSZ16 );
PCHAR szDos16PrintQPurge = "Dos16PrintQPurge";

SPLERR16 APIENTRY16 Trc_Dos16PrintQPurge( PSZ16 Parm1
                                        , PSZ16 Parm2
                                        )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQPurge );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szDos16PrintQPurge, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQPURGE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQPurge );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintQPurge, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQPurge() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16PrintQSetInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SPLERR16 APIENTRY16 DOSPRINTQSETINFO( PSZ16, PSZ16, USHORT, PBYTE16, USHORT, USHORT );
PCHAR szDos16PrintQSetInfo = "Dos16PrintQSetInfo";

SPLERR16 APIENTRY16 Trc_Dos16PrintQSetInfo( PSZ16   Parm1
                                          , PSZ16   Parm2
                                          , USHORT  Parm3
                                          , PBYTE16 Parm4
                                          , USHORT  Parm5
                                          , USHORT  Parm6
                                          )
{

   SPLERR16 usRC;                                /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szDos16PrintQSetInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PBYTE );
      T_LogPVOID16( Parm4 );
      if ( ( Parm3 == 1 ) || ( Parm3 == 3 ) || ( Parm3 == 6 ) || ( Parm3 == 8 ) ) T_LogD16PQSI_Parm4( Parm4, Parm5, Parm3, Parm6 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_USHORT );
      T_LogUSHORT( Parm6 );
   }
   HOOKENTRY16( szDos16PrintQSetInfo, &Parm1 );
   T_FreeLog( );

   usRC = DOSPRINTQSETINFO( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szDos16PrintQSetInfo );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szDos16PrintQSetInfo, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Dos16PrintQSetInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt16Abort()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
VOID APIENTRY16 PRTABORT( HFILE16 );
PCHAR szPrt16Abort = "Prt16Abort";

VOID APIENTRY16 Trc_Prt16Abort( HFILE16 Parm1 )
{

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16Abort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szPrt16Abort, &Parm1 );
   T_FreeLog( );

   PRTABORT( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16Abort );
   HOOKEXIT16( szPrt16Abort, &Parm1, NULL );
   T_FreeLog( );

}  /* Trc_Prt16Abort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32Abort()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
VOID APIENTRY PRT32ABORT( HFILE );
PCHAR szPrt32Abort = "Prt32Abort";

VOID APIENTRY Trc_Prt32Abort( HFILE Parm1 )
{

   T_WaitLog( );
   T_LogAPIEntry( szPrt32Abort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szPrt32Abort, &Parm1 );
   T_FreeLog( );

   PRT32ABORT( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32Abort );
   HOOKEXIT( szPrt32Abort, &Parm1, NULL );
   T_FreeLog( );

}  /* Trc_Prt32Abort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt16AbortDoc()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY16 PRT16ABORTDOC( HFILE16, PVOID16, ULONG, ULONG );
PCHAR szPrt16AbortDoc = "Prt16AbortDoc";

ULONG APIENTRY16 Trc_Prt16AbortDoc( HFILE16 Parm1
                                  , PVOID16 Parm2
                                  , ULONG   Parm3
                                  , ULONG   Parm4
                                  )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16AbortDoc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID16( Parm2 );
      T_LogCHARBuffer16( Parm2, Parm3 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY16( szPrt16AbortDoc, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT16ABORTDOC )( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16AbortDoc );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT16( szPrt16AbortDoc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt16AbortDoc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32AbortDoc()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY PRTABORTDOC( HFILE, PVOID, ULONG, ULONG );
PCHAR szPrt32AbortDoc = "Prt32AbortDoc";

ULONG APIENTRY Trc_Prt32AbortDoc( HFILE Parm1
                                , PVOID Parm2
                                , ULONG Parm3
                                , ULONG Parm4
                                )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32AbortDoc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID( Parm2 );
      T_LogCHARBuffer( Parm2, Parm3 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szPrt32AbortDoc, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT32ABORTDOC )( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32AbortDoc );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szPrt32AbortDoc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32AbortDoc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt16Close()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 PRTCLOSE( HFILE16 );
PCHAR szPrt16Close = "Prt16Close";

USHORT APIENTRY16 Trc_Prt16Close( HFILE16 Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16Close );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogUSHORT( Parm1 );
   }
   HOOKENTRY16( szPrt16Close, &Parm1 );
   T_FreeLog( );

   usRC = PRTCLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16Close );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szPrt16Close, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Prt16Close() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32Close()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY PRT32CLOSE( HFILE );
PCHAR szPrt32Close = "Prt32Close";

ULONG APIENTRY Trc_Prt32Close( HFILE Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32Close );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szPrt32Close, &Parm1 );
   T_FreeLog( );

   ulRC = PRT32CLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32Close );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szPrt32Close, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32Close() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt16DevIOCtl()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 PRTDEVIOCTL( PVOID16, PVOID16, USHORT, USHORT, HFILE16 );
PCHAR szPrt16DevIOCtl = "Prt16DevIOCtl";

USHORT APIENTRY16 Trc_Prt16DevIOCtl( PVOID16 Parm1
                                   , PVOID16 Parm2
                                   , USHORT  Parm3
                                   , USHORT  Parm4
                                   , HFILE16 Parm5
                                   )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16DevIOCtl );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVOID );
      T_LogPVOID16( Parm1 );
      T_LogCHARBufferUnformatted16( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID16( Parm2 );
      T_LogCHARBufferUnformatted16( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_USHORT );
      T_LogUSHORT( Parm4 );
      T_LogParameter( 5L, DT_HFILE );
      T_LogUSHORT( Parm5 );
   }
   HOOKENTRY16( szPrt16DevIOCtl, &Parm1 );
   T_FreeLog( );

   usRC = PRTDEVIOCTL( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16DevIOCtl );
   T_LogCPReturnCode( usRC );
   HOOKEXIT16( szPrt16DevIOCtl, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Prt16DevIOCtl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32DevIOCtl()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY PRT32DEVIOCTL( PVOID, PVOID, ULONG, ULONG, HFILE );
PCHAR szPrt32DevIOCtl = "Prt32DevIOCtl";

ULONG APIENTRY Trc_Prt32DevIOCtl( PVOID Parm1
                                , PVOID Parm2
                                , ULONG Parm3
                                , ULONG Parm4
                                , HFILE Parm5
                                )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32DevIOCtl );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PVOID );
      T_LogPVOID( Parm1 );
      T_LogCHARBufferUnformatted( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID( Parm2 );
      T_LogCHARBufferUnformatted( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_HFILE );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY( szPrt32DevIOCtl, &Parm1 );
   T_FreeLog( );

   ulRC = PRT32DEVIOCTL( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32DevIOCtl );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szPrt32DevIOCtl, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32DevIOCtl() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32NewPage()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY PRTNEWPAGE( HFILE, ULONG );
PCHAR szPrt32NewPage = "Prt32NewPage";

ULONG APIENTRY Trc_Prt32NewPage( HFILE Parm1
                               , ULONG Parm2
                               )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32NewPage );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szPrt32NewPage, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT32NEWPAGE )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32NewPage );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szPrt32NewPage, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32NewPage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt16Open()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 PRTOPEN( PSZ16, PHFILE16, PUSHORT16, ULONG, USHORT, USHORT, USHORT, ULONG );
PCHAR szPrt16Open = "Prt16Open";

USHORT APIENTRY16 Trc_Prt16Open( PSZ16     Parm1
                               , PHFILE16  Parm2
                               , PUSHORT16 Parm3
                               , ULONG     Parm4
                               , USHORT    Parm5
                               , USHORT    Parm6
                               , USHORT    Parm7
                               , ULONG     Parm8
                               )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16Open );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_PHFILE );
      T_LogPVOID16( Parm2 );
      T_LogParameter( 3L, DT_PUSHORT );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_USHORT );
      T_LogUSHORT( Parm5 );
      T_LogParameter( 6L, DT_USHORT );
      T_LogUSHORT( Parm6 );
      T_LogParameter( 7L, DT_USHORT );
      T_LogUSHORT( Parm7 );
      T_LogParameter( 8L, DT_ULONG );
      T_LogULONG( Parm8 );
   }
   HOOKENTRY16( szPrt16Open, &Parm1 );
   T_FreeLog( );

   usRC = PRTOPEN( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16Open );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PHFILE );
      T_LogPUSHORT16( Parm2 );
      T_LogParameter( 3L, DT_PUSHORT );
      T_LogPUSHORT16( Parm3 );
   }
   HOOKEXIT16( szPrt16Open, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Prt16Open() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32Open()                                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY PRT32OPEN( PSZ, PHFILE, PULONG, ULONG, ULONG, ULONG, ULONG, ULONG );
PCHAR szPrt32Open = "Prt32Open";

ULONG APIENTRY Trc_Prt32Open( PSZ    Parm1
                            , PHFILE Parm2
                            , PULONG Parm3
                            , ULONG  Parm4
                            , ULONG  Parm5
                            , ULONG  Parm6
                            , ULONG  Parm7
                            , ULONG  Parm8
                            )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32Open );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PHFILE );
      T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_PULONG );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_ULONG );
      T_LogULONG( Parm7 );
      T_LogParameter( 8L, DT_ULONG );
      T_LogULONG( Parm8 );
   }
   HOOKENTRY( szPrt32Open, &Parm1 );
   T_FreeLog( );

   ulRC = PRT32OPEN( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32Open );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PHFILE );
      T_LogPULONG( Parm2 );
      T_LogParameter( 3L, DT_PULONG );
      T_LogPULONG( Parm3 );
   }
   HOOKEXIT( szPrt32Open, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32Open() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt16Query()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY16 PRT16QUERY( PSZ16, PSZ16, ULONG, ULONG, PVOID16, ULONG, PVOID16, PULONG16 );
PCHAR szPrt16Query = "Prt16Query";

ULONG APIENTRY16 Trc_Prt16Query( PSZ16    Parm1
                               , PSZ16    Parm2
                               , ULONG    Parm3
                               , ULONG    Parm4
                               , PVOID16  Parm5
                               , ULONG    Parm6
                               , PVOID16  Parm7
                               , PULONG16 Parm8
                               )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16Query );
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
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID16( Parm5 );
      T_LogCHARBuffer16( Parm5, Parm6 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_PVOID );
      T_LogPVOID16( Parm7 );
      T_LogParameter( 8L, DT_PULONG );
      T_LogPULONG16( Parm8 );
   }
   HOOKENTRY16( szPrt16Query, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT16QUERY )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16Query );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 7L, DT_PVOID );
      T_LogPVOID16( Parm7 );
      if ( ( Parm8 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer16( Parm7, *Parm8 );
      T_LogParameter( 8L, DT_PULONG );
      T_LogPULONG16( Parm8 );
   }
   HOOKEXIT16( szPrt16Query, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt16Query() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32Query()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY PRTQUERY( PSZ, PSZ, ULONG, ULONG, PVOID, ULONG, PVOID, PULONG );
PCHAR szPrt32Query = "Prt32Query";

ULONG APIENTRY Trc_Prt32Query( PSZ    Parm1
                             , PSZ    Parm2
                             , ULONG  Parm3
                             , ULONG  Parm4
                             , PVOID  Parm5
                             , ULONG  Parm6
                             , PVOID  Parm7
                             , PULONG Parm8
                             )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32Query );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      T_LogCHARBuffer( Parm5, Parm6 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_PVOID );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_PULONG );
      T_LogPULONG( Parm8 );
   }
   HOOKENTRY( szPrt32Query, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT32QUERY )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32Query );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 7L, DT_PVOID );
      T_LogPVOID( Parm7 );
      if ( ( Parm8 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer( Parm7, *Parm8 );
      T_LogParameter( 8L, DT_PULONG );
      T_LogPULONG( Parm8 );
   }
   HOOKEXIT( szPrt32Query, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32Query() */
#endif

/* OS/2 BUG - Prt16ResetAbort thunk returns VOID */
/******************************************************************************/
/*                                                                            */
/* Trc_Prt16ResetAbort()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY16 PRT16RESETABORT( HFILE );
PCHAR szPrt16ResetAbort = "Prt16ResetAbort";

ULONG APIENTRY16 Trc_Prt16ResetAbort( HFILE Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16ResetAbort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szPrt16ResetAbort, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT16RESETABORT )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16ResetAbort );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT16( szPrt16ResetAbort, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt16ResetAbort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32ResetAbort()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY PRTRESETABORT( HFILE );
PCHAR szPrt32ResetAbort = "Prt32ResetAbort";

ULONG APIENTRY Trc_Prt32ResetAbort( HFILE Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32ResetAbort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szPrt32ResetAbort, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT32RESETABORT )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32ResetAbort );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szPrt32ResetAbort, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32ResetAbort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt16Set()                                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY16 PRT16SET( PSZ16, PSZ16, ULONG, ULONG, PVOID16, ULONG );
PCHAR szPrt16Set = "Prt16Set";

ULONG APIENTRY16 Trc_Prt16Set( PSZ16   Parm1
                             , PSZ16   Parm2
                             , ULONG   Parm3
                             , ULONG   Parm4
                             , PVOID16 Parm5
                             , ULONG   Parm6
                             )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16Set );
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
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID16( Parm5 );
      T_LogCHARBuffer16( Parm5, Parm6 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY16( szPrt16Set, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT16SET )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16Set );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT16( szPrt16Set, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt16Set() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32Set()                                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY PRTSET( PSZ, PSZ, ULONG, ULONG, PVOID, ULONG );
PCHAR szPrt32Set = "Prt32Set";

ULONG APIENTRY Trc_Prt32Set( PSZ   Parm1
                           , PSZ   Parm2
                           , ULONG Parm3
                           , ULONG Parm4
                           , PVOID Parm5
                           , ULONG Parm6
                           )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32Set );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      T_LogCHARBuffer( Parm5, Parm6 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szPrt32Set, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnPRT32SET )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32Set );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szPrt32Set, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32Set() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt16Write()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 PRTWRITE( HFILE16, PVOID16, USHORT, PUSHORT16 );
PCHAR szPrt16Write = "Prt16Write";

USHORT APIENTRY16 Trc_Prt16Write( HFILE16   Parm1
                                , PVOID16   Parm2
                                , USHORT    Parm3
                                , PUSHORT16 Parm4
                                )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szPrt16Write );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID16( Parm2 );
      T_LogCHARBuffer16( Parm2, Parm3 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPVOID16( Parm4 );
   }
   HOOKENTRY16( szPrt16Write, &Parm1 );
   T_FreeLog( );

   usRC = PRTWRITE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrt16Write );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
   }
   HOOKEXIT16( szPrt16Write, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Prt16Write() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Prt32Write()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY PRT32WRITE( HFILE, PVOID, ULONG, PULONG );
PCHAR szPrt32Write = "Prt32Write";

ULONG APIENTRY Trc_Prt32Write( HFILE  Parm1
                             , PVOID  Parm2
                             , ULONG  Parm3
                             , PULONG Parm4
                             )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szPrt32Write );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HFILE );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID( Parm2 );
      T_LogCHARBuffer( Parm2, Parm3 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPVOID( Parm4 );
   }
   HOOKENTRY( szPrt32Write, &Parm1 );
   T_FreeLog( );

   ulRC = PRT32WRITE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szPrt32Write );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PULONG );
      T_LogPULONG( Parm4 );
   }
   HOOKEXIT( szPrt32Write, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Prt32Write() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32ControlDevice()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLCONTROLDEVICE( PSZ, PSZ, ULONG );
PCHAR szSpl32ControlDevice = "Spl32ControlDevice";

SPLERR APIENTRY Trc_Spl32ControlDevice( PSZ   Parm1
                                      , PSZ   Parm2
                                      , ULONG Parm3
                                      )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32ControlDevice );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szSpl32ControlDevice, &Parm1 );
   T_FreeLog( );

   ulRC = SPLCONTROLDEVICE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32ControlDevice );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32ControlDevice, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32ControlDevice() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32CopyJob()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLCOPYJOB( PSZ, PSZ, ULONG, PSZ, PSZ, PULONG );
PCHAR szSpl32CopyJob = "Spl32CopyJob";

SPLERR APIENTRY Trc_Spl32CopyJob( PSZ    Parm1
                                , PSZ    Parm2
                                , ULONG  Parm3
                                , PSZ    Parm4
                                , PSZ    Parm5
                                , PULONG Parm6
                                )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32CopyJob );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ( Parm4 );
      T_LogParameter( 5L, DT_PSZ );
      T_LogPSZ( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
   }
   HOOKENTRY( szSpl32CopyJob, &Parm1 );
   T_FreeLog( );

   ulRC = SPLCOPYJOB( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32CopyJob );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKEXIT( szSpl32CopyJob, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32CopyJob() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32CreateDevice()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLCREATEDEVICE( PSZ, ULONG, PVOID, ULONG );
PCHAR szSpl32CreateDevice = "Spl32CreateDevice";

SPLERR APIENTRY Trc_Spl32CreateDevice( PSZ   Parm1
                                     , ULONG Parm2
                                     , PVOID Parm3
                                     , ULONG Parm4
                                     )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32CreateDevice );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogPPRDINFO( Parm3, Parm4, Parm2 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szSpl32CreateDevice, &Parm1 );
   T_FreeLog( );

   ulRC = SPLCREATEDEVICE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32CreateDevice );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32CreateDevice, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32CreateDevice() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32CreatePort()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
SPLERR APIENTRY SPLCREATEPORT( PSZ, PSZ, PSZ, ULONG, PVOID, ULONG );
PCHAR szSpl32CreatePort = "Spl32CreatePort";

SPLERR APIENTRY Trc_Spl32CreatePort( PSZ   Parm1
                                   , PSZ   Parm2
                                   , PSZ   Parm3
                                   , ULONG Parm4
                                   , PVOID Parm5
                                   , ULONG Parm6
                                   )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32CreatePort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      T_LogCHARBuffer( Parm5, Parm6 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szSpl32CreatePort, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32CREATEPORT )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32CreatePort );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32CreatePort, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32CreatePort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32CreateQueue()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLCREATEQUEUE( PSZ, ULONG, PVOID, ULONG );
PCHAR szSpl32CreateQueue = "Spl32CreateQueue";

SPLERR APIENTRY Trc_Spl32CreateQueue( PSZ   Parm1
                                    , ULONG Parm2
                                    , PVOID Parm3
                                    , ULONG Parm4
                                    )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32CreateQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogPPRQINFO( Parm3, Parm4, Parm2 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
   }
   HOOKENTRY( szSpl32CreateQueue, &Parm1 );
   T_FreeLog( );

   ulRC = SPLCREATEQUEUE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32CreateQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32CreateQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32CreateQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32DeleteDevice()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLDELETEDEVICE( PSZ, PSZ );
PCHAR szSpl32DeleteDevice = "Spl32DeleteDevice";

SPLERR APIENTRY Trc_Spl32DeleteDevice( PSZ Parm1
                                     , PSZ Parm2
                                     )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32DeleteDevice );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szSpl32DeleteDevice, &Parm1 );
   T_FreeLog( );

   ulRC = SPLDELETEDEVICE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32DeleteDevice );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32DeleteDevice, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32DeleteDevice() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32DeleteJob()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLDELETEJOB( PSZ, PSZ, ULONG );
PCHAR szSpl32DeleteJob = "Spl32DeleteJob";

SPLERR APIENTRY Trc_Spl32DeleteJob( PSZ   Parm1
                                  , PSZ   Parm2
                                  , ULONG Parm3
                                  )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32DeleteJob );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szSpl32DeleteJob, &Parm1 );
   T_FreeLog( );

   ulRC = SPLDELETEJOB( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32DeleteJob );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32DeleteJob, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32DeleteJob() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32DeletePort()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
SPLERR APIENTRY SPLDELETEPORT( PSZ, PSZ );
PCHAR szSpl32DeletePort = "Spl32DeletePort";

SPLERR APIENTRY Trc_Spl32DeletePort( PSZ Parm1
                                   , PSZ Parm2
                                   )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32DeletePort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szSpl32DeletePort, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32DELETEPORT )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32DeletePort );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32DeletePort, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32DeletePort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32DeleteQueue()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLDELETEQUEUE( PSZ, PSZ );
PCHAR szSpl32DeleteQueue = "Spl32DeleteQueue";

SPLERR APIENTRY Trc_Spl32DeleteQueue( PSZ Parm1
                                    , PSZ Parm2
                                    )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32DeleteQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szSpl32DeleteQueue, &Parm1 );
   T_FreeLog( );

   ulRC = SPLDELETEQUEUE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32DeleteQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32DeleteQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32DeleteQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32Disable()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLDISABLE( PSZ, PVOID );
PCHAR szSpl32Disable = "Spl32Disable";

ULONG APIENTRY Trc_Spl32Disable( PSZ   Parm1
                               , PVOID Parm2
                               )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32Disable );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PVOID );
      T_LogPVOID( Parm2 );
   }
   HOOKENTRY( szSpl32Disable, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32DISABLE )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32Disable );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32Disable, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32Disable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32DisplayControlPanel()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLDISPLAYCONTROLPANEL( PSZ, PSZ, PSZ, PSZ, HAB, ULONG );
PCHAR szSpl32DisplayControlPanel = "Spl32DisplayControlPanel";

ULONG APIENTRY Trc_Spl32DisplayControlPanel( PSZ   Parm1
                                           , PSZ   Parm2
                                           , PSZ   Parm3
                                           , PSZ   Parm4
                                           , HAB   Parm5
                                           , ULONG Parm6
                                           )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32DisplayControlPanel );
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
      T_LogParameter( 5L, DT_HAB );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szSpl32DisplayControlPanel, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32DISPLAYCONTROLPANEL )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32DisplayControlPanel );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32DisplayControlPanel, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32DisplayControlPanel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32Enable()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLENABLE( PSZ, PSZ, PVOID );
PCHAR szSpl32Enable = "Spl32Enable";

ULONG APIENTRY Trc_Spl32Enable( PSZ   Parm1
                              , PSZ   Parm2
                              , PVOID Parm3
                              )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32Enable );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
   }
   HOOKENTRY( szSpl32Enable, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32ENABLE )( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32Enable );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32Enable, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32Enable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32EnumDevice()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLENUMDEVICE( PSZ, ULONG, PVOID, ULONG, PULONG, PULONG, PULONG, PVOID );
PCHAR szSpl32EnumDevice = "Spl32EnumDevice";

SPLERR APIENTRY Trc_Spl32EnumDevice( PSZ    Parm1
                                   , ULONG  Parm2
                                   , PVOID  Parm3
                                   , ULONG  Parm4
                                   , PULONG Parm5
                                   , PULONG Parm6
                                   , PULONG Parm7
                                   , PVOID  Parm8
                                   )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32EnumDevice );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_PVOID );
      T_LogPVOID( Parm8 );
   }
   HOOKENTRY( szSpl32EnumDevice, &Parm1 );
   T_FreeLog( );

   ulRC = SPLENUMDEVICE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32EnumDevice );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      if ( ( Parm5 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogS32EDe_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32EnumDevice, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32EnumDevice() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32EnumDriver()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLENUMDRIVER( PSZ, ULONG, PVOID, ULONG, PULONG, PULONG, PULONG, PVOID );
PCHAR szSpl32EnumDriver = "Spl32EnumDriver";

SPLERR APIENTRY Trc_Spl32EnumDriver( PSZ    Parm1
                                   , ULONG  Parm2
                                   , PVOID  Parm3
                                   , ULONG  Parm4
                                   , PULONG Parm5
                                   , PULONG Parm6
                                   , PULONG Parm7
                                   , PVOID  Parm8
                                   )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32EnumDriver );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_PVOID );
      T_LogPVOID( Parm8 );
   }
   HOOKENTRY( szSpl32EnumDriver, &Parm1 );
   T_FreeLog( );

   ulRC = SPLENUMDRIVER( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32EnumDriver );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      if ( ( Parm5 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogS32EDr_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32EnumDriver, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32EnumDriver() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32EnumJob()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLENUMJOB( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG, PULONG, PULONG, PVOID );
PCHAR szSpl32EnumJob = "Spl32EnumJob";

SPLERR APIENTRY Trc_Spl32EnumJob( PSZ    Parm1
                                , PSZ    Parm2
                                , ULONG  Parm3
                                , PVOID  Parm4
                                , ULONG  Parm5
                                , PULONG Parm6
                                , PULONG Parm7
                                , PULONG Parm8
                                , PVOID  Parm9
                                )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32EnumJob );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_PULONG );
      T_LogPVOID( Parm8 );
      T_LogParameter( 9L, DT_PVOID );
      T_LogPVOID( Parm9 );
   }
   HOOKENTRY( szSpl32EnumJob, &Parm1 );
   T_FreeLog( );

   ulRC = SPLENUMJOB( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32EnumJob );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( Parm6 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogS32EJ_Parm4( Parm4, *Parm6, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
      T_LogParameter( 8L, DT_PULONG );
      T_LogPULONG( Parm8 );
   }
   HOOKEXIT( szSpl32EnumJob, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32EnumJob() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32EnumPort()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLENUMPORT( PSZ, ULONG, PVOID, ULONG, PULONG, PULONG, PULONG, PVOID );
PCHAR szSpl32EnumPort = "Spl32EnumPort";

SPLERR APIENTRY Trc_Spl32EnumPort( PSZ    Parm1
                                 , ULONG  Parm2
                                 , PVOID  Parm3
                                 , ULONG  Parm4
                                 , PULONG Parm5
                                 , PULONG Parm6
                                 , PULONG Parm7
                                 , PVOID  Parm8
                                 )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32EnumPort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_PVOID );
      T_LogPVOID( Parm8 );
   }
   HOOKENTRY( szSpl32EnumPort, &Parm1 );
   T_FreeLog( );

   ulRC = SPLENUMPORT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32EnumPort );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      if ( ( Parm5 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogS32EPo_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32EnumPort, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32EnumPort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32EnumPrinter()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLENUMPRINTER( PSZ, ULONG, ULONG, PVOID, ULONG, PULONG, PULONG, PULONG, PVOID );
PCHAR szSpl32EnumPrinter = "Spl32EnumPrinter";

SPLERR APIENTRY Trc_Spl32EnumPrinter( PSZ    Parm1
                                    , ULONG  Parm2
                                    , ULONG  Parm3
                                    , PVOID  Parm4
                                    , ULONG  Parm5
                                    , PULONG Parm6
                                    , PULONG Parm7
                                    , PULONG Parm8
                                    , PVOID  Parm9
                                    )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32EnumPrinter );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_PULONG );
      T_LogPVOID( Parm8 );
      T_LogParameter( 9L, DT_PVOID );
      T_LogPVOID( Parm9 );
   }
   HOOKENTRY( szSpl32EnumPrinter, &Parm1 );
   T_FreeLog( );

   ulRC = SPLENUMPRINTER( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32EnumPrinter );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( Parm6 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogS32EPr_Parm4( Parm4, *Parm6, Parm5, Parm2 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
      T_LogParameter( 8L, DT_PULONG );
      T_LogPULONG( Parm8 );
   }
   HOOKEXIT( szSpl32EnumPrinter, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32EnumPrinter() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32EnumQueue()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLENUMQUEUE( PSZ, ULONG, PVOID, ULONG, PULONG, PULONG, PULONG, PVOID );
PCHAR szSpl32EnumQueue = "Spl32EnumQueue";

SPLERR APIENTRY Trc_Spl32EnumQueue( PSZ    Parm1
                                  , ULONG  Parm2
                                  , PVOID  Parm3
                                  , ULONG  Parm4
                                  , PULONG Parm5
                                  , PULONG Parm6
                                  , PULONG Parm7
                                  , PVOID  Parm8
                                  )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32EnumQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_PVOID );
      T_LogPVOID( Parm8 );
   }
   HOOKENTRY( szSpl32EnumQueue, &Parm1 );
   T_FreeLog( );

   ulRC = SPLENUMQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32EnumQueue );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      if ( ( Parm5 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogS32EQ_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32EnumQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32EnumQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32EnumQueueProcessor()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLENUMQUEUEPROCESSOR( PSZ, ULONG, PVOID, ULONG, PULONG, PULONG, PULONG, PVOID );
PCHAR szSpl32EnumQueueProcessor = "Spl32EnumQueueProcessor";

SPLERR APIENTRY Trc_Spl32EnumQueueProcessor( PSZ    Parm1
                                           , ULONG  Parm2
                                           , PVOID  Parm3
                                           , ULONG  Parm4
                                           , PULONG Parm5
                                           , PULONG Parm6
                                           , PULONG Parm7
                                           , PVOID  Parm8
                                           )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32EnumQueueProcessor );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
      T_LogParameter( 8L, DT_PVOID );
      T_LogPVOID( Parm8 );
   }
   HOOKENTRY( szSpl32EnumQueueProcessor, &Parm1 );
   T_FreeLog( );

   ulRC = SPLENUMQUEUEPROCESSOR( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32EnumQueueProcessor );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      if ( ( Parm5 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogS32EQP_Parm3( Parm3, *Parm5, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32EnumQueueProcessor, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32EnumQueueProcessor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32GetControlPanelList()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLGETCONTROLPANELLIST( PSZ, PSZ, PSZ, ULONG, PSZ, ULONG, PULONG );
PCHAR szSpl32GetControlPanelList = "Spl32GetControlPanelList";

ULONG APIENTRY Trc_Spl32GetControlPanelList( PSZ    Parm1
                                           , PSZ    Parm2
                                           , PSZ    Parm3
                                           , ULONG  Parm4
                                           , PSZ    Parm5
                                           , ULONG  Parm6
                                           , PULONG Parm7
                                           )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32GetControlPanelList );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PSZ );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
   }
   HOOKENTRY( szSpl32GetControlPanelList, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32GETCONTROLPANELLIST )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32GetControlPanelList );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PSZ );
      if ( !ulRC ) T_LogPSZ( Parm5 );
      else T_LogPVOID( Parm5 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32GetControlPanelList, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32GetControlPanelList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32GetPortFromQ()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLGETPORTFROMQ( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
PCHAR szSpl32GetPortFromQ = "Spl32GetPortFromQ";

ULONG APIENTRY Trc_Spl32GetPortFromQ( PSZ    Parm1
                                    , PSZ    Parm2
                                    , ULONG  Parm3
                                    , PVOID  Parm4
                                    , ULONG  Parm5
                                    , PULONG Parm6
                                    )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32GetPortFromQ );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
   }
   HOOKENTRY( szSpl32GetPortFromQ, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32GETPORTFROMQ )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32GetPortFromQ );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) T_LogS32GPFQ_Parm4( Parm4, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKEXIT( szSpl32GetPortFromQ, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32GetPortFromQ() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32HoldJob()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLHOLDJOB( PSZ, PSZ, ULONG );
PCHAR szSpl32HoldJob = "Spl32HoldJob";

SPLERR APIENTRY Trc_Spl32HoldJob( PSZ   Parm1
                                , PSZ   Parm2
                                , ULONG Parm3
                                )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32HoldJob );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szSpl32HoldJob, &Parm1 );
   T_FreeLog( );

   ulRC = SPLHOLDJOB( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32HoldJob );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32HoldJob, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32HoldJob() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32HoldQueue()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLHOLDQUEUE( PSZ, PSZ );
PCHAR szSpl32HoldQueue = "Spl32HoldQueue";

SPLERR APIENTRY Trc_Spl32HoldQueue( PSZ Parm1
                                  , PSZ Parm2
                                  )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32HoldQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szSpl32HoldQueue, &Parm1 );
   T_FreeLog( );

   ulRC = SPLHOLDQUEUE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32HoldQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32HoldQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32HoldQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16MessageBox()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 SPLMESSAGEBOX( PSZ16, USHORT, USHORT, PSZ16, PSZ16, USHORT, USHORT );
PCHAR szSpl16MessageBox = "Spl16MessageBox";

USHORT APIENTRY16 Trc_Spl16MessageBox( PSZ16  Parm1
                                     , USHORT Parm2
                                     , USHORT Parm3
                                     , PSZ16  Parm4
                                     , PSZ16  Parm5
                                     , USHORT Parm6
                                     , USHORT Parm7
                                     )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16MessageBox );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_USHORT );
      T_LogUSHORT( Parm2 );
      T_LogParameter( 3L, DT_USHORT );
      T_LogUSHORT( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ16( Parm4 );
      T_LogParameter( 5L, DT_PSZ );
      T_LogPSZ16( Parm5 );
      T_LogParameter( 6L, DT_USHORT );
      T_LogUSHORT( Parm6 );
      T_LogParameter( 7L, DT_USHORT );
      T_LogUSHORT( Parm7 );
   }
   HOOKENTRY16( szSpl16MessageBox, &Parm1 );
   T_FreeLog( );

   usRC = SPLMESSAGEBOX( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16MessageBox );
   T_LogPMReturnCode( DT_USHORT, usRC, usRC != MBID_ERROR );
   HOOKEXIT16( szSpl16MessageBox, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Spl16MessageBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32MessageBox()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY SPL32MESSAGEBOX( PSZ, ULONG, ULONG, PSZ, PSZ, ULONG, ULONG );
PCHAR szSpl32MessageBox = "Spl32MessageBox";

ULONG APIENTRY Trc_Spl32MessageBox( PSZ   Parm1
                                  , ULONG Parm2
                                  , ULONG Parm3
                                  , PSZ   Parm4
                                  , PSZ   Parm5
                                  , ULONG Parm6
                                  , ULONG Parm7
                                  )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32MessageBox );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ( Parm4 );
      T_LogParameter( 5L, DT_PSZ );
      T_LogPSZ( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_ULONG );
      T_LogULONG( Parm7 );
   }
   HOOKENTRY( szSpl32MessageBox, &Parm1 );
   T_FreeLog( );

   ulRC = SPL32MESSAGEBOX( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32MessageBox );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != MBID_ERROR );
   HOOKEXIT( szSpl32MessageBox, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32MessageBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32ProtSendCmd()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLPROTSENDCMD( PSZ, ULONG, ULONG, PFN, PFN, PVOID, ULONG, PVOID, PULONG );
PCHAR szSpl32ProtSendCmd = "Spl32ProtSendCmd";

ULONG APIENTRY Trc_Spl32ProtSendCmd( PSZ    Parm1
                                   , ULONG  Parm2
                                   , ULONG  Parm3
                                   , PFN    Parm4
                                   , PFN    Parm5
                                   , PVOID  Parm6
                                   , ULONG  Parm7
                                   , PVOID  Parm8
                                   , PULONG Parm9
                                   )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32ProtSendCmd );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PFN );
      T_LogPFN( Parm4 );
      T_LogParameter( 5L, DT_PFN );
      T_LogPFN( Parm5 );
      T_LogParameter( 6L, DT_PVOID );
      T_LogPVOID( Parm6 );
      T_LogCHARBuffer( Parm6, Parm7 );
      T_LogParameter( 7L, DT_ULONG );
      T_LogULONG( Parm7 );
      T_LogParameter( 8L, DT_PVOID );
      T_LogPVOID( Parm8 );
      T_LogParameter( 9L, DT_PULONG );
      T_LogPULONG( Parm9 );
   }
   HOOKENTRY( szSpl32ProtSendCmd, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32PROTSENDCMD )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32ProtSendCmd );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 8L, DT_PVOID );
      T_LogPVOID( Parm8 );
      if ( ( Parm9 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) ) T_LogCHARBuffer( Parm8, *Parm9 );
      T_LogParameter( 9L, DT_PULONG );
      T_LogPULONG( Parm9 );
   }
   HOOKEXIT( szSpl32ProtSendCmd, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32ProtSendCmd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32ProtXlateCmd()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLPROTXLATECMD( PSZ, PFN, PVOID, ULONG, PVOID, PVOID, PULONG );
PCHAR szSpl32ProtXlateCmd = "Spl32ProtXlateCmd";

#define PD_ALERT      0x00000001
#define PD_RESPONSE   0x00000002
#define PD_PARTIAL    0x00000004
#define PD_SNMP       0x00000008
#define PD_ALTERNATE  0x00000010
#define PD_IGNORE     0x00000020
#define PD_NEXTALERT  0x00001000
#define PD_MOREALERTS 0x00002000

ULONG APIENTRY Trc_Spl32ProtXlateCmd( PSZ    Parm1
                                    , PFN    Parm2
                                    , PVOID  Parm3
                                    , ULONG  Parm4
                                    , PVOID  Parm5
                                    , PVOID  Parm6
                                    , PULONG Parm7
                                    )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32ProtXlateCmd );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PFN );
      T_LogPFN( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogCHARBuffer( Parm3, Parm4 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      T_LogPPDALERTINFO( Parm5 );
      T_LogParameter( 6L, DT_PVOID );
      T_LogPVOID( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKENTRY( szSpl32ProtXlateCmd, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32PROTXLATECMD )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32ProtXlateCmd );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      if ( !ulRC ) T_LogPPDALERTINFO( Parm5 );
      T_LogParameter( 6L, DT_PVOID );
      T_LogPVOID( Parm6 );
      if ( ( Parm7 ) && ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) && ( T_IsValidMem( Parm5, sizeof( PDALERTINFO ) ) ) && ( FLAGSET( ((PPDALERTINFO)Parm5)->ulFlags, PD_ALERT ) ) ) T_LogPPRTALERT( Parm6, *Parm7 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32ProtXlateCmd, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32ProtXlateCmd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32PurgeQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLPURGEQUEUE( PSZ, PSZ );
PCHAR szSpl32PurgeQueue = "Spl32PurgeQueue";

SPLERR APIENTRY Trc_Spl32PurgeQueue( PSZ Parm1
                                   , PSZ Parm2
                                   )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32PurgeQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szSpl32PurgeQueue, &Parm1 );
   T_FreeLog( );

   ulRC = SPLPURGEQUEUE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32PurgeQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32PurgeQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32PurgeQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16QmAbort()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLQMABORT( HSPL );
PCHAR szSpl16QmAbort = "Spl16QmAbort";

BOOL16 APIENTRY16 Trc_Spl16QmAbort( HSPL Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16QmAbort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16QmAbort, &Parm1 );
   T_FreeLog( );

   bRC = SPLQMABORT( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16QmAbort );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16QmAbort, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16QmAbort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmAbort()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32QMABORT( HSPL );
PCHAR szSpl32QmAbort = "Spl32QmAbort";

BOOL APIENTRY Trc_Spl32QmAbort( HSPL Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmAbort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32QmAbort, &Parm1 );
   T_FreeLog( );

   bRC = SPL32QMABORT( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmAbort );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32QmAbort, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32QmAbort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16QmAbortDoc()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLQMABORTDOC( HSPL );
PCHAR szSpl16QmAbortDoc = "Spl16QmAbortDoc";

BOOL16 APIENTRY16 Trc_Spl16QmAbortDoc( HSPL Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16QmAbortDoc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16QmAbortDoc, &Parm1 );
   T_FreeLog( );

   bRC = SPLQMABORTDOC( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16QmAbortDoc );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16QmAbortDoc, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16QmAbortDoc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmAbortDoc()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32QMABORTDOC( HSPL );
PCHAR szSpl32QmAbortDoc = "Spl32QmAbortDoc";

BOOL APIENTRY Trc_Spl32QmAbortDoc( HSPL Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmAbortDoc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32QmAbortDoc, &Parm1 );
   T_FreeLog( );

   bRC = SPL32QMABORTDOC( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmAbortDoc );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32QmAbortDoc, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32QmAbortDoc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16QmClose()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLQMCLOSE( HSPL );
PCHAR szSpl16QmClose = "Spl16QmClose";

BOOL16 APIENTRY16 Trc_Spl16QmClose( HSPL Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16QmClose );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16QmClose, &Parm1 );
   T_FreeLog( );

   bRC = SPLQMCLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16QmClose );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16QmClose, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16QmClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmClose()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32QMCLOSE( HSPL );
PCHAR szSpl32QmClose = "Spl32QmClose";

BOOL APIENTRY Trc_Spl32QmClose( HSPL Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmClose );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32QmClose, &Parm1 );
   T_FreeLog( );

   bRC = SPL32QMCLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmClose );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32QmClose, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32QmClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16QmEndDoc()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 SPLQMENDDOC( HSPL );
PCHAR szSpl16QmEndDoc = "Spl16QmEndDoc";

USHORT APIENTRY16 Trc_Spl16QmEndDoc( HSPL Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16QmEndDoc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16QmEndDoc, &Parm1 );
   T_FreeLog( );

   usRC = SPLQMENDDOC( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16QmEndDoc );
   T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC != SPL_ERROR );
   HOOKEXIT16( szSpl16QmEndDoc, &Parm1, &usRC );
   T_FreeLog( );

   return usRC;

}  /* Trc_Spl16QmEndDoc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmEndDoc()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY SPL32QMENDDOC( HSPL );
PCHAR szSpl32QmEndDoc = "Spl32QmEndDoc";

ULONG APIENTRY Trc_Spl32QmEndDoc( HSPL Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmEndDoc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32QmEndDoc, &Parm1 );
   T_FreeLog( );

   ulRC = SPL32QMENDDOC( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmEndDoc );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != SPL_ERROR );
   HOOKEXIT( szSpl32QmEndDoc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QmEndDoc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmGetJobID()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY SPLQMGETJOBID( HSPL, ULONG, PVOID, ULONG, PULONG );
PCHAR szSpl32QmGetJobID = "Spl32QmGetJobID";

ULONG APIENTRY Trc_Spl32QmGetJobID( HSPL   Parm1
                                  , ULONG  Parm2
                                  , PVOID  Parm3
                                  , ULONG  Parm4
                                  , PULONG Parm5
                                  )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmGetJobID );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPVOID( Parm5 );
   }
   HOOKENTRY( szSpl32QmGetJobID, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32QMGETJOBID )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmGetJobID );
   T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != SPL_ERROR );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      if ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) T_LogS32QGJI_Parm3( Parm3, Parm4, Parm2 );
      T_LogParameter( 5L, DT_PULONG );
      T_LogPULONG( Parm5 );
   }
   HOOKEXIT( szSpl32QmGetJobID, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QmGetJobID() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmNewPage()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
BOOL APIENTRY SPLQMNEWPAGE( HSPL, ULONG );
PCHAR szSpl32QmNewPage = "Spl32QmNewPage";

BOOL APIENTRY Trc_Spl32QmNewPage( HSPL  Parm1
                                , ULONG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmNewPage );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_ULONG );
      T_LogULONG( Parm2 );
   }
   HOOKENTRY( szSpl32QmNewPage, &Parm1 );
   T_FreeLog( );

   bRC = ( *pfnSPL32QMNEWPAGE )( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmNewPage );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32QmNewPage, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32QmNewPage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16QmOpen()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HSPL APIENTRY16 SPLQMOPEN( PSZ16, LONG, PQMOPENDATA16 );
PCHAR szSpl16QmOpen = "Spl16QmOpen";

HSPL APIENTRY16 Trc_Spl16QmOpen( PSZ16         Parm1
                               , LONG          Parm2
                               , PQMOPENDATA16 Parm3
                               )
{

   HSPL ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16QmOpen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ16( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PQMOPENDATA );
      T_LogPDEVOPENSTRUC16( (PDEVOPENSTRUC16)Parm3, Parm2 );
   }
   HOOKENTRY16( szSpl16QmOpen, &Parm1 );
   T_FreeLog( );

   ulRC = SPLQMOPEN( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16QmOpen );
   T_LogPMReturnCode( DT_HSPL, ulRC, ulRC != SPL_ERROR );
   HOOKEXIT16( szSpl16QmOpen, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl16QmOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmOpen()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HSPL APIENTRY SPL32QMOPEN( PSZ, LONG, PQMOPENDATA );
PCHAR szSpl32QmOpen = "Spl32QmOpen";

HSPL APIENTRY Trc_Spl32QmOpen( PSZ         Parm1
                             , LONG        Parm2
                             , PQMOPENDATA Parm3
                             )
{

   HSPL ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmOpen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PQMOPENDATA );
      T_LogPDEVOPENSTRUC( (PDEVOPENSTRUC)Parm3, Parm2 );
   }
   HOOKENTRY( szSpl32QmOpen, &Parm1 );
   T_FreeLog( );

   ulRC = SPL32QMOPEN( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmOpen );
   T_LogPMReturnCode( DT_HSPL, ulRC, ulRC != SPL_ERROR );
   HOOKEXIT( szSpl32QmOpen, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QmOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16QmStartDoc()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLQMSTARTDOC( HSPL, PSZ16 );
PCHAR szSpl16QmStartDoc = "Spl16QmStartDoc";

BOOL16 APIENTRY16 Trc_Spl16QmStartDoc( HSPL  Parm1
                                     , PSZ16 Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16QmStartDoc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ16( Parm2 );
   }
   HOOKENTRY16( szSpl16QmStartDoc, &Parm1 );
   T_FreeLog( );

   bRC = SPLQMSTARTDOC( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16QmStartDoc );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16QmStartDoc, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16QmStartDoc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmStartDoc()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32QMSTARTDOC( HSPL, PSZ );
PCHAR szSpl32QmStartDoc = "Spl32QmStartDoc";

BOOL APIENTRY Trc_Spl32QmStartDoc( HSPL Parm1
                                 , PSZ  Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmStartDoc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szSpl32QmStartDoc, &Parm1 );
   T_FreeLog( );

   bRC = SPL32QMSTARTDOC( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmStartDoc );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32QmStartDoc, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32QmStartDoc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16QmWrite()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLQMWRITE( HSPL, LONG, PBYTE16 );
PCHAR szSpl16QmWrite = "Spl16QmWrite";

BOOL16 APIENTRY16 Trc_Spl16QmWrite( HSPL    Parm1
                                  , LONG    Parm2
                                  , PBYTE16 Parm3
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16QmWrite );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PBYTE );
      T_LogPVOID16( Parm3 );
      T_LogCHARBuffer16( (PCHAR16)Parm3, Parm2 );
   }
   HOOKENTRY16( szSpl16QmWrite, &Parm1 );
   T_FreeLog( );

   bRC = SPLQMWRITE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16QmWrite );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16QmWrite, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16QmWrite() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QmWrite()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32QMWRITE( HSPL, LONG, PVOID );
PCHAR szSpl32QmWrite = "Spl32QmWrite";

BOOL APIENTRY Trc_Spl32QmWrite( HSPL  Parm1
                              , LONG  Parm2
                              , PVOID Parm3
                              )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QmWrite );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSPL );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_PVOID );
      T_LogPVOID( Parm3 );
      T_LogCHARBuffer( Parm3, Parm2 );
   }
   HOOKENTRY( szSpl32QmWrite, &Parm1 );
   T_FreeLog( );

   bRC = SPL32QMWRITE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QmWrite );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32QmWrite, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32QmWrite() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QueryControlPanel()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLQUERYCONTROLPANEL( PSZ, PSZ, PSZ, PSZ, ULONG, PULONG );
PCHAR szSpl32QueryControlPanel = "Spl32QueryControlPanel";

ULONG APIENTRY Trc_Spl32QueryControlPanel( PSZ    Parm1
                                         , PSZ    Parm2
                                         , PSZ    Parm3
                                         , PSZ    Parm4
                                         , ULONG  Parm5
                                         , PULONG Parm6
                                         )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QueryControlPanel );
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
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
   }
   HOOKENTRY( szSpl32QueryControlPanel, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32QUERYCONTROLPANEL )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QueryControlPanel );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKEXIT( szSpl32QueryControlPanel, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QueryControlPanel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QueryDevice()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLQUERYDEVICE( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
PCHAR szSpl32QueryDevice = "Spl32QueryDevice";

SPLERR APIENTRY Trc_Spl32QueryDevice( PSZ    Parm1
                                    , PSZ    Parm2
                                    , ULONG  Parm3
                                    , PVOID  Parm4
                                    , ULONG  Parm5
                                    , PULONG Parm6
                                    )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QueryDevice );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
   }
   HOOKENTRY( szSpl32QueryDevice, &Parm1 );
   T_FreeLog( );

   ulRC = SPLQUERYDEVICE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QueryDevice );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) T_LogS32QDe_Parm4( Parm4, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKEXIT( szSpl32QueryDevice, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QueryDevice() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QueryDriver()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLQUERYDRIVER( PSZ, PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
PCHAR szSpl32QueryDriver = "Spl32QueryDriver";

SPLERR APIENTRY Trc_Spl32QueryDriver( PSZ    Parm1
                                    , PSZ    Parm2
                                    , PSZ    Parm3
                                    , ULONG  Parm4
                                    , PVOID  Parm5
                                    , ULONG  Parm6
                                    , PULONG Parm7
                                    )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QueryDriver );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
   }
   HOOKENTRY( szSpl32QueryDriver, &Parm1 );
   T_FreeLog( );

   ulRC = SPLQUERYDRIVER( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QueryDriver );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      if ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) T_LogS32QDr_Parm5( Parm5, Parm6, Parm4 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32QueryDriver, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QueryDriver() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QueryJob()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLQUERYJOB( PSZ, PSZ, ULONG, ULONG, PVOID, ULONG, PULONG );
PCHAR szSpl32QueryJob = "Spl32QueryJob";

SPLERR APIENTRY Trc_Spl32QueryJob( PSZ    Parm1
                                 , PSZ    Parm2
                                 , ULONG  Parm3
                                 , ULONG  Parm4
                                 , PVOID  Parm5
                                 , ULONG  Parm6
                                 , PULONG Parm7
                                 )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QueryJob );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPVOID( Parm7 );
   }
   HOOKENTRY( szSpl32QueryJob, &Parm1 );
   T_FreeLog( );

   ulRC = SPLQUERYJOB( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QueryJob );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      if ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) T_LogS32QJ_Parm5( Parm5, Parm6, Parm4 );
      T_LogParameter( 7L, DT_PULONG );
      T_LogPULONG( Parm7 );
   }
   HOOKEXIT( szSpl32QueryJob, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QueryJob() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QueryPath()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLQUERYPATH( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
PCHAR szSpl32QueryPath = "Spl32QueryPath";

ULONG APIENTRY Trc_Spl32QueryPath( PSZ    Parm1
                                 , PSZ    Parm2
                                 , ULONG  Parm3
                                 , PVOID  Parm4
                                 , ULONG  Parm5
                                 , PULONG Parm6
                                 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QueryPath );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
   }
   HOOKENTRY( szSpl32QueryPath, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32QUERYPATH )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QueryPath );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) T_LogS32QPa_Parm4( Parm4, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKEXIT( szSpl32QueryPath, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QueryPath() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QueryPort()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLQUERYPORT( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
PCHAR szSpl32QueryPort = "Spl32QueryPort";

ULONG APIENTRY Trc_Spl32QueryPort( PSZ    Parm1
                                 , PSZ    Parm2
                                 , ULONG  Parm3
                                 , PVOID  Parm4
                                 , ULONG  Parm5
                                 , PULONG Parm6
                                 )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QueryPort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
   }
   HOOKENTRY( szSpl32QueryPort, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32QUERYPORT )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QueryPort );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) T_LogS32QPo_Parm4( Parm4, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKEXIT( szSpl32QueryPort, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QueryPort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32QueryQueue()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLQUERYQUEUE( PSZ, PSZ, ULONG, PVOID, ULONG, PULONG );
PCHAR szSpl32QueryQueue = "Spl32QueryQueue";

SPLERR APIENTRY Trc_Spl32QueryQueue( PSZ    Parm1
                                   , PSZ    Parm2
                                   , ULONG  Parm3
                                   , PVOID  Parm4
                                   , ULONG  Parm5
                                   , PULONG Parm6
                                   )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32QueryQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPVOID( Parm6 );
   }
   HOOKENTRY( szSpl32QueryQueue, &Parm1 );
   T_FreeLog( );

   ulRC = SPLQUERYQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32QueryQueue );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( !ulRC ) || ( ulRC == ERROR_MORE_DATA ) ) T_LogS32QQ_Parm4( Parm4, Parm5, Parm3 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKEXIT( szSpl32QueryQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32QueryQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32Register()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLREGISTER( PSZ, PSZ, HWND, ULONG, ULONG, PULONG );
PCHAR szSpl32Register = "Spl32Register";

ULONG APIENTRY Trc_Spl32Register( PSZ    Parm1
                                , PSZ    Parm2
                                , HWND   Parm3
                                , ULONG  Parm4
                                , ULONG  Parm5
                                , PULONG Parm6
                                )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32Register );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_HWND );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKENTRY( szSpl32Register, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32REGISTER )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32Register );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 6L, DT_PULONG );
      T_LogPULONG( Parm6 );
   }
   HOOKEXIT( szSpl32Register, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32Register() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32RegisterControlPanel()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLREGISTERCONTROLPANEL( PSZ, PSZ, ULONG, PSZ, ULONG );
PCHAR szSpl32RegisterControlPanel = "Spl32RegisterControlPanel";

ULONG APIENTRY Trc_Spl32RegisterControlPanel( PSZ   Parm1
                                            , PSZ   Parm2
                                            , ULONG Parm3
                                            , PSZ   Parm4
                                            , ULONG Parm5
                                            )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32RegisterControlPanel );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PSZ );
      T_LogPSZ( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY( szSpl32RegisterControlPanel, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32REGISTERCONTROLPANEL )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32RegisterControlPanel );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32RegisterControlPanel, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32RegisterControlPanel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32ReleaseJob()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLRELEASEJOB( PSZ, PSZ, ULONG );
PCHAR szSpl32ReleaseJob = "Spl32ReleaseJob";

SPLERR APIENTRY Trc_Spl32ReleaseJob( PSZ    Parm1
                                   , PSZ    Parm2
                                   , ULONG  Parm3
                                   )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32ReleaseJob );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
   }
   HOOKENTRY( szSpl32ReleaseJob, &Parm1 );
   T_FreeLog( );

   ulRC = SPLRELEASEJOB( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32ReleaseJob );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32ReleaseJob, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32ReleaseJob() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32ReleaseQueue()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLRELEASEQUEUE( PSZ, PSZ );
PCHAR szSpl32ReleaseQueue = "Spl32ReleaseQueue";

SPLERR APIENTRY Trc_Spl32ReleaseQueue( PSZ Parm1
                                     , PSZ Parm2
                                     )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32ReleaseQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
   }
   HOOKENTRY( szSpl32ReleaseQueue, &Parm1 );
   T_FreeLog( );

   ulRC = SPLRELEASEQUEUE( Parm1, Parm2 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32ReleaseQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32ReleaseQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32ReleaseQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32SetDevice()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLSETDEVICE( PSZ, PSZ, ULONG, PVOID, ULONG, ULONG );
PCHAR szSpl32SetDevice = "Spl32SetDevice";

SPLERR APIENTRY Trc_Spl32SetDevice( PSZ   Parm1
                                  , PSZ   Parm2
                                  , ULONG Parm3
                                  , PVOID Parm4
                                  , ULONG Parm5
                                  , ULONG Parm6
                                  )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32SetDevice );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( Parm3 == 3L ) T_LogS32SDe_Parm4( Parm4, Parm5, Parm6 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szSpl32SetDevice, &Parm1 );
   T_FreeLog( );

   ulRC = SPLSETDEVICE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32SetDevice );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32SetDevice, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32SetDevice() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32SetDriver()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLSETDRIVER( PSZ, PSZ, PSZ, ULONG, PVOID, ULONG, ULONG );
PCHAR szSpl32SetDriver = "Spl32SetDriver";

SPLERR APIENTRY Trc_Spl32SetDriver( PSZ   Parm1
                                  , PSZ   Parm2
                                  , PSZ   Parm3
                                  , ULONG Parm4
                                  , PVOID Parm5
                                  , ULONG Parm6
                                  , ULONG Parm7
                                  )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32SetDriver );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_PSZ );
      T_LogPSZ( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      if ( Parm4 == 2L ) T_LogS32SDr_Parm5( Parm5, Parm6, Parm7 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_ULONG );
      T_LogULONG( Parm7 );
   }
   HOOKENTRY( szSpl32SetDriver, &Parm1 );
   T_FreeLog( );

   ulRC = SPLSETDRIVER( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32SetDriver );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32SetDriver, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32SetDriver() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32SetJob()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLSETJOB( PSZ, PSZ, ULONG, ULONG, PVOID, ULONG, ULONG );
PCHAR szSpl32SetJob = "Spl32SetJob";

SPLERR APIENTRY Trc_Spl32SetJob( PSZ   Parm1
                               , PSZ   Parm2
                               , ULONG Parm3
                               , ULONG Parm4
                               , PVOID Parm5
                               , ULONG Parm6
                               , ULONG Parm7
                               )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32SetJob );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_PVOID );
      T_LogPVOID( Parm5 );
      if ( ( Parm4 == 3L ) || ( Parm4 == 4L ) ) T_LogS32SJ_Parm5( Parm5, Parm6, Parm4, Parm7 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
      T_LogParameter( 7L, DT_ULONG );
      T_LogULONG( Parm7 );
   }
   HOOKENTRY( szSpl32SetJob, &Parm1 );
   T_FreeLog( );

   ulRC = SPLSETJOB( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32SetJob );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32SetJob, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32SetJob() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32SetPort()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
SPLERR APIENTRY SPLSETPORT( PSZ, PSZ, ULONG, PVOID, ULONG, ULONG );
PCHAR szSpl32SetPort = "Spl32SetPort";

SPLERR APIENTRY Trc_Spl32SetPort( PSZ   Parm1
                                , PSZ   Parm2
                                , ULONG Parm3
                                , PVOID Parm4
                                , ULONG Parm5
                                , ULONG Parm6
                                )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32SetPort );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( Parm3 == 2L ) T_LogS32SP_Parm4( Parm4, Parm5, Parm6 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szSpl32SetPort, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32SETPORT )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32SetPort );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32SetPort, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32SetPort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32SetQueue()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
SPLERR APIENTRY SPLSETQUEUE( PSZ, PSZ, ULONG, PVOID, ULONG, ULONG );
PCHAR szSpl32SetQueue = "Spl32SetQueue";

SPLERR APIENTRY Trc_Spl32SetQueue( PSZ   Parm1
                                 , PSZ   Parm2
                                 , ULONG Parm3
                                 , PVOID Parm4
                                 , ULONG Parm5
                                 , ULONG Parm6
                                 )
{

   SPLERR ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32SetQueue );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PVOID );
      T_LogPVOID( Parm4 );
      if ( ( Parm3 == 3L ) || ( Parm3 == 6L ) || ( Parm3 == 8L ) ) T_LogS32SQ_Parm4( Parm4, Parm5, Parm3, Parm6 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
      T_LogParameter( 6L, DT_ULONG );
      T_LogULONG( Parm6 );
   }
   HOOKENTRY( szSpl32SetQueue, &Parm1 );
   T_FreeLog( );

   ulRC = SPLSETQUEUE( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32SetQueue );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32SetQueue, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32SetQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16StdClose()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLSTDCLOSE( HDC );
PCHAR szSpl16StdClose = "Spl16StdClose";

BOOL16 APIENTRY16 Trc_Spl16StdClose( HDC Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16StdClose );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16StdClose, &Parm1 );
   T_FreeLog( );

   bRC = SPLSTDCLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16StdClose );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16StdClose, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16StdClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32StdClose()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32STDCLOSE( HDC );
PCHAR szSpl32StdClose = "Spl32StdClose";

BOOL APIENTRY Trc_Spl32StdClose( HDC Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32StdClose );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32StdClose, &Parm1 );
   T_FreeLog( );

   bRC = SPL32STDCLOSE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32StdClose );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32StdClose, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32StdClose() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16StdDelete()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLSTDDELETE( HSTD );
PCHAR szSpl16StdDelete = "Spl16StdDelete";

BOOL16 APIENTRY16 Trc_Spl16StdDelete( HSTD Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16StdDelete );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTD );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16StdDelete, &Parm1 );
   T_FreeLog( );

   bRC = SPLSTDDELETE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16StdDelete );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16StdDelete, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16StdDelete() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32StdDelete()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32STDDELETE( HSTD );
PCHAR szSpl32StdDelete = "Spl32StdDelete";

BOOL APIENTRY Trc_Spl32StdDelete( HSTD Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32StdDelete );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTD );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32StdDelete, &Parm1 );
   T_FreeLog( );

   bRC = SPL32STDDELETE( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32StdDelete );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32StdDelete, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32StdDelete() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16StdGetBits()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLSTDGETBITS( HSTD, LONG, LONG, PCH16 );
PCHAR szSpl16StdGetBits = "Spl16StdGetBits";

BOOL16 APIENTRY16 Trc_Spl16StdGetBits( HSTD  Parm1
                                     , LONG  Parm2
                                     , LONG  Parm3
                                     , PCH16 Parm4
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16StdGetBits );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTD );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_LONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PCH );
      T_LogPVOID16( Parm4 );
   }
   HOOKENTRY16( szSpl16StdGetBits, &Parm1 );
   T_FreeLog( );

   bRC = SPLSTDGETBITS( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16StdGetBits );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PCH );
      T_LogPVOID16( Parm4 );
      if ( bRC ) T_LogCHARBuffer16( Parm4, Parm3 );
   }
   HOOKEXIT16( szSpl16StdGetBits, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16StdGetBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32StdGetBits()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32STDGETBITS( HSTD, LONG, LONG, PCH );
PCHAR szSpl32StdGetBits = "Spl32StdGetBits";

BOOL APIENTRY Trc_Spl32StdGetBits( HSTD Parm1
                                 , LONG Parm2
                                 , LONG Parm3
                                 , PCH  Parm4
                                 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32StdGetBits );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTD );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_LONG );
      T_LogULONG( Parm2 );
      T_LogParameter( 3L, DT_LONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_PCH );
      T_LogPVOID( Parm4 );
   }
   HOOKENTRY( szSpl32StdGetBits, &Parm1 );
   T_FreeLog( );

   bRC = SPL32STDGETBITS( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32StdGetBits );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 4L, DT_PCH );
      T_LogPVOID( Parm4 );
      if ( bRC ) T_LogCHARBuffer( Parm4, Parm3 );
   }
   HOOKEXIT( szSpl32StdGetBits, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32StdGetBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16StdOpen()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLSTDOPEN( HDC );
PCHAR szSpl16StdOpen = "Spl16StdOpen";

BOOL16 APIENTRY16 Trc_Spl16StdOpen( HDC Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16StdOpen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16StdOpen, &Parm1 );
   T_FreeLog( );

   bRC = SPLSTDOPEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16StdOpen );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16StdOpen, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16StdOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32StdOpen()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32STDOPEN( HDC );
PCHAR szSpl32StdOpen = "Spl32StdOpen";

BOOL APIENTRY Trc_Spl32StdOpen( HDC Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32StdOpen );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32StdOpen, &Parm1 );
   T_FreeLog( );

   bRC = SPL32STDOPEN( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32StdOpen );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32StdOpen, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32StdOpen() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16StdQueryLength()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 SPLSTDQUERYLENGTH( HSTD );
PCHAR szSpl16StdQueryLength = "Spl16StdQueryLength";

LONG APIENTRY16 Trc_Spl16StdQueryLength( HSTD Parm1 )
{

   LONG lRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16StdQueryLength );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTD );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16StdQueryLength, &Parm1 );
   T_FreeLog( );

   lRC = SPLSTDQUERYLENGTH( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16StdQueryLength );
   T_LogPMReturnCode( DT_LONG, lRC, lRC != SPL_ERROR );
   HOOKEXIT16( szSpl16StdQueryLength, &Parm1, &lRC );
   T_FreeLog( );

   return lRC;

}  /* Trc_Spl16StdQueryLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32StdQueryLength()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY SPL32STDQUERYLENGTH( HSTD );
PCHAR szSpl32StdQueryLength = "Spl32StdQueryLength";

LONG APIENTRY Trc_Spl32StdQueryLength( HSTD Parm1 )
{

   LONG lRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32StdQueryLength );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HSTD );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32StdQueryLength, &Parm1 );
   T_FreeLog( );

   lRC = SPL32STDQUERYLENGTH( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32StdQueryLength );
   T_LogPMReturnCode( DT_LONG, lRC, lRC != SPL_ERROR );
   HOOKEXIT( szSpl32StdQueryLength, &Parm1, &lRC );
   T_FreeLog( );

   return lRC;

}  /* Trc_Spl32StdQueryLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16StdStart()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 SPLSTDSTART( HDC );
PCHAR szSpl16StdStart = "Spl16StdStart";

BOOL16 APIENTRY16 Trc_Spl16StdStart( HDC Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16StdStart );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16StdStart, &Parm1 );
   T_FreeLog( );

   bRC = SPLSTDSTART( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16StdStart );
   T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
   HOOKEXIT16( szSpl16StdStart, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl16StdStart() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32StdStart()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY SPL32STDSTART( HDC );
PCHAR szSpl32StdStart = "Spl32StdStart";

BOOL APIENTRY Trc_Spl32StdStart( HDC Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32StdStart );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32StdStart, &Parm1 );
   T_FreeLog( );

   bRC = SPL32STDSTART( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32StdStart );
   T_LogPMReturnCode( DT_BOOL, bRC, bRC );
   HOOKEXIT( szSpl32StdStart, &Parm1, &bRC );
   T_FreeLog( );

   return bRC;

}  /* Trc_Spl32StdStart() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl16StdStop()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HSTD APIENTRY16 SPLSTDSTOP( HDC );
PCHAR szSpl16StdStop = "Spl16StdStop";

HSTD APIENTRY16 Trc_Spl16StdStop( HDC Parm1 )
{

   HSTD ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry16( szSpl16StdStop );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY16( szSpl16StdStop, &Parm1 );
   T_FreeLog( );

   ulRC = SPLSTDSTOP( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl16StdStop );
   T_LogPMReturnCode( DT_HSTD, ulRC, ulRC != SPL_ERROR );
   HOOKEXIT16( szSpl16StdStop, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl16StdStop() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32StdStop()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HSTD APIENTRY SPL32STDSTOP( HDC );
PCHAR szSpl32StdStop = "Spl32StdStop";

HSTD APIENTRY Trc_Spl32StdStop( HDC Parm1 )
{

   HSTD ulRC;                                    /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32StdStop );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_HDC );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szSpl32StdStop, &Parm1 );
   T_FreeLog( );

   ulRC = SPL32STDSTOP( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32StdStop );
   T_LogPMReturnCode( DT_HSTD, ulRC, ulRC != SPL_ERROR );
   HOOKEXIT( szSpl32StdStop, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32StdStop() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Spl32UnRegister()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
ULONG APIENTRY SPLUNREGISTER( PSZ, PSZ, ULONG, ULONG, ULONG );
PCHAR szSpl32UnRegister = "Spl32UnRegister";

ULONG APIENTRY Trc_Spl32UnRegister( PSZ   Parm1
                                   , PSZ   Parm2
                                   , ULONG Parm3
                                   , ULONG Parm4
                                   , ULONG Parm5
                                   )
{

   ULONG ulRC;                                   /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szSpl32UnRegister );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PSZ );
      T_LogPSZ( Parm1 );
      T_LogParameter( 2L, DT_PSZ );
      T_LogPSZ( Parm2 );
      T_LogParameter( 3L, DT_ULONG );
      T_LogULONG( Parm3 );
      T_LogParameter( 4L, DT_ULONG );
      T_LogULONG( Parm4 );
      T_LogParameter( 5L, DT_ULONG );
      T_LogULONG( Parm5 );
   }
   HOOKENTRY( szSpl32UnRegister, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnSPL32UNREGISTER )( Parm1, Parm2, Parm3, Parm4, Parm5 );

   T_WaitLog( );
   T_LogAPIExit( szSpl32UnRegister );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szSpl32UnRegister, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Spl32UnRegister() */
#endif

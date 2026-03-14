/******************************************************************************/
/*                                                                            */
/* File name    : T_N.C                                                       */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit NLS routines           */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from NLS.DLL.        */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.00 12Jun95 Created (supported LX format and 32-bit Dos */
/*                                APIs only)                                  */
/*                2.30.22 19Dec95 Added new OS/2 3.00 non-Uni Dos APIs        */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.21 20May97 Fixed/minimized 16-bit stack usage          */
/*                2.40.25 04Jun97 Fixed 16-bit registers structure init bug   */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.50 06Feb98 Fixed CX and DX  corruption by  16-bit Kbd, */
/*                                Mou, and Vio APIs                           */
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

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
#define ORD_DOS32QUERYCC      11                 /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32SETPROCESSCC 12                 /* OS/2 3.00 proc ordinal    */
#define ORD_DOS32SETPROCESSCP 10                 /* OS/2 3.00 proc ordinal    */
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
PCHAR   szDLL = "NLS";                           /* DLL module name           */
PCHAR   szTraceDLL = "T_N";                      /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
PFN     pfnDOS32QUERYCC;                         /* OS/2 3.00 procedure       */
PFN     pfnDOS32SETPROCESSCC;                    /* OS/2 3.00 procedure       */
PFN     pfnDOS32SETPROCESSCP;                    /* OS/2 3.00 procedure       */
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
    * Library initialization required if process attaching to T_N
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
       * Log error and terminate if T_N version does not match T_COMMON version
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
       * Load NLS library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_N library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register NLS trace DLL
       */
      T_RegisterDLL( hmodDLL, hmodTraceDLL );

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
      /*
       * Obtain OS/2 3.00 procedure addresses if OS/2 version number greater
       * than or equal to 3.00
       */
      if ( T_OS2Ver >= MAKEVER( 3, 0 ) )
      {
         /*
          * Obtain OS/2 3.00 procedure addresses, log error and terminate if
          * unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DOS32QUERYCC     , NULL, &pfnDOS32QUERYCC      ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DOS32SETPROCESSCC, NULL, &pfnDOS32SETPROCESSCC ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_DOS32SETPROCESSCP, NULL, &pfnDOS32SETPROCESSCP ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 3, 0, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_N
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload NLS library if library loaded
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
/* Trc_Dos16CaseMap()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSCASEMAP( USHORT, PCOUNTRYCODE16, PCHAR16 );
#define DOSCASEMAP_PARMBYTES ( sizeof( USHORT ) + sizeof( PCOUNTRYCODE16 ) + sizeof( PCHAR16 ) )
PCHAR szDos16CaseMap = "Dos16CaseMap";

APIRET16 APIENTRY16 Trc_Dos16CaseMap( USHORT         Parm1
                                    , PCOUNTRYCODE16 Parm2
                                    , PCHAR16        Parm3
                                    )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSCASEMAP_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16CaseMap, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PCOUNTRYCODE );
      T_LogPCOUNTRYCODE16( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
      T_LogCHARBuffer16( Parm3, Parm1 );
   }
   HOOKENTRY16( szDos16CaseMap, &Parm1 );
   T_FreeLog( );

   usRC = DOSCASEMAP( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16CaseMap );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
      if ( !usRC ) T_LogCHARBuffer16( Parm3, Parm1 );
   }
   HOOKEXIT16( szDos16CaseMap, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16CaseMap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetCollate()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETCOLLATE( USHORT, PCOUNTRYCODE16, PCHAR16, PUSHORT16 );
#define DOSGETCOLLATE_PARMBYTES ( sizeof( USHORT ) + sizeof( PCOUNTRYCODE16 ) + sizeof( PCHAR16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16GetCollate = "Dos16GetCollate";

APIRET16 APIENTRY16 Trc_Dos16GetCollate( USHORT         Parm1
                                       , PCOUNTRYCODE16 Parm2
                                       , PCHAR16        Parm3
                                       , PUSHORT16      Parm4
                                       )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETCOLLATE_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16GetCollate, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PCOUNTRYCODE );
      T_LogPCOUNTRYCODE16( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPVOID16( Parm4 );
   }
   HOOKENTRY16( szDos16GetCollate, &Parm1 );
   T_FreeLog( );

   usRC = DOSGETCOLLATE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos16GetCollate );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
      if ( ( Parm4 ) && ( ( !usRC ) || ( usRC == ERROR_NLS_TABLE_TRUNCATED ) ) ) T_LogCHARBuffer16( Parm3, *Parm4 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
   }
   HOOKEXIT16( szDos16GetCollate, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetCollate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetCtryInfo()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETCTRYINFO( USHORT, PCOUNTRYCODE16, PCOUNTRYINFO16, PUSHORT16 );
#define DOSGETCTRYINFO_PARMBYTES ( sizeof( USHORT ) + sizeof( PCOUNTRYCODE16 ) + sizeof( PCOUNTRYINFO16 ) + sizeof( PUSHORT16 ) )
PCHAR szDos16GetCtryInfo = "Dos16GetCtryInfo";

APIRET16 APIENTRY16 Trc_Dos16GetCtryInfo( USHORT         Parm1
                                        , PCOUNTRYCODE16 Parm2
                                        , PCOUNTRYINFO16 Parm3
                                        , PUSHORT16      Parm4
                                        )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETCTRYINFO_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16GetCtryInfo, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PCOUNTRYCODE );
      T_LogPCOUNTRYCODE16( Parm2 );
      T_LogParameter( 3L, DT_PCOUNTRYINFO );
      T_LogPVOID16( Parm3 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPVOID16( Parm4 );
   }
   HOOKENTRY16( szDos16GetCtryInfo, &Parm1 );
   T_FreeLog( );

   usRC = DOSGETCTRYINFO( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos16GetCtryInfo );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCOUNTRYINFO );
      T_LogPVOID16( Parm3 );
      if ( ( Parm4 ) && ( ( !usRC ) || ( usRC == ERROR_NLS_TABLE_TRUNCATED ) ) ) T_LogPCOUNTRYINFO16( Parm3, *Parm4 );
      T_LogParameter( 4L, DT_PUSHORT );
      T_LogPUSHORT16( Parm4 );
   }
   HOOKEXIT16( szDos16GetCtryInfo, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetCtryInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos16GetDBCSEv()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 DOSGETDBCSEV( USHORT, PCOUNTRYCODE16, PCHAR16 );
#define DOSGETDBCSEV_PARMBYTES ( sizeof( USHORT ) + sizeof( PCOUNTRYCODE16 ) + sizeof( PCHAR16 ) )
PCHAR szDos16GetDBCSEv = "Dos16GetDBCSEv";

APIRET16 APIENTRY16 Trc_Dos16GetDBCSEv( USHORT         Parm1
                                      , PCOUNTRYCODE16 Parm2
                                      , PCHAR16        Parm3
                                      )
{

   REGS16   sRegs16;                             /* 16-bit registers          */
   APIRET16 usRC;                                /* API return code           */

   sRegs16.usParmBytes = DOSGETDBCSEV_PARMBYTES;
   T_SaveRegs16( &sRegs16 );

   T_WaitLog( );
   T_LogBaseAPIEntry16( szDos16GetDBCSEv, sRegs16.ulCSIP );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_USHORT );
      T_LogUSHORT( Parm1 );
      T_LogParameter( 2L, DT_PCOUNTRYCODE );
      T_LogPCOUNTRYCODE16( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
   }
   HOOKENTRY16( szDos16GetDBCSEv, &Parm1 );
   T_FreeLog( );

   usRC = DOSGETDBCSEV( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos16GetDBCSEv );
   T_LogCPReturnCode( usRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID16( Parm3 );
      if ( ( !usRC ) || ( usRC == ERROR_NLS_TABLE_TRUNCATED ) ) T_LogCHARBuffer16( Parm3, Parm1 );
   }
   HOOKEXIT16( szDos16GetDBCSEv, &Parm1, &usRC );
   T_FreeLog( );

   T_RestoreRegs16( &sRegs16 );

   return usRC;

}  /* Trc_Dos16GetDBCSEv() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32MapCase()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32MAPCASE( ULONG, PCOUNTRYCODE, PCHAR );
PCHAR szDos32MapCase = "Dos32MapCase";

APIRET APIENTRY Trc_Dos32MapCase( ULONG        Parm1
                                , PCOUNTRYCODE Parm2
                                , PCHAR        Parm3
                                )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32MapCase );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PCOUNTRYCODE );
      T_LogPCOUNTRYCODE( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
      T_LogCHARBuffer( Parm3, Parm1 );
   }
   HOOKENTRY( szDos32MapCase, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32MAPCASE( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos32MapCase );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
      if ( !ulRC ) T_LogCHARBuffer( Parm3, Parm1 );
   }
   HOOKEXIT( szDos32MapCase, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32MapCase() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryCc()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
PCHAR szDos32QueryCc = "Dos32QueryCc";

APIRET APIENTRY Trc_Dos32QueryCc( PULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32QueryCc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PULONG );
      T_LogPVOID( Parm1 );
   }
   HOOKENTRY( szDos32QueryCc, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnDOS32QUERYCC )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos32QueryCc );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_PULONG );
      T_LogPULONG( Parm1 );
   }
   HOOKEXIT( szDos32QueryCc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32QueryCc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryCollate()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYCOLLATE( ULONG, PCOUNTRYCODE, PCHAR, PULONG );
PCHAR szDos32QueryCollate = "Dos32QueryCollate";

APIRET APIENTRY Trc_Dos32QueryCollate( ULONG        Parm1
                                     , PCOUNTRYCODE Parm2
                                     , PCHAR        Parm3
                                     , PULONG       Parm4
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32QueryCollate );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PCOUNTRYCODE );
      T_LogPCOUNTRYCODE( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPVOID( Parm4 );
   }
   HOOKENTRY( szDos32QueryCollate, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32QUERYCOLLATE( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos32QueryCollate );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
      if ( ( Parm4 ) && ( ( !ulRC ) || ( ulRC == ERROR_NLS_TABLE_TRUNCATED ) ) ) T_LogCHARBuffer( Parm3, *Parm4 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPULONG( Parm4 );
   }
   HOOKEXIT( szDos32QueryCollate, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32QueryCollate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryCp()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 200 ) )
APIRET APIENTRY DOS32QUERYCP( ULONG, PULONG, PULONG );
PCHAR szDos32QueryCp = "Dos32QueryCp";

APIRET APIENTRY Trc_Dos32QueryCp( ULONG  Parm1
                                , PULONG Parm2
                                , PULONG Parm3
                                )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32QueryCp );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PULONG );
      T_LogPVOID( Parm2 );
      T_LogParameter( 3L, DT_PULONG );
      T_LogPVOID( Parm3 );
   }
   HOOKENTRY( szDos32QueryCp, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32QUERYCP( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos32QueryCp );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 2L, DT_PULONG );
      T_LogPVOID( Parm2 );
      if ( ( Parm3 ) && ( ( !ulRC ) || ( ulRC == ERROR_CPLIST_TOO_SMALL ) ) ) T_LogULONGBuffer( Parm2, *Parm3 / sizeof( ULONG ) );
      T_LogParameter( 3L, DT_PULONG );
      T_LogPULONG( Parm3 );
   }
   HOOKEXIT( szDos32QueryCp, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32QueryCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryCtryInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYCTRYINFO( ULONG, PCOUNTRYCODE, PCOUNTRYINFO, PULONG );
PCHAR szDos32QueryCtryInfo = "Dos32QueryCtryInfo";

APIRET APIENTRY Trc_Dos32QueryCtryInfo( ULONG        Parm1
                                      , PCOUNTRYCODE Parm2
                                      , PCOUNTRYINFO Parm3
                                      , PULONG       Parm4
                                      )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32QueryCtryInfo );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PCOUNTRYCODE );
      T_LogPCOUNTRYCODE( Parm2 );
      T_LogParameter( 3L, DT_PCOUNTRYINFO );
      T_LogPVOID( Parm3 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPVOID( Parm4 );
   }
   HOOKENTRY( szDos32QueryCtryInfo, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32QUERYCTRYINFO( Parm1, Parm2, Parm3, Parm4 );

   T_WaitLog( );
   T_LogAPIExit( szDos32QueryCtryInfo );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCOUNTRYINFO );
      T_LogPVOID( Parm3 );
      if ( ( Parm4 ) && ( ( !ulRC ) || ( ulRC == ERROR_NLS_TABLE_TRUNCATED ) ) ) T_LogPCOUNTRYINFO( Parm3, *Parm4 );
      T_LogParameter( 4L, DT_PULONG );
      T_LogPULONG( Parm4 );
   }
   HOOKEXIT( szDos32QueryCtryInfo, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32QueryCtryInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32QueryDBCSEnv()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY DOS32QUERYDBCSENV( ULONG, PCOUNTRYCODE, PCHAR );
PCHAR szDos32QueryDBCSEnv = "Dos32QueryDBCSEnv";

APIRET APIENTRY Trc_Dos32QueryDBCSEnv( ULONG        Parm1
                                     , PCOUNTRYCODE Parm2
                                     , PCHAR        Parm3
                                     )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32QueryDBCSEnv );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
      T_LogParameter( 2L, DT_PCOUNTRYCODE );
      T_LogPCOUNTRYCODE( Parm2 );
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
   }
   HOOKENTRY( szDos32QueryDBCSEnv, &Parm1 );
   T_FreeLog( );

   ulRC = DOS32QUERYDBCSENV( Parm1, Parm2, Parm3 );

   T_WaitLog( );
   T_LogAPIExit( szDos32QueryDBCSEnv );
   T_LogCPReturnCode( ulRC );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 3L, DT_PCHAR );
      T_LogPVOID( Parm3 );
      if ( ( !ulRC ) || ( ulRC == ERROR_NLS_TABLE_TRUNCATED ) ) T_LogCHARBuffer( Parm3, Parm1 );
   }
   HOOKEXIT( szDos32QueryDBCSEnv, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32QueryDBCSEnv() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetProcessCc()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
PCHAR szDos32SetProcessCc = "Dos32SetProcessCc";

APIRET APIENTRY Trc_Dos32SetProcessCc( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32SetProcessCc );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDos32SetProcessCc, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnDOS32SETPROCESSCC )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos32SetProcessCc );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szDos32SetProcessCc, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32SetProcessCc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Dos32SetProcessCp()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( POWERPC ) ) && ( OS2VER >= 300 ) )
PCHAR szDos32SetProcessCp = "Dos32SetProcessCp";

APIRET APIENTRY Trc_Dos32SetProcessCp( ULONG Parm1 )
{

   APIRET ulRC;                                  /* API return code           */

   T_WaitLog( );
   T_LogAPIEntry( szDos32SetProcessCp );
   if ( LOGLEVEL( L_PARMS ) )
   {
      T_LogParameter( 1L, DT_ULONG );
      T_LogULONG( Parm1 );
   }
   HOOKENTRY( szDos32SetProcessCp, &Parm1 );
   T_FreeLog( );

   ulRC = ( *pfnDOS32SETPROCESSCP )( Parm1 );

   T_WaitLog( );
   T_LogAPIExit( szDos32SetProcessCp );
   T_LogCPReturnCode( ulRC );
   HOOKEXIT( szDos32SetProcessCp, &Parm1, &ulRC );
   T_FreeLog( );

   return ulRC;

}  /* Trc_Dos32SetProcessCp() */
#endif

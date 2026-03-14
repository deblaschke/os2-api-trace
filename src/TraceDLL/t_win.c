/******************************************************************************/
/*                                                                            */
/* File name    : T_WIN.C                                                     */
/*                                                                            */
/* Title        : OS/2 API trace DLL 16-bit and 32-bit PMWIN routines         */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Traces 16-bit and 32-bit APIs imported from PMWIN.DLL.      */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.03 20Jul95 Added 32-bit Win APIs                       */
/*                2.30.04 28Jul95 Fixed integer buffer logging                */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.30.23 27Mar96 Fixed PMWIN bugs                            */
/*                2.30.30 26Jun94 Added OS/2 2.00 PM debugger APIs            */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
/*                2.40.27 22Jul97 Added 16-bit Gpi APIs                       */
/*                2.40.31 08Aug97 Logged user-defined buffer initial bytes    */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.44 27Jan98 Fixed returned MRESULT thunk bug            */
/*                2.40.45 28Jan98 Added indeterminate API return codes        */
/*                2.40.48 30Jan98 Fixed Win16Catch bug                        */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs     */
/*                2.40.74 27Oct98 Added WinSetErrorInfo                       */
/*                2.40.76 28Oct98 Issued more specific trace DLL load errors  */
/*                2.40.77 28Oct98 Added Win32MessageBox2                      */
/*                2.45.02 02May99 Fixed WinQueryWindowText bug                */
/*                2.45.10 02Aug99 Moved common strings into T_COMMON          */
/*                2.45.16 18Feb00 Added -C, -PAUSE, and -RESUME options       */
/*                2.45.31 26Jun01 Added  capability of building  32-bit  only */
/*                                trace DLLs                                  */
/*                2.45.32 28Jun01 Added -U option                             */
/*                2.45.34 10Jul01 Moved C run-time functions into T_COMMON    */
/*                2.45.36 03Dec03 Fixed WinSetErrorInfo user hook bug         */
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
#include <string.h>                              /* C information             */
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
#define ORD_WIN16CHECKINPUT             640      /* OS/2 2.10 proc ordinal    */
#define ORD_WIN16LOCKPOINTERUPDATE      639      /* OS/2 2.10 proc ordinal    */
#endif
#define ORD_WIN32CHECKINPUT             998      /* OS/2 2.10 proc ordinal    */
#define ORD_WIN32LOCKPOINTERUPDATE      997      /* OS/2 2.10 proc ordinal    */
#define ORD_WIN32QUERYSYSPOINTERDATA    1003     /* OS/2 2.10 proc ordinal    */
#define ORD_WIN32QUERYVISIBLEREGION     1000     /* OS/2 2.10 proc ordinal    */
#define ORD_WIN32SETSYSPOINTERDATA      1002     /* OS/2 2.10 proc ordinal    */
#define ORD_WIN32SETVISIBLEREGIONNOTIFY 1001     /* OS/2 2.10 proc ordinal    */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
#define ORD_WIN32MESSAGEBOX2            1015     /* OS/2 2.30 proc ordinal    */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
#define ORD_WIN32QUERYCLIPREGION        1044     /* OS/2 2.40 proc ordinal    */
#define ORD_WIN32SETCLIPREGION          1043     /* OS/2 2.40 proc ordinal    */
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
PCHAR   szDLL = "PMWIN";                         /* DLL module name           */
PCHAR   szEmpty = "";                            /* Empty string              */
PCHAR   szTraceDLL = "T_WIN";                    /* Trace DLL module name     */
ULONG   ulOS2TRACEVer =                          /* OS2TRACE version number   */
                        ( VER_MAJOR << 16 ) +
                        ( VER_MINOR <<  8 ) +
                        ( VER_REV         ) ;

#if ( OS2VER >= 210 )
#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
BOOL16 (* _Seg16 APIENTRY16 pfnWINCHECKINPUT)( HAB );
                                                 /* OS/2 2.10 procedure       */
BOOL16 (* _Seg16 APIENTRY16 pfnWINLOCKPOINTERUPDATE)( HWND, HPOINTER, USHORT );
                                                 /* OS/2 2.10 procedure       */
#endif
BOOL (* APIENTRY            pfnWIN32CHECKINPUT)( HAB );
                                                 /* OS/2 2.10 procedure       */
BOOL (* APIENTRY            pfnWIN32LOCKPOINTERUPDATE)( HWND, HPOINTER, ULONG );
                                                 /* OS/2 2.10 procedure       */
BOOL (* APIENTRY            pfnWIN32QUERYSYSPOINTERDATA)( HWND, ULONG, PICONINFO );
                                                 /* OS/2 2.10 procedure       */
ULONG (* APIENTRY           pfnWIN32QUERYVISIBLEREGION)( HWND, HRGN );
                                                 /* OS/2 2.10 procedure       */
BOOL (* APIENTRY            pfnWIN32SETSYSPOINTERDATA)( HWND, ULONG, PICONINFO );
                                                 /* OS/2 2.10 procedure       */
BOOL (* APIENTRY            pfnWIN32SETVISIBLEREGIONNOTIFY)( HWND, BOOL );
                                                 /* OS/2 2.10 procedure       */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG (* APIENTRY           pfnWIN32MESSAGEBOX2)( HWND, HWND, PSZ, PSZ, ULONG, PMB2INFO );
                                                 /* OS/2 2.30 procedure       */
#endif
#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG (* APIENTRY            pfnWIN32QUERYCLIPREGION)( HWND, HRGN );
                                                 /* OS/2 2.40 procedure       */
BOOL (* APIENTRY            pfnWIN32SETCLIPREGION)( HWND, HRGN );
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
    * Library initialization required if process attaching to T_WIN
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
       * Log error and terminate if T_WIN version does not match T_COMMON
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
       * Load PMWIN library, log error and terminate if unsuccessful
       */
      if ( DosLoadModule( szObjName, sizeof( szObjName ), szDLL, &hmodDLL ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Load T_WIN library, log error and terminate if unsuccessful
       */
      if ( ( DosLoadModule( szObjName, sizeof( szObjName ), szTraceDLL, &hmodTraceDLL ) ) ||
           ( hmodTraceDLL != ulModule                                                   ) )
      {
         T_Error( TRUE, EM_LOADDLL, szDLL );
      }

      /*
       * Register PMWIN trace DLL
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
          * Obtain 16-bit OS/2 2.10 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16CHECKINPUT        ), (PPFN16)&pfnWINCHECKINPUT        ) ) ||
              ( DOS16GETPROCADDR( hmodDLL, (PSZ16)( ulOrdinal = ORD_WIN16LOCKPOINTERUPDATE ), (PPFN16)&pfnWINLOCKPOINTERUPDATE ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 16, 2, 10, szDLL, ulOrdinal );
         }
#endif

         /*
          * Obtain 32-bit OS/2 2.10 procedure addresses, log error and
          * terminate if unsuccessful
          */
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32CHECKINPUT            , NULL, (PFN *)&pfnWIN32CHECKINPUT             ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32LOCKPOINTERUPDATE     , NULL, (PFN *)&pfnWIN32LOCKPOINTERUPDATE      ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32QUERYSYSPOINTERDATA   , NULL, (PFN *)&pfnWIN32QUERYSYSPOINTERDATA    ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32QUERYVISIBLEREGION    , NULL, (PFN *)&pfnWIN32QUERYVISIBLEREGION     ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32SETSYSPOINTERDATA     , NULL, (PFN *)&pfnWIN32SETSYSPOINTERDATA      ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32SETVISIBLEREGIONNOTIFY, NULL, (PFN *)&pfnWIN32SETVISIBLEREGIONNOTIFY ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 10, szDLL, ulOrdinal );
         }
      }
#endif

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
      /*
       * Obtain OS/2 2.30 procedure address if OS/2 version number greater than
       * or equal to 2.30
       */
      if ( T_OS2Ver >= MAKEVER( 2, 30 ) )
      {
         /*
          * Obtain OS/2 2.30 procedure address, log error and terminate if
          * unsuccessful
          */
         if ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32MESSAGEBOX2, NULL, (PFN *)&pfnWIN32MESSAGEBOX2 ) )
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
         if ( ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32QUERYCLIPREGION, NULL, (PFN *)&pfnWIN32QUERYCLIPREGION ) ) ||
              ( DosQueryProcAddr( hmodDLL, ulOrdinal = ORD_WIN32SETCLIPREGION  , NULL, (PFN *)&pfnWIN32SETCLIPREGION   ) ) )
         {
            T_Error( TRUE, EM_GETPROC, 32, 2, 40, szDLL, ulOrdinal );
         }
      }
#endif
   }

   /*
    * Library termination required if process detaching from T_WIN
    */
   else if ( ulFlag == 1L )
   {
      /*
       * Unload PMWIN library if library loaded
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
/* C_PostWin16SetErrorInfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PCHAR szWin16SetErrorInfo = "Win16SetErrorInfo";

VOID APIENTRY16 C_PostWin16SetErrorInfo( ERRORID   *pulRC
                                       , PUSHORT16  Parms
                                       )
{

   if ( LOGWINAPI( W_SEI ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetErrorInfo );
      T_LogPMReturnCodeIndeterminate( DT_ERRORID, *pulRC );
      HOOKEXIT16( szWin16SetErrorInfo, Parms, pulRC );
      T_FreeLog( );
   }

}  /* C_PostWin16SetErrorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* C_PostWin32SetErrorInfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
PCHAR szWin32SetErrorInfo = "Win32SetErrorInfo";

VOID APIENTRY C_PostWin32SetErrorInfo( ERRORID *pulRC
                                     , PULONG   Parms
                                     )
{

   if ( LOGWINAPI( W_SEI ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetErrorInfo );
      T_LogPMReturnCodeIndeterminate( DT_ERRORID, *pulRC );
      HOOKEXIT( szWin32SetErrorInfo, Parms, pulRC );
      T_FreeLog( );
   }

}  /* C_PostWin32SetErrorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* C_PreWin16SetErrorInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 C_PreWin16SetErrorInfo( PVOID16   APICaller
                                        , ERRORID   Parm1
                                        , USHORT    Parm2
                                        , PUSHORT16 VarArgs
                                        , PUSHORT16 Parms
                                        )
{

   int    i;                                     /* Loop variable             */
   USHORT cParmUshorts = 0;                      /* Number USHORT arguments   */
   USHORT usParm = 1;                            /* Current parameter number  */

   if ( ( LOGWINAPI( W_SEI ) ) && ( LOGLEVEL( L_ENTEXT ) ) )
   {
      T_WaitLog( );
      T_LogAPIEntryWithCaller16( szWin16SetErrorInfo, APICaller );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( usParm++, DT_ERRORID );
         T_LogULONG( Parm1 );
         T_LogParameter( usParm++, DT_USHORT );
         T_LogUSHORT( Parm2 );
         if ( Parm2 & 0x0004 /* SEI_ARGCOUNT */ )
         {
            T_LogParameter( usParm++, DT_cParmUshorts );
            T_LogUSHORT( cParmUshorts = *VarArgs++ );
         }
         if ( Parm2 & 0x0008 /* SEI_DOSERROR */ )
         {
            T_LogParameter( usParm++, DT_dosErrCode );
            T_LogUSHORT( *VarArgs++ );
         }
         if ( Parm2 & 0x0010 /* SEI_MSGSTR */ )
         {
            T_LogParameter( usParm++, DT_pszErrorMsg );
            T_LogPSZ16( (PSZ16)*((PULONG16)VarArgs) );
            VarArgs += 2;
         }
         if ( cParmUshorts )
         {
            for ( i = 0; i < cParmUshorts; i++ )
            {
               T_LogParameter( usParm++, DT_pArgs );
               T_LogUSHORT( *VarArgs++ );
            }
         }
      }
      HOOKENTRY16( szWin16SetErrorInfo, Parms );
      T_FreeLog( );
   }
   else
   {
      usParm += 2;
      if ( Parm2 & 0x0004 /* SEI_ARGCOUNT */ )
      {
         usParm++;
         usParm += *VarArgs;
      }
      if ( Parm2 & 0x0008 /* SEI_DOSERROR */ )
      {
         usParm++;
      }
      if ( Parm2 & 0x0010 /* SEI_MSGSTR */ )
      {
         usParm++;
      }
   }

   return ( usParm + ( Parm2 & 0x0010 ? 1 : 0 ) ) * sizeof( USHORT );

}  /* C_PreWin16SetErrorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* C_PreWin32SetErrorInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY C_PreWin32SetErrorInfo( ULONG   APICaller
                                     , ERRORID Parm1
                                     , ULONG   Parm2
                                     , PULONG  VarArgs
                                     , PULONG  Parms
                                     )
{

   int   i;                                      /* Loop variable             */
   ULONG cParmUlongs = 0L;                       /* Number ULONG arguments    */
   ULONG ulParm = 1L;                            /* Current parameter number  */

   if ( ( LOGWINAPI( W_SEI ) ) && ( LOGLEVEL( L_ENTEXT ) ) )
   {
      T_WaitLog( );
      T_LogAPIEntryWithCaller( szWin32SetErrorInfo, APICaller );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( ulParm++, DT_ERRORID );
         T_LogULONG( Parm1 );
         T_LogParameter( ulParm++, DT_ULONG );
         T_LogULONG( Parm2 );
         if ( Parm2 & 0x0004 /* SEI_ARGCOUNT */ )
         {
            T_LogParameter( ulParm++, DT_cParmUlongs );
            T_LogULONG( cParmUlongs = *VarArgs++ );
         }
         if ( Parm2 & 0x0008 /* SEI_DOSERROR */ )
         {
            T_LogParameter( ulParm++, DT_dosErrCode );
            T_LogULONG( *VarArgs++ );
         }
         if ( Parm2 & 0x0010 /* SEI_MSGSTR */ )
         {
            T_LogParameter( ulParm++, DT_pszErrorMsg );
            T_LogPSZ( (PSZ)*VarArgs++ );
         }
         if ( cParmUlongs )
         {
            for ( i = 0; i < cParmUlongs; i++ )
            {
               T_LogParameter( ulParm++, DT_pArgs );
               T_LogULONG( *VarArgs++ );
            }
         }
      }
      HOOKENTRY( szWin32SetErrorInfo, Parms );
      T_FreeLog( );
   }
   else
   {
      ulParm += 2;
      if ( Parm2 & 0x0004 /* SEI_ARGCOUNT */ )
      {
         ulParm++;
         ulParm += *VarArgs;
      }
      if ( Parm2 & 0x0008 /* SEI_DOSERROR */ )
      {
         ulParm++;
      }
      if ( Parm2 & 0x0010 /* SEI_MSGSTR */ )
      {
         ulParm++;
      }
   }

   return ( ulParm - 1 ) * sizeof( ULONG );

}  /* C_PreWin32SetErrorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* C_Win16Catch()                                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINCATCH( PCATCHBUF16 );
PCHAR szWin16Catch = "Win16Catch";

SHORT APIENTRY16 C_Win16Catch( PCATCHBUF16 Parm1
                             , PCATCHBUF16 CatchBuf
                             )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_ENV ) )
   {
      T_WaitLog( );
      T_LogAPIEntryWithCaller16( szWin16Catch, (PVOID16)CatchBuf->reserved[ 0 ] );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCATCHBUF );
         T_LogPVOID16( Parm1 );
      }
      HOOKENTRY16( szWin16Catch, &Parm1 );
      T_FreeLog( );
   }

   if ( !( sRC = WINCATCH( Parm1 ) ) ) memcpy( (PBYTE)Parm1, (PBYTE)CatchBuf, sizeof( CATCHBUF16 ) );

   if ( LOGWINAPI( W_ENV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16Catch );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, !sRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCATCHBUF );
         if ( !sRC ) T_LogPCATCHBUF( Parm1 );
         else T_LogPVOID16( Parm1 );
      }
      HOOKEXIT16( szWin16Catch, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* C_Win16Catch() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Gpi16LoadBitmap()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HBITMAP APIENTRY16 GPILOADBITMAP( HPS, HMODULE16, USHORT, LONG, LONG );
PCHAR szGpi16LoadBitmap = "Gpi16LoadBitmap";

HBITMAP APIENTRY16 Trc_Gpi16LoadBitmap( HPS       Parm1
                                      , HMODULE16 Parm2
                                      , USHORT    Parm3
                                      , LONG      Parm4
                                      , LONG      Parm5
                                      )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szGpi16LoadBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY16( szGpi16LoadBitmap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = GPILOADBITMAP( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGGPIAPI( G_BIT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szGpi16LoadBitmap );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC != GPI_ERROR );
      HOOKEXIT16( szGpi16LoadBitmap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Gpi16LoadBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16AddAtom()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ATOM16 APIENTRY16 WINADDATOM( HATOMTBL, PSZ16 );
PCHAR szWin16AddAtom = "Win16AddAtom";

ATOM16 APIENTRY16 Trc_Win16AddAtom( HATOMTBL Parm1
                                  , PSZ16    Parm2
                                  )
{

   ATOM16 usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16AddAtom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szWin16AddAtom, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINADDATOM( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16AddAtom );
      T_LogPMReturnCodeUSHORT( DT_ATOM, usRC, usRC );
      HOOKEXIT16( szWin16AddAtom, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16AddAtom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32AddAtom()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ATOM APIENTRY WIN32ADDATOM( HATOMTBL, PSZ );
PCHAR szWin32AddAtom = "Win32AddAtom";

ATOM APIENTRY Trc_Win32AddAtom( HATOMTBL Parm1
                              , PSZ      Parm2
                              )
{

   ATOM ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32AddAtom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szWin32AddAtom, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32ADDATOM( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32AddAtom );
      T_LogPMReturnCode( DT_ATOM, ulRC, ulRC );
      HOOKEXIT( szWin32AddAtom, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32AddAtom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16Alarm()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINALARM( HWND, USHORT );
PCHAR szWin16Alarm = "Win16Alarm";

BOOL16 APIENTRY16 Trc_Win16Alarm( HWND   Parm1
                                , USHORT Parm2
                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16Alarm );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16Alarm, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINALARM( Parm1, Parm2 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16Alarm );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16Alarm, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16Alarm() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32Alarm()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ALARM( HWND, ULONG );
PCHAR szWin32Alarm = "Win32Alarm";

BOOL APIENTRY Trc_Win32Alarm( HWND  Parm1
                            , ULONG Parm2
                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32Alarm );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32Alarm, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ALARM( Parm1, Parm2 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32Alarm );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32Alarm, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32Alarm() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16AllocMem()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
NPBYTE16 APIENTRY16 WINALLOCMEM( HHEAP16, USHORT );
PCHAR szWin16AllocMem = "Win16AllocMem";

NPBYTE16 APIENTRY16 Trc_Win16AllocMem( HHEAP16 Parm1
                                     , USHORT  Parm2
                                     )
{

   NPBYTE16 usRC;                                /* API return code           */

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16AllocMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HHEAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16AllocMem, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINALLOCMEM( Parm1, Parm2 );

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16AllocMem );
      T_LogPMReturnCodeUSHORT( DT_NPBYTE, usRC, usRC );
      HOOKEXIT16( szWin16AllocMem, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16AllocMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16AvailMem()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINAVAILMEM( HHEAP16, BOOL16, USHORT );
PCHAR szWin16AvailMem = "Win16AvailMem";

USHORT APIENTRY16 Trc_Win16AvailMem( HHEAP16 Parm1
                                   , BOOL16  Parm2
                                   , USHORT  Parm3
                                   )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16AvailMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HHEAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16AvailMem, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINAVAILMEM( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16AvailMem );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC != 0xFFFF );
      HOOKEXIT16( szWin16AvailMem, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16AvailMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16BeginEnumWindows()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HENUM APIENTRY16 WINBEGINENUMWINDOWS( HWND );
PCHAR szWin16BeginEnumWindows = "Win16BeginEnumWindows";

HENUM APIENTRY16 Trc_Win16BeginEnumWindows( HWND Parm1 )
{

   HENUM ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16BeginEnumWindows );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16BeginEnumWindows, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINBEGINENUMWINDOWS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16BeginEnumWindows );
      T_LogPMReturnCode( DT_HENUM, ulRC, ulRC );
      HOOKEXIT16( szWin16BeginEnumWindows, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16BeginEnumWindows() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32BeginEnumWindows()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HENUM APIENTRY WIN32BEGINENUMWINDOWS( HWND );
PCHAR szWin32BeginEnumWindows = "Win32BeginEnumWindows";

HENUM APIENTRY Trc_Win32BeginEnumWindows( HWND Parm1 )
{

   HENUM ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32BeginEnumWindows );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32BeginEnumWindows, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32BEGINENUMWINDOWS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32BeginEnumWindows );
      T_LogPMReturnCode( DT_HENUM, ulRC, ulRC );
      HOOKEXIT( szWin32BeginEnumWindows, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32BeginEnumWindows() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16BeginPaint()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPS APIENTRY16 WINBEGINPAINT( HWND, HPS, PRECTL16 );
PCHAR szWin16BeginPaint = "Win16BeginPaint";

HPS APIENTRY16 Trc_Win16BeginPaint( HWND     Parm1
                                  , HPS      Parm2
                                  , PRECTL16 Parm3
                                  )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16BeginPaint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szWin16BeginPaint, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINBEGINPAINT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16BeginPaint );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PRECTL );
         if ( ulRC ) T_LogPRECTL16( Parm3 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szWin16BeginPaint, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16BeginPaint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32BeginPaint()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPS APIENTRY WIN32BEGINPAINT( HWND, HPS, PRECTL );
PCHAR szWin32BeginPaint = "Win32BeginPaint";

HPS APIENTRY Trc_Win32BeginPaint( HWND   Parm1
                                , HPS    Parm2
                                , PRECTL Parm3
                                )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32BeginPaint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32BeginPaint, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32BEGINPAINT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32BeginPaint );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PRECTL );
         if ( ulRC ) T_LogPRECTL( Parm3 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szWin32BeginPaint, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32BeginPaint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16BroadcastMsg()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINBROADCASTMSG( HWND, USHORT, MPARAM, MPARAM, USHORT );
PCHAR szWin16BroadcastMsg = "Win16BroadcastMsg";

BOOL16 APIENTRY16 Trc_Win16BroadcastMsg( HWND   Parm1
                                       , USHORT Parm2
                                       , MPARAM Parm3
                                       , MPARAM Parm4
                                       , USHORT Parm5
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16BroadcastMsg );
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
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szWin16BroadcastMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINBROADCASTMSG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16BroadcastMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16BroadcastMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16BroadcastMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32BroadcastMsg()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32BROADCASTMSG( HWND, ULONG, MPARAM, MPARAM, ULONG );
PCHAR szWin32BroadcastMsg = "Win32BroadcastMsg";

BOOL APIENTRY Trc_Win32BroadcastMsg( HWND   Parm1
                                   , ULONG  Parm2
                                   , MPARAM Parm3
                                   , MPARAM Parm4
                                   , ULONG  Parm5
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32BroadcastMsg );
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
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szWin32BroadcastMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32BROADCASTMSG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32BroadcastMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32BroadcastMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32BroadcastMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CalcFrameRect()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCALCFRAMERECT( HWND, PRECTL16, BOOL16 );
PCHAR szWin16CalcFrameRect = "Win16CalcFrameRect";

BOOL16 APIENTRY16 Trc_Win16CalcFrameRect( HWND     Parm1
                                        , PRECTL16 Parm2
                                        , BOOL16   Parm3
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CalcFrameRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16CalcFrameRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINCALCFRAMERECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CalcFrameRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16CalcFrameRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CalcFrameRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CalcFrameRect()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32CALCFRAMERECT( HWND, PRECTL, BOOL );
PCHAR szWin32CalcFrameRect = "Win32CalcFrameRect";

BOOL APIENTRY Trc_Win32CalcFrameRect( HWND   Parm1
                                    , PRECTL Parm2
                                    , BOOL   Parm3
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CalcFrameRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32CalcFrameRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32CALCFRAMERECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CalcFrameRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32CalcFrameRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CalcFrameRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CallMsgFilter()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCALLMSGFILTER( HAB, PQMSG16, USHORT );
PCHAR szWin16CallMsgFilter = "Win16CallMsgFilter";

BOOL16 APIENTRY16 Trc_Win16CallMsgFilter( HAB     Parm1
                                        , PQMSG16 Parm2
                                        , USHORT  Parm3
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CallMsgFilter );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPQMSG16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16CallMsgFilter, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINCALLMSGFILTER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CallMsgFilter );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      HOOKEXIT16( szWin16CallMsgFilter, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CallMsgFilter() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CallMsgFilter()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32CALLMSGFILTER( HAB, PQMSG, ULONG );
PCHAR szWin32CallMsgFilter = "Win32CallMsgFilter";

BOOL APIENTRY Trc_Win32CallMsgFilter( HAB   Parm1
                                    , PQMSG Parm2
                                    , ULONG Parm3
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CallMsgFilter );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPQMSG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32CallMsgFilter, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32CALLMSGFILTER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CallMsgFilter );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      HOOKEXIT( szWin32CallMsgFilter, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CallMsgFilter() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CancelShutdown()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCANCELSHUTDOWN( HMQ, BOOL16 );
PCHAR szWin16CancelShutdown = "Win16CancelShutdown";

BOOL16 APIENTRY16 Trc_Win16CancelShutdown( HMQ    Parm1
                                         , BOOL16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CancelShutdown );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16CancelShutdown, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINCANCELSHUTDOWN( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CancelShutdown );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16CancelShutdown, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CancelShutdown() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CancelShutdown()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32CANCELSHUTDOWN( HMQ, BOOL );
PCHAR szWin32CancelShutdown = "Win32CancelShutdown";

BOOL APIENTRY Trc_Win32CancelShutdown( HMQ  Parm1
                                     , BOOL Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CancelShutdown );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32CancelShutdown, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32CANCELSHUTDOWN( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CancelShutdown );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32CancelShutdown, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CancelShutdown() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CheckInput()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 210 ) )
BOOL16 APIENTRY16 WINCHECKINPUT( HAB );
PCHAR szWin16CheckInput = "Win16CheckInput";

BOOL16 APIENTRY16 Trc_Win16CheckInput( HAB Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CheckInput );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16CheckInput, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnWINCHECKINPUT )( Parm1 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CheckInput );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16CheckInput, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CheckInput() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CheckInput()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
BOOL APIENTRY WIN32CHECKINPUT( HAB );
PCHAR szWin32CheckInput = "Win32CheckInput";

BOOL APIENTRY Trc_Win32CheckInput( HAB Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CheckInput );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32CheckInput, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnWIN32CHECKINPUT )( Parm1 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CheckInput );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32CheckInput, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CheckInput() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CloseClipbrd()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCLOSECLIPBRD( HAB );
PCHAR szWin16CloseClipbrd = "Win16CloseClipbrd";

BOOL16 APIENTRY16 Trc_Win16CloseClipbrd( HAB Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CloseClipbrd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16CloseClipbrd, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINCLOSECLIPBRD( Parm1 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CloseClipbrd );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16CloseClipbrd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CloseClipbrd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CloseClipbrd()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32CLOSECLIPBRD( HAB );
PCHAR szWin32CloseClipbrd = "Win32CloseClipbrd";

BOOL APIENTRY Trc_Win32CloseClipbrd( HAB Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CloseClipbrd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32CloseClipbrd, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32CLOSECLIPBRD( Parm1 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CloseClipbrd );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32CloseClipbrd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CloseClipbrd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CompareStrings()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINCOMPARESTRINGS( HAB, USHORT, USHORT, PSZ16, PSZ16, USHORT );
PCHAR szWin16CompareStrings = "Win16CompareStrings";

USHORT APIENTRY16 Trc_Win16CompareStrings( HAB    Parm1
                                         , USHORT Parm2
                                         , USHORT Parm3
                                         , PSZ16  Parm4
                                         , PSZ16  Parm5
                                         , USHORT Parm6
                                         )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CompareStrings );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
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
      }
      HOOKENTRY16( szWin16CompareStrings, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINCOMPARESTRINGS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CompareStrings );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC != WCS_ERROR );
      HOOKEXIT16( szWin16CompareStrings, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16CompareStrings() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CompareStrings()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32COMPARESTRINGS( HAB, ULONG, ULONG, PSZ, PSZ, ULONG );
PCHAR szWin32CompareStrings = "Win32CompareStrings";

ULONG APIENTRY Trc_Win32CompareStrings( HAB   Parm1
                                      , ULONG Parm2
                                      , ULONG Parm3
                                      , PSZ   Parm4
                                      , PSZ   Parm5
                                      , ULONG Parm6
                                      )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CompareStrings );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
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
      }
      HOOKENTRY( szWin32CompareStrings, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32COMPARESTRINGS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CompareStrings );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != WCS_ERROR );
      HOOKEXIT( szWin32CompareStrings, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CompareStrings() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CopyAccelTable()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINCOPYACCELTABLE( HACCEL, PACCELTABLE16, USHORT );
PCHAR szWin16CopyAccelTable = "Win16CopyAccelTable";

USHORT APIENTRY16 Trc_Win16CopyAccelTable( HACCEL        Parm1
                                         , PACCELTABLE16 Parm2
                                         , USHORT        Parm3
                                         )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CopyAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HACCEL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PACCELTABLE );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16CopyAccelTable, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINCOPYACCELTABLE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CopyAccelTable );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PACCELTABLE );
         if ( usRC ) T_LogPACCELTABLE16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16CopyAccelTable, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16CopyAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CopyAccelTable()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32COPYACCELTABLE( HACCEL, PACCELTABLE, ULONG );
PCHAR szWin32CopyAccelTable = "Win32CopyAccelTable";

ULONG APIENTRY Trc_Win32CopyAccelTable( HACCEL      Parm1
                                      , PACCELTABLE Parm2
                                      , ULONG       Parm3
                                      )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CopyAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HACCEL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PACCELTABLE );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32CopyAccelTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32COPYACCELTABLE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CopyAccelTable );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PACCELTABLE );
         if ( ulRC ) T_LogPACCELTABLE( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32CopyAccelTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CopyAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CopyRect()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCOPYRECT( HAB, PRECTL16, PRECTL16 );
PCHAR szWin16CopyRect = "Win16CopyRect";

BOOL16 APIENTRY16 Trc_Win16CopyRect( HAB      Parm1
                                   , PRECTL16 Parm2
                                   , PRECTL16 Parm3
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CopyRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
      }
      HOOKENTRY16( szWin16CopyRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINCOPYRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CopyRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16CopyRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CopyRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CopyRect()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32COPYRECT( HAB, PRECTL, PRECTL );
PCHAR szWin32CopyRect = "Win32CopyRect";

BOOL APIENTRY Trc_Win32CopyRect( HAB    Parm1
                               , PRECTL Parm2
                               , PRECTL Parm3
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CopyRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
      }
      HOOKENTRY( szWin32CopyRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32COPYRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CopyRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32CopyRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CopyRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CpTranslateChar()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
UCHAR APIENTRY16 WINCPTRANSLATECHAR( HAB, USHORT, UCHAR, USHORT );
PCHAR szWin16CpTranslateChar = "Win16CpTranslateChar";

UCHAR APIENTRY16 Trc_Win16CpTranslateChar( HAB    Parm1
                                         , USHORT Parm2
                                         , UCHAR  Parm3
                                         , USHORT Parm4
                                         )
{

   UCHAR chRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CpTranslateChar );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_UCHAR );
         T_LogUCHAR( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16CpTranslateChar, &Parm1 );
      T_FreeLog( );
   }

   chRC = WINCPTRANSLATECHAR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CpTranslateChar );
      T_LogPMReturnCodeUSHORT( DT_UCHAR,chRC, chRC );
      HOOKEXIT16( szWin16CpTranslateChar, &Parm1, &chRC );
      T_FreeLog( );
   }

   return chRC;

}  /* Trc_Win16CpTranslateChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CpTranslateChar()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
UCHAR APIENTRY WIN32CPTRANSLATECHAR( HAB, ULONG, UCHAR, ULONG );
PCHAR szWin32CpTranslateChar = "Win32CpTranslateChar";

UCHAR APIENTRY Trc_Win32CpTranslateChar( HAB   Parm1
                                       , ULONG Parm2
                                       , UCHAR Parm3
                                       , ULONG Parm4
                                       )
{

   UCHAR chRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CpTranslateChar );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_UCHAR );
         T_LogUCHAR( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32CpTranslateChar, &Parm1 );
      T_FreeLog( );
   }

   chRC = WIN32CPTRANSLATECHAR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CpTranslateChar );
      T_LogPMReturnCode( DT_UCHAR,chRC, chRC );
      HOOKEXIT( szWin32CpTranslateChar, &Parm1, &chRC );
      T_FreeLog( );
   }

   return chRC;

}  /* Trc_Win32CpTranslateChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CpTranslateString()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCPTRANSLATESTRING( HAB, USHORT, PSZ16, USHORT, USHORT, PSZ16 );
PCHAR szWin16CpTranslateString = "Win16CpTranslateString";

BOOL16 APIENTRY16 Trc_Win16CpTranslateString( HAB    Parm1
                                            , USHORT Parm2
                                            , PSZ16  Parm3
                                            , USHORT Parm4
                                            , USHORT Parm5
                                            , PSZ16  Parm6
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CpTranslateString );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_PSZ );
         T_LogPVOID16( Parm6 );
      }
      HOOKENTRY16( szWin16CpTranslateString, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINCPTRANSLATESTRING( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CpTranslateString );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 6L, DT_PSZ );
         if ( bRC ) T_LogPSZ16( Parm6 );
         else T_LogPVOID16( Parm6 );
      }
      HOOKEXIT16( szWin16CpTranslateString, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CpTranslateString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CpTranslateString()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32CPTRANSLATESTRING( HAB, ULONG, PSZ, ULONG, ULONG, PSZ );
PCHAR szWin32CpTranslateString = "Win32CpTranslateString";

BOOL APIENTRY Trc_Win32CpTranslateString( HAB   Parm1
                                        , ULONG Parm2
                                        , PSZ   Parm3
                                        , ULONG Parm4
                                        , ULONG Parm5
                                        , PSZ   Parm6
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CpTranslateString );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PSZ );
         T_LogPVOID( Parm6 );
      }
      HOOKENTRY( szWin32CpTranslateString, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32CPTRANSLATESTRING( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CpTranslateString );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 6L, DT_PSZ );
         if ( bRC ) T_LogPSZ( Parm6 );
         else T_LogPVOID( Parm6 );
      }
      HOOKEXIT( szWin32CpTranslateString, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CpTranslateString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateAccelTable()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HACCEL APIENTRY16 WINCREATEACCELTABLE( HAB, PACCELTABLE16 );
PCHAR szWin16CreateAccelTable = "Win16CreateAccelTable";

HACCEL APIENTRY16 Trc_Win16CreateAccelTable( HAB           Parm1
                                           , PACCELTABLE16 Parm2
                                           )
{

   HACCEL ulRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PACCELTABLE );
         T_LogPACCELTABLE16( Parm2 );
      }
      HOOKENTRY16( szWin16CreateAccelTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEACCELTABLE( Parm1, Parm2 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateAccelTable );
      T_LogPMReturnCode( DT_HACCEL, ulRC, ulRC );
      HOOKEXIT16( szWin16CreateAccelTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreateAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateAccelTable()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HACCEL APIENTRY WIN32CREATEACCELTABLE( HAB, PACCELTABLE );
PCHAR szWin32CreateAccelTable = "Win32CreateAccelTable";

HACCEL APIENTRY Trc_Win32CreateAccelTable( HAB         Parm1
                                         , PACCELTABLE Parm2
                                         )
{

   HACCEL ulRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PACCELTABLE );
         T_LogPACCELTABLE( Parm2 );
      }
      HOOKENTRY( szWin32CreateAccelTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATEACCELTABLE( Parm1, Parm2 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateAccelTable );
      T_LogPMReturnCode( DT_HACCEL, ulRC, ulRC );
      HOOKEXIT( szWin32CreateAccelTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreateAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateAtomTable()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HATOMTBL APIENTRY16 WINCREATEATOMTABLE( USHORT, USHORT );
PCHAR szWin16CreateAtomTable = "Win16CreateAtomTable";

HATOMTBL APIENTRY16 Trc_Win16CreateAtomTable( USHORT Parm1
                                            , USHORT Parm2
                                            )
{

   HATOMTBL ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateAtomTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16CreateAtomTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEATOMTABLE( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateAtomTable );
      T_LogPMReturnCode( DT_HATOMTBL, ulRC, ulRC );
      HOOKEXIT16( szWin16CreateAtomTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreateAtomTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateAtomTable()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HATOMTBL APIENTRY WIN32CREATEATOMTABLE( ULONG, ULONG );
PCHAR szWin32CreateAtomTable = "Win32CreateAtomTable";

HATOMTBL APIENTRY Trc_Win32CreateAtomTable( ULONG Parm1
                                          , ULONG Parm2
                                          )
{

   HATOMTBL ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateAtomTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32CreateAtomTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATEATOMTABLE( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateAtomTable );
      T_LogPMReturnCode( DT_HATOMTBL, ulRC, ulRC );
      HOOKEXIT( szWin32CreateAtomTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreateAtomTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateCursor()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCREATECURSOR( HWND, SHORT, SHORT, SHORT, SHORT, USHORT, PRECTL16 );
PCHAR szWin16CreateCursor = "Win16CreateCursor";

BOOL16 APIENTRY16 Trc_Win16CreateCursor( HWND     Parm1
                                       , SHORT    Parm2
                                       , SHORT    Parm3
                                       , SHORT    Parm4
                                       , SHORT    Parm5
                                       , USHORT   Parm6
                                       , PRECTL16 Parm7
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateCursor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_USHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_PRECTL );
         T_LogPRECTL16( Parm7 );
      }
      HOOKENTRY16( szWin16CreateCursor, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINCREATECURSOR( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateCursor );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16CreateCursor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CreateCursor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateCursor()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32CREATECURSOR( HWND, LONG, LONG, LONG, LONG, ULONG, PRECTL );
PCHAR szWin32CreateCursor = "Win32CreateCursor";

BOOL APIENTRY Trc_Win32CreateCursor( HWND   Parm1
                                   , LONG   Parm2
                                   , LONG   Parm3
                                   , LONG   Parm4
                                   , LONG   Parm5
                                   , ULONG  Parm6
                                   , PRECTL Parm7
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateCursor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PRECTL );
         T_LogPRECTL( Parm7 );
      }
      HOOKENTRY( szWin32CreateCursor, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32CREATECURSOR( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateCursor );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32CreateCursor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CreateCursor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateDlg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINCREATEDLG( HWND, HWND, PFNWP16, PDLGTEMPLATE16, PVOID16 );
PCHAR szWin16CreateDlg = "Win16CreateDlg";

HWND APIENTRY16 Trc_Win16CreateDlg( HWND           Parm1
                                  , HWND           Parm2
                                  , PFNWP16        Parm3
                                  , PDLGTEMPLATE16 Parm4
                                  , PVOID16        Parm5
                                  )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateDlg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PFNWP );
         T_LogPFN16( (PFN16)Parm3 );
         T_LogParameter( 4L, DT_PDLGTEMPLATE );
         T_LogPDLGTEMPLATE16( Parm4 );
         T_LogParameter( 5L, DT_PVOID );
         T_LogPVOID16( Parm5 );
         T_LogCHARBufferUnformatted16( Parm5 );
      }
      HOOKENTRY16( szWin16CreateDlg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEDLG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateDlg );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16CreateDlg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreateDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateDlg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32CREATEDLG( HWND, HWND, PFNWP, PDLGTEMPLATE, PVOID );
PCHAR szWin32CreateDlg = "Win32CreateDlg";

HWND APIENTRY Trc_Win32CreateDlg( HWND         Parm1
                                , HWND         Parm2
                                , PFNWP        Parm3
                                , PDLGTEMPLATE Parm4
                                , PVOID        Parm5
                                )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateDlg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PFNWP );
         T_LogPFN( (PFN)Parm3 );
         T_LogParameter( 4L, DT_PDLGTEMPLATE );
         T_LogPDLGTEMPLATE( Parm4 );
         T_LogParameter( 5L, DT_PVOID );
         T_LogPVOID( Parm5 );
         T_LogCHARBufferUnformatted( Parm5 );
      }
      HOOKENTRY( szWin32CreateDlg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATEDLG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateDlg );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32CreateDlg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreateDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateFrameControls()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINCREATEFRAMECONTROLS( HWND, PFRAMECDATA16, PSZ16 );
PCHAR szWin16CreateFrameControls = "Win16CreateFrameControls";

BOOL16 APIENTRY16 Trc_Win16CreateFrameControls( HWND          Parm1
                                              , PFRAMECDATA16 Parm2
                                              , PSZ16         Parm3
                                              )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateFrameControls );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFRAMECDATA );
         T_LogPFRAMECDATA16( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
      }
      HOOKENTRY16( szWin16CreateFrameControls, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINCREATEFRAMECONTROLS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateFrameControls );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16CreateFrameControls, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16CreateFrameControls() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateFrameControls()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32CREATEFRAMECONTROLS( HWND, PFRAMECDATA, PSZ );
PCHAR szWin32CreateFrameControls = "Win32CreateFrameControls";

BOOL APIENTRY Trc_Win32CreateFrameControls( HWND        Parm1
                                          , PFRAMECDATA Parm2
                                          , PSZ         Parm3
                                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateFrameControls );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFRAMECDATA );
         T_LogPFRAMECDATA( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
      }
      HOOKENTRY( szWin32CreateFrameControls, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32CREATEFRAMECONTROLS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateFrameControls );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32CreateFrameControls, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32CreateFrameControls() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateHeap()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HHEAP16 APIENTRY16 WINCREATEHEAP( SEL, USHORT, USHORT, USHORT, USHORT, USHORT );
PCHAR szWin16CreateHeap = "Win16CreateHeap";

HHEAP16 APIENTRY16 Trc_Win16CreateHeap( SEL    Parm1
                                      , USHORT Parm2
                                      , USHORT Parm3
                                      , USHORT Parm4
                                      , USHORT Parm5
                                      , USHORT Parm6
                                      )
{

   HHEAP16 ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateHeap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_SEL );
         T_LogUSHORT( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_USHORT );
         T_LogUSHORT( Parm6 );
      }
      HOOKENTRY16( szWin16CreateHeap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEHEAP( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateHeap );
      T_LogPMReturnCode( DT_HHEAP, ulRC, ulRC );
      HOOKEXIT16( szWin16CreateHeap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreateHeap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateMenu()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINCREATEMENU( HWND, PVOID16 );
PCHAR szWin16CreateMenu = "Win16CreateMenu";

HWND APIENTRY16 Trc_Win16CreateMenu( HWND    Parm1
                                   , PVOID16 Parm2
                                   )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateMenu );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogLPMT16( Parm2 );
      }
      HOOKENTRY16( szWin16CreateMenu, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEMENU( Parm1, Parm2 );

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateMenu );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16CreateMenu, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreateMenu() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateMenu()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32CREATEMENU( HWND, PVOID );
PCHAR szWin32CreateMenu = "Win32CreateMenu";

HWND APIENTRY Trc_Win32CreateMenu( HWND  Parm1
                                 , PVOID Parm2
                                 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateMenu );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogLPMT( Parm2 );
      }
      HOOKENTRY( szWin32CreateMenu, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATEMENU( Parm1, Parm2 );

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateMenu );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32CreateMenu, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreateMenu() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateMsgQueue()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HMQ APIENTRY16 WINCREATEMSGQUEUE( HAB, SHORT );
PCHAR szWin16CreateMsgQueue = "Win16CreateMsgQueue";

HMQ APIENTRY16 Trc_Win16CreateMsgQueue( HAB   Parm1
                                      , SHORT Parm2
                                      )
{

   HMQ ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateMsgQueue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16CreateMsgQueue, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEMSGQUEUE( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateMsgQueue );
      T_LogPMReturnCode( DT_HMQ, ulRC, ulRC );
      HOOKEXIT16( szWin16CreateMsgQueue, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreateMsgQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateMsgQueue()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HMQ APIENTRY WIN32CREATEMSGQUEUE( HAB, LONG );
PCHAR szWin32CreateMsgQueue = "Win32CreateMsgQueue";

HMQ APIENTRY Trc_Win32CreateMsgQueue( HAB  Parm1
                                    , LONG Parm2
                                    )
{

   HMQ ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateMsgQueue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32CreateMsgQueue, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATEMSGQUEUE( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateMsgQueue );
      T_LogPMReturnCode( DT_HMQ, ulRC, ulRC );
      HOOKEXIT( szWin32CreateMsgQueue, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreateMsgQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreatePointer()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPOINTER APIENTRY16 WINCREATEPOINTER( HWND, HBITMAP, BOOL16, SHORT, SHORT );
PCHAR szWin16CreatePointer = "Win16CreatePointer";

HPOINTER APIENTRY16 Trc_Win16CreatePointer( HWND    Parm1
                                          , HBITMAP Parm2
                                          , BOOL16  Parm3
                                          , SHORT   Parm4
                                          , SHORT   Parm5
                                        )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreatePointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szWin16CreatePointer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEPOINTER( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreatePointer );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT16( szWin16CreatePointer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreatePointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreatePointer()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPOINTER APIENTRY WIN32CREATEPOINTER( HWND, HBITMAP, BOOL, LONG, LONG );
PCHAR szWin32CreatePointer = "Win32CreatePointer";

HPOINTER APIENTRY Trc_Win32CreatePointer( HWND    Parm1
                                        , HBITMAP Parm2
                                        , BOOL    Parm3
                                        , LONG    Parm4
                                        , LONG    Parm5
                                        )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreatePointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szWin32CreatePointer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATEPOINTER( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreatePointer );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT( szWin32CreatePointer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreatePointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreatePointerIndirect()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPOINTER APIENTRY16 WINCREATEPOINTERINDIRECT( HWND, PPOINTERINFO16 );
PCHAR szWin16CreatePointerIndirect = "Win16CreatePointerIndirect";

HPOINTER APIENTRY16 Trc_Win16CreatePointerIndirect( HWND           Parm1
                                                  , PPOINTERINFO16 Parm2
                                                  )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreatePointerIndirect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTERINFO );
         T_LogPPOINTERINFO16( Parm2 );
      }
      HOOKENTRY16( szWin16CreatePointerIndirect, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEPOINTERINDIRECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreatePointerIndirect );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT16( szWin16CreatePointerIndirect, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreatePointerIndirect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreatePointerIndirect()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPOINTER APIENTRY WIN32CREATEPOINTERINDIRECT( HWND, PPOINTERINFO );
PCHAR szWin32CreatePointerIndirect = "Win32CreatePointerIndirect";

HPOINTER APIENTRY Trc_Win32CreatePointerIndirect( HWND         Parm1
                                                , PPOINTERINFO Parm2
                                                )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreatePointerIndirect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTERINFO );
         T_LogPPOINTERINFO( Parm2 );
      }
      HOOKENTRY( szWin32CreatePointerIndirect, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATEPOINTERINDIRECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreatePointerIndirect );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT( szWin32CreatePointerIndirect, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreatePointerIndirect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateStdWindow()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINCREATESTDWINDOW( HWND, ULONG, PULONG16, PSZ16, PSZ16, ULONG, HMODULE16, USHORT, PHWND16 );
PCHAR szWin16CreateStdWindow = "Win16CreateStdWindow";

HWND APIENTRY16 Trc_Win16CreateStdWindow( HWND      Parm1
                                        , ULONG     Parm2
                                        , PULONG16  Parm3
                                        , PSZ16     Parm4
                                        , PSZ16     Parm5
                                        , ULONG     Parm6
                                        , HMODULE16 Parm7
                                        , USHORT    Parm8
                                        , PHWND16   Parm9
                                        )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateStdWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG16( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ16( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZ16( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_HMODULE );
         T_LogUSHORT( Parm7 );
         T_LogParameter( 8L, DT_USHORT );
         T_LogUSHORT( Parm8 );
         T_LogParameter( 9L, DT_PHWND );
         T_LogPVOID16( Parm9 );
      }
      HOOKENTRY16( szWin16CreateStdWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATESTDWINDOW( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateStdWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 9L, DT_PHWND );
         T_LogPULONG16( Parm9 );
      }
      HOOKEXIT16( szWin16CreateStdWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreateStdWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateStdWindow()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32CREATESTDWINDOW( HWND, ULONG, PULONG, PSZ, PSZ, ULONG, HMODULE, ULONG, PHWND );
PCHAR szWin32CreateStdWindow = "Win32CreateStdWindow";

HWND APIENTRY Trc_Win32CreateStdWindow( HWND    Parm1
                                      , ULONG   Parm2
                                      , PULONG  Parm3
                                      , PSZ     Parm4
                                      , PSZ     Parm5
                                      , ULONG   Parm6
                                      , HMODULE Parm7
                                      , ULONG   Parm8
                                      , PHWND   Parm9
                                      )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateStdWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZ( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_HMODULE );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_ULONG );
         T_LogULONG( Parm8 );
         T_LogParameter( 9L, DT_PHWND );
         T_LogPVOID( Parm9 );
      }
      HOOKENTRY( szWin32CreateStdWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATESTDWINDOW( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateStdWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 9L, DT_PHWND );
         T_LogPULONG( Parm9 );
      }
      HOOKEXIT( szWin32CreateStdWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreateStdWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16CreateWindow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINCREATEWINDOW( HWND, PSZ16, PSZ16, ULONG, SHORT, SHORT, SHORT, SHORT, HWND, HWND, USHORT, PVOID16, PVOID16 );
PCHAR szWin16CreateWindow = "Win16CreateWindow";

HWND APIENTRY16 Trc_Win16CreateWindow( HWND    Parm1
                                     , PSZ16   Parm2
                                     , PSZ16   Parm3
                                     , ULONG   Parm4
                                     , SHORT   Parm5
                                     , SHORT   Parm6
                                     , SHORT   Parm7
                                     , SHORT   Parm8
                                     , HWND    Parm9
                                     , HWND    Parm10
                                     , USHORT  Parm11
                                     , PVOID16 Parm12
                                     , PVOID16 Parm13
                                     )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16CreateWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_SHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_SHORT );
         T_LogUSHORT( Parm7 );
         T_LogParameter( 8L, DT_SHORT );
         T_LogUSHORT( Parm8 );
         T_LogParameter( 9L, DT_HWND );
         T_LogULONG( Parm9 );
         T_LogParameter( 10L, DT_HWND );
         T_LogULONG( Parm10 );
         T_LogParameter( 11L, DT_USHORT );
         T_LogUSHORT( Parm11 );
         T_LogParameter( 12L, DT_PVOID );
         T_LogPVOID16( Parm12 );
         T_LogCHARBufferUnformatted16( Parm12 );
         T_LogParameter( 13L, DT_PVOID );
         T_LogPVOID16( Parm13 );
         T_LogCHARBufferUnformatted16( Parm13 );
      }
      HOOKENTRY16( szWin16CreateWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINCREATEWINDOW( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16CreateWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16CreateWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16CreateWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32CreateWindow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32CREATEWINDOW( HWND, PSZ, PSZ, ULONG, LONG, LONG, LONG, LONG, HWND, HWND, ULONG, PVOID, PVOID );
PCHAR szWin32CreateWindow = "Win32CreateWindow";

HWND APIENTRY Trc_Win32CreateWindow( HWND  Parm1
                                   , PSZ   Parm2
                                   , PSZ   Parm3
                                   , ULONG Parm4
                                   , LONG  Parm5
                                   , LONG  Parm6
                                   , LONG  Parm7
                                   , LONG  Parm8
                                   , HWND  Parm9
                                   , HWND  Parm10
                                   , ULONG Parm11
                                   , PVOID Parm12
                                   , PVOID Parm13
                                   )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32CreateWindow );
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
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_LONG );
         T_LogULONG( Parm7 );
         T_LogParameter( 8L, DT_LONG );
         T_LogULONG( Parm8 );
         T_LogParameter( 9L, DT_HWND );
         T_LogULONG( Parm9 );
         T_LogParameter( 10L, DT_HWND );
         T_LogULONG( Parm10 );
         T_LogParameter( 11L, DT_ULONG );
         T_LogULONG( Parm11 );
         T_LogParameter( 12L, DT_PVOID );
         T_LogPVOID( Parm12 );
         T_LogCHARBufferUnformatted( Parm12 );
         T_LogParameter( 13L, DT_PVOID );
         T_LogPVOID( Parm13 );
         T_LogCHARBufferUnformatted( Parm13 );
      }
      HOOKENTRY( szWin32CreateWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32CREATEWINDOW( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32CreateWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32CreateWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32CreateWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DdeInitiate()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDDEINITIATE( HWND, PSZ16, PSZ16 );
PCHAR szWin16DdeInitiate = "Win16DdeInitiate";

BOOL16 APIENTRY16 Trc_Win16DdeInitiate( HWND  Parm1
                                      , PSZ16 Parm2
                                      , PSZ16 Parm3
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DdeInitiate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
      }
      HOOKENTRY16( szWin16DdeInitiate, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDDEINITIATE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DdeInitiate );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DdeInitiate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DdeInitiate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DdeInitiate()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DDEINITIATE( HWND, PSZ, PSZ, PCONVCONTEXT );
PCHAR szWin32DdeInitiate = "Win32DdeInitiate";

BOOL APIENTRY Trc_Win32DdeInitiate( HWND         Parm1
                                  , PSZ          Parm2
                                  , PSZ          Parm3
                                  , PCONVCONTEXT Parm4
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DdeInitiate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PCONVCONTEXT );
         T_LogPCONVCONTEXT( Parm4 );
      }
      HOOKENTRY( szWin32DdeInitiate, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DDEINITIATE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DdeInitiate );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DdeInitiate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DdeInitiate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DdePostMsg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDDEPOSTMSG( HWND, HWND, USHORT, PDDESTRUCT16, BOOL16 );
PCHAR szWin16DdePostMsg = "Win16DdePostMsg";

BOOL16 APIENTRY16 Trc_Win16DdePostMsg( HWND         Parm1
                                     , HWND         Parm2
                                     , USHORT       Parm3
                                     , PDDESTRUCT16 Parm4
                                     , BOOL16       Parm5
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DdePostMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogPMMessage16( Parm3 );
         T_LogParameter( 4L, DT_PDDESTRUCT );
         T_LogPDDESTRUCT16( Parm4 );
         T_LogParameter( 5L, DT_BOOL );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szWin16DdePostMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDDEPOSTMSG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DdePostMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DdePostMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DdePostMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DdePostMsg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DDEPOSTMSG( HWND, HWND, ULONG, PDDESTRUCT, ULONG );
PCHAR szWin32DdePostMsg = "Win32DdePostMsg";

BOOL APIENTRY Trc_Win32DdePostMsg( HWND       Parm1
                                 , HWND       Parm2
                                 , ULONG      Parm3
                                 , PDDESTRUCT Parm4
                                 , ULONG      Parm5
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DdePostMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogPMMessage( Parm3 );
         T_LogParameter( 4L, DT_PDDESTRUCT );
         T_LogPDDESTRUCT( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szWin32DdePostMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DDEPOSTMSG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DdePostMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DdePostMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DdePostMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DdeRespond()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
MRESULT16 APIENTRY16 WINDDERESPOND( HWND, HWND, PSZ16, PSZ16 );
PCHAR szWin16DdeRespond = "Win16DdeRespond";

MRESULT16 APIENTRY16 Trc_Win16DdeRespond( HWND  Parm1
                                        , HWND  Parm2
                                        , PSZ16 Parm3
                                        , PSZ16 Parm4
                                        )
{

   MRESULT16 ulRC;                               /* API return code           */

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DdeRespond );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ16( Parm4 );
      }
      HOOKENTRY16( szWin16DdeRespond, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINDDERESPOND( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DdeRespond );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT16( szWin16DdeRespond, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16DdeRespond() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DdeRespond()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY WIN32DDERESPOND( HWND, HWND, PSZ, PSZ, PCONVCONTEXT );
PCHAR szWin32DdeRespond = "Win32DdeRespond";

MRESULT APIENTRY Trc_Win32DdeRespond( HWND         Parm1
                                    , HWND         Parm2
                                    , PSZ          Parm3
                                    , PSZ          Parm4
                                    , PCONVCONTEXT Parm5
                                    )
{

   MRESULT ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DdeRespond );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
         T_LogParameter( 5L, DT_PCONVCONTEXT );
         T_LogPCONVCONTEXT( Parm5 );
      }
      HOOKENTRY( szWin32DdeRespond, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32DDERESPOND( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_DDE ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DdeRespond );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT( szWin32DdeRespond, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32DdeRespond() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DefDlgProc()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
MRESULT16 APIENTRY16 WINDEFDLGPROC( HWND, USHORT, MPARAM, MPARAM );
PCHAR szWin16DefDlgProc = "Win16DefDlgProc";

MRESULT16 APIENTRY16 Trc_Win16DefDlgProc( HWND   Parm1
                                        , USHORT Parm2
                                        , MPARAM Parm3
                                        , MPARAM Parm4
                                        )
{

   MRESULT16 ulRC;                               /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DefDlgProc );
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
      HOOKENTRY16( szWin16DefDlgProc, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINDEFDLGPROC( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DefDlgProc );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT16( szWin16DefDlgProc, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16DefDlgProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DefDlgProc()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY WIN32DEFDLGPROC( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32DefDlgProc = "Win32DefDlgProc";

MRESULT APIENTRY Trc_Win32DefDlgProc( HWND   Parm1
                                    , ULONG  Parm2
                                    , MPARAM Parm3
                                    , MPARAM Parm4
                                    )
{

   MRESULT ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DefDlgProc );
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
      HOOKENTRY( szWin32DefDlgProc, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32DEFDLGPROC( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DefDlgProc );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT( szWin32DefDlgProc, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32DefDlgProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DefWindowProc()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
MRESULT16 APIENTRY16 WINDEFWINDOWPROC( HWND, USHORT, MPARAM, MPARAM );
PCHAR szWin16DefWindowProc = "Win16DefWindowProc";

MRESULT16 APIENTRY16 Trc_Win16DefWindowProc( HWND   Parm1
                                           , USHORT Parm2
                                           , MPARAM Parm3
                                           , MPARAM Parm4
                                           )
{

   MRESULT16 ulRC;                               /* API return code           */

   if ( LOGWINAPI( W_DWIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DefWindowProc );
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
      HOOKENTRY16( szWin16DefWindowProc, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINDEFWINDOWPROC( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DWIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DefWindowProc );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT16( szWin16DefWindowProc, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16DefWindowProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DefWindowProc()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY WIN32DEFWINDOWPROC( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32DefWindowProc = "Win32DefWindowProc";

MRESULT APIENTRY Trc_Win32DefWindowProc( HWND   Parm1
                                       , ULONG  Parm2
                                       , MPARAM Parm3
                                       , MPARAM Parm4
                                       )
{

   MRESULT ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_DWIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DefWindowProc );
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
      HOOKENTRY( szWin32DefWindowProc, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32DEFWINDOWPROC( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DWIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DefWindowProc );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT( szWin32DefWindowProc, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32DefWindowProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DeleteAtom()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ATOM16 APIENTRY16 WINDELETEATOM( HATOMTBL, ATOM16 );
PCHAR szWin16DeleteAtom = "Win16DeleteAtom";

ATOM16 APIENTRY16 Trc_Win16DeleteAtom( HATOMTBL Parm1
                                     , ATOM16   Parm2
                                     )
{

   ATOM16 usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DeleteAtom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ATOM );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16DeleteAtom, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINDELETEATOM( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DeleteAtom );
      T_LogPMReturnCodeUSHORT( DT_ATOM, usRC, !usRC );
      HOOKEXIT16( szWin16DeleteAtom, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16DeleteAtom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DeleteAtom()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ATOM APIENTRY WIN32DELETEATOM( HATOMTBL, ATOM );
PCHAR szWin32DeleteAtom = "Win32DeleteAtom";

ATOM APIENTRY Trc_Win32DeleteAtom( HATOMTBL Parm1
                                 , ATOM     Parm2
                                 )
{

   ATOM ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DeleteAtom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ATOM );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32DeleteAtom, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32DELETEATOM( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DeleteAtom );
      T_LogPMReturnCode( DT_ATOM, ulRC, !ulRC );
      HOOKEXIT( szWin32DeleteAtom, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32DeleteAtom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DeleteLibrary()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDELETELIBRARY( HAB, HLIB16 );
PCHAR szWin16DeleteLibrary = "Win16DeleteLibrary";

BOOL16 APIENTRY16 Trc_Win16DeleteLibrary( HAB    Parm1
                                        , HLIB16 Parm2
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DeleteLibrary );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HLIB );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16DeleteLibrary, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDELETELIBRARY( Parm1, Parm2 );

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DeleteLibrary );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DeleteLibrary, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DeleteLibrary() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DeleteLibrary()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DELETELIBRARY( HAB, HLIB );
PCHAR szWin32DeleteLibrary = "Win32DeleteLibrary";

BOOL APIENTRY Trc_Win32DeleteLibrary( HAB  Parm1
                                    , HLIB Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DeleteLibrary );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HLIB );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32DeleteLibrary, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DELETELIBRARY( Parm1, Parm2 );

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DeleteLibrary );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DeleteLibrary, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DeleteLibrary() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DeleteProcedure()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDELETEPROCEDURE( HAB, PFNWP16 );
PCHAR szWin16DeleteProcedure = "Win16DeleteProcedure";

BOOL16 APIENTRY16 Trc_Win16DeleteProcedure( HAB     Parm1
                                          , PFNWP16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DeleteProcedure );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFNWP );
         T_LogPFN16( (PFN16)Parm2 );
      }
      HOOKENTRY16( szWin16DeleteProcedure, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDELETEPROCEDURE( Parm1, Parm2 );

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DeleteProcedure );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DeleteProcedure, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DeleteProcedure() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DeleteProcedure()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DELETEPROCEDURE( HAB, PFNWP );
PCHAR szWin32DeleteProcedure = "Win32DeleteProcedure";

BOOL APIENTRY Trc_Win32DeleteProcedure( HAB   Parm1
                                      , PFNWP Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DeleteProcedure );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFNWP );
         T_LogPFN( (PFN)Parm2 );
      }
      HOOKENTRY( szWin32DeleteProcedure, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DELETEPROCEDURE( Parm1, Parm2 );

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DeleteProcedure );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DeleteProcedure, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DeleteProcedure() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyAccelTable()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDESTROYACCELTABLE( HACCEL );
PCHAR szWin16DestroyAccelTable = "Win16DestroyAccelTable";

BOOL16 APIENTRY16 Trc_Win16DestroyAccelTable( HACCEL Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DestroyAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HACCEL );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16DestroyAccelTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDESTROYACCELTABLE( Parm1 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DestroyAccelTable );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DestroyAccelTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DestroyAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DestroyAccelTable()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DESTROYACCELTABLE( HACCEL );
PCHAR szWin32DestroyAccelTable = "Win32DestroyAccelTable";

BOOL APIENTRY Trc_Win32DestroyAccelTable( HACCEL Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DestroyAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HACCEL );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32DestroyAccelTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DESTROYACCELTABLE( Parm1 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DestroyAccelTable );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DestroyAccelTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DestroyAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyAtomTable()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HATOMTBL APIENTRY16 WINDESTROYATOMTABLE( HATOMTBL );
PCHAR szWin16DestroyAtomTable = "Win16DestroyAtomTable";

HATOMTBL APIENTRY16 Trc_Win16DestroyAtomTable( HATOMTBL Parm1 )
{

   HATOMTBL ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DestroyAtomTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16DestroyAtomTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINDESTROYATOMTABLE( Parm1 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DestroyAtomTable );
      T_LogPMReturnCode( DT_HATOMTBL, ulRC, !ulRC );
      HOOKEXIT16( szWin16DestroyAtomTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16DestroyAtomTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DestroyAtomTable()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HATOMTBL APIENTRY WIN32DESTROYATOMTABLE( HATOMTBL );
PCHAR szWin32DestroyAtomTable = "Win32DestroyAtomTable";

HATOMTBL APIENTRY Trc_Win32DestroyAtomTable( HATOMTBL Parm1 )
{

   HATOMTBL ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DestroyAtomTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32DestroyAtomTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32DESTROYATOMTABLE( Parm1 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DestroyAtomTable );
      T_LogPMReturnCode( DT_HATOMTBL, ulRC, !ulRC );
      HOOKEXIT( szWin32DestroyAtomTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32DestroyAtomTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyCursor()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDESTROYCURSOR( HWND );
PCHAR szWin16DestroyCursor = "Win16DestroyCursor";

BOOL16 APIENTRY16 Trc_Win16DestroyCursor( HWND Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DestroyCursor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16DestroyCursor, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDESTROYCURSOR( Parm1 );

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DestroyCursor );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DestroyCursor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DestroyCursor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DestroyCursor()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DESTROYCURSOR( HWND );
PCHAR szWin32DestroyCursor = "Win32DestroyCursor";

BOOL APIENTRY Trc_Win32DestroyCursor( HWND Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DestroyCursor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32DestroyCursor, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DESTROYCURSOR( Parm1 );

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DestroyCursor );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DestroyCursor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DestroyCursor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyHeap()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HHEAP16 APIENTRY16 WINDESTROYHEAP( HHEAP16 );
PCHAR szWin16DestroyHeap = "Win16DestroyHeap";

HHEAP16 APIENTRY16 Trc_Win16DestroyHeap( HHEAP16 Parm1 )
{

   HHEAP16 ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DestroyHeap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HHEAP );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16DestroyHeap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINDESTROYHEAP( Parm1 );

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DestroyHeap );
      T_LogPMReturnCode( DT_HHEAP, ulRC, !ulRC );
      HOOKEXIT16( szWin16DestroyHeap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16DestroyHeap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyMsgQueue()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDESTROYMSGQUEUE( HMQ );
PCHAR szWin16DestroyMsgQueue = "Win16DestroyMsgQueue";

BOOL16 APIENTRY16 Trc_Win16DestroyMsgQueue( HMQ Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DestroyMsgQueue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16DestroyMsgQueue, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDESTROYMSGQUEUE( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DestroyMsgQueue );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DestroyMsgQueue, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DestroyMsgQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DestroyMsgQueue()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DESTROYMSGQUEUE( HMQ );
PCHAR szWin32DestroyMsgQueue = "Win32DestroyMsgQueue";

BOOL APIENTRY Trc_Win32DestroyMsgQueue( HMQ Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DestroyMsgQueue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32DestroyMsgQueue, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DESTROYMSGQUEUE( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DestroyMsgQueue );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DestroyMsgQueue, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DestroyMsgQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyPointer()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDESTROYPOINTER( HPOINTER );
PCHAR szWin16DestroyPointer = "Win16DestroyPointer";

BOOL16 APIENTRY16 Trc_Win16DestroyPointer( HPOINTER Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DestroyPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPOINTER );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16DestroyPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDESTROYPOINTER( Parm1 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DestroyPointer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DestroyPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DestroyPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DestroyPointer()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DESTROYPOINTER( HPOINTER );
PCHAR szWin32DestroyPointer = "Win32DestroyPointer";

BOOL APIENTRY Trc_Win32DestroyPointer( HPOINTER Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DestroyPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPOINTER );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32DestroyPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DESTROYPOINTER( Parm1 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DestroyPointer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DestroyPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DestroyPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DestroyWindow()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDESTROYWINDOW( HWND );
PCHAR szWin16DestroyWindow = "Win16DestroyWindow";

BOOL16 APIENTRY16 Trc_Win16DestroyWindow( HWND Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DestroyWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16DestroyWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDESTROYWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DestroyWindow );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DestroyWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DestroyWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DestroyWindow()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DESTROYWINDOW( HWND );
PCHAR szWin32DestroyWindow = "Win32DestroyWindow";

BOOL APIENTRY Trc_Win32DestroyWindow( HWND Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DestroyWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32DestroyWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DESTROYWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DestroyWindow );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DestroyWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DestroyWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DismissDlg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDISMISSDLG( HWND, USHORT );
PCHAR szWin16DismissDlg = "Win16DismissDlg";

BOOL16 APIENTRY16 Trc_Win16DismissDlg( HWND   Parm1
                                     , USHORT Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DismissDlg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16DismissDlg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDISMISSDLG( Parm1, Parm2 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DismissDlg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DismissDlg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DismissDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DismissDlg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DISMISSDLG( HWND, ULONG );
PCHAR szWin32DismissDlg = "Win32DismissDlg";

BOOL APIENTRY Trc_Win32DismissDlg( HWND  Parm1
                                 , ULONG Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DismissDlg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32DismissDlg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DISMISSDLG( Parm1, Parm2 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DismissDlg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DismissDlg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DismissDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DispatchMsg()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
MRESULT16 APIENTRY16 WINDISPATCHMSG( HAB, PQMSG16 );
PCHAR szWin16DispatchMsg = "Win16DispatchMsg";

MRESULT16 APIENTRY16 Trc_Win16DispatchMsg( HAB     Parm1
                                         , PQMSG16 Parm2
                                         )
{

   MRESULT16 ulRC;                               /* API return code           */

   if ( LOGWINAPI( W_MSGL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DispatchMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPQMSG16( Parm2 );
      }
      HOOKENTRY16( szWin16DispatchMsg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINDISPATCHMSG( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSGL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DispatchMsg );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT16( szWin16DispatchMsg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16DispatchMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DispatchMsg()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY WIN32DISPATCHMSG( HAB, PQMSG );
PCHAR szWin32DispatchMsg = "Win32DispatchMsg";

MRESULT APIENTRY Trc_Win32DispatchMsg( HAB   Parm1
                                     , PQMSG Parm2
                                     )
{

   MRESULT ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_MSGL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DispatchMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPQMSG( Parm2 );
      }
      HOOKENTRY( szWin32DispatchMsg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32DISPATCHMSG( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSGL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DispatchMsg );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT( szWin32DispatchMsg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32DispatchMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DlgBox()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINDLGBOX( HWND, HWND, PFNWP16, HMODULE16, USHORT, PVOID16 );
PCHAR szWin16DlgBox = "Win16DlgBox";

USHORT APIENTRY16 Trc_Win16DlgBox( HWND      Parm1
                                 , HWND      Parm2
                                 , PFNWP16   Parm3
                                 , HMODULE16 Parm4
                                 , USHORT    Parm5
                                 , PVOID16   Parm6
                                 )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DlgBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PFNWP );
         T_LogPFN16( (PFN16)Parm3 );
         T_LogParameter( 4L, DT_HMODULE );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_PVOID );
         T_LogPVOID16( Parm6 );
         T_LogCHARBufferUnformatted16( Parm6 );
      }
      HOOKENTRY16( szWin16DlgBox, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINDLGBOX( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DlgBox );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC != DID_ERROR );
      HOOKEXIT16( szWin16DlgBox, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16DlgBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DlgBox()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32DLGBOX( HWND, HWND, PFNWP, HMODULE, ULONG, PVOID );
PCHAR szWin32DlgBox = "Win32DlgBox";

ULONG APIENTRY Trc_Win32DlgBox( HWND    Parm1
                              , HWND    Parm2
                              , PFNWP   Parm3
                              , HMODULE Parm4
                              , ULONG   Parm5
                              , PVOID   Parm6
                              )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DlgBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PFNWP );
         T_LogPFN( (PFN)Parm3 );
         T_LogParameter( 4L, DT_HMODULE );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PVOID );
         T_LogPVOID( Parm6 );
         T_LogCHARBufferUnformatted( Parm6 );
      }
      HOOKENTRY( szWin32DlgBox, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32DLGBOX( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DlgBox );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != DID_ERROR );
      HOOKEXIT( szWin32DlgBox, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32DlgBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DrawBitmap()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDRAWBITMAP( HPS, HBITMAP, PRECTL16, PPOINTL16, LONG, LONG, USHORT );
PCHAR szWin16DrawBitmap = "Win16DrawBitmap";

BOOL16 APIENTRY16 Trc_Win16DrawBitmap( HPS       Parm1
                                     , HBITMAP   Parm2
                                     , PRECTL16  Parm3
                                     , PPOINTL16 Parm4
                                     , LONG      Parm5
                                     , LONG      Parm6
                                     , USHORT    Parm7
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DrawBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPPOINTL16( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szWin16DrawBitmap, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDRAWBITMAP( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DrawBitmap );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DrawBitmap, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DrawBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DrawBitmap()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DRAWBITMAP( HPS, HBITMAP, PRECTL, PPOINTL, LONG, LONG, ULONG );
PCHAR szWin32DrawBitmap = "Win32DrawBitmap";

BOOL APIENTRY Trc_Win32DrawBitmap( HPS     Parm1
                                 , HBITMAP Parm2
                                 , PRECTL  Parm3
                                 , PPOINTL Parm4
                                 , LONG    Parm5
                                 , LONG    Parm6
                                 , ULONG   Parm7
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DrawBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HBITMAP );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
         T_LogParameter( 4L, DT_PPOINTL );
         T_LogPPOINTL( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szWin32DrawBitmap, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DRAWBITMAP( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DrawBitmap );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DrawBitmap, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DrawBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DrawBorder()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDRAWBORDER( HPS, PRECTL16, SHORT, SHORT, LONG, LONG, USHORT );
PCHAR szWin16DrawBorder = "Win16DrawBorder";

BOOL16 APIENTRY16 Trc_Win16DrawBorder( HPS      Parm1
                                     , PRECTL16 Parm2
                                     , SHORT    Parm3
                                     , SHORT    Parm4
                                     , LONG     Parm5
                                     , LONG     Parm6
                                     , USHORT   Parm7
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DrawBorder );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szWin16DrawBorder, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDRAWBORDER( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DrawBorder );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DrawBorder, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DrawBorder() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DrawBorder()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DRAWBORDER( HPS, PRECTL, LONG, LONG, LONG, LONG, ULONG );
PCHAR szWin32DrawBorder = "Win32DrawBorder";

BOOL APIENTRY Trc_Win32DrawBorder( HPS    Parm1
                                 , PRECTL Parm2
                                 , LONG   Parm3
                                 , LONG   Parm4
                                 , LONG   Parm5
                                 , LONG   Parm6
                                 , ULONG  Parm7
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DrawBorder );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szWin32DrawBorder, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DRAWBORDER( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DrawBorder );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DrawBorder, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DrawBorder() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DrawPointer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINDRAWPOINTER( HPS, SHORT, SHORT, HPOINTER, USHORT );
PCHAR szWin16DrawPointer = "Win16DrawPointer";

BOOL16 APIENTRY16 Trc_Win16DrawPointer( HPS      Parm1
                                      , SHORT    Parm2
                                      , SHORT    Parm3
                                      , HPOINTER Parm4
                                      , USHORT   Parm5
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DrawPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_HPOINTER );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szWin16DrawPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINDRAWPOINTER( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DrawPointer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16DrawPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16DrawPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DrawPointer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32DRAWPOINTER( HPS, LONG, LONG, HPOINTER, ULONG );
PCHAR szWin32DrawPointer = "Win32DrawPointer";

BOOL APIENTRY Trc_Win32DrawPointer( HPS      Parm1
                                  , LONG     Parm2
                                  , LONG     Parm3
                                  , HPOINTER Parm4
                                  , ULONG    Parm5
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DrawPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_HPOINTER );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szWin32DrawPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32DRAWPOINTER( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DrawPointer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32DrawPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32DrawPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16DrawText()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINDRAWTEXT( HPS, SHORT, PCH16, PRECTL16, LONG, LONG, USHORT );
PCHAR szWin16DrawText = "Win16DrawText";

SHORT APIENTRY16 Trc_Win16DrawText( HPS      Parm1
                                  , SHORT    Parm2
                                  , PCH16    Parm3
                                  , PRECTL16 Parm4
                                  , LONG     Parm5
                                  , LONG     Parm6
                                  , USHORT   Parm7
                                  )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16DrawText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID16( Parm3 );
         T_LogCHARBuffer16( Parm3, Parm2 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL16( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szWin16DrawText, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINDRAWTEXT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16DrawText );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PRECTL );
         if ( FLAGSET( Parm7, DT_QUERYEXTENT ) ) T_LogPRECTL16( Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szWin16DrawText, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16DrawText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32DrawText()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32DRAWTEXT( HPS, LONG, PCH, PRECTL, LONG, LONG, ULONG );
PCHAR szWin32DrawText = "Win32DrawText";

LONG APIENTRY Trc_Win32DrawText( HPS    Parm1
                               , LONG   Parm2
                               , PCH    Parm3
                               , PRECTL Parm4
                               , LONG   Parm5
                               , LONG   Parm6
                               , ULONG  Parm7
                               )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32DrawText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID( Parm3 );
         T_LogCHARBuffer( Parm3, Parm2 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szWin32DrawText, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32DRAWTEXT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32DrawText );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PRECTL );
         if ( FLAGSET( Parm7, DT_QUERYEXTENT ) ) T_LogPRECTL( Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szWin32DrawText, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32DrawText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EmptyClipbrd()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINEMPTYCLIPBRD( HAB );
PCHAR szWin16EmptyClipbrd = "Win16EmptyClipbrd";

BOOL16 APIENTRY16 Trc_Win16EmptyClipbrd( HAB Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EmptyClipbrd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16EmptyClipbrd, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINEMPTYCLIPBRD( Parm1 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EmptyClipbrd );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16EmptyClipbrd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16EmptyClipbrd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EmptyClipbrd()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32EMPTYCLIPBRD( HAB );
PCHAR szWin32EmptyClipbrd = "Win32EmptyClipbrd";

BOOL APIENTRY Trc_Win32EmptyClipbrd( HAB Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EmptyClipbrd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32EmptyClipbrd, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32EMPTYCLIPBRD( Parm1 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EmptyClipbrd );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32EmptyClipbrd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32EmptyClipbrd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EnablePhysInput()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINENABLEPHYSINPUT( HWND, BOOL16 );
PCHAR szWin16EnablePhysInput = "Win16EnablePhysInput";

BOOL16 APIENTRY16 Trc_Win16EnablePhysInput( HWND   Parm1
                                          , BOOL16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EnablePhysInput );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16EnablePhysInput, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINENABLEPHYSINPUT( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EnablePhysInput );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16EnablePhysInput, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16EnablePhysInput() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EnablePhysInput()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ENABLEPHYSINPUT( HWND, BOOL );
PCHAR szWin32EnablePhysInput = "Win32EnablePhysInput";

BOOL APIENTRY Trc_Win32EnablePhysInput( HWND Parm1
                                      , BOOL Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EnablePhysInput );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32EnablePhysInput, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ENABLEPHYSINPUT( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EnablePhysInput );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32EnablePhysInput, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32EnablePhysInput() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EnableWindow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINENABLEWINDOW( HWND, BOOL16 );
PCHAR szWin16EnableWindow = "Win16EnableWindow";

BOOL16 APIENTRY16 Trc_Win16EnableWindow( HWND   Parm1
                                       , BOOL16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EnableWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16EnableWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINENABLEWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EnableWindow );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16EnableWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16EnableWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EnableWindow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ENABLEWINDOW( HWND, BOOL );
PCHAR szWin32EnableWindow = "Win32EnableWindow";

BOOL APIENTRY Trc_Win32EnableWindow( HWND Parm1
                                   , BOOL Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EnableWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32EnableWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ENABLEWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EnableWindow );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32EnableWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32EnableWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EnableWindowUpdate()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINENABLEWINDOWUPDATE( HWND, BOOL16 );
PCHAR szWin16EnableWindowUpdate = "Win16EnableWindowUpdate";

BOOL16 APIENTRY16 Trc_Win16EnableWindowUpdate( HWND   Parm1
                                             , BOOL16 Parm2
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EnableWindowUpdate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16EnableWindowUpdate, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINENABLEWINDOWUPDATE( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EnableWindowUpdate );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16EnableWindowUpdate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16EnableWindowUpdate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EnableWindowUpdate()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ENABLEWINDOWUPDATE( HWND, BOOL );
PCHAR szWin32EnableWindowUpdate = "Win32EnableWindowUpdate";

BOOL APIENTRY Trc_Win32EnableWindowUpdate( HWND Parm1
                                         , BOOL Parm2
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EnableWindowUpdate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32EnableWindowUpdate, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ENABLEWINDOWUPDATE( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EnableWindowUpdate );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32EnableWindowUpdate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32EnableWindowUpdate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EndEnumWindows()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINENDENUMWINDOWS( HENUM );
PCHAR szWin16EndEnumWindows = "Win16EndEnumWindows";

BOOL16 APIENTRY16 Trc_Win16EndEnumWindows( HENUM Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EndEnumWindows );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HENUM );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16EndEnumWindows, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINENDENUMWINDOWS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EndEnumWindows );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16EndEnumWindows, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16EndEnumWindows() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EndEnumWindows()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ENDENUMWINDOWS( HENUM );
PCHAR szWin32EndEnumWindows = "Win32EndEnumWindows";

BOOL APIENTRY Trc_Win32EndEnumWindows( HENUM Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EndEnumWindows );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HENUM );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32EndEnumWindows, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ENDENUMWINDOWS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EndEnumWindows );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32EndEnumWindows, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32EndEnumWindows() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EndPaint()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINENDPAINT( HPS );
PCHAR szWin16EndPaint = "Win16EndPaint";

BOOL16 APIENTRY16 Trc_Win16EndPaint( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EndPaint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16EndPaint, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINENDPAINT( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EndPaint );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16EndPaint, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16EndPaint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EndPaint()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ENDPAINT( HPS );
PCHAR szWin32EndPaint = "Win32EndPaint";

BOOL APIENTRY Trc_Win32EndPaint( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EndPaint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32EndPaint, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ENDPAINT( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EndPaint );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32EndPaint, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32EndPaint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EnumClipbrdFmts()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINENUMCLIPBRDFMTS( HAB, USHORT );
PCHAR szWin16EnumClipbrdFmts = "Win16EnumClipbrdFmts";

USHORT APIENTRY16 Trc_Win16EnumClipbrdFmts( HAB    Parm1
                                          , USHORT Parm2
                                          )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EnumClipbrdFmts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16EnumClipbrdFmts, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINENUMCLIPBRDFMTS( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EnumClipbrdFmts );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, TRUE );
      HOOKEXIT16( szWin16EnumClipbrdFmts, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16EnumClipbrdFmts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EnumClipbrdFmts()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32ENUMCLIPBRDFMTS( HAB, ULONG );
PCHAR szWin32EnumClipbrdFmts = "Win32EnumClipbrdFmts";

ULONG APIENTRY Trc_Win32EnumClipbrdFmts( HAB   Parm1
                                       , ULONG Parm2
                                       )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EnumClipbrdFmts );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32EnumClipbrdFmts, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32ENUMCLIPBRDFMTS( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EnumClipbrdFmts );
      T_LogPMReturnCode( DT_ULONG, ulRC, TRUE );
      HOOKEXIT( szWin32EnumClipbrdFmts, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32EnumClipbrdFmts() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EnumDlgItem()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINENUMDLGITEM( HWND, HWND, USHORT, BOOL16 );
PCHAR szWin16EnumDlgItem = "Win16EnumDlgItem";

HWND APIENTRY16 Trc_Win16EnumDlgItem( HWND   Parm1
                                    , HWND   Parm2
                                    , USHORT Parm3
                                    , BOOL16 Parm4
                                    )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EnumDlgItem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_BOOL );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16EnumDlgItem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINENUMDLGITEM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EnumDlgItem );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16EnumDlgItem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16EnumDlgItem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EnumDlgItem()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32ENUMDLGITEM( HWND, HWND, ULONG );
PCHAR szWin32EnumDlgItem = "Win32EnumDlgItem";

HWND APIENTRY Trc_Win32EnumDlgItem( HWND  Parm1
                                  , HWND  Parm2
                                  , ULONG Parm3
                                  )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EnumDlgItem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32EnumDlgItem, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32ENUMDLGITEM( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EnumDlgItem );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32EnumDlgItem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32EnumDlgItem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16EqualRect()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINEQUALRECT( HAB, PRECTL16, PRECTL16 );
PCHAR szWin16EqualRect = "Win16EqualRect";

BOOL16 APIENTRY16 Trc_Win16EqualRect( HAB      Parm1
                                    , PRECTL16 Parm2
                                    , PRECTL16 Parm3
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16EqualRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
      }
      HOOKENTRY16( szWin16EqualRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINEQUALRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16EqualRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      HOOKEXIT16( szWin16EqualRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16EqualRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32EqualRect()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32EQUALRECT( HAB, PRECTL, PRECTL );
PCHAR szWin32EqualRect = "Win32EqualRect";

BOOL APIENTRY Trc_Win32EqualRect( HAB    Parm1
                                , PRECTL Parm2
                                , PRECTL Parm3
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32EqualRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
      }
      HOOKENTRY( szWin32EqualRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32EQUALRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32EqualRect );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      HOOKEXIT( szWin32EqualRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32EqualRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ExcludeUpdateRegion()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINEXCLUDEUPDATEREGION( HPS, HWND );
PCHAR szWin16ExcludeUpdateRegion = "Win16ExcludeUpdateRegion";

SHORT APIENTRY16 Trc_Win16ExcludeUpdateRegion( HPS  Parm1
                                             , HWND Parm2
                                             )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ExcludeUpdateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16ExcludeUpdateRegion, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINEXCLUDEUPDATEREGION( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ExcludeUpdateRegion );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC != RGN_ERROR );
      HOOKEXIT16( szWin16ExcludeUpdateRegion, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16ExcludeUpdateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ExcludeUpdateRegion()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32EXCLUDEUPDATEREGION( HPS, HWND );
PCHAR szWin32ExcludeUpdateRegion = "Win32ExcludeUpdateRegion";

LONG APIENTRY Trc_Win32ExcludeUpdateRegion( HPS  Parm1
                                          , HWND Parm2
                                          )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ExcludeUpdateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32ExcludeUpdateRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32EXCLUDEUPDATEREGION( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ExcludeUpdateRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT( szWin32ExcludeUpdateRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32ExcludeUpdateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16FillRect()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINFILLRECT( HPS, PRECTL16, LONG );
PCHAR szWin16FillRect = "Win16FillRect";

BOOL16 APIENTRY16 Trc_Win16FillRect( HPS      Parm1
                                   , PRECTL16 Parm2
                                   , LONG     Parm3
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16FillRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szWin16FillRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINFILLRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16FillRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16FillRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16FillRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FillRect()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32FILLRECT( HPS, PRECTL, LONG );
PCHAR szWin32FillRect = "Win32FillRect";

BOOL APIENTRY Trc_Win32FillRect( HPS    Parm1
                               , PRECTL Parm2
                               , LONG   Parm3
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32FillRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32FillRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32FILLRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32FillRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32FillRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32FillRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16FindAtom()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ATOM16 APIENTRY16 WINFINDATOM( HATOMTBL, PSZ16 );
PCHAR szWin16FindAtom = "Win16FindAtom";

ATOM16 APIENTRY16 Trc_Win16FindAtom( HATOMTBL Parm1
                                   , PSZ16    Parm2
                                   )
{

   ATOM16 usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16FindAtom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szWin16FindAtom, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINFINDATOM( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16FindAtom );
      T_LogPMReturnCodeUSHORT( DT_ATOM, usRC, usRC );
      HOOKEXIT16( szWin16FindAtom, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16FindAtom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FindAtom()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ATOM APIENTRY WIN32FINDATOM( HATOMTBL, PSZ );
PCHAR szWin32FindAtom = "Win32FindAtom";

ATOM APIENTRY Trc_Win32FindAtom( HATOMTBL Parm1
                               , PSZ      Parm2
                               )
{

   ATOM ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32FindAtom );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szWin32FindAtom, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32FINDATOM( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32FindAtom );
      T_LogPMReturnCode( DT_ATOM, ulRC, ulRC );
      HOOKEXIT( szWin32FindAtom, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32FindAtom() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16FlashWindow()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINFLASHWINDOW( HWND, BOOL16 );
PCHAR szWin16FlashWindow = "Win16FlashWindow";

BOOL16 APIENTRY16 Trc_Win16FlashWindow( HWND   Parm1
                                      , BOOL16 Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16FlashWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16FlashWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINFLASHWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16FlashWindow );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16FlashWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16FlashWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FlashWindow()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32FLASHWINDOW( HWND, BOOL );
PCHAR szWin32FlashWindow = "Win32FlashWindow";

BOOL APIENTRY Trc_Win32FlashWindow( HWND Parm1
                                  , BOOL Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32FlashWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32FlashWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32FLASHWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32FlashWindow );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32FlashWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32FlashWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16FocusChange()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINFOCUSCHANGE( HWND, HWND, USHORT );
PCHAR szWin16FocusChange = "Win16FocusChange";

BOOL16 APIENTRY16 Trc_Win16FocusChange( HWND   Parm1
                                      , HWND   Parm2
                                      , USHORT Parm3
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16FocusChange );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16FocusChange, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINFOCUSCHANGE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16FocusChange );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16FocusChange, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16FocusChange() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FocusChange()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32FOCUSCHANGE( HWND, HWND, ULONG );
PCHAR szWin32FocusChange = "Win32FocusChange";

BOOL APIENTRY Trc_Win32FocusChange( HWND  Parm1
                                  , HWND  Parm2
                                  , ULONG Parm3
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32FocusChange );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32FocusChange, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32FOCUSCHANGE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32FocusChange );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32FocusChange, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32FocusChange() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16FreeErrorInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINFREEERRORINFO( PERRINFO16 );
PCHAR szWin16FreeErrorInfo = "Win16FreeErrorInfo";

BOOL16 APIENTRY16 Trc_Win16FreeErrorInfo( PERRINFO16 Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16FreeErrorInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PERRINFO );
         T_LogPVOID16( Parm1 );
         T_LogPERRINFO16( Parm1 );
      }
      HOOKENTRY16( szWin16FreeErrorInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINFREEERRORINFO( Parm1 );

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16FreeErrorInfo );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      HOOKEXIT16( szWin16FreeErrorInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16FreeErrorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32FreeErrorInfo()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32FREEERRORINFO( PERRINFO );
PCHAR szWin32FreeErrorInfo = "Win32FreeErrorInfo";

BOOL APIENTRY Trc_Win32FreeErrorInfo( PERRINFO Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32FreeErrorInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PERRINFO );
         T_LogPVOID( Parm1 );
         T_LogPERRINFO( Parm1 );
      }
      HOOKENTRY( szWin32FreeErrorInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32FREEERRORINFO( Parm1 );

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32FreeErrorInfo );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      HOOKEXIT( szWin32FreeErrorInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32FreeErrorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16FreeMem()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
NPBYTE16 APIENTRY16 WINFREEMEM( HHEAP16, NPBYTE16, USHORT );
PCHAR szWin16FreeMem = "Win16FreeMem";

NPBYTE16 APIENTRY16 Trc_Win16FreeMem( HHEAP16  Parm1
                                    , NPBYTE16 Parm2
                                    , USHORT   Parm3
                                    )
{

   NPBYTE16 usRC;                                /* API return code           */

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16FreeMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HHEAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_NPBYTE );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16FreeMem, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINFREEMEM( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16FreeMem );
      T_LogPMReturnCodeUSHORT( DT_NPBYTE, usRC, !usRC );
      HOOKEXIT16( szWin16FreeMem, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16FreeMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetClipPS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPS APIENTRY16 WINGETCLIPPS( HWND, HWND, USHORT );
PCHAR szWin16GetClipPS = "Win16GetClipPS";

HPS APIENTRY16 Trc_Win16GetClipPS( HWND   Parm1
                                 , HWND   Parm2
                                 , USHORT Parm3
                                 )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetClipPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16GetClipPS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINGETCLIPPS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetClipPS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      HOOKEXIT16( szWin16GetClipPS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16GetClipPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetClipPS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPS APIENTRY WIN32GETCLIPPS( HWND, HWND, ULONG );
PCHAR szWin32GetClipPS = "Win32GetClipPS";

HPS APIENTRY Trc_Win32GetClipPS( HWND  Parm1
                               , HWND  Parm2
                               , ULONG Parm3
                               )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetClipPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32GetClipPS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32GETCLIPPS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetClipPS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      HOOKEXIT( szWin32GetClipPS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32GetClipPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetCurrentTime()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 WINGETCURRENTTIME( HAB );
PCHAR szWin16GetCurrentTime = "Win16GetCurrentTime";

ULONG APIENTRY16 Trc_Win16GetCurrentTime( HAB Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetCurrentTime );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16GetCurrentTime, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINGETCURRENTTIME( Parm1 );

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetCurrentTime );
      T_LogPMReturnCode( DT_ULONG, ulRC, TRUE );
      HOOKEXIT16( szWin16GetCurrentTime, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16GetCurrentTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetCurrentTime()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32GETCURRENTTIME( HAB );
PCHAR szWin32GetCurrentTime = "Win32GetCurrentTime";

ULONG APIENTRY Trc_Win32GetCurrentTime( HAB Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetCurrentTime );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32GetCurrentTime, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32GETCURRENTTIME( Parm1 );

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetCurrentTime );
      T_LogPMReturnCode( DT_ULONG, ulRC, TRUE );
      HOOKEXIT( szWin32GetCurrentTime, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32GetCurrentTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetDlgMsg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINGETDLGMSG( HWND, PQMSG16 );
PCHAR szWin16GetDlgMsg = "Win16GetDlgMsg";

BOOL16 APIENTRY16 Trc_Win16GetDlgMsg( HWND    Parm1
                                    , PQMSG16 Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetDlgMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16GetDlgMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINGETDLGMSG( Parm1, Parm2 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetDlgMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PQMSG );
         if ( bRC ) T_LogPQMSG16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16GetDlgMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16GetDlgMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetDlgMsg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32GETDLGMSG( HWND, PQMSG );
PCHAR szWin32GetDlgMsg = "Win32GetDlgMsg";

BOOL APIENTRY Trc_Win32GetDlgMsg( HWND  Parm1
                                , PQMSG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetDlgMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32GetDlgMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32GETDLGMSG( Parm1, Parm2 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetDlgMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PQMSG );
         if ( bRC ) T_LogPQMSG( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32GetDlgMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32GetDlgMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetErasePS()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPS APIENTRY16 WINGETERASEPS( HWND, HPS, PRECTL16 );
PCHAR szWin16GetErasePS = "Win16GetErasePS";

HPS APIENTRY16 Trc_Win16GetErasePS( HWND     Parm1
                                  , HPS      Parm2
                                  , PRECTL16 Parm3
                                  )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetErasePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
      }
      HOOKENTRY16( szWin16GetErasePS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINGETERASEPS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetErasePS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      HOOKEXIT16( szWin16GetErasePS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16GetErasePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetErasePS()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPS APIENTRY WIN32GETERASEPS( HWND, HPS, PRECTL );
PCHAR szWin32GetErasePS = "Win32GetErasePS";

HPS APIENTRY Trc_Win32GetErasePS( HWND   Parm1
                                , HPS    Parm2
                                , PRECTL Parm3
                                )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetErasePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
      }
      HOOKENTRY( szWin32GetErasePS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32GETERASEPS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetErasePS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      HOOKEXIT( szWin32GetErasePS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32GetErasePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetErrorInfo()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PERRINFO16 APIENTRY16 WINGETERRORINFO( HAB );
PCHAR szWin16GetErrorInfo = "Win16GetErrorInfo";

PERRINFO16 APIENTRY16 Trc_Win16GetErrorInfo( HAB Parm1 )
{

   PERRINFO16 pRC;                               /* API return code           */

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetErrorInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16GetErrorInfo, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINGETERRORINFO( Parm1 );

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetErrorInfo );
      T_LogPMReturnCodePtr16( DT_PERRINFO, pRC, TRUE );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) T_LogPERRINFO16( pRC );
      }
      HOOKEXIT16( szWin16GetErrorInfo, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16GetErrorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetErrorInfo()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PERRINFO APIENTRY WIN32GETERRORINFO( HAB );
PCHAR szWin32GetErrorInfo = "Win32GetErrorInfo";

PERRINFO APIENTRY Trc_Win32GetErrorInfo( HAB Parm1 )
{

   PERRINFO pRC;                                 /* API return code           */

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetErrorInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32GetErrorInfo, &Parm1 );
      T_FreeLog( );
   }

   pRC = WIN32GETERRORINFO( Parm1 );

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetErrorInfo );
      T_LogPMReturnCode( DT_PERRINFO, (ULONG)pRC, TRUE );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) T_LogPERRINFO( pRC );
      }
      HOOKEXIT( szWin32GetErrorInfo, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win32GetErrorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetKeyState()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINGETKEYSTATE( HWND, SHORT );
PCHAR szWin16GetKeyState = "Win16GetKeyState";

SHORT APIENTRY16 Trc_Win16GetKeyState( HWND  Parm1
                                     , SHORT Parm2
                                     )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetKeyState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16GetKeyState, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINGETKEYSTATE( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetKeyState );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      HOOKEXIT16( szWin16GetKeyState, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16GetKeyState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetKeyState()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32GETKEYSTATE( HWND, LONG );
PCHAR szWin32GetKeyState = "Win32GetKeyState";

LONG APIENTRY Trc_Win32GetKeyState( HWND Parm1
                                  , LONG Parm2
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetKeyState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32GetKeyState, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32GETKEYSTATE( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetKeyState );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      HOOKEXIT( szWin32GetKeyState, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32GetKeyState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetLastError()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ERRORID APIENTRY16 WINGETLASTERROR( HAB );
PCHAR szWin16GetLastError = "Win16GetLastError";

ERRORID APIENTRY16 Trc_Win16GetLastError( HAB Parm1 )
{

   ERRORID ulRC;                                 /* API return code           */
   ULONG   ulTID;                                /* Thread identifier         */

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetLastError );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16GetLastError, &Parm1 );
      T_FreeLog( );
   }

   if ( !( ulRC = WINGETLASTERROR( Parm1 ) ) )
   {
      if ( ( ulRC = T_PMErrorID[ ulTID = T_GetTID( ) ] ) != 0L )
      {
         T_PMErrorID[ ulTID ] = 0L;
      }
   }

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetLastError );
      T_LogPMReturnCode( DT_ERRORID, ulRC, TRUE );
      HOOKEXIT16( szWin16GetLastError, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16GetLastError() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetLastError()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ERRORID APIENTRY WIN32GETLASTERROR( HAB );
PCHAR szWin32GetLastError = "Win32GetLastError";

ERRORID APIENTRY Trc_Win32GetLastError( HAB Parm1 )
{

   ERRORID ulRC;                                 /* API return code           */
   ULONG   ulTID;                                /* Thread identifier         */

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetLastError );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32GetLastError, &Parm1 );
      T_FreeLog( );
   }

   if ( !( ulRC = WIN32GETLASTERROR( Parm1 ) ) )
   {
      if ( ( ulRC = T_PMErrorID[ ulTID = T_GetTID( ) ] ) != 0L )
      {
         T_PMErrorID[ ulTID ] = 0L;
      }
   }

   if ( LOGWINAPI( W_ERR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetLastError );
      T_LogPMReturnCode( DT_ERRORID, ulRC, TRUE );
      HOOKEXIT( szWin32GetLastError, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32GetLastError() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetMaxPosition()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINGETMAXPOSITION( HWND, PSWP16 );
PCHAR szWin16GetMaxPosition = "Win16GetMaxPosition";

BOOL16 APIENTRY16 Trc_Win16GetMaxPosition( HWND   Parm1
                                         , PSWP16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetMaxPosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16GetMaxPosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINGETMAXPOSITION( Parm1, Parm2 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetMaxPosition );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSWP );
         if ( bRC ) T_LogPSWP16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16GetMaxPosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16GetMaxPosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetMaxPosition()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32GETMAXPOSITION( HWND, PSWP );
PCHAR szWin32GetMaxPosition = "Win32GetMaxPosition";

BOOL APIENTRY Trc_Win32GetMaxPosition( HWND Parm1
                                     , PSWP Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetMaxPosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32GetMaxPosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32GETMAXPOSITION( Parm1, Parm2 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetMaxPosition );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSWP );
         if ( bRC ) T_LogPSWP( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32GetMaxPosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32GetMaxPosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetMinPosition()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINGETMINPOSITION( HWND, PSWP16, PPOINTL16 );
PCHAR szWin16GetMinPosition = "Win16GetMinPosition";

BOOL16 APIENTRY16 Trc_Win16GetMinPosition( HWND      Parm1
                                         , PSWP16    Parm2
                                         , PPOINTL16 Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetMinPosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL16( Parm3 );
      }
      HOOKENTRY16( szWin16GetMinPosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINGETMINPOSITION( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetMinPosition );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSWP );
         if ( bRC ) T_LogPSWP16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16GetMinPosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16GetMinPosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetMinPosition()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32GETMINPOSITION( HWND, PSWP, PPOINTL );
PCHAR szWin32GetMinPosition = "Win32GetMinPosition";

BOOL APIENTRY Trc_Win32GetMinPosition( HWND    Parm1
                                     , PSWP    Parm2
                                     , PPOINTL Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetMinPosition );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL( Parm3 );
      }
      HOOKENTRY( szWin32GetMinPosition, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32GETMINPOSITION( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetMinPosition );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSWP );
         if ( bRC ) T_LogPSWP( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32GetMinPosition, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32GetMinPosition() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetMsg()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINGETMSG( HAB, PQMSG16, HWND, USHORT, USHORT );
PCHAR szWin16GetMsg = "Win16GetMsg";

BOOL16 APIENTRY16 Trc_Win16GetMsg( HAB     Parm1
                                 , PQMSG16 Parm2
                                 , HWND    Parm3
                                 , USHORT  Parm4
                                 , USHORT  Parm5
                                 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSGL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_HWND );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szWin16GetMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINGETMSG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_MSGL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PQMSG );
         if ( bRC ) T_LogPQMSG16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16GetMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16GetMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetMsg()                                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32GETMSG( HAB, PQMSG, HWND, ULONG, ULONG );
PCHAR szWin32GetMsg = "Win32GetMsg";

BOOL APIENTRY Trc_Win32GetMsg( HAB   Parm1
                             , PQMSG Parm2
                             , HWND  Parm3
                             , ULONG Parm4
                             , ULONG Parm5
                             )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSGL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_HWND );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szWin32GetMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32GETMSG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_MSGL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PQMSG );
         if ( bRC ) T_LogPQMSG( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32GetMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32GetMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetNextWindow()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINGETNEXTWINDOW( HENUM );
PCHAR szWin16GetNextWindow = "Win16GetNextWindow";

HWND APIENTRY16 Trc_Win16GetNextWindow( HENUM Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetNextWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HENUM );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16GetNextWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINGETNEXTWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetNextWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16GetNextWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16GetNextWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetNextWindow()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32GETNEXTWINDOW( HENUM );
PCHAR szWin32GetNextWindow = "Win32GetNextWindow";

HWND APIENTRY Trc_Win32GetNextWindow( HENUM Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetNextWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HENUM );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32GetNextWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32GETNEXTWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetNextWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32GetNextWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32GetNextWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetPhysKeyState()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINGETPHYSKEYSTATE( HWND, SHORT );
PCHAR szWin16GetPhysKeyState = "Win16GetPhysKeyState";

SHORT APIENTRY16 Trc_Win16GetPhysKeyState( HWND  Parm1
                                         , SHORT Parm2
                                         )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetPhysKeyState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16GetPhysKeyState, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINGETPHYSKEYSTATE( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetPhysKeyState );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      HOOKEXIT16( szWin16GetPhysKeyState, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16GetPhysKeyState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetPhysKeyState()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32GETPHYSKEYSTATE( HWND, LONG );
PCHAR szWin32GetPhysKeyState = "Win32GetPhysKeyState";

LONG APIENTRY Trc_Win32GetPhysKeyState( HWND Parm1
                                      , LONG Parm2
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetPhysKeyState );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32GetPhysKeyState, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32GETPHYSKEYSTATE( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetPhysKeyState );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      HOOKEXIT( szWin32GetPhysKeyState, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32GetPhysKeyState() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetPS()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPS APIENTRY16 WINGETPS( HWND );
PCHAR szWin16GetPS = "Win16GetPS";

HPS APIENTRY16 Trc_Win16GetPS( HWND Parm1 )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16GetPS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINGETPS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetPS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      HOOKEXIT16( szWin16GetPS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16GetPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetPS()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPS APIENTRY WIN32GETPS( HWND );
PCHAR szWin32GetPS = "Win32GetPS";

HPS APIENTRY Trc_Win32GetPS( HWND Parm1 )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32GetPS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32GETPS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetPS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      HOOKEXIT( szWin32GetPS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32GetPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetScreenPS()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPS APIENTRY16 WINGETSCREENPS( HWND );
PCHAR szWin16GetScreenPS = "Win16GetScreenPS";

HPS APIENTRY16 Trc_Win16GetScreenPS( HWND Parm1 )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetScreenPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16GetScreenPS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINGETSCREENPS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetScreenPS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      HOOKEXIT16( szWin16GetScreenPS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16GetScreenPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetScreenPS()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPS APIENTRY WIN32GETSCREENPS( HWND );
PCHAR szWin32GetScreenPS = "Win32GetScreenPS";

HPS APIENTRY Trc_Win32GetScreenPS( HWND Parm1 )
{

   HPS ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetScreenPS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32GetScreenPS, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32GETSCREENPS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetScreenPS );
      T_LogPMReturnCode( DT_HPS, ulRC, ulRC );
      HOOKEXIT( szWin32GetScreenPS, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32GetScreenPS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16GetSysBitmap()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HBITMAP APIENTRY16 WINGETSYSBITMAP( HWND, USHORT );
PCHAR szWin16GetSysBitmap = "Win16GetSysBitmap";

HBITMAP APIENTRY16 Trc_Win16GetSysBitmap( HWND   Parm1
                                        , USHORT Parm2
                                        )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16GetSysBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16GetSysBitmap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINGETSYSBITMAP( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16GetSysBitmap );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC );
      HOOKEXIT16( szWin16GetSysBitmap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16GetSysBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32GetSysBitmap()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HBITMAP APIENTRY WIN32GETSYSBITMAP( HWND, ULONG );
PCHAR szWin32GetSysBitmap = "Win32GetSysBitmap";

HBITMAP APIENTRY Trc_Win32GetSysBitmap( HWND  Parm1
                                      , ULONG Parm2
                                      )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32GetSysBitmap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32GetSysBitmap, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32GETSYSBITMAP( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32GetSysBitmap );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC );
      HOOKEXIT( szWin32GetSysBitmap, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32GetSysBitmap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16InflateRect()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WININFLATERECT( HAB, PRECTL16, SHORT, SHORT );
PCHAR szWin16InflateRect = "Win16InflateRect";

BOOL16 APIENTRY16 Trc_Win16InflateRect( HAB      Parm1
                                      , PRECTL16 Parm2
                                      , SHORT    Parm3
                                      , SHORT    Parm4
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16InflateRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16InflateRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WININFLATERECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16InflateRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16InflateRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16InflateRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32InflateRect()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32INFLATERECT( HAB, PRECTL, LONG, LONG );
PCHAR szWin32InflateRect = "Win32InflateRect";

BOOL APIENTRY Trc_Win32InflateRect( HAB    Parm1
                                  , PRECTL Parm2
                                  , LONG   Parm3
                                  , LONG   Parm4
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32InflateRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32InflateRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32INFLATERECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32InflateRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32InflateRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32InflateRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16Initialize()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HAB APIENTRY16 WININITIALIZE( USHORT );
PCHAR szWin16Initialize = "Win16Initialize";

HAB APIENTRY16 Trc_Win16Initialize( USHORT Parm1 )
{

   HAB ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16Initialize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_USHORT );
         T_LogUSHORT( Parm1 );
      }
      HOOKENTRY16( szWin16Initialize, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WININITIALIZE( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16Initialize );
      T_LogPMReturnCode( DT_HAB, ulRC, ulRC );
      HOOKEXIT16( szWin16Initialize, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16Initialize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32Initialize()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HAB APIENTRY WIN32INITIALIZE( ULONG );
PCHAR szWin32Initialize = "Win32Initialize";

HAB APIENTRY Trc_Win32Initialize( ULONG Parm1 )
{

   HAB ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32Initialize );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_ULONG );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32Initialize, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32INITIALIZE( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32Initialize );
      T_LogPMReturnCode( DT_HAB, ulRC, ulRC );
      HOOKEXIT( szWin32Initialize, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32Initialize() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16InSendMsg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WININSENDMSG( HAB );
PCHAR szWin16InSendMsg = "Win16InSendMsg";

BOOL16 APIENTRY16 Trc_Win16InSendMsg( HAB Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16InSendMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16InSendMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WININSENDMSG( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16InSendMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      HOOKEXIT16( szWin16InSendMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16InSendMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32InSendMsg()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32INSENDMSG( HAB );
PCHAR szWin32InSendMsg = "Win32InSendMsg";

BOOL APIENTRY Trc_Win32InSendMsg( HAB Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32InSendMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32InSendMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32INSENDMSG( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32InSendMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      HOOKEXIT( szWin32InSendMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32InSendMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IntersectRect()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WININTERSECTRECT( HAB, PRECTL16, PRECTL16, PRECTL16 );
PCHAR szWin16IntersectRect = "Win16IntersectRect";

BOOL16 APIENTRY16 Trc_Win16IntersectRect( HAB      Parm1
                                        , PRECTL16 Parm2
                                        , PRECTL16 Parm3
                                        , PRECTL16 Parm4
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IntersectRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL16( Parm4 );
      }
      HOOKENTRY16( szWin16IntersectRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WININTERSECTRECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IntersectRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKEXIT16( szWin16IntersectRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IntersectRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IntersectRect()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32INTERSECTRECT( HAB, PRECTL, PRECTL, PRECTL );
PCHAR szWin32IntersectRect = "Win32IntersectRect";

BOOL APIENTRY Trc_Win32IntersectRect( HAB    Parm1
                                    , PRECTL Parm2
                                    , PRECTL Parm3
                                    , PRECTL Parm4
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IntersectRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL( Parm4 );
      }
      HOOKENTRY( szWin32IntersectRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32INTERSECTRECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IntersectRect );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKEXIT( szWin32IntersectRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IntersectRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16InvalidateRect()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WININVALIDATERECT( HWND, PRECTL16, BOOL16 );
PCHAR szWin16InvalidateRect = "Win16InvalidateRect";

BOOL16 APIENTRY16 Trc_Win16InvalidateRect( HWND     Parm1
                                         , PRECTL16 Parm2
                                         , BOOL16   Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16InvalidateRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16InvalidateRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WININVALIDATERECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16InvalidateRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16InvalidateRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16InvalidateRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32InvalidateRect()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32INVALIDATERECT( HWND, PRECTL, BOOL );
PCHAR szWin32InvalidateRect = "Win32InvalidateRect";

BOOL APIENTRY Trc_Win32InvalidateRect( HWND   Parm1
                                     , PRECTL Parm2
                                     , BOOL   Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32InvalidateRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32InvalidateRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32INVALIDATERECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32InvalidateRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32InvalidateRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32InvalidateRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16InvalidateRegion()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WININVALIDATEREGION( HWND, HRGN, BOOL16 );
PCHAR szWin16InvalidateRegion = "Win16InvalidateRegion";

BOOL16 APIENTRY16 Trc_Win16InvalidateRegion( HWND   Parm1
                                           , HRGN   Parm2
                                           , BOOL16 Parm3
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16InvalidateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16InvalidateRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = WININVALIDATEREGION( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16InvalidateRegion );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16InvalidateRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16InvalidateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32InvalidateRegion()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32INVALIDATEREGION( HWND, HRGN, BOOL );
PCHAR szWin32InvalidateRegion = "Win32InvalidateRegion";

BOOL APIENTRY Trc_Win32InvalidateRegion( HWND Parm1
                                       , HRGN Parm2
                                       , BOOL Parm3
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32InvalidateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32InvalidateRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32INVALIDATEREGION( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32InvalidateRegion );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32InvalidateRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32InvalidateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16InvertRect()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WININVERTRECT( HPS, PRECTL16 );
PCHAR szWin16InvertRect = "Win16InvertRect";

BOOL16 APIENTRY16 Trc_Win16InvertRect( HPS    Parm1
                                     , PRECTL16 Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16InvertRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szWin16InvertRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WININVERTRECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16InvertRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16InvertRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16InvertRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32InvertRect()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32INVERTRECT( HPS, PRECTL );
PCHAR szWin32InvertRect = "Win32InvertRect";

BOOL APIENTRY Trc_Win32InvertRect( HPS    Parm1
                                 , PRECTL Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32InvertRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szWin32InvertRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32INVERTRECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32InvertRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32InvertRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32InvertRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IsChild()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINISCHILD( HWND, HWND );
PCHAR szWin16IsChild = "Win16IsChild";

BOOL16 APIENTRY16 Trc_Win16IsChild( HWND Parm1
                                  , HWND Parm2
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IsChild );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16IsChild, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINISCHILD( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IsChild );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16IsChild, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IsChild() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsChild()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ISCHILD( HWND, HWND );
PCHAR szWin32IsChild = "Win32IsChild";

BOOL APIENTRY Trc_Win32IsChild( HWND Parm1
                              , HWND Parm2
                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IsChild );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32IsChild, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ISCHILD( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IsChild );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32IsChild, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IsChild() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IsPhysInputEnabled()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINISPHYSINPUTENABLED( HWND );
PCHAR szWin16IsPhysInputEnabled = "Win16IsPhysInputEnabled";

BOOL16 APIENTRY16 Trc_Win16IsPhysInputEnabled( HWND Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IsPhysInputEnabled );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16IsPhysInputEnabled, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINISPHYSINPUTENABLED( Parm1 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IsPhysInputEnabled );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      HOOKEXIT16( szWin16IsPhysInputEnabled, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IsPhysInputEnabled() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsPhysInputEnabled()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ISPHYSINPUTENABLED( HWND );
PCHAR szWin32IsPhysInputEnabled = "Win32IsPhysInputEnabled";

BOOL APIENTRY Trc_Win32IsPhysInputEnabled( HWND Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IsPhysInputEnabled );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32IsPhysInputEnabled, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ISPHYSINPUTENABLED( Parm1 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IsPhysInputEnabled );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      HOOKEXIT( szWin32IsPhysInputEnabled, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IsPhysInputEnabled() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IsRectEmpty()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINISRECTEMPTY( HAB, PRECTL16 );
PCHAR szWin16IsRectEmpty = "Win16IsRectEmpty";

BOOL16 APIENTRY16 Trc_Win16IsRectEmpty( HAB      Parm1
                                      , PRECTL16 Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IsRectEmpty );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szWin16IsRectEmpty, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINISRECTEMPTY( Parm1, Parm2 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IsRectEmpty );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      HOOKEXIT16( szWin16IsRectEmpty, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IsRectEmpty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsRectEmpty()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ISRECTEMPTY( HAB, PRECTL );
PCHAR szWin32IsRectEmpty = "Win32IsRectEmpty";

BOOL APIENTRY Trc_Win32IsRectEmpty( HAB    Parm1
                                  , PRECTL Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IsRectEmpty );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szWin32IsRectEmpty, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ISRECTEMPTY( Parm1, Parm2 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IsRectEmpty );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      HOOKEXIT( szWin32IsRectEmpty, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IsRectEmpty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IsThreadActive()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINISTHREADACTIVE( HAB );
PCHAR szWin16IsThreadActive = "Win16IsThreadActive";

BOOL16 APIENTRY16 Trc_Win16IsThreadActive( HAB Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IsThreadActive );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16IsThreadActive, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINISTHREADACTIVE( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IsThreadActive );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      HOOKEXIT16( szWin16IsThreadActive, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IsThreadActive() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsThreadActive()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ISTHREADACTIVE( HAB );
PCHAR szWin32IsThreadActive = "Win32IsThreadActive";

BOOL APIENTRY Trc_Win32IsThreadActive( HAB Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IsThreadActive );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32IsThreadActive, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ISTHREADACTIVE( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IsThreadActive );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      HOOKEXIT( szWin32IsThreadActive, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IsThreadActive() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IsWindow()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINISWINDOW( HAB, HWND );
PCHAR szWin16IsWindow = "Win16IsWindow";

BOOL16 APIENTRY16 Trc_Win16IsWindow( HAB  Parm1
                                   , HWND Parm2
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IsWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16IsWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINISWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IsWindow );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16IsWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IsWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsWindow()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ISWINDOW( HAB, HWND );
PCHAR szWin32IsWindow = "Win32IsWindow";

BOOL APIENTRY Trc_Win32IsWindow( HAB  Parm1
                               , HWND Parm2
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IsWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32IsWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ISWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IsWindow );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32IsWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IsWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IsWindowEnabled()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINISWINDOWENABLED( HWND );
PCHAR szWin16IsWindowEnabled = "Win16IsWindowEnabled";

BOOL16 APIENTRY16 Trc_Win16IsWindowEnabled( HWND Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IsWindowEnabled );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16IsWindowEnabled, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINISWINDOWENABLED( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IsWindowEnabled );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16IsWindowEnabled, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IsWindowEnabled() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsWindowEnabled()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ISWINDOWENABLED( HWND );
PCHAR szWin32IsWindowEnabled = "Win32IsWindowEnabled";

BOOL APIENTRY Trc_Win32IsWindowEnabled( HWND Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IsWindowEnabled );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32IsWindowEnabled, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ISWINDOWENABLED( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IsWindowEnabled );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32IsWindowEnabled, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IsWindowEnabled() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IsWindowShowing()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINISWINDOWSHOWING( HWND );
PCHAR szWin16IsWindowShowing = "Win16IsWindowShowing";

BOOL16 APIENTRY16 Trc_Win16IsWindowShowing( HWND Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IsWindowShowing );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16IsWindowShowing, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINISWINDOWSHOWING( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IsWindowShowing );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16IsWindowShowing, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IsWindowShowing() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsWindowShowing()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ISWINDOWSHOWING( HWND );
PCHAR szWin32IsWindowShowing = "Win32IsWindowShowing";

BOOL APIENTRY Trc_Win32IsWindowShowing( HWND Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IsWindowShowing );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32IsWindowShowing, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ISWINDOWSHOWING( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IsWindowShowing );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32IsWindowShowing, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IsWindowShowing() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16IsWindowVisible()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINISWINDOWVISIBLE( HWND );
PCHAR szWin16IsWindowVisible = "Win16IsWindowVisible";

BOOL16 APIENTRY16 Trc_Win16IsWindowVisible( HWND Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16IsWindowVisible );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16IsWindowVisible, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINISWINDOWVISIBLE( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16IsWindowVisible );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16IsWindowVisible, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16IsWindowVisible() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32IsWindowVisible()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32ISWINDOWVISIBLE( HWND );
PCHAR szWin32IsWindowVisible = "Win32IsWindowVisible";

BOOL APIENTRY Trc_Win32IsWindowVisible( HWND Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32IsWindowVisible );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32IsWindowVisible, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32ISWINDOWVISIBLE( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32IsWindowVisible );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32IsWindowVisible, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32IsWindowVisible() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadAccelTable()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HACCEL APIENTRY16 WINLOADACCELTABLE( HAB, HMODULE16, USHORT );
PCHAR szWin16LoadAccelTable = "Win16LoadAccelTable";

HACCEL APIENTRY16 Trc_Win16LoadAccelTable( HAB       Parm1
                                         , HMODULE16 Parm2
                                         , USHORT    Parm3
                                         )
{

   HACCEL ulRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LoadAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16LoadAccelTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINLOADACCELTABLE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LoadAccelTable );
      T_LogPMReturnCode( DT_HACCEL, ulRC, ulRC );
      HOOKEXIT16( szWin16LoadAccelTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16LoadAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadAccelTable()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HACCEL APIENTRY WIN32LOADACCELTABLE( HAB, HMODULE, ULONG );
PCHAR szWin32LoadAccelTable = "Win32LoadAccelTable";

HACCEL APIENTRY Trc_Win32LoadAccelTable( HAB     Parm1
                                       , HMODULE Parm2
                                       , ULONG   Parm3
                                       )
{

   HACCEL ulRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LoadAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32LoadAccelTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32LOADACCELTABLE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LoadAccelTable );
      T_LogPMReturnCode( DT_HACCEL, ulRC, ulRC );
      HOOKEXIT( szWin32LoadAccelTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32LoadAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadDlg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINLOADDLG( HWND, HWND, PFNWP16, HMODULE16, USHORT, PVOID16 );
PCHAR szWin16LoadDlg = "Win16LoadDlg";

HWND APIENTRY16 Trc_Win16LoadDlg( HWND      Parm1
                                , HWND      Parm2
                                , PFNWP16   Parm3
                                , HMODULE16 Parm4
                                , USHORT    Parm5
                                , PVOID16   Parm6
                                )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LoadDlg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PFNWP );
         T_LogPFN16( (PFN16)Parm3 );
         T_LogParameter( 4L, DT_HMODULE );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_PVOID );
         T_LogPVOID16( Parm6 );
         T_LogCHARBufferUnformatted16( Parm6 );
      }
      HOOKENTRY16( szWin16LoadDlg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINLOADDLG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LoadDlg );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16LoadDlg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16LoadDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadDlg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32LOADDLG( HWND, HWND, PFNWP, HMODULE, ULONG, PVOID );
PCHAR szWin32LoadDlg = "Win32LoadDlg";

HWND APIENTRY Trc_Win32LoadDlg( HWND    Parm1
                              , HWND    Parm2
                              , PFNWP   Parm3
                              , HMODULE Parm4
                              , ULONG   Parm5
                              , PVOID   Parm6
                              )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LoadDlg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PFNWP );
         T_LogPFN( (PFN)Parm3 );
         T_LogParameter( 4L, DT_HMODULE );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PVOID );
         T_LogPVOID( Parm6 );
         T_LogCHARBufferUnformatted( Parm6 );
      }
      HOOKENTRY( szWin32LoadDlg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32LOADDLG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LoadDlg );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32LoadDlg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32LoadDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadLibrary()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HLIB16 APIENTRY16 WINLOADLIBRARY( HAB, PSZ16 );
PCHAR szWin16LoadLibrary = "Win16LoadLibrary";

HLIB16 APIENTRY16 Trc_Win16LoadLibrary( HAB   Parm1
                                      , PSZ16 Parm2
                                      )
{

   HLIB16 usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LoadLibrary );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szWin16LoadLibrary, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINLOADLIBRARY( Parm1, Parm2 );

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LoadLibrary );
      T_LogPMReturnCodeUSHORT( DT_HLIB, usRC, usRC );
      HOOKEXIT16( szWin16LoadLibrary, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16LoadLibrary() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadLibrary()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HLIB APIENTRY WIN32LOADLIBRARY( HAB, PSZ );
PCHAR szWin32LoadLibrary = "Win32LoadLibrary";

HLIB APIENTRY Trc_Win32LoadLibrary( HAB Parm1
                                  , PSZ Parm2
                                  )
{

   HLIB ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LoadLibrary );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szWin32LoadLibrary, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32LOADLIBRARY( Parm1, Parm2 );

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LoadLibrary );
      T_LogPMReturnCode( DT_HLIB, ulRC, ulRC );
      HOOKEXIT( szWin32LoadLibrary, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32LoadLibrary() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadMenu()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINLOADMENU( HWND, HMODULE16, USHORT );
PCHAR szWin16LoadMenu = "Win16LoadMenu";

HWND APIENTRY16 Trc_Win16LoadMenu( HWND      Parm1
                                 , HMODULE16 Parm2
                                 , USHORT    Parm3
                                 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LoadMenu );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16LoadMenu, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINLOADMENU( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LoadMenu );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16LoadMenu, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16LoadMenu() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadMenu()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32LOADMENU( HWND, HMODULE, ULONG );
PCHAR szWin32LoadMenu = "Win32LoadMenu";

HWND APIENTRY Trc_Win32LoadMenu( HWND    Parm1
                               , HMODULE Parm2
                               , ULONG   Parm3
                               )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LoadMenu );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32LoadMenu, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32LOADMENU( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LoadMenu );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32LoadMenu, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32LoadMenu() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadMessage()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINLOADMESSAGE( HAB, HMODULE16, USHORT, SHORT, PSZ16 );
PCHAR szWin16LoadMessage = "Win16LoadMessage";

SHORT APIENTRY16 Trc_Win16LoadMessage( HAB       Parm1
                                     , HMODULE16 Parm2
                                     , USHORT    Parm3
                                     , SHORT     Parm4
                                     , PSZ16     Parm5
                                     )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LoadMessage );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szWin16LoadMessage, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINLOADMESSAGE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LoadMessage );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PSZ );
         if ( sRC ) T_LogPSZ16( Parm5 );
         else T_LogPVOID16( Parm5 );
      }
      HOOKEXIT16( szWin16LoadMessage, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16LoadMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadMessage()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32LOADMESSAGE( HAB, HMODULE, ULONG, LONG, PSZ );
PCHAR szWin32LoadMessage = "Win32LoadMessage";

LONG APIENTRY Trc_Win32LoadMessage( HAB     Parm1
                                  , HMODULE Parm2
                                  , ULONG   Parm3
                                  , LONG    Parm4
                                  , PSZ     Parm5
                                  )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LoadMessage );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szWin32LoadMessage, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32LOADMESSAGE( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LoadMessage );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PSZ );
         if ( lRC ) T_LogPSZ( Parm5 );
         else T_LogPVOID( Parm5 );
      }
      HOOKEXIT( szWin32LoadMessage, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32LoadMessage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadPointer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPOINTER APIENTRY16 WINLOADPOINTER( HWND, HMODULE16, USHORT );
PCHAR szWin16LoadPointer = "Win16LoadPointer";

HPOINTER APIENTRY16 Trc_Win16LoadPointer( HWND      Parm1
                                        , HMODULE16 Parm2
                                        , USHORT    Parm3
                                        )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LoadPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16LoadPointer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINLOADPOINTER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LoadPointer );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT16( szWin16LoadPointer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16LoadPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadPointer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPOINTER APIENTRY WIN32LOADPOINTER( HWND, HMODULE, ULONG );
PCHAR szWin32LoadPointer = "Win32LoadPointer";

HPOINTER APIENTRY Trc_Win32LoadPointer( HWND    Parm1
                                      , HMODULE Parm2
                                      , ULONG   Parm3
                                      )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LoadPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32LoadPointer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32LOADPOINTER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LoadPointer );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT( szWin32LoadPointer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32LoadPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadProcedure()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PFNWP16 APIENTRY16 WINLOADPROCEDURE( HAB, HLIB16, PSZ16 );
PCHAR szWin16LoadProcedure = "Win16LoadProcedure";

PFNWP16 APIENTRY16 Trc_Win16LoadProcedure( HAB    Parm1
                                         , HLIB16 Parm2
                                         , PSZ16  Parm3
                                         )
{

   PFNWP16 pRC;                                  /* API return code           */

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LoadProcedure );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HLIB );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
      }
      HOOKENTRY16( szWin16LoadProcedure, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINLOADPROCEDURE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LoadProcedure );
      T_LogPMReturnCodePtr16( DT_PFNWP, (PVOID16)pRC, (BOOL)pRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPFN16( (PFN16)pRC ); }
      }
      HOOKEXIT16( szWin16LoadProcedure, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16LoadProcedure() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadProcedure()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PFNWP APIENTRY WIN32LOADPROCEDURE( HAB, HLIB, PSZ );
PCHAR szWin32LoadProcedure = "Win32LoadProcedure";

PFNWP APIENTRY Trc_Win32LoadProcedure( HAB  Parm1
                                     , HLIB Parm2
                                     , PSZ  Parm3
                                     )
{

   PFNWP pRC;                                    /* API return code           */

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LoadProcedure );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HLIB );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
      }
      HOOKENTRY( szWin32LoadProcedure, &Parm1 );
      T_FreeLog( );
   }

   pRC = WIN32LOADPROCEDURE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_LOAD ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LoadProcedure );
      T_LogPMReturnCode( DT_PFNWP, (ULONG)pRC, (BOOL)pRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPFN( (PFN)pRC ); }
      }
      HOOKEXIT( szWin32LoadProcedure, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win32LoadProcedure() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LoadString()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINLOADSTRING( HAB, HMODULE16, USHORT, SHORT, PSZ16 );
PCHAR szWin16LoadString = "Win16LoadString";

SHORT APIENTRY16 Trc_Win16LoadString( HAB       Parm1
                                    , HMODULE16 Parm2
                                    , USHORT    Parm3
                                    , SHORT     Parm4
                                    , PSZ16     Parm5
                                    )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LoadString );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPVOID16( Parm5 );
      }
      HOOKENTRY16( szWin16LoadString, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINLOADSTRING( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LoadString );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PSZ );
         if ( sRC ) T_LogPSZ16( Parm5 );
         else T_LogPVOID16( Parm5 );
      }
      HOOKEXIT16( szWin16LoadString, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16LoadString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LoadString()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32LOADSTRING( HAB, HMODULE, ULONG, LONG, PSZ );
PCHAR szWin32LoadString = "Win32LoadString";

LONG APIENTRY Trc_Win32LoadString( HAB     Parm1
                                 , HMODULE Parm2
                                 , ULONG   Parm3
                                 , LONG    Parm4
                                 , PSZ     Parm5
                                 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LoadString );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMODULE );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPVOID( Parm5 );
      }
      HOOKENTRY( szWin32LoadString, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32LOADSTRING( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LoadString );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 5L, DT_PSZ );
         if ( lRC ) T_LogPSZ( Parm5 );
         else T_LogPVOID( Parm5 );
      }
      HOOKEXIT( szWin32LoadString, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32LoadString() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LockHeap()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PVOID16 APIENTRY16 WINLOCKHEAP( HHEAP16 );
PCHAR szWin16LockHeap = "Win16LockHeap";

PVOID16 APIENTRY16 Trc_Win16LockHeap( HHEAP16 Parm1 )
{

   PVOID16 pRC;                                  /* API return code           */

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LockHeap );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HHEAP );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16LockHeap, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINLOCKHEAP( Parm1 );

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LockHeap );
      T_LogPMReturnCodePtr16( DT_PVOID, pRC, (BOOL)pRC );
      HOOKEXIT16( szWin16LockHeap, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16LockHeap() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LockInput()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINLOCKINPUT( HMQ, USHORT );
PCHAR szWin16LockInput = "Win16LockInput";

BOOL16 APIENTRY16 Trc_Win16LockInput( HMQ    Parm1
                                    , USHORT Parm2
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LockInput );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16LockInput, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINLOCKINPUT( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LockInput );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16LockInput, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16LockInput() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LockInput()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32LOCKINPUT( HMQ, ULONG );
PCHAR szWin32LockInput = "Win32LockInput";

BOOL APIENTRY Trc_Win32LockInput( HMQ   Parm1
                                , ULONG Parm2
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LockInput );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32LockInput, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32LOCKINPUT( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LockInput );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32LockInput, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32LockInput() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LockPointerUpdate()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 210 ) )
BOOL16 APIENTRY16 WINLOCKPOINTERUPDATE( HWND, HPOINTER, USHORT );
PCHAR szWin16LockPointerUpdate = "Win16LockPointerUpdate";

BOOL16 APIENTRY16 Trc_Win16LockPointerUpdate( HWND     Parm1
                                            , HPOINTER Parm2
                                            , USHORT   Parm3
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LockPointerUpdate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPOINTER );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16LockPointerUpdate, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnWINLOCKPOINTERUPDATE )( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LockPointerUpdate );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16LockPointerUpdate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16LockPointerUpdate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LockPointerUpdate()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
BOOL APIENTRY WIN32LOCKPOINTERUPDATE( HWND, HPOINTER, ULONG );
PCHAR szWin32LockPointerUpdate = "Win32LockPointerUpdate";

BOOL APIENTRY Trc_Win32LockPointerUpdate( HWND     Parm1
                                        , HPOINTER Parm2
                                        , ULONG    Parm3
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LockPointerUpdate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPOINTER );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32LockPointerUpdate, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnWIN32LOCKPOINTERUPDATE )( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LockPointerUpdate );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32LockPointerUpdate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32LockPointerUpdate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LockVisRegions()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINLOCKVISREGIONS( HWND, BOOL16 );
PCHAR szWin16LockVisRegions = "Win16LockVisRegions";

BOOL16 APIENTRY16 Trc_Win16LockVisRegions( HWND   Parm1
                                         , BOOL16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LockVisRegions );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16LockVisRegions, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINLOCKVISREGIONS( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LockVisRegions );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16LockVisRegions, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16LockVisRegions() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LockVisRegions()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32LOCKVISREGIONS( HWND, BOOL );
PCHAR szWin32LockVisRegions = "Win32LockVisRegions";

BOOL APIENTRY Trc_Win32LockVisRegions( HWND Parm1
                                     , BOOL Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LockVisRegions );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32LockVisRegions, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32LOCKVISREGIONS( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LockVisRegions );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32LockVisRegions, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32LockVisRegions() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LockWindow()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINLOCKWINDOW( HWND, BOOL16 );
PCHAR szWin16LockWindow = "Win16LockWindow";

HWND APIENTRY16 Trc_Win16LockWindow( HWND   Parm1
                                   , BOOL16 Parm2
                                   )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LockWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16LockWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINLOCKWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LockWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16LockWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16LockWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16LockWindowUpdate()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINLOCKWINDOWUPDATE( HWND, HWND );
PCHAR szWin16LockWindowUpdate = "Win16LockWindowUpdate";

BOOL16 APIENTRY16 Trc_Win16LockWindowUpdate( HWND Parm1
                                           , HWND Parm2
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16LockWindowUpdate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16LockWindowUpdate, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINLOCKWINDOWUPDATE( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16LockWindowUpdate );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16LockWindowUpdate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16LockWindowUpdate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32LockWindowUpdate()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32LOCKWINDOWUPDATE( HWND, HWND );
PCHAR szWin32LockWindowUpdate = "Win32LockWindowUpdate";

BOOL APIENTRY Trc_Win32LockWindowUpdate( HWND Parm1
                                       , HWND Parm2
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32LockWindowUpdate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32LockWindowUpdate, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32LOCKWINDOWUPDATE( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32LockWindowUpdate );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32LockWindowUpdate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32LockWindowUpdate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16MakePoints()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINMAKEPOINTS( HAB, PWPOINT16, USHORT );
PCHAR szWin16MakePoints = "Win16MakePoints";

BOOL16 APIENTRY16 Trc_Win16MakePoints( HAB       Parm1
                                     , PWPOINT16 Parm2
                                     , USHORT    Parm3
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16MakePoints );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PWPOINT );
         T_LogWPOINTBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16MakePoints, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINMAKEPOINTS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16MakePoints );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PWPOINT );
         if ( bRC ) T_LogWPOINTBuffer( Parm2, Parm3 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16MakePoints, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16MakePoints() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MakePoints()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32MAKEPOINTS( HAB, PPOINTL, ULONG );
PCHAR szWin32MakePoints = "Win32MakePoints";

BOOL APIENTRY Trc_Win32MakePoints( HAB     Parm1
                                 , PPOINTL Parm2
                                 , ULONG   Parm3
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MakePoints );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32MakePoints, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32MAKEPOINTS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32MakePoints );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer( Parm2, Parm3 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32MakePoints, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32MakePoints() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16MakeRect()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINMAKERECT( HAB, PRECTL16 );
PCHAR szWin16MakeRect = "Win16MakeRect";

BOOL16 APIENTRY16 Trc_Win16MakeRect( HAB      Parm1
                                   , PRECTL16 Parm2
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16MakeRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szWin16MakeRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINMAKERECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16MakeRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16MakeRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16MakeRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MakeRect()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32MAKERECT( HAB, PRECTL );
PCHAR szWin32MakeRect = "Win32MakeRect";

BOOL APIENTRY Trc_Win32MakeRect( HAB    Parm1
                               , PRECTL Parm2
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MakeRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szWin32MakeRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32MAKERECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32MakeRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32MakeRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32MakeRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16MapDlgPoints()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINMAPDLGPOINTS( HWND, PPOINTL16, USHORT, BOOL16 );
PCHAR szWin16MapDlgPoints = "Win16MapDlgPoints";

BOOL16 APIENTRY16 Trc_Win16MapDlgPoints( HWND      Parm1
                                       , PPOINTL16 Parm2
                                       , USHORT    Parm3
                                       , BOOL16    Parm4
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16MapDlgPoints );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm2, Parm3 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_BOOL );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16MapDlgPoints, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINMAPDLGPOINTS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16MapDlgPoints );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer16( Parm2, Parm3 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16MapDlgPoints, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16MapDlgPoints() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MapDlgPoints()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32MAPDLGPOINTS( HWND, PPOINTL, ULONG, BOOL );
PCHAR szWin32MapDlgPoints = "Win32MapDlgPoints";

BOOL APIENTRY Trc_Win32MapDlgPoints( HWND    Parm1
                                   , PPOINTL Parm2
                                   , ULONG   Parm3
                                   , BOOL    Parm4
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MapDlgPoints );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_BOOL );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32MapDlgPoints, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32MAPDLGPOINTS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32MapDlgPoints );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer( Parm2, Parm3 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32MapDlgPoints, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32MapDlgPoints() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16MapWindowPoints()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINMAPWINDOWPOINTS( HWND, HWND, PPOINTL16, SHORT );
PCHAR szWin16MapWindowPoints = "Win16MapWindowPoints";

BOOL16 APIENTRY16 Trc_Win16MapWindowPoints( HWND      Parm1
                                          , HWND      Parm2
                                          , PPOINTL16 Parm3
                                          , SHORT     Parm4
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16MapWindowPoints );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer16( Parm3, Parm4 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16MapWindowPoints, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINMAPWINDOWPOINTS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16MapWindowPoints );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer16( Parm3, Parm4 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szWin16MapWindowPoints, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16MapWindowPoints() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MapWindowPoints()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32MAPWINDOWPOINTS( HWND, HWND, PPOINTL, LONG );
PCHAR szWin32MapWindowPoints = "Win32MapWindowPoints";

BOOL APIENTRY Trc_Win32MapWindowPoints( HWND    Parm1
                                      , HWND    Parm2
                                      , PPOINTL Parm3
                                      , LONG    Parm4
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MapWindowPoints );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPOINTLBuffer( Parm3, Parm4 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32MapWindowPoints, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32MAPWINDOWPOINTS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32MapWindowPoints );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PPOINTL );
         if ( bRC ) T_LogPOINTLBuffer( Parm3, Parm4 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szWin32MapWindowPoints, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32MapWindowPoints() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16MessageBox()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINMESSAGEBOX( HWND, HWND, PSZ16, PSZ16, USHORT, USHORT );
PCHAR szWin16MessageBox = "Win16MessageBox";

USHORT APIENTRY16 Trc_Win16MessageBox( HWND   Parm1
                                     , HWND   Parm2
                                     , PSZ16  Parm3
                                     , PSZ16  Parm4
                                     , USHORT Parm5
                                     , USHORT Parm6
                                     )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16MessageBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ16( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_USHORT );
         T_LogUSHORT( Parm6 );
      }
      HOOKENTRY16( szWin16MessageBox, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINMESSAGEBOX( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16MessageBox );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC != MBID_ERROR );
      HOOKEXIT16( szWin16MessageBox, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16MessageBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MessageBox()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32MESSAGEBOX( HWND, HWND, PSZ, PSZ, ULONG, ULONG );
PCHAR szWin32MessageBox = "Win32MessageBox";

ULONG APIENTRY Trc_Win32MessageBox( HWND  Parm1
                                  , HWND  Parm2
                                  , PSZ   Parm3
                                  , PSZ   Parm4
                                  , ULONG Parm5
                                  , ULONG Parm6
                                  )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MessageBox );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szWin32MessageBox, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32MESSAGEBOX( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32MessageBox );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != MBID_ERROR );
      HOOKEXIT( szWin32MessageBox, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32MessageBox() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MessageBox2()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 230 ) )
ULONG APIENTRY WIN32MESSAGEBOX2( HWND, HWND, PSZ, PSZ, ULONG, PMB2INFO );
PCHAR szWin32MessageBox2 = "Win32MessageBox2";

ULONG APIENTRY Trc_Win32MessageBox2( HWND     Parm1
                                   , HWND     Parm2
                                   , PSZ      Parm3
                                   , PSZ      Parm4
                                   , ULONG    Parm5
                                   , PMB2INFO Parm6
                                   )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MessageBox2 );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PMB2INFO );
         T_LogPMB2INFO( Parm6 );
      }
      HOOKENTRY( szWin32MessageBox2, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnWIN32MESSAGEBOX2 )( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32MessageBox2 );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != MBID_ERROR );
      HOOKEXIT( szWin32MessageBox2, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32MessageBox2() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16MsgMuxSemWait()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 WINMSGMUXSEMWAIT( PUSHORT16, PVOID16, LONG );
PCHAR szWin16MsgMuxSemWait = "Win16MsgMuxSemWait";

APIRET16 APIENTRY16 Trc_Win16MsgMuxSemWait( PUSHORT16 Parm1
                                          , PVOID16   Parm2
                                          , LONG      Parm3
                                          )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET16 usRC;                                /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16MsgMuxSemWait );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPVOID16( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID16( Parm2 );
         T_LogPMUXSEMLIST( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szWin16MsgMuxSemWait, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   usRC = WINMSGMUXSEMWAIT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin16MsgMuxSemWait );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPUSHORT16( Parm1 );
      }
      HOOKEXIT16( szWin16MsgMuxSemWait, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16MsgMuxSemWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MsgMuxSemWait()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY WIN32MSGMUXSEMWAIT( PUSHORT, PVOID, LONG );
PCHAR szWin32MsgMuxSemWait = "Win32MsgMuxSemWait";

APIRET APIENTRY Trc_Win32MsgMuxSemWait( PUSHORT Parm1
                                      , PVOID   Parm2
                                      , LONG    Parm3
                                      )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MsgMuxSemWait );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPVOID( Parm1 );
         T_LogParameter( 2L, DT_PVOID );
         T_LogPVOID( Parm2 );
         T_LogPMUXSEMLIST( (PVOID16)Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32MsgMuxSemWait, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   ulRC = WIN32MSGMUXSEMWAIT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin32MsgMuxSemWait );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 1L, DT_PUSHORT );
         T_LogPUSHORT( Parm1 );
      }
      HOOKEXIT( szWin32MsgMuxSemWait, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32MsgMuxSemWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16MsgSemWait()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
APIRET16 APIENTRY16 WINMSGSEMWAIT( HSEM, LONG );
PCHAR szWin16MsgSemWait = "Win16MsgSemWait";

APIRET16 APIENTRY16 Trc_Win16MsgSemWait( HSEM Parm1
                                       , LONG Parm2
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET16 usRC;                                /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16MsgSemWait );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16MsgSemWait, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   usRC = WINMSGSEMWAIT( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin16MsgSemWait );
      T_LogCPReturnCode( usRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT16( szWin16MsgSemWait, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16MsgSemWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MsgSemWait()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY WIN32MSGSEMWAIT( HSEM, LONG );
PCHAR szWin32MsgSemWait = "Win32MsgSemWait";

APIRET APIENTRY Trc_Win32MsgSemWait( HSEM Parm1
                                   , LONG Parm2
                                   )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MsgSemWait );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSEM );
         T_LogULONG( (ULONG)Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32MsgSemWait, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   ulRC = WIN32MSGSEMWAIT( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin32MsgSemWait );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szWin32MsgSemWait, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32MsgSemWait() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16MultWindowFromIDs()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINMULTWINDOWFROMIDS( HWND, PHWND16, USHORT, USHORT );
PCHAR szWin16MultWindowFromIDs = "Win16MultWindowFromIDs";

SHORT APIENTRY16 Trc_Win16MultWindowFromIDs( HWND    Parm1
                                           , PHWND16 Parm2
                                           , USHORT  Parm3
                                           , USHORT  Parm4
                                           )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16MultWindowFromIDs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PHWND );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16MultWindowFromIDs, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINMULTWINDOWFROMIDS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16MultWindowFromIDs );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHWND );
         T_LogPVOID16( Parm2 );
         if ( sRC ) T_LogULONGBuffer16( Parm2, sRC );
      }
      HOOKEXIT16( szWin16MultWindowFromIDs, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16MultWindowFromIDs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32MultWindowFromIDs()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32MULTWINDOWFROMIDS( HWND, PHWND, ULONG, ULONG );
PCHAR szWin32MultWindowFromIDs = "Win32MultWindowFromIDs";

LONG APIENTRY Trc_Win32MultWindowFromIDs( HWND  Parm1
                                        , PHWND Parm2
                                        , ULONG Parm3
                                        , ULONG Parm4
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32MultWindowFromIDs );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PHWND );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32MultWindowFromIDs, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32MULTWINDOWFROMIDS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32MultWindowFromIDs );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PHWND );
         T_LogPVOID( Parm2 );
         if ( lRC ) T_LogULONGBuffer( Parm2, lRC );
      }
      HOOKEXIT( szWin32MultWindowFromIDs, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32MultWindowFromIDs() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16NextChar()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PSZ16 APIENTRY16 WINNEXTCHAR( HAB, USHORT, USHORT, PSZ16 );
PCHAR szWin16NextChar = "Win16NextChar";

PSZ16 APIENTRY16 Trc_Win16NextChar( HAB    Parm1
                                  , USHORT Parm2
                                  , USHORT Parm3
                                  , PSZ16  Parm4
                                  )
{

   PSZ16 pRC;                                    /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16NextChar );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ16( Parm4 );
      }
      HOOKENTRY16( szWin16NextChar, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINNEXTCHAR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16NextChar );
      T_LogPMReturnCodePtr16( DT_PSZ, pRC, TRUE );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPSZ16( pRC ); }
      }
      HOOKEXIT16( szWin16NextChar, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16NextChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32NextChar()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PSZ APIENTRY WIN32NEXTCHAR( HAB, ULONG, ULONG, PSZ );
PCHAR szWin32NextChar = "Win32NextChar";

PSZ APIENTRY Trc_Win32NextChar( HAB   Parm1
                              , ULONG Parm2
                              , ULONG Parm3
                              , PSZ   Parm4
                              )
{

   PSZ pRC;                                      /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32NextChar );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
      }
      HOOKENTRY( szWin32NextChar, &Parm1 );
      T_FreeLog( );
   }

   pRC = WIN32NEXTCHAR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32NextChar );
      T_LogPMReturnCode( DT_PSZ, (ULONG)pRC, TRUE );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPSZ( pRC ); }
      }
      HOOKEXIT( szWin32NextChar, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win32NextChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16OffsetRect()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINOFFSETRECT( HAB, PRECTL16, SHORT, SHORT );
PCHAR szWin16OffsetRect = "Win16OffsetRect";

BOOL16 APIENTRY16 Trc_Win16OffsetRect( HAB      Parm1
                                     , PRECTL16 Parm2
                                     , SHORT    Parm3
                                     , SHORT    Parm4
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16OffsetRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16OffsetRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINOFFSETRECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16OffsetRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16OffsetRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16OffsetRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32OffsetRect()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32OFFSETRECT( HAB, PRECTL, LONG, LONG );
PCHAR szWin32OffsetRect = "Win32OffsetRect";

BOOL APIENTRY Trc_Win32OffsetRect( HAB    Parm1
                                 , PRECTL Parm2
                                 , LONG   Parm3
                                 , LONG   Parm4
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32OffsetRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32OffsetRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32OFFSETRECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32OffsetRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32OffsetRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32OffsetRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16OpenClipbrd()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINOPENCLIPBRD( HAB );
PCHAR szWin16OpenClipbrd = "Win16OpenClipbrd";

BOOL16 APIENTRY16 Trc_Win16OpenClipbrd( HAB Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16OpenClipbrd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16OpenClipbrd, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINOPENCLIPBRD( Parm1 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16OpenClipbrd );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16OpenClipbrd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16OpenClipbrd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32OpenClipbrd()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32OPENCLIPBRD( HAB );
PCHAR szWin32OpenClipbrd = "Win32OpenClipbrd";

BOOL APIENTRY Trc_Win32OpenClipbrd( HAB Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32OpenClipbrd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32OpenClipbrd, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32OPENCLIPBRD( Parm1 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32OpenClipbrd );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32OpenClipbrd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32OpenClipbrd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16OpenWindowDC()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HDC APIENTRY16 WINOPENWINDOWDC( HWND );
PCHAR szWin16OpenWindowDC = "Win16OpenWindowDC";

HDC APIENTRY16 Trc_Win16OpenWindowDC( HWND Parm1 )
{

   HDC ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16OpenWindowDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16OpenWindowDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINOPENWINDOWDC( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16OpenWindowDC );
      T_LogPMReturnCode( DT_HDC, ulRC, ulRC );
      HOOKEXIT16( szWin16OpenWindowDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16OpenWindowDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32OpenWindowDC()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HDC APIENTRY WIN32OPENWINDOWDC( HWND );
PCHAR szWin32OpenWindowDC = "Win32OpenWindowDC";

HDC APIENTRY Trc_Win32OpenWindowDC( HWND Parm1 )
{

   HDC ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32OpenWindowDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32OpenWindowDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32OPENWINDOWDC( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32OpenWindowDC );
      T_LogPMReturnCode( DT_HDC, ulRC, ulRC );
      HOOKEXIT( szWin32OpenWindowDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32OpenWindowDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16PeekMsg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINPEEKMSG( HAB, PQMSG16, HWND, USHORT, USHORT, USHORT );
PCHAR szWin16PeekMsg = "Win16PeekMsg";

BOOL16 APIENTRY16 Trc_Win16PeekMsg( HAB     Parm1
                                  , PQMSG16 Parm2
                                  , HWND    Parm3
                                  , USHORT  Parm4
                                  , USHORT  Parm5
                                  , USHORT  Parm6
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16PeekMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_HWND );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_USHORT );
         T_LogUSHORT( Parm6 );
      }
      HOOKENTRY16( szWin16PeekMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINPEEKMSG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16PeekMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PQMSG );
         if ( bRC ) T_LogPQMSG16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16PeekMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16PeekMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32PeekMsg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32PEEKMSG( HAB, PQMSG, HWND, ULONG, ULONG, ULONG );
PCHAR szWin32PeekMsg = "Win32PeekMsg";

BOOL APIENTRY Trc_Win32PeekMsg( HAB   Parm1
                              , PQMSG Parm2
                              , HWND  Parm3
                              , ULONG Parm4
                              , ULONG Parm5
                              , ULONG Parm6
                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32PeekMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PQMSG );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_HWND );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_ULONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szWin32PeekMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32PEEKMSG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32PeekMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PQMSG );
         if ( bRC ) T_LogPQMSG( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32PeekMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32PeekMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16PopupMenu()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINPOPUPMENU( HWND, HWND, HWND, SHORT, SHORT, SHORT, USHORT );
PCHAR szWin16PopupMenu = "Win16PopupMenu";

BOOL16 APIENTRY16 Trc_Win16PopupMenu( HWND   Parm1
                                    , HWND   Parm2
                                    , HWND   Parm3
                                    , SHORT  Parm4
                                    , SHORT  Parm5
                                    , SHORT  Parm6
                                    , USHORT Parm7
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16PopupMenu );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HWND );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_SHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szWin16PopupMenu, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINPOPUPMENU( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16PopupMenu );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16PopupMenu, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16PopupMenu() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32PopupMenu()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32POPUPMENU( HWND, HWND, HWND, LONG, LONG, LONG, ULONG );
PCHAR szWin32PopupMenu = "Win32PopupMenu";

BOOL APIENTRY Trc_Win32PopupMenu( HWND  Parm1
                                , HWND  Parm2
                                , HWND  Parm3
                                , LONG  Parm4
                                , LONG  Parm5
                                , LONG  Parm6
                                , ULONG Parm7
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32PopupMenu );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HWND );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szWin32PopupMenu, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32POPUPMENU( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_MENU ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32PopupMenu );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32PopupMenu, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32PopupMenu() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16PostMsg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINPOSTMSG( HWND, USHORT, MPARAM, MPARAM );
PCHAR szWin16PostMsg = "Win16PostMsg";

BOOL16 APIENTRY16 Trc_Win16PostMsg( HWND   Parm1
                                  , USHORT Parm2
                                  , MPARAM Parm3
                                  , MPARAM Parm4
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16PostMsg );
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
      HOOKENTRY16( szWin16PostMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINPOSTMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16PostMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16PostMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16PostMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32PostMsg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32POSTMSG( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32PostMsg = "Win32PostMsg";

BOOL APIENTRY Trc_Win32PostMsg( HWND   Parm1
                              , ULONG  Parm2
                              , MPARAM Parm3
                              , MPARAM Parm4
                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32PostMsg );
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
      HOOKENTRY( szWin32PostMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32POSTMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32PostMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32PostMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32PostMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16PostQueueMsg()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINPOSTQUEUEMSG( HMQ, USHORT, MPARAM, MPARAM );
PCHAR szWin16PostQueueMsg = "Win16PostQueueMsg";

BOOL16 APIENTRY16 Trc_Win16PostQueueMsg( HMQ    Parm1
                                       , USHORT Parm2
                                       , MPARAM Parm3
                                       , MPARAM Parm4
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16PostQueueMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogPMMessage16( Parm2 );
         T_LogParameter( 3L, DT_MPARAM );
         T_LogULONG( (ULONG)Parm3 );
         T_LogParameter( 4L, DT_MPARAM );
         T_LogULONG( (ULONG)Parm4 );
      }
      HOOKENTRY16( szWin16PostQueueMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINPOSTQUEUEMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16PostQueueMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16PostQueueMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16PostQueueMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32PostQueueMsg()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32POSTQUEUEMSG( HMQ, ULONG, MPARAM, MPARAM );
PCHAR szWin32PostQueueMsg = "Win32PostQueueMsg";

BOOL APIENTRY Trc_Win32PostQueueMsg( HMQ    Parm1
                                   , ULONG  Parm2
                                   , MPARAM Parm3
                                   , MPARAM Parm4
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32PostQueueMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogPMMessage( Parm2 );
         T_LogParameter( 3L, DT_MPARAM );
         T_LogULONG( (ULONG)Parm3 );
         T_LogParameter( 4L, DT_MPARAM );
         T_LogULONG( (ULONG)Parm4 );
      }
      HOOKENTRY( szWin32PostQueueMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32POSTQUEUEMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32PostQueueMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32PostQueueMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32PostQueueMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16PrevChar()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PSZ16 APIENTRY16 WINPREVCHAR( HAB, USHORT, USHORT, PSZ16, PSZ16 );
PCHAR szWin16PrevChar = "Win16PrevChar";

PSZ16 APIENTRY16 Trc_Win16PrevChar( HAB    Parm1
                                  , USHORT Parm2
                                  , USHORT Parm3
                                  , PSZ16  Parm4
                                  , PSZ16  Parm5
                                  )
{

   PSZ16 pRC;                                    /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16PrevChar );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ16( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZ16( Parm5 );
      }
      HOOKENTRY16( szWin16PrevChar, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINPREVCHAR( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16PrevChar );
      T_LogPMReturnCodePtr16( DT_PSZ, pRC, TRUE );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPSZ16( pRC ); }
      }
      HOOKEXIT16( szWin16PrevChar, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16PrevChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32PrevChar()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PSZ APIENTRY WIN32PREVCHAR( HAB, ULONG, ULONG, PSZ, PSZ );
PCHAR szWin32PrevChar = "Win32PrevChar";

PSZ APIENTRY Trc_Win32PrevChar( HAB   Parm1
                              , ULONG Parm2
                              , ULONG Parm3
                              , PSZ   Parm4
                              , PSZ   Parm5
                              )
{

   PSZ pRC;                                      /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32PrevChar );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
         T_LogParameter( 5L, DT_PSZ );
         T_LogPSZ( Parm5 );
      }
      HOOKENTRY( szWin32PrevChar, &Parm1 );
      T_FreeLog( );
   }

   pRC = WIN32PREVCHAR( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32PrevChar );
      T_LogPMReturnCode( DT_PSZ, (ULONG)pRC, TRUE );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPSZ( pRC ); }
      }
      HOOKEXIT( szWin32PrevChar, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win32PrevChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ProcessDlg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINPROCESSDLG( HWND );
PCHAR szWin16ProcessDlg = "Win16ProcessDlg";

USHORT APIENTRY16 Trc_Win16ProcessDlg( HWND Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ProcessDlg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16ProcessDlg, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINPROCESSDLG( Parm1 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ProcessDlg );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      HOOKEXIT16( szWin16ProcessDlg, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16ProcessDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ProcessDlg()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32PROCESSDLG( HWND );
PCHAR szWin32ProcessDlg = "Win32ProcessDlg";

ULONG APIENTRY Trc_Win32ProcessDlg( HWND Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ProcessDlg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32ProcessDlg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32PROCESSDLG( Parm1 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ProcessDlg );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32ProcessDlg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32ProcessDlg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16PtInRect()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINPTINRECT( HAB, PRECTL16, PPOINTL16 );
PCHAR szWin16PtInRect = "Win16PtInRect";

BOOL16 APIENTRY16 Trc_Win16PtInRect( HAB       Parm1
                                   , PRECTL16  Parm2
                                   , PPOINTL16 Parm3
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16PtInRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL16( Parm3 );
      }
      HOOKENTRY16( szWin16PtInRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINPTINRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16PtInRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, TRUE );
      HOOKEXIT16( szWin16PtInRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16PtInRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32PtInRect()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32PTINRECT( HAB, PRECTL, PPOINTL );
PCHAR szWin32PtInRect = "Win32PtInRect";

BOOL APIENTRY Trc_Win32PtInRect( HAB     Parm1
                               , PRECTL  Parm2
                               , PPOINTL Parm3
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32PtInRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_PPOINTL );
         T_LogPPOINTL( Parm3 );
      }
      HOOKENTRY( szWin32PtInRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32PTINRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32PtInRect );
      T_LogPMReturnCode( DT_BOOL, bRC, TRUE );
      HOOKEXIT( szWin32PtInRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32PtInRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryAccelTable()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HACCEL APIENTRY16 WINQUERYACCELTABLE( HAB, HWND );
PCHAR szWin16QueryAccelTable = "Win16QueryAccelTable";

HACCEL APIENTRY16 Trc_Win16QueryAccelTable( HAB  Parm1
                                          , HWND Parm2
                                          )
{

   HACCEL ulRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16QueryAccelTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYACCELTABLE( Parm1, Parm2 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryAccelTable );
      T_LogPMReturnCode( DT_HACCEL, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryAccelTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryAccelTable()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HACCEL APIENTRY WIN32QUERYACCELTABLE( HAB, HWND );
PCHAR szWin32QueryAccelTable = "Win32QueryAccelTable";

HACCEL APIENTRY Trc_Win32QueryAccelTable( HAB  Parm1
                                        , HWND Parm2
                                        )
{

   HACCEL ulRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryAccelTable, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYACCELTABLE( Parm1, Parm2 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryAccelTable );
      T_LogPMReturnCode( DT_HACCEL, ulRC, ulRC );
      HOOKEXIT( szWin32QueryAccelTable, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryActiveWindow()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYACTIVEWINDOW( HWND, BOOL16 );
PCHAR szWin16QueryActiveWindow = "Win16QueryActiveWindow";

HWND APIENTRY16 Trc_Win16QueryActiveWindow( HWND   Parm1
                                          , BOOL16 Parm2
                                          )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryActiveWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryActiveWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYACTIVEWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryActiveWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryActiveWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryActiveWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryActiveWindow()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYACTIVEWINDOW( HWND );
PCHAR szWin32QueryActiveWindow = "Win32QueryActiveWindow";

HWND APIENTRY Trc_Win32QueryActiveWindow( HWND Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryActiveWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryActiveWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYACTIVEWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryActiveWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QueryActiveWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryActiveWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryAnchorBlock()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HAB APIENTRY16 WINQUERYANCHORBLOCK( HWND );
PCHAR szWin16QueryAnchorBlock = "Win16QueryAnchorBlock";

HAB APIENTRY16 Trc_Win16QueryAnchorBlock( HWND Parm1 )
{

   HAB ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryAnchorBlock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryAnchorBlock, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYANCHORBLOCK( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryAnchorBlock );
      T_LogPMReturnCode( DT_HAB, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryAnchorBlock, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryAnchorBlock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryAnchorBlock()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HAB APIENTRY WIN32QUERYANCHORBLOCK( HWND );
PCHAR szWin32QueryAnchorBlock = "Win32QueryAnchorBlock";

HAB APIENTRY Trc_Win32QueryAnchorBlock( HWND Parm1 )
{

   HAB ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryAnchorBlock );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryAnchorBlock, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYANCHORBLOCK( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryAnchorBlock );
      T_LogPMReturnCode( DT_HAB, ulRC, ulRC );
      HOOKEXIT( szWin32QueryAnchorBlock, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryAnchorBlock() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryAtomLength()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYATOMLENGTH( HATOMTBL, ATOM16 );
PCHAR szWin16QueryAtomLength = "Win16QueryAtomLength";

USHORT APIENTRY16 Trc_Win16QueryAtomLength( HATOMTBL Parm1
                                          , ATOM16   Parm2
                                          )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryAtomLength );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ATOM );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryAtomLength, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINQUERYATOMLENGTH( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryAtomLength );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      HOOKEXIT16( szWin16QueryAtomLength, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16QueryAtomLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryAtomLength()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYATOMLENGTH( HATOMTBL, ATOM );
PCHAR szWin32QueryAtomLength = "Win32QueryAtomLength";

ULONG APIENTRY Trc_Win32QueryAtomLength( HATOMTBL Parm1
                                       , ATOM     Parm2
                                       )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryAtomLength );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ATOM );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryAtomLength, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYATOMLENGTH( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryAtomLength );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32QueryAtomLength, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryAtomLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryAtomName()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYATOMNAME( HATOMTBL, ATOM16, PSZ16, USHORT );
PCHAR szWin16QueryAtomName = "Win16QueryAtomName";

USHORT APIENTRY16 Trc_Win16QueryAtomName( HATOMTBL Parm1
                                        , ATOM16   Parm2
                                        , PSZ16    Parm3
                                        , USHORT   Parm4
                                        )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryAtomName );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ATOM );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16QueryAtomName, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINQUERYATOMNAME( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryAtomName );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSZ );
         if ( usRC ) T_LogPSZ16( Parm3 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szWin16QueryAtomName, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16QueryAtomName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryAtomName()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYATOMNAME( HATOMTBL, ATOM, PSZ, ULONG );
PCHAR szWin32QueryAtomName = "Win32QueryAtomName";

ULONG APIENTRY Trc_Win32QueryAtomName( HATOMTBL Parm1
                                     , ATOM     Parm2
                                     , PSZ      Parm3
                                     , ULONG    Parm4
                                     )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryAtomName );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ATOM );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32QueryAtomName, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYATOMNAME( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryAtomName );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSZ );
         if ( ulRC ) T_LogPSZ( Parm3 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szWin32QueryAtomName, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryAtomName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryAtomUsage()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYATOMUSAGE( HATOMTBL, ATOM16 );
PCHAR szWin16QueryAtomUsage = "Win16QueryAtomUsage";

USHORT APIENTRY16 Trc_Win16QueryAtomUsage( HATOMTBL Parm1
                                         , ATOM16   Parm2
                                         )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryAtomUsage );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ATOM );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryAtomUsage, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINQUERYATOMUSAGE( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryAtomUsage );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      HOOKEXIT16( szWin16QueryAtomUsage, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16QueryAtomUsage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryAtomUsage()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYATOMUSAGE( HATOMTBL, ATOM );
PCHAR szWin32QueryAtomUsage = "Win32QueryAtomUsage";

ULONG APIENTRY Trc_Win32QueryAtomUsage( HATOMTBL Parm1
                                      , ATOM     Parm2
                                      )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryAtomUsage );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HATOMTBL );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ATOM );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryAtomUsage, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYATOMUSAGE( Parm1, Parm2 );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryAtomUsage );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32QueryAtomUsage, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryAtomUsage() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryCapture()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYCAPTURE( HWND, BOOL16 );
PCHAR szWin16QueryCapture = "Win16QueryCapture";

HWND APIENTRY16 Trc_Win16QueryCapture( HWND   Parm1
                                     , BOOL16 Parm2
                                     )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryCapture );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryCapture, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYCAPTURE( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryCapture );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryCapture, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryCapture() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryCapture()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYCAPTURE( HWND );
PCHAR szWin32QueryCapture = "Win32QueryCapture";

HWND APIENTRY Trc_Win32QueryCapture( HWND Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryCapture );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryCapture, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYCAPTURE( Parm1 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryCapture );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QueryCapture, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryCapture() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryClassInfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYCLASSINFO( HAB, PSZ16, PCLASSINFO16 );
PCHAR szWin16QueryClassInfo = "Win16QueryClassInfo";

BOOL16 APIENTRY16 Trc_Win16QueryClassInfo( HAB          Parm1
                                         , PSZ16        Parm2
                                         , PCLASSINFO16 Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryClassInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PCLASSINFO );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szWin16QueryClassInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYCLASSINFO( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryClassInfo );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCLASSINFO );
         if ( bRC ) T_LogPCLASSINFO16( Parm3 );
         else T_LogPVOID16( Parm3 );
      }
      HOOKEXIT16( szWin16QueryClassInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryClassInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryClassInfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYCLASSINFO( HAB, PSZ, PCLASSINFO );
PCHAR szWin32QueryClassInfo = "Win32QueryClassInfo";

BOOL APIENTRY Trc_Win32QueryClassInfo( HAB        Parm1
                                     , PSZ        Parm2
                                     , PCLASSINFO Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryClassInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PCLASSINFO );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32QueryClassInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYCLASSINFO( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryClassInfo );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCLASSINFO );
         if ( bRC ) T_LogPCLASSINFO( Parm3 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szWin32QueryClassInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryClassInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryClassName()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINQUERYCLASSNAME( HWND, SHORT, PCH16 );
PCHAR szWin16QueryClassName = "Win16QueryClassName";

SHORT APIENTRY16 Trc_Win16QueryClassName( HWND  Parm1
                                        , SHORT Parm2
                                        , PCH16 Parm3
                                        )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryClassName );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szWin16QueryClassName, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINQUERYCLASSNAME( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryClassName );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID16( Parm3 );
         if ( sRC ) T_LogCHARBuffer16( Parm3, sRC );
      }
      HOOKEXIT16( szWin16QueryClassName, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16QueryClassName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryClassName()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32QUERYCLASSNAME( HWND, LONG, PCH );
PCHAR szWin32QueryClassName = "Win32QueryClassName";

LONG APIENTRY Trc_Win32QueryClassName( HWND Parm1
                                     , LONG Parm2
                                     , PCH  Parm3
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryClassName );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32QueryClassName, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32QUERYCLASSNAME( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryClassName );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID( Parm3 );
         if ( lRC ) T_LogCHARBuffer( Parm3, lRC );
      }
      HOOKEXIT( szWin32QueryClassName, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QueryClassName() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryClassThunkProc()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PFN16 APIENTRY16 WINQUERYCLASSTHUNKPROC( PSZ16 );
PCHAR szWin16QueryClassThunkProc = "Win16QueryClassThunkProc";

PFN16 APIENTRY16 Trc_Win16QueryClassThunkProc( PSZ16 Parm1 )
{

   PFN16 pRC;                                    /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryClassThunkProc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
      }
      HOOKENTRY16( szWin16QueryClassThunkProc, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINQUERYCLASSTHUNKPROC( Parm1 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryClassThunkProc );
      T_LogPMReturnCodePtr16( DT_PFN, (PVOID16)pRC, (BOOL)pRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPFN16( pRC ); }
      }
      HOOKEXIT16( szWin16QueryClassThunkProc, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16QueryClassThunkProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryClassThunkProc()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PFN APIENTRY WIN32QUERYCLASSTHUNKPROC( PSZ );
PCHAR szWin32QueryClassThunkProc = "Win32QueryClassThunkProc";

PFN APIENTRY Trc_Win32QueryClassThunkProc( PSZ Parm1 )
{

   PFN pRC;                                      /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryClassThunkProc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
      }
      HOOKENTRY( szWin32QueryClassThunkProc, &Parm1 );
      T_FreeLog( );
   }

   pRC = WIN32QUERYCLASSTHUNKPROC( Parm1 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryClassThunkProc );
      T_LogPMReturnCode( DT_PFN, (ULONG)pRC, (BOOL)pRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPFN( pRC ); }
      }
      HOOKEXIT( szWin32QueryClassThunkProc, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win32QueryClassThunkProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryClipbrdData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 WINQUERYCLIPBRDDATA( HAB, USHORT );
PCHAR szWin16QueryClipbrdData = "Win16QueryClipbrdData";

ULONG APIENTRY16 Trc_Win16QueryClipbrdData( HAB    Parm1
                                          , USHORT Parm2
                                          )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryClipbrdData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryClipbrdData, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYCLIPBRDDATA( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryClipbrdData );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryClipbrdData, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryClipbrdData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryClipbrdData()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYCLIPBRDDATA( HAB, ULONG );
PCHAR szWin32QueryClipbrdData = "Win32QueryClipbrdData";

ULONG APIENTRY Trc_Win32QueryClipbrdData( HAB   Parm1
                                        , ULONG Parm2
                                        )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryClipbrdData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryClipbrdData, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYCLIPBRDDATA( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryClipbrdData );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32QueryClipbrdData, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryClipbrdData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryClipbrdFmtInfo()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYCLIPBRDFMTINFO( HAB, USHORT, PUSHORT16 );
PCHAR szWin16QueryClipbrdFmtInfo = "Win16QueryClipbrdFmtInfo";

BOOL16 APIENTRY16 Trc_Win16QueryClipbrdFmtInfo( HAB       Parm1
                                              , USHORT    Parm2
                                              , PUSHORT16 Parm3
                                              )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryClipbrdFmtInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szWin16QueryClipbrdFmtInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYCLIPBRDFMTINFO( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryClipbrdFmtInfo );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szWin16QueryClipbrdFmtInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryClipbrdFmtInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryClipbrdFmtInfo()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYCLIPBRDFMTINFO( HAB, ULONG, PULONG );
PCHAR szWin32QueryClipbrdFmtInfo = "Win32QueryClipbrdFmtInfo";

BOOL APIENTRY Trc_Win32QueryClipbrdFmtInfo( HAB    Parm1
                                          , ULONG  Parm2
                                          , PULONG Parm3
                                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryClipbrdFmtInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32QueryClipbrdFmtInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYCLIPBRDFMTINFO( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryClipbrdFmtInfo );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szWin32QueryClipbrdFmtInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryClipbrdFmtInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryClipbrdOwner()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYCLIPBRDOWNER( HAB, BOOL16 );
PCHAR szWin16QueryClipbrdOwner = "Win16QueryClipbrdOwner";

HWND APIENTRY16 Trc_Win16QueryClipbrdOwner( HAB    Parm1
                                          , BOOL16 Parm2
                                          )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryClipbrdOwner );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryClipbrdOwner, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYCLIPBRDOWNER( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryClipbrdOwner );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryClipbrdOwner, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryClipbrdOwner() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryClipbrdOwner()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYCLIPBRDOWNER( HAB );
PCHAR szWin32QueryClipbrdOwner = "Win32QueryClipbrdOwner";

HWND APIENTRY Trc_Win32QueryClipbrdOwner( HAB Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryClipbrdOwner );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryClipbrdOwner, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYCLIPBRDOWNER( Parm1 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryClipbrdOwner );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QueryClipbrdOwner, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryClipbrdOwner() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryClipbrdViewer()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYCLIPBRDVIEWER( HAB, BOOL16 );
PCHAR szWin16QueryClipbrdViewer = "Win16QueryClipbrdViewer";

HWND APIENTRY16 Trc_Win16QueryClipbrdViewer( HAB    Parm1
                                           , BOOL16 Parm2
                                           )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryClipbrdViewer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryClipbrdViewer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYCLIPBRDVIEWER( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryClipbrdViewer );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryClipbrdViewer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryClipbrdViewer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryClipbrdViewer()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYCLIPBRDVIEWER( HAB );
PCHAR szWin32QueryClipbrdViewer = "Win32QueryClipbrdViewer";

HWND APIENTRY Trc_Win32QueryClipbrdViewer( HAB Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryClipbrdViewer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryClipbrdViewer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYCLIPBRDVIEWER( Parm1 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryClipbrdViewer );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QueryClipbrdViewer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryClipbrdViewer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryClipRegion()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
LONG APIENTRY WIN32QUERYCLIPREGION( HWND, HRGN );
PCHAR szWin32QueryClipRegion = "Win32QueryClipRegion";

#define QCRGN_ERROR          0
#define QCRGN_OK             1
#define QCRGN_NO_CLIP_REGION 2

LONG APIENTRY Trc_Win32QueryClipRegion( HWND Parm1
                                      , HRGN Parm2
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryClipRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryClipRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = ( *pfnWIN32QUERYCLIPREGION )( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryClipRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != QCRGN_ERROR );
      HOOKEXIT( szWin32QueryClipRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QueryClipRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryCp()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYCP( HMQ );
PCHAR szWin16QueryCp = "Win16QueryCp";

USHORT APIENTRY16 Trc_Win16QueryCp( HMQ Parm1 )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryCp, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINQUERYCP( Parm1 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryCp );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      HOOKEXIT16( szWin16QueryCp, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16QueryCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryCp()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYCP( HMQ );
PCHAR szWin32QueryCp = "Win32QueryCp";

ULONG APIENTRY Trc_Win32QueryCp( HMQ Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryCp, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYCP( Parm1 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryCp );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32QueryCp, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryCpList()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYCPLIST( HAB, USHORT, PUSHORT16 );
PCHAR szWin16QueryCpList = "Win16QueryCpList";

USHORT APIENTRY16 Trc_Win16QueryCpList( HAB       Parm1
                                      , USHORT    Parm2
                                      , PUSHORT16 Parm3
                                      )
{

   USHORT usRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryCpList );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szWin16QueryCpList, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINQUERYCPLIST( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryCpList );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PUSHORT );
         T_LogPVOID16( Parm3 );
         if ( usRC ) T_LogUSHORTBuffer16( Parm3, usRC );
      }
      HOOKEXIT16( szWin16QueryCpList, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16QueryCpList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryCpList()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYCPLIST( HAB, ULONG, PULONG );
PCHAR szWin32QueryCpList = "Win32QueryCpList";

ULONG APIENTRY Trc_Win32QueryCpList( HAB    Parm1
                                   , ULONG  Parm2
                                   , PULONG Parm3
                                   )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryCpList );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32QueryCpList, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYCPLIST( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryCpList );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
         if ( ulRC ) T_LogULONGBuffer( Parm3, ulRC );
      }
      HOOKEXIT( szWin32QueryCpList, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryCpList() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryCursorInfo()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYCURSORINFO( HWND, PCURSORINFO16 );
PCHAR szWin16QueryCursorInfo = "Win16QueryCursorInfo";

BOOL16 APIENTRY16 Trc_Win16QueryCursorInfo( HWND          Parm1
                                          , PCURSORINFO16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryCursorInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PCURSORINFO );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16QueryCursorInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYCURSORINFO( Parm1, Parm2 );

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryCursorInfo );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PCURSORINFO );
         if ( bRC ) T_LogPCURSORINFO16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryCursorInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryCursorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryCursorInfo()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYCURSORINFO( HWND, PCURSORINFO );
PCHAR szWin32QueryCursorInfo = "Win32QueryCursorInfo";

BOOL APIENTRY Trc_Win32QueryCursorInfo( HWND        Parm1
                                      , PCURSORINFO Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryCursorInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PCURSORINFO );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32QueryCursorInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYCURSORINFO( Parm1, Parm2 );

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryCursorInfo );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PCURSORINFO );
         if ( bRC ) T_LogPCURSORINFO( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryCursorInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryCursorInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryDesktopBkgnd()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYDESKTOPBKGND( HWND, PDESKTOP16 );
PCHAR szWin16QueryDesktopBkgnd = "Win16QueryDesktopBkgnd";

BOOL16 APIENTRY16 Trc_Win16QueryDesktopBkgnd( HWND       Parm1
                                            , PDESKTOP16 Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DESK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryDesktopBkgnd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PDESKTOP );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16QueryDesktopBkgnd, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYDESKTOPBKGND( Parm1, Parm2 );

   if ( LOGWINAPI( W_DESK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryDesktopBkgnd );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PDESKTOP );
         if ( bRC ) T_LogPDESKTOP16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryDesktopBkgnd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryDesktopBkgnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryDesktopBkgnd()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYDESKTOPBKGND( HWND, PDESKTOP );
PCHAR szWin32QueryDesktopBkgnd = "Win32QueryDesktopBkgnd";

BOOL APIENTRY Trc_Win32QueryDesktopBkgnd( HWND     Parm1
                                        , PDESKTOP Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DESK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryDesktopBkgnd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PDESKTOP );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32QueryDesktopBkgnd, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYDESKTOPBKGND( Parm1, Parm2 );

   if ( LOGWINAPI( W_DESK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryDesktopBkgnd );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PDESKTOP );
         if ( bRC ) T_LogPDESKTOP( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryDesktopBkgnd, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryDesktopBkgnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryDesktopWindow()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYDESKTOPWINDOW( HAB, HDC );
PCHAR szWin16QueryDesktopWindow = "Win16QueryDesktopWindow";

HWND APIENTRY16 Trc_Win16QueryDesktopWindow( HAB Parm1
                                           , HDC Parm2
                                           )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryDesktopWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HDC );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16QueryDesktopWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYDESKTOPWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryDesktopWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryDesktopWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryDesktopWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryDesktopWindow()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYDESKTOPWINDOW( HAB, HDC );
PCHAR szWin32QueryDesktopWindow = "Win32QueryDesktopWindow";

HWND APIENTRY Trc_Win32QueryDesktopWindow( HAB Parm1
                                         , HDC Parm2
                                         )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryDesktopWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HDC );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryDesktopWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYDESKTOPWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryDesktopWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QueryDesktopWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryDesktopWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryDlgItemShort()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYDLGITEMSHORT( HWND, USHORT, PSHORT16, BOOL16 );
PCHAR szWin16QueryDlgItemShort = "Win16QueryDlgItemShort";

BOOL16 APIENTRY16 Trc_Win16QueryDlgItemShort( HWND     Parm1
                                            , USHORT   Parm2
                                            , PSHORT16 Parm3
                                            , BOOL16   Parm4
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryDlgItemShort );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PSHORT );
         T_LogPVOID16( Parm3 );
         T_LogParameter( 4L, DT_BOOL );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16QueryDlgItemShort, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYDLGITEMSHORT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryDlgItemShort );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSHORT );
         T_LogPUSHORT16( (PUSHORT)Parm3 );
      }
      HOOKEXIT16( szWin16QueryDlgItemShort, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryDlgItemShort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryDlgItemShort()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYDLGITEMSHORT( HWND, ULONG, PSHORT, BOOL );
PCHAR szWin32QueryDlgItemShort = "Win32QueryDlgItemShort";

BOOL APIENTRY Trc_Win32QueryDlgItemShort( HWND   Parm1
                                        , ULONG  Parm2
                                        , PSHORT Parm3
                                        , BOOL   Parm4
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryDlgItemShort );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSHORT );
         T_LogPVOID( Parm3 );
         T_LogParameter( 4L, DT_BOOL );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32QueryDlgItemShort, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYDLGITEMSHORT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryDlgItemShort );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PSHORT );
         T_LogPUSHORT( (PUSHORT)Parm3 );
      }
      HOOKEXIT( szWin32QueryDlgItemShort, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryDlgItemShort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryDlgItemText()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYDLGITEMTEXT( HWND, USHORT, SHORT, PSZ16 );
PCHAR szWin16QueryDlgItemText = "Win16QueryDlgItemText";

USHORT APIENTRY16 Trc_Win16QueryDlgItemText( HWND   Parm1
                                           , USHORT Parm2
                                           , SHORT  Parm3
                                           , PSZ16  Parm4
                                           )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryDlgItemText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szWin16QueryDlgItemText, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINQUERYDLGITEMTEXT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryDlgItemText );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSZ );
         if ( usRC ) T_LogPSZ16( Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szWin16QueryDlgItemText, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16QueryDlgItemText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryDlgItemText()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYDLGITEMTEXT( HWND, ULONG, LONG, PSZ );
PCHAR szWin32QueryDlgItemText = "Win32QueryDlgItemText";

ULONG APIENTRY Trc_Win32QueryDlgItemText( HWND  Parm1
                                        , ULONG Parm2
                                        , LONG  Parm3
                                        , PSZ   Parm4
                                        )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryDlgItemText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szWin32QueryDlgItemText, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYDLGITEMTEXT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryDlgItemText );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSZ );
         if ( ulRC ) T_LogPSZ( Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szWin32QueryDlgItemText, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryDlgItemText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryDlgItemTextLength()                                          */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINQUERYDLGITEMTEXTLENGTH( HWND, USHORT );
PCHAR szWin16QueryDlgItemTextLength = "Win16QueryDlgItemTextLength";

SHORT APIENTRY16 Trc_Win16QueryDlgItemTextLength( HWND   Parm1
                                                , USHORT Parm2
                                                )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryDlgItemTextLength );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryDlgItemTextLength, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINQUERYDLGITEMTEXTLENGTH( Parm1, Parm2 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryDlgItemTextLength );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      HOOKEXIT16( szWin16QueryDlgItemTextLength, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16QueryDlgItemTextLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryDlgItemTextLength()                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32QUERYDLGITEMTEXTLENGTH( HWND, ULONG );
PCHAR szWin32QueryDlgItemTextLength = "Win32QueryDlgItemTextLength";

LONG APIENTRY Trc_Win32QueryDlgItemTextLength( HWND  Parm1
                                             , ULONG Parm2
                                             )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryDlgItemTextLength );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryDlgItemTextLength, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32QUERYDLGITEMTEXTLENGTH( Parm1, Parm2 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryDlgItemTextLength );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      HOOKEXIT( szWin32QueryDlgItemTextLength, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QueryDlgItemTextLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryFocus()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYFOCUS( HWND, BOOL16 );
PCHAR szWin16QueryFocus = "Win16QueryFocus";

HWND APIENTRY16 Trc_Win16QueryFocus( HWND   Parm1
                                   , BOOL16 Parm2
                                   )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryFocus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryFocus, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYFOCUS( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryFocus );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryFocus, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryFocus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryFocus()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYFOCUS( HWND );
PCHAR szWin32QueryFocus = "Win32QueryFocus";

HWND APIENTRY Trc_Win32QueryFocus( HWND Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryFocus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryFocus, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYFOCUS( Parm1 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryFocus );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QueryFocus, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryFocus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryMsgPos()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYMSGPOS( HAB, PPOINTL16 );
PCHAR szWin16QueryMsgPos = "Win16QueryMsgPos";

BOOL16 APIENTRY16 Trc_Win16QueryMsgPos( HAB       Parm1
                                      , PPOINTL16 Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryMsgPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16QueryMsgPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYMSGPOS( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryMsgPos );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryMsgPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryMsgPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryMsgPos()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYMSGPOS( HAB, PPOINTL );
PCHAR szWin32QueryMsgPos = "Win32QueryMsgPos";

BOOL APIENTRY Trc_Win32QueryMsgPos( HAB     Parm1
                                  , PPOINTL Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryMsgPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32QueryMsgPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYMSGPOS( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryMsgPos );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryMsgPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryMsgPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryMsgTime()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 WINQUERYMSGTIME( HAB );
PCHAR szWin16QueryMsgTime = "Win16QueryMsgTime";

ULONG APIENTRY16 Trc_Win16QueryMsgTime( HAB Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryMsgTime );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryMsgTime, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYMSGTIME( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryMsgTime );
      T_LogPMReturnCode( DT_ULONG, ulRC, TRUE );
      HOOKEXIT16( szWin16QueryMsgTime, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryMsgTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryMsgTime()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYMSGTIME( HAB );
PCHAR szWin32QueryMsgTime = "Win32QueryMsgTime";

ULONG APIENTRY Trc_Win32QueryMsgTime( HAB Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryMsgTime );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryMsgTime, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYMSGTIME( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryMsgTime );
      T_LogPMReturnCode( DT_ULONG, ulRC, TRUE );
      HOOKEXIT( szWin32QueryMsgTime, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryMsgTime() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryObjectWindow()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYOBJECTWINDOW( HWND );
PCHAR szWin16QueryObjectWindow = "Win16QueryObjectWindow";

HWND APIENTRY16 Trc_Win16QueryObjectWindow( HWND Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryObjectWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryObjectWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYOBJECTWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryObjectWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryObjectWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryObjectWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryObjectWindow()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYOBJECTWINDOW( HWND );
PCHAR szWin32QueryObjectWindow = "Win32QueryObjectWindow";

HWND APIENTRY Trc_Win32QueryObjectWindow( HWND Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryObjectWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryObjectWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYOBJECTWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryObjectWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QueryObjectWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryObjectWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryPointer()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPOINTER APIENTRY16 WINQUERYPOINTER( HWND );
PCHAR szWin16QueryPointer = "Win16QueryPointer";

HPOINTER APIENTRY16 Trc_Win16QueryPointer( HWND Parm1 )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryPointer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYPOINTER( Parm1 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryPointer );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryPointer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryPointer()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPOINTER APIENTRY WIN32QUERYPOINTER( HWND );
PCHAR szWin32QueryPointer = "Win32QueryPointer";

HPOINTER APIENTRY Trc_Win32QueryPointer( HWND Parm1 )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryPointer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYPOINTER( Parm1 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryPointer );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT( szWin32QueryPointer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryPointerInfo()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYPOINTERINFO( HPOINTER, PPOINTERINFO16 );
PCHAR szWin16QueryPointerInfo = "Win16QueryPointerInfo";

BOOL16 APIENTRY16 Trc_Win16QueryPointerInfo( HPOINTER       Parm1
                                           , PPOINTERINFO16 Parm2
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryPointerInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPOINTER );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTERINFO );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16QueryPointerInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYPOINTERINFO( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryPointerInfo );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTERINFO );
         if ( bRC ) T_LogPPOINTERINFO16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryPointerInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryPointerInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryPointerInfo()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYPOINTERINFO( HPOINTER, PPOINTERINFO );
PCHAR szWin32QueryPointerInfo = "Win32QueryPointerInfo";

BOOL APIENTRY Trc_Win32QueryPointerInfo( HPOINTER     Parm1
                                       , PPOINTERINFO Parm2
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryPointerInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPOINTER );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTERINFO );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32QueryPointerInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYPOINTERINFO( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryPointerInfo );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTERINFO );
         if ( bRC ) T_LogPPOINTERINFO( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryPointerInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryPointerInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryPointerPos()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYPOINTERPOS( HWND, PPOINTL16 );
PCHAR szWin16QueryPointerPos = "Win16QueryPointerPos";

BOOL16 APIENTRY16 Trc_Win16QueryPointerPos( HWND      Parm1
                                          , PPOINTL16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryPointerPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16QueryPointerPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYPOINTERPOS( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryPointerPos );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryPointerPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryPointerPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryPointerPos()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYPOINTERPOS( HWND, PPOINTL );
PCHAR szWin32QueryPointerPos = "Win32QueryPointerPos";

BOOL APIENTRY Trc_Win32QueryPointerPos( HWND    Parm1
                                      , PPOINTL Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryPointerPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32QueryPointerPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYPOINTERPOS( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryPointerPos );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPOINTL );
         if ( bRC ) T_LogPPOINTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryPointerPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryPointerPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryPresParam()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 WINQUERYPRESPARAM( HWND, ULONG, ULONG, PULONG16, ULONG, PVOID16, USHORT );
PCHAR szWin16QueryPresParam = "Win16QueryPresParam";

ULONG APIENTRY16 Trc_Win16QueryPresParam( HWND     Parm1
                                        , ULONG    Parm2
                                        , ULONG    Parm3
                                        , PULONG16 Parm4
                                        , ULONG    Parm5
                                        , PVOID16  Parm6
                                        , USHORT   Parm7
                                        )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryPresParam );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG16( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PVOID );
         T_LogPVOID16( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szWin16QueryPresParam, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYPRESPARAM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryPresParam );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG16( Parm4 );
         T_LogParameter( 6L, DT_PVOID );
         T_LogPVOID16( Parm6 );
         if ( ulRC ) T_LogCHARBuffer16( Parm6, ulRC );
      }
      HOOKEXIT16( szWin16QueryPresParam, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryPresParam() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryPresParam()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYPRESPARAM( HWND, ULONG, ULONG, PULONG, ULONG, PVOID, ULONG );
PCHAR szWin32QueryPresParam = "Win32QueryPresParam";

ULONG APIENTRY Trc_Win32QueryPresParam( HWND   Parm1
                                      , ULONG  Parm2
                                      , ULONG  Parm3
                                      , PULONG Parm4
                                      , ULONG  Parm5
                                      , PVOID  Parm6
                                      , ULONG  Parm7
                                      )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryPresParam );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PVOID );
         T_LogPVOID( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szWin32QueryPresParam, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYPRESPARAM( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryPresParam );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PULONG );
         T_LogPULONG( Parm4 );
         T_LogParameter( 6L, DT_PVOID );
         T_LogPVOID( Parm6 );
         if ( ulRC ) T_LogCHARBuffer( Parm6, ulRC );
      }
      HOOKEXIT( szWin32QueryPresParam, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryPresParam() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryQueueInfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYQUEUEINFO( HMQ, PMQINFO16, USHORT );
PCHAR szWin16QueryQueueInfo = "Win16QueryQueueInfo";

BOOL16 APIENTRY16 Trc_Win16QueryQueueInfo( HMQ       Parm1
                                         , PMQINFO16 Parm2
                                         , USHORT    Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryQueueInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMQINFO );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16QueryQueueInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYQUEUEINFO( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryQueueInfo );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMQINFO );
         if ( bRC ) T_LogPMQINFO16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryQueueInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryQueueInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryQueueInfo()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYQUEUEINFO( HMQ, PMQINFO, ULONG );
PCHAR szWin32QueryQueueInfo = "Win32QueryQueueInfo";

BOOL APIENTRY Trc_Win32QueryQueueInfo( HMQ     Parm1
                                     , PMQINFO Parm2
                                     , ULONG   Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryQueueInfo );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PMQINFO );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32QueryQueueInfo, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYQUEUEINFO( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryQueueInfo );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PMQINFO );
         if ( bRC ) T_LogPMQINFO( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryQueueInfo, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryQueueInfo() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryQueueStatus()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 WINQUERYQUEUESTATUS( HWND );
PCHAR szWin16QueryQueueStatus = "Win16QueryQueueStatus";

ULONG APIENTRY16 Trc_Win16QueryQueueStatus( HWND Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryQueueStatus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryQueueStatus, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYQUEUESTATUS( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryQueueStatus );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryQueueStatus, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryQueueStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryQueueStatus()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYQUEUESTATUS( HWND );
PCHAR szWin32QueryQueueStatus = "Win32QueryQueueStatus";

ULONG APIENTRY Trc_Win32QueryQueueStatus( HWND Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryQueueStatus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryQueueStatus, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYQUEUESTATUS( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryQueueStatus );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32QueryQueueStatus, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryQueueStatus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySendMsg()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HMQ APIENTRY16 WINQUERYSENDMSG( HAB, HMQ, HMQ, PQMSG16 );
PCHAR szWin16QuerySendMsg = "Win16QuerySendMsg";

HMQ APIENTRY16 Trc_Win16QuerySendMsg( HAB     Parm1
                                    , HMQ     Parm2
                                    , HMQ     Parm3
                                    , PQMSG16 Parm4
                                    )
{

   HMQ ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QuerySendMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HMQ );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PQMSG );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szWin16QuerySendMsg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYSENDMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QuerySendMsg );
      T_LogPMReturnCode( DT_HMQ, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PQMSG );
         if ( ulRC ) T_LogPQMSG16( Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szWin16QuerySendMsg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QuerySendMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySendMsg()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HMQ APIENTRY WIN32QUERYSENDMSG( HAB, HMQ, HMQ, PQMSG );
PCHAR szWin32QuerySendMsg = "Win32QuerySendMsg";

HMQ APIENTRY Trc_Win32QuerySendMsg( HAB   Parm1
                                  , HMQ   Parm2
                                  , HMQ   Parm3
                                  , PQMSG Parm4
                                  )
{

   HMQ ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QuerySendMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HMQ );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PQMSG );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szWin32QuerySendMsg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYSENDMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QuerySendMsg );
      T_LogPMReturnCode( DT_HMQ, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PQMSG );
         if ( ulRC ) T_LogPQMSG( Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szWin32QuerySendMsg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QuerySendMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySysColor()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 WINQUERYSYSCOLOR( HWND, LONG, LONG );
PCHAR szWin16QuerySysColor = "Win16QuerySysColor";

LONG APIENTRY16 Trc_Win16QuerySysColor( HWND Parm1
                                      , LONG Parm2
                                      , LONG Parm3
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QuerySysColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szWin16QuerySysColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = WINQUERYSYSCOLOR( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QuerySysColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      HOOKEXIT16( szWin16QuerySysColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win16QuerySysColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySysColor()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32QUERYSYSCOLOR( HWND, LONG, LONG );
PCHAR szWin32QuerySysColor = "Win32QuerySysColor";

LONG APIENTRY Trc_Win32QuerySysColor( HWND Parm1
                                    , LONG Parm2
                                    , LONG Parm3
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QuerySysColor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32QuerySysColor, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32QUERYSYSCOLOR( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QuerySysColor );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      HOOKEXIT( szWin32QuerySysColor, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QuerySysColor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySysModalWindow()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYSYSMODALWINDOW( HWND, BOOL16 );
PCHAR szWin16QuerySysModalWindow = "Win16QuerySysModalWindow";

HWND APIENTRY16 Trc_Win16QuerySysModalWindow( HWND   Parm1
                                            , BOOL16 Parm2
                                            )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QuerySysModalWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QuerySysModalWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYSYSMODALWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QuerySysModalWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QuerySysModalWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QuerySysModalWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySysModalWindow()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYSYSMODALWINDOW( HWND );
PCHAR szWin32QuerySysModalWindow = "Win32QuerySysModalWindow";

HWND APIENTRY Trc_Win32QuerySysModalWindow( HWND Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QuerySysModalWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QuerySysModalWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYSYSMODALWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QuerySysModalWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QuerySysModalWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QuerySysModalWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySysPointer()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HPOINTER APIENTRY16 WINQUERYSYSPOINTER( HWND, SHORT, BOOL16 );
PCHAR szWin16QuerySysPointer = "Win16QuerySysPointer";

HPOINTER APIENTRY16 Trc_Win16QuerySysPointer( HWND   Parm1
                                            , SHORT  Parm2
                                            , BOOL16 Parm3
                                            )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QuerySysPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16QuerySysPointer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYSYSPOINTER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QuerySysPointer );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT16( szWin16QuerySysPointer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QuerySysPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySysPointer()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HPOINTER APIENTRY WIN32QUERYSYSPOINTER( HWND, LONG, BOOL );
PCHAR szWin32QuerySysPointer = "Win32QuerySysPointer";

HPOINTER APIENTRY Trc_Win32QuerySysPointer( HWND Parm1
                                          , LONG Parm2
                                          , BOOL Parm3
                                          )
{

   HPOINTER ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QuerySysPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32QuerySysPointer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYSYSPOINTER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QuerySysPointer );
      T_LogPMReturnCode( DT_HPOINTER, ulRC, ulRC );
      HOOKEXIT( szWin32QuerySysPointer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QuerySysPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySysPointerData()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
BOOL APIENTRY WIN32QUERYSYSPOINTERDATA( HWND, ULONG, PICONINFO );
PCHAR szWin32QuerySysPointerData = "Win32QuerySysPointerData";

BOOL APIENTRY Trc_Win32QuerySysPointerData( HWND      Parm1
                                          , ULONG     Parm2
                                          , PICONINFO Parm3
                                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QuerySysPointerData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PICONINFO );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32QuerySysPointerData, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnWIN32QUERYSYSPOINTERDATA )( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QuerySysPointerData );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PICONINFO );
         if ( bRC ) T_LogPICONINFO( Parm3 );
         else T_LogPVOID( Parm3 );
      }
      HOOKEXIT( szWin32QuerySysPointerData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QuerySysPointerData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySystemAtomTable()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HATOMTBL APIENTRY16 WINQUERYSYSTEMATOMTABLE( VOID );
PCHAR szWin16QuerySystemAtomTable = "Win16QuerySystemAtomTable";

HATOMTBL APIENTRY16 Trc_Win16QuerySystemAtomTable( VOID )
{

   HATOMTBL ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QuerySystemAtomTable );
      HOOKENTRY16( szWin16QuerySystemAtomTable, NULL );
      T_FreeLog( );
   }

   ulRC = WINQUERYSYSTEMATOMTABLE( );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QuerySystemAtomTable );
      T_LogPMReturnCode( DT_HATOMTBL, ulRC, TRUE );
      HOOKEXIT16( szWin16QuerySystemAtomTable, NULL, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QuerySystemAtomTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySystemAtomTable()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HATOMTBL APIENTRY WIN32QUERYSYSTEMATOMTABLE( VOID );
PCHAR szWin32QuerySystemAtomTable = "Win32QuerySystemAtomTable";

HATOMTBL APIENTRY Trc_Win32QuerySystemAtomTable( VOID )
{

   HATOMTBL ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QuerySystemAtomTable );
      HOOKENTRY( szWin32QuerySystemAtomTable, NULL );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYSYSTEMATOMTABLE( );

   if ( LOGWINAPI( W_ATOM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QuerySystemAtomTable );
      T_LogPMReturnCode( DT_HATOMTBL, ulRC, TRUE );
      HOOKEXIT( szWin32QuerySystemAtomTable, NULL, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QuerySystemAtomTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QuerySysValue()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 WINQUERYSYSVALUE( HWND, SHORT );
PCHAR szWin16QuerySysValue = "Win16QuerySysValue";

LONG APIENTRY16 Trc_Win16QuerySysValue( HWND  Parm1
                                      , SHORT Parm2
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QuerySysValue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QuerySysValue, &Parm1 );
      T_FreeLog( );
   }

   lRC = WINQUERYSYSVALUE( Parm1, Parm2 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QuerySysValue );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      HOOKEXIT16( szWin16QuerySysValue, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win16QuerySysValue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QuerySysValue()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32QUERYSYSVALUE( HWND, LONG );
PCHAR szWin32QuerySysValue = "Win32QuerySysValue";

LONG APIENTRY Trc_Win32QuerySysValue( HWND Parm1
                                    , LONG Parm2
                                    )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QuerySysValue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QuerySysValue, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32QUERYSYSVALUE( Parm1, Parm2 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QuerySysValue );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      HOOKEXIT( szWin32QuerySysValue, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QuerySysValue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryUpdateRect()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYUPDATERECT( HWND, PRECTL16 );
PCHAR szWin16QueryUpdateRect = "Win16QueryUpdateRect";

BOOL16 APIENTRY16 Trc_Win16QueryUpdateRect( HWND     Parm1
                                          , PRECTL16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryUpdateRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16QueryUpdateRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYUPDATERECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryUpdateRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryUpdateRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryUpdateRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryUpdateRect()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYUPDATERECT( HWND, PRECTL );
PCHAR szWin32QueryUpdateRect = "Win32QueryUpdateRect";

BOOL APIENTRY Trc_Win32QueryUpdateRect( HWND   Parm1
                                      , PRECTL Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryUpdateRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32QueryUpdateRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYUPDATERECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryUpdateRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryUpdateRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryUpdateRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryUpdateRegion()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINQUERYUPDATEREGION( HWND, HRGN );
PCHAR szWin16QueryUpdateRegion = "Win16QueryUpdateRegion";

SHORT APIENTRY16 Trc_Win16QueryUpdateRegion( HWND Parm1
                                           , HRGN Parm2
                                           )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryUpdateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16QueryUpdateRegion, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINQUERYUPDATEREGION( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryUpdateRegion );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC != RGN_ERROR );
      HOOKEXIT16( szWin16QueryUpdateRegion, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16QueryUpdateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryUpdateRegion()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32QUERYUPDATEREGION( HWND, HRGN );
PCHAR szWin32QueryUpdateRegion = "Win32QueryUpdateRegion";

LONG APIENTRY Trc_Win32QueryUpdateRegion( HWND Parm1
                                        , HRGN Parm2
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryUpdateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryUpdateRegion, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32QUERYUPDATEREGION( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryUpdateRegion );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      HOOKEXIT( szWin32QueryUpdateRegion, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QueryUpdateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryVersion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 WINQUERYVERSION( HAB );
PCHAR szWin16QueryVersion = "Win16QueryVersion";

ULONG APIENTRY16 Trc_Win16QueryVersion( HAB Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryVersion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryVersion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYVERSION( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryVersion );
      T_LogPMReturnCode( DT_ULONG, ulRC, TRUE );
      HOOKEXIT16( szWin16QueryVersion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryVersion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryVersion()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYVERSION( HAB );
PCHAR szWin32QueryVersion = "Win32QueryVersion";

ULONG APIENTRY Trc_Win32QueryVersion( HAB Parm1 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryVersion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryVersion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYVERSION( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryVersion );
      T_LogPMReturnCode( DT_ULONG, ulRC, TRUE );
      HOOKEXIT( szWin32QueryVersion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryVersion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryVisibleRegion()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
ULONG APIENTRY WIN32QUERYVISIBLEREGION( HWND, HRGN );
PCHAR szWin32QueryVisibleRegion = "Win32QueryVisibleRegion";

ULONG APIENTRY Trc_Win32QueryVisibleRegion( HWND Parm1
                                          , HRGN Parm2
                                          )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryVisibleRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryVisibleRegion, &Parm1 );
      T_FreeLog( );
   }

   ulRC = ( *pfnWIN32QUERYVISIBLEREGION )( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryVisibleRegion );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC != RGN_ERROR );
      HOOKEXIT( szWin32QueryVisibleRegion, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryVisibleRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindow()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINQUERYWINDOW( HWND, SHORT, BOOL16 );
PCHAR szWin16QueryWindow = "Win16QueryWindow";

HWND APIENTRY16 Trc_Win16QueryWindow( HWND   Parm1
                                    , SHORT  Parm2
                                    , BOOL16 Parm3
                                    )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16QueryWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYWINDOW( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindow()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32QUERYWINDOW( HWND, LONG );
PCHAR szWin32QueryWindow = "Win32QueryWindow";

HWND APIENTRY Trc_Win32QueryWindow( HWND Parm1
                                  , LONG Parm2
                                  )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryWindow, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindow );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32QueryWindow, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowDC()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HDC APIENTRY16 WINQUERYWINDOWDC( HWND );
PCHAR szWin16QueryWindowDC = "Win16QueryWindowDC";

HDC APIENTRY16 Trc_Win16QueryWindowDC( HWND Parm1 )
{

   HDC ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryWindowDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYWINDOWDC( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowDC );
      T_LogPMReturnCode( DT_HDC, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryWindowDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryWindowDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowDC()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HDC APIENTRY WIN32QUERYWINDOWDC( HWND );
PCHAR szWin32QueryWindowDC = "Win32QueryWindowDC";

HDC APIENTRY Trc_Win32QueryWindowDC( HWND Parm1 )
{

   HDC ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryWindowDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYWINDOWDC( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowDC );
      T_LogPMReturnCode( DT_HDC, ulRC, ulRC );
      HOOKEXIT( szWin32QueryWindowDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryWindowDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowLockCount()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINQUERYWINDOWLOCKCOUNT( HWND );
PCHAR szWin16QueryWindowLockCount = "Win16QueryWindowLockCount";

SHORT APIENTRY16 Trc_Win16QueryWindowLockCount( HWND Parm1 )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowLockCount );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryWindowLockCount, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINQUERYWINDOWLOCKCOUNT( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowLockCount );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      HOOKEXIT16( szWin16QueryWindowLockCount, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16QueryWindowLockCount() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowModel()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINQUERYWINDOWMODEL( HWND );
PCHAR szWin16QueryWindowModel = "Win16QueryWindowModel";

SHORT APIENTRY16 Trc_Win16QueryWindowModel( HWND Parm1 )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowModel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryWindowModel, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINQUERYWINDOWMODEL( Parm1 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowModel );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, ( sRC != PM_MODEL_1X ) && ( sRC != PM_MODEL_2X ) );
      HOOKEXIT16( szWin16QueryWindowModel, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16QueryWindowModel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowModel()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32QUERYWINDOWMODEL( HWND );
PCHAR szWin32QueryWindowModel = "Win32QueryWindowModel";

LONG APIENTRY Trc_Win32QueryWindowModel( HWND Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowModel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryWindowModel, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32QUERYWINDOWMODEL( Parm1 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowModel );
      T_LogPMReturnCode( DT_LONG, lRC, ( lRC != PM_MODEL_1X ) && ( lRC != PM_MODEL_2X ) );
      HOOKEXIT( szWin32QueryWindowModel, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QueryWindowModel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowPos()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYWINDOWPOS( HWND, PSWP16 );
PCHAR szWin16QueryWindowPos = "Win16QueryWindowPos";

BOOL16 APIENTRY16 Trc_Win16QueryWindowPos( HWND   Parm1
                                         , PSWP16 Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16QueryWindowPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYWINDOWPOS( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowPos );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSWP );
         if ( bRC ) T_LogPSWP16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryWindowPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowPos()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYWINDOWPOS( HWND, PSWP );
PCHAR szWin32QueryWindowPos = "Win32QueryWindowPos";

BOOL APIENTRY Trc_Win32QueryWindowPos( HWND Parm1
                                     , PSWP Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32QueryWindowPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYWINDOWPOS( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowPos );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PSWP );
         if ( bRC ) T_LogPSWP( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryWindowPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowProcess()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYWINDOWPROCESS( HWND, PPID16, PTID16 );
PCHAR szWin16QueryWindowProcess = "Win16QueryWindowProcess";

BOOL16 APIENTRY16 Trc_Win16QueryWindowProcess( HWND   Parm1
                                             , PPID16 Parm2
                                             , PTID16 Parm3
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowProcess );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPID );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PTID );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szWin16QueryWindowProcess, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYWINDOWPROCESS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowProcess );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPID );
         T_LogPUSHORT16( Parm2 );
         T_LogParameter( 3L, DT_PTID );
         T_LogPUSHORT16( Parm3 );
      }
      HOOKEXIT16( szWin16QueryWindowProcess, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryWindowProcess() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowProcess()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYWINDOWPROCESS( HWND, PPID, PTID );
PCHAR szWin32QueryWindowProcess = "Win32QueryWindowProcess";

BOOL APIENTRY Trc_Win32QueryWindowProcess( HWND Parm1
                                         , PPID Parm2
                                         , PTID Parm3
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowProcess );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPID );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PTID );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32QueryWindowProcess, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYWINDOWPROCESS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowProcess );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PPID );
         T_LogPULONG( Parm2 );
         T_LogParameter( 3L, DT_PTID );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szWin32QueryWindowProcess, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryWindowProcess() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowPtr()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PVOID16 APIENTRY16 WINQUERYWINDOWPTR( HWND, SHORT );
PCHAR szWin16QueryWindowPtr = "Win16QueryWindowPtr";

PVOID16 APIENTRY16 Trc_Win16QueryWindowPtr( HWND  Parm1
                                          , SHORT Parm2
                                          )
{

   PVOID16 pRC;                                  /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowPtr );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryWindowPtr, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINQUERYWINDOWPTR( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowPtr );
      T_LogPMReturnCodePtr16( DT_PVOID, pRC, (BOOL)pRC );
      HOOKEXIT16( szWin16QueryWindowPtr, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16QueryWindowPtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowPtr()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PVOID APIENTRY WIN32QUERYWINDOWPTR( HWND, LONG );
PCHAR szWin32QueryWindowPtr = "Win32QueryWindowPtr";

PVOID APIENTRY Trc_Win32QueryWindowPtr( HWND Parm1
                                      , LONG Parm2
                                      )
{

   PVOID pRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowPtr );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryWindowPtr, &Parm1 );
      T_FreeLog( );
   }

   pRC = WIN32QUERYWINDOWPTR( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowPtr );
      T_LogPMReturnCode( DT_PVOID, (ULONG)pRC, (BOOL)pRC );
      HOOKEXIT( szWin32QueryWindowPtr, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win32QueryWindowPtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowRect()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINQUERYWINDOWRECT( HWND, PRECTL16 );
PCHAR szWin16QueryWindowRect = "Win16QueryWindowRect";

BOOL16 APIENTRY16 Trc_Win16QueryWindowRect( HWND     Parm1
                                          , PRECTL16 Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
      }
      HOOKENTRY16( szWin16QueryWindowRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINQUERYWINDOWRECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL16( Parm2 );
         else T_LogPVOID16( Parm2 );
      }
      HOOKEXIT16( szWin16QueryWindowRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16QueryWindowRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowRect()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32QUERYWINDOWRECT( HWND, PRECTL );
PCHAR szWin32QueryWindowRect = "Win32QueryWindowRect";

BOOL APIENTRY Trc_Win32QueryWindowRect( HWND   Parm1
                                      , PRECTL Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
      }
      HOOKENTRY( szWin32QueryWindowRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32QUERYWINDOWRECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         if ( bRC ) T_LogPRECTL( Parm2 );
         else T_LogPVOID( Parm2 );
      }
      HOOKEXIT( szWin32QueryWindowRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32QueryWindowRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowText()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINQUERYWINDOWTEXT( HWND, SHORT, PCH16 );
PCHAR szWin16QueryWindowText = "Win16QueryWindowText";

SHORT APIENTRY16 Trc_Win16QueryWindowText( HWND  Parm1
                                         , SHORT Parm2
                                         , PCH16 Parm3
                                         )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szWin16QueryWindowText, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINQUERYWINDOWTEXT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowText );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID16( Parm3 );
         if ( sRC ) T_LogCHARBuffer16( Parm3, sRC );
      }
      HOOKEXIT16( szWin16QueryWindowText, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16QueryWindowText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowText()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32QUERYWINDOWTEXT( HWND, LONG, PCH );
PCHAR szWin32QueryWindowText = "Win32QueryWindowText";

LONG APIENTRY Trc_Win32QueryWindowText( HWND Parm1
                                      , LONG Parm2
                                      , PCH  Parm3
                                      )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32QueryWindowText, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32QUERYWINDOWTEXT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowText );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PCH );
         T_LogPVOID( Parm3 );
         if ( lRC ) T_LogCHARBuffer( Parm3, lRC );
      }
      HOOKEXIT( szWin32QueryWindowText, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QueryWindowText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowTextLength()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINQUERYWINDOWTEXTLENGTH( HWND );
PCHAR szWin16QueryWindowTextLength = "Win16QueryWindowTextLength";

SHORT APIENTRY16 Trc_Win16QueryWindowTextLength( HWND Parm1 )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowTextLength );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryWindowTextLength, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINQUERYWINDOWTEXTLENGTH( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowTextLength );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      HOOKEXIT16( szWin16QueryWindowTextLength, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16QueryWindowTextLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowTextLength()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32QUERYWINDOWTEXTLENGTH( HWND );
PCHAR szWin32QueryWindowTextLength = "Win32QueryWindowTextLength";

LONG APIENTRY Trc_Win32QueryWindowTextLength( HWND Parm1 )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowTextLength );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryWindowTextLength, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32QUERYWINDOWTEXTLENGTH( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowTextLength );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      HOOKEXIT( szWin32QueryWindowTextLength, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32QueryWindowTextLength() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowThunkProc()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PFN16 APIENTRY16 WINQUERYWINDOWTHUNKPROC( HWND );
PCHAR szWin16QueryWindowThunkProc = "Win16QueryWindowThunkProc";

PFN16 APIENTRY16 Trc_Win16QueryWindowThunkProc( HWND Parm1 )
{

   PFN16 pRC;                                    /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowThunkProc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16QueryWindowThunkProc, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINQUERYWINDOWTHUNKPROC( Parm1 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowThunkProc );
      T_LogPMReturnCodePtr16( DT_PFN, (PVOID16)pRC, (BOOL)pRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPFN16( pRC ); }
      }
      HOOKEXIT16( szWin16QueryWindowThunkProc, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16QueryWindowThunkProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowThunkProc()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PFN APIENTRY WIN32QUERYWINDOWTHUNKPROC( HWND );
PCHAR szWin32QueryWindowThunkProc = "Win32QueryWindowThunkProc";

PFN APIENTRY Trc_Win32QueryWindowThunkProc( HWND Parm1 )
{

   PFN pRC;                                      /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowThunkProc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32QueryWindowThunkProc, &Parm1 );
      T_FreeLog( );
   }

   pRC = WIN32QUERYWINDOWTHUNKPROC( Parm1 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowThunkProc );
      T_LogPMReturnCode( DT_PFN, (ULONG)pRC, (BOOL)pRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPFN( pRC ); }
      }
      HOOKEXIT( szWin32QueryWindowThunkProc, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win32QueryWindowThunkProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowULong()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
ULONG APIENTRY16 WINQUERYWINDOWULONG( HWND, SHORT );
PCHAR szWin16QueryWindowULong = "Win16QueryWindowULong";

ULONG APIENTRY16 Trc_Win16QueryWindowULong( HWND  Parm1
                                          , SHORT Parm2
                                          )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowULong );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryWindowULong, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUERYWINDOWULONG( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowULong );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT16( szWin16QueryWindowULong, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueryWindowULong() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowULong()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32QUERYWINDOWULONG( HWND, LONG );
PCHAR szWin32QueryWindowULong = "Win32QueryWindowULong";

ULONG APIENTRY Trc_Win32QueryWindowULong( HWND Parm1
                                        , LONG Parm2
                                        )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowULong );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryWindowULong, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUERYWINDOWULONG( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowULong );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32QueryWindowULong, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueryWindowULong() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueryWindowUShort()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINQUERYWINDOWUSHORT( HWND, SHORT );
PCHAR szWin16QueryWindowUShort = "Win16QueryWindowUShort";

USHORT APIENTRY16 Trc_Win16QueryWindowUShort( HWND  Parm1
                                            , SHORT Parm2
                                            )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueryWindowUShort );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16QueryWindowUShort, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINQUERYWINDOWUSHORT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueryWindowUShort );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      HOOKEXIT16( szWin16QueryWindowUShort, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16QueryWindowUShort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueryWindowUShort()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
USHORT APIENTRY WIN32QUERYWINDOWUSHORT( HWND, LONG );
PCHAR szWin32QueryWindowUShort = "Win32QueryWindowUShort";

USHORT APIENTRY Trc_Win32QueryWindowUShort( HWND Parm1
                                          , LONG Parm2
                                          )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueryWindowUShort );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32QueryWindowUShort, &Parm1 );
      T_FreeLog( );
   }

   usRC = WIN32QUERYWINDOWUSHORT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueryWindowUShort );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      HOOKEXIT( szWin32QueryWindowUShort, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win32QueryWindowUShort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16QueueFromID()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HMQ APIENTRY16 WINQUEUEFROMID( HAB, PID16, TID16 );
PCHAR szWin16QueueFromID = "Win16QueueFromID";

HMQ APIENTRY16 Trc_Win16QueueFromID( HAB   Parm1
                                   , PID16 Parm2
                                   , TID16 Parm3
                                   )
{

   HMQ ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16QueueFromID );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_TID );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16QueueFromID, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINQUEUEFROMID( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16QueueFromID );
      T_LogPMReturnCode( DT_HMQ, ulRC, ulRC );
      HOOKEXIT16( szWin16QueueFromID, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16QueueFromID() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32QueueFromID()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HMQ APIENTRY WIN32QUEUEFROMID( HAB, PID, TID );
PCHAR szWin32QueueFromID = "Win32QueueFromID";

HMQ APIENTRY Trc_Win32QueueFromID( HAB Parm1
                                 , PID Parm2
                                 , TID Parm3
                                 )
{

   HMQ ulRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32QueueFromID );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_TID );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32QueueFromID, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32QUEUEFROMID( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32QueueFromID );
      T_LogPMReturnCode( DT_HMQ, ulRC, ulRC );
      HOOKEXIT( szWin32QueueFromID, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32QueueFromID() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RealizePalette()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
LONG APIENTRY16 WINREALIZEPALETTE( HWND, HPS, PULONG16 );
PCHAR szWin16RealizePalette = "Win16RealizePalette";

LONG APIENTRY16 Trc_Win16RealizePalette( HWND     Parm1
                                       , HPS      Parm2
                                       , PULONG16 Parm3
                                       )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PAL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16RealizePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID16( Parm3 );
      }
      HOOKENTRY16( szWin16RealizePalette, &Parm1 );
      T_FreeLog( );
   }

   lRC = WINREALIZEPALETTE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PAL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16RealizePalette );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PAL_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG16( Parm3 );
      }
      HOOKEXIT16( szWin16RealizePalette, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win16RealizePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RealizePalette()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32REALIZEPALETTE( HWND, HPS, PULONG );
PCHAR szWin32RealizePalette = "Win32RealizePalette";

LONG APIENTRY Trc_Win32RealizePalette( HWND   Parm1
                                     , HPS    Parm2
                                     , PULONG Parm3
                                     )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PAL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32RealizePalette );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32RealizePalette, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32REALIZEPALETTE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PAL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32RealizePalette );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != PAL_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szWin32RealizePalette, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32RealizePalette() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ReallocMem()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
NPBYTE16 APIENTRY16 WINREALLOCMEM( HHEAP16, NPBYTE16, USHORT, USHORT );
PCHAR szWin16ReallocMem = "Win16ReallocMem";

NPBYTE16 APIENTRY16 Trc_Win16ReallocMem( HHEAP16  Parm1
                                       , NPBYTE16 Parm2
                                       , USHORT   Parm3
                                       , USHORT   Parm4
                                       )
{

   NPBYTE16 usRC;                                /* API return code           */

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ReallocMem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HHEAP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_NPBYTE );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16ReallocMem, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINREALLOCMEM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_HEAP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ReallocMem );
      T_LogPMReturnCodeUSHORT( DT_NPBYTE, usRC, !usRC );
      HOOKEXIT16( szWin16ReallocMem, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16ReallocMem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RegisterClass()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINREGISTERCLASS( HAB, PSZ16, PFNWP16, ULONG, USHORT );
PCHAR szWin16RegisterClass = "Win16RegisterClass";

BOOL16 APIENTRY16 Trc_Win16RegisterClass( HAB     Parm1
                                        , PSZ16   Parm2
                                        , PFNWP16 Parm3
                                        , ULONG   Parm4
                                        , USHORT  Parm5
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16RegisterClass );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_PFNWP );
         T_LogPFN16( (PFN16)Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_USHORT );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szWin16RegisterClass, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINREGISTERCLASS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16RegisterClass );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16RegisterClass, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16RegisterClass() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RegisterClass()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32REGISTERCLASS( HAB, PSZ, PFNWP, ULONG, ULONG );
PCHAR szWin32RegisterClass = "Win32RegisterClass";

BOOL APIENTRY Trc_Win32RegisterClass( HAB   Parm1
                                    , PSZ   Parm2
                                    , PFNWP Parm3
                                    , ULONG Parm4
                                    , ULONG Parm5
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32RegisterClass );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_PFNWP );
         T_LogPFN( (PFN)Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szWin32RegisterClass, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32REGISTERCLASS( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32RegisterClass );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32RegisterClass, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32RegisterClass() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RegisterUserDatatype()                                            */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINREGISTERUSERDATATYPE( HAB, SHORT, SHORT, PSHORT );
PCHAR szWin16RegisterUserDatatype = "Win16RegisterUserDatatype";

BOOL16 APIENTRY16 Trc_Win16RegisterUserDatatype( HAB      Parm1
                                               , SHORT    Parm2
                                               , SHORT    Parm3
                                               , PSHORT16 Parm4
                                               )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16RegisterUserDatatype );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSHORT );
         T_LogPVOID16( Parm4 );
         T_LogUSHORTBuffer16( (PUSHORT16)Parm4, Parm3 );
      }
      HOOKENTRY16( szWin16RegisterUserDatatype, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINREGISTERUSERDATATYPE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16RegisterUserDatatype );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16RegisterUserDatatype, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16RegisterUserDatatype() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RegisterUserDatatype()                                            */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32REGISTERUSERDATATYPE( HAB, LONG, LONG, PLONG );
PCHAR szWin32RegisterUserDatatype = "Win32RegisterUserDatatype";

BOOL APIENTRY Trc_Win32RegisterUserDatatype( HAB   Parm1
                                           , LONG  Parm2
                                           , LONG  Parm3
                                           , PLONG Parm4
                                           )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32RegisterUserDatatype );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PLONG );
         T_LogPVOID( Parm4 );
         T_LogULONGBuffer( (PULONG)Parm4, Parm3 );
      }
      HOOKENTRY( szWin32RegisterUserDatatype, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32REGISTERUSERDATATYPE( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32RegisterUserDatatype );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32RegisterUserDatatype, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32RegisterUserDatatype() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RegisterUserMsg()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINREGISTERUSERMSG( HAB, USHORT, SHORT, SHORT, SHORT, SHORT, SHORT );
PCHAR szWin16RegisterUserMsg = "Win16RegisterUserMsg";

BOOL16 APIENTRY16 Trc_Win16RegisterUserMsg( HAB    Parm1
                                          , USHORT Parm2
                                          , SHORT  Parm3
                                          , SHORT  Parm4
                                          , SHORT  Parm5
                                          , SHORT  Parm6
                                          , SHORT  Parm7
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16RegisterUserMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_SHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_SHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szWin16RegisterUserMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINREGISTERUSERMSG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16RegisterUserMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16RegisterUserMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16RegisterUserMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RegisterUserMsg()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32REGISTERUSERMSG( HAB, ULONG, LONG, LONG, LONG, LONG, LONG );
PCHAR szWin32RegisterUserMsg = "Win32RegisterUserMsg";

BOOL APIENTRY Trc_Win32RegisterUserMsg( HAB   Parm1
                                      , ULONG Parm2
                                      , LONG  Parm3
                                      , LONG  Parm4
                                      , LONG  Parm5
                                      , LONG  Parm6
                                      , LONG  Parm7
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32RegisterUserMsg );
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
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_LONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szWin32RegisterUserMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32REGISTERUSERMSG( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32RegisterUserMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32RegisterUserMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32RegisterUserMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RegisterWindowDestroy()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINREGISTERWINDOWDESTROY( HWND, BOOL16 );
PCHAR szWin16RegisterWindowDestroy = "Win16RegisterWindowDestroy";

BOOL16 APIENTRY16 Trc_Win16RegisterWindowDestroy( HWND   Parm1
                                                , BOOL16 Parm2
                                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16RegisterWindowDestroy );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16RegisterWindowDestroy, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINREGISTERWINDOWDESTROY( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16RegisterWindowDestroy );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16RegisterWindowDestroy, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16RegisterWindowDestroy() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ReleaseErasePS()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINRELEASEERASEPS( HPS );
PCHAR szWin16ReleaseErasePS = "Win16ReleaseErasePS";

BOOL16 APIENTRY16 Trc_Win16ReleaseErasePS( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ReleaseErasePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16ReleaseErasePS, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINRELEASEERASEPS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ReleaseErasePS );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ReleaseErasePS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ReleaseErasePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ReleaseErasePS()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32RELEASEERASEPS( HPS );
PCHAR szWin32ReleaseErasePS = "Win32ReleaseErasePS";

BOOL APIENTRY Trc_Win32ReleaseErasePS( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ReleaseErasePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32ReleaseErasePS, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32RELEASEERASEPS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ReleaseErasePS );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ReleaseErasePS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ReleaseErasePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ReleaseHook()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINRELEASEHOOK( HAB, HMQ, SHORT, PFN16, HMODULE16 );
PCHAR szWin16ReleaseHook = "Win16ReleaseHook";

BOOL16 APIENTRY16 Trc_Win16ReleaseHook( HAB       Parm1
                                      , HMQ       Parm2
                                      , SHORT     Parm3
                                      , PFN16     Parm4
                                      , HMODULE16 Parm5
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ReleaseHook );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PFN );
         T_LogPFN16( Parm4 );
         T_LogParameter( 5L, DT_HMODULE );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szWin16ReleaseHook, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINRELEASEHOOK( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ReleaseHook );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ReleaseHook, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ReleaseHook() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ReleaseHook()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32RELEASEHOOK( HAB, HMQ, LONG, PFN, HMODULE );
PCHAR szWin32ReleaseHook = "Win32ReleaseHook";

BOOL APIENTRY Trc_Win32ReleaseHook( HAB     Parm1
                                  , HMQ     Parm2
                                  , LONG    Parm3
                                  , PFN     Parm4
                                  , HMODULE Parm5
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ReleaseHook );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PFN );
         T_LogPFN( Parm4 );
         T_LogParameter( 5L, DT_HMODULE );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szWin32ReleaseHook, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32RELEASEHOOK( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ReleaseHook );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ReleaseHook, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ReleaseHook() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ReleasePS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINRELEASEPS( HPS );
PCHAR szWin16ReleasePS = "Win16ReleasePS";

BOOL16 APIENTRY16 Trc_Win16ReleasePS( HPS Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ReleasePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16ReleasePS, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINRELEASEPS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ReleasePS );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ReleasePS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ReleasePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ReleasePS()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32RELEASEPS( HPS );
PCHAR szWin32ReleasePS = "Win32ReleasePS";

BOOL APIENTRY Trc_Win32ReleasePS( HPS Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ReleasePS );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32ReleasePS, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32RELEASEPS( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ReleasePS );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ReleasePS, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ReleasePS() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16RemovePresParam()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINREMOVEPRESPARAM( HWND, ULONG );
PCHAR szWin16RemovePresParam = "Win16RemovePresParam";

BOOL16 APIENTRY16 Trc_Win16RemovePresParam( HWND  Parm1
                                          , ULONG Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16RemovePresParam );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16RemovePresParam, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINREMOVEPRESPARAM( Parm1, Parm2 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16RemovePresParam );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16RemovePresParam, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16RemovePresParam() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RemovePresParam()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32REMOVEPRESPARAM( HWND, ULONG );
PCHAR szWin32RemovePresParam = "Win32RemovePresParam";

BOOL APIENTRY Trc_Win32RemovePresParam( HWND  Parm1
                                      , ULONG Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32RemovePresParam );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32RemovePresParam, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32REMOVEPRESPARAM( Parm1, Parm2 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32RemovePresParam );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32RemovePresParam, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32RemovePresParam() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ReplyMsg()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINREPLYMSG( HAB, HMQ, HMQ, MRESULT16 );
PCHAR szWin16ReplyMsg = "Win16ReplyMsg";

BOOL16 APIENTRY16 Trc_Win16ReplyMsg( HAB       Parm1
                                   , HMQ       Parm2
                                   , HMQ       Parm3
                                   , MRESULT16 Parm4
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ReplyMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HMQ );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_MRESULT );
         T_LogULONG( (ULONG)Parm4 );
      }
      HOOKENTRY16( szWin16ReplyMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINREPLYMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ReplyMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ReplyMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ReplyMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ReplyMsg()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32REPLYMSG( HAB, HMQ, HMQ, MRESULT );
PCHAR szWin32ReplyMsg = "Win32ReplyMsg";

BOOL APIENTRY Trc_Win32ReplyMsg( HAB     Parm1
                               , HMQ     Parm2
                               , HMQ     Parm3
                               , MRESULT Parm4
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ReplyMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HMQ );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_MRESULT );
         T_LogULONG( (ULONG)Parm4 );
      }
      HOOKENTRY( szWin32ReplyMsg, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32REPLYMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ReplyMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ReplyMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ReplyMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32RequestMutexSem()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY WIN32REQUESTMUTEXSEM( HMTX, ULONG );
PCHAR szWin32RequestMutexSem = "Win32RequestMutexSem";

APIRET APIENTRY Trc_Win32RequestMutexSem( HMTX  Parm1
                                        , ULONG Parm2
                                        )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32RequestMutexSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMTX );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32RequestMutexSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   ulRC = WIN32REQUESTMUTEXSEM( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin32RequestMutexSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szWin32RequestMutexSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32RequestMutexSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SaveWindowPos()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSAVEWINDOWPOS( HSAVEWP, PSWP16, USHORT );
PCHAR szWin16SaveWindowPos = "Win16SaveWindowPos";

BOOL16 APIENTRY16 Trc_Win16SaveWindowPos( HSAVEWP Parm1
                                        , PSWP16  Parm2
                                        , USHORT  Parm3
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SaveWindowPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSAVEWP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogSWPBuffer16( Parm2, Parm3 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SaveWindowPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSAVEWINDOWPOS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SaveWindowPos );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SaveWindowPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SaveWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SaveWindowPos()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SAVEWINDOWPOS( HSAVEWP, PSWP, ULONG );
PCHAR szWin32SaveWindowPos = "Win32SaveWindowPos";

BOOL APIENTRY Trc_Win32SaveWindowPos( HSAVEWP Parm1
                                    , PSWP    Parm2
                                    , ULONG   Parm3
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SaveWindowPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HSAVEWP );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogSWPBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SaveWindowPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SAVEWINDOWPOS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_FRAM ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SaveWindowPos );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SaveWindowPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SaveWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ScrollWindow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINSCROLLWINDOW( HWND, SHORT, SHORT, PRECTL16, PRECTL16, HRGN, PRECTL16, USHORT );
PCHAR szWin16ScrollWindow = "Win16ScrollWindow";

SHORT APIENTRY16 Trc_Win16ScrollWindow( HWND     Parm1
                                      , SHORT    Parm2
                                      , SHORT    Parm3
                                      , PRECTL16 Parm4
                                      , PRECTL16 Parm5
                                      , HRGN     Parm6
                                      , PRECTL16 Parm7
                                      , USHORT   Parm8
                                      )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ScrollWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL16( Parm4 );
         T_LogParameter( 5L, DT_PRECTL );
         T_LogPRECTL16( Parm5 );
         T_LogParameter( 6L, DT_HRGN );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PRECTL );
         T_LogPRECTL16( Parm7 );
         T_LogParameter( 8L, DT_USHORT );
         T_LogUSHORT( Parm8 );
      }
      HOOKENTRY16( szWin16ScrollWindow, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINSCROLLWINDOW( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ScrollWindow );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC != RGN_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PRECTL );
         if ( sRC != RGN_ERROR ) T_LogPRECTL16( Parm7 );
         else T_LogPVOID16( Parm7 );
      }
      HOOKEXIT16( szWin16ScrollWindow, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16ScrollWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ScrollWindow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32SCROLLWINDOW( HWND, LONG, LONG, PRECTL, PRECTL, HRGN, PRECTL, ULONG );
PCHAR szWin32ScrollWindow = "Win32ScrollWindow";

LONG APIENTRY Trc_Win32ScrollWindow( HWND   Parm1
                                   , LONG   Parm2
                                   , LONG   Parm3
                                   , PRECTL Parm4
                                   , PRECTL Parm5
                                   , HRGN   Parm6
                                   , PRECTL Parm7
                                   , ULONG  Parm8
                                   )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ScrollWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL( Parm4 );
         T_LogParameter( 5L, DT_PRECTL );
         T_LogPRECTL( Parm5 );
         T_LogParameter( 6L, DT_HRGN );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_PRECTL );
         T_LogPRECTL( Parm7 );
         T_LogParameter( 8L, DT_ULONG );
         T_LogULONG( Parm8 );
      }
      HOOKENTRY( szWin32ScrollWindow, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32SCROLLWINDOW( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ScrollWindow );
      T_LogPMReturnCode( DT_LONG, lRC, lRC != RGN_ERROR );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 7L, DT_PRECTL );
         if ( lRC != RGN_ERROR ) T_LogPRECTL( Parm7 );
         else T_LogPVOID( Parm7 );
      }
      HOOKEXIT( szWin32ScrollWindow, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32ScrollWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SendDlgItemMsg()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
MRESULT16 APIENTRY16 WINSENDDLGITEMMSG( HWND, USHORT, USHORT, MPARAM, MPARAM );
PCHAR szWin16SendDlgItemMsg = "Win16SendDlgItemMsg";

MRESULT16 APIENTRY16 Trc_Win16SendDlgItemMsg( HWND   Parm1
                                            , USHORT Parm2
                                            , USHORT Parm3
                                            , MPARAM Parm4
                                            , MPARAM Parm5
                                            )
{

   MRESULT16 ulRC;                               /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SendDlgItemMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogPMMessage16( Parm3 );
         T_LogParameter( 4L, DT_MPARAM );
         T_LogULONG( (ULONG)Parm4 );
         T_LogParameter( 5L, DT_MPARAM );
         T_LogULONG( (ULONG)Parm5 );
      }
      HOOKENTRY16( szWin16SendDlgItemMsg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINSENDDLGITEMMSG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SendDlgItemMsg );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT16( szWin16SendDlgItemMsg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16SendDlgItemMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SendDlgItemMsg()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY WIN32SENDDLGITEMMSG( HWND, ULONG, ULONG, MPARAM, MPARAM );
PCHAR szWin32SendDlgItemMsg = "Win32SendDlgItemMsg";

MRESULT APIENTRY Trc_Win32SendDlgItemMsg( HWND   Parm1
                                        , ULONG  Parm2
                                        , ULONG  Parm3
                                        , MPARAM Parm4
                                        , MPARAM Parm5
                                        )
{

   MRESULT ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SendDlgItemMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogPMMessage( Parm3 );
         T_LogParameter( 4L, DT_MPARAM );
         T_LogULONG( (ULONG)Parm4 );
         T_LogParameter( 5L, DT_MPARAM );
         T_LogULONG( (ULONG)Parm5 );
      }
      HOOKENTRY( szWin32SendDlgItemMsg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32SENDDLGITEMMSG( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SendDlgItemMsg );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT( szWin32SendDlgItemMsg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32SendDlgItemMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SendMsg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
MRESULT16 APIENTRY16 WINSENDMSG( HWND, USHORT, MPARAM, MPARAM );
PCHAR szWin16SendMsg = "Win16SendMsg";

MRESULT16 APIENTRY16 Trc_Win16SendMsg( HWND   Parm1
                                     , USHORT Parm2
                                     , MPARAM Parm3
                                     , MPARAM Parm4
                                     )
{

   MRESULT16 ulRC;                               /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SendMsg );
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
      HOOKENTRY16( szWin16SendMsg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINSENDMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SendMsg );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT16( szWin16SendMsg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16SendMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SendMsg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
MRESULT APIENTRY WIN32SENDMSG( HWND, ULONG, MPARAM, MPARAM );
PCHAR szWin32SendMsg = "Win32SendMsg";

MRESULT APIENTRY Trc_Win32SendMsg( HWND   Parm1
                                 , ULONG  Parm2
                                 , MPARAM Parm3
                                 , MPARAM Parm4
                                 )
{

   MRESULT ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SendMsg );
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
      HOOKENTRY( szWin32SendMsg, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32SENDMSG( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SendMsg );
      T_LogPMReturnCodeIndeterminate( DT_MRESULT, (ULONG)ulRC );
      HOOKEXIT( szWin32SendMsg, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32SendMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetAccelTable()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETACCELTABLE( HAB, HACCEL, HWND );
PCHAR szWin16SetAccelTable = "Win16SetAccelTable";

BOOL16 APIENTRY16 Trc_Win16SetAccelTable( HAB    Parm1
                                        , HACCEL Parm2
                                        , HWND   Parm3
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HACCEL );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HWND );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szWin16SetAccelTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETACCELTABLE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetAccelTable );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetAccelTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetAccelTable()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETACCELTABLE( HAB, HACCEL, HWND );
PCHAR szWin32SetAccelTable = "Win32SetAccelTable";

BOOL APIENTRY Trc_Win32SetAccelTable( HAB    Parm1
                                    , HACCEL Parm2
                                    , HWND   Parm3
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetAccelTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HACCEL );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HWND );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetAccelTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETACCELTABLE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetAccelTable );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetAccelTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetAccelTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetActiveWindow()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETACTIVEWINDOW( HWND, HWND );
PCHAR szWin16SetActiveWindow = "Win16SetActiveWindow";

BOOL16 APIENTRY16 Trc_Win16SetActiveWindow( HWND Parm1
                                          , HWND Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetActiveWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16SetActiveWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETACTIVEWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetActiveWindow );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetActiveWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetActiveWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetActiveWindow()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETACTIVEWINDOW( HWND, HWND );
PCHAR szWin32SetActiveWindow = "Win32SetActiveWindow";

BOOL APIENTRY Trc_Win32SetActiveWindow( HWND Parm1
                                      , HWND Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetActiveWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetActiveWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETACTIVEWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetActiveWindow );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetActiveWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetActiveWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetCapture()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETCAPTURE( HWND, HWND );
PCHAR szWin16SetCapture = "Win16SetCapture";

BOOL16 APIENTRY16 Trc_Win16SetCapture( HWND Parm1
                                     , HWND Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetCapture );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16SetCapture, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETCAPTURE( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetCapture );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetCapture, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetCapture() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetCapture()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETCAPTURE( HWND, HWND );
PCHAR szWin32SetCapture = "Win32SetCapture";

BOOL APIENTRY Trc_Win32SetCapture( HWND Parm1
                                 , HWND Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetCapture );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetCapture, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETCAPTURE( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetCapture );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetCapture, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetCapture() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetClassMsgInterest()                                             */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETCLASSMSGINTEREST( HAB, PSZ16, USHORT, SHORT );
PCHAR szWin16SetClassMsgInterest = "Win16SetClassMsgInterest";

BOOL16 APIENTRY16 Trc_Win16SetClassMsgInterest( HAB    Parm1
                                              , PSZ16  Parm2
                                              , USHORT Parm3
                                              , SHORT  Parm4
                                              )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetClassMsgInterest );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogPMMessage16( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16SetClassMsgInterest, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETCLASSMSGINTEREST( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetClassMsgInterest );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetClassMsgInterest, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetClassMsgInterest() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetClassMsgInterest()                                             */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETCLASSMSGINTEREST( HAB, PSZ, ULONG, LONG );
PCHAR szWin32SetClassMsgInterest = "Win32SetClassMsgInterest";

BOOL APIENTRY Trc_Win32SetClassMsgInterest( HAB   Parm1
                                          , PSZ   Parm2
                                          , ULONG Parm3
                                          , LONG  Parm4
                                          )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetClassMsgInterest );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogPMMessage( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32SetClassMsgInterest, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETCLASSMSGINTEREST( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetClassMsgInterest );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetClassMsgInterest, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetClassMsgInterest() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetClassThunkProc()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETCLASSTHUNKPROC( PSZ16, PFN16 );
PCHAR szWin16SetClassThunkProc = "Win16SetClassThunkProc";

BOOL16 APIENTRY16 Trc_Win16SetClassThunkProc( PSZ16 Parm1
                                            , PFN16 Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetClassThunkProc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ16( Parm1 );
         T_LogParameter( 2L, DT_PFN );
         T_LogPFN16( Parm2 );
      }
      HOOKENTRY16( szWin16SetClassThunkProc, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETCLASSTHUNKPROC( Parm1, Parm2 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetClassThunkProc );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetClassThunkProc, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetClassThunkProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetClassThunkProc()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETCLASSTHUNKPROC( PSZ, PFN );
PCHAR szWin32SetClassThunkProc = "Win32SetClassThunkProc";

BOOL APIENTRY Trc_Win32SetClassThunkProc( PSZ Parm1
                                        , PFN Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetClassThunkProc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PSZ );
         T_LogPSZ( Parm1 );
         T_LogParameter( 2L, DT_PFN );
         T_LogPFN( Parm2 );
      }
      HOOKENTRY( szWin32SetClassThunkProc, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETCLASSTHUNKPROC( Parm1, Parm2 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetClassThunkProc );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetClassThunkProc, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetClassThunkProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetClipbrdData()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETCLIPBRDDATA( HAB, ULONG, USHORT, USHORT );
PCHAR szWin16SetClipbrdData = "Win16SetClipbrdData";

BOOL16 APIENTRY16 Trc_Win16SetClipbrdData( HAB    Parm1
                                         , ULONG  Parm2
                                         , USHORT Parm3
                                         , USHORT Parm4
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetClipbrdData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16SetClipbrdData, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETCLIPBRDDATA( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetClipbrdData );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetClipbrdData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetClipbrdData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetClipbrdData()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETCLIPBRDDATA( HAB, ULONG, ULONG, ULONG );
PCHAR szWin32SetClipbrdData = "Win32SetClipbrdData";

BOOL APIENTRY Trc_Win32SetClipbrdData( HAB   Parm1
                                     , ULONG Parm2
                                     , ULONG Parm3
                                     , ULONG Parm4
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetClipbrdData );
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
      }
      HOOKENTRY( szWin32SetClipbrdData, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETCLIPBRDDATA( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetClipbrdData );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetClipbrdData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetClipbrdData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetClipbrdOwner()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETCLIPBRDOWNER( HAB, HWND );
PCHAR szWin16SetClipbrdOwner = "Win16SetClipbrdOwner";

BOOL16 APIENTRY16 Trc_Win16SetClipbrdOwner( HAB  Parm1
                                          , HWND Parm2
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetClipbrdOwner );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16SetClipbrdOwner, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETCLIPBRDOWNER( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetClipbrdOwner );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetClipbrdOwner, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetClipbrdOwner() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetClipbrdOwner()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETCLIPBRDOWNER( HAB, HWND );
PCHAR szWin32SetClipbrdOwner = "Win32SetClipbrdOwner";

BOOL APIENTRY Trc_Win32SetClipbrdOwner( HAB  Parm1
                                      , HWND Parm2
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetClipbrdOwner );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetClipbrdOwner, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETCLIPBRDOWNER( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetClipbrdOwner );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetClipbrdOwner, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetClipbrdOwner() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetClipbrdViewer()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETCLIPBRDVIEWER( HAB, HWND );
PCHAR szWin16SetClipbrdViewer = "Win16SetClipbrdViewer";

BOOL16 APIENTRY16 Trc_Win16SetClipbrdViewer( HAB  Parm1
                                           , HWND Parm2
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetClipbrdViewer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16SetClipbrdViewer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETCLIPBRDVIEWER( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetClipbrdViewer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetClipbrdViewer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetClipbrdViewer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetClipbrdViewer()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETCLIPBRDVIEWER( HAB, HWND );
PCHAR szWin32SetClipbrdViewer = "Win32SetClipbrdViewer";

BOOL APIENTRY Trc_Win32SetClipbrdViewer( HAB  Parm1
                                       , HWND Parm2
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetClipbrdViewer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetClipbrdViewer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETCLIPBRDVIEWER( Parm1, Parm2 );

   if ( LOGWINAPI( W_CLIP ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetClipbrdViewer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetClipbrdViewer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetClipbrdViewer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetClipRegion()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( OS2VER >= 240 ) )
BOOL APIENTRY WIN32SETCLIPREGION( HWND, HRGN );
PCHAR szWin32SetClipRegion = "Win32SetClipRegion";

BOOL APIENTRY Trc_Win32SetClipRegion( HWND Parm1
                                    , HRGN Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetClipRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetClipRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnWIN32SETCLIPREGION )( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetClipRegion );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetClipRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetClipRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETCP( HMQ, USHORT );
PCHAR szWin16SetCp = "Win16SetCp";

BOOL16 APIENTRY16 Trc_Win16SetCp( HMQ    Parm1
                                , USHORT Parm2
                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16SetCp, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETCP( Parm1, Parm2 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetCp );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetCp, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetCp()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETCP( HMQ, ULONG );
PCHAR szWin32SetCp = "Win32SetCp";

BOOL APIENTRY Trc_Win32SetCp( HMQ   Parm1
                            , ULONG Parm2
                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetCp );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetCp, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETCP( Parm1, Parm2 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetCp );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetCp, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetCp() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetDesktopBkgnd()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HBITMAP APIENTRY16 WINSETDESKTOPBKGND( HWND, PDESKTOP16 );
PCHAR szWin16SetDesktopBkgnd = "Win16SetDesktopBkgnd";

HBITMAP APIENTRY16 Trc_Win16SetDesktopBkgnd( HWND       Parm1
                                           , PDESKTOP16 Parm2
                                           )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_DESK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetDesktopBkgnd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PDESKTOP );
         T_LogPDESKTOP16( Parm2 );
      }
      HOOKENTRY16( szWin16SetDesktopBkgnd, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINSETDESKTOPBKGND( Parm1, Parm2 );

   if ( LOGWINAPI( W_DESK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetDesktopBkgnd );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC );
      HOOKEXIT16( szWin16SetDesktopBkgnd, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16SetDesktopBkgnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetDesktopBkgnd()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HBITMAP APIENTRY WIN32SETDESKTOPBKGND( HWND, PDESKTOP );
PCHAR szWin32SetDesktopBkgnd = "Win32SetDesktopBkgnd";

HBITMAP APIENTRY Trc_Win32SetDesktopBkgnd( HWND     Parm1
                                         , PDESKTOP Parm2
                                         )
{

   HBITMAP ulRC;                                 /* API return code           */

   if ( LOGWINAPI( W_DESK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetDesktopBkgnd );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PDESKTOP );
         T_LogPDESKTOP( Parm2 );
      }
      HOOKENTRY( szWin32SetDesktopBkgnd, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32SETDESKTOPBKGND( Parm1, Parm2 );

   if ( LOGWINAPI( W_DESK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetDesktopBkgnd );
      T_LogPMReturnCode( DT_HBITMAP, ulRC, ulRC );
      HOOKEXIT( szWin32SetDesktopBkgnd, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32SetDesktopBkgnd() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetDlgItemShort()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETDLGITEMSHORT( HWND, USHORT, USHORT, BOOL16 );
PCHAR szWin16SetDlgItemShort = "Win16SetDlgItemShort";

BOOL16 APIENTRY16 Trc_Win16SetDlgItemShort( HWND   Parm1
                                          , USHORT Parm2
                                          , USHORT Parm3
                                          , BOOL16 Parm4
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetDlgItemShort );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_BOOL );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16SetDlgItemShort, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETDLGITEMSHORT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetDlgItemShort );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetDlgItemShort, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetDlgItemShort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetDlgItemShort()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETDLGITEMSHORT( HWND, ULONG, USHORT, BOOL );
PCHAR szWin32SetDlgItemShort = "Win32SetDlgItemShort";

BOOL APIENTRY Trc_Win32SetDlgItemShort( HWND   Parm1
                                      , ULONG  Parm2
                                      , USHORT Parm3
                                      , BOOL   Parm4
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetDlgItemShort );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_BOOL );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32SetDlgItemShort, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETDLGITEMSHORT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetDlgItemShort );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetDlgItemShort, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetDlgItemShort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetDlgItemText()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETDLGITEMTEXT( HWND, USHORT, PSZ16 );
PCHAR szWin16SetDlgItemText = "Win16SetDlgItemText";

BOOL16 APIENTRY16 Trc_Win16SetDlgItemText( HWND   Parm1
                                         , USHORT Parm2
                                         , PSZ16  Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetDlgItemText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ16( Parm3 );
      }
      HOOKENTRY16( szWin16SetDlgItemText, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETDLGITEMTEXT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetDlgItemText );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetDlgItemText, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetDlgItemText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetDlgItemText()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETDLGITEMTEXT( HWND, ULONG, PSZ );
PCHAR szWin32SetDlgItemText = "Win32SetDlgItemText";

BOOL APIENTRY Trc_Win32SetDlgItemText( HWND  Parm1
                                     , ULONG Parm2
                                     , PSZ   Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetDlgItemText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PSZ );
         T_LogPSZ( Parm3 );
      }
      HOOKENTRY( szWin32SetDlgItemText, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETDLGITEMTEXT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetDlgItemText );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetDlgItemText, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetDlgItemText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetFocus()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETFOCUS( HWND, HWND );
PCHAR szWin16SetFocus = "Win16SetFocus";

BOOL16 APIENTRY16 Trc_Win16SetFocus( HWND Parm1
                                   , HWND Parm2
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetFocus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16SetFocus, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETFOCUS( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetFocus );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetFocus, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetFocus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetFocus()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETFOCUS( HWND, HWND );
PCHAR szWin32SetFocus = "Win32SetFocus";

BOOL APIENTRY Trc_Win32SetFocus( HWND Parm1
                               , HWND Parm2
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetFocus );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetFocus, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETFOCUS( Parm1, Parm2 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetFocus );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetFocus, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetFocus() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetHook()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETHOOK( HAB, HMQ, SHORT, PFN16, HMODULE16 );
PCHAR szWin16SetHook = "Win16SetHook";

BOOL16 APIENTRY16 Trc_Win16SetHook( HAB       Parm1
                                  , HMQ       Parm2
                                  , SHORT     Parm3
                                  , PFN16     Parm4
                                  , HMODULE16 Parm5
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetHook );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PFN );
         T_LogPFN16( Parm4 );
         T_LogParameter( 5L, DT_HMODULE );
         T_LogUSHORT( Parm5 );
      }
      HOOKENTRY16( szWin16SetHook, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETHOOK( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetHook );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetHook, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetHook() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetHook()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETHOOK( HAB, HMQ, LONG, PFN, HMODULE );
PCHAR szWin32SetHook = "Win32SetHook";

BOOL APIENTRY Trc_Win32SetHook( HAB     Parm1
                              , HMQ     Parm2
                              , LONG    Parm3
                              , PFN     Parm4
                              , HMODULE Parm5
                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetHook );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PFN );
         T_LogPFN( Parm4 );
         T_LogParameter( 5L, DT_HMODULE );
         T_LogULONG( Parm5 );
      }
      HOOKENTRY( szWin32SetHook, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETHOOK( Parm1, Parm2, Parm3, Parm4, Parm5 );

   if ( LOGWINAPI( W_HOOK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetHook );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetHook, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetHook() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetKeyboardStateTable()                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETKEYBOARDSTATETABLE( HWND, PBYTE16, BOOL16 );
PCHAR szWin16SetKeyboardStateTable = "Win16SetKeyboardStateTable";

BOOL16 APIENTRY16 Trc_Win16SetKeyboardStateTable( HWND    Parm1
                                                , PBYTE16 Parm2
                                                , BOOL16  Parm3
                                                )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetKeyboardStateTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID16( Parm2 );
         if ( Parm3 ) T_LogCHARBuffer16( (PCHAR)Parm2, VK_USERFIRST );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SetKeyboardStateTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETKEYBOARDSTATETABLE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetKeyboardStateTable );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID16( Parm2 );
         if ( ( !Parm3 ) && ( bRC ) ) T_LogCHARBuffer16( (PCHAR)Parm2, VK_USERFIRST );
      }
      HOOKEXIT16( szWin16SetKeyboardStateTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetKeyboardStateTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetKeyboardStateTable()                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETKEYBOARDSTATETABLE( HWND, PBYTE, BOOL );
PCHAR szWin32SetKeyboardStateTable = "Win32SetKeyboardStateTable";

BOOL APIENTRY Trc_Win32SetKeyboardStateTable( HWND  Parm1
                                            , PBYTE Parm2
                                            , BOOL  Parm3
                                            )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetKeyboardStateTable );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID( Parm2 );
         if ( Parm3 ) T_LogCHARBuffer( (PCHAR)Parm2, VK_USERFIRST );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetKeyboardStateTable, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETKEYBOARDSTATETABLE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_INPT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetKeyboardStateTable );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PBYTE );
         T_LogPVOID( Parm2 );
         if ( ( !Parm3 ) && ( bRC ) ) T_LogCHARBuffer( (PCHAR)Parm2, VK_USERFIRST );
      }
      HOOKEXIT( szWin32SetKeyboardStateTable, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetKeyboardStateTable() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetMsgInterest()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETMSGINTEREST( HWND, USHORT, SHORT );
PCHAR szWin16SetMsgInterest = "Win16SetMsgInterest";

BOOL16 APIENTRY16 Trc_Win16SetMsgInterest( HWND   Parm1
                                         , USHORT Parm2
                                         , SHORT  Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetMsgInterest );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogPMMessage16( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SetMsgInterest, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETMSGINTEREST( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetMsgInterest );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetMsgInterest, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetMsgInterest() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetMsgInterest()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETMSGINTEREST( HWND, ULONG, LONG );
PCHAR szWin32SetMsgInterest = "Win32SetMsgInterest";

BOOL APIENTRY Trc_Win32SetMsgInterest( HWND  Parm1
                                     , ULONG Parm2
                                     , LONG  Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetMsgInterest );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogPMMessage( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetMsgInterest, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETMSGINTEREST( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetMsgInterest );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetMsgInterest, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetMsgInterest() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetMsgMode()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETMSGMODE( HAB, PSZ16, SHORT );
PCHAR szWin16SetMsgMode = "Win16SetMsgMode";

BOOL16 APIENTRY16 Trc_Win16SetMsgMode( HAB   Parm1
                                     , PSZ16 Parm2
                                     , SHORT Parm3
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetMsgMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SetMsgMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETMSGMODE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetMsgMode );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetMsgMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetMsgMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetMsgMode()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETMSGMODE( HAB, PSZ, LONG );
PCHAR szWin32SetMsgMode = "Win32SetMsgMode";

BOOL APIENTRY Trc_Win32SetMsgMode( HAB  Parm1
                                 , PSZ  Parm2
                                 , LONG Parm3
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetMsgMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetMsgMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETMSGMODE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetMsgMode );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetMsgMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetMsgMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetMultWindowPos()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETMULTWINDOWPOS( HAB, PSWP16, USHORT );
PCHAR szWin16SetMultWindowPos = "Win16SetMultWindowPos";

BOOL16 APIENTRY16 Trc_Win16SetMultWindowPos( HAB    Parm1
                                           , PSWP16 Parm2
                                           , USHORT Parm3
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetMultWindowPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogSWPBuffer16( Parm2, Parm3 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SetMultWindowPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETMULTWINDOWPOS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetMultWindowPos );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetMultWindowPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetMultWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetMultWindowPos()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETMULTWINDOWPOS( HAB, PSWP, ULONG );
PCHAR szWin32SetMultWindowPos = "Win32SetMultWindowPos";

BOOL APIENTRY Trc_Win32SetMultWindowPos( HAB   Parm1
                                       , PSWP  Parm2
                                       , ULONG Parm3
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetMultWindowPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSWP );
         T_LogSWPBuffer( Parm2, Parm3 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetMultWindowPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETMULTWINDOWPOS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetMultWindowPos );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetMultWindowPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetMultWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetOwner()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETOWNER( HWND, HWND );
PCHAR szWin16SetOwner = "Win16SetOwner";

BOOL16 APIENTRY16 Trc_Win16SetOwner( HWND Parm1
                                   , HWND Parm2
                                   )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetOwner );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16SetOwner, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETOWNER( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetOwner );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetOwner, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetOwner() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetOwner()                                                        */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETOWNER( HWND, HWND );
PCHAR szWin32SetOwner = "Win32SetOwner";

BOOL APIENTRY Trc_Win32SetOwner( HWND Parm1
                               , HWND Parm2
                               )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetOwner );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetOwner, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETOWNER( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetOwner );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetOwner, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetOwner() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetParent()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETPARENT( HWND, HWND, BOOL16 );
PCHAR szWin16SetParent = "Win16SetParent";

BOOL16 APIENTRY16 Trc_Win16SetParent( HWND   Parm1
                                    , HWND   Parm2
                                    , BOOL16 Parm3
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetParent );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SetParent, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETPARENT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetParent );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetParent, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetParent() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetParent()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETPARENT( HWND, HWND, BOOL );
PCHAR szWin32SetParent = "Win32SetParent";

BOOL APIENTRY Trc_Win32SetParent( HWND Parm1
                                , HWND Parm2
                                , BOOL Parm3
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetParent );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetParent, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETPARENT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetParent );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetParent, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetParent() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetPointer()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETPOINTER( HWND, HPOINTER );
PCHAR szWin16SetPointer = "Win16SetPointer";

BOOL16 APIENTRY16 Trc_Win16SetPointer( HWND     Parm1
                                     , HPOINTER Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPOINTER );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16SetPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETPOINTER( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetPointer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetPointer()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETPOINTER( HWND, HPOINTER );
PCHAR szWin32SetPointer = "Win32SetPointer";

BOOL APIENTRY Trc_Win32SetPointer( HWND     Parm1
                                 , HPOINTER Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPOINTER );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETPOINTER( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetPointer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetPointerOwner()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETPOINTEROWNER( HPOINTER, PID16, BOOL16 );
PCHAR szWin16SetPointerOwner = "Win16SetPointerOwner";

BOOL16 APIENTRY16 Trc_Win16SetPointerOwner( HPOINTER Parm1
                                          , PID16    Parm2
                                          , BOOL16   Parm3
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetPointerOwner );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPOINTER );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SetPointerOwner, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETPOINTEROWNER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetPointerOwner );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetPointerOwner, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetPointerOwner() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetPointerOwner()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETPOINTEROWNER( HPOINTER, PID, BOOL );
PCHAR szWin32SetPointerOwner = "Win32SetPointerOwner";

BOOL APIENTRY Trc_Win32SetPointerOwner( HPOINTER Parm1
                                      , PID      Parm2
                                      , BOOL     Parm3
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetPointerOwner );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPOINTER );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PID );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetPointerOwner, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETPOINTEROWNER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetPointerOwner );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetPointerOwner, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetPointerOwner() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetPointerPos()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETPOINTERPOS( HWND, SHORT, SHORT );
PCHAR szWin16SetPointerPos = "Win16SetPointerPos";

BOOL16 APIENTRY16 Trc_Win16SetPointerPos( HWND  Parm1
                                        , SHORT Parm2
                                        , SHORT Parm3
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetPointerPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SetPointerPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETPOINTERPOS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetPointerPos );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetPointerPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetPointerPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetPointerPos()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETPOINTERPOS( HWND, LONG, LONG );
PCHAR szWin32SetPointerPos = "Win32SetPointerPos";

BOOL APIENTRY Trc_Win32SetPointerPos( HWND Parm1
                                    , LONG Parm2
                                    , LONG Parm3
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetPointerPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetPointerPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETPOINTERPOS( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetPointerPos );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetPointerPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetPointerPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetPresParam()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETPRESPARAM( HWND, ULONG, ULONG, PVOID16 );
PCHAR szWin16SetPresParam = "Win16SetPresParam";

BOOL16 APIENTRY16 Trc_Win16SetPresParam( HWND    Parm1
                                       , ULONG   Parm2
                                       , ULONG   Parm3
                                       , PVOID16 Parm4
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetPresParam );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID16( Parm4 );
         T_LogCHARBuffer16( Parm4, Parm3 );
      }
      HOOKENTRY16( szWin16SetPresParam, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETPRESPARAM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetPresParam );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetPresParam, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetPresParam() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetPresParam()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETPRESPARAM( HWND, ULONG, ULONG, PVOID );
PCHAR szWin32SetPresParam = "Win32SetPresParam";

BOOL APIENTRY Trc_Win32SetPresParam( HWND  Parm1
                                   , ULONG Parm2
                                   , ULONG Parm3
                                   , PVOID Parm4
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetPresParam );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PVOID );
         T_LogPVOID( Parm4 );
         T_LogCHARBuffer( Parm4, Parm3 );
      }
      HOOKENTRY( szWin32SetPresParam, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETPRESPARAM( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetPresParam );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetPresParam, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetPresParam() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetRect()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETRECT( HAB, PRECTL16, SHORT, SHORT, SHORT, SHORT );
PCHAR szWin16SetRect = "Win16SetRect";

BOOL16 APIENTRY16 Trc_Win16SetRect( HAB      Parm1
                                  , PRECTL16 Parm2
                                  , SHORT    Parm3
                                  , SHORT    Parm4
                                  , SHORT    Parm5
                                  , SHORT    Parm6
                                  )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_SHORT );
         T_LogUSHORT( Parm6 );
      }
      HOOKENTRY16( szWin16SetRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETRECT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKEXIT16( szWin16SetRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetRect()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETRECT( HAB, PRECTL, LONG, LONG, LONG, LONG );
PCHAR szWin32SetRect = "Win32SetRect";

BOOL APIENTRY Trc_Win32SetRect( HAB    Parm1
                              , PRECTL Parm2
                              , LONG   Parm3
                              , LONG   Parm4
                              , LONG   Parm5
                              , LONG   Parm6
                              )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
      }
      HOOKENTRY( szWin32SetRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETRECT( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKEXIT( szWin32SetRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetRectEmpty()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETRECTEMPTY( HAB, PRECTL16 );
PCHAR szWin16SetRectEmpty = "Win16SetRectEmpty";

BOOL16 APIENTRY16 Trc_Win16SetRectEmpty( HAB      Parm1
                                       , PRECTL16 Parm2
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetRectEmpty );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKENTRY16( szWin16SetRectEmpty, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETRECTEMPTY( Parm1, Parm2 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetRectEmpty );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKEXIT16( szWin16SetRectEmpty, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetRectEmpty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetRectEmpty()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETRECTEMPTY( HAB, PRECTL );
PCHAR szWin32SetRectEmpty = "Win32SetRectEmpty";

BOOL APIENTRY Trc_Win32SetRectEmpty( HAB    Parm1
                                   , PRECTL Parm2
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetRectEmpty );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKENTRY( szWin32SetRectEmpty, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETRECTEMPTY( Parm1, Parm2 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetRectEmpty );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKEXIT( szWin32SetRectEmpty, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetRectEmpty() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetSynchroMode()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETSYNCHROMODE( HAB, SHORT );
PCHAR szWin16SetSynchroMode = "Win16SetSynchroMode";

BOOL16 APIENTRY16 Trc_Win16SetSynchroMode( HAB   Parm1
                                         , SHORT Parm2
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetSynchroMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16SetSynchroMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETSYNCHROMODE( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetSynchroMode );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetSynchroMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetSynchroMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetSynchroMode()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETSYNCHROMODE( HAB, LONG );
PCHAR szWin32SetSynchroMode = "Win32SetSynchroMode";

BOOL APIENTRY Trc_Win32SetSynchroMode( HAB  Parm1
                                     , LONG Parm2
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetSynchroMode );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetSynchroMode, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETSYNCHROMODE( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetSynchroMode );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetSynchroMode, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetSynchroMode() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetSysColors()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETSYSCOLORS( HWND, ULONG, ULONG, LONG, ULONG, PLONG );
PCHAR szWin16SetSysColors = "Win16SetSysColors";

BOOL16 APIENTRY16 Trc_Win16SetSysColors( HWND    Parm1
                                       , ULONG   Parm2
                                       , ULONG   Parm3
                                       , LONG    Parm4
                                       , ULONG   Parm5
                                       , PLONG16 Parm6
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetSysColors );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID16( Parm6 );
         T_LogULONGBuffer16( (PULONG)Parm6, Parm5 );
      }
      HOOKENTRY16( szWin16SetSysColors, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETSYSCOLORS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetSysColors );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetSysColors, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetSysColors() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetSysColors()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETSYSCOLORS( HWND, ULONG, ULONG, LONG, ULONG, PLONG );
PCHAR szWin32SetSysColors = "Win32SetSysColors";

BOOL APIENTRY Trc_Win32SetSysColors( HWND  Parm1
                                   , ULONG Parm2
                                   , ULONG Parm3
                                   , LONG  Parm4
                                   , ULONG Parm5
                                   , PLONG Parm6
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetSysColors );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_ULONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_PLONG );
         T_LogPVOID( Parm6 );
         T_LogULONGBuffer( (PULONG)Parm6, Parm5 );
      }
      HOOKENTRY( szWin32SetSysColors, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETSYSCOLORS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetSysColors );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetSysColors, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetSysColors() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetSysModalWindow()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETSYSMODALWINDOW( HWND, HWND );
PCHAR szWin16SetSysModalWindow = "Win16SetSysModalWindow";

BOOL16 APIENTRY16 Trc_Win16SetSysModalWindow( HWND Parm1
                                            , HWND Parm2
                                            )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetSysModalWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16SetSysModalWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETSYSMODALWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetSysModalWindow );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetSysModalWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetSysModalWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetSysModalWindow()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETSYSMODALWINDOW( HWND, HWND );
PCHAR szWin32SetSysModalWindow = "Win32SetSysModalWindow";

BOOL APIENTRY Trc_Win32SetSysModalWindow( HWND Parm1
                                        , HWND Parm2
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetSysModalWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetSysModalWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETSYSMODALWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetSysModalWindow );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetSysModalWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetSysModalWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetSysPointerData()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
BOOL APIENTRY WIN32SETSYSPOINTERDATA( HWND, ULONG, PICONINFO );
PCHAR szWin32SetSysPointerData = "Win32SetSysPointerData";

BOOL APIENTRY Trc_Win32SetSysPointerData( HWND      Parm1
                                        , ULONG     Parm2
                                        , PICONINFO Parm3
                                        )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetSysPointerData );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PICONINFO );
         T_LogPICONINFO( Parm3 );
      }
      HOOKENTRY( szWin32SetSysPointerData, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnWIN32SETSYSPOINTERDATA )( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetSysPointerData );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetSysPointerData, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetSysPointerData() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetSysValue()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETSYSVALUE( HWND, SHORT, LONG );
PCHAR szWin16SetSysValue = "Win16SetSysValue";

BOOL16 APIENTRY16 Trc_Win16SetSysValue( HWND  Parm1
                                      , SHORT Parm2
                                      , LONG  Parm3
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetSysValue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szWin16SetSysValue, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETSYSVALUE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetSysValue );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetSysValue, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetSysValue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetSysValue()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETSYSVALUE( HWND, LONG, LONG );
PCHAR szWin32SetSysValue = "Win32SetSysValue";

BOOL APIENTRY Trc_Win32SetSysValue( HWND Parm1
                                  , LONG Parm2
                                  , LONG Parm3
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetSysValue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetSysValue, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETSYSVALUE( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_SYS ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetSysValue );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetSysValue, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetSysValue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetVisibleRegionNotify()                                          */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 210 )
BOOL APIENTRY WIN32SETVISIBLEREGIONNOTIFY( HWND, BOOL );
PCHAR szWin32SetVisibleRegionNotify = "Win32SetVisibleRegionNotify";

BOOL APIENTRY Trc_Win32SetVisibleRegionNotify( HWND Parm1
                                             , BOOL Parm2
                                             )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetVisibleRegionNotify );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32SetVisibleRegionNotify, &Parm1 );
      T_FreeLog( );
   }

   bRC = ( *pfnWIN32SETVISIBLEREGIONNOTIFY )( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetVisibleRegionNotify );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetVisibleRegionNotify, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetVisibleRegionNotify() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetWindowBits()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETWINDOWBITS( HWND, SHORT, ULONG, ULONG );
PCHAR szWin16SetWindowBits = "Win16SetWindowBits";

BOOL16 APIENTRY16 Trc_Win16SetWindowBits( HWND  Parm1
                                        , SHORT Parm2
                                        , ULONG Parm3
                                        , ULONG Parm4
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetWindowBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY16( szWin16SetWindowBits, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETWINDOWBITS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetWindowBits );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetWindowBits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetWindowBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetWindowBits()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETWINDOWBITS( HWND, LONG, ULONG, ULONG );
PCHAR szWin32SetWindowBits = "Win32SetWindowBits";

BOOL APIENTRY Trc_Win32SetWindowBits( HWND  Parm1
                                    , LONG  Parm2
                                    , ULONG Parm3
                                    , ULONG Parm4
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetWindowBits );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32SetWindowBits, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETWINDOWBITS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetWindowBits );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetWindowBits, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetWindowBits() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetWindowPos()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETWINDOWPOS( HWND, HWND, SHORT, SHORT, SHORT, SHORT, USHORT );
PCHAR szWin16SetWindowPos = "Win16SetWindowPos";

BOOL16 APIENTRY16 Trc_Win16SetWindowPos( HWND   Parm1
                                       , HWND   Parm2
                                       , SHORT  Parm3
                                       , SHORT  Parm4
                                       , SHORT  Parm5
                                       , SHORT  Parm6
                                       , USHORT Parm7
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetWindowPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_SHORT );
         T_LogUSHORT( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szWin16SetWindowPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETWINDOWPOS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetWindowPos );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetWindowPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetWindowPos()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETWINDOWPOS( HWND, HWND, LONG, LONG, LONG, LONG, ULONG );
PCHAR szWin32SetWindowPos = "Win32SetWindowPos";

BOOL APIENTRY Trc_Win32SetWindowPos( HWND  Parm1
                                   , HWND  Parm2
                                   , LONG  Parm3
                                   , LONG  Parm4
                                   , LONG  Parm5
                                   , LONG  Parm6
                                   , ULONG Parm7
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetWindowPos );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_LONG );
         T_LogULONG( Parm4 );
         T_LogParameter( 5L, DT_LONG );
         T_LogULONG( Parm5 );
         T_LogParameter( 6L, DT_LONG );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_ULONG );
         T_LogULONG( Parm7 );
      }
      HOOKENTRY( szWin32SetWindowPos, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETWINDOWPOS( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetWindowPos );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetWindowPos, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetWindowPos() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetWindowPtr()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETWINDOWPTR( HWND, SHORT, PVOID16 );
PCHAR szWin16SetWindowPtr = "Win16SetWindowPtr";

BOOL16 APIENTRY16 Trc_Win16SetWindowPtr( HWND    Parm1
                                       , SHORT   Parm2
                                       , PVOID16 Parm3
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetWindowPtr );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID16( Parm3 );
         T_LogCHARBufferUnformatted16( Parm3 );
      }
      HOOKENTRY16( szWin16SetWindowPtr, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETWINDOWPTR( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetWindowPtr );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetWindowPtr, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetWindowPtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetWindowPtr()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETWINDOWPTR( HWND, LONG, PVOID );
PCHAR szWin32SetWindowPtr = "Win32SetWindowPtr";

BOOL APIENTRY Trc_Win32SetWindowPtr( HWND  Parm1
                                   , LONG  Parm2
                                   , PVOID Parm3
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetWindowPtr );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PVOID );
         T_LogPVOID( Parm3 );
         T_LogCHARBufferUnformatted( Parm3 );
      }
      HOOKENTRY( szWin32SetWindowPtr, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETWINDOWPTR( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetWindowPtr );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetWindowPtr, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetWindowPtr() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetWindowText()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETWINDOWTEXT( HWND, PSZ16 );
PCHAR szWin16SetWindowText = "Win16SetWindowText";

BOOL16 APIENTRY16 Trc_Win16SetWindowText( HWND  Parm1
                                        , PSZ16 Parm2
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetWindowText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
      }
      HOOKENTRY16( szWin16SetWindowText, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETWINDOWTEXT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetWindowText );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetWindowText, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetWindowText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetWindowText()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETWINDOWTEXT( HWND, PSZ );
PCHAR szWin32SetWindowText = "Win32SetWindowText";

BOOL APIENTRY Trc_Win32SetWindowText( HWND Parm1
                                    , PSZ  Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetWindowText );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
      }
      HOOKENTRY( szWin32SetWindowText, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETWINDOWTEXT( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetWindowText );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetWindowText, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetWindowText() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetWindowThunkProc()                                              */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETWINDOWTHUNKPROC( HWND, PFN16 );
PCHAR szWin16SetWindowThunkProc = "Win16SetWindowThunkProc";

BOOL16 APIENTRY16 Trc_Win16SetWindowThunkProc( HWND  Parm1
                                             , PFN16 Parm2
                                             )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetWindowThunkProc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFN );
         T_LogPFN16( Parm2 );
      }
      HOOKENTRY16( szWin16SetWindowThunkProc, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETWINDOWTHUNKPROC( Parm1, Parm2 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetWindowThunkProc );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetWindowThunkProc, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetWindowThunkProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetWindowThunkProc()                                              */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETWINDOWTHUNKPROC( HWND, PFN );
PCHAR szWin32SetWindowThunkProc = "Win32SetWindowThunkProc";

BOOL APIENTRY Trc_Win32SetWindowThunkProc( HWND Parm1
                                         , PFN  Parm2
                                         )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetWindowThunkProc );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFN );
         T_LogPFN( Parm2 );
      }
      HOOKENTRY( szWin32SetWindowThunkProc, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETWINDOWTHUNKPROC( Parm1, Parm2 );

   if ( LOGWINAPI( W_THK ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetWindowThunkProc );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetWindowThunkProc, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetWindowThunkProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetWindowULong()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETWINDOWULONG( HWND, SHORT, ULONG );
PCHAR szWin16SetWindowULong = "Win16SetWindowULong";

BOOL16 APIENTRY16 Trc_Win16SetWindowULong( HWND  Parm1
                                         , SHORT Parm2
                                         , ULONG Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetWindowULong );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY16( szWin16SetWindowULong, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETWINDOWULONG( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetWindowULong );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetWindowULong, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetWindowULong() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetWindowULong()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETWINDOWULONG( HWND, LONG, ULONG );
PCHAR szWin32SetWindowULong = "Win32SetWindowULong";

BOOL APIENTRY Trc_Win32SetWindowULong( HWND  Parm1
                                     , LONG  Parm2
                                     , ULONG Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetWindowULong );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32SetWindowULong, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETWINDOWULONG( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetWindowULong );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetWindowULong, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetWindowULong() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SetWindowUShort()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSETWINDOWUSHORT( HWND, SHORT, USHORT );
PCHAR szWin16SetWindowUShort = "Win16SetWindowUShort";

BOOL16 APIENTRY16 Trc_Win16SetWindowUShort( HWND   Parm1
                                          , SHORT  Parm2
                                          , USHORT Parm3
                                          )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SetWindowUShort );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16SetWindowUShort, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSETWINDOWUSHORT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SetWindowUShort );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16SetWindowUShort, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SetWindowUShort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SetWindowUShort()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SETWINDOWUSHORT( HWND, LONG, USHORT );
PCHAR szWin32SetWindowUShort = "Win32SetWindowUShort";

BOOL APIENTRY Trc_Win32SetWindowUShort( HWND   Parm1
                                      , LONG   Parm2
                                      , USHORT Parm3
                                      )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SetWindowUShort );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_LONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY( szWin32SetWindowUShort, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SETWINDOWUSHORT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SetWindowUShort );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32SetWindowUShort, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SetWindowUShort() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ShowCursor()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSHOWCURSOR( HWND, BOOL16 );
PCHAR szWin16ShowCursor = "Win16ShowCursor";

BOOL16 APIENTRY16 Trc_Win16ShowCursor( HWND   Parm1
                                     , BOOL16 Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ShowCursor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16ShowCursor, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSHOWCURSOR( Parm1, Parm2 );

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ShowCursor );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ShowCursor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ShowCursor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ShowCursor()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SHOWCURSOR( HWND, BOOL );
PCHAR szWin32ShowCursor = "Win32ShowCursor";

BOOL APIENTRY Trc_Win32ShowCursor( HWND Parm1
                                 , BOOL Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ShowCursor );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32ShowCursor, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SHOWCURSOR( Parm1, Parm2 );

   if ( LOGWINAPI( W_CUR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ShowCursor );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ShowCursor, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ShowCursor() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ShowPointer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSHOWPOINTER( HWND, BOOL16 );
PCHAR szWin16ShowPointer = "Win16ShowPointer";

BOOL16 APIENTRY16 Trc_Win16ShowPointer( HWND   Parm1
                                      , BOOL16 Parm2
                                      )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ShowPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16ShowPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSHOWPOINTER( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ShowPointer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ShowPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ShowPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ShowPointer()                                                     */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SHOWPOINTER( HWND, BOOL );
PCHAR szWin32ShowPointer = "Win32ShowPointer";

BOOL APIENTRY Trc_Win32ShowPointer( HWND Parm1
                                  , BOOL Parm2
                                  )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ShowPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32ShowPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SHOWPOINTER( Parm1, Parm2 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ShowPointer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ShowPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ShowPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ShowTrackRect()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSHOWTRACKRECT( HWND, BOOL16 );
PCHAR szWin16ShowTrackRect = "Win16ShowTrackRect";

BOOL16 APIENTRY16 Trc_Win16ShowTrackRect( HWND   Parm1
                                        , BOOL16 Parm2
                                        )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_TREC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ShowTrackRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16ShowTrackRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSHOWTRACKRECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_TREC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ShowTrackRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ShowTrackRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ShowTrackRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ShowTrackRect()                                                   */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SHOWTRACKRECT( HWND, BOOL );
PCHAR szWin32ShowTrackRect = "Win32ShowTrackRect";

BOOL APIENTRY Trc_Win32ShowTrackRect( HWND Parm1
                                    , BOOL Parm2
                                    )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_TREC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ShowTrackRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32ShowTrackRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SHOWTRACKRECT( Parm1, Parm2 );

   if ( LOGWINAPI( W_TREC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ShowTrackRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ShowTrackRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ShowTrackRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ShowWindow()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSHOWWINDOW( HWND, BOOL16 );
PCHAR szWin16ShowWindow = "Win16ShowWindow";

BOOL16 APIENTRY16 Trc_Win16ShowWindow( HWND   Parm1
                                     , BOOL16 Parm2
                                     )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ShowWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16ShowWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSHOWWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ShowWindow );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ShowWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ShowWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ShowWindow()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SHOWWINDOW( HWND, BOOL );
PCHAR szWin32ShowWindow = "Win32ShowWindow";

BOOL APIENTRY Trc_Win32ShowWindow( HWND Parm1
                                 , BOOL Parm2
                                 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ShowWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2, DT_BOOL );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32ShowWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SHOWWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ShowWindow );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ShowWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ShowWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16StartTimer()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINSTARTTIMER( HAB, HWND, USHORT, USHORT );
PCHAR szWin16StartTimer = "Win16StartTimer";

USHORT APIENTRY16 Trc_Win16StartTimer( HAB    Parm1
                                     , HWND   Parm2
                                     , USHORT Parm3
                                     , USHORT Parm4
                                     )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16StartTimer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16StartTimer, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINSTARTTIMER( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16StartTimer );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      HOOKEXIT16( szWin16StartTimer, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16StartTimer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32StartTimer()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32STARTTIMER( HAB, HWND, ULONG, ULONG );
PCHAR szWin32StartTimer = "Win32StartTimer";

ULONG APIENTRY Trc_Win32StartTimer( HAB   Parm1
                                  , HWND  Parm2
                                  , ULONG Parm3
                                  , ULONG Parm4
                                  )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32StartTimer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_ULONG );
         T_LogULONG( Parm4 );
      }
      HOOKENTRY( szWin32StartTimer, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32STARTTIMER( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32StartTimer );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32StartTimer, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32StartTimer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16StopTimer()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSTOPTIMER( HAB, HWND, USHORT );
PCHAR szWin16StopTimer = "Win16StopTimer";

BOOL16 APIENTRY16 Trc_Win16StopTimer( HAB    Parm1
                                    , HWND   Parm2
                                    , USHORT Parm3
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16StopTimer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16StopTimer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSTOPTIMER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16StopTimer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16StopTimer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16StopTimer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32StopTimer()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32STOPTIMER( HAB, HWND, ULONG );
PCHAR szWin32StopTimer = "Win32StopTimer";

BOOL APIENTRY Trc_Win32StopTimer( HAB   Parm1
                                , HWND  Parm2
                                , ULONG Parm3
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32StopTimer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32StopTimer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32STOPTIMER( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_TIME ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32StopTimer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32StopTimer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32StopTimer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16StretchPointer()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSTRETCHPOINTER( HPS, SHORT, SHORT, SHORT, SHORT, HPOINTER, USHORT );
PCHAR szWin16StretchPointer = "Win16StretchPointer";

BOOL16 APIENTRY16 Trc_Win16StretchPointer( HPS      Parm1
                                         , SHORT    Parm2
                                         , SHORT    Parm3
                                         , SHORT    Parm4
                                         , SHORT    Parm5
                                         , HPOINTER Parm6
                                         , USHORT   Parm7
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16StretchPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_HPOINTER );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY16( szWin16StretchPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSTRETCHPOINTER( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16StretchPointer );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16StretchPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16StretchPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32StretchPointer()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32STRETCHPOINTER( HPS, SHORT, SHORT, SHORT, SHORT, HPOINTER, USHORT );
PCHAR szWin32StretchPointer = "Win32StretchPointer";

BOOL APIENTRY Trc_Win32StretchPointer( HPS      Parm1
                                     , SHORT    Parm2
                                     , SHORT    Parm3
                                     , SHORT    Parm4
                                     , SHORT    Parm5
                                     , HPOINTER Parm6
                                     , USHORT   Parm7
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32StretchPointer );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HPS );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_SHORT );
         T_LogUSHORT( Parm4 );
         T_LogParameter( 5L, DT_SHORT );
         T_LogUSHORT( Parm5 );
         T_LogParameter( 6L, DT_HPOINTER );
         T_LogULONG( Parm6 );
         T_LogParameter( 7L, DT_USHORT );
         T_LogUSHORT( Parm7 );
      }
      HOOKENTRY( szWin32StretchPointer, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32STRETCHPOINTER( Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7 );

   if ( LOGWINAPI( W_PTR ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32StretchPointer );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32StretchPointer, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32StretchPointer() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SubclassWindow()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
PFNWP16 APIENTRY16 WINSUBCLASSWINDOW( HWND, PFNWP16 );
PCHAR szWin16SubclassWindow = "Win16SubclassWindow";

PFNWP16 APIENTRY16 Trc_Win16SubclassWindow( HWND    Parm1
                                          , PFNWP16 Parm2
                                          )
{

   PFNWP16 pRC;                                  /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SubclassWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFNWP );
         T_LogPFN16( (PFN16)Parm2 );
      }
      HOOKENTRY16( szWin16SubclassWindow, &Parm1 );
      T_FreeLog( );
   }

   pRC = WINSUBCLASSWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SubclassWindow );
      T_LogPMReturnCodePtr16( DT_PFNWP, (PVOID16)pRC, (BOOL)pRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPFN16( (PFN16)pRC ); }
      }
      HOOKEXIT16( szWin16SubclassWindow, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win16SubclassWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SubclassWindow()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
PFNWP APIENTRY WIN32SUBCLASSWINDOW( HWND, PFNWP );
PCHAR szWin32SubclassWindow = "Win32SubclassWindow";

PFNWP APIENTRY Trc_Win32SubclassWindow( HWND  Parm1
                                      , PFNWP Parm2
                                      )
{

   PFNWP pRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SubclassWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PFNWP );
         T_LogPFN( (PFN)Parm2 );
      }
      HOOKENTRY( szWin32SubclassWindow, &Parm1 );
      T_FreeLog( );
   }

   pRC = WIN32SUBCLASSWINDOW( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SubclassWindow );
      T_LogPMReturnCode( DT_PFNWP, (ULONG)pRC, (BOOL)pRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( pRC ) { T_LogLineHeader( szEmpty ); T_LogPFN( (PFN)pRC ); }
      }
      HOOKEXIT( szWin32SubclassWindow, &Parm1, &pRC );
      T_FreeLog( );
   }

   return pRC;

}  /* Trc_Win32SubclassWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SubstituteStrings()                                               */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
SHORT APIENTRY16 WINSUBSTITUTESTRINGS( HWND, PSZ16, SHORT, PSZ16 );
PCHAR szWin16SubstituteStrings = "Win16SubstituteStrings";

SHORT APIENTRY16 Trc_Win16SubstituteStrings( HWND  Parm1
                                           , PSZ16 Parm2
                                           , SHORT Parm3
                                           , PSZ16 Parm4
                                           )
{

   SHORT sRC;                                    /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SubstituteStrings );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ16( Parm2 );
         T_LogParameter( 3L, DT_SHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID16( Parm4 );
      }
      HOOKENTRY16( szWin16SubstituteStrings, &Parm1 );
      T_FreeLog( );
   }

   sRC = WINSUBSTITUTESTRINGS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SubstituteStrings );
      T_LogPMReturnCodeUSHORT( DT_SHORT, sRC, sRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSZ );
         if ( sRC ) T_LogCHARBuffer16( Parm4, sRC );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szWin16SubstituteStrings, &Parm1, &sRC );
      T_FreeLog( );
   }

   return sRC;

}  /* Trc_Win16SubstituteStrings() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SubstituteStrings()                                               */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
LONG APIENTRY WIN32SUBSTITUTESTRINGS( HWND, PSZ, LONG, PSZ );
PCHAR szWin32SubstituteStrings = "Win32SubstituteStrings";

LONG APIENTRY Trc_Win32SubstituteStrings( HWND Parm1
                                        , PSZ  Parm2
                                        , LONG Parm3
                                        , PSZ  Parm4
                                        )
{

   LONG lRC;                                     /* API return code           */

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SubstituteStrings );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PSZ );
         T_LogPSZ( Parm2 );
         T_LogParameter( 3L, DT_LONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPVOID( Parm4 );
      }
      HOOKENTRY( szWin32SubstituteStrings, &Parm1 );
      T_FreeLog( );
   }

   lRC = WIN32SUBSTITUTESTRINGS( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_DLG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SubstituteStrings );
      T_LogPMReturnCode( DT_LONG, lRC, lRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSZ );
         if ( lRC ) T_LogCHARBuffer( Parm4, lRC );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szWin32SubstituteStrings, &Parm1, &lRC );
      T_FreeLog( );
   }

   return lRC;

}  /* Trc_Win32SubstituteStrings() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16SubtractRect()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINSUBTRACTRECT( HAB, PRECTL16, PRECTL16, PRECTL16 );
PCHAR szWin16SubtractRect = "Win16SubtractRect";

BOOL16 APIENTRY16 Trc_Win16SubtractRect( HAB      Parm1
                                       , PRECTL16 Parm2
                                       , PRECTL16 Parm3
                                       , PRECTL16 Parm4
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16SubtractRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL16( Parm4 );
      }
      HOOKENTRY16( szWin16SubtractRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINSUBTRACTRECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16SubtractRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKEXIT16( szWin16SubtractRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16SubtractRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32SubtractRect()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32SUBTRACTRECT( HAB, PRECTL, PRECTL, PRECTL );
PCHAR szWin32SubtractRect = "Win32SubtractRect";

BOOL APIENTRY Trc_Win32SubtractRect( HAB    Parm1
                                   , PRECTL Parm2
                                   , PRECTL Parm3
                                   , PRECTL Parm4
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32SubtractRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL( Parm4 );
      }
      HOOKENTRY( szWin32SubtractRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32SUBTRACTRECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32SubtractRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKEXIT( szWin32SubtractRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32SubtractRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16Terminate()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINTERMINATE( HAB );
PCHAR szWin16Terminate = "Win16Terminate";

BOOL16 APIENTRY16 Trc_Win16Terminate( HAB Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16Terminate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16Terminate, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINTERMINATE( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16Terminate );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16Terminate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16Terminate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32Terminate()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32TERMINATE( HAB );
PCHAR szWin32Terminate = "Win32Terminate";

BOOL APIENTRY Trc_Win32Terminate( HAB Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32Terminate );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32Terminate, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32TERMINATE( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32Terminate );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32Terminate, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32Terminate() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ThreadAssocQueue()                                                */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINTHREADASSOCQUEUE( HAB, HMQ );
PCHAR szWin16ThreadAssocQueue = "Win16ThreadAssocQueue";

BOOL16 APIENTRY16 Trc_Win16ThreadAssocQueue( HAB Parm1
                                           , HMQ Parm2
                                           )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ThreadAssocQueue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY16( szWin16ThreadAssocQueue, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINTHREADASSOCQUEUE( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ThreadAssocQueue );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ThreadAssocQueue, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ThreadAssocQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ThreadAssocQueue()                                                */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32THREADASSOCQUEUE( HAB, HMQ );
PCHAR szWin32ThreadAssocQueue = "Win32ThreadAssocQueue";

BOOL APIENTRY Trc_Win32ThreadAssocQueue( HAB Parm1
                                       , HMQ Parm2
                                       )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ThreadAssocQueue );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HMQ );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32ThreadAssocQueue, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32THREADASSOCQUEUE( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ThreadAssocQueue );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ThreadAssocQueue, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ThreadAssocQueue() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16Throw()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
VOID APIENTRY16 WINTHROW( PCATCHBUF16, USHORT );
PCHAR szWin16Throw = "Win16Throw";

VOID APIENTRY16 Trc_Win16Throw( PCATCHBUF16 Parm1
                              , SHORT       Parm2
                              )
{

   if ( LOGWINAPI( W_ENV ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16Throw );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_PCATCHBUF );
         T_LogPCATCHBUF( Parm1 );
         T_LogParameter( 2L, DT_SHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16Throw, &Parm1 );
      T_FreeLog( );
   }

   WINTHROW( Parm1, Parm2 );

   if ( LOGWINAPI( W_ENV ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16Throw );
      HOOKEXIT16( szWin16Throw, &Parm1, NULL );
      T_FreeLog( );
   }

}  /* Trc_Win16Throw() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16TrackRect()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINTRACKRECT( HWND, HPS, PTRACKINFO16 );
PCHAR szWin16TrackRect = "Win16TrackRect";

BOOL16 APIENTRY16 Trc_Win16TrackRect( HWND         Parm1
                                    , HPS          Parm2
                                    , PTRACKINFO16 Parm3
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_TREC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16TrackRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PTRACKINFO );
         T_LogPTRACKINFO16( Parm3 );
      }
      HOOKENTRY16( szWin16TrackRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINTRACKRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_TREC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16TrackRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PTRACKINFO );
         T_LogPTRACKINFO16( Parm3 );
      }
      HOOKEXIT16( szWin16TrackRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16TrackRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32TrackRect()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32TRACKRECT( HWND, HPS, PTRACKINFO );
PCHAR szWin32TrackRect = "Win32TrackRect";

BOOL APIENTRY Trc_Win32TrackRect( HWND       Parm1
                                , HPS        Parm2
                                , PTRACKINFO Parm3
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_TREC ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32TrackRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HPS );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PTRACKINFO );
         T_LogPTRACKINFO( Parm3 );
      }
      HOOKENTRY( szWin32TrackRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32TRACKRECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_TREC ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32TrackRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 3L, DT_PTRACKINFO );
         T_LogPTRACKINFO( Parm3 );
      }
      HOOKEXIT( szWin32TrackRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32TrackRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16TranslateAccel()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINTRANSLATEACCEL( HAB, HWND, HACCEL, PQMSG16 );
PCHAR szWin16TranslateAccel = "Win16TranslateAccel";

BOOL16 APIENTRY16 Trc_Win16TranslateAccel( HAB     Parm1
                                         , HWND    Parm2
                                         , HACCEL  Parm3
                                         , PQMSG16 Parm4
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16TranslateAccel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HACCEL );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PQMSG );
         T_LogPQMSG16( Parm4 );
      }
      HOOKENTRY16( szWin16TranslateAccel, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINTRANSLATEACCEL( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16TranslateAccel );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PQMSG );
         if ( bRC ) T_LogPQMSG16( Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szWin16TranslateAccel, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16TranslateAccel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32TranslateAccel()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32TRANSLATEACCEL( HAB, HWND, HACCEL, PQMSG );
PCHAR szWin32TranslateAccel = "Win32TranslateAccel";

BOOL APIENTRY Trc_Win32TranslateAccel( HAB    Parm1
                                     , HWND   Parm2
                                     , HACCEL Parm3
                                     , PQMSG  Parm4
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32TranslateAccel );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HWND );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_HACCEL );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PQMSG );
         T_LogPQMSG( Parm4 );
      }
      HOOKENTRY( szWin32TranslateAccel, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32TRANSLATEACCEL( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_ACCL ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32TranslateAccel );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PQMSG );
         if ( bRC ) T_LogPQMSG( Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szWin32TranslateAccel, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32TranslateAccel() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16UnionRect()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINUNIONRECT( HAB, PRECTL16, PRECTL16, PRECTL16 );
PCHAR szWin16UnionRect = "Win16UnionRect";

BOOL16 APIENTRY16 Trc_Win16UnionRect( HAB      Parm1
                                    , PRECTL16 Parm2
                                    , PRECTL16 Parm3
                                    , PRECTL16 Parm4
                                    )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16UnionRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID16( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL16( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL16( Parm4 );
      }
      HOOKENTRY16( szWin16UnionRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINUNIONRECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16UnionRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
      }
      HOOKEXIT16( szWin16UnionRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16UnionRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32UnionRect()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32UNIONRECT( HAB, PRECTL, PRECTL, PRECTL );
PCHAR szWin32UnionRect = "Win32UnionRect";

BOOL APIENTRY Trc_Win32UnionRect( HAB    Parm1
                                , PRECTL Parm2
                                , PRECTL Parm3
                                , PRECTL Parm4
                                )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32UnionRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPVOID( Parm2 );
         T_LogParameter( 3L, DT_PRECTL );
         T_LogPRECTL( Parm3 );
         T_LogParameter( 4L, DT_PRECTL );
         T_LogPRECTL( Parm4 );
      }
      HOOKENTRY( szWin32UnionRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32UNIONRECT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_RECT ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32UnionRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
      }
      HOOKEXIT( szWin32UnionRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32UnionRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16UpdateWindow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINUPDATEWINDOW( HWND );
PCHAR szWin16UpdateWindow = "Win16UpdateWindow";

BOOL16 APIENTRY16 Trc_Win16UpdateWindow( HWND Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16UpdateWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16UpdateWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINUPDATEWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16UpdateWindow );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16UpdateWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16UpdateWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32UpdateWindow()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32UPDATEWINDOW( HWND );
PCHAR szWin32UpdateWindow = "Win32UpdateWindow";

BOOL APIENTRY Trc_Win32UpdateWindow( HWND Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32UpdateWindow );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32UpdateWindow, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32UPDATEWINDOW( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32UpdateWindow );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32UpdateWindow, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32UpdateWindow() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16Upper()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINUPPER( HAB, USHORT, USHORT, PSZ16 );
PCHAR szWin16Upper = "Win16Upper";

USHORT APIENTRY16 Trc_Win16Upper( HAB    Parm1
                                , USHORT Parm2
                                , USHORT Parm3
                                , PSZ16  Parm4
                                )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16Upper );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ16( Parm4 );
      }
      HOOKENTRY16( szWin16Upper, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINUPPER( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16Upper );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSZ );
         if ( usRC ) T_LogPSZ16( Parm4 );
         else T_LogPVOID16( Parm4 );
      }
      HOOKEXIT16( szWin16Upper, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16Upper() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32Upper()                                                           */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32UPPER( HAB, ULONG, ULONG, PSZ );
PCHAR szWin32Upper = "Win32Upper";

ULONG APIENTRY Trc_Win32Upper( HAB   Parm1
                             , ULONG Parm2
                             , ULONG Parm3
                             , PSZ   Parm4
                             )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32Upper );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogULONG( Parm3 );
         T_LogParameter( 4L, DT_PSZ );
         T_LogPSZ( Parm4 );
      }
      HOOKENTRY( szWin32Upper, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32UPPER( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32Upper );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 4L, DT_PSZ );
         if ( ulRC ) T_LogPSZ( Parm4 );
         else T_LogPVOID( Parm4 );
      }
      HOOKEXIT( szWin32Upper, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32Upper() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16UpperChar()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
USHORT APIENTRY16 WINUPPERCHAR( HAB, USHORT, USHORT, USHORT );
PCHAR szWin16UpperChar = "Win16UpperChar";

USHORT APIENTRY16 Trc_Win16UpperChar( HAB    Parm1
                                    , USHORT Parm2
                                    , USHORT Parm3
                                    , USHORT Parm4
                                    )
{

   USHORT usRC;                                  /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16UpperChar );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_USHORT );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16UpperChar, &Parm1 );
      T_FreeLog( );
   }

   usRC = WINUPPERCHAR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16UpperChar );
      T_LogPMReturnCodeUSHORT( DT_USHORT, usRC, usRC );
      HOOKEXIT16( szWin16UpperChar, &Parm1, &usRC );
      T_FreeLog( );
   }

   return usRC;

}  /* Trc_Win16UpperChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32UpperChar()                                                       */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
ULONG APIENTRY WIN32UPPERCHAR( HAB, ULONG, ULONG, ULONG );
PCHAR szWin32UpperChar = "Win32UpperChar";

ULONG APIENTRY Trc_Win32UpperChar( HAB   Parm1
                                 , ULONG Parm2
                                 , ULONG Parm3
                                 , ULONG Parm4
                                 )
{

   ULONG ulRC;                                   /* API return code           */

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32UpperChar );
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
      }
      HOOKENTRY( szWin32UpperChar, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32UPPERCHAR( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_CTRY ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32UpperChar );
      T_LogPMReturnCode( DT_ULONG, ulRC, ulRC );
      HOOKEXIT( szWin32UpperChar, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32UpperChar() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ValidateRect()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINVALIDATERECT( HWND, PRECTL16, BOOL16 );
PCHAR szWin16ValidateRect = "Win16ValidateRect";

BOOL16 APIENTRY16 Trc_Win16ValidateRect( HWND     Parm1
                                       , PRECTL16 Parm2
                                       , BOOL16   Parm3
                                       )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ValidateRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL16( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16ValidateRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINVALIDATERECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ValidateRect );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ValidateRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ValidateRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ValidateRect()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32VALIDATERECT( HWND, PRECTL, BOOL );
PCHAR szWin32ValidateRect = "Win32ValidateRect";

BOOL APIENTRY Trc_Win32ValidateRect( HWND   Parm1
                                   , PRECTL Parm2
                                   , BOOL   Parm3
                                   )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ValidateRect );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PRECTL );
         T_LogPRECTL( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32ValidateRect, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32VALIDATERECT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ValidateRect );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ValidateRect, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ValidateRect() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16ValidateRegion()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINVALIDATEREGION( HWND, HRGN, BOOL16 );
PCHAR szWin16ValidateRegion = "Win16ValidateRegion";

BOOL16 APIENTRY16 Trc_Win16ValidateRegion( HWND   Parm1
                                         , HRGN   Parm2
                                         , BOOL16 Parm3
                                         )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16ValidateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
      }
      HOOKENTRY16( szWin16ValidateRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINVALIDATEREGION( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16ValidateRegion );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16ValidateRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16ValidateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32ValidateRegion()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32VALIDATEREGION( HWND, HRGN, BOOL );
PCHAR szWin32ValidateRegion = "Win32ValidateRegion";

BOOL APIENTRY Trc_Win32ValidateRegion( HWND Parm1
                                     , HRGN Parm2
                                     , BOOL Parm3
                                     )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32ValidateRegion );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_HRGN );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32ValidateRegion, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32VALIDATEREGION( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32ValidateRegion );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32ValidateRegion, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32ValidateRegion() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32WaitEventSem()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY WIN32WAITEVENTSEM( HEV, ULONG );
PCHAR szWin32WaitEventSem = "Win32WaitEventSem";

APIRET APIENTRY Trc_Win32WaitEventSem( HEV   Parm1
                                     , ULONG Parm2
                                     )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32WaitEventSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HEV );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32WaitEventSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   ulRC = WIN32WAITEVENTSEM( Parm1, Parm2 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin32WaitEventSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szWin32WaitEventSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32WaitEventSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16WaitMsg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINWAITMSG( HAB, USHORT, USHORT );
PCHAR szWin16WaitMsg = "Win16WaitMsg";

BOOL16 APIENTRY16 Trc_Win16WaitMsg( HAB    Parm1
                                  , USHORT Parm2
                                  , USHORT Parm3
                                  )
{

   BOOL16   bRC;                                 /* API return code           */
   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16WaitMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogPMMessage16( Parm2 );
         T_LogParameter( 3L, DT_USHORT );
         T_LogPMMessage16( Parm3 );
      }
      HOOKENTRY16( szWin16WaitMsg, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   bRC = WINWAITMSG( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin16WaitMsg );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT16( szWin16WaitMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16WaitMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32WaitMsg()                                                         */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32WAITMSG( HAB, ULONG, ULONG );
PCHAR szWin32WaitMsg = "Win32WaitMsg";

BOOL APIENTRY Trc_Win32WaitMsg( HAB   Parm1
                              , ULONG Parm2
                              , ULONG Parm3
                              )
{

   BOOL     bRC;                                 /* API return code           */
   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32WaitMsg );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HAB );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogPMMessage( Parm2 );
         T_LogParameter( 3L, DT_ULONG );
         T_LogPMMessage( Parm3 );
      }
      HOOKENTRY( szWin32WaitMsg, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   bRC = WIN32WAITMSG( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin32WaitMsg );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
      HOOKEXIT( szWin32WaitMsg, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32WaitMsg() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32WaitMuxWaitSem()                                                  */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
APIRET APIENTRY WIN32WAITMUXWAITSEM( HMUX, ULONG, PULONG );
PCHAR szWin32WaitMuxWaitSem = "Win32WaitMuxWaitSem";

APIRET APIENTRY Trc_Win32WaitMuxWaitSem( HMUX   Parm1
                                       , ULONG  Parm2
                                       , PULONG Parm3
                                       )
{

   DATETIME dtEntry;                             /* API entry date and time   */
   DATETIME dtExit;                              /* API entry date and time   */
   APIRET   ulRC;                                /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32WaitMuxWaitSem );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMUX );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPVOID( Parm3 );
      }
      HOOKENTRY( szWin32WaitMuxWaitSem, &Parm1 );
      T_FreeLog( );
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtEntry );
   }

   ulRC = WIN32WAITMUXWAITSEM( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_MSG ) )
   {
      if ( LOGLEVEL( L_CONTENTS ) ) DosGetDateTime( &dtExit );
      T_WaitLog( );
      T_LogAPIExit( szWin32WaitMuxWaitSem );
      T_LogCPReturnCode( ulRC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         if ( LOGLEVEL( L_CONTENTS ) ) T_LogTimes( &dtEntry, &dtExit );
         T_LogParameter( 3L, DT_PULONG );
         T_LogPULONG( Parm3 );
      }
      HOOKEXIT( szWin32WaitMuxWaitSem, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32WaitMuxWaitSem() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16WakeThread()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
BOOL16 APIENTRY16 WINWAKETHREAD( HMQ );
PCHAR szWin16WakeThread = "Win16WakeThread";

BOOL16 APIENTRY16 Trc_Win16WakeThread( HMQ Parm1 )
{

   BOOL16 bRC;                                   /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16WakeThread );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16WakeThread, &Parm1 );
      T_FreeLog( );
   }

   bRC = WINWAKETHREAD( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16WakeThread );
      T_LogPMReturnCodeUSHORT( DT_BOOL, bRC, bRC );
      HOOKEXIT16( szWin16WakeThread, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win16WakeThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32WakeThread()                                                      */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
BOOL APIENTRY WIN32WAKETHREAD( HMQ );
PCHAR szWin32WakeThread = "Win32WakeThread";

BOOL APIENTRY Trc_Win32WakeThread( HMQ Parm1 )
{

   BOOL bRC;                                     /* API return code           */

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32WakeThread );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HMQ );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32WakeThread, &Parm1 );
      T_FreeLog( );
   }

   bRC = WIN32WAKETHREAD( Parm1 );

   if ( LOGWINAPI( W_MSG ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32WakeThread );
      T_LogPMReturnCode( DT_BOOL, bRC, bRC );
      HOOKEXIT( szWin32WakeThread, &Parm1, &bRC );
      T_FreeLog( );
   }

   return bRC;

}  /* Trc_Win32WakeThread() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16WindowFromDC()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINWINDOWFROMDC( HDC );
PCHAR szWin16WindowFromDC = "Win16WindowFromDC";

HWND APIENTRY16 Trc_Win16WindowFromDC( HDC Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16WindowFromDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY16( szWin16WindowFromDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINWINDOWFROMDC( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16WindowFromDC );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16WindowFromDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16WindowFromDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32WindowFromDC()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32WINDOWFROMDC( HDC );
PCHAR szWin32WindowFromDC = "Win32WindowFromDC";

HWND APIENTRY Trc_Win32WindowFromDC( HDC Parm1 )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32WindowFromDC );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HDC );
         T_LogULONG( Parm1 );
      }
      HOOKENTRY( szWin32WindowFromDC, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32WINDOWFROMDC( Parm1 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32WindowFromDC );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32WindowFromDC, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32WindowFromDC() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16WindowFromID()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINWINDOWFROMID( HWND, USHORT );
PCHAR szWin16WindowFromID = "Win16WindowFromID";

HWND APIENTRY16 Trc_Win16WindowFromID( HWND   Parm1
                                     , USHORT Parm2
                                     )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16WindowFromID );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_USHORT );
         T_LogUSHORT( Parm2 );
      }
      HOOKENTRY16( szWin16WindowFromID, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINWINDOWFROMID( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16WindowFromID );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16WindowFromID, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16WindowFromID() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32WindowFromID()                                                    */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32WINDOWFROMID( HWND, ULONG );
PCHAR szWin32WindowFromID = "Win32WindowFromID";

HWND APIENTRY Trc_Win32WindowFromID( HWND  Parm1
                                   , ULONG Parm2
                                   )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32WindowFromID );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_ULONG );
         T_LogULONG( Parm2 );
      }
      HOOKENTRY( szWin32WindowFromID, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32WINDOWFROMID( Parm1, Parm2 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32WindowFromID );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32WindowFromID, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32WindowFromID() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win16WindowFromPoint()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) && ( OS2VER >= 200 ) )
HWND APIENTRY16 WINWINDOWFROMPOINT( HWND, PPOINTL16, BOOL16, BOOL16 );
PCHAR szWin16WindowFromPoint = "Win16WindowFromPoint";

HWND APIENTRY16 Trc_Win16WindowFromPoint( HWND      Parm1
                                        , PPOINTL16 Parm2
                                        , BOOL16    Parm3
                                        , BOOL16    Parm4
                                        )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry16( szWin16WindowFromPoint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL16( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogUSHORT( Parm3 );
         T_LogParameter( 4L, DT_BOOL );
         T_LogUSHORT( Parm4 );
      }
      HOOKENTRY16( szWin16WindowFromPoint, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WINWINDOWFROMPOINT( Parm1, Parm2, Parm3, Parm4 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin16WindowFromPoint );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT16( szWin16WindowFromPoint, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win16WindowFromPoint() */
#endif

/******************************************************************************/
/*                                                                            */
/* Trc_Win32WindowFromPoint()                                                 */
/*                                                                            */
/******************************************************************************/

#if ( OS2VER >= 200 )
HWND APIENTRY WIN32WINDOWFROMPOINT( HWND, PPOINTL, BOOL );
PCHAR szWin32WindowFromPoint = "Win32WindowFromPoint";

HWND APIENTRY Trc_Win32WindowFromPoint( HWND    Parm1
                                      , PPOINTL Parm2
                                      , BOOL    Parm3
                                      )
{

   HWND ulRC;                                    /* API return code           */

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIEntry( szWin32WindowFromPoint );
      if ( LOGLEVEL( L_PARMS ) )
      {
         T_LogParameter( 1L, DT_HWND );
         T_LogULONG( Parm1 );
         T_LogParameter( 2L, DT_PPOINTL );
         T_LogPPOINTL( Parm2 );
         T_LogParameter( 3L, DT_BOOL );
         T_LogULONG( Parm3 );
      }
      HOOKENTRY( szWin32WindowFromPoint, &Parm1 );
      T_FreeLog( );
   }

   ulRC = WIN32WINDOWFROMPOINT( Parm1, Parm2, Parm3 );

   if ( LOGWINAPI( W_WIN ) )
   {
      T_WaitLog( );
      T_LogAPIExit( szWin32WindowFromPoint );
      T_LogPMReturnCode( DT_HWND, ulRC, ulRC );
      HOOKEXIT( szWin32WindowFromPoint, &Parm1, &ulRC );
      T_FreeLog( );
   }

   return ulRC;

}  /* Trc_Win32WindowFromPoint() */
#endif

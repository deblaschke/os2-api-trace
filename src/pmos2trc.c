/******************************************************************************/
/*                                                                            */
/* File name    : PMOS2TRC.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace PM routines                                  */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : See OS2TRACE.C                                              */
/*                                                                            */
/* Installation : Place the executable,  PMOS2TRC.EXE,  in a directory  along */
/*                the  PATH  environment  variable,   place  the  help  file, */
/*                PMOS2TRC.HLP,  in a directory  along the  HELP  environment */
/*                variable, and place the trace DLLs, T_*.DLL, in a directory */
/*                along the system LIBPATH.                                   */
/*                                                                            */
/* Usage        : PMOS2TRC                                                    */
/*                                                                            */
/* Scenario     : The following  example shows a  typical scenario  where the */
/*                three personalities of PMOS2TRC  can be used in  conjuction */
/*                to produce  a summary  of NLS  APIs used  by TEST.EXE.  The */
/*                following starts the PM interface:                          */
/*                                                                            */
/*                PMOS2TRC                                                    */
/*                                                                            */
/*                The following then enables launching of .EXE files from the */
/*                PM interface:                                               */
/*                                                                            */
/*                1. Select Options/Launch .EXE/.COM files from main window   */
/*                                                                            */
/*                The following then sets the logging level to 1  (equivalent */
/*                to OS2TRACE -L 1):                                          */
/*                                                                            */
/*                2. Select Customize/Logging level... from main window       */
/*                3. Click on Log API entry/exit (level 1) information button */
/*                4. Click on OK button to dismiss logging level dialog       */
/*                                                                            */
/*                The following then sets the DOSCALLS APIs group to NLS only */
/*                (equivalent to OS2TRACE -D NLS):                            */
/*                                                                            */
/*                5. Select Customize/DOSCALLS APIs... from main window       */
/*                6. Click on Clear button in DOSCALLS APIs dialog            */
/*                7. Click on NLS button in same dialog                       */
/*                8. Click on OK button to dismiss DOSCALLS APIs dialog       */
/*                                                                            */
/*                The following then enables tracing of DOSCALLS and NLS DLLs */
/*                by  TEST.EXE  (equivalent  to  OS2TRACE -ON  -DOSCALLS -NLS */
/*                TEST.EXE):                                                  */
/*                                                                            */
/*                9. Select Enable/Open file... from main window              */
/*                10. Enter TEST.EXE in entry field of open file dialog       */
/*                11. Click on OK button to dismiss open file dialog          */
/*                12. Click on All Off button in enablement dialog            */
/*                13. Click on DOSCALLS "On" button in same dialog            */
/*                14. Click on NLS "On" button in same dialog                 */
/*                15. Click on OK button to dismiss enablement dialog         */
/*                                                                            */
/*                At this point a dialog is  presented for launching the .EXE */
/*                file.  Because TEST.EXE requires no command line parameters */
/*                and can run in the foreground,  the following then launches */
/*                TEST.EXE:                                                   */
/*                                                                            */
/*                16. Click on OK button to dismiss launch dialog             */
/*                                                                            */
/*                After TEST.EXE has completed executing,  the following then */
/*                disables  tracing of  all DLLs  by TEST.EXE  (equivalent to */
/*                OS2TRACE -OFF -ALL TEST.EXE):                               */
/*                                                                            */
/*                17. Select Enable/Open file... from main window             */
/*                18. Enter TEST.EXE in entry field of open file dialog       */
/*                19. Click on OK button to dismiss open file dialog          */
/*                20. Click on All Off button in enablement dialog            */
/*                21. Click on OK button to dismiss enablement dialog         */
/*                                                                            */
/*                The  following  then  summarizes   API  tracing  logged  in */
/*                TEST.TRC and places the results in TEST.NLS  (equivalent to */
/*                OS2TRACE -S TEST.TRC > TEST.NLS):                           */
/*                                                                            */
/*                22. Select Summarize/Open file... from main window          */
/*                23. Enter TEST.TRC in entry field of open file dialog       */
/*                24. Click on OK button to dismiss open file dialog          */
/*                25. Click on Save As.. button in summarization dialog       */
/*                26. Enter TEST.NLS in entry field of save file dialog       */
/*                27. Click on OK button to dismiss save file dialog          */
/*                28. Click on OK button to dismiss summarization dialog      */
/*                                                                            */
/* Output       : See OS2TRACE.C                                              */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.24 29Mar96 Added PM interface (supported customization */
/*                                and help only)                              */
/*                2.30.25 15Apr96 Verified DOS header new header file address */
/*                2.30.26 23May96 Issued more specific enablement messages    */
/*                2.30.28 14Jun96 Changed "\r\n" to "\n" in output            */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.05 12Mar97 Added enablement support to PM interface    */
/*                2.40.07 13Mar97 Added customization cancel confirmation     */
/*                2.40.08 13Mar97 Added summarization support to PM interface */
/*                2.40.09 14Mar97 Removed  summarization   API  exit  without */
/*                                entry error                                 */
/*                2.40.11 19Mar97 Added  save  window  position  option to PM */
/*                                interface                                   */
/*                2.40.12 19Mar97 Added support for NE format                 */
/*                2.40.13 20Mar97 Added  launch   .EXE  files  option  to  PM */
/*                                interface                                   */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.18 30Apr97 Added 16-bit Kbd APIs                       */
/*                2.40.19 01May97 Added 16-bit Mou APIs                       */
/*                2.40.20 05May97 Added 16-bit Vio APIs                       */
/*                2.40.24 03Jun97 Added FAQ to PM interface help submenu      */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.45 28Jan98 Added indeterminate API return codes        */
/*                2.40.46 28Jan98 Fixed summarization alphabetical API bug    */
/*                2.40.47 29Jan98 Added Win16DefAVioWindowProc                */
/*                2.40.51 18Feb98 Verified saved  window  position  origin on */
/*                                desktop                                     */
/*                2.40.52 16Mar98 Fixed file dialog root directory bug        */
/*                2.40.53 16Mar98 Fixed file dialog clean up bug              */
/*                2.40.56 02Jun98 Changed default selection  for cancellation */
/*                                dialog to "No"                              */
/*                2.40.60 08Jul98 Added  title/action  bar only  option to PM */
/*                                interface                                   */
/*                2.40.61 17Aug98 Added SMP Dos APIs                          */
/*                2.40.65 28Aug98 Added monitor trace options to PM interface */
/*                2.40.74 27Oct98 Added WinSetErrorInfo                       */
/*                2.40.75 28Oct98 Added Gpi/Win APIs exported from PMBIDI     */
/*                2.40.78 16Nov98 Added miscellaneous Dos APIs                */
/*                2.40.81 02Dec98 Added -E option                             */
/*                2.40.83 12Jan99 Enhanced trace monitoring                   */
/*                2.40.85 21Jan99 Added -P option                             */
/*                2.40.86 03Mar99 Added support  for executables  without DOS */
/*                                header                                      */
/*                2.45.06 16May99 Added support for  KB and MB  values to  PM */
/*                                interface                                   */
/*                2.45.08 11Jun99 Added support for .COM files                */
/*                2.45.14 02Feb00 Saved OS2.INI information only when changed */
/*                2.45.16 18Feb00 Added -C, -PAUSE, and -RESUME options       */
/*                2.45.17 21Feb00 Added restore window  position option to PM */
/*                                interface                                   */
/*                2.45.18 28Feb00 Removed import by name restriction          */
/*                2.45.19 29Feb00 Fixed  trace monitoring without  background */
/*                                font                                        */
/*                2.45.21 29Feb00 Fixed resizing main window with zero height */
/*                                twice during trace monitoring               */
/*                2.45.22 01Mar00 Removed  drawing  background font if bitmap */
/*                                calculation too long                        */
/*                2.45.24 01Jun00 Added -A option                             */
/*                2.45.29 19Jun01 Added enter key support to PM interface     */
/*                2.45.32 28Jun01 Added -U option                             */
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
#include <newexe.h>                              /* DOS/NE executable info    */
#define  FOR_EXEHDR 1                            /* Avoid multiple definition */
typedef  unsigned short WORD;                    /* Required by EXE386.H      */
#ifdef INTEL
typedef  unsigned long DWORD;                    /* Required by EXE386.H      */
#endif
#include <exe386.h>                              /* LX executable info        */
#include <ctype.h>                               /* C information             */
#include <malloc.h>                              /* C information             */
#include <math.h>                                /* C information             */
#include <stdarg.h>                              /* C information             */
#include <stdio.h>                               /* C information             */
#include <stdlib.h>                              /* C information             */
#include <string.h>                              /* C information             */
#include "os2trace.h"                            /* OS2TRACE information      */
#include "pmos2trc.h"                            /* PMOS2TRC information      */

/******************************************************************************/
/*                                                                            */
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#define INC_MAINCHAR 6                           /* Main window char incremnt */
#define LEN_MAINTEXT 256                         /* Main window text length   */
#define MAX_CALCTIME 25                          /* Maximum calculation time  */
#define NUM_MAINLINE 6                           /* Number main window lines  */

#define PI           3.14159265                  /* Pi                        */
#define ANGLE45      ( PI / 4.0 )                /* 45-degree angle (radians) */
#define ANGLE90      ( PI / 2.0 )                /* 90-degree angle (radians) */

#define TXT_MAXULONG "4294967295"                /* Max ULONG value string    */

#define HUNPERSEC    100                         /* Hundredths per second     */
#define HUNPERMIN    ( 60 * HUNPERSEC )          /* Hundredths per minute     */
#define HUNPERHR     ( 60 * HUNPERMIN )          /* Hundredths per hour       */

/******************************************************************************/
/*                                                                            */
/* Macro                                                                      */
/*                                                                            */
/******************************************************************************/

#ifdef DEBUG
#define DEBUGOUT     DebugOut
#else
#define DEBUGOUT
#endif

/******************************************************************************/
/*                                                                            */
/* Function declarations                                                      */
/*                                                                            */
/******************************************************************************/

void             main( int, char ** );
PAPIINFO         AddAPIInfo( PSUMINFO, PCHAR, ULONG, ULONG, ULONG, ULONG );
BOOL             AddMonInfo( ULONG, PMONINITINFO );
VOID             AnalyzeAPIUsage( PENABINFO );
VOID             CalcMonInfoRects( VOID );
MRESULT EXPENTRY ControlDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustAltDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustBufDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustCtlDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustDosDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustEBCDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustFilDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustGpiDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustIntDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustLvlDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY CustTimDlgProc( HWND, ULONG, MPARAM, MPARAM );
#ifdef T_USERHOOK
MRESULT EXPENTRY CustUsrDlgProc( HWND, ULONG, MPARAM, MPARAM );
#endif
MRESULT EXPENTRY CustWinDlgProc( HWND, ULONG, MPARAM, MPARAM );
#ifdef DEBUG
VOID             DebugOut( PCHAR, ... );
#endif
BOOL             DelMonInfo( ULONG );
VOID             DisplayBanner( FILE * );
VOID             EnableAPITracing( PENABINFO );
MRESULT EXPENTRY EnablementDlgProc( HWND, ULONG, MPARAM, MPARAM );
ULONG APIENTRY   EnablementExceptionHandler( PREPREC, PREGREC, PCTXREC, PVOID );
VOID APIENTRY    EnablementThread( ULONG );
VOID             Error( PCHAR, ... );
VOID             ErrorMessageBox( ULONG, PCHAR, ... );
ULONG APIENTRY   ExceptionHandler( PREPREC, PREGREC, PCTXREC, PVOID );
PAPIINFO         GetAPIInfo( PSUMINFO, PCHAR );
PMONINFO         GetMonInfo( ULONG );
MRESULT EXPENTRY HelpInfoDlgProc( HWND, ULONG, MPARAM, MPARAM );
VOID             InitializeEnablement( PENABINFO );
VOID             InitializeSummarizattion( PSUMINFO );
BOOL             IsUnsupportedNRRNAM( PCHAR, PCHAR, ULONG );
MRESULT EXPENTRY LaunchExeDlgProc( HWND, ULONG, MPARAM, MPARAM );
ULONG            LookupDLL( PCHAR );
ULONG            LookupTraceDLL( PCHAR );
MRESULT EXPENTRY MainWindowProc( HWND, ULONG, MPARAM, MPARAM );
ULONG APIENTRY   MonitorExceptionHandler( PREPREC, PREGREC, PCTXREC, PVOID );
VOID APIENTRY    MonitorThread( ULONG );
MRESULT EXPENTRY OptsAPIDlgProc( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY OptsLenDlgProc( HWND, ULONG, MPARAM, MPARAM );
BOOL             QueryMessageBox( PCHAR, PCHAR );
VOID             SetCustDosDlgCheck( HWND );
VOID             SetCustGpiDlgCheck( HWND );
VOID             SetCustWinDlgCheck( HWND );
LONG             Strcmpi( PCHAR, PCHAR );
PCHAR            Strupr( PCHAR );
MRESULT EXPENTRY SummarizationDlgProc( HWND, ULONG, MPARAM, MPARAM );
ULONG APIENTRY   SummarizationExceptionHandler( PREPREC, PREGREC, PCTXREC, PVOID );
VOID APIENTRY    SummarizationThread( ULONG );
VOID             SummarizeAPITracing( PSUMINFO );
VOID             TerminateEnablement( BOOL, PENABINFO );
VOID             TerminateSummarization( BOOL, PSUMINFO );

/******************************************************************************/
/*                                                                            */
/* Variable definitions                                                       */
/*                                                                            */
/******************************************************************************/

POINTL    apointlBitBlt[ 3 ];                    /* Bit map bit blit points   */
CHAR      chMainText[ LEN_MAINTEXT ];            /* Main window text          */
HAB       habMain;                               /* Main window anchor block  */
HBITMAP   hbitmapMain;                           /* Main window bit map       */
HDC       hdcMain;                               /* Main window device context*/
HDC       hdcBitmap;                             /* Bit map device context    */
HELPINIT  hmiMain;                               /* Main window help mgr init */
HMTX      hmtxSys;                               /* System semaphore          */
HMQ       hmqMain;                               /* Main window message queue */
HPS       hpsMain;                               /* Main window pres space    */
HPS       hpsBitmap;                             /* Bit map presentation space*/
HQUEUE    hqueueMon;                             /* Trace monitor queue       */
HWND      hwndMainClient;                        /* Main window client        */
HWND      hwndMainFrame;                         /* Main window frame         */
HWND      hwndMainHelp;                          /* Main window help          */
LONG      lCalcTime;                             /* Calculation time          */
PMONINFO  pMonFirst;                             /* First trace monitor info  */
SHORT     sCurSBIdx;                             /* Current spin button index */
SWP       swpDesktop;                            /* Desktop window position   */
CHAR      szAltDir[ LEN_FILENAME ];              /* Alternative directory     */
PCHAR     szAltDirKey = ALTDIR_KEY;              /* Alternative directory key */
PCHAR     szAppName = VER_NAME;                  /* OS2TRACE application name */
PCHAR     szBufLenKey = BUFLEN_KEY;              /* Buffer length key name    */
PCHAR     szCtlFlgKey = CTLFLG_KEY;              /* Trace ctrl flags key name */
PCHAR     szDosGrpKey = DOSGRP_KEY;              /* DOSCALLS groups key name  */
PCHAR     szFilLenKey = FILLEN_KEY;              /* File wrap length key name */
PCHAR     szGpiGrpKey = GPIGRP_KEY;              /* PMGPI groups key name     */
CHAR      szInFile[ LEN_FILENAME ];              /* Input file name           */
PCHAR     szMainClass = MAIN_CLASS;              /* Main window class         */
PCHAR     szMainTitle = "OS/2 API Trace";        /* Main window title         */
PCHAR     szMonAPIKey = MONAPI_KEY;              /* Monitor API key name      */
PCHAR     szMonFilLen = "File Length  ";         /* Trc mon file length text  */
PCHAR     szMonLenKey = MONLEN_KEY;              /* Monitor file len key name */
PCHAR     szMonNumAPI = "Number APIs  ";         /* Trc mon number APIs text  */
CHAR      szOldAltDir[ LEN_FILENAME ];           /* Old alternative directory */
#ifdef T_USERHOOK
CHAR      szOldUserHk[ LEN_FILENAME ];           /* Old user hook             */
#endif
PCHAR     szPMAppName = VER_PMNAME;              /* PMOS2TRC application name */
PCHAR     szTrcFlgKey = TRCFLG_KEY;              /* Trace flags key name      */
PCHAR     szTrcLvlKey = TRCLVL_KEY;              /* Trace level key name      */
#ifdef T_USERHOOK
CHAR      szUserHk[ LEN_FILENAME ];              /* User hook                 */
PCHAR     szUserHkKey = USERHK_KEY;              /* User hook key             */
#endif
PCHAR     szWinGrpKey = WINGRP_KEY;              /* PMWIN groups key name     */
PCHAR     szWinPosKey = WINPOS_KEY;              /* Window position key name  */
PCHAR     szWinSizKey = WINSIZ_KEY;              /* Window size key name      */
ULONG     ulBitmapFont = 1L;                     /* Main window bit map font  */
ULONG     ulCtrlFlg;                             /* Trace control flags       */
ULONG     ulMainCharHeight;                      /* Main window char height   */
ULONG     ulMainHeight;                          /* Main window height        */
ULONG     ulMainWidth;                           /* Main window width         */
ULONG     ulMajorVer;                            /* OS/2 major version number */
ULONG     ulMinorVer;                            /* OS/2 minor version number */
ULONG     ulMonFilLen;                           /* Trc mon file len text len */
ULONG     ulMonNumAPI;                           /* Trc mon num APIs text len */
ULONG     ulMonNumWidth;                         /* Trace monitor number width*/
ULONG     ulMonTxtWidth;                         /* Trace monitor text width  */
ULONG     ulNewBufLen;                           /* New buffer length         */
ULONG     ulNewDosGrp;                           /* New DOSCALLS groups       */
ULONG     ulNewFilLen;                           /* New file wrapping length  */
ULONG     ulNewGpiGrp;                           /* New PMGPI groups          */
ULONG     ulNewMonAPI;                           /* New API monitoring        */
ULONG     ulNewMonLen;                           /* New file length monitoring*/
ULONG     ulNewTrcFlg;                           /* New trace flags           */
ULONG     ulNewTrcLvl;                           /* New trace level           */
ULONG     ulNewWinGrp;                           /* New PMWIN groups          */
ULONG     ulOldBufLen;                           /* Old buffer length         */
ULONG     ulOldDosGrp;                           /* Old DOSCALLS groups       */
ULONG     ulOldFilLen;                           /* Old file wrapping length  */
ULONG     ulOldGpiGrp;                           /* Old PMGPI groups          */
ULONG     ulOldMonAPI;                           /* Old API monitoring        */
ULONG     ulOldMonLen;                           /* Old file length monitoring*/
ULONG     ulOldTrcFlg;                           /* Old trace flags           */
ULONG     ulOldTrcLvl;                           /* Old trace level           */
ULONG     ulOldWinGrp;                           /* Old PMWIN groups          */

#include  "dllinfo.h"                            /* DLL information table     */
#include  "nrrnam.h"                             /* Unsupported NRRNAM info   */

/******************************************************************************/
/*                                                                            */
/* AddMonInfo()                                                               */
/*                                                                            */
/******************************************************************************/

BOOL AddMonInfo( ULONG        ulPID              /* Process identifier        */
               , PMONINITINFO pMonInitInfo       /* Trace monitor init info   */
               )
{

   PMONINFO pMonInfo;                            /* Trace monitor information */

   /*
    * Allocate memory for new trace monitor information, issue error and
    * terminate if unsuccessful
    */
   if ( !( pMonInfo = malloc( sizeof( MONINFO ) ) ) )
   {
      Error( "Unable to allocate memory for trace monitor information" );
   }

   /*
    * Initialize new trace monitor information
    */
   memset( pMonInfo, 0, sizeof( MONINFO ) );
   pMonInfo->ulPID   = ulPID;
   pMonInfo->ulFlags = pMonInitInfo->ulFlags;
   strcpy( pMonInfo->szExeName, pMonInitInfo->szExeName );
   strcpy( pMonInfo->szLogName, pMonInitInfo->szLogName );

   /*
    * Place new trace monitor information at beginning of trace monitor
    * information chain
    */
   pMonInfo->pNext = pMonFirst;
   pMonFirst       = pMonInfo;

   /*
    * Indicate trace monitor information added
    */
   return TRUE;

}  /* AddMonInfo() */

/******************************************************************************/
/*                                                                            */
/* CalcMonInfoRects()                                                         */
/*                                                                            */
/******************************************************************************/

VOID CalcMonInfoRects( VOID )
{

   int      i;                                   /* Loop variable             */
   PMONINFO pMonInfo;                            /* Trace monitor information */
   SWP      swpWin;                              /* Main window position      */
   APIRET   ulRC;                                /* Return code               */

   /*
    * Obtain ownership of system semaphore, ignoring asynchronous interrupts
    */
   ulRC = ERROR_INTERRUPT;
   while ( ulRC == ERROR_INTERRUPT )
   {
      ulRC = DosRequestMutexSem( hmtxSys, SEM_INDEFINITE_WAIT );
   }

   /*
    * Recalculate trace monitor information rectangles for each process being
    * monitored
    */
   for ( i = ulMainHeight - ulMainCharHeight, pMonInfo = pMonFirst
       ; pMonInfo != NULL
       ; i-= ulMainCharHeight, pMonInfo = pMonInfo->pNext
       )
   {
      /*
       * Skip process name and identifer
       */
      i -= ulMainCharHeight;

      /*
       * Initialize number APIs update rectangle if monitoring APIs
       */
      if ( FLAGCLR( pMonInfo->ulFlags, fMONAPI_OFF ) )
      {
         pMonInfo->rectlNumAPI.xLeft    = 10L + ulMonTxtWidth;
         pMonInfo->rectlNumAPI.yBottom  = i;
         pMonInfo->rectlNumAPI.xRight   = 10L + ulMonTxtWidth + ulMonNumWidth;
         pMonInfo->rectlNumAPI.yTop     = i + ulMainCharHeight;
         i -= ulMainCharHeight;
      }

      /*
       * Initialize log file length update rectangle if monitoring length
       */
      if ( FLAGCLR( pMonInfo->ulFlags, fMONLEN_OFF ) )
      {
         pMonInfo->rectlFilLen.xLeft    = 10L + ulMonTxtWidth;
         pMonInfo->rectlFilLen.yBottom  = i;
         pMonInfo->rectlFilLen.xRight   = 10L + ulMonTxtWidth + ulMonNumWidth;
         pMonInfo->rectlFilLen.yTop     = i + ulMainCharHeight;
         i -= ulMainCharHeight;
      }
   }

   /*
    * Release ownership of system semaphore
    */
   DosReleaseMutexSem( hmtxSys );

   /*
    * Handle too small main window height
    */
   if ( ( pMonFirst ) && ( ( i += ( ulMainCharHeight * 3 / 2 ) ) < 0 ) )
   {
      /*
       * Resize main window height if user requests to do so
       */
      if ( QueryMessageBox( "Trace Monitoring" , "Main window height too small to display all trace monitoring information, resize?" ) )
      {
         /*
          * Obtain main window position
          */
         WinQueryWindowPos( hwndMainFrame, &swpWin );

         /*
          * Adjust main window height and origin
          */
         swpWin.y  = swpWin.y + i - 2;
         swpWin.cy = swpWin.cy - i + 2;

         /*
          * Readjust main window height and origin if main window height zero
          *
          * NOTE: THIS IS REQUIRED TO FIX PROBLEM ON EARLIER VERSIONS OF OS/2
          *       WHERE THIS CODE WOULD EXECUTE TWICE IF CLIENT HEIGHT ZERO!
          */
         if ( !ulMainHeight ) {
            swpWin.y  -= 3;
            swpWin.cy += 3;
         }

         /*
          * Reposition main window if main window origin not on desktop and
          * user requests to do so
          */
         if ( ( swpWin.y < 0 ) && ( QueryMessageBox( "Trace Monitoring" , "Main window too close to bottom of screen to display all trace monitoring information, reposition?" ) ) )
         {
            swpWin.y = 0;
         }

         /*
          * Size and position main window
          */
         WinSetWindowPos( hwndMainFrame, HWND_TOP, swpWin.x, swpWin.y, swpWin.cx, swpWin.cy, SWP_SIZE | SWP_MOVE );
      }
   }

}  /* CalcMonInfoRects() */

/******************************************************************************/
/*                                                                            */
/* DelMonInfo()                                                               */
/*                                                                            */
/******************************************************************************/

BOOL DelMonInfo( ULONG ulPID )                   /* Process identifier        */
{

   PMONINFO pMonInfo;                            /* Trace monitor information */
   PMONINFO pDelInfo = NULL;                     /* Found trace monitor info  */

   /*
    * Start at beginning of trace monitor information chain
    */
   if ( ( pMonInfo = pMonFirst ) != NULL )
   {
      /*
       * Matching process identifier found at beginning of chain
       */
      if ( pMonInfo->ulPID == ulPID )
      {
         pDelInfo = pMonFirst;
         pMonFirst = pMonInfo->pNext;
      }

      /*
       * Matching process identifier not found at beginning of chain
       */
      else
      {
         /*
          * Search through trace monitor information for matching process
          * identifier
          */
         while ( pMonInfo->pNext )
         {
            /*
             * Matching process identifier found
             */
            if ( pMonInfo->pNext->ulPID == ulPID )
            {
               pDelInfo = pMonInfo->pNext;
               pMonInfo->pNext = pMonInfo->pNext->pNext;
               break;
            }

            /*
             * Skip trace monitor information
             */
            pMonInfo = pMonInfo->pNext;
         }
      }
   }

   /*
    * Delete trace monitor information and indicate match found if match found
    */
   if ( pDelInfo )
   {
      free( pDelInfo );
      return TRUE;
   }

   /*
    * Indicate match not found
    */
   else
   {
      return FALSE;
   }

}  /* DelMonInfo() */

/******************************************************************************/
/*                                                                            */
/* GetMonInfo()                                                               */
/*                                                                            */
/******************************************************************************/

PMONINFO GetMonInfo( ULONG ulPID )               /* Process identifier        */
{

   PMONINFO pMonInfo;                            /* Trace monitor information */

   /*
    * Start at beginning of trace monitor information chain
    */
   pMonInfo = pMonFirst;

   /*
    * Search through trace monitor information for matching process identifier
    */
   while ( pMonInfo )
   {
      /*
       * Return trace monitor information address if matching process
       * identifier found
       */
      if ( pMonInfo->ulPID == ulPID )
      {
         return pMonInfo;
      }

      /*
       * Skip trace monitor information
       */
      pMonInfo = pMonInfo->pNext;
   }

   /*
    * Indicate match not found
    */
   return NULL;

}  /* GetMonInfo() */

/******************************************************************************/
/*                                                                            */
/* main()                                                                     */
/*                                                                            */
/******************************************************************************/

void main( int    argc                           /* Argument count            */
         , char **argv                           /* Argument pointer          */
         )
{

   LONG   lcx, lcy;                              /* Main window size          */
   LONG   lx, ly;                                /* Main window position      */
   QMSG   qmsgMain;                              /* Main window message       */
   REGREC sRegRec;                               /* Registration record       */
   CHAR   szString[ 32 ];                        /* String                    */
   CHAR   szTitle[ LEN_IOLINE ];                 /* Help window title         */
   ULONG  ulBufLen;                              /* Buffer length             */
   ULONG  ulDiskNum;                             /* Disk number               */
   ULONG  ulFlags;                               /* Main win creation flags   */
   ULONG  ulLogMap;                              /* Logical drives mapping    */
   ULONG  ulNamLen;                              /* File name length          */
   ULONG  ulVersion[ 2 ];                        /* Major/minor version nums  */

   /*
    * Register exception handler
    */
   sRegRec.ExceptionHandler = ExceptionHandler;
   DosSetExceptionHandler( &sRegRec );

   /*
    * Obtain OS/2 major and minor version numbers
    */
   DosQuerySysInfo( QSV_VERSION_MAJOR, QSV_VERSION_MINOR, ulVersion, sizeof( ulVersion ) );
   ulMajorVer = ulVersion[ 0 ] / 10;
   ulMinorVer = ulVersion[ 1 ];

   /*
    * Obtain current disk, initialize input file name if successful
    */
   if ( !DosQueryCurrentDisk( &ulDiskNum, &ulLogMap ) )
   {
      /*
       * Format drive portion of input file name
       */
      sprintf( szInFile, "%c:\\", ulDiskNum + 'A' - 1 );

      /*
       * Append current directory to input file name
       */
      ulBufLen = sizeof( szInFile ) - ( ulNamLen = strlen( szInFile ) );
      if ( ( !DosQueryCurrentDir( ulDiskNum, (PBYTE)&szInFile[ ulNamLen ], &ulBufLen ) ) && ( strlen( szInFile ) > ulNamLen ) )
      {
         strcat( szInFile, "\\" );
      }
   }

   /*
    * Obtain boot drive, initialize input file name if successful
    */
   else if ( !DosQuerySysInfo( QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulDiskNum, sizeof( ulDiskNum ) ) )
   {
      /*
       * Format drive portion of input file name
       */
      sprintf( szInFile, "%c:\\", ulDiskNum + 'A' - 1 );
   }

   /*
    * Initialize Presentation Manager facilities, continue main window creation
    * if successful
    */
   if ( ( habMain = WinInitialize( 0L ) ) != 0L )
   {
      /*
       * Create main window message queue, continue main window creation if
       * successful
       */
      if ( ( hmqMain = WinCreateMsgQueue( habMain, 0L ) ) != 0L )
      {
         /*
          * Register main window class, continue main window creation if
          * successful
          */
         if ( WinRegisterClass( habMain, szMainClass, MainWindowProc, CS_SIZEREDRAW, 0L ) )
         {
            /*
             * Format help window title
             */
            sprintf( szTitle, "%s Help", VER_FULLNAME );

            /*
             * Initialize main window help manager initialization information
             */
            hmiMain.cb                       = sizeof( HELPINIT );
            hmiMain.ulReturnCode             = 0L;
            hmiMain.pszTutorialName          = NULL;
            hmiMain.phtHelpTable             = (PHELPTABLE)(0xFFFF0000 | HLP_MAINWIN );
            hmiMain.hmodHelpTableModule      = 0L;
            hmiMain.hmodAccelActionBarModule = 0L;
            hmiMain.idAccelTable             = 0L;
            hmiMain.idActionBar              = 0L;
            hmiMain.pszHelpWindowTitle       = szTitle;
            hmiMain.fShowPanelId             = CMIC_HIDE_PANEL_ID;
            hmiMain.pszHelpLibraryName       = DEF_HELPFILE;

            /*
             * Create main window help instance, issue warning if unsuccessful
             */
            if ( !( hwndMainHelp = WinCreateHelpInstance( habMain, &hmiMain ) ) )
            {
               ErrorMessageBox( MB_WARNING, "Unable to create main window help instance, help disabled" );
            }

            /*
             * Initialize main window creation flags
             */
            ulFlags = FCF_STANDARD;

            /*
             * Create main window, initiate main window message processing if
             * successful
             */
            if ( ( hwndMainFrame = WinCreateStdWindow( HWND_DESKTOP, FS_STANDARD, &ulFlags, szMainClass, szMainTitle, 0L, 0L, ID_MAINWIN, &hwndMainClient ) ) != 0L )
            {
               /*
                * Associate main window help instance with main window if
                * instance created
                */
               if ( hwndMainHelp )
               {
                  /*
                   * Associate main window help instance with main window,
                   * issue warning if unsuccessful
                   */
                  if ( !WinAssociateHelpInstance( hwndMainHelp, hwndMainFrame ) )
                  {
                     ErrorMessageBox( MB_WARNING, "Unable to associate main window help instance, help disabled" );
                  }
               }

               /*
                * Create system mutual exclusion semaphore, issue error and
                * terminate if semaphore already exists
                */
               if ( DosCreateMutexSem( SYS_SEMAPHORE, &hmtxSys, 0L, FALSE ) == ERROR_DUPLICATE_NAME )
               {
                  Error( "Another instance of %s is already running", VER_FULLNAME );
               }

               /*
                * Size, position, and show main window if able to obtain window
                * position and size from operating system profile
                */
               if ( ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szWinPosKey, NULL, szString, sizeof( szString ) ) ) && ( sscanf( szString, "%ld,%ld", &lx, &ly ) == 2 )   ) &&
                    ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szWinSizKey, NULL, szString, sizeof( szString ) ) ) && ( sscanf( szString, "%ld,%ld", &lcx, &lcy ) == 2 ) ) )
               {
                  /*
                   * Size, position, and show main window if main window origin
                   * on desktop
                   */
                  if ( ( lx < swpDesktop.cx ) && ( ly + lcy <= swpDesktop.cy ) )
                  {
                     WinSetWindowPos( hwndMainFrame, HWND_TOP, lx, ly, lcx, lcy, SWP_SIZE | SWP_MOVE | SWP_SHOW );
                  }

                  /*
                   * Issue warning and show main window if main window origin
                   * not on desktop
                   */
                  else
                  {
                     ErrorMessageBox( MB_WARNING, "Unable to restore main window position (window origin not on desktop), shell position used" );
                     WinShowWindow( hwndMainFrame, TRUE );
                  }
               }

               /*
                * Show main window if unable to obtain window position and size
                * from operating system profile
                */
               else
               {
                  WinShowWindow( hwndMainFrame, TRUE );
               }

               /*
                * Process main window messages
                */
               while( WinGetMsg( habMain, &qmsgMain, 0L, 0L, 0L ) )
               {
                  WinDispatchMsg( habMain, &qmsgMain );
               }

               /*
                * Delete system semaphore
                */
               DosCloseMutexSem( hmtxSys );

               /*
                * Destroy main window
                */
               WinDestroyWindow( hwndMainFrame );
            }

            /*
             * Issue error and terminate if main window creation unsuccessful
             */
            else
            {
               Error( "Unable to create main window" );
            }

            /*
             * Destroy main window help instance if instance created
             */
            if ( hwndMainHelp )
            {
               WinDestroyHelpInstance( hwndMainHelp );
            }
         }

         /*
          * Issue error and terminate if main window class registration
          * unsuccessful
          */
         else
         {
            Error( "Unable to register main window class" );
         }

         /*
          * Destroy main window message queue
          */
         WinDestroyMsgQueue( hmqMain );
      }

      /*
       * Issue error and terminate if main window message queue creation
       * unsuccessful
       */
      else
      {
         Error( "Unable to create main window message queue" );
      }

      /*
       * Terminate Presentation Manager facilities
       */
      WinTerminate( habMain );
   }

   /*
    * Issue error and terminate if Presentation Manager facilities
    * initialization unsuccessful
    */
   else
   {
      Error( "Unable to initialize Presentation Manager" );
   }

   /*
    * Unregister exception handler
    */
   DosUnsetExceptionHandler( &sRegRec );

   /*
    * Terminate program
    */
   exit( NO_ERROR );

}  /* main() */

/******************************************************************************/
/*                                                                            */
/* AddAPIInfo()                                                               */
/*                                                                            */
/******************************************************************************/

PAPIINFO AddAPIInfo( PSUMINFO pSumInfo           /* Summarization information */
                   , PCHAR    szName             /* API name                  */
                   , ULONG    ulCall             /* API invocations           */
                   , ULONG    ulPass             /* Passed API invocations    */
                   , ULONG    ulFail             /* Failed API invocations    */
                   , ULONG    ulIndeterminate    /* Indeterminate invocations */
                   )
{

   LONG     lResult;                             /* String comparison result  */
   PAPIINFO pAPI;                                /* API information           */
   PAPIINFO pAPINew;                             /* New API information       */
   PAPIINFO pAPIPrev = NULL;                     /* Previous API information  */

   /*
    * Allocate memory for new API information, issue error and terminate if
    * unsuccessful
    */
   if ( !( pAPINew = malloc( sizeof( APIINFO ) ) ) )
   {
      Error( "Unable to allocate memory for API information" );
   }

   /*
    * Initialize new API information
    */
   pAPINew->pNext           = NULL;
   strcpy( pAPINew->szName, szName );
   pAPINew->ulCall          = ulCall;
   pAPINew->ulPass          = ulPass;
   pAPINew->ulFail          = ulFail;
   pAPINew->ulIndeterminate = ulIndeterminate;

   /*
    * Place new API information at beginning of API information chain if
    * initial API or new API name precedes initial API name alphabetically
    */
   if ( !( pAPI = pSumInfo->pAPIFirst ) || ( strcmp( pAPI->szName, szName ) > 0 ) )
   {
      /*
       * Place new API information at beginning of API information chain
       */
      pAPINew->pNext = pSumInfo->pAPIFirst;
      pSumInfo->pAPIFirst = pAPINew;
   }

   /*
    * Place new API information alphabetically in API information chain
    */
   else
   {
      /*
       * Search for API name in API information chain that succeeds new API
       * name alphabetically or until end of information chain reached
       */
      while ( ( ( lResult = strcmp( pAPI->szName, szName ) ) < 0 ) && ( pAPI->pNext ) )
      {
         pAPIPrev = pAPI;
         pAPI = pAPI->pNext;
      }

      /*
       * Place new API information at end of API information chain if end of
       * information chain reached
       */
      if ( ( lResult < 0 ) && ( !pAPI->pNext ) )
      {
         pAPI->pNext = pAPINew;
      }

      /*
       * Place new API information in midst of API information chain if new API
       * name succeeds current API name alphabetically
       */
      else
      {
         pAPIPrev->pNext = pAPINew;
         pAPINew->pNext = pAPI;
      }
   }

   /*
    * Return new API information
    */
   return pAPINew;

}  /* AddAPIInfo() */

/******************************************************************************/
/*                                                                            */
/* AnalyzeAPIUsage()                                                          */
/*                                                                            */
/******************************************************************************/

VOID AnalyzeAPIUsage( PENABINFO pEnabInfo )      /* Enablement information    */
{

   CHAR   chChnCnt;                              /* Source offset list count  */
   CHAR   chSrcTyp;                              /* Source type               */
   CHAR   chTgtFlg;                              /* Target flag               */
   int    i = 0, j;                              /* Loop variables            */
   PCHAR  pch;                                   /* Character pointer         */
   ULONG  ulDLLIdx;                              /* DLL information index     */
   ULONG  ulImpOrd;                              /* Imported ordinal number   */
   ULONG  ulPrcOff;                              /* Proc name table offset    */
   USHORT usFixups;                              /* Number fixup records      */
   USHORT usModOrd;                              /* Module name table index   */
   USHORT usSrcOff;                              /* Source offset             */

   /*
    * Analyze API usage in NE executable file
    */
   if ( FLAGSET( pEnabInfo->ulFlags, fNE_FMT ) )
   {
      /*
       * Analyze each segment in executable file for fixup records (fixup
       * records, if present, are grouped with corresponding segment data)
       */
      for ( i = 0; i < pEnabInfo->ulSegment; i++ )
      {
         /*
          * Analyze segment if segment contains fixup records
          */
         if ( FLAGSET( pEnabInfo->pSegmentTbl[ i ].ns_flags, NSRELOC ) )
         {
            /*
             * Allocate memory for segment, issue error and terminate if
             * unsuccessful
             */
            if ( !( pEnabInfo->pSegment = malloc( pEnabInfo->pSegmentTbl[ i ].ns_cbseg ) ) )
            {
               Error( "Unable to allocate memory for segment: %s", pEnabInfo->szInFile );
            }

            /*
             * Locate beginning of segment, read segment, and read number of
             * fixup records; issue error and terminate if unsuccessful
             */
            if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->pSegmentTbl[ i ].ns_sector * pEnabInfo->usSector, SEEK_SET ) ) ||
                 ( fread( pEnabInfo->pSegment, pEnabInfo->pSegmentTbl[ i ].ns_cbseg, 1, pEnabInfo->fpInFile ) != 1     ) ||
                 ( fread( &usFixups, sizeof( USHORT ), 1, pEnabInfo->fpInFile ) != 1                                   ) )
            {
               Error( "Unable to read segment: %s", pEnabInfo->szInFile );
            }

            /*
             * Determine fixup record table size based on number of fixup
             * records and size of fixup record
             */
            pEnabInfo->ulFixupTbl = usFixups * sizeof( NERLC );

            /*
             * Allocate memory for fixup record table, issue error and
             * terminate if unsuccessful
             */
            if ( !( pEnabInfo->pFixupTbl = malloc( pEnabInfo->ulFixupTbl ) ) )
            {
               Error( "Unable to allocate memory for fixup record table: %s", pEnabInfo->szInFile );
            }

            /*
             * Read fixup record table, issue error and terminate if
             * unsuccessful
             */
            if ( fread( pEnabInfo->pFixupTbl, pEnabInfo->ulFixupTbl, 1, pEnabInfo->fpInFile ) != 1 )
            {
               Error( "Unable to read fixup record table: %s", pEnabInfo->szInFile );
            }

            /*
             * Analyze each fixup record in fixup record table
             */
            j = 0;
            while ( j < pEnabInfo->ulFixupTbl )
            {
               /*
                * Skip source type
                */
               j++;

               /*
                * Extract target flags and source offset from fixup record
                */
               chTgtFlg = pEnabInfo->pFixupTbl[ j++ ];
               usSrcOff = *(PUSHORT)( pEnabInfo->pFixupTbl + j );
               j += sizeof( USHORT );

               /*
                * Ignore fixup record if internal fixup
                */
               if ( ( chTgtFlg & NRRTYP ) == NRRINT )
               {
                  /*
                   * Skip remainder of fixup record
                   */
                  j += sizeof( ULONG );
               }

               /*
                * Process fixup record if import by ordinal fixup
                */
               else if ( ( chTgtFlg & NRRTYP ) == NRRORD )
               {
                  /*
                   * Extract module name table index and imported ordinal
                   * number from fixup record
                   */
                  usModOrd = *(PUSHORT)( pEnabInfo->pFixupTbl + j );
                  j += sizeof( USHORT );
                  ulImpOrd = (ULONG)*(PUSHORT)( pEnabInfo->pFixupTbl + j );
                  j += sizeof( USHORT );

                  /*
                   * Determine DLL information index (module index table
                   * zero-based, module name table index one-based)
                   */
                  ulDLLIdx = pEnabInfo->pModIdxTbl[ usModOrd - 1 ];

                  /*
                   * Issue error and disable tracing if fixup record references
                   * supported DLL and ordinal not in DLL information table
                   */
                  if ( ( ulDLLIdx != UNKNOWN_DLL                                                                          ) &&
                       ( FLAGSET( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL )                                                 ) &&
                       ( ( ulImpOrd < DLLInfo[ ulDLLIdx ].ulMinOrd386 ) || ( ulImpOrd > DLLInfo[ ulDLLIdx ].ulMaxOrd386 ) ) )
                  {
                     /*
                      * Issue error
                      */
                     ErrorMessageBox( MB_ERROR, "File imports unsupported ordinal %s.%lu, %s API tracing cannot be enabled", DLLInfo[ ulDLLIdx ].szName, ulImpOrd, DLLInfo[ ulDLLIdx ].szName );

                     /*
                      * Indicate tracing disabled
                      */
                     CLRFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL );
                  }
               }

               /*
                * Process fixup record if import by name fixup
                */
               else if ( ( chTgtFlg & NRRTYP ) == NRRNAM )
               {
                  /*
                   * Extract module name table index and procedure name table
                   * offset from fixup record
                   */
                  usModOrd = *(PUSHORT)( pEnabInfo->pFixupTbl + j );
                  j += sizeof( USHORT );
                  ulPrcOff = (ULONG)*(PUSHORT)( pEnabInfo->pFixupTbl + j );
                  j += sizeof( USHORT );

                  /*
                   * Determine DLL information index (module index table
                   * zero-based, module name table index one-based)
                   */
                  ulDLLIdx = pEnabInfo->pModIdxTbl[ usModOrd - 1 ];

                  /*
                   * Check for unsupported API name if fixup record references
                   * supported DLL with trace on/off enabled
                   */
                  if ( ( ulDLLIdx != UNKNOWN_DLL                          ) &&
                       ( FLAGSET( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL ) ) )
                  {
                     /*
                      * Obtain imported API name
                      */
                     pch = (PCHAR)( (ULONG)pEnabInfo->pImpNamTbl + ulPrcOff );

                     /*
                      * Issue error and disable trace on/off if unsupported API
                      * name
                      */
                     if ( IsUnsupportedNRRNAM( DLLInfo[ ulDLLIdx ].szName, pch + 1, *pch ) )
                     {
                        /*
                         * Issue error
                         */
                        ErrorMessageBox( MB_ERROR, "File imports %s.%.*s by name, %s API tracing cannot be enabled", DLLInfo[ ulDLLIdx ].szName, *pch, pch + 1, DLLInfo[ ulDLLIdx ].szName );

                        /*
                         * Indicate tracing disabled
                         */
                        CLRFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL );
                     }
                  }
               }

               /*
                * Ignore fixup record if operating system fixup
                */
               else if ( ( chTgtFlg & NRRTYP ) == NRROSF )
               {
                  /*
                   * Skip remainder of fixup record
                   */
                  j += sizeof( ULONG );
               }
            }

            /*
             * Free memory for segment
             */
            free( pEnabInfo->pSegment );
            pEnabInfo->pSegment = NULL;

            /*
             * Free memory for fixup record table
             */
            free( pEnabInfo->pFixupTbl );
            pEnabInfo->pFixupTbl = NULL;
         }
      }
   }

   /*
    * Analyze API usage in LX executable file
    */
   else if ( FLAGSET( pEnabInfo->ulFlags, fLX_FMT ) )
   {
      /*
       * Analyze each fixup record in fixup record table
       */
      while ( i < pEnabInfo->ulFixupTbl )
      {
         /*
          * Extract source type and target flags from fixup record
          */
         chSrcTyp = pEnabInfo->pFixupTbl[ i++ ];
         chTgtFlg = pEnabInfo->pFixupTbl[ i++ ];

         /*
          * Extract source offset list count from fixup record if fixup record
          * contains chain of source offsets
          */
         if ( FLAGSET( chSrcTyp, NRCHAIN ) )
         {
            chChnCnt = pEnabInfo->pFixupTbl[ i++ ];
         }

         /*
          * Source offset list count is one if fixup record contains single
          * source offset
          */
         else
         {
            chChnCnt = 1;
            i += sizeof( USHORT );
         }

         /*
          * Extract 16-bit module name table index from fixup record if
          * appropriate flag set
          */
         if ( FLAGSET( chTgtFlg, NR16OBJMOD ) )
         {
            usModOrd = *(PUSHORT)( pEnabInfo->pFixupTbl + i );
            i += sizeof( USHORT );
         }

         /*
          * Extract 8-bit module name table index from fixup record if
          * appropriate flag clear
          */
         else
         {
            usModOrd = (USHORT)pEnabInfo->pFixupTbl[ i++ ];
         }

         /*
          * Ignore fixup record if internal fixup
          */
         if ( ( chTgtFlg & NRRTYP ) == NRRINT )
         {
            /*
             * Fixup record contains target offset if not 16-bit selector fixup
             * record
             */
            if ( ( chSrcTyp & NRSTYP ) != NRSSEG )
            {
               /*
                * Skip 32-bit target offset from fixup record if appropriate
                * flag set
                */
               if ( FLAGSET( chTgtFlg, NR32BITOFF ) )
               {
                  i += sizeof( ULONG );
               }

               /*
                * Skip 16-bit target offset from fixup record if appropriate
                * flag clear
                */
               else
               {
                  i += sizeof( USHORT );
               }
            }
         }

         /*
          * Process fixup record if import by ordinal fixup
          */
         else if ( ( chTgtFlg & NRRTYP ) == NRRORD )
         {
            /*
             * Extract 8-bit imported ordinal number from fixup record if
             * appropriate flag set
             */
            if ( FLAGSET( chTgtFlg, NR8BITORD ) )
            {
               ulImpOrd = (ULONG)pEnabInfo->pFixupTbl[ i++ ];
            }

            /*
             * Extract 32-bit imported ordinal number from fixup record if
             * appropriate flag set
             */
            else if ( FLAGSET( chTgtFlg, NR32BITOFF ) )
            {
               ulImpOrd = *(PULONG)( pEnabInfo->pFixupTbl + i );
               i += sizeof( ULONG );
            }

            /*
             * Extract 16-bit imported ordinal number from fixup record if
             * neither flag set
             */
            else
            {
               ulImpOrd = (ULONG)*(PUSHORT)( pEnabInfo->pFixupTbl + i );
               i += sizeof( USHORT );
            }

            /*
             * Determine DLL information index (module index table zero-based,
             * module name table index one-based)
             */
            ulDLLIdx = pEnabInfo->pModIdxTbl[ usModOrd - 1 ];

            /*
             * Issue error and disable tracing if fixup record references
             * supported DLL and ordinal not in DLL information table
             */
            if ( ( ulDLLIdx != UNKNOWN_DLL                                                                          ) &&
                 ( FLAGSET( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL )                                                 ) &&
                 ( ( ulImpOrd < DLLInfo[ ulDLLIdx ].ulMinOrd386 ) || ( ulImpOrd > DLLInfo[ ulDLLIdx ].ulMaxOrd386 ) ) )
            {
               /*
                * Issue error
                */
               ErrorMessageBox( MB_ERROR, "File imports unsupported ordinal %s.%lu, %s API tracing cannot be enabled", DLLInfo[ ulDLLIdx ].szName, ulImpOrd, DLLInfo[ ulDLLIdx ].szName );

               /*
                * Indicate tracing disabled
                */
               CLRFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL );
            }
         }

         /*
          * Process fixup record if import by name fixup
          */
         else if ( ( chTgtFlg & NRRTYP ) == NRRNAM )
         {
            /*
             * Extract 32-bit procedure name table offset from fixup record if
             * appropriate flag set
             */
            if ( FLAGSET( chTgtFlg, NR32BITOFF ) )
            {
               ulPrcOff = *(PULONG)( pEnabInfo->pFixupTbl + i );
               i += sizeof( ULONG );
            }

            /*
             * Extract 16-bit procedure name table offset from fixup record if
             * appropriate flag clear
             */
            else
            {
               ulPrcOff = (ULONG)*(PUSHORT)( pEnabInfo->pFixupTbl + i );
               i += sizeof( USHORT );
            }

            /*
             * Determine DLL information index (module index table zero-based,
             * module name table index one-based)
             */
            ulDLLIdx = pEnabInfo->pModIdxTbl[ usModOrd - 1 ];

            /*
             * Check for unsupported API name if fixup record references
             * supported DLL with trace on/off enabled
             */
            if ( ( ulDLLIdx != UNKNOWN_DLL                          ) &&
                 ( FLAGSET( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL ) ) )
            {
               /*
                * Obtain imported API name
                */
               pch = (PCHAR)( (ULONG)pEnabInfo->pPrcNamTbl + ulPrcOff );

               /*
                * Issue error and disable trace on/off if unsupported API name
                */
               if ( IsUnsupportedNRRNAM( DLLInfo[ ulDLLIdx ].szName, pch + 1, *pch ) )
               {
                  /*
                   * Issue error
                   */
                  ErrorMessageBox( MB_ERROR, "File imports %s.%.*s by name, %s API tracing cannot be enabled", DLLInfo[ ulDLLIdx ].szName, *pch, pch + 1, DLLInfo[ ulDLLIdx ].szName );

                  /*
                   * Indicate tracing disabled
                   */
                  CLRFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL );
               }
            }
         }

         /*
          * Fixup record contains additive value if not internal fixup record
          * and appropriate flag set
          */
         if ( ( ( chTgtFlg & NRRTYP ) != NRRINT ) && ( FLAGSET( chTgtFlg, NRADD ) ) )
         {
            /*
             * Skip 32-bit additive value if appropriate flag set
             */
            if ( FLAGSET( chTgtFlg, NR32BITADD ) )
            {
               i += sizeof( ULONG );
            }

            /*
             * Skip 16-bit additive value if appropriate flag clear
             */
            else
            {
               i += sizeof( USHORT );
            }
         }

         /*
          * Skip remainder of fixup record if fixup record contains chain of
          * source offsets
          */
         if ( FLAGSET( chSrcTyp, NRCHAIN ) )
         {
            i += chChnCnt * sizeof( USHORT );
         }
      }
   }

}  /* AnalyzeAPIUsage() */

/******************************************************************************/
/*                                                                            */
/* ControlDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY ControlDlgProc( HWND   hwndDlg       /* Dialog handle        */
                               , ULONG  ulMsg         /* Message number       */
                               , MPARAM mp1Dlg        /* Message parameter 1  */
                               , MPARAM mp2Dlg        /* Message parameter 2  */
                               )
{

   BYTE        bPriority;                        /* Queue data priority       */
   HEV         hevCtl = 0L;                      /* Trace control semaphore   */
   HQUEUE      hqueueCtl;                        /* Trace control queue       */
   int         i;                                /* Loop variable             */
   IPT         ipt = -1L;                        /* Insertion point           */
   PVOID       pData;                            /* Queue data                */
   REQUESTDATA sReqData;                         /* Queue request data        */
   SWP         swpDlg;                           /* Dialog window position    */
   CHAR        szModName[ LEN_FILENAME ];        /* Module name               */
   CHAR        szText[ CCHMAXPATH ];             /* Dialog text               */
   ULONG       ulCurElems;                       /* Current queue elements    */
   ULONG       ulDataLen;                        /* Queue data length         */
   ULONG       ulLength;                         /* Dialog text length        */
   ULONG       ulPost;                           /* Control semaphore posts   */
   ULONG       ulTotElems = 0L;                  /* Totle queue elements      */

   /*
    * Identify control dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Closing control dialog
       */
      case WM_CLOSE:
         /*
          * Translate as OK push button
          */
         WinPostMsg( hwndDlg, WM_COMMAND, MPFROMSHORT( DLG_CTLDLGOK ), MPVOID );

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying control dialog
       */
      case WM_COMMAND:
         /*
          * Identify control dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process help push button
             */
            case DLG_CTLDLGHELP:
               /*
                * Display control help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CTL ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_CTLDLGOK:
               /*
                * Close control dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing control dialog
       */
      case WM_INITDLG:
         /*
          * Obtain control dialog window position
          */
         WinQueryWindowPos( hwndDlg, &swpDlg );

         /*
          * Move control dialog to center of desktop
          */
         WinSetWindowPos( hwndDlg, 0L, ( swpDesktop.cx - swpDlg.cx ) / 2, ( swpDesktop.cy - swpDlg.cy ) / 2, 0L, 0L, SWP_MOVE );

         /*
          * Format control dialog title
          */
         sprintf( szText, "Tracing %s", FLAGSET( ulCtrlFlg, C_PAUSE ) ? "paused" : "resumed" );

         /*
          * Set control dialog title
          */
         WinSetWindowText( hwndDlg, szText );

         /*
          * Initialize control multi-line entry field
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_CTLMLE ), MLM_FORMAT, MPFROMSHORT( MLFIE_NOTRANS ), MPFROMLONG( 0L ) );
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_CTLMLE ), MLM_SETIMPORTEXPORT, MPFROMP( szText ), MPFROMLONG( sizeof( szText ) ) );
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_CTLMLE ), MLM_SETREADONLY, MPFROMSHORT( TRUE ), MPFROMLONG( 0L ) );
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_CTLMLE ), MLM_SETTEXTLIMIT, MPFROMLONG( sizeof( szText ) - 1 ), MPFROMLONG( 0L ) );
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_CTLMLE ), MLM_SETWRAP, MPFROMSHORT( FALSE ), MPFROMLONG( 0L ) );

         /*
          * Create trace control queue
          */
         DosCreateQueue( &hqueueCtl, QUE_FIFO, SYS_QUEUECTL );

         /*
          * Notify all running trace-enabled applications to pause tracing if
          * able to open pause semaphore
          */
         if ( DosOpenEventSem( FLAGSET( ulCtrlFlg, C_PAUSE ) ? SYS_SEMPAUSE : SYS_SEMRESUME, &hevCtl ) == NO_ERROR )
         {
            /*
             * Notify all running trace-enabled applications to pause tracing
             */
            DosPostEventSem( hevCtl );
            DosResetEventSem( hevCtl, &ulPost );
            DosCloseEventSem( hevCtl );

            /*
             * Wait for all running trace-enabled applications to respond to
             * pause request
             */
            for ( i = 0; ( DosQueryQueue( hqueueCtl, &ulCurElems ) == NO_ERROR ) && ( i < 10 ); i++, DosSleep( 1 ) )
            {
               if ( ulTotElems != ulCurElems )
               {
                  ulTotElems = ulCurElems;
                  i = 0;
               }
            }
         }

         /*
          * Display paused trace-enable applications if applications exist
          */
         if ( ulTotElems )
         {
            while ( ulTotElems-- > 0 )
            {
               if ( ( DosReadQueue( hqueueCtl, &sReqData, &ulDataLen, &pData, 0, DCWW_WAIT, &bPriority, 0L ) == NO_ERROR ) &&
                    ( sReqData.ulData == ( FLAGSET( ulCtrlFlg, C_PAUSE ) ? TRCCTL_PAUSE : TRCCTL_RESUME )                ) &&
                    ( DosQueryModuleName( ulDataLen, sizeof( szModName ), szModName ) == NO_ERROR                        ) )
               {
                  ulLength = sprintf( szText, "%s (PID %04lX) %s\n", szModName, sReqData.pid, FLAGSET( ulCtrlFlg, C_PAUSE ) ? "paused" : "resumed" );

                  /*
                   * Place API information line in control multi-line entry
                   * field
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_CTLMLE ), MLM_IMPORT, MPFROMP( &ipt ), MPFROMLONG( ulLength ) );
               }
            }
         }

         /*
          * Display message if no trace-enabled applications exist
          */
         else
         {
            ulLength = sprintf( szText, "No active trace-enabled applications\n" );

            /*
             * Place API information line in control multi-line entry
             * field
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_CTLMLE ), MLM_IMPORT, MPFROMP( &ipt ), MPFROMLONG( ulLength ) );
         }

         /*
          * Close trace control queue
          */
         DosCloseQueue( hqueueCtl );

         /*
          * Give control multi-line entry field focus
          */
         WinSetFocus( HWND_DESKTOP, WinWindowFromID( hwndDlg, DLG_CTLMLE ) );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* ControlDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustAltDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustAltDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   LONG lColor;                                  /* Foreground color index    */

   /*
    * Identify alternative directory dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying alternative directory dialog
       */
      case WM_COMMAND:
         /*
          * Identify alternative directory dialog control processing based on
          * command value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_ALTCANCEL:
               /*
                * Obtain new alternative directory, set to default if entry
                * field window disabled or unable to obtain directory
                */
               if ( ( !WinIsWindowEnabled( WinWindowFromID( hwndDlg, DLG_ALTDIR ) )                               ) ||
                    ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_ALTDIR ), sizeof( szAltDir ), szAltDir ) ) )
               {
                  strcpy( szAltDir, DEF_ALTDIR );
               }

               /*
                * Verify cancellation if new alternative directory different
                * from original
                */
               if ( Strcmpi( szAltDir, szOldAltDir ) )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "Alternative Directory", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close alternative directory dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_ALTDEFAULT:
               /*
                * Obtain new alternative directory, set to default if entry
                * field window disabled or unable to obtain directory
                */
               if ( ( !WinIsWindowEnabled( WinWindowFromID( hwndDlg, DLG_ALTDIR ) )                               ) ||
                    ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_ALTDIR ), sizeof( szAltDir ), szAltDir ) ) )
               {
                  strcpy( szAltDir, szOldAltDir );
               }

               /*
                * Reset new alternative directory to default if different
                */
               if ( Strcmpi( szAltDir, DEF_ALTDIR ) )
               {
                  /*
                   * Set new alternative directory to default
                   */
                  strcpy( szAltDir, DEF_ALTDIR );

                  /*
                   * Disable entry field window
                   */
                  WinEnableWindow( WinWindowFromID( hwndDlg, DLG_ALTDIR ), FALSE );

                  /*
                   * Set default radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_ALTDEFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_ALTHELP:
               /*
                * Display alternative directory help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTALT ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_ALTOK:
               /*
                * Obtain new alternative directory, set to default if entry
                * field window disabled or unable to obtain directory
                */
               if ( !WinIsWindowEnabled( WinWindowFromID( hwndDlg, DLG_ALTDIR ) ) )
               {
                  strcpy( szAltDir, DEF_ALTDIR );
               }

               /*
                * Issue error if new alternative directory empty or invalid
                */
               else if ( ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_ALTDIR ), sizeof( szAltDir ), szAltDir ) ) ||
                         ( strlen( szAltDir ) < 3                                                                      ) ||
                         ( szAltDir[ 1 ] != ':'                                                                        ) ||
                         ( szAltDir[ 2 ] != '\\'                                                                       ) )
               {
                  ErrorMessageBox( MB_ERROR, "Alternative directory must be fully qualified drive and path" );
                  break;
               }

               /*
                * Upper case new alternative directory if string argument valid
                */
               else
               {
                  Strupr( szAltDir );
               }

               /*
                * Save new alternative directory customization option in
                * operating system profile, issue error and terminate if
                * unsuccessful
                */
               if ( ( Strcmpi( szAltDir, szOldAltDir )                                             ) &&
                    ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szAltDirKey, szAltDir ) ) )
               {
                  Error( "Unable to store alternative directory customization option" );
               }

               /*
                * Close alternative directory dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_ALTUNDO:
               /*
                * Obtain new alternative directory, set to default if entry
                * field window disabled or unable to obtain directory
                */
               if ( ( !WinIsWindowEnabled( WinWindowFromID( hwndDlg, DLG_ALTDIR ) )                               ) ||
                    ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_ALTDIR ), sizeof( szAltDir ), szAltDir ) ) )
               {
                  strcpy( szAltDir, DEF_ALTDIR );
               }

               /*
                * Reset new alternative directory to original if different
                */
               if ( Strcmpi( szAltDir, szOldAltDir ) )
               {
                  /*
                   * Reset new alternative directory to original
                   */
                  strcpy( szAltDir, szOldAltDir );

                  /*
                   * Set appropriate windows if original directory alternative
                   * directory
                   */
                  if ( szAltDir[ 0 ] )
                  {
                     /*
                      * Set entry field window text
                      */
                     WinSetWindowText( WinWindowFromID( hwndDlg, DLG_ALTDIR ), szOldAltDir );

                     /*
                      * Enable entry field window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_ALTDIR ), TRUE );

                     /*
                      * Set directory radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_ALTDIRRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }

                  /*
                   * Set appropriate windows if original directory default
                   * directory
                   */
                  else
                  {
                     /*
                      * Disable entry field window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_ALTDIR ), FALSE );

                     /*
                      * Set default radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_ALTDEFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying alternative directory dialog
       */
      case WM_CONTROL:
         /*
          * Identify alternative directory dialog control processing based on
          * button control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process default radio button
                   */
                  case DLG_ALTDEFRAD:
                     /*
                      * Set new alternative directory to default
                      */
                     strcpy( szAltDir, DEF_ALTDIR );

                     /*
                      * Disable entry field window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_ALTDIR ), FALSE );

                     break;

                  /*
                   * Process directory radio button
                   */
                  case DLG_ALTDIRRAD:
                     /*
                      * Obtain new alternative directory
                      */
                     WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_ALTDIR ), sizeof( szAltDir ), szAltDir );

                     /*
                      * Enable entry field window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_ALTDIR ), TRUE );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing alternative directory dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old alternative directory customization option from
          * operating system profile, issue error and terminate if unsuccessful
          */
         if ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szAltDirKey, DEF_ALTDIR, szOldAltDir, sizeof( szOldAltDir ) ) == 0L )
         {
            Error( "Unable to retrieve alternative directory customization option" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ALTDEFTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ALTDIRTXT1 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ALTDIRTXT2 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Initialize entry field window
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_ALTDIR ), EM_SETTEXTLIMIT, MPFROMLONG( sizeof( szAltDir ) - 1 ), MPFROMLONG( 0L ) );

         /*
          * Set appropriate windows if no original alternative directory
          */
         if ( !Strcmpi( szOldAltDir, DEF_ALTDIR ) )
         {
            /*
             * Set default radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_ALTDEFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Set appropriate windows if original alternative directory
          */
         else
         {
            /*
             * Set entry field window text
             */
            WinSetWindowText( WinWindowFromID( hwndDlg, DLG_ALTDIR ), szOldAltDir );

            /*
             * Set directory radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_ALTDIRRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Give alternative directory customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;

      /*
       * Key pressed
       */
      case WM_CHAR:
         /*
          * Simulate OK push button if either enter key pressed in entry field
          */
         if ( ( SHORT1FROMMP( mp1Dlg ) & KC_VIRTUALKEY                                             ) &&
              ( ( SHORT2FROMMP( mp2Dlg ) == VK_ENTER ) || ( SHORT2FROMMP( mp2Dlg ) == VK_NEWLINE ) ) &&
              ( WinQueryFocus( HWND_DESKTOP ) == WinWindowFromID( hwndDlg, DLG_ALTDIR )            ) )
         {
            /*
             * Simulate OK push button
             */
            WinPostMsg( hwndDlg, WM_COMMAND, MPFROM2SHORT( DLG_ALTOK, 0 ), MPVOID );

            /*
             * Indicate focus window unchanged
             */
            return (MRESULT)FALSE;
         }

         break;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustAltDlgProc() */

/******************************************************************************/
/*                                                                            */
/* Buffer length dialog spin button information                               */
/*                                                                            */
/******************************************************************************/

#define DEF_BUFSBIDX 0

SBINFO BufSBInfo[] =
{
   { "bytes", MIN_BUFLEN,            MAX_BUFLEN, INC_BUFLEN,        1 },
   { "KB"   ,          1, MAX_BUFLEN / KILOBYTE,          1, KILOBYTE }
};

ULONG ulBufSBInfo = sizeof( BufSBInfo ) / sizeof( SBINFO );

/******************************************************************************/
/*                                                                            */
/* CustBufDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustBufDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   PCHAR apsz[sizeof(BufSBInfo)/sizeof(SBINFO)]; /* String array              */
   LONG  lColor;                                 /* Foreground color index    */
   CHAR  szFlags[ 16 ];                          /* Custom options string     */

   /*
    * Identify buffer length dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying buffer length dialog
       */
      case WM_COMMAND:
         /*
          * Identify buffer length dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_BUFCANCEL:
               /*
                * Verify cancellation if new buffer length different from
                * original
                */
               if ( ulNewBufLen * BufSBInfo[ sCurSBIdx ].ulMultiplier != ulOldBufLen )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "Buffer Length", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close buffer length dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_BUFDEFAULT:
               /*
                * Reset new buffer length to default length if different
                */
               if ( ulNewBufLen != DEF_BUFLENInt )
               {
                  /*
                   * Set new buffer length to default
                   */
                  ulNewBufLen = DEF_BUFLENInt;

                  /*
                   * Set type spin button to default if current spin button
                   * index not default
                   */
                  if ( sCurSBIdx != DEF_BUFSBIDX )
                  {
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_BUFSBIDX ), MPFROMSHORT( TRUE ) );
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMaxValue + BufSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMinValue ) );
                  }

                  /*
                   * Set value spin button to default buffer length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewBufLen ), MPVOID );

                  /*
                   * Set value radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_BUFHELP:
               /*
                * Display buffer length help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTBUF ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_BUFOK:
               /*
                * Save new buffer length customization option in operating
                * system profile, issue error and terminate if unsuccessful
                */
               if ( ( ulNewBufLen * BufSBInfo[ sCurSBIdx ].ulMultiplier != ulOldBufLen                                                                                                         ) &&
                    ( ( !sprintf( szFlags, "0x%08lX", ulNewBufLen * BufSBInfo[ sCurSBIdx ].ulMultiplier ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szBufLenKey, szFlags ) ) ) )
               {
                  Error( "Unable to store buffer length customization option" );
               }

               /*
                * Close buffer length dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_BUFUNDO:
               /*
                * Reset new buffer length to original length if different
                */
               if ( ulNewBufLen != ulOldBufLen )
               {
                  /*
                   * Set appropriate windows if no original buffer length
                   * maximum
                   */
                  if ( !( ulNewBufLen = ulOldBufLen ) )
                  {
                     /*
                      * Set type spin button to default if current spin button
                      * index not default
                      */
                     if ( sCurSBIdx != DEF_BUFSBIDX )
                     {
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_BUFSBIDX ), MPFROMSHORT( TRUE ) );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMaxValue + BufSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMinValue ) );
                     }

                     /*
                      * Set value spin button to default buffer length
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( DEF_BUFLENInt ), MPVOID );

                     /*
                      * Set ALL radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFALLRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }

                  /*
                   * Set appropriate windows if original buffer length maximum
                   */
                  else
                  {
                     /*
                      * Set type spin button to default if current spin button
                      * index not default
                      */
                     if ( sCurSBIdx != DEF_BUFSBIDX )
                     {
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_BUFSBIDX ), MPFROMSHORT( TRUE ) );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMaxValue + BufSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMinValue ) );
                     }

                     /*
                      * Set value spin button to original buffer length
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewBufLen ), MPVOID );

                     /*
                      * Set value radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying buffer length dialog
       */
      case WM_CONTROL:
         /*
          * Identify buffer length dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process ALL radio button
                   */
                  case DLG_BUFALLRAD:
                     /*
                      * Set new buffer length to all
                      */
                     ulNewBufLen = 0L;

                     /*
                      * Disable value and type spin button windows
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), FALSE );
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), FALSE );

                     break;

                  /*
                   * Process value radio button
                   */
                  case DLG_BUFVALRAD:
                     /*
                      * Obtain new buffer length from value spin button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_QUERYVALUE, MPFROMP( &ulNewBufLen ), MPFROM2SHORT( 0, SPBQ_DONOTUPDATE ) );

                     /*
                      * Enable value and type spin button windows
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), TRUE );
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), TRUE );

                     break;

               }
               break;

            /*
             * Spinning spin button down
             */
            case SPBN_DOWNARROW:
               /*
                * Spinning value spin button down
                */
               if ( SHORT1FROMMP( mp1Dlg ) == DLG_BUFVALSPIN )
               {
                  /*
                   * Set new buffer length to maximum if current length under
                   * minimum
                   */
                  if ( ulNewBufLen <= BufSBInfo[ sCurSBIdx ].ulMinValue )
                  {
                     ulNewBufLen = BufSBInfo[ sCurSBIdx ].ulMaxValue;
                  }

                  /*
                   * Decrement new buffer length by appropriate amount
                   */
                  else
                  {
                     ulNewBufLen -= BufSBInfo[ sCurSBIdx ].ulIncrement;
                  }

                  /*
                   * Set value spin button to decremented buffer length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewBufLen ), MPVOID );
               }

               /*
                * Spinning type spin button down
                */
               else if ( SHORT1FROMMP( mp1Dlg ) == DLG_BUFTYPSPIN )
               {
                  /*
                   * Decrement current spin button index, reset to maximum
                   * index if less than zero
                   */
                  if ( --sCurSBIdx < 0 )
                  {
                     sCurSBIdx = ulBufSBInfo - 1;
                  }

                  /*
                   * Adjust new buffer length by appropriate amount
                   */
                  switch ( sCurSBIdx )
                  {
                     /*
                      * New buffer length in bytes
                      */
                     case 0:
                        ulNewBufLen <<= 10L;
                        break;

                     /*
                      * New buffer length in kilobytes
                      */
                     case 1:
                        if ( ulNewBufLen & ( KILOBYTE - 1 ) )
                        {
                           ulNewBufLen += KILOBYTE - 1;
                        }
                        ulNewBufLen >>= 10L;
                        break;
                  }

                  /*
                   * Set value spin button limits
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMaxValue + BufSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMinValue ) );

                  /*
                   * Set value spin button to adjusted buffer length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewBufLen ), MPVOID );
               }

               break;

            /*
             * Spinning spin button up
             */
            case SPBN_UPARROW:
               /*
                * Spinning value spin button up
                */
               if ( SHORT1FROMMP( mp1Dlg ) == DLG_BUFVALSPIN )
               {
                  /*
                   * Set new buffer length to minimum if current length over
                   * maximum
                   */
                  if ( ulNewBufLen >= BufSBInfo[ sCurSBIdx ].ulMaxValue )
                  {
                     ulNewBufLen = BufSBInfo[ sCurSBIdx ].ulMinValue;
                  }

                  /*
                   * Increment new buffer length by appropriate amount
                   */
                  else
                  {
                     ulNewBufLen += BufSBInfo[ sCurSBIdx ].ulIncrement;
                  }

                  /*
                   * Set value spin button to incremented buffer length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewBufLen ), MPVOID );
               }

               /*
                * Spinning type spin button up
                */
               else if ( SHORT1FROMMP( mp1Dlg ) == DLG_BUFTYPSPIN )
               {
                  /*
                   * Increment current spin button index, reset to minimum
                   * index if greater than maximum
                   */
                  if ( ++sCurSBIdx >= ulBufSBInfo )
                  {
                     sCurSBIdx = 0;
                  }

                  /*
                   * Adjust new buffer length by appropriate amount
                   */
                  switch ( sCurSBIdx )
                  {
                     /*
                      * New buffer length in bytes
                      */
                     case 0:
                        ulNewBufLen <<= 10L;
                        break;

                     /*
                      * New buffer length in kilobytes
                      */
                     case 1:
                        if ( ulNewBufLen & ( KILOBYTE - 1 ) )
                        {
                           ulNewBufLen += KILOBYTE - 1;
                        }
                        ulNewBufLen >>= 10L;
                        break;
                  }

                  /*
                   * Set value spin button limits
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMaxValue + BufSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( BufSBInfo[ sCurSBIdx ].ulMinValue ) );

                  /*
                   * Set value spin button to adjusted buffer length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewBufLen ), MPVOID );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing buffer length dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old buffer length customization option from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szBufLenKey, DEF_BUFLEN, szFlags, sizeof( szFlags ) ) != sizeof( DEF_BUFLEN ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldBufLen ) != 1 ) )
         {
            Error( "Unable to retrieve buffer length customization option" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_BUFALLTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_BUFVALTXT1 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_BUFVALTXT2 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_BUFVALTXT3 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set value spin button upper and lower limits
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( BufSBInfo[ DEF_BUFSBIDX ].ulMaxValue + BufSBInfo[ DEF_BUFSBIDX ].ulIncrement ), MPFROMLONG( BufSBInfo[ DEF_BUFSBIDX ].ulMinValue ) );

         /*
          * Initialize string array
          */
         for ( sCurSBIdx = 0; sCurSBIdx < ulBufSBInfo; sCurSBIdx++ )
         {
            apsz[ sCurSBIdx ] = BufSBInfo[ sCurSBIdx ].szName;
         }

         /*
          * Set type spin button values
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), SPBM_SETARRAY, MPFROMP( apsz ), MPFROMSHORT( ulBufSBInfo ) );

         /*
          * Set type spin button current value
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMSHORT( sCurSBIdx = DEF_BUFSBIDX ), MPVOID );

         /*
          * Set appropriate windows if no original buffer length maximum
          */
         if ( !( ulNewBufLen = ulOldBufLen ) )
         {
            /*
             * Set value spin button to default buffer length
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( DEF_BUFLENInt ), MPVOID );

            /*
             * Set ALL radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFALLRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Set appropriate windows if original buffer length maximum
          */
         else
         {
            /*
             * Set value spin button to initial buffer length
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewBufLen ), MPVOID );

            /*
             * Set value radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_BUFVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Give buffer length customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustBufDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustCtlDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustCtlDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   LONG lColor;                                  /* Foreground color index    */
   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify tracing control dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying tracing control dialog
       */
      case WM_COMMAND:
         /*
          * Identify tracing control dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_CTLCANCEL:
               /*
                * Verify cancellation if new tracing control different from
                * original
                */
               if ( FLAGSET( ulNewTrcFlg, F_CONTROL ) != FLAGSET( ulOldTrcFlg, F_CONTROL ) )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "Tracing Control", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close tracing control dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_CTLDEFAULT:
               /*
                * Reset tracing control to default interception if different
                */
               if ( FLAGSET( ulNewTrcFlg, F_CONTROL ) != FLAGSET( DEF_TRCFLGInt, F_CONTROL ) )
               {
                  /*
                   * Set new tracing control to default
                   */
                  if ( FLAGSET( DEF_TRCFLGInt, F_CONTROL ) )
                  {
                     SETFLAG( ulNewTrcFlg, F_CONTROL );
                  }
                  else
                  {
                     CLRFLAG( ulNewTrcFlg, F_CONTROL );
                  }

                  /*
                   * Set default tracing control radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ulNewTrcFlg, F_CONTROL ) ? DLG_CTLONRAD : DLG_CTLOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_CTLHELP:
               /*
                * Display tracing control help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTCTL ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_CTLOK:
               /*
                * Save new tracing control customization option in operating
                * system profile, issue error and terminate if unsuccessful
                */
               if ( ( ulNewTrcFlg != ulOldTrcFlg ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewTrcFlg ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, szFlags ) ) ) )
               {
                  Error( "Unable to store tracing control customization option" );
               }

               /*
                * Close tracing control dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_CTLUNDO:
               /*
                * Reset tracing control to original interception if different
                */
               if ( FLAGSET( ulNewTrcFlg, F_CONTROL ) != FLAGSET( ulOldTrcFlg, F_CONTROL ) )
               {
                  /*
                   * Set new tracing control to original
                   */
                  if ( FLAGSET( ulOldTrcFlg, F_CONTROL ) )
                  {
                     SETFLAG( ulNewTrcFlg, F_CONTROL );
                  }
                  else
                  {
                     CLRFLAG( ulNewTrcFlg, F_CONTROL );
                  }

                  /*
                   * Set original tracing control radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ulNewTrcFlg, F_CONTROL ) ? DLG_CTLONRAD : DLG_CTLOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying tracing control dialog
       */
      case WM_CONTROL:
         /*
          * Identify tracing control dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process enable radio button
                   */
                  case DLG_CTLONRAD:
                     /*
                      * Set new tracing control to on
                      */
                     SETFLAG( ulNewTrcFlg, F_CONTROL );

                     break;

                  /*
                   * Process disable radio button
                   */
                  case DLG_CTLOFFRAD:
                     /*
                      * Set new tracing control to off
                      */
                     CLRFLAG( ulNewTrcFlg, F_CONTROL );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing tracing control dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old tracing control customization option from operating
          * system profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, DEF_TRCFLG, szFlags, sizeof( szFlags ) ) != sizeof( DEF_TRCFLG ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldTrcFlg ) != 1 ) )
         {
            Error( "Unable to retrieve tracing control customization option" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_CTLONTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_CTLOFFTXT ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set original stamping radio button
          */
         WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ( ulNewTrcFlg = ulOldTrcFlg ), F_CONTROL ) ? DLG_CTLONRAD : DLG_CTLOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );

         /*
          * Give tracing control customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustCtlDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustDosDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustDosDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify DOSCALLS APIs dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying DOSCALLS APIs dialog
       */
      case WM_COMMAND:
         /*
          * Identify DOSCALLS APIs dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_DOSCANCEL:
               /*
                * Verify cancellation if new DOSCALLS APIs different from
                * original
                */
               if ( ulNewDosGrp != ulOldDosGrp )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "DOSCALLS APIs", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close DOSCALLS APIs dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process clear push button
             */
            case DLG_DOSCLEAR:
               /*
                * Reset new DOSCALLS APIs to zero APIs if different
                */
               if ( ulNewDosGrp )
               {
                  /*
                   * Set new DOSCALLS APIs to zero
                   */
                  ulNewDosGrp = 0L;

                  /*
                   * Set DOSCALLS APIs customization dialog check buttons
                   */
                  SetCustDosDlgCheck( hwndDlg );
               }

               break;

            /*
             * Process default push button
             */
            case DLG_DOSDEFAULT:
               /*
                * Reset new DOSCALLS APIs to default APIs if different
                */
               if ( ulNewDosGrp != DEF_DOSGRPInt )
               {
                  /*
                   * Set new DOSCALLS APIs to default
                   */
                  ulNewDosGrp = DEF_DOSGRPInt;

                  /*
                   * Set DOSCALLS APIs customization dialog check buttons
                   */
                  SetCustDosDlgCheck( hwndDlg );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_DOSHELP:
               /*
                * Display DOSCALLS APIs help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTDOS ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_DOSOK:
               /*
                * Issue error if new DOSCALLS APIs zero
                */
               if ( !ulNewDosGrp )
               {
                  ErrorMessageBox( MB_ERROR, "No DOSCALLS API groups selected" );
               }

               /*
                * Save new DOSCALLS APIs customization option in operating
                * system profile, issue error and terminate if unsuccessful
                */
               else if ( ( ulNewDosGrp != ulOldDosGrp ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewDosGrp ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szDosGrpKey, szFlags ) ) ) )
               {
                  Error( "Unable to store DOSCALLS APIs customization option" );
               }

               /*
                * Close DOSCALLS APIs dialog indicating OK
                */
               else
               {
                  WinDismissDlg( hwndDlg, DID_OK );
               }

               break;

            /*
             * Process undo push button
             */
            case DLG_DOSUNDO:
               /*
                * Reset new DOSCALLS APIs to original APIs if different
                */
               if ( ulNewDosGrp != ulOldDosGrp )
               {
                  /*
                   * Set new DOSCALLS APIs to original
                   */
                  ulNewDosGrp = ulOldDosGrp;

                  /*
                   * Set DOSCALLS APIs customization dialog check buttons
                   */
                  SetCustDosDlgCheck( hwndDlg );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying DOSCALLS APIs dialog
       */
      case WM_CONTROL:
         /*
          * Identify DOSCALLS APIs dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify check box control processing based on command value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process DOSCALLS DEV APIs check box
                   */
                  case DLG_DOSDEVCHK:
                     /*
                      * Toggle DOSCALLS DEV APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_DEV ) ) CLRFLAG( ulNewDosGrp, D_DEV ); else SETFLAG( ulNewDosGrp, D_DEV );

                     break;

                  /*
                   * Process DOSCALLS FILE APIs check box
                   */
                  case DLG_DOSFILECHK:
                     /*
                      * Toggle DOSCALLS FILE APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_FILE ) ) CLRFLAG( ulNewDosGrp, D_FILE ); else SETFLAG( ulNewDosGrp, D_FILE );

                     break;

                  /*
                   * Process DOSCALLS INFO APIs check box
                   */
                  case DLG_DOSINFOCHK:
                     /*
                      * Toggle DOSCALLS INFO APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_INFO ) ) CLRFLAG( ulNewDosGrp, D_INFO ); else SETFLAG( ulNewDosGrp, D_INFO );

                     break;

                  /*
                   * Process DOSCALLS MEM APIs check box
                   */
                  case DLG_DOSMEMCHK:
                     /*
                      * Toggle DOSCALLS MEM APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_MEM ) ) CLRFLAG( ulNewDosGrp, D_MEM ); else SETFLAG( ulNewDosGrp, D_MEM );

                     break;

                  /*
                   * Process DOSCALLS MISC APIs check box
                   */
                  case DLG_DOSMISCCHK:
                     /*
                      * Toggle DOSCALLS MISC APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_MISC ) ) CLRFLAG( ulNewDosGrp, D_MISC ); else SETFLAG( ulNewDosGrp, D_MISC );

                     break;

                  /*
                   * Process DOSCALLS MOD APIs check box
                   */
                  case DLG_DOSMODCHK:
                     /*
                      * Toggle DOSCALLS MOD APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_MOD ) ) CLRFLAG( ulNewDosGrp, D_MOD ); else SETFLAG( ulNewDosGrp, D_MOD );

                     break;

                  /*
                   * Process DOSCALLS MSG APIs check box
                   */
                  case DLG_DOSMSGCHK:
                     /*
                      * Toggle DOSCALLS MSG APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_MSG ) ) CLRFLAG( ulNewDosGrp, D_MSG ); else SETFLAG( ulNewDosGrp, D_MSG );

                     break;

                  /*
                   * Process DOSCALLS MVDM APIs check box
                   */
                  case DLG_DOSMVDMCHK:
                     /*
                      * Toggle DOSCALLS MVDM APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_MVDM ) ) CLRFLAG( ulNewDosGrp, D_MVDM ); else SETFLAG( ulNewDosGrp, D_MVDM );

                     break;

                  /*
                   * Process DOSCALLS NLS APIs check box
                   */
                  case DLG_DOSNLSCHK:
                     /*
                      * Toggle DOSCALLS NLS APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_NLS ) ) CLRFLAG( ulNewDosGrp, D_NLS ); else SETFLAG( ulNewDosGrp, D_NLS );

                     break;

                  /*
                   * Process DOSCALLS PIPE APIs check box
                   */
                  case DLG_DOSPIPECHK:
                     /*
                      * Toggle DOSCALLS PIPE APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_PIPE ) ) CLRFLAG( ulNewDosGrp, D_PIPE ); else SETFLAG( ulNewDosGrp, D_PIPE );

                     break;

                  /*
                   * Process DOSCALLS PRF APIs check box
                   */
                  case DLG_DOSPRFCHK:
                     /*
                      * Toggle DOSCALLS PRF APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_PRF ) ) CLRFLAG( ulNewDosGrp, D_PRF ); else SETFLAG( ulNewDosGrp, D_PRF );

                     break;

                  /*
                   * Process DOSCALLS PROC APIs check box
                   */
                  case DLG_DOSPROCCHK:
                     /*
                      * Toggle DOSCALLS PROC APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_PROC ) ) CLRFLAG( ulNewDosGrp, D_PROC ); else SETFLAG( ulNewDosGrp, D_PROC );

                     break;

                  /*
                   * Process DOSCALLS PROF APIs check box
                   */
                  case DLG_DOSPROFCHK:
                     /*
                      * Toggle DOSCALLS PROF APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_PROF ) ) CLRFLAG( ulNewDosGrp, D_PROF ); else SETFLAG( ulNewDosGrp, D_PROF );

                     break;

                  /*
                   * Process DOSCALLS RES APIs check box
                   */
                  case DLG_DOSRESCHK:
                     /*
                      * Toggle DOSCALLS RES APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_RES ) ) CLRFLAG( ulNewDosGrp, D_RES ); else SETFLAG( ulNewDosGrp, D_RES );

                     break;

                  /*
                   * Process DOSCALLS SEM APIs check box
                   */
                  case DLG_DOSSEMCHK:
                     /*
                      * Toggle DOSCALLS SEM APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_SEM ) ) CLRFLAG( ulNewDosGrp, D_SEM ); else SETFLAG( ulNewDosGrp, D_SEM );

                     break;

                  /*
                   * Process DOSCALLS SES APIs check box
                   */
                  case DLG_DOSSESCHK:
                     /*
                      * Toggle DOSCALLS SES APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_SES ) ) CLRFLAG( ulNewDosGrp, D_SES ); else SETFLAG( ulNewDosGrp, D_SES );

                     break;

                  /*
                   * Process DOSCALLS SIG APIs check box
                   */
                  case DLG_DOSSIGCHK:
                     /*
                      * Toggle DOSCALLS SIG APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_SIG ) ) CLRFLAG( ulNewDosGrp, D_SIG ); else SETFLAG( ulNewDosGrp, D_SIG );

                     break;

                  /*
                   * Process DOSCALLS SMP APIs check box
                   */
                  case DLG_DOSSMPCHK:
                     /*
                      * Toggle DOSCALLS SMP APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_SMP ) ) CLRFLAG( ulNewDosGrp, D_SMP ); else SETFLAG( ulNewDosGrp, D_SMP );

                     break;

                  /*
                   * Process DOSCALLS TIME APIs check box
                   */
                  case DLG_DOSTIMECHK:
                     /*
                      * Toggle DOSCALLS TIME APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_TIME ) ) CLRFLAG( ulNewDosGrp, D_TIME ); else SETFLAG( ulNewDosGrp, D_TIME );

                     break;

                  /*
                   * Process DOSCALLS XCPT APIs check box
                   */
                  case DLG_DOSXCPTCHK:
                     /*
                      * Toggle DOSCALLS XCPT APIs flag
                      */
                     if ( FLAGSET( ulNewDosGrp, D_XCPT ) ) CLRFLAG( ulNewDosGrp, D_XCPT ); else SETFLAG( ulNewDosGrp, D_XCPT );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing DOSCALLS APIs dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old DOSCALLS APIs customization option from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szDosGrpKey, DEF_DOSGRP, szFlags, sizeof( szFlags ) ) != sizeof( DEF_DOSGRP ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldDosGrp ) != 1 ) )
         {
            Error( "Unable to retrieve DOSCALLS APIs customization option" );
         }

         /*
          * Set new DOSCALLS APIs to original
          */
         ulNewDosGrp = ulOldDosGrp;

         /*
          * Set DOSCALLS APIs customization dialog check buttons
          */
         SetCustDosDlgCheck( hwndDlg );

         /*
          * Give DOSCALLS APIs customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustDosDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustEBCDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustEBCDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   LONG lColor;                                  /* Foreground color index    */
   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify EBCDIC translation dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying EBCDIC translation dialog
       */
      case WM_COMMAND:
         /*
          * Identify EBCDIC translation dialog control processing based on
          * command value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_EBCCANCEL:
               /*
                * Verify cancellation if new EBCDIC translation different from
                * original
                */
               if ( FLAGSET( ulNewTrcFlg, F_EBCDIC ) != FLAGSET( ulOldTrcFlg, F_EBCDIC ) )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "EBCDIC Translation", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close EBCDIC translation dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_EBCDEFAULT:
               /*
                * Reset EBCDIC translation to default interception if different
                */
               if ( FLAGSET( ulNewTrcFlg, F_EBCDIC ) != FLAGSET( DEF_TRCFLGInt, F_EBCDIC ) )
               {
                  /*
                   * Set new EBCDIC translation to default
                   */
                  if ( FLAGSET( DEF_TRCFLGInt, F_EBCDIC ) )
                  {
                     SETFLAG( ulNewTrcFlg, F_EBCDIC );
                  }
                  else
                  {
                     CLRFLAG( ulNewTrcFlg, F_EBCDIC );
                  }

                  /*
                   * Set default EBCDIC translation radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ulNewTrcFlg, F_EBCDIC ) ? DLG_EBCONRAD : DLG_EBCOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_EBCHELP:
               /*
                * Display EBCDIC translation help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTEBC ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_EBCOK:
               /*
                * Save new EBCDIC translation customization option in operating
                * system profile, issue error and terminate if unsuccessful
                */
               if ( ( ulNewTrcFlg != ulOldTrcFlg ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewTrcFlg ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, szFlags ) ) ) )
               {
                  Error( "Unable to store EBCDIC translation customization option" );
               }

               /*
                * Close EBCDIC translation dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_EBCUNDO:
               /*
                * Reset EBCDIC translation to original interception if
                * different
                */
               if ( FLAGSET( ulNewTrcFlg, F_EBCDIC ) != FLAGSET( ulOldTrcFlg, F_EBCDIC ) )
               {
                  /*
                   * Set new EBCDIC translation to original
                   */
                  if ( FLAGSET( ulOldTrcFlg, F_EBCDIC ) )
                  {
                     SETFLAG( ulNewTrcFlg, F_EBCDIC );
                  }
                  else
                  {
                     CLRFLAG( ulNewTrcFlg, F_EBCDIC );
                  }

                  /*
                   * Set original EBCDIC translation radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ulNewTrcFlg, F_EBCDIC ) ? DLG_EBCONRAD : DLG_EBCOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying EBCDIC translation dialog
       */
      case WM_CONTROL:
         /*
          * Identify EBCDIC translation dialog control processing based on
          * button control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process enable radio button
                   */
                  case DLG_EBCONRAD:
                     /*
                      * Set new EBCDIC translation to on
                      */
                     SETFLAG( ulNewTrcFlg, F_EBCDIC );

                     break;

                  /*
                   * Process disable radio button
                   */
                  case DLG_EBCOFFRAD:
                     /*
                      * Set new EBCDIC translation to off
                      */
                     CLRFLAG( ulNewTrcFlg, F_EBCDIC );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing EBCDIC translation dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old EBCDIC translation customization option from operating
          * system profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, DEF_TRCFLG, szFlags, sizeof( szFlags ) ) != sizeof( DEF_TRCFLG ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldTrcFlg ) != 1 ) )
         {
            Error( "Unable to retrieve EBCDIC translation customization option" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_EBCONTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_EBCOFFTXT ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set original stamping radio button
          */
         WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ( ulNewTrcFlg = ulOldTrcFlg ), F_EBCDIC ) ? DLG_EBCONRAD : DLG_EBCOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );

         /*
          * Give EBCDIC translation customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustEBCDlgProc() */

/******************************************************************************/
/*                                                                            */
/* File wrapping dialog spin button information                               */
/*                                                                            */
/******************************************************************************/

#define DEF_FILSBIDX 0

SBINFO FilSBInfo[] =
{
   { "bytes",            MIN_FILLEN,            MAX_FILLEN,            INC_FILLEN,        1 },
   { "KB"   , MIN_FILLEN / KILOBYTE, MAX_FILLEN / KILOBYTE, INC_FILLEN / KILOBYTE, KILOBYTE },
   { "MB"   ,                     1, MAX_FILLEN / MEGABYTE,                     1, MEGABYTE }
};

ULONG ulFilSBInfo = sizeof( FilSBInfo ) / sizeof( SBINFO );

/******************************************************************************/
/*                                                                            */
/* CustFilDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustFilDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   PCHAR apsz[sizeof(FilSBInfo)/sizeof(SBINFO)]; /* String array              */
   LONG lColor;                                  /* Foreground color index    */
   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify file wrapping dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying file wrapping dialog
       */
      case WM_COMMAND:
         /*
          * Identify file wrapping dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_FILCANCEL:
               /*
                * Verify cancellation if new file wrapping length different
                * from original
                */
               if ( ulNewFilLen * FilSBInfo[ sCurSBIdx ].ulMultiplier != ulOldFilLen )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "File Wrapping", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close file wrapping dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_FILDEFAULT:
               /*
                * Reset new file wrapping length to default length if different
                */
               if ( ulNewFilLen != DEF_FILLENInt )
               {
                  /*
                   * Set new file wrapping length to default
                   */
                  ulNewFilLen = DEF_FILLENInt;

                  /*
                   * Set type spin button to default if current spin button
                   * index not default
                   */
                  if ( sCurSBIdx != DEF_FILSBIDX )
                  {
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_FILSBIDX ), MPFROMSHORT( TRUE ) );
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMaxValue + FilSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMinValue ) );
                  }

                  /*
                   * Set ALL radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILALLRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_FILHELP:
               /*
                * Display file wrapping help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTFIL ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_FILOK:
               /*
                * Save new file wrapping customization option in operating
                * system profile, issue error and terminate if unsuccessful
                */
               if ( ( ulNewFilLen * FilSBInfo[ sCurSBIdx ].ulMultiplier != ulOldFilLen                                                                                                         ) &&
                    ( ( !sprintf( szFlags, "0x%08lX", ulNewFilLen * FilSBInfo[ sCurSBIdx ].ulMultiplier ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szFilLenKey, szFlags ) ) ) )
               {
                  Error( "Unable to store file wrapping customization option" );
               }

               /*
                * Close file wrapping dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_FILUNDO:
               /*
                * Reset new file wrapping length to original length if
                * different
                */
               if ( ulNewFilLen != ulOldFilLen )
               {
                  /*
                   * Set appropriate windows if no original file wrapping
                   * length maximum
                   */
                  if ( !( ulNewFilLen = ulOldFilLen ) )
                  {
                     /*
                      * Set type spin button to default if current spin button
                      * index not default
                      */
                     if ( sCurSBIdx != DEF_FILSBIDX )
                     {
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_FILSBIDX ), MPFROMSHORT( TRUE ) );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMaxValue + FilSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMinValue ) );
                     }

                     /*
                      * Set value spin button to default file wrapping length
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( DEF_FILLENInt ), MPVOID );

                     /*
                      * Set ALL radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILALLRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }

                  /*
                   * Set appropriate windows if original file wrapping length
                   * maximum
                   */
                  else
                  {
                     /*
                      * Set type spin button to default if current spin button
                      * index not default
                      */
                     if ( sCurSBIdx != DEF_FILSBIDX )
                     {
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_FILSBIDX ), MPFROMSHORT( TRUE ) );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMaxValue + FilSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMinValue ) );
                     }

                     /*
                      * Set value spin button to original file wrapping length
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewFilLen ), MPVOID );

                     /*
                      * Set value radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying file wrapping dialog
       */
      case WM_CONTROL:
         /*
          * Identify file wrapping dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process ALL radio button
                   */
                  case DLG_FILALLRAD:
                     /*
                      * Set new file wrapping length to all
                      */
                     ulNewFilLen = 0L;

                     /*
                      * Disable value and type spin button windows
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), FALSE );
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), FALSE );

                     break;

                  /*
                   * Process value radio button
                   */
                  case DLG_FILVALRAD:
                     /*
                      * Obtain new file wrapping length from value spin button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_QUERYVALUE, MPFROMP( &ulNewFilLen ), MPFROM2SHORT( 0, SPBQ_DONOTUPDATE ) );

                     /*
                      * Enable value and type spin button windows
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), TRUE );
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), TRUE );

                     break;
               }
               break;

            /*
             * Spinning spin button down
             */
            case SPBN_DOWNARROW:
               /*
                * Spinning value spin button down
                */
               if ( SHORT1FROMMP( mp1Dlg ) == DLG_FILVALSPIN )
               {
                  /*
                   * Set new file wrapping length to maximum if current length
                   * under minimum
                   */
                  if ( ulNewFilLen <= FilSBInfo[ sCurSBIdx ].ulMinValue )
                  {
                     ulNewFilLen = FilSBInfo[ sCurSBIdx ].ulMaxValue;
                  }

                  /*
                   * Decrement new file wrapping length by appropriate amount
                   */
                  else
                  {
                     ulNewFilLen -= FilSBInfo[ sCurSBIdx ].ulIncrement;
                  }

                  /*
                   * Set value spin button to decremented file wrapping length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewFilLen ), MPVOID );
               }

               /*
                * Spinning type spin button down
                */
               else if ( SHORT1FROMMP( mp1Dlg ) == DLG_FILTYPSPIN )
               {
                  /*
                   * Decrement current spin button index, reset to maximum
                   * index if less than zero
                   */
                  if ( --sCurSBIdx < 0 )
                  {
                     sCurSBIdx = ulFilSBInfo - 1;
                  }

                  /*
                   * Adjust new file wrapping length by appropriate amount
                   */
                  switch ( sCurSBIdx )
                  {
                     /*
                      * New file wrapping length in bytes
                      */
                     case 0:
                        ulNewFilLen <<= 10L;
                        break;

                     /*
                      * New file wrapping length in kilobytes
                      */
                     case 1:
                        ulNewFilLen <<= 10L;
                        break;

                     /*
                      * New file wrapping length in megabytes
                      */
                     case 2:
                        if ( ulNewFilLen & ( MEGABYTE - 1 ) )
                        {
                           ulNewFilLen += MEGABYTE - 1;
                        }
                        ulNewFilLen >>= 20L;
                        break;
                  }

                  /*
                   * Set value spin button limits
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMaxValue + FilSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMinValue ) );

                  /*
                   * Set value spin button to adjusted file wrapping length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewFilLen ), MPVOID );
               }

               break;

            /*
             * Spinning spin button up
             */
            case SPBN_UPARROW:
               /*
                * Spinning value spin button up
                */
               if ( SHORT1FROMMP( mp1Dlg ) == DLG_FILVALSPIN )
               {
                  /*
                   * Set new file wrapping length to minimum if current length
                   * over maximum
                   */
                  if ( ulNewFilLen >= FilSBInfo[ sCurSBIdx ].ulMaxValue )
                  {
                     ulNewFilLen = FilSBInfo[ sCurSBIdx ].ulMinValue;
                  }

                  /*
                   * Increment new file wrapping length by appropriate amount
                   */
                  else
                  {
                     ulNewFilLen += FilSBInfo[ sCurSBIdx ].ulIncrement;
                  }

                  /*
                   * Set value spin button to incremented file wrapping length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewFilLen ), MPVOID );
               }

               /*
                * Spinning type spin button up
                */
               else if ( SHORT1FROMMP( mp1Dlg ) == DLG_FILTYPSPIN )
               {
                  /*
                   * Increment current spin button index, reset to minimum
                   * index if greater than maximum
                   */
                  if ( ++sCurSBIdx >= ulFilSBInfo )
                  {
                     sCurSBIdx = 0;
                  }

                  /*
                   * Adjust new file wrapping length by appropriate amount
                   */
                  switch ( sCurSBIdx )
                  {
                     /*
                      * New file wrapping length in bytes
                      */
                     case 0:
                        ulNewFilLen <<= 20L;
                        break;

                     /*
                      * New file wrapping length in kilobytes
                      */
                     case 1:
                        if ( ulNewFilLen & ( KILOBYTE - 1 ) )
                        {
                           ulNewFilLen += KILOBYTE - 1;
                        }
                        ulNewFilLen >>= 10L;
                        break;

                     /*
                      * New file wrapping length in megabytes
                      */
                     case 2:
                        if ( ulNewFilLen & ( KILOBYTE - 1 ) )
                        {
                           ulNewFilLen += KILOBYTE - 1;
                        }
                        ulNewFilLen >>= 10L;
                        break;
                  }

                  /*
                   * Set value spin button limits
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMaxValue + FilSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( FilSBInfo[ sCurSBIdx ].ulMinValue ) );

                  /*
                   * Set value spin button to adjusted file wrapping length
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewFilLen ), MPVOID );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing file wrapping dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old file wrapping customization option from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szFilLenKey, DEF_FILLEN, szFlags, sizeof( szFlags ) ) != sizeof( DEF_FILLEN ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldFilLen ) != 1 ) )
         {
            Error( "Unable to retrieve file wrapping customization option" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_FILALLTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_FILVALTXT1 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_FILVALTXT2 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set value spin button upper and lower limits
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( FilSBInfo[ DEF_FILSBIDX ].ulMaxValue + FilSBInfo[ DEF_FILSBIDX ].ulIncrement ), MPFROMLONG( FilSBInfo[ DEF_FILSBIDX ].ulMinValue ) );

         /*
          * Initialize string array
          */
         for ( sCurSBIdx = 0; sCurSBIdx < ulFilSBInfo; sCurSBIdx++ )
         {
            apsz[ sCurSBIdx ] = FilSBInfo[ sCurSBIdx ].szName;
         }

         /*
          * Set type spin button values
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), SPBM_SETARRAY, MPFROMP( apsz ), MPFROMSHORT( ulFilSBInfo ) );

         /*
          * Set type spin button current value
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMSHORT( sCurSBIdx = DEF_FILSBIDX ), MPVOID );

         /*
          * Set appropriate windows if no original file wrapping length maximum
          */
         if ( !( ulNewFilLen = ulOldFilLen ) )
         {
            /*
             * Set value spin button to default file wrapping length
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( DEF_FILLENInt ), MPVOID );

            /*
             * Set ALL radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILALLRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Set appropriate windows if original file wrapping length maximum
          */
         else
         {
            /*
             * Set value spin button to initial file wrapping length
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewFilLen ), MPVOID );

            /*
             * Set value radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_FILVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Give file wrapping customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustFilDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustGpiDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustGpiDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify PMGPI APIs dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying PMGPI APIs dialog
       */
      case WM_COMMAND:
         /*
          * Identify PMGPI APIs dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_GPICANCEL:
               /*
                * Verify cancellation if new PMGPI APIs different from original
                */
               if ( ulNewGpiGrp != ulOldGpiGrp )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "PMGPI APIs", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close PMGPI APIs dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process clear push button
             */
            case DLG_GPICLEAR:
               /*
                * Reset new PMGPI APIs to zero APIs if different
                */
               if ( ulNewGpiGrp )
               {
                  /*
                   * Set new PMGPI APIs to zero
                   */
                  ulNewGpiGrp = 0L;

                  /*
                   * Set PMGPI APIs customization dialog check buttons
                   */
                  SetCustGpiDlgCheck( hwndDlg );
               }

               break;

            /*
             * Process default push button
             */
            case DLG_GPIDEFAULT:
               /*
                * Reset new PMGPI APIs to default APIs if different
                */
               if ( ulNewGpiGrp != DEF_GPIGRPInt )
               {
                  /*
                   * Set new PMGPI APIs to default
                   */
                  ulNewGpiGrp = DEF_GPIGRPInt;

                  /*
                   * Set PMGPI APIs customization dialog check buttons
                   */
                  SetCustGpiDlgCheck( hwndDlg );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_GPIHELP:
               /*
                * Display PMGPI APIs help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTGPI ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_GPIOK:
               /*
                * Issue error if new PMGPI APIs zero
                */
               if ( !ulNewGpiGrp )
               {
                  ErrorMessageBox( MB_ERROR, "No PMGPI API groups selected" );
               }

               /*
                * Save new PMGPI APIs customization option in operating system
                * profile, issue error and terminate if unsuccessful
                */
               else if ( ( ulNewGpiGrp != ulOldGpiGrp ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewGpiGrp ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szGpiGrpKey, szFlags ) ) ) )
               {
                  Error( "Unable to store PMGPI APIs customization option" );
               }

               /*
                * Close PMGPI APIs dialog indicating OK
                */
               else
               {
                  WinDismissDlg( hwndDlg, DID_OK );
               }

               break;

            /*
             * Process undo push button
             */
            case DLG_GPIUNDO:
               /*
                * Reset new PMGPI APIs to original APIs if different
                */
               if ( ulNewGpiGrp != ulOldGpiGrp )
               {
                  /*
                   * Set new PMGPI APIs to original
                   */
                  ulNewGpiGrp = ulOldGpiGrp;

                  /*
                   * Set PMGPI APIs customization dialog check buttons
                   */
                  SetCustGpiDlgCheck( hwndDlg );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying PMGPI APIs dialog
       */
      case WM_CONTROL:
         /*
          * Identify PMGPI APIs dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify check box control processing based on command value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process PMGPI BIT APIs check box
                   */
                  case DLG_GPIBITCHK:
                     /*
                      * Toggle PMGPI BIT APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_BIT ) ) CLRFLAG( ulNewGpiGrp, G_BIT ); else SETFLAG( ulNewGpiGrp, G_BIT );

                     break;

                  /*
                   * Process PMGPI CORR APIs check box
                   */
                  case DLG_GPICORRCHK:
                     /*
                      * Toggle PMGPI CORR APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_CORR ) ) CLRFLAG( ulNewGpiGrp, G_CORR ); else SETFLAG( ulNewGpiGrp, G_CORR );

                     break;

                  /*
                   * Process PMGPI CTRL APIs check box
                   */
                  case DLG_GPICTRLCHK:
                     /*
                      * Toggle PMGPI CTRL APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_CTRL ) ) CLRFLAG( ulNewGpiGrp, G_CTRL ); else SETFLAG( ulNewGpiGrp, G_CTRL );

                     break;

                  /*
                   * Process PMGPI DEF APIs check box
                   */
                  case DLG_GPIDEFCHK:
                     /*
                      * Toggle PMGPI DEF APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_DEF ) ) CLRFLAG( ulNewGpiGrp, G_DEF ); else SETFLAG( ulNewGpiGrp, G_DEF );

                     break;

                  /*
                   * Process PMGPI DEV APIs check box
                   */
                  case DLG_GPIDEVCHK:
                     /*
                      * Toggle PMGPI DEV APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_DEV ) ) CLRFLAG( ulNewGpiGrp, G_DEV ); else SETFLAG( ulNewGpiGrp, G_DEV );

                     break;

                  /*
                   * Process PMGPI EDIT APIs check box
                   */
                  case DLG_GPIEDITCHK:
                     /*
                      * Toggle PMGPI EDIT APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_EDIT ) ) CLRFLAG( ulNewGpiGrp, G_EDIT ); else SETFLAG( ulNewGpiGrp, G_EDIT );

                     break;

                  /*
                   * Process PMGPI INK APIs check box
                   */
                  case DLG_GPIINKCHK:
                     /*
                      * Toggle PMGPI INK APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_INK ) ) CLRFLAG( ulNewGpiGrp, G_INK ); else SETFLAG( ulNewGpiGrp, G_INK );

                     break;

                  /*
                   * Process PMGPI LCID APIs check box
                   */
                  case DLG_GPILCIDCHK:
                     /*
                      * Toggle PMGPI LCID APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_LCID ) ) CLRFLAG( ulNewGpiGrp, G_LCID ); else SETFLAG( ulNewGpiGrp, G_LCID );

                     break;

                  /*
                   * Process PMGPI LCT APIs check box
                   */
                  case DLG_GPILCTCHK:
                     /*
                      * Toggle PMGPI LCT APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_LCT ) ) CLRFLAG( ulNewGpiGrp, G_LCT ); else SETFLAG( ulNewGpiGrp, G_LCT );

                     break;

                  /*
                   * Process PMGPI META APIs check box
                   */
                  case DLG_GPIMETACHK:
                     /*
                      * Toggle PMGPI META APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_META ) ) CLRFLAG( ulNewGpiGrp, G_META ); else SETFLAG( ulNewGpiGrp, G_META );

                     break;

                  /*
                   * Process PMGPI PATH APIs check box
                   */
                  case DLG_GPIPATHCHK:
                     /*
                      * Toggle PMGPI PATH APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_PATH ) ) CLRFLAG( ulNewGpiGrp, G_PATH ); else SETFLAG( ulNewGpiGrp, G_PATH );

                     break;

                  /*
                   * Process PMGPI POLY APIs check box
                   */
                  case DLG_GPIPOLYCHK:
                     /*
                      * Toggle PMGPI POLY APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_POLY ) ) CLRFLAG( ulNewGpiGrp, G_POLY ); else SETFLAG( ulNewGpiGrp, G_POLY );

                     break;

                  /*
                   * Process PMGPI PRIM APIs check box
                   */
                  case DLG_GPIPRIMCHK:
                     /*
                      * Toggle PMGPI PRIM APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_PRIM ) ) CLRFLAG( ulNewGpiGrp, G_PRIM ); else SETFLAG( ulNewGpiGrp, G_PRIM );

                     break;

                  /*
                   * Process PMGPI RGN APIs check box
                   */
                  case DLG_GPIRGNCHK:
                     /*
                      * Toggle PMGPI RGN APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_RGN ) ) CLRFLAG( ulNewGpiGrp, G_RGN ); else SETFLAG( ulNewGpiGrp, G_RGN );

                     break;

                  /*
                   * Process PMGPI SEG APIs check box
                   */
                  case DLG_GPISEGCHK:
                     /*
                      * Toggle PMGPI SEG APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_SEG ) ) CLRFLAG( ulNewGpiGrp, G_SEG ); else SETFLAG( ulNewGpiGrp, G_SEG );

                     break;

                  /*
                   * Process PMGPI TRAN APIs check box
                   */
                  case DLG_GPITRANCHK:
                     /*
                      * Toggle PMGPI TRAN APIs flag
                      */
                     if ( FLAGSET( ulNewGpiGrp, G_TRAN ) ) CLRFLAG( ulNewGpiGrp, G_TRAN ); else SETFLAG( ulNewGpiGrp, G_TRAN );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing PMGPI APIs dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old PMGPI APIs customization option from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szGpiGrpKey, DEF_GPIGRP, szFlags, sizeof( szFlags ) ) != sizeof( DEF_GPIGRP ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldGpiGrp ) != 1 ) )
         {
            Error( "Unable to retrieve PMGPI APIs customization option" );
         }

         /*
          * Set new PMGPI APIs to original
          */
         ulNewGpiGrp = ulOldGpiGrp;

         /*
          * Set PMGPI APIs customization dialog check buttons
          */
         SetCustGpiDlgCheck( hwndDlg );

         /*
          * Give PMGPI APIs customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustGpiDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustIntDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustIntDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   LONG lColor;                                  /* Foreground color index    */
   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify API interception dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying API interception dialog
       */
      case WM_COMMAND:
         /*
          * Identify API interception dialog control processing based on
          * command value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_INTCANCEL:
               /*
                * Verify cancellation if new API interception different from
                * original
                */
               if ( FLAGSET( ulNewTrcFlg, F_INTERCEPT ) != FLAGSET( ulOldTrcFlg, F_INTERCEPT ) )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "API Interception", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close API interception dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_INTDEFAULT:
               /*
                * Reset API interception to default interception if different
                */
               if ( FLAGSET( ulNewTrcFlg, F_INTERCEPT ) != FLAGSET( DEF_TRCFLGInt, F_INTERCEPT ) )
               {
                  /*
                   * Set new API interception to default
                   */
                  if ( FLAGSET( DEF_TRCFLGInt, F_INTERCEPT ) )
                  {
                     SETFLAG( ulNewTrcFlg, F_INTERCEPT );
                  }
                  else
                  {
                     CLRFLAG( ulNewTrcFlg, F_INTERCEPT );
                  }

                  /*
                   * Set default API interception radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ulNewTrcFlg, F_INTERCEPT ) ? DLG_INTONRAD : DLG_INTOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_INTHELP:
               /*
                * Display API interception help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTINT ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_INTOK:
               /*
                * Save new API interception customization option in operating
                * system profile, issue error and terminate if unsuccessful
                */
               if ( ( ulNewTrcFlg != ulOldTrcFlg ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewTrcFlg ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, szFlags ) ) ) )
               {
                  Error( "Unable to store API interception customization option" );
               }

               /*
                * Close API interception dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_INTUNDO:
               /*
                * Reset API interception to original interception if different
                */
               if ( FLAGSET( ulNewTrcFlg, F_INTERCEPT ) != FLAGSET( ulOldTrcFlg, F_INTERCEPT ) )
               {
                  /*
                   * Set new API interception to original
                   */
                  if ( FLAGSET( ulOldTrcFlg, F_INTERCEPT ) )
                  {
                     SETFLAG( ulNewTrcFlg, F_INTERCEPT );
                  }
                  else
                  {
                     CLRFLAG( ulNewTrcFlg, F_INTERCEPT );
                  }

                  /*
                   * Set original API interception radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ulNewTrcFlg, F_INTERCEPT ) ? DLG_INTONRAD : DLG_INTOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying API interception dialog
       */
      case WM_CONTROL:
         /*
          * Identify API interception dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process enable radio button
                   */
                  case DLG_INTONRAD:
                     /*
                      * Set new API interception to on
                      */
                     SETFLAG( ulNewTrcFlg, F_INTERCEPT );

                     break;

                  /*
                   * Process disable radio button
                   */
                  case DLG_INTOFFRAD:
                     /*
                      * Set new API interception to off
                      */
                     CLRFLAG( ulNewTrcFlg, F_INTERCEPT );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing API interception dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old API interception customization option from operating
          * system profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, DEF_TRCFLG, szFlags, sizeof( szFlags ) ) != sizeof( DEF_TRCFLG ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldTrcFlg ) != 1 ) )
         {
            Error( "Unable to retrieve API interception customization option" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_INTONTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_INTOFFTXT ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set original stamping radio button
          */
         WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ( ulNewTrcFlg = ulOldTrcFlg ), F_INTERCEPT ) ? DLG_INTONRAD : DLG_INTOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );

         /*
          * Give API interception customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustIntDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustLvlDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustLvlDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   LONG lColor;                                  /* Foreground color index    */
   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify logging level dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying logging level dialog
       */
      case WM_COMMAND:
         /*
          * Identify logging level dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_LVLCANCEL:
               /*
                * Verify cancellation if new logging level different from
                * original
                */
               if ( ulNewTrcLvl != ulOldTrcLvl )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "Logging Level", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close logging level dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_LVLDEFAULT:
               /*
                * Reset logging level to default level if different
                */
               if ( ulNewTrcLvl != DEF_TRCLVLInt )
               {
                  /*
                   * Set new logging level to default
                   */
                  ulNewTrcLvl = DEF_TRCLVLInt;

                  /*
                   * Set default level radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LVL1RAD - 1 + ulNewTrcLvl ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_LVLHELP:
               /*
                * Display logging level help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTLVL ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_LVLOK:
               /*
                * Save new logging level customization option in operating
                * system profile, issue error and terminate if unsuccessful
                */
               if ( ( ulNewTrcLvl != ulOldTrcLvl ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewTrcLvl ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szTrcLvlKey, szFlags ) ) ) )
               {
                  Error( "Unable to store logging level customization option" );
               }

               /*
                * Close logging level dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_LVLUNDO:
               /*
                * Reset logging level to original level if different
                */
               if ( ulNewTrcLvl != ulOldTrcLvl )
               {
                  /*
                   * Set new logging level to original
                   */
                  ulNewTrcLvl = ulOldTrcLvl;

                  /*
                   * Set original level radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LVL1RAD - 1 + ulNewTrcLvl ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying logging level dialog
       */
      case WM_CONTROL:
         /*
          * Identify logging level dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process level 1 radio button
                   */
                  case DLG_LVL1RAD:
                     /*
                      * Set new logging level to one
                      */
                     ulNewTrcLvl = L_ENTEXT;

                     break;

                  /*
                   * Process level 2 radio button
                   */
                  case DLG_LVL2RAD:
                     /*
                      * Set new logging level to two
                      */
                     ulNewTrcLvl = L_PARMS;

                     break;

                  /*
                   * Process level 3 radio button
                   */
                  case DLG_LVL3RAD:
                     /*
                      * Set new logging level to three
                      */
                     ulNewTrcLvl = L_CONTENTS;

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing logging level dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old logging level customization option from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szTrcLvlKey, DEF_TRCLVL, szFlags, sizeof( szFlags ) ) != sizeof( DEF_TRCLVL ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldTrcLvl ) != 1 ) )
         {
            Error( "Unable to retrieve logging level customization option" );
         }
         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_LVL1TXT ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_LVL2TXT ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_LVL3TXT ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set original level radio button
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_LVL1RAD - 1 + ( ulNewTrcLvl = ulOldTrcLvl ) ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );

         /*
          * Give logging level customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustLvlDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustTimDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustTimDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   LONG lColor;                                  /* Foreground color index    */
   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify time stamping dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying time stamping dialog
       */
      case WM_COMMAND:
         /*
          * Identify time stamping dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_TIMCANCEL:
               /*
                * Verify cancellation if new time stamping different from
                * original
                */
               if ( FLAGSET( ulNewTrcFlg, F_TIMESTAMP ) != FLAGSET( ulOldTrcFlg, F_TIMESTAMP ) )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "Time Stamping", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close time stamping dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_TIMDEFAULT:
               /*
                * Reset time stamping to default stamping if different
                */
               if ( FLAGSET( ulNewTrcFlg, F_TIMESTAMP ) != FLAGSET( DEF_TRCFLGInt, F_TIMESTAMP ) )
               {
                  /*
                   * Set new time stamping to default
                   */
                  if ( FLAGSET( DEF_TRCFLGInt, F_TIMESTAMP ) )
                  {
                     SETFLAG( ulNewTrcFlg, F_TIMESTAMP );
                  }
                  else
                  {
                     CLRFLAG( ulNewTrcFlg, F_TIMESTAMP );
                  }

                  /*
                   * Set default time stamping radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ulNewTrcFlg, F_TIMESTAMP ) ? DLG_TIMONRAD : DLG_TIMOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_TIMHELP:
               /*
                * Display time stamping help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTTIM ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_TIMOK:
               /*
                * Save new time stamping customization option in operating
                * system profile, issue error and terminate if unsuccessful
                */
               if ( ( ulNewTrcFlg != ulOldTrcFlg ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewTrcFlg ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, szFlags ) ) ) )
               {
                  Error( "Unable to store time stamping customization option" );
               }

               /*
                * Close time stamping dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_TIMUNDO:
               /*
                * Reset time stamping to original stamping if different
                */
               if ( FLAGSET( ulNewTrcFlg, F_TIMESTAMP ) != FLAGSET( ulOldTrcFlg, F_TIMESTAMP ) )
               {
                  /*
                   * Set new time stamping to original
                   */
                  if ( FLAGSET( ulOldTrcFlg, F_TIMESTAMP ) )
                  {
                     SETFLAG( ulNewTrcFlg, F_TIMESTAMP );
                  }
                  else
                  {
                     CLRFLAG( ulNewTrcFlg, F_TIMESTAMP );
                  }

                  /*
                   * Set original time stamping radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ulNewTrcFlg, F_TIMESTAMP ) ? DLG_TIMONRAD : DLG_TIMOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying time stamping dialog
       */
      case WM_CONTROL:
         /*
          * Identify time stamping dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process enable radio button
                   */
                  case DLG_TIMONRAD:
                     /*
                      * Set new time stamping to on
                      */
                     SETFLAG( ulNewTrcFlg, F_TIMESTAMP );

                     break;

                  /*
                   * Process disable radio button
                   */
                  case DLG_TIMOFFRAD:
                     /*
                      * Set new time stamping to off
                      */
                     CLRFLAG( ulNewTrcFlg, F_TIMESTAMP );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing time stamping dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old time stamping customization option from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, DEF_TRCFLG, szFlags, sizeof( szFlags ) ) != sizeof( DEF_TRCFLG ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldTrcFlg ) != 1 ) )
         {
            Error( "Unable to retrieve time stamping customization option" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_TIMONTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_TIMOFFTXT ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set original stamping radio button
          */
         WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( ( ulNewTrcFlg = ulOldTrcFlg ), F_TIMESTAMP ) ? DLG_TIMONRAD : DLG_TIMOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );

         /*
          * Give time stamping customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustTimDlgProc() */

/******************************************************************************/
/*                                                                            */
/* CustUsrDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

#ifdef T_USERHOOK
MRESULT EXPENTRY CustUsrDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   int   i;                                      /* Integer                   */
   LONG  lColor;                                 /* Foreground color index    */
   PCHAR pch;                                    /* Character pointer         */

   /*
    * Identify user hook dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying user hook dialog
       */
      case WM_COMMAND:
         /*
          * Identify user hook dialog control processing based on command value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_USRCANCEL:
               /*
                * Obtain new user hook, set to default if entry field window
                * disabled or unable to obtain hook
                */
               if ( ( !WinIsWindowEnabled( WinWindowFromID( hwndDlg, DLG_USRHOOK ) )                               ) ||
                    ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_USRHOOK ), sizeof( szUserHk ), szUserHk ) ) )
               {
                  strcpy( szUserHk, DEF_USERHK );
               }

               /*
                * Verify cancellation if new user hook different from original
                */
               if ( strcmp( szUserHk, szOldUserHk ) )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "User Hook", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close user hook dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_USRDEFAULT:
               /*
                * Obtain new user hook, set to default if entry field window
                * disabled or unable to obtain hook
                */
               if ( ( !WinIsWindowEnabled( WinWindowFromID( hwndDlg, DLG_USRHOOK ) )                               ) ||
                    ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_USRHOOK ), sizeof( szUserHk ), szUserHk ) ) )
               {
                  strcpy( szUserHk, szOldUserHk );
               }

               /*
                * Reset new user hook to default if different
                */
               if ( strcmp( szUserHk, DEF_USERHK ) )
               {
                  /*
                   * Set new user hook to default
                   */
                  strcpy( szUserHk, DEF_USERHK );

                  /*
                   * Disable entry field window
                   */
                  WinEnableWindow( WinWindowFromID( hwndDlg, DLG_USRHOOK ), FALSE );

                  /*
                   * Set default radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_USRDEFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_USRHELP:
               /*
                * Display user hook help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTUSR ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_USROK:
               /*
                * Obtain new user hook, set to default if entry field window
                * disabled or unable to obtain hook
                */
               if ( !WinIsWindowEnabled( WinWindowFromID( hwndDlg, DLG_USRHOOK ) ) )
               {
                  strcpy( szUserHk, DEF_USERHK );
               }

               /*
                * Issue error if new user hook empty or invalid
                */
               else if ( ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_USRHOOK ), sizeof( szUserHk ), szUserHk ) ) ||
                         ( ( i = strlen( szUserHk ) ) < 3                                                               ) ||
                         ( strchr( szUserHk, ':' ) != NULL                                                              ) ||
                         ( strchr( szUserHk, '\\' ) != NULL                                                             ) ||
                         ( ( pch = strchr( szUserHk, '.' ) ) == NULL                                                    ) ||
                         ( strchr( pch + 1, '.' ) != NULL                                                               ) ||
                         ( szUserHk[ 0 ] == '.'                                                                         ) ||
                         ( szUserHk[ i - 1 ] == '.'                                                                     ) )
               {
                  ErrorMessageBox( MB_ERROR, "User hook must be of the format DLLNAME.HOOKNAME" );
                  break;
               }

               /*
                * Upper case library name if string argument valid
                */
               else
               {
                  i = strcspn( szUserHk, "." );
                  szUserHk[ i ] = '\0';
                  Strupr( szUserHk );
                  szUserHk[ i ] = '.';
               }

               /*
                * Save new user hook customization option in operating system
                * profile, issue error and terminate if unsuccessful
                */
               if ( ( strcmp( szUserHk, szOldUserHk )                                              ) &&
                    ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szUserHkKey, szUserHk ) ) )
               {
                  Error( "Unable to store user hook customization option" );
               }

               /*
                * Close user hook dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_USRUNDO:
               /*
                * Obtain new user hook, set to default if entry field window
                * disabled or unable to obtain hook
                */
               if ( ( !WinIsWindowEnabled( WinWindowFromID( hwndDlg, DLG_USRHOOK ) )                               ) ||
                    ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_USRHOOK ), sizeof( szUserHk ), szUserHk ) ) )
               {
                  strcpy( szUserHk, DEF_USERHK );
               }

               /*
                * Reset new user hook to original if different
                */
               if ( strcmp( szUserHk, szOldUserHk ) )
               {
                  /*
                   * Reset new user hook to original
                   */
                  strcpy( szUserHk, szOldUserHk );

                  /*
                   * Set appropriate windows if original hook user hook
                   */
                  if ( szUserHk[ 0 ] )
                  {
                     /*
                      * Set entry field window text
                      */
                     WinSetWindowText( WinWindowFromID( hwndDlg, DLG_USRHOOK ), szOldUserHk );

                     /*
                      * Enable entry field window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_USRHOOK ), TRUE );

                     /*
                      * Set hook radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_USRHOOKRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }

                  /*
                   * Set appropriate windows if original hook default hook
                   */
                  else
                  {
                     /*
                      * Disable entry field window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_USRHOOK ), FALSE );

                     /*
                      * Set default radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_USRDEFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying user hook dialog
       */
      case WM_CONTROL:
         /*
          * Identify user hook dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process default radio button
                   */
                  case DLG_USRDEFRAD:
                     /*
                      * Set new user hook to default
                      */
                     strcpy( szUserHk, DEF_USERHK );

                     /*
                      * Disable entry field window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_USRHOOK ), FALSE );

                     break;

                  /*
                   * Process hook radio button
                   */
                  case DLG_USRHOOKRAD:
                     /*
                      * Obtain new user hook
                      */
                     WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_USRHOOK ), sizeof( szUserHk ), szUserHk );

                     /*
                      * Enable entry field window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_USRHOOK ), TRUE );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing user hook dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old user hook customization option from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szUserHkKey, DEF_USERHK, szOldUserHk, sizeof( szOldUserHk ) ) == 0L )
         {
            Error( "Unable to retrieve user hook customization option" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_USRDEFTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_USRHOOKTXT ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Initialize entry field window
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_USRHOOK ), EM_SETTEXTLIMIT, MPFROMLONG( sizeof( szUserHk ) - 1 ), MPFROMLONG( 0L ) );

         /*
          * Set appropriate windows if no original user hook
          */
         if ( !strcmp( szOldUserHk, DEF_USERHK ) )
         {
            /*
             * Set default radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_USRDEFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Set appropriate windows if original user hook
          */
         else
         {
            /*
             * Set entry field window text
             */
            WinSetWindowText( WinWindowFromID( hwndDlg, DLG_USRHOOK ), szOldUserHk );

            /*
             * Set hook radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_USRHOOKRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Give user hook customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;

      /*
       * Key pressed
       */
      case WM_CHAR:
         /*
          * Simulate OK push button if either enter key pressed in entry field
          */
         if ( ( SHORT1FROMMP( mp1Dlg ) & KC_VIRTUALKEY                                             ) &&
              ( ( SHORT2FROMMP( mp2Dlg ) == VK_ENTER ) || ( SHORT2FROMMP( mp2Dlg ) == VK_NEWLINE ) ) &&
              ( WinQueryFocus( HWND_DESKTOP ) == WinWindowFromID( hwndDlg, DLG_USRHOOK )           ) )
         {
            /*
             * Simulate OK push button
             */
            WinPostMsg( hwndDlg, WM_COMMAND, MPFROM2SHORT( DLG_USROK, 0 ), MPVOID );

            /*
             * Indicate focus window unchanged
             */
            return (MRESULT)FALSE;
         }

         break;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustUsrDlgProc() */
#endif

/******************************************************************************/
/*                                                                            */
/* CustWinDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY CustWinDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify PMWIN APIs dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying PMWIN APIs dialog
       */
      case WM_COMMAND:
         /*
          * Identify PMWIN APIs dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_WINCANCEL:
               /*
                * Verify cancellation if new PMWIN APIs different from original
                */
               if ( ulNewWinGrp != ulOldWinGrp )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "PMWIN APIs", "Customization has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close PMWIN APIs dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process clear push button
             */
            case DLG_WINCLEAR:
               /*
                * Reset new PMWIN APIs to zero APIs if different
                */
               if ( ulNewWinGrp )
               {
                  /*
                   * Set new PMWIN APIs to zero
                   */
                  ulNewWinGrp = 0L;

                  /*
                   * Set PMWIN APIs customization dialog check buttons
                   */
                  SetCustWinDlgCheck( hwndDlg );
               }

               break;

            /*
             * Process default push button
             */
            case DLG_WINDEFAULT:
               /*
                * Reset new PMWIN APIs to default APIs if different
                */
               if ( ulNewWinGrp != DEF_WINGRPInt )
               {
                  /*
                   * Set new PMWIN APIs to default
                   */
                  ulNewWinGrp = DEF_WINGRPInt;

                  /*
                   * Set PMWIN APIs customization dialog check buttons
                   */
                  SetCustWinDlgCheck( hwndDlg );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_WINHELP:
               /*
                * Display PMWIN APIs help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_CUSTWIN ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_WINOK:
               /*
                * Issue error if new PMWIN APIs zero
                */
               if ( !ulNewWinGrp )
               {
                  ErrorMessageBox( MB_ERROR, "No PMWIN API groups selected" );
               }

               /*
                * Save new PMWIN APIs customization option in operating system
                * profile, issue error and terminate if unsuccessful
                */
               else if ( ( ulNewWinGrp != ulOldWinGrp ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewWinGrp ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szWinGrpKey, szFlags ) ) ) )
               {
                  Error( "Unable to store PMWIN APIs customization option" );
               }

               /*
                * Close PMWIN APIs dialog indicating OK
                */
               else
               {
                  WinDismissDlg( hwndDlg, DID_OK );
               }

               break;

            /*
             * Process undo push button
             */
            case DLG_WINUNDO:
               /*
                * Reset new PMWIN APIs to original APIs if different
                */
               if ( ulNewWinGrp != ulOldWinGrp )
               {
                  /*
                   * Set new PMWIN APIs to original
                   */
                  ulNewWinGrp = ulOldWinGrp;

                  /*
                   * Set PMWIN APIs customization dialog check buttons
                   */
                  SetCustWinDlgCheck( hwndDlg );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying PMWIN APIs dialog
       */
      case WM_CONTROL:
         /*
          * Identify PMWIN APIs dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify check box control processing based on command value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process PMWIN ACCL APIs check box
                   */
                  case DLG_WINACCLCHK:
                     /*
                      * Toggle PMWIN ACCL APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_ACCL ) ) CLRFLAG( ulNewWinGrp, W_ACCL ); else SETFLAG( ulNewWinGrp, W_ACCL );

                     break;

                  /*
                   * Process PMWIN ATOM APIs check box
                   */
                  case DLG_WINATOMCHK:
                     /*
                      * Toggle PMWIN ATOM APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_ATOM ) ) CLRFLAG( ulNewWinGrp, W_ATOM ); else SETFLAG( ulNewWinGrp, W_ATOM );

                     break;

                  /*
                   * Process PMWIN CLIP APIs check box
                   */
                  case DLG_WINCLIPCHK:
                     /*
                      * Toggle PMWIN CLIP APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_CLIP ) ) CLRFLAG( ulNewWinGrp, W_CLIP ); else SETFLAG( ulNewWinGrp, W_CLIP );

                     break;

                  /*
                   * Process PMWIN CTRY APIs check box
                   */
                  case DLG_WINCTRYCHK:
                     /*
                      * Toggle PMWIN CTRY APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_CTRY ) ) CLRFLAG( ulNewWinGrp, W_CTRY ); else SETFLAG( ulNewWinGrp, W_CTRY );

                     break;

                  /*
                   * Process PMWIN CUR APIs check box
                   */
                  case DLG_WINCURCHK:
                     /*
                      * Toggle PMWIN CUR APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_CUR ) ) CLRFLAG( ulNewWinGrp, W_CUR ); else SETFLAG( ulNewWinGrp, W_CUR );

                     break;

                  /*
                   * Process PMWIN DDE APIs check box
                   */
                  case DLG_WINDDECHK:
                     /*
                      * Toggle PMWIN DDE APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_DDE ) ) CLRFLAG( ulNewWinGrp, W_DDE ); else SETFLAG( ulNewWinGrp, W_DDE );

                     break;

                  /*
                   * Process PMWIN DESK APIs check box
                   */
                  case DLG_WINDESKCHK:
                     /*
                      * Toggle PMWIN DESK APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_DESK ) ) CLRFLAG( ulNewWinGrp, W_DESK ); else SETFLAG( ulNewWinGrp, W_DESK );

                     break;

                  /*
                   * Process PMWIN DLG APIs check box
                   */
                  case DLG_WINDLGCHK:
                     /*
                      * Toggle PMWIN DLG APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_DLG ) ) CLRFLAG( ulNewWinGrp, W_DLG ); else SETFLAG( ulNewWinGrp, W_DLG );

                     break;

                  /*
                   * Process PMWIN DWIN APIs check box
                   */
                  case DLG_WINDWINCHK:
                     /*
                      * Toggle PMWIN DWIN APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_DWIN ) ) CLRFLAG( ulNewWinGrp, W_DWIN ); else SETFLAG( ulNewWinGrp, W_DWIN );

                     break;

                  /*
                   * Process PMWIN ENV APIs check box
                   */
                  case DLG_WINENVCHK:
                     /*
                      * Toggle PMWIN ENV APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_ENV ) ) CLRFLAG( ulNewWinGrp, W_ENV ); else SETFLAG( ulNewWinGrp, W_ENV );

                     break;

                  /*
                   * Process PMWIN ERR APIs check box
                   */
                  case DLG_WINERRCHK:
                     /*
                      * Toggle PMWIN ERR APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_ERR ) ) CLRFLAG( ulNewWinGrp, W_ERR ); else SETFLAG( ulNewWinGrp, W_ERR );

                     break;

                  /*
                   * Process PMWIN FRAM APIs check box
                   */
                  case DLG_WINFRAMCHK:
                     /*
                      * Toggle PMWIN FRAM APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_FRAM ) ) CLRFLAG( ulNewWinGrp, W_FRAM ); else SETFLAG( ulNewWinGrp, W_FRAM );

                     break;

                  /*
                   * Process PMWIN HEAP APIs check box
                   */
                  case DLG_WINHEAPCHK:
                     /*
                      * Toggle PMWIN HEAP APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_HEAP ) ) CLRFLAG( ulNewWinGrp, W_HEAP ); else SETFLAG( ulNewWinGrp, W_HEAP );

                     break;

                  /*
                   * Process PMWIN HOOK APIs check box
                   */
                  case DLG_WINHOOKCHK:
                     /*
                      * Toggle PMWIN HOOK APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_HOOK ) ) CLRFLAG( ulNewWinGrp, W_HOOK ); else SETFLAG( ulNewWinGrp, W_HOOK );

                     break;

                  /*
                   * Process PMWIN INPT APIs check box
                   */
                  case DLG_WININPTCHK:
                     /*
                      * Toggle PMWIN INPT APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_INPT ) ) CLRFLAG( ulNewWinGrp, W_INPT ); else SETFLAG( ulNewWinGrp, W_INPT );

                     break;

                  /*
                   * Process PMWIN LOAD APIs check box
                   */
                  case DLG_WINLOADCHK:
                     /*
                      * Toggle PMWIN LOAD APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_LOAD ) ) CLRFLAG( ulNewWinGrp, W_LOAD ); else SETFLAG( ulNewWinGrp, W_LOAD );

                     break;

                  /*
                   * Process PMWIN MENU APIs check box
                   */
                  case DLG_WINMENUCHK:
                     /*
                      * Toggle PMWIN MENU APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_MENU ) ) CLRFLAG( ulNewWinGrp, W_MENU ); else SETFLAG( ulNewWinGrp, W_MENU );

                     break;

                  /*
                   * Process PMWIN MSG APIs check box
                   */
                  case DLG_WINMSGCHK:
                     /*
                      * Toggle PMWIN MSG APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_MSG ) ) CLRFLAG( ulNewWinGrp, W_MSG ); else SETFLAG( ulNewWinGrp, W_MSG );

                     break;

                  /*
                   * Process PMWIN MSGL APIs check box
                   */
                  case DLG_WINMSGLCHK:
                     /*
                      * Toggle PMWIN MSGL APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_MSGL ) ) CLRFLAG( ulNewWinGrp, W_MSGL ); else SETFLAG( ulNewWinGrp, W_MSGL );

                     break;

                  /*
                   * Process PMWIN PAL APIs check box
                   */
                  case DLG_WINPALCHK:
                     /*
                      * Toggle PMWIN PAL APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_PAL ) ) CLRFLAG( ulNewWinGrp, W_PAL ); else SETFLAG( ulNewWinGrp, W_PAL );

                     break;

                  /*
                   * Process PMWIN PTR APIs check box
                   */
                  case DLG_WINPTRCHK:
                     /*
                      * Toggle PMWIN PTR APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_PTR ) ) CLRFLAG( ulNewWinGrp, W_PTR ); else SETFLAG( ulNewWinGrp, W_PTR );

                     break;

                  /*
                   * Process PMWIN RECT APIs check box
                   */
                  case DLG_WINRECTCHK:
                     /*
                      * Toggle PMWIN RECT APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_RECT ) ) CLRFLAG( ulNewWinGrp, W_RECT ); else SETFLAG( ulNewWinGrp, W_RECT );

                     break;

                  /*
                   * Process PMWIN SEI APIs check box
                   */
                  case DLG_WINSEICHK:
                     /*
                      * Toggle PMWIN SEI APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_SEI ) ) CLRFLAG( ulNewWinGrp, W_SEI ); else SETFLAG( ulNewWinGrp, W_SEI );

                     break;

                  /*
                   * Process PMWIN SYS APIs check box
                   */
                  case DLG_WINSYSCHK:
                     /*
                      * Toggle PMWIN SYS APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_SYS ) ) CLRFLAG( ulNewWinGrp, W_SYS ); else SETFLAG( ulNewWinGrp, W_SYS );

                     break;

                  /*
                   * Process PMWIN THK APIs check box
                   */
                  case DLG_WINTHKCHK:
                     /*
                      * Toggle PMWIN THK APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_THK ) ) CLRFLAG( ulNewWinGrp, W_THK ); else SETFLAG( ulNewWinGrp, W_THK );

                     break;

                  /*
                   * Process PMWIN TIME APIs check box
                   */
                  case DLG_WINTIMECHK:
                     /*
                      * Toggle PMWIN TIME APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_TIME ) ) CLRFLAG( ulNewWinGrp, W_TIME ); else SETFLAG( ulNewWinGrp, W_TIME );

                     break;

                  /*
                   * Process PMWIN TREC APIs check box
                   */
                  case DLG_WINTRECCHK:
                     /*
                      * Toggle PMWIN TREC APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_TREC ) ) CLRFLAG( ulNewWinGrp, W_TREC ); else SETFLAG( ulNewWinGrp, W_TREC );

                     break;

                  /*
                   * Process PMWIN WIN APIs check box
                   */
                  case DLG_WINWINCHK:
                     /*
                      * Toggle PMWIN WIN APIs flag
                      */
                     if ( FLAGSET( ulNewWinGrp, W_WIN ) ) CLRFLAG( ulNewWinGrp, W_WIN ); else SETFLAG( ulNewWinGrp, W_WIN );

                     break;
               }
               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing PMWIN APIs dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old PMWIN APIs customization option from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szWinGrpKey, DEF_WINGRP, szFlags, sizeof( szFlags ) ) != sizeof( DEF_WINGRP ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldWinGrp ) != 1 ) )
         {
            Error( "Unable to retrieve PMWIN APIs customization option" );
         }

         /*
          * Set new PMWIN APIs to original
          */
         ulNewWinGrp = ulOldWinGrp;

         /*
          * Set PMWIN APIs customization dialog check buttons
          */
         SetCustWinDlgCheck( hwndDlg );

         /*
          * Give PMWIN APIs customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* CustWinDlgProc() */

/******************************************************************************/
/*                                                                            */
/* DebugOut()                                                                 */
/*                                                                            */
/******************************************************************************/

#ifdef DEBUG
VOID DebugOut( PCHAR szFmt                       /* Debug message format      */
             , ...                               /* Debug message inserts     */
             )
{

   va_list  pArgs;                               /* Debug message inserts     */
   FILE    *pFile;                               /* Debug file                */
   CHAR     szFmtMsg[ LEN_IOLINE ];              /* Formatted debug message   */

   /*
    * Format error message including message inserts
    */
   va_start( pArgs, szFmt );
   vsprintf( szFmtMsg, szFmt, pArgs );
   va_end( pArgs );

   /*
    * Open debug message file, log debug message if successful
    */
   if ( ( pFile = fopen( DEF_DEBUGFILE, "a" ) ) != NULL )
   {
      /*
       * Log debug message to debug message file
       */
      fprintf( pFile, "%s\n", szFmtMsg );

      /*
       * Close error message file
       */
      fclose( pFile );
   }

}  /* DebugOut() */
#endif

/******************************************************************************/
/*                                                                            */
/* DisplayBanner()                                                            */
/*                                                                            */
/******************************************************************************/

VOID DisplayBanner( FILE *fpOut )                /* Output file               */
{

   /*
    * Display program banner
    */
   fprintf( fpOut, "Operating System/2 API Trace\n" );
#ifdef PRIVATE
   fprintf( fpOut, "PRIVATE Version %1d.%02d.%02d (%s)\n", VER_MAJOR, VER_MINOR, VER_REV, VER_DATE );
#else
   fprintf( fpOut, "Version %1d.%02d.%02d (%s)\n", VER_MAJOR, VER_MINOR, VER_REV, VER_DATE );
#endif
   fprintf( fpOut, "Written by Dave Blaschke\n" );

}  /* DisplayBanner() */

/******************************************************************************/
/*                                                                            */
/* EnableAPITracing()                                                         */
/*                                                                            */
/******************************************************************************/

VOID EnableAPITracing( PENABINFO pEnabInfo )     /* Enablement information    */
{

   int     i;                                    /* Loop variable             */
   PCHAR   pch;                                  /* Character pointer         */
   ULONG   ulDLLIdx;                             /* DLL information index     */

   /*
    * Save any API enablement changes in NE executable file
    */
   if ( FLAGSET( pEnabInfo->ulFlags, fNE_FMT ) )
   {
      /*
       * Examine each DLL name in module index table
       */
      for ( i = 0; i < pEnabInfo->ulModIdxTbl; i++ )
      {
         /*
          * Process supported DLL with changed enablement state
          */
         if ( ( pEnabInfo->pModIdxTbl[ i ] != UNKNOWN_DLL ) && ( FLAGSET( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fCHANGED ) ) )
         {
            /*
             * Obtain module name
             */
            pch = (PCHAR)( (ULONG)pEnabInfo->pImpNamTbl + pEnabInfo->pModRefTbl[ i ] );

            /*
             * Replace DLL name in import name table with corresponding trace
             * DLL name if trace DLL
             */
            if ( FLAGSET( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fTRACEDLL ) )
            {
               memcpy( pch + 1, DLLInfo[ pEnabInfo->pModIdxTbl[ i ] ].szTraceDLLName, *pch );
            }

            /*
             * Replace trace DLL name in import name table with corresponding
             * DLL name if DLL
             */
            else if ( FLAGSET( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fDLL ) )
            {
               memcpy( pch + 1, DLLInfo[ pEnabInfo->pModIdxTbl[ i ] ].szName, *pch );
            }

            /*
             * Indicate import name table changed
             */
            SETFLAG( pEnabInfo->ulFlags, fCHANGED );
         }
      }

      /*
       * Write import name table to executable file if table changed
       */
      if ( FLAGSET( pEnabInfo->ulFlags, fCHANGED ) )
      {
         /*
          * Locate beginning of import name table and write import name table;
          * issue error and terminate if unsuccessful
          */
         if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->pNEHdr->ne_imptab, SEEK_SET ) ) ||
              ( fwrite( pEnabInfo->pImpNamTbl, pEnabInfo->ulImpNamTbl, 1, pEnabInfo->fpInFile ) != 1              ) )
         {
            Error( "Unable to write import name table: %s", pEnabInfo->szInFile );
         }
      }

      /*
       * Disable -P option if executable file does not need to be changed
       */
      else if ( FLAGSET( pEnabInfo->ulFlags, fPOPTION ) )
      {
         CLRFLAG( pEnabInfo->ulFlags, fPOPTION );
      }
   }

   /*
    * Save any API enablement changes in LX executable file
    */
   else if ( FLAGSET( pEnabInfo->ulFlags, fLX_FMT ) )
   {
      /*
       * Examine each DLL name in module index table
       */
      for ( i = 0, pch = pEnabInfo->pModNamTbl; i < pEnabInfo->ulModIdxTbl; i++, pch += *pch + 1 )
      {
         /*
          * Process supported DLL with changed enablement state
          */
         if ( ( pEnabInfo->pModIdxTbl[ i ] != UNKNOWN_DLL ) && ( FLAGSET( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fCHANGED ) ) )
         {
            /*
             * Replace DLL name in module name table with corresponding trace
             * DLL name if trace DLL
             */
            if ( FLAGSET( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fTRACEDLL ) )
            {
               memcpy( pch + 1, DLLInfo[ pEnabInfo->pModIdxTbl[ i ] ].szTraceDLLName, *pch );
            }

            /*
             * Replace trace DLL name in module name table with corresponding
             * DLL name if DLL
             */
            else if ( FLAGSET( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fDLL ) )
            {
               memcpy( pch + 1, DLLInfo[ pEnabInfo->pModIdxTbl[ i ] ].szName, *pch );
            }

            /*
             * Indicate module name table changed
             */
            SETFLAG( pEnabInfo->ulFlags, fCHANGED );
         }
      }

      /*
       * Write module name table to executable file if table changed
       */
      if ( FLAGSET( pEnabInfo->ulFlags, fCHANGED ) )
      {
         /*
          * Locate beginning of module name table and write module name table;
          * issue error and terminate if unsuccessful
          */
         if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->sLXHdr.e32_impmod, SEEK_SET ) ) ||
              ( fwrite( pEnabInfo->pModNamTbl, pEnabInfo->ulModNamTbl, 1, pEnabInfo->fpInFile ) != 1              ) )
         {
            Error( "Unable to write module name table: %s", pEnabInfo->szInFile );
         }
      }

      /*
       * Disable -P option if executable file does not need to be changed
       */
      else if ( FLAGSET( pEnabInfo->ulFlags, fPOPTION ) )
      {
         CLRFLAG( pEnabInfo->ulFlags, fPOPTION );
      }
   }

}  /* EnableAPITracing() */

/******************************************************************************/
/*                                                                            */
/* EnablementDlgProc()                                                        */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY EnablementDlgProc( HWND   hwndDlg /* Dialog handle           */
                                  , ULONG  ulMsg   /* Message number          */
                                  , MPARAM mp1Dlg  /* Message parameter 1     */
                                  , MPARAM mp2Dlg  /* Message parameter 2     */
                                  )
{

   BOOL      bChanged;                           /* Changed table indicator   */
   int       i;                                  /* Loop variable             */
   LONG      lColor;                             /* Foreground color index    */
   PCHAR     pch;                                /* Character pointer         */
   PENABINFO pEnabInfo;                          /* Enablement information    */
   SWP       swpDlg;                             /* Dialog window position    */
   CHAR      szText[ CCHMAXPATH ];               /* Dialog text               */
   ULONG     ulDLLIdx;                           /* DLL information index     */

   /*
    * Identify enablement dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Closing enablement dialog
       */
      case WM_CLOSE:
         /*
          * Translate as cancel push button
          */
         WinPostMsg( hwndDlg, WM_COMMAND, MPFROMSHORT( DLG_ENABCANCEL ), MPVOID );

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying enablement dialog
       */
      case WM_COMMAND:
         /*
          * Obtain enablement information, issue error and terminate if
          * unsuccessful
          */
         if ( !( pEnabInfo = (PENABINFO)WinQueryWindowPtr( hwndDlg, 0 ) ) )
         {
            Error( "Unable to obtain enablement information" );
         }

         /*
          * Identify enablement dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process all off push button
             */
            case DLG_ENABALLOFF:
               /*
                * Examine each supported DLL
                */
               for ( i = 0; i < SUPPORTED_DLL; i++ )
               {
                  /*
                   * Set trace off radio button if trace DLL
                   */
                  if ( FLAGSET( pEnabInfo->DLLFlags[ i ], fTRACEDLL ) )
                  {
                     WinSendMsg( WinWindowFromID( hwndDlg, MIN_ENABOFFRAD + ( i * INC_ENABRAD ) ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;

            /*
             * Process all on push button
             */
            case DLG_ENABALLON:
               /*
                * Examine each supported DLL
                */
               for ( i = 0; i < SUPPORTED_DLL; i++ )
               {
                  /*
                   * Set trace on radio button if DLL
                   */
                  if ( FLAGSET( pEnabInfo->DLLFlags[ i ], fDLL ) )
                  {
                     WinSendMsg( WinWindowFromID( hwndDlg, MIN_ENABONRAD + ( i * INC_ENABRAD ) ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;

            /*
             * Process cancel push button
             */
            case DLG_ENABCANCEL:
               /*
                * Examine each supported DLL
                */
               for ( i = 0, bChanged = FALSE; i < SUPPORTED_DLL; i++ )
               {
                  /*
                   * Indicate table of strings changed if at least one DLL's
                   * enablement state changed
                   */
                  if ( FLAGSET( pEnabInfo->DLLFlags[ i ], fCHANGED ) )
                  {
                     bChanged = TRUE;
                     break;
                  }
               }

               /*
                * Verify cancellation if table of strings changed
                */
               if ( bChanged )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( ( ( ( pch = strrchr( pEnabInfo->szInFile, '\\' ) ) != NULL ) || ( ( pch = strrchr( pEnabInfo->szInFile, ':' ) ) != NULL ) ) ? pch + 1 : pEnabInfo->szInFile,
                                      "Trace enablement has changed for this executable file, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close enablement dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               /*
                * Terminate processing enablement dialog messages
                */
               WinPostMsg( hwndDlg, WM_QUIT, MPVOID, MPVOID );

               break;

            /*
             * Process help push button
             */
            case DLG_ENABHELP:
               /*
                * Display enablement help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_ENAB ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_ENABOK:
               /*
                * Enable or disable API tracing in executable file
                */
               EnableAPITracing( pEnabInfo );

               /*
                * Indicate launch executable if menu item checked
                */
               if ( WinIsMenuItemChecked( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_OPTSEXE ) )
               {
                  SETFLAG( pEnabInfo->ulFlags, fLAUNCH );
               }

               /*
                * Close enablement dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               /*
                * Terminate processing enablement dialog messages
                */
               WinPostMsg( hwndDlg, WM_QUIT, MPVOID, MPVOID );

               break;

            /*
             * Process undo push button
             */
            case DLG_ENABUNDO:
               /*
                * Examine each supported DLL
                */
               for ( i = 0; i < SUPPORTED_DLL; i++ )
               {
                  /*
                   * Reset trace enablement to original enablement if
                   * enablement state changed
                   */
                  if ( FLAGSET( pEnabInfo->DLLFlags[ i ], fCHANGED ) )
                  {
                     WinSendMsg( WinWindowFromID( hwndDlg, FLAGSET( pEnabInfo->DLLFlags[ i ], fTRACEDLL ) ? MIN_ENABOFFRAD + ( i * INC_ENABRAD ) : MIN_ENABONRAD + ( i * INC_ENABRAD ) ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying enablement dialog
       */
      case WM_CONTROL:
         /*
          * Obtain enablement information, issue error and terminate if
          * unsuccessful
          */
         if ( !( pEnabInfo = (PENABINFO)WinQueryWindowPtr( hwndDlg, 0 ) ) )
         {
            Error( "Unable to obtain enablement information" );
         }

         /*
          * Identify enablement dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process trace on radio button
                   */
                  case DLG_ENABDOSONRAD:
                  case DLG_ENABHLPONRAD:
                  case DLG_ENABKBDONRAD:
                  case DLG_ENABMONONRAD:
                  case DLG_ENABMOUONRAD:
                  case DLG_ENABMSGONRAD:
                  case DLG_ENABPIPONRAD:
                  case DLG_ENABNLSONRAD:
                  case DLG_ENABPBDONRAD:
                  case DLG_ENABCTLONRAD:
                  case DLG_ENABDRGONRAD:
                  case DLG_ENABGPIONRAD:
                  case DLG_ENABMRGONRAD:
                  case DLG_ENABPICONRAD:
                  case DLG_ENABSHLONRAD:
                  case DLG_ENABSPLONRAD:
                  case DLG_ENABPVPONRAD:
                  case DLG_ENABWINONRAD:
                  case DLG_ENABWPONRAD :
                  case DLG_ENABQUEONRAD:
                  case DLG_ENABSESONRAD:
                  case DLG_ENABVIOONRAD:
                     /*
                      * Determine DLL information index
                      */
                     ulDLLIdx = ( SHORT1FROMMP( mp1Dlg ) - MIN_ENABONRAD ) / INC_ENABRAD;

                     /*
                      * Enable API tracing if DLL
                      */
                     if ( FLAGSET( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL ) )
                     {
                        /*
                         * Enable API tracing
                         */
                        CLRFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL );
                        SETFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fTRACEDLL );

                        /*
                         * Indicate enablement state unchanged if change
                         * indicated
                         */
                        if ( FLAGSET( pEnabInfo->DLLFlags[ ulDLLIdx ], fCHANGED ) )
                        {
                           CLRFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fCHANGED );
                        }

                        /*
                         * Indicate enablement state changed if change not
                         * indicated
                         */
                        else
                        {
                           SETFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fCHANGED );
                        }
                     }

                     break;

                  /*
                   * Process trace off radio button
                   */
                  case DLG_ENABDOSOFFRAD:
                  case DLG_ENABHLPOFFRAD:
                  case DLG_ENABKBDOFFRAD:
                  case DLG_ENABMONOFFRAD:
                  case DLG_ENABMOUOFFRAD:
                  case DLG_ENABMSGOFFRAD:
                  case DLG_ENABPIPOFFRAD:
                  case DLG_ENABNLSOFFRAD:
                  case DLG_ENABPBDOFFRAD:
                  case DLG_ENABCTLOFFRAD:
                  case DLG_ENABDRGOFFRAD:
                  case DLG_ENABGPIOFFRAD:
                  case DLG_ENABMRGOFFRAD:
                  case DLG_ENABPICOFFRAD:
                  case DLG_ENABSHLOFFRAD:
                  case DLG_ENABSPLOFFRAD:
                  case DLG_ENABPVPOFFRAD:
                  case DLG_ENABWINOFFRAD:
                  case DLG_ENABWPOFFRAD :
                  case DLG_ENABQUEOFFRAD:
                  case DLG_ENABSESOFFRAD:
                  case DLG_ENABVIOOFFRAD:
                     /*
                      * Determine DLL information index
                      */
                     ulDLLIdx = ( SHORT1FROMMP( mp1Dlg ) - MIN_ENABOFFRAD ) / INC_ENABRAD;

                     /*
                      * Disable API tracing if trace DLL
                      */
                     if ( FLAGSET( pEnabInfo->DLLFlags[ ulDLLIdx ], fTRACEDLL ) )
                     {
                        /*
                         * Disable API tracing
                         */
                        CLRFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fTRACEDLL );
                        SETFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fDLL );

                        /*
                         * Indicate enablement state unchanged if change
                         * indicated
                         */
                        if ( FLAGSET( pEnabInfo->DLLFlags[ ulDLLIdx ], fCHANGED ) )
                        {
                           CLRFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fCHANGED );
                        }

                        /*
                         * Indicate enablement state changed if change not
                         * indicated
                         */
                        else
                        {
                           SETFLAG( pEnabInfo->DLLFlags[ ulDLLIdx ], fCHANGED );
                        }
                     }

                     break;
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing enablement dialog
       */
      case WM_INITDLG:
         /*
          * Obtain enablement dialog window position
          */
         WinQueryWindowPos( hwndDlg, &swpDlg );

         /*
          * Move enablement dialog to center of desktop
          */
         WinSetWindowPos( hwndDlg, 0L, ( swpDesktop.cx - swpDlg.cx ) / 2, ( swpDesktop.cy - swpDlg.cy ) / 2, 0L, 0L, SWP_MOVE );

         /*
          * Obtain enablement information, issue error and terminate if
          * unsuccessful
          */
         if ( !( pEnabInfo = (PENABINFO)mp2Dlg ) )
         {
            Error( "Unable to obtain enablement information" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABONTXT1  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABOFFTXT1 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABONTXT2  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABOFFTXT2 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABDOSTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABHLPTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABKBDTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABMONTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABMOUTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABMSGTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABPIPTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABNLSTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABPBDTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABCTLTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABDRGTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABGPITXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABMRGTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABPICTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABSHLTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABSPLTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABPVPTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABWINTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABWPTXT   ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABQUETXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABSESTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_ENABVIOTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set enablement dialog window pointer, issue error and terminate if
          * unsuccessful
          */
         if ( !WinSetWindowPtr( hwndDlg, 0L, pEnabInfo ) )
         {
            Error( "Unable to set enablement dialog window pointer" );
         }

         /*
          * Format enablement dialog title
          */
         sprintf( szText, "%s Enablement", ( ( ( pch = strrchr( pEnabInfo->szInFile, '\\' ) ) != NULL ) || ( ( pch = strrchr( pEnabInfo->szInFile, ':' ) ) != NULL ) ) ? pch + 1 : pEnabInfo->szInFile );

         /*
          * Set enablement dialog title
          */
         WinSetWindowText( hwndDlg, szText );

         /*
          * Format executable file type text
          */
         sprintf( szText, "%s Executable File", FLAGSET( pEnabInfo->ulFlags, fLX_FMT ) ? "LX" : ( FLAGSET( pEnabInfo->ulFlags, fNE_FMT ) ? "NE" : "Unknown" ) );

         /*
          * Set executable file type text
          */
         WinSetWindowText( WinWindowFromID( hwndDlg, DLG_ENABFILETXT ), szText );

         /*
          * Examine each supported DLL
          */
         for ( i = 0; i < SUPPORTED_DLL; i++ )
         {
            /*
             * Set trace on radio button if trace DLL
             */
            if ( FLAGSET( pEnabInfo->DLLFlags[ i ], fTRACEDLL ) )
            {
               WinSendMsg( WinWindowFromID( hwndDlg, MIN_ENABONRAD + ( i * INC_ENABRAD ) ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
            }

            /*
             * Set trace off radio button if DLL
             */
            else if ( FLAGSET( pEnabInfo->DLLFlags[ i ], fDLL ) )
            {
               WinSendMsg( WinWindowFromID( hwndDlg, MIN_ENABOFFRAD + ( i * INC_ENABRAD ) ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
            }

            /*
             * Disable trace on and off radio buttons and text if DLL not in
             * module name table
             */
            else
            {
               WinEnableControl( hwndDlg, MIN_ENABONRAD  + ( i * INC_ENABRAD ), FALSE );
               WinEnableControl( hwndDlg, MIN_ENABOFFRAD + ( i * INC_ENABRAD ), FALSE );
               WinEnableControl( hwndDlg, MIN_ENABTXT    + ( i * INC_ENABRAD ), FALSE );
            }
         }

         /*
          * Give enablement dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* EnablementDlgProc() */

/******************************************************************************/
/*                                                                            */
/* EnablementExceptionHandler()                                               */
/*                                                                            */
/******************************************************************************/

ULONG APIENTRY EnablementExceptionHandler
                                  ( PREPREC pRep /* Exception information     */
                                  , PREGREC pReg /* Registration record       */
                                  , PCTXREC pCtx /* Exception context         */
                                  , PVOID   p    /* Not used                  */
                                  )
{

   /*
    * Clean up if being removed from exception handler chain
    */
   if ( FLAGSET( pRep->fHandlerFlags, EH_UNWINDING ) )
   {
      /*
       * Close executable file and terminate tables if enablement information
       * available
       */
      if ( ((PENABREGREC)pReg)->pEnabInfo )
      {
         TerminateEnablement( FALSE, ((PENABREGREC)pReg)->pEnabInfo );
      }

      /*
       * Destroy enablement dialog message queue if queue exists
       */
      if ( ((PENABREGREC)pReg)->hmqEnab )
      {
         WinDestroyMsgQueue( ((PENABREGREC)pReg)->hmqEnab );
      }

      /*
       * Terminate Presentation Manager facilities if facilities initialized
       */
      if ( ((PENABREGREC)pReg)->habEnab )
      {
         WinTerminate( ((PENABREGREC)pReg)->habEnab );
      }

      /*
       * Release access to system semaphore
       */
      while ( !DosReleaseMutexSem( hmtxSys ) );
   }

   /*
    * Indicate ignore process terminate exceptions
    */
   else if ( ( pRep->ExceptionNum == XCPT_PROCESS_TERMINATE ) || ( pRep->ExceptionNum == XCPT_ASYNC_PROCESS_TERMINATE ) )
   {
      return XCPT_CONTINUE_SEARCH;
   }

   /*
    * Issue error and terminate if fatal exception generated
    */
   else if ( ( pRep->ExceptionNum & XCPT_SEVERITY_CODE ) == XCPT_FATAL_EXCEPTION )
   {
#ifdef DEBUG
      Error( "Enablement thread fatal exception at 0x%08lX: 0x%08lX", pRep->ExceptionAddress, pRep->ExceptionNum );
#else
      Error( "Enablement thread fatal exception: 0x%08lX", pRep->ExceptionNum );
#endif
   }

   /*
    * Indicate ignore exception
    */
   return XCPT_CONTINUE_SEARCH;

}  /* EnablementExceptionHandler() */

/******************************************************************************/
/*                                                                            */
/* EnablementThread()                                                         */
/*                                                                            */
/******************************************************************************/

VOID APIENTRY EnablementThread( ULONG ulParam )  /* Thread parameter          */
{

   HELPINIT   hmiEnab;                           /* Enab dialog help mgr init */
   HELPINIT   hmiExe;                            /* Exe dialog help mgr init  */
   HWND       hwndEnab;                          /* Enablement dialog         */
   HWND       hwndExe;                           /* Launch exe dialog         */
   HWND       hwndEnabHelp;                      /* Enablement dialog help    */
   HWND       hwndExeHelp;                       /* Launch exe dialog help    */
   PENABINFO  pEnabInfo;                         /* Enablement information    */
   QMSG       qmsgEnab;                          /* Enablement dialog message */
   QMSG       qmsgExe;                           /* Launch exe dialog message */
   ENABREGREC sEnabRegRec;                       /* Enab registration record  */
   FILEDLG    sFileDlg;                          /* File dialog structure     */
   ULONG      ulAppType;                         /* Application type          */
   APIRET     ulRC;                              /* Return code               */

   /*
    * Register exception handler
    */
   sEnabRegRec.sRegRec.ExceptionHandler = EnablementExceptionHandler;
   sEnabRegRec.pEnabInfo = NULL;
   DosSetExceptionHandler( (PREGREC)&sEnabRegRec );

   /*
    * Initialize Presentation Manager facilities, continue enablement dialog
    * creation if successful
    */
   if ( ( sEnabRegRec.habEnab = WinInitialize( 0L ) ) != 0L )
   {
      /*
       * Create enablement dialog message queue, continue enablement dialog
       * creation if successful
       */
      if ( ( sEnabRegRec.hmqEnab = WinCreateMsgQueue( sEnabRegRec.habEnab, 0L ) ) != 0L )
      {
         /*
          * Obtain ownership of system semaphore, ignoring asynchronous
          * interrupts
          */
         ulRC = ERROR_INTERRUPT;
         while ( ulRC == ERROR_INTERRUPT )
         {
            ulRC = DosRequestMutexSem( hmtxSys, SEM_INDEFINITE_WAIT );
         }

         /*
          * Initialize file dialog structure
          */
         memset( &sFileDlg, 0, sizeof( FILEDLG ) );
         sFileDlg.cbSize = sizeof( FILEDLG );
         sFileDlg.fl = FDS_CENTER | FDS_OPEN_DIALOG;
         sFileDlg.pszTitle = "Open Executable File";
         strcpy( sFileDlg.szFullFile, szInFile );
         strcpy( strrchr( sFileDlg.szFullFile, '\\' ) + 1, ( ( ulMajorVer == 2L ) && ( ulMinorVer >= 40L ) ) ? "*.EXE;*.COM;*.DLL" : "*.EXE" );

         /*
          * Create file dialog to obtain executable file name, process user's
          * selection if successful
          */
         if ( WinFileDlg( HWND_DESKTOP, hwndMainClient, &sFileDlg ) )
         {
            /*
             * Terminate enablement thread if user selected cancel button
             */
            if ( sFileDlg.lReturn == DID_CANCEL )
            {
               DosReleaseMutexSem( hmtxSys );
               DosExit( EXIT_THREAD, 0L );
            }

            /*
             * Save input file name if user selected OK button
             */
            else if ( sFileDlg.lReturn == DID_OK )
            {
               strcpy( szInFile, sFileDlg.szFullFile );
            }
         }

         /*
          * Issue error and terminate if file dialog creation unsuccessful
          */
         else
         {
            Error( "Unable to create enablement thread file dialog" );
         }

         /*
          * Release ownership of system semaphore
          */
         DosReleaseMutexSem( hmtxSys );

         /*
          * Allocate enablement information, issue error and terminate if
          * unsuccessful
          */
         if ( !( pEnabInfo = malloc( sizeof( ENABINFO ) + ( ( SUPPORTED_DLL - 1 ) * sizeof( pEnabInfo->DLLFlags[ 0 ] ) ) ) ) )
         {
            Error( "Unable to allocate memory for enablement thread information" );
         }

         /*
          * Set enablement information in exception registration record
          */
         sEnabRegRec.pEnabInfo = pEnabInfo;

         /*
          * Initialize enablement information
          */
         memset( pEnabInfo, 0, sizeof( ENABINFO ) + ( ( SUPPORTED_DLL - 1 ) * sizeof( pEnabInfo->DLLFlags[ 0 ] ) ) );

         /*
          * Save executable file name in enablement information
          */
         strcpy( pEnabInfo->szInFile, sFileDlg.szFullFile );

         /*
          * Indicate preserve file date/time stamp if menu item checked
          */
         if ( WinIsMenuItemChecked( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_ENABPDTS ) )
         {
            SETFLAG( pEnabInfo->ulFlags, fPOPTION );
         }

         /*
          * Read executable file and initialize tables
          */
         InitializeEnablement( pEnabInfo );

         /*
          * Analyze API usage in executable file
          */
         AnalyzeAPIUsage( pEnabInfo );

         /*
          * Initialize enablement dialog help manager initialization
          * information
          */
         hmiEnab.cb                       = sizeof( HELPINIT );
         hmiEnab.ulReturnCode             = 0L;
         hmiEnab.pszTutorialName          = NULL;
         hmiEnab.phtHelpTable             = (PHELPTABLE)(0xFFFF0000 | HLP_ENAB );
         hmiEnab.hmodHelpTableModule      = 0L;
         hmiEnab.hmodAccelActionBarModule = 0L;
         hmiEnab.idAccelTable             = 0L;
         hmiEnab.idActionBar              = 0L;
         hmiEnab.pszHelpWindowTitle       = hmiMain.pszHelpWindowTitle;
         hmiEnab.fShowPanelId             = CMIC_HIDE_PANEL_ID;
         hmiEnab.pszHelpLibraryName       = DEF_HELPFILE;

         /*
          * Create enablement dialog help instance, issue warning if
          * unsuccessful
          */
         if ( !( hwndEnabHelp = WinCreateHelpInstance( sEnabRegRec.habEnab, &hmiEnab ) ) )
         {
            ErrorMessageBox( MB_WARNING, "Unable to create enablement window help instance, help disabled" );
         }

         /*
          * Create enablement dialog, initiate enablement dialog message
          * processing if successful
          */
         if ( ( hwndEnab = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, EnablementDlgProc, NULLHANDLE, DLG_ENAB, pEnabInfo ) ) != 0L )
         {
            /*
             * Associate enablement dialog help instance with enablement dialog
             * if instance created
             */
            if ( hwndEnabHelp )
            {
               /*
                * Associate enablement dialog help instance with enablement
                * dialog, issue warning if unsuccessful
                */
               if ( !WinAssociateHelpInstance( hwndEnabHelp, hwndEnab ) )
               {
                  ErrorMessageBox( MB_WARNING, "Unable to associate enablement dialog help instance, help disabled" );
               }
            }

            /*
             * Process enablement dialog messages
             */
            while( WinGetMsg( sEnabRegRec.habEnab, &qmsgEnab, 0L, 0L, 0L ) )
            {
               WinDispatchMsg( sEnabRegRec.habEnab, &qmsgEnab );
            }

            /*
             * Destroy enablement dialog
             */
            WinDestroyWindow( hwndEnab );
         }

         /*
          * Issue error and terminate if enablement dialog creation
          * unsuccessful
          */
         else
         {
            Error( "Unable to create enablement dialog" );
         }

         /*
          * Destroy enablement dialog help instance if instance created
          */
         if ( hwndEnabHelp )
         {
            WinDestroyHelpInstance( hwndEnabHelp );
         }

         /*
          * Close executable file and terminate tables
          */
         TerminateEnablement( TRUE, pEnabInfo );

         /*
          * Launch executable file if launch indicated and file is .EXE/.COM
          */
         if ( ( FLAGSET( pEnabInfo->ulFlags, fLAUNCH ) ) && ( !DosQueryAppType( pEnabInfo->szInFile, &ulAppType ) ) && ( FLAGCLR( ulAppType, FAPPTYP_DLL | FAPPTYP_DOS | FAPPTYP_PHYSDRV | FAPPTYP_VIRTDRV | FAPPTYP_PROTDLL ) ) )
         {
            /*
             * Initialize enablement dialog help manager initialization
             * information
             */
            hmiExe.cb                       = sizeof( HELPINIT );
            hmiExe.ulReturnCode             = 0L;
            hmiExe.pszTutorialName          = NULL;
            hmiExe.phtHelpTable             = (PHELPTABLE)(0xFFFF0000 | HLP_EXE );
            hmiExe.hmodHelpTableModule      = 0L;
            hmiExe.hmodAccelActionBarModule = 0L;
            hmiExe.idAccelTable             = 0L;
            hmiExe.idActionBar              = 0L;
            hmiExe.pszHelpWindowTitle       = hmiMain.pszHelpWindowTitle;
            hmiExe.fShowPanelId             = CMIC_HIDE_PANEL_ID;
            hmiExe.pszHelpLibraryName       = DEF_HELPFILE;

            /*
             * Create enablement dialog help instance, issue warning if
             * unsuccessful
             */
            if ( !( hwndExeHelp = WinCreateHelpInstance( sEnabRegRec.habEnab, &hmiExe ) ) )
            {
               ErrorMessageBox( MB_WARNING, "Unable to create launch .EXE/.COM window help instance, help disabled" );
            }

            /*
             * Create launch executable dialog, initiate launch executable
             * dialog message processing if successful
             */
            if ( ( hwndExe = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, LaunchExeDlgProc, NULLHANDLE, DLG_EXE, pEnabInfo->szInFile ) ) != 0L )
            {
               /*
                * Associate launch executable dialog help instance with launch
                * executable dialog if instance created
                */
               if ( hwndExeHelp )
               {
                  /*
                   * Associate launch executable dialog help instance with
                   * launch executable dialog, issue warning if unsuccessful
                   */
                  if ( !WinAssociateHelpInstance( hwndExeHelp, hwndExe ) )
                  {
                     ErrorMessageBox( MB_WARNING, "Unable to associate launch .EXE/.COM dialog help instance, help disabled" );
                  }
               }

               /*
                * Process launch executable dialog messages
                */
               while( WinGetMsg( sEnabRegRec.habEnab, &qmsgExe, 0L, 0L, 0L ) )
               {
                  WinDispatchMsg( sEnabRegRec.habEnab, &qmsgExe );
               }

               /*
                * Destroy launch executable dialog
                */
               WinDestroyWindow( hwndExe );
            }

            /*
             * Issue error and terminate if launch executable dialog creation
             * unsuccessful
             */
            else
            {
               Error( "Unable to create launch .EXE/.COM dialog" );
            }

            /*
             * Destroy launch executable dialog help instance if instance
             * created
             */
            if ( hwndExeHelp )
            {
               WinDestroyHelpInstance( hwndExeHelp );
            }
         }

         /*
          * Free memory for enablement information
          */
         free( pEnabInfo );

         /*
          * Clear enablement information in exception registration record
          */
         sEnabRegRec.pEnabInfo = NULL;

         /*
          * Destroy enablement dialog message queue
          */
         WinDestroyMsgQueue( sEnabRegRec.hmqEnab );
      }

      /*
       * Issue error and terminate if enablement dialog message queue creation
       * unsuccessful
       */
      else
      {
         Error( "Unable to create enablement dialog message queue" );
      }

      /*
       * Terminate Presentation Manager facilities
       */
      WinTerminate( sEnabRegRec.habEnab );
   }

   /*
    * Issue error and terminate if Presentation Manager facilities
    * initialization unsuccessful
    */
   else
   {
      Error( "Unable to initialize Presentation Manager" );
   }

   /*
    * Unregister exception handler
    */
   DosUnsetExceptionHandler( (PREGREC)&sEnabRegRec );

}  /* EnablementThread() */

/******************************************************************************/
/*                                                                            */
/* Error()                                                                    */
/*                                                                            */
/******************************************************************************/

VOID Error( PCHAR szFmt                          /* Error message format      */
          , ...                                  /* Error message inserts     */
          )
{

   va_list  pArgs;                               /* Error message inserts     */
   FILE    *pFile;                               /* Error file                */
   CHAR     szFmtMsg[ LEN_IOLINE ];              /* Formatted error message   */

   /*
    * Format error message including message inserts
    */
   va_start( pArgs, szFmt );
   vsprintf( szFmtMsg, szFmt, pArgs );
   va_end( pArgs );

   /*
    * Display error message in message box if main anchor block initialized
    */
   if ( habMain )
   {
      ErrorMessageBox( MB_ERROR, szFmtMsg );
   }

   /*
    * Place error message in error message file if main anchor block not
    * initialized
    */
   else
   {
      /*
       * Open error message file, log error message if successful
       */
      if ( ( pFile = fopen( DEF_ERRORFILE, "a" ) ) != NULL )
      {
         /*
          * Log error message to error message file
          */
         fprintf( pFile, "%s: %s\n", szPMAppName, szFmtMsg );

         /*
          * Close error message file
          */
         fclose( pFile );
      }
   }

   /*
    * Terminate program
    */
   DosExit( EXIT_THREAD, ERROR );

}  /* Error() */

/******************************************************************************/
/*                                                                            */
/* ErrorMessageBox()                                                          */
/*                                                                            */
/******************************************************************************/

VOID ErrorMessageBox( ULONG ulType               /* Error message type        */
                    , PCHAR szFmt                /* Error message format      */
                    , ...                        /* Error message inserts     */
                    )
{

   va_list  pArgs;                               /* Error message inserts     */
   CHAR     szFmtMsg[ LEN_IOLINE ];              /* Formatted error message   */
   CHAR     szTitle[ LEN_IOLINE ];               /* Error title               */

   /*
    * Format error message including message inserts
    */
   va_start( pArgs, szFmt );
   vsprintf( szFmtMsg, szFmt, pArgs );
   va_end( pArgs );

   /*
    * Format error title
    */
   sprintf( szTitle, "%s %s", VER_FULLNAME, ulType == MB_ERROR ? "Error" : "Warning" );

   /*
    * Display error message in message box
    */
   WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, (PCH)szFmtMsg, (PCH)szTitle, ID_MAINMSGBOX, MB_OK | MB_APPLMODAL | MB_MOVEABLE | ulType );

}  /* ErrorMessageBox() */

/******************************************************************************/
/*                                                                            */
/* ExceptionHandler()                                                         */
/*                                                                            */
/******************************************************************************/

ULONG APIENTRY ExceptionHandler( PREPREC pRep    /* Exception information     */
                               , PREGREC pReg    /* Registration record       */
                               , PCTXREC pCtx    /* Exception context         */
                               , PVOID   p       /* Not used                  */
                               )
{

   /*
    * Clean up if being removed from exception handler chain
    */
   if ( FLAGSET( pRep->fHandlerFlags, EH_UNWINDING ) )
   {
      /*
       * Release access to system semaphore
       */
      while ( !DosReleaseMutexSem( hmtxSys ) );

      /*
       * Delete system semaphore
       */
      DosCloseMutexSem( hmtxSys );
   }

   /*
    * Indicate ignore process terminate exceptions
    */
   else if ( ( pRep->ExceptionNum == XCPT_PROCESS_TERMINATE ) || ( pRep->ExceptionNum == XCPT_ASYNC_PROCESS_TERMINATE ) )
   {
      return XCPT_CONTINUE_SEARCH;
   }

   /*
    * Issue error and terminate if signal received from another process
    */
   else if ( pRep->ExceptionNum == XCPT_SIGNAL )
   {
      Error( "User interrupt" );
   }

   /*
    * Issue error and terminate if fatal exception generated
    */
   else if ( ( pRep->ExceptionNum & XCPT_SEVERITY_CODE ) == XCPT_FATAL_EXCEPTION )
   {
#ifdef DEBUG
      Error( "Fatal exception at 0x%08lX: 0x%08lX", pRep->ExceptionAddress, pRep->ExceptionNum );
#else
      Error( "Fatal exception: 0x%08lX", pRep->ExceptionNum );
#endif
   }

   /*
    * Indicate ignore exception
    */
   return XCPT_CONTINUE_SEARCH;

}  /* ExceptionHandler() */

/******************************************************************************/
/*                                                                            */
/* GetAPIInfo()                                                               */
/*                                                                            */
/******************************************************************************/

PAPIINFO GetAPIInfo( PSUMINFO pSumInfo           /* Summarization information */
                   , PCHAR    szName             /* API name                  */
                   )
{

   PAPIINFO pAPI;                                /* API information           */

   /*
    * Start at beginning of API information chain
    */
   pAPI = pSumInfo->pAPIFirst;

   /*
    * Search through API information for matching API name
    */
   while ( pAPI )
   {
      /*
       * Return API information address if matching API name found
       */
      if ( !Strcmpi( pAPI->szName, szName ) )
      {
         return pAPI;
      }

      /*
       * Skip API information
       */
      pAPI = pAPI->pNext;
   }

   /*
    * Indicate match not found
    */
   return NULL;

}  /* GetAPIInfo() */

/******************************************************************************/
/*                                                                            */
/* HelpInfoDlgProc()                                                          */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY HelpInfoDlgProc( HWND   hwndDlg /* Window handle             */
                                , ULONG  ulMsg   /* Message number            */
                                , MPARAM mp1Dlg  /* Message parameter 1       */
                                , MPARAM mp2Dlg  /* Message parameter 2       */
                                )
{

   CHAR szText[ LEN_IOLINE ];                    /* Dialog text               */

   /*
    * Identify help information dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying help information dialog
       */
      case WM_COMMAND:
         /*
          * Identify help information dialog control processing based on
          * command value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process OK push button
             */
            case DLG_INFOOK:
               /*
                * Close help information dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;
         }

         return (MRESULT)FALSE;

      /*
       * Initializing help information dialog
       */
      case WM_INITDLG:
         /*
          * Format help information program version
          */
#ifdef PRIVATE
         sprintf( szText, "PRIVATE Version %1d.%02d.%02d", VER_MAJOR, VER_MINOR, VER_REV );
#else
         sprintf( szText, "Version %1d.%02d.%02d", VER_MAJOR, VER_MINOR, VER_REV );
#endif

         /*
          * Set help information program version text
          */
         WinSetWindowText( WinWindowFromID( hwndDlg, DLG_INFOTXT2 ), szText );

         /*
          * Give help information customization dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* HelpInfoDlgProc() */

/******************************************************************************/
/*                                                                            */
/* InitializeEnablement()                                                     */
/*                                                                            */
/******************************************************************************/

VOID InitializeEnablement( PENABINFO pEnabInfo ) /* Enablement information    */
{

   FILESTATUS3  filestatus3;                     /* File information          */
   HFILE        hfile;                           /* File handle               */
   int          i;                               /* Loop variable             */
   PCHAR        pch;                             /* Character pointer         */
   CHAR         szModNam[ LEN_FILENAME ];        /* Module name               */
   ULONG        ulAction;                        /* Action taken by DosOpen   */
   ULONG        ulAttr;                          /* File open attributes      */
   ULONG        ulBaseAddr;                      /* Object base address       */
   ULONG        ulDLLIdx;                        /* DLL information index     */
   ULONG        ulFlag;                          /* File open flags           */
   ULONG        ulMode;                          /* File open mode            */
   USHORT       usMagic;                         /* Magic number              */

   /*
    * Save executable file date/time stamp if -P option specified
    */
   if ( FLAGSET( pEnabInfo->ulFlags, fPOPTION ) )
   {
      /*
       * Initialize open file flags
       */
      ulAttr = FILE_NORMAL;
      ulFlag = OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW;
      ulMode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE;

      /*
       * Open executable file, obtain file information if successful
       */
      if ( !DosOpen( pEnabInfo->szInFile, &hfile, &ulAction, 0L, ulAttr, ulFlag, ulMode, NULL ) )
      {
         /*
          * Obtain executable file information, save file date/time stamp if
          * successful
          */
         if ( !DosQueryFileInfo( hfile, FIL_STANDARD, &filestatus3, sizeof( FILESTATUS3 ) ) )
         {
            pEnabInfo->fdateInFile = filestatus3.fdateLastWrite;
            pEnabInfo->ftimeInFile = filestatus3.ftimeLastWrite;
         }

         /*
          * Disable -P option if unable to obtain executable file information
          */
         else
         {
            CLRFLAG( pEnabInfo->ulFlags, fPOPTION );
         }

         /*
          * Close executable file
          */
         DosClose( hfile );
      }

      /*
       * Disable -P option if unable to open executable file
       */
      else
      {
         CLRFLAG( pEnabInfo->ulFlags, fPOPTION );
      }
   }

   /*
    * Open executable file, issue error and terminate if unsuccessful
    */
   if ( !( pEnabInfo->fpInFile = fopen( pEnabInfo->szInFile, "rb+" ) ) )
   {
      Error( "Unable to open executable file: %s", pEnabInfo->szInFile );
   }

   /*
    * Read DOS header and identify DOS or OS/2 signature, process OS/2 header
    * if successful
    */
   if ( ( fread( &pEnabInfo->sMZHdr, sizeof( pEnabInfo->sMZHdr ), 1, pEnabInfo->fpInFile ) == 1 ) &&
        ( ( pEnabInfo->sMZHdr.e_magic == EMAGIC   ) ||
          ( pEnabInfo->sMZHdr.e_magic == NEMAGIC  ) ||
          ( pEnabInfo->sMZHdr.e_magic == E32MAGIC )                                             ) )
   {
      /*
       * Zero DOS header if DOS header not present
       */
      if ( pEnabInfo->sMZHdr.e_magic != EMAGIC )
      {
         memset( &pEnabInfo->sMZHdr, 0, sizeof( MZHDR ) );
      }

      /*
       * Locate beginning of OS/2 header and read OS/2 header; issue error and
       * terminate if unsuccessful
       */
      if ( ( fseek( pEnabInfo->fpInFile, 0L, SEEK_END )                                              ) ||
           ( ftell( pEnabInfo->fpInFile ) < pEnabInfo->sMZHdr.e_lfanew + sizeof( pEnabInfo->sLXHdr ) ) ||
           ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew, SEEK_SET )                      ) ||
           ( fread( &pEnabInfo->sLXHdr, sizeof( pEnabInfo->sLXHdr ), 1, pEnabInfo->fpInFile ) != 1   ) )
      {
         Error( "File DOS executable: %s", pEnabInfo->szInFile );
      }

      /*
       * Executable file follows segmented format if magic number NE
       */
      if ( *( (PUSHORT)pEnabInfo->sLXHdr.e32_magic ) == NEMAGIC )
      {
         /*
          * Indicate NE executable format
          */
         SETFLAG( pEnabInfo->ulFlags, fNE_FMT );

         /*
          * Obtain 16-bit executable header and sector size
          */
         pEnabInfo->pNEHdr = (NEHDR *)&pEnabInfo->sLXHdr;
         pEnabInfo->usSector = 1 << pEnabInfo->pNEHdr->ne_align;

         /*
          * Issue error and terminate if module reference table empty
          * (executable file does not import anything)
          */
         if ( !( pEnabInfo->ulModIdxTbl = pEnabInfo->pNEHdr->ne_cmod ) )
         {
            Error( "File does not import APIs: %s", pEnabInfo->szInFile );
         }

         /*
          * Allocate memory for and read module reference table
          */
         if ( ( pEnabInfo->ulModRefTbl = pEnabInfo->ulModIdxTbl * sizeof( USHORT ) ) > 0L )
         {
            /*
             * Allocate memory for module reference table, issue error and
             * terminate if unsuccessful
             */
            if ( !( pEnabInfo->pModRefTbl = malloc( pEnabInfo->ulModRefTbl ) ) )
            {
               Error( "Unable to allocate memory for module reference table: %s", pEnabInfo->szInFile );
            }

            /*
             * Locate beginning of module reference table and read module
             * reference table; issue error and terminate if unsuccessful
             */
            if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->pNEHdr->ne_modtab, SEEK_SET ) ) ||
                 ( fread( pEnabInfo->pModRefTbl, pEnabInfo->ulModRefTbl, 1, pEnabInfo->fpInFile ) != 1               ) )
            {
               Error( "Unable to read module reference table: %s", pEnabInfo->szInFile );
            }
         }

         /*
          * Allocate memory for and read import name table if one exists
          */
         if ( ( pEnabInfo->ulImpNamTbl = pEnabInfo->pNEHdr->ne_nrestab - ( pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->pNEHdr->ne_imptab ) ) > 0L )
         {
            /*
             * Allocate memory for import name table, issue error and terminate
             * if unsuccessful
             */
            if ( !( pEnabInfo->pImpNamTbl = malloc( pEnabInfo->ulImpNamTbl ) ) )
            {
               Error( "Unable to allocate memory for import name table: %s", pEnabInfo->szInFile );
            }

            /*
             * Locate beginning of import name table and read import name
             * table; issue error and terminate if unsuccessful
             */
            if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->pNEHdr->ne_imptab, SEEK_SET ) ) ||
                 ( fread( pEnabInfo->pImpNamTbl, pEnabInfo->ulImpNamTbl, 1, pEnabInfo->fpInFile ) != 1               ) )
            {
               Error( "Unable to read import name table: %s", pEnabInfo->szInFile );
            }
         }

         /*
          * Obtain number of segments
          */
         pEnabInfo->ulSegment = pEnabInfo->pNEHdr->ne_cseg;

         /*
          * Allocate memory for and read segment table if one exists
          */
         if ( ( pEnabInfo->ulSegmentTbl = pEnabInfo->ulSegment * sizeof( NESEG ) ) > 0L )
         {
            /*
             * Allocate memory for segment table, issue error and terminate if
             * unsuccessful
             */
            if ( !( pEnabInfo->pSegmentTbl = malloc( pEnabInfo->ulSegmentTbl ) ) )
            {
               Error( "Unable to allocate memory for segment table: %s", pEnabInfo->szInFile );
            }

            /*
             * Locate beginning of segment table and read segment table; issue
             * error and terminate if unsuccessful
             */
            if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->pNEHdr->ne_segtab, SEEK_SET ) ) ||
                 ( fread( pEnabInfo->pSegmentTbl, pEnabInfo->ulSegmentTbl, 1, pEnabInfo->fpInFile ) != 1             ) )
            {
               Error( "Unable to read segment table: %s", pEnabInfo->szInFile );
            }
         }

         /*
          * Allocate memory for module index table, issue error and terminate
          * if unsuccessful
          */
         if ( !( pEnabInfo->pModIdxTbl = malloc( pEnabInfo->ulModIdxTbl * sizeof( ULONG ) ) ) )
         {
            Error( "Unable to allocate memory for module index table: %s", pEnabInfo->szInFile );
         }

         /*
          * Initialize module index table
          */
         for ( i = 0; i < pEnabInfo->ulModIdxTbl; i++ )
         {
            /*
             * Obtain module name
             */
            pch = (PCHAR)( (ULONG)pEnabInfo->pImpNamTbl + pEnabInfo->pModRefTbl[ i ] );
            memcpy( szModNam, pch + 1, *pch );
            szModNam[ *pch ] = '\0';

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate DLL if supported DLL
             */
            if ( ( pEnabInfo->pModIdxTbl[ i ] = LookupDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fDLL );
            }

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate trace DLL if supported trace DLL
             */
            else if ( ( pEnabInfo->pModIdxTbl[ i ] = LookupTraceDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fTRACEDLL );
            }
         }
      }

      /*
       * Executable file follows linear format if magic number LX
       */
      else if ( *( (PUSHORT)pEnabInfo->sLXHdr.e32_magic ) == E32MAGIC )
      {
         /*
          * Indicate LX executable format
          */
         SETFLAG( pEnabInfo->ulFlags, fLX_FMT );

         /*
          * Issue error and terminate if module name table empty (executable
          * file does not import anything)
          */
         if ( !( pEnabInfo->ulModIdxTbl = pEnabInfo->sLXHdr.e32_impmodcnt ) )
         {
            Error( "File does not import APIs: %s", pEnabInfo->szInFile );
         }

         /*
          * Allocate memory for and read module name table
          */
         if ( ( pEnabInfo->ulModNamTbl = pEnabInfo->sLXHdr.e32_impproc - pEnabInfo->sLXHdr.e32_impmod ) > 0L )
         {
            /*
             * Allocate memory for module name table, issue error and terminate
             * if unsuccessful
             */
            if ( !( pEnabInfo->pModNamTbl = malloc( pEnabInfo->ulModNamTbl ) ) )
            {
               Error( "Unable to allocate memory for module name table: %s", pEnabInfo->szInFile );
            }

            /*
             * Locate beginning of module name table and read module name
             * table; issue error and terminate if unsuccessful
             */
            if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->sLXHdr.e32_impmod, SEEK_SET ) ) ||
                 ( fread( pEnabInfo->pModNamTbl, pEnabInfo->ulModNamTbl, 1, pEnabInfo->fpInFile ) != 1               ) )
            {
               Error( "Unable to read module name table: %s", pEnabInfo->szInFile );
            }
         }

         /*
          * Allocate memory for and read procedure name table if one exists
          */
         if ( ( pEnabInfo->ulPrcNamTbl = pEnabInfo->sLXHdr.e32_fpagetab + pEnabInfo->sLXHdr.e32_fixupsize - pEnabInfo->sLXHdr.e32_impproc ) > 0L )
         {
            /*
             * Allocate memory for procedure name table, issue error and
             * terminate if unsuccessful
             */
            if ( !( pEnabInfo->pPrcNamTbl = malloc( pEnabInfo->ulPrcNamTbl ) ) )
            {
               Error( "Unable to allocate memory for procedure name table: %s", pEnabInfo->szInFile );
            }

            /*
             * Locate beginning of procedure name table and read procedure name
             * table; issue error and terminate if unsuccessful
             */
            if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->sLXHdr.e32_impproc, SEEK_SET ) ) ||
                 ( fread( pEnabInfo->pPrcNamTbl, pEnabInfo->ulPrcNamTbl, 1, pEnabInfo->fpInFile ) != 1                ) )
            {
               Error( "Unable to read procedure name table: %s", pEnabInfo->szInFile );
            }
         }

         /*
          * Allocate memory for and read fixup table if one exists
          */
         if ( ( pEnabInfo->ulFixupTbl = pEnabInfo->sLXHdr.e32_impmod - pEnabInfo->sLXHdr.e32_frectab ) > 0L )
         {
            /*
             * Allocate memory for fixup record table, issue error and
             * terminate if unsuccessful
             */
            if ( !( pEnabInfo->pFixupTbl = malloc( pEnabInfo->ulFixupTbl ) ) )
            {
               Error( "Unable to allocate memory for fixup record table: %s", pEnabInfo->szInFile );
            }

            /*
             * Locate beginning of fixup record table and read fixup record
             * table; issue error and terminate if unsuccessful
             */
            if ( ( fseek( pEnabInfo->fpInFile, pEnabInfo->sMZHdr.e_lfanew + pEnabInfo->sLXHdr.e32_frectab, SEEK_SET ) ) ||
                 ( fread( pEnabInfo->pFixupTbl, pEnabInfo->ulFixupTbl, 1, pEnabInfo->fpInFile ) != 1                  ) )
            {
               Error( "Unable to read fixup record table: %s", pEnabInfo->szInFile );
            }
         }

         /*
          * Allocate memory for module index table, issue error and terminate
          * if unsuccessful
          */
         if ( !( pEnabInfo->pModIdxTbl = malloc( pEnabInfo->ulModIdxTbl * sizeof( ULONG ) ) ) )
         {
            Error( "Unable to allocate memory for module index table: %s", pEnabInfo->szInFile );
         }

         /*
          * Initialize module index table
          */
         for ( i = 0, pch = pEnabInfo->pModNamTbl; i < pEnabInfo->ulModIdxTbl; i++, pch += *pch + 1 )
         {
            /*
             * Obtain module name
             */
            memcpy( szModNam, pch + 1, *pch );
            szModNam[ *pch ] = '\0';

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate DLL if supported DLL
             */
            if ( ( pEnabInfo->pModIdxTbl[ i ] = LookupDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fDLL );
            }

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate trace DLL if supported trace DLL
             */
            else if ( ( pEnabInfo->pModIdxTbl[ i ] = LookupTraceDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( pEnabInfo->DLLFlags[ pEnabInfo->pModIdxTbl[ i ] ], fTRACEDLL );
            }
         }
      }

      /*
       * Issue error and terminate if magic number unrecognized
       */
      else
      {
         Error( "File not OS/2 executable: %s", pEnabInfo->szInFile );
      }
   }

   /*
    * Issue error and terminate if DOS signature absent
    */
   else
   {
      Error( "File not executable: %s", pEnabInfo->szInFile );
   }

}  /* InitializeEnablement() */

/******************************************************************************/
/*                                                                            */
/* InitializeSummarization()                                                  */
/*                                                                            */
/******************************************************************************/

VOID InitializeSummarization( PSUMINFO pSumInfo )/* Summarization information */
{

   /*
    * Open trace information file, issue error and terminate if unsuccessful
    */
   if ( !( pSumInfo->fpInFile = fopen( pSumInfo->szInFile, "r+" ) ) )
   {
      Error( "Unable to open trace information file: %s", pSumInfo->szInFile );
   }

}  /* InitializeSummarization() */

/******************************************************************************/
/*                                                                            */
/* IsUnsupportedNRRNAM()                                                      */
/*                                                                            */
/******************************************************************************/

BOOL IsUnsupportedNRRNAM( PCHAR szDLL            /* DLL name                  */
                        , PCHAR szAPI            /* API name                  */
                        , ULONG ulAPILen         /* API name length           */
                        )
{

   int        i, j;                              /* Loop variables            */
   PUNSNRRNAM pNRRNAM;                           /* Unsupported NRRNAM        */

   /*
    * Search through unsupported NRRNAMs table for matching DLL name
    */
   for ( i = 0, pNRRNAM = UnsNRRNAM; i < sizeof( UnsNRRNAM ) / sizeof( UNSNRRNAM ); i++, pNRRNAM++ )
   {
      /*
       * Search complete if DLL name found in unsupported NRRNAMs table
       */
      if ( !strcmp( szDLL, pNRRNAM->szDLL ) )
      {
         /*
          * Search through unsupported NRRNAMs table for matching API name
          */
         for ( j = 0; j < pNRRNAM->ulNumAPI; j++ )
         {
            /*
             * Search complete if API name found in unsupported NRRNAMs table
             */
            if ( !strncmp( szAPI, pNRRNAM->szAPI[ j ], ulAPILen ) )
            {
               return TRUE;
            }
         }
      }
   }

   /*
    * Indicate API name not found in unsupported NRRNAMs table
    */
   return FALSE;

}  /* IsUnsupportedNRRNAM() */

/******************************************************************************/
/*                                                                            */
/* LaunchExeDlgProc()                                                         */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY LaunchExeDlgProc( HWND   hwndDlg/* Dialog handle             */
                                 , ULONG  ulMsg  /* Message number            */
                                 , MPARAM mp1Dlg /* Message parameter 1       */
                                 , MPARAM mp2Dlg /* Message parameter 2       */
                                 )
{

   PCHAR     pch;                                /* Character pointer         */
   PID       pidExe;                             /* Exe process identifier    */
   ULONG     sidExe;                             /* Exe session identifier    */
   STARTDATA sStartData;                         /* Start session data        */
   SWP       swpDlg;                             /* Dialog window position    */
   CHAR      szParms[ LEN_IOLINE ];              /* Exe parameters            */
   CHAR      szText[ CCHMAXPATH ];               /* Dialog text               */
   APIRET    ulRC;                               /* Return code               */

   /*
    * Identify launch executable dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Closing launch executable dialog
       */
      case WM_CLOSE:
         /*
          * Translate as OK push button
          */
         WinPostMsg( hwndDlg, WM_COMMAND, MPFROMSHORT( DLG_EXECANCEL ), MPVOID );

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying launch executable dialog
       */
      case WM_COMMAND:
         /*
          * Identify launch executable dialog control processing based on
          * command value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_EXECANCEL:
               /*
                * Close launch executable dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               /*
                * Terminate processing launch executable dialog messages
                */
               WinPostMsg( hwndDlg, WM_QUIT, MPVOID, MPVOID );

               break;

            /*
             * Process help push button
             */
            case DLG_EXEHELP:
               /*
                * Display launch executable help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_EXE ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_EXEOK:
               /*
                * Obtain parameters, initialize parameters to empty if
                * parameters entry field unchanged or unable to obtain
                * parameters
                */
               if ( ( !WinSendMsg( WinWindowFromID( hwndDlg, DLG_EXEPARM ), EM_QUERYCHANGED, MPFROMLONG( 0L ), MPFROMLONG( 0L ) ) ) ||
                    ( !WinQueryWindowText( WinWindowFromID( hwndDlg, DLG_EXEPARM ), sizeof( szParms ), szParms )                  ) )
               {
                  szParms[ 0 ] = '\0';
               }

               /*
                * Initialize start session data structure
                */
               memset( &sStartData, 0, sizeof( STARTDATA ) );
               sStartData.Length = sizeof( STARTDATA );
               sStartData.Related = SSF_RELATED_INDEPENDENT;
               sStartData.FgBg = ( (USHORT)WinSendMsg( WinWindowFromID( hwndDlg, DLG_EXEBGSESRAD ), BM_QUERYCHECK, MPVOID, MPVOID ) == 1 ) ? SSF_FGBG_BACK : SSF_FGBG_FORE;
               sStartData.TraceOpt = SSF_TRACEOPT_NONE;
               sStartData.PgmTitle = NULL;
               sStartData.PgmName = (PSZ)WinQueryWindowPtr( hwndDlg, 0 );
               sStartData.PgmInputs = (PBYTE)szParms;
               sStartData.TermQ = NULL;
               sStartData.Environment = NULL;
               sStartData.InheritOpt = SSF_INHERTOPT_SHELL;
               sStartData.SessionType = SSF_TYPE_DEFAULT;
               sStartData.IconFile = NULL;
               sStartData.PgmHandle = 0L;
               sStartData.PgmControl = SSF_CONTROL_VISIBLE;
               sStartData.InitXPos = 0;
               sStartData.InitYPos = 0;
               sStartData.InitXSize = 0;
               sStartData.InitYSize = 0;
               szText[ 0 ] = '\0';
               sStartData.ObjectBuffer = szText;
               sStartData.ObjectBuffLen = sizeof( szText );

               /*
                * Launch executable, issue error if unsuccessful
                */
               if ( ( ulRC = DosStartSession( &sStartData, &sidExe, &pidExe ) ) != NO_ERROR )
               {
                  ErrorMessageBox( MB_ERROR, "Unable to launch executable, OS/2 error %d%s%s", ulRC, szText[ 0 ] ? " with failing object " : "", szText[ 0 ] ? szText : "" );
               }

               /*
                * Close launch executable dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               /*
                * Terminate processing launch executable dialog messages
                */
               WinPostMsg( hwndDlg, WM_QUIT, MPVOID, MPVOID );

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing launch executable dialog
       */
      case WM_INITDLG:
         /*
          * Obtain launch executable dialog window position
          */
         WinQueryWindowPos( hwndDlg, &swpDlg );

         /*
          * Move launch executable dialog to center of desktop
          */
         WinSetWindowPos( hwndDlg, 0L, ( swpDesktop.cx - swpDlg.cx ) / 2, ( swpDesktop.cy - swpDlg.cy ) / 2, 0L, 0L, SWP_MOVE );

         /*
          * Set launch executable dialog window pointer, issue error and
          * terminate if unsuccessful
          */
         if ( !WinSetWindowPtr( hwndDlg, 0L, mp2Dlg ) )
         {
            Error( "Unable to set launch .EXE/.COM dialog window pointer" );
         }

         /*
          * Format launch executable dialog title
          */
         sprintf( szText, "Launch %s", ( ( ( pch = strrchr( (PCHAR)mp2Dlg, '\\' ) ) != NULL ) || ( ( pch = strrchr( (PCHAR)mp2Dlg, ':' ) ) != NULL ) ) ? pch + 1 : (PCHAR)mp2Dlg );

         /*
          * Set launch executable dialog title
          */
         WinSetWindowText( hwndDlg, szText );

         /*
          * Initialize parameters entry field
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_EXEPARM ), EM_SETTEXTLIMIT, MPFROMLONG( sizeof( szParms ) - 1 ), MPFROMLONG( 0L ) );

         /*
          * Set foreground session radio button
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_EXEFGSESRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );

         /*
          * Give parameters entry field focus
          */
         WinSetFocus( HWND_DESKTOP, WinWindowFromID( hwndDlg, DLG_EXEPARM ) );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;

      /*
       * Key pressed
       */
      case WM_CHAR:
         /*
          * Simulate OK push button if either enter key pressed in entry field
          */
         if ( ( SHORT1FROMMP( mp1Dlg ) & KC_VIRTUALKEY                                             ) &&
              ( ( SHORT2FROMMP( mp2Dlg ) == VK_ENTER ) || ( SHORT2FROMMP( mp2Dlg ) == VK_NEWLINE ) ) &&
              ( WinQueryFocus( HWND_DESKTOP ) == WinWindowFromID( hwndDlg, DLG_EXEPARM )           ) )
         {
            /*
             * Simulate OK push button
             */
            WinPostMsg( hwndDlg, WM_COMMAND, MPFROM2SHORT( DLG_EXEOK, 0 ), MPVOID );

            /*
             * Indicate focus window unchanged
             */
            return (MRESULT)FALSE;
         }

         break;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* LaunchExeDlgProc() */

/******************************************************************************/
/*                                                                            */
/* LookupDLL()                                                                */
/*                                                                            */
/******************************************************************************/

ULONG LookupDLL( PCHAR szDLL )                   /* DLL name                  */
{

   int i;                                        /* Loop variable             */

   /*
    * Search through DLL information table for matching DLL name
    */
   for ( i = 0; i < SUPPORTED_DLL; i++ )
   {
      /*
       * Search complete if DLL name found in DLL information table
       */
      if ( !Strcmpi( szDLL, DLLInfo[ i ].szName ) )
      {
         /*
          * Return index of DLL name in DLL information table
          */
         return i;
      }
   }

   /*
    * Indicate DLL name not found in DLL information table
    */
   return UNKNOWN_DLL;

}  /* LookupDLL() */

/******************************************************************************/
/*                                                                            */
/* LookupTraceDLL()                                                           */
/*                                                                            */
/******************************************************************************/

ULONG LookupTraceDLL( PCHAR szDLL )              /* Trace DLL name            */
{

   int i;                                        /* Loop variable             */

   /*
    * Search through DLL information table for matching trace DLL name
    */
   for ( i = 0; i < SUPPORTED_DLL; i++ )
   {
      /*
       * Search complete if trace DLL name found in DLL information table
       */
      if ( !Strcmpi( szDLL, DLLInfo[ i ].szTraceDLLName ) )
      {
         /*
          * Return index of trace DLL name in DLL information table
          */
         return i;
      }
   }

   /*
    * Indicate trace DLL name not found in DLL information table
    */
   return UNKNOWN_DLL;

}  /* LookupTraceDLL() */

/******************************************************************************/
/*                                                                            */
/* MainWindowProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY MainWindowProc( HWND   hwndWin  /* Window handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Win   /* Message parameter 1       */
                               , MPARAM mp2Win   /* Message parameter 2       */
                               )
{

   POINTL           apointlTxtBox[TXTBOX_COUNT]; /* Text box point array      */
   BITMAPINFOHEADER bitmapInfoHeader;            /* Bit map information header*/
   BOOL             bSuccess;                    /* Success indicator         */
   DATETIME         dtBegin;                     /* Beginning time            */
   DATETIME         dtEnd;                       /* Ending time               */
   double           dWinAngle;                   /* Window character angle    */
   FATTRS           fattrsWin;                   /* Window font attributes    */
   FONTMETRICS      fontmetricsWin;              /* Window font metrics       */
   GRADIENTL        gradientlWin;                /* Window character angle    */
   HPS              hpsWin;                      /* Window presentation space */
   int              i, j;                        /* Loop variables            */
   LONG             lcx, lcy;                    /* Window size               */
   LONG             lx, ly;                      /* Window coordinates        */
   PCHAR            pch;                         /* Character pointer         */
   PMONINFO         pMonInfo;                    /* Trace monitor information */
   POINTL           pointlWin;                   /* Window point              */
   RECTL            rectlWin;                    /* Window rectangle          */
   SIZEF            sizefWin;                    /* Window character box size */
   SIZEL            sizelWin;                    /* Window pres page size     */
   SWP              swpWin;                      /* Window position           */
   CHAR             szFlags[ 16 ];               /* Control option string     */
   CHAR             szString[ LEN_FILENAME * 2 ];/* String                    */
   TID              tidThread;                   /* Thread identifier         */
   ULONG            ulWinDiag;                   /* Window diagonal           */
   ULONG            ulWinIncr;                   /* Window char increment     */
   ULONG            ulWinXIncr;                  /* Window horz increment     */
   ULONG            ulWinYIncr;                  /* Window vert increment     */

   /*
    * Identify main window processing based on message number
    */
   switch( ulMsg )
   {
      /*
       * Querying main window keys help window
       */
      case HM_QUERY_KEYS_HELP:
         return (MRESULT)HLP_HELPKEY;

      /*
       * Closing main window
       */
      case WM_CLOSE:
         /*
          * Terminate processing main window messages
          */
         WinPostMsg( hwndWin, WM_QUIT, MPVOID, MPVOID );

         break;

      /*
       * Control notifying main window
       */
      case WM_COMMAND:
         /*
          * Identify main window control processing based on command value
          */
         switch ( SHORT1FROMMP( mp1Win ) )
         {
            /*
             * Process pause and resume tracing control menu items
             */
            case IDM_CTLPAUSE:
            case IDM_CTLRESUME:
               /*
                * Toggle tracing control flag
                */
               if ( FLAGCLR( ulCtrlFlg, C_PAUSE ) ) SETFLAG( ulCtrlFlg, C_PAUSE ); else CLRFLAG( ulCtrlFlg, C_PAUSE );

               /*
                * Save trace control option in operating system profile, issue
                * error and terminate if unsuccessful
                */
               if ( ( ( !sprintf( szFlags, "0x%08lX", ulCtrlFlg ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szCtlFlgKey, szFlags ) ) ) )
               {
                  Error( "Unable to store trace control option" );
               }

               /*
                * Toggle tracing control menu items enabled states
                */
               WinEnableMenuItem( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_CTLPAUSE, FLAGCLR( ulCtrlFlg, C_PAUSE ) );
               WinEnableMenuItem( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_CTLRESUME, FLAGSET( ulCtrlFlg, C_PAUSE ) );

               /*
                * Initiate tracing control dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), ControlDlgProc, 0L, DLG_CTL, NULL );

               break;

            /*
             * Process alternative directory customization menu item
             */
            case IDM_CUSTALT:
               /*
                * Initiate alternative directory customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustAltDlgProc, 0L, DLG_CUSTALT, NULL );

               break;

            /*
             * Process buffer length customization menu item
             */
            case IDM_CUSTBUF:
               /*
                * Initiate buffer length customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustBufDlgProc, 0L, DLG_CUSTBUF, NULL );

               break;

            /*
             * Process tracing control customization menu item
             */
            case IDM_CUSTCTL:
               /*
                * Initiate tracing control customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustCtlDlgProc, 0L, DLG_CUSTCTL, NULL );

               break;

            /*
             * Process DOSCALLS APIs customization menu item
             */
            case IDM_CUSTDOS:
               /*
                * Initiate DOSCALLS APIs customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustDosDlgProc, 0L, DLG_CUSTDOS, NULL );

               break;

            /*
             * Process EBCDIC translation customization menu item
             */
            case IDM_CUSTEBC:
               /*
                * Initiate EBCDIC translation customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustEBCDlgProc, 0L, DLG_CUSTEBC, NULL );

               break;

            /*
             * Process file wrapping customization menu item
             */
            case IDM_CUSTFIL:
               /*
                * Initiate file wrapping customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustFilDlgProc, 0L, DLG_CUSTFIL, NULL );

               break;

            /*
             * Process PMGPI APIs customization menu item
             */
            case IDM_CUSTGPI:
               /*
                * Initiate PMGPI APIs customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustGpiDlgProc, 0L, DLG_CUSTGPI, NULL );

               break;

            /*
             * Process API interception customization menu item
             */
            case IDM_CUSTINT:
               /*
                * Initiate API interception customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustIntDlgProc, 0L, DLG_CUSTINT, NULL );

               break;

            /*
             * Process logging level customization menu item
             */
            case IDM_CUSTLVL:
               /*
                * Initiate logging level customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustLvlDlgProc, 0L, DLG_CUSTLVL, NULL );

               break;

            /*
             * Process time stamping customization menu item
             */
            case IDM_CUSTTIM:
               /*
                * Initiate time stamping customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustTimDlgProc, 0L, DLG_CUSTTIM, NULL );

               break;

#ifdef T_USERHOOK
            /*
             * Process user hook customization menu item
             */
            case IDM_CUSTUSR:
               /*
                * Initiate user hook customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustUsrDlgProc, 0L, DLG_CUSTUSR, NULL );

               break;
#endif

            /*
             * Process PMWIN APIs customization menu item
             */
            case IDM_CUSTWIN:
               /*
                * Initiate PMWIN APIs customization dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), CustWinDlgProc, 0L, DLG_CUSTWIN, NULL );

               break;

            /*
             * Process open file enablement menu item
             */
            case IDM_ENABOPEN:
               /*
                * Create enablement thread, issue error and terminate if
                * unsuccessful
                */
               if ( DosCreateThread( &tidThread, EnablementThread, 0L, CREATE_READY | STACK_SPARSE, 0x8000 ) )
               {
                  Error( "Unable to create enablement thread" );
               }

               break;

            /*
             * Process preserve file date/time stamp enablement menu item
             */
            case IDM_ENABPDTS:
               /*
                * Toggle preserve file date/time stamp menu item check state
                */
               WinCheckMenuItem( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_ENABPDTS, !WinIsMenuItemChecked( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_ENABPDTS ) );

               break;

            /*
             * Process frequently asked questions help push button
             */
            case IDM_HELPFAQ:
               /*
                * Display frequently asked questions help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( HLP_HELPFAQ ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process general help menu item
             */
            case IDM_HELPGEN:
               /*
                * Display main window general help window
                */
               WinSendMsg( hwndMainHelp, HM_GENERAL_HELP, MPVOID, MPVOID );

               break;

            /*
             * Process help index menu item
             */
            case IDM_HELPIDX:
               /*
                * Display main window help index window
                */
               WinSendMsg( hwndMainHelp, HM_HELP_INDEX, MPVOID, MPVOID );

               break;

            /*
             * Process product information help menu item
             */
            case IDM_HELPINFO:
               /*
                * Initiate help information dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), HelpInfoDlgProc, 0L, DLG_HELPINFO, NULL );

               break;

            /*
             * Process keys help menu item
             */
            case IDM_HELPKEY:
               /*
                * Display main window keys help window
                */
               WinSendMsg( hwndMainHelp, HM_KEYS_HELP, MPVOID, MPVOID );

               break;

            /*
             * Process using help menu item
             */
            case IDM_HELPUSE:
               /*
                * Display main window using help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPVOID, MPVOID );

               break;

            /*
             * Process exit main window accelerator
             */
            case IDM_MAINEXIT:
               /*
                * Close main window
                */
               WinPostMsg( hwndWin, WM_CLOSE, MPVOID, MPVOID );

               break;

            /*
             * Process launch executable files menu item
             */
            case IDM_OPTSEXE:
               /*
                * Toggle launch executable files menu item check state
                */
               WinCheckMenuItem( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_OPTSEXE, !WinIsMenuItemChecked( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_OPTSEXE ) );

               break;

            /*
             * Process monitor tracing menu item
             */
            case IDM_OPTSMON:
               /*
                * Toggle monitor tracing menu item check state
                */
               WinCheckMenuItem( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_OPTSMON, !WinIsMenuItemChecked( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_OPTSMON ) );

               /*
                * Initialize trace monitoring if menu item checked
                */
               if ( WinIsMenuItemChecked( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_OPTSMON ) )
               {
                  /*
                   * Create trace monitor queue, issue error and disable trace
                   * monitoring if unsuccessful
                   */
                  if ( DosCreateQueue( &hqueueMon, QUE_FIFO, SYS_QUEUEMON ) )
                  {
                     ErrorMessageBox( MB_ERROR, "Unable to create trace monitor queue, trace monitoring disabled" );
                     WinCheckMenuItem( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_OPTSMON, FALSE );
                  }

                  /*
                   * Create trace monitor thread, issue error and disable trace
                   * monitoring if unsuccessful
                   */
                  else if ( DosCreateThread( &tidThread, MonitorThread, 0L, CREATE_READY | STACK_SPARSE, 0x8000 ) )
                  {
                     ErrorMessageBox( MB_ERROR, "Unable to create trace monitor thread, trace monitoring disabled" );
                     WinCheckMenuItem( WinWindowFromID( hwndMainFrame, FID_MENU ), IDM_OPTSMON, FALSE );
                  }
               }

               /*
                * Terminate trace monitoring if menu item not checked
                */
               else
               {
                  /*
                   * Destroy trace monitor queue if queue exists
                   */
                  if ( hqueueMon )
                  {
                     /*
                      * Quit trace monitoring, making sure trace monitor thread
                      * gets message before destroying queue
                      */
                     DosWriteQueue( hqueueMon, TRCMON_QUIT, 0, NULL, 0 );
                     DosSleep( 1 );

                     /*
                      * Destroy trace monitor queue and indicate queue no
                      * longer exists
                      */
                     DosCloseQueue( hqueueMon );
                     hqueueMon = 0L;
                  }
               }

               break;

            /*
             * Process number APIs menu item
             */
            case IDM_OPTSAPI:
               /*
                * Initiate number APIs dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), OptsAPIDlgProc, 0L, DLG_OPTSAPI, NULL );

               break;

            /*
             * Process file length menu item
             */
            case IDM_OPTSLEN:
               /*
                * Initiate file length dialog
                */
               WinDlgBox( HWND_DESKTOP, WinQueryWindow( hwndWin, QW_PARENT ), OptsLenDlgProc, 0L, DLG_OPTSLEN, NULL );

               break;

            /*
             * Process title/action bar only menu items
             */
            case IDM_OPTSTOP:
            case IDM_OPTSCUR:
            case IDM_OPTSBOT:
               /*
                * Start with no errors
                */
               bSuccess = TRUE;

               /*
                * Obtain main window position
                */
               bSuccess &= WinQueryWindowPos( hwndMainFrame, &swpWin );

               /*
                * Initialize title/action bar only window height to main window
                * frame height
                */
               lcy = swpWin.cy;

               /*
                * Obtain main window client rectangle
                */
               bSuccess &= WinQueryWindowRect( WinWindowFromID( hwndMainFrame, FID_CLIENT ), &rectlWin );

               /*
                * Subtract main window client height from title/action bar only
                * window height
                */
               lcy -= rectlWin.yTop - rectlWin.yBottom + 1;

               /*
                * Obtain main window menu rectangle
                */
               bSuccess &= WinQueryWindowRect( WinWindowFromID( hwndMainFrame, FID_MENU ), &rectlWin );

               /*
                * Subtract main window menu height greater than single-line
                * menu height from title/action bar only window height
                */
               lcy -= rectlWin.yTop - rectlWin.yBottom - WinQuerySysValue( HWND_DESKTOP, SV_CYMENU );

               /*
                * Initialize title/action bar only window width to width of
                * nominal-width border
                */
               lcx = 2 * WinQuerySysValue( HWND_DESKTOP, SV_CXBORDER );

               /*
                * Obtain main window enablement menu rectangle
                */
               bSuccess &= WinSendMsg( WinWindowFromID( hwndMainFrame, FID_MENU ), MM_QUERYITEMRECT, MPFROM2SHORT( IDM_ENABMENU, FALSE ), &rectlWin ) ? TRUE : FALSE;

               /*
                * Add main window enablement menu width to title/action bar
                * only window width
                */
               lcx += rectlWin.xRight - rectlWin.xLeft + 1;

               /*
                * Obtain main window customization menu rectangle
                */
               bSuccess &= WinSendMsg( WinWindowFromID( hwndMainFrame, FID_MENU ), MM_QUERYITEMRECT, MPFROM2SHORT( IDM_CUSTMENU, FALSE), &rectlWin ) ? TRUE : FALSE;

               /*
                * Add main window customization menu width to title/action bar
                * only window width
                */
               lcx += rectlWin.xRight - rectlWin.xLeft + 1;

               /*
                * Obtain main window control menu rectangle
                */
               bSuccess &= WinSendMsg( WinWindowFromID( hwndMainFrame, FID_MENU ), MM_QUERYITEMRECT, MPFROM2SHORT( IDM_CTLMENU, FALSE), &rectlWin ) ? TRUE : FALSE;

               /*
                * Add main window control menu width to title/action bar only
                * window width
                */
               lcx += rectlWin.xRight - rectlWin.xLeft + 1;

               /*
                * Obtain main window summarization menu rectangle
                */
               bSuccess &= WinSendMsg( WinWindowFromID( hwndMainFrame, FID_MENU ), MM_QUERYITEMRECT, MPFROM2SHORT( IDM_SUMMENU, FALSE), &rectlWin ) ? TRUE : FALSE;

               /*
                * Add main window summarization menu width to title/action bar
                * only window width
                */
               lcx += rectlWin.xRight - rectlWin.xLeft + 1;

               /*
                * Obtain main window options menu rectangle
                */
               bSuccess &= WinSendMsg( WinWindowFromID( hwndMainFrame, FID_MENU ), MM_QUERYITEMRECT, MPFROM2SHORT( IDM_OPTSMENU, FALSE), &rectlWin ) ? TRUE : FALSE;

               /*
                * Add main window options menu width to title/action bar only
                * window width
                */
               lcx += rectlWin.xRight - rectlWin.xLeft + 1;

               /*
                * Obtain main window help menu rectangle
                */
               bSuccess &= WinSendMsg( WinWindowFromID( hwndMainFrame, FID_MENU ), MM_QUERYITEMRECT, MPFROM2SHORT( IDM_HELPMENU, FALSE), &rectlWin ) ? TRUE : FALSE;

               /*
                * Add main window help menu width to title/action bar only
                * window width
                */
               lcx += rectlWin.xRight - rectlWin.xLeft + 1;

               /*
                * Calculate title/action bar only window coordinates if no
                * errors
                */
               if ( bSuccess )
               {
                  /*
                   * Center title/action bar only window along top of screen
                   */
                  if ( SHORT1FROMMP( mp1Win ) == IDM_OPTSTOP )
                  {
                     lx = ( swpDesktop.cx - lcx ) / 2;
                     ly = swpDesktop.cy - lcy;
                  }

                  /*
                   * Overlay title/action bar only window system menu on main
                   * window system menu
                   */
                  else if ( SHORT1FROMMP( mp1Win ) == IDM_OPTSCUR )
                  {
                     lx = swpWin.x;
                     ly = swpWin.y + swpWin.cy - lcy;
                  }

                  /*
                   * Center title/action bar only window along bottom of screen
                   */
                  else
                  {
                     lx = ( swpDesktop.cx - lcx ) / 2;
                     ly = 0L;
                  }

                  /*
                   * Position title/action bar only window, issue error if
                   * unsuccessful
                   */
                  if ( !WinSetWindowPos( hwndMainFrame, HWND_TOP, lx, ly, lcx, lcy, SWP_SIZE | SWP_MOVE | SWP_SHOW ) )
                  {
                     ErrorMessageBox( MB_ERROR, "Unable to set title/action bar only window position, position not changed" );
                  }
               }

               /*
                * Issue error if unable to calculate title/action bar only
                * window size
                */
               else
               {
                  ErrorMessageBox( MB_ERROR, "Unable to calculate title/action bar only window size, position not changed" );
               }

               break;

            /*
             * Process restore window position menu item
             */
            case IDM_OPTRSWP:
               /*
                * Size, position, and show main window if able to obtain window
                * position and size from operating system profile
                */
               if ( ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szWinPosKey, NULL, szString, sizeof( szString ) ) ) && ( sscanf( szString, "%ld,%ld", &lx, &ly ) == 2 )   ) &&
                    ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szWinSizKey, NULL, szString, sizeof( szString ) ) ) && ( sscanf( szString, "%ld,%ld", &lcx, &lcy ) == 2 ) ) )
               {
                  /*
                   * Size, position, and show main window if main window origin
                   * on desktop
                   */
                  if ( ( lx < swpDesktop.cx ) && ( ly + lcy <= swpDesktop.cy ) )
                  {
                     WinSetWindowPos( hwndMainFrame, HWND_TOP, lx, ly, lcx, lcy, SWP_SIZE | SWP_MOVE | SWP_SHOW );
                  }

                  /*
                   * Issue error if main window origin not on desktop
                   */
                  else
                  {
                     ErrorMessageBox( MB_ERROR, "Unable to restore main window position (window origin not on desktop)" );
                  }
               }

               /*
                * Issue error if unable to obtain main window position and size
                * from operating system profile
                */
               else
               {
                  ErrorMessageBox( MB_ERROR, "Unable to retrieve saved window position, position not restored" );
               }

               break;

            /*
             * Process save window position menu item
             */
            case IDM_OPTSSWP:
               /*
                * Obtain main window position, save main window position and
                * size in operating system profile if successful
                */
               if ( WinQueryWindowPos( hwndMainFrame, &swpWin ) )
               {
                  /*
                   * Save main window position and size in operating system
                   * profile, issue error and terminate if unsuccessful
                   */
                  if ( ( ( !sprintf( szString, "%ld,%ld", swpWin.x, swpWin.y ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szWinPosKey, szString ) )   ) ||
                       ( ( !sprintf( szString, "%ld,%ld", swpWin.cx, swpWin.cy ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szWinSizKey, szString ) ) ) )
                  {
                     Error( "Unable to store main window position" );
                  }
               }

               /*
                * Issue error if unable to obtain main window position
                */
               else
               {
                  ErrorMessageBox( MB_ERROR, "Unable to obtain main window position, position not saved" );
               }

               break;

            /*
             * Process open file summarization menu item
             */
            case IDM_SUMOPEN:
               /*
                * Create summarization thread, issue error and terminate if
                * unsuccessful
                */
               if ( DosCreateThread( &tidThread, SummarizationThread, 0L, CREATE_READY | STACK_SPARSE, 0x8000 ) )
               {
                  Error( "Unable to create summarization thread" );
               }

               break;
         }
         break;

      /*
       * Creating main window
       */
      case WM_CREATE:
         /*
          * Obtain old trace control option from operating system profile,
          * issue warning if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szCtlFlgKey, DEF_CTLFLG, szFlags, sizeof( szFlags ) ) != sizeof( DEF_CTLFLG ) ) || ( sscanf( szFlags, "0x%08lX", &ulCtrlFlg ) != 1 ) )
         {
            WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwndWin, QW_PARENT ), FID_MENU ), IDM_CTLPAUSE, FALSE );
            WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwndWin, QW_PARENT ), FID_MENU ), IDM_CTLRESUME, FALSE );

            ErrorMessageBox( MB_WARNING, "Unable to retrieve trace control option, tracing control disabled" );
         }

         /*
          * Toggle tracing control menu items enabled states
          */
         else
         {
            WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwndWin, QW_PARENT ), FID_MENU ), IDM_CTLPAUSE, FLAGCLR( ulCtrlFlg, C_PAUSE ) );
            WinEnableMenuItem( WinWindowFromID( WinQueryWindow( hwndWin, QW_PARENT ), FID_MENU ), IDM_CTLRESUME, FLAGSET( ulCtrlFlg, C_PAUSE ) );
         }

         /*
          * Obtain desktop window position
          */
         WinQueryWindowPos( HWND_DESKTOP, &swpDesktop );

         /*
          * Open main window device context, issue error and terminate if
          * unsuccessful
          */
         if ( !( hdcMain = WinOpenWindowDC( hwndWin ) ) )
         {
            Error( "Unable to open main window device context" );
         }

         /*
          * Initialize main window presentation page size to default dimensions
          */
         sizelWin.cx = sizelWin.cy = 0L;

         /*
          * Create main window presentation space, issue error and terminate if
          * unsuccessful
          */
         if ( ( hpsMain = GpiCreatePS( habMain, hdcMain, &sizelWin, GPIT_NORMAL | PU_PELS | GPIF_LONG | GPIA_ASSOC ) ) == GPI_ERROR )
         {
            Error( "Unable to create main window presentation space" );
         }

         /*
          * Initialize trace monitor number APIs and file length strings
          * lengths
          */
         ulMonNumAPI = strlen( szMonNumAPI );
         ulMonFilLen = strlen( szMonFilLen );

         /*
          * Determine maximum trace monitor text width
          */
         i = ( GpiQueryTextBox( hpsMain, ulMonNumAPI, szMonNumAPI, TXTBOX_COUNT, apointlTxtBox ) ? apointlTxtBox[ TXTBOX_BOTTOMRIGHT ].x - apointlTxtBox[ TXTBOX_BOTTOMLEFT ].x + 1L : 0L );
         j = ( GpiQueryTextBox( hpsMain, ulMonFilLen, szMonFilLen, TXTBOX_COUNT, apointlTxtBox ) ? apointlTxtBox[ TXTBOX_BOTTOMRIGHT ].x - apointlTxtBox[ TXTBOX_BOTTOMLEFT ].x + 1L : 0L );
         ulMonTxtWidth = ( ( i < j ) ? j : i );

         /*
          * Determine maximum trace monitor number width
          */
         ulMonNumWidth = ( GpiQueryTextBox( hpsMain, strlen( TXT_MAXULONG ), TXT_MAXULONG, TXTBOX_COUNT, apointlTxtBox ) ? apointlTxtBox[ TXTBOX_BOTTOMRIGHT ].x - apointlTxtBox[ TXTBOX_BOTTOMLEFT ].x + 1L : 0L );

         /*
          * Determine main window character height
          */
         ulMainCharHeight = ( GpiQueryFontMetrics( hpsMain, sizeof( FONTMETRICS ), &fontmetricsWin ) ? fontmetricsWin.lMaxBaselineExt : 0L );

         /*
          * Open main window bit map device context, issue error and terminate
          * if unsuccessful
          */
         if ( ( hdcBitmap = DevOpenDC( habMain, OD_MEMORY, "*", 0, NULL, hdcMain ) ) != DEV_ERROR )
         {
            /*
             * Initialize main window bit map presentation page size to default
             * dimensions
             */
            sizelWin.cx = sizelWin.cy = 0L;

            /*
             * Create main window bit map presentation space, issue error and
             * terminate if unsuccessful
             */
            if ( ( hpsBitmap = GpiCreatePS( habMain, hdcBitmap, &sizelWin, GPIT_NORMAL | PU_PELS | GPIF_LONG | GPIA_ASSOC ) ) != GPI_ERROR )
            {
               /*
                * Initialize main window bit map customized font attributes
                */
               fattrsWin.usRecordLength  = sizeof( FATTRS );
               fattrsWin.fsSelection     = FATTR_SEL_OUTLINE;
               fattrsWin.lMatch          = 0L;
               strcpy( fattrsWin.szFacename, "Helv" );
               fattrsWin.idRegistry      = 0;
               fattrsWin.usCodePage      = 437;
               fattrsWin.lMaxBaselineExt = 0L;
               fattrsWin.lAveCharWidth   = 0L;
               fattrsWin.fsType          = 0;
               fattrsWin.fsFontUse       = FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_TRANSFORMABLE;

               /*
                * Create and set main window bit map customized font, initialize main window
                * font characteristics if successful
                */
               if ( ( GpiCreateLogFont( hpsBitmap, NULL, ulBitmapFont, &fattrsWin ) == FONT_MATCH ) && ( GpiSetCharSet( hpsBitmap, ulBitmapFont ) ) )
               {
                  /*
                   * Set main window bit map font character shear
                   */
                  pointlWin.x = 1L;
                  pointlWin.y = 4L;
                  GpiSetCharShear( hpsBitmap, &pointlWin );

                  /*
                   * Initialize main window text
                   */
                  for ( i = 0; i < ( ( (ULONG)ceil( (double)NUM_MAINLINE * (double)INC_MAINCHAR / (double)LEN_FULLNAME ) ) + 1 ) * 2; i++ )
                  {
                     strcat( chMainText, VER_FULLNAME );
                     strcat( chMainText, " " );
                  }

                  /*
                   * Issue error and terminate if main window text buffer too small
                   */
                  if ( strlen( chMainText ) >= LEN_MAINTEXT )
                  {
                     Error( "Main window text buffer too small" );
                  }
               }

               /*
                * Indicate main window font not available if customized font creation
                * unsuccessful
                */
               else
               {
                  ulBitmapFont = 0L;
               }
            }
         }

         break;

      /*
       * Destroying main window
       */
      case WM_DESTROY:
         /*
          * Delete main window customized font if font available
          */
         if ( ulBitmapFont )
         {
            GpiDeleteSetId( hpsBitmap, ulBitmapFont );
         }

         /*
          * Destroy main window presentation space
          */
         GpiDestroyPS( hpsMain );

         break;

      /*
       * Painting main window
       */
      case WM_PAINT:
         /*
          * Prepare main window presentation space for painting, issue error
          * and terminate if unsuccessful
          */
         if ( !WinBeginPaint( hwndWin, hpsMain, &rectlWin ) )
         {
            Error( "Unable to prepare main window for painting" );
         }

         /*
          * Paint main window background text if customized font available
          */
         if ( ulBitmapFont )
         {
            /*
             * Set main window color
             */
            GpiSetColor( hpsMain, CLR_PALEGRAY );

            /*
             * Draw background bit map
             */
            GpiBitBlt( hpsMain, hpsBitmap, sizeof( apointlBitBlt ) / sizeof( POINTL ), apointlBitBlt, ROP_SRCCOPY, 0 );
         }

         /*
          * Paint main window default color if customized font not available
          */
         else
         {
            /*
             * Paint main window client background
             */
            WinFillRect( hpsMain, &rectlWin, SYSCLR_WINDOW );
         }

         /*
          * Paint trace monitoring information if trace monitor queue and
          * information exists
          */
         if ( ( hqueueMon ) && ( pMonFirst ) )
         {
            /*
             * Draw trace monitor information for each process being monitored
             */
            for ( i = ulMainHeight - ulMainCharHeight, pMonInfo = pMonFirst
                ; pMonInfo != NULL
                ; i -= ulMainCharHeight, pMonInfo = pMonInfo->pNext
                )
            {
               /*
                * Set main window color
                */
               GpiSetColor( hpsMain, CLR_RED );

               /*
                * Draw executable and log file names and identifer
                */
               pointlWin.x = 10L;
               pointlWin.y = i;
               GpiCharStringAt( hpsMain, &pointlWin, sprintf( szString, "%s (%s, PID %04lX)", pMonInfo->szLogName, strrchr( pMonInfo->szExeName, '\\' ) + 1, pMonInfo->ulPID ), szString );
               i -= ulMainCharHeight;

               /*
                * Set main window color
                */
               GpiSetColor( hpsMain, CLR_BLUE );

               /*
                * Draw number APIs if monitoring APIs
                */
               if ( FLAGCLR( pMonInfo->ulFlags, fMONAPI_OFF ) )
               {
                  /*
                   * Draw number APIs
                   */
                  pointlWin.x = 10L;
                  pointlWin.y = i;
                  GpiCharStringAt( hpsMain, &pointlWin, ulMonNumAPI, szMonNumAPI );
                  pointlWin.x = 10L + ulMonTxtWidth;
                  GpiMove( hpsMain, &pointlWin );
                  GpiCharString( hpsMain, sprintf( szString, "%d", pMonInfo->ulNumAPI ), szString );
                  i -= ulMainCharHeight;
               }

               /*
                * Draw log file length if monitoring length
                */
               if ( FLAGCLR( pMonInfo->ulFlags, fMONLEN_OFF ) )
               {
                  /*
                   * Draw log file length
                   */
                  pointlWin.x = 10L;
                  pointlWin.y = i;
                  GpiCharStringAt( hpsMain, &pointlWin, ulMonFilLen, szMonFilLen );
                  pointlWin.x = 10L + ulMonTxtWidth;
                  GpiMove( hpsMain, &pointlWin );
                  GpiCharString( hpsMain, sprintf( szString, "%dKB", pMonInfo->ulFilLen >> 10L ), szString );
                  i -= ulMainCharHeight;
               }
            }
         }

         /*
          * Release main window presentation space from painting
          */
         WinEndPaint( hpsMain );

         break;

      /*
       * Sizing main window
       */
      case WM_SIZE:
         /*
          * Obtain main window rectangle, issue error and terminate if
          * unsuccessful
          */
         if ( !WinQueryWindowRect( hwndWin, &rectlWin ) )
         {
            Error( "Unable to obtain main window rectangle" );
         }

         /*
          * Calculate main window width and height
          */
         ulMainWidth = rectlWin.xRight - rectlWin.xLeft;
         ulMainHeight = rectlWin.yTop - rectlWin.yBottom;

         /*
          * Calculate main window attributes if customized font available and
          * window not minimized
          */
         if ( ( ulBitmapFont ) && ( ulMainHeight ) && ( ulMainWidth ) )
         {
            /*
             * Obtain current system date and time if calculation time not yet
             * calculated
             */
            if ( !lCalcTime )
            {
               DosGetDateTime( &dtBegin );
            }

            /*
             * Calculate main window character angle, diagonal, and character
             * increment
             */
            dWinAngle = atan( (double)ulMainHeight / (double)ulMainWidth );
            ulWinDiag = (ULONG)( (double)ulMainHeight / sin( dWinAngle ) );
            ulWinIncr = ulWinDiag / ( LEN_FULLNAME + 2 );

            /*
             * Calculate main window horizontal and vertical character
             * increment if main window angle less than 45 degrees
             */
            if ( dWinAngle < ANGLE45 )
            {
               ulWinXIncr = (ULONG)( (double)ulWinIncr * 1.5 * sin( dWinAngle ) + ( (double)ulWinIncr * tan( dWinAngle ) ) );
               ulWinYIncr = (ULONG)( (double)ulWinIncr * 1.5 * sin( dWinAngle ) + ( (double)ulWinIncr * tan( dWinAngle ) ) );
            }

            /*
             * Calculate main window horizontal and vertical character
             * increment if main window angle greater than 45 degrees
             */
            else
            {
               ulWinXIncr = (ULONG)( (double)ulWinIncr * 1.5 * cos( dWinAngle ) + ( (double)ulWinIncr * tan( ANGLE90 - dWinAngle ) ) );
               ulWinYIncr = (ULONG)( (double)ulWinIncr * 1.5 * cos( dWinAngle ) + ( (double)ulWinIncr * tan( ANGLE90 - dWinAngle ) ) );
            }

            /*
             * Delete main window bit map if bit map exists
             */
            if ( hbitmapMain )
            {
               GpiDeleteBitmap( hbitmapMain );
            }

            /*
             * Initialize bit map header information
             */
            bitmapInfoHeader.cbFix        = sizeof( BITMAPINFOHEADER );
            bitmapInfoHeader.cx           = (USHORT)ulMainWidth;
            bitmapInfoHeader.cy           = (USHORT)ulMainHeight;
            bitmapInfoHeader.cPlanes      = 1;
            bitmapInfoHeader.cBitCount    = 1;

            /*
             * Create main window bit map, draw main window background text
             * into bit map if successful
             */
            if ( ( ( hbitmapMain = GpiCreateBitmap( hpsBitmap, (PBITMAPINFOHEADER2)&bitmapInfoHeader, 0, NULL, NULL ) ) != GPI_ERROR ) && ( GpiSetBitmap( hpsBitmap, hbitmapMain ) != HBM_ERROR ) )
            {
               /*
                * Initialize bit map to all 0's
                */
               apointlBitBlt[ 1 ].x = ulMainWidth;
               apointlBitBlt[ 1 ].y = ulMainHeight;
               GpiBitBlt( hpsBitmap, hpsBitmap, sizeof( apointlBitBlt ) / sizeof( POINTL ), apointlBitBlt, ROP_ZERO, 0 );

               /*
                * Set bit map font character angle
                */
               gradientlWin.x = ulMainWidth;
               gradientlWin.y = ulMainHeight;
               GpiSetCharAngle( hpsBitmap, &gradientlWin );

               /*
                * Set bit map font character box
                */
               sizefWin.cx = MAKEFIXED( ulWinIncr * 2, 0L );
               sizefWin.cy = MAKEFIXED( (ULONG)( (double)ulWinIncr * 4.0 * tan( dWinAngle < ANGLE45 ? dWinAngle : ANGLE90 - dWinAngle ) ), 0L );
               GpiSetCharBox( hpsBitmap, &sizefWin );

               /*
                * Set bit map color (CLR_WHITE will draw 1's)
                */
               GpiSetColor( hpsBitmap, CLR_WHITE );

               /*
                * Draw main window background text into bit map
                */
               for ( i = 0, lx = ulWinXIncr, ly = 0L, pch = &chMainText[ ( ( (ULONG)ceil( (double)NUM_MAINLINE * (double)INC_MAINCHAR / (double)LEN_MAINTEXT ) ) + 1 ) * ( LEN_FULLNAME + 1 ) ]; i < NUM_MAINLINE; i++ )
               {
                  pointlWin.x = lx - ( i * ulWinXIncr );
                  pointlWin.y = ly + ( i * ulWinYIncr );
                  GpiCharStringAt( hpsBitmap, &pointlWin, LEN_FULLNAME + ( i * 2 ), pch + ( i * INC_MAINCHAR ) );
                  pointlWin.x = lx + ( i * ulWinXIncr );
                  pointlWin.y = ly - ( i * ulWinYIncr );
                  GpiCharStringAt( hpsBitmap, &pointlWin, LEN_FULLNAME + ( i * 2 ), pch - ( i * INC_MAINCHAR ) );
               }
            }

            /*
             * Determine calculation time if not yet calculated
             */
            if ( !lCalcTime )
            {
               /*
                * Obtain current system date and time if calculation time not yet
                * calculated
                */
               DosGetDateTime( &dtEnd );

               /*
                * Calculate calculation time in hundredths of seconds
                */
               lCalcTime  = ( ( ( dtBegin.hours == 23 ) && ( dtEnd.hours == 0 ) ) ? 24 : dtEnd.hours ) * HUNPERHR;
               lCalcTime += dtEnd.minutes * HUNPERMIN;
               lCalcTime += dtEnd.seconds * HUNPERSEC;
               lCalcTime += dtEnd.hundredths;
               lCalcTime -= dtBegin.hours   * HUNPERHR;
               lCalcTime -= dtBegin.minutes * HUNPERMIN;
               lCalcTime -= dtBegin.seconds * HUNPERSEC;
               lCalcTime -= dtBegin.hundredths;

               /*
                * Disable main window background text if calculation time
                * greater than maximum
                */
               if ( lCalcTime > MAX_CALCTIME )
               {
                  /*
                   * Delete main window customized font if font available
                   */
                  if ( ulBitmapFont )
                  {
                     GpiDeleteSetId( hpsBitmap, ulBitmapFont );
                     ulBitmapFont = 0L;
                  }

                  /*
                   * Delete main window bit map if bit map exists
                   */
                  if ( hbitmapMain )
                  {
                     GpiDeleteBitmap( hbitmapMain );
                  }
               }
            }
         }

         /*
          * Recalculate trace monitor information rectangles if trace monitor
          * queue exists
          */
         if ( hqueueMon )
         {
            CalcMonInfoRects( );
         }

         /*
          * Repaint main window
          */
         WinInvalidateRect( hwndWin, NULL, FALSE );

         break;

      /*
       * Other window processing
       */
      default:
         /*
          * Perform default window processing
          */
         return WinDefWindowProc( hwndWin, ulMsg, mp1Win, mp2Win );
   }

   /*
    * Indicate continue main window message processing
    */
   return (MRESULT)FALSE;

}  /* MainWindowProc() */

/******************************************************************************/
/*                                                                            */
/* MonitorExceptionHandler()                                                  */
/*                                                                            */
/******************************************************************************/

ULONG APIENTRY MonitorExceptionHandler
                                  ( PREPREC pRep /* Exception information     */
                                  , PREGREC pReg /* Registration record       */
                                  , PCTXREC pCtx /* Exception context         */
                                  , PVOID   p    /* Not used                  */
                                  )
{

   /*
    * Clean up if being removed from exception handler chain
    */
   if ( FLAGSET( pRep->fHandlerFlags, EH_UNWINDING ) )
   {
      /*
       * Release access to system semaphore
       */
      while ( !DosReleaseMutexSem( hmtxSys ) );
   }

   /*
    * Indicate ignore process terminate exceptions
    */
   else if ( ( pRep->ExceptionNum == XCPT_PROCESS_TERMINATE ) || ( pRep->ExceptionNum == XCPT_ASYNC_PROCESS_TERMINATE ) )
   {
      return XCPT_CONTINUE_SEARCH;
   }

   /*
    * Issue error and terminate if fatal exception generated
    */
   else if ( ( pRep->ExceptionNum & XCPT_SEVERITY_CODE ) == XCPT_FATAL_EXCEPTION )
   {
#ifdef DEBUG
      Error( "Enablement thread fatal exception at 0x%08lX: 0x%08lX", pRep->ExceptionAddress, pRep->ExceptionNum );
#else
      Error( "Enablement thread fatal exception: 0x%08lX", pRep->ExceptionNum );
#endif
   }

   /*
    * Indicate ignore exception
    */
   return XCPT_CONTINUE_SEARCH;

}  /* MonitorExceptionHandler() */

/******************************************************************************/
/*                                                                            */
/* MonitorThread()                                                            */
/*                                                                            */
/******************************************************************************/

VOID APIENTRY MonitorThread( ULONG ulParam )     /* Thread parameter          */
{

   BYTE        bPriority;                        /* Queue data priority       */
   HAB         habMon;                           /* Mon thread anchor block   */
   HMQ         hmqMon;                           /* Mon thread message queue  */
   PVOID       pData;                            /* Queue data                */
   PMONINFO    pMonInfo;                         /* Trace monitor information */
   REGREC      sRegRec;                          /* Registration record       */
   REQUESTDATA sReqData;                         /* Queue request data        */
   ULONG       ulDataLen;                        /* Queue data length         */

   /*
    * Register exception handler
    */
   sRegRec.ExceptionHandler = MonitorExceptionHandler;
   DosSetExceptionHandler( &sRegRec );

   /*
    * Initialize Presentation Manager facilities, continue trace monitoring if
    * successful
    */
   if ( ( habMon = WinInitialize( 0L ) ) != 0L )
   {
      /*
       * Create trace monitor thread message queue, continue trace monitoring
       * if successful
       */
      if ( ( hmqMon = WinCreateMsgQueue( habMon, 0L ) ) != 0L )
      {
         /*
          * Process trace monitor queue while queue data exists
          */
         while ( !DosReadQueue( hqueueMon, &sReqData, &ulDataLen, &pData, 0L, DCWW_WAIT, &bPriority, 0L ) )
         {
            /*
             * Process trace monitor queue data based on request data
             */
            switch ( sReqData.ulData )
            {
               /*
                * Initialize process requesting to be monitored
                */
               case TRCMON_PRCINI:
                  if ( AddMonInfo( sReqData.pid, pData ) )
                  {
                     DosFreeMem( pData );
                     CalcMonInfoRects( );
                     WinInvalidateRect( hwndMainClient, NULL, FALSE );
                  }
                  break;

               /*
                * Update number APIs information
                */
               case TRCMON_NUMAPI:
                  if ( ( pMonInfo = GetMonInfo( sReqData.pid ) ) != NULL )
                  {
                     pMonInfo->ulNumAPI = (ULONG)pData;
                     WinInvalidateRect( hwndMainClient, &pMonInfo->rectlNumAPI, FALSE );
                  }
                  break;

               /*
                * Update log file length information
                */
               case TRCMON_FILLEN:
                  if ( ( pMonInfo = GetMonInfo( sReqData.pid ) ) != NULL )
                  {
                     pMonInfo->ulFilLen = (ULONG)pData;
                     WinInvalidateRect( hwndMainClient, &pMonInfo->rectlFilLen, FALSE );
                  }
                  break;

               /*
                * Terminate process requesting to no longer be monitored
                */
               case TRCMON_PRCTRM:
                  if ( DelMonInfo( sReqData.pid ) )
                  {
                     CalcMonInfoRects( );
                     WinInvalidateRect( hwndMainClient, NULL, FALSE );
                  }
                  break;

               /*
                * Quit trace monitoring
                */
               case TRCMON_QUIT:
                  while ( pMonFirst )
                  {
                     DelMonInfo( pMonFirst->ulPID );
                  }
                  WinInvalidateRect( hwndMainClient, NULL, FALSE );
                  DosExit( EXIT_THREAD, 0L );
                  break;
            }
         }

         /*
          * Destroy trace monitor thread message queue
          */
         WinDestroyMsgQueue( hmqMon );
      }

      /*
       * Issue error and terminate if trace monitor thread message queue
       * creation unsuccessful
       */
      else
      {
         Error( "Unable to create trace monitor thread message queue" );
      }

      /*
       * Terminate Presentation Manager facilities
       */
      WinTerminate( habMon );
   }

   /*
    * Unregister exception handler
    */
   DosUnsetExceptionHandler( &sRegRec );

}  /* MonitorThread() */

/******************************************************************************/
/*                                                                            */
/* OptsAPIDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY OptsAPIDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   LONG lColor;                                  /* Foreground color index    */
   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify API monitoring dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying API monitoring dialog
       */
      case WM_COMMAND:
         /*
          * Identify API monitoring dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_APICANCEL:
               /*
                * Verify cancellation if new API monitoring different from
                * original
                */
               if ( ulNewMonAPI != ulOldMonAPI )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "API Monitoring", "Value has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close API monitoring dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_APIDEFAULT:
               /*
                * Reset new API monitoring to default monitoring if different
                */
               if ( ulNewMonAPI != DEF_MONAPIInt )
               {
                  /*
                   * Set new API monitoring to default
                   */
                  ulNewMonAPI = DEF_MONAPIInt;

                  /*
                   * Set value spin button to default API monitoring
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonAPI ), MPVOID );

                  /*
                   * Set value radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_APIHELP:
               /*
                * Display API monitoring help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_OPTSAPI ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_APIOK:
               /*
                * Save new API monitoring value in operating system profile,
                * issue error and terminate if unsuccessful
                */
               if ( ( ulNewMonAPI != ulOldMonAPI ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewMonAPI ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szMonAPIKey, szFlags ) ) ) )
               {
                  Error( "Unable to store API monitoring value" );
               }

               /*
                * Close API monitoring dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_APIUNDO:
               /*
                * Reset new API monitoring to original monitoring if different
                */
               if ( ulNewMonAPI != ulOldMonAPI )
               {
                  /*
                   * Set appropriate windows if no original API monitoring
                   */
                  if ( !( ulNewMonAPI = ulOldMonAPI ) )
                  {
                     /*
                      * Set value spin button to default API monitoring
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( DEF_MONAPIInt ), MPVOID );

                     /*
                      * Set off radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }

                  /*
                   * Set appropriate windows if original API monitoring
                   */
                  else
                  {
                     /*
                      * Set value spin button to original API monitoring
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonAPI ), MPVOID );

                     /*
                      * Set value radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying API monitoring dialog
       */
      case WM_CONTROL:
         /*
          * Identify API monitoring dialog control processing based on button
          * control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process off radio button
                   */
                  case DLG_APIOFFRAD:
                     /*
                      * Set new API monitoring to none
                      */
                     ulNewMonAPI = 0L;

                     /*
                      * Disable value spin button window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), FALSE );

                     break;

                  /*
                   * Process value radio button
                   */
                  case DLG_APIVALRAD:
                     /*
                      * Obtain new API monitoring from value spin button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_QUERYVALUE, MPFROMP( &ulNewMonAPI ), MPFROM2SHORT( 0, SPBQ_DONOTUPDATE ) );

                     /*
                      * Enable value spin button window
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), TRUE );

                     break;
               }
               break;

            /*
             * Spinning value spin button down
             */
            case SPBN_DOWNARROW:
               /*
                * Set new API monitoring to maximum if current monitoring under
                * minimum
                */
               if ( ulNewMonAPI <= MIN_MONAPI )
               {
                  ulNewMonAPI = MAX_MONAPI;
               }

               /*
                * Decrement new API monitoring by appropriate amount
                */
               else
               {
                  ulNewMonAPI -= INC_MONAPI;
               }

               /*
                * Set value spin button to decremented API monitoring
                */
               WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonAPI ), MPVOID );

               break;

            /*
             * Spinning value spin button up
             */
            case SPBN_UPARROW:
               /*
                * Set new API monitoring to minimum if current monitoring over
                * maximum
                */
               if ( ulNewMonAPI >= MAX_MONAPI )
               {
                  ulNewMonAPI = MIN_MONAPI;
               }

               /*
                * Increment new API monitoring by appropriate amount
                */
               else
               {
                  ulNewMonAPI += INC_MONAPI;
               }

               /*
                * Set value spin button to incremented API monitoring
                */
               WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonAPI ), MPVOID );

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing API monitoring dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old API monitoring value from operating system profile,
          * issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szMonAPIKey, DEF_MONAPI, szFlags, sizeof( szFlags ) ) != sizeof( DEF_MONAPI ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldMonAPI ) != 1 ) )
         {
            Error( "Unable to retrieve API monitoring value" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_APIOFFTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_APIVALTXT1 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_APIVALTXT2 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set value spin button upper and lower limits
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( MAX_MONAPI + INC_MONAPI ), MPFROMLONG( MIN_MONAPI ) );

         /*
          * Set appropriate windows if no original API monitoring
          */
         if ( !( ulNewMonAPI = ulOldMonAPI ) )
         {
            /*
             * Set value spin button to default API monitoring
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( DEF_MONAPIInt ), MPVOID );

            /*
             * Set off radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Set appropriate windows if original API monitoring
          */
         else
         {
            /*
             * Set value spin button to initial API monitoring
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonAPI ), MPVOID );

            /*
             * Set value radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_APIVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Give API monitoring dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* OptsAPIDlgProc() */

/******************************************************************************/
/*                                                                            */
/* File length monitoring dialog spin button information                      */
/*                                                                            */
/******************************************************************************/

#define DEF_LENSBIDX 0

SBINFO LenSBInfo[] =
{
   { "KB", MIN_MONLEN / KILOBYTE, MAX_MONLEN / KILOBYTE, INC_MONLEN / KILOBYTE, KILOBYTE },
   { "MB",                     1, MAX_MONLEN / MEGABYTE,                     1, MEGABYTE }
};

ULONG ulLenSBInfo = sizeof( LenSBInfo ) / sizeof( SBINFO );

/******************************************************************************/
/*                                                                            */
/* OptsLenDlgProc()                                                           */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY OptsLenDlgProc( HWND   hwndDlg  /* Dialog handle             */
                               , ULONG  ulMsg    /* Message number            */
                               , MPARAM mp1Dlg   /* Message parameter 1       */
                               , MPARAM mp2Dlg   /* Message parameter 2       */
                               )
{

   PCHAR apsz[sizeof(LenSBInfo)/sizeof(SBINFO)]; /* String array              */
   LONG lColor;                                  /* Foreground color index    */
   CHAR szFlags[ 16 ];                           /* Custom options string     */

   /*
    * Identify file length monitoring dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Control notifying file length monitoring dialog
       */
      case WM_COMMAND:
         /*
          * Identify file length monitoring dialog control processing based on
          * command value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process cancel push button
             */
            case DLG_LENCANCEL:
               /*
                * Verify cancellation if new file length monitoring different
                * from original
                */
               if ( ulNewMonLen * LenSBInfo[ sCurSBIdx ].ulMultiplier != ulOldMonLen )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( "File Length Monitoring", "Value has changed for this attribute, cancel?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close file length monitoring dialog indicating cancel
                */
               WinDismissDlg( hwndDlg, DID_CANCEL );

               break;

            /*
             * Process default push button
             */
            case DLG_LENDEFAULT:
               /*
                * Reset new file length monitoring to default monitoring if
                * different
                */
               if ( ulNewMonLen != DEF_MONLENInt )
               {
                  /*
                   * Set new file length monitoring to default
                   */
                  ulNewMonLen = DEF_MONLENInt;

                  /*
                   * Set type spin button to default if current spin button
                   * index not default
                   */
                  if ( sCurSBIdx != DEF_LENSBIDX )
                  {
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_LENSBIDX ), MPFROMSHORT( TRUE ) );
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMaxValue + LenSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMinValue ) );
                  }

                  /*
                   * Set value spin button to default file length monitoring
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonLen >> 10L ), MPVOID );

                  /*
                   * Set value radio button
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
               }

               break;

            /*
             * Process help push button
             */
            case DLG_LENHELP:
               /*
                * Display file length monitoring help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_OPTSLEN ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_LENOK:
               /*
                * Save new file length monitoring value in operating system
                * profile, issue error and terminate if unsuccessful
                */
               if ( ( ulNewMonLen * LenSBInfo[ sCurSBIdx ].ulMultiplier != ulOldMonLen                                                                                                         ) &&
                    ( ( !sprintf( szFlags, "0x%08lX", ulNewMonLen * LenSBInfo[ sCurSBIdx ].ulMultiplier ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szMonLenKey, szFlags ) ) ) )
               {
                  Error( "Unable to store file length monitoring value" );
               }

               /*
                * Close file length monitoring dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               break;

            /*
             * Process undo push button
             */
            case DLG_LENUNDO:
               /*
                * Reset new file length monitoring to original monitoring if
                * different
                */
               if ( ulNewMonLen != ulOldMonLen )
               {
                  /*
                   * Set appropriate windows if no original file length
                   * monitoring
                   */
                  if ( !( ulNewMonLen = ulOldMonLen ) )
                  {
                     /*
                      * Set type spin button to default if current spin button
                      * index not default
                      */
                     if ( sCurSBIdx != DEF_LENSBIDX )
                     {
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_LENSBIDX ), MPFROMSHORT( TRUE ) );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMaxValue + LenSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMinValue ) );
                     }

                     /*
                      * Set value spin button to default file length monitoring
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( DEF_MONLENInt >> 10L ), MPVOID );

                     /*
                      * Set off radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }

                  /*
                   * Set appropriate windows if original file length monitoring
                   */
                  else
                  {
                     /*
                      * Set type spin button to default if current spin button
                      * index not default
                      */
                     if ( sCurSBIdx != DEF_LENSBIDX )
                     {
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), LM_SELECTITEM, MPFROMSHORT( sCurSBIdx = DEF_LENSBIDX ), MPFROMSHORT( TRUE ) );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( sCurSBIdx ), MPVOID );
                        WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMaxValue + LenSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMinValue ) );
                     }

                     /*
                      * Set value spin button to original file length
                      * monitoring
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonLen >> 10L ), MPVOID );

                     /*
                      * Set value radio button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
                  }
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying file length monitoring dialog
       */
      case WM_CONTROL:
         /*
          * Identify file length monitoring dialog control processing based on
          * button control
          */
         switch ( SHORT2FROMMP( mp1Dlg ) )
         {
            /*
             * Clicking radio button
             */
            case BN_CLICKED:
               /*
                * Identify radio button control processing based on command
                * value
                */
               switch ( SHORT1FROMMP( mp1Dlg ) )
               {
                  /*
                   * Process off radio button
                   */
                  case DLG_LENOFFRAD:
                     /*
                      * Set new file length monitoring to none
                      */
                     ulNewMonLen = 0L;

                     /*
                      * Disable value and type spin button windows
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), FALSE );
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), FALSE );

                     break;

                  /*
                   * Process value radio button
                   */
                  case DLG_LENVALRAD:
                     /*
                      * Obtain new file length monitoring from value spin
                      * button
                      */
                     WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_QUERYVALUE, MPFROMP( &ulNewMonLen ), MPFROM2SHORT( 0, SPBQ_DONOTUPDATE ) );

                     /*
                      * Enable value and type spin button windows
                      */
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), TRUE );
                     WinEnableWindow( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), TRUE );

                     break;
               }
               break;

            /*
             * Spinning spin button down
             */
            case SPBN_DOWNARROW:
               /*
                * Spinning value spin button down
                */
               if ( SHORT1FROMMP( mp1Dlg ) == DLG_LENVALSPIN )
               {
                  /*
                   * Set new file length monitoring to maximum if current
                   * monitoring under minimum
                   */
                  if ( ulNewMonLen <= LenSBInfo[ sCurSBIdx ].ulMinValue )
                  {
                     ulNewMonLen = LenSBInfo[ sCurSBIdx ].ulMaxValue;
                  }

                  /*
                   * Decrement new file length monitoring by appropriate amount
                   */
                  else
                  {
                     ulNewMonLen -= LenSBInfo[ sCurSBIdx ].ulIncrement;
                  }

                  /*
                   * Set value spin button to decremented file length monitoring
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonLen ), MPVOID );
               }

               /*
                * Spinning type spin button down
                */
               else if ( SHORT1FROMMP( mp1Dlg ) == DLG_LENTYPSPIN )
               {
                  /*
                   * Decrement current spin button index, reset to maximum
                   * index if less than zero
                   */
                  if ( --sCurSBIdx < 0 )
                  {
                     sCurSBIdx = ulLenSBInfo - 1;
                  }

                  /*
                   * Adjust new file length monitoring by appropriate amount
                   */
                  switch ( sCurSBIdx )
                  {
                     /*
                      * New file length monitoring in kilobytes
                      */
                     case 0:
                        ulNewMonLen <<= 10L;
                        break;

                     /*
                      * New file length monitoring in megabytes
                      */
                     case 1:
                        if ( ulNewMonLen & ( KILOBYTE - 1 ) )
                        {
                           ulNewMonLen += KILOBYTE - 1;
                        }
                        ulNewMonLen >>= 10L;
                        break;
                  }

                  /*
                   * Set value spin button limits
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMaxValue + LenSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMinValue ) );

                  /*
                   * Set value spin button to adjusted file length monitoring
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonLen ), MPVOID );
               }

               break;

            /*
             * Spinning spin button up
             */
            case SPBN_UPARROW:
               /*
                * Spinning value spin button up
                */
               if ( SHORT1FROMMP( mp1Dlg ) == DLG_LENVALSPIN )
               {
                  /*
                   * Set new file length monitoring to minimum if current
                   * monitoring over maximum
                   */
                  if ( ulNewMonLen >= LenSBInfo[ sCurSBIdx ].ulMaxValue )
                  {
                     ulNewMonLen = LenSBInfo[ sCurSBIdx ].ulMinValue;
                  }

                  /*
                   * Increment new file length monitoring by appropriate amount
                   */
                  else
                  {
                     ulNewMonLen += LenSBInfo[ sCurSBIdx ].ulIncrement;
                  }

                  /*
                   * Set value spin button to incremented file length monitoring
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonLen ), MPVOID );
               }

               /*
                * Spinning type spin button up
                */
               else if ( SHORT1FROMMP( mp1Dlg ) == DLG_LENTYPSPIN )
               {
                  /*
                   * Increment current spin button index, reset to minimum
                   * index if greater than maximum
                   */
                  if ( ++sCurSBIdx >= ulLenSBInfo )
                  {
                     sCurSBIdx = 0;
                  }

                  /*
                   * Adjust new file length monitoring by appropriate amount
                   */
                  switch ( sCurSBIdx )
                  {
                     /*
                      * New file length monitoring in kilobytes
                      */
                     case 0:
                        ulNewMonLen <<= 10L;
                        break;

                     /*
                      * New file length monitoring in megabytes
                      */
                     case 1:
                        if ( ulNewMonLen & ( KILOBYTE - 1 ) )
                        {
                           ulNewMonLen += KILOBYTE - 1;
                        }
                        ulNewMonLen >>= 10L;
                        break;
                  }

                  /*
                   * Set value spin button limits
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMaxValue + LenSBInfo[ sCurSBIdx ].ulIncrement ), MPFROMLONG( LenSBInfo[ sCurSBIdx ].ulMinValue ) );

                  /*
                   * Set value spin button to adjusted file length monitoring
                   */
                  WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonLen ), MPVOID );
               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing file length monitoring dialog
       */
      case WM_INITDLG:
         /*
          * Obtain old file length monitoring value from operating system
          * profile, issue error and terminate if unsuccessful
          */
         if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szMonLenKey, DEF_MONLEN, szFlags, sizeof( szFlags ) ) != sizeof( DEF_MONLEN ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldMonLen ) != 1 ) )
         {
            Error( "Unable to retrieve file length monitoring value" );
         }

         /*
          * Set text presentation parameters, issue error and terminate if
          * unsuccessful
          */
         lColor = CLR_BLACK;
         if ( ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_LENOFFTXT  ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) ||
              ( !WinSetPresParam( WinWindowFromID( hwndDlg, DLG_LENVALTXT1 ), PP_FOREGROUNDCOLORINDEX, sizeof( ULONG ), &lColor ) ) )
         {
            Error( "Unable to set text presentation parameters" );
         }

         /*
          * Set value spin button upper and lower limits
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETLIMITS, MPFROMLONG( LenSBInfo[ DEF_LENSBIDX ].ulMaxValue + LenSBInfo[ DEF_LENSBIDX ].ulIncrement ), MPFROMLONG( LenSBInfo[ DEF_LENSBIDX ].ulMinValue ) );

         /*
          * Initialize string array
          */
         for ( sCurSBIdx = 0; sCurSBIdx < ulLenSBInfo; sCurSBIdx++ )
         {
            apsz[ sCurSBIdx ] = LenSBInfo[ sCurSBIdx ].szName;
         }

         /*
          * Set type spin button values
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), SPBM_SETARRAY, MPFROMP( apsz ), MPFROMSHORT( ulLenSBInfo ) );

         /*
          * Set type spin button current value
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENTYPSPIN ), SPBM_SETCURRENTVALUE, MPFROMSHORT( sCurSBIdx = DEF_LENSBIDX ), MPVOID );

         /*
          * Set appropriate windows if no original file length monitoring
          */
         if ( !( ulNewMonLen = ulOldMonLen ) )
         {
            /*
             * Set value spin button to default file length monitoring
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( DEF_MONLENInt >> 10L ), MPVOID );

            /*
             * Set off radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENOFFRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Set appropriate windows if original file length monitoring
          */
         else
         {
            /*
             * Set value spin button to initial file length monitoring
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALSPIN ), SPBM_SETCURRENTVALUE, MPFROMLONG( ulNewMonLen >> 10L ), MPVOID );

            /*
             * Set value radio button
             */
            WinSendMsg( WinWindowFromID( hwndDlg, DLG_LENVALRAD ), BM_CLICK, MPFROMSHORT( TRUE ), MPVOID );
         }

         /*
          * Give file length monitoring dialog focus
          */
         WinSetFocus( HWND_DESKTOP, hwndDlg );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* OptsLenDlgProc() */

/******************************************************************************/
/*                                                                            */
/* QueryMessageBox()                                                          */
/*                                                                            */
/******************************************************************************/

BOOL QueryMessageBox( PCHAR szTitle              /* Query dialog title        */
                    , PCHAR szText               /* Query dialog text         */
                    )
{

   /*
    * Query user, return indication based on push button selected
    */
   return WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, szText, szTitle, ID_MAINMSGBOX, MB_APPLMODAL | MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 | MB_MOVEABLE ) == MBID_NO ? FALSE : TRUE;

}  /* QueryMessageBox() */

/******************************************************************************/
/*                                                                            */
/* SetCustDosDlgCheck()                                                       */
/*                                                                            */
/******************************************************************************/

VOID SetCustDosDlgCheck( HWND hwndDlg )          /* Dialog window             */
{

   /*
    * Set DOSCALLS APIs customization dialog check buttons
    */
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSDEVCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_DEV  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSFILECHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_FILE ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSINFOCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_INFO ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSMEMCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_MEM  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSMISCCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_MISC ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSMODCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_MOD  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSMSGCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_MSG  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSMVDMCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_MVDM ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSNLSCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_NLS  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSPIPECHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_PIPE ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSPRFCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_PRF  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSPROCCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_PROC ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSPROFCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_PROF ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSRESCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_RES  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSSEMCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_SEM  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSSESCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_SES  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSSIGCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_SIG  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSSMPCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_SMP  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSTIMECHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_TIME ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_DOSXCPTCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewDosGrp, D_XCPT ) ? 1 : 0 ), MPVOID );

}  /* SetCustDosDlgCheck() */

/******************************************************************************/
/*                                                                            */
/* SetCustGpiDlgCheck()                                                       */
/*                                                                            */
/******************************************************************************/

VOID SetCustGpiDlgCheck( HWND hwndDlg )          /* Dialog window             */
{

   /*
    * Set PMGPI APIs customization dialog check buttons
    */
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIBITCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_BIT  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPICORRCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_CORR ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPICTRLCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_CTRL ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIDEFCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_DEF  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIDEVCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_DEV  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIEDITCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_EDIT ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIINKCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_INK  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPILCIDCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_LCID ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPILCTCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_LCT  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIMETACHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_META ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIPATHCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_PATH ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIPOLYCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_POLY ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIPRIMCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_PRIM ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPIRGNCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_RGN  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPISEGCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_SEG  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_GPITRANCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewGpiGrp, G_TRAN ) ? 1 : 0 ), MPVOID );

}  /* SetCustGpiDlgCheck() */

/******************************************************************************/
/*                                                                            */
/* SetCustWinDlgCheck()                                                       */
/*                                                                            */
/******************************************************************************/

VOID SetCustWinDlgCheck( HWND hwndDlg )          /* Dialog window             */
{

   /*
    * Set PMWIN APIs customization dialog check buttons
    */
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINACCLCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_ACCL ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINATOMCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_ATOM ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINCLIPCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_CLIP ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINCTRYCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_CTRY ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINCURCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_CUR  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINDDECHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_DDE  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINDESKCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_DESK ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINDLGCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_DLG  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINDWINCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_DWIN ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINENVCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_ENV  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINERRCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_ERR  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINFRAMCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_FRAM ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINHEAPCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_HEAP ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINHOOKCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_HOOK ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WININPTCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_INPT ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINLOADCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_LOAD ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINMENUCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_MENU ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINMSGCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_MSG  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINMSGLCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_MSGL ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINPALCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_PAL  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINPTRCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_PTR  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINRECTCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_RECT ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINSEICHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_SEI  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINSYSCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_SYS  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINTHKCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_THK  ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINTIMECHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_TIME ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINTRECCHK ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_TREC ) ? 1 : 0 ), MPVOID );
   WinSendMsg( WinWindowFromID( hwndDlg, DLG_WINWINCHK  ), BM_SETCHECK, MPFROMSHORT( FLAGSET( ulNewWinGrp, W_WIN  ) ? 1 : 0 ), MPVOID );

}  /* SetCustWinDlgCheck() */

/******************************************************************************/
/*                                                                            */
/* Strcmpi()                                                                  */
/*                                                                            */
/******************************************************************************/

LONG Strcmpi( PCHAR szStr1                       /* First string              */
            , PCHAR szStr2                       /* Second string             */
            )
{

   /*
    * Compare each character in first string to corresponding character in
    * second string ignoring case
    */
   for ( ; tolower( *szStr1 ) == tolower( *szStr2 ); szStr1++, szStr2++ )
   {
      /*
       * Indicate strings equal if end of strings encountered
       */
      if ( !*szStr1 )
      {
         return 0L;
      }
   }

   /*
    * Indicate strings not equal
    */
   return *szStr1 - *szStr2;

}  /* Strcmpi() */

/******************************************************************************/
/*                                                                            */
/* Strupr()                                                                   */
/*                                                                            */
/******************************************************************************/

PCHAR Strupr( PCHAR szStr )                      /* String                    */
{

   PCHAR pch = szStr;                            /* Character pointer         */

   /*
    * Convert each character in string to upper case
    */
   for ( ; *pch; pch++ )
   {
      /*
       * Convert character to upper case if lower case
       */
      if ( islower( *pch ) )
      {
         *pch = toupper( *pch );
      }
   }

   /*
    * Return string
    */
   return szStr;

}  /* Strupr() */

/******************************************************************************/
/*                                                                            */
/* SummarizationDlgProc()                                                     */
/*                                                                            */
/******************************************************************************/

MRESULT EXPENTRY SummarizationDlgProc( HWND   hwndDlg /* Dialog handle        */
                                     , ULONG  ulMsg   /* Message number       */
                                     , MPARAM mp1Dlg  /* Message parameter 1  */
                                     , MPARAM mp2Dlg  /* Message parameter 2  */
                                     )
{

   FILE     *fpOutFile;                          /* Output file               */
   IPT       ipt = -1L;                          /* Insertion point           */
   LONG      lNoReturn;                          /* Invocations without return*/
   PAPIINFO  pAPI;                               /* API information           */
   PCHAR     pch;                                /* Character pointer         */
   PSUMINFO  pSumInfo;                           /* Summarization information */
   FILEDLG   sFileDlg;                           /* File dialog structure     */
   SWP       swpDlg;                             /* Dialog window position    */
   CHAR      szText[ CCHMAXPATH ];               /* Dialog text               */
   ULONG     ulDLLIdx;                           /* DLL information index     */
   ULONG     ulLength;                           /* Dialog text length        */

   /*
    * Identify summarization dialog processing based on message number
    */
   switch ( ulMsg )
   {
      /*
       * Closing summarization dialog
       */
      case WM_CLOSE:
         /*
          * Translate as OK push button
          */
         WinPostMsg( hwndDlg, WM_COMMAND, MPFROMSHORT( DLG_SUMOK ), MPVOID );

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Control notifying summarization dialog
       */
      case WM_COMMAND:
         /*
          * Obtain summarization information, issue error and terminate if
          * unsuccessful
          */
         if ( !( pSumInfo = (PSUMINFO)WinQueryWindowPtr( hwndDlg, 0 ) ) )
         {
            Error( "Unable to obtain summarization information" );
         }

         /*
          * Identify summarization dialog control processing based on command
          * value
          */
         switch ( SHORT1FROMMP( mp1Dlg ) )
         {
            /*
             * Process help push button
             */
            case DLG_SUMHELP:
               /*
                * Display summarization help window
                */
               WinSendMsg( hwndMainHelp, HM_DISPLAY_HELP, MPFROMSHORT( PNL_DLG_SUM ), MPFROMSHORT( HM_RESOURCEID ) );

               break;

            /*
             * Process OK push button
             */
            case DLG_SUMOK:
               /*
                * Verify cancellation if trace information summary not saved
                * and information exists
                */
               if ( ( !pSumInfo->bSaved ) && ( pSumInfo->pAPIFirst ) )
               {
                  /*
                   * Verify cancellation, continue processing if NO push button
                   * selected
                   */
                  if ( !QueryMessageBox( ( ( ( pch = strrchr( pSumInfo->szInFile, '\\' ) ) != NULL ) || ( ( pch = strrchr( pSumInfo->szInFile, ':' ) ) != NULL ) ) ? pch + 1 : pSumInfo->szInFile,
                                      "Trace information summarization has not been saved, exit?" ) )
                  {
                     break;
                  }
               }

               /*
                * Close summarization dialog indicating OK
                */
               WinDismissDlg( hwndDlg, DID_OK );

               /*
                * Terminate processing summarization dialog messages
                */
               WinPostMsg( hwndDlg, WM_QUIT, MPVOID, MPVOID );

               break;

            /*
             * Process save as push button
             */
            case DLG_SUMSAVEAS:
               /*
                * Save trace information summary if summary not saved and
                * information exists
                */
               if ( ( !pSumInfo->bSaved ) && ( pSumInfo->pAPIFirst ) )
               {
                  /*
                   * Initialize file dialog structure
                   */
                  memset( &sFileDlg, 0, sizeof( FILEDLG ) );
                  sFileDlg.cbSize = sizeof( FILEDLG );
                  sFileDlg.fl = FDS_CENTER | FDS_SAVEAS_DIALOG;
                  sFileDlg.pszTitle = "Save Trace Information Summary File";
                  strcpy( sFileDlg.szFullFile, pSumInfo->szInFile );
                  strcpy( ( ( pch = strrchr( sFileDlg.szFullFile, '.' ) ) ? pch : sFileDlg.szFullFile ), ".SUM" );

                  /*
                   * Create file dialog to obtain trace information summary
                   * file name, process user's selection if successful
                   */
                  if ( WinFileDlg( HWND_DESKTOP, hwndMainClient, &sFileDlg ) )
                  {
                     /*
                      * Continue processing if user selected cancel button
                      */
                     if ( sFileDlg.lReturn == DID_CANCEL )
                     {
                        break;
                     }

                     /*
                      * Save trace information summary if user selected OK
                      * button
                      */
                     else if ( sFileDlg.lReturn == DID_OK )
                     {
                        /*
                         * Display API summary statistics starting at beginning
                         * of API information chain if the chain is not empty
                         */
                        if ( ( pAPI = pSumInfo->pAPIFirst ) != NULL )
                        {
                           /*
                            * Open trace information summary file, issue error
                            * and terminate if unsuccessful
                            */
                           if ( !( fpOutFile = fopen( sFileDlg.szFullFile, "w" ) ) )
                           {
                              Error( "Unable to open trace information summary file: %s", sFileDlg.szFullFile );
                           }

                           /*
                            * Write program banner
                            */
                           DisplayBanner( fpOutFile );

                           /*
                            * Write used APIs header
                            */
                           fprintf( fpOutFile, "Used APIs:\n" );

                           /*
                            * Write summary statistics for each API in API
                            * information chain
                            */
                           while ( pAPI )
                           {
                              /*
                               * Write API name
                               */
                              fprintf( fpOutFile, "  %s (", pAPI->szName );

                              /*
                               * Write any successful API invocations
                               */
                              if ( pAPI->ulPass )
                              {
                                 fprintf( fpOutFile, "%d Pass", pAPI->ulPass );
                              }

                              /*
                               * Write any unsuccessful API invocations
                               */
                              if ( pAPI->ulFail )
                              {
                                 fprintf( fpOutFile, "%s%d Fail", pAPI->ulPass ? ", " : "", pAPI->ulFail );
                              }

                              /*
                               * Write any indeterminate API invocations
                               */
                              if ( pAPI->ulIndeterminate )
                              {
                                 fprintf( fpOutFile, "%s%d Indeterminate", pAPI->ulPass || pAPI->ulFail ? ", " : "", pAPI->ulIndeterminate );
                              }

                              /*
                               * Write any API invocations that did not return
                               */
                              if ( ( pAPI->ulCall != pAPI->ulPass + pAPI->ulFail + pAPI->ulIndeterminate ) && ( ( lNoReturn = pAPI->ulCall - pAPI->ulPass - pAPI->ulFail - pAPI->ulIndeterminate ) > 0L ) )
                              {
                                 fprintf( fpOutFile, "%s%d No Return", pAPI->ulPass || pAPI->ulFail || pAPI->ulIndeterminate ? ", " : "", lNoReturn );
                              }

                              /*
                               * Terminate API information line
                               */
                              fprintf( fpOutFile, ")\n" );

                              /*
                               * Skip API information
                               */
                              pAPI = pAPI->pNext;
                           }

                           /*
                            * Close trace information summary file, issue error
                            * and terminate if unsuccessful
                            */
                           if ( fclose( fpOutFile ) )
                           {
                              Error( "Unable to close trace information summary file: %s", sFileDlg.szFullFile );
                           }

                           /*
                            * Indicate trace information summary saved
                            */
                           pSumInfo->bSaved = TRUE;
                        }
                     }
                  }

                  /*
                   * Issue error and terminate if file dialog creation
                   * unsuccessful
                   */
                  else
                  {
                     Error( "Unable to create trace information summary file dialog" );
                  }

               }

               break;
         }

         /*
          * Indicate focus window unchanged
          */
         return (MRESULT)FALSE;

      /*
       * Initializing summarization dialog
       */
      case WM_INITDLG:
         /*
          * Obtain summarization dialog window position
          */
         WinQueryWindowPos( hwndDlg, &swpDlg );

         /*
          * Move summarization dialog to center of desktop
          */
         WinSetWindowPos( hwndDlg, 0L, ( swpDesktop.cx - swpDlg.cx ) / 2, ( swpDesktop.cy - swpDlg.cy ) / 2, 0L, 0L, SWP_MOVE );

         /*
          * Obtain summarization information, issue error and terminate if
          * unsuccessful
          */
         if ( !( pSumInfo = (PSUMINFO)mp2Dlg ) )
         {
            Error( "Unable to obtain summarization information" );
         }

         /*
          * Set summarization dialog window pointer, issue error and terminate
          * if unsuccessful
          */
         if ( !WinSetWindowPtr( hwndDlg, 0L, pSumInfo ) )
         {
            Error( "Unable to set summarization dialog window pointer" );
         }

         /*
          * Format summarization dialog title
          */
         sprintf( szText, "%s Summarization", ( ( ( pch = strrchr( pSumInfo->szInFile, '\\' ) ) != NULL ) || ( ( pch = strrchr( pSumInfo->szInFile, ':' ) ) != NULL ) ) ? pch + 1 : pSumInfo->szInFile );

         /*
          * Set summarization dialog title
          */
         WinSetWindowText( hwndDlg, szText );

         /*
          * Initialize summarization multi-line entry field
          */
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_SUMMLE ), MLM_FORMAT, MPFROMSHORT( MLFIE_NOTRANS ), MPFROMLONG( 0L ) );
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_SUMMLE ), MLM_SETIMPORTEXPORT, MPFROMP( szText ), MPFROMLONG( sizeof( szText ) ) );
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_SUMMLE ), MLM_SETREADONLY, MPFROMSHORT( TRUE ), MPFROMLONG( 0L ) );
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_SUMMLE ), MLM_SETTEXTLIMIT, MPFROMLONG( sizeof( szText ) - 1 ), MPFROMLONG( 0L ) );
         WinSendMsg( WinWindowFromID( hwndDlg, DLG_SUMMLE ), MLM_SETWRAP, MPFROMSHORT( FALSE ), MPFROMLONG( 0L ) );

         /*
          * Display API summary statistics starting at beginning of API information
          * chain if the chain is not empty
          */
         if ( ( pAPI = pSumInfo->pAPIFirst ) != NULL )
         {
            /*
             * Display summary statistics for each API in API information chain
             */
            while ( pAPI )
            {
               /*
                * Display API name
                */
               ulLength = sprintf( szText, "%s (", pAPI->szName );

               /*
                * Display any successful API invocations
                */
               if ( pAPI->ulPass )
               {
                  ulLength += sprintf( &szText[ ulLength ], "%d Pass", pAPI->ulPass );
               }

               /*
                * Display any unsuccessful API invocations
                */
               if ( pAPI->ulFail )
               {
                  ulLength += sprintf( &szText[ ulLength ], "%s%d Fail", pAPI->ulPass ? ", " : "", pAPI->ulFail );
               }

               /*
                * Display any indeterminate API invocations
                */
               if ( pAPI->ulIndeterminate )
               {
                  ulLength += sprintf( &szText[ ulLength ], "%s%d Indeterminate", pAPI->ulPass || pAPI->ulFail ? ", " : "", pAPI->ulIndeterminate );
               }

               /*
                * Display any API invocations that did not return
                */
               if ( ( pAPI->ulCall != pAPI->ulPass + pAPI->ulFail + pAPI->ulIndeterminate ) && ( ( lNoReturn = pAPI->ulCall - pAPI->ulPass - pAPI->ulFail - pAPI->ulIndeterminate ) > 0L ) )
               {
                  ulLength += sprintf( &szText[ ulLength ], "%s%d No Return", pAPI->ulPass || pAPI->ulFail || pAPI->ulIndeterminate ? ", " : "", lNoReturn );
               }

               /*
                * Terminate API information line
                */
               ulLength += sprintf( &szText[ ulLength ], ")\n" );

               /*
                * Place API information line in summarization multi-line entry
                * field
                */
               WinSendMsg( WinWindowFromID( hwndDlg, DLG_SUMMLE ), MLM_IMPORT, MPFROMP( &ipt ), MPFROMLONG( ulLength ) );

               /*
                * Skip API information
                */
               pAPI = pAPI->pNext;
            }
         }

         /*
          * Give summarization multi-line entry field focus
          */
         WinSetFocus( HWND_DESKTOP, WinWindowFromID( hwndDlg, DLG_SUMMLE ) );

         /*
          * Indicate focus window changed
          */
         return (MRESULT)TRUE;
   }

   /*
    * Perform default dialog processing
    */
   return WinDefDlgProc( hwndDlg, ulMsg, mp1Dlg, mp2Dlg );

}  /* SummarizationDlgProc() */

/******************************************************************************/
/*                                                                            */
/* SummarizationExceptionHandler()                                            */
/*                                                                            */
/******************************************************************************/

ULONG APIENTRY SummarizationExceptionHandler
                                  ( PREPREC pRep /* Exception information     */
                                  , PREGREC pReg /* Registration record       */
                                  , PCTXREC pCtx /* Exception context         */
                                  , PVOID   p    /* Not used                  */
                                  )
{

   /*
    * Clean up if being removed from exception handler chain
    */
   if ( FLAGSET( pRep->fHandlerFlags, EH_UNWINDING ) )
   {
      /*
       * Close trace information file and terminate tables if summarization
       * information available
       */
      if ( ((PSUMREGREC)pReg)->pSumInfo )
      {
         TerminateSummarization( FALSE, ((PSUMREGREC)pReg)->pSumInfo );
      }

      /*
       * Destroy summarization dialog message queue if queue exists
       */
      if ( ((PSUMREGREC)pReg)->hmqSum )
      {
         WinDestroyMsgQueue( ((PSUMREGREC)pReg)->hmqSum );
      }

      /*
       * Terminate Presentation Manager facilities if facilities initialized
       */
      if ( ((PSUMREGREC)pReg)->habSum )
      {
         WinTerminate( ((PSUMREGREC)pReg)->habSum );
      }

      /*
       * Release access to system semaphore
       */
      while ( !DosReleaseMutexSem( hmtxSys ) );
   }

   /*
    * Indicate ignore process terminate exceptions
    */
   else if ( ( pRep->ExceptionNum == XCPT_PROCESS_TERMINATE ) || ( pRep->ExceptionNum == XCPT_ASYNC_PROCESS_TERMINATE ) )
   {
      return XCPT_CONTINUE_SEARCH;
   }

   /*
    * Issue error and terminate if fatal exception generated
    */
   else if ( ( pRep->ExceptionNum & XCPT_SEVERITY_CODE ) == XCPT_FATAL_EXCEPTION )
   {
#ifdef DEBUG
      Error( "Summarization thread fatal exception at 0x%08lX: 0x%08lX", pRep->ExceptionAddress, pRep->ExceptionNum );
#else
      Error( "Summarization thread fatal exception: 0x%08lX", pRep->ExceptionNum );
#endif
   }

   /*
    * Indicate ignore exception
    */
   return XCPT_CONTINUE_SEARCH;

}  /* SummarizationExceptionHandler() */


/******************************************************************************/
/*                                                                            */
/* SummarizationThread()                                                      */
/*                                                                            */
/******************************************************************************/

VOID APIENTRY SummarizationThread( ULONG ulParam )/* Thread parameter         */
{

   HELPINIT  hmiSum;                             /* Sum dialog help mgr init  */
   HWND      hwndSum;                            /* Summarization dialog      */
   HWND      hwndSumHelp;                        /* Summarization dialog help */
   PSUMINFO  pSumInfo;                           /* Summarization information */
   QMSG      qmsgSum;                            /* Summarization dialog msg  */
   SUMREGREC sSumRegRec;                         /* Sum registration record   */
   FILEDLG   sFileDlg;                           /* File dialog structure     */
   APIRET    ulRC;                               /* Return code               */

   /*
    * Register exception handler
    */
   sSumRegRec.sRegRec.ExceptionHandler = SummarizationExceptionHandler;
   sSumRegRec.pSumInfo = NULL;
   DosSetExceptionHandler( (PREGREC)&sSumRegRec );

   /*
    * Initialize Presentation Manager facilities, continue summarization dialog
    * creation if successful
    */
   if ( ( sSumRegRec.habSum = WinInitialize( 0L ) ) != 0L )
   {
      /*
       * Create summarization dialog message queue, continue summarization
       * dialog creation if successful
       */
      if ( ( sSumRegRec.hmqSum = WinCreateMsgQueue( sSumRegRec.habSum, 0L ) ) != 0L )
      {
         /*
          * Obtain ownership of system semaphore, ignoring asynchronous
          * interrupts
          */
         ulRC = ERROR_INTERRUPT;
         while ( ulRC == ERROR_INTERRUPT )
         {
            ulRC = DosRequestMutexSem( hmtxSys, SEM_INDEFINITE_WAIT );
         }

         /*
          * Initialize file dialog structure
          */
         memset( &sFileDlg, 0, sizeof( FILEDLG ) );
         sFileDlg.cbSize = sizeof( FILEDLG );
         sFileDlg.fl = FDS_CENTER | FDS_OPEN_DIALOG;
         sFileDlg.pszTitle = "Open Trace Information File";
         strcpy( sFileDlg.szFullFile, szInFile );
         strcpy( strrchr( sFileDlg.szFullFile, '\\' ) + 1, "*.TRC" );

         /*
          * Create file dialog to obtain trace information file name, process
          * user's selection if successful
          */
         if ( WinFileDlg( HWND_DESKTOP, hwndMainClient, &sFileDlg ) )
         {
            /*
             * Terminate summarization thread if user selected cancel button
             */
            if ( sFileDlg.lReturn == DID_CANCEL )
            {
               DosExit( EXIT_THREAD, 0L );
            }

            /*
             * Save input file name if user selected OK button
             */
            else if ( sFileDlg.lReturn == DID_OK )
            {
               strcpy( szInFile, sFileDlg.szFullFile );
            }
         }

         /*
          * Issue error and terminate if file dialog creation unsuccessful
          */
         else
         {
            Error( "Unable to create summarization thread file dialog" );
         }

         /*
          * Release ownership of system semaphore
          */
         DosReleaseMutexSem( hmtxSys );

         /*
          * Allocate summarization information, issue error and terminate if
          * unsuccessful
          */
         if ( !( pSumInfo = malloc( sizeof( SUMINFO ) ) ) )
         {
            Error( "Unable to allocate memory for summarization thread information" );
         }

         /*
          * Set summarization information in exception registration record
          */
         sSumRegRec.pSumInfo = pSumInfo;

         /*
          * Initialize summarization information
          */
         memset( pSumInfo, 0, sizeof( SUMINFO ) );

         /*
          * Save trace information file name in summarization information
          */
         strcpy( pSumInfo->szInFile, sFileDlg.szFullFile );

         /*
          * Read trace information file and initialize tables
          */
         InitializeSummarization( pSumInfo );

         /*
          * Summarize API tracing in trace information file
          */
         SummarizeAPITracing( pSumInfo );

         /*
          * Initialize summarization dialog help manager initialization
          * information
          */
         hmiSum.cb                       = sizeof( HELPINIT );
         hmiSum.ulReturnCode             = 0L;
         hmiSum.pszTutorialName          = NULL;
         hmiSum.phtHelpTable             = (PHELPTABLE)(0xFFFF0000 | HLP_SUM );
         hmiSum.hmodHelpTableModule      = 0L;
         hmiSum.hmodAccelActionBarModule = 0L;
         hmiSum.idAccelTable             = 0L;
         hmiSum.idActionBar              = 0L;
         hmiSum.pszHelpWindowTitle       = hmiMain.pszHelpWindowTitle;
         hmiSum.fShowPanelId             = CMIC_HIDE_PANEL_ID;
         hmiSum.pszHelpLibraryName       = DEF_HELPFILE;

         /*
          * Create summarization dialog help instance, issue warning if
          * unsuccessful
          */
         if ( !( hwndSumHelp = WinCreateHelpInstance( sSumRegRec.habSum, &hmiSum ) ) )
         {
            ErrorMessageBox( MB_WARNING, "Unable to create summarization window help instance, help disabled" );
         }

         /*
          * Create summarization dialog, initiate summarization dialog message
          * processing if successful
          */
         if ( ( hwndSum = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, SummarizationDlgProc, NULLHANDLE, DLG_SUM, pSumInfo ) ) != 0L )
         {
            /*
             * Associate summarization dialog help instance with summarization
             * dialog if instance created
             */
            if ( hwndSumHelp )
            {
               /*
                * Associate summarization dialog help instance with
                * summarization dialog, issue warning if unsuccessful
                */
               if ( !WinAssociateHelpInstance( hwndSumHelp, hwndSum ) )
               {
                  ErrorMessageBox( MB_WARNING, "Unable to associate summarization dialog help instance, help disabled" );
               }
            }

            /*
             * Process summarization dialog messages
             */
            while( WinGetMsg( sSumRegRec.habSum, &qmsgSum, 0L, 0L, 0L ) )
            {
               WinDispatchMsg( sSumRegRec.habSum, &qmsgSum );
            }

            /*
             * Destroy summarization dialog
             */
            WinDestroyWindow( hwndSum );
         }

         /*
          * Issue error and terminate if summarization dialog creation
          * unsuccessful
          */
         else
         {
            Error( "Unable to create summarization dialog" );
         }

         /*
          * Destroy summarization dialog help instance if instance created
          */
         if ( hwndSumHelp )
         {
            WinDestroyHelpInstance( hwndSumHelp );
         }

         /*
          * Close executable file and terminate tables
          */
         TerminateSummarization( TRUE, pSumInfo );

         /*
          * Free memory for summarization information
          */
         free( pSumInfo );

         /*
          * Clear summarization information in exception registration record
          */
         sSumRegRec.pSumInfo = NULL;

         /*
          * Destroy summarization dialog message queue
          */
         WinDestroyMsgQueue( sSumRegRec.hmqSum );
      }

      /*
       * Issue error and terminate if summarization dialog message queue
       * creation unsuccessful
       */
      else
      {
         Error( "Unable to create summarization dialog message queue" );
      }

      /*
       * Terminate Presentation Manager facilities
       */
      WinTerminate( sSumRegRec.habSum );
   }

   /*
    * Issue error and terminate if Presentation Manager facilities
    * initialization unsuccessful
    */
   else
   {
      Error( "Unable to initialize Presentation Manager" );
   }

   /*
    * Unregister exception handler
    */
   DosUnsetExceptionHandler( (PREGREC)&sSumRegRec );

}  /* SummarizationThread() */

/******************************************************************************/
/*                                                                            */
/* SummarizeAPITracing()                                                      */
/*                                                                            */
/******************************************************************************/

VOID SummarizeAPITracing( PSUMINFO pSumInfo )    /* Summarization information */
{

   BOOL     bAPIEntry = FALSE;                   /* API entry found indicator */
   BOOL     bWarning = FALSE;                    /* Display warning indicator */
   PAPIINFO pAPI;                                /* API information           */
   PAPIINFO pLastAPI = NULL;                     /* Last API information      */
   CHAR     szAPIExit[ LEN_IOLINE ];             /* Initial API exit name     */
   CHAR     szAPIName[ MAX_APINAME ];            /* API name                  */
   CHAR     szEntExt[ LEN_IOLINE ];              /* API entry/exit string     */
   CHAR     szLine[ LEN_IOLINE ];                /* Trace info file line      */
   CHAR     szStatus[ LEN_IOLINE ];              /* API exit status string    */
   CHAR     szTemp[ LEN_IOLINE ];                /* Temporary string          */
   ULONG    ulAPIExit = 0L;                      /* Initial API exit result   */

   /*
    * Read, parse, and analyze each line in trace information file
    */
   while ( fgets( szLine, LEN_IOLINE, pSumInfo->fpInFile ) )
   {
      /*
       * There is one line to look for that indicates the entry to an API call:
       *
       *  PID  TID | API Entry
       *
       * And there are two lines to look for that indicate the exit from an API
       * call:
       *
       *  PID  TID | API Exit
       *      PASS | Return code: 0
       *
       * Parse line from trace information file and analyze if line contains
       * same number of tokens as API entry or exit line
       */
      if ( sscanf( szLine, "%s %s | %s %s", szTemp, szTemp, szAPIName, szEntExt ) == 4 )
      {
         /*
          * API entry line found
          */
         if ( !strncmp( szEntExt, "Entry", 5 ) )
         {
            /*
             * Update API information if API already called
             */
            if ( ( pAPI = GetAPIInfo( pSumInfo, szAPIName ) ) != NULL )
            {
               /*
                * Update API information
                */
               pAPI->ulCall++;
            }

            /*
             * Add API information if API not already called
             */
            else
            {
               /*
                * Add API information
                */
               pAPI = AddAPIInfo( pSumInfo, szAPIName, 1L, 0L, 0L, 0L );
            }

            /*
             * Indicate API entry found
             */
            bAPIEntry = TRUE;

            /*
             * Save last API information
             */
            pLastAPI  = pAPI;
         }

         /*
          * API exit line found
          */
         else if ( !strncmp( szEntExt, "Exit", 4 ) )
         {
            /*
             * Read and parse line from trace information file, update API
             * information if successfule
             */
            if ( fgets( szLine, LEN_IOLINE, pSumInfo->fpInFile ) )
            {
               /*
                * Update API information if API already called
                */
               if ( ( pAPI = GetAPIInfo( pSumInfo, szAPIName ) ) != NULL )
               {
                  /*
                   * Clear last API information
                   */
                  pLastAPI = NULL;

                  /*
                   * Update API information if indeterminate API invocation
                   */
                  if ( !strncmp( &szLine[ strspn( szLine, " \t" ) ], "| Return code:", 14 ) )
                  {
                     /*
                      * Update API information
                      */
                     pAPI->ulIndeterminate++;
                  }

                  /*
                   * Update API information if successful or unsuccessful API
                   * invocation
                   */
                  else if ( sscanf( szLine, "%s | Return code:", szStatus ) == 1 )
                  {
                     /*
                      * Update API information
                      */
                     strcmp( szStatus, "PASS" ) ? pAPI->ulFail++ : pAPI->ulPass++;
                  }
               }

               /*
                * Update saved API information if API not already called and
                * API entry not yet found (trace information file begins with
                * API exit)
                */
               else if ( !bAPIEntry )
               {
                  /*
                   * Save initial API exit name
                   */
                  strcpy( szAPIExit, szAPIName );

                  /*
                   * Save initial API exit result if indeterminate API
                   * invocation
                   */
                  if ( !strncmp( &szLine[ strspn( szLine, " \t" ) ], "| Return code:", 14 ) )
                  {
                     ulAPIExit = APIEXT_IND;
                  }

                  /*
                   * Save initial API exit result if successful or unsuccessful
                   * API invocation
                   */
                  else if ( sscanf( szLine, "%s | Return code:", szStatus ) == 1 )
                  {
                     ulAPIExit = strcmp( szStatus, "PASS" ) ? APIEXT_FAIL : APIEXT_PASS;
                  }
               }
            }
         }
      }

      /*
       * There are four lines to look for that indicate API entry/exit
       * information may be lost:
       *
       *  < OS2TRACE: API entry/exit information lost >
       *  < OS2TRACE: Trace information lost due to log file wrapping x times >
       *  < OS2TRACE: Pausing trace at xx:xx:xx.xx >
       *  < OS2TRACE: Resuming trace at xx:xx:xx.xx >
       *
       * Parse line from trace information file and analyze if line contains
       * "< OS2TRACE: " prefix
       */
      else if ( sscanf( szLine, "< OS2TRACE: %s", szTemp ) == 1 )
      {
         /*
          * Indicate display warning if possible lost API entry/exit
          * information found
          */
         if ( ( !strncmp( szTemp, "API", 3 )      ) ||
              ( !strncmp( szTemp, "Trace", 5 )    ) ||
              ( !strncmp( szTemp, "Pausing", 7 )  ) ||
              ( !strncmp( szTemp, "Resuming", 8 ) ) )
         {
            bWarning = TRUE;
         }
      }
   }

   /*
    * Update API information if trace information file begins with API exit and
    * ends with API entry and it is same API
    */
   if ( ( ulAPIExit ) && ( pLastAPI ) && ( !strcmp( pLastAPI->szName, szAPIExit ) ) )
   {
      /*
       * Update API information
       */
      switch ( ulAPIExit )
      {
         case APIEXT_PASS: pLastAPI->ulPass++; break;
         case APIEXT_FAIL: pLastAPI->ulFail++; break;
         case APIEXT_IND : pLastAPI->ulIndeterminate++; break;
      }
   }

   /*
    * Display warning if so indicated
    */
   if ( bWarning )
   {
      ErrorMessageBox( MB_WARNING, "Summarization may not be accurate due to lost API entries/exits, pausing/resuming trace, or file wrapping" );
   }

}  /* SummarizeAPITracing() */

/******************************************************************************/
/*                                                                            */
/* TerminateEnablement()                                                      */
/*                                                                            */
/******************************************************************************/

VOID TerminateEnablement( BOOL      bErrMsgs     /* Error messages allowed    */
                        , PENABINFO pEnabInfo    /* Enablement information    */
                        )
{

   FILESTATUS3 filestatus3;                      /* File information          */
   HFILE       hfile;                            /* File handle               */
   ULONG       ulAction;                         /* Action taken by DosOpen   */
   ULONG       ulAttr;                           /* File open attributes      */
   ULONG       ulFlag;                           /* File open flags           */
   ULONG       ulMode;                           /* File open mode            */

   /*
    * Free memory for dynamic linking information table if one exists
    */
   if ( pEnabInfo->pDynLnkInf )
   {
      free( pEnabInfo->pDynLnkInf );
      pEnabInfo->pDynLnkInf = NULL;
   }

   /*
    * Free memory for fixup record table if one exists
    */
   if ( pEnabInfo->pFixupTbl )
   {
      free( pEnabInfo->pFixupTbl );
      pEnabInfo->pFixupTbl = NULL;
   }

   /*
    * Free memory for import name table if one exists
    */
   if ( pEnabInfo->pImpNamTbl )
   {
      free( pEnabInfo->pImpNamTbl );
      pEnabInfo->pImpNamTbl = NULL;
   }

   /*
    * Free memory for module index table if one exists
    */
   if ( pEnabInfo->pModIdxTbl )
   {
      free( pEnabInfo->pModIdxTbl );
      pEnabInfo->pModIdxTbl = NULL;
   }

   /*
    * Free memory for module name table if one exists
    */
   if ( pEnabInfo->pModNamTbl )
   {
      free( pEnabInfo->pModNamTbl );
      pEnabInfo->pModNamTbl = NULL;
   }

   /*
    * Free memory for module reference table if one exists
    */
   if ( pEnabInfo->pModRefTbl )
   {
      free( pEnabInfo->pModRefTbl );
      pEnabInfo->pModRefTbl = NULL;
   }

   /*
    * Free memory for procedure name table if one exists
    */
   if ( pEnabInfo->pPrcNamTbl )
   {
      free( pEnabInfo->pPrcNamTbl );
      pEnabInfo->pPrcNamTbl = NULL;
   }

   /*
    * Free memory for program header table if one exists
    */
   if ( pEnabInfo->pProgHdr )
   {
      free( pEnabInfo->pProgHdr );
      pEnabInfo->pProgHdr = NULL;
   }

   /*
    * Free memory for string table if one exists
    */
   if ( pEnabInfo->pStrTbl )
   {
      free( pEnabInfo->pStrTbl );
      pEnabInfo->pStrTbl = NULL;
   }

   /*
    * Free memory for segment if one exists
    */
   if ( pEnabInfo->pSegment )
   {
      free( pEnabInfo->pSegment );
      pEnabInfo->pSegment = NULL;
   }

   /*
    * Free memory for segment table if one exists
    */
   if ( pEnabInfo->pSegmentTbl )
   {
      free( pEnabInfo->pSegmentTbl );
      pEnabInfo->pSegmentTbl = NULL;
   }

   /*
    * Close executable file if file open
    */
   if ( ( pEnabInfo->fpInFile ) && ( fclose( pEnabInfo->fpInFile ) ) )
   {
      if ( bErrMsgs ) Error( "Unable to close file: %s", pEnabInfo->szInFile );
   }
   else
   {
      pEnabInfo->fpInFile = NULL;
   }

   /*
    * Restore executable file date/time stamp if -P option specified
    */
   if ( FLAGSET( pEnabInfo->ulFlags, fPOPTION ) )
   {
      /*
       * Disable -P option
       */
      CLRFLAG( pEnabInfo->ulFlags, fPOPTION );

      /*
       * Initialize open file flags
       */
      ulAttr = FILE_NORMAL;
      ulFlag = OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW;
      ulMode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE;

      /*
       * Open executable file, obtain file information if successful
       */
      if ( !DosOpen( pEnabInfo->szInFile, &hfile, &ulAction, 0L, ulAttr, ulFlag, ulMode, NULL ) )
      {
         /*
          * Obtain executable file information, restore file date/time stamp if
          * successful
          */
         if ( !DosQueryFileInfo( hfile, FIL_STANDARD, &filestatus3, sizeof( FILESTATUS3 ) ) )
         {
            filestatus3.fdateLastWrite = pEnabInfo->fdateInFile;
            filestatus3.ftimeLastWrite = pEnabInfo->ftimeInFile;

            /*
             * Restore file date/time stamp, issue error and terminate if
             * unsuccessful
             */
            if ( DosSetFileInfo( hfile, FIL_STANDARD, &filestatus3, sizeof( FILESTATUS3 ) ) )
            {
               if ( bErrMsgs ) Error( "Unable to preserve file date/time stamp" );
            }
         }

         /*
          * Issue error and terminate if unable to obtain executable file
          * information
          */
         else
         {
            if ( bErrMsgs ) Error( "Unable to preserve file date/time stamp" );
         }

         /*
          * Close executable file
          */
         DosClose( hfile );
      }

      /*
       * Issue error and terminate if unable to open executable file
       */
      else
      {
         if ( bErrMsgs ) Error( "Unable to preserve file date/time stamp" );
      }
   }

}  /* TerminateEnablement() */

/******************************************************************************/
/*                                                                            */
/* TerminateSummarization()                                                   */
/*                                                                            */
/******************************************************************************/

VOID TerminateSummarization( BOOL     bErrMsgs   /* Error messages allowed    */
                           , PSUMINFO pSumInfo   /* Summarization information */
                           )
{

   PAPIINFO pAPI;                                /* API information           */
   PAPIINFO pAPIFree;                            /* API information to free   */

   /*
    * Free memory for API information if information exists
    */
   if ( pSumInfo->pAPIFirst )
   {
      /*
       * Start at beginning of API information chain
       */
      pAPI = pSumInfo->pAPIFirst;

      /*
       * Free memory for each API until last API encountered
       */
      while ( pAPI->pNext )
      {
         pAPIFree = pAPI;
         pAPI = pAPI->pNext;
         free( pAPIFree );
      }

      /*
       * Free memory for last API
       */
      free( pAPI );
   }

   /*
    * Close trace information file if file open
    */
   if ( ( pSumInfo->fpInFile ) && ( fclose( pSumInfo->fpInFile ) ) )
   {
      if ( bErrMsgs ) Error( "Unable to close file: %s", pSumInfo->szInFile );
   }
   else
   {
      pSumInfo->fpInFile = NULL;
   }

}  /* TerminateSummarization() */

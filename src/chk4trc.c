/******************************************************************************/
/*                                                                            */
/* File name    : CHK4TRC.C                                                   */
/*                                                                            */
/* Title        : OS/2 API trace-enabled executable checker routines          */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Checks  for and  reports the  executable  file(s)  that are */
/*                trace-enabled.                                              */
/*                                                                            */
/*                This feat is  accomplished by processing  each entry in the */
/*                table  of  strings  within   the  executable  file(s)  that */
/*                contains  the names  of the  imported DLLs.  If  the  entry */
/*                represents one of the trace DLLs,  the executable file name */
/*                is displayed to standard output.                            */
/*                                                                            */
/*                The user can request  to display the DLLs  that are enabled */
/*                for tracing in the executable  file(s) by specifying the -D */
/*                option.  The user can  request to also check the executable */
/*                file(s) in any subdirectories by specifying the -S option.  */
/*                                                                            */
/*                The user can request helping  information by specifying the */
/*                -?, -H, or -HELP option.                                    */
/*                                                                            */
/* Installation : Place the executable, CHK4TRC.EXE, in a directory along the */
/*                PATH environment variable.                                  */
/*                                                                            */
/* Usage        : CHK4TRC [-D] [-S] file                                      */
/*                                                                            */
/*                Where:                                                      */
/*                  -D   indicates display DLLs enabled for tracing           */
/*                  -S   indicates check subdirectories                       */
/*                  file indicates name of  executable file(s)  to be checked */
/*                       (can include global file name characters * and ?)    */
/*                                                                            */
/*                In the following example, all DLLs in the current directory */
/*                are checked:                                                */
/*                                                                            */
/*                CHK4TRC *.DLL                                               */
/*                                                                            */
/*                In the  following  example,  all  executable  files  in the */
/*                C:\OS2 directory and its subdirectories are checked:        */
/*                                                                            */
/*                CHK4TRC -S C:\OS2                                           */
/*                                                                            */
/*                In the following example,  TEST.EXE is checked and any DLLs */
/*                enabled for tracing are displayed:                          */
/*                                                                            */
/*                CHK4TRC -D TEST.EXE                                         */
/*                                                                            */
/* Output       : Information similar to the following is displayed if any of */
/*                the executable files are trace-enabled:                     */
/*                                                                            */
/*                Trace-enabled executable file(s):                           */
/*                  filename                                                  */
/*                     :                                                      */
/*                  filename                                                  */
/*                                                                            */
/*                n executable files checked                                  */
/*                n trace-enabled executable files found                      */
/*                                                                            */
/*                If the  user  requests  the  display  of DLLs  enabled  for */
/*                tracing,  information similar to the following is displayed */
/*                if any of the executable files are trace-enabled:           */
/*                                                                            */
/*                Trace-enabled executable file(s):                           */
/*                  filename (DLLNAME,DLLNAME)                                */
/*                     :                                                      */
/*                  filename (DLLNAME)                                        */
/*                                                                            */
/*                n executable files checked                                  */
/*                n trace-enabled executable files found                      */
/*                                                                            */
/*                Information similar  to the following  is displayed if none */
/*                of the executable files are trace-enabled:                  */
/*                                                                            */
/*                n executable files checked                                  */
/*                0 trace-enabled executable files found                      */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.57 25Jun98 Added trace-enabled executable checker      */
/*                2.40.86 03Mar99 Added support  for executables  without DOS */
/*                                header                                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#define  INCL_BASE                               /* Include base functions    */
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
#include <stdarg.h>                              /* C information             */
#include <stdio.h>                               /* C information             */
#include <stdlib.h>                              /* C information             */
#include <string.h>                              /* C information             */
#include "os2trace.h"                            /* OS2TRACE information      */
#include "chk4trc.h"                             /* CHK4TRC information       */

/******************************************************************************/
/*                                                                            */
/* Function declarations                                                      */
/*                                                                            */
/******************************************************************************/

void           main( int, char ** );
VOID           AnalyzeEnablement( VOID );
VOID           CheckEnablement( PCHAR, PCHAR );
VOID           DisplayBanner( VOID );
VOID           Error( PCHAR, ... );
ULONG APIENTRY ExceptionHandler( PREPREC, PREGREC, PCTXREC, PVOID );
BOOL           InitializeEnablementCheck( VOID );
ULONG          LookupTraceDLL( PCHAR );
VOID           ParseCommandLine( int, char ** );
LONG           Strcmpi( PCHAR, PCHAR );
VOID           TerminateEnablementCheck( VOID );
VOID           Usage( VOID );

/******************************************************************************/
/*                                                                            */
/* Variable definitions                                                       */
/*                                                                            */
/******************************************************************************/

FILE     *fpInFile;                              /* Input file pointer        */
PCHAR     pFixupTbl;                             /* Fixup record table        */
PCHAR     pImpNamTbl;                            /* Import name table         */
PULONG    pModIdxTbl;                            /* Module index table        */
PCHAR     pModNamTbl;                            /* Module name table         */
PUSHORT   pModRefTbl;                            /* Module reference table    */
NEHDR    *pNEHdr;                                /* NE executable header      */
PCHAR     pPrcNamTbl;                            /* Procedure name table      */
PCHAR     pSegment;                              /* Segment                   */
NESEG    *pSegmentTbl;                           /* Segment table             */
PCHAR     pStrTbl;                               /* String table              */
LXHDR     sLXHdr;                                /* LX executable header      */
MZHDR     sMZHdr;                                /* DOS executable header     */
PCHAR     szAppName = VER_CHKNAME;               /* OS2TRACE application name */
CHAR      szInFile[ LEN_FILENAME ];              /* Input file name           */
ULONG     ulFileFound;                           /* Trace-enabled file found  */
ULONG     ulFixupTbl;                            /* Fixup record table size   */
ULONG     ulFlags;                               /* Flags                     */
ULONG     ulImpNamTbl;                           /* Import name table size    */
ULONG     ulModIdxTbl;                           /* Module index table size   */
ULONG     ulModNamTbl;                           /* Module name table size    */
ULONG     ulModRefTbl;                           /* Module reference tbl size */
ULONG     ulNumFiles;                            /* Number executable files   */
ULONG     ulPrcNamTbl;                           /* Procedure name table size */
ULONG     ulSegment;                             /* Number segments           */
ULONG     ulSegmentTbl;                          /* Segment table size        */
ULONG     ulStrTbl;                              /* String table size         */
ULONG     ulStrTblOff;                           /* String table file offset  */
USHORT    usSector;                              /* Sector size               */

#include  "dllinfo.h"                            /* DLL information table     */

/******************************************************************************/
/*                                                                            */
/* main()                                                                     */
/*                                                                            */
/******************************************************************************/

void main( int    argc                           /* Argument count            */
         , char **argv                           /* Argument pointer          */
         )
{

   HDIR         hDir;                            /* Directory handle          */
   PCHAR        pch;                             /* Character pointer         */
   FILEFINDBUF3 sFileInfo;                       /* File information struct   */
   REGREC       sRegRec;                         /* Registration record       */
   PCHAR        szFileName;                      /* File name                 */
   CHAR         szFullName[ LEN_FILENAME ];      /* Full file name            */
   ULONG        ulCount;                         /* Number file info structs  */
   ULONG        ulNamLen;                        /* File name length          */
   APIRET       ulRC;                            /* API return code           */

   /*
    * Register exception handler
    */
   sRegRec.ExceptionHandler = ExceptionHandler;
   DosSetExceptionHandler( &sRegRec );

   /*
    * Display program banner
    */
   DisplayBanner( );

   /*
    * Parse command line arguments
    */
   ParseCommandLine( --argc, ++argv );

   /*
    * Append "\*" to input file name if name ends in "." or ".."
    */
   if ( ( ( ( ulNamLen = strlen( szInFile ) ) == 1 ) && ( szInFile[ 0 ] == '.' )                                                                                                             ) ||
        ( ( ulNamLen == 2 ) && ( szInFile[ 0 ] == '.' ) && ( szInFile[ 1 ] == '.' )                                                                                                          ) ||
        ( ( ulNamLen >= 2 ) && ( szInFile[ ulNamLen - 1 ] == '.' ) && ( ( szInFile[ ulNamLen - 2 ] == '\\' ) || ( szInFile[ ulNamLen - 2 ] == ':' ) )                                        ) ||
        ( ( ulNamLen >= 3 ) && ( szInFile[ ulNamLen - 1 ] == '.' ) && ( szInFile[ ulNamLen - 2 ] == '.' ) && ( ( szInFile[ ulNamLen - 3 ] == '\\' ) || ( szInFile[ ulNamLen - 3 ] == ':' ) ) ) )
   {
      strcat( szInFile, "\\*" );
   }

   /*
    * Append "*" to input file name if name ends in ":" or "\"
    */
   else if ( ( ( ulNamLen == 2 ) && ( szInFile[ 1 ] == ':' )             ) ||
             ( ( ulNamLen >= 1 ) && ( szInFile[ ulNamLen - 1 ] == '\\' ) ) )
   {
      strcat( szInFile, "*" );
   }

   /*
    * Check for enablement if able to query full input file name
    */
   if ( ( ulRC = DosQueryPathInfo( szInFile, FIL_QUERYFULLNAME, szFullName, sizeof( szFullName ) ) ) == NO_ERROR )
   {
      /*
       * Initialize DosFindFirst input parameters
       */
      hDir    = HDIR_CREATE;
      ulCount = 1L;

      /*
       * Check for enablement if at least one file found or no files found but
       * checking subdirectories
       */
      if ( ( ( ulRC = DosFindFirst( szFullName, &hDir, FILE_NORMAL | FILE_DIRECTORY, &sFileInfo, sizeof( sFileInfo ), &ulCount, FIL_STANDARD ) ) == NO_ERROR ) ||
           ( ( ulRC == ERROR_NO_MORE_FILES ) && ( FLAGSET( ulFlags, fSOPTION ) )                                                                             ) )
      {
         /*
          * Find last backslash in full input file name
          */
         pch = strrchr( szFullName, '\\' );

         /*
          * Initialize path and file name if first file found is directory
          */
         if ( sFileInfo.attrFile & FILE_DIRECTORY )
         {
            /*
             * Full file name contains file and path name if global character
             * found
             */
            if ( strpbrk( pch + 1, "*?" ) )
            {
               *pch = '\0';
               szFileName = pch + 1;
            }

            /*
             * Full file name only contains path name if global character not
             * found
             */
            else
            {
               szFileName = "*";
            }
         }

         /*
          * Initialize path and file name if first file found is not directory
          */
         else
         {
            *pch = '\0';
            szFileName = pch + 1;
         }

         /*
          * Check for enablement of input file
          */
         CheckEnablement( szFullName, szFileName );
      }

      /*
       * Display number of executable files checked
       */
      printf( "%s%d executable file%s checked\n", ulFileFound ? "\n" : "", ulNumFiles, ulNumFiles == 1 ? "" : "s" );

      /*
       * Display number of trace-enabled executable files found
       */
      printf( "%d trace-enabled executable file%s found\n", ulFileFound, ulFileFound == 1 ? "" : "s" );
   }

   /*
    * Issue error and terminate if unable to query full input file name
    */
   else switch ( ulRC )
   {
      case ERROR_PATH_NOT_FOUND:
         Error( "File name contains invalid path" );

      case ERROR_INVALID_DRIVE :
         Error( "File name contains invalid drive" );

      case ERROR_INVALID_NAME  :
         Error( "Invalid file name" );

      default                  :
         Error( "Unable to query full file name (OS/2 error %d)", ulRC );
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
/* AnalyzeEnablement()                                                        */
/*                                                                            */
/******************************************************************************/

VOID AnalyzeEnablement( VOID )                   /* No arguments              */
{

   BOOL bDLLFound = FALSE;                       /* DLL enabled for trc found */
   int  i;                                       /* Loop variable             */

   /*
    * Search through DLL information table for DLL enabled for tracing
    */
   for ( i = 0; i < SUPPORTED_DLL; i++ )
   {
      /*
       * Found DLL enabled for tracing
       */
      if ( FLAGSET( DLLInfo[ i ].ulFlags, fTRACEDLL ) )
      {
         /*
          * Display output header if first trace-enabled executable
          */
         if ( !ulFileFound )
         {
            printf( "Trace-enabled executable file(s):\n" );
         }

         /*
          * Display DLL enabled for tracing if -D option specified
          */
         if ( FLAGSET( ulFlags, fDOPTION ) )
         {
            /*
             * Display full file name and DLL name if first DLL enabled for
             * tracing
             */
            if ( !bDLLFound )
            {
               printf( "  %s (%s", szInFile, DLLInfo[ i ].szName );
               ulFileFound++;
               bDLLFound = TRUE;
            }

            /*
             * Display DLL name if not first DLL enabled for tracing
             */
            else
            {
               printf( ",%s", DLLInfo[ i ].szName );
            }
         }

         /*
          * Do not display DLL enabled for tracing if -D option not specified
          */
         else
         {
            /*
             * Display full file name if first DLL enabled for tracing
             */
            if ( !bDLLFound )
            {
               printf( "  %s\n", szInFile );
               ulFileFound++;
               bDLLFound = TRUE;
            }
         }
      }
   }

   /*
    * Terminate DLL names if -D option specified and at least one DLL enabled
    * for tracing
    */
   if ( ( FLAGSET( ulFlags, fDOPTION ) ) && ( bDLLFound ) )
   {
      printf( ")\n" );
   }

}  /* AnalyzeEnablement() */

/******************************************************************************/
/*                                                                            */
/* CheckEnablement()                                                          */
/*                                                                            */
/******************************************************************************/

VOID CheckEnablement( PCHAR szPathName           /* Path name                 */
                    , PCHAR szFileName           /* File name                 */
                    )
{

   HDIR         hDir;                            /* Directory handle          */
   PCHAR        pch;                             /* Character pointer         */
   FILEFINDBUF3 sFileInfo;                       /* File information struct   */
   CHAR         szFullName[ LEN_FILENAME ];      /* Full file name            */
   ULONG        ulCount;                         /* Number file info structs  */
   APIRET       ulRC;                            /* API return code           */

   /*
    * Eliminate terminating backslash in path name if present
    */
   if ( szPathName[ ( ulCount = strlen( szPathName ) - 1 ) ] == '\\' )
   {
      szPathName[ ulCount ] = '\0';
   }

   /*
    * Combine path and file name into full file name
    */
   sprintf( szFullName, "%s\\%s", szPathName, szFileName );

   /*
    * Initialize DosFindFirst input parameters
    */
   hDir    = HDIR_CREATE;
   ulCount = 1;

   /*
    * Quit checking for enablement if no files found and not checking
    * subdirectories or error occurred
    */
   if ( ( ulRC = DosFindFirst( szFullName, &hDir, FILE_NORMAL, &sFileInfo, sizeof( sFileInfo ), &ulCount, FIL_STANDARD ) ) != NO_ERROR )
   {
      if ( ( ( ulRC == ERROR_NO_MORE_FILES ) && ( FLAGCLR( ulFlags, fSOPTION ) ) ) ||
           ( ulRC != ERROR_NO_MORE_FILES                                         ) )
      {
         return;
      }
   }

   /*
    * Process all files matching full file name
    */
   while ( ulRC != ERROR_NO_MORE_FILES )
   {
      /*
       * Combine path and true file name
       */
      sprintf( szInFile, "%s\\%s", szPathName, sFileInfo.achName );

      /*
       * Read executable file and initialize tables if executable file
       */
      if ( InitializeEnablementCheck( ) )
      {
         /*
          * Analyze executable file and check for trace-enablement
          */
         AnalyzeEnablement( );

         /*
          * Increment number of executable files checked
          */
         ulNumFiles++;
      }

      /*
       * Clean up tables
       */
      TerminateEnablementCheck( );

      /*
       * Initialize DosFindNext input parameters
       */
      ulCount = 1;

      /*
       * Quit checking for enablement if no files found
       */
      if ( ( ( ulRC = DosFindNext( hDir, &sFileInfo, sizeof( sFileInfo ), &ulCount ) ) != NO_ERROR ) &&
           ( ulRC != ERROR_NO_MORE_FILES                                                           ) )
      {
         return;
      }
   }

   /*
    * Done searching for executable files
    */
   DosFindClose( hDir );

   /*
    * Check any subdirectories for enablement if -S option specified
    */
   if ( FLAGSET( ulFlags, fSOPTION ) )
   {
      /*
       * Append asterisk to full file name, implying directory name
       */
      pch = strrchr( szFullName, '\\' );
      *pch = '\0';
      strcat( szFullName, "\\*" );

      /*
       * Initialize DosFindFirst input parameters
       */
      hDir    = HDIR_CREATE;
      ulCount = 1;

      /*
       * Quit checking for enablement if no directories found and not checking
       * subdirectories or error occurred
       */
      if ( ( ulRC = DosFindFirst( szFullName, &hDir, FILE_DIRECTORY, &sFileInfo, sizeof( sFileInfo ), &ulCount, FIL_STANDARD ) ) != NO_ERROR )
      {
         if ( ( ( ulRC == ERROR_NO_MORE_FILES ) && ( FLAGCLR( ulFlags, fSOPTION ) ) ) ||
              ( ulRC != ERROR_NO_MORE_FILES                                         ) )
         {
            return;
         }
      }

      /*
       * Process all directories matching full file name
       */
      while ( ulRC != ERROR_NO_MORE_FILES )
      {
         /*
          * Check all files in directory for enablement if directory found
          */
         if ( sFileInfo.attrFile & FILE_DIRECTORY )
         {
            /*
             * Check all files in directory for enablement if directory neither
             * current (".") nor parent ("..")
             */
            if ( ( strcmp( sFileInfo.achName, "." ) != 0  ) &&
                 ( strcmp( sFileInfo.achName, ".." ) != 0 ) )
            {
               /*
                * Combine path and true file name into full file name
                */
               sprintf( szFullName, "%s\\%s", szPathName, sFileInfo.achName );

               /*
                * Recursively check all files in directory for enablement
                */
               CheckEnablement( szFullName, szFileName );
            }
         }

         /*
          * Initialize DosFindNext input parameters
          */
         ulCount = 1;

         /*
          * Quit checking for enablement if no directories found
          */
         if ( ( ( ulRC = DosFindNext( hDir, &sFileInfo, sizeof( sFileInfo ), &ulCount ) ) != NO_ERROR ) &&
              ( ulRC != ERROR_NO_MORE_FILES                                                           ) )
         {
            return;
         }
      }

      /*
       * Done searching for directories
       */
      DosFindClose( hDir );
   }

}  /* CheckEnablement() */

/******************************************************************************/
/*                                                                            */
/* DisplayBanner()                                                            */
/*                                                                            */
/******************************************************************************/

VOID DisplayBanner( VOID )                       /* No parameters             */
{

   /*
    * Display program banner
    */
   printf( "Operating System/2 API Trace-Enabled Executable Checker\n" );
   printf( "Version %1d.%02d.%02d (%s)\n", VER_MAJOR, VER_MINOR, VER_REV, VER_DATE );
   printf( "Written by Dave Blaschke\n" );

}  /* DisplayBanner() */

/******************************************************************************/
/*                                                                            */
/* Error()                                                                    */
/*                                                                            */
/******************************************************************************/

VOID Error( PCHAR szFmt                          /* Error message format      */
          , ...                                  /* Error message inserts     */
          )
{

   va_list pArgs;                                /* Error message inserts     */
   CHAR    szFmtMsg[ LEN_IOLINE ];               /* Formatted error message   */

   /*
    * Format error message including message inserts
    */
   va_start( pArgs, szFmt );
   vsprintf( szFmtMsg, szFmt, pArgs );
   va_end( pArgs );

   /*
    * Display error message
    */
   printf( "%s: %s\n", szAppName, szFmtMsg );

   /*
    * Terminate program
    */
   exit( ERROR );

}  /* Error() */

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
    * Clean up tables if being removed from exception handler chain
    */
   if ( FLAGSET( pRep->fHandlerFlags, EH_UNWINDING ) )
   {
      /*
       * Clean up trace enablement tables
       */
      TerminateEnablementCheck( );
   }

   /*
    * Indicate ignore process terminate exception
    */
   else if ( pRep->ExceptionNum == XCPT_PROCESS_TERMINATE )
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
/* InitializeEnablementCheck()                                                */
/*                                                                            */
/******************************************************************************/

BOOL InitializeEnablementCheck( VOID )           /* No parameters             */
{

   int     i;                                    /* Loop variable             */
   PCHAR   pch;                                  /* Character pointer         */
   CHAR    szModNam[ LEN_FILENAME ];             /* Module name               */
   ULONG   ulBaseAddr;                           /* Object base address       */
   ULONG   ulDLLIdx;                             /* DLL information index     */

   /*
    * Initialize flags in DLL information table to zero
    */
   for ( i = 0; i < SUPPORTED_DLL; i++ )
   {
      DLLInfo[ i ].ulFlags = 0L;
   }

   /*
    * Open executable file, indicate failure if unsuccessful
    */
   if ( !( fpInFile = fopen( szInFile, "rb+" ) ) )
   {
      return FALSE;
   }

   /*
    * Read DOS header and identify DOS or OS/2 signature, process OS/2 header
    * if successful
    */
   if ( ( fread( &sMZHdr, sizeof( sMZHdr ), 1, fpInFile ) == 1 ) &&
        ( ( sMZHdr.e_magic == EMAGIC   ) ||
          ( sMZHdr.e_magic == NEMAGIC  ) ||
          ( sMZHdr.e_magic == E32MAGIC )                       ) )
   {
      /*
       * Zero DOS header if DOS header not present
       */
      if ( sMZHdr.e_magic != EMAGIC )
      {
         memset( &sMZHdr, 0, sizeof( MZHDR ) );
      }

      /*
       * Locate beginning of OS/2 header and read OS/2 header; indicate failure
       * if unsuccessful
       */
      if ( ( fseek( fpInFile, 0L, SEEK_END )                        ) ||
           ( ftell( fpInFile ) < sMZHdr.e_lfanew + sizeof( sLXHdr ) ) ||
           ( fseek( fpInFile, sMZHdr.e_lfanew, SEEK_SET )           ) ||
           ( fread( &sLXHdr, sizeof( sLXHdr ), 1, fpInFile ) != 1   ) )
      {
         return FALSE;
      }

      /*
       * Executable file follows segmented format if magic number NE
       */
      if ( *( (PUSHORT)sLXHdr.e32_magic ) == NEMAGIC )
      {
         /*
          * Indicate NE executable format
          */
         SETFLAG( ulFlags, fNE_FMT );

         /*
          * Obtain 16-bit executable header and sector size
          */
         pNEHdr = (NEHDR *)&sLXHdr;
         usSector = 1 << pNEHdr->ne_align;

         /*
          * Indicate failure if module reference table empty (executable file
          * does not import anything)
          */
         if ( !( ulModIdxTbl = pNEHdr->ne_cmod ) )
         {
            return FALSE;
         }

         /*
          * Allocate memory for and read module reference table
          */
         if ( ( ulModRefTbl = ulModIdxTbl * sizeof( USHORT ) ) > 0L )
         {
            /*
             * Allocate memory for module reference table, indicate failire if
             * unsuccessful
             */
            if ( !( pModRefTbl = malloc( ulModRefTbl ) ) )
            {
               return FALSE;
            }

            /*
             * Locate beginning of module reference table and read module
             * reference table; indicate failure if unsuccessful
             */
            if ( ( fseek( fpInFile, sMZHdr.e_lfanew + pNEHdr->ne_modtab, SEEK_SET ) ) ||
                 ( fread( pModRefTbl, ulModRefTbl, 1, fpInFile ) != 1               ) )
            {
               return FALSE;
            }
         }

         /*
          * Allocate memory for and read import name table if one exists
          */
         if ( ( ulImpNamTbl = pNEHdr->ne_nrestab - ( sMZHdr.e_lfanew + pNEHdr->ne_imptab ) ) > 0L )
         {
            /*
             * Allocate memory for import name table, indicate failure if
             * unsuccessful
             */
            if ( !( pImpNamTbl = malloc( ulImpNamTbl ) ) )
            {
               return FALSE;
            }

            /*
             * Locate beginning of import name table and read import name
             * table; indicate failure if unsuccessful
             */
            if ( ( fseek( fpInFile, sMZHdr.e_lfanew + pNEHdr->ne_imptab, SEEK_SET ) ) ||
                 ( fread( pImpNamTbl, ulImpNamTbl, 1, fpInFile ) != 1               ) )
            {
               return FALSE;
            }
         }

         /*
          * Obtain number of segments
          */
         ulSegment = pNEHdr->ne_cseg;

         /*
          * Allocate memory for and read segment table if one exists
          */
         if ( ( ulSegmentTbl = ulSegment * sizeof( NESEG ) ) > 0L )
         {
            /*
             * Allocate memory for segment table, indicate failure if
             * unsuccessful
             */
            if ( !( pSegmentTbl = malloc( ulSegmentTbl ) ) )
            {
               return FALSE;
            }

            /*
             * Locate beginning of segment table and read segment table;
             * indicate failure if unsuccessful
             */
            if ( ( fseek( fpInFile, sMZHdr.e_lfanew + pNEHdr->ne_segtab, SEEK_SET ) ) ||
                 ( fread( pSegmentTbl, ulSegmentTbl, 1, fpInFile ) != 1             ) )
            {
               return FALSE;
            }
         }

         /*
          * Allocate memory for module index table, indicate failure if
          * unsuccessful
          */
         if ( !( pModIdxTbl = malloc( ulModIdxTbl * sizeof( ULONG ) ) ) )
         {
            return FALSE;
         }

         /*
          * Initialize module index table
          */
         for ( i = 0; i < ulModIdxTbl; i++ )
         {
            /*
             * Obtain module name
             */
            pch = (PCHAR)( (ULONG)pImpNamTbl + pModRefTbl[ i ] );
            memcpy( szModNam, pch + 1, *pch );
            szModNam[ *pch ] = '\0';

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate trace DLL if supported trace DLL
             */
            if ( ( pModIdxTbl[ i ] = LookupTraceDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fTRACEDLL );
            }
         }
      }

      /*
       * Executable file follows linear format if magic number LX
       */
      else if ( *( (PUSHORT)sLXHdr.e32_magic ) == E32MAGIC )
      {
         /*
          * Indicate LX executable format
          */
         SETFLAG( ulFlags, fLX_FMT );

         /*
          * Indicate failure if module name table empty (executable file does
          * not import anything)
          */
         if ( !( ulModIdxTbl = sLXHdr.e32_impmodcnt ) )
         {
            return FALSE;
         }

         /*
          * Allocate memory for and read module name table
          */
         if ( ( ulModNamTbl = sLXHdr.e32_impproc - sLXHdr.e32_impmod ) > 0L )
         {
            /*
             * Allocate memory for module name table, indicate failure if
             * unsuccessful
             */
            if ( !( pModNamTbl = malloc( ulModNamTbl ) ) )
            {
               return FALSE;
            }

            /*
             * Locate beginning of module name table and read module name
             * table; indicate failure if unsuccessful
             */
            if ( ( fseek( fpInFile, sMZHdr.e_lfanew + sLXHdr.e32_impmod, SEEK_SET ) ) ||
                 ( fread( pModNamTbl, ulModNamTbl, 1, fpInFile ) != 1               ) )
            {
               return FALSE;
            }
         }

         /*
          * Read procedure name table and fixup table if enabling API tracing
          */
         if ( FLAGSET( ulFlags, fTRACE_ON ) )
         {
            /*
             * Allocate memory for and read procedure name table if one exists
             */
            if ( ( ulPrcNamTbl = sLXHdr.e32_fpagetab + sLXHdr.e32_fixupsize - sLXHdr.e32_impproc ) > 0L )
            {
               /*
                * Allocate memory for procedure name table, indicate failure if
                * unsuccessful
                */
               if ( !( pPrcNamTbl = malloc( ulPrcNamTbl ) ) )
               {
                  return FALSE;
               }

               /*
                * Locate beginning of procedure name table and read procedure
                * name table; indicate failure if unsuccessful
                */
               if ( ( fseek( fpInFile, sMZHdr.e_lfanew + sLXHdr.e32_impproc, SEEK_SET ) ) ||
                    ( fread( pPrcNamTbl, ulPrcNamTbl, 1, fpInFile ) != 1                ) )
               {
                  return FALSE;
               }
            }

            /*
             * Allocate memory for and read fixup table if one exists
             */
            if ( ( ulFixupTbl = sLXHdr.e32_impmod - sLXHdr.e32_frectab ) > 0L )
            {
               /*
                * Allocate memory for fixup record table, indicate failure if
                * unsuccessful
                */
               if ( !( pFixupTbl = malloc( ulFixupTbl ) ) )
               {
                  return FALSE;
               }

               /*
                * Locate beginning of fixup record table and read fixup record
                * table; indicate failure if unsuccessful
                */
               if ( ( fseek( fpInFile, sMZHdr.e_lfanew + sLXHdr.e32_frectab, SEEK_SET ) ) ||
                    ( fread( pFixupTbl, ulFixupTbl, 1, fpInFile ) != 1                  ) )
               {
                  return FALSE;
               }
            }
         }

         /*
          * Allocate memory for module index table, indicate failure if
          * unsuccessful
          */
         if ( !( pModIdxTbl = malloc( ulModIdxTbl * sizeof( ULONG ) ) ) )
         {
            return FALSE;
         }

         /*
          * Initialize module index table
          */
         for ( i = 0, pch = pModNamTbl; i < ulModIdxTbl; i++, pch += *pch + 1 )
         {
            /*
             * Obtain module name
             */
            memcpy( szModNam, pch + 1, *pch );
            szModNam[ *pch ] = '\0';

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate trace DLL if supported trace DLL
             */
            if ( ( pModIdxTbl[ i ] = LookupTraceDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fTRACEDLL );
            }
         }
      }

      /*
       * Indicate failure if magic number unrecognized
       */
      else
      {
         return FALSE;
      }
   }

   /*
    * Indicate failure if DOS signature absent
    */
   else
   {
      return FALSE;
   }

   /*
    * Indicate success
    */
   return TRUE;

}  /* InitializeEnablementCheck() */

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
/* ParseCommandLine()                                                         */
/*                                                                            */
/******************************************************************************/

VOID ParseCommandLine( int    argc               /* Argument count            */
                     , char **argv               /* Argument pointer          */
                     )
{

   /*
    * Issue error and terminate if no command line arguments specified
    */
   if ( !argc )
   {
      Error( "Missing command line arguments, use \'-?\' to display help" );
   }

   /*
    * Parse each command line argument
    */
   while ( argc-- )
   {
      /*
       * Valid command line option starts with dash or slash
       */
      if ( ( argv[ 0 ][ 0 ] == '-' ) || ( argv[ 0 ][ 0 ] == '/' ) )
      {
         /*
          * Display help if -?, -H, or -HELP specified
          */
         if ( ( !Strcmpi( &argv[ 0 ][ 1 ], "?" ) ) || ( !Strcmpi( &argv[ 0 ][ 1 ], "H" ) ) || ( !Strcmpi( &argv[ 0 ][ 1 ], "HELP" ) ) )
         {
            /*
             * Display help
             */
            Usage( );
         }

         /*
          * Display DLLs enabled for tracing if -D specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "D" ) )
         {
            /*
             * Indicate trace enablement option specified
             */
            SETFLAG( ulFlags, fDOPTION );
         }

         /*
          * Check subdirectories if -S specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "S" ) )
         {
            /*
             * Indicate trace enablement option specified
             */
            SETFLAG( ulFlags, fSOPTION );
         }

         /*
          * Issue error and terminate if unrecognized command line option
          */
         else
         {
            Error( "Unrecognized command line option: %s", argv[ 0 ] );
         }
      }

      /*
       * Valid file name starts with any other character
       */
      else
      {
         /*
          * Issue error and terminate if file name already specified
          */
         if ( szInFile[ 0 ] )
         {
            Error( "Extra file name: %s", argv[ 0 ] );
         }

         /*
          * Save file name
          */
         strcpy( szInFile, argv[ 0 ] );
      }

      /*
       * Skip command line argument
       */
      argv++;
   }

   /*
    * Issue error and terminate if trace enablement or trace summarization
    * specified and file name missing
    */
   if ( !szInFile[ 0 ] )
   {
      Error( "Missing file name" );
   }

}  /* ParseCommandLine() */

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
/* TerminateEnablementCheck()                                                 */
/*                                                                            */
/******************************************************************************/

VOID TerminateEnablementCheck( VOID )            /* No parameters             */
{

   /*
    * Free memory for dynamic linking information table if one exists
    */
   if ( pDynLnkInf )
   {
      free( pDynLnkInf );
      pDynLnkInf = NULL;
   }

   /*
    * Free memory for fixup record table if one exists
    */
   if ( pFixupTbl )
   {
      free( pFixupTbl );
      pFixupTbl = NULL;
   }

   /*
    * Free memory for import name table if one exists
    */
   if ( pImpNamTbl )
   {
      free( pImpNamTbl );
      pImpNamTbl = NULL;
   }

   /*
    * Free memory for module index table if one exists
    */
   if ( pModIdxTbl )
   {
      free( pModIdxTbl );
      pModIdxTbl = NULL;
   }

   /*
    * Free memory for module name table if one exists
    */
   if ( pModNamTbl )
   {
      free( pModNamTbl );
      pModNamTbl = NULL;
   }

   /*
    * Free memory for module reference table if one exists
    */
   if ( pModRefTbl )
   {
      free( pModRefTbl );
      pModRefTbl = NULL;
   }

   /*
    * Free memory for procedure name table if one exists
    */
   if ( pPrcNamTbl )
   {
      free( pPrcNamTbl );
      pPrcNamTbl = NULL;
   }

   /*
    * Free memory for program header table if one exists
    */
   if ( pProgHdr )
   {
      free( pProgHdr );
      pProgHdr = NULL;
   }

   /*
    * Free memory for string table if one exists
    */
   if ( pStrTbl )
   {
      free( pStrTbl );
      pStrTbl = NULL;
   }

   /*
    * Free memory for segment if one exists
    */
   if ( pSegment )
   {
      free( pSegment );
      pSegment = NULL;
   }

   /*
    * Free memory for segment table if one exists
    */
   if ( pSegmentTbl )
   {
      free( pSegmentTbl );
      pSegmentTbl = NULL;
   }

   /*
    * Close executable file if opened
    */
   if ( fpInFile )
   {
      fclose( fpInFile );
   }

}  /* TerminateEnablementCheck() */

/******************************************************************************/
/*                                                                            */
/* Usage()                                                                    */
/*                                                                            */
/******************************************************************************/

VOID Usage( VOID )                               /* No parameters             */
{

   /*
    * Display helping information
    */
   printf( "Usage:\n" );
   printf( "\t%s [-D] [-S] file\n", szAppName );
   printf( "Where:\n" );
   printf( "\t-D   indicates display DLLs enabled for tracing\n" );
   printf( "\t-S   indicates check subdirectories\n" );
   printf( "\tfile indicates name of  executable file(s)  to be checked\n" );
   printf( "\t     (can include global file name characters * and ?)\n" );

   /*
    * Terminate program
    */
   exit( NO_ERROR );

}  /* Usage() */

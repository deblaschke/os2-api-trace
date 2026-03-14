/******************************************************************************/
/*                                                                            */
/* File name    : STRIPAPI.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace information file API stripper                */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Strips API  entries and/or exits  from a trace  information */
/*                file.                                                       */
/*                                                                            */
/*                This feat is  accomplished by  processing each  line within */
/*                the trace information file.  Whenever an API  entry/exit is */
/*                encountered,  it is  dissected to  determine  which  API is */
/*                being entered/exited.  If it is an entry/exit that does not */
/*                match  the  requested  API,   the  complete  entry/exit  is */
/*                displayed to standard output.                               */
/*                                                                            */
/*                The user can  request to  strip API entries  from the trace */
/*                information file by specifying the -ENTRY option.  The user */
/*                can request to strip  API exits from the  trace information */
/*                file by specifying the -EXIT  option.  The user can request */
/*                to suppress displaying the program banner by specifying the */
/*                -Q option.                                                  */
/*                                                                            */
/*                The user can request helping  information by specifying the */
/*                -?, -H, or -HELP option.                                    */
/*                                                                            */
/* Installation : Place the executable,  STRIPAPI.EXE,  in a directory  along */
/*                the PATH environment variable.                              */
/*                                                                            */
/* Usage        : STRIPAPI {-ENTRY|-EXIT}... [-Q] api file                    */
/*                                                                            */
/*                Where:                                                      */
/*                  -ENTRY indicates strip API entries                        */
/*                  -EXIT  indicates strip API exits                          */
/*                  -Q     indicates suppress displaying banner               */
/*                  api    indicates name of API to be stripped               */
/*                  file   indicates name  of trace  information  file to  be */
/*                         stripped                                           */
/*                                                                            */
/*                In the following example,  all Dos32CreateQueue entries are */
/*                stripped from TEST.TRC:                                     */
/*                                                                            */
/*                STRIPAPI -ENTRY DOS32CREATEQUEUE TEST.TRC                   */
/*                                                                            */
/*                In  the  following  example,  the  program  banner  is  not */
/*                displayed and all Dos32CreateQueue  exits are stripped from */
/*                TEST.TRC:                                                   */
/*                                                                            */
/*                STRIPAPI -EXIT -Q DOS32CREATEQUEUE TEST.TRC                 */
/*                                                                            */
/*                In the following example,  all Dos32CreateQueue entries and */
/*                exits are stripped from TEST.TRC:                           */
/*                                                                            */
/*                STRIPAPI -ENTRY -EXIT DOS32CREATEQUEUE TEST.TRC             */
/*                                                                            */
/* Output       : The contents of the  trace information file  minus the  API */
/*                entries and/or exits.                                       */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.45.25 03Jun00 Added trace information file API stripper   */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#define  INCL_BASE                               /* Include base functions    */
#include <os2.h>                                 /* OS/2 information          */
#include <ctype.h>                               /* C information             */
#include <stdarg.h>                              /* C information             */
#include <stdio.h>                               /* C information             */
#include <stdlib.h>                              /* C information             */
#include <string.h>                              /* C information             */
#include "os2trace.h"                            /* OS2TRACE information      */
#include "stripapi.h"                            /* STRIPAPI information      */

/******************************************************************************/
/*                                                                            */
/* Function declarations                                                      */
/*                                                                            */
/******************************************************************************/

void           main( int, char ** );
VOID           DisplayBanner( VOID );
VOID           Error( PCHAR, ... );
ULONG APIENTRY ExceptionHandler( PREPREC, PREGREC, PCTXREC, PVOID );
VOID           ParseCommandLine( int, char ** );
LONG           Strcmpi( PCHAR, PCHAR );
VOID           StripAPI( VOID );
VOID           Usage( VOID );

/******************************************************************************/
/*                                                                            */
/* Variable definitions                                                       */
/*                                                                            */
/******************************************************************************/

FILE  *fpInFile;                                 /* Input file pointer        */
PCHAR  szAppName = VER_STRNAME;                  /* STRIPAPI application name */
CHAR   szInAPI[ LEN_FILENAME ];                  /* Input API name            */
CHAR   szInFile[ LEN_FILENAME ];                 /* Input file name           */
ULONG  ulFlags;                                  /* Flags                     */

/******************************************************************************/
/*                                                                            */
/* main()                                                                     */
/*                                                                            */
/******************************************************************************/

void main( int    argc                           /* Argument count            */
         , char **argv                           /* Argument pointer          */
         )
{

   REGREC sRegRec;                               /* Registration record       */

   /*
    * Register exception handler
    */
   sRegRec.ExceptionHandler = ExceptionHandler;
   DosSetExceptionHandler( &sRegRec );

   /*
    * Parse command line arguments
    */
   ParseCommandLine( --argc, ++argv );

   /*
    * Display program banner if -Q option not specified
    */
   if ( FLAGCLR( ulFlags, fQOPTION ) )
   {
      DisplayBanner( );
   }

   /*
    * Strip API entries/exits from trace information file
    */
   StripAPI( );

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
/* DisplayBanner()                                                            */
/*                                                                            */
/******************************************************************************/

VOID DisplayBanner( VOID )                       /* No parameters             */
{

   /*
    * Display program banner
    */
   printf( "Operating System/2 API Trace Information File API Stripper\n" );
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
    * Display program banner if -Q option not specified
    */
   if ( FLAGCLR( ulFlags, fQOPTION ) )
   {
      DisplayBanner( );
   }

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
    * Clean up if being removed from exception handler chain
    */
   if ( FLAGSET( pRep->fHandlerFlags, EH_UNWINDING ) )
   {
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
/* ParseCommandLine()                                                         */
/*                                                                            */
/******************************************************************************/

VOID ParseCommandLine( int    argc               /* Argument count            */
                     , char **argv               /* Argument pointer          */
                     )
{

   BOOL  bAllGrp;                                /* All API groups indicator  */
   BOOL  bSetBit;                                /* Set bit(s) indicator      */
   int   i;                                      /* Loop and temp variable    */
   PCHAR szGroup;                                /* API groups string         */

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
          * Strip API entries if -ENTRY specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "ENTRY" ) )
         {
            /*
             * Indicate -ENTRY option specified
             */
            SETFLAG( ulFlags, fENTRYOPT );
         }

         /*
          * Strip API exits if -EXIT specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "EXIT" ) )
         {
            /*
             * Indicate -EXIT option specified
             */
            SETFLAG( ulFlags, fEXITOPT );
         }

         /*
          * Suppress program banner if -Q specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "Q" ) )
         {
            /*
             * Indicate -Q option specified
             */
            SETFLAG( ulFlags, fQOPTION );
         }
      }

      /*
       * Valid API and file names start with any other character
       */
      else
      {
         /*
          * Save file name if API name already specified
          */
         if ( szInAPI[ 0 ] )
         {
            /*
             * Issue error and terminate if file name already specified
             */
            if ( szInFile[ 0 ] )
            {
               Error( "Extra parameter: %s", argv[ 0 ] );
            }

            /*
             * Save file name
             */
            strcpy( szInFile, argv[ 0 ] );
         }

         /*
          * Save API name if API name not already specified
          */
         else
         {
            strcpy( szInAPI, argv[ 0 ] );
         }
      }

      /*
       * Skip command line argument
       */
      argv++;
   }

   /*
    * Issue error and terminate if flag missing
    */
   if ( FLAGCLR( ulFlags, fENTRYOPT | fEXITOPT ) )
   {
      Error( "Missing option(s)" );
   }

   /*
    * Issue error and terminate if API name missing
    */
   else if ( !szInAPI[ 0 ] )
   {
      Error( "Missing API name" );
   }

   /*
    * Issue error and terminate if file name missing
    */
   else if ( !szInFile[ 0 ] )
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
/* StripAPI()                                                                 */
/*                                                                            */
/******************************************************************************/

VOID StripAPI( VOID )
{

   BOOL bEcho = TRUE;                            /* Echo line indicator       */
   CHAR szAPIName[ MAX_APINAME ];                /* API name                  */
   CHAR szEntExt[ LEN_IOLINE ];                  /* API entry/exit string     */
   CHAR szLine[ LEN_IOLINE ];                    /* Trace info file line      */
   CHAR szTemp[ LEN_IOLINE ];                    /* Temporary string          */

   /*
    * Open trace information file, issue error and terminate if unsuccessful
    */
   if ( !( fpInFile = fopen( szInFile, "r" ) ) )
   {
      Error( "Unable to open file: %s", szInFile );
   }

   /*
    * Read, parse, and analyze each line in trace information file
    */
   while ( fgets( szLine, LEN_IOLINE, fpInFile ) )
   {
      /*
       * There is one line to look for that indicates the entry to an API call:
       *
       *  PID  TID | API Entry
       *
       * And there is one line to look for that indicate the exit from an API
       * call:
       *
       *  PID  TID | API Exit
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
            bEcho = ( ( Strcmpi( szInAPI, szAPIName ) ) || ( FLAGCLR( ulFlags, fENTRYOPT ) ) );
         }

         /*
          * API exit line found
          */
         else if ( !strncmp( szEntExt, "Exit", 4 ) )
         {
            bEcho = ( ( Strcmpi( szInAPI, szAPIName ) ) || ( FLAGCLR( ulFlags, fEXITOPT ) ) );
         }
      }

      /*
       * Echo line if so indicated
       */
      if ( bEcho )
      {
          printf( "%s", szLine );
      }
   }

   /*
    * Close trace information file
    */
   fclose( fpInFile );

}  /* StripAPI() */

/******************************************************************************/
/*                                                                            */
/* Usage()                                                                    */
/*                                                                            */
/******************************************************************************/

VOID Usage( VOID )                               /* No parameters             */
{

   int i;                                        /* Loop variable             */

   /*
    * Display program banner
    */
   DisplayBanner();

   /*
    * Display trace enablement helping information
    */
   printf( "Usage:\n" );
   printf( "\t%s {-ENTRY|-EXIT}... [-Q] api file\n", szAppName );
   printf( "Where:\n" );
   printf( "\t-ENTRY indicates strip API entries\n" );
   printf( "\t-EXIT  indicates strip API exits\n" );
   printf( "\t-Q     indicates suppress displaying banner\n" );
   printf( "\tapi    indicates name of API to be stripped\n" );
   printf( "\tfile   indicates name of trace information file to be stripped\n" );

   /*
    * Terminate program
    */
   exit( NO_ERROR );

}  /* Usage() */

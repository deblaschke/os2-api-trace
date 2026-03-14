/******************************************************************************/
/*                                                                            */
/* File name    : MARKOBJ.C                                                   */
/*                                                                            */
/* Title        : OS/2 API trace DLL object file segment attributes marker    */
/*                routine                                                     */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Changes all "word-aligned private Use16" segments within an */
/*                OMF-conforming object file to "word-aligned public Use16."  */
/*                                                                            */
/* Notes        : This executable must change  all word-aligned private Use16 */
/*                segment attributes in the  following trace DLL object files */
/*                to  word-aligned  public Use16  in  order  to  fix  an  IBM */
/*                VisualAge C++ compiler problem:                             */
/*                                                                            */
/*                  T_DOS___.OBJ                                              */
/*                  T_DRG_.OBJ                                                */
/*                  T_GPI.OBJ                                                 */
/*                  T_HLP__.OBJ                                               */
/*                  T_KBD___.OBJ                                              */
/*                  T_M.OBJ                                                   */
/*                  T_MON___.OBJ                                              */
/*                  T_MOU___.OBJ                                              */
/*                  T_N.OBJ                                                   */
/*                  T_PIC.OBJ                                                 */
/*                  T_PIP___.OBJ                                              */
/*                  T_PVP_.OBJ                                                */
/*                  T_QUE___.OBJ                                              */
/*                  T_SES_.OBJ                                                */
/*                  T_SHL__.OBJ                                               */
/*                  T_SPL.OBJ                                                 */
/*                  T_VIO___.OBJ                                              */
/*                  T_WIN.OBJ                                                 */
/*                                                                            */
/*                The problem in the compiler is that the back end is marking */
/*                the  16-bit segments  it creates for  thunking purposes  as */
/*                unique  instead  of  public,   thereby  prohibiting  module */
/*                definitions files  from defining segment  attributes in the */
/*                following manner:                                           */
/*                                                                            */
/*                  SEGMENTS CODE16 CLASS 'CODE' CONFORMING IOPL              */
/*                                                                            */
/*                This executable changes those private segments to public.   */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.18 30Apr97 Added 16-bit Kbd APIs                       */
/*                2.40.19 01May97 Added 16-bit Mou APIs                       */
/*                2.40.20 05May97 Added 16-bit Vio APIs                       */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
/*                2.40.27 22Jul97 Added 16-bit Gpi APIs                       */
/*                2.40.29 07Aug97 Added 16-bit Drg APIs                       */
/*                2.40.30 08Aug97 Added 16-bit Pic APIs                       */
/*                2.40.32 11Aug97 Added 16-bit Prf APIs                       */
/*                2.40.33 11Aug97 Added 16-bit Prt APIs                       */
/*                2.40.47 29Jan98 Added Win16DefAVioWindowProc                */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <os2.h>                                 /* OS/2 information          */
#include <malloc.h>                              /* C information             */
#include <stdio.h>                               /* C information             */
#include <stdlib.h>                              /* C information             */

/******************************************************************************/
/*                                                                            */
/* Constant                                                                   */
/*                                                                            */
/******************************************************************************/

#define NO_ERROR 0                               /* Normal termination code   */
#define ERROR    1                               /* Abnormal termination code */

/******************************************************************************/
/*                                                                            */
/* main()                                                                     */
/*                                                                            */
/******************************************************************************/

void main( int    argc                           /* Argument count            */
         , char **argv                           /* Argument pointer          */
         )
{

   BOOL   bWrite = FALSE;                        /* Write file indicator      */
   FILE  *fpObjFile;                             /* Object file               */
   int    i;                                     /* Loop variable             */
   PCHAR  pchObjFile;                            /* Object file contents      */
   ULONG  ulObjFile;                             /* Object file size          */

   /*
    * Issue error and terminate if object file name not specified
    */
   if ( argc != 2 )
   {
      fprintf( stderr, "Usage: MARKOBJ file" );
      exit( ERROR + 0 );
   }

   /*
    * Open object file, issue error and terminate if unsuccessful
    */
   if ( !( fpObjFile = fopen( argv[ 1 ], "rb+" ) ) )
   {
      fprintf( stderr, "Unable to open object file: %s\n", argv[ 1 ] );
      exit( ERROR + 1 );
   }

   /*
    * Determine object file length, issue error and terminate if unsuccessful
    */
   if ( ( fseek( fpObjFile, 0L, SEEK_END )         ) ||
        ( ( ulObjFile = ftell( fpObjFile ) ) == 0L ) )
   {
      fprintf( stderr, "Unable to determine object file length: %s\n", argv[ 1 ] );
      exit( ERROR + 2 );
   }

   /*
    * Allocate memory for object file, issue error and terminate if
    * unsuccessful
    */
   if ( !( pchObjFile = malloc( ulObjFile ) ) )
   {
      fprintf( stderr, "Unable to allocate memory for object file: %s\n", argv[ 1 ] );
      exit( ERROR + 3 );
   }

   /*
    * Locate beginning of object file and read file; issue error and terminate
    * if unsuccessful
    */
   if ( ( fseek( fpObjFile, 0L, SEEK_SET )                  ) ||
        ( fread( pchObjFile, ulObjFile, 1, fpObjFile ) != 1 ) )
   {
      fprintf( stderr, "Unable to read object file: %s\n", argv[ 1 ] );
      exit( ERROR + 4 );
   }

   /*
    * Issue error and terminate if THEADR record not first record
    */
   if ( pchObjFile[ i = 0 ] != 0x80 )
   {
      fprintf( stderr, "Object file does not follow OMF: %s\n", argv[ 1 ] );
      exit( ERROR + 5 );
   }

   /*
    * Process each record in object file
    */
   while ( i < ulObjFile )
   {
      /*
       * Change segment attributes if SEGDEF record and segment word aligned
       * and segment Use16 and segment private
       */
      if ( ( pchObjFile[ i ] == 0x99                ) &&
           ( ( pchObjFile[ i + 3 ] & 0xE0 ) == 0x40 ) &&
           ( ( pchObjFile[ i + 3 ] & 0x01 ) == 0x00 ) &&
           ( ( pchObjFile[ i + 3 ] & 0x1C ) == 0x00 ) )
      {
         printf( "Changing SEGDEF segment attributes to %x\n", pchObjFile[ i + 3 ] |= 0x08 );
         bWrite = TRUE;
      }

      /*
       * Skip remainder of record
       */
      i += *((PUSHORT)&pchObjFile[ i + 1 ]) + 3;
   }

   /*
    * Write object file if write file indicated
    */
   if ( bWrite )
   {
      /*
       * Locate beginning of object file and write file; issue error and
       * terminate if unsuccessful
       */
      if ( ( fseek( fpObjFile, 0, SEEK_SET )                    ) ||
           ( fwrite( pchObjFile, ulObjFile, 1, fpObjFile ) != 1 ) )
      {
         fprintf( stderr, "Unable to write object file: %s\n", argv[ 1 ] );
         exit( ERROR + 6 );
      }
   }

   /*
    * Terminate program
    */
   exit( NO_ERROR );

}  /* main() */

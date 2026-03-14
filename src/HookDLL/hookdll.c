#define INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <string.h>

/* Constants                                                                 */
#define BUFSIZE 4096

/* Global variables                                                          */
CHAR    chBuffer[ BUFSIZE ];
HMODULE hmodDOSCALLS;

/* External procedures                                                       */
APIRET (* APIENTRY pfnDOS32QUERYMODFROMEIP)( PHMODULE, PULONG, ULONG,
                                             PCHAR, PULONG, ULONG );

/* Internal procedures                                                       */
BOOL AddrToMod( PHMODULE, PULONG, ULONG, PCHAR, PULONG, ULONG );
BOOL IsValidMem( PVOID, ULONG );

/* LogStackTrace: Traces the stack upon entry to a 32-bit OS/2 API, logging  */
/*                the following items for each chained call frame:           */
/*                  Return address                                           */
/*                  Module name, object number and offset of return address  */
/*                  First four parameters on stack                           */

PSZ APIENTRY LogStackTrace( ULONG ulReason      /* Invocation reason         */
                          , PSZ   pszAPIName    /* API name                  */
                          , PVOID pAPIParms     /* API parameters            */
                          , PVOID pAPIRC        /* API return code           */
                          )
{

   PULONG EBP;
   int    i;
   CHAR   szModName[ 260 ];
   ULONG  ulCaller;
   ULONG  ulModule;
   ULONG  ulObjNum;
   ULONG  ulOffset;

   chBuffer[ 0 ] = '\0';

   switch ( ulReason ) {
      case 0: /* Initialization */
         if ( DosLoadModule( chBuffer, sizeof( chBuffer ), "DOSCALLS",
                             &hmodDOSCALLS ) ) {
            hmodDOSCALLS = 0;
         } else if ( DosQueryProcAddr( hmodDOSCALLS, 360, NULL,
                                       (PFN *)&pfnDOS32QUERYMODFROMEIP ) ) {
            pfnDOS32QUERYMODFROMEIP = NULL;
         }

         sprintf( chBuffer, "Initialized, DosQueryModFromEIP %s available",
                  pfnDOS32QUERYMODFROMEIP == NULL ? "is not" : "is" );

         break;

      case 1: /* Termination */
         if ( hmodDOSCALLS ) {
            DosFreeModule( hmodDOSCALLS );
         }

         strcpy( chBuffer, "Terminated" );

         break;

      case 2: /* API Entry */
         if ( ( pszAPIName[ 3 ] == '3' ) && ( pszAPIName[ 4 ] == '2' ) ) {
            EBP = ((PULONG)&ulReason) - 2; /* EBP -> LogStackTrace frame */
            i   = sprintf( chBuffer, "%s's 32-bit stack trace\n", pszAPIName );

            while ( ( EBP != NULL                        ) &&
                    ( IsValidMem( EBP, sizeof( ULONG ) ) ) &&
                    ( i < BUFSIZE - 256 )                ) {
               /* Add function address and name (if available) to buffer */
               if ( AddrToMod( &ulModule, &ulObjNum, sizeof( szModName ),
                               szModName, &ulOffset, *(EBP+1) ) ) {
                  i += sprintf( &chBuffer[ i ],
                                "0x%08lX (%s %04lX:%08lX)",
                                *(EBP+1), szModName, ulObjNum, ulOffset );
               } else {
                  i += sprintf( &chBuffer[ i ], "0x%08lX", *(EBP+1) );
               }

               /* Add four parameters (if available) to buffer */
               if ( IsValidMem( EBP + 2, 4 * sizeof( ULONG ) ) ) {
                  i += sprintf( &chBuffer[ i ],
                                " [P1 = 0x%08lX, P2 = 0x%08lX,"
                                " P3 = 0x%08lX, P4 = 0x%08lX]\n",
                                *(EBP+2), *(EBP+3), *(EBP+4), *(EBP+5) );
               } else {
                  i += sprintf( &chBuffer[ i ], "\n" );
               }

               EBP = (PULONG)*EBP;
            }
         } else if ( ( pszAPIName[ 3 ] == '1' ) && ( pszAPIName[ 4 ] == '6' ) ) {
            sprintf( chBuffer, "%s is 16-bit API type", pszAPIName );
         } else {
            sprintf( chBuffer, "%s is unknown API type", pszAPIName );
         }

         break;

      case 3: /* API Exit */
         break;
   }

   return chBuffer;
}

/* AddrToMod: Converts a linear address to a module name, object number and  */
/*            offset, returning TRUE if successful, FALSE if not             */

BOOL AddrToMod( PHMODULE pulModule              /* Module handle             */
              , PULONG   pulObject              /* Object number             */
              , ULONG    ulBufLen               /* Module name buffer length */
              , PCHAR    szBuffer               /* Module name buffer        */
              , PULONG   pulOffset              /* Offset                    */
              , ULONG    ulAddress              /* Address                   */
              )
{

   if ( ( pfnDOS32QUERYMODFROMEIP                                 ) &&
        ( !( *pfnDOS32QUERYMODFROMEIP )( pulModule, pulObject,
                                         ulBufLen, szBuffer,
                                         pulOffset, ulAddress ) ) ) {
      (*pulObject)++;
      return TRUE;
   } else {
      return FALSE;
   }

}

/* IsValidMem: Determines if a linear address is valid, returning TRUE if    */
/*             valid, FALSE if not                                           */

BOOL IsValidMem( PVOID pBase                    /* Memory base address       */
               , ULONG ulLength                 /* Memory length             */
               )
{

   ULONG ulActual = ulLength;
   ULONG ulFlags;

   return ( DosQueryMem( pBase, &ulActual, &ulFlags ) ) ||
          ( ulLength > ulActual                       ) ||
          ( !( ulFlags & PAG_COMMIT )                 ) ? FALSE : TRUE;
}

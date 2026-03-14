/******************************************************************************/
/*                                                                            */
/* File name    : CHK4TRC.H                                                   */
/*                                                                            */
/* Title        : OS/2 API trace-enabled executable checker include file      */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.57 25Jun98 Added trace-enabled executable checker      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#define VER_CHKNAME   "CHK4TRC"                  /* Trace-enabled checker name*/

#define fDOPTION      0x40000000                 /* -D option indicator       */
#define fSOPTION      0x80000000                 /* -S option indicator       */

/******************************************************************************/
/*                                                                            */
/* Structure declarations                                                     */
/*                                                                            */
/******************************************************************************/

typedef struct new_exe NEHDR;                    /* NE executable header      */
typedef struct new_rlc NERLC;                    /* NE relocation item        */
typedef struct new_seg NESEG;                    /* NE segment table entry    */
typedef struct exe_hdr MZHDR;                    /* DOS executable header     */
typedef struct e32_exe LXHDR;                    /* LX executable header      */

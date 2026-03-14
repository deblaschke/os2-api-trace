/******************************************************************************/
/*                                                                            */
/* File name    : DLLINFO.H                                                   */
/*                                                                            */
/* Title        : DLL information table include file                          */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.40.57 25Jun98 Added trace-enabled executable checker      */
/*                2.40.75 28Oct98 Added Gpi/Win APIs exported from PMBIDI     */
/*                2.45.00 15Apr99 Added new OS/2 2.45 APIs                    */
/*                                                                            */
/******************************************************************************/

DLLINFO   DLLInfo[] =                            /* OS/2 DLLs                 */
          {
             { "DOSCALLS",    1L,  1117L, 110L, 9018L, "T_DOS___", FALSE, 0L },
             { "HELPMGR",     1L,    90L,  51L,   88L, "T_HLP__",  FALSE, 0L },
             { "KBDCALLS",    1L,    25L,   0L,    0L, "T_KBD___", FALSE, 0L },
             { "MONCALLS",    1L,     5L,   0L,    0L, "T_MON___", FALSE, 0L },
             { "MOUCALLS",    1L,    30L,   0L,    0L, "T_MOU___", FALSE, 0L },
             { "MSG",         1L,     9L,   0L,    0L, "T_M",      FALSE, 0L },
             { "NAMPIPES",    1L,    14L,   0L,    0L, "T_PIP___", FALSE, 0L },
             { "NLS",         1L,     8L,   5L,   12L, "T_N",      FALSE, 0L },
             { "PMBIDI",      1L,   999L,   0L,    0L, "T_PBD_",   FALSE, 0L },
             { "PMCTLS",      1L,   301L,   2L,   24L, "T_CTL_",   FALSE, 0L },
             { "PMDRAG",      1L,    76L,  32L,   74L, "T_DRG_",   FALSE, 0L },
             { "PMGPI",       1L,   730L, 351L,  657L, "T_GPI",    FALSE, 0L },
             { "PMMERGE",  2000L, 10021L,   0L,    0L, "T_MRG__",  FALSE, 0L },
             { "PMPIC",       1L,    13L,  11L,   12L, "T_PIC",    FALSE, 0L },
             { "PMSHAPI",     1L,   213L, 101L,  156L, "T_SHL__",  FALSE, 0L },
             { "PMSPL",       1L,   625L, 301L,  442L, "T_SPL",    FALSE, 0L },
             { "PMVIOP",      1L,   122L,   0L,    0L, "T_PVP_",   FALSE, 0L },
             { "PMWIN",       1L, 10021L, 700L, 1003L, "T_WIN",    FALSE, 0L },
             { "PMWP",        1L,   513L, 149L,  480L, "T_WP",     FALSE, 0L },
             { "QUECALLS",    1L,    16L,   9L,   16L, "T_QUE___", FALSE, 0L },
             { "SESMGR",      1L,    41L,   0L,    0L, "T_SES_",   FALSE, 0L },
             { "VIOCALLS",    1L,    76L,   0L,    0L, "T_VIO___", FALSE, 0L }
          };

/******************************************************************************/
/*                                                                            */
/* File name    : PMOS2TRC.H                                                  */
/*                                                                            */
/* Title        : OS/2 API trace PM include file                              */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.24 29Mar96 Added PM interface (supported customization */
/*                                and help only)                              */
/*                2.30.25 15Apr96 Verified DOS header new header file address */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.05 12Mar97 Added enablement support to PM interface    */
/*                2.40.08 13Mar97 Added summarization support to PM interface */
/*                2.40.11 19Mar97 Added  save  window  position  option to PM */
/*                                interface                                   */
/*                2.40.12 19Mar97 Added support for NE format                 */
/*                2.40.13 20Mar97 Added  launch   .EXE  files  option  to  PM */
/*                                interface                                   */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.18 30Apr97 Added 16-bit Kbd APIs                       */
/*                2.40.19 01May97 Added 16-bit Mou APIs                       */
/*                2.40.20 05May97 Added 16-bit Vio APIs                       */
/*                2.40.22 22May97 Added FAQ                                   */
/*                2.40.24 03Jun97 Added FAQ to PM interface help submenu      */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.47 29Jan98 Added Win16DefAVioWindowProc                */
/*                2.40.53 16Mar98 Fixed file dialog clean up bug              */
/*                2.40.54 16Mar98 Changed main window identifier to 1         */
/*                2.40.60 08Jul98 Added  title/action  bar only  option to PM */
/*                                interface                                   */
/*                2.40.61 17Aug98 Added SMP Dos APIs                          */
/*                2.40.65 28Aug98 Added monitor trace options to PM interface */
/*                2.40.74 27Oct98 Added WinSetErrorInfo                       */
/*                2.40.75 28Oct98 Added Gpi/Win APIs exported from PMBIDI     */
/*                2.40.78 16Nov98 Added miscellaneous Dos APIs                */
/*                2.40.81 02Dec98 Added -E option                             */
/*                2.40.85 21Jan99 Added -P option                             */
/*                2.45.00 15Apr99 Added new OS/2 2.45 APIs                    */
/*                2.45.06 16May99 Added support for  KB and MB  values to  PM */
/*                                interface                                   */
/*                2.45.08 11Jun99 Added support for .COM files                */
/*                2.45.16 18Feb00 Added -C, -PAUSE, and -RESUME options       */
/*                2.45.17 21Feb00 Added restore window  position option to PM */
/*                                interface                                   */
/*                2.45.24 01Jun00 Added -A option                             */
/*                2.45.26 10Jan01 Handled OS/2 2.45 APIs removed by WSeB CP   */
/*                2.45.32 28Jun01 Added -U option                             */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#define VER_PMNAME        "PMOS2TRC"             /* PM API trace enabler name */
#define DEF_DEBUGFILE     "PMOS2TRC.DBG"         /* Default dbg msg file name */
#define DEF_ERRORFILE     "PMOS2TRC.ERR"         /* Default err msg file name */
#define DEF_HELPFILE      "PMOS2TRC.HLP"         /* Default help file name    */

#define WINPOS_KEY        "Window position"      /* Window position key       */
#define WINSIZ_KEY        "Window size"          /* Window size key           */

#define HLP_INTELONLY     11                     /* Intel only help           */
#define HLP_POWERPCONLY   12                     /* PowerPC only help         */
#define HLP_SMPONLY       13                     /* SMP only help             */
#define HLP_245TOCPONLY   14                     /* OS/2 2.45 to WSeB CP only */
#define HLP_OS2VER210     21                     /* OS/2 2.10 help            */
#define HLP_OS2VER230     23                     /* OS/2 2.30 help            */
#define HLP_OS2VER240     24                     /* OS/2 2.40 help            */
#define HLP_OS2VER245     245                    /* OS/2 2.45 help            */
#define HLP_OS2VER300     30                     /* OS/2 3.00 help            */

/******************************************************************************/
/*                                                                            */
/* Main window constants                                                      */
/*                                                                            */
/******************************************************************************/

#define MAIN_CLASS        "PMOS2TRC Main Window"
#define ID_MAINWIN        1
#define HLP_MAINWIN       101
#define HLP_ID_MAINWIN    102
#define PNL_ID_MAINWIN    103
#define ID_MAINMSGBOX     104
#define IDM_MAINEXIT      105
#define IDM_ENABMENU      200
#define HLP_ENABMENU      201
#define IDM_ENABOPEN      202
#define HLP_ENABOPEN      203
#define IDM_ENABPDTS      204
#define HLP_ENABPDTS      205
#define IDM_CUSTMENU      300
#define HLP_CUSTMENU      301
#define IDM_CUSTALT       302
#define HLP_CUSTALT       303
#define IDM_CUSTBUF       304
#define HLP_CUSTBUF       305
#define IDM_CUSTCTL       306
#define HLP_CUSTCTL       307
#define IDM_CUSTDOS       308
#define HLP_CUSTDOS       309
#define IDM_CUSTEBC       310
#define HLP_CUSTEBC       311
#define IDM_CUSTFIL       312
#define HLP_CUSTFIL       313
#define IDM_CUSTGPI       314
#define HLP_CUSTGPI       315
#define IDM_CUSTINT       316
#define HLP_CUSTINT       317
#define IDM_CUSTLVL       318
#define HLP_CUSTLVL       319
#define IDM_CUSTTIM       320
#define HLP_CUSTTIM       321
#ifdef T_USERHOOK
#define IDM_CUSTUSR       322
#define HLP_CUSTUSR       323
#endif
#define IDM_CUSTWIN       324
#define HLP_CUSTWIN       325
#define IDM_CTLMENU       400
#define HLP_CTLMENU       401
#define IDM_CTLPAUSE      402
#define HLP_CTLPAUSE      403
#define IDM_CTLRESUME     404
#define HLP_CTLRESUME     405
#define IDM_SUMMENU       500
#define HLP_SUMMENU       501
#define IDM_SUMOPEN       502
#define HLP_SUMOPEN       503
#define IDM_OPTSMENU      600
#define HLP_OPTSMENU      601
#define IDM_OPTSEXE       602
#define HLP_OPTSEXE       603
#define IDM_OPTSMON       604
#define HLP_OPTSMON       605
#define IDM_OPTSAPI       606
#define HLP_OPTSAPI       607
#define IDM_OPTSLEN       608
#define HLP_OPTSLEN       609
#define IDM_OPTSBAR       610
#define HLP_OPTSBAR       611
#define IDM_OPTSTOP       612
#define HLP_OPTSTOP       613
#define IDM_OPTSCUR       614
#define HLP_OPTSCUR       615
#define IDM_OPTSBOT       616
#define HLP_OPTSBOT       617
#define IDM_OPTSSWP       618
#define HLP_OPTSSWP       619
#define IDM_OPTRSWP       620
#define HLP_OPTRSWP       621
#define IDM_HELPMENU      700
#define HLP_HELPMENU      701
#define IDM_HELPIDX       702
#define HLP_HELPIDX       703
#define IDM_HELPGEN       704
#define HLP_HELPGEN       705
#define IDM_HELPUSE       706
#define HLP_HELPUSE       707
#define IDM_HELPKEY       708
#define HLP_HELPKEY       709
#define IDM_HELPFAQ       710
#define HLP_HELPFAQ       711
#define IDM_HELPINFO      712
#define HLP_HELPINFO      713

/******************************************************************************/
/*                                                                            */
/* Alternative directory customization dialog constants                       */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTALT       18000
#define HLP_DLG_CUSTALT   18001
#define PNL_DLG_CUSTALT   18002
#define DLG_ALTDEFRAD     18003
#define ALT_DEFTEXT       "Log to default directory"
#define DLG_ALTDEFTXT     18004
#define DLG_ALTDIRRAD     18005
#define ALT_DIRTEXT1      "Log to"
#define DLG_ALTDIRTXT1    18006
#define DLG_ALTDIR        18007
#define ALT_DIRTEXT2      "directory"
#define DLG_ALTDIRTXT2    18008
#define DLG_ALTOK         18009
#define HLP_ALTOK         18010
#define DLG_ALTCANCEL     18011
#define HLP_ALTCANCEL     18012
#define DLG_ALTDEFAULT    18013
#define HLP_ALTDEFAULT    18014
#define DLG_ALTUNDO       18015
#define HLP_ALTUNDO       18016
#define DLG_ALTHELP       18017

/******************************************************************************/
/*                                                                            */
/* Buffer length customization dialog constants                               */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTBUF       1000
#define HLP_DLG_CUSTBUF   1001
#define PNL_DLG_CUSTBUF   1002
#define DLG_BUFALLRAD     1003
#define BUF_ALLTEXT       "Log ALL bytes from buffers"
#define DLG_BUFALLTXT     1004
#define DLG_BUFVALRAD     1005
#define BUF_VALTEXT1      "Log maximum of"
#define DLG_BUFVALTXT1    1006
#define DLG_BUFVALSPIN    1007
#define DLG_BUFTYPSPIN    1008
#define BUF_VALTEXT2      "from"
#define DLG_BUFVALTXT2    1009
#define BUF_VALTEXT3      "buffers"
#define DLG_BUFVALTXT3    1010
#define DLG_BUFOK         1011
#define HLP_BUFOK         1012
#define DLG_BUFCANCEL     1013
#define HLP_BUFCANCEL     1014
#define DLG_BUFDEFAULT    1015
#define HLP_BUFDEFAULT    1016
#define DLG_BUFUNDO       1017
#define HLP_BUFUNDO       1018
#define DLG_BUFHELP       1019

/******************************************************************************/
/*                                                                            */
/* Tracing control customization dialog constants                             */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTCTL       16000
#define HLP_DLG_CUSTCTL   16001
#define PNL_DLG_CUSTCTL   16002
#define DLG_CTLTITLE      16003
#define DLG_CTLONRAD      16004
#define DLG_CTLOFFRAD     16005
#define CTL_CTLONTEXT     "Enable tracing control (pause/resume)"
#define DLG_CTLONTXT      16006
#define CTL_CTLOFFTEXT    "Disable tracing control (pause/resume)"
#define DLG_CTLOFFTXT     16007
#define DLG_CTLOK         16008
#define HLP_CTLOK         16009
#define DLG_CTLCANCEL     16010
#define HLP_CTLCANCEL     16011
#define DLG_CTLDEFAULT    16012
#define HLP_CTLDEFAULT    16013
#define DLG_CTLUNDO       16014
#define HLP_CTLUNDO       16015
#define DLG_CTLHELP       16016

/******************************************************************************/
/*                                                                            */
/* DOSCALLS APIs customization dialog constants                               */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTDOS       2000
#define HLP_DLG_CUSTDOS   2001
#define PNL_DLG_CUSTDOS   2002
#define DOS_TEXT          "Trace specific DOSCALLS API groups:"
#define DLG_DOSTXT        2003
#define DLG_DOSDEVCHK     2004
#define HLP_DOSDEVCHK     2005
#define DLG_DOSFILECHK    2006
#define HLP_DOSFILECHK    2007
#define DLG_DOSINFOCHK    2008
#define HLP_DOSINFOCHK    2009
#define DLG_DOSMEMCHK     2010
#define HLP_DOSMEMCHK     2011
#define DLG_DOSMISCCHK    2012
#define HLP_DOSMISCCHK    2013
#define DLG_DOSMODCHK     2014
#define HLP_DOSMODCHK     2015
#define DLG_DOSMSGCHK     2016
#define HLP_DOSMSGCHK     2017
#define DLG_DOSMVDMCHK    2018
#define HLP_DOSMVDMCHK    2019
#define DLG_DOSNLSCHK     2020
#define HLP_DOSNLSCHK     2021
#define DLG_DOSPIPECHK    2022
#define HLP_DOSPIPECHK    2023
#define DLG_DOSPRFCHK     2024
#define HLP_DOSPRFCHK     2025
#define DLG_DOSPROCCHK    2026
#define HLP_DOSPROCCHK    2027
#define DLG_DOSPROFCHK    2028
#define HLP_DOSPROFCHK    2029
#define DLG_DOSRESCHK     2030
#define HLP_DOSRESCHK     2031
#define DLG_DOSSEMCHK     2032
#define HLP_DOSSEMCHK     2033
#define DLG_DOSSESCHK     2034
#define HLP_DOSSESCHK     2035
#define DLG_DOSSIGCHK     2036
#define HLP_DOSSIGCHK     2037
#define DLG_DOSSMPCHK     2038
#define HLP_DOSSMPCHK     2039
#define DLG_DOSTIMECHK    2040
#define HLP_DOSTIMECHK    2041
#define DLG_DOSXCPTCHK    2042
#define HLP_DOSXCPTCHK    2043
#define DLG_DOSOK         2044
#define HLP_DOSOK         2045
#define DLG_DOSCANCEL     2046
#define HLP_DOSCANCEL     2047
#define DLG_DOSCLEAR      2048
#define HLP_DOSCLEAR      2049
#define DLG_DOSDEFAULT    2050
#define HLP_DOSDEFAULT    2051
#define DLG_DOSUNDO       2052
#define HLP_DOSUNDO       2053
#define DLG_DOSHELP       2054

/******************************************************************************/
/*                                                                            */
/* EBCDIC translation customization dialog constants                          */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTEBC       15000
#define HLP_DLG_CUSTEBC   15001
#define PNL_DLG_CUSTEBC   15002
#define DLG_EBCTITLE      15003
#define DLG_EBCONRAD      15004
#define DLG_EBCOFFRAD     15005
#define EBC_EBCONTEXT     "Enable EBCDIC translation of character buffers"
#define DLG_EBCONTXT      15006
#define EBC_EBCOFFTEXT    "Disable EBCDIC translation of character buffers"
#define DLG_EBCOFFTXT     15007
#define DLG_EBCOK         15008
#define HLP_EBCOK         15009
#define DLG_EBCCANCEL     15010
#define HLP_EBCCANCEL     15011
#define DLG_EBCDEFAULT    15012
#define HLP_EBCDEFAULT    15013
#define DLG_EBCUNDO       15014
#define HLP_EBCUNDO       15015
#define DLG_EBCHELP       15016

/******************************************************************************/
/*                                                                            */
/* File wrapping customization dialog constants                               */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTFIL       3000
#define HLP_DLG_CUSTFIL   3001
#define PNL_DLG_CUSTFIL   3002
#define DLG_FILALLRAD     3003
#define FIL_ALLTEXT       "Log ALL information without log file wrapping"
#define DLG_FILALLTXT     3004
#define DLG_FILVALRAD     3005
#define FIL_VALTEXT1      "Log maximum of"
#define DLG_FILVALTXT1    3006
#define DLG_FILVALSPIN    3007
#define DLG_FILTYPSPIN    3008
#define FIL_VALTEXT2      "before log file wrapping occurs"
#define DLG_FILVALTXT2    3009
#define DLG_FILOK         3010
#define HLP_FILOK         3011
#define DLG_FILCANCEL     3012
#define HLP_FILCANCEL     3013
#define DLG_FILDEFAULT    3014
#define HLP_FILDEFAULT    3015
#define DLG_FILUNDO       3016
#define HLP_FILUNDO       3017
#define DLG_FILHELP       3018

/******************************************************************************/
/*                                                                            */
/* PMGPI APIs customization dialog constants                                  */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTGPI       4000
#define HLP_DLG_CUSTGPI   4001
#define PNL_DLG_CUSTGPI   4002
#define GPI_TEXT          "Trace specific PMGPI API groups:"
#define DLG_GPITXT        4003
#define DLG_GPIBITCHK     4004
#define HLP_GPIBITCHK     4005
#define DLG_GPICORRCHK    4006
#define HLP_GPICORRCHK    4007
#define DLG_GPICTRLCHK    4008
#define HLP_GPICTRLCHK    4009
#define DLG_GPIDEFCHK     4010
#define HLP_GPIDEFCHK     4011
#define DLG_GPIDEVCHK     4012
#define HLP_GPIDEVCHK     4013
#define DLG_GPIEDITCHK    4014
#define HLP_GPIEDITCHK    4015
#define DLG_GPIINKCHK     4016
#define HLP_GPIINKCHK     4017
#define DLG_GPILCIDCHK    4018
#define HLP_GPILCIDCHK    4019
#define DLG_GPILCTCHK     4020
#define HLP_GPILCTCHK     4021
#define DLG_GPIMETACHK    4022
#define HLP_GPIMETACHK    4023
#define DLG_GPIPATHCHK    4024
#define HLP_GPIPATHCHK    4025
#define DLG_GPIPOLYCHK    4026
#define HLP_GPIPOLYCHK    4027
#define DLG_GPIPRIMCHK    4028
#define HLP_GPIPRIMCHK    4029
#define DLG_GPIRGNCHK     4030
#define HLP_GPIRGNCHK     4031
#define DLG_GPISEGCHK     4032
#define HLP_GPISEGCHK     4033
#define DLG_GPITRANCHK    4034
#define HLP_GPITRANCHK    4035
#define DLG_GPIOK         4036
#define HLP_GPIOK         4037
#define DLG_GPICANCEL     4038
#define HLP_GPICANCEL     4039
#define DLG_GPICLEAR      4040
#define HLP_GPICLEAR      4041
#define DLG_GPIDEFAULT    4042
#define HLP_GPIDEFAULT    4043
#define DLG_GPIUNDO       4044
#define HLP_GPIUNDO       4045
#define DLG_GPIHELP       4046

/******************************************************************************/
/*                                                                            */
/* API interception customization dialog constants                            */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTINT       12000
#define HLP_DLG_CUSTINT   12001
#define PNL_DLG_CUSTINT   12002
#define DLG_INTTITLE      12003
#define DLG_INTONRAD      12004
#define DLG_INTOFFRAD     12005
#define INT_INTONTEXT     "Enable interception of dynamic API calls"
#define DLG_INTONTXT      12006
#define INT_INTOFFTEXT    "Disable interception of dynamic API calls"
#define DLG_INTOFFTXT     12007
#define DLG_INTOK         12008
#define HLP_INTOK         12009
#define DLG_INTCANCEL     12010
#define HLP_INTCANCEL     12011
#define DLG_INTDEFAULT    12012
#define HLP_INTDEFAULT    12013
#define DLG_INTUNDO       12014
#define HLP_INTUNDO       12015
#define DLG_INTHELP       12016

/******************************************************************************/
/*                                                                            */
/* Logging level customization dialog constants                               */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTLVL       5000
#define HLP_DLG_CUSTLVL   5001
#define PNL_DLG_CUSTLVL   5002
#define DLG_LVL1RAD       5003
#define DLG_LVL2RAD       5004
#define DLG_LVL3RAD       5005
#define LVL_LVL1TEXT      "Log API entry/exit (level 1) information"
#define DLG_LVL1TXT       5006
#define LVL_LVL2TEXT      "Log API parameters (level 2) information"
#define DLG_LVL2TXT       5007
#define LVL_LVL3TEXT      "Log API parameter contents (level 3) information"
#define DLG_LVL3TXT       5008
#define DLG_LVLOK         5009
#define HLP_LVLOK         5010
#define DLG_LVLCANCEL     5011
#define HLP_LVLCANCEL     5012
#define DLG_LVLDEFAULT    5013
#define HLP_LVLDEFAULT    5014
#define DLG_LVLUNDO       5015
#define HLP_LVLUNDO       5016
#define DLG_LVLHELP       5017

/******************************************************************************/
/*                                                                            */
/* Time stamping customization dialog constants                               */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTTIM       6000
#define HLP_DLG_CUSTTIM   6001
#define PNL_DLG_CUSTTIM   6002
#define DLG_TIMTITLE      6003
#define DLG_TIMONRAD      6004
#define DLG_TIMOFFRAD     6005
#define TIM_TIMONTEXT     "Enable time stamping of API entries and exits"
#define DLG_TIMONTXT      6006
#define TIM_TIMOFFTEXT    "Disable time stamping of API entries and exits"
#define DLG_TIMOFFTXT     6007
#define DLG_TIMOK         6008
#define HLP_TIMOK         6009
#define DLG_TIMCANCEL     6010
#define HLP_TIMCANCEL     6011
#define DLG_TIMDEFAULT    6012
#define HLP_TIMDEFAULT    6013
#define DLG_TIMUNDO       6014
#define HLP_TIMUNDO       6015
#define DLG_TIMHELP       6016

/******************************************************************************/
/*                                                                            */
/* User hook customization dialog constants                                   */
/*                                                                            */
/******************************************************************************/

#ifdef T_USERHOOK
#define DLG_CUSTUSR       19000
#define HLP_DLG_CUSTUSR   19001
#define PNL_DLG_CUSTUSR   19002
#define DLG_USRDEFRAD     19003
#define USR_DEFTEXT       "Do not call user hook"
#define DLG_USRDEFTXT     19004
#define DLG_USRHOOKRAD    19005
#define USR_HOOKTEXT      "Call user hook"
#define DLG_USRHOOKTXT    19006
#define DLG_USRHOOK       19007
#define DLG_USROK         19008
#define HLP_USROK         19009
#define DLG_USRCANCEL     19010
#define HLP_USRCANCEL     19011
#define DLG_USRDEFAULT    19012
#define HLP_USRDEFAULT    19013
#define DLG_USRUNDO       19014
#define HLP_USRUNDO       19015
#define DLG_USRHELP       19016
#define HLP_USREXAMPLE    19015
#endif

/******************************************************************************/
/*                                                                            */
/* PMWIN APIs customization dialog constants                                  */
/*                                                                            */
/******************************************************************************/

#define DLG_CUSTWIN       7000
#define HLP_DLG_CUSTWIN   7001
#define PNL_DLG_CUSTWIN   7002
#define WIN_TEXT          "Trace specific PMWIN API groups:"
#define DLG_WINTXT        7003
#define DLG_WINACCLCHK    7004
#define HLP_WINACCLCHK    7005
#define DLG_WINATOMCHK    7006
#define HLP_WINATOMCHK    7007
#define DLG_WINCLIPCHK    7008
#define HLP_WINCLIPCHK    7009
#define DLG_WINCTRYCHK    7010
#define HLP_WINCTRYCHK    7011
#define DLG_WINCURCHK     7012
#define HLP_WINCURCHK     7013
#define DLG_WINDDECHK     7014
#define HLP_WINDDECHK     7015
#define DLG_WINDESKCHK    7016
#define HLP_WINDESKCHK    7017
#define DLG_WINDLGCHK     7018
#define HLP_WINDLGCHK     7019
#define DLG_WINDWINCHK    7020
#define HLP_WINDWINCHK    7021
#define DLG_WINENVCHK     7022
#define HLP_WINENVCHK     7023
#define DLG_WINERRCHK     7024
#define HLP_WINERRCHK     7025
#define DLG_WINFRAMCHK    7026
#define HLP_WINFRAMCHK    7027
#define DLG_WINHEAPCHK    7028
#define HLP_WINHEAPCHK    7029
#define DLG_WINHOOKCHK    7030
#define HLP_WINHOOKCHK    7031
#define DLG_WININPTCHK    7032
#define HLP_WININPTCHK    7033
#define DLG_WINLOADCHK    7034
#define HLP_WINLOADCHK    7035
#define DLG_WINMENUCHK    7036
#define HLP_WINMENUCHK    7037
#define DLG_WINMSGCHK     7038
#define HLP_WINMSGCHK     7039
#define DLG_WINMSGLCHK    7040
#define HLP_WINMSGLCHK    7041
#define DLG_WINPALCHK     7042
#define HLP_WINPALCHK     7043
#define DLG_WINPTRCHK     7044
#define HLP_WINPTRCHK     7045
#define DLG_WINRECTCHK    7046
#define HLP_WINRECTCHK    7047
#define DLG_WINSEICHK     7048
#define HLP_WINSEICHK     7049
#define DLG_WINSYSCHK     7050
#define HLP_WINSYSCHK     7051
#define DLG_WINTHKCHK     7052
#define HLP_WINTHKCHK     7053
#define DLG_WINTIMECHK    7054
#define HLP_WINTIMECHK    7055
#define DLG_WINTRECCHK    7056
#define HLP_WINTRECCHK    7057
#define DLG_WINWINCHK     7058
#define HLP_WINWINCHK     7059
#define DLG_WINOK         7060
#define HLP_WINOK         7061
#define DLG_WINCANCEL     7062
#define HLP_WINCANCEL     7063
#define DLG_WINCLEAR      7064
#define HLP_WINCLEAR      7065
#define DLG_WINDEFAULT    7066
#define HLP_WINDEFAULT    7067
#define DLG_WINUNDO       7068
#define HLP_WINUNDO       7069
#define DLG_WINHELP       7070

/******************************************************************************/
/*                                                                            */
/* Main window help information dialog constants                              */
/*                                                                            */
/******************************************************************************/

#define DLG_HELPINFO      8000
#define HLP_DLG_HELPINFO  8001
#define PNL_DLG_HELPINFO  8002
#define INF_INFOTEXT1     "Operating System/2 API Trace"
#define DLG_INFOTXT1      8003
#ifdef PRIVATE
#define INF_INFOTEXT2     "PRIVATE Version #.##.##"
#else
#define INF_INFOTEXT2     "Version #.##.##"
#endif
#define DLG_INFOTXT2      8004
#define INF_INFOTEXT3     "Written by Dave Blaschke"
#define DLG_INFOTXT3      8005
#define DLG_INFOOK        8006
#define HLP_INFOOK        8007

/******************************************************************************/
/*                                                                            */
/* Enablement dialog constants                                                */
/*                                                                            */
/******************************************************************************/

#define DLG_ENAB          9000
#define HLP_ENAB          9001
#define HLP_DLG_ENAB      9002
#define PNL_DLG_ENAB      9003
#define DLG_ENABFILETXT   9004
#define ENA_ENABONTEXT    "On"
#define DLG_ENABONTXT1    9005
#define ENA_ENABOFFTEXT   "Off"
#define DLG_ENABOFFTXT1   9006
#define DLG_ENABONTXT2    9007
#define DLG_ENABOFFTXT2   9008
#define DLG_ENABDOSONRAD  9009
#define DLG_ENABDOSOFFRAD 9010
#define HLP_ENABDOS       9011
#define ENA_ENABDOSTEXT   "DOSCALLS"
#define DLG_ENABDOSTXT    9012
#define DLG_ENABHLPONRAD  9013
#define DLG_ENABHLPOFFRAD 9014
#define HLP_ENABHLP       9015
#define ENA_ENABHLPTEXT   "HELPMGR"
#define DLG_ENABHLPTXT    9016
#define DLG_ENABKBDONRAD  9017
#define DLG_ENABKBDOFFRAD 9018
#define HLP_ENABKBD       9019
#define ENA_ENABKBDTEXT   "KBDCALLS"
#define DLG_ENABKBDTXT    9020
#define DLG_ENABMONONRAD  9021
#define DLG_ENABMONOFFRAD 9022
#define HLP_ENABMON       9023
#define ENA_ENABMONTEXT   "MONCALLS"
#define DLG_ENABMONTXT    9024
#define DLG_ENABMOUONRAD  9025
#define DLG_ENABMOUOFFRAD 9026
#define HLP_ENABMOU       9027
#define ENA_ENABMOUTEXT   "MOUCALLS"
#define DLG_ENABMOUTXT    9028
#define DLG_ENABMSGONRAD  9029
#define DLG_ENABMSGOFFRAD 9030
#define HLP_ENABMSG       9031
#define ENA_ENABMSGTEXT   "MSG"
#define DLG_ENABMSGTXT    9032
#define DLG_ENABPIPONRAD  9033
#define DLG_ENABPIPOFFRAD 9034
#define HLP_ENABPIP       9035
#define ENA_ENABPIPTEXT   "NAMPIPES"
#define DLG_ENABPIPTXT    9036
#define DLG_ENABNLSONRAD  9037
#define DLG_ENABNLSOFFRAD 9038
#define HLP_ENABNLS       9039
#define ENA_ENABNLSTEXT   "NLS"
#define DLG_ENABNLSTXT    9040
#define DLG_ENABPBDONRAD  9041
#define DLG_ENABPBDOFFRAD 9042
#define HLP_ENABPBD       9043
#define ENA_ENABPBDTEXT   "PMBIDI"
#define DLG_ENABPBDTXT    9044
#define DLG_ENABCTLONRAD  9045
#define DLG_ENABCTLOFFRAD 9046
#define HLP_ENABCTL       9047
#define ENA_ENABCTLTEXT   "PMCTLS"
#define DLG_ENABCTLTXT    9048
#define DLG_ENABDRGONRAD  9049
#define DLG_ENABDRGOFFRAD 9050
#define HLP_ENABDRG       9051
#define ENA_ENABDRGTEXT   "PMDRAG"
#define DLG_ENABDRGTXT    9052
#define DLG_ENABGPIONRAD  9053
#define DLG_ENABGPIOFFRAD 9054
#define HLP_ENABGPI       9055
#define ENA_ENABGPITEXT   "PMGPI"
#define DLG_ENABGPITXT    9056
#define DLG_ENABMRGONRAD  9057
#define DLG_ENABMRGOFFRAD 9058
#define HLP_ENABMRG       9059
#define ENA_ENABMRGTEXT   "PMMERGE"
#define DLG_ENABMRGTXT    9060
#define DLG_ENABPICONRAD  9061
#define DLG_ENABPICOFFRAD 9062
#define HLP_ENABPIC       9063
#define ENA_ENABPICTEXT   "PMPIC"
#define DLG_ENABPICTXT    9064
#define DLG_ENABSHLONRAD  9065
#define DLG_ENABSHLOFFRAD 9066
#define HLP_ENABSHL       9067
#define ENA_ENABSHLTEXT   "PMSHAPI"
#define DLG_ENABSHLTXT    9068
#define DLG_ENABSPLONRAD  9069
#define DLG_ENABSPLOFFRAD 9070
#define HLP_ENABSPL       9071
#define ENA_ENABSPLTEXT   "PMSPL"
#define DLG_ENABSPLTXT    9072
#define DLG_ENABPVPONRAD  9073
#define DLG_ENABPVPOFFRAD 9074
#define HLP_ENABPVP       9075
#define ENA_ENABPVPTEXT   "PMVIOP"
#define DLG_ENABPVPTXT    9076
#define DLG_ENABWINONRAD  9077
#define DLG_ENABWINOFFRAD 9078
#define HLP_ENABWIN       9079
#define ENA_ENABWINTEXT   "PMWIN"
#define DLG_ENABWINTXT    9080
#define DLG_ENABWPONRAD   9081
#define DLG_ENABWPOFFRAD  9082
#define HLP_ENABWP        9083
#define ENA_ENABWPTEXT    "PMWP"
#define DLG_ENABWPTXT     9084
#define DLG_ENABQUEONRAD  9085
#define DLG_ENABQUEOFFRAD 9086
#define HLP_ENABQUE       9087
#define ENA_ENABQUETEXT   "QUECALLS"
#define DLG_ENABQUETXT    9088
#define DLG_ENABSESONRAD  9089
#define DLG_ENABSESOFFRAD 9090
#define HLP_ENABSES       9091
#define ENA_ENABSESTEXT   "SESMGR"
#define DLG_ENABSESTXT    9092
#define DLG_ENABVIOONRAD  9093
#define DLG_ENABVIOOFFRAD 9094
#define HLP_ENABVIO       9095
#define ENA_ENABVIOTEXT   "VIOCALLS"
#define DLG_ENABVIOTXT    9096
#define DLG_ENABOK        9097
#define HLP_ENABOK        9098
#define DLG_ENABCANCEL    9099
#define HLP_ENABCANCEL    9100
#define DLG_ENABALLON     9101
#define HLP_ENABALLON     9102
#define DLG_ENABALLOFF    9103
#define HLP_ENABALLOFF    9104
#define DLG_ENABUNDO      9105
#define HLP_ENABUNDO      9106
#define DLG_ENABHELP      9107

#define MIN_ENABONRAD     DLG_ENABDOSONRAD       /* Minimum enab on button    */
#define MIN_ENABOFFRAD    DLG_ENABDOSOFFRAD      /* Minimum enab off button   */
#define MIN_ENABTXT       DLG_ENABDOSTXT         /* Minimum enablement text   */
#define INC_ENABRAD       4                      /* Enab button increment     */

/******************************************************************************/
/*                                                                            */
/* Summarization dialog constants                                             */
/*                                                                            */
/******************************************************************************/

#define DLG_SUM           10000
#define HLP_SUM           10001
#define HLP_DLG_SUM       10002
#define PNL_DLG_SUM       10003
#define DLG_SUMMLE        10004
#define HLP_SUMMLE        10005
#define DLG_SUMOK         10006
#define HLP_SUMOK         10007
#define DLG_SUMSAVEAS     10008
#define HLP_SUMSAVEAS     10009
#define DLG_SUMHELP       10010

/******************************************************************************/
/*                                                                            */
/* Launch executable dialog constants                                         */
/*                                                                            */
/******************************************************************************/

#define DLG_EXE           11000
#define HLP_EXE           11001
#define HLP_DLG_EXE       11002
#define PNL_DLG_EXE       11003
#define DLG_EXEPARMTEXT   "Parameters:"
#define DLG_EXEPARMTXT    11004
#define DLG_EXEPARM       11005
#define HLP_EXEPARM       11006
#define DLG_EXESESTEXT    "Start session in:"
#define DLG_EXESESTXT     11007
#define DLG_EXEFGSESTEXT  "Foreground"
#define DLG_EXEFGSESRAD   11008
#define HLP_EXEFGSES      11009
#define DLG_EXEBGSESTEXT  "Background"
#define DLG_EXEBGSESRAD   11010
#define HLP_EXEBGSES      11011
#define DLG_EXEOK         11012
#define HLP_EXEOK         11013
#define DLG_EXECANCEL     11014
#define HLP_EXECANCEL     11015
#define DLG_EXEHELP       11016

/******************************************************************************/
/*                                                                            */
/* API monitoring dialog constants                                            */
/*                                                                            */
/******************************************************************************/

#define DLG_OPTSAPI       13000
#define HLP_DLG_OPTSAPI   13001
#define PNL_DLG_OPTSAPI   13002
#define DLG_APIOFFRAD     13003
#define API_OFFTEXT       "Do not monitor number of APIs"
#define DLG_APIOFFTXT     13004
#define DLG_APIVALRAD     13005
#define API_VALTEXT1      "Update number of APIs every"
#define DLG_APIVALTXT1    13006
#define DLG_APIVALSPIN    13007
#define API_VALTEXT2      "APIs"
#define DLG_APIVALTXT2    13008
#define DLG_APIOK         13009
#define HLP_APIOK         13010
#define DLG_APICANCEL     13011
#define HLP_APICANCEL     13012
#define DLG_APIDEFAULT    13013
#define HLP_APIDEFAULT    13014
#define DLG_APIUNDO       13015
#define HLP_APIUNDO       13016
#define DLG_APIHELP       13017

/******************************************************************************/
/*                                                                            */
/* File length monitoring dialog constants                                    */
/*                                                                            */
/******************************************************************************/

#define DLG_OPTSLEN       14000
#define HLP_DLG_OPTSLEN   14001
#define PNL_DLG_OPTSLEN   14002
#define DLG_LENOFFRAD     14003
#define LEN_OFFTEXT       "Do not monitor log file length"
#define DLG_LENOFFTXT     14004
#define DLG_LENVALRAD     14005
#define LEN_VALTEXT1      "Update log file length every"
#define DLG_LENVALTXT1    14006
#define DLG_LENVALSPIN    14007
#define DLG_LENTYPSPIN    14008
#define DLG_LENOK         14009
#define HLP_LENOK         14010
#define DLG_LENCANCEL     14011
#define HLP_LENCANCEL     14012
#define DLG_LENDEFAULT    14013
#define HLP_LENDEFAULT    14014
#define DLG_LENUNDO       14015
#define HLP_LENUNDO       14016
#define DLG_LENHELP       14017

/******************************************************************************/
/*                                                                            */
/* Control dialog constants                                                   */
/*                                                                            */
/******************************************************************************/

#define DLG_CTL           17000
#define HLP_CTL           17001
#define HLP_DLG_CTL       17002
#define PNL_DLG_CTL       17003
#define DLG_CTLMLE        17004
#define HLP_CTLMLE        17005
#define DLG_CTLDLGOK      17006
#define HLP_CTLDLGOK      17007
#define DLG_CTLDLGHELP    17008

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

typedef struct _enabinfo                         /* Enablement information    */
{
   FDATE    fdateInFile;                         /* Input file write date     */
   FILE    *fpInFile;                            /* Input file pointer        */
   FTIME    ftimeInFile;                         /* Input file write time     */
   PCHAR    pFixupTbl;                           /* Fixup record table        */
   PCHAR    pImpNamTbl;                          /* Import name table         */
   PULONG   pModIdxTbl;                          /* Module index table        */
   PCHAR    pModNamTbl;                          /* Module name table         */
   PUSHORT  pModRefTbl;                          /* Module reference table    */
   NEHDR   *pNEHdr;                              /* NE executable header      */
   PCHAR    pPrcNamTbl;                          /* Procedure name table      */
   PCHAR    pSegment;                            /* Segment                   */
   NESEG   *pSegmentTbl;                         /* Segment table             */
   PCHAR    pStrTbl;                             /* String table              */
   LXHDR    sLXHdr;                              /* LX executable header      */
   MZHDR    sMZHdr;                              /* DOS executable header     */
   CHAR     szInFile[ CCHMAXPATH ];              /* Input file name           */
   ULONG    ulFixupTbl;                          /* Fixup record table size   */
   ULONG    ulFlags;                             /* Flags                     */
   ULONG    ulImpNamTbl;                         /* Import name table size    */
   ULONG    ulModIdxTbl;                         /* Module index table size   */
   ULONG    ulModNamTbl;                         /* Module name table size    */
   ULONG    ulModRefTbl;                         /* Module reference tbl size */
   ULONG    ulPrcNamTbl;                         /* Procedure name table size */
   ULONG    ulSegment;                           /* Number segments           */
   ULONG    ulSegmentTbl;                        /* Segment table size        */
   ULONG    ulStrTbl;                            /* String table size         */
   ULONG    ulStrTblOff;                         /* String table file offset  */
   USHORT   usSector;                            /* Sector size               */
   ULONG    DLLFlags[ 1 ];                       /* DLL flags                 */
}  ENABINFO, *PENABINFO;

typedef struct _enabregrec                       /* Enab registration record  */
{
   REGREC    sRegRec;                            /* Registration record       */
   HAB       habEnab;                            /* Enab dialog anchor block  */
   HMQ       hmqEnab;                            /* Enab dialog message queue */
   PENABINFO pEnabInfo;                          /* Enablement information    */
}  ENABREGREC, *PENABREGREC;

typedef struct _moninfo                          /* Trace monitor information */
{
   ULONG            ulPID;                       /* Process identifier        */
   ULONG            ulFlags;                     /* Flags                     */
   ULONG            ulNumAPI;                    /* Number APIs               */
   RECTL            rectlNumAPI;                 /* Number APIs rectangle     */
   ULONG            ulFilLen;                    /* Log file length           */
   RECTL            rectlFilLen;                 /* Log file length rectangle */
   CHAR             szExeName[ LEN_FILENAME ];   /* Executable file name      */
   CHAR             szLogName[ LEN_FILENAME ];   /* Log file name             */
   struct _moninfo *pNext;                       /* Next trace monitor info   */
}  MONINFO, *PMONINFO;

typedef struct _sbinfo                           /* Spin button information   */
{
   PCHAR szName;                                 /* Value name                */
   ULONG ulMinValue;                             /* Minimum value             */
   ULONG ulMaxValue;                             /* Maximum value             */
   ULONG ulIncrement;                            /* Increment value           */
   ULONG ulMultiplier;                           /* Value multiplier          */
}  SBINFO, *PSBINFO;

typedef struct _suminfo                          /* Summarization information */
{
   BOOL      bSaved;                             /* Saved file indicator      */
   FILE     *fpInFile;                           /* Input file pointer        */
   PAPIINFO  pAPIFirst;                          /* First API information     */
   CHAR      szInFile[ CCHMAXPATH ];             /* Input file name           */
}  SUMINFO, *PSUMINFO;

typedef struct _sumregrec                        /* Sum registration record   */
{
   REGREC   sRegRec;                             /* Registration record       */
   HAB      habSum;                              /* Sum dialog anchor block   */
   HMQ      hmqSum;                              /* Sum dialog message queue  */
   PSUMINFO pSumInfo;                            /* Summarization information */
}  SUMREGREC, *PSUMREGREC;

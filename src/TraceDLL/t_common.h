/******************************************************************************/
/*                                                                            */
/* File name    : T_COMMON.H                                                  */
/*                                                                            */
/* Title        : OS/2 API trace DLL common routines include file             */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.00 12Jun95 Created (supported LX format and 32-bit Dos */
/*                                APIs only)                                  */
/*                2.30.03 20Jul95 Added 32-bit Win APIs                       */
/*                2.30.05 04Aug95 Added 32-bit Gpi APIs                       */
/*                2.30.09 12Aug95 Added 32-bit Dev APIs                       */
/*                2.30.10 13Aug95 Added 32-bit Drg APIs                       */
/*                2.30.11 14Aug95 Added 32-bit Prf APIs                       */
/*                2.30.14 29Aug95 Added 32-bit Prt and Spl APIs               */
/*                2.30.22 19Dec95 Added new OS/2 3.00 non-Uni Dos APIs        */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.18 30Apr97 Added 16-bit Kbd APIs                       */
/*                2.40.19 01May97 Added 16-bit Mou APIs                       */
/*                2.40.20 05May97 Added 16-bit Vio APIs                       */
/*                2.40.21 20May97 Fixed/minimized 16-bit stack usage          */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
/*                2.40.27 22Jul97 Added 16-bit Gpi APIs                       */
/*                2.40.28 06Aug97 Added 16-bit Dev APIs                       */
/*                2.40.29 07Aug97 Added 16-bit Drg APIs                       */
/*                2.40.31 08Aug97 Logged user-defined buffer initial bytes    */
/*                2.40.34 13Aug97 Added 16-bit DosPrint and Spl APIs          */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.40 12Jan98 Enhanced performance                        */
/*                2.40.42 23Jan98 Added PRQINFO8 structure                    */
/*                2.40.44 27Jan98 Fixed returned MRESULT thunk bug            */
/*                2.40.45 28Jan98 Added indeterminate API return codes        */
/*                2.40.48 30Jan98 Fixed Win16Catch bug                        */
/*                2.40.50 06Feb98 Fixed CX and DX  corruption by  16-bit Kbd, */
/*                                Mou, and Vio APIs                           */
/*                2.40.58 01Jul98 Added version checking to trace DLLs        */
/*                2.40.61 17Aug98 Added SMP Dos APIs                          */
/*                2.40.71 22Oct98 Added PM AVIO Vio APIs                      */
/*                2.40.73 26Oct98 Fixed   Dos32GetMessage/Dos32QueryMessageCP */
/*                                bug                                         */
/*                2.40.77 28Oct98 Added Win32MessageBox2                      */
/*                2.40.78 16Nov98 Added miscellaneous Dos APIs                */
/*                2.40.80 02Dec98 Added undocumented -DUMPSTACK option        */
/*                2.45.00 15Apr99 Added new OS/2 2.45 APIs                    */
/*                2.45.10 02Aug99 Moved common strings into T_COMMON          */
/*                2.45.32 28Jun01 Added -U option                             */
/*                2.45.40 25Oct10 Fixed PEAOP/PEAOP2 bug                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#define DEF_ERRORFILE " :\\OS2TRACE.ERR"         /* Default err msg file name */

#ifdef T_MINSTACK
   #define T_ENTRY
#else
   #define T_ENTRY    APIENTRY
#endif

#define DT_ATOM                           0
#define DT_BOOL                           1
#define DT_BOOL32                         2
#define DT_COLOR                          3
#define DT_cParmUlongs                    4
#define DT_cParmUshorts                   5
#define DT_dosErrCode                     6
#define DT_ERRORID                        7
#define DT_FHLOCK                         8
#define DT_FIXED                          9
#define DT_HAB                           10
#define DT_HACCEL                        11
#define DT_HAPP                          12
#define DT_HATOMTBL                      13
#define DT_HBITMAP                       14
#define DT_HDC                           15
#define DT_HDDF                          16
#define DT_HDIR                          17
#define DT_HENUM                         18
#define DT_HEV                           19
#define DT_HFILE                         20
#define DT_HHEAP                         21
#define DT_HINI                          22
#define DT_HKBD                          23
#define DT_HLIB                          24
#define DT_HMF                           25
#define DT_HMODULE                       26
#define DT_HMONITOR                      27
#define DT_HMOU                          28
#define DT_HMQ                           29
#define DT_HMTX                          30
#define DT_HMUX                          31
#define DT_HOBJECT                       32
#define DT_HPAL                          33
#define DT_HPIPE                         34
#define DT_HPOINTER                      35
#define DT_HPROGRAM                      36
#define DT_HPS                           37
#define DT_HQUEUE                        38
#define DT_HRGN                          39
#define DT_HSAVEWP                       40
#define DT_HSEM                          41
#define DT_HSPINLOCK                     42
#define DT_HSPL                          43
#define DT_HSTD                          44
#define DT_HSTR                          45
#define DT_HSWITCH                       46
#define DT_HTIMER                        47
#define DT_HVDD                          48
#define DT_HVIO                          49
#define DT_HVPS                          50
#define DT_HWND                          51
#define DT_LONG                          52
#define DT_LONGLONG                      53
#define DT_MPARAM                        54
#define DT_MRESULT                       55
#define DT_NPBYTE                        56
#define DT_PACCELTABLE                   57
#define DT_PAPSZ                         58
#define DT_PARCPARAMS                    59
#define DT_pArgs                         60
#define DT_PAVAILDATA                    61
#define DT_PBITMAPINFO2                  62
#define DT_PBITMAPINFOHEADER             63
#define DT_PBITMAPINFOHEADER2            64
#define DT_PBOOL32                       65
#define DT_PBUNDLE                       66
#define DT_PBYTE                         67
#define DT_PCATCHBUF                     68
#define DT_PCH                           69
#define DT_PCHAR                         70
#define DT_PCLASSINFO                    71
#define DT_PCONTEXTRECORD                72
#define DT_PCONVCONTEXT                  73
#define DT_PCOUNTRYCODE                  74
#define DT_PCOUNTRYINFO                  75
#define DT_PCTLCOLOR                     76
#define DT_PCURSORINFO                   77
#define DT_PDATETIME                     78
#define DT_PDDESTRUCT                    79
#define DT_PDESKTOP                      80
#define DT_PDEVOPENDATA                  81
#define DT_PDLGTEMPLATE                  82
#define DT_PDOSFSRSEM                    83
#define DT_PDRAGIMAGE                    84
#define DT_PDRAGINFO                     85
#define DT_PDRAGITEM                     86
#define DT_PDRAGTRANSFER                 87
#define DT_PDRIVDATA                     88
#define DT_PEAOP                         89
#define DT_PEAOP2                        90
#define DT_PERRINFO                      91
/* NOTE: ANY CHANGES TO DT_PEXCEPTIONREGISTRATIONRECORD NEED TO BE REFLECTED IN T_DOS386.ASM */
#define DT_PEXCEPTIONREGISTRATIONRECORD  92
/* NOTE: ANY CHANGES TO DT_PEXCEPTIONREPORTRECORD NEED TO BE REFLECTED IN T_DOS386.ASM */
#define DT_PEXCEPTIONREPORTRECORD        93
#define DT_PFACENAMEDESC                 94
#define DT_PFATTRS                       95
#define DT_PFFDESCS                      96
#define DT_PFHLOCK                       97
#define DT_PFILEDLG                      98
#define DT_PFILEFINDBUF                  99
#define DT_PFILELOCK                    100
#define DT_PFILELOCKL                   101
#define DT_PFIXED                       102
#define DT_PFN                          103
#define DT_PFNEXITLIST                  104
#define DT_PFNSIGHANDLER                105
#define DT_PFNTHREAD                    106
#define DT_PFNWP                        107
#define DT_PFONTDLG                     108
#define DT_PFONTMETRICS                 109
#define DT_PFRAMECDATA                  110
#define DT_PFSQBUFFER2                  111
#define DT_PFSRSEM2                     112
#define DT_PGRADIENTL                   113
#define DT_PHCINFO                      114
#define DT_PHDIR                        115
#define DT_PHELPINIT                    116
#define DT_PHELPTABLE                   117
#define DT_PHEV                         118
#define DT_PHFILE                       119
#define DT_PHKBD                        120
#define DT_PHMODULE                     121
#define DT_PHMONITOR                    122
#define DT_PHMOU                        123
#define DT_PHMTX                        124
#define DT_PHMUX                        125
#define DT_PHPIPE                       126
#define DT_PHPROGARRAY                  127
#define DT_PHQUEUE                      128
#define DT_PHRGN                        129
#define DT_PHSEM                        130
#define DT_PHSPINLOCK                   131
#define DT_PHSYSSEM                     132
#define DT_PHTIMER                      133
#define DT_PHVDD                        134
#define DT_PHVPS                        135
#define DT_PHWND                        136
#define DT_PICONINFO                    137
#define DT_PID                          138
#define DT_PKBDHWID                     139
#define DT_PKBDINFO                     140
#define DT_PKBDKEYINFO                  141
#define DT_PKBDTRANS                    142
#define DT_PKERNINGPAIRS                143
#define DT_PLISTIO                      144
#define DT_PLONG                        145
#define DT_PLONGLONG                    146
#define DT_PMATRIXLF                    147
#define DT_PMB2INFO                     148
#define DT_PMOUEVENTINFO                149
#define DT_PMOUQUEINFO                  150
#define DT_PMPAFFINITY                  151
#define DT_PMQINFO                      152
#define DT_PNOPTRRECT                   153
#define DT_POBJCLASS                    154
#define DT_PPIBSTRUCT                   155
#define DT_PPID                         156
#define DT_PPIDINFO                     157
#define DT_PPIPESEMSTATE                158
#define DT_PPOINTERINFO                 159
#define DT_PPOINTL                      160
#define DT_PPOLYGON                     161
#define DT_PPRFPROFILE                  162
#define DT_PPRIDINFO                    163
#define DT_PPROGDETAILS                 164
#define DT_PPROGRAMENTRY                165
#define DT_PPROGTITLE                   166
#define DT_PPTRLOC                      167
#define DT_PPTRSHAPE                    168
#define DT_PPVOID                       169
#define DT_PQMOPENDATA                  170
#define DT_PQMRESULT                    171
#define DT_PQMSG                        172
#define DT_PQWORD                       173
#define DT_PRASTERIZERCAPS              174
#define DT_PRECTL                       175
#define DT_PREGISTERDATA                176
#define DT_PREQUESTDATA                 177
#define DT_PRESULTCODES                 178
#define DT_PRGNRECT                     179
#define DT_PROGCATEGORY                 180
#define DT_PSCALEFACT                   181
#define DT_PSEL                         182
#define DT_PSEMRECORD                   183
#define DT_PSHORT                       184
#define DT_PSIZEF                       185
#define DT_PSIZEL                       186
#define DT_PSTARTDATA                   187
#define DT_PSTATUSDATA                  188
#define DT_PSTR16                       189
#define DT_PSTR32                       190
#define DT_PSTR64                       191
#define DT_PSTR8                        192
#define DT_PSTRINGINBUF                 193
#define DT_PSWBLOCK                     194
#define DT_PSWCNTRL                     195
#define DT_PSWP                         196
#define DT_PSZ                          197
#define DT_pszErrorMsg                  198
#define DT_PTHREADCREATE                199
#define DT_PTHRESHOLD                   200
#define DT_PTID                         201
#define DT_PTRACKINFO                   202
#define DT_PUCHAR                       203
#define DT_PULONG                       204
#define DT_PUSHORT                      205
#define DT_PVIOCONFIGINFO               206
#define DT_PVIOCURSORINFO               207
#define DT_PVIOFONTINFO                 208
#define DT_PVIOMODEINFO                 209
#define DT_PVIOPHYSBUF                  210
/* NOTE: ANY CHANGES TO DT_PVOID NEED TO BE REFLECTED IN T_DOS386.ASM */
#define DT_PVOID                        211
#define DT_PWPOINT                      212
#define DT_SEL                          213
#define DT_SGID                         214
#define DT_SHORT                        215
#define DT_TID                          216
#define DT_UCHAR                        217
#define DT_ULONG                        218
#define DT_USHORT                       219
#define DT_GRESEMTSTAR                  220
#define DT_PBYTESTAR                    221
#define DT_PCHARFARSTAR                 222
#define DT_PCHARSTAR                    223
#define DT_PDRSTAR                      224
#define DT_PFNFARSTAR                   225
#define DT_PFNSIGHANDLERFARSTAR         226
#define DT_PFNSTAR                      227
#define DT_PIDSTAR                      228
#define DT_PIRSTAR                      229
#define DT_PPIBSTAR                     230
#define DT_PSZFARSTAR                   231
#define DT_PSZSTAR                      232
#define DT_PTIBSTAR                     233
#define DT_PULONGSTAR                   234
#define DT_PVOIDFARSTAR                 235
#define DT_TIDSTAR                      236
#define DT_ULONGSTAR                    237
#define DT_VOIDSTAR                     238

#define EM_VERMATCH                       0
#define EM_LOADDLL                        1
#define EM_GETPROC                        2
#define EM_DFFLIMIT                       3

#define PEAOP_GEA                       0x1
#define PEAOP_FEA                       0x2
#define PEAOP_ERR                       0x4
#define PEAOP_ALL                       0x7

/******************************************************************************/
/*                                                                            */
/* Macros                                                                     */
/*                                                                            */
/******************************************************************************/

#define DWALIGN(p)    {\
                        if (((ULONG)p)&(sizeof(ULONG)-1))\
                          (p)+=(sizeof(ULONG)-(((ULONG)p)%(sizeof(ULONG))));\
                      }

#ifdef T_USERHOOK
   #define HOOKENTRY(n,p)    if ( T_UserHook ) T_HookEntry( n, p )
   #define HOOKENTRY16(n,p)  if ( T_UserHook ) T_HookEntry( n, (PPVOID)p )
   #define HOOKEXIT(n,p,r)   if ( T_UserHook ) T_HookExit( n, p, r )
   #define HOOKEXIT16(n,p,r) if ( T_UserHook ) T_HookExit( n, (PPVOID)p, (PPVOID)r )
#else
   #define HOOKENTRY(n,p)
   #define HOOKENTRY16(n,p)
   #define HOOKEXIT(n,p,r)
   #define HOOKEXIT16(n,p,r)
#endif

#define HIWORD(p)     ( (USHORT)( ( (ULONG)p & 0xFFFF0000 ) >> 16 ) )

#define INDENTLOGIN   ulLogIndent++

#define INDENTLOGOUT  ulLogIndent--

#define LOGDOSAPI(f)  FLAGSET( T_DosAPIGrp, (f) )

#define LOGGPIAPI(f)  FLAGSET( T_GpiAPIGrp, (f) )

#define LOGLEVEL(n)   T_TraceLvl >= n

#define LOGWINAPI(f)  FLAGSET( T_WinAPIGrp, (f) )

#define LOWORD(p)     ( (USHORT)( (ULONG)p & 0x0000FFFF ) )

#define MAJORVER(v)   (( v & 0xFF00 ) >> 8)

#define MAKEVER(a,b)  (( a << 8 ) + b)

#define MAX(a,b)      ((a) > (b)) ? (a) : (b)

#define MIN(a,b)      ((a) < (b)) ? (a) : (b)

#define MINORVER(v)   ( v & 0xFF )

/******************************************************************************/
/*                                                                            */
/* Data Types                                                                 */
/*                                                                            */
/******************************************************************************/

typedef int (* _Seg16 _Far16 _Pascal     PFN16)();
typedef LONG * _Seg16                    PLONG16;
typedef PFN16 * _Seg16                   PPFN16;
typedef VOID * _Seg16 * _Seg16           PPVOID16;
typedef SHORT * _Seg16                   PSHORT16;
typedef CHAR * _Seg16                    PSZ16;
typedef ULONG * _Seg16                   PULONG16;
typedef USHORT * _Seg16                  PUSHORT16;
#ifdef INCL_BASE
typedef USHORT                           HDIR16;
typedef USHORT                           HFILE16;
typedef USHORT                           HKBD16;
typedef USHORT                           HMODULE16;
typedef USHORT                           HMONITOR16;
typedef USHORT                           HMOU16;
typedef USHORT                           HPIPE16;
typedef USHORT                           HQUEUE16;
typedef ULONG                            HSPINLOCK;
typedef USHORT                           HTIMER16;
typedef USHORT                           HVIO16;
typedef USHORT                           HVPS16;
typedef AVAILDATA * _Seg16               PAVAILDATA16;
typedef VOID * _Seg16                    PBUNDLE16;
typedef BYTE * _Seg16                    PBYTE16;
typedef CHAR * _Seg16                    PCH16;
typedef DATETIME * _Seg16                PDATETIME16;
typedef DENA1 * _Seg16                   PDENA116;
typedef FEA     * _Seg16                 PFEA16;
typedef FEALIST * _Seg16                 PFEALIST16;
typedef ULONG * _Seg16                   PFHLOCK16;
typedef FILEFINDBUF * _Seg16             PFILEFINDBUF16;
typedef FILEFINDBUF2 * _Seg16            PFILEFINDBUF216;
typedef FILELOCK * _Seg16                PFILELOCK16;
typedef FILESTATUS * _Seg16              PFILESTATUS16;
typedef FILESTATUS2 * _Seg16             PFILESTATUS216;
typedef VOID (* _Seg16 _Far16 _Pascal    PFNEXITLIST16)( USHORT );
typedef VOID (* _Seg16 _Far16 _Pascal    PFNSIGHANDLER16)( USHORT, USHORT );
typedef VOID (* _Seg16 _Far16 _Pascal    PFNTHREAD16)();
typedef FSALLOCATE * _Seg16              PFSALLOCATE16;
typedef FSQBUFFER * _Seg16               PFSQBUFFER16;
typedef GEA * _Seg16                     PGEA16;
typedef GEALIST * _Seg16                 PGEALIST16;
typedef USHORT * _Seg16                  PHDIR16;
typedef USHORT * _Seg16                  PHFILE16;
typedef USHORT * _Seg16                  PHKBD16;
typedef USHORT * _Seg16                  PHMODULE16;
typedef USHORT * _Seg16                  PHMONITOR16;
typedef USHORT * _Seg16                  PHMOU16;
typedef USHORT * _Seg16                  PHPIPE16;
typedef USHORT * _Seg16                  PHQUEUE16;
typedef USHORT * _Seg16                  PHTIMER16;
typedef ULONG * _Seg16                   PHSEM16;
typedef ULONG *                          PHSPINLOCK;
typedef ULONG * _Seg16                   PHSPINLOCK16;
typedef ULONG * _Seg16                   PHSYSSEM16;
typedef ULONG * _Seg16                   PHVDD16;
typedef USHORT * _Seg16                  PHVIO16;
typedef USHORT * _Seg16                  PHVPS16;
typedef USHORT                           PID16;
typedef KBDHWID * _Seg16                 PKBDHWID16;
typedef KBDINFO * _Seg16                 PKBDINFO16;
typedef KBDKEYINFO * _Seg16              PKBDKEYINFO16;
typedef KBDTRANS * _Seg16                PKBDTRANS16;
typedef ULONG *                          PLONGLONG;
typedef MOUEVENTINFO * _Seg16            PMOUEVENTINFO16;
typedef MOUQUEINFO * _Seg16              PMOUQUEINFO16;
typedef NOPTRRECT * _Seg16               PNOPTRRECT16;
typedef CHAR * _Seg16 * _Seg16           PPCHAR16;
typedef PFNSIGHANDLER16 * _Seg16         PPFNSIGHANDLER16;
typedef USHORT * _Seg16                  PPID16;
typedef PIPEINFO * _Seg16                PPIPEINFO16;
typedef PIPESEMSTATE * _Seg16            PPIPESEMSTATE16;
typedef CHAR * _Seg16 * _Seg16           PPSZ16;
typedef PTRLOC * _Seg16                  PPTRLOC16;
typedef PTRSHAPE * _Seg16                PPTRSHAPE16;
typedef QWORD * _Seg16                   PQWORD16;
typedef SCALEFACT * _Seg16               PSCALEFACT16;
typedef USHORT * _Seg16                  PSEL16;
typedef STATUSDATA * _Seg16              PSTATUSDATA16;
typedef STRINGINBUF * _Seg16             PSTRINGINBUF16;
typedef THRESHOLD * _Seg16               PTHRESHOLD16;
typedef USHORT * _Seg16                  PTID16;
typedef VIOCONFIGINFO * _Seg16           PVIOCONFIGINFO16;
typedef VIOCURSORINFO * _Seg16           PVIOCURSORINFO16;
typedef VIOFONTINFO * _Seg16             PVIOFONTINFO16;
typedef VIOINTENSITY * _Seg16            PVIOINTENSITY16;
typedef VIOOVERSCAN * _Seg16             PVIOOVERSCAN16;
typedef VIOPALSTATE * _Seg16             PVIOPALSTATE16;
typedef VIOSETTARGET * _Seg16            PVIOSETTARGET16;
typedef VIOSETULINELOC * _Seg16          PVIOSETULINELOC16;
typedef USHORT                           SGID16;
typedef USHORT                           TID16;
#endif
#ifdef INCL_PM
typedef USHORT                           ATOM16;
typedef ULONG                            HHEAP16;
typedef USHORT                           HLIB16;
typedef MT * _Seg16                      LPMT16;
typedef VOID * _Seg16                    MRESULT16;
typedef USHORT                           NPBYTE16;
typedef ACCELTABLE * _Seg16              PACCELTABLE16;
typedef ARCPARAMS * _Seg16               PARCPARAMS16;
typedef BITMAPINFO * _Seg16              PBITMAPINFO16;
typedef BITMAPINFO2 * _Seg16             PBITMAPINFO216;
typedef BITMAPINFOHEADER * _Seg16        PBITMAPINFOHEADER16;
typedef BITMAPINFOHEADER2 * _Seg16       PBITMAPINFOHEADER216;
typedef DDESTRUCT * _Seg16               PDDESTRUCT16;
typedef PSZ16 * _Seg16                   PDEVOPENDATA16;
typedef DESKTOP * _Seg16                 PDESKTOP16;
typedef DLGTEMPLATE * _Seg16             PDLGTEMPLATE16;
typedef DRAGIMAGE * _Seg16               PDRAGIMAGE16;
typedef DRAGINFO * _Seg16                PDRAGINFO16;
typedef DRAGITEM * _Seg16                PDRAGITEM16;
typedef DRIVDATA * _Seg16                PDRIVDATA16;
typedef FACENAMEDESC * _Seg16            PFACENAMEDESC16;
typedef FATTRS * _Seg16                  PFATTRS16;
typedef FFDESCS * _Seg16                 PFFDESCS16;
typedef FIXED * _Seg16                   PFIXED16;
typedef MRESULT (* _Seg16 _Far16 _Pascal PFNWP16)( HWND, USHORT, MPARAM, MPARAM );
typedef FONTMETRICS * _Seg16             PFONTMETRICS16;
typedef FRAMECDATA * _Seg16              PFRAMECDATA16;
typedef GRADIENTL * _Seg16               PGRADIENTL16;
typedef HCINFO * _Seg16                  PHCINFO16;
typedef USHORT * _Seg16                  PHELPSUBTABLE16;
typedef HPROGARRAY * _Seg16              PHPROGARRAY16;
typedef HRGN * _Seg16                    PHRGN16;
typedef HWND * _Seg16                    PHWND16;
typedef MATRIXLF * _Seg16                PMATRIXLF16;
typedef POINTL * _Seg16                  PPOINTL16;
typedef PSZ16 * _Seg16                   PQMOPENDATA16;
typedef RECTL * _Seg16                   PRECTL16;
typedef UCHAR                            PROGCATEGORY16;
typedef SIZEF * _Seg16                   PSIZEF16;
typedef SIZEL * _Seg16                   PSIZEL16;
typedef STR8 * _Seg16                    PSTR816;
typedef STR16 * _Seg16                   PSTR1616;
typedef STR32 * _Seg16                   PSTR3216;
typedef STR64 * _Seg16                   PSTR6416;
typedef USHORT                           SPLERR16;
#endif

/******************************************************************************/
/*                                                                            */
/* Structure declarations                                                     */
/*                                                                            */
/******************************************************************************/

typedef struct _dlltable                         /* DLL table                 */
{
   ULONG hmodDLL;                                /* DLL module handle         */
   ULONG hmodTraceDLL;                           /* Trace DLL module handle   */
}  DLLTABLE, *PDLLTABLE;

#pragma pack(1)
#ifdef INCL_BASE
typedef struct _COUNTRYCODE16                    /* 16-bit COUNTRYCODE        */
{
   USHORT country;
   USHORT codepage;
}  COUNTRYCODE16, * _Seg16 PCOUNTRYCODE16;

typedef struct _COUNTRYINFO16                    /* 16-bit COUNTRYINFO        */
{
   USHORT country;
   USHORT codepage;
   USHORT fsDateFmt;
   CHAR   szCurrency[5];
   CHAR   szThousandsSeparator[2];
   CHAR   szDecimal[2];
   CHAR   szDateSeparator[2];
   CHAR   szTimeSeparator[2];
   UCHAR  fsCurrencyFmt;
   UCHAR  cDecimalPlace;
   UCHAR  fsTimeFmt;
   USHORT abReserved1[2];
   CHAR   szDataSeparator[2];
   USHORT abReserved2[5];
}  COUNTRYINFO16, * _Seg16 PCOUNTRYINFO16;

typedef struct _DOSFSRSEM16                      /* 16-bit DOSFSRSEM          */
{
   USHORT cb;
   PID16  pid;
   TID16  tid;
   USHORT cUsage;
   USHORT client;
   ULONG  sem;
}  DOSFSRSEM16, * _Seg16 PDOSFSRSEM16;

typedef struct _EAOP16                           /* 16-bit EAOP               */
{
   PGEALIST16 fpGEAList;
   PFEALIST16 fpFEAList;
   ULONG      oError;
}  EAOP16, * _Seg16 PEAOP16;

typedef struct _FILELOCKL                        /* 32-bit FILELOCKL          */
{
   ULONG lOffsetLo;
   ULONG lOffsetHi;
   ULONG lRangeLo;
   ULONG lRangeHi;
}  FILELOCKL, *PFILELOCKL;

typedef struct _VOLUMELABEL16                    /* 16-bit VOLUMELABEL        */
{
   BYTE cch;
   CHAR szVolLabel[12];
}  VOLUMELABEL16, * _Seg16 PVOLUMELABEL16;

typedef struct _FSINFO16                         /* 16-bit FSINFO             */
{
   ULONG         ulVSN;
   VOLUMELABEL16 vol;
}  FSINFO16, * _Seg16 PFSINFO16;

typedef struct _FSRSEM216                        /* 16-bit FSRSEM2            */
{
   USHORT Length;
   USHORT Padding;
   USHORT ProcID;
   USHORT ThrdID;
   USHORT Usage;
   USHORT Client;
   ULONG  Timeout;
   ULONG  RAMSem;
}  FSRSEM216, * _Seg16 PFSRSEM216;

typedef struct _GINFOSEG16                       /* 16-bit GINFOSEG           */
{
   ULONG  time;
   ULONG  msecs;
   UCHAR  hour;
   UCHAR  minutes;
   UCHAR  seconds;
   UCHAR  hundredths;
   USHORT timezone;
   USHORT cusecTimerInterval;
   UCHAR  day;
   UCHAR  month;
   USHORT year;
   UCHAR  weekday;
   UCHAR  uchMajorVersion;
   UCHAR  uchMinorVersion;
   UCHAR  chRevisionLetter;
   UCHAR  sgCurrent;
   UCHAR  sgMax;
   UCHAR  cHugeShift;
   UCHAR  fProtectModeOnly;
   USHORT pidForeground;
   UCHAR  fDynamicSched;
   UCHAR  csecMaxWait;
   USHORT cmsecMinSlice;
   USHORT cmsecMaxSlice;
   USHORT bootdrive;
   UCHAR  amecRAS[32];
   UCHAR  csgWindowableVioMax;
   UCHAR  csgPMMax;
}  GINFOSEG16, * _Seg16 PGINFOSEG16;

typedef struct _GRESEMT                          /* 32-bit GRESEM             */
{
   BYTE  gs_acIdent[7];
   BYTE  gs_fcSet;
   ULONG gs_ulProcessThread;
   ULONG gs_ulNestedUseCount;
   ULONG gs_ulWaitingCount;
   ULONG gs_ulUseCount;
   ULONG gs_ulEventHandle;
   ULONG gs_ulCallerAddr;
}  GRESEMT, *PGRESEMT;

typedef struct _LINFOSEG16                       /* 16-bit LINFOSEG           */
{
   PID16     pidCurrent;
   PID16     pidParent;
   USHORT    prtyCurrent;
   TID16     tidCurrent;
   USHORT    sgCurrent;
   UCHAR     rfProcStatus;
   UCHAR     dummy1;
   BOOL16    fForeground;
   UCHAR     typeProcess;
   UCHAR     dummy2;
   SEL       selEnvironment;
   USHORT    offCmdLine;
   USHORT    cbDataSegment;
   USHORT    cbStack;
   USHORT    cbHeap;
   HMODULE16 hmod;
   SEL       selDS;
}  LINFOSEG16, * _Seg16 PLINFOSEG16;

#define LISTIO_READ      0x0004
#define LISTIO_WRITE     0x0008
#define LISTIO_ORDERED   0x0001
#define LISTIO_UNORDERED 0x0002
typedef struct _LISTIO_CB                        /* 32-bit LISTIO             */
{
   HFILE hFile;
   ULONG CmdFlag;
   LONG  Offset;
   PVOID pBuffer;
   ULONG NumBytes;
   ULONG Actual;
   ULONG RetCode;
   ULONG Reserved;
   ULONG Reserved2[3];
   ULONG Reserved3[2];
}  LISTIO, *PLISTIO;

typedef struct _LISTIO_CBL                       /* 32-bit LISTIOL            */
{
   HFILE hFile;
   ULONG CmdFlag;
   ULONG OffsetLo;
   ULONG OffsetHi;
   PVOID pBuffer;
   ULONG NumBytes;
   ULONG Actual;
   ULONG RetCode;
   ULONG Reserved;
   ULONG Reserved2[3];
   ULONG Reserved3[2];
}  LISTIOL, *PLISTIOL;

typedef struct _MPAFFINITY                       /* 32-bit MPAFFINITY         */
{
   ULONG mask[2];
}  MPAFFINITY, *PMPAFFINITY;

typedef struct _MUXSEM16                         /* 16-bit MUXSEM             */
{
   USHORT zero;
   HSEM   hsem;
}  MUXSEM16, * _Seg16 PMUXSEM16;

typedef struct _MUXSEMLIST16                     /* 16-bit MUXSEMLIST         */
{
   USHORT   cmxs;
   MUXSEM16 amxs[16];
}  MUXSEMLIST16, * _Seg16 PMUXSEMLIST16;

typedef unsigned long LADDR;                     /* 32-bit LADDR              */

typedef struct _PDR                              /* 32-bit PDR                */
{
   BYTE   bFunc;
   USHORT iThread;
   LADDR  laddrModule;
   ULONG  cbDumpData;
   PBYTE  abDumpData;
}  PDR, *PPDR;

typedef struct _PIDINFO16                        /* 16-bit PIDINFO            */
{
   PID16 pid;
   TID16 tid;
   PID16 pidParent;
}  PIDINFO16, * _Seg16 PPIDINFO16;

typedef struct _PROF_RANGE                       /* 32-bit PROF_RANGE         */
{
   LADDR laddrRange;
   ULONG cbRange;
   ULONG fFullCollection;
}  PROF_RANGE, *PPROF_RANGE;

typedef struct _PIR                              /* 32-bit PIR                */
{
   PROF_RANGE *aRange;
   USHORT      cRange;
   USHORT      fsOptions;
   ULONG       cbRecordBuffer;
   USHORT      sSamplePeriod;
   PBYTE       aRandomCount;
   BYTE        cRandomCount;
}  PIR, *PPIR;

typedef struct _QMRESULT16                       /* 16-bit QMRESULT           */
{
   USHORT seg;
   USHORT hmte;
   char   name[CCHMAXPATH];
}  QMRESULT16, * _Seg16 PQMRESULT16;

typedef struct _THREADCREATE                     /* 32-bit THREADCREATE       */
{
   ULONG     cbSize;
   PTID      pTid;
   PFNTHREAD pfnStart;
   ULONG     lParam;
   ULONG     lFlag;
   PBYTE     pStack;
   ULONG     cbStack;
}  THREADCREATE, *PTHREADCREATE;

typedef struct _Ptrace_B16                       /* 16-bit Ptrace_B           */
{
   PID16     PID;
   TID16     TID;
   USHORT    Cmd;
   USHORT    Value;
   USHORT    OffV;
   USHORT    SegV;
   HMODULE16 MTE;
}  Ptrace_B16, * _Seg16 PPtrace_B16;

typedef struct _REGISTERDATA16                   /* 16-bit REGISTERDATA       */
{
   USHORT Length;
   USHORT NotifType;
   PSZ16  DDName;
}  REGISTERDATA16, *PREGISTERDATA16;

typedef struct _REGS16                           /* 16-bit registers          */
{
   USHORT usParmBytes;                           /* Parameter bytes           */
   USHORT usCX;                                  /* CX                        */
   USHORT usDX;                                  /* DX                        */
   ULONG  ulCSIP;                                /* CS:IP                     */
}  REGS16, *PREGS16;

typedef struct _RESULTCODES16                    /* 16-bit RESULTCODES        */
{
   USHORT codeTerminate;
   USHORT codeResult;
}  RESULTCODES16, * _Seg16 PRESULTCODES16;

typedef struct _STARTDATA16                      /* 16-bit STARTDATA          */
{
   USHORT  Length;
   USHORT  Related;
   USHORT  FgBg;
   USHORT  TraceOpt;
   PSZ16   PgmTitle;
   PSZ16   PgmName;
   PBYTE16 PgmInputs;
   PBYTE16 TermQ;
   PBYTE16 Environment;
   USHORT  InheritOpt;
   USHORT  SessionType;
   PSZ16   IconFile;
   ULONG   PgmHandle;
   USHORT  PgmControl;
   USHORT  InitXPos;
   USHORT  InitYPos;
   USHORT  InitXSize;
   USHORT  InitYSize;
   USHORT  Reserved;
   PSZ16   ObjectBuffer;
   ULONG   ObjectBuffLen;
}  STARTDATA16, * _Seg16 PSTARTDATA16;

typedef struct _VIOCOLORREG16                    /* 16-bit VIOCOLORREG        */
{
   USHORT cb;
   USHORT type;
   USHORT firstcolorreg;
   USHORT numcolorregs;
   PCH16  colorregaddr;
}  VIOCOLORREG16, * _Seg16 PVIOCOLORREG16;

typedef struct _VIOMODEINFO16                    /* 16-bit VIOMODEINFO        */
{
   USHORT cb;
   UCHAR  fbType;
   UCHAR  color;
   USHORT col;
   USHORT row;
   USHORT hres;
   USHORT vres;
   UCHAR  fmt_ID;
   UCHAR  attrib;
   ULONG  buf_addr;
   ULONG  buf_length;
   ULONG  full_length;
   ULONG  partial_length;
   PCH16  ext_data_addr;
}  VIOMODEINFO16, * _Seg16 PVIOMODEINFO16;

typedef struct _VIOPHYSBUF16                     /* 16-bit VIOPHYSBUF         */
{
   PBYTE16 pBuf;
   ULONG   cb;
   SEL     asel[1];
}  VIOPHYSBUF16, * _Seg16 PVIOPHYSBUF16;
#endif
#ifdef INCL_PM
typedef struct _CATCHBUF16                       /* 16-bit CATCHBUF           */
{
   ULONG reserved[4];
}  CATCHBUF16, * _Seg16 PCATCHBUF16;

typedef struct _CHOOSECOLOR                      /* 32-bit CHOOSECOLOR        */
{
   ULONG   cbSize;
   ULONG   fl;
   PULONG  paulCustomColors;
   ULONG   ulReturn;
   ULONG   ulDlgResult;
   LONG    lSRC;
   ULONG   ulUser;
   PSZ     pszTitle;
   PFNWP   pfnDlgProc;
   HWND    hwndNotify;
   HMODULE hmod;
   ULONG   ulDlgId;
   PVOID   pvDlgTemplate;
   LONG    x;
   LONG    y;
   ULONG   ulReserved1;
   ULONG   ulReserved2;
}  CHOOSECOLOR, *PCHOOSECOLOR;

typedef struct _CLASSINFO16                      /* 16-bit CLASSINFO          */
{
   ULONG   flClassStyle;
   PFNWP16 pfnWindowProc;
   USHORT  cbWindowData;
}  CLASSINFO16, * _Seg16 PCLASSINFO16;

typedef struct _CTLCOLOR                         /* 32-bit CTLCOLOR           */
{
   LONG clrIndex;
   LONG clrValue;
}  CTLCOLOR, *PCTLCOLOR;

typedef struct _CURSORINFO16                     /* 16-bit CURSORINFO         */
{
   HWND   hwnd;
   SHORT  x;
   SHORT  y;
   SHORT  cx;
   SHORT  cy;
   USHORT fs;
   RECTL  rclClip;
}  CURSORINFO16, * _Seg16 PCURSORINFO16;

typedef struct _DEVOPENSTRUC16                   /* 16-bit DEVOPENSTRUC       */
{
   PSZ16       pszLogAddress;
   PSZ16       pszDriverName;
   PDRIVDATA16 pdriv;
   PSZ16       pszDataType;
   PSZ16       pszComment;
   PSZ16       pszQueueProcName;
   PSZ16       pszQueueProcParams;
   PSZ16       pszSpoolerParams;
   PSZ16       pszNetworkParams;
}  DEVOPENSTRUC16, * _Seg16 PDEVOPENSTRUC16;

typedef struct _DRAGTRANSFER16                   /* 16-bit DRAGTRANSFER       */
{
   ULONG       cb;
   HWND        hwndClient;
   PDRAGITEM16 pditem;
   HSTR        hstrSelectedRMF;
   HSTR        hstrRenderToName;
   ULONG       ulTargetInfo;
   USHORT      usOperation;
   USHORT      fsReply;
}  DRAGTRANSFER16, * _Seg16 PDRAGTRANSFER16;

typedef struct _ERRINFO16                        /* 16-bit ERRINFO            */
{
   USHORT  cbFixedErrInfo;
   ERRORID idError;
   USHORT  cDetailLevel;
   USHORT  offaoffszMsg;
   USHORT  offBinaryData;
}  ERRINFO16, * _Seg16 PERRINFO16;

typedef struct _FINDDLG                          /*32-bit  FINDDLG            */
{
   ULONG        cbSize;
   HWND         hwndOwner;
   HMODULE      hmod;
   ULONG        fl;
   PSZ          pszFindWhat;
   PSZ          pszReplaceWith;
   USHORT       cbFindWhat;
   USHORT       cbReplaceWith;
   ULONG        ulUser;
   PFNWP        pfnDlgProc;
   ULONG        ulDlgId;
   PDLGTEMPLATE pDlgTemplate;
   LONG         x;
   LONG         y;
   ULONG        ulReturn;
   LONG         lSRC;
}  FINDDLG, *PFINDDLG;

typedef struct _PORTFROMQ                        /* 32-bit PORTFROMQ          */
{
   PSZ   pszComputer;
   PSZ   pszPort;
   PSZ   pszDeviceID;
   ULONG flBidiCapabilities;
   ULONG flJobs;
}  PORTFROMQ, *PPORTFROMQ;

typedef struct _GETPORTFROMQ                     /* 32-bit GETPORTFROMQ       */
{
   ULONG     ulPorts;
   PORTFROMQ PortFromQ[1];
}  GETPORTFROMQ, *PGETPORTFROMQ;

typedef struct _HELPTABLE16                      /* 16-bit HELPTABLE          */
{
   USHORT          idAppWindow;
   PHELPSUBTABLE16 phstHelpSubTable;
   USHORT          idExtPanel;
}  HELPTABLE16, * _Seg16 PHELPTABLE16;

typedef struct _HELPINIT16                       /* 16-bit HELPINIT           */
{
   USHORT       cb;
   ULONG        ulReturnCode;
   PSZ16        pszTutorialName;
   PHELPTABLE16 phtHelpTable;
   USHORT       hmodHelpTableModule;
   USHORT       hmodAccelActionBarModule;
   USHORT       idAccelTable;
   USHORT       idActionBar;
   PSZ16        pszHelpWindowTitle;
   USHORT       usShowPanelId;
   PSZ16        pszHelpLibraryName;
}  HELPINIT16, * _Seg16 PHELPINIT16;

typedef struct _KERNINGPAIRS16                   /* 16-bit KERNINGPAIRS       */
{
   SHORT sFirstChar;
   SHORT sSecondChar;
   SHORT sKerningAmount;
}  KERNINGPAIRS16, * _Seg16 PKERNINGPAIRS16;

typedef struct _MQINFO16                         /* 16-bit MQINFO             */
{
   USHORT  cb;
   USHORT  pid;
   USHORT  tid;
   USHORT  cmsgs;
   PVOID16 pReserved;
}  MQINFO16, * _Seg16 PMQINFO16;

typedef struct _PDALERTINFO                      /* 32-bit PDALERTINFO        */
{
   ULONG ulVersion;
   ULONG ulFlags;
   ULONG ulCommand;
}  PDALERTINFO, *PPDALERTINFO;

typedef struct _PROGTYPE16                       /* 16-bit PROGTYPE           */
{
   char  progc;
   UCHAR fbVisible;
}  PROGTYPE16, * _Seg16 PPROGTYPE16;

typedef struct _XYWINSIZE16                      /* 16-bit XYWINSIZE          */
{
   SHORT  x;
   SHORT  y;
   SHORT  cx;
   SHORT  cy;
   USHORT fsWindow;
}  XYWINSIZE16, * _Seg16 PXYWINSIZE;

typedef struct _PIBSTRUCT16                      /* 16-bit PIBSTRUCT          */
{
   PROGTYPE16  progt;
   CHAR        szTitle[MAXNAMEL+1];
   CHAR        szIconFileName[MAXPATHL+1];
   CHAR        szExecutable[MAXPATHL+1];
   CHAR        szStartupDir[MAXPATHL+1];
   XYWINSIZE16 xywinInitial;
   USHORT      res1;
   LHANDLE     res2;
   USHORT      cchEnvironmentVars;
   PCH16       pchEnvironmentVars;
   USHORT      cchProgramParameter;
   PCH16       pchProgramParameter;
}  PIBSTRUCT16, * _Seg16 PPIBSTRUCT16;

typedef struct _POINTERINFO16                    /* 16-bit POINTERINFO        */
{
   USHORT  fPointer;
   SHORT   xHotspot;
   SHORT   yHotspot;
   HBITMAP hbmPointer;
   HBITMAP hbmColor;
}  POINTERINFO16, * _Seg16 PPOINTERINFO16;

#define CNLEN 15
#define DTLEN 9
#define PDLEN 8
#define QNLEN 12
#define UNLEN 20
typedef struct _PRDINFO16                        /* 16-bit PRDINFO            */
{
   CHAR   szName[PDLEN+1];
   CHAR   szUserName[UNLEN+1];
   USHORT uJobId;
   USHORT fsStatus;
   PSZ16  pszStatus;
   USHORT time;
}  PRDINFO16, * _Seg16 PPRDINFO16;

typedef struct _PRDINFO316                       /* 16-bit PRDINFO3           */
{
   PSZ16  pszPrinterName;
   PSZ16  pszUserName;
   PSZ16  pszLogAddr;
   USHORT uJobId;
   USHORT fsStatus;
   PSZ16  pszStatus;
   PSZ16  pszComment;
   PSZ16  pszDrivers;
   USHORT time;
   USHORT usTimeOut;
}  PRDINFO316, * _Seg16 PPRDINFO316;

typedef struct _PRDRIVINFO2                      /* 32-bit PRDRIVINFO2        */
{
   PSZ    pszPrinterName;
   PSZ    pszDriverName;
   USHORT usFlags;
   USHORT cDriverProps;
}  PRDRIVINFO2, *PPRDRIVINFO2;

typedef struct _PRFPROFILE16                     /* 16-bit PRFPROFILE         */
{
   ULONG cchUserName;
   PSZ16 pszUserName;
   ULONG cchSysName;
   PSZ16 pszSysName;
}  PRFPROFILE16, * _Seg16 PPRFPROFILE16;

typedef struct _PRIDINFO16                       /* 16-bit PRIDINFO           */
{
   USHORT uJobId;
   CHAR   szComputerName[CNLEN+1];
   CHAR   szQueueName[QNLEN+1];
   CHAR   pad_1;
}  PRIDINFO16, * _Seg16 PPRIDINFO16;

typedef struct _DJPITEM                          /* 32-bit DJP_ITEM           */
{
   ULONG cb;
   ULONG ulProperty;
   LONG  lType;
   ULONG ulNumReturned;
   ULONG ulValue;
}  DJP_ITEM, *PDJP_ITEM;

typedef struct _PRINTDLG                         /* 32-bit PRINTDLG           */
{
   ULONG        cbSize;
   ULONG        fl;
   HWND         hwndOwner;
   PFNWP        pfnDlgProc;
   ULONG        ulUser;
   HMODULE      hmod;
   ULONG        ulDlgId;
   PDLGTEMPLATE pDlgTemplate;
   LONG         x;
   LONG         y;
   ULONG        ulReturn;
   LONG         lSRC;
   PSZ          pszDocName;
   PSZ          pszOption1;
   PSZ          pszOption2;
   PSZ          pszOption3;
   PSZ          pszOption4;
   ULONG        ulMinPage;
   ULONG        ulMaxPage;
   ULONG        ulFromPage;
   ULONG        ulToPage;
   ULONG        ulCopies;
   ULONG        flResult;
   PSZ          pszQueueName;
   PDRIVDATA    pPrJobProp;
   PDJP_ITEM    pAppJobProp;
   LONG         cbAppJobPropSize;
   LONG         lAppJobPropRC;
   HDC          hdc;
   PVOID        pReserved;
}  PRINTDLG, *PPRINTDLG;

typedef struct _PRJINFO16                        /* 16-bit PRJINFO            */
{
   USHORT uJobId;
   CHAR   szUserName[UNLEN+1];
   CHAR   pad_1;
   CHAR   szNotifyName[CNLEN+1];
   CHAR   szDataType[DTLEN+1];
   PSZ16  pszParms;
   USHORT uPosition;
   USHORT fsStatus;
   PSZ16  pszStatus;
   ULONG  ulSubmitted;
   ULONG  ulSize;
   PSZ16  pszComment;
}  PRJINFO16, * _Seg16 PPRJINFO16;

typedef struct _PRJINFO216                       /* 16-bit PRJINFO2           */
{
   USHORT uJobId;
   USHORT uPriority;
   PSZ16  pszUserName;
   USHORT uPosition;
   USHORT fsStatus;
   ULONG  ulSubmitted;
   ULONG  ulSize;
   PSZ16  pszComment;
   PSZ16  pszDocument;
}  PRJINFO216, * _Seg16 PPRJINFO216;

typedef struct _PRJINFO316                       /* 16-bit PRJINFO3           */
{
   USHORT      uJobId;
   USHORT      uPriority;
   PSZ16       pszUserName;
   USHORT      uPosition;
   USHORT      fsStatus;
   ULONG       ulSubmitted;
   ULONG       ulSize;
   PSZ16       pszComment;
   PSZ16       pszDocument;
   PSZ16       pszNotifyName;
   PSZ16       pszDataType;
   PSZ16       pszParms;
   PSZ16       pszStatus;
   PSZ16       pszQueue;
   PSZ16       pszQProcName;
   PSZ16       pszQProcParms;
   PSZ16       pszDriverName;
   PDRIVDATA16 pDriverData;
   PSZ16       pszPrinterName;
}  PRJINFO316, * _Seg16 PPRJINFO316;

#define PRJ_SPOOLFILENAME_PARMNUM 19
#define PRJ_PAGESSPOOLED_PARMNUM  20
#define PRJ_PAGESSENT_PARMNUM     21
#define PRJ_PAGESPRINTED_PARMNUM  22
#define PRJ_TIMEPRINTED_PARMNUM   23
#define PRJ_EXTENDSTATUS_PARMNUM  24
#define PRJ_STARTPAGE_PARMNUM     25
#define PRJ_ENDPAGE_PARMNUM       26
typedef struct _PRJINFO4                         /* 32-bit PRJINFO4           */
{
   USHORT uJobId;
   USHORT uPriority;
   PSZ    pszUserName;
   USHORT uPosition;
   USHORT fsStatus;
   ULONG  ulSubmitted;
   ULONG  ulSize;
   PSZ    pszComment;
   PSZ    pszDocument;
   PSZ    pszSpoolFileName;
   PSZ    pszPortName;
   PSZ    pszStatus;
   ULONG  ulPagesSpooled;
   ULONG  ulPagesSent;
   ULONG  ulPagesPrinted;
   ULONG  ulTimePrinted;
   ULONG  ulExtendJobStatus;
   ULONG  ulStartPage;
   ULONG  ulEndPage;
} PRJINFO4, *PPRJINFO4;

typedef struct _PRJINFO416                       /* 16-bit PRJINFO4           */
{
   USHORT uJobId;
   USHORT uPriority;
   PSZ16  pszUserName;
   USHORT uPosition;
   USHORT fsStatus;
   ULONG  ulSubmitted;
   ULONG  ulSize;
   PSZ16  pszComment;
   PSZ16  pszDocument;
   PSZ16  pszSpoolFileName;
   PSZ16  pszPortName;
   PSZ16  pszStatus;
   ULONG  ulPagesSpooled;
   ULONG  ulPagesSent;
   ULONG  ulPagesPrinted;
   ULONG  ulTimePrinted;
   ULONG  ulExtendJobStatus;
   ULONG  ulStartPage;
   ULONG  ulEndPage;
}  PRJINFO416, * _Seg16 PPRJINFO416;

typedef struct _PRPORTINFO16                     /* 16-bit PRPORTINFO         */
{
   CHAR szPortName[PDLEN+1];
}  PRPORTINFO16, * _Seg16 PPRPORTINFO16;

typedef struct _PRPORTINFO116                    /* 16-bit PRPORTINFO1        */
{
   PSZ16 pszPortName;
   PSZ16 pszPortDriverName;
   PSZ16 pszPortDriverPathName;
}  PRPORTINFO116, * _Seg16 PPRPORTINFO116;

typedef struct _PRPORTINFO2                      /* 32-bit PRPORTINFO2        */
{
   PSZ   pszPortName;
   PSZ   pszPortDriver;
   PSZ   pszProtocolConverter;
   ULONG ulReserved;
   ULONG ulMode;
   ULONG ulPriority;
}  PRPORTINFO2, *PPRPORTINFO2;

typedef struct _PRQINFO16                        /* 16-bit PRQINFO            */
{
   CHAR   szName[QNLEN+1];
   CHAR   pad_1;
   USHORT uPriority;
   USHORT uStartTime;
   USHORT uUntilTime;
   PSZ16  pszSepFile;
   PSZ16  pszPrProc;
   PSZ16  pszDestinations;
   PSZ16  pszParms;
   PSZ16  pszComment;
   USHORT fsStatus;
   USHORT cJobs;
}  PRQINFO16, * _Seg16 PPRQINFO16;

typedef struct _PRQINFO316                       /* 16-bit PRQINFO3           */
{
   PSZ16       pszName;
   USHORT      uPriority;
   USHORT      uStartTime;
   USHORT      uUntilTime;
   USHORT      fsType;
   PSZ16       pszSepFile;
   PSZ16       pszPrProc;
   PSZ16       pszParms;
   PSZ16       pszComment;
   USHORT      fsStatus;
   USHORT      cJobs;
   PSZ16       pszPrinters;
   PSZ16       pszDriverName;
   PDRIVDATA16 pDriverData;
}  PRQINFO316, * _Seg16 PPRQINFO316;

typedef struct _PRQINFO616                       /* 16-bit PRQINFO6           */
{
   PSZ16       pszName;
   USHORT      uPriority;
   USHORT      uStartTime;
   USHORT      uUntilTime;
   USHORT      fsType;
   PSZ16       pszSepFile;
   PSZ16       pszPrProc;
   PSZ16       pszParms;
   PSZ16       pszComment;
   USHORT      fsStatus;
   USHORT      cJobs;
   PSZ16       pszPrinters;
   PSZ16       pszDriverName;
   PDRIVDATA16 pDriverData;
   PSZ16       pszRemoteComputerName;
   PSZ16       pszRemoteQueueName;
}  PRQINFO616, * _Seg16 PPRQINFO616;

#define PRQ_SETUP_STRING_PARMNUM 17
typedef struct _PRQINFO8                         /* 32-bit PRQINFO8           */
{
   PSZ    pszName;
   USHORT fsType;
   USHORT fsStatus;
   ULONG  flSettings;
   ULONG  ulTimeoutToBackup;
   ULONG  ulMaxJobs;
   ULONG  ulMaxPagesPerJob;
   ULONG  ulMaxKBPerJob;
   PSZ    pszComment;
   PSZ    pszPorts;
   PSZ    pszDriverName;
   PSZ    pszRemoteComputerName;
   PSZ    pszRemoteQueueName;
   PSZ    pszBackupComputerName;
   PSZ    pszBackupQueueName;
   PSZ    pszQueueSetup;
}  PRQINFO8, *PPRQINFO8;

typedef struct _PRQINFO816                       /* 16-bit PRQINFO8           */
{
   PSZ16  pszName;
   USHORT fsType;
   USHORT fsStatus;
   ULONG  flSettings;
   ULONG  ulTimeoutToBackup;
   ULONG  ulMaxJobs;
   ULONG  ulMaxPagesPerJob;
   ULONG  ulMaxKBPerJob;
   PSZ16  pszComment;
   PSZ16  pszPorts;
   PSZ16  pszDriverName;
   PSZ16  pszRemoteComputerName;
   PSZ16  pszRemoteQueueName;
   PSZ16  pszBackupComputerName;
   PSZ16  pszBackupQueueName;
   PSZ16  pszQueueSetup;
}  PRQINFO816, * _Seg16 PPRQINFO816;

typedef struct _PRQPROCINFO16                    /* 16-bit PRQPROCINFO        */
{
   CHAR szQProcName[QNLEN+1];
}  PRQPROCINFO16, * _Seg16 PPRQPROCINFO16;

typedef struct _PRTALERT                         /* 32-bit PRTALERT           */
{
   ULONG  ulpszPortName;
   ULONG  ulCategory;
   ULONG  ulType;
   BYTE   interpreterID;
   BYTE   bReserved;
   USHORT usSeverity;
   ULONG  ulValue;
   ULONG  ulPrinterJobID;
   ULONG  ulpszAlertMsg;
}  PRTALERT, *PPRTALERT;

typedef struct _QMJOBINFO                        /* 32-bit QMJOBINFO          */
{
   ULONG ulJobID;
   PSZ   pszComputerName;
   PSZ   pszQueueName;
}  QMJOBINFO, *PQMJOBINFO;

typedef struct _SWP16                            /* 16-bit SWP                */
{
   USHORT fs;
   SHORT  cy;
   SHORT  cx;
   SHORT  y;
   SHORT  x;
   HWND   hwndInsertBehind;
   HWND   hwnd;
}  SWP16, * _Seg16 PSWP16;

typedef struct _PROGDETAILS16                    /* 16-bit PROGDETAILS        */
{
   ULONG      Length;
   PROGTYPE16 progt;
   USHORT     pad1[3];
   PSZ16      pszTitle;
   PSZ16      pszExecutable;
   PSZ16      pszParameters;
   PSZ16      pszStartupDir;
   PSZ16      pszIcon;
   PSZ16      pszEnvironment;
   SWP16      swpInitial;
   USHORT     pad2[5];
}  PROGDETAILS16, * _Seg16 PPROGDETAILS16;

typedef struct _PROGRAMENTRY16                   /* 16-bit PROGRAMENTRY       */
{
   HPROGRAM   hprog;
   PROGTYPE16 progt;
   CHAR       szTitle[MAXNAMEL+1];
}  PROGRAMENTRY16, * _Seg16 PPROGRAMENTRY16;

typedef struct _PROGTITLE16                      /* 16-bit PROGTITLE          */
{
   HPROGRAM   hprog;
   PROGTYPE16 progt;
   USHORT     pad1[3];
   PSZ16      pszTitle;
}  PROGTITLE16, * _Seg16 PPROGTITLE16;

typedef struct _QMSG16                           /* 16-bit QMSG               */
{
   HWND   hwnd;
   USHORT msg;
   MPARAM mp1;
   MPARAM mp2;
   ULONG  time;
   POINTL ptl;
}  QMSG16, * _Seg16 PQMSG16;

typedef struct _RASTERIZERCAPS                   /* 32-bit RASTERIZERCAPS     */
{
   ULONG cb;
   ULONG ulAvailFlags;
   ULONG ulEnabledFlags;
}  RASTERIZERCAPS, *PRASTERIZERCAPS;

typedef struct _RGNRECT16                        /* 16-bit RGNRECT            */
{
   USHORT ircStart;
   USHORT crc;
   USHORT crcReturned;
   USHORT usDirection;
}  RGNRECT16, * _Seg16 PRGNRECT16;

typedef struct _SWCNTRL16                        /* 16-bit SWCNTRL            */
{
   HWND     hwnd;
   HWND     hwndIcon;
   HPROGRAM hprog;
   USHORT   idProcess;
   USHORT   idSession;
   BYTE     uchVisibility;
   BYTE     fbJump;
   CHAR     szSwtitle[MAXNAMEL+1];
   BYTE     bProgType;
}  SWCNTRL16, * _Seg16 PSWCNTRL16;

typedef struct _SWENTRY16                        /* 16-bit SWENTRY            */
{
   HSWITCH   hswitch;
   SWCNTRL16 swctl;
}  SWENTRY16, * _Seg16 PSWENTRY16;

typedef struct _SWBLOCK16                        /* 16-bit SWBLOCK            */
{
   USHORT    cswentry;
   SWENTRY16 aswentry[1];
}  SWBLOCK16, * _Seg16 PSWBLOCK16;

typedef struct _TRACKINFO16                      /* 16-bit TRACKINFO          */
{
   SHORT  cxBorder;
   SHORT  cyBorder;
   SHORT  cxGrid;
   SHORT  cyGrid;
   SHORT  cxKeyboard;
   SHORT  cyKeyboard;
   RECTL  rclTrack;
   RECTL  rclBoundary;
   POINTL ptlMinTrackSize;
   POINTL ptlMaxTrackSize;
   USHORT fs;
}  TRACKINFO16, * _Seg16 PTRACKINFO16;

typedef struct _WPOINT16                         /* 16-bit WPOINT             */
{
   SHORT x;
   SHORT dummy1;
   SHORT y;
   SHORT dummy2;
}  WPOINT16, * _Seg16 PWPOINT16;
#endif
#pragma pack()

/******************************************************************************/
/*                                                                            */
/* Global variable declarations                                               */
/*                                                                            */
/******************************************************************************/

extern ULONG          T_DosAPIGrp;               /* DOSCALLS API groups       */
extern ULONG          T_GpiAPIGrp;               /* PMGPI API groups          */
extern ULONG          T_OS2Ver;                  /* OS/2 version number       */
extern ULONG          T_OS2TRACEVer;             /* OS2TRACE version number   */
extern ULONG          T_PMErrorID[];             /* PM error identifiers      */
extern ULONG          T_TraceFlg;                /* Trace flags               */
extern ULONG          T_TraceLvl;                /* Trace information level   */
#ifdef T_USERHOOK
extern PSZ (* APIENTRY T_UserHook)( ULONG        /* User hook procedure       */
                                  , PSZ
                                  , PVOID
                                  , PVOID
                                  );
#endif
extern ULONG          T_WinAPIGrp;               /* PMWIN API groups          */

/******************************************************************************/
/*                                                                            */
/* Global function declarations                                               */
/*                                                                            */
/******************************************************************************/

ULONG T_ENTRY         T_DLLToTraceDLL( ULONG );
VOID  APIENTRY        T_Error( BOOL, ULONG, ... );
VOID  T_ENTRY         T_FreeLog( VOID );
ULONG T_ENTRY         T_GetTID( VOID );
BOOL  T_ENTRY         T_IsValidMem( PVOID, ULONG );
VOID  T_ENTRY         T_HookEntry( PCHAR, PVOID );
VOID  T_ENTRY         T_HookExit( PCHAR, PVOID, PVOID );
VOID  T_ENTRY         T_LogAPIEntry( PCHAR );
VOID  T_ENTRY         T_LogAPIEntryWithCaller( PCHAR, ULONG );
VOID  T_ENTRY         T_LogAPIExit( PCHAR );
VOID  T_ENTRY         T_LogAPIInterception( ULONG, ULONG, PCHAR );
VOID  T_ENTRY         T_LogCHARBuffer( PCHAR, ULONG );
VOID  T_ENTRY         T_LogCHARBufferUnformatted( PCHAR );
VOID  T_ENTRY         T_LogLineHeader( PCHAR );
VOID  T_ENTRY         T_LogLONGLONG( ULONG, ULONG );
VOID  APIENTRY        T_LogMessage( PCHAR, ... );
VOID  T_ENTRY         T_LogParameter( ULONG, ULONG );
VOID  T_ENTRY         T_LogPCHAR( PCHAR );
VOID  T_ENTRY         T_LogPFN( PFN );
VOID  T_ENTRY         T_LogPLONGLONG( PLONGLONG );
VOID  T_ENTRY         T_LogPPFN( PFN * );
VOID  T_ENTRY         T_LogPPSZ( PSZ * );
VOID  T_ENTRY         T_LogPSZ( PSZ );
VOID  T_ENTRY         T_LogPSZBuffer( PSZ *, ULONG );
VOID  T_ENTRY         T_LogPSZs( PSZ );
VOID  T_ENTRY         T_LogPULONG( PULONG );
VOID  T_ENTRY         T_LogPUSHORT( PUSHORT );
VOID  T_ENTRY         T_LogPVOID( PVOID );
#ifdef INTEL
VOID  T_ENTRY         T_LogStackDump( VOID );
#endif
VOID  T_ENTRY         T_LogTimes( PDATETIME, PDATETIME );
VOID  T_ENTRY         T_LogUCHAR( UCHAR );
VOID  T_ENTRY         T_LogULONG( ULONG );
VOID  T_ENTRY         T_LogULONGBuffer( PULONG, ULONG );
VOID  T_ENTRY         T_LogUSHORT( USHORT );
VOID  T_ENTRY         T_RegisterDLL( ULONG, ULONG );
VOID  T_ENTRY         T_WaitLog( VOID );

#if ( ( defined( INTEL ) ) && ( defined( T_16BIT ) ) )
BOOL  T_ENTRY         T_IsValidMem16( PVOID16, ULONG );
VOID  T_ENTRY         T_LogAPIEntry16( PCHAR );
VOID  T_ENTRY         T_LogAPIEntryWithCaller16( PCHAR, PVOID16 );
VOID  T_ENTRY         T_LogAPIInterception16( USHORT, PCHAR16 );
VOID  T_ENTRY         T_LogBaseAPIEntry16( PCHAR, ULONG );
VOID  T_ENTRY         T_LogCHARBuffer16( PCHAR16, ULONG );
VOID  T_ENTRY         T_LogCHARBufferUnformatted16( PCHAR16 );
VOID  T_ENTRY         T_LogPCHAR16( PCHAR16 );
VOID  T_ENTRY         T_LogPFN16( PFN16 );
VOID  T_ENTRY         T_LogPPFN16( PPFN16 );
VOID  T_ENTRY         T_LogPPSZ16( PPSZ16 );
VOID  T_ENTRY         T_LogPSZ16( PSZ16 );
VOID  T_ENTRY         T_LogPSZBuffer16( PPSZ16, USHORT );
VOID  T_ENTRY         T_LogPSZs16( PSZ16 );
VOID  T_ENTRY         T_LogPUSHORT16( PUSHORT16 );
VOID  T_ENTRY         T_LogPULONG16( PULONG16 );
VOID  T_ENTRY         T_LogPVOID16( PVOID16 );
VOID  T_ENTRY         T_LogStackDump16( VOID );
VOID  T_ENTRY         T_LogULONGBuffer16( PULONG16, USHORT );
VOID  T_ENTRY         T_LogUSHORTBuffer16( PUSHORT16, USHORT );
VOID                  T_RestoreRegs16( PREGS16 );
VOID                  T_SaveRegs16( PREGS16 );
#ifdef INCL_BASE
VOID  T_ENTRY         T_LogD16DT_Parm2( PVOID16, USHORT );
VOID  T_ENTRY         T_LogD16FF_Parm4( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16FIO_Parm2( PVOID16, USHORT );
VOID  T_ENTRY         T_LogD16FSA_Parm3( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16QFI_Parm3( PVOID16, USHORT );
VOID  T_ENTRY         T_LogD16QFSI_Parm3( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16QNPI_Parm3( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16QPI_Parm3( PVOID16, USHORT );
VOID  T_ENTRY         T_LogD16SFI_Parm3( PVOID16, USHORT );
VOID  T_ENTRY         T_LogDENA1Buffer( PDENA116, ULONG );
VOID  T_ENTRY         T_LogPAVAILDATA16( PAVAILDATA16 );
VOID  T_ENTRY         T_LogPCOUNTRYCODE16( PCOUNTRYCODE16 );
VOID  T_ENTRY         T_LogPCOUNTRYINFO16( PCOUNTRYINFO16, USHORT );
VOID  T_ENTRY         T_LogPDATETIME16( PDATETIME16 );
VOID  T_ENTRY         T_LogPDOSFSRSEM( PDOSFSRSEM16 );
VOID  T_ENTRY         T_LogPEAOP( PEAOP16, USHORT );
VOID  T_ENTRY         T_LogPFILELOCK16( PFILELOCK16 );
VOID  T_ENTRY         T_LogPFSQBUFFER( PFSQBUFFER16, USHORT );
VOID  T_ENTRY         T_LogPFSRSEM2( PFSRSEM216 );
VOID  T_ENTRY         T_LogPGINFOSEG( PGINFOSEG16 );
VOID  T_ENTRY         T_LogPKBDHWID( PKBDHWID16 );
VOID  T_ENTRY         T_LogPKBDINFO( PKBDINFO16 );
VOID  T_ENTRY         T_LogPKBDKEYINFO( PKBDKEYINFO16 );
VOID  T_ENTRY         T_LogPKBDTRANS( PKBDTRANS16 );
VOID  T_ENTRY         T_LogPLINFOSEG( PLINFOSEG16 );
VOID  T_ENTRY         T_LogPMOUEVENTINFO( PMOUEVENTINFO16 );
VOID  T_ENTRY         T_LogPMOUQUEINFO( PMOUQUEINFO16 );
VOID  T_ENTRY         T_LogPNOPTRRECT( PNOPTRRECT16 );
VOID  T_ENTRY         T_LogPPIDINFO( PPIDINFO16 );
VOID  T_ENTRY         T_LogPPtrace_B( PPtrace_B16 );
VOID  T_ENTRY         T_LogPPTRLOC( PPTRLOC16 );
VOID  T_ENTRY         T_LogPPTRSHAPE( PPTRSHAPE16 );
VOID  T_ENTRY         T_LogPQMRESULT( PQMRESULT16 );
VOID  T_ENTRY         T_LogPQWORD16( PQWORD16 );
VOID  T_ENTRY         T_LogPREGISTERDATA( PREGISTERDATA16 );
VOID  T_ENTRY         T_LogPRESULTCODES16( PRESULTCODES16 );
VOID  T_ENTRY         T_LogPSCALEFACT( PSCALEFACT16 );
VOID  T_ENTRY         T_LogPSTARTDATA16( PSTARTDATA16 );
VOID  T_ENTRY         T_LogPSTATUSDATA16( PSTATUSDATA16 );
VOID  T_ENTRY         T_LogPSTRINGINBUF( PSTRINGINBUF16 );
VOID  T_ENTRY         T_LogPTHRESHOLD( PTHRESHOLD16 );
VOID  T_ENTRY         T_LogPVIOCONFIGINFO( PVIOCONFIGINFO16 );
VOID  T_ENTRY         T_LogPVIOCURSORINFO( PVIOCURSORINFO16 );
VOID  T_ENTRY         T_LogPVIOFONTINFO( PVIOFONTINFO16 );
VOID  T_ENTRY         T_LogPVIOMODEINFO( PVIOMODEINFO16 );
VOID  T_ENTRY         T_LogPVIOPHYSBUF( PVIOPHYSBUF16 );
VOID  T_ENTRY         T_LogV16GS_Parm1( PVOID16 );
#endif
#ifdef INCL_PM
VOID  T_ENTRY         T_LogD16PDeE_Parm3( PVOID16, USHORT, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PDrE_Parm3( PVOID16, USHORT, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PDGI_Parm4( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PDSI_Parm4( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PJE_Parm4( PVOID16, USHORT, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PJGI_Parm4( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PJSI_Parm4( PVOID16, USHORT, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PPE_Parm3( PVOID16, USHORT, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PQE_Parm3( PVOID16, USHORT, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PQGI_Parm4( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PQPE_Parm3( PVOID16, USHORT, USHORT, USHORT );
VOID  T_ENTRY         T_LogD16PQSI_Parm4( PVOID16, USHORT, USHORT, USHORT );
VOID  T_ENTRY         T_LogDRAGIMAGEBuffer16( PDRAGIMAGE16, USHORT );
VOID  T_ENTRY         T_LogFFDESCSBuffer16( PFFDESCS16, ULONG );
VOID  T_ENTRY         T_LogFIXEDBuffer16( PFIXED16, ULONG );
VOID  T_ENTRY         T_LogFONTMETRICSBuffer16( PFONTMETRICS16, LONG, ULONG );
VOID  T_ENTRY         T_LogG16CB_Parm4( PBYTE16, PBITMAPINFO216 );
VOID  T_ENTRY         T_LogG16DB_Parm2( PVOID16, PBITMAPINFO216, PPOINTL16 );
VOID  T_ENTRY         T_LogG16QBB_Parm4( PBYTE16, PBITMAPINFO216, LONG );
VOID  T_ENTRY         T_LogG16SBB_Parm4( PBYTE16, PBITMAPINFO216 );
VOID  T_ENTRY         T_LogKERNINGPAIRSBuffer16( PKERNINGPAIRS16, ULONG );
VOID  T_ENTRY         T_LogLPMT16( LPMT16 );
VOID  T_ENTRY         T_LogPACCELTABLE16( PACCELTABLE16 );
VOID  T_ENTRY         T_LogPARCPARAMS16( PARCPARAMS16 );
VOID  T_ENTRY         T_LogPBITMAPINFO216( PBITMAPINFO216 );
VOID  T_ENTRY         T_LogPBITMAPINFOHEADER216( PBITMAPINFOHEADER216 );
VOID  T_ENTRY         T_LogPBUNDLE16( PBUNDLE16, LONG );
VOID  T_ENTRY         T_LogPCATCHBUF( PCATCHBUF16 );
VOID  T_ENTRY         T_LogPCLASSINFO16( PCLASSINFO16 );
VOID  T_ENTRY         T_LogPCURSORINFO16( PCURSORINFO16 );
VOID  T_ENTRY         T_LogPDDESTRUCT16( PDDESTRUCT16 );
VOID  T_ENTRY         T_LogPDESKTOP16( PDESKTOP16 );
VOID  T_ENTRY         T_LogPDEVOPENSTRUC16( PDEVOPENSTRUC16, ULONG );
VOID  T_ENTRY         T_LogPDLGTEMPLATE16( PDLGTEMPLATE16 );
VOID  T_ENTRY         T_LogPDRAGINFO16( PDRAGINFO16 );
VOID  T_ENTRY         T_LogPDRAGITEM16( PDRAGITEM16 );
VOID  T_ENTRY         T_LogPDRAGTRANSFER16( PDRAGTRANSFER16 );
VOID  T_ENTRY         T_LogPDRIVDATA16( PDRIVDATA16 );
VOID  T_ENTRY         T_LogPERRINFO16( PERRINFO16 );
VOID  T_ENTRY         T_LogPFACENAMEDESC16( PFACENAMEDESC16 );
VOID  T_ENTRY         T_LogPFATTRS16( PFATTRS16 );
VOID  T_ENTRY         T_LogPFIXED16( PFIXED16 );
VOID  T_ENTRY         T_LogPFONTMETRICS16( PFONTMETRICS16, LONG );
VOID  T_ENTRY         T_LogPFRAMECDATA16( PFRAMECDATA16 );
VOID  T_ENTRY         T_LogPGRADIENTL16( PGRADIENTL16 );
VOID  T_ENTRY         T_LogPHCINFO16( PHCINFO16, LONG );
VOID  T_ENTRY         T_LogPHELPINIT16( PHELPINIT16 );
VOID  T_ENTRY         T_LogPHELPTABLE16( PHELPTABLE16 );
VOID  T_ENTRY         T_LogPHPROGARRAY( PHPROGARRAY16, ULONG );
VOID  T_ENTRY         T_LogPMATRIXLF16( PMATRIXLF16 );
VOID  T_ENTRY         T_LogPMMessage16( USHORT );
VOID  T_ENTRY         T_LogPMQINFO16( PMQINFO16 );
VOID  T_ENTRY         T_LogPMReturnCodeUCHAR( ULONG, UCHAR, BOOL );
VOID  T_ENTRY         T_LogPMReturnCodePtr16( ULONG, PVOID16, BOOL );
VOID  T_ENTRY         T_LogPOINTLBuffer16( PPOINTL16, USHORT );
VOID  T_ENTRY         T_LogPPIBSTRUCT( PPIBSTRUCT16, USHORT );
VOID  T_ENTRY         T_LogPPOINTERINFO16( PPOINTERINFO16 );
VOID  T_ENTRY         T_LogPPOINTL16( PPOINTL16 );
VOID  T_ENTRY         T_LogPPRDINFO16( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogPPRFPROFILE16( PPRFPROFILE16 );
VOID  T_ENTRY         T_LogPPRIDINFO( PPRIDINFO16, USHORT );
VOID  T_ENTRY         T_LogPPROGDETAILS16( PPROGDETAILS16 );
VOID  T_ENTRY         T_LogPPRQINFO16( PVOID16, USHORT, USHORT );
VOID  T_ENTRY         T_LogPQMSG16( PQMSG16 );
VOID  T_ENTRY         T_LogPRECTL16( PRECTL16 );
VOID  T_ENTRY         T_LogPRGNRECT16( PRGNRECT16 );
VOID  T_ENTRY         T_LogPROGRAMENTRYBuffer( PPROGRAMENTRY16, USHORT );
VOID  T_ENTRY         T_LogPROGTITLEBuffer16( PPROGTITLE16, ULONG );
VOID  T_ENTRY         T_LogPSIZEF16( PSIZEF16 );
VOID  T_ENTRY         T_LogPSIZEL16( PSIZEL16 );
VOID  T_ENTRY         T_LogPSWBLOCK16( PSWBLOCK16 );
VOID  T_ENTRY         T_LogPSWCNTRL16( PSWCNTRL16 );
VOID  T_ENTRY         T_LogPSWP16( PSWP16 );
VOID  T_ENTRY         T_LogPTRACKINFO16( PTRACKINFO16 );
VOID  T_ENTRY         T_LogRECTLBuffer16( PRECTL16, ULONG );
VOID  T_ENTRY         T_LogSWPBuffer16( PSWP16, USHORT );
VOID  T_ENTRY         T_LogWPOINTBuffer( PWPOINT16, USHORT );
#endif
#endif

#ifdef INCL_BASE
VOID  T_ENTRY         T_LogCPReturnCode( ULONG );
VOID  T_ENTRY         T_LogD32DT_Parm2( PVOID, ULONG, BOOL );
VOID  T_ENTRY         T_LogD32DT_Parm3( PVOID, ULONG, PVOID );
VOID  T_ENTRY         T_LogD32FF_Parm4( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogD32FSA_Parm3( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogD32QFI_Parm3( PVOID, ULONG );
VOID  T_ENTRY         T_LogD32QFSI_Parm3( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogD32QMCP_Parm1( PSHORT, ULONG );
VOID  T_ENTRY         T_LogD32QNPI_Parm3( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogD32QPI_Parm3( PVOID, ULONG );
VOID  T_ENTRY         T_LogD32SFI_Parm3( PVOID, ULONG );
VOID  T_ENTRY         T_LogDENA2Buffer( PDENA2, ULONG );
VOID  T_ENTRY         T_LogPAVAILDATA( PAVAILDATA );
VOID  T_ENTRY         T_LogPCONTEXTRECORD( PCONTEXTRECORD, ULONG );
VOID  T_ENTRY         T_LogPCOUNTRYCODE( PCOUNTRYCODE );
VOID  T_ENTRY         T_LogPCOUNTRYINFO( PCOUNTRYINFO, ULONG );
VOID  T_ENTRY         T_LogPDATETIME( PDATETIME );
VOID  T_ENTRY         T_LogPEAOP2( PEAOP2, ULONG );
VOID  T_ENTRY         T_LogPEXCEPTIONREGISTRATIONRECORD( PEXCEPTIONREGISTRATIONRECORD );
VOID  T_ENTRY         T_LogPEXCEPTIONREPORTRECORD( PEXCEPTIONREPORTRECORD );
VOID  T_ENTRY         T_LogPFILELOCK( PFILELOCK );
VOID  T_ENTRY         T_LogPFILELOCKL( PFILELOCKL );
VOID  T_ENTRY         T_LogPFSQBUFFER2( PFSQBUFFER2, ULONG );
VOID  T_ENTRY         T_LogPGRESEMT( PGRESEMT );
VOID  T_ENTRY         T_LogPLISTIO( PLISTIO, ULONG, ULONG, BOOL );
VOID  T_ENTRY         T_LogPLISTIOL( PLISTIOL, ULONG, ULONG, BOOL );
VOID  T_ENTRY         T_LogPMPAFFINITY( PMPAFFINITY );
VOID  T_ENTRY         T_LogPMUXSEMLIST( PMUXSEMLIST16 );
VOID  T_ENTRY         T_LogPPDR( PPDR, BOOL );
VOID  T_ENTRY         T_LogPPIB( PPIB );
VOID  T_ENTRY         T_LogPPIPESEMSTATE( PPIPESEMSTATE, ULONG );
VOID  T_ENTRY         T_LogPPIR( PPIR );
VOID  T_ENTRY         T_LogPQWORD( PQWORD );
VOID  T_ENTRY         T_LogPREQUESTDATA( PREQUESTDATA );
VOID  T_ENTRY         T_LogPRESULTCODES( PRESULTCODES );
VOID  T_ENTRY         T_LogPSEMRECORD( PSEMRECORD );
VOID  T_ENTRY         T_LogPSTARTDATA( PSTARTDATA );
VOID  T_ENTRY         T_LogPSTATUSDATA( PSTATUSDATA );
VOID  T_ENTRY         T_LogPTHREADCREATE( PTHREADCREATE );
VOID  T_ENTRY         T_LogPTIB( PTIB );
VOID  T_ENTRY         T_LogpuDB_t( uDB_t * );
VOID  T_ENTRY         T_LogSEMRECORDBuffer( PSEMRECORD, ULONG );
#ifdef POWERPC
VOID  T_ENTRY         T_LogPRAMSEM( PRAMSEM );
VOID  T_ENTRY         T_LogQPROCINFOBuffer( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogQTHREADINFOBuffer( PVOID, ULONG, ULONG, ULONG );
#endif
#endif

#ifdef INCL_PM
VOID  T_ENTRY         T_LogCTLCOLORBuffer( PCTLCOLOR, ULONG );
VOID  T_ENTRY         T_LogDRAGIMAGEBuffer( PDRAGIMAGE, ULONG );
VOID  T_ENTRY         T_LogFFDESCSBuffer( PFFDESCS, ULONG );
VOID  T_ENTRY         T_LogFFDESCS2Buffer( PVOID, ULONG );
VOID  T_ENTRY         T_LogFIXED( FIXED );
VOID  T_ENTRY         T_LogFIXEDBuffer( PFIXED, ULONG );
VOID  T_ENTRY         T_LogFONTMETRICSBuffer( PFONTMETRICS, LONG, ULONG );
VOID  T_ENTRY         T_LogG32CB_Parm4( PBYTE, PBITMAPINFO2 );
VOID  T_ENTRY         T_LogG32DB_Parm2( PVOID, PBITMAPINFO2, PPOINTL );
VOID  T_ENTRY         T_LogG32QBB_Parm4( PBYTE, PBITMAPINFO2, LONG );
VOID  T_ENTRY         T_LogG32SBB_Parm4( PBYTE, PBITMAPINFO2 );
VOID  T_ENTRY         T_LogKERNINGPAIRSBuffer( PKERNINGPAIRS, ULONG );
VOID  T_ENTRY         T_LogLPMT( LPMT );
VOID  T_ENTRY         T_LogPACCELTABLE( PACCELTABLE );
VOID  T_ENTRY         T_LogPAPSZ( PSZ * );
VOID  T_ENTRY         T_LogPARCPARAMS( PARCPARAMS );
VOID  T_ENTRY         T_LogPBITMAPINFO2( PBITMAPINFO2 );
VOID  T_ENTRY         T_LogPBITMAPINFOHEADER2( PBITMAPINFOHEADER2 );
VOID  T_ENTRY         T_LogPBUNDLE( PBUNDLE, LONG );
VOID  T_ENTRY         T_LogPCLASSINFO( PCLASSINFO );
VOID  T_ENTRY         T_LogPCONVCONTEXT( PCONVCONTEXT );
VOID  T_ENTRY         T_LogPCURSORINFO( PCURSORINFO );
VOID  T_ENTRY         T_LogPDDESTRUCT( PDDESTRUCT );
VOID  T_ENTRY         T_LogPDESKTOP( PDESKTOP );
VOID  T_ENTRY         T_LogPDEVOPENSTRUC( PDEVOPENSTRUC, ULONG );
VOID  T_ENTRY         T_LogPDLGTEMPLATE( PDLGTEMPLATE );
VOID  T_ENTRY         T_LogPDRAGINFO( PDRAGINFO );
VOID  T_ENTRY         T_LogPDRAGITEM( PDRAGITEM );
VOID  T_ENTRY         T_LogPDRAGTRANSFER( PDRAGTRANSFER );
VOID  T_ENTRY         T_LogPDRIVDATA( PDRIVDATA );
VOID  T_ENTRY         T_LogPERRINFO( PERRINFO );
VOID  T_ENTRY         T_LogPFACENAMEDESC( PFACENAMEDESC );
VOID  T_ENTRY         T_LogPFATTRS( PFATTRS );
VOID  T_ENTRY         T_LogPFILEDLG( PFILEDLG );
VOID  T_ENTRY         T_LogPFIXED( PFIXED );
VOID  T_ENTRY         T_LogPFONTDLG( PFONTDLG );
VOID  T_ENTRY         T_LogPFONTMETRICS( PFONTMETRICS, LONG );
VOID  T_ENTRY         T_LogPFRAMECDATA( PFRAMECDATA );
VOID  T_ENTRY         T_LogPGRADIENTL( PGRADIENTL );
VOID  T_ENTRY         T_LogPHCINFO( PHCINFO, LONG );
VOID  T_ENTRY         T_LogPHELPINIT( PHELPINIT );
VOID  T_ENTRY         T_LogPHELPTABLE( PHELPTABLE );
VOID  T_ENTRY         T_LogPICONINFO( PICONINFO );
VOID  T_ENTRY         T_LogPMATRIXLF( PMATRIXLF );
VOID  T_ENTRY         T_LogPMB2INFO( PMB2INFO );
VOID  T_ENTRY         T_LogPMMessage( ULONG );
VOID  T_ENTRY         T_LogPMQINFO( PMQINFO );
VOID  T_ENTRY         T_LogPMReturnCode( ULONG, ULONG, BOOL );
VOID  T_ENTRY         T_LogPMReturnCodeIndeterminate( ULONG, ULONG );
VOID  T_ENTRY         T_LogPMReturnCodeUSHORT( ULONG, USHORT, BOOL );
VOID  T_ENTRY         T_LogPOBJCLASS( POBJCLASS );
VOID  T_ENTRY         T_LogPOLYGONBuffer( PPOLYGON, ULONG );
VOID  T_ENTRY         T_LogPOINTLBuffer( PPOINTL, ULONG );
#ifdef INCL_SPLBIDI
VOID  T_ENTRY         T_LogPPDALERTINFO( PPDALERTINFO );
#endif
VOID  T_ENTRY         T_LogPPOINTERINFO( PPOINTERINFO );
VOID  T_ENTRY         T_LogPPOINTL( PPOINTL );
VOID  T_ENTRY         T_LogPPRDINFO( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogPPRFPROFILE( PPRFPROFILE );
VOID  T_ENTRY         T_LogPPROGDETAILS( PPROGDETAILS );
VOID  T_ENTRY         T_LogPPRQINFO( PVOID, ULONG, ULONG );
#ifdef INCL_SPLBIDI
VOID  T_ENTRY         T_LogPPRTALERT( PPRTALERT, ULONG );
#endif
VOID  T_ENTRY         T_LogPQMSG( PQMSG );
VOID  T_ENTRY         T_LogPRASTERIZERCAPS( PRASTERIZERCAPS );
VOID  T_ENTRY         T_LogPRECTL( PRECTL );
VOID  T_ENTRY         T_LogPRGNRECT( PRGNRECT );
VOID  T_ENTRY         T_LogPROGTITLEBuffer( PPROGTITLE, ULONG );
VOID  T_ENTRY         T_LogPSIZEF( PSIZEF );
VOID  T_ENTRY         T_LogPSIZEL( PSIZEL );
VOID  T_ENTRY         T_LogPSWBLOCK( PSWBLOCK );
VOID  T_ENTRY         T_LogPSWCNTRL( PSWCNTRL );
VOID  T_ENTRY         T_LogPSWP( PSWP );
VOID  T_ENTRY         T_LogPTRACKINFO( PTRACKINFO );
VOID  T_ENTRY         T_LogRECTLBuffer( PRECTL, ULONG );
VOID  T_ENTRY         T_LogS32EDe_Parm3( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32EDr_Parm3( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32EJ_Parm4( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32EPo_Parm3( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32EPr_Parm4( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32EQ_Parm3( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32EQP_Parm3( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32GPFQ_Parm4( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32QDe_Parm4( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32QDr_Parm5( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32QGJI_Parm3( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32QJ_Parm5( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32QPa_Parm4( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32QPo_Parm4( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32QQ_Parm4( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32SDe_Parm4( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32SDr_Parm5( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32SJ_Parm5( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32SP_Parm4( PVOID, ULONG, ULONG );
VOID  T_ENTRY         T_LogS32SQ_Parm4( PVOID, ULONG, ULONG, ULONG );
VOID  T_ENTRY         T_LogSWPBuffer( PSWP, ULONG );
#endif

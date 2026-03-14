.*******************************************************************************
.*                                                                             *
.* File name    : PMOS2TRC.IPF                                                 *
.*                                                                             *
.* Title        : OS/2 API trace PM help                                       *
.*                                                                             *
.* Author       : Dave Blaschke                                                *
.*                deblaschke@yahoo.com                                         *
.*                                                                             *
.* History      : Version Date    Item                                         *
.*                2.30.24 29Mar96 Added PM interface  (supported customization *
.*                                and help only)                               *
.*                2.30.30 26Jun94 Added OS/2 2.00 PM debugger APIs             *
.*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                     *
.*                2.40.05 12Mar97 Added enablement support to PM interface     *
.*                2.40.08 13Mar97 Added summarization support to PM interface  *
.*                2.40.10 14Mar97 Fixed Dev32StdOpen bug                       *
.*                2.40.11 19Mar97 Added  save  window  position  option  to PM *
.*                                interface                                    *
.*                2.40.12 19Mar97 Added support for NE format                  *
.*                2.40.13 20Mar97 Added  launch   .EXE   files  option  to  PM *
.*                                interface                                    *
.*                2.40.16 15Apr97 Removed OS/2 for PowerPC from PM help        *
.*                2.40.17 21Apr97 Added 16-bit Dos APIs                        *
.*                2.40.18 30Apr97 Added 16-bit Kbd APIs                        *
.*                2.40.19 01May97 Added 16-bit Mou APIs                        *
.*                2.40.20 05May97 Added 16-bit Vio APIs                        *
.*                2.40.22 22May97 Added FAQ                                    *
.*                2.40.24 03Jun97 Added FAQ to PM interface help submenu       *
.*                2.40.26 24Jun97 Added 16-bit Win APIs                        *
.*                2.40.27 22Jul97 Added 16-bit Gpi APIs                        *
.*                2.40.28 06Aug97 Added 16-bit Dev APIs                        *
.*                2.40.29 07Aug97 Added 16-bit Drg APIs                        *
.*                2.40.30 08Aug97 Added 16-bit Pic APIs                        *
.*                2.40.31 08Aug97 Logged user-defined buffer initial bytes     *
.*                2.40.32 11Aug97 Added 16-bit Prf APIs                        *
.*                2.40.33 11Aug97 Added 16-bit Prt APIs                        *
.*                2.40.34 13Aug97 Added 16-bit DosPrint and Spl APIs           *
.*                2.40.35 13Aug97 Fixed Spl32EnumDevice/Spl32EnumQueue bug     *
.*                2.40.36 14Aug97 Added -I option                              *
.*                2.40.37 15Aug97 Fixed Vio16GetPhysBuf bug                    *
.*                2.40.38 15Aug97 Added TRACE-IT batch file                    *
.*                2.40.39 17Sep97 Fixed 16-bit DosProtect APIs bug             *
.*                2.40.40 12Jan98 Enhanced performance                         *
.*                2.40.41 12Jan98 Fixed Dos16GetInfoSeg bug                    *
.*                2.40.42 23Jan98 Added PRQINFO8 structure                     *
.*                2.40.43 23Jan98 Fixed Dos16PrintQSetInfo/Spl32SetQueue bug   *
.*                2.40.44 27Jan98 Fixed returned MRESULT thunk bug             *
.*                2.40.45 28Jan98 Added indeterminate API invocations          *
.*                2.40.46 28Jan98 Fixed summarization alphabetical API bug     *
.*                2.40.47 29Jan98 Added Win16DefAVioWindowProc                 *
.*                2.40.48 30Jan98 Fixed Win16Catch bug                         *
.*                2.40.49 04Feb98 Added OS2TRACE.API text file                 *
.*                2.40.50 06Feb98 Fixed CX  and DX  corruption by  16-bit Kbd, *
.*                                Mou, and Vio APIs                            *
.*                2.40.51 18Feb98 Verified  saved  window  position  origin on *
.*                                desktop                                      *
.*                2.40.52 16Mar98 Fixed file dialog root directory bug         *
.*                2.40.53 16Mar98 Fixed file dialog clean up bug               *
.*                2.40.54 16Mar98 Changed main window identifier to 1          *
.*                2.40.55 02Jun98 Fixed DevQueryHardcopyCaps bug               *
.*                2.40.56 02Jun98 Changed default  selection  for cancellation *
.*                                dialog to "No"                               *
.*                2.40.57 25Jun98 Added trace-enabled executable checker       *
.*                2.40.58 01Jul98 Added version checking to trace DLLs         *
.*                2.40.59 06Jul98 Added bitmaps of dialogs to PM help          *
.*                2.40.60 08Jul98 Added  title/action  bar only  option to  PM *
.*                                interface                                    *
.*                2.40.61 17Aug98 Added SMP Dos APIs                           *
.*                2.40.62 18Aug98 Removed  unnecesary   semaphore  nesting  in *
.*                                DosFindFirst/DosFindClose                    *
.*                2.40.63 19Aug98 Fixed -F option                              *
.*                2.40.64 21Aug98 Added bitmap of logo to PM help              *
.*                2.40.65 28Aug98 Added monitor trace options to PM interface  *
.*                2.40.66 09Sep98 Added private profile Dos APIs               *
.*                2.40.67 24Sep98 Removed unloading trace DLLs from .DLLs      *
.*                2.40.68 12Oct98 Fixed/enhanced helping information           *
.*                2.40.69 21Oct98 Forwarded removed entry table ordinals       *
.*                2.40.70 21Oct98 Added miscellaneous Dos APIs                 *
.*                2.40.71 22Oct98 Added PM AVIO Vio APIs                       *
.*                2.40.72 22Oct98 Moved Prt/Spl APIs loading to OS/2 2.30      *
.*                2.40.73 26Oct98 Fixed    Dos32GetMessage/Dos32QueryMessageCP *
.*                                bug                                          *
.*                2.40.74 27Oct98 Added WinSetErrorInfo                        *
.*                2.40.75 28Oct98 Added Gpi/Win APIs exported from PMBIDI      *
.*                2.40.76 28Oct98 Issued more specific trace DLL load errors   *
.*                2.40.77 28Oct98 Added Win32MessageBox2                       *
.*                2.40.78 16Nov98 Added miscellaneous Dos APIs                 *
.*                2.40.79 24Nov98 Fixed Dos32QueryModFromEIP usage bug         *
.*                2.40.80 02Dec98 Added undocumented -DUMPSTACK option         *
.*                2.40.81 02Dec98 Added -E option                              *
.*                2.40.82 07Dec98 Fixed stack dump at DosExit bug              *
.*                2.40.83 12Jan99 Enhanced trace monitoring                    *
.*                2.40.84 20Jan99 Fixed monitor trace options dialogs bugs     *
.*                2.40.85 21Jan99 Added -P option                              *
.*                2.40.86 03Mar99 Added support  for executables  without  DOS *
.*                                header                                       *
.*                2.40.87 23Mar99 Fixed 16-bit SMP Dos APIs loading bug        *
.*                2.45.00 15Apr99 Added new OS/2 2.45 APIs                     *
.*                2.45.01 02May99 Added  support  for  Gpi/Win APIs  in PMGPI, *
.*                                PMSHAPI, and PMWIN exported by PMMERGE       *
.*                2.45.02 02May99 Fixed WinQueryWindowText bug                 *
.*                2.45.03 02May99 Added   support  for   Drg  APIs  in  PMDRAG *
.*                                exported by PMCTLS                           *
.*                2.45.04 05May99 Added -Q option                              *
.*                2.45.05 09May99 Added  support for  K and M  suffixes to  -B *
.*                                and -F options                               *
.*                2.45.06 16May99 Added  support for  KB and MB  values to  PM *
.*                                interface                                    *
.*                2.45.07 21May99 Fixed Dos32Debug bug                         *
.*                2.45.08 11Jun99 Added support for .COM files                 *
.*                2.45.09 11Jun99 Logged Dos16DevIOCtl(2) output buffers       *
.*                2.45.10 02Aug99 Moved common strings into T_COMMON           *
.*                2.45.11 03Aug99 Logged name  of module  containing API being *
.*                                intercepted                                  *
.*                2.45.12 01Feb00 Displayed customization options on new line  *
.*                2.45.13 01Feb00 Logged PID in errors and warnings            *
.*                2.45.14 02Feb00 Saved OS2.INI information only when changed  *
.*                2.45.15 08Feb00 Standardized output                          *
.*                2.45.16 18Feb00 Added -C, -PAUSE, and -RESUME options        *
.*                2.45.17 21Feb00 Added restore window  position option  to PM *
.*                                interface                                    *
.*                2.45.18 28Feb00 Removed import by name restriction           *
.*                2.45.19 29Feb00 Fixed  trace monitoring  without  background *
.*                                font                                         *
.*                2.45.20 29Feb00 Fixed control semaphore IPE under OS/2 2.00  *
.*                2.45.21 29Feb00 Fixed resizing main  window with zero height *
.*                                twice during trace monitoring                *
.*                2.45.22 01Mar00 Removed  drawing  background  font if bitmap *
.*                                calculation too long                         *
.*                2.45.23 24May00 Handled running out of disk space            *
.*                2.45.24 01Jun00 Added -A option                              *
.*                2.45.25 03Jun00 Added trace information file API stripper    *
.*                2.45.26 10Jan01 Handled OS/2 2.45 APIs removed by WSeB CP    *
.*                2.45.27 05Jun01 Fixed Dos32CreateThread bug                  *
.*                2.45.28 14Jun01 Fixed Dos16(Protect)QFileInfo bug            *
.*                2.45.29 19Jun01 Added enter key support to PM interface      *
.*                2.45.30 22Jun01 Fixed Dos32UnwindException bug               *
.*                2.45.31 26Jun01 Added  capability  of building  32-bit  only *
.*                                trace DLLs                                   *
.*                2.45.32 28Jun01 Added -U option                              *
.*                2.45.33 29Jun01 Added Win APIs exported  from PMSHAPI not in *
.*                                public include file                          *
.*                2.45.34 10Jul01 Moved C run-time functions into T_COMMON     *
.*                2.45.35 02Dec03 Fixed Dos16CreateThread bug                  *
.*                2.45.36 03Dec03 Fixed WinSetErrorInfo user hook bug          *
.*                2.45.37 30Apr09 Fixed error-to-string bugs                   *
.*                2.45.38 04May09 Added Dos32DumpProcess errors                *
.*                2.45.39 10Oct10 Preserved FS in T_GetTID                     *
.*                2.45.40 25Oct10 Fixed PEAOP/PEAOP2 bug                       *
.*                                                                             *
.*******************************************************************************
:userdoc.
.*POWERPC .*
.*POWERPC .* HLP_INTELONLY
.*POWERPC .*
.*POWERPC :h1 res=11.Help for Intel Only
.*POWERPC :p.This API is only available on Intel versions of OS/2 (OS/2 2.xx).
.*POWERPC .*
.*POWERPC .* HLP_POWERPCONLY
.*POWERPC .*
.*POWERPC :h1 res=12.Help for PowerPC Only
.*POWERPC :p.This API is only available on PowerPC versions of OS/2 (OS/2 3.00).
.*
.* HLP_SMPONLY
.*
:h1 res=13.Help for SMP Only
:p.This API is only available on SMP (:hp5.S:ehp5.ymmetric
:hp5.M:ehp5.ulti:hp5.p:ehp5.rocessing) versions of OS/2.
.*
.* HLP_245TOCPONLY
.*
:h1 res=14.Help for OS/2 2.45 to WSeB CP Only
:p.This API is only available on OS/2 Version 2.45 (Warp Server for e-business) :hp2.up to but NOT including:ehp2. the Convenience Package for Warp Server for e-business.
.*
.* HLP_OS2VER210
.*
:h1 res=21.Help for OS/2 2.10+
:p.This API is only available on OS/2 Version 2.10 or later.
.*
.* HLP_OS2VER230
.*
:h1 res=23.Help for OS/2 2.30+
:p.This API is only available on OS/2 Version 2.30 (Warp) or later.
.*
.* HLP_OS2VER240
.*
:h1 res=24.Help for OS/2 2.40+
:p.This API is only available on OS/2 Version 2.40 (Warp Version 4) or later.
.*
.* HLP_OS2VER245
.*
:h1 res=245.Help for OS/2 2.45+
:p.This API is only available on OS/2 Version 2.45 (Warp Server for e-business) or later.
.*POWERPC .*
.*POWERPC .* HLP_OS2VER300
.*POWERPC .*
.*POWERPC :h1 res=30.Help for OS/2 3.00
.*POWERPC :p.This API is only available on OS/2 Version 3.00 (OS/2 for PowerPC).
.*
.* PNL_ID_MAINWIN
.*
:h1 res=103.Help for OS/2 API Trace
:i1 id=trace.OS/2 API Trace
:artwork name='bitmaps\os2trace.bmp' align=center.
:p.Use OS/2 API Trace to enable and/or disable the tracing of 16-bit and 32-bit
OS/2 APIs imported by an executable file without affecting its source code.
Also use OS/2 API Trace to customize, control, and summarize the tracing of
16-bit and 32-bit OS/2 APIs.
:p.For a detailed explanation of each menu item, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=201.
:dd.Enable
:elink.
:dt.o
:link reftype=hd res=301.
:dd.Customize
:elink.
:dt.o
:link reftype=hd res=401.
:dd.Control
:elink.
:dt.o
:link reftype=hd res=501.
:dd.Summarize
:elink.
:dt.o
:link reftype=hd res=601.
:dd.Options
:elink.
:edl.
:p.Answers to :link reftype=hd res=711.Frequently Asked Questions:elink. are
also available.
.*
.* HLP_ENABMENU
.*
:h1 res=201.Help for Enable
:i1 id=enab.Enable
:p.Select :hp2.Enable:ehp2. to enable and/or disable the tracing of OS/2 APIs
imported by an executable file.
:p.This is accomplished by processing each entry in the table of strings within
the executable file that contains the names of the imported DLLs.
If the entry represents one of the supported OS/2 DLLs and the user requests
enablement of API tracing for this DLL, the entry is replaced with the name of
the appropriate trace DLL.
When the executable file is invoked, the trace DLL intercepts API calls to its
corresponding OS/2 DLL, logs API input information, invokes the API, and logs
API output information.
If the entry represents one of the trace DLLs and the user requests disablement
of API tracing for this DLL, the entry is replaced with the name of the
supported OS/2 DLL.
When finished, the updated table of strings is written to the executable file.
:note.All private APIs are simply forwarded to the corresponding OS/2 DLL with
no intervention from the trace DLL.
:p.All levels of information, from the trace-enabled .EXE/.COM and/or any
trace-enabled .DLLs that are attached to the .EXE/.COM, are logged to a text
file with a file name that matches the .EXE/.COM file name and an extension of
&period.TRC.
This trace information file resides in the same directory as the .EXE/.COM
file or the alternative directory, if one exists.
If another instance of the same .EXE/.COM is already running or the text file
is already open in the alternative directory, the file name of the text file is
changed to PROC followed by the hexadecimal process identifier (i.e.
PROC003A.TRC).
If this occurs, a warning message is issued.
:note.If a REXX program loads any trace-enabled .DLLs, all trace information is
logged to CMD.TRC in the same directory as the CMD.EXE executable that loaded
the REXX program or the alternative directory, if one exists.
:p.For a detailed explanation of each menu item, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=203.
:dd.Open File
:elink.
:dt.o
:link reftype=hd res=205.
:dd.Preserve File Date/Time Stamp
:elink.
:edl.
.*
.* HLP_ENABOPEN
.*
:h1 res=203.Help for Open File
:i1 id=enabopen.Open File for Enablement
:p.Use :hp2.Open file:ehp2. to open an executable file in order to enable
and/or disable the tracing of OS/2 APIs imported by the file.
:note.Trace enablement alters the contents of the executable file's import
module name table.
Although this action does not affect the functionality of the executable, it
does affect its date and time stamp unless the preserve file date/time stamp
menu intem is enabled (checked).
For more details on preserving the file's date and time stamp, see
:link reftype=hd res=205.Preserve File Date/Time Stamp:elink..
.*
.* HLP_ENABPDTS
.*
:h1 res=205.Help for Preserve File Date/Time Stamp
:i1 id=enabpdts.Preserve File Date/Time Stamp
:p.Use :hp2.Preserve file date/time stamp:ehp2. to enable or disable the
preserving of an executable file's date and time stamp during trace enablement.
If this menu item is enabled (checked), an executable file's date and time
stamp is preserved if the executable file is modified during trace enablement.
If this menu item is disabled (unchecked), an executable file's date and time
stamp is not preserved when the executable file is modified during trace
enablement.
:p.This menu item provides the same functionality as OS2TRACE's -P option, and
is only effective when opening a file for enablement.
.*
.* HLP_CUSTMENU
.*
:h1 res=301.Help for Customize
:i1 id=cust.Customize
:p.Select :hp2.Customize:ehp2. to customize the tracing of OS/2 APIs.
:p.This is accomplished by storing the state of the trace customization options
in the operating system's user profile, OS2.INI.
The state of these options can then be retrieved by the trace DLLs when a
trace-enabled executable commences running.
:p.For a detailed explanation of each menu item, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=303.
:dd.Alternative Directory
:elink.
:dt.o
:link reftype=hd res=305.
:dd.Buffer Length
:elink.
:dt.o
:link reftype=hd res=307.
:dd.Tracing Control
:elink.
:dt.o
:link reftype=hd res=309.
:dd.DOSCALLS APIs
:elink.
:dt.o
:link reftype=hd res=311.
:dd.EBCDIC Translation
:elink.
:dt.o
:link reftype=hd res=313.
:dd.File Wrapping
:elink.
:dt.o
:link reftype=hd res=315.
:dd.PMGPI APIs
:elink.
:dt.o
:link reftype=hd res=317.
:dd.API Interception
:elink.
:dt.o
:link reftype=hd res=319.
:dd.Logging Level
:elink.
:dt.o
:link reftype=hd res=321.
:dd.Time Stamping
:elink.
:dt.o
:link reftype=hd res=323.
:dd.User Hook
:elink.
:dt.o
:link reftype=hd res=325.
:dd.PMWIN APIs
:elink.
:edl.
.*
.* HLP_CUSTALT
.*
:h1 res=303.Help for Alternative Directory
:i1 id=custalt.Alternative Directory
:i2 refid=cust.Alternative Directory
:p.Use :hp2.Alternative directory:ehp2. to specify the alternative logging
directory.
Logging to an alternative directory is especially useful when tracing a
&period.EXE/.COM that resides on a disk drive that does not have sufficient
free space available.
:p.This window provides the same functionality as OS2TRACE's -A option.
.*
.* HLP_CUSTBUF
.*
:h1 res=305.Help for Buffer Length
:i1 id=custbuf.Buffer Length
:i2 refid=cust.Buffer Length
:p.Use :hp2.Buffer length:ehp2. to specify the maximum number of bytes of level
three trace information to log from buffers.
Imposing a limit on buffer length is especially useful when tracing APIs that
input and/or output large arrays comprising of:
:ul compact.
:li.ASCIIZ strings
:li.Characters (bytes)
:li.Colors
:li.FIXED structures
:li.Integers
:li.POINTL structures
:li.RECTL structures
:li.User-defined buffers
:li.WPOINT structures
:eul.
:p.and only the first few entries are needed to provide a general idea of the
contents of the entire buffer.
:p.This window provides the same functionality as OS2TRACE's -B option, and is
only effective when logging level three trace information.
.*
.* HLP_CUSTCTL
.*
:h1 res=307.Help for Tracing Control
:i1 id=custctl.Tracing Control
:i2 refid=cust.Tracing Control
:p.Use :hp2.Tracing Control:ehp2. to specify enablement or disablement of
tracing control, the ability to pause and resume tracing through OS2TRACE's
-PAUSE and -RESUME options and PMOS2TRC's "Pause tracing" and "Resume tracing"
items on the "Control" submenu.
Controlling tracing is especially useful when only interested in tracing a
specific behavior of the application, such as opening a file or painting a
window.
:p.This window provides the same functionality as OS2TRACE's -C option.
.*
.* HLP_CUSTDOS
.*
:h1 res=309.Help for DOSCALLS APIs
:i1 id=custdos.DOSCALLS APIs
:i2 refid=cust.DOSCALLS APIs
:p.Use :hp2.DOSCALLS APIs:ehp2. to specify one or more groups of APIs to trace
from within DOSCALLS.DLL.
Limiting the number of groups of DOSCALLS APIs is especially useful when only
interested in tracing a few specific areas within DOSCALLS.DLL, such as:
:ul compact.
:li.Date/time and timer support
:li.Device specific, ring 2 support
:li.DosProfile API
:li.Exception management support
:li.File management
:li.Infoseg support
:li.Memory management
:li.Message
:li.Miscellaneous
:li.Module manager
:li.MVDM support
:li.Named pipes support
:li.National language support
:li.Performance
:li.Process and thread support
:li.Resource support
:li.Semaphore support
:li.Session manager support
:li.Signals
:li.SMP support
:eul.
:p.This window provides the same functionality as OS2TRACE's -D option, and is
only effective when DOSCALLS.DLL is enabled for tracing.
.*
.* HLP_CUSTEBC
.*
:h1 res=311.Help for EBCDIC Translation
:i1 id=custebc.EBCDIC Translation
:i2 refid=cust.EBCDIC Translation
:p.Use :hp2.EBCDIC Translation:ehp2. to specify enablement or disablement of
logging EBCDIC translation of character buffers.
Logging EBCDIC translation of character buffers is especially useful when the
contents of character buffers are not guaranteed to be ASCII.
:p.This window provides the same functionality as OS2TRACE's -E option.
.*
.* HLP_CUSTFIL
.*
:h1 res=313.Help for File Wrapping
:i1 id=custfil.File Wrapping
:i2 refid=cust.File Wrapping
:p.Use :hp2.File wrapping:ehp2. to specify the maximum number of bytes of trace
information to log before log file wrapping (overwriting from the beginning)
occurs.
Imposing a limit on log file length is especially useful when tracing large
numbers of APIs, when tracing over extended periods of time, or when disk space
is limited and only the most recent information is required.
However, imposing such a limit will further impact the performance of the
application being traced, as log file wrapping requires the log file to be
unbuffered, meaning that trace information cannot be buffered in a memory
cache before being written to disk.
:p.This window provides the same functionality as OS2TRACE's -F option.
.*
.* HLP_CUSTGPI
.*
:h1 res=315.Help for PMGPI APIs
:i1 id=custgpi.PMGPI APIs
:i2 refid=cust.PMGPI APIs
:p.Use :hp2.PMGPI APIs:ehp2. to specify one or more groups of APIs to trace
from within PMGPI.DLL.
Limiting the number of groups of PMGPI APIs is especially useful when only
interested in tracing a few specific areas within PMGPI.DLL, such as:
:ul compact.
:li.Basic PS control
:li.Bitmaps and pels operations
:li.Default primitive attributes
:li.Device contexts
:li.Drawing primitives and primitive attributes
:li.Inking
:li.Logical color tables
:li.Metafiles
:li.Paths and clipping with paths
:li.Phyical and logical fonts with lcids
:li.Picking, boundary and correlation
:li.Polygons
:li.Regions and clipping with regions
:li.Segment control and drawing
:li.Segment editing via elements
:li.Transform and transform conversion
:eul.
:p.This window provides the same functionality as OS2TRACE's -G option, and is
only effective when PMGPI.DLL is enabled for tracing.
.*
.* HLP_CUSTINT
.*
:h1 res=317.Help for API Interception
:i1 id=custint.API Interception
:i2 refid=cust.API Interception
:p.Use :hp2.API Interception:ehp2. to specify enablement or disablement of
interception of dynamic API calls.
Intercepting dynamic API calls is especially useful when tracing applications
that seem to be accomplishing much more through the operating system than is
evident in the trace file or applications that are backwards compatible across
several versions of the operating system.
For instance, even though OS2TRACE supports all APIs through OS/2 2.40, it will
run on all 32-bit versions of OS/2 prior to 2.40 because it dynamically loads
the APIs added after OS/2 2.00.
:p.This window provides the same functionality as OS2TRACE's -I option.
.*
.* HLP_CUSTLVL
.*
:h1 res=319.Help for Logging Level
:i1 id=custlvl.Logging Level
:i2 refid=cust.Logging Level
:p.Use :hp2.Logging level:ehp2. to specify the level of trace information to
log.
Logging specific amounts of trace information is especially useful when only
interested in summarizing trace information (minimum level) or when interested
in debugging trace information (maximum level).
Available trace information levels are:
:ol compact.
:li.API entry/exit information
:li.API parameters (includes level one information)
:li.API parameter contents (includes level two information)
:eol.
:p.This window provides the same functionality as OS2TRACE's -L option.
.*
.* HLP_CUSTTIM
.*
:h1 res=321.Help for Time Stamping
:i1 id=custtim.Time Stamping
:i2 refid=cust.Time Stamping
:p.Use :hp2.Time stamping:ehp2. to specify enablement or disablement of time
stamping of API entries and exits.
Time stamping API entries and exits is especially useful when tracing over
extended periods of time or when tracing multi-threaded applications.
:p.This window provides the same functionality as OS2TRACE's -T option.
.*
.* HLP_CUSTUSR
.*
:h1 res=323.Help for User Hook
:i1 id=custusr.User Hook
:i2 refid=cust.User Hook
:p.Use :hp2.User hook:ehp2. to specify a user-provided procedure (user hook)
that is called under the following circumstances:
:ol compact.
:li.Initialization
:li.Termination
:li.API Entry
:li.API Exit
:eol.
:p.Having a user hook called is especially useful when customized tracing
behavior is desired, such as tracing the stack whenever a specific API is
entered or logging the amount of memory available after every API is exited.
Because the user provides the hook, the user can perform whatever tracing
behavior is desired.
:p.This window provides the same functionality as OS2TRACE's -U option.
.*
.* HLP_CUSTWIN
.*
:h1 res=325.Help for PMWIN APIs
:i1 id=custwin.PMWIN APIs
:i2 refid=cust.PMWIN APIs
:p.Use :hp2.PMWIN APIs:ehp2. to specify one or more groups of APIs to trace
from within PMWIN.DLL.
Limiting the number of groups of PMWIN APIs is especially useful when only
interested in tracing a few specific areas within PMWIN.DLL, such as:
:ul compact.
:li.Atom manager
:li.Clipboard manager
:li.Country support
:li.Desktop API
:li.Dialog boxes
:li.Dynamic data exchange
:li.Error code definitions
:li.Frame manager
:li.General window management
:li.Heap manager
:li.Hook manager
:li.Keyboard accelerators
:li.Load/delete library/procedure
:li.Menu controls
:li.Message management
:li.Mouse and keyboard input
:li.Mouse pointers
:li.Palettes
:li.Rectangle routines
:li.Set error info API
:li.System values (and colors)
:li.Text cursors
:li.Thunk procedure API
:li.Timer routines
:li.WinCatch/WinThrow support
:li.WinDefWindowProc
:li.WinDispatchMsg and WinGetMsg
:li.WinTrackRect() function
:eul.
:p.This window provides the same functionality as OS2TRACE's -W option, and is
only effective when PMWIN.DLL is enabled for tracing.
.*
.* HLP_CTLMENU
.*
:h1 res=401.Help for Control
:i1 id=ctl.Control
:p.Select :hp2.Control:ehp2. to control the tracing of OS/2 APIs.
:p.This is accomplished by creating a thread within the application being
traced that awaits pause and resume requests.
When such a request arrives the thread awakens, pauses or resumes tracing, and
goes back to sleep awaiting the next request.
When finished, the paused or resumed trace-enabled applications, if any, are
displayed in a dialog similar to the following:
:artwork name='bitmaps\ctldlg.bmp' align=left.
:note.In order for the "Pause tracing" and "Resume tracing" items to be
effective on a trace-enabled executable, tracing control must be enabled
through OS2TRACE's -C option or PMOS2TRC's "Tracing control" item on the
"Customize" submenu before the executable commences running.
:note.A trace-enabled executable can be started while tracing is paused, as the
state of the trace control option is stored in OS2.INI and retrieved by the
trace DLLs when the executable commences running.
:p.For a detailed explanation of each menu item, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=403.
:dd.Pause tracing
:elink.
:dt.o
:link reftype=hd res=405.
:dd.Resume tracing
:elink.
:edl.
.*
.* HLP_CTLPAUSE
.*
:h1 res=403.Help for Pause Tracing
:i1 id=ctlpause.Pause Tracing
:p.Use :hp2.Pause tracing:ehp2. to pause the tracing of any trace-enabled
applications started while tracing control is enabled.
If this menu item is disabled (grayed-out) tracing is paused, and a
trace-enabled application can be started or continues to run without logging.
If this menu item is enabled (visible) tracing is resumed, and a
trace-enabled application can be started or continues to run with logging.
:p.When tracing control is enabled, information similar to the following is
logged if tracing is paused before the trace-enabled application commences
running:
:xmp.
  < OS2TRACE: Tracing initially paused >
:exmp.
:p.When tracing control is enabled, information similar to the following is
logged if tracing is paused after the trace-enabled application commences
running:
:xmp.
  < OS2TRACE: Pausing trace at nn&colon.nn&colon.nn.nn >
:exmp.
:p.This menu item provides the same functionality as OS2TRACE's -PAUSE option,
and is only effective when tracing control is enabled.
For more details on tracing control, see :link reftype=hd res=307.Help for
Tracing Control:elink..
.*
.* HLP_CTLRESUME
.*
:h1 res=405.Help for Resume Tracing
:i1 id=ctlresume.Resume Tracing
:p.Use :hp2.Resume tracing:ehp2. to resume the tracing of any trace-enabled
applications started while tracing control is enabled.
If this menu item is disabled (grayed-out) tracing is resumed, and a
trace-enabled application can be started or continues to run with logging.
If this menu item is enabled (visible) tracing is paused, and a
trace-enabled application can be started or continues to run without logging.
:p.When tracing control is enabled, information similar to the following is
logged if tracing is resumed after the trace-enabled application commences
running:
:xmp.
  < OS2TRACE: Resuming trace at nn&colon.nn&colon.nn.nn >
:exmp.
:p.This menu item provides the same functionality as OS2TRACE's -RESUME option,
and is only effective when tracing control is enabled.
For more details on tracing control, see :link reftype=hd res=307.Help for
Tracing Control:elink..
.*
.* HLP_SUMMENU
.*
:h1 res=501.Help for Summarize
:i1 id=sum.Summarize
:p.Select :hp2.Summarize:ehp2. to summarize the tracing of OS/2 APIs.
:p.This is accomplished by parsing the trace information file generated by a
trace-enabled executable and recording the number of API entries and exits
logged in the file.
When finished, the summary statistics are displayed in alphabetical order in a
dialog similar to the following:
:artwork name='bitmaps\sumdlg.bmp' align=left.
:p.For a detailed explanation of each menu item, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=503.
:dd.Open File
:elink.
:edl.
.*
.* HLP_SUMOPEN
.*
:h1 res=503.Help for Open File
:i1 id=sumopen.Open File for Summarization
:p.Use :hp2.Open file:ehp2. to open a trace information file in order to
summarize the tracing of OS/2 APIs logged in the file.
.*
.* HLP_OPTSMENU
.*
:h1 res=601.Help for Options
:i1 id=opts.Options
:p.Select :hp2.Options:ehp2. to take advantage of optional OS/2 API Trace
functionality.
:p.For a detailed explanation of each menu item, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=603.
:dd.Launch .EXE/.COM Files
:elink.
:dt.o
:link reftype=hd res=605.
:dd.Monitor Tracing
:elink.
:dt.o
:link reftype=hd res=607.
:dd.API Monitoring
:elink.
:dt.o
:link reftype=hd res=609.
:dd.File Length Monitoring
:elink.
:dt.o
:link reftype=hd res=611.
:dd.Title/Action Bar Only
:elink.
:dt.o
:link reftype=hd res=619.
:dd.Save Window Position
:elink.
:dt.o
:link reftype=hd res=621.
:dd.Restore Window Position
:elink.
:edl.
.*
.* HLP_OPTSEXE
.*
:h1 res=603.Help for Launch .EXE/.COM Files
:i1 id=optsexe.Launch .EXE/.COM Files
:i2 refid=opts.Launch .EXE/.COM Files
:p.Use :hp2.Launch .EXE/.COM files:ehp2. to enable or disable the launching of
&period.EXE and .COM files.
If this menu item is enabled (checked), a dialog similar to the following is
presented for launching a .EXE/.COM file whenever any trace enablement changes
are saved to the file and the enablement window is exited:
:artwork name='bitmaps\lchdlg.bmp' align=left.
If this menu item is disabled (unchecked), a dialog is not presented for
launching a .EXE/.COM file.
.*
.* HLP_OPTSMON
.*
:h1 res=605.Help for Monitor Tracing
:i1 id=optsmon.Monitor Tracing
:i2 refid=opts.Monitor Tracing
:p.Use :hp2.Monitor tracing:ehp2. to enable or disable the monitoring of
tracing.
If this menu item is enabled (checked), the following trace monitoring
information is displayed in the window while a trace-enabled executable runs:
:ul compact.
:li.Fully qualified log file name
:li.Executable file name
:li.Hexadecimal process identifier
:li.Number of APIs (optional)
:li.Log file length (optional)
:eul.
:p.The information displayed is similar to the following when all optional
information is included:
:artwork name='bitmaps\moninfo.bmp' align=left.
:p.If this menu item is disabled (unchecked), trace monitoring information is
not displayed in the window.
:note.When trace monitoring is enabled and the main window height is too small
to display all trace monitoring information, a dialog with Yes and No buttons
is presented which asks whether the window should be resized such that all
information is displayed.
:note.When trace monitoring is enabled and the main window is too close to the
bottom of the screen to display all trace monitoring information, a dialog with
Yes and No buttons is presented which asks whether the window should be
repositioned such that all information is displayed.
.*
.* HLP_OPTSAPI
.*
:h1 res=607.Help for API Monitoring
:i1 id=optsapi.API Monitoring
:i2 refid=opts.API Monitoring
:p.Use :hp2.API monitoring:ehp2. to specify the API monitoring frequency (the
rate at which the number of APIs is updated) or to disable the monitoring of
APIs altogether.
:p.This option is only effective when monitor tracing is enabled.
For more details on monitor tracing, see :link reftype=hd res=605.Help for
Monitor Tracing:elink..
.*
.* HLP_OPTSLEN
.*
:h1 res=609.Help for File Length Monitoring
:i1 id=optslen.File Length Monitoring
:i2 refid=opts.File Length Monitoring
:p.Use :hp2.File length monitoring:ehp2. to specify the file length monitoring
frequency (the rate at which the log file length is updated) or to disable the
monitoring of file length altogether.
:p.This option is only effective when monitor tracing is enabled.
For more details on monitor tracing, see :link reftype=hd res=605.Help for
Monitor Tracing:elink..
.*
.* HLP_OPTSBAR
.*
:h1 res=611.Help for Title/Action Bar Only
:i1 id=optsbar.Title/Action Bar Only
:i2 refid=opts.Title/Action Bar Only
:p.Use :hp2.Title/Action bar only:ehp2. to change the window's size such that
only the title bar and a single-line action bar are displayed, similar to the
following:
:artwork name='bitmaps\baronly.bmp' align=left.
:p.Displaying the title bar and action bar only is optimal because the client
window does not have to be painted, yet the window is large enough to display
the entire title and all menu items.
:note.Window width is based on the combined widths of all menu items.
:p.Based on the submenu selection, this resized window can be centered along
the top or bottom of the screen or can remain at the current window position.
:p.For a detailed explanation of each submenu item, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=613.
:dd.Top of Screen
:elink.
:dt.o
:link reftype=hd res=615.
:dd.Current Position
:elink.
:dt.o
:link reftype=hd res=617.
:dd.Bottom of Screen
:elink.
:edl.
.*
.* HLP_OPTSTOP
.*
:h1 res=613.Help for Top of Screen
:i1 id=optstop.Top of Screen
:i2 refid=optsbar.Top of Screen
:p.Use :hp2.Top of screen:ehp2. to position the window consisting of a title
and action bar only centered along the top of the screen.
:p.For more details on the resized window, see :link reftype=hd res=611.Help
for Title/Action Bar Only:elink..
.*
.* HLP_OPTSCUR
.*
:h1 res=615.Help for Current Position
:i1 id=optscur.Current Position
:i2 refid=optsbar.Current Position
:p.Use :hp2.Current position:ehp2. to position the window consisting of a title
and action bar only over the current window such that the upper-left corner
remains stationary.
:p.For more details on the resized window, see :link reftype=hd res=611.Help
for Title/Action Bar Only:elink..
.*
.* HLP_OPTSBOT
.*
:h1 res=617.Help for Bottom of Screen
:i1 id=optsbot.Bottom of Screen
:i2 refid=optsbar.Bottom of Screen
:p.Use :hp2.Bottom of screen:ehp2. to position the window consisting of a title
and action bar only centered along the bottom of the screen.
:p.For more details on the resized window, see :link reftype=hd res=611.Help
for Title/Action Bar Only:elink..
.*
.* HLP_OPTSSWP
.*
:h1 res=619.Help for Save Window Position
:i1 id=optsswp.Save Window Position
:i2 refid=opts.Save Window Position
:p.Use :hp2.Save window position:ehp2. to save the window's current window
position in the operating system's user profile, OS2.INI.
This window position is then restored upon future invocations of OS/2 API
Trace and when :hp2.Restore window position:ehp2. is used.
:note.The saved window position is not restored if the window's origin is not
on the OS/2 desktop.
This prevents the window from being drawn off the screen if the system's screen
resolution changes.
:p.For more details on :hp2.Restore window position:ehp2., see
:link reftype=hd res=621.Help for Restore Window Position:elink..
.*
.* HLP_OPTRSWP
.*
:h1 res=621.Help for Restore Window Position
:i1 id=optrswp.Restore Window Position
:i2 refid=opts.Restore Window Position
:p.Use :hp2.Restore window position:ehp2. to restore the window's current
window position from the position saved in the operating system's user profile,
OS2.INI, by the previous :hp2.Save window position:ehp2. use.
:note.The saved window position is not restored if the window's origin is not
on the OS/2 desktop.
This prevents the window from being drawn off the screen if the system's screen
resolution changes.
:p.For more details on :hp2.Save window position:ehp2., see
:link reftype=hd res=619.Help for Save Window Position:elink..
.*
.* HLP_HELPKEY
.*
:h1 res=709.Keys Help for OS/2 API Trace
:p.The following keys can be used as short cuts for commonly performed actions:
:dl.
:dt.:hp2.F1:ehp2.
:dd.Displays online help information.
:dt.:hp2.F3:ehp2.
:dd.Exits OS/2 API Trace application.
:dt.:hp2.Ctrl+A:ehp2.
:dd.Customizes alternative directory.
:dt.:hp2.Ctrl+B:ehp2.
:dd.Customizes buffer length.
:dt.:hp2.Ctrl+C:ehp2.
:dd.Customizes tracing control.
:dt.:hp2.Ctrl+D:ehp2.
:dd.Customizes DOSCALLS APIs.
:dt.:hp2.Ctrl+E:ehp2.
:dd.Enables and/or disables executable file.
:dt.:hp2.Ctrl+F:ehp2.
:dd.Customizes file wrapping.
:dt.:hp2.Ctrl+G:ehp2.
:dd.Customizes PMGPI APIs.
:dt.:hp2.Ctrl+I:ehp2.
:dd.Customizes API interception.
:dt.:hp2.Ctrl+L:ehp2.
:dd.Customizes logging level.
:dt.:hp2.Ctrl+M:ehp2.
:dd.Toggles monitoring of tracing.
:dt.:hp2.Ctrl+P:ehp2.
:dd.Saves window position.
:dt.:hp2.Ctrl+R:ehp2.
:dd.Toggles launching of .EXE/.COM files.
:dt.:hp2.Ctrl+S:ehp2.
:dd.Summarizes trace information file.
:dt.:hp2.Ctrl+T:ehp2.
:dd.Customizes time stamping.
:dt.:hp2.Ctrl+W:ehp2.
:dd.Customizes PMWIN GPIs.
:edl.
.*
.* HLP_FAQ
.*
:h1 res=711.Help for Frequently Asked Questions
:i1 id=faqlong.Frequently Asked Questions
:i1 id=faqshort.FAQ
:artwork name='bitmaps\os2trace.bmp' align=center.
:dl tsize=3.
:dt.:hp2.Q&colon.:ehp2.
:dd.In as simple terms as possible, how does tracing with OS/2 API Trace really
work?
:dt.:hp2.A&colon.:ehp2.
:dd.In short, OS/2 API Trace intercepts the OS/2 API calls an application
makes, allowing it to log tracing information both before and after calling the
intended OS/2 API.
How this is done is really quite simple, albeit a little lengthy to describe in
"simple" terms.
:p.Every time an executable file makes a call to a procedure external to
itself, such as an OS/2 API, a fixup record is generated by the program linker
that tells the operating system which procedure is required when the file is
loaded at run-time.
This fixup record includes information such as what dynamic link library (DLL)
contains the procedure and which procedure it is, identified by either name or
ordinal.
Instead of storing the name of each procedure's DLL within the actual fixup
record, the DLL names are instead stored together in a separate place within
the executable file, the import module name table, and an index into this table
is stored within the fixup record.
This is done to save space, in that a DLL's name is stored once per executable
file instead of once per fixup record.
:p.When tracing is enabled for one of the supported DLLs, OS/2 API Trace simply
goes into the executable file's import module name table and overwrites the
name of the OS/2 DLL being enabled, such as "DOSCALLS", with the name of the
corresponding trace DLL, such as "T_DOS___".
When the executable file is loaded, the program loader redirects all API calls
intended for the OS/2 DLL to the exact same ordinals within the trace DLL
instead because the trace DLL's name now resides within the import module name
table.
Each trace DLL mirrors its corresponding OS/2 DLL ordinal for ordinal.
If the ordinal within the OS/2 DLL represents a published API, the trace DLL
contains a matching trace API (with identical parameter list) that logs API
entry information, calls the OS/2 API, and logs API exit information.
If the ordinal within the OS/2 DLL represents an unpublished (private) API,
the trace DLL passes the call directly to the OS/2 DLL through use of a
forwarder entry.
:p.When tracing is disabled for one of the supported DLLs, OS/2 API Trace
simply undoes what was done during enablement.
That is, it goes into the executable file's import module name table and
overwrites the name of the trace DLL being disabled, such as "T_DOS___", with
the name of the corresponding OS/2 DLL, such as "DOSCALLS".
When complete, the executable file is restored to its original state and all
OS/2 API calls go directly to the OS/2 DLL.
:dt.:hp2.Q&colon.:ehp2.
:dd.How easy is it for a non-technical person to trace an application with OS/2
API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.It is really quite easy to trace an application with OS/2 API Trace as long
as one doesn't get overwhelmed by the number of options the utility provides.
Keep in mind that OS/2 API Trace provides four unique functions: trace
enablement, trace customization, trace control, and trace summarization.
Trace enablement turns on/off tracing of an application, trace customization
sets the types of trace information generated by the application, trace control
pauses/resumes tracing of the application, and trace summarization totals the
APIs in the trace information generated by the application.
:p.As an example, follow the simple scenario below to trace an executable
present on every OS/2 system, CMD.EXE.
:p.Begin by opening an OS/2 window or full screen session and entering the
following commands:
:xmp.
  md \cmdtrace
  cd \cmdtrace
  copy x&colon.\os2\cmd.exe
:exmp.
:p.where x is the drive on which OS/2 is installed.
These commands create a temporary copy of CMD.EXE to work on so as not to
interfere with the original copy in the \OS2 directory.
A temporary copy is not really necessary, but will avoid duplicate trace files
being created in the event that some other process uses CMD.EXE while running
this scenario.
:p.Start the tracing process by customizing OS/2 API Trace so as to get as much
trace information as possible:
:xmp.
  os2trace -a none -b all -c off -d all -e on -f all -g all -i on -l 3 -t on -u none -w all
:exmp.
:p.After the program banner, the utility displays the former options as well as
these new options.
OS/2 API Trace remembers the states of these customization options and uses
them for all future traces until they are changed by some subsequent
customization commands.
:p.Continue the tracing process by trace-enabling CMD.EXE:
:xmp.
  os2trace -on -all cmd.exe
:exmp.
:p.After the program banner, the utility displays which DLLs were and were not
enabled for tracing.
Note that in this case CMD.EXE only imports APIs from DOSCALLS.
:p.At this point, CMD is ready to be traced.
Go ahead and execute it by entering the following command:
:xmp.
  cmd
:exmp.
:p.Enter as few or as many commands in this new command shell as desired; when
complete, terminate the command shell by entering the following command:
:xmp.
  exit
:exmp.
:p.Complete the tracing process by trace-disabling CMD.EXE so that any future
invocations of CMD.EXE do not produce additional tracing information which
overwrites CMD.TRC:
:xmp.
  os2trace -off -all cmd.exe
:exmp.
:p.After the program banner, the utility displays which DLLs were and were not
disabled for tracing.
:note.The TRACE-IT batch file incorporates all of the above OS2TRACE commands
and invokes the executable, so the following works just as well:
:xmp.
  trace-it cmd
:exmp.
:p.Now go ahead and edit CMD.TRC, which contains entry and exit information for
every API used by CMD.EXE while it was executing.
For instance, note how every keystroke is read in by Kbd16CharIn and echoed to
the display by Vio16WrtTTY, using the editor's find/search capabilities to
locate these APIs within the trace information file.
:p.Optionally extend the tracing process by summarizing the APIs used by
CMD.EXE:
:xmp.
  os2trace -s cmd.trc
:exmp.
:p.After the program banner, the utility displays an alphabetical list of the
APIs used and how many times each was invoked successfully and unsuccessfully.
:p.Finish by entering the following commands:
:xmp.
  del cmd.*
  cd \
  rd \cmdtrace
:exmp.
:p.to remove the temporary files and directory.
:dt.:hp2.Q&colon.:ehp2.
:dd.What versions of OS/2 are supported by OS/2 API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.OS/2 API Trace supports all 32-bit versions of OS/2:
:table cols='36 15 15'.
:row.
:c.:hp1.Name:ehp1.
:c.:hp1.Major Version:ehp1.
:c.:hp1.Minor Version:ehp1.
:row.
:c.OS/2 Version 2.0x
:c.           20
:c.           0x
:row.
:c.OS/2 Version 2.1x
:c.           20
:c.           1x
:row.
:c.OS/2 Warp Version 3
:c.           20
:c.           30
:row.
:c.OS/2 Warp Version 4
:c.           20
:c.           40
:row.
:c.OS/2 Warp Server for e-business
:c.           20
:c.           45
:row.
:c.OS/2 Warp Connect (PowerPC Edition)
:c.           30
:c.           00
:etable.
:p.where major and minor versions are the corresponding values returned from
DosQuerySysInfo.
Note that the ELF versions of the executables and trace DLLs are required to
run on OS/2 Warp Connect (PowerPC Edition).
:dt.:hp2.Q&colon.:ehp2.
:dd.What purpose does each file serve in OS/2 API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.The files that comprise the OS/2 API Trace utility are described in the
following table:
:table cols='13 40'.
:row.
:c.:hp1.Name:ehp1.
:c.:hp1.Purpose:ehp1.
:row.
:c.OS2TRACE.NWS
:c.Text file describing the change history of the complete OS/2 API Trace
package
:row.
:c.OS2TRACE.DOC
:c.Text file providing a technical description of OS/2 API Trace, its usage and
output, and a typical scenario
:row.
:c.OS2TRACE.FAQ
:c.Text file providing answers to the most frequently asked questions about
OS/2 API Trace
:row.
:c.OS2TRACE.API
:c.Text file containing all of the supported APIs along with their OS/2 DLL,
trace API group (if any), and minimum required version of 32-bit OS/2
:row.
:c.OS2TRACE.EXE
:c.OS/2 API Trace command line interface
:row.
:c.PMOS2TRC.EXE
:c.OS/2 API Trace Presentation Manager (graphical) interface
:row.
:c.PMOS2TRC.HLP
:c.Online help for the Presentation Manager interface
:row.
:c.TRACE-IT.CMD
:c.Batch file that performs all commands necessary to trace a .EXE file
:row.
:c.CHK4TRC.DOC
:c.Text file providing a technical description of the trace-enabled executable
checker, its usage and output
:row.
:c.CHK4TRC.EXE
:c.Trace-enabled executable checker command line interface
:row.
:c.STRIPAPI.DOC
:c.Text file providing a technical description of the trace information file
API stripper, its usage and output
:row.
:c.STRIPAPI.EXE
:c.Trace information file API stripper command line interface
:row.
:c.T_*.DLL
:c.Trace DLLs
:row.
:c.HOOKDLL\*
:c.User hook DLL source code example
:etable.
:dt.:hp2.Q&colon.:ehp2.
:dd.Why won't OS/2 API Trace enable tracing of a DLL that has an API imported
by name?
:dt.:hp2.A&colon.:ehp2.
:dd.This restriction was removed as of version 2.45.18 except for the following
APIs:
:table cols='35 19'.
:row.
:c.PMDRAG.DRGACCESSDRAGINFO
:c.PMPIC.PICPRINT
:row.
:c.PMDRAG.DRGADDSTRHANDLE
:c.PMPIC.PICICHG
:row.
:c.PMDRAG.DRGALLOCDRAGINFO
:c.PMPIC.PIF2MET
:row.
:c.PMDRAG.DRGALLOCDRAGTRANSFER
:c.PMPIC.PRFPIF2MET
:row.
:c.PMDRAG.DRGDELETEDRAGINFOSTRHANDLES
:c.PMPIC.PIC32PRINT
:row.
:c.PMDRAG.DRGDELETESTRHANDLE
:c.PMPIC.PIC32ICHG
:row.
:c.PMDRAG.DRGDRAG
:c.PMPIC.PRF32PIF2MET
:row.
:c.PMDRAG.DRGFREEDRAGINFO
:c.
:row.
:c.PMDRAG.DRGFREEDRAGTRANSFER
:c.
:row.
:c.PMDRAG.DRGGETPS
:c.
:row.
:c.PMDRAG.DRGPOSTTRANSFERMSG
:c.
:row.
:c.PMDRAG.DRGPUSHDRAGINFO
:c.
:row.
:c.PMDRAG.DRGQUERYDRAGITEM
:c.
:row.
:c.PMDRAG.DRGQUERYDRAGITEMCOUNT
:c.
:row.
:c.PMDRAG.DRGQUERYDRAGITEMPTR
:c.
:row.
:c.PMDRAG.DRGQUERYNATIVERMF
:c.
:row.
:c.PMDRAG.DRGQUERYNATIVERMFLEN
:c.
:row.
:c.PMDRAG.DRGQUERYSTRNAME
:c.
:row.
:c.PMDRAG.DRGQUERYSTRNAMELEN
:c.
:row.
:c.PMDRAG.DRGQUERYTRUETYPE
:c.
:row.
:c.PMDRAG.DRGQUERYTRUETYPELEN
:c.
:row.
:c.PMDRAG.DRGRELEASEPS
:c.
:row.
:c.PMDRAG.DRGSENDTRANSFERMSG
:c.
:row.
:c.PMDRAG.DRGSETDRAGPOINTER
:c.
:row.
:c.PMDRAG.DRGSETDRAGIMAGE
:c.
:row.
:c.PMDRAG.DRGSETDRAGITEM
:c.
:row.
:c.PMDRAG.DRGVERIFYNATIVERMF
:c.
:row.
:c.PMDRAG.DRGVERIFYRMF
:c.
:row.
:c.PMDRAG.DRGVERIFYTRUETYPE
:c.
:row.
:c.PMDRAG.DRGVERIFYTYPE
:c.
:row.
:c.PMDRAG.DRGVERIFYTYPESET
:c.
:row.
:c.PMDRAG.DRGDRAGFILES
:c.
:row.
:c.PMDRAG.DRGACCEPTDROPPEDFILES
:c.
:etable.
:p.The reason that these PMDRAG APIs are unsupported is that between OS/2 2.10
and 2.30 their names were changed to include "16" in the prefix (for example,
DRGACCESSDRAGINFO became DRG16ACCESSDRAGINFO); the newer names are supported.
The reason that these PMPIC APIs are unsupported is that between OS/2 2.30 and
2.40 their names were changed to mixed case (for  example, PICPRINT became
PicPrint); the newer names are supported.
Special versions of the PMDRAG and PMPIC trace DLLs that support the older
names can be requested from the author.
:dt.:hp2.Q&colon.:ehp2.
:dd.Why does the date and time stamp of an executable file change when tracing
is enabled or disabled?
:dt.:hp2.A&colon.:ehp2.
:dd.An executable file's date and time stamp changes whenever a supported DLL
is enabled or disabled for tracing because the file's import module name table
is updated and the changes written to disk.
Nothing else within the executable file is affected by OS/2 API Trace.
:note.As of version 2.40.85, the executable file's date and time stamp can be
preserved through use of OS2TRACE's -P option and PMOS2TRC's "Preserve file
date/time stamp" item on the "Enable" submenu.
:dt.:hp2.Q&colon.:ehp2.
:dd.Can DLLs be traced through OS/2 API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.Yes, OS/2 API Trace will work on any segmented (16-bit, NE format) or
linear (32-bit, LX format) executable file, such as .EXE, .COM, and .DLL files.
Any trace information generated by a DLL, however, cannot be stored in a trace
information file named after the .DLL file because DLLs can be shared by more
than one process.
As a result, all trace information generated by a DLL is stored in a trace
information file named after the .EXE or .COM file.
For example, if A.EXE imports from B.DLL and B.DLL is trace-enabled, the trace
information from B.DLL is logged to A.TRC when A.EXE is executed.
:p.There is no limit to the number of DLLs that can be traced at once, but
trace information from all DLLs is logged to a single trace information file
named after the .EXE or .COM file that directly or indirectly loaded the DLLs.
:dt.:hp2.Q&colon.:ehp2.
:dd.Can 16-bit APIs be traced with OS/2 API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.Yes, all the 16-bit APIs present in the Base subsystem (Dos, Kbd, Mou, and
Vio APIs) and Presentation Manager subsystem (Dev, Drg, Gpi, Pic, Prf, Prt,
Spl, and Win APIs) are supported as of version 2.40.34.
See OS2TRACE.API for a table containing all supported APIs.
:dt.:hp2.Q&colon.:ehp2.
:dd.Can SMP (Symmetric Multiprocessing) APIs be traced with OS/2 API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.Yes, all the 16-bit and 32-bit SMP APIs present in the Base subsystem (Dos
APIs) are supported as of version 2.40.61.
See OS2TRACE.API for a table containing all supported APIs.
:dt.:hp2.Q&colon.:ehp2.
:dd.Can APIs loaded by the application at run-time through Dos16GetProcAddr or
Dos32QueryProcAddr be traced with OS/2 API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.Yes, OS2TRACE's -I option and PMOS2TRC's "API interception" item on the
"Customize" submenu enable interception and tracing of these APIs, also known
as dynamically loaded APIs.
This functionality is available as of version 2.40.36.
:note.In order to intercept dynamic API calls, DOSCALLS must be enabled for
tracing even though Dos16GetProcAddr and Dos32QueryProcAddr do not have to
actually be traced.
:dt.:hp2.Q&colon.:ehp2.
:dd.What needs to be done to trace all APIs of a specific type, such as Dos
APIs or Win APIs?
:dt.:hp2.A&colon.:ehp2.
:dd.Use the following table to determine which DLL or DLLs to enable for
tracing in order to trace all APIs of a specific API type:
:table cols='13 40'.
:row.
:c.:hp1.API Type:ehp1.
:c.:hp1.DLL(s):ehp1.
:row.
:c.Ddf (32-bit)
:c.HELPMGR
:row.
:c.Dev (16-bit)
:c.PMGPI
:row.
:c.Dev (32-bit)
:c.PMGPI
:row.
:c.Dos (16-bit)
:c.DOSCALLS, MSG, NLS, MONCALLS, NAMPIPES, PMSPL, SESMGR, QUECALLS
:row.
:c.Dos (32-bit)
:c.DOSCALLS, MSG, NLS, SESMGR, QUECALLS
:row.
:c.Drg (16-bit)
:c.PMDRAG
:row.
:c.Drg (32-bit)
:c.PMDRAG
:row.
:c.Gpi (16-bit)
:c.PMBIDI, PMGPI, PMWIN
:row.
:c.Gpi (32-bit)
:c.PMBIDI, PMGPI
:row.
:c.Kbd (16-bit)
:c.KBDCALLS
:row.
:c.Mou (16-bit)
:c.MOUCALLS
:row.
:c.Pic (16-bit)
:c.PMPIC
:row.
:c.Pic (32-bit)
:c.PMPIC
:row.
:c.Prf (16-bit)
:c.PMSHAPI
:row.
:c.Prf (32-bit)
:c.PMSHAPI
:row.
:c.Prt (16-bit)
:c.PMSPL
:row.
:c.Prt (32-bit)
:c.PMSPL
:row.
:c.Spl (16-bit)
:c.PMSPL
:row.
:c.Spl (32-bit)
:c.PMSPL
:row.
:c.Vio (16-bit)
:c.VIOCALLS
:row.
:c.Win (16-bit)
:c.HELPMGR, PMBIDI, PMSHAPI, PMVIOP, PMWIN
:row.
:c.Win (32-bit)
:c.HELPMGR, PMBIDI, PMCTLS, PMMERGE, PMSHAPI, PMWIN, PMWP
:etable.
:p.Note that some DLLs contain more than one API type, such as HELPMGR and
PMSPL, so there may be some overlap of API types involved when enabling one of
these DLLs for tracing.
:dt.:hp2.Q&colon.:ehp2.
:dd.Where are all those APIs in the trace information file coming from that
aren't part of the application's source code?
:dt.:hp2.A&colon.:ehp2.
:dd.The C run-time.
Remember that OS/2 API Trace intercepts every OS/2 API call made by an
application being traced, and the C run-time requires OS/2 APIs to function
properly under OS/2.
For instance, the run-time needs DosWrite in order to implement fprintf.
:dt.:hp2.Q&colon.:ehp2.
:dd.Can network APIs be traced with OS/2 API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.No, OS/2 API Trace only covers APIs included in the basic operating system,
which includes the Base, Presentation Manager, and Workplace Shell subsystems.
Operating system extensions such as Multimedia, TCP/IP, and Network are not
supported.
See OS2TRACE.API for a table containing all supported APIs.
:dt.:hp2.Q&colon.:ehp2.
:dd.Are there any special steps should be taken to trace an application over a
long period of time, such as hours or even days?
:dt.:hp2.A&colon.:ehp2.
:dd.Outside of a minimal performance impact, OS/2 API Trace does not hinder the
functionality of any APIs being traced, so no unusual steps are required to
trace an application over a long period of time.
However, a couple of customization options are provided that may be beneficial
when tracing over large periods of time.
:p.The first such option is time stamping, which is specified through
OS2TRACE's -T option and PMOS2TRC's "Time stamping" item on the "Customize"
submenu and is available as of version 2.30.18.
When time stamping is enabled, all API entries and exits are marked with the
time they occurred accurate to a hundredth of a second.
Time stamping can be quite useful to record when exactly APIs were called while
logging several hours of trace information.
:p.The second option is file wrapping, which is specified through OS2TRACE's
-F option and PMOS2TRC's "File wrapping" item on the "Customize" submenu  and
is available as of version 2.30.17.
When file wrapping is enabled, only a certain amount of trace information is
logged before the trace information file is overwritten starting from the
beginning.
File wrapping can be quite useful to set an absolute limit to the trace
information file size, because, depending on the level of trace information
requested and the number of APIs being traced, OS/2 API Trace can generate
megabytes upon megabytes of information rather quickly.
:note.Setting the log file wrapping length to anything other than ALL will
further impact the performance of the application being traced, as log file
wrapping requires the log file to be unbuffered, meaning that trace information
cannot be buffered in a memory cache before being written to disk.
:dt.:hp2.Q&colon.:ehp2.
:dd.It is documented that OS/2 API Trace can trace APIs from the latest version
of OS/2.
Does this mean that it cannot work on earlier versions of OS/2 because the
latest APIs were not yet present on them?
:dt.:hp2.A&colon.:ehp2.
:dd.OS/2 API Trace will work on ALL versions of 32-bit OS/2, or OS/2 2.x.
This is possible because the trace DLLs dynamically load all APIs added to OS/2
after version 2.00 through use of the DosQueryProcAddr API.
Only APIs supported by OS/2 2.00 are actually imported by the trace DLLs.
:dt.:hp2.Q&colon.:ehp2.
:dd.Will OS/2 API Trace work on 16-bit versions of OS/2?
:dt.:hp2.A&colon.:ehp2.
:dd.No, all executable files that comprise OS/2 API Trace are of the linear
(32-bit, LX) executable file format, which is unknown to all versions of 16-bit
OS/2, or OS/2 1.x.
The 16-bit APIs that OS/2 API Trace supports can only be traced under OS/2 2.x.
:dt.:hp2.Q&colon.:ehp2.
:dd.How can an application be traced if it resides on a disk with little, if
any,
available space?
:dt.:hp2.A&colon.:ehp2.
:dd.An alternative logging directory can be specified through OS2TRACE's -A
option and PMOS2TRC's "Alternative directory" item on the "Customize" submenu
and is available as of version 2.45.24.
When a fully qualified drive and path is specified for the alternative
directory, it overrides the default directory (same as the .EXE/.COM file) and
all trace information files are generated within it.
This allows applications that reside on disks with space constraints to be
traced by specifying an alternative directory on a different drive altogether.
:dt.:hp2.Q&colon.:ehp2.
:dd.When under disk space constraints, can only the trace DLLs needed be placed
on the system where the tracing will take place?
:dt.:hp2.A&colon.:ehp2.
:dd.Yes, the only trace DLLs that are required in order for tracing to take
place are T_COMMON.DLL and whatever trace DLLs have been enabled for tracing.
The only exceptions to this rule are in the following table:
:table cols='13 40'.
:row.
:c.:hp1.Trace DLL:ehp1.
:c.:hp1.Other Required Trace DLLs:ehp1.
:row.
:c.T_CTL_.DLL
:c.T_DRG_.DLL
:row.
:c.T_DOS___.DLL
:c.T_KBD___.DLL, T_M.DLL, T_MON___.DLL, T_MOU___.DLL, T_N.DLL, T_PIP___.DLL,
T_QUE___.DLL, T_SES_.DLL, T_VIO___.DLL
:row.
:c.T_MRG__.DLL
:c.T_GPI.DLL, T_SHL__.DLL, T_WIN.DLL
:etable.
These other trace DLLs are required because the trace DLLs in the left column
import trace APIs from the trace DLLs in the right column.
The following table shows all of the supported OS/2 DLLs and their
corresponding trace DLLs:
:table cols='13 13'.
:row.
:c.:hp1.OS/2 DLL:ehp1.
:c.:hp1.Trace DLL:ehp1.
:row.
:c.DOSCALLS.DLL
:c.T_DOS___.DLL
:row.
:c.HELPMGR.DLL
:c.T_HLP__.DLL
:row.
:c.KBDCALLS.DLL
:c.T_KBD___.DLL
:row.
:c.MONCALLS.DLL
:c.T_MON___.DLL
:row.
:c.MOUCALLS.DLL
:c.T_MOU___.DLL
:row.
:c.MSG.DLL
:c.T_M.DLL
:row.
:c.NAMPIPES.DLL
:c.T_PIP___.DLL
:row.
:c.NLS.DLL
:c.T_N.DLL
:row.
:c.PMBIDI.DLL
:c.T_PBD_.DLL
:row.
:c.PMCTLS.DLL
:c.T_CTL_.DLL
:row.
:c.PMDRAG.DLL
:c.T_DRG_.DLL
:row.
:c.PMGPI.DLL
:c.T_GPI.DLL
:row.
:c.PMMERGE.DLL
:c.T_MRG__.DLL
:row.
:c.PMPIC.DLL
:c.T_PIC.DLL
:row.
:c.PMSHAPI.DLL
:c.T_SHL__.DLL
:row.
:c.PMSPL.DLL
:c.T_SPL.DLL
:row.
:c.PMVIOP.DLL
:c.T_PVP_.DLL
:row.
:c.PMWIN.DLL
:c.T_WIN.DLL
:row.
:c.PMWP.DLL
:c.T_WP.DLL
:row.
:c.QUECALLS.DLL
:c.T_QUE___.DLL
:row.
:c.SESMGR.DLL
:c.T_SES_.DLL
:row.
:c.VIOCALLS.DLL
:c.T_VIO___.DLL
:etable.
:dt.:hp2.Q&colon.:ehp2.
:dd.Can only the files needed be replaced when updating to a later version of
OS/2 API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.It is not recommended, as OS/2 API Trace may (and usually will) change
internally from release to release.
In fact, as of version 2.40.58, an error is issued if the version of one of the
required trace DLLs does not match the version of T_COMMON.
Make sure that all the files that comprise OS/2 API Trace have the same date
and time stamps before attempting to use the utility.
:p.The sizes and date and time stamps of all the files that comprise the latest
release, Version 2.45.40, are shown in the following table:
:table cols='13 6 8 5'.
:row.
:c.:hp1.Name:ehp1.
:c.:hp1.Size:ehp1.
:c.:hp1.Date:ehp1.
:c.:hp1.Time:ehp1.
:row.
:c.OS2TRACE.NWS
:c. 42127
:c.10-25-10
:c.5&colon.00p
:row.
:c.OS2TRACE.DOC
:c. 54566
:c.10-25-10
:c.5&colon.00p
:row.
:c.OS2TRACE.FAQ
:c. 32087
:c.10-25-10
:c.5&colon.00p
:row.
:c.OS2TRACE.API
:c.125829
:c.10-25-10
:c.5&colon.00p
:row.
:c.OS2TRACE.EXE
:c. 91762
:c.10-25-10
:c.5&colon.00p
:row.
:c.PMOS2TRC.EXE
:c.163247
:c.10-25-10
:c.5&colon.00p
:row.
:c.PMOS2TRC.HLP
:c.241353
:c.10-25-10
:c.5&colon.00p
:row.
:c.TRACE-IT.CMD
:c.  3083
:c.10-25-10
:c.5&colon.00p
:row.
:c.CHK4TRC.DOC
:c.  3644
:c.10-25-10
:c.5&colon.00p
:row.
:c.CHK4TRC.EXE
:c. 49405
:c.10-25-10
:c.5&colon.00p
:row.
:c.STRIPAPI.DOC
:c.  2926
:c.10-25-10
:c.5&colon.00p
:row.
:c.STRIPAPI.EXE
:c. 54496
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_COMMON.DLL
:c.331553
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_CTL_.DLL
:c.  5464
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_DOS___.DLL
:c.186938
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_DRG_.DLL
:c. 31208
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_GPI.DLL
:c.249575
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_HLP__.DLL
:c. 11673
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_KBD___.DLL
:c. 11066
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_M.DLL
:c.  5317
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_MON___.DLL
:c.  3834
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_MOU___.DLL
:c. 13658
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_MRG__.DLL
:c. 19305
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_N.DLL
:c.  5173
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_PBD_.DLL
:c.  9960
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_PIC.DLL
:c.  2855
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_PIP___.DLL
:c.  9706
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_PVP_.DLL
:c.  3656
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_QUE___.DLL
:c.  9562
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_SES_.DLL
:c.  5480
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_SHL__.DLL
:c. 44169
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_SPL.DLL
:c. 60807
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_VIO___.DLL
:c. 33754
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_WIN.DLL
:c.237383
:c.10-25-10
:c.5&colon.00p
:row.
:c.T_WP.DLL
:c. 12278
:c.10-25-10
:c.5&colon.00p
:etable.
:dt.:hp2.Q&colon.:ehp2.
:dd.Why do some 16-bit applications trap or behave differently when being
traced?
:dt.:hp2.A&colon.:ehp2.
:dd.Traps and/or different behavior when tracing 16-bit applications is usually
a sign of stack overwriting and/or stack overflow.
The problem stems from the fact that stacks for all 16-bit threads must be
provided by the programmer, which often leads to stacks that are surrounded by
data variables and/or stacks that do not have the recommended amount of space
available (4KB) when calling into an OS/2 API.
This problem does not usually occur in the case of 32-bit threads because the
programmer need only provide a stack size, resulting in a separate object for
the stack being allocated by the operating system.
Regardless, tracing does require additional stack space over and above what is
required normally, as shown in the following table:
:table cols='7 35'.
:row.
:c.:hp1.API:ehp1.
:c.:hp1.Additional Stack Space Requirement (in bytes):ehp1.
:row.
:c.16-bit
:c.122 + ( 2 * sizeof( parameters ) )
:row.
:c.32-bit
:c.sizeof( parameters )
:etable.
:p.This slight overhead can easily lead to traps or different behavior.
If these symptoms occur when tracing your own application, increase the stack
size for all of your threads and try again.
:dt.:hp2.Q&colon.:ehp2.
:dd.What is trace control good for?
:dt.:hp2.A&colon.:ehp2.
:dd.Trace control allows tracing to be paused (no logging occurs) and resumed
(normal logging occurs) while trace-enabled applications are running.
This is especially useful when only interested in tracing a specific behavior
of an application.
For example, pause tracing, start the application and let it run to the point
where tracing is desired, resume tracing, perform the desired function, pause
tracing again, and let the application run to termination.
The trace information file will only contain API calls made while tracing was
active (resumed), which are just those APIs associated with the specific
behavior.
:dt.:hp2.Q&colon.:ehp2.
:dd.What is trace summarization good for?
:dt.:hp2.A&colon.:ehp2.
:dd.Summarizing a trace information file produces an alphabetical listing of
all the APIs called by an application and how many times each API was
successfully, unsuccessfully, and indeterminately called.
This information can be quite useful in determining where erroneous API calls
can be removed or repaired and where performance enhancements can be made.
:dt.:hp2.Q&colon.:ehp2.
:dd.Why are there so many failures logged when tracing certain APIs, such as
WinDefWindowProc, when the application clearly works?
:dt.:hp2.A&colon.:ehp2.
:dd.Some Presentation Manager APIs return identical values with different
meanings based solely on the input parameters.
This is most evident in APIs dealing with messages, where some messages will
return zero to indicate success while others will return zero to indicate
failure.
Instead of going through rather lengthy logic to determine whether a return
value indicates success or failure for this small group of APIs, OS/2 API Trace
assumes that zero indicates failure and non-zero indicates success.
:note.As of version 2.40.45, OS/2 API Trace considers the following APIs'
return codes to be indeterminate (neither successful nor unsuccessful):
:ul compact.
:li.Drg16SendTransferMsg
:li.Drg32SendTransferMsg
:li.Win16DdeRespond
:li.Win32DdeRespond
:li.Win16DefDlgProc
:li.Win32DefDlgProc
:li.Win32DefFileDlgProc
:li.Win32DefFontDlgProc
:li.Win16DefWindowProc
:li.Win32DefWindowProc
:li.Win16DispatchMsg
:li.Win32DispatchMsg
:li.Win16SendDlgItemMsg
:li.Win32SendDlgItemMsg
:li.Win16SendMsg
:li.Win32SendMsg
:li.Win16SetErrorInfo
:li.Win32SetErrorInfo
:eul.
:dt.:hp2.Q&colon.:ehp2.
:dd.Why did OS/2 API Trace abort tracing and prematurely terminate the
application?
:dt.:hp2.A&colon.:ehp2.
:dd.OS/2 API Trace is an OS/2 application and thereby constrained by OS/2
resource limits, especially those that apply to memory, semaphores, files, and
Presentation Manager.
Should tracing appear to terminate the application prematurely, make sure to
pay attention to any error messages displayed either on the console or in a
message box.
Also, make sure to check the contents of both OS2TRACE.ERR and PMOS2TRC.ERR in
the root directory of the boot drive.
:dt.:hp2.Q&colon.:ehp2.
:dd.Why aren't some of the supported Dos APIs in DOSCALLS.DLL getting traced
even when DOSCALLS is enabled for tracing?
:dt.:hp2.A&colon.:ehp2.
:dd.If the missing APIs are listed in OS2TRACE.API, then make sure the
application does not import any of the APIs from DOSCALL1.DLL through its
module definitions file (.DEF), as in the following:
:xmp.
  IMPORTS
    DosQueryThreadContext=DOSCALL1.877
:exmp.
:p.If this is indeed the case, change DOSCALL1 to DOSCALLS and rebuild the
application.
This will not affect the application as DOSCALLS and DOSCALL1 and virtually the
same to the application even though it is possible to import APIs from both
simultaneously.
:p.Currently OS/2 API Trace only traces DOSCALLS APIs because the standard
32-bit OS/2 library, OS2386.LIB, does not contain any references to DOSCALL1.
:dt.:hp2.Q&colon.:ehp2.
:dd.Is there a means for requesting that special functionality be added to OS/2
API Trace?
:dt.:hp2.A&colon.:ehp2.
:dd.Yes, the author has gotten so many requests for adding new funtionality,
such as dumping the stack at DosExit and tracing the chain of callers at API
entry, that he came up with a generic solution that allows the user to "do it
yourself."
So, as of version 2.45.32, OS2TRACE's -U option and PMOS2TRC's "User hook" item
on the "Customize" submenu accept a user-provided procedure, also known as a
user hook, that is called under the following circumstances:
:ul compact.
:li.Initialization
:li.Termination
:li.API Entry
:li.API Exit
:eul.
:p.For API entries and exits, the user hook is passed the name of the API and a
pointer to the API's parameters.
For API exits, the user hook is also passed a pointer to the API's return code.
With this information available to the hook, it can easily examine or even
modify information passed to or returned from any API being traced.
This makes the user hook a very powerful tool available to the user.
:p.A source code example of a user hook DLL can be found in the HOOKDLL
directory.
:p.If the special functionality cannot be accomplished through use of this user
hook capability, please feel free to contact the author at any of the below
IDs.
:dt.:hp2.Q&colon.:ehp2.
:dd.Is there a means for reporting OS/2 API Trace bugs?
:dt.:hp2.A&colon.:ehp2.
:dd.Yes!
Please send all problem reports to the author, Dave Blaschke, at
deblaschke@yahoo.com.  Comments, questions, and suggestions are also welcome.
:edl.
.*
.* PNL_DLG_CUSTALT
.*
:h1 res=18002.Help for Alternative Directory
:p.Select :hp2.Log to default directory:ehp2. to log all trace information to
the default directory, which is the same directory as the .EXE/.COM file.
Select :hp2.Log to s directory:ehp2. to log all trace information to an
alternative directory, using the entry field to enter the fully qualified
drive and path of the directory.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=18010.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=18012.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=18014.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=18016.
:dd.Undo
:elink.
:edl.
:p.For more information on alternative directory, select :hp2.More on
alternative directory:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=303.
:dd.More on alternative directory
:elink.
:edl.
.*
.* HLP_ALTOK
.*
:h1 res=18010.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the alternative directory
settings and exit this window.
.*
.* HLP_ALTCANCEL
.*
:h1 res=18012.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the alternative directory settings.
.*
.* HLP_ALTDEFAULT
.*
:h1 res=18014.Help for Default
:p.Select :hp2.Default:ehp2. to change the alternative directory settings to
those that were active when the OS/2 API Trace application was first installed.
.*
.* HLP_ALTUNDO
.*
:h1 res=18016.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the alternative directory settings to
those that were active before this window was displayed.
.*
.* PNL_DLG_CUSTBUF
.*
:h1 res=1002.Help for Buffer Length
:p.Select :hp2.Log ALL bytes from buffers:ehp2. to log all level three trace
information from buffers.
Select :hp2.Log maximum of n from buffers:ehp2. to log a maximum number of
bytes of level three trace information from buffers, using the spin buttons to
choose the maximum buffer length.
If the right spin button is "bytes", the values on the left spin button
represent bytes and are multiples of 16 between 16 and 65536, inclusive.
If the right spin button is "KB", the values on the left spin button represent
kilobytes and are between 1 and 64, inclusive.
:note.When changing from bytes to kilobytes, the maximum buffer length is
rounded up to the nearest kilobyte.
:p.If a buffer is larger than the maximum, the data is truncated and the
following is logged:
:xmp.
  -- More --
:exmp.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=1012.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=1014.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=1016.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=1018.
:dd.Undo
:elink.
:edl.
:p.For more information on buffer length, select :hp2.More on buffer length
:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=305.
:dd.More on buffer length
:elink.
:edl.
.*
.* HLP_BUFOK
.*
:h1 res=1012.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the buffer length settings and
exit this window.
.*
.* HLP_BUFCANCEL
.*
:h1 res=1014.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the buffer length settings.
.*
.* HLP_BUFDEFAULT
.*
:h1 res=1016.Help for Default
:p.Select :hp2.Default:ehp2. to change the buffer length settings to those that
were active when the OS/2 API Trace application was first installed.
.*
.* HLP_BUFUNDO
.*
:h1 res=1018.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the buffer length settings to those that
were active before this window was displayed.
.*
.* PNL_DLG_CUSTCTL
.*
:h1 res=16002.Help for Tracing Control
:p.Select :hp2.Enable tracing control (pause/resume):ehp2. to enable tracing
control, the ability to pause and resume tracing through OS2TRACE's -PAUSE
and -RESUME options and PMOS2TRC's "Pause tracing" and "Resume tracing" items
on the "Control" submenu.
When tracing control is enabled, all trace-enabled applications started before
tracing control is disabled can have their tracing paused and resumed both
before and during their execution.
:p.Select :hp2.Disable tracing control (pause/resume):ehp2. to disable tracing
control.
When tracing control is disabled, all trace-enabled applications started before
tracing control is enabled cannot have their tracing paused and resumed either
before and during their execution.
:note.When tracing control is enabled, a thread is created within the
application being traced for the sole purpose of monitoring pause and resume
requests.
If this additional thread is not desired for performance or other reasons,
tracing control should be disabled.

:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=16009.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=16011.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=16013.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=16015.
:dd.Undo
:elink.
:edl.
:p.For more information on tracing control, select :hp2.More on tracing
control:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=307.
:dd.More on tracing control
:elink.
:edl.
.*
.* HLP_CTLOK
.*
:h1 res=16009.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the tracing control settings
and exit this window.
.*
.* HLP_CTLCANCEL
.*
:h1 res=16011.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the tracing control settings.
.*
.* HLP_CTLDEFAULT
.*
:h1 res=16013.Help for Default
:p.Select :hp2.Default:ehp2. to change the tracing control settings to those
that were active when the OS/2 API Trace application was first installed.
.*
.* HLP_CTLUNDO
.*
:h1 res=16015.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the tracing control settings to those that
were active before this window was displayed.
.*
.* PNL_DLG_CUSTDOS
.*
:h1 res=2002.Help for DOSCALLS APIs
:p.Select one or more groups of APIs to trace when DOSCALLS.DLL is being
traced.
When a check box is selected, the APIs within the corresponding API group are
traced only when DOSCALLS.DLL is enabled for tracing.
When a check box is not selected, the APIs within the corresponding API group
are not traced even when DOSCALLS.DLL is enabled for tracing.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=2005.
:dd.DEV
:elink.
:dt.o
:link reftype=hd res=2007.
:dd.FILE
:elink.
:dt.o
:link reftype=hd res=2009.
:dd.INFO
:elink.
:dt.o
:link reftype=hd res=2011.
:dd.MEM
:elink.
:dt.o
:link reftype=hd res=2013.
:dd.MISC
:elink.
:dt.o
:link reftype=hd res=2015.
:dd.MOD
:elink.
:dt.o
:link reftype=hd res=2017.
:dd.MSG
:elink.
:dt.o
:link reftype=hd res=2019.
:dd.MVDM
:elink.
:dt.o
:link reftype=hd res=2021.
:dd.NLS
:elink.
:dt.o
:link reftype=hd res=2023.
:dd.PIPE
:elink.
:dt.o
:link reftype=hd res=2025.
:dd.PRF
:elink.
:dt.o
:link reftype=hd res=2027.
:dd.PROC
:elink.
:dt.o
:link reftype=hd res=2029.
:dd.PROF
:elink.
:dt.o
:link reftype=hd res=2031.
:dd.RES
:elink.
:dt.o
:link reftype=hd res=2033.
:dd.SEM
:elink.
:dt.o
:link reftype=hd res=2035.
:dd.SES
:elink.
:dt.o
:link reftype=hd res=2037.
:dd.SIG
:elink.
:dt.o
:link reftype=hd res=2039.
:dd.SMP
:elink.
:dt.o
:link reftype=hd res=2041.
:dd.TIME
:elink.
:dt.o
:link reftype=hd res=2043.
:dd.XCPT
:elink.
:dt.o
:link reftype=hd res=2045.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=2047.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=2049.
:dd.Clear
:elink.
:dt.o
:link reftype=hd res=2051.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=2053.
:dd.Undo
:elink.
:edl.
:p.For more information on DOSCALLS APIs, select :hp2.More on DOSCALLS APIs
:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=309.
:dd.More on DOSCALLS APIs
:elink.
:edl.
.*
.* HLP_DOSDEVCHK
.*
:h1 res=2005.Help for DEV
:i2 refid=custdos.Device
:p.Select :hp2.DEV:ehp2. to trace APIs within the DOSCALLS device API group.
These APIs are included from BSEDOS.H by defining INCL_DOSDEVICES (device
specific, ring 2 support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16Callback
:li.Dos16CLIAccess
:li.Dos16DevConfig
:li.Dos16DevIOCtl
:li.Dos16DevIOCtl2
:li.Dos16PhysicalDisk
:li.Dos16PortAccess
:li.Dos16R2StackRealloc
:li.Dos16SystemService
:li.Dos32DevConfig
:li.Dos32DevIOCtl
:li.Dos32PhysicalDisk
.*POWERPC :li.Dos32SystemService (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SystemService (:link reftype=hd res=245.OS/2 2.45+:elink.)
:eul.
.*
.* HLP_DOSFILECHK
.*
:h1 res=2007.Help for FILE
:i2 refid=custdos.File
:p.Select :hp2.FILE:ehp2. to trace APIs within the DOSCALLS file API group.
These APIs are included from BSEDOS.H by defining INCL_DOSFILEMGR (file
management).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16BufReset
:li.Dos16ChDir
:li.Dos16ChgFilePtr
:li.Dos16Close
:li.Dos16Copy
:li.Dos16Delete
:li.Dos16DupHandle
:li.Dos16EditName
:li.Dos16EnumAttribute
:li.Dos16FileIO
:li.Dos16FileLocks
:li.Dos16FindClose
:li.Dos16FindFirst
:li.Dos16FindFirst2
:li.Dos16FindNext
:li.Dos16ForceDelete
:li.Dos16FSAttach
:li.Dos16FSCtl
:li.Dos16MkDir
:li.Dos16MkDir2
:li.Dos16Move
:li.Dos16NewSize
:li.Dos16Open
:li.Dos16Open2
:li.Dos16ProtectChgFilePtr (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectClose (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectEnumAttribute (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectFileIO (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectFileLocks (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectNewSize (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectOpen (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectQFHandState (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectQFileInfo (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectSetFHandState (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectSetFileInfo (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16QCurDir
:li.Dos16QCurDisk
:li.Dos16QFHandState
:li.Dos16QFileInfo
:li.Dos16QFileMode
:li.Dos16QFSAttach
:li.Dos16QFSInfo
:li.Dos16QHandType
:li.Dos16QPathInfo
:li.Dos16QVerify
:li.Dos16Read
:li.Dos16ReadAsync
:li.Dos16RmDir
:li.Dos16SelectDisk
:li.Dos16SetFHandState
:li.Dos16SetFileInfo
:li.Dos16SetFileMode
:li.Dos16SetFSInfo
:li.Dos16SetMaxFH
:li.Dos16SetPathInfo
:li.Dos16SetVerify
:li.Dos16Shutdown
:li.Dos16Write
:li.Dos16WriteAsync
:li.Dos32CancelLockRequest
.*POWERPC:li.Dos32CancelLockRequestL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32CancelLockRequestL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32Close
:li.Dos32Copy
:li.Dos32CreateDir
:li.Dos32Delete
:li.Dos32DeleteDir
:li.Dos32DupHandle
:li.Dos32EditName
:li.Dos32EnumAttribute
:li.Dos32FindClose
:li.Dos32FindFirst
.*POWERPC :li.Dos32FindFromName (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32FindNext
:li.Dos32ForceDelete
:li.Dos32FSAttach
:li.Dos32FSCtl
.*POWERPC:li.Dos32ListIO (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ListIO (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC:li.Dos32ListIOL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ListIOL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32Move
:li.Dos32Open
.*POWERPC:li.Dos32OpenL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32OpenL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectClose (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectEnumAttribute (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectOpen (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32ProtectOpenL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ProtectOpenL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectQueryFHState (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectQueryFileInfo (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectRead (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectSetFHState (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectSetFileInfo (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectSetFileLocks (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32ProtectSetFileLocksL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ProtectSetFileLocksL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectSetFilePtr (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32ProtectSetFilePtrL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ProtectSetFilePtrL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectSetFileSize (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32ProtectSetFileSizeL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ProtectSetFileSizeL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectWrite (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32QueryCurrentDir
:li.Dos32QueryCurrentDisk
:li.Dos32QueryFHState
:li.Dos32QueryFileInfo
:li.Dos32QueryFSAttach
:li.Dos32QueryFSInfo
:li.Dos32QueryHType
:li.Dos32QueryPathInfo
:li.Dos32QueryVerify
:li.Dos32Read
:li.Dos32ResetBuffer
:li.Dos32SetCurrentDir
:li.Dos32SetDefaultDisk
:li.Dos32SetFHState
:li.Dos32SetFileInfo
:li.Dos32SetFileLocks
.*POWERPC :li.Dos32SetFileLocksL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetFileLocksL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32SetFilePtr
.*POWERPC :li.Dos32SetFilePtrL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetFilePtrL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32SetFileSize
.*POWERPC :li.Dos32SetFileSizeL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetFileSizeL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32SetFSInfo
:li.Dos32SetMaxFH
:li.Dos32SetPathInfo
:li.Dos32SetRelMaxFH
:li.Dos32SetVerify
:li.Dos32Shutdown
:li.Dos32Write
.*POWERPC :li.Temp32Write (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:eul.
.*
.* HLP_DOSINFOCHK
.*
:h1 res=2009.Help for INFO
:i2 refid=custdos.InfoSeg
:p.Select :hp2.INFO:ehp2. to trace APIs within the DOSCALLS InfoSeg API group.
These APIs are included from BSEDOS.H by defining INCL_DOSINFOSEG (InfoSeg
support).
.*POWERPC :note.Selecting this group is only effective on the Intel architecture, as the
.*POWERPC INFO API group contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following API comprises this group:
:ul compact.
:li.Dos16GetInfoSeg
:eul.
.*
.* HLP_DOSMEMCHK
.*
:h1 res=2011.Help for MEM
:i2 refid=custdos.Memory
:p.Select :hp2.MEM:ehp2. to trace APIs within the DOSCALLS memory API group.
These APIs are included from BSEDOS.H by defining INCL_DOSMEMMGR (memory
management).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16AllocHuge
:li.Dos16AllocSeg
:li.Dos16AllocShrSeg
:li.Dos16CreateCSAlias
:li.Dos16FreeSeg
:li.Dos16GetHugeShift
:li.Dos16GetSeg
:li.Dos16GetShrSeg
:li.Dos16GiveSeg
:li.Dos16LockSeg
:li.Dos16MemAvail
:li.Dos16ReallocHuge
:li.Dos16ReallocSeg
:li.Dos16SizeSeg
:li.Dos16SubAlloc
:li.Dos16SubFree
:li.Dos16SubSet
:li.Dos16UnlockSeg
.*POWERPC :li.Dos32AliasMem (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32AliasMem
:li.Dos32AllocMem
:li.Dos32AllocSharedMem
:li.Dos32FreeMem
:li.Dos32GetNamedSharedMem
:li.Dos32GetSharedMem
:li.Dos32GiveSharedMem
:li.Dos32QueryMem
.*POWERPC :li.Dos32QueryMemState (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryMemState
:li.Dos32SetMem
:li.Dos32SubAllocMem
:li.Dos32SubFreeMem
:li.Dos32SubSetMem
:li.Dos32SubUnsetMem
:eul.
.*
.* HLP_DOSMISCCHK
.*
:h1 res=2013.Help for MISC
:i2 refid=custdos.Miscellaneous
:p.Select :hp2.MISC:ehp2. to trace APIs within the DOSCALLS miscellaneous API
group.
These APIs are included from BSEDOS.H by defining INCL_DOSMISC (miscellaneous).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16DynamicTrace
:li.Dos16ErrClass
:li.Dos16Error
:li.Dos16GetEnv
:li.Dos16GetMachineMode
:li.Dos16GetVersion
:li.Dos16QSysInfo
:li.Dos16QueryExtLIBPATH (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ScanEnv
:li.Dos16SearchPath
:li.Dos16SetExtLIBPATH (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16SetVec
:li.Dos16SysTrace
.*POWERPC :li.Dos32DumpProcess (:link reftype=hd res=21.OS/2 2.10+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32DumpProcess (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32DynamicTrace (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32DynamicTrace
:li.Dos32ErrClass
:li.Dos32Error
:li.Dos32QueryExtLIBPATH (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Dos32QueryRASInfo (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryRASInfo
:li.Dos32QuerySysInfo
:li.Dos32ScanEnv
:li.Dos32SearchPath
:li.Dos32SetExtLIBPATH (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Dos32SuppressPopUps (:link reftype=hd res=21.OS/2 2.10+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SuppressPopUps (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32VerifyPidTid (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32VerifyPidTid (:link reftype=hd res=23.OS/2 2.30+:elink.)
:eul.
.*
.* HLP_DOSMODCHK
.*
:h1 res=2015.Help for MOD
:i2 refid=custdos.Module
:p.Select :hp2.MOD:ehp2. to trace APIs within the DOSCALLS module API group.
These APIs are included from BSEDOS.H by defining INCL_DOSMODULEMGR (module
manager).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16FreeModule
:li.Dos16GetModHandle
:li.Dos16GetModName
:li.Dos16GetProcAddr
:li.Dos16LoadModule
:li.Dos16QueryModFromCS
:li.Dos16QueryProcType
:li.Dos32FreeModule
.*POWERPC :li.Dos32GetLocaleModule (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32LoadModule
.*POWERPC :li.Dos32QueryABIOSSupport (:link reftype=hd res=21.OS/2 2.10+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryABIOSSupport (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32QueryModFromAddr (:link reftype=hd res=30.OS/2 3.00+:elink.,:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32QueryModFromEIP (:link reftype=hd res=21.OS/2 2.10+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryModFromEIP (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32QueryModuleHandle
:li.Dos32QueryModuleName
:li.Dos32QueryProcAddr
:li.Dos32QueryProcType
:li.Dos32ReplaceModule
:eul.
.*
.* HLP_DOSMSGCHK
.*
:h1 res=2017.Help for MSG
:i2 refid=custdos.Message
:p.Select :hp2.MSG:ehp2. to trace APIs within the DOSCALLS message API group.
:note.Selecting this group has no effect on the Intel architecture.
.*POWERPC :note.Selecting this group is only effective on the PowerPC architecture.
.*POWERPC To obtain the same functionality on the Intel architecture, enable tracing of
.*POWERPC MSG.DLL.
.*POWERPC :p.The following APIs comprise this group:
.*POWERPC :ul compact.
.*POWERPC :li.Dos32GetMessage (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32InsertMessage (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32PutMessage (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32QueryMessageCP (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :eul.
.*
.* HLP_DOSMVDMCHK
.*
:h1 res=2019.Help for MVDM
:i2 refid=custdos.MVDM
:p.Select :hp2.MVDM:ehp2. to trace APIs within the DOSCALLS MVDM API group.
These APIs are included from BSEDOS.H by defining INCL_DOSMVDM (MVDM support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16CloseVDD
:li.Dos16OpenVDD
:li.Dos16QueryDOSProperty
:li.Dos16RequestVDD
:li.Dos16SetDOSProperty
:li.Dos32CloseVDD
:li.Dos32OpenVDD
:li.Dos32QueryDOSProperty
:li.Dos32RequestVDD
:li.Dos32SetDOSProperty
:eul.
.*
.* HLP_DOSNLSCHK
.*
:h1 res=2021.Help for NLS
:i2 refid=custdos.National Language Support
:p.Select :hp2.NLS:ehp2. to trace APIs within the DOSCALLS national language
support API group.
These APIs are included from BSEDOS.H by defining INCL_DOSNLS (national
language support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16GetCp
:li.Dos16SetCp
:li.Dos16SetProcCp
.*POWERPC :li.Dos32QueryCp (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryCp
.*POWERPC :li.Dos32SetCp (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetCp
:li.Dos32SetProcessCp
:eul.
.*POWERPC :note.To trace Dos32QueryCp on the PowerPC architecture, enable tracing of
.*POWERPC NLS.DLL.
.*
.* HLP_DOSPIPECHK
.*
:h1 res=2023.Help for PIPE
:i2 refid=custdos.Pipe
:p.Select :hp2.PIPE:ehp2. to trace APIs within the DOSCALLS pipe API group.
These APIs are included from BSEDOS.H by defining INCL_DOSNMPIPES (named pipes
support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16MakePipe
:li.Dos32CallNPipe
:li.Dos32ConnectNPipe
:li.Dos32CreateNPipe
:li.Dos32CreatePipe
:li.Dos32DisConnectNPipe
:li.Dos32PeekNPipe
:li.Dos32QueryNPHState
:li.Dos32QueryNPipeInfo
:li.Dos32QueryNPipeSemState
.*POWERPC :li.Dos32RawReadNPipe (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32RawReadNPipe
.*POWERPC :li.Dos32RawWriteNPipe (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32RawWriteNPipe
:li.Dos32SetNPHState
:li.Dos32SetNPipeSem
:li.Dos32TransactNPipe
:li.Dos32WaitNPipe
:eul.
.*
.* HLP_DOSPRFCHK
.*
:h1 res=2025.Help for PRF
:i2 refid=custdos.Performance
:p.Select :hp2.PRF:ehp2. to trace APIs within the DOSCALLS performance APIs
group.
.*POWERPC :note.Selecting this group is only effective on the Intel architecture.
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16RegisterPerfCtrs
:li.Dos16TmrQueryFreq
:li.Dos16TmrQueryTime
.*POWERPC :li.Dos32AliasPerfCtrs (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32AliasPerfCtrs
.*POWERPC :li.Dos32ConfigurePerf (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ConfigurePerf
.*POWERPC :li.Dos32DeconPerf (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32DeconPerf
.*POWERPC :li.Dos32QuerySysState (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QuerySysState
.*POWERPC :li.Dos32RegisterPerfCtrs (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32RegisterPerfCtrs
.*POWERPC :li.Dos32TmrQueryFreq (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32TmrQueryFreq
.*POWERPC :li.Dos32TmrQueryTime (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32TmrQueryTime
:eul.
.*
.* HLP_DOSPROCCHK
.*
:h1 res=2027.Help for PROC
:i2 refid=custdos.Process and Thread
:p.Select :hp2.PROC:ehp2. to trace APIs within the DOSCALLS process and thread
API group.
These APIs are included from BSEDOS.H by defining INCL_DOSPROCESS (process and
thread support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16Beep
:li.Dos16CreateThread
:li.Dos16Cwait
:li.Dos16EnterCritSec
:li.Dos16ExecPgm
:li.Dos16Exit
:li.Dos16ExitCritSec
:li.Dos16ExitList
:li.Dos16GetPID
:li.Dos16GetPPID
:li.Dos16GetPrty
:li.Dos16KillProcess
:li.Dos16Ptrace
:li.Dos16ResumeThread
:li.Dos16SetPrty
:li.Dos16Sleep
:li.Dos16SuspendThread
.*POWERPC :li.Dos32AllocThreadLocalMemory (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32AllocThreadLocalMemory (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32Beep
:li.Dos32CreateThread
.*POWERPC:li.Dos32CreateThread2 (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32CreateThread2 (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32Debug
:li.Dos32EnterCritSec
:li.Dos32ExecPgm
:li.Dos32Exit
:li.Dos32ExitCritSec
:li.Dos32ExitList
.*POWERPC :li.Dos32FreeThreadLocalMemory (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32FreeThreadLocalMemory (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32GetInfoBlocks
:li.Dos32KillProcess
:li.Dos32KillThread
.*POWERPC :li.Dos32QueryProcessInfo (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32QueryThreadAffinity (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryThreadAffinity (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC :li.Dos32QueryThreadInfo (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32ResumeThread
:li.Dos32SetPriority
.*POWERPC :li.Dos32SetThreadAffinity (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetThreadAffinity (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32Sleep
:li.Dos32SuspendThread
:li.Dos32WaitChild
:li.Dos32WaitThread
:eul.
.*
.* HLP_DOSPROFCHK
.*
:h1 res=2029.Help for PROF
:i2 refid=custdos.DosProfile
:p.Select :hp2.PROF:ehp2. to trace Dos32Profile.
This API is included from BSEDOS.H by defining INCL_DOSPROFILE (DosProfile
API).
.*POWERPC :note.Selecting this group is only effective on the Intel architecture.
.*
.* HLP_DOSRESCHK
.*
:h1 res=2031.Help for RES
:i2 refid=custdos.Resource
:p.Select :hp2.RES:ehp2. to trace APIs within the DOSCALLS resource API group.
These APIs are included from BSEDOS.H by defining INCL_DOSRESOURCES (resource
support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16FreeResource
:li.Dos16GetResource
:li.Dos16GetResource2
:li.Dos16QueryResourceSize
:li.Dos32FreeResource
:li.Dos32GetResource
:li.Dos32QueryResourceSize
:eul.
.*
.* HLP_DOSSEMCHK
.*
:h1 res=2033.Help for SEM
:i2 refid=custdos.Semaphore
:p.Select :hp2.SEM:ehp2. to trace APIs within the DOSCALLS semaphore APIs
group.
These APIs are included from BSEDOS.H by defining INCL_DOSSEMAPHORES (semaphore
support).
:p.If the user requests to trace the DOSCALLS SEM API group and a semaphore API
removed by the Convenience Package for Warp Server for e-business is called,
information similar to the following is logged:
:xmp.
  < OS2TRACE: Returning n for API removed by WSeB Convenience Package >
:exmp.
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16CloseSem
:li.Dos16CreateSem
:li.Dos16FSRamSemClear
:li.Dos16FSRamSemClear2 (:link reftype=hd res=14.OS/2 2.45 to WSeB CP only:elink.)
:li.Dos16FSRamSemRequest
:li.Dos16FSRamSemRequest2 (:link reftype=hd res=14.OS/2 2.45 to WSeB CP only:elink.)
:li.Dos16MuxSemWait
:li.Dos16OpenSem
:li.Dos16SemClear
:li.Dos16SemRequest
:li.Dos16SemSet
:li.Dos16SemSetWait
:li.Dos16SemWait
:li.Dos32AddMuxWaitSem
:li.Dos32CloseEventSem
:li.Dos32CloseMutexSem
:li.Dos32CloseMuxWaitSem
.*POWERPC :li.Dos32CloseRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32CreateEventSem
:li.Dos32CreateMutexSem
:li.Dos32CreateMuxWaitSem
.*POWERPC :li.Dos32CreateRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32DeleteMuxWaitSem
:li.Dos32OpenEventSem
:li.Dos32OpenMutexSem
:li.Dos32OpenMuxWaitSem
.*POWERPC :li.Dos32OpenRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32PMPostEventSem (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMPostEventSem (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Dos32PMR3PostEventSem (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMR3PostEventSem (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC :li.Dos32PMR3WaitEventSem (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMR3WaitEventSem (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC :li.Dos32PMRequestMutexSem (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMRequestMutexSem (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Dos32PMWaitEventSem (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMWaitEventSem (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Dos32PMWaitMuxWaitSem (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMWaitMuxWaitSem (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32PostEventSem
:li.Dos32QueryEventSem
:li.Dos32QueryMutexSem
:li.Dos32QueryMuxWaitSem
:li.Dos32ReleaseMutexSem
.*POWERPC :li.Dos32ReleaseRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32RequestMutexSem
.*POWERPC :li.Dos32RequestRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32ResetEventSem
:li.Dos32WaitEventSem
:li.Dos32WaitMuxWaitSem
:eul.
.*
.* HLP_DOSSESCHK
.*
:h1 res=2035.Help for SES
:i2 refid=custdos.Session
:p.Select :hp2.SES:ehp2. to trace APIs within the DOSCALLS session API group.
These APIs are included from BSEDOS.H by defining INCL_DOSSESMGR (session
manager support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16QAppType
:li.Dos32QueryAppType
.*POWERPC :li.Dos32SelectSession (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32SetSession (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32StartSession (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32StopSession (:link reftype=hd res=12.PowerPC only:elink.)
:eul.
.*POWERPC :note.To trace Dos32SelectSession, Dos32SetSession, Dos32StartSession, and
.*POWERPC Dos32StopSession on the Intel architecture, enable tracing of SESMGR.DLL.
.*
.* HLP_DOSSIGCHK
.*
:h1 res=2037.Help for SIG
:i2 refid=custdos.Signals
:p.Select :hp2.SIG:ehp2. to trace APIs within the DOSCALLS signals API group.
These APIs are included from BSEDOS.H by defining INCL_DOSSIGNALS (signals).
.*POWERPC :note.Selecting this group is only effective on the Intel architecture, as the
.*POWERPC SIG API group contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16FlagProcess
:li.Dos16HoldSignal
:li.Dos16SendSignal
:li.Dos16SetSigHandler
:eul.
.*
.* HLP_DOSSMPCHK
.*
:h1 res=2039.Help for SMP
:i2 refid=custdos.SMP
:p.Select :hp2.SMP:ehp2. to trace APIs within the DOSCALLS SMP API group.
These APIs are included from BSEDOS.H by defining INCL_DOSSPINLOCK (SMP
support).
.*POWERPC :note.Selecting this group is only effective on the Intel architecture, as the
.*POWERPC SMP API group contains only SMP APIs and the PowerPC architecture does not
.*POWERPC support any SMP APIs.
:p.If the user requests to trace the DOSCALLS SMP API group and the version of
OS/2 is not the multiprocessing version, information similar to the following
is logged after each SMP API call:
:xmp.
  < OS2TRACE: Returning n for API not present on this version of OS/2 >
:exmp.
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16AcquireSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos16CreateSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos16FreeSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos16ReleaseSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32AcquireSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32CreateSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32FreeSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32GetProcessorStatus (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32PerfSysCall (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Dos32ReleaseSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32SetProcessorStatus (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32TestPSD (:link reftype=hd res=13.SMP only:elink.)
:eul.
.*
.* HLP_DOSTIMECHK
.*
:h1 res=2041.Help for TIME
:i2 refid=custdos.Date/time and Timer
:p.Select :hp2.TIME:ehp2. to trace APIs within the DOSCALLS date/time and timer
API group.
These APIs are included from BSEDOS.H by defining INCL_DOSDATETIME (date/time
and timer support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos16GetDateTime
:li.Dos16SetDateTime
:li.Dos16TimerAsync
:li.Dos16TimerStart
:li.Dos16TimerStop
:li.Dos32AsyncTimer
:li.Dos32GetDateTime
:li.Dos32SetDateTime
:li.Dos32StartTimer
:li.Dos32StopTimer
:eul.
.*
.* HLP_DOSXCPTCHK
.*
:h1 res=2043.Help for XCPT
:i2 refid=custdos.Exception
:p.Select :hp2.XCPT:ehp2. to trace APIs within the DOSCALLS exception APIs
group.
These APIs are included from BSEDOS.H by defining INCL_DOSEXCEPTIONS (exception
management support).
:p.The following APIs comprise this group:
:ul compact.
:li.Dos32AcknowledgeSignalException
:li.Dos32EnterMustComplete
:li.Dos32ExitMustComplete
.*POWERPC :li.Dos32QueryThreadContext (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryThreadContext (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Dos32RaiseException
:li.Dos32SendSignalException
:li.Dos32SetExceptionHandler
:li.Dos32SetSignalExceptionFocus
:li.Dos32UnwindException
:li.Dos32UnsetExceptionHandler
:eul.
.*
.* HLP_DOSOK
.*
:h1 res=2045.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the DOSCALLS APIs settings and
exit this window.
.*
.* HLP_DOSCANCEL
.*
:h1 res=2047.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the DOSCALLS APIs settings.
.*
.* HLP_DOSCLEAR
.*
:h1 res=2049.Help for Clear
:p.Select :hp2.Clear:ehp2. to deselect all DOSCALLS API groups in this window.
:note.At least one DOSCALLS API group must be selected before saving any
changes to the DOSCALLS APIs settings.
.*
.* HLP_DOSDEFAULT
.*
:h1 res=2051.Help for Default
:p.Select :hp2.Default:ehp2. to change the DOSCALLS APIs settings to those that
were active when the OS/2 API Trace application was first installed.
.*
.* HLP_DOSUNDO
.*
:h1 res=2053.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the DOSCALLS APIs settings to those that
were active before this window was displayed.
.*
.* PNL_DLG_CUSTEBC
.*
:h1 res=15002.Help for EBCDIC Translation
:p.Select :hp2.Enable EBCDIC translation of character buffers:ehp2. to log
ASCII and EBCDIC translation of character buffers.
When EBCDIC translation is enabled, information similar to the following is
logged for each character buffer:
:xmp.
  |                                                  |----ASCII-----|  |----EBCDIC----|
  | 57 69 6E 49 6E 69 74 69 61 6C 69 7A 65 20 72 65  WinInitialize re  ..>.>.../%.&colon.....
  | 74 75 72 6E 65 64 20 30 30 43 33 30 30 30 31 0D  turned 00C30001.  ...>............
:exmp.
:p.where the first string after the actual characters represents the ASCII
translation of the characters and the second string represents the EBCDIC
translation of the characters.
:p.Select :hp2.Disable EBCDIC translation of character buffers:ehp2. to log
only ASCII translation of character buffers.
When EBCDIC translation is disabled, information similar to the following is
logged for each character buffer:
:xmp.
  | 57 69 6E 49 6E 69 74 69 61 6C 69 7A 65 20 72 65  WinInitialize re
  | 74 75 72 6E 65 64 20 30 30 43 33 30 30 30 31 0D  turned 00C30001.
:exmp.
:p.where the only string after the actual characters represents the ASCII
translation of the characters.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=15009.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=15011.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=15013.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=15015.
:dd.Undo
:elink.
:edl.
:p.For more information on EBCDIC translation, select :hp2.More on EBCDIC
translation:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=311.
:dd.More on EBCDIC translation
:elink.
:edl.
.*
.* HLP_EBCOK
.*
:h1 res=15009.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the EBCDIC translation settings
and exit this window.
.*
.* HLP_EBCCANCEL
.*
:h1 res=15011.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the EBCDIC translation settings.
.*
.* HLP_EBCDEFAULT
.*
:h1 res=15013.Help for Default
:p.Select :hp2.Default:ehp2. to change the EBCDIC translation settings to those
that were active when the OS/2 API Trace application was first installed.
.*
.* HLP_EBCUNDO
.*
:h1 res=15015.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the EBCDIC translation settings to those
that were active before this window was displayed.
.*
.* PNL_DLG_CUSTFIL
.*
:h1 res=3002.Help for File Wrapping
:p.Select :hp2.Log ALL information without log file wrapping:ehp2. to log all
all trace information without log file wrapping (overwriting from the
beginning).
Select :hp2.Log maximum of n before log file wrapping occurs:ehp2. to log a
maximum number of bytes of trace information before log file wrapping occurs,
using the spin buttons to choose the maximum log file length.
If the right spin button is "bytes", the values on the left spin button
represent bytes and are multiples of 4096 between 4096 and 67108864, inclusive.
If the right spin button is "KB", the values on the left spin button represent
kilobytes and are multiples of 4 between 4 and 65536, inclusive.
If the right spin button is "MB", the values on the left spin button represent
megabytes and are between 1 and 64, inclusive.
:note.When changing from bytes to kilobytes, the maximum log file length is
rounded up to the nearest kilobyte.
Likewise, when changing from kilobytes to megabytes, the maximum log file
length is rounded up to the nearest megabyte.
:note.Tracing without file wrapping appends trace information to the end of any
preexisting trace information file, while tracing with file wrapping erases the
contents of any preexisting trace information file.
:note.Setting the log file wrapping length to anything other than ALL will
further impact the performance of the application being traced, as log file
wrapping requires the log file to be unbuffered, meaning that trace information
cannot be buffered in a memory cache before being written to disk.
:p.If log file wrapping occurs, information similar to the following is logged
after the executable's stopping time, OS2TRACE banner, and executable's
starting time:
:xmp.
  < OS2TRACE: Trace information lost due to log file wrapping n times >
:exmp.
:p.where n is the number of times the log file wrapped.
:p.If the length of an API's entry/exit information is greater than the log
file wrapping length, the following is logged in lieu of the entry/exit:
:xmp.
  < OS2TRACE: API entry/exit information lost because length (n) greater than log file wrapping length >
:exmp.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=3011.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=3013.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=3015.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=3017.
:dd.Undo
:elink.
:edl.
:p.For more information on file wrapping, select :hp2.More on file wrapping
:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=313.
:dd.More on file wrapping
:elink.
:edl.
.*
.* HLP_FILOK
.*
:h1 res=3011.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the file wrapping settings and
exit this window.
.*
.* HLP_FILCANCEL
.*
:h1 res=3013.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the file wrapping settings.
.*
.* HLP_FILDEFAULT
.*
:h1 res=3015.Help for Default
:p.Select :hp2.Default:ehp2. to change the file wrapping settings to those that
were active when the OS/2 API Trace application was first installed.
.*
.* HLP_FILUNDO
.*
:h1 res=3017.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the file wrapping settings to those that
were active before this window was displayed.
.*
.* PNL_DLG_CUSTGPI
.*
:h1 res=4002.Help for PMGPI APIs
:p.Select one or more groups of APIs to trace when PMGPI.DLL is enabled for
tracing.
When a check box is selected, the APIs within the corresponding API group are
traced only when PMGPI.DLL is enabled for tracing.
When a check box is not selected, the APIs within the corresponding API group
are not traced even when PMGPI.DLL is enabled for tracing.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=4005.
:dd.BIT
:elink.
:dt.o
:link reftype=hd res=4007.
:dd.CORR
:elink.
:dt.o
:link reftype=hd res=4009.
:dd.CTRL
:elink.
:dt.o
:link reftype=hd res=4011.
:dd.DEF
:elink.
:dt.o
:link reftype=hd res=4013.
:dd.DEV
:elink.
:dt.o
:link reftype=hd res=4015.
:dd.EDIT
:elink.
:dt.o
:link reftype=hd res=4017.
:dd.INK
:elink.
:dt.o
:link reftype=hd res=4019.
:dd.LCID
:elink.
:dt.o
:link reftype=hd res=4021.
:dd.LCT
:elink.
:dt.o
:link reftype=hd res=4023.
:dd.META
:elink.
:dt.o
:link reftype=hd res=4025.
:dd.PATH
:elink.
:dt.o
:link reftype=hd res=4027.
:dd.POLY
:elink.
:dt.o
:link reftype=hd res=4029.
:dd.PRIM
:elink.
:dt.o
:link reftype=hd res=4031.
:dd.RGN
:elink.
:dt.o
:link reftype=hd res=4033.
:dd.SEG
:elink.
:dt.o
:link reftype=hd res=4035.
:dd.TRAN
:elink.
:dt.o
:link reftype=hd res=4037.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=4039.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=4041.
:dd.Clear
:elink.
:dt.o
:link reftype=hd res=4043.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=4045.
:dd.Undo
:elink.
:edl.
:p.For more information on PMGPI APIs, select :hp2.More on PMGPI APIs:ehp2.
below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=315.
:dd.More on PMGPI APIs
:elink.
:edl.
.*
.* HLP_GPIBITCHK
.*
:h1 res=4005.Help for BIT
:i2 refid=custgpi.Bitmap
:p.Select :hp2.BIT:ehp2. to trace APIs within the PMGPI bitmap API group.
These APIs are included from PMGPI.H by defining INCL_GPIBITMAPS (bitmaps and
pel operations).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16BitBlt
:li.Gpi16CreateBitmap
:li.Gpi16DeleteBitmap
:li.Gpi16DrawBits
:li.Gpi16FloodFill
:li.Gpi16LoadBitmap *
:li.Gpi16QueryBitmapBits
:li.Gpi16QueryBitmapDimension
:li.Gpi16QueryBitmapHandle
:li.Gpi16QueryBitmapInfoHeader
:li.Gpi16QueryBitmapParameters
:li.Gpi16QueryDeviceBitmapFormats
:li.Gpi16QueryPel
:li.Gpi16SetBitmap
:li.Gpi16SetBitmapBits
:li.Gpi16SetBitmapDimension
:li.Gpi16SetBitmapId
:li.Gpi16SetPel
:li.Gpi16WCBitBlt
:li.Gpi32BitBlt
:li.Gpi32CreateBitmap
:li.Gpi32DeleteBitmap
:li.Gpi32DrawBits
:li.Gpi32FloodFill
:li.Gpi32LoadBitmap
:li.Gpi32QueryBitmapBits
:li.Gpi32QueryBitmapDimension
:li.Gpi32QueryBitmapHandle
:li.Gpi32QueryBitmapInfoHeader
:li.Gpi32QueryBitmapParameters
:li.Gpi32QueryDeviceBitmapFormats
:li.Gpi32QueryPel
:li.Gpi32SetBitmap
:li.Gpi32SetBitmapBits
:li.Gpi32SetBitmapDimension
:li.Gpi32SetBitmapId
:li.Gpi32SetPel
:li.Gpi32WCBitBlt
:eul.
:p.*
Although Gpi16LoadBitmap is part of the PMGPI bitmap API group, the API itself
physically resides within PMWIN.DLL.
As a result, PMWIN must be enabled for tracing AND the PMGPI bitmap API group
must be selected in order to trace Gpi16LoadBitmap.
.*
.* HLP_GPICORRCHK
.*
:h1 res=4007.Help for CORR
:i2 refid=custgpi.Correlation
:p.Select :hp2.CORR:ehp2. to trace APIs within the PMGPI correlation API group.
These APIs are included from PMGPI.H by defining INCL_GPICORRELATION (picking,
boundary and correlation).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16CorrelateChain
:li.Gpi16CorrelateFrom
:li.Gpi16CorrelateSegment
:li.Gpi16QueryBoundaryData
:li.Gpi16QueryPickAperturePosition
:li.Gpi16QueryPickApertureSize
:li.Gpi16QueryTag
:li.Gpi16ResetBoundaryData
:li.Gpi16SetPickAperturePosition
:li.Gpi16SetPickApertureSize
:li.Gpi16SetTag
:li.Gpi32CorrelateChain
:li.Gpi32CorrelateFrom
:li.Gpi32CorrelateSegment
:li.Gpi32QueryBoundaryData
:li.Gpi32QueryPickAperturePosition
:li.Gpi32QueryPickApertureSize
:li.Gpi32QueryTag
:li.Gpi32ResetBoundaryData
:li.Gpi32SetPickAperturePosition
:li.Gpi32SetPickApertureSize
:li.Gpi32SetTag
:eul.
.*
.* HLP_GPICTRLCHK
.*
:h1 res=4009.Help for CTRL
:i2 refid=custgpi.Control
:p.Select :hp2.CTRL:ehp2. to trace APIs within the PMGPI control API group.
These APIs are included from PMGPI.H by defining INCL_GPICONTROL (basic PS
control).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16Associate
:li.Gpi16CreatePS
:li.Gpi16DestroyPS
:li.Gpi16Erase
:li.Gpi16ErrorSegmentData
:li.Gpi16QueryDevice
:li.Gpi16QueryDrawControl
:li.Gpi16QueryDrawingMode
:li.Gpi16QueryPS
:li.Gpi16QueryStopDraw
:li.Gpi16ResetPS
:li.Gpi16RestorePS
:li.Gpi16SavePS
:li.Gpi16SetDrawControl
:li.Gpi16SetDrawingMode
:li.Gpi16SetPS
:li.Gpi16SetStopDraw
:li.Gpi32Associate
:li.Gpi32CreatePS
:li.Gpi32DestroyPS
:li.Gpi32Erase
:li.Gpi32ErrorSegmentData
:li.Gpi32QueryDevice
:li.Gpi32QueryDrawControl
:li.Gpi32QueryDrawingMode
:li.Gpi32QueryPS
:li.Gpi32QueryStopDraw
:li.Gpi32ResetPS
:li.Gpi32RestorePS
:li.Gpi32SavePS
:li.Gpi32SetDrawControl
:li.Gpi32SetDrawingMode
:li.Gpi32SetPS
:li.Gpi32SetStopDraw
:eul.
.*
.* HLP_GPIDEFCHK
.*
:h1 res=4011.Help for DEF
:i2 refid=custgpi.Defaults
:p.Select :hp2.DEF:ehp2. to trace APIs within the PMGPI defaults API group.
These APIs are included from PMGPI.H by defining INCL_GPIDEFAULTS (default
primitve attributes).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16QueryDefArcParams
:li.Gpi16QueryDefAttrs
:li.Gpi16QueryDefTag
:li.Gpi16QueryDefViewingLimits
:li.Gpi16SetDefArcParams
:li.Gpi16SetDefAttrs
:li.Gpi16SetDefTag
:li.Gpi16SetDefViewingLimits
:li.Gpi32QueryDefArcParams
:li.Gpi32QueryDefAttrs
:li.Gpi32QueryDefTag
:li.Gpi32QueryDefViewingLimits
:li.Gpi32SetDefArcParams
:li.Gpi32SetDefAttrs
:li.Gpi32SetDefTag
:li.Gpi32SetDefViewingLimits
:eul.
.*
.* HLP_GPIDEVCHK
.*
:h1 res=4013.Help for DEV
:i2 refid=custgpi.Device
:p.Select :hp2.DEV:ehp2. to trace APIs within the PMGPI device API group.
:p.The following APIs comprise this group:
:ul compact.
:li.Dev16CloseDC
:li.Dev16Escape
:li.Dev16OpenDC
:li.Dev16PostDeviceModes
:li.Dev16QueryCaps
:li.Dev16QueryDeviceNames
:li.Dev16QueryHardcopyCaps
:li.Dev16StdOpen (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dev32CloseDC
:li.Dev32Escape
:li.Dev32OpenDC
:li.Dev32PostDeviceModes
.*POWERPC :li.Dev32PostEscape (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dev32PostEscape (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Dev32QueryCaps
:li.Dev32QueryDeviceNames
:li.Dev32QueryHardcopyCaps
:li.Dev32StdOpen (:link reftype=hd res=21.OS/2 2.10+:elink.)
:eul.
.*
.* HLP_GPIEDITCHK
.*
:h1 res=4015.Help for EDIT
:i2 refid=custgpi.Segment Editing
:p.Select :hp2.EDIT:ehp2. to trace APIs within the PMGPI segment editing API
group.
These APIs are included from PMGPI.H by defining INCL_GPISEGEDITING (segment
editing via elements).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16BeginElement
:li.Gpi16DeleteElement
:li.Gpi16DeleteElementRange
:li.Gpi16DeleteElementsBetweenLabels
:li.Gpi16Element
:li.Gpi16EndElement
:li.Gpi16Label
:li.Gpi16OffsetElementPointer
:li.Gpi16QueryEditMode
:li.Gpi16QueryElement
:li.Gpi16QueryElementPointer
:li.Gpi16QueryElementType
:li.Gpi16SetEditMode
:li.Gpi16SetElementPointer
:li.Gpi16SetElementPointerAtLabel
:li.Gpi32BeginElement
:li.Gpi32DeleteElement
:li.Gpi32DeleteElementRange
:li.Gpi32DeleteElementsBetweenLabels
:li.Gpi32Element
:li.Gpi32EndElement
:li.Gpi32Label
:li.Gpi32OffsetElementPointer
:li.Gpi32QueryEditMode
:li.Gpi32QueryElement
:li.Gpi32QueryElementPointer
:li.Gpi32QueryElementType
:li.Gpi32SetEditMode
:li.Gpi32SetElementPointer
:li.Gpi32SetElementPointerAtLabel
:eul.
.*
.* HLP_GPIINKCHK
.*
:h1 res=4017.Help for INK
:i2 refid=custgpi.Inking
:p.Select :hp2.INK:ehp2. to trace APIs within the PMGPI inking API group.
These APIs are included from PMGPI.H by defining INCL_GPIINK (inking).
.*POWERPC :note.Selecting this group is only effective on the Intel architecture.
:p.The following APIs comprise this group:
:ul compact.
.*POWERPC :li.Gpi32BeginInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32BeginInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Gpi32EndInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32EndInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Gpi32StrokeInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32StrokeInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.)
:eul.
.*
.* HLP_GPILCIDCHK
.*
:h1 res=4019.Help for LCID
:i2 refid=custgpi.LCID
:p.Select :hp2.LCID:ehp2. to trace APIs within the PMGPI LCID API group.
These APIs are included from PMGPI.H by defining INCL_GPILCIDS (physical and
logical fonts with lcids).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16CreateLogFont
:li.Gpi16DeleteSetId
:li.Gpi16LoadFonts
:li.Gpi16LoadPublicFonts
:li.Gpi16QueryCp
:li.Gpi16QueryFaceString
:li.Gpi16QueryFontAction
:li.Gpi16QueryFontFileDescriptions
:li.Gpi16QueryFontMetrics
:li.Gpi16QueryFonts
:li.Gpi16QueryKerningPairs
:li.Gpi16QueryLogicalFont
:li.Gpi16QueryNumberSetIds
:li.Gpi16QuerySetIds
:li.Gpi16QueryWidthTable
:li.Gpi16SetCp
:li.Gpi16UnloadFonts
:li.Gpi16UnloadPublicFonts
:li.Gpi32CreateLogFont
:li.Gpi32DeleteSetId
:li.Gpi32LoadFonts
:li.Gpi32LoadPublicFonts
:li.Gpi32QueryCp
:li.Gpi32QueryFaceString
:li.Gpi32QueryFontAction
:li.Gpi32QueryFontFileDescriptions
.*POWERPC :li.Gpi32QueryFontMappingFlags (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryFontMappingFlags (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32QueryFontMetrics
:li.Gpi32QueryFonts
:li.Gpi32QueryFullFontFileDescs
:li.Gpi32QueryKerningPairs
:li.Gpi32QueryLogicalFont
:li.Gpi32QueryNumberSetIds
.*POWERPC :li.Gpi32QueryRasterizerCaps (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryRasterizerCaps (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32QuerySetIds
:li.Gpi32QueryWidthTable
:li.Gpi32SetCp
.*POWERPC :li.Gpi32SetFontMappingFlags (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32SetFontMappingFlags (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32UnloadFonts
:li.Gpi32UnloadPublicFonts
:eul.
.*
.* HLP_GPILCTCHK
.*
:h1 res=4021.Help for LCT
:i2 refid=custgpi.Logical Color Table
:p.Select :hp2.LCT:ehp2. to trace APIs within the PMGPI logical color table API
group.
These APIs are included from PMGPI.H by defining INCL_GPILOGCOLORTABLE (logical
color tables).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16AnimatePalette
:li.Gpi16CreateLogColorTable
:li.Gpi16CreatePalette
:li.Gpi16DeletePalette
:li.Gpi16QueryColorData
:li.Gpi16QueryColorIndex
:li.Gpi16QueryLogColorTable
:li.Gpi16QueryNearestColor
:li.Gpi16QueryPalette
:li.Gpi16QueryPaletteInfo
:li.Gpi16QueryRealColors
:li.Gpi16QueryRGBColor
:li.Gpi16RealizeColorTable
:li.Gpi16SelectPalette
:li.Gpi16SetPaletteEntries
:li.Gpi16UnrealizeColorTable
:li.Gpi32AnimatePalette
:li.Gpi32CreateLogColorTable
:li.Gpi32CreatePalette
:li.Gpi32DeletePalette
:li.Gpi32QueryColorData
:li.Gpi32QueryColorIndex
:li.Gpi32QueryLogColorTable
:li.Gpi32QueryNearestColor
.*POWERPC :li.Gpi32QueryNearestPaletteIndex (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryNearestPaletteIndex (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32QueryPalette
:li.Gpi32QueryPaletteInfo
:li.Gpi32QueryRealColors
:li.Gpi32QueryRGBColor
.*POWERPC :li.Gpi32ResizePalette (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32ResizePalette (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32SelectPalette
:li.Gpi32SetPaletteEntries
:eul.
.*
.* HLP_GPIMETACHK
.*
:h1 res=4023.Help for META
:i2 refid=custgpi.Metafile
:p.Select :hp2.META:ehp2. to trace APIs within the PMGPI metafile API group.
These APIs are included from PMGPI.H by defining INCL_GPIMETAFILES (metafiles).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16CopyMetaFile
:li.Gpi16DeleteMetaFile
:li.Gpi16LoadMetaFile
:li.Gpi16PlayMetaFile
:li.Gpi16QueryMetaFileBits
:li.Gpi16QueryMetaFileLength
:li.Gpi16ResumePlay
:li.Gpi16SaveMetaFile
:li.Gpi16SetMetaFileBits
:li.Gpi16SuspendPlay
:li.Gpi32CopyMetaFile
:li.Gpi32DeleteMetaFile
:li.Gpi32LoadMetaFile
:li.Gpi32PlayMetaFile
:li.Gpi32QueryMetaFileBits
:li.Gpi32QueryMetaFileLength
:li.Gpi32ResumePlay
:li.Gpi32SaveMetaFile
:li.Gpi32SetMetaFileBits
:li.Gpi32SuspendPlay
:eul.
.*
.* HLP_GPIPATHCHK
.*
:h1 res=4025.Help for PATH
:i2 refid=custgpi.Path
:p.Select :hp2.PATH:ehp2. to trace APIs within the PMGPI path API group.
These APIs are included from PMGPI.H by defining INCL_GPIPATHS (paths and
clipping with paths).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16BeginPath
:li.Gpi16CloseFigure
:li.Gpi16EndPath
:li.Gpi16FillPath
:li.Gpi16ModifyPath
:li.Gpi16OutlinePath
:li.Gpi16PathToRegion
:li.Gpi16SetClipPath
:li.Gpi16StrokePath
:li.Gpi32BeginPath
:li.Gpi32CloseFigure
:li.Gpi32EndPath
:li.Gpi32FillPath
:li.Gpi32ModifyPath
:li.Gpi32OutlinePath
:li.Gpi32PathToRegion
:li.Gpi32SetClipPath
:li.Gpi32StrokePath
:eul.
.*
.* HLP_GPIPOLYCHK
.*
:h1 res=4027.Help for POLY
:i2 refid=custgpi.Polygon
:p.Select :hp2.POLY:ehp2. to trace the API within the PMGPI polygon API group.
This API is included from PMGPI.H by defining INCL_GPIPOLYGON (polygons).
:p.The following APIs comprise this group:
:ul compact.
.*POWERPC :li.Gpi32CreatePolygonRegion (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32CreatePolygonRegion (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32Polygons
:eul.
.*
.* HLP_GPIPRIMCHK
.*
:h1 res=4029.Help for PRIM
:i2 refid=custgpi.Primitive
:p.Select :hp2.PRIM:ehp2. to trace APIs within the PMGPI primitive API group.
These APIs are included from PMGPI.H by defining INCL_GPIPRIMITIVES (drawing
primitives and primitive attributes).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16BeginArea
:li.Gpi16Box
:li.Gpi16CharString
:li.Gpi16CharStringAt
:li.Gpi16CharStringPos
:li.Gpi16CharStringPosAt
:li.Gpi16Comment
:li.Gpi16EndArea
:li.Gpi16FullArc
:li.Gpi16Image
:li.Gpi16Line
:li.Gpi16Marker
:li.Gpi16Move
:li.Gpi16PartialArc
:li.Gpi16PointArc
:li.Gpi16PolyFillet
:li.Gpi16PolyFilletSharp
:li.Gpi16PolyLine
:li.Gpi16PolyLineDisjoint
:li.Gpi16PolyMarker
:li.Gpi16PolySpline
:li.Gpi16Pop
:li.Gpi16PtVisible
:li.Gpi16QueryArcParams
:li.Gpi16QueryAttrMode
:li.Gpi16QueryAttrs
:li.Gpi16QueryBackColor
:li.Gpi16QueryBackMix
:li.Gpi16QueryCharAngle
:li.Gpi16QueryCharBox
:li.Gpi16QueryCharBreakExtra
:li.Gpi16QueryCharDirection
:li.Gpi16QueryCharExtra
:li.Gpi16QueryCharMode
:li.Gpi16QueryCharSet
:li.Gpi16QueryCharShear
:li.Gpi16QueryCharStringPos
:li.Gpi16QueryCharStringPosAt
:li.Gpi16QueryColor
:li.Gpi16QueryCurrentPosition
:li.Gpi16QueryDefCharBox
:li.Gpi16QueryLineEnd
:li.Gpi16QueryLineJoin
:li.Gpi16QueryLineType
:li.Gpi16QueryLineWidth
:li.Gpi16QueryLineWidthGeom
:li.Gpi16QueryMarker
:li.Gpi16QueryMarkerBox
:li.Gpi16QueryMarkerSet
:li.Gpi16QueryMix
:li.Gpi16QueryPattern
:li.Gpi16QueryPatternRefPoint
:li.Gpi16QueryPatternSet
:li.Gpi16QueryTextBox
:li.Gpi16RectVisible
:li.Gpi16SetArcParams
:li.Gpi16SetAttrMode
:li.Gpi16SetAttrs
:li.Gpi16SetBackColor
:li.Gpi16SetBackMix
:li.Gpi16SetCharAngle
:li.Gpi16SetCharBox
:li.Gpi16SetCharBreakExtra
:li.Gpi16SetCharDirection
:li.Gpi16SetCharExtra
:li.Gpi16SetCharMode
:li.Gpi16SetCharSet
:li.Gpi16SetCharShear
:li.Gpi16SetColor
:li.Gpi16SetCurrentPosition
:li.Gpi16SetLineEnd
:li.Gpi16SetLineJoin
:li.Gpi16SetLineType
:li.Gpi16SetLineWidth
:li.Gpi16SetLineWidthGeom
:li.Gpi16SetMarker
:li.Gpi16SetMarkerBox
:li.Gpi16SetMarkerSet
:li.Gpi16SetMix
:li.Gpi16SetPattern
:li.Gpi16SetPatternRefPoint
:li.Gpi16SetPatternSet
:li.Gpi32BeginArea
:li.Gpi32Box
:li.Gpi32CharString
:li.Gpi32CharStringAt
:li.Gpi32CharStringPos
:li.Gpi32CharStringPosAt
:li.Gpi32Comment
:li.Gpi32EndArea
:li.Gpi32FullArc
:li.Gpi32Image
:li.Gpi32Line
:li.Gpi32Marker
:li.Gpi32Move
.*POWERPC :li.Gpi32Oval (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32Oval (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Gpi32PartialArc
:li.Gpi32PointArc
:li.Gpi32PolyFillet
:li.Gpi32PolyFilletSharp
:li.Gpi32PolyLine
:li.Gpi32PolyLineDisjoint
:li.Gpi32PolyMarker
:li.Gpi32PolySpline
:li.Gpi32Pop
:li.Gpi32PtVisible
:li.Gpi32QueryArcParams
:li.Gpi32QueryAttrMode
:li.Gpi32QueryAttrs
:li.Gpi32QueryBackColor
:li.Gpi32QueryBackMix
:li.Gpi32QueryCharAngle
:li.Gpi32QueryCharBox
:li.Gpi32QueryCharBreakExtra
:li.Gpi32QueryCharDirection
:li.Gpi32QueryCharExtra
:li.Gpi32QueryCharMode
:li.Gpi32QueryCharSet
:li.Gpi32QueryCharShear
:li.Gpi32QueryCharStringPos
:li.Gpi32QueryCharStringPosAt
:li.Gpi32QueryColor
:li.Gpi32QueryCurrentPosition
:li.Gpi32QueryDefCharBox
:li.Gpi32QueryLineEnd
:li.Gpi32QueryLineJoin
:li.Gpi32QueryLineType
:li.Gpi32QueryLineWidth
:li.Gpi32QueryLineWidthGeom
:li.Gpi32QueryMarker
:li.Gpi32QueryMarkerBox
:li.Gpi32QueryMarkerSet
:li.Gpi32QueryMix
:li.Gpi32QueryPattern
:li.Gpi32QueryPatternRefPoint
:li.Gpi32QueryPatternSet
.*POWERPC :li.Gpi32QueryTabbedTextExtent (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryTabbedTextExtent (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32QueryTextAlignment
:li.Gpi32QueryTextBox
:li.Gpi32RectVisible
:li.Gpi32SetArcParams
:li.Gpi32SetAttrMode
:li.Gpi32SetAttrs
:li.Gpi32SetBackColor
:li.Gpi32SetBackMix
:li.Gpi32SetCharAngle
:li.Gpi32SetCharBox
:li.Gpi32SetCharBreakExtra
:li.Gpi32SetCharDirection
:li.Gpi32SetCharExtra
:li.Gpi32SetCharMode
:li.Gpi32SetCharSet
:li.Gpi32SetCharShear
:li.Gpi32SetColor
:li.Gpi32SetCurrentPosition
:li.Gpi32SetLineEnd
:li.Gpi32SetLineJoin
:li.Gpi32SetLineType
:li.Gpi32SetLineWidth
:li.Gpi32SetLineWidthGeom
:li.Gpi32SetMarker
:li.Gpi32SetMarkerBox
:li.Gpi32SetMarkerSet
:li.Gpi32SetMix
:li.Gpi32SetPattern
:li.Gpi32SetPatternRefPoint
:li.Gpi32SetPatternSet
:li.Gpi32SetTextAlignment
.*POWERPC :li.Gpi32TabbedCharStringAt (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32TabbedCharStringAt (:link reftype=hd res=24.OS/2 2.40+:elink.)
:eul.
.*
.* HLP_GPIRGNCHK
.*
:h1 res=4031.Help for RGN
:i2 refid=custgpi.Region
:p.Select :hp2.RGN:ehp2. to trace APIs within the PMGPI region API group.
These APIs are included from PMGPI.H by defining INCL_GPIREGIONS (regions and
clipping with regions).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16CombineRegion
:li.Gpi16CreateRegion
:li.Gpi16DestroyRegion
:li.Gpi16EqualRegion
:li.Gpi16ExcludeClipRectangle
:li.Gpi16FrameRegion
:li.Gpi16IntersectClipRectangle
:li.Gpi16OffsetClipRegion
:li.Gpi16OffsetRegion
:li.Gpi16PaintRegion
:li.Gpi16PtInRegion
:li.Gpi16QueryClipBox
:li.Gpi16QueryClipRegion
:li.Gpi16QueryRegionBox
:li.Gpi16QueryRegionRects
:li.Gpi16RectInRegion
:li.Gpi16SetClipRegion
:li.Gpi16SetRegion
:li.Gpi32CombineRegion
.*POWERPC :li.Gpi32CreateEllipticRegion (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32CreateEllipticRegion (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32CreateRegion
.*POWERPC :li.Gpi32CreateRoundRectRegion (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32CreateRoundRectRegion (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32DestroyRegion
:li.Gpi32EqualRegion
:li.Gpi32ExcludeClipRectangle
:li.Gpi32FrameRegion
:li.Gpi32IntersectClipRectangle
:li.Gpi32OffsetClipRegion
:li.Gpi32OffsetRegion
:li.Gpi32PaintRegion
:li.Gpi32PtInRegion
:li.Gpi32QueryClipBox
:li.Gpi32QueryClipRegion
:li.Gpi32QueryRegionBox
:li.Gpi32QueryRegionRects
:li.Gpi32RectInRegion
:li.Gpi32SetClipRegion
:li.Gpi32SetRegion
:eul.
.*
.* HLP_GPISEGCHK
.*
:h1 res=4033.Help for SEG
:i2 refid=custgpi.Segment
:p.Select :hp2.SEG:ehp2. to trace APIs within the PMGPI segment API group.
These APIs are included from PMGPI.H by defining INCL_GPISEGMENTS (segment
control and drawing).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16CloseSegment
:li.Gpi16DeleteSegment
:li.Gpi16DeleteSegments
:li.Gpi16DrawChain
:li.Gpi16DrawDynamics
:li.Gpi16DrawFrom
:li.Gpi16DrawSegment
:li.Gpi16GetData
:li.Gpi16OpenSegment
:li.Gpi16PutData
:li.Gpi16QueryInitialSegmentAttrs
:li.Gpi16QuerySegmentAttrs
:li.Gpi16QuerySegmentNames
:li.Gpi16QuerySegmentPriority
:li.Gpi16RemoveDynamics
:li.Gpi16SetInitialSegmentAttrs
:li.Gpi16SetSegmentAttrs
:li.Gpi16SetSegmentPriority
:li.Gpi32CloseSegment
:li.Gpi32DeleteSegment
:li.Gpi32DeleteSegments
:li.Gpi32DrawChain
:li.Gpi32DrawDynamics
:li.Gpi32DrawFrom
:li.Gpi32DrawSegment
:li.Gpi32GetData
:li.Gpi32OpenSegment
:li.Gpi32PutData
:li.Gpi32QueryInitialSegmentAttrs
:li.Gpi32QuerySegmentAttrs
:li.Gpi32QuerySegmentNames
:li.Gpi32QuerySegmentPriority
:li.Gpi32RemoveDynamics
:li.Gpi32SetInitialSegmentAttrs
:li.Gpi32SetSegmentAttrs
:li.Gpi32SetSegmentPriority
:eul.
.*
.* HLP_GPITRANCHK
.*
:h1 res=4035.Help for TRAN
:i2 refid=custgpi.Transform
:p.Select :hp2.TRAN:ehp2. to trace APIs within the PMGPI transform API group.
These APIs are included from PMGPI.H by defining INCL_GPITRANSFORMS (transform
and transform conversion).
:p.The following APIs comprise this group:
:ul compact.
:li.Gpi16CallSegmentMatrix
:li.Gpi16Convert
:li.Gpi16ConvertWithMatrix
:li.Gpi16QueryDefaultViewMatrix
:li.Gpi16QueryGraphicsField
:li.Gpi16QueryModelTransformMatrix
:li.Gpi16QueryPageViewport
:li.Gpi16QuerySegmentTransformMatrix
:li.Gpi16QueryViewingLimits
:li.Gpi16QueryViewingTransformMatrix
:li.Gpi16Rotate
:li.Gpi16Scale
:li.Gpi16SetDefaultViewMatrix
:li.Gpi16SetGraphicsField
:li.Gpi16SetModelTransformMatrix
:li.Gpi16SetPageViewport
:li.Gpi16SetSegmentTransformMatrix
:li.Gpi16SetViewingLimits
:li.Gpi16SetViewingTransformMatrix
:li.Gpi16Translate
:li.Gpi32CallSegmentMatrix
:li.Gpi32Convert
:li.Gpi32ConvertWithMatrix
:li.Gpi32QueryDefaultViewMatrix
:li.Gpi32QueryGraphicsField
:li.Gpi32QueryModelTransformMatrix
:li.Gpi32QueryPageViewport
:li.Gpi32QuerySegmentTransformMatrix
:li.Gpi32QueryViewingLimits
:li.Gpi32QueryViewingTransformMatrix
:li.Gpi32Rotate
:li.Gpi32Scale
:li.Gpi32SetDefaultViewMatrix
:li.Gpi32SetGraphicsField
:li.Gpi32SetModelTransformMatrix
:li.Gpi32SetPageViewport
:li.Gpi32SetSegmentTransformMatrix
:li.Gpi32SetViewingLimits
:li.Gpi32SetViewingTransformMatrix
:li.Gpi32Translate
:eul.
.*
.* HLP_GPIOK
.*
:h1 res=4037.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the PMGPI APIs settings and
exit this window.
.*
.* HLP_GPICANCEL
.*
:h1 res=4039.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the PMGPI APIs settings.
.*
.* HLP_GPICLEAR
.*
:h1 res=4041.Help for Clear
:p.Select :hp2.Clear:ehp2. to deselect all PMGPI API groups in this window.
:note.At least one PMGPI API group must be selected before saving any
changes to the PMGPI APIs settings.
.*
.* HLP_GPIDEFAULT
.*
:h1 res=4043.Help for Default
:p.Select :hp2.Default:ehp2. to change the PMGPI APIs settings to those that
were active when the OS/2 API Trace application was first installed.
.*
.* HLP_GPIUNDO
.*
:h1 res=4045.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the PMGPI APIs settings to those that
were active before this window was displayed.
.*
.* PNL_DLG_CUSTINT
.*
:h1 res=12002.Help for API Interception
:p.Select :hp2.Enable interception of dynamic API calls:ehp2. to intercept and
trace APIs loaded dynamically through use of Dos16GetProcAddr and
Dos32QueryProcAddr.
When API interception is enabled, information similar to the following is
logged after each Dos16GetProcAddr and/or Dos32QueryProcAddr call that loads an
API within a supported DLL that is enabled for tracing:
:xmp.
  < OS2TRACE: Intercepting ordinal n in module DLLNAME >
:exmp.
:note.Dos16GetProcAddr and Dos32QueryProcAddr do NOT have to be traced in order
to intercept dynamic API calls, but DOSCALLS must be enabled for tracing.
:p.Select :hp2.Disable interception of dynamic API calls:ehp2. to not intercept
and trace APIs loaded dynamically through use of Dos16GetProcAddr and
Dos32QueryProcAddr.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=12009.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=12011.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=12013.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=12015.
:dd.Undo
:elink.
:edl.
:p.For more information on API interception, select :hp2.More on API
interception:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=317.
:dd.More on API interception
:elink.
:edl.
.*
.* HLP_INTOK
.*
:h1 res=12009.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the API interception settings
and exit this window.
.*
.* HLP_INTCANCEL
.*
:h1 res=12011.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the API interception settings.
.*
.* HLP_INTDEFAULT
.*
:h1 res=12013.Help for Default
:p.Select :hp2.Default:ehp2. to change the API interception settings to those
that were active when the OS/2 API Trace application was first installed.
.*
.* HLP_INTUNDO
.*
:h1 res=12015.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the API interception settings to those
that were active before this window was displayed.
.*
.* PNL_DLG_CUSTLVL
.*
:h1 res=5002.Help for Logging Level
:p.Select :hp2.Log API entry/exit (level 1) information:ehp2. to log low
detailed (terse) trace information similar to the following:
:xmp.
  003A 0001 | Dos32CreateQueue Entry

  003A 0001 | Dos32CreateQueue Exit
       PASS | Return code&colon. 0
:exmp.
:p.Select :hp2.Log API parameters (level 2) information:ehp2. to log medium
detailed trace information similar to the following:
:xmp.
  003A 0001 | Dos32CreateQueue Entry
            | Parameter 1&colon. PHQUEUE = 0x00028BE8
            | Parameter 2&colon. ULONG = 0x00000002
            | Parameter 3&colon. PSZ = 0x000200A4

  003A 0001 | Dos32CreateQueue Exit
       PASS | Return code&colon. 0
            | Parameter 1&colon. PHQUEUE = 0x00028BE8
:exmp.
:p.Select :hp2.Log API parameter contents (level 3) information:ehp2. to log
high detailed (verbose) trace information similar to the following:
:xmp.
  003A 0001 | Dos32CreateQueue Entry, Return Address = 0x000136FD (TEST 0001&colon.000036FD)
            | Parameter 1&colon. PHQUEUE = 0x00028BE8
            | Parameter 2&colon. ULONG = 0x00000002
            | Parameter 3&colon. PSZ = 0x000200A4 ["\QUEUES\TEST.QUE"]

  003A 0001 | Dos32CreateQueue Exit
       PASS | Return code&colon. 0 (NO_ERROR)
            | Parameter 1&colon. PHQUEUE = 0x00028BE8 [0x00000007]
:exmp.
:note.The first number on both the API entry and exit lines is the hexadecimal
process identifier (PID) of the executable.
The second number is the hexadecimal thread identifier (TID) of the thread
within the executable that invoked the API.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=5010.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=5012.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=5014.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=5016.
:dd.Undo
:elink.
:edl.
:p.For more information on logging level, select :hp2.More on logging level
:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=319.
:dd.More on logging level
:elink.
:edl.
.*
.* HLP_LVLOK
.*
:h1 res=5010.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the logging level settings and
exit this window.
.*
.* HLP_LVLCANCEL
.*
:h1 res=5012.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the logging level settings.
.*
.* HLP_LVLDEFAULT
.*
:h1 res=5014.Help for Default
:p.Select :hp2.Default:ehp2. to change the logging level settings to those that
were active when the OS/2 API Trace application was first installed.
.*
.* HLP_LVLUNDO
.*
:h1 res=5016.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the logging level settings to those that
were active before this window was displayed.
.*
.* PNL_DLG_CUSTTIM
.*
:h1 res=6002.Help for Time Stamping
:p.Select :hp2.Enable time stamping of API entries and exits:ehp2. to time
stamp API entries and exits and log trace information similar to the following:
:xmp.
  00AC 0001 | Dos32Write Entry at 17&colon.46&colon.51.69

  00AC 0001 | Dos32Write Exit at 17&colon.46&colon.51.72
       PASS | Return code&colon. 0
:exmp.
:p.Select :hp2.Disable time stamping of API entries and exits:ehp2. to not
time stamp API entries and exits.
:note.API entry time stamps reflect entry into the trace API, not entry into
the actual OS/2 API.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=6009.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=6011.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=6013.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=6015.
:dd.Undo
:elink.
:edl.
:p.For more information on time stamping, select :hp2.More on time stamping
:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=321.
:dd.More on time stamping
:elink.
:edl.
.*
.* HLP_TIMOK
.*
:h1 res=6009.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the time stamping settings and
exit this window.
.*
.* HLP_TIMCANCEL
.*
:h1 res=6011.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the time stamping settings.
.*
.* HLP_TIMDEFAULT
.*
:h1 res=6013.Help for Default
:p.Select :hp2.Default:ehp2. to change the time stamping settings to those that
were active when the OS/2 API Trace application was first installed.
.*
.* HLP_TIMUNDO
.*
:h1 res=6015.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the time stamping settings to those that
were active before this window was displayed.
.*
.* PNL_DLG_CUSTUSR
.*
:h1 res=19002.Help for User Hook
:p.Select :hp2.Do not call user hook:ehp2. to not have a user-provided
procedure (user hook) called under certain circumstances.
Select :hp2.Call user hook:ehp2. to have a user hook called under certain
circumstances, using the entry field to enter the hook of the format
DLLNAME.HOOKNAME.
:note.The hook name is case sensitive, the DLL name is not.
:p.A user hook has the following syntax:
:xmp.
  PSZ APIENTRY UserHook( ULONG ulReason
                       , PSZ   pszAPIName
                       , PVOID pAPIParms
                       , PVOID pAPIRC
                       );
:exmp.
:p.A user hook is called under the following circumstances:
:table cols='13 12 20 20'.
:row.
:c.:hp1.ulReason:ehp1.
:c.:hp1.pszAPIName:ehp1.
:c.:hp1.pAPIParms:ehp1.
:c.:hp1.pAPIRC:ehp1.
:row.
:c.0 (init)
:c.NULL
:c.NULL
:c.NULL
:row.
:c.1 (term)
:c.NULL
:c.NULL
:c.NULL
:row.
:c.2 (API entry)
:c.API name
:c.Pointer to API parameters or NULL if none
:c.NULL
:row.
:c.3 (API exit)
:c.API name
:c.Pointer to API parameters or NULL if none
:c.Pointer to API return code or NULL if none
:etable.
:p.A user hook can return a pointer to a string that will be written to the
trace information file.  If a valid string is returned, information similar to
the following is logged:
:xmp.
  <+ DLLNAME.HOOKNAME(n): string +>
:exmp.
:note.Because a user hook is a 32-bit procedure, all parameters are 32-bit
aligned, even if they are parameters to a 16-bit API.
For example, when examining a USHORT parameter being passed to a 16-bit API,
the high word may not necessarily be zero and should be ignored.
:note.User hooks are only called for API entries and exits if the API is being
traced AND tracing is not paused.
:note.User hooks do not need to be reentrant.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=19009.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=19011.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=19013.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=19015.
:dd.Undo
:elink.
:edl.
:p.For more information on user hook, select :hp2.More on user hook:ehp2.
below.  For an example of a user hook, select :hp2.Example of user hook:ehp2.
below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=323.
:dd.More on user hook
:elink.
:dt.o
:link reftype=hd res=19017.
:dd.Example of user hook
:elink.
:edl.
.*
.* HLP_USROK
.*
:h1 res=19009.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the user hook settings and exit
this window.
.*
.* HLP_USRCANCEL
.*
:h1 res=19011.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the user hook settings.
.*
.* HLP_USRDEFAULT
.*
:h1 res=19013.Help for Default
:p.Select :hp2.Default:ehp2. to change the user hook settings to those that
were active when the OS/2 API Trace application was first installed.
.*
.* HLP_USRUNDO
.*
:h1 res=19015.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the user hook settings to those that were
active before this window was displayed.
.*
.* HLP_USREXAMPLE
.*
:h1 res=19017.Example of User Hook
:p.The following example provides a user hook, LogStackTrace, that traces the
stack upon entry to a 32-bit OS/2 API, logging the following items for each
chained call frame:
:ul compact.
:li.Return address
:li.Module name, object number and offset of return address
:li.First four parameters on stack
:eul.
:p.The C source file, USERDLL.C, contains the user hook (LogStackTrace) and two
additional procedures, one (AddrToMod) to convert a linear address to a module
name, object number and offset, and another (IsValidMem) to determine if a
linear address is valid.
The module definitions file, USERDLL.DEF, provides the necessary information to
the linker to satisfy C run-time initialization requirements for a DLL and
export LogStackTrace by name.
Finally, MAKEFILE provides the dependencies and rules for building USERDLL.DLL
under IBM VisualAge C++ for OS/2, Version 3.
:xmp.
:hp2.USERDLL.C -:ehp2.

#define INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 4096

CHAR               chBuffer[ BUFSIZE ];
HMODULE            hmodDOSCALLS;

APIRET (* APIENTRY pfnDOS32QUERYMODFROMEIP)( PHMODULE, PULONG, ULONG,
                                             PCHAR, PULONG, ULONG );

BOOL AddrToMod( PHMODULE, PULONG, ULONG, PCHAR, PULONG, ULONG );
BOOL IsValidMem( PVOID, ULONG );

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
      case 0&colon. /* Initialization */
         if ( DosLoadModule( chBuffer, sizeof( chBuffer ), "DOSCALLS",
                             &amp.hmodDOSCALLS ) ) {
            hmodDOSCALLS = 0;
         } else if ( DosQueryProcAddr( hmodDOSCALLS, 360, NULL,
                                       (PFN *)&amp.pfnDOS32QUERYMODFROMEIP ) ) {
            pfnDOS32QUERYMODFROMEIP = NULL;
         }

         sprintf( chBuffer, "Initialized, DosQueryModFromEIP %s available",
                  pfnDOS32QUERYMODFROMEIP == NULL ? "is not" &colon. "is" );

         break;

      case 1&colon. /* Termination */
         if ( hmodDOSCALLS ) {
            DosFreeModule( hmodDOSCALLS );
         }

         strcpy( chBuffer, "Terminated" );

         break;

      case 2&colon. /* API Entry */
         if ( ( pszAPIName[ 3 ] == '3' ) &amp.&amp. ( pszAPIName[ 4 ] == '2' ) ) {
            EBP = ((PULONG)&amp.ulReason) - 2; /* EBP -> LogStackTrace frame */
            i   = sprintf( chBuffer, "%s's 32-bit stack trace\n", pszAPIName );

            while ( ( EBP != NULL                        ) &amp.&amp.
                    ( IsValidMem( EBP, sizeof( ULONG ) ) ) &amp.&amp.
                    ( i < BUFSIZE - 256 )                ) {
               /* Add function address and name (if available) to buffer */
               if ( AddrToMod( &amp.ulModule, &amp.ulObjNum, sizeof( szModName ),
                               szModName, &amp.ulOffset, *(EBP+1) ) ) {
                  i += sprintf( &amp.chBuffer[ i ],
                                "0x%08lX (%s %04lX&colon.%08lX)",
                                *(EBP+1), szModName, ulObjNum, ulOffset );
               } else {
                  i += sprintf( &amp.chBuffer[ i ], "0x%08lX", *(EBP+1) );
               }

               /* Add four parameters (if available) to buffer */
               if ( IsValidMem( EBP + 2, 4 * sizeof( ULONG ) ) ) {
                  i += sprintf( &amp.chBuffer[ i ],
                                " [P1 = 0x%08lX, P2 = 0x%08lX,"
                                " P3 = 0x%08lX, P4 = 0x%08lX]\n",
                                *(EBP+2), *(EBP+3), *(EBP+4), *(EBP+5) );
               } else {
                  i += sprintf( &amp.chBuffer[ i ], "\n" );
               }

               EBP = (PULONG)*EBP;
            }
         } else if ( ( pszAPIName[ 3 ] == '1' ) &amp.&amp. ( pszAPIName[ 4 ] == '6' ) ) {
            sprintf( chBuffer, "%s is 16-bit API type", pszAPIName );
         } else {
            sprintf( chBuffer, "%s is unknown API type", pszAPIName );
         }

         break;

      case 3&colon. /* API Exit */
         break;
   }

   return chBuffer;
}

BOOL AddrToMod( PHMODULE pulModule              /* Module handle             */
              , PULONG   pulObject              /* Object number             */
              , ULONG    ulBufLen               /* Module name buffer length */
              , PCHAR    szBuffer               /* Module name buffer        */
              , PULONG   pulOffset              /* Offset                    */
              , ULONG    ulAddress              /* Address                   */
              )
{

   if ( ( pfnDOS32QUERYMODFROMEIP                                 ) &amp.&amp.
        ( !( *pfnDOS32QUERYMODFROMEIP )( pulModule, pulObject,
                                         ulBufLen, szBuffer,
                                         pulOffset, ulAddress ) ) ) {
      (*pulObject)++;
      return TRUE;
   } else {
      return FALSE;
   }

}

BOOL IsValidMem( PVOID pBase                    /* Memory base address       */
               , ULONG ulLength                 /* Memory length             */
               )
{

   ULONG ulActual = ulLength;
   ULONG ulFlags;

   return ( DosQueryMem( pBase, &amp.ulActual, &amp.ulFlags ) ) ||
          ( ulLength > ulActual                       ) ||
          ( !( ulFlags &amp. PAG_COMMIT )                 ) ? FALSE &colon. TRUE;
}

:hp2.USERDLL.DEF -:ehp2.

LIBRARY HOOKDLL INITINSTANCE TERMINSTANCE
DATA    MULTIPLE NONSHARED
EXPORTS LogStackTrace

:hp2.MAKEFILE -:ehp2.

hookdll.dll&colon. hookdll.obj hookdll.def makefile
        ilink hookdll.obj hookdll.def
hookdll.obj&colon. hookdll.c makefile
        icc -c -Ge- hookdll.c
:exmp.
.*
.* PNL_DLG_CUSTWIN
.*
:h1 res=7002.Help for PMWIN APIs
:p.Select one or more groups of APIs to trace when PMWIN.DLL is enabled for
tracing.
When a check box is selected, the APIs within the corresponding API group are
traced only when PMWIN.DLL is enabled for tracing.
When a check box is not selected, the APIs within the corresponding API group
are not traced even when PMWIN.DLL is enabled for tracing.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=7005.
:dd.ACCL
:elink.
:dt.o
:link reftype=hd res=7007.
:dd.ATOM
:elink.
:dt.o
:link reftype=hd res=7009.
:dd.CLIP
:elink.
:dt.o
:link reftype=hd res=7011.
:dd.CTRY
:elink.
:dt.o
:link reftype=hd res=7013.
:dd.CUR
:elink.
:dt.o
:link reftype=hd res=7015.
:dd.DDE
:elink.
:dt.o
:link reftype=hd res=7017.
:dd.DESK
:elink.
:dt.o
:link reftype=hd res=7019.
:dd.DLG
:elink.
:dt.o
:link reftype=hd res=7021.
:dd.DWIN
:elink.
:dt.o
:link reftype=hd res=7023.
:dd.ENV
:elink.
:dt.o
:link reftype=hd res=7025.
:dd.ERR
:elink.
:dt.o
:link reftype=hd res=7027.
:dd.FRAM
:elink.
:dt.o
:link reftype=hd res=7029.
:dd.HEAP
:elink.
:dt.o
:link reftype=hd res=7031.
:dd.HOOK
:elink.
:dt.o
:link reftype=hd res=7033.
:dd.INPT
:elink.
:dt.o
:link reftype=hd res=7035.
:dd.LOAD
:elink.
:dt.o
:link reftype=hd res=7037.
:dd.MENU
:elink.
:dt.o
:link reftype=hd res=7039.
:dd.MSG
:elink.
:dt.o
:link reftype=hd res=7041.
:dd.MSGL
:elink.
:dt.o
:link reftype=hd res=7043.
:dd.PAL
:elink.
:dt.o
:link reftype=hd res=7045.
:dd.PTR
:elink.
:dt.o
:link reftype=hd res=7047.
:dd.RECT
:elink.
:dt.o
:link reftype=hd res=7049.
:dd.SEI
:elink.
:dt.o
:link reftype=hd res=7051.
:dd.SYS
:elink.
:dt.o
:link reftype=hd res=7053.
:dd.THK
:elink.
:dt.o
:link reftype=hd res=7055.
:dd.TIME
:elink.
:dt.o
:link reftype=hd res=7057.
:dd.TREC
:elink.
:dt.o
:link reftype=hd res=7059.
:dd.WIN
:elink.
:dt.o
:link reftype=hd res=7061.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=7063.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=7065.
:dd.Clear
:elink.
:dt.o
:link reftype=hd res=7067.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=7069.
:dd.Undo
:elink.
:edl.
:p.For more information on PMWIN APIs, select :hp2.More on PMWIN APIs
:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=325.
:dd.More on PMWIN APIs
:elink.
:edl.
.*
.* HLP_WINACCLCHK
.*
:h1 res=7005.Help for ACCL
:i2 refid=custwin.Accelerator
:p.Select :hp2.ACCL:ehp2. to trace APIs within the PMWIN accelerator API group.
These APIs are included from PMWIN.H by defining INCL_WINACCELERATORS (keyboard
accelerators).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CopyAccelTable
:li.Win16CreateAccelTable
:li.Win16DestroyAccelTable
:li.Win16LoadAccelTable
:li.Win16QueryAccelTable
:li.Win16SetAccelTable
:li.Win16TranslateAccel
:li.Win32CopyAccelTable
:li.Win32CreateAccelTable
:li.Win32DestroyAccelTable
:li.Win32LoadAccelTable
:li.Win32QueryAccelTable
:li.Win32SetAccelTable
:li.Win32TranslateAccel
:eul.
.*
.* HLP_WINATOMCHK
.*
:h1 res=7007.Help for ATOM
:i2 refid=custwin.Atom
:p.Select :hp2.ATOM:ehp2. to trace APIs within the PMWIN atom API group.
These APIs are included from PMWIN.H by defining INCL_WINATOM (atom manager).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16AddAtom
:li.Win16CreateAtomTable
:li.Win16DeleteAtom
:li.Win16DestroyAtomTable
:li.Win16FindAtom
:li.Win16QueryAtomLength
:li.Win16QueryAtomName
:li.Win16QueryAtomUsage
:li.Win16QuerySystemAtomTable
:li.Win32AddAtom
:li.Win32CreateAtomTable
:li.Win32DeleteAtom
:li.Win32DestroyAtomTable
:li.Win32FindAtom
:li.Win32QueryAtomLength
:li.Win32QueryAtomName
:li.Win32QueryAtomUsage
:li.Win32QuerySystemAtomTable
:eul.
.*
.* HLP_WINCLIPCHK
.*
:h1 res=7009.Help for CLIP
:i2 refid=custwin.Clipboard
:p.Select :hp2.CLIP:ehp2. to trace APIs within the PMWIN clipboard API group.
These APIs are included from PMWIN.H by defining INCL_WINCLIPBOARD (clipboard
manager).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CloseClipbrd
:li.Win16EmptyClipbrd
:li.Win16EnumClipbrdFmts
:li.Win16OpenClipbrd
:li.Win16QueryClipbrdData
:li.Win16QueryClipbrdFmtInfo
:li.Win16QueryClipbrdOwner
:li.Win16QueryClipbrdViewer
:li.Win16SetClipbrdData
:li.Win16SetClipbrdOwner
:li.Win16SetClipbrdViewer
:li.Win32CloseClipbrd
:li.Win32EmptyClipbrd
:li.Win32EnumClipbrdFmts
:li.Win32OpenClipbrd
:li.Win32QueryClipbrdData
:li.Win32QueryClipbrdFmtInfo
:li.Win32QueryClipbrdOwner
:li.Win32QueryClipbrdViewer
:li.Win32SetClipbrdData
:li.Win32SetClipbrdOwner
:li.Win32SetClipbrdViewer
:eul.
.*
.* HLP_WINCTRYCHK
.*
:h1 res=7011.Help for CTRY
:i2 refid=custwin.Country
:p.Select :hp2.CTRY:ehp2. to trace APIs within the PMWIN country API group.
These APIs are included from PMWIN.H by defining INCL_WINCOUNTRY (country
support).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CompareStrings
:li.Win16CpTranslateChar
:li.Win16CpTranslateString
:li.Win16NextChar
:li.Win16PrevChar
:li.Win16QueryCp
:li.Win16QueryCpList
:li.Win16SetCp
:li.Win16Upper
:li.Win16UpperChar
:li.Win32CompareStrings
:li.Win32CpTranslateChar
:li.Win32CpTranslateString
:li.Win32NextChar
:li.Win32PrevChar
:li.Win32QueryCp
:li.Win32QueryCpList
:li.Win32SetCp
:li.Win32Upper
:li.Win32UpperChar
:eul.
.*
.* HLP_WINCURCHK
.*
:h1 res=7013.Help for CUR
:i2 refid=custwin.Cursor
:p.Select :hp2.CUR:ehp2. to trace APIs within the PMWIN cursor API group.
These APIs are included from PMWIN.H by defining INCL_WINCURSORS (text
cursors).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CreateCursor
:li.Win16DestroyCursor
:li.Win16QueryCursorInfo
:li.Win16ShowCursor
:li.Win32CreateCursor
:li.Win32DestroyCursor
:li.Win32QueryCursorInfo
:li.Win32ShowCursor
:eul.
.*
.* HLP_WINDDETCHK
.*
:h1 res=7015.Help for DDE
:i2 refid=custwin.DDE
:p.Select :hp2.DDE:ehp2. to trace APIs within the PMWIN DDE API group.
These APIs are included from PMWIN.H by defining INCL_WINDDE (dynamic data
exchange).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16DdeInitiate
:li.Win16DdePostMsg
:li.Win16DdeRespond
:li.Win32DdeInitiate
:li.Win32DdePostMsg
:li.Win32DdeRespond
:eul.
.*
.* HLP_WINDESKCHK
.*
:h1 res=7017.Help for DESK
:i2 refid=custwin.Desktop
:p.Select :hp2.DESK:ehp2. to trace APIs within the PMWIN desktop API group.
These APIs are included from PMWIN.H by defining INCL_WINDESKTOP (desktop API).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16QueryDesktopBkgnd
:li.Win16SetDesktopBkgnd
:li.Win32QueryDesktopBkgnd
:li.Win32SetDesktopBkgnd
:eul.
.*
.* HLP_WINDLGCHK
.*
:h1 res=7019.Help for DLG
:i2 refid=custwin.Dialog
:p.Select :hp2.DLG:ehp2. to trace APIs within the PMWIN dialog API group.
These APIs are included from PMWIN.H by defining INCL_WINDIALOGS (dialog
boxes).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16Alarm
:li.Win16CreateDlg
:li.Win16DefDlgProc
:li.Win16DismissDlg
:li.Win16DlgBox
:li.Win16EnumDlgItem
:li.Win16GetDlgMsg
:li.Win16LoadDlg
:li.Win16MapDlgPoints
:li.Win16MessageBox
:li.Win16ProcessDlg
:li.Win16QueryDlgItemShort
:li.Win16QueryDlgItemText
:li.Win16QueryDlgItemTextLength
:li.Win16SendDlgItemMsg
:li.Win16SetDlgItemShort
:li.Win16SetDlgItemText
:li.Win16SubstituteStrings
:li.Win32Alarm
:li.Win32CreateDlg
:li.Win32DefDlgProc
:li.Win32DismissDlg
:li.Win32DlgBox
:li.Win32EnumDlgItem
:li.Win32GetDlgMsg
:li.Win32LoadDlg
:li.Win32MapDlgPoints
:li.Win32MessageBox
.*POWERPC :li.Win32MessageBox2 (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32MessageBox2 (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32ProcessDlg
:li.Win32QueryDlgItemShort
:li.Win32QueryDlgItemText
:li.Win32QueryDlgItemTextLength
:li.Win32SendDlgItemMsg
:li.Win32SetDlgItemShort
:li.Win32SetDlgItemText
:li.Win32SubstituteStrings
:eul.
.*
.* HLP_WINDWINCHK
.*
:h1 res=7021.Help for DWIN
:i2 refid=custwin.WinDefWindowProc
:p.Select :hp2.DWIN:ehp2. to trace Win16DefWindowProc and Win32DefWindowProc.
:note.Tracing Win16DefWindowProc and Win32DefWindowProc is not recommended
unless absolutely necessary as they can severely affect the performance of a
trace-enabled executable file.
.*
.* HLP_WINENVCHK
.*
:h1 res=7023.Help for ENV
:i2 refid=custwin.WinCatch and WinThrow
:p.Select :hp2.ENV:ehp2. to trace Win16Catch and Win16Throw.
These APIs are included from PMWIN.H by defining INCL_WINCATCHTHROW
(WinCatch/WinThrow support).
.*POWERPC :note.Selecting this group is only effective on the Intel architecture, as the
.*POWERPC ENV API group contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs comprise this group:
:ul compact.
:li.Win16Catch
:li.Win16Throw
:eul.
.*
.* HLP_WINERRCHK
.*
:h1 res=7025.Help for ERR
:i2 refid=custwin.Error
:p.Select :hp2.ERR:ehp2. to trace APIs within the PMWIN error API group.
These APIs are included from PMWIN.H by defining INCL_WINERRORS (error code
definitions).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16FreeErrorInfo
:li.Win16GetErrorInfo
:li.Win16GetLastError
:li.Win32FreeErrorInfo
:li.Win32GetErrorInfo
:li.Win32GetLastError
:eul.
.*
.* HLP_WINFRAMCHK
.*
:h1 res=7027.Help for FRAM
:i2 refid=custwin.Frame
:p.Select :hp2.FRAM:ehp2. to trace APIs within the PMWIN frame API group.
These APIs are included from PMWIN.H by defining INCL_WINFRAMEMGR (frame
manager).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CalcFrameRect
:li.Win16CreateFrameControls
:li.Win16CreateStdWindow
:li.Win16FlashWindow
:li.Win16GetMaxPosition
:li.Win16GetMinPosition
:li.Win16SaveWindowPos
:li.Win32CalcFrameRect
:li.Win32CreateFrameControls
:li.Win32CreateStdWindow
:li.Win32FlashWindow
:li.Win32GetMaxPosition
:li.Win32GetMinPosition
:li.Win32SaveWindowPos
:eul.
.*
.* HLP_WINHEAPCHK
.*
:h1 res=7029.Help for HEAP
:i2 refid=custwin.Heap
:p.Select :hp2.HEAP:ehp2. to trace APIs within the PMWIN heap API group.
These APIs are included from PMWIN.H by defining INCL_WINHEAP (heap manager).
.*POWERPC :note.Selecting this group is only effective on the Intel architecture, as the
.*POWERPC HEAP API group contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs comprise this group:
:ul compact.
:li.Win16AllocMem
:li.Win16AvailMem
:li.Win16CreateHeap
:li.Win16DestroyHeap
:li.Win16FreeMem
:li.Win16LockHeap
:li.Win16ReallocMem
:eul.
.*
.* HLP_WINHOOKCHK
.*
:h1 res=7031.Help for HOOK
:i2 refid=custwin.Hook
:p.Select :hp2.HOOK:ehp2. to trace APIs within the PMWIN hook API group.
These APIs are included from PMWIN.H by defining INCL_WINHOOKS (hook manager).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CallMsgFilter
:li.Win16ReleaseHook
:li.Win16SetHook
:li.Win32CallMsgFilter
:li.Win32ReleaseHook
:li.Win32SetHook
:eul.
.*
.* HLP_WININPTCHK
.*
:h1 res=7033.Help for INPT
:i2 refid=custwin.Input
:p.Select :hp2.INPT:ehp2. to trace APIs within the PMWIN input API group.
These APIs are included from PMWIN.H by defining INCL_WININPUT (mouse and
keyboard input).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CheckInput (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win16EnablePhysInput
:li.Win16FocusChange
:li.Win16GetKeyState
:li.Win16GetPhysKeyState
:li.Win16IsPhysInputEnabled
:li.Win16QueryCapture
:li.Win16QueryFocus
:li.Win16SetCapture
:li.Win16SetFocus
:li.Win16SetKeyboardStateTable
:li.Win32CheckInput (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32EnablePhysInput
:li.Win32FocusChange
:li.Win32GetKeyState
:li.Win32GetPhysKeyState
:li.Win32IsPhysInputEnabled
:li.Win32QueryCapture
:li.Win32QueryFocus
:li.Win32SetCapture
:li.Win32SetFocus
:li.Win32SetKeyboardStateTable
:eul.
.*
.* HLP_WINLOADCHK
.*
:h1 res=7035.Help for LOAD
:i2 refid=custwin.Load
:p.Select :hp2.LOAD:ehp2. to trace APIs within the PMWIN load API group.
These APIs are included from PMWIN.H by defining INCL_WINLOAD (load/delete
library/procedure).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16DeleteLibrary
:li.Win16DeleteProcedure
:li.Win16LoadLibrary
:li.Win16LoadProcedure
:li.Win32DeleteLibrary
:li.Win32DeleteProcedure
:li.Win32LoadLibrary
:li.Win32LoadProcedure
:eul.
.*
.* HLP_WINMENUCHK
.*
:h1 res=7037.Help for MENU
:i2 refid=custwin.Menu
:p.Select :hp2.MENU:ehp2. to trace APIs within the PMWIN menu API group.
These APIs are included from PMWIN.H by defining INCL_WINMENUS (menu controls).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CreateMenu
:li.Win16LoadMenu
:li.Win16PopupMenu
:li.Win32CreateMenu
:li.Win32LoadMenu
:li.Win32PopupMenu
:eul.
.*
.* HLP_WINMSGCHK
.*
:h1 res=7039.Help for MSG
:i2 refid=custwin.Message
:p.Select :hp2.MSG:ehp2. to trace APIs within the PMWIN message API group.
These APIs are included from PMWIN.H by defining INCL_WINMESSAGEMGR (message
management).
:note.Even though WinDispatchMsg and WinGetMsg belong in the message API group,
they are included in a different group, :link reftype=hd res=7041.MSGL:elink.,
to address performance issues.
:p.The following APIs comprise this group:
:ul compact.
:li.Win16BroadcastMsg
:li.Win16CancelShutdown
:li.Win16CreateMsgQueue
:li.Win16DestroyMsgQueue
:li.Win16InSendMsg
:li.Win16LockInput
:li.Win16MsgMuxSemWait
:li.Win16MsgSemWait
:li.Win16PeekMsg
:li.Win16PostMsg
:li.Win16PostQueueMsg
:li.Win16QueryMsgPos
:li.Win16QueryMsgTime
:li.Win16QueryQueueInfo
:li.Win16QueryQueueStatus
:li.Win16QuerySendMsg
:li.Win16QueueFromID
:li.Win16RegisterUserDatatype
:li.Win16RegisterUserMsg
:li.Win16ReplyMsg
:li.Win16SendMsg
:li.Win16SetClassMsgInterest
:li.Win16SetMsgInterest
:li.Win16SetMsgMode
:li.Win16SetSynchroMode
:li.Win16ThreadAssocQueue
:li.Win16WaitMsg
:li.Win16WakeThread
:li.Win32BroadcastMsg
:li.Win32CancelShutdown
:li.Win32CreateMsgQueue
:li.Win32DestroyMsgQueue
:li.Win32InSendMsg
:li.Win32LockInput
:li.Win32MsgMuxSemWait
:li.Win32MsgSemWait
:li.Win32PeekMsg
:li.Win32PostMsg
:li.Win32PostQueueMsg
:li.Win32QueryMsgPos
:li.Win32QueryMsgTime
:li.Win32QueryQueueInfo
:li.Win32QueryQueueStatus
:li.Win32QuerySendMsg
:li.Win32QueueFromID
:li.Win32RegisterUserDatatype
:li.Win32RegisterUserMsg
:li.Win32ReplyMsg
:li.Win32RequestMutexSem
:li.Win32SendMsg
:li.Win32SetClassMsgInterest
:li.Win32SetMsgInterest
:li.Win32SetMsgMode
:li.Win32SetSynchroMode
:li.Win32ThreadAssocQueue
:li.Win32WaitEventSem
:li.Win32WaitMsg
:li.Win32WaitMuxWaitSem
:li.Win32WakeThread
:eul.
.*
.* HLP_WINMSGLCHK
.*
:h1 res=7041.Help for MSGL
:i2 refid=custwin.WinDispatchMsg and WinGetMsg
:p.Select :hp2.MSGL:ehp2. to trace Win16DispatchMsg, Win16GetMsg,
Win32DispatchMsg, and Win32GetMsg.
:note.Tracing Win16DispatchMsg, Win16GetMsg, Win32DispatchMsg, and Win32GetMsg
is not recommended unless absolutely necessary as they can severely affect the
performance of a trace-enabled executable file.
.*
.* HLP_WINPALCHK
.*
:h1 res=7043.Help for PAL
:i2 refid=custwin.Palette
:p.Select :hp2.PAL:ehp2. to trace the APIs within the PMWIN palette API group.
These APIs are included from PMWIN.H by defining INCL_WINPALETTE (palette
management).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16RealizePalette
:li.Win32RealizePalette
:eul.
.*
.* HLP_WINPTRCHK
.*
:h1 res=7045.Help for PTR
:i2 refid=custwin.Pointer
:p.Select :hp2.PTR:ehp2. to trace APIs within the PMWIN pointer API group.
These APIs are included from PMWIN.H by defining INCL_WINPOINTERS (mouse
pointers).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CreatePointer
:li.Win16CreatePointerIndirect
:li.Win16DestroyPointer
:li.Win16DrawPointer
:li.Win16GetSysBitmap
:li.Win16LoadPointer
:li.Win16LockPointerUpdate (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win16QueryPointer
:li.Win16QueryPointerInfo
:li.Win16QueryPointerPos
:li.Win16QuerySysPointer
:li.Win16SetPointer
:li.Win16SetPointerOwner
:li.Win16SetPointerPos
:li.Win16ShowPointer
:li.Win16StretchPointer
:li.Win32CreatePointer
:li.Win32CreatePointerIndirect
:li.Win32DestroyPointer
:li.Win32DrawPointer
:li.Win32GetSysBitmap
:li.Win32LoadPointer
:li.Win32LockPointerUpdate (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32QueryPointer
:li.Win32QueryPointerInfo
:li.Win32QueryPointerPos
:li.Win32QuerySysPointer
:li.Win32QuerySysPointerData (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32SetPointer
:li.Win32SetPointerOwner
:li.Win32SetPointerPos
:li.Win32SetSysPointerData (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32ShowPointer
:li.Win32StretchPointer
:eul.
.*
.* HLP_WINRECTCHK
.*
:h1 res=7047.Help for RECT
:i2 refid=custwin.Rectangle
:p.Select :hp2.RECT:ehp2. to trace APIs within the PMWIN rectangle API group.
These APIs are included from PMWIN.H by defining INCL_WINRECTANGLES
(rectangle routines).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16CopyRect
:li.Win16EqualRect
:li.Win16InflateRect
:li.Win16IntersectRect
:li.Win16IsRectEmpty
:li.Win16MakePoints
:li.Win16MakeRect
:li.Win16OffsetRect
:li.Win16PtInRect
:li.Win16SetRect
:li.Win16SetRectEmpty
:li.Win16SubtractRect
:li.Win16UnionRect
:li.Win32CopyRect
:li.Win32EqualRect
:li.Win32InflateRect
:li.Win32IntersectRect
:li.Win32IsRectEmpty
:li.Win32MakePoints
:li.Win32MakeRect
:li.Win32OffsetRect
:li.Win32PtInRect
:li.Win32SetRect
:li.Win32SetRectEmpty
:li.Win32SubtractRect
:li.Win32UnionRect
:eul.
.*
.* HLP_WINSEICHK
.*
:h1 res=7049.Help for SEI
:i2 refid=custwin.Set Error Info
:p.Select :hp2.SEI:ehp2. to trace APIs within the PMWIN set error info API
group.
These APIs are included from PMWIN.H by defining INCL_WINSEI (set error info
API).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16SetErrorInfo
:li.Win32SetErrorInfo
:eul.
.*
.* HLP_WINSYSCHK
.*
:h1 res=7051.Help for SYS
:i2 refid=custwin.System
:p.Select :hp2.SYS:ehp2. to trace APIs within the PMWIN system API group.
These APIs are included from PMWIN.H by defining INCL_WINSYS (system values and
colors).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16QueryPresParam
:li.Win16QuerySysColor
:li.Win16QuerySysValue
:li.Win16RemovePresParam
:li.Win16SetPresParam
:li.Win16SetSysColors
:li.Win16SetSysValue
:li.Win32QueryPresParam
:li.Win32QuerySysColor
:li.Win32QuerySysValue
:li.Win32RemovePresParam
:li.Win32SetPresParam
:li.Win32SetSysColors
:li.Win32SetSysValue
:eul.
.*
.* HLP_WINTHKCHK
.*
:h1 res=7053.Help for THK
:i2 refid=custwin.Thunk
:p.Select :hp2.THK:ehp2. to trace APIs within the PMWIN thunk API group.
These APIs are included from PMWIN.H by defining INCL_WINTHUNKAPI (thunk
procedure API).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16QueryClassThunkProc
:li.Win16QueryWindowModel
:li.Win16QueryWindowThunkProc
:li.Win16SetClassThunkProc
:li.Win16SetWindowThunkProc
:li.Win32QueryClassThunkProc
:li.Win32QueryWindowModel
:li.Win32QueryWindowThunkProc
:li.Win32SetClassThunkProc
:li.Win32SetWindowThunkProc
:eul.
.*
.* HLP_WINTIMECHK
.*
:h1 res=7055.Help for TIME
:i2 refid=custwin.Time
:p.Select :hp2.TIME:ehp2. to trace APIs within the PMWIN time API group.
These APIs are included from PMWIN.H by defining INCL_WINTIMER (timer
routines).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16GetCurrentTime
:li.Win16StartTimer
:li.Win16StopTimer
:li.Win32GetCurrentTime
:li.Win32StartTimer
:li.Win32StopTimer
:eul.
.*
.* HLP_WINTRECCHK
.*
:h1 res=7057.Help for TREC
:i2 refid=custwin.Track Rectangle
:p.Select :hp2.TREC:ehp2. to trace APIs within the PMWIN track rectangle API
group.
These APIs are included from PMWIN.H by defining INCL_WINTRACKRECT
(WinTrackRect() function).
:p.The following APIs comprise this group:
:ul compact.
:li.Win16ShowTrackRect
:li.Win16TrackRect
:li.Win32ShowTrackRect
:li.Win32TrackRect
:eul.
.*
.* HLP_WINWINCHK
.*
:h1 res=7059.Help for WIN
:i2 refid=custwin.Window
:p.Select :hp2.WIN:ehp2. to trace APIs within the PMWIN window API group.
These APIs are included from PMWIN.H by defining INCL_WINWINDOWMGR (general
window management).
:note.Even though WinDefWindowProc belongs in the window API group, it is
included in a different group, :link reftype=hd res=7021.DWIN:elink., to
address performance issues.
:p.The following APIs comprise this group:
:ul compact.
:li.Win16BeginEnumWindows
:li.Win16BeginPaint
:li.Win16CreateWindow
:li.Win16DestroyWindow
:li.Win16DrawBitmap
:li.Win16DrawBorder
:li.Win16DrawText
:li.Win16EnableWindow
:li.Win16EnableWindowUpdate
:li.Win16EndEnumWindows
:li.Win16EndPaint
:li.Win16ExcludeUpdateRegion
:li.Win16FillRect
:li.Win16GetClipPS
:li.Win16GetErasePS
:li.Win16GetNextWindow
:li.Win16GetPS
:li.Win16GetScreenPS
:li.Win16Initialize
:li.Win16InvalidateRect
:li.Win16InvalidateRegion
:li.Win16InvertRect
:li.Win16IsChild
:li.Win16IsThreadActive
:li.Win16IsWindow
:li.Win16IsWindowEnabled
:li.Win16IsWindowShowing
:li.Win16IsWindowVisible
:li.Win16LoadMessage
:li.Win16LoadString
:li.Win16LockVisRegions
:li.Win16LockWindow
:li.Win16LockWindowUpdate
:li.Win16MapWindowPoints
:li.Win16MultWindowFromIDs
:li.Win16OpenWindowDC
:li.Win16QueryActiveWindow
:li.Win16QueryAnchorBlock
:li.Win16QueryClassInfo
:li.Win16QueryClassName
:li.Win16QueryDesktopWindow
:li.Win16QueryObjectWindow
:li.Win16QuerySysModalWindow
:li.Win16QueryUpdateRect
:li.Win16QueryUpdateRegion
:li.Win16QueryVersion
:li.Win16QueryWindow
:li.Win16QueryWindowDC
:li.Win16QueryWindowLockCount
:li.Win16QueryWindowPos
:li.Win16QueryWindowProcess
:li.Win16QueryWindowPtr
:li.Win16QueryWindowRect
:li.Win16QueryWindowText
:li.Win16QueryWindowTextLength
:li.Win16QueryWindowULong
:li.Win16QueryWindowUShort
:li.Win16RegisterClass
:li.Win16RegisterWindowDestroy
:li.Win16ReleaseErasePS
:li.Win16ReleasePS
:li.Win16ScrollWindow
:li.Win16SetActiveWindow
:li.Win16SetMultWindowPos
:li.Win16SetOwner
:li.Win16SetParent
:li.Win16SetSysModalWindow
:li.Win16SetWindowBits
:li.Win16SetWindowPos
:li.Win16SetWindowPtr
:li.Win16SetWindowText
:li.Win16SetWindowULong
:li.Win16SetWindowUShort
:li.Win16ShowWindow
:li.Win16SubclassWindow
:li.Win16Terminate
:li.Win16UpdateWindow
:li.Win16ValidateRect
:li.Win16ValidateRegion
:li.Win16WindowFromDC
:li.Win16WindowFromID
:li.Win16WindowFromPoint
:li.Win32BeginEnumWindows
:li.Win32BeginPaint
:li.Win32CreateWindow
:li.Win32DestroyWindow
:li.Win32DrawBitmap
:li.Win32DrawBorder
:li.Win32DrawText
:li.Win32EnableWindow
:li.Win32EnableWindowUpdate
:li.Win32EndEnumWindows
:li.Win32EndPaint
:li.Win32ExcludeUpdateRegion
:li.Win32FillRect
:li.Win32GetClipPS
:li.Win32GetErasePS
:li.Win32GetNextWindow
:li.Win32GetPS
:li.Win32GetScreenPS
:li.Win32Initialize
:li.Win32InvalidateRect
:li.Win32InvalidateRegion
:li.Win32InvertRect
:li.Win32IsChild
:li.Win32IsThreadActive
:li.Win32IsWindow
:li.Win32IsWindowEnabled
:li.Win32IsWindowShowing
:li.Win32IsWindowVisible
:li.Win32LoadMessage
:li.Win32LoadString
:li.Win32LockVisRegions
:li.Win32LockWindowUpdate
:li.Win32MapWindowPoints
:li.Win32MultWindowFromIDs
:li.Win32OpenWindowDC
:li.Win32QueryActiveWindow
:li.Win32QueryAnchorBlock
:li.Win32QueryClassInfo
:li.Win32QueryClassName
.*POWERPC :li.Win32QueryClipRegion (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32QueryClipRegion (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Win32QueryDesktopWindow
:li.Win32QueryObjectWindow
:li.Win32QuerySysModalWindow
:li.Win32QueryUpdateRect
:li.Win32QueryUpdateRegion
:li.Win32QueryVersion
:li.Win32QueryVisibleRegion (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32QueryWindow
:li.Win32QueryWindowDC
:li.Win32QueryWindowPos
:li.Win32QueryWindowProcess
:li.Win32QueryWindowPtr
:li.Win32QueryWindowRect
:li.Win32QueryWindowText
:li.Win32QueryWindowTextLength
:li.Win32QueryWindowULong
:li.Win32QueryWindowUShort
:li.Win32RegisterClass
:li.Win32ReleaseErasePS
:li.Win32ReleasePS
:li.Win32ScrollWindow
:li.Win32SetActiveWindow
.*POWERPC :li.Win32SetClipRegion (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32SetClipRegion (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Win32SetMultWindowPos
:li.Win32SetOwner
:li.Win32SetParent
:li.Win32SetSysModalWindow
:li.Win32SetVisibleRegionNotify (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32SetWindowBits
:li.Win32SetWindowPos
:li.Win32SetWindowPtr
:li.Win32SetWindowText
:li.Win32SetWindowULong
:li.Win32SetWindowUShort
:li.Win32ShowWindow
:li.Win32SubclassWindow
:li.Win32Terminate
:li.Win32UpdateWindow
:li.Win32ValidateRect
:li.Win32ValidateRegion
:li.Win32WindowFromDC
:li.Win32WindowFromID
:li.Win32WindowFromPoint
:eul.
.*
.* HLP_WINOK
.*
:h1 res=7061.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the PMWIN APIs settings and
exit this window.
.*
.* HLP_WINCANCEL
.*
:h1 res=7063.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the PMWIN APIs settings.
.*
.* HLP_WINCLEAR
.*
:h1 res=7065.Help for Clear
:p.Select :hp2.Clear:ehp2. to deselect all PMWIN API groups in this window.
:note.At least one PMWIN API group must be selected before saving any
changes to the PMWIN APIs settings.
.*
.* HLP_WINDEFAULT
.*
:h1 res=7067.Help for Default
:p.Select :hp2.Default:ehp2. to change the PMWIN APIs settings to those that
were active when the OS/2 API Trace application was first installed.
.*
.* HLP_WINUNDO
.*
:h1 res=7069.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the PMWIN APIs settings to those that
were active before this window was displayed.
.*
.* HLP_INFOOK
.*
:h1 res=8009.Help for OK
:p.Select :hp2.OK:ehp2. to exit this window.
.*
.* PNL_DLG_ENAB
.*
:h1 res=9003.Help for Enablement
:p.Enable and/or disable the tracing of OS/2 APIs imported by the executable
file from one or more of the supported DLLs.
When a DLL's name and corresponding buttons are disabled (gray), the DLL is not
imported by the executable file and subsequently trace enablement is not
required as there are no APIs within the DLL to trace.
Otherwise, the DLL is imported by the executable file and it can therefore be
enabled or disabled for tracing.
:p.When the On button is selected, the corresponding DLL is enabled for
tracing, meaning that the APIs within the DLL are traced when the executable is
run.
When the Off button is selected, the corresponding DLL is disabled for tracing,
meaning that the APIs within the DLL are not traced when the executable is run.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=9011.
:dd.DOSCALLS
:elink.
:dt.o
:link reftype=hd res=9015.
:dd.HELPMGR
:elink.
:dt.o
:link reftype=hd res=9019.
:dd.KBDCALLS
:elink.
:dt.o
:link reftype=hd res=9023.
:dd.MONCALLS
:elink.
:dt.o
:link reftype=hd res=9027.
:dd.MOUCALLS
:elink.
:dt.o
:link reftype=hd res=9031.
:dd.MSG
:elink.
:dt.o
:link reftype=hd res=9035.
:dd.NAMPIPES
:elink.
:dt.o
:link reftype=hd res=9039.
:dd.NLS
:elink.
:dt.o
:link reftype=hd res=9043.
:dd.PMBIDI
:elink.
:dt.o
:link reftype=hd res=9047.
:dd.PMCTLS
:elink.
:dt.o
:link reftype=hd res=9051.
:dd.PMDRAG
:elink.
:dt.o
:link reftype=hd res=9055.
:dd.PMGPI
:elink.
:dt.o
:link reftype=hd res=9059.
:dd.PMMERGE
:elink.
:dt.o
:link reftype=hd res=9063.
:dd.PMPIC
:elink.
:dt.o
:link reftype=hd res=9067.
:dd.PMSHAPI
:elink.
:dt.o
:link reftype=hd res=9071.
:dd.PMSPL
:elink.
:dt.o
:link reftype=hd res=9075.
:dd.PMVIOP
:elink.
:dt.o
:link reftype=hd res=9079.
:dd.PMWIN
:elink.
:dt.o
:link reftype=hd res=9083.
:dd.PMWP
:elink.
:dt.o
:link reftype=hd res=9087.
:dd.QUECALLS
:elink.
:dt.o
:link reftype=hd res=9091.
:dd.SESMGR
:elink.
:dt.o
:link reftype=hd res=9095.
:dd.VIOCALLS
:elink.
:dt.o
:link reftype=hd res=9098.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=9100.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=9102.
:dd.All On
:elink.
:dt.o
:link reftype=hd res=9104.
:dd.All Off
:elink.
:dt.o
:link reftype=hd res=9106.
:dd.Undo
:elink.
:edl.
.*
.* HLP_ENABDOS
.*
:h1 res=9011.Help for DOSCALLS
:i2 refid=enab.DOSCALLS
:p.Select the :hp2.DOSCALLS:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from DOSCALLS.DLL.
When the On button is selected, the APIs within DOSCALLS.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within DOSCALLS.DLL are not traced
when the executable is run.
:note.The actual APIs traced within DOSCALLS.DLL are determined by the API
groups selected through the DOSCALLS APIs customization window, which provides
the same functionality as OS2TRACE's -D option.
:p.The following APIs are traced when DOSCALLS is enabled for tracing:
:ul compact.
:li.Dos16AcquireSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos16AllocHuge
:li.Dos16AllocSeg
:li.Dos16AllocShrSeg
:li.Dos16Beep
:li.Dos16BufReset
:li.Dos16Callback
:li.Dos16ChDir
:li.Dos16ChgFilePtr
:li.Dos16CLIAccess
:li.Dos16Close
:li.Dos16CloseSem
:li.Dos16CloseVDD
:li.Dos16Copy
:li.Dos16CreateCSAlias
:li.Dos16CreateSem
:li.Dos16CreateSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos16CreateThread
:li.Dos16Cwait
:li.Dos16Delete
:li.Dos16DevConfig
:li.Dos16DevIOCtl
:li.Dos16DevIOCtl2
:li.Dos16DupHandle
:li.Dos16DynamicTrace
:li.Dos16EditName
:li.Dos16EnterCritSec
:li.Dos16EnumAttribute
:li.Dos16ErrClass
:li.Dos16Error
:li.Dos16ExecPgm
:li.Dos16Exit
:li.Dos16ExitCritSec
:li.Dos16ExitList
:li.Dos16FileIO
:li.Dos16FileLocks
:li.Dos16FindClose
:li.Dos16FindFirst
:li.Dos16FindFirst2
:li.Dos16FindNext
:li.Dos16FlagProcess
:li.Dos16ForceDelete
:li.Dos16FreeModule
:li.Dos16FreeResource
:li.Dos16FreeSeg
:li.Dos16FreeSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos16FSAttach
:li.Dos16FSCtl
:li.Dos16FSRamSemClear
:li.Dos16FSRamSemClear2 (:link reftype=hd res=14.OS/2 2.45 to WSeB CP only:elink.)
:li.Dos16FSRamSemRequest
:li.Dos16FSRamSemRequest2 (:link reftype=hd res=14.OS/2 2.45 to WSeB CP only:elink.)
:li.Dos16GetCp
:li.Dos16GetDateTime
:li.Dos16GetEnv
:li.Dos16GetHugeShift
:li.Dos16GetInfoSeg
:li.Dos16GetMachineMode
:li.Dos16GetModHandle
:li.Dos16GetModName
:li.Dos16GetPID
:li.Dos16GetPPID
:li.Dos16GetProcAddr
:li.Dos16GetPrty
:li.Dos16GetResource
:li.Dos16GetResource2
:li.Dos16GetSeg
:li.Dos16GetShrSeg
:li.Dos16GetVersion
:li.Dos16GiveSeg
:li.Dos16HoldSignal
:li.Dos16KillProcess
:li.Dos16LoadModule
:li.Dos16LockSeg
:li.Dos16MakePipe
:li.Dos16MemAvail
:li.Dos16MkDir
:li.Dos16MkDir2
:li.Dos16Move
:li.Dos16MuxSemWait
:li.Dos16NewSize
:li.Dos16Open
:li.Dos16Open2
:li.Dos16OpenSem
:li.Dos16OpenVDD
:li.Dos16PhysicalDisk
:li.Dos16PortAccess
:li.Dos16ProtectChgFilePtr (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectClose (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectEnumAttribute (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectFileIO (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectFileLocks (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectNewSize (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectOpen (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectQFHandState (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectQFileInfo (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectSetFHandState (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16ProtectSetFileInfo (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16Ptrace
:li.Dos16QAppType
:li.Dos16QCurDir
:li.Dos16QCurDisk
:li.Dos16QFHandState
:li.Dos16QFileInfo
:li.Dos16QFileMode
:li.Dos16QFSAttach
:li.Dos16QFSInfo
:li.Dos16QHandType
:li.Dos16QPathInfo
:li.Dos16QSysInfo
:li.Dos16QueryDOSProperty
:li.Dos16QueryExtLIBPATH (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16QueryModFromCS
:li.Dos16QueryProcType
:li.Dos16QueryResourceSize
:li.Dos16QVerify
:li.Dos16R2StackRealloc
:li.Dos16Read
:li.Dos16ReadAsync
:li.Dos16ReallocHuge
:li.Dos16ReallocSeg
:li.Dos16RegisterPerfCtrs
:li.Dos16ReleaseSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos16RequestVDD
:li.Dos16ResumeThread
:li.Dos16RmDir
:li.Dos16ScanEnv
:li.Dos16SearchPath
:li.Dos16SelectDisk
:li.Dos16SemClear
:li.Dos16SemRequest
:li.Dos16SemSet
:li.Dos16SemSetWait
:li.Dos16SemWait
:li.Dos16SendSignal
:li.Dos16SetCp
:li.Dos16SetDateTime
:li.Dos16SetDOSProperty
:li.Dos16SetExtLIBPATH (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos16SetFHandState
:li.Dos16SetFileInfo
:li.Dos16SetFileMode
:li.Dos16SetFSInfo
:li.Dos16SetMaxFH
:li.Dos16SetPathInfo
:li.Dos16SetProcCp
:li.Dos16SetPrty
:li.Dos16SetSigHandler
:li.Dos16SetVec
:li.Dos16SetVerify
:li.Dos16Shutdown
:li.Dos16SizeSeg
:li.Dos16Sleep
:li.Dos16SubAlloc
:li.Dos16SubFree
:li.Dos16SubSet
:li.Dos16SuspendThread
:li.Dos16SystemService
:li.Dos16SysTrace
:li.Dos16TimerAsync
:li.Dos16TimerStart
:li.Dos16TimerStop
:li.Dos16TmrQueryFreq
:li.Dos16TmrQueryTime
:li.Dos16UnlockSeg
:li.Dos16Write
:li.Dos16WriteAsync
:li.Dos32AcknowledgeSignalException
:li.Dos32AcquireSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32AddMuxWaitSem
.*POWERPC :li.Dos32AliasMem (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32AliasMem
.*POWERPC :li.Dos32AliasPerfCtrs (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32AliasPerfCtrs
:li.Dos32AllocMem
:li.Dos32AllocSharedMem
.*POWERPC :li.Dos32AllocThreadLocalMemory (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32AllocThreadLocalMemory (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32AsyncTimer
:li.Dos32Beep
:li.Dos32CallNPipe
:li.Dos32CancelLockRequest
.*POWERPC:li.Dos32CancelLockRequestL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32CancelLockRequestL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32Close
:li.Dos32CloseEventSem
:li.Dos32CloseMutexSem
:li.Dos32CloseMuxWaitSem
.*POWERPC :li.Dos32CloseRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32CloseVDD
.*POWERPC :li.Dos32ConfigurePerf (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ConfigurePerf
:li.Dos32ConnectNPipe
:li.Dos32Copy
:li.Dos32CreateDir
:li.Dos32CreateEventSem
:li.Dos32CreateMutexSem
:li.Dos32CreateMuxWaitSem
:li.Dos32CreateNPipe
:li.Dos32CreatePipe
.*POWERPC :li.Dos32CreateRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32CreateSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32CreateThread
.*POWERPC:li.Dos32CreateThread2 (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32CreateThread2 (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32Debug
.*POWERPC :li.Dos32DeconPerf (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32DeconPerf
:li.Dos32Delete
:li.Dos32DeleteDir
:li.Dos32DeleteMuxWaitSem
:li.Dos32DevConfig
:li.Dos32DevIOCtl
:li.Dos32DisConnectNPipe
.*POWERPC :li.Dos32DumpProcess (:link reftype=hd res=21.OS/2 2.10+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32DumpProcess (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32DupHandle
.*POWERPC :li.Dos32DynamicTrace (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32DynamicTrace
:li.Dos32EditName
:li.Dos32EnterCritSec
:li.Dos32EnterMustComplete
:li.Dos32EnumAttribute
:li.Dos32ErrClass
:li.Dos32Error
:li.Dos32ExecPgm
:li.Dos32Exit
:li.Dos32ExitCritSec
:li.Dos32ExitList
:li.Dos32ExitMustComplete
:li.Dos32FindClose
:li.Dos32FindFirst
.*POWERPC :li.Dos32FindFromName (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32FindNext
:li.Dos32ForceDelete
:li.Dos32FreeMem
:li.Dos32FreeModule
:li.Dos32FreeResource
:li.Dos32FreeSpinLock (:link reftype=hd res=13.SMP only:elink.)
.*POWERPC :li.Dos32FreeThreadLocalMemory (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32FreeThreadLocalMemory (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32FSAttach
:li.Dos32FSCtl
:li.Dos32GetDateTime
:li.Dos32GetInfoBlocks
.*POWERPC :li.Dos32GetLocaleModule (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32GetMessage (:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32GetNamedSharedMem
:li.Dos32GetProcessorStatus (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32GetResource
:li.Dos32GetSharedMem
:li.Dos32GiveSharedMem
.*POWERPC :li.Dos32InsertMessage (:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32KillProcess
:li.Dos32KillThread
.*POWERPC:li.Dos32ListIO (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ListIO (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC:li.Dos32ListIOL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ListIOL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32LoadModule
:li.Dos32Move
:li.Dos32Open
:li.Dos32OpenEventSem
.*POWERPC:li.Dos32OpenL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32OpenL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32OpenMutexSem
:li.Dos32OpenMuxWaitSem
.*POWERPC :li.Dos32OpenRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32OpenVDD
:li.Dos32PeekNPipe
:li.Dos32PerfSysCall (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Dos32PhysicalDisk
.*POWERPC :li.Dos32PMPostEventSem (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMPostEventSem (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Dos32PMR3PostEventSem (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMR3PostEventSem (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC :li.Dos32PMR3WaitEventSem (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMR3WaitEventSem (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC :li.Dos32PMRequestMutexSem (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMRequestMutexSem (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Dos32PMWaitEventSem (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMWaitEventSem (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Dos32PMWaitMuxWaitSem (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PMWaitMuxWaitSem (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32PostEventSem
.*POWERPC :li.Dos32Profile (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32Profile
:li.Dos32ProtectClose (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectEnumAttribute (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectOpen (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32ProtectOpenL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ProtectOpenL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectQueryFHState (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectQueryFileInfo (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectRead (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectSetFHState (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectSetFileInfo (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32ProtectSetFileLocks (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32ProtectSetFileLocksL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ProtectSetFileLocksL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectSetFilePtr (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32ProtectSetFilePtrL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ProtectSetFilePtrL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectSetFileSize (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32ProtectSetFileSizeL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32ProtectSetFileSizeL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32ProtectWrite (:link reftype=hd res=21.OS/2 2.10+:elink.)
.*POWERPC :li.Dos32PutMessage (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32QueryABIOSSupport (:link reftype=hd res=21.OS/2 2.10+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryABIOSSupport (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32QueryAppType
.*POWERPC :li.Dos32QueryCp (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryCp
:li.Dos32QueryCurrentDir
:li.Dos32QueryCurrentDisk
:li.Dos32QueryDOSProperty
:li.Dos32QueryEventSem
:li.Dos32QueryExtLIBPATH (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32QueryFHState
:li.Dos32QueryFileInfo
:li.Dos32QueryFSAttach
:li.Dos32QueryFSInfo
:li.Dos32QueryHType
.*POWERPC :li.Dos32QueryMemState (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryMemState
:li.Dos32QueryMem
.*POWERPC :li.Dos32QueryMessageCP (:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32QueryModFromAddr (:link reftype=hd res=30.OS/2 3.00+:elink.,:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32QueryModFromEIP (:link reftype=hd res=21.OS/2 2.10+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryModFromEIP (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32QueryModuleHandle
:li.Dos32QueryModuleName
:li.Dos32QueryMutexSem
:li.Dos32QueryMuxWaitSem
:li.Dos32QueryNPHState
:li.Dos32QueryNPipeInfo
:li.Dos32QueryNPipeSemState
:li.Dos32QueryPathInfo
:li.Dos32QueryProcAddr
.*POWERPC :li.Dos32QueryProcessInfo (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32QueryProcType
.*POWERPC :li.Dos32QueryRASInfo (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryRASInfo
:li.Dos32QueryResourceSize
:li.Dos32QuerySysInfo
.*POWERPC :li.Dos32QuerySysState (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QuerySysState
.*POWERPC :li.Dos32QueryThreadAffinity (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryThreadAffinity (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC :li.Dos32QueryThreadContext (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryThreadContext (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Dos32QueryThreadInfo (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32QueryVerify
:li.Dos32RaiseException
.*POWERPC :li.Dos32RawReadNPipe (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32RawReadNPipe
.*POWERPC :li.Dos32RawWriteNPipe (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32RawWriteNPipe
:li.Dos32Read
.*POWERPC :li.Dos32RegisterPerfCtrs (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32RegisterPerfCtrs
:li.Dos32ReleaseMutexSem
.*POWERPC :li.Dos32ReleaseRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32ReleaseSpinLock (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32ReplaceModule
:li.Dos32RequestMutexSem
.*POWERPC :li.Dos32RequestRamSem (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32RequestVDD
:li.Dos32ResetBuffer
:li.Dos32ResetEventSem
:li.Dos32ResumeThread
:li.Dos32ScanEnv
:li.Dos32SearchPath
.*POWERPC :li.Dos32SelectSession (:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32SendSignalException
.*POWERPC :li.Dos32SetCp (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetCp
:li.Dos32SetCurrentDir
:li.Dos32SetDateTime
:li.Dos32SetDefaultDisk
:li.Dos32SetDOSProperty
:li.Dos32SetExceptionHandler
:li.Dos32SetExtLIBPATH (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32SetFHState
:li.Dos32SetFileInfo
:li.Dos32SetFileLocks
.*POWERPC :li.Dos32SetFileLocksL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetFileLocksL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32SetFilePtr
.*POWERPC :li.Dos32SetFilePtrL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetFilePtrL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32SetFileSize
.*POWERPC :li.Dos32SetFileSizeL (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetFileSizeL (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32SetFSInfo
:li.Dos32SetMaxFH
:li.Dos32SetMem
:li.Dos32SetNPHState
:li.Dos32SetNPipeSem
:li.Dos32SetPathInfo
:li.Dos32SetPriority
:li.Dos32SetProcessCp
:li.Dos32SetProcessorStatus (:link reftype=hd res=13.SMP only:elink.)
:li.Dos32SetRelMaxFH
.*POWERPC :li.Dos32SetSession (:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32SetSignalExceptionFocus
.*POWERPC :li.Dos32SetThreadAffinity (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetThreadAffinity (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32SetVerify
:li.Dos32Shutdown
:li.Dos32Sleep
.*POWERPC :li.Dos32StartSession (:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32StartTimer
.*POWERPC :li.Dos32StopSession (:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32StopTimer
:li.Dos32SubAllocMem
:li.Dos32SubFreeMem
:li.Dos32SubSetMem
:li.Dos32SubUnsetMem
.*POWERPC :li.Dos32SuppressPopUps (:link reftype=hd res=21.OS/2 2.10+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SuppressPopUps (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dos32SuspendThread
.*POWERPC :li.Dos32SystemService (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SystemService (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Dos32TestPSD (:link reftype=hd res=13.SMP only:elink.)
.*POWERPC :li.Dos32TmrQueryFreq (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32TmrQueryFreq
.*POWERPC :li.Dos32TmrQueryTime (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32TmrQueryTime
:li.Dos32TransactNPipe
:li.Dos32UnsetExceptionHandler
:li.Dos32UnwindException
.*POWERPC :li.Dos32VerifyPidTid (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dos32VerifyPidTid (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Dos32WaitChild
:li.Dos32WaitEventSem
:li.Dos32WaitMuxWaitSem
:li.Dos32WaitNPipe
:li.Dos32WaitThread
:li.Dos32Write
.*POWERPC :li.Temp32Write (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:eul.
.*POWERPC :note.To trace Dos32SelectSession, Dos32SetSession, Dos32StartSession, and
.*POWERPC Dos32StopSession on the Intel architecture, enable tracing of SESMGR.DLL.
.*POWERPC :note.To trace Dos32GetMessage, Dos32InsertMessage, Dos32PutMessage, and
.*POWERPC Dos32QueryProcessCp on the Intel architecture, enable tracing of MSG.DLL.
.*POWERPC :note.To trace Dos32QueryCp on the PowerPC architecture, enable tracing of
.*POWERPC NLS.DLL.
.*
.* HLP_ENABHLP
.*
:h1 res=9015.Help for HELPMGR
:i2 refid=enab.HELPMGR
:p.Select the :hp2.HELPMGR:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from HELPMGR.DLL.
When the On button is selected, the APIs within HELPMGR.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within HELPMGR.DLL are not traced
when the executable is run.
:p.The following APIs are traced when HELPMGR is enabled for tracing:
:ul compact.
:li.Ddf32BeginList
:li.Ddf32Bitmap
:li.Ddf32EndList
:li.Ddf32HyperText
:li.Ddf32Inform
:li.Ddf32Initialize
:li.Ddf32ListItem
:li.Ddf32Metafile
:li.Ddf32Para
:li.Ddf32SetColor
:li.Ddf32SetFont
:li.Ddf32SetFontStyle
:li.Ddf32SetFormat
:li.Ddf32SetTextAlign
:li.Ddf32Text
:li.Win16AssociateHelpInstance
:li.Win16CreateHelpInstance
:li.Win16CreateHelpTable
:li.Win16DestroyHelpInstance
:li.Win16LoadHelpTable
:li.Win16QueryHelpInstance
:li.Win32AssociateHelpInstance
:li.Win32CreateHelpInstance
:li.Win32CreateHelpTable
:li.Win32DestroyHelpInstance
:li.Win32LoadHelpTable
:li.Win32QueryHelpInstance
:eul.
.*
.* HLP_ENABKBD
.*
:h1 res=9019.Help for KBDCALLS
:i2 refid=enab.KBDCALLS
:p.Select the :hp2.KBDCALLS:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from KBDCALLS.DLL.
When the On button is selected, the APIs within KBDCALLS.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within KBDCALLS.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture, as
.*POWERPC KBDCALLS.DLL contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs are traced when KBDCALLS is enabled for tracing:
:ul compact.
:li.Kbd16CharIn
:li.Kbd16Close
:li.Kbd16DeRegister
:li.Kbd16FlushBuffer
:li.Kbd16FreeFocus
:li.Kbd16GetCp
:li.Kbd16GetFocus
:li.Kbd16GetHWId
:li.Kbd16GetStatus
:li.Kbd16Open
:li.Kbd16Peek
:li.Kbd16Register
:li.Kbd16SetCp
:li.Kbd16SetCustXt
:li.Kbd16SetFgnd
:li.Kbd16SetHWId
:li.Kbd16SetStatus
:li.Kbd16StringIn
:li.Kbd16Synch
:li.Kbd16Xlate
:eul.
.*
.* HLP_ENABMON
.*
:h1 res=9023.Help for MONCALLS
:i2 refid=enab.MONCALLS
:p.Select the :hp2.MONCALLS:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from MONCALLS.DLL.
When the On button is selected, the APIs within MONCALLS.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within MONCALLS.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture, as
.*POWERPC MONCALLS.DLL contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs are traced when MONCALLS is enabled for tracing:
:ul compact.
:li.Dos16MonClose
:li.Dos16MonOpen
:li.Dos16MonRead
:li.Dos16MonReg
:li.Dos16MonWrite
:eul.
.*
.* HLP_ENABMOU
.*
:h1 res=9027.Help for MOUCALLS
:i2 refid=enab.MOUCALLS
:p.Select the :hp2.MOUCALLS:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from MOUCALLS.DLL.
When the On button is selected, the APIs within MOUCALLS.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within MOUCALLS.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture, as
.*POWERPC MOUCALLS.DLL contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs are traced when MOUCALLS is enabled for tracing:
:ul compact.
:li.Mou16Close
:li.Mou16DeRegister
:li.Mou16DrawPtr
:li.Mou16FlushQue
:li.Mou16GetDevStatus
:li.Mou16GetEventMask
:li.Mou16GetNumButtons
:li.Mou16GetNumMickeys
:li.Mou16GetNumQueEl
:li.Mou16GetPtrPos
:li.Mou16GetPtrShape
:li.Mou16GetScaleFact
:li.Mou16GetThreshold
:li.Mou16InitReal
:li.Mou16Open
:li.Mou16ReadEventQue
:li.Mou16Register
:li.Mou16RemovePtr
:li.Mou16SetDevStatus
:li.Mou16SetEventMask
:li.Mou16SetPtrPos
:li.Mou16SetPtrShape
:li.Mou16SetScaleFact
:li.Mou16SetThreshold
:li.Mou16Synch
:eul.
.*
.* HLP_ENABMSG
.*
:h1 res=9031.Help for MSG
:i2 refid=enab.MSG
:p.Select the :hp2.MSG:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from MSG.DLL.
When the On button is selected, the APIs within MSG.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within MSG.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture.
.*POWERPC To obtain the same functionality on the PowerPC architecture, enable tracing of
.*POWERPC DOSCALLS.DLL and select the DOSCALLS MSG API group.
:p.The following APIs are traced when MSG is enabled for tracing:
:ul compact.
:li.Dos16GetMessage
:li.Dos16InsMessage
:li.Dos16PutMessage
.*POWERPC :li.Dos32InsertMessage (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32InsertMessage
.*POWERPC :li.Dos32PutMessage (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32PutMessage
.*POWERPC :li.Dos32GetMessage (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32GetMessage
.*POWERPC :li.Dos32QueryMessageCP (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32QueryMessageCP
:eul.
.*
.* HLP_ENABPIP
.*
:h1 res=9035.Help for NAMPIPES
:i2 refid=enab.NAMPIPES
:p.Select the :hp2.NAMPIPES:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from NAMPIPES.DLL.
When the On button is selected, the APIs within NAMPIPES.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within NAMPIPES.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture, as
.*POWERPC NAMPIPES.DLL contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs are traced when NAMPIPES is enabled for tracing:
:ul compact.
:li.Dos16CallNmPipe
:li.Dos16ConnectNmPipe
:li.Dos16DisConnectNmPipe
:li.Dos16MakeNmPipe
:li.Dos16PeekNmPipe
:li.Dos16QNmPHandState
:li.Dos16QNmPipeInfo
:li.Dos16QNmPipeSemState
:li.Dos16RawReadNmPipe
:li.Dos16RawWriteNmPipe
:li.Dos16SetNmPHandState
:li.Dos16SetNmPipeSem
:li.Dos16TransactNmPipe
:li.Dos16WaitNmPipe
:eul.
.*
.*
.* HLP_ENABNLS
.*
:h1 res=9039.Help for NLS
:i2 refid=enab.NLS
:p.Select the :hp2.NLS:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from NLS.DLL.
When the On button is selected, the APIs within NLS.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within NLS.DLL are not traced
when the executable is run.
:p.The following APIs are traced when NLS is enabled for tracing:
:ul compact.
:li.Dos16CaseMap
:li.Dos16GetCollate
:li.Dos16GetCtryInfo
:li.Dos16GetDBCSEv
:li.Dos32MapCase
.*POWERPC :li.Dos32QueryCc (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32QueryCollate
.*POWERPC :li.Dos32QueryCp (:link reftype=hd res=12.PowerPC only:elink.)
:li.Dos32QueryCtryInfo
:li.Dos32QueryDBCSEnv
.*POWERPC :li.Dos32SetProcessCc (:link reftype=hd res=30.OS/2 3.00:elink.,:link reftype=hd res=12.PowerPC only:elink.)
.*POWERPC :li.Dos32SetProcessCp (:link reftype=hd res=12.PowerPC only:elink.)
:eul.
.*POWERPC :note.To trace Dos32QueryCp and Dos32SetProcessCp on the Intel architecture,
.*POWERPC enable tracing of DOSCALLS.DLL and select the DOSCALLS NLS API group.
.*
.* HLP_ENABPBD
.*
:h1 res=9043.Help for PMBIDI
:i2 refid=enab.PMBIDI
:p.Select the :hp2.PMBIDI:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMBIDI.DLL.
When the On button is selected, the APIs within PMBIDI.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMBIDI.DLL are not traced
when the executable is run.
:note.Prior to OS/2 Version 2.40, PMBIDI.DLL is only present on NLS versions of
OS/2 that support a bidirectional language.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture.
:p.The following APIs are traced when PMBIDI is enabled for tracing:
:ul compact.
:li.Gpi16QueryBidiAttr (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Gpi16SetBidiAttr (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Gpi32QueryBidiAttr (:link reftype=hd res=23.OS/2 2.30:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryBidiAttr (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Gpi32SetBidiAttr (:link reftype=hd res=23.OS/2 2.30:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32SetBidiAttr (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Win16QueryKbdLayer (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Win16QueryLangInfo (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Win16QueryLangViewer (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Win16SetKbdLayer (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Win16SetLangInfo (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Win16SetLangViewer (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Win32QueryKbdLayer (:link reftype=hd res=23.OS/2 2.30:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32QueryKbdLayer (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Win32QueryLangInfo (:link reftype=hd res=23.OS/2 2.30:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32QueryLangInfo (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Win32QueryLangViewer (:link reftype=hd res=23.OS/2 2.30:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32QueryLangViewer (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Win32SetKbdLayer (:link reftype=hd res=23.OS/2 2.30:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32SetKbdLayer (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Win32SetLangInfo (:link reftype=hd res=23.OS/2 2.30:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32SetLangInfo (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Win32SetLangViewer (:link reftype=hd res=23.OS/2 2.30:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32SetLangViewer (:link reftype=hd res=23.OS/2 2.30:elink.)
:eul.
.*
.* HLP_ENABCTL
.*
:h1 res=9047.Help for PMCTLS
:i2 refid=enab.PMCTLS
:p.Select the :hp2.PMCTLS:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMCTLS.DLL.
When the On button is selected, the APIs within PMCTLS.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMCTLS.DLL are not traced
when the executable is run.
:p.The following APIs are traced when PMCTLS is enabled for tracing:
:ul compact.
:li.Win32DefFileDlgProc
:li.Win32DefFontDlgProc
:li.Win32FileDlg
:li.Win32FontDlg
:li.Win32FreeFileDlgList
:li.Win32RegisterCircularSlider (:link reftype=hd res=23.OS/2 2.30:elink.)
:eul.
.*
.* HLP_ENABDRG
.*
:h1 res=9051.Help for PMDRAG
:i2 refid=enab.PMDRAG
:p.Select the :hp2.PMDRAG:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMDRAG.DLL.
When the On button is selected, the APIs within PMDRAG.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMDRAG.DLL are not traced
when the executable is run.
:p.The following APIs are traced when PMDRAG is enabled for tracing:
:ul compact.
:li.Drg16AcceptDroppedFiles
:li.Drg16AccessDraginfo
:li.Drg16AddStrHandle
:li.Drg16AllocDraginfo
:li.Drg16AllocDragtransfer
:li.Drg16DeleteDraginfoStrHandles
:li.Drg16DeleteStrHandle
:li.Drg16Drag
:li.Drg16DragFiles
:li.Drg16FreeDraginfo
:li.Drg16FreeDragtransfer
:li.Drg16GetPS
:li.Drg16PostTransferMsg
:li.Drg16PushDraginfo
:li.Drg16QueryDragitem
:li.Drg16QueryDragitemCount
:li.Drg16QueryDragitemPtr
:li.Drg16QueryNativeRMF
:li.Drg16QueryNativeRMFLen
:li.Drg16QueryStrName
:li.Drg16QueryStrNameLen
:li.Drg16QueryTrueType
:li.Drg16QueryTrueTypeLen
:li.Drg16ReleasePS
:li.Drg16SendTransferMsg
:li.Drg16SetDragImage
:li.Drg16SetDragitem
:li.Drg16SetDragPointer
:li.Drg16VerifyNativeRMF
:li.Drg16VerifyRMF
:li.Drg16VerifyTrueType
:li.Drg16VerifyType
:li.Drg16VerifyTypeSet
:li.Drg32AcceptDroppedFiles
:li.Drg32AccessDraginfo
:li.Drg32AddStrHandle
:li.Drg32AllocDraginfo
:li.Drg32AllocDragtransfer
:li.Drg32CancelLazyDrag (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Drg32DeleteDraginfoStrHandles
:li.Drg32DeleteStrHandle
:li.Drg32Drag
:li.Drg32DragFiles
:li.Drg32FreeDraginfo
:li.Drg32FreeDragtransfer
:li.Drg32GetPS
:li.Drg32LazyDrag (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Drg32LazyDrop (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Drg32PostTransferMsg
:li.Drg32PushDraginfo
:li.Drg32QueryDraginfoPtr (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Drg32QueryDraginfoPtrFromDragitem (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Drg32QueryDraginfoPtrFromHwnd (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Drg32QueryDragitem
:li.Drg32QueryDragitemCount
:li.Drg32QueryDragitemPtr
:li.Drg32QueryDragStatus (:link reftype=hd res=23.OS/2 2.30:elink.)
.*POWERPC :li.Drg32QueryFormat (:link reftype=hd res=24.OS/2 2.40:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Drg32QueryFormat (:link reftype=hd res=24.OS/2 2.40:elink.)
:li.Drg32QueryNativeRMF
:li.Drg32QueryNativeRMFLen
:li.Drg32QueryStrName
:li.Drg32QueryStrNameLen
:li.Drg32QueryTrueType
:li.Drg32QueryTrueTypeLen
:li.Drg32ReallocDraginfo (:link reftype=hd res=23.OS/2 2.30:elink.)
:li.Drg32ReleasePS
:li.Drg32SendTransferMsg
:li.Drg32SetDragImage
:li.Drg32SetDragitem
:li.Drg32SetDragPointer
:li.Drg32VerifyNativeRMF
:li.Drg32VerifyRMF
:li.Drg32VerifyTrueType
:li.Drg32VerifyType
:li.Drg32VerifyTypeSet
:eul.
.*
.* HLP_ENABGPI
.*
:h1 res=9055.Help for PMGPI
:i2 refid=enab.PMGPI
:p.Select the :hp2.PMGPI:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMGPI DLL.
When the On button is selected, the APIs within PMGPI.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMGPI.DLL are not traced
when the executable is run.
:note.The actual APIs traced within PMGPI.DLL are determined by the API
groups selected through the PMGPI APIs customization window, which provides
the same functionality as OS2TRACE's -G option.
:p.The following APIs are traced when PMGPI is enabled for tracing:
:ul compact.
:li.Dev16CloseDC
:li.Dev16Escape
:li.Dev16OpenDC
:li.Dev16PostDeviceModes
:li.Dev16QueryCaps
:li.Dev16QueryDeviceNames
:li.Dev16QueryHardcopyCaps
:li.Dev16StdOpen (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Dev32CloseDC
:li.Dev32Escape
:li.Dev32OpenDC
:li.Dev32PostDeviceModes
.*POWERPC :li.Dev32PostEscape (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Dev32PostEscape (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Dev32QueryCaps
:li.Dev32QueryDeviceNames
:li.Dev32QueryHardcopyCaps
:li.Dev32StdOpen (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Gpi16AnimatePalette
:li.Gpi16Associate
:li.Gpi16BeginArea
:li.Gpi16BeginElement
:li.Gpi16BeginPath
:li.Gpi16BitBlt
:li.Gpi16Box
:li.Gpi16CallSegmentMatrix
:li.Gpi16CharString
:li.Gpi16CharStringAt
:li.Gpi16CharStringPos
:li.Gpi16CharStringPosAt
:li.Gpi16CloseFigure
:li.Gpi16CloseSegment
:li.Gpi16CombineRegion
:li.Gpi16Comment
:li.Gpi16Convert
:li.Gpi16ConvertWithMatrix
:li.Gpi16CopyMetaFile
:li.Gpi16CorrelateChain
:li.Gpi16CorrelateFrom
:li.Gpi16CorrelateSegment
:li.Gpi16CreateBitmap
:li.Gpi16CreateLogColorTable
:li.Gpi16CreateLogFont
:li.Gpi16CreatePalette
:li.Gpi16CreatePS
:li.Gpi16CreateRegion
:li.Gpi16DeleteBitmap
:li.Gpi16DeleteElement
:li.Gpi16DeleteElementRange
:li.Gpi16DeleteElementsBetweenLabels
:li.Gpi16DeleteMetaFile
:li.Gpi16DeletePalette
:li.Gpi16DeleteSegment
:li.Gpi16DeleteSegments
:li.Gpi16DeleteSetId
:li.Gpi16DestroyPS
:li.Gpi16DestroyRegion
:li.Gpi16DrawBits
:li.Gpi16DrawChain
:li.Gpi16DrawDynamics
:li.Gpi16DrawFrom
:li.Gpi16DrawSegment
:li.Gpi16Element
:li.Gpi16EndArea
:li.Gpi16EndElement
:li.Gpi16EndPath
:li.Gpi16EqualRegion
:li.Gpi16Erase
:li.Gpi16ErrorSegmentData
:li.Gpi16ExcludeClipRectangle
:li.Gpi16FillPath
:li.Gpi16FloodFill
:li.Gpi16FrameRegion
:li.Gpi16FullArc
:li.Gpi16GetData
:li.Gpi16Image
:li.Gpi16IntersectClipRectangle
:li.Gpi16Label
:li.Gpi16Line
:li.Gpi16LoadFonts
:li.Gpi16LoadMetaFile
:li.Gpi16LoadPublicFonts
:li.Gpi16Marker
:li.Gpi16ModifyPath
:li.Gpi16Move
:li.Gpi16OffsetClipRegion
:li.Gpi16OffsetElementPointer
:li.Gpi16OffsetRegion
:li.Gpi16OpenSegment
:li.Gpi16OutlinePath
:li.Gpi16PaintRegion
:li.Gpi16PartialArc
:li.Gpi16PathToRegion
:li.Gpi16PlayMetaFile
:li.Gpi16PointArc
:li.Gpi16PolyFillet
:li.Gpi16PolyFilletSharp
:li.Gpi16PolyLine
:li.Gpi16PolyLineDisjoint
:li.Gpi16PolyMarker
:li.Gpi16PolySpline
:li.Gpi16Pop
:li.Gpi16PtInRegion
:li.Gpi16PtVisible
:li.Gpi16PutData
:li.Gpi16QueryArcParams
:li.Gpi16QueryAttrMode
:li.Gpi16QueryAttrs
:li.Gpi16QueryBackColor
:li.Gpi16QueryBackMix
:li.Gpi16QueryBitmapBits
:li.Gpi16QueryBitmapDimension
:li.Gpi16QueryBitmapHandle
:li.Gpi16QueryBitmapInfoHeader
:li.Gpi16QueryBitmapParameters
:li.Gpi16QueryBoundaryData
:li.Gpi16QueryCharAngle
:li.Gpi16QueryCharBox
:li.Gpi16QueryCharBreakExtra
:li.Gpi16QueryCharDirection
:li.Gpi16QueryCharExtra
:li.Gpi16QueryCharMode
:li.Gpi16QueryCharSet
:li.Gpi16QueryCharShear
:li.Gpi16QueryCharStringPos
:li.Gpi16QueryCharStringPosAt
:li.Gpi16QueryClipBox
:li.Gpi16QueryClipRegion
:li.Gpi16QueryColor
:li.Gpi16QueryColorData
:li.Gpi16QueryColorIndex
:li.Gpi16QueryCp
:li.Gpi16QueryCurrentPosition
:li.Gpi16QueryDefArcParams
:li.Gpi16QueryDefAttrs
:li.Gpi16QueryDefaultViewMatrix
:li.Gpi16QueryDefCharBox
:li.Gpi16QueryDefTag
:li.Gpi16QueryDefViewingLimits
:li.Gpi16QueryDevice
:li.Gpi16QueryDeviceBitmapFormats
:li.Gpi16QueryDrawControl
:li.Gpi16QueryDrawingMode
:li.Gpi16QueryEditMode
:li.Gpi16QueryElement
:li.Gpi16QueryElementPointer
:li.Gpi16QueryElementType
:li.Gpi16QueryFaceString
:li.Gpi16QueryFontAction
:li.Gpi16QueryFontFileDescriptions
:li.Gpi16QueryFontMetrics
:li.Gpi16QueryFonts
:li.Gpi16QueryGraphicsField
:li.Gpi16QueryInitialSegmentAttrs
:li.Gpi16QueryKerningPairs
:li.Gpi16QueryLineEnd
:li.Gpi16QueryLineJoin
:li.Gpi16QueryLineType
:li.Gpi16QueryLineWidth
:li.Gpi16QueryLineWidthGeom
:li.Gpi16QueryLogColorTable
:li.Gpi16QueryLogicalFont
:li.Gpi16QueryMarker
:li.Gpi16QueryMarkerBox
:li.Gpi16QueryMarkerSet
:li.Gpi16QueryMetaFileBits
:li.Gpi16QueryMetaFileLength
:li.Gpi16QueryMix
:li.Gpi16QueryModelTransformMatrix
:li.Gpi16QueryNearestColor
:li.Gpi16QueryNumberSetIds
:li.Gpi16QueryPageViewport
:li.Gpi16QueryPalette
:li.Gpi16QueryPaletteInfo
:li.Gpi16QueryPattern
:li.Gpi16QueryPatternRefPoint
:li.Gpi16QueryPatternSet
:li.Gpi16QueryPel
:li.Gpi16QueryPickAperturePosition
:li.Gpi16QueryPickApertureSize
:li.Gpi16QueryPS
:li.Gpi16QueryRegionBox
:li.Gpi16QueryRealColors
:li.Gpi16QueryRegionRects
:li.Gpi16QueryRGBColor
:li.Gpi16QuerySegmentAttrs
:li.Gpi16QuerySegmentNames
:li.Gpi16QuerySegmentPriority
:li.Gpi16QuerySegmentTransformMatrix
:li.Gpi16QuerySetIds
:li.Gpi16QueryStopDraw
:li.Gpi16QueryTag
:li.Gpi16QueryTextBox
:li.Gpi16QueryViewingLimits
:li.Gpi16QueryViewingTransformMatrix
:li.Gpi16QueryWidthTable
:li.Gpi16RealizeColorTable
:li.Gpi16RectInRegion
:li.Gpi16RectVisible
:li.Gpi16RemoveDynamics
:li.Gpi16ResetBoundaryData
:li.Gpi16ResetPS
:li.Gpi16RestorePS
:li.Gpi16ResumePlay
:li.Gpi16Rotate
:li.Gpi16SaveMetaFile
:li.Gpi16SavePS
:li.Gpi16Scale
:li.Gpi16SelectPalette
:li.Gpi16SetArcParams
:li.Gpi16SetAttrMode
:li.Gpi16SetAttrs
:li.Gpi16SetBackColor
:li.Gpi16SetBackMix
:li.Gpi16SetBitmap
:li.Gpi16SetBitmapBits
:li.Gpi16SetBitmapDimension
:li.Gpi16SetBitmapId
:li.Gpi16SetCharAngle
:li.Gpi16SetCharBox
:li.Gpi16SetCharBreakExtra
:li.Gpi16SetCharDirection
:li.Gpi16SetCharExtra
:li.Gpi16SetCharMode
:li.Gpi16SetCharSet
:li.Gpi16SetCharShear
:li.Gpi16SetClipPath
:li.Gpi16SetClipRegion
:li.Gpi16SetColor
:li.Gpi16SetCp
:li.Gpi16SetCurrentPosition
:li.Gpi16SetDefArcParams
:li.Gpi16SetDefAttrs
:li.Gpi16SetDefaultViewMatrix
:li.Gpi16SetDefTag
:li.Gpi16SetDefViewingLimits
:li.Gpi16SetDrawControl
:li.Gpi16SetDrawingMode
:li.Gpi16SetEditMode
:li.Gpi16SetElementPointer
:li.Gpi16SetElementPointerAtLabel
:li.Gpi16SetGraphicsField
:li.Gpi16SetInitialSegmentAttrs
:li.Gpi16SetLineEnd
:li.Gpi16SetLineJoin
:li.Gpi16SetLineType
:li.Gpi16SetLineWidth
:li.Gpi16SetLineWidthGeom
:li.Gpi16SetMarker
:li.Gpi16SetMarkerBox
:li.Gpi16SetMarkerSet
:li.Gpi16SetMetaFileBits
:li.Gpi16SetMix
:li.Gpi16SetModelTransformMatrix
:li.Gpi16SetPageViewport
:li.Gpi16SetPaletteEntries
:li.Gpi16SetPattern
:li.Gpi16SetPatternRefPoint
:li.Gpi16SetPatternSet
:li.Gpi16SetPel
:li.Gpi16SetPickAperturePosition
:li.Gpi16SetPickApertureSize
:li.Gpi16SetPS
:li.Gpi16SetRegion
:li.Gpi16SetSegmentAttrs
:li.Gpi16SetSegmentPriority
:li.Gpi16SetSegmentTransformMatrix
:li.Gpi16SetStopDraw
:li.Gpi16SetTag
:li.Gpi16SetViewingLimits
:li.Gpi16SetViewingTransformMatrix
:li.Gpi16StrokePath
:li.Gpi16SuspendPlay
:li.Gpi16Translate
:li.Gpi16UnloadFonts
:li.Gpi16UnloadPublicFonts
:li.Gpi16UnrealizeColorTable
:li.Gpi16WCBitBlt
:li.Gpi32AnimatePalette
:li.Gpi32Associate
:li.Gpi32BeginArea
:li.Gpi32BeginElement
.*POWERPC :li.Gpi32BeginInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32BeginInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32BeginPath
:li.Gpi32BitBlt
:li.Gpi32Box
:li.Gpi32CallSegmentMatrix
:li.Gpi32CharString
:li.Gpi32CharStringAt
:li.Gpi32CharStringPos
:li.Gpi32CharStringPosAt
:li.Gpi32CloseFigure
:li.Gpi32CloseSegment
:li.Gpi32CombineRegion
:li.Gpi32Comment
:li.Gpi32Convert
:li.Gpi32ConvertWithMatrix
:li.Gpi32CopyMetaFile
:li.Gpi32CorrelateChain
:li.Gpi32CorrelateFrom
:li.Gpi32CorrelateSegment
:li.Gpi32CreateBitmap
.*POWERPC :li.Gpi32CreateEllipticRegion (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32CreateEllipticRegion (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32CreateLogColorTable
:li.Gpi32CreateLogFont
:li.Gpi32CreatePalette
.*POWERPC :li.Gpi32CreatePolygonRegion (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32CreatePolygonRegion (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32CreatePS
:li.Gpi32CreateRegion
.*POWERPC :li.Gpi32CreateRoundRectRegion (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32CreateRoundRectRegion (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32DeleteBitmap
:li.Gpi32DeleteElement
:li.Gpi32DeleteElementRange
:li.Gpi32DeleteElementsBetweenLabels
:li.Gpi32DeleteMetaFile
:li.Gpi32DeletePalette
:li.Gpi32DeleteSegment
:li.Gpi32DeleteSegments
:li.Gpi32DeleteSetId
:li.Gpi32DestroyPS
:li.Gpi32DestroyRegion
:li.Gpi32DrawBits
:li.Gpi32DrawChain
:li.Gpi32DrawDynamics
:li.Gpi32DrawFrom
:li.Gpi32DrawSegment
:li.Gpi32Element
:li.Gpi32EndArea
:li.Gpi32EndElement
.*POWERPC :li.Gpi32EndInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32EndInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32EndPath
:li.Gpi32EqualRegion
:li.Gpi32Erase
:li.Gpi32ErrorSegmentData
:li.Gpi32ExcludeClipRectangle
:li.Gpi32FillPath
:li.Gpi32FloodFill
:li.Gpi32FrameRegion
:li.Gpi32FullArc
:li.Gpi32GetData
:li.Gpi32Image
:li.Gpi32IntersectClipRectangle
:li.Gpi32Label
:li.Gpi32Line
:li.Gpi32LoadBitmap
:li.Gpi32LoadFonts
:li.Gpi32LoadMetaFile
:li.Gpi32LoadPublicFonts
:li.Gpi32Marker
:li.Gpi32ModifyPath
:li.Gpi32Move
:li.Gpi32OffsetClipRegion
:li.Gpi32OffsetElementPointer
:li.Gpi32OffsetRegion
:li.Gpi32OpenSegment
:li.Gpi32OutlinePath
.*POWERPC :li.Gpi32Oval (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32Oval (:link reftype=hd res=245.OS/2 2.45+:elink.)
:li.Gpi32PaintRegion
:li.Gpi32PartialArc
:li.Gpi32PathToRegion
:li.Gpi32PlayMetaFile
:li.Gpi32PointArc
:li.Gpi32PolyFillet
:li.Gpi32PolyFilletSharp
:li.Gpi32Polygons
:li.Gpi32PolyLine
:li.Gpi32PolyLineDisjoint
:li.Gpi32PolyMarker
:li.Gpi32PolySpline
:li.Gpi32Pop
:li.Gpi32PtInRegion
:li.Gpi32PtVisible
:li.Gpi32PutData
:li.Gpi32QueryArcParams
:li.Gpi32QueryAttrMode
:li.Gpi32QueryAttrs
:li.Gpi32QueryBackColor
:li.Gpi32QueryBackMix
:li.Gpi32QueryBitmapBits
:li.Gpi32QueryBitmapDimension
:li.Gpi32QueryBitmapHandle
:li.Gpi32QueryBitmapInfoHeader
:li.Gpi32QueryBitmapParameters
:li.Gpi32QueryBoundaryData
:li.Gpi32QueryCharAngle
:li.Gpi32QueryCharBox
:li.Gpi32QueryCharBreakExtra
:li.Gpi32QueryCharDirection
:li.Gpi32QueryCharExtra
:li.Gpi32QueryCharMode
:li.Gpi32QueryCharSet
:li.Gpi32QueryCharShear
:li.Gpi32QueryCharStringPos
:li.Gpi32QueryCharStringPosAt
:li.Gpi32QueryClipBox
:li.Gpi32QueryClipRegion
:li.Gpi32QueryColor
:li.Gpi32QueryColorData
:li.Gpi32QueryColorIndex
:li.Gpi32QueryCp
:li.Gpi32QueryCurrentPosition
:li.Gpi32QueryDefArcParams
:li.Gpi32QueryDefAttrs
:li.Gpi32QueryDefaultViewMatrix
:li.Gpi32QueryDefCharBox
:li.Gpi32QueryDefTag
:li.Gpi32QueryDefViewingLimits
:li.Gpi32QueryDevice
:li.Gpi32QueryDeviceBitmapFormats
:li.Gpi32QueryDrawControl
:li.Gpi32QueryDrawingMode
:li.Gpi32QueryEditMode
:li.Gpi32QueryElement
:li.Gpi32QueryElementPointer
:li.Gpi32QueryElementType
:li.Gpi32QueryFaceString
:li.Gpi32QueryFontAction
:li.Gpi32QueryFontFileDescriptions
.*POWERPC :li.Gpi32QueryFontMappingFlags (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryFontMappingFlags (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32QueryFontMetrics
:li.Gpi32QueryFonts
:li.Gpi32QueryFullFontFileDescs
:li.Gpi32QueryGraphicsField
:li.Gpi32QueryInitialSegmentAttrs
:li.Gpi32QueryKerningPairs
:li.Gpi32QueryLineEnd
:li.Gpi32QueryLineJoin
:li.Gpi32QueryLineType
:li.Gpi32QueryLineWidth
:li.Gpi32QueryLineWidthGeom
:li.Gpi32QueryLogColorTable
:li.Gpi32QueryLogicalFont
:li.Gpi32QueryMarker
:li.Gpi32QueryMarkerBox
:li.Gpi32QueryMarkerSet
:li.Gpi32QueryMetaFileBits
:li.Gpi32QueryMetaFileLength
:li.Gpi32QueryMix
:li.Gpi32QueryModelTransformMatrix
:li.Gpi32QueryNearestColor
.*POWERPC :li.Gpi32QueryNearestPaletteIndex (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryNearestPaletteIndex (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32QueryNumberSetIds
:li.Gpi32QueryPageViewport
:li.Gpi32QueryPalette
:li.Gpi32QueryPaletteInfo
:li.Gpi32QueryPattern
:li.Gpi32QueryPatternRefPoint
:li.Gpi32QueryPatternSet
:li.Gpi32QueryPel
:li.Gpi32QueryPickAperturePosition
:li.Gpi32QueryPickApertureSize
:li.Gpi32QueryPS
.*POWERPC :li.Gpi32QueryRasterizerCaps (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryRasterizerCaps (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32QueryRealColors
:li.Gpi32QueryRegionBox
:li.Gpi32QueryRegionRects
:li.Gpi32QueryRGBColor
:li.Gpi32QuerySegmentAttrs
:li.Gpi32QuerySegmentNames
:li.Gpi32QuerySegmentPriority
:li.Gpi32QuerySegmentTransformMatrix
:li.Gpi32QuerySetIds
:li.Gpi32QueryStopDraw
.*POWERPC :li.Gpi32QueryTabbedTextExtent (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32QueryTabbedTextExtent (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32QueryTag
:li.Gpi32QueryTextAlignment
:li.Gpi32QueryTextBox
:li.Gpi32QueryViewingLimits
:li.Gpi32QueryViewingTransformMatrix
:li.Gpi32QueryWidthTable
:li.Gpi32RectInRegion
:li.Gpi32RectVisible
:li.Gpi32RemoveDynamics
:li.Gpi32ResetBoundaryData
:li.Gpi32ResetPS
.*POWERPC :li.Gpi32ResizePalette (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32ResizePalette (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32RestorePS
:li.Gpi32ResumePlay
:li.Gpi32Rotate
:li.Gpi32SaveMetaFile
:li.Gpi32SavePS
:li.Gpi32Scale
:li.Gpi32SelectPalette
:li.Gpi32SetArcParams
:li.Gpi32SetAttrMode
:li.Gpi32SetAttrs
:li.Gpi32SetBackColor
:li.Gpi32SetBackMix
:li.Gpi32SetBitmap
:li.Gpi32SetBitmapBits
:li.Gpi32SetBitmapDimension
:li.Gpi32SetBitmapId
:li.Gpi32SetCharAngle
:li.Gpi32SetCharBox
:li.Gpi32SetCharBreakExtra
:li.Gpi32SetCharDirection
:li.Gpi32SetCharExtra
:li.Gpi32SetCharMode
:li.Gpi32SetCharSet
:li.Gpi32SetCharShear
:li.Gpi32SetClipPath
:li.Gpi32SetClipRegion
:li.Gpi32SetColor
:li.Gpi32SetCp
:li.Gpi32SetCurrentPosition
:li.Gpi32SetDefArcParams
:li.Gpi32SetDefAttrs
:li.Gpi32SetDefaultViewMatrix
:li.Gpi32SetDefTag
:li.Gpi32SetDefViewingLimits
:li.Gpi32SetDrawControl
:li.Gpi32SetDrawingMode
:li.Gpi32SetEditMode
:li.Gpi32SetElementPointer
:li.Gpi32SetElementPointerAtLabel
.*POWERPC :li.Gpi32SetFontMappingFlags (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32SetFontMappingFlags (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32SetGraphicsField
:li.Gpi32SetInitialSegmentAttrs
:li.Gpi32SetLineEnd
:li.Gpi32SetLineJoin
:li.Gpi32SetLineType
:li.Gpi32SetLineWidth
:li.Gpi32SetLineWidthGeom
:li.Gpi32SetMarker
:li.Gpi32SetMarkerBox
:li.Gpi32SetMarkerSet
:li.Gpi32SetMetaFileBits
:li.Gpi32SetMix
:li.Gpi32SetModelTransformMatrix
:li.Gpi32SetPageViewport
:li.Gpi32SetPaletteEntries
:li.Gpi32SetPattern
:li.Gpi32SetPatternRefPoint
:li.Gpi32SetPatternSet
:li.Gpi32SetPel
:li.Gpi32SetPickAperturePosition
:li.Gpi32SetPickApertureSize
:li.Gpi32SetPS
:li.Gpi32SetRegion
:li.Gpi32SetSegmentAttrs
:li.Gpi32SetSegmentPriority
:li.Gpi32SetSegmentTransformMatrix
:li.Gpi32SetStopDraw
:li.Gpi32SetTag
:li.Gpi32SetTextAlignment
:li.Gpi32SetViewingLimits
:li.Gpi32SetViewingTransformMatrix
.*POWERPC :li.Gpi32StrokeInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32StrokeInkPath (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32StrokePath
:li.Gpi32SuspendPlay
.*POWERPC :li.Gpi32TabbedCharStringAt (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Gpi32TabbedCharStringAt (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Gpi32Translate
:li.Gpi32UnloadFonts
:li.Gpi32UnloadPublicFonts
:li.Gpi32WCBitBlt
:eul.
.*
.* HLP_ENABMRG
.*
:h1 res=9059.Help for PMMERGE
:i2 refid=enab.PMMERGE
:p.Select the :hp2.PMMERGE:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMMERGE.DLL.
When the On button is selected, the APIs within PMMERGE.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMMERGE.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture.
:p.The following APIs are traced when PMMERGE is enabled for tracing:
:ul compact.
.*POWERPC :li.Win32QueryControlColors (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32QueryControlColors (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Win32QueryCpType (:link reftype=hd res=245.OS/2 2.45+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32QueryCpType (:link reftype=hd res=245.OS/2 2.45+:elink.)
.*POWERPC :li.Win32SetControlColors (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32SetControlColors (:link reftype=hd res=24.OS/2 2.40+:elink.)
:eul.
.*
.* HLP_ENABPIC
.*
:h1 res=9063.Help for PMPIC
:i2 refid=enab.PMPIC
:p.Select the :hp2.PMPIC:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMPIC.DLL.
When the On button is selected, the APIs within PMPIC.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMPIC.DLL are not traced
when the executable is run.
:p.The following APIs are traced when PMPIC is enabled for tracing:
:ul compact.
:li.Pic16Ichg
:li.Pic16Print
:li.Pic32Ichg
:li.Pic32Print
:eul.
.*
.* HLP_ENABSHL
.*
:h1 res=9067.Help for PMSHAPI
:i2 refid=enab.PMSHAPI
:p.Select the :hp2.PMSHAPI:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMSHAPI.DLL.
When the On button is selected, the APIs within PMSHAPI.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMSHAPI.DLL are not traced
when the executable is run.
:p.The following APIs are traced when PMSHAPI is enabled for tracing:
:ul compact.
:li.Prf16AddProgram
:li.Prf16ChangeProgram
:li.Prf16CloseProfile
:li.Prf16CreateGroup
:li.Prf16DestroyGroup
:li.Prf16OpenProfile
:li.Prf16QueryDefinition
:li.Prf16QueryProfile
:li.Prf16QueryProfileData
:li.Prf16QueryProfileInt
:li.Prf16QueryProfileSize
:li.Prf16QueryProfileString
:li.Prf16QueryProgramCategory
:li.Prf16QueryProgramHandle
:li.Prf16QueryProgramTitles
:li.Prf16RemoveProgram
:li.Prf16Reset
:li.Prf16WriteProfileData
:li.Prf16WriteProfileString
:li.Prf32AddProgram
:li.Prf32ChangeProgram
:li.Prf32CloseProfile
:li.Prf32DestroyGroup
:li.Prf32OpenProfile
:li.Prf32QueryDefinition
:li.Prf32QueryProfile
:li.Prf32QueryProfileData
:li.Prf32QueryProfileInt
:li.Prf32QueryProfileSize
:li.Prf32QueryProfileString
:li.Prf32QueryProgramTitles
:li.Prf32RemoveProgram
:li.Prf32Reset
:li.Prf32WriteProfileData
:li.Prf32WriteProfileString
:li.Win16AddProgram
:li.Win16AddSwitchEntry
:li.Win16AddToGroup
:li.Win16ChangeProgram
:li.Win16ChangeSwitchEntry
:li.Win16CreateGroup
:li.Win16CreateSwitchEntry
:li.Win16DestroyGroup
:li.Win16EndProgram
:li.Win16HAPPfromPID
:li.Win16HSWITCHfromHAPP
:li.Win16InstStartApp
:li.Win16NoShutdown
:li.Win16QueryDefinition
:li.Win16QueryProfileData
:li.Win16QueryProfileInt
:li.Win16QueryProfileSize
:li.Win16QueryProfileString
:li.Win16QueryProgramTitles
:li.Win16QueryProgramType
:li.Win16QueryProgramUse
:li.Win16QuerySessionTitle
:li.Win16QuerySwitchEntry
:li.Win16QuerySwitchHandle
:li.Win16QuerySwitchList
:li.Win16QueryTaskSizePos
:li.Win16QueryTaskTitle
:li.Win16RemoveFromGroup
:li.Win16RemoveProgram
:li.Win16RemoveSwitchEntry
:li.Win16SetFgndWindow
:li.Win16SetTitle
:li.Win16SetTitleAndHwndIcon (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win16SetTitleAndIcon
:li.Win16StartApp
:li.Win16StopProgram
:li.Win16SwitchProgramRegister
:li.Win16SwitchToProgram
:li.Win16TerminateApp
:li.Win16WriteProfileData
:li.Win16WriteProfileString
:li.Win32AddSwitchEntry
:li.Win32ChangeSwitchEntry
:li.Win32CreateSwitchEntry
:li.Win32QuerySessionTitle
:li.Win32QuerySwitchEntry
:li.Win32QuerySwitchHandle
:li.Win32QuerySwitchList
:li.Win32QueryTaskSizePos
:li.Win32QueryTaskTitle
:li.Win32RemoveSwitchEntry
:li.Win32StartApp
:li.Win32SwitchProgramRegister
:li.Win32SwitchToProgram
:li.Win32TerminateApp
:eul.
.*
.* HLP_ENABSPL
.*
:h1 res=9071.Help for PMSPL
:i2 refid=enab.PMSPL
:p.Select the :hp2.PMSPL:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMSPL.DLL.
When the On button is selected, the APIs within PMSPL.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMSPL.DLL are not traced
when the executable is run.
:p.The following APIs are traced when PMSPL is enabled for tracing:
:ul compact.
:li.Dos16PrintDestAdd
:li.Dos16PrintDestControl
:li.Dos16PrintDestDel
:li.Dos16PrintDestEnum
:li.Dos16PrintDestGetInfo
:li.Dos16PrintDestSetInfo
:li.Dos16PrintDriverEnum
:li.Dos16PrintJobContinue
:li.Dos16PrintJobDel
:li.Dos16PrintJobEnum
:li.Dos16PrintJobPause
:li.Dos16PrintJobGetInfo
:li.Dos16PrintJobGetId
:li.Dos16PrintJobSetInfo
:li.Dos16PrintPortEnum
:li.Dos16PrintQAdd
:li.Dos16PrintQContinue
:li.Dos16PrintQDel
:li.Dos16PrintQEnum
:li.Dos16PrintQGetInfo
:li.Dos16PrintQPause
:li.Dos16PrintQProcessorEnum
:li.Dos16PrintQPurge
:li.Dos16PrintQSetInfo
:li.Prt16Abort
:li.Prt16AbortDoc (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Prt16Close
:li.Prt16DevIOCtl
:li.Prt16Open
:li.Prt16Query (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Prt16ResetAbort (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Prt16Set (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Prt16Write
:li.Prt32Abort
.*POWERPC :li.Prt32AbortDoc (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Prt32AbortDoc (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Prt32Close
:li.Prt32DevIOCtl
.*POWERPC :li.Prt32NewPage (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Prt32NewPage (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Prt32Open
.*POWERPC :li.Prt32Query (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Prt32Query (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Prt32ResetAbort (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Prt32ResetAbort (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Prt32Set (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Prt32Set (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Prt32Write
:li.Spl16MessageBox
:li.Spl16QmAbort
:li.Spl16QmAbortDoc
:li.Spl16QmClose
:li.Spl16QmEndDoc
:li.Spl16QmOpen
:li.Spl16QmStartDoc
:li.Spl16QmWrite
:li.Spl16StdClose
:li.Spl16StdDelete
:li.Spl16StdGetBits
:li.Spl16StdOpen
:li.Spl16StdQueryLength
:li.Spl16StdStart
:li.Spl16StdStop
:li.Spl32ControlDevice
:li.Spl32CopyJob
:li.Spl32CreateDevice
.*POWERPC :li.Spl32CreatePort (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32CreatePort (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32CreateQueue
:li.Spl32DeleteDevice
:li.Spl32DeleteJob
.*POWERPC :li.Spl32DeletePort (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32DeletePort (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32DeleteQueue
.*POWERPC :li.Spl32Disable (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32Disable (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Spl32DisplayControlPanel (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32DisplayControlPanel (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Spl32Enable (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32Enable (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32EnumDevice
:li.Spl32EnumDriver
:li.Spl32EnumJob
:li.Spl32EnumPort
:li.Spl32EnumPrinter
:li.Spl32EnumQueue
:li.Spl32EnumQueueProcessor
.*POWERPC :li.Spl32GetControlPanelList (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32GetControlPanelList (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Spl32GetPortFromQ (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32GetPortFromQ (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32HoldJob
:li.Spl32HoldQueue
:li.Spl32MessageBox
.*POWERPC :li.Spl32ProtSendCmd (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32ProtSendCmd (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Spl32ProtXlateCmd (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32ProtXlateCmd (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32PurgeQueue
:li.Spl32QmAbort
:li.Spl32QmAbortDoc
:li.Spl32QmClose
:li.Spl32QmEndDoc
.*POWERPC :li.Spl32QmGetJobID (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32QmGetJobID (:link reftype=hd res=23.OS/2 2.30+:elink.)
.*POWERPC :li.Spl32QmNewPage (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32QmNewPage (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Spl32QmOpen
:li.Spl32QmStartDoc
:li.Spl32QmWrite
.*POWERPC :li.Spl32QueryControlPanel (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32QueryControlPanel (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32QueryDevice
:li.Spl32QueryDriver
:li.Spl32QueryJob
.*POWERPC :li.Spl32QueryPath (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32QueryPath (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Spl32QueryPort (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32QueryPort (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32QueryQueue
.*POWERPC :li.Spl32Register (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32Register (:link reftype=hd res=24.OS/2 2.40+:elink.)
.*POWERPC :li.Spl32RegisterControlPanel (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32RegisterControlPanel (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32ReleaseJob
:li.Spl32ReleaseQueue
:li.Spl32SetDevice
:li.Spl32SetDriver
:li.Spl32SetJob
.*POWERPC :li.Spl32SetPort (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32SetPort (:link reftype=hd res=24.OS/2 2.40+:elink.)
:li.Spl32SetQueue
:li.Spl32StdClose
:li.Spl32StdDelete
:li.Spl32StdGetBits
:li.Spl32StdOpen
:li.Spl32StdQueryLength
:li.Spl32StdStart
:li.Spl32StdStop
.*POWERPC :li.Spl32UnRegister (:link reftype=hd res=24.OS/2 2.40+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Spl32UnRegister (:link reftype=hd res=24.OS/2 2.40+:elink.)
:eul.
.*
.* HLP_ENABPVP
.*
:h1 res=9075.Help for PMVIOP
:i2 refid=enab.PMVIOP
:p.Select the :hp2.PMVIOP:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMVIOP.DLL.
When the On button is selected, the APIs within PMVIOP.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMVIOP.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture, as
.*POWERPC PMVIOP.DLL contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs are traced when PMVIOP is enabled for tracing:
:ul compact.
:li.Win16DefAVioWindowProc
:eul.
.*
.* HLP_ENABWIN
.*
:h1 res=9079.Help for PMWIN
:i2 refid=enab.PMWIN
:p.Select the :hp2.PMWIN:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMWIN.DLL.
When the On button is selected, the APIs within PMWIN.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMWIN.DLL are not traced
when the executable is run.
:note.The actual APIs traced within PMWIN.DLL are determined by the API
groups selected through the PMWIN APIs customization window, which provides
the same functionality as OS2TRACE's -W option.
:p.The following APIs are traced when PMWIN is enabled for tracing:
:ul compact.
:li.Gpi16LoadBitmap *
:li.Win16AddAtom
:li.Win16Alarm
:li.Win16AllocMem
:li.Win16AvailMem
:li.Win16BeginEnumWindows
:li.Win16BeginPaint
:li.Win16BroadcastMsg
:li.Win16CalcFrameRect
:li.Win16CallMsgFilter
:li.Win16CancelShutdown
:li.Win16Catch
:li.Win16CheckInput (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win16CloseClipbrd
:li.Win16CompareStrings
:li.Win16CopyAccelTable
:li.Win16CopyRect
:li.Win16CpTranslateChar
:li.Win16CpTranslateString
:li.Win16CreateAccelTable
:li.Win16CreateAtomTable
:li.Win16CreateCursor
:li.Win16CreateDlg
:li.Win16CreateFrameControls
:li.Win16CreateHeap
:li.Win16CreateMenu
:li.Win16CreateMsgQueue
:li.Win16CreatePointer
:li.Win16CreatePointerIndirect
:li.Win16CreateStdWindow
:li.Win16CreateWindow
:li.Win16DdeInitiate
:li.Win16DdePostMsg
:li.Win16DdeRespond
:li.Win16DefDlgProc
:li.Win16DefWindowProc
:li.Win16DeleteAtom
:li.Win16DeleteLibrary
:li.Win16DeleteProcedure
:li.Win16DestroyAccelTable
:li.Win16DestroyAtomTable
:li.Win16DestroyCursor
:li.Win16DestroyHeap
:li.Win16DestroyMsgQueue
:li.Win16DestroyPointer
:li.Win16DestroyWindow
:li.Win16DismissDlg
:li.Win16DispatchMsg
:li.Win16DlgBox
:li.Win16DrawBitmap
:li.Win16DrawBorder
:li.Win16DrawPointer
:li.Win16DrawText
:li.Win16EmptyClipbrd
:li.Win16EnablePhysInput
:li.Win16EnableWindow
:li.Win16EnableWindowUpdate
:li.Win16EndEnumWindows
:li.Win16EndPaint
:li.Win16EnumClipbrdFmts
:li.Win16EnumDlgItem
:li.Win16EqualRect
:li.Win16ExcludeUpdateRegion
:li.Win16FillRect
:li.Win16FindAtom
:li.Win16FlashWindow
:li.Win16FocusChange
:li.Win16FreeErrorInfo
:li.Win16FreeMem
:li.Win16GetClipPS
:li.Win16GetCurrentTime
:li.Win16GetDlgMsg
:li.Win16GetErasePS
:li.Win16GetErrorInfo
:li.Win16GetKeyState
:li.Win16GetLastError
:li.Win16GetMaxPosition
:li.Win16GetMinPosition
:li.Win16GetMsg
:li.Win16GetNextWindow
:li.Win16GetPhysKeyState
:li.Win16GetPS
:li.Win16GetScreenPS
:li.Win16GetSysBitmap
:li.Win16InflateRect
:li.Win16Initialize
:li.Win16InSendMsg
:li.Win16IntersectRect
:li.Win16InvalidateRect
:li.Win16InvalidateRegion
:li.Win16InvertRect
:li.Win16IsChild
:li.Win16IsPhysInputEnabled
:li.Win16IsRectEmpty
:li.Win16IsThreadActive
:li.Win16IsWindow
:li.Win16IsWindowEnabled
:li.Win16IsWindowShowing
:li.Win16IsWindowVisible
:li.Win16LoadAccelTable
:li.Win16LoadDlg
:li.Win16LoadLibrary
:li.Win16LoadMenu
:li.Win16LoadMessage
:li.Win16LoadPointer
:li.Win16LoadProcedure
:li.Win16LoadString
:li.Win16LockHeap
:li.Win16LockInput
:li.Win16LockPointerUpdate (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win16LockVisRegions
:li.Win16LockWindow
:li.Win16LockWindowUpdate
:li.Win16MakePoints
:li.Win16MakeRect
:li.Win16MapDlgPoints
:li.Win16MapWindowPoints
:li.Win16MessageBox
:li.Win16MsgMuxSemWait
:li.Win16MsgSemWait
:li.Win16MultWindowFromIDs
:li.Win16NextChar
:li.Win16OffsetRect
:li.Win16OpenClipbrd
:li.Win16OpenWindowDC
:li.Win16PeekMsg
:li.Win16PopupMenu
:li.Win16PostMsg
:li.Win16PostQueueMsg
:li.Win16PrevChar
:li.Win16ProcessDlg
:li.Win16PtInRect
:li.Win16QueryAccelTable
:li.Win16QueryActiveWindow
:li.Win16QueryAnchorBlock
:li.Win16QueryAtomLength
:li.Win16QueryAtomName
:li.Win16QueryAtomUsage
:li.Win16QueryCapture
:li.Win16QueryClassInfo
:li.Win16QueryClassName
:li.Win16QueryClassThunkProc
:li.Win16QueryClipbrdData
:li.Win16QueryClipbrdFmtInfo
:li.Win16QueryClipbrdOwner
:li.Win16QueryClipbrdViewer
:li.Win16QueryCp
:li.Win16QueryCpList
:li.Win16QueryCursorInfo
:li.Win16QueryDesktopBkgnd
:li.Win16QueryDesktopWindow
:li.Win16QueryDlgItemShort
:li.Win16QueryDlgItemText
:li.Win16QueryDlgItemTextLength
:li.Win16QueryFocus
:li.Win16QueryMsgPos
:li.Win16QueryMsgTime
:li.Win16QueryObjectWindow
:li.Win16QueryPointer
:li.Win16QueryPointerInfo
:li.Win16QueryPointerPos
:li.Win16QueryPresParam
:li.Win16QueryQueueInfo
:li.Win16QueryQueueStatus
:li.Win16QuerySendMsg
:li.Win16QuerySysColor
:li.Win16QuerySysModalWindow
:li.Win16QuerySysPointer
:li.Win16QuerySystemAtomTable
:li.Win16QuerySysValue
:li.Win16QueryUpdateRect
:li.Win16QueryUpdateRegion
:li.Win16QueryVersion
:li.Win16QueryWindow
:li.Win16QueryWindowDC
:li.Win16QueryWindowLockCount
:li.Win16QueryWindowModel
:li.Win16QueryWindowPos
:li.Win16QueryWindowProcess
:li.Win16QueryWindowPtr
:li.Win16QueryWindowRect
:li.Win16QueryWindowText
:li.Win16QueryWindowTextLength
:li.Win16QueryWindowThunkProc
:li.Win16QueryWindowULong
:li.Win16QueryWindowUShort
:li.Win16QueueFromID
:li.Win16RealizePalette
:li.Win16ReallocMem
:li.Win16RegisterClass
:li.Win16RegisterUserDatatype
:li.Win16RegisterUserMsg
:li.Win16RegisterWindowDestroy
:li.Win16ReleaseErasePS
:li.Win16ReleaseHook
:li.Win16ReleasePS
:li.Win16RemovePresParam
:li.Win16ReplyMsg
:li.Win16SaveWindowPos
:li.Win16ScrollWindow
:li.Win16SendDlgItemMsg
:li.Win16SendMsg
:li.Win16SetAccelTable
:li.Win16SetActiveWindow
:li.Win16SetCapture
:li.Win16SetClassMsgInterest
:li.Win16SetClassThunkProc
:li.Win16SetClipbrdData
:li.Win16SetClipbrdOwner
:li.Win16SetClipbrdViewer
:li.Win16SetCp
:li.Win16SetDesktopBkgnd
:li.Win16SetDlgItemShort
:li.Win16SetDlgItemText
:li.Win16SetErrorInfo
:li.Win16SetFocus
:li.Win16SetHook
:li.Win16SetKeyboardStateTable
:li.Win16SetMsgInterest
:li.Win16SetMsgMode
:li.Win16SetMultWindowPos
:li.Win16SetOwner
:li.Win16SetParent
:li.Win16SetPointer
:li.Win16SetPointerOwner
:li.Win16SetPointerPos
:li.Win16SetPresParam
:li.Win16SetRect
:li.Win16SetRectEmpty
:li.Win16SetSynchroMode
:li.Win16SetSysColors
:li.Win16SetSysModalWindow
:li.Win16SetSysValue
:li.Win16SetWindowBits
:li.Win16SetWindowPos
:li.Win16SetWindowPtr
:li.Win16SetWindowText
:li.Win16SetWindowThunkProc
:li.Win16SetWindowULong
:li.Win16SetWindowUShort
:li.Win16ShowCursor
:li.Win16ShowPointer
:li.Win16ShowTrackRect
:li.Win16ShowWindow
:li.Win16StartTimer
:li.Win16StopTimer
:li.Win16StretchPointer
:li.Win16SubclassWindow
:li.Win16SubstituteStrings
:li.Win16SubtractRect
:li.Win16Terminate
:li.Win16ThreadAssocQueue
:li.Win16Throw
:li.Win16TrackRect
:li.Win16TranslateAccel
:li.Win16UnionRect
:li.Win16UpdateWindow
:li.Win16Upper
:li.Win16UpperChar
:li.Win16ValidateRect
:li.Win16ValidateRegion
:li.Win16WaitMsg
:li.Win16WakeThread
:li.Win16WindowFromDC
:li.Win16WindowFromID
:li.Win16WindowFromPoint
:li.Win32AddAtom
:li.Win32Alarm
:li.Win32BeginEnumWindows
:li.Win32BeginPaint
:li.Win32BroadcastMsg
:li.Win32CalcFrameRect
:li.Win32CallMsgFilter
:li.Win32CancelShutdown
:li.Win32CheckInput (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32CloseClipbrd
:li.Win32CompareStrings
:li.Win32CopyAccelTable
:li.Win32CopyRect
:li.Win32CpTranslateChar
:li.Win32CpTranslateString
:li.Win32CreateAccelTable
:li.Win32CreateAtomTable
:li.Win32CreateCursor
:li.Win32CreateDlg
:li.Win32CreateFrameControls
:li.Win32CreateMenu
:li.Win32CreateMsgQueue
:li.Win32CreatePointer
:li.Win32CreatePointerIndirect
:li.Win32CreateStdWindow
:li.Win32CreateWindow
:li.Win32DdeInitiate
:li.Win32DdePostMsg
:li.Win32DdeRespond
:li.Win32DefDlgProc
:li.Win32DefWindowProc
:li.Win32DeleteAtom
:li.Win32DeleteLibrary
:li.Win32DeleteProcedure
:li.Win32DestroyAccelTable
:li.Win32DestroyAtomTable
:li.Win32DestroyCursor
:li.Win32DestroyMsgQueue
:li.Win32DestroyPointer
:li.Win32DestroyWindow
:li.Win32DismissDlg
:li.Win32DispatchMsg
:li.Win32DlgBox
:li.Win32DrawBitmap
:li.Win32DrawBorder
:li.Win32DrawPointer
:li.Win32DrawText
:li.Win32EmptyClipbrd
:li.Win32EnablePhysInput
:li.Win32EnableWindow
:li.Win32EnableWindowUpdate
:li.Win32EndEnumWindows
:li.Win32EndPaint
:li.Win32EnumClipbrdFmts
:li.Win32EnumDlgItem
:li.Win32EqualRect
:li.Win32ExcludeUpdateRegion
:li.Win32FillRect
:li.Win32FindAtom
:li.Win32FlashWindow
:li.Win32FocusChange
:li.Win32FreeErrorInfo
:li.Win32GetClipPS
:li.Win32GetCurrentTime
:li.Win32GetDlgMsg
:li.Win32GetErasePS
:li.Win32GetErrorInfo
:li.Win32GetKeyState
:li.Win32GetLastError
:li.Win32GetMaxPosition
:li.Win32GetMinPosition
:li.Win32GetMsg
:li.Win32GetNextWindow
:li.Win32GetPhysKeyState
:li.Win32GetPS
:li.Win32GetScreenPS
:li.Win32GetSysBitmap
:li.Win32InflateRect
:li.Win32Initialize
:li.Win32InSendMsg
:li.Win32IntersectRect
:li.Win32InvalidateRect
:li.Win32InvalidateRegion
:li.Win32InvertRect
:li.Win32IsChild
:li.Win32IsPhysInputEnabled
:li.Win32IsRectEmpty
:li.Win32IsThreadActive
:li.Win32IsWindow
:li.Win32IsWindowEnabled
:li.Win32IsWindowShowing
:li.Win32IsWindowVisible
:li.Win32LoadAccelTable
:li.Win32LoadDlg
:li.Win32LoadLibrary
:li.Win32LoadMenu
:li.Win32LoadMessage
:li.Win32LoadPointer
:li.Win32LoadProcedure
:li.Win32LoadString
:li.Win32LockInput
:li.Win32LockPointerUpdate (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32LockVisRegions
:li.Win32LockWindowUpdate
:li.Win32MakePoints
:li.Win32MakeRect
:li.Win32MapDlgPoints
:li.Win32MapWindowPoints
:li.Win32MessageBox
.*POWERPC :li.Win32MessageBox2 (:link reftype=hd res=23.OS/2 2.30+:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32MessageBox2 (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32MsgMuxSemWait
:li.Win32MsgSemWait
:li.Win32MultWindowFromIDs
:li.Win32NextChar
:li.Win32OffsetRect
:li.Win32OpenClipbrd
:li.Win32OpenWindowDC
:li.Win32PeekMsg
:li.Win32PopupMenu
:li.Win32PostMsg
:li.Win32PostQueueMsg
:li.Win32PrevChar
:li.Win32ProcessDlg
:li.Win32PtInRect
:li.Win32QueryAccelTable
:li.Win32QueryActiveWindow
:li.Win32QueryAnchorBlock
:li.Win32QueryAtomLength
:li.Win32QueryAtomName
:li.Win32QueryAtomUsage
:li.Win32QueryCapture
:li.Win32QueryClassInfo
:li.Win32QueryClassName
:li.Win32QueryClassThunkProc
:li.Win32QueryClipbrdData
:li.Win32QueryClipbrdFmtInfo
:li.Win32QueryClipbrdOwner
:li.Win32QueryClipbrdViewer
.*POWERPC :li.Win32QueryClipRegion (:link reftype=hd res=24.OS/2 2.40:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32QueryClipRegion (:link reftype=hd res=24.OS/2 2.40:elink.)
:li.Win32QueryCp
:li.Win32QueryCpList
:li.Win32QueryCursorInfo
:li.Win32QueryDesktopBkgnd
:li.Win32QueryDesktopWindow
:li.Win32QueryDlgItemShort
:li.Win32QueryDlgItemText
:li.Win32QueryDlgItemTextLength
:li.Win32QueryFocus
:li.Win32QueryMsgPos
:li.Win32QueryMsgTime
:li.Win32QueryObjectWindow
:li.Win32QueryPointer
:li.Win32QueryPointerInfo
:li.Win32QueryPointerPos
:li.Win32QueryPresParam
:li.Win32QueryQueueInfo
:li.Win32QueryQueueStatus
:li.Win32QuerySendMsg
:li.Win32QuerySysColor
:li.Win32QuerySysModalWindow
:li.Win32QuerySysPointer
:li.Win32QuerySysPointerData (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32QuerySystemAtomTable
:li.Win32QuerySysValue
:li.Win32QueryUpdateRect
:li.Win32QueryUpdateRegion
:li.Win32QueryVersion
:li.Win32QueryVisibleRegion (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32QueryWindow
:li.Win32QueryWindowDC
:li.Win32QueryWindowModel
:li.Win32QueryWindowPos
:li.Win32QueryWindowProcess
:li.Win32QueryWindowPtr
:li.Win32QueryWindowRect
:li.Win32QueryWindowText
:li.Win32QueryWindowTextLength
:li.Win32QueryWindowThunkProc
:li.Win32QueryWindowULong
:li.Win32QueryWindowUShort
:li.Win32QueueFromID
:li.Win32RealizePalette
:li.Win32RegisterClass
:li.Win32RegisterUserDatatype
:li.Win32RegisterUserMsg
:li.Win32ReleaseErasePS
:li.Win32ReleaseHook
:li.Win32ReleasePS
:li.Win32RemovePresParam
:li.Win32ReplyMsg
:li.Win32RequestMutexSem
:li.Win32SaveWindowPos
:li.Win32ScrollWindow
:li.Win32SendDlgItemMsg
:li.Win32SendMsg
:li.Win32SetAccelTable
:li.Win32SetActiveWindow
:li.Win32SetCapture
:li.Win32SetClassMsgInterest
:li.Win32SetClassThunkProc
:li.Win32SetClipbrdData
:li.Win32SetClipbrdOwner
:li.Win32SetClipbrdViewer
.*POWERPC :li.Win32SetClipRegion (:link reftype=hd res=24.OS/2 2.40:elink.,:link reftype=hd res=11.Intel only:elink.)
:li.Win32SetClipRegion (:link reftype=hd res=24.OS/2 2.40:elink.)
:li.Win32SetCp
:li.Win32SetDesktopBkgnd
:li.Win32SetDlgItemShort
:li.Win32SetDlgItemText
:li.Win32SetErrorInfo
:li.Win32SetFocus
:li.Win32SetHook
:li.Win32SetKeyboardStateTable
:li.Win32SetMsgInterest
:li.Win32SetMsgMode
:li.Win32SetMultWindowPos
:li.Win32SetOwner
:li.Win32SetParent
:li.Win32SetPointer
:li.Win32SetPointerOwner
:li.Win32SetPointerPos
:li.Win32SetPresParam
:li.Win32SetRect
:li.Win32SetRectEmpty
:li.Win32SetSynchroMode
:li.Win32SetSysColors
:li.Win32SetSysModalWindow
:li.Win32SetSysPointerData (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32SetSysValue
:li.Win32SetVisibleRegionNotify (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32SetWindowBits
:li.Win32SetWindowPos
:li.Win32SetWindowPtr
:li.Win32SetWindowText
:li.Win32SetWindowThunkProc
:li.Win32SetWindowULong
:li.Win32SetWindowUShort
:li.Win32ShowCursor
:li.Win32ShowPointer
:li.Win32ShowTrackRect
:li.Win32ShowWindow
:li.Win32StartTimer
:li.Win32StopTimer
:li.Win32StretchPointer
:li.Win32SubclassWindow
:li.Win32SubstituteStrings
:li.Win32SubtractRect
:li.Win32Terminate
:li.Win32ThreadAssocQueue
:li.Win32TrackRect
:li.Win32TranslateAccel
:li.Win32UnionRect
:li.Win32UpdateWindow
:li.Win32Upper
:li.Win32UpperChar
:li.Win32ValidateRect
:li.Win32ValidateRegion
:li.Win32WaitEventSem
:li.Win32WaitMsg
:li.Win32WaitMuxWaitSem
:li.Win32WakeThread
:li.Win32WindowFromDC
:li.Win32WindowFromID
:li.Win32WindowFromPoint
:eul.
:p.*
Although Gpi16LoadBitmap physically resides within PMWIN.DLL, the API itself is
part of the PMGPI bitmap API group.
As a result, PMWIN must be enabled for tracing AND the PMGPI bitmap API group
must be selected in order to trace Gpi16LoadBitmap.
:note.The following OS/2 1.x APIs are not traced when PMWIN is enabled for
tracing because they are only required by COBOL and FORTRAN, and are therefore
not present within the OS/2 2.x version of PMWIN.DLL:
:ul compact.
:li.Win16CreateDataStructure
:li.Win16DestroyDataStructure
:li.Win16FreeMsg
:li.Win16ModifyDataStructure
:li.Win16QueryBits
:li.Win16QueryBitsUnderMask
:li.Win16QueryDataStructure
:li.Win16QueryValue
:li.Win16SetBits
:li.Win16SetBitsUnderMask
:eul.
.*
.* HLP_ENABWP
.*
:h1 res=9083.Help for PMWP
:i2 refid=enab.PMWP
:p.Select the :hp2.PMWP:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from PMWP.DLL.
When the On button is selected, the APIs within PMWP.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within PMWP.DLL are not traced
when the executable is run.
:p.The following APIs are traced when PMWP is enabled for tracing:
:ul compact.
:li.Win32CopyObject (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32CreateObject
:li.Win32CreateShadow (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32DeregisterObjectClass
:li.Win32DestroyObject
:li.Win32EnumObjectClasses
:li.Win32FreeFileIcon
:li.Win32IsSOMDDReady (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32IsWPDServerReady (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32LoadFileIcon
:li.Win32LockupSystem (:link reftype=hd res=21.OS/2 2.10+:elink.)
:li.Win32MoveObject (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32OpenObject (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32QueryActiveDesktopPathname (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32QueryObject
:li.Win32QueryObjectPath (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32RegisterObjectClass
:li.Win32ReplaceObjectClass
:li.Win32RestartSOMDD (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32RestartWPDServer (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32RestoreWindowPos
:li.Win32SaveObject (:link reftype=hd res=23.OS/2 2.30+:elink.)
:li.Win32SetFileIcon
:li.Win32SetObjectData
:li.Win32ShutdownSystem
:li.Win32StoreWindowPos
:li.Win32UnlockSystem (:link reftype=hd res=21.OS/2 2.10+:elink.)
:eul.
.*
.* HLP_ENABQUE
.*
:h1 res=9087.Help for QUECALLS
:i2 refid=enab.QUECALLS
:p.Select the :hp2.QUECALLS:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from QUECALLS.DLL.
When the On button is selected, the APIs within QUECALLS.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within QUECALLS.DLL are not traced
when the executable is run.
:p.The following APIs are traced when QUECALLS is enabled for tracing:
:ul compact.
:li.Dos16CloseQueue
:li.Dos16CreateQueue
:li.Dos16OpenQueue
:li.Dos16PeekQueue
:li.Dos16PurgeQueue
:li.Dos16QueryQueue
:li.Dos16ReadQueue
:li.Dos16WriteQueue
:li.Dos32CloseQueue
:li.Dos32CreateQueue
:li.Dos32OpenQueue
:li.Dos32PeekQueue
:li.Dos32PurgeQueue
:li.Dos32QueryQueue
:li.Dos32ReadQueue
:li.Dos32WriteQueue
:eul.
.*
.* HLP_ENABSES
.*
:h1 res=9091.Help for SESMGR
:i2 refid=enab.SESMGR
:p.Select the :hp2.SESMGR:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from SESMGR.DLL.
When the On button is selected, the APIs within SESMGR.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within SESMGR.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture.
.*POWERPC To obtain the same functionality on the PowerPC architecture, enable tracing of
.*POWERPC DOSCALLS.DLL and select the DOSCALLS SES API group.
:p.The following APIs are traced when SESMGR is enabled for tracing:
:ul compact.
:li.Dos16SelectSession
:li.Dos16SetSession
:li.Dos16SMRegisterDD
:li.Dos16StartSession
:li.Dos16StopSession
.*POWERPC :li.Dos32SelectSession (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SelectSession
.*POWERPC :li.Dos32SetSession (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32SetSession
.*POWERPC :li.Dos32StartSession (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32StartSession
.*POWERPC :li.Dos32StopSession (:link reftype=hd res=11.Intel only:elink.)
:li.Dos32StopSession
:eul.
.*
.* HLP_ENABVIO
.*
:h1 res=9095.Help for VIOCALLS
:i2 refid=enab.VIOCALLS
:p.Select the :hp2.VIOCALLS:ehp2. On or Off button to enable or disable the
tracing of APIs imported by the executable file from VIOCALLS.DLL.
When the On button is selected, the APIs within VIOCALLS.DLL are traced when
the executable is run.
When the Off button is selected, the APIs within VIOCALLS.DLL are not traced
when the executable is run.
.*POWERPC :note.Selecting this DLL is only effective on the Intel architecture, as
.*POWERPC VIOCALLS.DLL contains only 16-bit APIs and the PowerPC architecture does not
.*POWERPC support any 16-bit APIs.
:p.The following APIs are traced when VIOCALLS is enabled for tracing:
:ul compact.
:li.Vio16Associate
:li.Vio16CheckCharType
:li.Vio16CreateLogFont
:li.Vio16CreatePS
:li.Vio16DeleteSetId
:li.Vio16DeRegister
:li.Vio16DestroyPS
:li.Vio16EndPopUp
:li.Vio16GetAnsi
:li.Vio16GetBuf
:li.Vio16GetConfig
:li.Vio16GetCp
:li.Vio16GetCurPos
:li.Vio16GetCurType
:li.Vio16GetDeviceCellSize
:li.Vio16GetFont
:li.Vio16GetMode
:li.Vio16GetOrg
:li.Vio16GetPhysBuf
:li.Vio16GetState
:li.Vio16GlobalReg
:li.Vio16ModeUndo
:li.Vio16ModeWait
:li.Vio16PopUp
:li.Vio16PrtSc
:li.Vio16PrtScToggle
:li.Vio16QueryFonts
:li.Vio16QuerySetIds
:li.Vio16ReadCellStr
:li.Vio16ReadCharStr
:li.Vio16RedrawSize
:li.Vio16Register
:li.Vio16SavRedrawUndo
:li.Vio16SavRedrawWait
:li.Vio16ScrLock
:li.Vio16ScrollDn
:li.Vio16ScrollLf
:li.Vio16ScrollRt
:li.Vio16ScrollUp
:li.Vio16ScrUnLock
:li.Vio16SetAnsi
:li.Vio16SetCp
:li.Vio16SetCurPos
:li.Vio16SetCurType
:li.Vio16SetDeviceCellSize
:li.Vio16SetFont
:li.Vio16SetMode
:li.Vio16SetOrg
:li.Vio16SetState
:li.Vio16ShowBuf
:li.Vio16ShowPS
:li.Vio16WrtCellStr
:li.Vio16WrtCharStr
:li.Vio16WrtCharStrAtt
:li.Vio16WrtNAttr
:li.Vio16WrtNCell
:li.Vio16WrtNChar
:li.Vio16WrtTTY
:eul.
.*
.* HLP_ENABOK
.*
:h1 res=9098.Help for OK
:p.Select :hp2.OK:ehp2. to save any trace enablement changes to the executable
file and exit this window.
If the launch .EXE/.COM files option is enabled (checked) and the executable
file is a .EXE or .COM, a dialog is presented for launching the executable.
.*
.* HLP_ENABCANCEL
.*
:h1 res=9100.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any trace
enablement changes to the executable file.
.*
.* HLP_ENABALLON
.*
:h1 res=9102.Help for All On
:p.Select :hp2.All On:ehp2. to enable API tracing for all supported DLLs
imported by the executable file.
.*
.* HLP_ENABALLOFF
.*
:h1 res=9104.Help for All Off
:p.Select :hp2.All Off:ehp2. to disable API tracing for all supported DLLs
imported by the executable file.
.*
.* HLP_ENABUNDO
.*
:h1 res=9106.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the trace enablement states of the
supported DLLs to those which were active before this window was displayed.
.*
.* PNL_DLG_CTL
.*
:h1 res=17003.Help for Control
:p.View the paused or resumed trace-enabled applications, if any.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=17007.
:dd.OK
:elink.
:edl.
.*
.* HLP_CTLMLE
.*
:h1 res=17005.Help for Control
:i2 refid=ctlmle.CTLMLE
:p.Use the scroll bars to view the paused or resumed trace-enabled
applications, if any, not visible within the multi-line entry field.
.*
.* HLP_CTLDLGOK
.*
:h1 res=17007.Help for OK
:p.Select :hp2.OK:ehp2. to exit this window.
.*
.* PNL_DLG_SUM
.*
:h1 res=10003.Help for Summarization
:p.View and optionally save the summary of the tracing of OS/2 APIs logged in
the trace information file.
:note.A warning message is issued if summarization information may not be
accurate due to lost API entries/exits, which result from pausing/resuming
trace and/or entry/exit information greater than log file wrapping length.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=10007.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=10009.
:dd.Save As...
:elink.
:edl.
.*
.* HLP_SUMMLE
.*
:h1 res=10005.Help for Summarization
:i2 refid=summle.SUMMLE
:p.Use the scroll bars to view the trace information summarization not visible
within the multi-line entry field.
.*
.* HLP_SUMOK
.*
:h1 res=10007.Help for OK
:p.Select :hp2.OK:ehp2. to exit this window.
.*
.* HLP_SUMSAVEAS
.*
:h1 res=10009.Help for Save As...
:p.Select :hp2.Save As...:ehp2. to save any trace information summarization
to a file.
.*
.* PNL_DLG_EXE
.*
:h1 res=11003.Help for Launch
:p.Enter any command line parameters to be passed to the .EXE/.COM in the entry
field.
If specifying more than one parameter, the parameters must be separated by
blanks.
Select :hp2.Foreground:ehp2. to start the .EXE/.COM in the foreground or select
:hp2.Background:ehp2. to start the .EXE/.COM in the background.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=11013.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=11015.
:dd.Cancel
:elink.
:edl.
.*
.* HLP_EXEPARM
.*
:h1 res=11006.Help for Parameters
:p.Enter any command line parameters to be passed to the .EXE/.COM.
If specifying more than one parameter, the parameters must be separated by
blanks.
.*
.* HLP_EXEFGSES
.*
:h1 res=11009.Help for Foreground Session
:p.Select :hp2.Foreground:ehp2. to start the .EXE/.COM in the foreground.
.*
.* HLP_EXEBGSES
.*
:h1 res=11011.Help for Background Session
:p.Select :hp2.Background:ehp2. to start the .EXE/.COM in the background.
.*
.* HLP_EXEOK
.*
:h1 res=11013.Help for OK
:p.Select :hp2.OK:ehp2. to exit this window and launch the .EXE/.COM.
.*
.* HLP_EXECANCEL
.*
:h1 res=11015.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without launching the
&period.EXE/.COM.
.*
.* PNL_DLG_OPTSAPI
.*
:h1 res=13002.Help for API Monitoring
:p.Select :hp2.Do not monitor number of APIs:ehp2. to disable the monitoring of
APIs.
Select :hp2.Update number of APIs every n APIs:ehp2. to specify the API
monitoring frequency (the rate at which the number of APIs is updated), using
the spin button to choose the frequency which is a multiple of 25 between 25
and 1000000, inclusive.
:note.If API monitoring is enabled, smaller monitoring frequencies result in
greater performance impacts to the application being traced as more API
monitoring information packets are passed between the application and PMOS2TRC.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=13010.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=13012.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=13014.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=13016.
:dd.Undo
:elink.
:edl.
:p.For more information on API monitoring, select :hp2.More on API monitoring
:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=607.
:dd.More on API monitoring
:elink.
:edl.
.*
.* HLP_APIOK
.*
:h1 res=13010.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the API monitoring settings and
exit this window.
.*
.* HLP_APICANCEL
.*
:h1 res=13012.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the API monitoring settings.
.*
.* HLP_APIDEFAULT
.*
:h1 res=13014.Help for Default
:p.Select :hp2.Default:ehp2. to change the API monitoring settings to those
that were active when the OS/2 API Trace application was first installed.
.*
.* HLP_APIUNDO
.*
:h1 res=13016.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the API monitoring settings to those that
were active before this window was displayed.
.*
.* PNL_DLG_OPTSLEN
.*
:h1 res=14002.Help for File Length Monitoring
:p.Select :hp2.Do not monitor log file length:ehp2. to disable the monitoring
of file length.
Select :hp2.Update log file length every n:ehp2. to specify the file length
monitoring frequency (the rate at which the log file length is updated), using
the spin buttons to choose the frequency.
If the right spin button is "KB", the values on the left spin button
represent kilobytes and are multiples of 4 between 4 and 65536, inclusive.
If the right spin button is "MB", the values on the left spin button represent
megabytes and are between 1 and 64, inclusive.
:note.When changing from kilobytes to megabytes, the frequency is rounded up to
the nearest megabyte multiple.
:note.If file length monitoring is enabled, smaller monitoring frequencies
result in greater performance impacts to the application being traced as more
file length monitoring information packets are passed between the application
and PMOS2TRC.
:p.For a detailed explanation of each button, select from the list below:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=14010.
:dd.OK
:elink.
:dt.o
:link reftype=hd res=14012.
:dd.Cancel
:elink.
:dt.o
:link reftype=hd res=14014.
:dd.Default
:elink.
:dt.o
:link reftype=hd res=14016.
:dd.Undo
:elink.
:edl.
:p.For more information on file length monitoring, select :hp2.More on file
length monitoring:ehp2. below.
:p.Related Information:
:dl compact tsize=3.
:dt.o
:link reftype=hd res=609.
:dd.More on file length monitoring
:elink.
:edl.
.*
.* HLP_APIOK
.*
:h1 res=14010.Help for OK
:p.Select :hp2.OK:ehp2. to save any changes to the file length monitoring
settings and exit this window.
.*
.* HLP_APICANCEL
.*
:h1 res=14012.Help for Cancel
:p.Select :hp2.Cancel:ehp2. to exit this window without saving any changes to
the file length monitoring settings.
.*
.* HLP_APIDEFAULT
.*
:h1 res=14014.Help for Default
:p.Select :hp2.Default:ehp2. to change the file length monitoring settings to
those that were active when the OS/2 API Trace application was first installed.
.*
.* HLP_APIUNDO
.*
:h1 res=14016.Help for Undo
:p.Select :hp2.Undo:ehp2. to restore the file length monitoring settings to
those that were active before this window was displayed.
.*
:euserdoc.

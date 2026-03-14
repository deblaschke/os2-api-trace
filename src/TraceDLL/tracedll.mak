################################################################################
#                                                                              #
#  File name    : TRACEDLL.MAK                                                 #
#                                                                              #
#  Title        : OS/2 API trace DLL makefile                                  #
#                                                                              #
#  Author       : Dave Blaschke                                                #
#                 deblaschke@yahoo.com		                               #
#                                                                              #
#  History      : Version Date    Item                                         #
#                 2.30.00 12Jun95 Created (supported LX format  and 32-bit Dos #
#                                 APIs only)                                   #
#                 2.30.03 20Jul95 Added 32-bit Win APIs                        #
#                 2.30.05 04Aug95 Added 32-bit Gpi APIs                        #
#                 2.30.10 13Aug95 Added 32-bit Drg APIs                        #
#                 2.30.13 27Aug95 Added OS/2 for PowerPC support               #
#                 2.30.14 29Aug95 Added 32-bit Prt and Spl APIs                #
#                 2.30.15 29Aug95 Added 32-bit Pic APIs                        #
#                 2.30.29 21Jun96 Fixed Dos32UnwindException bug               #
#                 2.40.00 05Sep96 Added new OS/2 2.40 APIs                     #
#                 2.40.03 11Mar97 Converted to IBM VisualAge C++               #
#                 2.40.17 21Apr97 Added 16-bit Dos APIs                        #
#                 2.40.18 30Apr97 Added 16-bit Kbd APIs                        #
#                 2.40.19 01May97 Added 16-bit Mou APIs                        #
#                 2.40.20 05May97 Added 16-bit Vio APIs                        #
#                 2.40.21 20May97 Fixed/minimized 16-bit stack usage           #
#                 2.40.26 24Jun97 Added 16-bit Win APIs                        #
#                 2.40.27 22Jul97 Added 16-bit Gpi APIs                        #
#                 2.40.29 07Aug97 Added 16-bit Drg APIs                        #
#                 2.40.30 08Aug97 Added 16-bit Pic APIs                        #
#                 2.40.32 11Aug97 Added 16-bit Prf APIs                        #
#                 2.40.33 11Aug97 Added 16-bit Prt APIs                        #
#                 2.40.47 29Jan98 Added Win16DefAVioWindowProc                 #
#                 2.40.63 19Aug98 Fixed -F option                              #
#                 2.40.75 28Oct98 Added Gpi/Win APIs exported from PMBIDI      #
#                 2.40.78 16Nov98 Added miscellaneous Dos APIs                 #
#                 2.45.00 15Apr99 Added new OS/2 2.45 APIs                     #
#                 2.45.31 26Jun01 Added  capability  of building  32-bit  only #
#                                 trace DLLs                                   #
#                 2.45.32 28Jun01 Added -U option                              #
#                                                                              #
################################################################################

source  = .

!if ( "$(T_16BIT)" == "ON" )
def_ext = I16
flags16 = /DT_16BIT
!else
def_ext = I32
flags16 =
!endif

################################################################################
#                                                                              #
#  Assembler and options                                                       #
#                                                                              #
################################################################################

assemble = MASM
!if ( "$(DEBUG)" == "ON" )
aflags   = /Ml /Zi /DDEBUG /DINTEL $(flags16) /DT_MINSTACK /DT_USERHOOK
!else
aflags   = /Ml /DINTEL $(flags16) /DT_MINSTACK /DT_USERHOOK
!endif

################################################################################
#                                                                              #
#  Compiler and options                                                        #
#                                                                              #
################################################################################

compile = ICC
!if ( "$(DEBUG)" == "ON" )
cflags_dll = /C+ /Gd- /Ge- /Gm+ /I. /I.. /O+ /Op- /Se /Ti+ /Wpro /DDEBUG /DINTEL /DOS2VER=245 $(flags16) /DT_MINSTACK /DT_USERHOOK /DT_COMRUNTIME
cflags_exe = /C+ /Gd- /Ge+ /Gm+ /O+ /Sa /Ti+ /Wpro /DDEBUG /DINTEL
!else
cflags_dll = /C+ /Gd- /Ge- /Gm+ /I. /I.. /O+ /Op- /Se /Wpro /DINTEL /DOS2VER=245 $(flags16) /DT_MINSTACK /DT_USERHOOK /DT_COMRUNTIME
cflags_exe = /C+ /Gd- /Ge+ /Gm+ /O+ /Sa /Wpro /DINTEL
!endif

################################################################################
#                                                                              #
#  Linker and options                                                          #
#                                                                              #
################################################################################

link    = ILINK
!if ( "$(DEBUG)" == "ON" )
lflags_dll = /NOFREEFORMAT /ALIGNMENT:16 /DEBUG /EXEPACK /MAP /NOEXTDICTIONARY /NOIGNORECASE /PACKCODE
lflags_exe = /NOFREEFORMAT /ALIGNMENT:16 /BASE:0x10000 /DEBUG /EXEPACK /MAP /NOIGNORECASE /STACK:0x8000
!else
lflags_dll = /NOFREEFORMAT /ALIGNMENT:16 /EXEPACK /MAP /NOEXTDICTIONARY /NOIGNORECASE /PACKCODE
lflags_exe = /NOFREEFORMAT /ALIGNMENT:16 /BASE:0x10000 /EXEPACK /MAP /NOIGNORECASE /STACK:0x8000
!endif

################################################################################
#                                                                              #
#  Dependencies and rules                                                      #
#                                                                              #
################################################################################

all         : markobj.exe t_common.lib t_ctl_.dll t_dos___.dll t_drg_.dll\
              t_gpi.dll t_hlp__.dll t_kbd___.dll t_m.dll t_mon___.dll\
              t_mou___.dll t_mrg__.dll t_n.dll t_pbd_.dll t_pic.dll\
              t_pip___.dll t_pvp_.dll t_que___.dll t_ses_.dll t_shl__.dll\
              t_spl.dll t_vio___.dll t_win.dll t_wp.dll

markobj.exe : markobj.obj $(source)\markobj.def
        $(link) $(lflags_exe) $*.obj, $*.exe, $*.map, , $(source)\$*.def
        mapsym $*.map

markobj.obj : $(source)\markobj.c $(source)\tracedll.mak
        $(compile) $(cflags_exe) /Fo$*.obj $(source)\$*.c

t_common.lib: t_common.dll $(source)\t_common.386
        implib /NOIGNORECASE t_common.lib $(source)\t_common.386

t_common.dll: t_common.obj t_com386.obj $(source)\t_common.386
        $(link) $(lflags_dll) $*.obj+t_com386.obj, $*.dll, $*.map, , $(source)\$*.386
        mapsym $*.map

t_common.obj: $(source)\t_common.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_com386.obj: $(source)\t_com386.asm $(source)\tracedll.mak
        $(assemble) $(aflags) $(source)\$*.asm, $*.obj;

t_ctl_.dll  : t_ctl_.obj t_common.lib $(source)\t_ctl_.$(def_ext)
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_ctl_.obj  : $(source)\t_ctl_.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_dos___.dll: t_dos___.obj t_dos386.obj t_common.lib $(source)\t_dos___.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj+t_dos386.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_dos___.obj: $(source)\t_dos___.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_dos386.obj: $(source)\t_dos386.asm $(source)\tracedll.mak
        $(assemble) $(aflags) $(source)\$*.asm, $*.obj;

t_drg_.dll  : t_drg_.obj t_common.lib $(source)\t_drg_.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_drg_.obj  : $(source)\t_drg_.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_gpi.dll   : t_gpi.obj t_common.lib $(source)\t_gpi.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_gpi.obj   : $(source)\t_gpi.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_hlp__.dll : t_hlp__.obj t_common.lib $(source)\t_hlp__.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_hlp__.obj : $(source)\t_hlp__.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_kbd___.dll: t_kbd___.obj t_common.lib $(source)\t_kbd___.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_kbd___.obj: $(source)\t_kbd___.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_m.dll     : t_m.obj t_m__386.obj t_common.lib $(source)\t_m.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj+t_m__386.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_m.obj     : $(source)\t_m.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_m__386.obj: $(source)\t_m__386.asm $(source)\tracedll.mak
        $(assemble) $(aflags) $(source)\$*.asm, $*.obj;

t_mon___.dll: t_mon___.obj t_common.lib $(source)\t_mon___.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_mon___.obj: $(source)\t_mon___.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_mou___.dll: t_mou___.obj t_common.lib $(source)\t_mou___.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_mou___.obj: $(source)\t_mou___.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_mrg__.dll : t_mrg__.obj t_common.lib $(source)\t_mrg__.$(def_ext)
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_mrg__.obj : $(source)\t_mrg__.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_n.dll     : t_n.obj t_common.lib $(source)\t_n.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_n.obj     : $(source)\t_n.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_pbd_.dll  : t_pbd_.obj t_common.lib $(source)\t_pbd_.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_pbd_.obj  : $(source)\t_pbd_.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_pic.dll   : t_pic.obj t_common.lib $(source)\t_pic.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_pic.obj   : $(source)\t_pic.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_pip___.dll: t_pip___.obj t_common.lib $(source)\t_pip___.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_pip___.obj: $(source)\t_pip___.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_pvp_.dll  : t_pvp_.obj t_common.lib $(source)\t_pvp_.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_pvp_.obj  : $(source)\t_pvp_.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_que___.dll: t_que___.obj t_common.lib $(source)\t_que___.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_que___.obj: $(source)\t_que___.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_ses_.dll  : t_ses_.obj t_common.lib $(source)\t_ses_.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_ses_.obj  : $(source)\t_ses_.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_shl__.dll : t_shl__.obj t_common.lib $(source)\t_shl__.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_shl__.obj : $(source)\t_shl__.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_spl.dll   : t_spl.obj t_common.lib $(source)\t_spl.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_spl.obj   : $(source)\t_spl.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_vio___.dll: t_vio___.obj t_common.lib $(source)\t_vio___.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_vio___.obj: $(source)\t_vio___.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_win.dll   : t_win.obj t_win386.obj t_common.lib $(source)\t_win.$(def_ext)
        markobj $*.obj
        -4$(link) $(lflags_dll) $*.obj+t_win386.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_win.obj   : $(source)\t_win.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

t_win386.obj: $(source)\t_win386.asm $(source)\tracedll.mak
        $(assemble) $(aflags) $(source)\$*.asm, $*.obj;

t_wp.dll    : t_wp.obj t_common.lib $(source)\t_wp.$(def_ext)
        -4$(link) $(lflags_dll) $*.obj, $*.dll, $*.map, t_common, $(source)\$*.$(def_ext)
        mapsym $*.map

t_wp.obj    : $(source)\t_wp.c $(source)\..\os2trace.h $(source)\t_common.h\
              $(source)\tracedll.mak
        $(compile) $(cflags_dll) /Fo$*.obj $(source)\$*.c

clean       :
        @for %%e in (obj dll exe map sym lib) do @if exist *.%%e del *.%%e

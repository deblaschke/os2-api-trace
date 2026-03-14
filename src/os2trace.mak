################################################################################
#                                                                              #
#  File name    : OS2TRACE.MAK                                                 #
#                                                                              #
#  Title        : OS/2 API trace makefile                                      #
#                                                                              #
#  Author       : Dave Blaschke                                                #
#                 deblaschke@yahoo.com		                               #
#                                                                              #
#  History      : Version Date    Item                                         #
#                 2.30.00 12Jun95 Created (supported LX format  and 32-bit Dos #
#                                 APIs only)                                   #
#                 2.30.24 29Mar96 Added PM interface  (supported customization #
#                                 and help only)                               #
#                 2.40.03 11Mar97 Converted to IBM VisualAge C++               #
#                 2.40.23 27May97 Linked .EXEs with proper threaded C library  #
#                 2.40.57 25Jun98 Added trace-enabled executable checker       #
#                 2.40.59 06Jul98 Added bitmaps of dialogs to PM help          #
#                 2.40.60 08Jul98 Added  title/action  bar only  option to PM  #
#                                 interface                                    #
#                 2.40.64 21Aug98 Added bitmap of logo to PM help              #
#                 2.40.65 28Aug98 Added monitor trace options to PM interface  #
#                 2.45.16 18Feb00 Added -C, -PAUSE, and -RESUME options        #
#                 2.45.25 03Jun00 Added trace information file API stripper    #
#                 2.45.32 28Jun01 Added -U option                              #
#                                                                              #
################################################################################

source  = .

################################################################################
#                                                                              #
#  Compiler and options                                                        #
#                                                                              #
################################################################################

compile = ICC
!if ( "$(DEBUG)" == "ON" )
cflags  = /C+ /Gd- /Ge+ /I. /O+ /Sa /Ti+ /Wpro /DDEBUG /DINTEL /DT_USERHOOK
!else
cflags  = /C+ /Gd- /Ge+ /I. /O+ /Sa /Wpro /DINTEL /DT_USERHOOK
!endif

################################################################################
#                                                                              #
#  Resource compiler and options                                               #
#                                                                              #
################################################################################

rcompile = RC
rcflags  = -r -DT_USERHOOK

################################################################################
#                                                                              #
#  Information presentation facility compiler                                  #
#                                                                              #
################################################################################

ipfcompile = IPFC

################################################################################
#                                                                              #
#  Linker and options                                                          #
#                                                                              #
################################################################################

link    = ILINK
!if ( "$(DEBUG)" == "ON" )
lflags  = /NOFREEFORMAT /ALIGNMENT:16 /BASE:0x10000 /DEBUG /EXEPACK /MAP /NOIGNORECASE /STACK:0x8000
!else
lflags  = /NOFREEFORMAT /ALIGNMENT:16 /BASE:0x10000 /EXEPACK /MAP /NOIGNORECASE /STACK:0x8000
!endif

################################################################################
#                                                                              #
#  Dependencies and rules                                                      #
#                                                                              #
################################################################################

all         : os2trace.exe pmos2trc.exe chk4trc.exe stripapi.exe

os2trace.exe: os2trace.obj $(source)\os2trace.def
        $(link) $(lflags) $*.obj, $*.exe, $*.map, , $(source)\$*.def
        mapsym $*.map

os2trace.obj: $(source)\os2trace.c $(source)\os2trace.h $(source)\dllinfo.h\
              $(source)\os2trace.mak
        $(compile) $(cflags) /Gm- /Fo$*.obj $(source)\$*.c

pmos2trc.exe: pmos2trc.obj pmos2trc.def pmos2trc.res pmos2trc.hlp
        $(link) $(lflags) /STACK:0xB000 $*.obj, $*.exe, $*.map,, $*.def
        $(rcompile) $*.res $*.exe
        mapsym $*.map

pmos2trc.obj: $(source)\pmos2trc.c $(source)\os2trace.h $(source)\pmos2trc.h\
              $(source)\dllinfo.h $(source)\os2trace.mak
        $(compile) $(cflags) /Gm+ /Fo$*.obj $(source)\$*.c

pmos2trc.res: $(source)\pmos2trc.rc $(source)\pmos2trc.h $(source)\pmos2trc.ico
        $(rcompile) $(rcflags) $(source)\$*.rc

pmos2trc.hlp: $(source)\pmos2trc.ipf $(source)\bitmaps\baronly.bmp\
              $(source)\bitmaps\ctldlg.bmp $(source)\bitmaps\lchdlg.bmp\
              $(source)\bitmaps\moninfo.bmp $(source)\bitmaps\os2trace.bmp\
              $(source)\bitmaps\sumdlg.bmp
        $(ipfcompile) $(source)\$*.ipf

chk4trc.exe : chk4trc.obj $(source)\chk4trc.def
        $(link) $(lflags) $*.obj, $*.exe, $*.map, , $(source)\$*.def
        mapsym $*.map

chk4trc.obj : $(source)\chk4trc.c $(source)\os2trace.h $(source)\chk4trc.h\
              $(source)\dllinfo.h $(source)\os2trace.mak
        $(compile) $(cflags) /Gm- /Fo$*.obj $(source)\$*.c

stripapi.exe: stripapi.obj $(source)\stripapi.def
        $(link) $(lflags) $*.obj, $*.exe, $*.map, , $(source)\$*.def
        mapsym $*.map

stripapi.obj: $(source)\stripapi.c $(source)\os2trace.h $(source)\stripapi.h\
              $(source)\os2trace.mak
        $(compile) $(cflags) /Gm- /Fo$*.obj $(source)\$*.c

clean       :
        @for %%e in (obj exe map sym res hlp) do @if exist *.%%e del *.%%e

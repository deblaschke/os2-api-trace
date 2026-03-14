@rem @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@rem
@rem File name    : TRACE-IT.CMD
@rem
@rem Title        : OS/2 API Trace Batch File
@rem
@rem Description  : Performs all the commands necessary to trace a .EXE file.
@rem                The most possible trace information is logged.  Up to eight
@rem                command line arguments can be passed to the .EXE.
@rem
@rem Usage        : TRACE-IT progname [progparm]...
@rem
@rem                Where:
@rem                  progname is exectuable name without .EXE extension
@rem                  progparm is optional program parameter (8 maximum)
@rem
@rem                In the following example,  EDIT.EXE is traced while editing
@rem                CONFIG.SYS in the root directory of the C: drive:
@rem
@rem                TRACE-IT edit c:\config.sys
@rem
@rem History      : Version Date    Item
@rem                2.40.38 15Aug97 Added TRACE-IT batch file
@rem                2.40.81 02Dec98 Added -E option
@rem                2.45.16 18Feb00 Added -C, -PAUSE, and -RESUME options
@rem                2.45.24 01Jun00 Added -A option
@rem                2.45.32 28Jun01 Added -U option
@rem
@rem @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@rem Issue an error if at least one argument, the executable file name without
@rem an extension, is not passed in on the command line
@
@if .%1 == . goto error1
@
@rem Issue an error if the executable file does not exist
@
@if not exist %1.exe goto error2
@
@rem Customize tracing to log the most information possible
@
@os2trace -a none -b all -c off -d all -e on -f all -g all -i on -l 3 -t on -u none -w all
@
@rem Enable the executable file to trace APIs from all of the supported DLLs
@
@os2trace -on -all %1.exe
@
@rem Run the executable file, passing in up to eight command line arguments
@
@%1 %2 %3 %4 %5 %6 %7 %8 %9
@
@rem Disable the executable file from tracing
@
@os2trace -off -all %1.exe
@
@rem Issue an error if the trace information file does not have the same file
@rem name as the executable, indicating that the executable imports from none
@rem of the supported DLLs or another instance of the executable was running
@
@if not exist %1.trc goto error3
@echo.
@echo Trace results are in %1.TRC
@goto quit
@
@rem Issue usage message and terminate
@
:error1
@echo.
@echo Usage: TRACE-IT progname [progparm]...
@echo Where: progname is exectuable name without .EXE extension
@echo        progparm is optional program parameter (8 maximum)
@goto quit
@
@rem Issue executable file not found message and terminate
@
:error2
@echo.
@echo TRACE-IT Error: %1.EXE does not exist in current directory
@goto quit
@
@rem Issue trace information file not found message and terminate
@
:error3
@echo.
@if exist PROC*.TRC goto error3b
:error3a
@echo TRACE-IT Error: %1.TRC does not exist
@goto quit
:error3b
@echo TRACE-IT Error: %1.TRC does not exist, check for PROCxxxx.TRC
@goto quit
@
@rem Terminate
@
:quit

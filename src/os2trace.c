/******************************************************************************/
/*                                                                            */
/* File name    : OS2TRACE.C                                                  */
/*                                                                            */
/* Title        : OS/2 API trace routines                                     */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* Description  : Enables, customizes, controls and summarizes the tracing of */
/*                OS/2 APIs imported  by a 16-bit  or 32-bit  executable file */
/*                without affecting its source  code or requiring recompiling */
/*                or relinking.                                               */
/*                                                                            */
/*                The  first  feat,   trace  enabling,   is  accomplished  by */
/*                processing  each  entry in  the table of strings within the */
/*                executable  file that  contains the  names of  the imported */
/*                DLLs.  If the entry  represents one  of the  supported OS/2 */
/*                DLLs and the  user requested  enablement of API tracing for */
/*                this  DLL,  the  entry is  replaced  with the  name  of the */
/*                appropriate  trace  DLL.    When  the  executable  file  is */
/*                invoked,   the  trace  DLL  intercepts  API  calls  to  its */
/*                corresponding OS/2 DLL, logs API input information, invokes */
/*                the API,  and logs  API output  information.  If  the entry */
/*                represents one  of the  trace DLLs  and the  user requested */
/*                disablement  of  API  tracing  for this  DLL,  the entry is */
/*                replaced  with the  name of the  supported OS/2  DLL.  When */
/*                finished,  the updated table  of strings is  written to the */
/*                executable file.                                            */
/*                                                                            */
/*                NOTE:   All  private  APIs  are  simply  forwarded  to  the */
/*                corresponding OS/2 DLL with  no intervention from the trace */
/*                DLL.                                                        */
/*                                                                            */
/*                The  user   can  request   enablement  of  API  tracing  by */
/*                specifying the -ON option or can request disablement of API */
/*                tracing  by  specifying  the  -OFF  option.  The  user  can */
/*                request enablement or disablement of API tracing for one or */
/*                more of the supported DLLs  individually by specifying each */
/*                DLL's name  as an  option,  or can  request  enablement  or */
/*                disablement  of  API  tracing  for all  supported  DLLs  by */
/*                specifying  the  -ALL  option.  The  user  can  request  to */
/*                preserve  the  executable  file's date  and  time  stamp by */
/*                specifying the -P option.                                   */
/*                                                                            */
/*                NOTE:  Trace enablement alters the contents of the table of */
/*                strings within the executable file  that contains the names */
/*                of the imported DLLs.  Although this action does not affect */
/*                the  functionality of  the executable,  it does  affect its */
/*                date and time stamp unless the -P option is specified.      */
/*                                                                            */
/*                The  second feat,  trace customizing,  is  accomplished  by */
/*                storing the state of the trace customization options in the */
/*                operating  system's  user profile,  OS2.INI.  The  state of */
/*                these options can then be  retrieved by the trace DLLs when */
/*                a trace-enabled executable commences running.               */
/*                                                                            */
/*                The user can  request to  log all  trace information  to an */
/*                alternative directory by specifying the -A s option,  where */
/*                s is the fully  qualified drive and path  of the directory. */
/*                The user can  request to log  all trace information  to the */
/*                default directory (same directory as the .EXE/.COM file) by */
/*                specifying the  -A NONE  option.  The user  can  request to */
/*                log a maximum number of  bytes of level 3 trace information */
/*                from buffers by  specifying the -B n  option,  where n is a */
/*                decimal number between 16 and 65536 (64KB),  inclusive, and */
/*                is rounded up to  the nearest multiple  of 16.  The decimal */
/*                number can include the 'K' (kilobyte) suffix, in which case */
/*                the maximum possible value is 64K.  The  user  can  request */
/*                to log  all  level 3  trace  information  from  buffers  by */
/*                specifying  the  -B ALL  option.  The user  can  request to */
/*                enable tracing control  (the -PAUSE and -RESUME options) by */
/*                specifying  the  -C ON  option.  The  user  can request  to */
/*                disable tracing  control by specifying the -C OFF option.   */
/*                                                                            */
/*                NOTE: When tracing control is enabled,  a thread is created */
/*                within the application being traced for the sole purpose of */
/*                monitoring pause  and resume  requests.  If this additional */
/*                thread  is not  desired for  performance or  other reasons, */
/*                tracing control should be disabled.                         */
/*                                                                            */
/*                The user can request to  trace specific groups of APIs from */
/*                DOSCALLS.DLL by specifying the -D g option,  where g is ALL */
/*                (request all  API groups),  one or  more of  the  following */
/*                delimited by commas (request only specific API groups),  or */
/*                ALL and one  or more  of the  following  each prefixed by a */
/*                "NO" and delimited  by commas  (request all except specific */
/*                API groups):                                                */
/*                                                                            */
/*                  DEV  indicates trace device API group                     */
/*                  FILE indicates trace file API group                       */
/*                  MEM  indicates trace memory API group                     */
/*                  MISC indicates trace miscellaneous API group              */
/*                  MOD  indicates trace module API group                     */
/*                  MVDM indicates trace MVDM API group                       */
/*                  NLS  indicates trace national language support API group  */
/*                  PIPE indicates trace pipe API group                       */
/*                  PRF  indicates trace performance API group                */
/*                  PROC indicates trace process and thread API group         */
/*                  RES  indicates trace resource API group                   */
/*                  SEM  indicates trace semaphore API group                  */
/*                  SES  indicates trace session API group                    */
/*                  TIME indicates trace date/time and timer API group        */
/*                  XCPT indicates trace exception API group                  */
/*                  MSG  indicates trace message API group                    */
/*                  INFO indicates trace InfoSeg API group                    */
/*                  SIG  indicates trace signal API group                     */
/*                  SMP  indicates trace SMP API group                        */
/*                  PROF indicates trace DosProfile                           */
/*                                                                            */
/*                The  user   can  request  to   enable  logging   of  EBCDIC */
/*                translation  of character  buffers by  specifying the -E ON */
/*                option.  The user can request  to disable logging of EBCDIC */
/*                translation of  character buffers by  specifying the -E OFF */
/*                option.                                                     */
/*                                                                            */
/*                NOTE:  Logging of  ASCII translation  of character  buffers */
/*                occurs regardless of the state of the -E option.            */
/*                                                                            */
/*                The user can  request to log a  maximum  number of bytes of */
/*                trace  information  before  log file wrapping  (overwriting */
/*                from the beginning) occurs by  specifying the  -F n option, */
/*                where n is a decimal number between 4096 (4KB) and 67108864 */
/*                (64MB),  inclusive,  and  is  rounded  up  to  the  nearest */
/*                multiple of 4096.  The decimal  number can  include the 'K' */
/*                (kilobyte) suffix, in which case the maximum possible value */
/*                is 65536K,  or the 'M' (megabyte) suffix, in which case the */
/*                maximum possible value is 64M.  The user can request to log */
/*                all  trace   information  without  log  file   wrapping  by */
/*                specifying the -F ALL option.                               */
/*                                                                            */
/*                NOTE:  -F ALL appends trace  information to the  end of any */
/*                preexisting trace information file,  while -F n  erases the */
/*                contents of any preexisting trace information file.         */
/*                                                                            */
/*                NOTE:  Setting the  log file  wrapping  length to  anything */
/*                other than ALL  will further impact the  performance of the */
/*                application being traced, as log file wrapping requires the */
/*                log file to be unbuffered,  meaning that  trace information */
/*                cannot be buffered  in a memory cache before  being written */
/*                to disk.                                                    */
/*                                                                            */
/*                The user can request to  trace specific groups of APIs from */
/*                PMGPI.DLL by  specifying  the -G g  option, where  g is ALL */
/*                (request all  API groups),  one  or more  of the  following */
/*                delimited by commas (request only specific API groups),  or */
/*                ALL and one  or more of  the following  each prefixed  by a */
/*                "NO" and delimited  by commas  (request all except specific */
/*                API groups):                                                */
/*                                                                            */
/*                  BIT  indicates trace bitmap API group                     */
/*                  CORR indicates trace correlation API group                */
/*                  CTRL indicates trace control API group                    */
/*                  DEF  indicates trace defaults API group                   */
/*                  EDIT indicates trace segment editing API group            */
/*                  LCID indicates trace LCID API group                       */
/*                  LCT  indicates trace logical color table API group        */
/*                  META indicates trace metafile API group                   */
/*                  PATH indicates trace path API group                       */
/*                  POLY indicates trace polygon API group                    */
/*                  PRIM indicates trace primitive API group                  */
/*                  RGN  indicates trace region API group                     */
/*                  SEG  indicates trace segment API group                    */
/*                  TRAN indicates trace transform API group                  */
/*                  DEV  indicates trace device API group                     */
/*                                                                            */
/*                The user can request to  enable interception of dynamic API */
/*                calls by specifying the -I ON option.  The user can request */
/*                to disable intercaption of  dynamic API calls by specifying */
/*                the -I OFF option.  The  user can request  to log  specific */
/*                levels of  trace information by specifying the -L n option, */
/*                where n is one of the                                       */
/*                following:                                                  */
/*                                                                            */
/*                  1 indicates log API entry/exit information                */
/*                  2 indicates log API parameters                            */
/*                  3 indicates log API parameter contents                    */
/*                                                                            */
/*                The user can request to enable time stamping of API entries */
/*                and exits by  specifying the  -T ON  option.  The user  can */
/*                request to disable  time stamping of API  entries and exits */
/*                by specifying the -T OFF option.                            */
/*                                                                            */
/*                NOTE:  API entry time  stamps reflect  entry into the trace */
/*                API, not entry into the actual OS/2 API.                    */
/*                                                                            */
/*                The  user can  request to  have a  user-provided  procedure */
/*                (user hook)  called by specifying the -U s option,  where s */
/*                is of the format DLLNAME.HOOKNAME  and HOOKNAME is exported */
/*                from  DLLNAME  by  name.  A user  hook  has  the  following */
/*                syntax:                                                     */
/*                                                                            */
/*                  PSZ APIENTRY UserHook( ULONG ulReason                     */
/*                                       , PSZ   pszAPIName                   */
/*                                       , PVOID pAPIParms                    */
/*                                       , PVOID pAPIRC                       */
/*                                       );                                   */
/*                                                                            */
/*                A user hook is called under the following circumstances:    */
/*                                                                            */
/*                  Initialization - ulReason = 0                             */
/*                                   pszAPIName = pAPIParms = pAPIRC = NULL   */
/*                  Termination    - ulReason = 1                             */
/*                                   pszAPIName = pAPIParms = pAPIRC = NULL   */
/*                  API Entry      - ulReason = 2                             */
/*                                   pszAPIName = API name                    */
/*                                   pAPIParms = Pointer to parameters or     */
/*                                     NULL if none                           */
/*                                   pAPIRC = NULL                            */
/*                  API Exit       - ulReason = 3                             */
/*                                   pszAPIName = API name                    */
/*                                   pAPIParms = Pointer to parameters or     */
/*                                     NULL if none                           */
/*                                   pAPIRC = Pointer to return code or NULL  */
/*                                     if none                                */
/*                                                                            */
/*                A user hook  can return a pointer to a  string that will be */
/*                written  to  the  trace  information  file.  The  user  can */
/*                request to not have a user hook called by specifying the -U */
/*                NONE option.                                                */
/*                                                                            */
/*                NOTE:   Because  a user  hook is  a 32-bit  procedure,  all */
/*                parameters are 32-bit aligned,  even if they are parameters */
/*                to a  16-bit API.  For  example,  when  examining  a USHORT */
/*                parameter being  passed to a 16-bit API,  the high word may */
/*                not necessarily be zero and should be ignored.              */
/*                                                                            */
/*                NOTE:  User hooks are only called for API entries and exits */
/*                if the API is being traced AND tracing is not paused.       */
/*                                                                            */
/*                NOTE:  User hooks do not need to be reentrant.              */
/*                                                                            */
/*                The user can request to  trace specific groups of APIs from */
/*                PMWIN.DLL by  specifying  the -W g  option, where  g is ALL */
/*                (request all  API groups),  one  or more  of the  following */
/*                delimited by commas (request only specific API groups),  or */
/*                ALL and one  or more of  the following  each prefixed  by a */
/*                "NO" and delimited  by commas  (request all except specific */
/*                API groups):                                                */
/*                                                                            */
/*                  ACCL indicates trace accelerator API group                */
/*                  ATOM indicates trace atom API group                       */
/*                  CLIP indicates trace clipboard API group                  */
/*                  CTRY indicates trace country API group                    */
/*                  CUR  indicates trace cursor API group                     */
/*                  DDE  indicates trace DDE API group                        */
/*                  DESK indicates trace desktop API group                    */
/*                  DLG  indicates trace dialog API group                     */
/*                  DWIN indicates trace WinDefWindowProc                     */
/*                  ERR  indicates trace error API group                      */
/*                  FRAM indicates trace frame API group                      */
/*                  HOOK indicates trace hook API group                       */
/*                  INPT indicates trace input API group                      */
/*                  LOAD indicates trace load API group                       */
/*                  MENU indicates trace menu API group                       */
/*                  MSG  indicates trace message API group  (does not include */
/*                       MSGL group)                                          */
/*                  MSGL indicates trace WinDispatchMsg and WinGetMsg         */
/*                  PAL  indicates trace palette API group                    */
/*                  PTR  indicates trace pointer API group                    */
/*                  RECT indicates trace rectangle API group                  */
/*                  SYS  indicates trace system API group                     */
/*                  THK  indicates trace thunk API group                      */
/*                  TIME indicates trace time API group                       */
/*                  TREC indicates trace track rectangle API group            */
/*                  WIN  indicates trace window API group  (does not  include */
/*                       DWIN group)                                          */
/*                  ENV  indicates trace WinCatch and WinThrow                */
/*                  HEAP indicates trace heap API group                       */
/*                  SEI  indicates trace set error info API group             */
/*                                                                            */
/*                NOTE:  DWIN and MSGL are separated from WIN and MSG because */
/*                these  APIs,  when being  traced,  can severely  impact the */
/*                performance of the trace-enabled executable.                */
/*                                                                            */
/*                The user can  request to  display the  current state of the */
/*                trace customization options stored in OS2.INI by specifying */
/*                the -Q option.                                              */
/*                                                                            */
/*                NOTE:   No  changes  are made  to the  state  of  the trace */
/*                customization options stored in OS2.INI if the -Q option is */
/*                specified,  even if other customization options are present */
/*                on the command line.                                        */
/*                                                                            */
/*                The  third feat,  trace  controlling,  is  accomplished  by */
/*                creating a thread within the  application being traced that */
/*                awaits  pause  and resume  requests.  When  such  a request */
/*                arrives the thread awakens,  pauses or resumes tracing, and */
/*                goes back to sleep awaiting the next request.               */
/*                                                                            */
/*                The  user can  request to pause  tracing by  specifying the */
/*                -PAUSE option.  The user  can request to  resume tracing by */
/*                specifying the -RESUME option.                              */
/*                                                                            */
/*                NOTE:  In order for  the -PAUSE  and -RESUME  options to be */
/*                effective on a  trace-enabled  executable,  tracing control */
/*                must be enabled by  specifying the -C ON  option before the */
/*                executable commences running.                               */
/*                                                                            */
/*                NOTE:  A trace-enabled  executable  can  be  started  while */
/*                tracing is paused, as the state of the trace control option */
/*                is stored in OS2.INI  and retrieved by the  trace DLLs when */
/*                the executable commences running.                           */
/*                                                                            */
/*                The  fourth feat,  trace  summarizing,  is  accomplished by */
/*                parsing  the   trace  information   file  generated   by  a */
/*                trace-enabled  executable and  recording the  number of API */
/*                entries and exits  logged in the file.  When finished,  the */
/*                summary statistics  are displayed in  alphabetical order to */
/*                standard output.                                            */
/*                                                                            */
/*                The user  can request to summarize  the contents of a trace */
/*                information file by specifying the -S option.               */
/*                                                                            */
/*                The user can request helping  information by specifying the */
/*                -?, -H, or -HELP option.                                    */
/*                                                                            */
/*                OS/2 API Trace works on all 32-bit versions of OS/2:        */
/*                                                                            */
/*                                                    |  Major  |  Minor      */
/*                Name                                | Version | Version     */
/*                ------------------------------------+---------+---------    */
/*                OS/2 Version 2.0x                   |    20   |    0x       */
/*                OS/2 Version 2.1x                   |    20   |    1x       */
/*                OS/2 Warp Version 3                 |    20   |    30       */
/*                OS/2 Warp Version 4                 |    20   |    40       */
/*                OS/2 Warp Server for e-business     |    20   |    45       */
/*                OS/2 Warp Connect (PowerPC Edition) |    30   |    00       */
/*                                                                            */
/*                where major and minor versions are the corresponding values */
/*                returned from DosQuerySysInfo.  Note that the  ELF versions */
/*                of the executables  and trace DLLs  are required  to run on */
/*                OS/2 Warp Connect (PowerPC Edition).                        */
/*                                                                            */
/* Installation : Place the executable,  OS2TRACE.EXE,  in a directory  along */
/*                the PATH  environment variable,  and place the  trace DLLs, */
/*                T_*.DLL,  in a directory  along the  system LIBPATH.   If a */
/*                user hook is provided, the DLL that it resides in must also */
/*                be placed along the system LIBPATH.                         */
/*                                                                            */
/* Usage        : Trace enabler -                                             */
/*                                                                            */
/*                OS2TRACE -OFF|-ON {-ALL|-dll}... [-P] file                  */
/*                                                                            */
/*                Where:                                                      */
/*                  -OFF indicates disable API tracing                        */
/*                  -ON  indicates enable API tracing                         */
/*                  -ALL indicates enable/disable API tracing for all DLLs    */
/*                  -dll indicates  enable/disable  API tracing  for specific */
/*                       DLL, where dll can be one of the following:          */
/*                         DOSCALLS  HELPMGR   KBDCALLS  MONCALLS  MOUCALLS   */
/*                         MSG       NAMPIPES  NLS       PMBIDI    PMCTLS     */
/*                         PMDRAG    PMGPI     PMMERGE   PMPIC     PMSHAPI    */
/*                         PMSPL     PMVIOP    PMWIN     PMWP      QUECALLS   */
/*                         SESMGR    VIOCALLS                                 */
/*                  -P   indicates preserve executable file date/time stamp   */
/*                  file indicates  name  of  executable  file  to  be  trace */
/*                       enabled/disabled                                     */
/*                                                                            */
/*                NOTE:     Trace  enablement  alters  the  contents  of  the */
/*                executable file's import module name table.                 */
/*                                                                            */
/*                In  the following example,  tracing is enabled  in TEST.EXE */
/*                for  the  APIs  imported  from  QUECALLS  and  SESMGR,  and */
/*                TEST.EXE's date and time stamp is preserved:                */
/*                                                                            */
/*                OS2TRACE -ON -QUECALLS -SESMGR -P TEST.EXE                  */
/*                                                                            */
/*                In  the following example,  tracing is disabled in TEST.EXE */
/*                for the APIs imported from all supported DLLs:              */
/*                                                                            */
/*                OS2TRACE -OFF -ALL TEST.EXE                                 */
/*                                                                            */
/*                Trace customizer -                                          */
/*                                                                            */
/*                OS2TRACE {-A s|-B n|-C f|-D g|-E f|-F n|                    */
/*                          -G g|-I f|-L n|-Q|-T f|-U s|-W g}...              */
/*                                                                            */
/*                Where:                                                      */
/*                  -A s indicates   fully   qualified  drive   and  path  of */
/*                       alternative  logging  directory or  NONE for default */
/*                       directory                                            */
/*                  -B n indicates log maximum of n bytes from buffers, where */
/*                       16 ó n ó 65536  (K suffix allowed)  or n is  ALL  to */
/*                       indicate log all bytes from buffers                  */
/*                  -C f indicates  enable  (f = ON)   or  disable  (f = OFF) */
/*                       tracing control (-PAUSE and -RESUME options)         */
/*                  -D g indicates trace specific DOSCALLS API groups,  where */
/*                       g is either ALL[,NOgrp]...  or grp[,grp]...  and grp */
/*                       is one of the following:                             */
/*                         DEV       FILE      MEM       MISC      MOD        */
/*                         MVDM      NLS       PIPE      PRF       PROC       */
/*                         RES       SEM       SES       TIME      XCPT       */
/*                         MSG       INFO      SIG       SMP       PROF       */
/*                  -E f indicates  enable  (f = ON)   or  disable  (f = OFF) */
/*                       logging of EBCDIC translation of character buffers   */
/*                  -F n indicates  log maximum  of n  bytes before  log file */
/*                       wrapping occurs,  where 4096 ó n ó 67108864  (K or M */
/*                       suffixes allowed)  or n is  ALL  to indicate log all */
/*                       information without log file wrapping                */
/*                  -G g indicates  trace  specific  PMGPI API groups,  where */
/*                       g is either ALL[,NOgrp]...  or grp[,grp]...  and grp */
/*                       is one of the following:                             */
/*                         BIT       CORR      CTRL      DEF       EDIT       */
/*                         LCID      LCT       META      PATH      POLY       */
/*                         PRIM      RGN       SEG       TRAN      DEV        */
/*                         INK                                                */
/*                  -I f indicates  enable  (f = ON)   or  disable  (f = OFF) */
/*                       interception of dynamic API calls                    */
/*                  -L n indicates log level n information, where 1 ó n ó 3:  */
/*                         1 indicates log API entry/exit information         */
/*                         2 indicates log API parameters                     */
/*                         3 indicates log API parameter contents             */
/*                  -Q   indicates query current trace customization options  */
/*                  -T f indicates enable (f = ON)  or disable (f = OFF) time */
/*                       stamping of API entries and exits                    */
/*                  -U s indicates user hook  of the format  DLLNAME.HOOKNAME */
/*                       or NONE for no user hook                             */
/*                  -W g indicates  trace  specific  PMWIN API groups,  where */
/*                       g is either ALL[,NOgrp]...  or grp[,grp]...  and grp */
/*                       is one of the following:                             */
/*                         ACCL      ATOM      CLIP      CTRY      CUR        */
/*                         DDE       DESK      DLG       ERR       FRAM       */
/*                         HOOK      INPT      LOAD      MENU      MSG        */
/*                         PAL       PTR       RECT      SYS       THK        */
/*                         TIME      TREC      WIN       ENV       HEAP       */
/*                         SEI                                                */
/*                                                                            */
/*                NOTE: The default trace customization option settings are:  */
/*                                                                            */
/*                -A NONE                                                     */
/*                -B 256                                                      */
/*                -C OFF                                                      */
/*                -D ALL                                                      */
/*                -E OFF                                                      */
/*                -F ALL                                                      */
/*                -G ALL                                                      */
/*                -I OFF                                                      */
/*                -L 1                                                        */
/*                -T OFF                                                      */
/*                -U NONE                                                     */
/*                -W ALL                                                      */
/*                                                                            */
/*                In the following example,  tracing is customized to log all */
/*                trace information to the D:\TRACELOG directory.             */
/*                                                                            */
/*                OS2TRACE -A D:\TRACELOG                                     */
/*                                                                            */
/*                In the  following  example,  tracing is  customized  to log */
/*                a maximum of  512 bytes of  level 3 trace information  from */
/*                buffers:                                                    */
/*                                                                            */
/*                OS2TRACE -B 512                                             */
/*                                                                            */
/*                In the  following example,  tracing is customized to enable */
/*                tracing control (-PAUSE/-RESUME options):                   */
/*                                                                            */
/*                OS2TRACE -C ON                                              */
/*                                                                            */
/*                In the  following example,  tracing is customized  to trace */
/*                only memory and semaphore API groups from DOSCALLS.DLL:     */
/*                                                                            */
/*                OS2TRACE -D MEM,SEM                                         */
/*                                                                            */
/*                In the  following example,  tracing is customized to enable */
/*                logging of EBCDIC translation of character buffers:         */
/*                                                                            */
/*                OS2TRACE -E ON                                              */
/*                                                                            */
/*                In the  following  example,  tracing is  customized  to log */
/*                a maximum of 16384 (16K) bytes of  trace information before */
/*                log file wrapping occurs:                                   */
/*                                                                            */
/*                OS2TRACE -F 16K                                             */
/*                                                                            */
/*                In the  following example,  tracing is customized  to trace */
/*                only  bitmap,  metafile,  and  transform  API  groups  from */
/*                PMGPI.DLL:                                                  */
/*                                                                            */
/*                OS2TRACE -G BIT,META,TRAN                                   */
/*                                                                            */
/*                In the  following example,  tracing is customized to enable */
/*                interception of dynamic API calls:                          */
/*                                                                            */
/*                OS2TRACE -I ON                                              */
/*                                                                            */
/*                In the  following  example,  tracing is  customized  to log */
/*                level 2 information:                                        */
/*                                                                            */
/*                OS2TRACE -L 2                                               */
/*                                                                            */
/*                In the  following example,  the current state  of the trace */
/*                customization options stored in OS2.INI is displayed:       */
/*                                                                            */
/*                OS2TRACE -Q                                                 */
/*                                                                            */
/*                In the  following example,  tracing is customized to enable */
/*                time stamping of API entries/exits:                         */
/*                                                                            */
/*                OS2TRACE -T ON                                              */
/*                                                                            */
/*                In the  following example,  tracing is  customized to  call */
/*                user hook MyHook() in MYDLL.DLL:                            */
/*                                                                            */
/*                OS2TRACE -U MYDLL.MyHook                                    */
/*                                                                            */
/*                In the  following example,  tracing is customized  to trace */
/*                all except hook and system API groups from PMWIN.DLL:       */
/*                                                                            */
/*                OS2TRACE -W ALL,NOHOOK,NOSYS                                */
/*                                                                            */
/*                Trace controller -                                          */
/*                                                                            */
/*                OS2TRACE -PAUSE|-RESUME                                     */
/*                                                                            */
/*                Where:                                                      */
/*                  -PAUSE  indicates pause API tracing                       */
/*                  -RESUME indicates resume API tracing                      */
/*                                                                            */
/*                In the following  example, tracing is paused:               */
/*                                                                            */
/*                OS2TRACE -PAUSE                                             */
/*                                                                            */
/*                In the following  example, tracing is resumed:              */
/*                                                                            */
/*                OS2TRACE -RESUME                                            */
/*                                                                            */
/*                Trace summarizer -                                          */
/*                                                                            */
/*                OS2TRACE -S file                                            */
/*                                                                            */
/*                Where:                                                      */
/*                  -S   indicates summarize API tracing                      */
/*                  file indicates name of trace information file to be trace */
/*                       summarized                                           */
/*                                                                            */
/*                In the following example, tracing in TEST.TRC is summarized */
/*                and placed in TEST.SUM:                                     */
/*                                                                            */
/*                OS2TRACE -S TEST.TRC > TEST.SUM                             */
/*                                                                            */
/* Scenario     : The following  example shows a  typical scenario  where the */
/*                several personalities of OS2TRACE can be used in conjuction */
/*                to produce a summary of NLS APIs used by TEST.EXE:          */
/*                                                                            */
/*                OS2TRACE -D NLS -L 1                                        */
/*                OS2TRACE -ON -DOSCALLS -NLS TEST.EXE                        */
/*                TEST                                                        */
/*                OS2TRACE -OFF -DOSCALLS -NLS TEST.EXE                       */
/*                OS2TRACE -S TEST.TRC > TEST.NLS                             */
/*                                                                            */
/*                The first  line  customizes  API tracing,  the second  line */
/*                enables API tracing, the third line invokes the executable, */
/*                which places  all its API tracing  information in TEST.TRC, */
/*                the fourth line  disables API tracing,  and the fifth  line */
/*                summarizes API tracing.                                     */
/*                                                                            */
/* Output       : Trace enabler -                                             */
/*                                                                            */
/*                If  the  user  requests  enablement  of  API  tracing  (-ON */
/*                option),  information similar to the following is displayed */
/*                for each requested  DLL that is imported  by the executable */
/*                file:                                                       */
/*                                                                            */
/*                DLLNAME : File imports from DLL, API tracing enabled        */
/*                                                                            */
/*                information similar to the  following is displayed for each */
/*                requested DLL  whose trace DLL  is already imported  by the */
/*                executable file:                                            */
/*                                                                            */
/*                DLLNAME : File imports from trace DLL,  API tracing already */
/*                          enabled                                           */
/*                                                                            */
/*                information similar to the  following is displayed for each */
/*                requested DLL that is not imported by the executable file:  */
/*                                                                            */
/*                DLLNAME : File does not  import from DLL,  API  tracing not */
/*                          enabled                                           */
/*                                                                            */
/*                information similar to the  following is displayed for each */
/*                requested DLL that has an  unsupported API name imported by */
/*                the executable file:                                        */
/*                                                                            */
/*                DLLNAME : File imports APINAME by name,  API tracing cannot */
/*                          be enabled                                        */
/*                                                                            */
/*                and information similar  to the following  is displayed for */
/*                each requested DLL that has an unsupported ordinal imported */
/*                by the executable file:                                     */
/*                                                                            */
/*                DLLNAME : File imports  unsupported ordinal N,  API tracing */
/*                          cannot be enabled                                 */
/*                                                                            */
/*                If  the  user  requests disablement  of  API tracing  (-OFF */
/*                option),  information similar to the following is displayed */
/*                for  each  requested  trace  DLL that  is imported  by  the */
/*                executable file:                                            */
/*                                                                            */
/*                DLLNAME : File imports from trace DLL, API tracing disabled */
/*                                                                            */
/*                information similar to the  following is displayed for each */
/*                requested trace DLL  whose DLL  is already imported  by the */
/*                executable file:                                            */
/*                                                                            */
/*                DLLNAME : File  imports  from  DLL,   API  tracing  already */
/*                          disabled                                          */
/*                                                                            */
/*                and information  similar to the following  is displayed for */
/*                each  requested  trace DLL  that is  not  imported  by  the */
/*                executable file:                                            */
/*                                                                            */
/*                DLLNAME : File does not import from trace DLL,  API tracing */
/*                          not disabled                                      */
/*                                                                            */
/*                Trace customizer -                                          */
/*                                                                            */
/*                If the user requests customization of API tracing  (-A, -B, */
/*                -C,  -D, -E,  -F, -G, -I,  -L, -T,  -U and/or  -W options), */
/*                information similar to the following is displayed:          */
/*                                                                            */
/*                Old trace customization options:                            */
/*                  -A D:\ -B 64 -C OFF -D ALL -E ON -F 8192 -G DEV,INK ...   */
/*                New trace customization options:                            */
/*                  -A NONE -B 256 -C ON -D MEM,SEM -E ON -F ALL -G ALL ...   */
/*                                                                            */
/*                The  first  two  lines  contain  the  state  of  the  trace */
/*                customization  options prior to the invocation of  OS2TRACE */
/*                while  the last two lines  contain the  state of  the trace */
/*                customization options after the invocation of OS2TRACE.     */
/*                                                                            */
/*                Trace controller -                                          */
/*                                                                            */
/*                If  the  user  requests  pausing  of  API  tracing  (-PAUSE */
/*                option),  information similar to the following is displayed */
/*                if tracing  is not  already  paused  and there  are  active */
/*                trace-enabled applications  that were started while tracing */
/*                control was enabled (-C ON option):                         */
/*                                                                            */
/*                Tracing paused                                              */
/*                  C:\OS2\E.EXE (PID 0F1C) paused                            */
/*                                                                            */
/*                information  similar  to  the  following  is  displayed  if */
/*                tracing  is not  already  paused  and  there  are no active */
/*                trace-enabled applications there were started while tracing */
/*                control was enabled (-C ON option):                         */
/*                                                                            */
/*                Tracing paused                                              */
/*                  No active trace-enabled applications                      */
/*                                                                            */
/*                and information  similar  to the following  is displayed if */
/*                tracing is already paused:                                  */
/*                                                                            */
/*                Tracing already paused                                      */
/*                                                                            */
/*                If the  user  requests resuming  of  API  tracing  (-RESUME */
/*                option),  information similar to the following is displayed */
/*                if tracing  is not  already resumed  and there  are  active */
/*                trace-enabled applications  that were started while tracing */
/*                control was enabled (-C ON option):                         */
/*                                                                            */
/*                Tracing resumed                                             */
/*                  C:\OS2\E.EXE (PID 0F1C) resumed                           */
/*                                                                            */
/*                information  similar  to  the  following  is  displayed  if */
/*                tracing  is not  already resumed  and  there  are no active */
/*                trace-enabled applications there were started while tracing */
/*                control was enabled (-C ON option):                         */
/*                                                                            */
/*                Tracing resumed                                             */
/*                  No active trace-enabled applications                      */
/*                                                                            */
/*                and information  similar  to the following  is displayed if */
/*                tracing is already resumed:                                 */
/*                                                                            */
/*                Tracing already resumed                                     */
/*                                                                            */
/*                Trace summarizer -                                          */
/*                                                                            */
/*                If the user  requests  summarization  of the  contents of a */
/*                trace information file (-S option),  information similar to */
/*                the following is displayed:                                 */
/*                                                                            */
/*                Used APIs:                                                  */
/*                  APINAME (12 Pass, 4 Fail, 1 Indeterminate, 2 No Return)   */
/*                                                                            */
/*                Each line contains the name of the used API,  the number of */
/*                successful invocations, if any,  the number of unsuccessful */
/*                invocations,    if  any,    the  number  of   indeterminate */
/*                invocations, if any,  and the number of API entries without */
/*                matching  exits  (indicated  by  "No Return"),  if any.  It */
/*                should be noted that these  APIs are listed in alphabetical */
/*                order.                                                      */
/*                                                                            */
/*                NOTE:    A  warning  message  is  issued  if  summarization */
/*                information  may   not  be   accurate  due   to  lost   API */
/*                entries/exits, pausing/resuming trace, or file wrapping.    */
/*                                                                            */
/*                Trace-enabled executable -                                  */
/*                                                                            */
/*                All levels of information, from the trace-enabled .EXE/.COM */
/*                and/or any  trace-enabled  .DLLs that  are attached  to the */
/*                .EXE/.COM,  are logged to a text file with a file name that */
/*                matches the  .EXE/.COM file name and  an extension of .TRC. */
/*                This trace information  file resides in the  same directory */
/*                as the .EXE/.COM  file or the alternative directory, if one */
/*                exists.  If  another  instance  of the  same  .EXE/.COM  is */
/*                already  running or  the text  file  is already open in the */
/*                alternative directory, the file  name of the  text file  is */
/*                changed  to  PROC  followed   by  the  hexadecimal  process */
/*                identifier (i.e. PROC003A.TRC).   If this occurs, a warning */
/*                message is issued.                                          */
/*                                                                            */
/*                NOTE:  If a REXX program loads any trace-enabled .DLLs, all */
/*                trace  information  is  logged  to   CMD.TRC  in  the  same */
/*                directory  as the  CMD.EXE executable  that loaded the REXX */
/*                program or the alternative directory, if one exists.        */
/*                                                                            */
/*                If the user requests  level 1 information,  data similar to */
/*                the following is logged for each API call:                  */
/*                                                                            */
/*                003A 0001 | Dos32CreateQueue Entry                          */
/*                                                                            */
/*                003A 0001 | Dos32CreateQueue Exit                           */
/*                     PASS | Return code: 0                                  */
/*                                                                            */
/*                If the user requests  level 2 information,  data similar to */
/*                the following is logged for each API call:                  */
/*                                                                            */
/*                003A 0001 | Dos32CreateQueue Entry                          */
/*                          | Parameter 1: PHQUEUE = 0x00028BE8               */
/*                          | Parameter 2: ULONG = 0x00000002                 */
/*                          | Parameter 3: PSZ = 0x000200A4                   */
/*                                                                            */
/*                003A 0001 | Dos32CreateQueue Exit                           */
/*                     PASS | Return code: 0                                  */
/*                          | Parameter 1: PHQUEUE = 0x00028BE8               */
/*                                                                            */
/*                If the user requests  level 3 information,  data similar to */
/*                to the following is logged for each API call:               */
/*                                                                            */
/*                003A 0001 | Dos32CreateQueue Entry, Return Address =...     */
/*                          | Parameter 1: PHQUEUE = 0x00028BE8               */
/*                          | Parameter 2: ULONG = 0x00000002                 */
/*                          | Parameter 3: PSZ = 0x000200A4 ["\QUEUES\..."]   */
/*                                                                            */
/*                003A 0001 | Dos32CreateQueue Exit                           */
/*                     PASS | Return code: 0 (NO_ERROR)                       */
/*                          | Parameter 1: PHQUEUE = 0x00028BE8 [0x00000007]  */
/*                                                                            */
/*                The first number  on both  the API entry  and exit lines is */
/*                the hexadecimal process identifier (PID) of the executable. */
/*                The  second  number is the  hexadecimal  thread  identifier */
/*                (TID) of  the thread within the executable that invoked the */
/*                API.                                                        */
/*                                                                            */
/*                If the user requests  level 3 information  and a character, */
/*                ASCIIZ  string,  integer,  color,   FIXED,  POINTL,  RECTL, */
/*                WPOINT,  or user-defined buffer is  larger than the maximum */
/*                specified by the -B n  option,  the  data is  truncated and */
/*                -- More -- is logged.                                       */
/*                                                                            */
/*                If log  file wrapping  occurs,  information similar  to the */
/*                following is  logged after the executable's  stopping time, */
/*                OS2TRACE banner, and executable's starting time:            */
/*                                                                            */
/*                < Trace information lost due to log file wrapping n times > */
/*                                                                            */
/*                If the length of an API's entry/exit information is greater */
/*                than the log file  wrapping length,  information similar to */
/*                the following is logged in lieu of the entry/exit:          */
/*                                                                            */
/*                < API entry/exit information lost >                         */
/*                                                                            */
/*                If  interception   of   dynamic  API   calls   is  enabled, */
/*                information  similar to the following is  logged after each */
/*                Dos16GetProcAddr and/or  Dos32QueryProcAddr call that loads */
/*                an API within a supported DLL that is enabled for tracing:  */
/*                                                                            */
/*                < Intercepting ordinal n in module DLLNAME >                */
/*                                                                            */
/*                NOTE:  Dos16GetProcAddr and  Dos32QueryProcAddr do NOT have */
/*                to be traced in order to  intercept dynamic API calls,  but */
/*                DOSCALLS must be enabled for tracing.                       */
/*                                                                            */
/*                If the user  requests to trace  the DOSCALLS SMP API  group */
/*                and the version of OS/2 is not the multiprocessing version, */
/*                information similar  to the following  is logged after each */
/*                SMP API call:                                               */
/*                                                                            */
/*                < Returning n for API not present on this version of OS/2 > */
/*                                                                            */
/*                If the user requests  to trace the  DOSCALLS SEM API  group */
/*                and a semaphore API removed  by the Convenience Package for */
/*                Warp Server for e-business is called,  information  similar */
/*                to the following is logged:                                 */
/*                                                                            */
/*                < Returning n for API removed by WSeB Convenience Package > */
/*                                                                            */
/*                If tracing is paused when the executable commences running, */
/*                information similar to the following is logged:             */
/*                                                                            */
/*                < Tracing initially paused >                                */
/*                                                                            */
/*                If tracing  is  paused  while  the  executable  is running, */
/*                information similar to the following is logged:             */
/*                                                                            */
/*                < Pausing trace at nn:nn:nn.nn >                            */
/*                                                                            */
/*                If tracing  is resumed  while  the  executable  is running, */
/*                information similar to the following is logged:             */
/*                                                                            */
/*                < Resuming trace at nn:nn:nn.nn >                           */
/*                                                                            */
/*                If the user  requests to  have a user  hook called  and the */
/*                hook  returns  a valid string,  information similar  to the */
/*                following is logged:                                        */
/*                                                                            */
/*                <+ DLLNAME.HOOKNAME(n): string +>                           */
/*                                                                            */
/*                All  warning   and  error   messages   generated   by   the */
/*                trace-enabled  executable  are placed in the  error message */
/*                file OS2TRACE.ERR  in the  root  directory of the operating */
/*                system's  boot drive in order  to avoid conflicts  with the */
/*                executable's output.                                        */
/*                                                                            */
/* Notes        : The following  steps are  required to  add an API  to a DLL */
/*                that is already supported:                                  */
/*                                                                            */
/*                1) Insert a trace API  into the  corresponding  source (.c) */
/*                   file, using the minimum contents described at the end of */
/*                   this section.  A source file  exists for every supported */
/*                   trace DLL with  a file name  that matches  the trace DLL */
/*                   name and an extension of .c.                             */
/*                                                                            */
/*                2) Insert a line containing the  trace API name and ordinal */
/*                   number into  the exports  section of  the  corresponding */
/*                   module definitions file (.def) file:                     */
/*                                                                            */
/*                   Trc_APINAME @ord                                         */
/*                                                                            */
/*                   A module  definitions  file exists  for every  supported */
/*                   trace DLL  with a file name  that matches  the trace DLL */
/*                   name and an extension of .def.                           */
/*                                                                            */
/*                   NOTE: The trace API must have the same ordinal number as */
/*                   the OS/2 API it is replacing.                            */
/*                                                                            */
/*                3) Update  the minimum  or maximum  ordinals in  the  DLL's */
/*                   DLLINFO array entry (located in DLLINFO.H) if necessary: */
/*                                                                            */
/*                   { "DLLNAME", 386minord, 386maxord, PPCminord, PPCmaxord, */
/*                     "TRCNAME", FALSE, 0L},                                 */
/*                                                                            */
/*                The following  steps are required to add  a DLL that is not */
/*                already supported:                                          */
/*                                                                            */
/*                1) Create a source  file with a file name  that matches the */
/*                   trace DLL name and an extension of .c.                   */
/*                                                                            */
/*                   NOTE:  The trace DLL  name must have the  same length as */
/*                   the OS/2 DLL name it is replacing.                       */
/*                                                                            */
/*                2) Create a module definitions file  with a file name  that */
/*                   matches the trace DLL name and an extension of  .def and */
/*                   place every API exported from the OS/2 DLL into both the */
/*                   exports  and  imports  section  so  that  all  APIs  are */
/*                   forwarded.                                               */
/*                                                                            */
/*                   NOTE:  Only OS/2  2.00 APIs  should be  placed into  the */
/*                   imports  section  of the  module  definitions  file,  as */
/*                   DosQueryProcAddr  is  used   to  call  APIs  added  into */
/*                   subsequent  versions  of   the  operating  system.  This */
/*                   enables the trace DLL to be  used on all 32-bit versions */
/*                   of OS/2.                                                 */
/*                                                                            */
/*                3) Place an instance  _DLL_InitTerm routine  in the  source */
/*                   file  that   makes  the   following  calls  during   DLL */
/*                   initialization:                                          */
/*                                                                            */
/*                   a) _fpreset( ) to reset FP unit to default state         */
/*                   b) _CRT_init( ) to initialize C run-time                 */
/*                   c) __ctordtorInit( ) to initialize C++ run-time          */
/*                   d) DosLoadModule to load corresponding OS/2 DLL          */
/*                   e) DosLoadModule to load trace DLL (required in order to */
/*                      trace dynamic API calls)                              */
/*                   f) T_RegisterDLL to register trace DLL                   */
/*                   g) DosQueryProcAddr to obtain the addresses of each API  */
/*                      added after OS/2 2.00                                 */
/*                                                                            */
/*                   and the following calls during DLL termination:          */
/*                                                                            */
/*                   a) DosFreeModule to unload corresponding OS/2 DLL        */
/*                   b) __ctordtorTerm( ) to terminate C++ run-time           */
/*                   c) _CRT_term( ) to terminate C run-time                  */
/*                                                                            */
/*                   NOTE:  Do not unload  trace DLL,  as there  is a problem */
/*                   present  on all  32-bit  versions  of OS/2  which  could */
/*                   result in a trap.                                        */
/*                                                                            */
/*                   NOTE: Steps a-c under initialization and steps b-c under */
/*                   termination should be enclosed in an:                    */
/*                                                                            */
/*                   #ifndef T_COMRUNTIME-#endif                              */
/*                                                                            */
/*                   preprocessor clause.                                     */
/*                                                                            */
/*                5) Insert the lines necessary to compile and link the trace */
/*                   DLL into TRACEDLL.MAK:                                   */
/*                                                                            */
/*                   trcname.dll: trcname.obj t_common.lib trcname.def        */
/*                           $(link) $(lflags) $*.obj,$*.dll,,t_common,$*.def */
/*                                                                            */
/*                   trcname.obj: trcname.c t_common.h tracedll.mak           */
/*                                                                            */
/*                6) Insert  a line containing  the  DLL's name,  minimum and */
/*                   maximum  Intel  ordinal,  minimum  and  maximum  PowerPC */
/*                   ordinal,  trace DLL name,  enablement flag (FALSE),  and */
/*                   flags (0L) into the DLLINFO array (located in DLLINFO.H) */
/*                   in alphabetical order based on the DLL's name:           */
/*                                                                            */
/*                   { "DLLNAME", 386minord, 386maxord, PPCminord, PPCmaxord, */
/*                     "TRCNAME", FALSE, 0L },                                */
/*                                                                            */
/*                7) Increment SUPPORTED_DLL constant in OS2TRACE.H.          */
/*                                                                            */
/*                8) Follow the steps  above for adding an  API to a DLL that */
/*                   is already supported for each trace API.                 */
/*                                                                            */
/*                Each trace API must at a minimum contain the following:     */
/*                                                                            */
/*                1) The entire  API needs to  be enclosed  in an  #if-#endif */
/*                   preprocessor  clause which  defines the version  of OS/2 */
/*                   where the API was first available.                       */
/*                                                                            */
/*                2) One line  defining  local  variable for  the API  return */
/*                   code.                                                    */
/*                                                                            */
/*                3) One line calling T_LogAPIEntry to log the entry into the */
/*                   API.                                                     */
/*                                                                            */
/*                4) At least two lines  for each API parameter that logs the */
/*                   type  of  the  parameter  and  the  parameter  itself if */
/*                   logging level 2 or 3 information.                        */
/*                                                                            */
/*                5) One line calling the API directly if the API was present */
/*                   in  OS/2 2.00,  or one  line  calling  the  API  through */
/*                   indirection  using the  function  pointer  returned from */
/*                   DosQueryProcAddr in  _DLL_InitTerm  if the API was added */
/*                   after OS/2 2.00.                                         */
/*                                                                            */
/*                6) One line calling  T_LogAPIExit to log  the exit from the */
/*                   API.                                                     */
/*                                                                            */
/*                7) One line that logs the return code.                      */
/*                                                                            */
/*                8) At least  two lines for each  API output  parameter that */
/*                   logs the type  of the parameter and the parameter itself */
/*                   if logging level 2 or 3 information.                     */
/*                                                                            */
/*                9) One line returning the API return code.                  */
/*                                                                            */
/*                All logging  calls must be  made  after invoking  T_WaitLog */
/*                and before invoking  T_FreeLog.  API entry  and exit  times */
/*                can be logged by invoking DosGetDateTime immediately before */
/*                and after  calling the API  and then passing  the  DATETIME */
/*                structures to T_LogTimes.  If the trace API  is 16-bit Dos, */
/*                T_SaveRegs16  must be  invoked  immediately  after defining */
/*                local  variables  and   T_RestoreRegs16   must  be  invoked */
/*                immediately before returning the API return code.           */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.30.00 12Jun95 Created (supported 32-bit Dos APIs only)    */
/*                2.30.01 09Jul95 Issued warning if trace DLL not on LIBPATH  */
/*                2.30.02 18Jul95 Added optional "NO" prefix to API groups    */
/*                2.30.03 20Jul95 Added 32-bit Win APIs                       */
/*                2.30.05 04Aug95 Added 32-bit Gpi APIs                       */
/*                2.30.09 12Aug95 Added 32-bit Dev APIs                       */
/*                2.30.10 13Aug95 Added 32-bit Drg APIs                       */
/*                2.30.13 27Aug95 Added OS/2 for PowerPC support              */
/*                2.30.14 29Aug95 Added 32-bit Prt and Spl APIs               */
/*                2.30.15 29Aug95 Added 32-bit Pic APIs                       */
/*                2.30.16 22Sep95 Enhanced -B option                          */
/*                2.30.17 22Sep95 Added -F option                             */
/*                2.30.18 24Sep95 Added -T option                             */
/*                2.30.21 13Dec95 Forwarded private entry table ordinals      */
/*                2.30.22 19Dec95 Added new OS/2 for PowerPC non-Uni Dos APIs */
/*                2.30.24 29Mar96 Added PM interface (supported customization */
/*                                and help only)                              */
/*                2.30.25 15Apr96 Verified DOS header new header file address */
/*                2.30.26 23May96 Issued more specific enablement messages    */
/*                2.30.28 14Jun96 Changed "\r\n" to "\n" in output            */
/*                2.40.00 05Sep96 Added new OS/2 2.40 APIs                    */
/*                2.40.02 11Mar97 Removed loading/unloading trace DLLs        */
/*                2.40.03 11Mar97 Converted to IBM VisualAge C++              */
/*                2.40.04 12Mar97 Fixed OS/2 for PowerPC DLL support          */
/*                2.40.09 14Mar97 Removed  summarization   API  exit  without */
/*                                entry error                                 */
/*                2.40.12 19Mar97 Added support for NE format                 */
/*                2.40.17 21Apr97 Added 16-bit Dos APIs                       */
/*                2.40.18 30Apr97 Added 16-bit Kbd APIs                       */
/*                2.40.19 01May97 Added 16-bit Mou APIs                       */
/*                2.40.20 05May97 Added 16-bit Vio APIs                       */
/*                2.40.26 24Jun97 Added 16-bit Win APIs                       */
/*                2.40.36 14Aug97 Added -I option                             */
/*                2.40.45 28Jan98 Added indeterminate API return codes        */
/*                2.40.46 28Jan98 Fixed summarization alphabetical API bug    */
/*                2.40.47 29Jan98 Added Win16DefAVioWindowProc                */
/*                2.40.61 17Aug98 Added SMP Dos APIs                          */
/*                2.40.68 12Oct98 Fixed/enhanced helping information          */
/*                2.40.74 27Oct98 Added WinSetErrorInfo                       */
/*                2.40.75 28Oct98 Added Gpi/Win APIs exported from PMBIDI     */
/*                2.40.78 16Nov98 Added miscellaneous Dos APIs                */
/*                2.40.80 02Dec98 Added undocumented -DUMPSTACK option        */
/*                2.40.81 02Dec98 Added -E option                             */
/*                2.40.85 21Jan99 Added -P option                             */
/*                2.40.86 03Mar99 Added support  for executables  without DOS */
/*                                header                                      */
/*                2.45.04 05May99 Added -Q option                             */
/*                2.45.05 09May99 Added support for  K and M  suffixes to  -B */
/*                                and -F options                              */
/*                2.45.08 11Jun99 Added support for .COM files                */
/*                2.45.12 01Feb00 Displayed customization options on new line */
/*                2.45.14 02Feb00 Saved OS2.INI information only when changed */
/*                2.45.15 08Feb00 Standardized output                         */
/*                2.45.16 18Feb00 Added -C, -PAUSE, and -RESUME options       */
/*                2.45.18 28Feb00 Removed import by name restriction          */
/*                2.45.24 01Jun00 Added -A option                             */
/*                2.45.32 28Jun01 Added -U option                             */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#define  INCL_BASE                               /* Include base functions    */
#define  INCL_PM                                 /* Include PM functions      */
#include <os2.h>                                 /* OS/2 information          */
#include <newexe.h>                              /* DOS/NE executable info    */
#define  FOR_EXEHDR 1                            /* Avoid multiple definition */
typedef  unsigned short WORD;                    /* Required by EXE386.H      */
#ifdef INTEL
typedef  unsigned long DWORD;                    /* Required by EXE386.H      */
#endif
#include <exe386.h>                              /* LX executable info        */
#include <ctype.h>                               /* C information             */
#include <malloc.h>                              /* C information             */
#include <stdarg.h>                              /* C information             */
#include <stdio.h>                               /* C information             */
#include <stdlib.h>                              /* C information             */
#include <string.h>                              /* C information             */
#include "os2trace.h"                            /* OS2TRACE information      */

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

/******************************************************************************/
/*                                                                            */
/* Function declarations                                                      */
/*                                                                            */
/******************************************************************************/

void           main( int, char ** );
PAPIINFO       AddAPIInfo( PCHAR, ULONG, ULONG, ULONG, ULONG );
VOID           AnalyzeAPIUsage( VOID );
int            Atoi( PCHAR );
VOID           ControlAPITracing( VOID );
VOID           CustomizeAPITracing( VOID );
VOID           DisplayBanner( VOID );
VOID           DisplayDOSCALLSGroups( ULONG );
VOID           DisplayPMGPIGroups( ULONG );
VOID           DisplayPMWINGroups( ULONG );
VOID           EnableAPITracing( VOID );
VOID           Error( PCHAR, ... );
ULONG APIENTRY ExceptionHandler( PREPREC, PREGREC, PCTXREC, PVOID );
PAPIINFO       GetAPIInfo( PCHAR );
VOID           InitializeEnablement( VOID );
BOOL           IsUnsupportedNRRNAM( PCHAR, PCHAR, ULONG );
ULONG          LookupDLL( PCHAR );
ULONG          LookupTraceDLL( PCHAR );
VOID           ParseCommandLine( int, char ** );
LONG           Strcmpi( PCHAR, PCHAR );
PCHAR          Strupr( PCHAR );
VOID           SummarizeAPITracing( VOID );
VOID           TerminateEnablement( BOOL );
VOID           TerminateSummarization( BOOL );
VOID           Usage( VOID );

/******************************************************************************/
/*                                                                            */
/* Variable definitions                                                       */
/*                                                                            */
/******************************************************************************/

FDATE     fdateInFile;                           /* Input file write date     */
FILE     *fpInFile;                              /* Input file pointer        */
FTIME     ftimeInFile;                           /* Input file write time     */
HMTX      hmtxSys;                               /* System semaphore          */
PAPIINFO  pAPIFirst;                             /* First API information     */
PCHAR     pFixupTbl;                             /* Fixup record table        */
PCHAR     pImpNamTbl;                            /* Import name table         */
PULONG    pModIdxTbl;                            /* Module index table        */
PCHAR     pModNamTbl;                            /* Module name table         */
PUSHORT   pModRefTbl;                            /* Module reference table    */
NEHDR    *pNEHdr;                                /* NE executable header      */
PCHAR     pPrcNamTbl;                            /* Procedure name table      */
PCHAR     pSegment;                              /* Segment                   */
NESEG    *pSegmentTbl;                           /* Segment table             */
PCHAR     pStrTbl;                               /* String table              */
LXHDR     sLXHdr;                                /* LX executable header      */
MZHDR     sMZHdr;                                /* DOS executable header     */
CHAR      szAltDir[ LEN_FILENAME ];              /* Alternative directory     */
PCHAR     szAltDirKey = ALTDIR_KEY;              /* Alternative directory key */
PCHAR     szAppName = VER_NAME;                  /* OS2TRACE application name */
PCHAR     szBufLenKey = BUFLEN_KEY;              /* Buffer length key name    */
PCHAR     szCtlFlgKey = CTLFLG_KEY;              /* Trace ctrl flags key name */
PCHAR     szDosGroups[] =                        /* DOSCALLS API groups       */
          { "DEV" , "FILE", "MEM" , "MISC", "MOD"
          , "MVDM", "NLS" , "PIPE", "PRF" , "PROC"
          , "RES" , "SEM" , "SES" , "TIME", "XCPT"
          , "MSG" , "INFO", "SIG" , "SMP" , "PROF"
          };
PCHAR     szDosGrpKey = DOSGRP_KEY;              /* DOSCALLS groups key name  */
PCHAR     szFilLenKey = FILLEN_KEY;              /* File wrap length key name */
PCHAR     szGpiGroups[] =                        /* PMGPI API groups          */
          { "BIT" , "CORR", "CTRL", "DEF" , "EDIT"
          , "LCID", "LCT" , "META", "PATH", "POLY"
          , "PRIM", "RGN" , "SEG" , "TRAN", "DEV"
          , "INK"
          };
PCHAR     szGpiGrpKey = GPIGRP_KEY;              /* PMGPI groups key name     */
PCHAR     szGrpDelimiter = ",";                  /* API groups delimiters     */
CHAR      szInFile[ LEN_FILENAME ];              /* Input file name           */
PCHAR     szTrcFlgKey = TRCFLG_KEY;              /* Trace flags key name      */
PCHAR     szTrcLvlKey = TRCLVL_KEY;              /* Trace level key name      */
#ifdef T_USERHOOK
CHAR      szUserHk[ LEN_FILENAME ];              /* User hook                 */
PCHAR     szUserHkKey = USERHK_KEY;              /* User hook key             */
#endif
PCHAR     szWinGroups[] =                        /* PMWIN API groups          */
          { "ACCL", "ATOM", "CLIP", "CTRY", "CUR"
          , "DDE" , "DESK", "DLG" , "DWIN", "ERR"
          , "FRAM", "HOOK", "INPT", "LOAD", "MENU"
          , "MSG" , "MSGL", "PAL" , "PTR" , "RECT"
          , "SYS" , "THK" , "TIME", "TREC", "WIN"
          , "ENV" , "HEAP", "SEI"
          };
PCHAR     szWinGrpKey = WINGRP_KEY;              /* PMWIN groups key name     */
ULONG     ulBufLen;                              /* Buffer length             */
ULONG     ulDosGrp;                              /* DOSCALLS groups           */
ULONG     ulFilLen;                              /* File wrapping length      */
ULONG     ulFixupTbl;                            /* Fixup record table size   */
ULONG     ulFlags;                               /* Flags                     */
ULONG     ulGpiGrp;                              /* PMGPI groups              */
ULONG     ulImpNamTbl;                           /* Import name table size    */
ULONG     ulModIdxTbl;                           /* Module index table size   */
ULONG     ulModNamTbl;                           /* Module name table size    */
ULONG     ulModRefTbl;                           /* Module reference tbl size */
ULONG     ulPrcNamTbl;                           /* Procedure name table size */
ULONG     ulSegment;                             /* Number segments           */
ULONG     ulSegmentTbl;                          /* Segment table size        */
ULONG     ulStrTbl;                              /* String table size         */
ULONG     ulStrTblOff;                           /* String table file offset  */
ULONG     ulTrcFlg;                              /* Trace flags               */
ULONG     ulTrcLvl;                              /* Trace level               */
ULONG     ulWinGrp;                              /* PMWIN groups              */
USHORT    usSector;                              /* Sector size               */

#include  "dllinfo.h"                            /* DLL information table     */
#include  "nrrnam.h"                             /* Unsupported NRRNAM info   */

/******************************************************************************/
/*                                                                            */
/* main()                                                                     */
/*                                                                            */
/******************************************************************************/

void main( int    argc                           /* Argument count            */
         , char **argv                           /* Argument pointer          */
         )
{

   REGREC sRegRec;                               /* Registration record       */

   /*
    * Register exception handler
    */
   sRegRec.ExceptionHandler = ExceptionHandler;
   DosSetExceptionHandler( &sRegRec );

   /*
    * Display program banner
    */
   DisplayBanner( );

   /*
    * Parse command line arguments
    */
   ParseCommandLine( --argc, ++argv );

   /*
    * Create system mutual exclusion semaphore, issue error and terminate if
    * semaphore already exists
    */
   if ( DosCreateMutexSem( SYS_SEMAPHORE, &hmtxSys, 0L, FALSE ) == ERROR_DUPLICATE_NAME )
   {
      Error( "Another instance of %s is already running", VER_FULLNAME );
   }

   /*
    * Enable API tracing if command line so indicates
    */
   if ( FLAGSET( ulFlags, fENABLE ) )
   {
      /*
       * Read executable file and initialize tables
       */
      InitializeEnablement( );

      /*
       * Analyze API usage in executable file if enabling API tracing
       */
      if ( FLAGSET( ulFlags, fTRACE_ON ) )
      {
         AnalyzeAPIUsage( );
      }

      /*
       * Enable or disable API tracing in executable file
       */
      EnableAPITracing( );

      /*
       * Clean up tables
       */
      TerminateEnablement( TRUE );
   }

   /*
    * Customize API tracing if command line so indicates
    */
   else if ( FLAGSET( ulFlags, fCUSTOM ) )
   {
      /*
       * Customize API tracing in operating system profile
       */
      CustomizeAPITracing( );
   }

   /*
    * Control API tracing if command line so indicates
    */
   else if ( FLAGSET( ulFlags, fCONTROL ) )
   {
      /*
       * Control API tracing
       */
      ControlAPITracing( );
   }

   /*
    * Summarize API tracing if command line so indicates
    */
   else if ( FLAGSET( ulFlags, fSUMMARY ) )
   {
      /*
       * Summarize API tracing
       */
      SummarizeAPITracing( );
   }

   /*
    * Delete system semaphore
    */
   DosCloseMutexSem( hmtxSys );

   /*
    * Unregister exception handler
    */
   DosUnsetExceptionHandler( &sRegRec );

   /*
    * Terminate program
    */
   exit( NO_ERROR );

}  /* main() */

/******************************************************************************/
/*                                                                            */
/* AddAPIInfo()                                                               */
/*                                                                            */
/******************************************************************************/

PAPIINFO AddAPIInfo( PCHAR szName                /* API name                  */
                   , ULONG ulCall                /* API invocations           */
                   , ULONG ulPass                /* Passed API invocations    */
                   , ULONG ulFail                /* Failed API invocations    */
                   , ULONG ulIndeterminate       /* Indeterminate invocations */
                   )
{

   LONG     lResult;                             /* String comparison result  */
   PAPIINFO pAPI;                                /* API information           */
   PAPIINFO pAPINew;                             /* New API information       */
   PAPIINFO pAPIPrev = NULL;                     /* Previous API information  */

   /*
    * Allocate memory for new API information, issue error and terminate if
    * unsuccessful
    */
   if ( !( pAPINew = malloc( sizeof( APIINFO ) ) ) )
   {
      Error( "Unable to allocate memory for API information" );
   }

   /*
    * Initialize new API information
    */
   pAPINew->pNext           = NULL;
   strcpy( pAPINew->szName, szName );
   pAPINew->ulCall          = ulCall;
   pAPINew->ulPass          = ulPass;
   pAPINew->ulFail          = ulFail;
   pAPINew->ulIndeterminate = ulIndeterminate;

   /*
    * Place new API information at beginning of API information chain if
    * initial API or new API name precedes initial API name alphabetically
    */
   if ( !( pAPI = pAPIFirst ) || ( strcmp( pAPI->szName, szName ) > 0 ) )
   {
      /*
       * Place new API information at beginning of API information chain
       */
      pAPINew->pNext = pAPIFirst;
      pAPIFirst = pAPINew;
   }

   /*
    * Place new API information alphabetically in API information chain
    */
   else
   {
      /*
       * Search for API name in API information chain that succeeds new API
       * name alphabetically or until end of information chain reached
       */
      while ( ( ( lResult = strcmp( pAPI->szName, szName ) ) < 0 ) && ( pAPI->pNext ) )
      {
         pAPIPrev = pAPI;
         pAPI = pAPI->pNext;
      }

      /*
       * Place new API information at end of API information chain if end of
       * information chain reached
       */
      if ( ( lResult < 0 ) && ( !pAPI->pNext ) )
      {
         pAPI->pNext = pAPINew;
      }

      /*
       * Place new API information in midst of API information chain if new API
       * name succeeds current API name alphabetically
       */
      else
      {
         pAPIPrev->pNext = pAPINew;
         pAPINew->pNext = pAPI;
      }
   }

   /*
    * Return new API information
    */
   return pAPINew;

}  /* AddAPIInfo() */

/******************************************************************************/
/*                                                                            */
/* AnalyzeAPIUsage()                                                          */
/*                                                                            */
/******************************************************************************/

VOID AnalyzeAPIUsage( VOID )                     /* No parameters             */
{

   CHAR   chChnCnt;                              /* Source offset list count  */
   CHAR   chSrcTyp;                              /* Source type               */
   CHAR   chTgtFlg;                              /* Target flag               */
   int    i = 0, j;                              /* Loop variables            */
   PCHAR  pch;                                   /* Character pointer         */
   ULONG  ulDLLIdx;                              /* DLL information index     */
   ULONG  ulImpOrd;                              /* Imported ordinal number   */
   ULONG  ulPrcOff;                              /* Proc name table offset    */
   USHORT usFixups;                              /* Number fixup records      */
   USHORT usModOrd;                              /* Module name table index   */
   USHORT usSrcOff;                              /* Source offset             */

   /*
    * Analyze API usage in NE executable file
    */
   if ( FLAGSET( ulFlags, fNE_FMT ) )
   {
      /*
       * Analyze each segment in executable file for fixup records (fixup
       * records, if present, are grouped with corresponding segment data)
       */
      for ( i = 0; i < ulSegment; i++ )
      {
         /*
          * Analyze segment if segment contains fixup records
          */
         if ( FLAGSET( pSegmentTbl[ i ].ns_flags, NSRELOC ) )
         {
            /*
             * Allocate memory for segment, issue error and terminate if
             * unsuccessful
             */
            if ( !( pSegment = malloc( pSegmentTbl[ i ].ns_cbseg ) ) )
            {
               Error( "Unable to allocate memory for segment: %s", szInFile );
            }

            /*
             * Locate beginning of segment, read segment, and read number of
             * fixup records; issue error and terminate if unsuccessful
             */
            if ( ( fseek( fpInFile, pSegmentTbl[ i ].ns_sector * usSector, SEEK_SET ) ) ||
                 ( fread( pSegment, pSegmentTbl[ i ].ns_cbseg, 1, fpInFile ) != 1     ) ||
                 ( fread( &usFixups, sizeof( USHORT ), 1, fpInFile ) != 1             ) )
            {
               Error( "Unable to read segment: %s", szInFile );
            }

            /*
             * Determine fixup record table size based on number of fixup
             * records and size of fixup record
             */
            ulFixupTbl = usFixups * sizeof( NERLC );

            /*
             * Allocate memory for fixup record table, issue error and
             * terminate if unsuccessful
             */
            if ( !( pFixupTbl = malloc( ulFixupTbl ) ) )
            {
               Error( "Unable to allocate memory for fixup record table: %s", szInFile );
            }

            /*
             * Read fixup record table, issue error and terminate if
             * unsuccessful
             */
            if ( fread( pFixupTbl, ulFixupTbl, 1, fpInFile ) != 1 )
            {
               Error( "Unable to read fixup record table: %s", szInFile );
            }

            /*
             * Analyze each fixup record in fixup record table
             */
            j = 0;
            while ( j < ulFixupTbl )
            {
               /*
                * Skip source type
                */
               j++;

               /*
                * Extract target flags and source offset from fixup record
                */
               chTgtFlg = pFixupTbl[ j++ ];
               usSrcOff = *(PUSHORT)( pFixupTbl + j );
               j += sizeof( USHORT );

               /*
                * Ignore fixup record if internal fixup
                */
               if ( ( chTgtFlg & NRRTYP ) == NRRINT )
               {
                  /*
                   * Skip remainder of fixup record
                   */
                  j += sizeof( ULONG );
               }

               /*
                * Process fixup record if import by ordinal fixup
                */
               else if ( ( chTgtFlg & NRRTYP ) == NRRORD )
               {
                  /*
                   * Extract module name table index and imported ordinal
                   * number from fixup record
                   */
                  usModOrd = *(PUSHORT)( pFixupTbl + j );
                  j += sizeof( USHORT );
                  ulImpOrd = (ULONG)*(PUSHORT)( pFixupTbl + j );
                  j += sizeof( USHORT );

                  /*
                   * Determine DLL information index (module index table
                   * zero-based, module name table index one-based)
                   */
                  ulDLLIdx = pModIdxTbl[ usModOrd - 1 ];

                  /*
                   * Issue error and disable trace on/off if fixup record
                   * references supported DLL with trace on/off enabled and
                   * ordinal not in DLL information table
                   */
                  if ( ( ulDLLIdx != UNKNOWN_DLL                                                                          ) &&
                       ( FLAGSET( DLLInfo[ ulDLLIdx ].ulFlags, fDLL )                                                     ) &&
                       ( DLLInfo[ ulDLLIdx ].bEnable                                                                      ) &&
                       ( ( ulImpOrd < DLLInfo[ ulDLLIdx ].ulMinOrd386 ) || ( ulImpOrd > DLLInfo[ ulDLLIdx ].ulMaxOrd386 ) ) )
                  {
                     /*
                      * Issue error
                      */
                     printf( "  %-8s: File imports unsupported ordinal %lu, API tracing cannot be enabled\n", DLLInfo[ ulDLLIdx ].szName, ulImpOrd );

                     /*
                      * Indicate trace on/off disabled
                      */
                     DLLInfo[ ulDLLIdx ].bEnable = FALSE;
                  }
               }

               /*
                * Process fixup record if import by name fixup
                */
               else if ( ( chTgtFlg & NRRTYP ) == NRRNAM )
               {
                  /*
                   * Extract module name table index and procedure name table
                   * offset from fixup record
                   */
                  usModOrd = *(PUSHORT)( pFixupTbl + j );
                  j += sizeof( USHORT );
                  ulPrcOff = (ULONG)*(PUSHORT)( pFixupTbl + j );
                  j += sizeof( USHORT );

                  /*
                   * Determine DLL information index (module index table
                   * zero-based, module name table index one-based)
                   */
                  ulDLLIdx = pModIdxTbl[ usModOrd - 1 ];

                  /*
                   * Check for unsupported API name if fixup record references
                   * supported DLL with trace on/off enabled
                   */
                  if ( ( ulDLLIdx != UNKNOWN_DLL                      ) &&
                       ( FLAGSET( DLLInfo[ ulDLLIdx ].ulFlags, fDLL ) ) &&
                       ( DLLInfo[ ulDLLIdx ].bEnable                  ) )
                  {
                     /*
                      * Obtain imported API name
                      */
                     pch = (PCHAR)( (ULONG)pImpNamTbl + ulPrcOff );

                     /*
                      * Issue error and disable trace on/off if unsupported API
                      * name
                      */
                     if ( IsUnsupportedNRRNAM( DLLInfo[ ulDLLIdx ].szName, pch + 1, *pch ) )
                     {
                        /*
                         * Issue error
                         */
                        printf( "  %-8s: File imports %.*s by name, API tracing cannot be enabled\n", DLLInfo[ ulDLLIdx ].szName, *pch, pch + 1 );

                        /*
                         * Indicate trace on/off disabled
                         */
                        DLLInfo[ ulDLLIdx ].bEnable = FALSE;
                     }
                  }
               }

               /*
                * Ignore fixup record if operating system fixup
                */
               else if ( ( chTgtFlg & NRRTYP ) == NRROSF )
               {
                  /*
                   * Skip remainder of fixup record
                   */
                  j += sizeof( ULONG );
               }
            }

            /*
             * Free memory for segment
             */
            free( pSegment );
            pSegment = NULL;

            /*
             * Free memory for fixup record table
             */
            free( pFixupTbl );
            pFixupTbl = NULL;
         }
      }
   }

   /*
    * Analyze API usage in LX executable file
    */
   else if ( FLAGSET( ulFlags, fLX_FMT ) )
   {
      /*
       * Analyze each fixup record in fixup record table
       */
      while ( i < ulFixupTbl )
      {
         /*
          * Extract source type and target flags from fixup record
          */
         chSrcTyp = pFixupTbl[ i++ ];
         chTgtFlg = pFixupTbl[ i++ ];

         /*
          * Extract source offset list count from fixup record if fixup record
          * contains chain of source offsets
          */
         if ( FLAGSET( chSrcTyp, NRCHAIN ) )
         {
            chChnCnt = pFixupTbl[ i++ ];
         }

         /*
          * Source offset list count is one if fixup record contains single
          * source offset
          */
         else
         {
            chChnCnt = 1;
            i += sizeof( USHORT );
         }

         /*
          * Extract 16-bit module name table index from fixup record if
          * appropriate flag set
          */
         if ( FLAGSET( chTgtFlg, NR16OBJMOD ) )
         {
            usModOrd = *(PUSHORT)( pFixupTbl + i );
            i += sizeof( USHORT );
         }

         /*
          * Extract 8-bit module name table index from fixup record if
          * appropriate flag clear
          */
         else
         {
            usModOrd = (USHORT)pFixupTbl[ i++ ];
         }

         /*
          * Ignore fixup record if internal fixup
          */
         if ( ( chTgtFlg & NRRTYP ) == NRRINT )
         {
            /*
             * Fixup record contains target offset if not 16-bit selector fixup
             * record
             */
            if ( ( chSrcTyp & NRSTYP ) != NRSSEG )
            {
               /*
                * Skip 32-bit target offset from fixup record if appropriate
                * flag set
                */
               if ( FLAGSET( chTgtFlg, NR32BITOFF ) )
               {
                  i += sizeof( ULONG );
               }

               /*
                * Skip 16-bit target offset from fixup record if appropriate
                * flag clear
                */
               else
               {
                  i += sizeof( USHORT );
               }
            }
         }

         /*
          * Process fixup record if import by ordinal fixup
          */
         else if ( ( chTgtFlg & NRRTYP ) == NRRORD )
         {
            /*
             * Extract 8-bit imported ordinal number from fixup record if
             * appropriate flag set
             */
            if ( FLAGSET( chTgtFlg, NR8BITORD ) )
            {
               ulImpOrd = (ULONG)pFixupTbl[ i++ ];
            }

            /*
             * Extract 32-bit imported ordinal number from fixup record if
             * appropriate flag set
             */
            else if ( FLAGSET( chTgtFlg, NR32BITOFF ) )
            {
               ulImpOrd = *(PULONG)( pFixupTbl + i );
               i += sizeof( ULONG );
            }

            /*
             * Extract 16-bit imported ordinal number from fixup record if
             * neither flag set
             */
            else
            {
               ulImpOrd = (ULONG)*(PUSHORT)( pFixupTbl + i );
               i += sizeof( USHORT );
            }

            /*
             * Determine DLL information index (module index table zero-based,
             * module name table index one-based)
             */
            ulDLLIdx = pModIdxTbl[ usModOrd - 1 ];

            /*
             * Issue error and disable trace on/off if fixup record references
             * supported DLL with trace on/off enabled and ordinal not in DLL
             * information table
             */
            if ( ( ulDLLIdx != UNKNOWN_DLL                                                                          ) &&
                 ( FLAGSET( DLLInfo[ ulDLLIdx ].ulFlags, fDLL )                                                     ) &&
                 ( DLLInfo[ ulDLLIdx ].bEnable                                                                      ) &&
                 ( ( ulImpOrd < DLLInfo[ ulDLLIdx ].ulMinOrd386 ) || ( ulImpOrd > DLLInfo[ ulDLLIdx ].ulMaxOrd386 ) ) )
            {
               /*
                * Issue error
                */
               printf( "  %-8s: File imports unsupported ordinal %lu, API tracing cannot be enabled\n", DLLInfo[ ulDLLIdx ].szName, ulImpOrd );

               /*
                * Indicate trace on/off disabled
                */
               DLLInfo[ ulDLLIdx ].bEnable = FALSE;
            }
         }

         /*
          * Process fixup record if import by name fixup
          */
         else if ( ( chTgtFlg & NRRTYP ) == NRRNAM )
         {
            /*
             * Extract 32-bit procedure name table offset from fixup record if
             * appropriate flag set
             */
            if ( FLAGSET( chTgtFlg, NR32BITOFF ) )
            {
               ulPrcOff = *(PULONG)( pFixupTbl + i );
               i += sizeof( ULONG );
            }

            /*
             * Extract 16-bit procedure name table offset from fixup record if
             * appropriate flag clear
             */
            else
            {
               ulPrcOff = (ULONG)*(PUSHORT)( pFixupTbl + i );
               i += sizeof( USHORT );
            }

            /*
             * Determine DLL information index (module index table zero-based,
             * module name table index one-based)
             */
            ulDLLIdx = pModIdxTbl[ usModOrd - 1 ];

            /*
             * Check for unsupported API name if fixup record references
             * supported DLL with trace on/off enabled
             */
            if ( ( ulDLLIdx != UNKNOWN_DLL                      ) &&
                 ( FLAGSET( DLLInfo[ ulDLLIdx ].ulFlags, fDLL ) ) &&
                 ( DLLInfo[ ulDLLIdx ].bEnable                  ) )
            {
               /*
                * Obtain imported API name
                */
               pch = (PCHAR)( (ULONG)pPrcNamTbl + ulPrcOff );

               /*
                * Issue error and disable trace on/off if unsupported API name
                */
               if ( IsUnsupportedNRRNAM( DLLInfo[ ulDLLIdx ].szName, pch + 1, *pch ) )
               {
                  /*
                   * Issue error
                   */
                  printf( "  %-8s: File imports %.*s by name, API tracing cannot be enabled\n", DLLInfo[ ulDLLIdx ].szName, *pch, pch + 1 );

                  /*
                   * Indicate trace on/off disabled
                   */
                  DLLInfo[ ulDLLIdx ].bEnable = FALSE;
               }
            }
         }

         /*
          * Fixup record contains additive value if not internal fixup record
          * and appropriate flag set
          */
         if ( ( ( chTgtFlg & NRRTYP ) != NRRINT ) && ( FLAGSET( chTgtFlg, NRADD ) ) )
         {
            /*
             * Skip 32-bit additive value if appropriate flag set
             */
            if ( FLAGSET( chTgtFlg, NR32BITADD ) )
            {
               i += sizeof( ULONG );
            }

            /*
             * Skip 16-bit additive value if appropriate flag clear
             */
            else
            {
               i += sizeof( USHORT );
            }
         }

         /*
          * Skip remainder of fixup record if fixup record contains chain of
          * source offsets
          */
         if ( FLAGSET( chSrcTyp, NRCHAIN ) )
         {
            i += chChnCnt * sizeof( USHORT );
         }
      }
   }

}  /* AnalyzeAPIUsage() */

/******************************************************************************/
/*                                                                            */
/* Atoi()                                                                     */
/*                                                                            */
/******************************************************************************/

int Atoi( PCHAR szInteger )                      /* Integer string            */
{

   PCHAR pch;                                    /* Character pointer         */
   ULONG ulMult = 1L;                            /* Multiplier                */

   /*
    * Process each character in integer string
    */
   for ( pch = szInteger; *pch; pch++ )
   {
      /*
       * Processing complete if non-digit encountered
       */
      if ( !isdigit( *pch ) )
      {
         /*
          * Non-digit valid only if last character
          */
         if ( *(pch + 1) == '\0' )
         {
            /*
             * Kilobyte multiplier if last character K
             */
            if ( ( *pch == 'K' ) || ( *pch == 'k' ) )
            {
               ulMult = 1024;
            }

            /*
             * Megabyte multiplier if last character M
             */
            else if ( ( *pch == 'M' ) || ( *pch == 'm' ) )
            {
               ulMult = 1048576;
            }

            /*
             * Invalid integer string if last character not K or M
             */
            else
            {
               return 0;
            }
         }

         /*
          * Invalid integer string if non-digit not last character
          */
         else
         {
            return 0;
         }
      }
   }

   /*
    * Return integer, taking into account possible multiplier
    */
   return atoi( szInteger ) * ulMult;

}  /* Atoi() */

/******************************************************************************/
/*                                                                            */
/* ControlAPITracing()                                                        */
/*                                                                            */
/******************************************************************************/

VOID ControlAPITracing( VOID )                   /* No parameters             */
{

   BYTE        bPriority;                        /* Queue data priority       */
   HEV         hevCtl = 0L;                      /* Trace control semaphore   */
   HQUEUE      hqueueCtl;                        /* Trace control queue       */
   int         i;                                /* Loop variable             */
   PVOID       pData;                            /* Queue data                */
   REQUESTDATA sReqData;                         /* Queue request data        */
   CHAR        szFlags[ 16 ];                    /* Custom options string     */
   CHAR        szModName[ LEN_FILENAME ];        /* Module name               */
   ULONG       ulCurElems;                       /* Current queue elements    */
   ULONG       ulDataLen;                        /* Queue data length         */
   ULONG       ulNewCtlFlg;                      /* New trace control flags   */
   ULONG       ulOldCtlFlg;                      /* Old trace control flags   */
   ULONG       ulPost;                           /* Control semaphore posts   */
   ULONG       ulTotElems = 0L;                  /* Totle queue elements      */

   /*
    * Obtain old trace control option from operating system profile, issue
    * error and terminate if unsuccessful
    */
   if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szCtlFlgKey, DEF_CTLFLG, szFlags, sizeof( szFlags ) ) != sizeof( DEF_CTLFLG ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldCtlFlg ) != 1 ) )
   {
      Error( "Unable to retrieve trace control option" );
   }

   /*
    * Start with current trace options
    */
   ulNewCtlFlg = ulOldCtlFlg;

   /*
    * Pause tracing if -PAUSE option specified
    */
   if ( FLAGSET( ulFlags, fPAUSEOPT ) )
   {
      /*
       * Pause tracing if tracing currently not paused
       */
      if ( FLAGCLR( ulOldCtlFlg, C_PAUSE ) )
      {
         printf( "Tracing paused\n" );
         SETFLAG( ulNewCtlFlg, C_PAUSE );

         /*
          * Create trace control queue
          */
         DosCreateQueue( &hqueueCtl, QUE_FIFO, SYS_QUEUECTL );

         /*
          * Notify all running trace-enabled applications to pause tracing if
          * able to open pause semaphore
          */
         if ( DosOpenEventSem( SYS_SEMPAUSE, &hevCtl ) == NO_ERROR )
         {
            /*
             * Notify all running trace-enabled applications to pause tracing
             */
            DosPostEventSem( hevCtl );
            DosResetEventSem( hevCtl, &ulPost );
            DosCloseEventSem( hevCtl );

            /*
             * Wait for all running trace-enabled applications to respond to
             * pause request
             */
            for ( i = 0; ( DosQueryQueue( hqueueCtl, &ulCurElems ) == NO_ERROR ) && ( i < 10 ); i++, DosSleep( 1 ) )
            {
               if ( ulTotElems != ulCurElems )
               {
                  ulTotElems = ulCurElems;
                  i = 0;
               }
            }
         }

         /*
          * Display paused trace-enable applications if applications exist
          */
         if ( ulTotElems )
         {
            while ( ulTotElems-- > 0 )
            {
               if ( ( DosReadQueue( hqueueCtl, &sReqData, &ulDataLen, &pData, 0, DCWW_WAIT, &bPriority, 0L ) == NO_ERROR ) &&
                    ( sReqData.ulData == TRCCTL_PAUSE                                                                    ) &&
                    ( DosQueryModuleName( ulDataLen, sizeof( szModName ), szModName ) == NO_ERROR                        ) )
               {
                  printf( "  %s (PID %04lX) paused\n", szModName, sReqData.pid );
               }
            }
         }

         /*
          * Display message if no trace-enabled applications exist
          */
         else
         {
            printf( "  No active trace-enabled applications\n" );
         }

         /*
          * Close trace control queue
          */
         DosCloseQueue( hqueueCtl );
      }

      /*
       * Display message if tracing already paused
       */
      else
      {
         printf( "Tracing already paused\n" );
      }
   }

   /*
    * Resume tracing if -RESUME option specified
    */
   else if ( FLAGSET( ulFlags, fRESUMEOPT ) )
   {
      /*
       * Resume tracing if tracing currently paused
       */
      if ( FLAGSET( ulOldCtlFlg, C_PAUSE ) )
      {
         printf( "Tracing resumed\n" );
         CLRFLAG( ulNewCtlFlg, C_PAUSE );

         /*
          * Create trace control queue
          */
         DosCreateQueue( &hqueueCtl, QUE_FIFO, SYS_QUEUECTL );

         /*
          * Notify all running trace-enabled applications to resume tracing if
          * able to open resume semaphore
          */
         if ( DosOpenEventSem( SYS_SEMRESUME, &hevCtl ) == NO_ERROR )
         {
            /*
             * Notify all running trace-enabled applications to resume tracing
             */
            DosPostEventSem( hevCtl );
            DosResetEventSem( hevCtl, &ulPost );
            DosCloseEventSem( hevCtl );

            /*
             * Wait for all running trace-enabled applications to respond to
             * resume request
             */
            for ( i = 0, ulTotElems = 0L; ( DosQueryQueue( hqueueCtl, &ulCurElems ) == NO_ERROR ) && ( i < 10 ); i++, DosSleep( 10 ) )
            {
               if ( ulTotElems != ulCurElems )
               {
                  ulTotElems = ulCurElems;
                  i = 0;
               }
            }
         }

         /*
          * Display resumed trace-enable applications if applications exist
          */
         if ( ulTotElems )
         {
            while ( ulTotElems-- > 0 )
            {
               if ( ( DosReadQueue( hqueueCtl, &sReqData, &ulDataLen, &pData, 0, DCWW_WAIT, &bPriority, 0L ) == NO_ERROR ) &&
                    ( sReqData.ulData == TRCCTL_RESUME                                                                   ) &&
                    ( DosQueryModuleName( ulDataLen, sizeof( szModName ), szModName ) == NO_ERROR                        ) )
               {
                  printf( "  %s (PID %04lX) resumed\n", szModName, sReqData.pid );
               }
            }
         }

         /*
          * Display message if no trace-enabled applications exist
          */
         else
         {
            printf( "  No active trace-enabled applications\n" );
         }

         /*
          * Close trace control queue
          */
         DosCloseQueue( hqueueCtl );
      }

      /*
       * Display message if tracing already resumed
       */
      else
      {
         printf( "Tracing already resumed\n" );
      }
   }

   /*
    * Save new trace control option in operating system profile, issue error
    * and terminate if unsuccessful
    */
   if ( ( ulNewCtlFlg != ulOldCtlFlg ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewCtlFlg ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szCtlFlgKey, szFlags ) ) ) )
   {
      Error( "Unable to store trace control option" );
   }

}  /* ControlAPITracing() */

/******************************************************************************/
/*                                                                            */
/* CustomizeAPITracing()                                                      */
/*                                                                            */
/******************************************************************************/

VOID CustomizeAPITracing( VOID )                 /* No parameters             */
{

   PCHAR pszNewAltDir;                           /* New user hook             */
#ifdef T_USERHOOK
   PCHAR pszNewUserHk;                           /* New alternative directory */
#endif
   CHAR  szFlags[ sizeof( DEF_BUFLEN ) ];        /* Custom options string     */
   CHAR  szOldAltDir[ LEN_FILENAME ];            /* Old alternative directory */
#ifdef T_USERHOOK
   CHAR  szOldUserHk[ LEN_FILENAME ];            /* Old user hook             */
#endif
   ULONG ulNewBufLen;                            /* New buffer length         */
   ULONG ulNewDosGrp;                            /* New DOSCALLS groups       */
   ULONG ulNewFilLen;                            /* New file wrapping length  */
   ULONG ulNewGpiGrp;                            /* New PMGPI groups          */
   ULONG ulNewTrcFlg = 0L;                       /* New trace flags           */
   ULONG ulNewTrcLvl;                            /* New trace level           */
   ULONG ulNewWinGrp;                            /* New PMWIN groups          */
   ULONG ulOldBufLen;                            /* Old buffer length         */
   ULONG ulOldDosGrp;                            /* Old DOSCALLS groups       */
   ULONG ulOldFilLen;                            /* Old file wrapping length  */
   ULONG ulOldGpiGrp;                            /* Old PMGPI groups          */
   ULONG ulOldTrcFlg;                            /* Old trace flags           */
   ULONG ulOldTrcLvl;                            /* Old trace level           */
   ULONG ulOldWinGrp;                            /* Old PMWIN groups          */

   /*
    * Obtain old trace customization options from operating system profile,
    * issue error and terminate if unsuccessful
    */
   if ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szAltDirKey, DEF_ALTDIR, szOldAltDir, sizeof( szOldAltDir ) ) == 0L                                                                      ) ||
        ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szBufLenKey, DEF_BUFLEN, szFlags, sizeof( szFlags ) ) != sizeof( DEF_BUFLEN ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldBufLen ) != 1 ) ) ||
        ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szDosGrpKey, DEF_DOSGRP, szFlags, sizeof( szFlags ) ) != sizeof( DEF_DOSGRP ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldDosGrp ) != 1 ) ) ||
        ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szFilLenKey, DEF_FILLEN, szFlags, sizeof( szFlags ) ) != sizeof( DEF_FILLEN ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldFilLen ) != 1 ) ) ||
        ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szGpiGrpKey, DEF_GPIGRP, szFlags, sizeof( szFlags ) ) != sizeof( DEF_GPIGRP ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldGpiGrp ) != 1 ) ) ||
        ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, DEF_TRCFLG, szFlags, sizeof( szFlags ) ) != sizeof( DEF_TRCFLG ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldTrcFlg ) != 1 ) ) ||
        ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szTrcLvlKey, DEF_TRCLVL, szFlags, sizeof( szFlags ) ) != sizeof( DEF_TRCLVL ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldTrcLvl ) != 1 ) ) ||
#ifdef T_USERHOOK
        ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szUserHkKey, DEF_USERHK, szOldUserHk, sizeof( szOldUserHk ) ) == 0L )                                                                  ) ||
#endif
        ( ( PrfQueryProfileString( HINI_USERPROFILE, szAppName, szWinGrpKey, DEF_WINGRP, szFlags, sizeof( szFlags ) ) != sizeof( DEF_WINGRP ) ) || ( sscanf( szFlags, "0x%08lX", &ulOldWinGrp ) != 1 ) ) )
   {
      Error( "Unable to retrieve trace customization options" );
   }

   /*
    * Display current/old trace customization options
    */
   printf( "%s trace customization options:\n ", FLAGSET( ulFlags, fQOPTION ) ? "Current" : "Old" );
   printf( szOldAltDir[ 0 ] ? " -A %s" : " -A NONE", szOldAltDir );
   printf( ulOldBufLen ? " -B %d" : " -B ALL", ulOldBufLen );
   printf( " -C %s -D ", FLAGSET( ulOldTrcFlg, F_CONTROL ) ? "ON" : "OFF" );
   DisplayDOSCALLSGroups( ulOldDosGrp );
   printf( " -E %s", FLAGSET( ulOldTrcFlg, F_EBCDIC ) ? "ON" : "OFF" );
   printf( ulOldFilLen ? " -F %d -G " : " -F ALL -G ", ulOldFilLen );
   DisplayPMGPIGroups( ulOldGpiGrp );
   printf( " -I %s -L %d -T %s", FLAGSET( ulOldTrcFlg, F_INTERCEPT ) ? "ON" : "OFF", ulOldTrcLvl, FLAGSET( ulOldTrcFlg, F_TIMESTAMP ) ? "ON" : "OFF" );
#ifdef T_USERHOOK
   printf( szOldUserHk[ 0 ] ? " -U %s" : " -U NONE", szOldUserHk );
#endif
   printf( " -W ");
   DisplayPMWINGroups( ulOldWinGrp );
   putchar( '\n' );

   /*
    * Terminate program if -Q option specified
    */
   if ( FLAGSET( ulFlags, fQOPTION ) )
   {
      /*
       * Display undocumented trace customization options
       */
      if ( FLAGSET( ulOldTrcFlg, F_DUMPSTACK ) )
      {
         printf( "\nNOTE: Undocumented trace customization option -DUMPSTACK is enabled\n" );
      }

      /*
       * Terminate program
       */
      exit( NO_ERROR );
   }

   /*
    * Start with trace customization options present on command line
    */
   pszNewAltDir = FLAGSET( ulFlags, fAOPTION ) ? szAltDir : szOldAltDir;
   ulNewBufLen = FLAGSET( ulFlags, fBOPTION ) ? ulBufLen : ulOldBufLen;
   ulNewDosGrp = ulDosGrp ? ulDosGrp : ulOldDosGrp;
   ulNewFilLen = FLAGSET( ulFlags, fFOPTION ) ? ulFilLen : ulOldFilLen;
   ulNewGpiGrp = ulGpiGrp ? ulGpiGrp : ulOldGpiGrp;
   ulNewTrcFlg |= ( ( FLAGSET( ulFlags, fCOPTION ) ? ulTrcFlg : ulOldTrcFlg ) & F_CONTROL );
   ulNewTrcFlg |= ( ( FLAGSET( ulFlags, fEOPTION ) ? ulTrcFlg : ulOldTrcFlg ) & F_EBCDIC );
   ulNewTrcFlg |= ( ( FLAGSET( ulFlags, fIOPTION ) ? ulTrcFlg : ulOldTrcFlg ) & F_INTERCEPT );
   ulNewTrcFlg |= ( ( FLAGSET( ulFlags, fTOPTION ) ? ulTrcFlg : ulOldTrcFlg ) & F_TIMESTAMP );
   ulNewTrcFlg |= ( ( FLAGSET( ulFlags, fUNDOCOPT ) ? ulTrcFlg : ulOldTrcFlg ) & F_DUMPSTACK );
   ulNewTrcLvl = ulTrcLvl ? ulTrcLvl : ulOldTrcLvl;
#ifdef T_USERHOOK
   pszNewUserHk = FLAGSET( ulFlags, fUOPTION ) ? szUserHk : szOldUserHk;
#endif
   ulNewWinGrp = ulWinGrp ? ulWinGrp : ulOldWinGrp;

   /*
    * Save new trace customization options in operating system profile, issue
    * error and terminate if unsuccessful
    */
   if ( ( ( Strcmpi( pszNewAltDir, szOldAltDir ) ) && ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szAltDirKey, pszNewAltDir ) )                                         ) ||
        ( ( ulNewBufLen != ulOldBufLen ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewBufLen ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szBufLenKey, szFlags ) ) ) ) ||
        ( ( ulNewDosGrp != ulOldDosGrp ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewDosGrp ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szDosGrpKey, szFlags ) ) ) ) ||
        ( ( ulNewFilLen != ulOldFilLen ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewFilLen ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szFilLenKey, szFlags ) ) ) ) ||
        ( ( ulNewGpiGrp != ulOldGpiGrp ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewGpiGrp ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szGpiGrpKey, szFlags ) ) ) ) ||
        ( ( ulNewTrcFlg != ulOldTrcFlg ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewTrcFlg ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szTrcFlgKey, szFlags ) ) ) ) ||
        ( ( ulNewTrcLvl != ulOldTrcLvl ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewTrcLvl ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szTrcLvlKey, szFlags ) ) ) ) ||
#ifdef T_USERHOOK
        ( ( strcmp( pszNewUserHk, szOldUserHk ) ) && ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szUserHkKey, pszNewUserHk ) )                                          ) ||
#endif
        ( ( ulNewWinGrp != ulOldWinGrp ) && ( ( !sprintf( szFlags, "0x%08lX", ulNewWinGrp ) ) || ( !PrfWriteProfileString( HINI_USERPROFILE, szAppName, szWinGrpKey, szFlags ) ) ) ) )
   {
      Error( "Unable to store trace customization options" );
   }

   /*
    * Display new trace customization options
    */
   printf( "New trace customization options:\n " );
   printf( pszNewAltDir[ 0 ] ? " -A %s" : " -A NONE", pszNewAltDir );
   printf( ulNewBufLen ? " -B %d" : " -B ALL", ulNewBufLen );
   printf( " -C %s -D ", FLAGSET( ulNewTrcFlg, F_CONTROL ) ? "ON" : "OFF" );
   DisplayDOSCALLSGroups( ulNewDosGrp );
   printf( " -E %s", FLAGSET( ulNewTrcFlg, F_EBCDIC ) ? "ON" : "OFF" );
   printf( ulNewFilLen ? " -F %d -G " : " -F ALL -G ", ulNewFilLen );
   DisplayPMGPIGroups( ulNewGpiGrp );
   printf( " -I %s -L %d -T %s", FLAGSET( ulNewTrcFlg, F_INTERCEPT ) ? "ON" : "OFF", ulNewTrcLvl, FLAGSET( ulNewTrcFlg, F_TIMESTAMP ) ? "ON" : "OFF" );
#ifdef T_USERHOOK
   printf( pszNewUserHk[ 0 ] ? " -U %s" : " -U NONE", pszNewUserHk );
#endif
   printf( " -W " );
   DisplayPMWINGroups( ulNewWinGrp );
   putchar( '\n' );

   /*
    * Display undocumented trace customization options
    */
   if ( ( FLAGSET( ulFlags, fUNDOCOPT ) ) || ( FLAGSET( ulOldTrcFlg, F_DUMPSTACK ) ) )
   {
      printf( "\nNOTE: Undocumented trace customization option -DUMPSTACK is %sabled\n", ulNewTrcFlg & F_DUMPSTACK ? "en" : "dis" );
   }

}  /* CustomizeAPITracing() */

/******************************************************************************/
/*                                                                            */
/* DisplayBanner()                                                            */
/*                                                                            */
/******************************************************************************/

VOID DisplayBanner( VOID )                       /* No parameters             */
{

   /*
    * Display program banner
    */
   printf( "Operating System/2 API Trace\n" );
#ifdef PRIVATE
   printf( "PRIVATE Version %1d.%02d.%02d (%s)\n", VER_MAJOR, VER_MINOR, VER_REV, VER_DATE );
#else
   printf( "Version %1d.%02d.%02d (%s)\n", VER_MAJOR, VER_MINOR, VER_REV, VER_DATE );
#endif
   printf( "Written by Dave Blaschke\n" );

}  /* DisplayBanner() */

/******************************************************************************/
/*                                                                            */
/* DisplayDOSCALLSGroups()                                                    */
/*                                                                            */
/******************************************************************************/

VOID DisplayDOSCALLSGroups( ULONG ulGroups )     /* DOSCALLS API groups       */
{

   BOOL  bComma = FALSE;                         /* Display comma indicator   */
   int   i;                                      /* Loop variable             */
   ULONG ulNumGrp = 0L;                          /* Number API group bits set */

   /*
    * Display "ALL" if all DOSCALLS API groups bits set
    */
   if ( ulGroups == D_ALL )
   {
      printf( "ALL" );
   }

   /*
    * Display each API group if not all DOSCALLS API groups bits set
    */
   else
   {
      /*
       * Determine number of DOSCALLS API group bits set
       */
      for ( i = 0; i < sizeof( szDosGroups ) / sizeof( PCHAR ); i++ )
      {
         /*
          * Increment number of DOSCALLS API group bits set if bit set
          */
         if ( FLAGSET( ulGroups, 1 << i ) )
         {
            ulNumGrp++;
         }
      }

      /*
       * Display each API group being traced if number of DOSCALLS API group
       * bit sets is less than half the total number of DOSCALLS API groups
       */
      if ( ulNumGrp < ( sizeof( szDosGroups ) / sizeof( PCHAR ) ) / 2 )
      {
         /*
          * Display each API group being traced
          */
         for ( i = 0; i < sizeof( szDosGroups ) / sizeof( PCHAR ); i++ )
         {
            /*
             * Display API group if corresponding DOSCALLS API group bit set
             */
            if ( FLAGSET( ulGroups, 1 << i ) )
            {
               /*
                * Display comma if so indicated
                */
               if ( bComma )
               {
                  putchar( ',' );
               }

               /*
                * Display API group
                */
               printf( "%s", szDosGroups[ i ] );

               /*
                * Indicate display comma
                */
               bComma = TRUE;
            }
         }
      }

      /*
       * Display "ALL" followed by each API group not being traced if number of
       * DOSCALLS API group bit sets is more than half the total number of
       * DOSCALLS API groups
       */
      else
      {
         /*
          * Display "ALL"
          */
         printf( "ALL" );

         /*
          * Display each API group not being traced
          */
         for ( i = 0; i < sizeof( szDosGroups ) / sizeof( PCHAR ); i++ )
         {
            /*
             * Display API group if corresponding DOSCALLS API group bit clear
             */
            if ( FLAGCLR( ulGroups, 1 << i ) )
            {
               /*
                * Display API group
                */
               printf( ",NO%s", szDosGroups[ i ] );
            }
         }
      }
   }

}  /* DisplayDOSCALLSGroups() */

/******************************************************************************/
/*                                                                            */
/* DisplayPMGPIGroups()                                                       */
/*                                                                            */
/******************************************************************************/

VOID DisplayPMGPIGroups( ULONG ulGroups )        /* PMGPI API groups          */
{

   BOOL  bComma = FALSE;                         /* Display comma indicator   */
   int   i;                                      /* Loop variable             */
   ULONG ulNumGrp = 0L;                          /* Number API group bits set */

   /*
    * Display "ALL" if all PMGPI API groups bits set
    */
   if ( ulGroups == W_ALL )
   {
      printf( "ALL" );
   }

   /*
    * Display each API group if not all PMGPI API groups bits set
    */
   else
   {
      /*
       * Determine number of PMGPI API group bits set
       */
      for ( i = 0; i < sizeof( szGpiGroups ) / sizeof( PCHAR ); i++ )
      {
         /*
          * Increment number of PMGPI API group bits set if bit set
          */
         if ( FLAGSET( ulGroups, 1 << i ) )
         {
            ulNumGrp++;
         }
      }

      /*
       * Display each API group being traced if number of PMGPI API group
       * bit sets is less than half the total number of PMGPI API groups
       */
      if ( ulNumGrp < ( sizeof( szGpiGroups ) / sizeof( PCHAR ) ) / 2 )
      {
         /*
          * Display each API group being traced
          */
         for ( i = 0; i < sizeof( szGpiGroups ) / sizeof( PCHAR ); i++ )
         {
            /*
             * Display API group if corresponding PMGPI API group bit set
             */
            if ( FLAGSET( ulGroups, 1 << i ) )
            {
               /*
                * Display comma if so indicated
                */
               if ( bComma )
               {
                  putchar( ',' );
               }

               /*
                * Display API group
                */
               printf( "%s", szGpiGroups[ i ] );

               /*
                * Indicate display comma
                */
               bComma = TRUE;
            }
         }
      }

      /*
       * Display "ALL" followed by each API group not being traced if number of
       * PMGPI API group bit sets is more than half the total number of PMGPI
       * API groups
       */
      else
      {
         /*
          * Display "ALL"
          */
         printf( "ALL" );

         /*
          * Display each API group not being traced
          */
         for ( i = 0; i < sizeof( szGpiGroups ) / sizeof( PCHAR ); i++ )
         {
            /*
             * Display API group if corresponding PMGPI API group bit clear
             */
            if ( FLAGCLR( ulGroups, 1 << i ) )
            {
               /*
                * Display API group
                */
               printf( ",NO%s", szGpiGroups[ i ] );
            }
         }
      }
   }

}  /* DisplayPMGPIGroups() */

/******************************************************************************/
/*                                                                            */
/* DisplayPMWINGroups()                                                       */
/*                                                                            */
/******************************************************************************/

VOID DisplayPMWINGroups( ULONG ulGroups )        /* PMWIN API groups          */
{

   BOOL  bComma = FALSE;                         /* Display comma indicator   */
   int   i;                                      /* Loop variable             */
   ULONG ulNumGrp = 0L;                          /* Number API group bits set */

   /*
    * Display "ALL" if all PMWIN API groups bits set
    */
   if ( ulGroups == W_ALL )
   {
      printf( "ALL" );
   }

   /*
    * Display each API group if not all PMWIN API groups bits set
    */
   else
   {
      /*
       * Determine number of PMWIN API group bits set
       */
      for ( i = 0; i < sizeof( szWinGroups ) / sizeof( PCHAR ); i++ )
      {
         /*
          * Increment number of PMWIN API group bits set if bit set
          */
         if ( FLAGSET( ulGroups, 1 << i ) )
         {
            ulNumGrp++;
         }
      }

      /*
       * Display each API group being traced if number of PMWIN API group
       * bit sets is less than half the total number of PMWIN API groups
       */
      if ( ulNumGrp < ( sizeof( szWinGroups ) / sizeof( PCHAR ) ) / 2 )
      {
         /*
          * Display each API group being traced
          */
         for ( i = 0; i < sizeof( szWinGroups ) / sizeof( PCHAR ); i++ )
         {
            /*
             * Display API group if corresponding PMWIN API group bit set
             */
            if ( FLAGSET( ulGroups, 1 << i ) )
            {
               /*
                * Display comma if so indicated
                */
               if ( bComma )
               {
                  putchar( ',' );
               }

               /*
                * Display API group
                */
               printf( "%s", szWinGroups[ i ] );

               /*
                * Indicate display comma
                */
               bComma = TRUE;
            }
         }
      }

      /*
       * Display "ALL" followed by each API group not being traced if number of
       * PMWIN API group bit sets is more than half the total number of PMWIN
       * API groups
       */
      else
      {
         /*
          * Display "ALL"
          */
         printf( "ALL" );

         /*
          * Display each API group not being traced
          */
         for ( i = 0; i < sizeof( szWinGroups ) / sizeof( PCHAR ); i++ )
         {
            /*
             * Display API group if corresponding PMWIN API group bit clear
             */
            if ( FLAGCLR( ulGroups, 1 << i ) )
            {
               /*
                * Display API group
                */
               printf( ",NO%s", szWinGroups[ i ] );
            }
         }
      }
   }

}  /* DisplayPMWINGroups() */

/******************************************************************************/
/*                                                                            */
/* EnableAPITracing()                                                         */
/*                                                                            */
/******************************************************************************/

VOID EnableAPITracing( VOID )                    /* No parameters             */
{

   BOOL    bWrite = FALSE;                       /* Write table indicator     */
   int     i;                                    /* Loop variable             */
   PCHAR   pch;                                  /* Character pointer         */
   PPIB    pPIB;                                 /* Process information block */
   PTIB    pTIB;                                 /* Thread information block  */
   CHAR    szModNam[ LEN_FILENAME ];             /* Module name               */
   ULONG   ulDLLIdx;                             /* DLL information index     */

   /*
    * Enable or disable API tracing in NE executable file
    */
   if ( FLAGSET( ulFlags, fNE_FMT ) )
   {
      /*
       * Replace DLL names in import name table with corresponding trace DLL
       * names if enabling API tracing
       */
      if ( FLAGSET( ulFlags, fTRACE_ON ) )
      {
         /*
          * Examine each DLL name in module index table
          */
         for ( i = 0; i < ulModIdxTbl; i++ )
         {
            /*
             * Process supported DLL with trace on/off enabled
             */
            if ( ( pModIdxTbl[ i ] != UNKNOWN_DLL ) && ( DLLInfo[ pModIdxTbl[ i ] ].bEnable ) )
            {
               /*
                * Replace DLL name in import name table with corresponding
                * trace DLL name if DLL
                */
               if ( FLAGSET( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fDLL ) )
               {
                  /*
                   * Obtain module name
                   */
                   pch = (PCHAR)( (ULONG)pImpNamTbl + pModRefTbl[ i ] );

                  /*
                   * Replace DLL name in import name table with corresponding
                   * trace DLL name
                   */
                  memcpy( pch + 1, DLLInfo[ pModIdxTbl[ i ] ].szTraceDLLName, *pch );

                  /*
                   * Issue message indicating enabling API tracing
                   */
                  printf( "  %-8s: File imports from DLL, API tracing enabled\n", DLLInfo[ pModIdxTbl[ i ] ].szName );

                  /*
                   * Indicate write import name table to executable file
                   */
                  bWrite = TRUE;
               }

               /*
                * Issue message indicating API tracing already enabled if trace
                * DLL
                */
               else
               {
                  /*
                   * Issue message indicating API tracing already enabled
                   */
                  printf( "  %-8s: File imports from trace DLL, API tracing already enabled\n", DLLInfo[ pModIdxTbl[ i ] ].szName );
               }

               /*
                * Indicate trace on/off disabled
                */
               DLLInfo[ pModIdxTbl[ i ] ].bEnable = FALSE;
            }
         }

         /*
          * Examine each supported DLL
          */
         for ( i = 0; i < SUPPORTED_DLL; i++ )
         {
            /*
             * Issue message indicating not enabling API tracing if trace
             * on/off enabled
             */
            if ( DLLInfo[ i ].bEnable )
            {
               printf( "  %-8s: File does not import from DLL, API tracing not enabled\n", DLLInfo[ i ].szName );
            }
         }
      }

      /*
       * Replace trace DLL names in import name table with corresponding DLL
       * names if disabling API tracing
       */
      else
      {
         /*
          * Examine each DLL name in module index table
          */
         for ( i = 0; i < ulModIdxTbl; i++ )
         {
            /*
             * Process supported DLL with trace on/off enabled
             */
            if ( ( pModIdxTbl[ i ] != UNKNOWN_DLL ) && ( DLLInfo[ pModIdxTbl[ i ] ].bEnable ) )
            {
               /*
                * Replace trace DLL name in import name table with
                * corresponding DLL name if trace DLL
                */
               if ( FLAGSET( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fTRACEDLL ) )
               {
                  /*
                   * Obtain module name
                   */
                   pch = (PCHAR)( (ULONG)pImpNamTbl + pModRefTbl[ i ] );

                  /*
                   * Replace trace DLL name in import name table with
                   * corresponding DLL name
                   */
                  memcpy( pch + 1, DLLInfo[ pModIdxTbl[ i ] ].szName, *pch );

                  /*
                   * Issue message indicating disabling API tracing
                   */
                  printf( "  %-8s: File imports from trace DLL, API tracing disabled\n", DLLInfo[ pModIdxTbl[ i ] ].szName );

                  /*
                   * Indicate write import name table to executable file
                   */
                  bWrite = TRUE;
               }

               /*
                * Issue message indicating API tracing already disabled if DLL
                */
               else
               {
                  /*
                   * Issue message indicating API tracing already disabled
                   */
                  printf( "  %-8s: File imports from DLL, API tracing already disabled\n", DLLInfo[ pModIdxTbl[ i ] ].szName );
               }

               /*
                * Indicate trace on/off disabled
                */
               DLLInfo[ pModIdxTbl[ i ] ].bEnable = FALSE;
            }
         }

         /*
          * Examine each supported DLL
          */
         for ( i = 0; i < SUPPORTED_DLL; i++ )
         {
            /*
             * Issue message indicating not disabling API tracing if trace
             * on/off enabled
             */
            if ( DLLInfo[ i ].bEnable )
            {
               printf( "  %-8s: File does not import from trace DLL, API tracing not disabled\n", DLLInfo[ i ].szName );
            }
         }
      }

      /*
       * Write import name table to executable file if indicated
       */
      if ( bWrite )
      {
         /*
          * Locate beginning of import name table and write import name table;
          * issue error and terminate if unsuccessful
          */
         if ( ( fseek( fpInFile, sMZHdr.e_lfanew + pNEHdr->ne_imptab, SEEK_SET ) ) ||
              ( fwrite( pImpNamTbl, ulImpNamTbl, 1, fpInFile ) != 1              ) )
         {
            Error( "Unable to write import name table: %s", szInFile );
         }
      }

      /*
       * Disable -P option if executable file does not need to be changed
       */
      else if ( FLAGSET( ulFlags, fPOPTION ) )
      {
         CLRFLAG( ulFlags, fPOPTION );
      }
   }

   /*
    * Enable or disable API tracing in LX executable file
    */
   else if ( FLAGSET( ulFlags, fLX_FMT ) )
   {
      /*
       * Replace DLL names in module name table with corresponding trace DLL
       * names if enabling API tracing
       */
      if ( FLAGSET( ulFlags, fTRACE_ON ) )
      {
         /*
          * Examine each DLL name in module index table
          */
         for ( i = 0, pch = pModNamTbl; i < ulModIdxTbl; i++, pch += *pch + 1 )
         {
            /*
             * Process supported DLL with trace on/off enabled
             */
            if ( ( pModIdxTbl[ i ] != UNKNOWN_DLL ) && ( DLLInfo[ pModIdxTbl[ i ] ].bEnable ) )
            {
               /*
                * Replace DLL name in module name table with corresponding
                * trace DLL name if DLL
                */
               if ( FLAGSET( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fDLL ) )
               {
                  /*
                   * Replace DLL name in module name table with corresponding
                   * trace DLL name
                   */
                  memcpy( pch + 1, DLLInfo[ pModIdxTbl[ i ] ].szTraceDLLName, *pch );

                  /*
                   * Issue message indicating enabling API tracing
                   */
                  printf( "  %-8s: File imports from DLL, API tracing enabled\n", DLLInfo[ pModIdxTbl[ i ] ].szName );

                  /*
                   * Indicate write module name table to executable file
                   */
                  bWrite = TRUE;
               }

               /*
                * Issue message indicating API tracing already enabled if trace
                * DLL
                */
               else
               {
                  /*
                   * Issue message indicating API tracing already enabled
                   */
                  printf( "  %-8s: File imports from trace DLL, API tracing already enabled\n", DLLInfo[ pModIdxTbl[ i ] ].szName );
               }

               /*
                * Indicate trace on/off disabled
                */
               DLLInfo[ pModIdxTbl[ i ] ].bEnable = FALSE;
            }
         }

         /*
          * Examine each supported DLL
          */
         for ( i = 0; i < SUPPORTED_DLL; i++ )
         {
            /*
             * Issue message indicating not enabling API tracing if trace
             * on/off enabled
             */
            if ( DLLInfo[ i ].bEnable )
            {
               printf( "  %-8s: File does not import from DLL, API tracing not enabled\n", DLLInfo[ i ].szName );
            }
         }
      }

      /*
       * Replace trace DLL names in module name table with corresponding DLL
       * names if disabling API tracing
       */
      else
      {
         /*
          * Examine each DLL name in module index table
          */
         for ( i = 0, pch = pModNamTbl; i < ulModIdxTbl; i++, pch += *pch + 1 )
         {
            /*
             * Process supported DLL with trace on/off enabled
             */
            if ( ( pModIdxTbl[ i ] != UNKNOWN_DLL ) && ( DLLInfo[ pModIdxTbl[ i ] ].bEnable ) )
            {
               /*
                * Replace trace DLL name in module name table with
                * corresponding DLL name if trace DLL
                */
               if ( FLAGSET( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fTRACEDLL ) )
               {
                  /*
                   * Replace trace DLL name in module name table with
                   * corresponding DLL name
                   */
                  memcpy( pch + 1, DLLInfo[ pModIdxTbl[ i ] ].szName, *pch );

                  /*
                   * Issue message indicating disabling API tracing
                   */
                  printf( "  %-8s: File imports from trace DLL, API tracing disabled\n", DLLInfo[ pModIdxTbl[ i ] ].szName );

                  /*
                   * Indicate write module name table to executable file
                   */
                  bWrite = TRUE;
               }

               /*
                * Issue message indicating API tracing already disabled if DLL
                */
               else
               {
                  /*
                   * Issue message indicating API tracing already disabled
                   */
                  printf( "  %-8s: File imports from DLL, API tracing already disabled\n", DLLInfo[ pModIdxTbl[ i ] ].szName );
               }

               /*
                * Indicate trace on/off disabled
                */
               DLLInfo[ pModIdxTbl[ i ] ].bEnable = FALSE;
            }
         }

         /*
          * Examine each supported DLL
          */
         for ( i = 0; i < SUPPORTED_DLL; i++ )
         {
            /*
             * Issue message indicating not disabling API tracing if trace on/off
             * enabled
             */
            if ( DLLInfo[ i ].bEnable )
            {
               printf( "  %-8s: File does not import from trace DLL, API tracing not disabled\n", DLLInfo[ i ].szName );
            }
         }
      }

      /*
       * Write module name table to executable file if indicated
       */
      if ( bWrite )
      {
         /*
          * Locate beginning of module name table and write module name table;
          * issue error and terminate if unsuccessful
          */
         if ( ( fseek( fpInFile, sMZHdr.e_lfanew + sLXHdr.e32_impmod, SEEK_SET ) ) ||
              ( fwrite( pModNamTbl, ulModNamTbl, 1, fpInFile ) != 1              ) )
         {
            Error( "Unable to write module name table: %s", szInFile );
         }
      }

      /*
       * Disable -P option if executable file does not need to be changed
       */
      else if ( FLAGSET( ulFlags, fPOPTION ) )
      {
         CLRFLAG( ulFlags, fPOPTION );
      }
   }

}  /* EnableAPITracing() */

/******************************************************************************/
/*                                                                            */
/* Error()                                                                    */
/*                                                                            */
/******************************************************************************/

VOID Error( PCHAR szFmt                          /* Error message format      */
          , ...                                  /* Error message inserts     */
          )
{

   va_list pArgs;                                /* Error message inserts     */
   CHAR    szFmtMsg[ LEN_IOLINE ];               /* Formatted error message   */

   /*
    * Format error message including message inserts
    */
   va_start( pArgs, szFmt );
   vsprintf( szFmtMsg, szFmt, pArgs );
   va_end( pArgs );

   /*
    * Display error message
    */
   printf( "%s: %s\n", szAppName, szFmtMsg );

   /*
    * Terminate program
    */
   exit( ERROR );

}  /* Error() */

/******************************************************************************/
/*                                                                            */
/* ExceptionHandler()                                                         */
/*                                                                            */
/******************************************************************************/

ULONG APIENTRY ExceptionHandler( PREPREC pRep    /* Exception information     */
                               , PREGREC pReg    /* Registration record       */
                               , PCTXREC pCtx    /* Exception context         */
                               , PVOID   p       /* Not used                  */
                               )
{

   /*
    * Clean up tables if being removed from exception handler chain
    */
   if ( FLAGSET( pRep->fHandlerFlags, EH_UNWINDING ) )
   {
      /*
       * Clean up trace enablement tables if enabling API tracing
       */
      if ( FLAGSET( ulFlags, fENABLE ) )
      {
         TerminateEnablement( FALSE );
      }

      /*
       * Clean up trace summarization tables if summarizing API tracing
       */
      else if ( FLAGSET( ulFlags, fSUMMARY ) )
      {
         TerminateSummarization( FALSE );
      }

      /*
       * Delete system semaphore
       */
      DosCloseMutexSem( hmtxSys );
   }

   /*
    * Indicate ignore process terminate exception
    */
   else if ( pRep->ExceptionNum == XCPT_PROCESS_TERMINATE )
   {
      return XCPT_CONTINUE_SEARCH;
   }

   /*
    * Issue error and terminate if signal received from another process
    */
   else if ( pRep->ExceptionNum == XCPT_SIGNAL )
   {
      Error( "User interrupt" );
   }

   /*
    * Issue error and terminate if fatal exception generated
    */
   else if ( ( pRep->ExceptionNum & XCPT_SEVERITY_CODE ) == XCPT_FATAL_EXCEPTION )
   {
#ifdef DEBUG
      Error( "Fatal exception at 0x%08lX: 0x%08lX", pRep->ExceptionAddress, pRep->ExceptionNum );
#else
      Error( "Fatal exception: 0x%08lX", pRep->ExceptionNum );
#endif
   }

   /*
    * Indicate ignore exception
    */
   return XCPT_CONTINUE_SEARCH;

}  /* ExceptionHandler() */

/******************************************************************************/
/*                                                                            */
/* GetAPIInfo()                                                               */
/*                                                                            */
/******************************************************************************/

PAPIINFO GetAPIInfo( PCHAR szName )              /* API name                  */
{

   PAPIINFO pAPI;                                /* API information           */

   /*
    * Start at beginning of API information chain
    */
   pAPI = pAPIFirst;

   /*
    * Search through API information for matching API name
    */
   while ( pAPI )
   {
      /*
       * Return API information address if matching API name found
       */
      if ( !Strcmpi( pAPI->szName, szName ) )
      {
         return pAPI;
      }

      /*
       * Skip API information
       */
      pAPI = pAPI->pNext;
   }

   /*
    * Indicate match not found
    */
   return NULL;

}  /* GetAPIInfo() */

/******************************************************************************/
/*                                                                            */
/* InitializeEnablement()                                                     */
/*                                                                            */
/******************************************************************************/

VOID InitializeEnablement( VOID )                /* No parameters             */
{

   FILESTATUS3  filestatus3;                     /* File information          */
   HFILE        hfile;                           /* File handle               */
   int          i;                               /* Loop variable             */
   PCHAR        pch;                             /* Character pointer         */
   CHAR         szModNam[ LEN_FILENAME ];        /* Module name               */
   ULONG        ulAction;                        /* Action taken by DosOpen   */
   ULONG        ulAttr;                          /* File open attributes      */
   ULONG        ulBaseAddr;                      /* Object base address       */
   ULONG        ulFlag;                          /* File open flags           */
   ULONG        ulMode;                          /* File open mode            */

   /*
    * Save executable file date/time stamp if -P option specified
    */
   if ( FLAGSET( ulFlags, fPOPTION ) )
   {
      /*
       * Initialize open file flags
       */
      ulAttr = FILE_NORMAL;
      ulFlag = OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW;
      ulMode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE;

      /*
       * Open executable file, obtain file information if successful
       */
      if ( !DosOpen( szInFile, &hfile, &ulAction, 0L, ulAttr, ulFlag, ulMode, NULL ) )
      {
         /*
          * Obtain executable file information, save file date/time stamp if
          * successful
          */
         if ( !DosQueryFileInfo( hfile, FIL_STANDARD, &filestatus3, sizeof( FILESTATUS3 ) ) )
         {
            fdateInFile = filestatus3.fdateLastWrite;
            ftimeInFile = filestatus3.ftimeLastWrite;
         }

         /*
          * Disable -P option if unable to obtain executable file information
          */
         else
         {
            CLRFLAG( ulFlags, fPOPTION );
         }

         /*
          * Close executable file
          */
         DosClose( hfile );
      }

      /*
       * Disable -P option if unable to open executable file
       */
      else
      {
         CLRFLAG( ulFlags, fPOPTION );
      }
   }

   /*
    * Open executable file, issue error and terminate if unsuccessful
    */
   if ( !( fpInFile = fopen( szInFile, "rb+" ) ) )
   {
      Error( "Unable to open file: %s", szInFile );
   }

   /*
    * Read DOS header and identify DOS or OS/2 signature, process OS/2 header
    * if successful
    */
   if ( ( fread( &sMZHdr, sizeof( sMZHdr ), 1, fpInFile ) == 1 ) &&
        ( ( sMZHdr.e_magic == EMAGIC   ) ||
          ( sMZHdr.e_magic == NEMAGIC  ) ||
          ( sMZHdr.e_magic == E32MAGIC )                       ) )
   {
      /*
       * Zero DOS header if DOS header not present
       */
      if ( sMZHdr.e_magic != EMAGIC )
      {
         memset( &sMZHdr, 0, sizeof( MZHDR ) );
      }

      /*
       * Locate beginning of OS/2 header and read OS/2 header; issue error and
       * terminate if unsuccessful
       */
      if ( ( fseek( fpInFile, 0L, SEEK_END )                        ) ||
           ( ftell( fpInFile ) < sMZHdr.e_lfanew + sizeof( sLXHdr ) ) ||
           ( fseek( fpInFile, sMZHdr.e_lfanew, SEEK_SET )           ) ||
           ( fread( &sLXHdr, sizeof( sLXHdr ), 1, fpInFile ) != 1   ) )
      {
         Error( "File DOS executable: %s", szInFile );
      }

      /*
       * Executable file follows segmented format if magic number NE
       */
      if ( *( (PUSHORT)sLXHdr.e32_magic ) == NEMAGIC )
      {
         /*
          * Indicate NE executable format
          */
         SETFLAG( ulFlags, fNE_FMT );

         /*
          * Obtain 16-bit executable header and sector size
          */
         pNEHdr = (NEHDR *)&sLXHdr;
         usSector = 1 << pNEHdr->ne_align;

         /*
          * Issue error and terminate if module reference table empty
          * (executable file does not import anything)
          */
         if ( !( ulModIdxTbl = pNEHdr->ne_cmod ) )
         {
            Error( "File does not import APIs: %s", szInFile );
         }

         /*
          * Allocate memory for and read module reference table
          */
         if ( ( ulModRefTbl = ulModIdxTbl * sizeof( USHORT ) ) > 0L )
         {
            /*
             * Allocate memory for module reference table, issue error and
             * terminate if unsuccessful
             */
            if ( !( pModRefTbl = malloc( ulModRefTbl ) ) )
            {
               Error( "Unable to allocate memory for module reference table: %s", szInFile );
            }

            /*
             * Locate beginning of module reference table and read module
             * reference table; issue error and terminate if unsuccessful
             */
            if ( ( fseek( fpInFile, sMZHdr.e_lfanew + pNEHdr->ne_modtab, SEEK_SET ) ) ||
                 ( fread( pModRefTbl, ulModRefTbl, 1, fpInFile ) != 1               ) )
            {
               Error( "Unable to read module reference table: %s", szInFile );
            }
         }

         /*
          * Allocate memory for and read import name table if one exists
          */
         if ( ( ulImpNamTbl = pNEHdr->ne_nrestab - ( sMZHdr.e_lfanew + pNEHdr->ne_imptab ) ) > 0L )
         {
            /*
             * Allocate memory for import name table, issue error and terminate
             * if unsuccessful
             */
            if ( !( pImpNamTbl = malloc( ulImpNamTbl ) ) )
            {
               Error( "Unable to allocate memory for import name table: %s", szInFile );
            }

            /*
             * Locate beginning of import name table and read import name
             * table; issue error and terminate if unsuccessful
             */
            if ( ( fseek( fpInFile, sMZHdr.e_lfanew + pNEHdr->ne_imptab, SEEK_SET ) ) ||
                 ( fread( pImpNamTbl, ulImpNamTbl, 1, fpInFile ) != 1               ) )
            {
               Error( "Unable to read import name table: %s", szInFile );
            }
         }

         /*
          * Obtain number of segments
          */
         ulSegment = pNEHdr->ne_cseg;

         /*
          * Allocate memory for and read segment table if one exists
          */
         if ( ( ulSegmentTbl = ulSegment * sizeof( NESEG ) ) > 0L )
         {
            /*
             * Allocate memory for segment table, issue error and terminate if
             * unsuccessful
             */
            if ( !( pSegmentTbl = malloc( ulSegmentTbl ) ) )
            {
               Error( "Unable to allocate memory for segment table: %s", szInFile );
            }

            /*
             * Locate beginning of segment table and read segment table; issue
             * error and terminate if unsuccessful
             */
            if ( ( fseek( fpInFile, sMZHdr.e_lfanew + pNEHdr->ne_segtab, SEEK_SET ) ) ||
                 ( fread( pSegmentTbl, ulSegmentTbl, 1, fpInFile ) != 1             ) )
            {
               Error( "Unable to read segment table: %s", szInFile );
            }
         }

         /*
          * Allocate memory for module index table, issue error and terminate
          * if unsuccessful
          */
         if ( !( pModIdxTbl = malloc( ulModIdxTbl * sizeof( ULONG ) ) ) )
         {
            Error( "Unable to allocate memory for module index table: %s", szInFile );
         }

         /*
          * Initialize module index table
          */
         for ( i = 0; i < ulModIdxTbl; i++ )
         {
            /*
             * Obtain module name
             */
            pch = (PCHAR)( (ULONG)pImpNamTbl + pModRefTbl[ i ] );
            memcpy( szModNam, pch + 1, *pch );
            szModNam[ *pch ] = '\0';

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate DLL if supported DLL
             */
            if ( ( pModIdxTbl[ i ] = LookupDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fDLL );
            }

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate trace DLL if supported trace DLL
             */
            else if ( ( pModIdxTbl[ i ] = LookupTraceDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fTRACEDLL );
            }
         }

         /*
          * Issue message indicating enabling/disabling API tracing in NE
          * executable file
          */
         printf( "%s (NE Executable File) -\n", szInFile );
      }

      /*
       * Executable file follows linear format if magic number LX
       */
      else if ( *( (PUSHORT)sLXHdr.e32_magic ) == E32MAGIC )
      {
         /*
          * Indicate LX executable format
          */
         SETFLAG( ulFlags, fLX_FMT );

         /*
          * Issue error and terminate if module name table empty (executable
          * file does not import anything)
          */
         if ( !( ulModIdxTbl = sLXHdr.e32_impmodcnt ) )
         {
            Error( "File does not import APIs: %s", szInFile );
         }

         /*
          * Allocate memory for and read module name table
          */
         if ( ( ulModNamTbl = sLXHdr.e32_impproc - sLXHdr.e32_impmod ) > 0L )
         {
            /*
             * Allocate memory for module name table, issue error and terminate
             * if unsuccessful
             */
            if ( !( pModNamTbl = malloc( ulModNamTbl ) ) )
            {
               Error( "Unable to allocate memory for module name table: %s", szInFile );
            }

            /*
             * Locate beginning of module name table and read module name
             * table; issue error and terminate if unsuccessful
             */
            if ( ( fseek( fpInFile, sMZHdr.e_lfanew + sLXHdr.e32_impmod, SEEK_SET ) ) ||
                 ( fread( pModNamTbl, ulModNamTbl, 1, fpInFile ) != 1               ) )
            {
               Error( "Unable to read module name table: %s", szInFile );
            }
         }

         /*
          * Read procedure name table and fixup table if enabling API tracing
          */
         if ( FLAGSET( ulFlags, fTRACE_ON ) )
         {
            /*
             * Allocate memory for and read procedure name table if one exists
             */
            if ( ( ulPrcNamTbl = sLXHdr.e32_fpagetab + sLXHdr.e32_fixupsize - sLXHdr.e32_impproc ) > 0L )
            {
               /*
                * Allocate memory for procedure name table, issue error and
                * terminate if unsuccessful
                */
               if ( !( pPrcNamTbl = malloc( ulPrcNamTbl ) ) )
               {
                  Error( "Unable to allocate memory for procedure name table: %s", szInFile );
               }

               /*
                * Locate beginning of procedure name table and read procedure
                * name table; issue error and terminate if unsuccessful
                */
               if ( ( fseek( fpInFile, sMZHdr.e_lfanew + sLXHdr.e32_impproc, SEEK_SET ) ) ||
                    ( fread( pPrcNamTbl, ulPrcNamTbl, 1, fpInFile ) != 1                ) )
               {
                  Error( "Unable to read procedure name table: %s", szInFile );
               }
            }

            /*
             * Allocate memory for and read fixup table if one exists
             */
            if ( ( ulFixupTbl = sLXHdr.e32_impmod - sLXHdr.e32_frectab ) > 0L )
            {
               /*
                * Allocate memory for fixup record table, issue error and
                * terminate if unsuccessful
                */
               if ( !( pFixupTbl = malloc( ulFixupTbl ) ) )
               {
                  Error( "Unable to allocate memory for fixup record table: %s", szInFile );
               }

               /*
                * Locate beginning of fixup record table and read fixup record
                * table; issue error and terminate if unsuccessful
                */
               if ( ( fseek( fpInFile, sMZHdr.e_lfanew + sLXHdr.e32_frectab, SEEK_SET ) ) ||
                    ( fread( pFixupTbl, ulFixupTbl, 1, fpInFile ) != 1                  ) )
               {
                  Error( "Unable to read fixup record table: %s", szInFile );
               }
            }
         }

         /*
          * Allocate memory for module index table, issue error and
          * terminate if unsuccessful
          */
         if ( !( pModIdxTbl = malloc( ulModIdxTbl * sizeof( ULONG ) ) ) )
         {
            Error( "Unable to allocate memory for module index table: %s", szInFile );
         }

         /*
          * Initialize module index table
          */
         for ( i = 0, pch = pModNamTbl; i < ulModIdxTbl; i++, pch += *pch + 1 )
         {
            /*
             * Obtain module name
             */
            memcpy( szModNam, pch + 1, *pch );
            szModNam[ *pch ] = '\0';

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate DLL if supported DLL
             */
            if ( ( pModIdxTbl[ i ] = LookupDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fDLL );
            }

            /*
             * Initialize module index table entry as index into DLL
             * information table and indicate trace DLL if supported trace DLL
             */
            else if ( ( pModIdxTbl[ i ] = LookupTraceDLL( szModNam ) ) != UNKNOWN_DLL )
            {
               SETFLAG( DLLInfo[ pModIdxTbl[ i ] ].ulFlags, fTRACEDLL );
            }
         }

         /*
          * Issue message indicating enabling/disabling API tracing in LX
          * executable file
          */
         printf( "%s (LX Executable File) -\n", szInFile );
      }

      /*
       * Issue error and terminate if magic number unrecognized
       */
      else
      {
         Error( "File not OS/2 executable: %s", szInFile );
      }
   }

   /*
    * Issue error and terminate if DOS signature absent
    */
   else
   {
      Error( "File not executable: %s", szInFile );
   }

}  /* InitializeEnablement() */

/******************************************************************************/
/*                                                                            */
/* IsUnsupportedNRRNAM()                                                      */
/*                                                                            */
/******************************************************************************/

BOOL IsUnsupportedNRRNAM( PCHAR szDLL            /* DLL name                  */
                        , PCHAR szAPI            /* API name                  */
                        , ULONG ulAPILen         /* API name length           */
                        )
{

   int        i, j;                              /* Loop variables            */
   PUNSNRRNAM pNRRNAM;                           /* Unsupported NRRNAM        */

   /*
    * Search through unsupported NRRNAMs table for matching DLL name
    */
   for ( i = 0, pNRRNAM = UnsNRRNAM; i < sizeof( UnsNRRNAM ) / sizeof( UNSNRRNAM ); i++, pNRRNAM++ )
   {
      /*
       * Search complete if DLL name found in unsupported NRRNAMs table
       */
      if ( !strcmp( szDLL, pNRRNAM->szDLL ) )
      {
         /*
          * Search through unsupported NRRNAMs table for matching API name
          */
         for ( j = 0; j < pNRRNAM->ulNumAPI; j++ )
         {
            /*
             * Search complete if API name found in unsupported NRRNAMs table
             */
            if ( !strncmp( szAPI, pNRRNAM->szAPI[ j ], ulAPILen ) )
            {
               return TRUE;
            }
         }
      }
   }

   /*
    * Indicate API name not found in unsupported NRRNAMs table
    */
   return FALSE;

}  /* IsUnsupportedNRRNAM() */

/******************************************************************************/
/*                                                                            */
/* LookupDLL()                                                                */
/*                                                                            */
/******************************************************************************/

ULONG LookupDLL( PCHAR szDLL )                   /* DLL name                  */
{

   int i;                                        /* Loop variable             */

   /*
    * Search through DLL information table for matching DLL name
    */
   for ( i = 0; i < SUPPORTED_DLL; i++ )
   {
      /*
       * Search complete if DLL name found in DLL information table
       */
      if ( !Strcmpi( szDLL, DLLInfo[ i ].szName ) )
      {
         /*
          * Return index of DLL name in DLL information table
          */
         return i;
      }
   }

   /*
    * Indicate DLL name not found in DLL information table
    */
   return UNKNOWN_DLL;

}  /* LookupDLL() */

/******************************************************************************/
/*                                                                            */
/* LookupTraceDLL()                                                           */
/*                                                                            */
/******************************************************************************/

ULONG LookupTraceDLL( PCHAR szDLL )              /* Trace DLL name            */
{

   int i;                                        /* Loop variable             */

   /*
    * Search through DLL information table for matching trace DLL name
    */
   for ( i = 0; i < SUPPORTED_DLL; i++ )
   {
      /*
       * Search complete if trace DLL name found in DLL information table
       */
      if ( !Strcmpi( szDLL, DLLInfo[ i ].szTraceDLLName ) )
      {
         /*
          * Return index of trace DLL name in DLL information table
          */
         return i;
      }
   }

   /*
    * Indicate trace DLL name not found in DLL information table
    */
   return UNKNOWN_DLL;

}  /* LookupTraceDLL() */

/******************************************************************************/
/*                                                                            */
/* ParseCommandLine()                                                         */
/*                                                                            */
/******************************************************************************/

VOID ParseCommandLine( int    argc               /* Argument count            */
                     , char **argv               /* Argument pointer          */
                     )
{

   BOOL  bAllGrp;                                /* All API groups indicator  */
   BOOL  bSetBit;                                /* Set bit(s) indicator      */
   int   i;                                      /* Loop and temp variable    */
   PCHAR pch;                                    /* Character pointer         */
   PCHAR szGroup;                                /* API groups string         */

   /*
    * Issue error and terminate if no command line arguments specified
    */
   if ( !argc )
   {
      Error( "Missing command line arguments, use \'-?\' to display help" );
   }

   /*
    * Parse each command line argument
    */
   while ( argc-- )
   {
      /*
       * Valid command line option starts with dash or slash
       */
      if ( ( argv[ 0 ][ 0 ] == '-' ) || ( argv[ 0 ][ 0 ] == '/' ) )
      {
         /*
          * Display help if -?, -H, or -HELP specified
          */
         if ( ( !Strcmpi( &argv[ 0 ][ 1 ], "?" ) ) || ( !Strcmpi( &argv[ 0 ][ 1 ], "H" ) ) || ( !Strcmpi( &argv[ 0 ][ 1 ], "HELP" ) ) )
         {
            /*
             * Display help
             */
            Usage( );
         }

         /*
          * Enable/disable API tracing for all supported DLLs if -ALL specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "ALL" ) )
         {
            /*
             * Indicate trace on/off enabled for all supported DLLs
             */
            for ( i = 0; i < SUPPORTED_DLL; i++ )
            {
               DLLInfo[ i ].bEnable = TRUE;
            }

            /*
             * Indicate trace enablement option specified
             */
            SETFLAG( ulFlags, fENABLE );
         }

         /*
          * Customize API tracing for alternative directory if -A specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "A" ) )
         {
            /*
             * Obtain alternative directory if argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate no alternative directory if "NONE" specified
                */
               if ( !Strcmpi( argv[ 0 ], "NONE" ) )
               {
                  szAltDir[ 0 ] = '\0';
               }

               /*
                * Issue error and terminate if string argument invalid
                */
               else if ( ( strlen( argv[ 0 ] ) < 3 ) ||
                         ( argv[ 0 ][ 1 ] != ':'   ) ||
                         ( argv[ 0 ][ 2 ] != '\\'  ) )
               {
                  Error( "Invalid -A command line option argument: %s", argv[ 0 ] );
               }

               /*
                * Save alternative directory if string argument valid
                */
               else
               {
                  strcpy( szAltDir, Strupr( argv[ 0 ] ) );
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -A command line option argument" );
            }

            /*
             * Indicate -A option specified
             */
            SETFLAG( ulFlags, fAOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for maximum buffer length if -B specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "B" ) )
         {
            /*
             * Obtain maximum buffer length if argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate no maximum buffer length if "ALL" specified
                */
               if ( !Strcmpi( argv[ 0 ], "ALL" ) )
               {
                  ulBufLen = 0L;
               }

               /*
                * Save maximum buffer length rounded up to nearest multiple of
                * sixteen if decimal argument valid
                */
               else if ( ( ( i = Atoi( argv[ 0 ] ) ) >= MIN_BUFLEN ) && ( i <= MAX_BUFLEN ) )
               {
                  ulBufLen = i % MIN_BUFLEN ? i + ( MIN_BUFLEN - ( i % MIN_BUFLEN ) ) : i;
               }

               /*
                * Issue error and terminate if argument invalid
                */
               else
               {
                  Error( "Invalid -B command line option argument: %s", argv[ 0 ] );
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -B command line option argument" );
            }

            /*
             * Indicate -B option specified
             */
            SETFLAG( ulFlags, fBOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for tracing control enablement if -C
          * specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "C" ) )
         {
            /*
             * Obtain tracing control enablement if string argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate disable tracing control if "OFF" specified
                */
               if ( !Strcmpi( argv[ 0 ], "OFF" ) )
               {
                  CLRFLAG( ulTrcFlg, F_CONTROL );
               }

               /*
                * Indicate enable tracing control if "ON" specified
                */
               else if ( !Strcmpi( argv[ 0 ], "ON" ) )
               {
                  SETFLAG( ulTrcFlg, F_CONTROL );
               }

               /*
                * Issue error and terminate if argument invalid
                */
               else
               {
                  Error( "Invalid -C command line option argument: %s", argv[ 0 ] );
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -C command line option argument" );
            }

            /*
             * Indicate -C option specified
             */
            SETFLAG( ulFlags, fCOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for DOSCALLS API groups if -D specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "D" ) )
         {
            /*
             * Obtain DOSCALLS API groups if string argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Obtain first DOSCALLS API group
                */
               szGroup = strtok( argv[ 0 ], szGrpDelimiter );

               /*
                * Set all DOSCALLS API groups bits if "ALL" specified
                */
               if ( !Strcmpi( szGroup, "ALL" ) )
               {
                  /*
                   * Set all DOSCALLS API groups bits
                   */
                  ulDosGrp = D_ALL;

                  /*
                   * Obtain next DOSCALLS API group, parse next command line
                   * argument if next group absent
                   */
                  if ( !( szGroup = strtok( NULL, szGrpDelimiter ) ) )
                  {
                     /*
                      * Indicate trace customization option specified
                      */
                     SETFLAG( ulFlags, fCUSTOM );

                     /*
                      * Skip command line argument
                      */
                     argv++;

                     continue;
                  }

                  /*
                   * Indicate all API groups specified
                   */
                  bAllGrp = TRUE;
               }

               /*
                * Indicate not all API groups specified if "ALL" not specified
                */
               else
               {
                  bAllGrp = FALSE;
               }

               /*
                * Process every specified DOSCALLS API group
                */
               do
               {
                  /*
                   * Indicate clear bit(s) if "NO" prefix specified
                   */
                  if ( ( toupper( szGroup[ 0 ] ) == 'N' ) && ( toupper( szGroup[ 1 ] ) == 'O' ) )
                  {
                     /*
                      * Indicate clear bit(s)
                      */
                     bSetBit = FALSE;

                     /*
                      * Skip "NO" prefix
                      */
                     szGroup += 2;
                  }

                  /*
                   * Indicate set bit(s) if "NO" prefix not specified
                   */
                  else
                  {
                     bSetBit = TRUE;
                  }

                  /*
                   * Search for matching DOSCALLS API group
                   */
                  for ( i = 0; i < sizeof( szDosGroups ) / sizeof( PCHAR ); i++ )
                  {
                     /*
                      * Search complete if matching DOSCALLS API group found
                      */
                     if ( !Strcmpi( szGroup, szDosGroups[ i ] ) )
                     {
                        /*
                         * Set specified DOSCALLS API groups bit if set bit(s)
                         * indicated
                         */
                        if ( bSetBit )
                        {
                           /*
                            * Set specified DOSCALLS API groups bit if all API
                            * groups not indicated
                            */
                           if ( !bAllGrp )
                           {
                              SETFLAG( ulDosGrp, 1 << i );
                           }

                           /*
                            * Issue error and terminate if all API groups
                            * indicated
                            */
                           else
                           {
                              Error( "Invalid API group in -D command line option argument when ALL specified first: %s", szGroup );
                           }
                        }

                        /*
                         * Clear specified DOSCALLS API groups bit if set
                         * bit(s) not indicated
                         */
                        else
                        {
                           /*
                            * Clear specified DOSCALLS API groups bit if all API
                            * groups indicated
                            */
                           if ( bAllGrp )
                           {
                              CLRFLAG( ulDosGrp, 1 << i );
                           }

                           /*
                            * Issue error and terminate if all API groups not
                            * indicated
                            */
                           else
                           {
                              Error( "Invalid API group in -D command line option argument when ALL not specified first: %s", szGroup -= 2 );
                           }
                        }

                        break;
                     }
                  }

                  /*
                   * Issue error and terminate if string argument invalid
                   */
                  if ( i >= sizeof( szDosGroups ) / sizeof( PCHAR ) )
                  {
                     Error( "Invalid API group in -D command line option argument: %s", bSetBit ? szGroup : ( szGroup -= 2 ) );
                  }
               } while ( ( szGroup = strtok( NULL, szGrpDelimiter ) ) != NULL );
            }

            /*
             * Issue error and terminate if string argument absent
             */
            else
            {
               Error( "Missing -D command line option argument" );
            }

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for stack dump enablement if -DUMPSTACK
          * specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "DUMPSTACK" ) )
         {
            /*
             * Obtain stack dump enablement if string argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate disable stack dump if "OFF" specified
                */
               if ( !Strcmpi( argv[ 0 ], "OFF" ) )
               {
                  CLRFLAG( ulTrcFlg, F_DUMPSTACK );
               }

               /*
                * Indicate enable stack dump if "ON" specified
                */
               else if ( !Strcmpi( argv[ 0 ], "ON" ) )
               {
                  SETFLAG( ulTrcFlg, F_DUMPSTACK );
               }

               /*
                * Issue error and terminate if argument invalid
                */
               else
               {
                  Error( "Invalid -DUMPSTACK command line option argument: %s", argv[ 0 ] );
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -DUMPSTACK command line option argument" );
            }

            /*
             * Indicate -DUMPSTACK option specified
             */
            SETFLAG( ulFlags, fUNDOCOPT );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for EBCDIC translation enablement if -E
          * specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "E" ) )
         {
            /*
             * Obtain EBCDIC translation enablement if string argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate disable EBCDIC translation if "OFF" specified
                */
               if ( !Strcmpi( argv[ 0 ], "OFF" ) )
               {
                  CLRFLAG( ulTrcFlg, F_EBCDIC );
               }

               /*
                * Indicate enable EBCDIC translation if "ON" specified
                */
               else if ( !Strcmpi( argv[ 0 ], "ON" ) )
               {
                  SETFLAG( ulTrcFlg, F_EBCDIC );
               }

               /*
                * Issue error and terminate if argument invalid
                */
               else
               {
                  Error( "Invalid -E command line option argument: %s", argv[ 0 ] );
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -E command line option argument" );
            }

            /*
             * Indicate -E option specified
             */
            SETFLAG( ulFlags, fEOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for maximum file wrapping length if -F
          * specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "F" ) )
         {
            /*
             * Obtain maximum file wrapping length if argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate no maximum file wrapping length if "ALL" specified
                */
               if ( !Strcmpi( argv[ 0 ], "ALL" ) )
               {
                  ulFilLen = 0L;
               }

               /*
                * Save maximum file wrapping length rounded up to nearest
                * multiple of 4KB if decimal argument valid
                */
               else if ( ( ( i = Atoi( argv[ 0 ] ) ) >= MIN_FILLEN ) && ( i <= MAX_FILLEN ) )
               {
                  ulFilLen = i % MIN_FILLEN ? i + ( MIN_FILLEN - ( i % MIN_FILLEN ) ) : i;
               }

               /*
                * Issue error and terminate if argument invalid
                */
               else
               {
                  Error( "Invalid -F command line option argument: %s", argv[ 0 ] );
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -F command line option argument" );
            }

            /*
             * Indicate -F option specified
             */
            SETFLAG( ulFlags, fFOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for PMGPI API groups if -G specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "G" ) )
         {
            /*
             * Obtain PMGPI API groups if string argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Obtain first PMGPI API group
                */
               szGroup = strtok( argv[ 0 ], szGrpDelimiter );

               /*
                * Set all PMGPI API groups bits if "ALL" specified
                */
               if ( !Strcmpi( szGroup, "ALL" ) )
               {
                  /*
                   * Set all PMGPI API groups bits
                   */
                  ulGpiGrp = G_ALL;

                  /*
                   * Obtain next PMGPI API group, parse next command line
                   * argument if next group absent
                   */
                  if ( !( szGroup = strtok( NULL, szGrpDelimiter ) ) )
                  {
                     /*
                      * Indicate trace customization option specified
                      */
                     SETFLAG( ulFlags, fCUSTOM );

                     /*
                      * Skip command line argument
                      */
                     argv++;

                     continue;
                  }

                  /*
                   * Indicate all API groups specified
                   */
                  bAllGrp = TRUE;
               }

               /*
                * Indicate not all API groups specified if "ALL" not specified
                */
               else
               {
                  bAllGrp = FALSE;
               }

               /*
                * Process every specified PMGPI API group
                */
               do
               {
                  /*
                   * Indicate clear bit(s) if "NO" prefix specified
                   */
                  if ( ( toupper( szGroup[ 0 ] ) == 'N' ) && ( toupper( szGroup[ 1 ] ) == 'O' ) )
                  {
                     /*
                      * Indicate clear bit(s)
                      */
                     bSetBit = FALSE;

                     /*
                      * Skip "NO" prefix
                      */
                     szGroup += 2;
                  }

                  /*
                   * Indicate set bit(s) if "NO" prefix not specified
                   */
                  else
                  {
                     bSetBit = TRUE;
                  }

                  /*
                   * Search for matching PMGPI API group
                   */
                  for ( i = 0; i < sizeof( szGpiGroups ) / sizeof( PCHAR ); i++ )
                  {
                     /*
                      * Search complete if matching PMGPI API group found
                      */
                     if ( !Strcmpi( szGroup, szGpiGroups[ i ] ) )
                     {
                        /*
                         * Set specified PMGPI API groups bit if set bit(s)
                         * indicated
                         */
                        if ( bSetBit )
                        {
                           /*
                            * Set specified PMGPI API groups bit if all API
                            * groups not indicated
                            */
                           if ( !bAllGrp )
                           {
                              SETFLAG( ulGpiGrp, 1 << i );
                           }

                           /*
                            * Issue error and terminate if all API groups
                            * indicated
                            */
                           else
                           {
                              Error( "Invalid API group in -G command line option argument when ALL specified first: %s", szGroup );
                           }
                        }

                        /*
                         * Clear specified PMGPI API groups bit if set bit(s)
                         * not indicated
                         */
                        else
                        {
                           /*
                            * Clear specified PMGPI API groups bit if all API
                            * groups indicated
                            */
                           if ( bAllGrp )
                           {
                              CLRFLAG( ulGpiGrp, 1 << i );
                           }

                           /*
                            * Issue error and terminate if all API groups not
                            * indicated
                            */
                           else
                           {
                              Error( "Invalid API group in -G command line option argument when ALL not specified first: %s", szGroup -= 2 );
                           }
                        }

                        break;
                     }
                  }

                  /*
                   * Issue error and terminate if string argument invalid
                   */
                  if ( i >= sizeof( szGpiGroups ) / sizeof( PCHAR ) )
                  {
                     Error( "Invalid API group in -G command line option argument: %s", bSetBit ? szGroup : ( szGroup -= 2 ) );
                  }
               } while ( ( szGroup = strtok( NULL, szGrpDelimiter ) ) != NULL );
            }

            /*
             * Issue error and terminate if string argument absent
             */
            else
            {
               Error( "Missing -G command line option argument" );
            }

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for dynamic API interception enablement if -I
          * specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "I" ) )
         {
            /*
             * Obtain dynamic API interception enablement if string argument
             * present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate disable dynamic API interception if "OFF" specified
                */
               if ( !Strcmpi( argv[ 0 ], "OFF" ) )
               {
                  CLRFLAG( ulTrcFlg, F_INTERCEPT );
               }

               /*
                * Indicate enable dynamic API interception if "ON" specified
                */
               else if ( !Strcmpi( argv[ 0 ], "ON" ) )
               {
                  SETFLAG( ulTrcFlg, F_INTERCEPT );
               }

               /*
                * Issue error and terminate if argument invalid
                */
               else
               {
                  Error( "Invalid -I command line option argument: %s", argv[ 0 ] );
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -I command line option argument" );
            }

            /*
             * Indicate -I option specified
             */
            SETFLAG( ulFlags, fIOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Customize API tracing for trace information level if -L specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "L" ) )
         {
            /*
             * Obtain trace information level if decimal argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Issue error and terminate if decimal argument invalid
                */
               if ( ( ( i = atoi( argv[ 0 ] ) ) < MIN_TRCLVL ) || ( i > MAX_TRCLVL ) )
               {
                  Error( "Invalid -L command line option argument: %s", argv[ 0 ] );
               }

               /*
                * Save trace information level
                */
               ulTrcLvl = i;
            }

            /*
             * Issue error and terminate if decimal argument absent
             */
            else
            {
               Error( "Missing -L command line option argument" );
            }

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Disable API tracing if -OFF specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "OFF" ) )
         {
            /*
             * Issue error and terminate if enable API tracing already
             * specified
             */
            if ( FLAGSET( ulFlags, fTRACE_ON ) )
            {
               Error( "-OFF and -ON are mutually exclusive" );
            }

            /*
             * Indicate disable API tracing specified
             */
            SETFLAG( ulFlags, fTRACE_OFF );

            /*
             * Indicate trace enablement option specified
             */
            SETFLAG( ulFlags, fENABLE );
         }

         /*
          * Enable API tracing if -ON specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "ON" ) )
         {
            /*
             * Issue error and terminate if disable API tracing already
             * specified
             */
            if ( FLAGSET( ulFlags, fTRACE_OFF ) )
            {
               Error( "-OFF and -ON are mutually exclusive" );
            }

            /*
             * Indicate enable API tracing specified
             */
            SETFLAG( ulFlags, fTRACE_ON );

            /*
             * Indicate trace enablement option specified
             */
            SETFLAG( ulFlags, fENABLE );
         }

         /*
          * Preserve executable file date/time stamp if -P specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "P" ) )
         {
            /*
             * Indicate -P option specified
             */
            SETFLAG( ulFlags, fPOPTION );

            /*
             * Indicate trace enablement option specified
             */
            SETFLAG( ulFlags, fENABLE );
         }

         /*
          * Control API tracing if -PAUSE specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "PAUSE" ) )
         {
            /*
             * Indicate -PAUSE option specified
             */
            SETFLAG( ulFlags, fPAUSEOPT );

            /*
             * Indicate trace control option specified
             */
            SETFLAG( ulFlags, fCONTROL );
         }

         /*
          * Query trace customization options if -Q specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "Q" ) )
         {
            /*
             * Indicate -Q option specified
             */
            SETFLAG( ulFlags, fQOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Control API tracing if -RESUME specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "RESUME" ) )
         {
            /*
             * Indicate -RESUME option specified
             */
            SETFLAG( ulFlags, fRESUMEOPT );

            /*
             * Indicate trace control option specified
             */
            SETFLAG( ulFlags, fCONTROL );
         }

         /*
          * Summarize API tracing if -S specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "S" ) )
         {
            /*
             * Indicate trace summarization option specified
             */
            SETFLAG( ulFlags, fSUMMARY );
         }

         /*
          * Customize API tracing for API entry/exit time stamping enablement
          * if -T specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "T" ) )
         {
            /*
             * Obtain API entry/exit time stamping enablement if string
             * argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate disable API entry/exit time stamping if "OFF"
                * specified
                */
               if ( !Strcmpi( argv[ 0 ], "OFF" ) )
               {
                  CLRFLAG( ulTrcFlg, F_TIMESTAMP );
               }

               /*
                * Indicate enable API entry/exit time stamping if "ON"
                * specified
                */
               else if ( !Strcmpi( argv[ 0 ], "ON" ) )
               {
                  SETFLAG( ulTrcFlg, F_TIMESTAMP );
               }

               /*
                * Issue error and terminate if argument invalid
                */
               else
               {
                  Error( "Invalid -T command line option argument: %s", argv[ 0 ] );
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -T command line option argument" );
            }

            /*
             * Indicate -T option specified
             */
            SETFLAG( ulFlags, fTOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

#ifdef T_USERHOOK
         /*
          * Customize API tracing for user hook if -U specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "U" ) )
         {
            /*
             * Obtain user hook if argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Indicate no user hook if "NONE" specified
                */
               if ( !Strcmpi( argv[ 0 ], "NONE" ) )
               {
                  szUserHk[ 0 ] = '\0';
               }

               /*
                * Issue error and terminate if string argument invalid
                */
               else if ( ( ( i = strlen( argv[ 0 ] ) ) < 3            ) ||
                         ( strchr( argv[ 0 ], ':' ) != NULL           ) ||
                         ( strchr( argv[ 0 ], '\\' ) != NULL          ) ||
                         ( ( pch = strchr( argv[ 0 ], '.' ) ) == NULL ) ||
                         ( strchr( pch + 1, '.' ) != NULL             ) ||
                         ( argv[ 0 ][ 0 ] == '.'                      ) ||
                         ( argv[ 0 ][ i - 1 ] == '.'                  ) )
               {
                  Error( "Invalid -U command line option argument: %s", argv[ 0 ] );
               }

               /*
                * Save user hook and upper case library name if string argument
                * valid
                */
               else
               {
                  strcpy( szUserHk, argv[ 0 ] );
                  i = strcspn( szUserHk, "." );
                  szUserHk[ i ] = '\0';
                  Strupr( szUserHk );
                  szUserHk[ i ] = '.';
               }
            }

            /*
             * Issue error and terminate if argument absent
             */
            else
            {
               Error( "Missing -U command line option argument" );
            }

            /*
             * Indicate -U option specified
             */
            SETFLAG( ulFlags, fUOPTION );

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }
#endif

         /*
          * Customize API tracing for PMWIN API groups if -W specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "W" ) )
         {
            /*
             * Obtain PMWIN API groups if string argument present
             */
            if ( --argc >= 0 )
            {
               /*
                * Skip command line argument
                */
               argv++;

               /*
                * Obtain first PMWIN API group
                */
               szGroup = strtok( argv[ 0 ], szGrpDelimiter );

               /*
                * Set all PMWIN API groups bits if "ALL" specified
                */
               if ( !Strcmpi( szGroup, "ALL" ) )
               {
                  /*
                   * Set all PMWIN API groups bits
                   */
                  ulWinGrp = W_ALL;

                  /*
                   * Obtain next PMWIN API group, parse next command line
                   * argument if next group absent
                   */
                  if ( !( szGroup = strtok( NULL, szGrpDelimiter ) ) )
                  {
                     /*
                      * Indicate trace customization option specified
                      */
                     SETFLAG( ulFlags, fCUSTOM );

                     /*
                      * Skip command line argument
                      */
                     argv++;

                     continue;
                  }

                  /*
                   * Indicate all API groups specified
                   */
                  bAllGrp = TRUE;
               }

               /*
                * Indicate not all API groups specified if "ALL" not specified
                */
               else
               {
                  bAllGrp = FALSE;
               }

               /*
                * Process every specified PMWIN API group
                */
               do
               {
                  /*
                   * Indicate clear bit(s) if "NO" prefix specified
                   */
                  if ( ( toupper( szGroup[ 0 ] ) == 'N' ) && ( toupper( szGroup[ 1 ] ) == 'O' ) )
                  {
                     /*
                      * Indicate clear bit(s)
                      */
                     bSetBit = FALSE;

                     /*
                      * Skip "NO" prefix
                      */
                     szGroup += 2;
                  }

                  /*
                   * Indicate set bit(s) if "NO" prefix not specified
                   */
                  else
                  {
                     bSetBit = TRUE;
                  }

                  /*
                   * Search for matching PMWIN API group
                   */
                  for ( i = 0; i < sizeof( szWinGroups ) / sizeof( PCHAR ); i++ )
                  {
                     /*
                      * Search complete if matching PMWIN API group found
                      */
                     if ( !Strcmpi( szGroup, szWinGroups[ i ] ) )
                     {
                        /*
                         * Set specified PMWIN API groups bit if set bit(s)
                         * indicated
                         */
                        if ( bSetBit )
                        {
                           /*
                            * Set specified PMWIN API groups bit if all API
                            * groups not indicated
                            */
                           if ( !bAllGrp )
                           {
                              SETFLAG( ulWinGrp, 1 << i );
                           }

                           /*
                            * Issue error and terminate if all API groups
                            * indicated
                            */
                           else
                           {
                              Error( "Invalid API group in -W command line option argument when ALL specified first: %s", szGroup );
                           }
                        }

                        /*
                         * Clear specified PMWIN API groups bit if set bit(s)
                         * not indicated
                         */
                        else
                        {
                           /*
                            * Clear specified PMWIN API groups bit if all API
                            * groups indicated
                            */
                           if ( bAllGrp )
                           {
                              CLRFLAG( ulWinGrp, 1 << i );
                           }

                           /*
                            * Issue error and terminate if all API groups not
                            * indicated
                            */
                           else
                           {
                              Error( "Invalid API group in -W command line option argument when ALL not specified first: %s", szGroup -= 2 );
                           }
                        }

                        break;
                     }
                  }

                  /*
                   * Issue error and terminate if string argument invalid
                   */
                  if ( i >= sizeof( szWinGroups ) / sizeof( PCHAR ) )
                  {
                     Error( "Invalid API group in -W command line option argument: %s", bSetBit ? szGroup : ( szGroup -= 2 ) );
                  }
               } while ( ( szGroup = strtok( NULL, szGrpDelimiter ) ) != NULL );
            }

            /*
             * Issue error and terminate if string argument absent
             */
            else
            {
               Error( "Missing -W command line option argument" );
            }

            /*
             * Indicate trace customization option specified
             */
            SETFLAG( ulFlags, fCUSTOM );
         }

         /*
          * Enable/disable API tracing for specific DLL if -dll specified
          */
         else if ( ( i = LookupDLL( &argv[ 0 ][ 1 ] ) ) != UNKNOWN_DLL )
         {
            /*
             * Indicate trace on/off enabled for specific DLL
             */
            DLLInfo[ i ].bEnable = TRUE;

            /*
             * Indicate trace enablement option specified
             */
            SETFLAG( ulFlags, fENABLE );
         }

         /*
          * Enable/disable API tracing for DOSCALLS.DLL if -DOSCALL1 specified
          */
         else if ( !Strcmpi( &argv[ 0 ][ 1 ], "DOSCALL1" ) )
         {
            /*
             * Indicate trace on/off enabled for DOSCALLS.DLL
             */
            DLLInfo[ LookupDLL( "DOSCALLS" ) ].bEnable = TRUE;

            /*
             * Indicate trace enablement option specified
             */
            SETFLAG( ulFlags, fENABLE );
         }

         /*
          * Issue error and terminate if unrecognized command line option
          */
         else
         {
            Error( "Unrecognized command line option: %s", argv[ 0 ] );
         }
      }

      /*
       * Valid file name starts with any other character
       */
      else
      {
         /*
          * Issue error and terminate if file name already specified
          */
         if ( szInFile[ 0 ] )
         {
            Error( "Extra file name: %s", argv[ 0 ] );
         }

         /*
          * Save file name
          */
         strcpy( szInFile, argv[ 0 ] );
      }

      /*
       * Issue error and terminate if both trace customization option and trace
       * enablement option specified
       */
      if ( ( FLAGSET( ulFlags, fCUSTOM ) ) && ( FLAGSET( ulFlags, fENABLE ) ) )
      {
         Error( "Trace customization and trace enablement are mutually exclusive" );
      }

      /*
       * Issue error and terminate if both trace enablement option and trace
       * summarization option specified
       */
      else if ( ( FLAGSET( ulFlags, fENABLE ) ) && ( FLAGSET( ulFlags, fSUMMARY ) ) )
      {
         Error( "Trace enablement and trace summarization are mutually exclusive" );
      }

      /*
       * Issue error and terminate if both trace summarization option and trace
       * customization option specified
       */
      else if ( ( FLAGSET( ulFlags, fSUMMARY ) ) && ( FLAGSET( ulFlags, fCUSTOM ) ) )
      {
         Error( "Trace summarization and trace customization are mutually exclusive" );
      }

      /*
       * Issue error and terminate if both trace control option and trace
       * customization option specified
       */
      else if ( ( FLAGSET( ulFlags, fCONTROL ) ) && ( FLAGSET( ulFlags, fCUSTOM ) ) )
      {
         Error( "Trace control and trace customization are mutually exclusive" );
      }

      /*
       * Issue error and terminate if both trace control option and trace
       * enablement option specified
       */
      else if ( ( FLAGSET( ulFlags, fCONTROL ) ) && ( FLAGSET( ulFlags, fENABLE ) ) )
      {
         Error( "Trace control and trace enablement are mutually exclusive" );
      }

      /*
       * Issue error and terminate if both trace control option and trace
       * summarization option specified
       */
      else if ( ( FLAGSET( ulFlags, fCONTROL ) ) && ( FLAGSET( ulFlags, fSUMMARY ) ) )
      {
         Error( "Trace control and trace summarization are mutually exclusive" );
      }

      /*
       * Skip command line argument
       */
      argv++;
   }

   /*
    * Issue error and terminate if trace enablement or trace customization or
    * trace summarization flag missing
    */
   if ( FLAGCLR( ulFlags, fCONTROL | fCUSTOM | fENABLE | fSUMMARY ) )
   {
      Error( "Missing option(s)" );
   }

   /*
    * Issue error and terminate if trace enablement or trace summarization
    * specified and file name missing
    */
   else if ( ( FLAGSET( ulFlags, fENABLE | fSUMMARY ) ) && ( !szInFile[ 0 ] ) )
   {
      Error( "Missing file name" );
   }

   /*
    * Issue error and terminate if trace control or trace customization
    * specified and file name present
    */
   else if ( ( FLAGSET( ulFlags, fCONTROL | fCUSTOM ) ) && ( szInFile[ 0 ] ) )
   {
      Error( "Unnecessary file name: %s", szInFile );
   }

   /*
    * Issue error and terminate if trace enablement specified and disable or
    * enable trace flag missing
    */
   else if ( ( FLAGSET( ulFlags, fENABLE ) ) && ( FLAGCLR( ulFlags, fTRACE_OFF | fTRACE_ON ) ) )
   {
      Error( "Missing enable/disable trace option (-ON|-OFF)" );
   }

   /*
    * Issue error and terminate if trace enablement specified and no DLL(s)
    * with trace on/off enabled
    */
   else if ( FLAGSET( ulFlags, fENABLE ) )
   {
      /*
       * Examine each supported DLL
       */
      for ( i = 0; i < SUPPORTED_DLL; i++ )
      {
         /*
          * Search complete if DLL found with trace on/off enabled
          */
         if ( DLLInfo[ i ].bEnable )
         {
            break;
         }
      }

      /*
       * Issue error if no DLLs with trace on/off enabled
       */
      if ( i >= SUPPORTED_DLL )
      {
         Error( "Missing DLL option (-ALL|-dll)" );
      }
   }

}  /* ParseCommandLine() */

/******************************************************************************/
/*                                                                            */
/* Strcmpi()                                                                  */
/*                                                                            */
/******************************************************************************/

LONG Strcmpi( PCHAR szStr1                       /* First string              */
            , PCHAR szStr2                       /* Second string             */
            )
{

   /*
    * Compare each character in first string to corresponding character in
    * second string ignoring case
    */
   for ( ; tolower( *szStr1 ) == tolower( *szStr2 ); szStr1++, szStr2++ )
   {
      /*
       * Indicate strings equal if end of strings encountered
       */
      if ( !*szStr1 )
      {
         return 0L;
      }
   }

   /*
    * Indicate strings not equal
    */
   return *szStr1 - *szStr2;

}  /* Strcmpi() */

/******************************************************************************/
/*                                                                            */
/* Strupr()                                                                   */
/*                                                                            */
/******************************************************************************/

PCHAR Strupr( PCHAR szStr )                      /* String                    */
{

   PCHAR pch = szStr;                            /* Character pointer         */

   /*
    * Convert each character in string to upper case
    */
   for ( ; *pch; pch++ )
   {
      /*
       * Convert character to upper case if lower case
       */
      if ( islower( *pch ) )
      {
         *pch = toupper( *pch );
      }
   }

   /*
    * Return string
    */
   return szStr;

}  /* Strupr() */

/******************************************************************************/
/*                                                                            */
/* SummarizeAPITracing()                                                      */
/*                                                                            */
/******************************************************************************/

VOID SummarizeAPITracing( VOID )                 /* No parameters             */
{

   BOOL     bAPIEntry = FALSE;                   /* API entry found indicator */
   BOOL     bWarning = FALSE;                    /* Display warning indicator */
   LONG     lNoReturn;                           /* Invocations without return*/
   PAPIINFO pAPI;                                /* API information           */
   PAPIINFO pLastAPI = NULL;                     /* Last API information      */
   CHAR     szAPIExit[ LEN_IOLINE ];             /* Initial API exit name     */
   CHAR     szAPIName[ MAX_APINAME ];            /* API name                  */
   CHAR     szEntExt[ LEN_IOLINE ];              /* API entry/exit string     */
   CHAR     szLine[ LEN_IOLINE ];                /* Trace info file line      */
   CHAR     szStatus[ LEN_IOLINE ];              /* API exit status string    */
   CHAR     szTemp[ LEN_IOLINE ];                /* Temporary string          */
   ULONG    ulAPIExit = 0L;                      /* Initial API exit result   */

   /*
    * Open trace information file, issue error and terminate if unsuccessful
    */
   if ( !( fpInFile = fopen( szInFile, "r" ) ) )
   {
      Error( "Unable to open file: %s", szInFile );
   }

   /*
    * Read, parse, and analyze each line in trace information file
    */
   while ( fgets( szLine, LEN_IOLINE, fpInFile ) )
   {
      /*
       * There is one line to look for that indicates the entry to an API call:
       *
       *  PID  TID | API Entry
       *
       * And there are two lines to look for that indicate the exit from an API
       * call:
       *
       *  PID  TID | API Exit
       *      PASS | Return code: 0
       *
       * Parse line from trace information file and analyze if line contains
       * same number of tokens as API entry or exit line
       */
      if ( sscanf( szLine, "%s %s | %s %s", szTemp, szTemp, szAPIName, szEntExt ) == 4 )
      {
         /*
          * API entry line found
          */
         if ( !strncmp( szEntExt, "Entry", 5 ) )
         {
            /*
             * Update API information if API already called
             */
            if ( ( pAPI = GetAPIInfo( szAPIName ) ) != NULL )
            {
               /*
                * Update API information
                */
               pAPI->ulCall++;
            }

            /*
             * Add API information if API not already called
             */
            else
            {
               /*
                * Add API information
                */
               pAPI = AddAPIInfo( szAPIName, 1L, 0L, 0L, 0L );
            }

            /*
             * Indicate API entry found
             */
            bAPIEntry = TRUE;

            /*
             * Save last API information
             */
            pLastAPI  = pAPI;
         }

         /*
          * API exit line found
          */
         else if ( !strncmp( szEntExt, "Exit", 4 ) )
         {
            /*
             * Read and parse line from trace information file, update API
             * information if successful
             */
            if ( fgets( szLine, LEN_IOLINE, fpInFile ) )
            {
               /*
                * Update API information if API already called
                */
               if ( ( pAPI = GetAPIInfo( szAPIName ) ) != NULL )
               {
                  /*
                   * Clear last API information
                   */
                  pLastAPI = NULL;

                  /*
                   * Update API information if indeterminate API invocation
                   */
                  if ( !strncmp( &szLine[ strspn( szLine, " \t" ) ], "| Return code:", 14 ) )
                  {
                     /*
                      * Update API information
                      */
                     pAPI->ulIndeterminate++;
                  }

                  /*
                   * Update API information if successful or unsuccessful API
                   * invocation
                   */
                  else if ( sscanf( szLine, "%s | Return code:", szStatus ) == 1 )
                  {
                     /*
                      * Update API information
                      */
                     strcmp( szStatus, "PASS" ) ? pAPI->ulFail++ : pAPI->ulPass++;
                  }
               }

               /*
                * Update saved API information if API not already called and
                * API entry not yet found (trace information file begins with
                * API exit)
                */
               else if ( !bAPIEntry )
               {
                  /*
                   * Save initial API exit name
                   */
                  strcpy( szAPIExit, szAPIName );

                  /*
                   * Save initial API exit result if indeterminate API
                   * invocation
                   */
                  if ( !strncmp( &szLine[ strspn( szLine, " \t" ) ], "| Return code:", 14 ) )
                  {
                     ulAPIExit = APIEXT_IND;
                  }

                  /*
                   * Save initial API exit result if successful or unsuccessful
                   * API invocation
                   */
                  else if ( sscanf( szLine, "%s | Return code:", szStatus ) == 1 )
                  {
                     ulAPIExit = strcmp( szStatus, "PASS" ) ? APIEXT_FAIL : APIEXT_PASS;
                  }
               }
            }
         }
      }

      /*
       * There are four lines to look for that indicate API entry/exit
       * information may be lost:
       *
       *  < OS2TRACE: API entry/exit information lost >
       *  < OS2TRACE: Trace information lost due to log file wrapping x times >
       *  < OS2TRACE: Pausing trace at xx:xx:xx.xx >
       *  < OS2TRACE: Resuming trace at xx:xx:xx.xx >
       *
       * Parse line from trace information file and analyze if line contains
       * "< OS2TRACE: " prefix
       */
      else if ( sscanf( szLine, "< OS2TRACE: %s", szTemp ) == 1 )
      {
         /*
          * Indicate display warning if possible lost API entry/exit
          * information found
          */
         if ( ( !strncmp( szTemp, "API", 3 )      ) ||
              ( !strncmp( szTemp, "Trace", 5 )    ) ||
              ( !strncmp( szTemp, "Pausing", 7 )  ) ||
              ( !strncmp( szTemp, "Resuming", 8 ) ) )
         {
            bWarning = TRUE;
         }
      }
   }

   /*
    * Update API information if trace information file begins with API exit and
    * ends with API entry and it is same API
    */
   if ( ( ulAPIExit ) && ( pLastAPI ) && ( !strcmp( pLastAPI->szName, szAPIExit ) ) )
   {
      /*
       * Update API information
       */
      switch ( ulAPIExit )
      {
         case APIEXT_PASS: pLastAPI->ulPass++; break;
         case APIEXT_FAIL: pLastAPI->ulFail++; break;
         case APIEXT_IND : pLastAPI->ulIndeterminate++; break;
      }
   }

   /*
    * Close trace information file
    */
   fclose( fpInFile );

   /*
    * Display warning if so indicated
    */
   if ( bWarning )
   {
      printf( "%s: Summarization may not be accurate due to lost API entries/exits, pausing/resuming trace, or file wrapping\n\n", szAppName );
   }

   /*
    * Display used APIs header
    */
   printf( "Used APIs:\n" );

   /*
    * Display API summary statistics starting at beginning of API information
    * chain if the chain is not empty
    */
   if ( ( pAPI = pAPIFirst ) != NULL )
   {
      /*
       * Display summary statistics for each API in API information chain
       */
      while ( ( pAPI ) && ( pAPI->ulCall ) )
      {
         /*
          * Display API name
          */
         printf( "  %s (", pAPI->szName );

         /*
          * Display any successful API invocations
          */
         if ( pAPI->ulPass )
         {
            printf( "%d Pass", pAPI->ulPass );
         }

         /*
          * Display any unsuccessful API invocations
          */
         if ( pAPI->ulFail )
         {
            printf( "%s%d Fail", pAPI->ulPass ? ", " : "", pAPI->ulFail );
         }

         /*
          * Display any indeterminate API invocations
          */
         if ( pAPI->ulIndeterminate )
         {
            printf( "%s%d Indeterminate", pAPI->ulPass || pAPI->ulFail ? ", " : "", pAPI->ulIndeterminate );
         }

         /*
          * Display any API invocations that did not return
          */
         if ( ( pAPI->ulCall != pAPI->ulPass + pAPI->ulFail + pAPI->ulIndeterminate ) && ( ( lNoReturn = pAPI->ulCall - pAPI->ulPass - pAPI->ulFail - pAPI->ulIndeterminate ) > 0L ) )
         {
            printf( "%s%d No Return", pAPI->ulPass || pAPI->ulFail || pAPI->ulIndeterminate ? ", " : "", lNoReturn );
         }

         /*
          * Terminate API information line
          */
         printf( ")\n" );

         /*
          * Skip API information
          */
         pAPI = pAPI->pNext;
      }

      /*
       * Clean up API information
       */
      TerminateSummarization( TRUE );
   }

   /*
    * Display None if API information chain is empty
    */
   else
   {
      printf( "  None\n" );
   }

}  /* SummarizeAPITracing() */

/******************************************************************************/
/*                                                                            */
/* TerminateEnablement()                                                      */
/*                                                                            */
/******************************************************************************/

VOID TerminateEnablement( BOOL bErrMsgs )        /* Error messages allowed    */
{

   FILESTATUS3 filestatus3;                      /* File information          */
   HFILE       hfile;                            /* File handle               */
   ULONG       ulAction;                         /* Action taken by DosOpen   */
   ULONG       ulAttr;                           /* File open attributes      */
   ULONG       ulFlag;                           /* File open flags           */
   ULONG       ulMode;                           /* File open mode            */

   /*
    * Free memory for dynamic linking information table if one exists
    */
   if ( pDynLnkInf )
   {
      free( pDynLnkInf );
      pDynLnkInf = NULL;
   }

   /*
    * Free memory for fixup record table if one exists
    */
   if ( pFixupTbl )
   {
      free( pFixupTbl );
      pFixupTbl = NULL;
   }

   /*
    * Free memory for import name table if one exists
    */
   if ( pImpNamTbl )
   {
      free( pImpNamTbl );
      pImpNamTbl = NULL;
   }

   /*
    * Free memory for module index table if one exists
    */
   if ( pModIdxTbl )
   {
      free( pModIdxTbl );
      pModIdxTbl = NULL;
   }

   /*
    * Free memory for module name table if one exists
    */
   if ( pModNamTbl )
   {
      free( pModNamTbl );
      pModNamTbl = NULL;
   }

   /*
    * Free memory for module reference table if one exists
    */
   if ( pModRefTbl )
   {
      free( pModRefTbl );
      pModRefTbl = NULL;
   }

   /*
    * Free memory for procedure name table if one exists
    */
   if ( pPrcNamTbl )
   {
      free( pPrcNamTbl );
      pPrcNamTbl = NULL;
   }

   /*
    * Free memory for program header table if one exists
    */
   if ( pProgHdr )
   {
      free( pProgHdr );
      pProgHdr = NULL;
   }

   /*
    * Free memory for string table if one exists
    */
   if ( pStrTbl )
   {
      free( pStrTbl );
      pStrTbl = NULL;
   }

   /*
    * Free memory for segment if one exists
    */
   if ( pSegment )
   {
      free( pSegment );
      pSegment = NULL;
   }

   /*
    * Free memory for segment table if one exists
    */
   if ( pSegmentTbl )
   {
      free( pSegmentTbl );
      pSegmentTbl = NULL;
   }

   /*
    * Close executable file if opened, issue error and terminate if
    * unsuccessful
    */
   if ( ( fpInFile ) && ( fclose( fpInFile ) ) )
   {
      if ( bErrMsgs ) Error( "Unable to close file: %s", szInFile );
   }
   else
   {
      fpInFile = NULL;
   }

   /*
    * Restore executable file date/time stamp if -P option specified
    */
   if ( FLAGSET( ulFlags, fPOPTION ) )
   {
      /*
       * Disable -P option
       */
      CLRFLAG( ulFlags, fPOPTION );

      /*
       * Initialize open file flags
       */
      ulAttr = FILE_NORMAL;
      ulFlag = OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW;
      ulMode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE;

      /*
       * Open executable file, obtain file information if successful
       */
      if ( !DosOpen( szInFile, &hfile, &ulAction, 0L, ulAttr, ulFlag, ulMode, NULL ) )
      {
         /*
          * Obtain executable file information, restore file date/time stamp if
          * successful
          */
         if ( !DosQueryFileInfo( hfile, FIL_STANDARD, &filestatus3, sizeof( FILESTATUS3 ) ) )
         {
            filestatus3.fdateLastWrite = fdateInFile;
            filestatus3.ftimeLastWrite = ftimeInFile;

            /*
             * Restore file date/time stamp, issue error and terminate if
             * unsuccessful
             */
            if ( DosSetFileInfo( hfile, FIL_STANDARD, &filestatus3, sizeof( FILESTATUS3 ) ) )
            {
               if ( bErrMsgs ) Error( "Unable to preserve file date/time stamp" );
            }
         }

         /*
          * Issue error and terminate if unable to obtain executable file
          * information
          */
         else
         {
            if ( bErrMsgs ) Error( "Unable to preserve file date/time stamp" );
         }

         /*
          * Close executable file
          */
         DosClose( hfile );
      }

      /*
       * Issue error and terminate if unable to open executable file
       */
      else
      {
         if ( bErrMsgs ) Error( "Unable to preserve file date/time stamp" );
      }
   }

}  /* TerminateEnablement() */

/******************************************************************************/
/*                                                                            */
/* TerminateSummarization()                                                   */
/*                                                                            */
/******************************************************************************/

VOID TerminateSummarization( BOOL bErrMsgs )     /* Error messages allowed    */
{

   PAPIINFO pAPI;                                /* API information           */
   PAPIINFO pAPIFree;                            /* API information to free   */

   /*
    * Free memory for API information if information exists
    */
   if ( pAPIFirst )
   {
      /*
       * Start at beginning of API information chain
       */
      pAPI = pAPIFirst;

      /*
       * Free memory for each API until last API encountered
       */
      while ( pAPI->pNext )
      {
         pAPIFree = pAPI;
         pAPI = pAPI->pNext;
         free( pAPIFree );
      }

      /*
       * Free memory for last API
       */
      free( pAPI );
   }

}  /* TerminateSummarization() */

/******************************************************************************/
/*                                                                            */
/* Usage()                                                                    */
/*                                                                            */
/******************************************************************************/

VOID Usage( VOID )                               /* No parameters             */
{

   int i;                                        /* Loop variable             */

   /*
    * Display trace enablement helping information separator
    */
   printf( "------------------------------ Trace Enabler -------------------------------\n" );

   /*
    * Display trace enablement helping information
    */
   printf( "\nUsage:\n" );
   printf( "\t%s -OFF|-ON {-ALL|-dll}... [-P] file\n", szAppName );
   printf( "Where:\n" );
   printf( "\t-OFF indicates disable API tracing\n" );
   printf( "\t-ON  indicates enable API tracing\n" );
   printf( "\t-ALL indicates enable/disable API tracing for all DLLs\n" );
   printf( "\t-dll indicates enable/disable  API tracing  for specific DLL,  where\n" );
   printf( "\t     dll can be one of the following:" );

   /*
    * Display table of supported DLLs
    */
   for ( i = 0; i < SUPPORTED_DLL; i++ )
   {
      /*
       * Display up to five DLLs per line from table of supported DLLs
       */
      if ( !( i % 5 ) )
      {
         printf( "\n\t       " );
      }

      printf( "%-10s", DLLInfo[ i ].szName );
   }
   putchar( '\n' );

   /*
    * Display remainder of trace enablement helping information
    */
   printf( "\t-P   indicates preserve executable file date/time stamp\n" );
   printf( "\tfile indicates name of executable file to be trace enabled/disabled\n" );

   /*
    * Display trace enablement note
    */
   printf( "\nNote:" );
   printf( "\tTrace enablement alters the contents of the executable file's import\n" );
   printf( "\tmodule name table.\n" );

   /*
    * Display trace customization helping information separator
    */
   printf( "\n----------------------------- Trace Customizer -----------------------------\n" );

   /*
    * Display trace customization helping information
    */
   printf( "\nUsage:\n" );
#ifdef T_USERHOOK
   printf( "\t%s {-A s|-B n|-C f|-D g|-E f|-F n|-G g|-I f|-L n|-Q|-T f|-U s|-W g}...\n", szAppName );
#else
   printf( "\t%s {-A s|-B n|-C f|-D g|-E f|-F n|-G g|-I f|-L n|-Q|-T f|-W g}...\n", szAppName );
#endif
   printf( "Where:\n" );
   printf( "\t-A s indicates fully qualified drive and path of alternative logging\n" );
   printf( "\t     directory or NONE for default directory\n" );
   printf( "\t-B n indicates  log  maximum  of  n  bytes  from  buffers,     where\n" );
   printf( "\t     %d ó n ó %d (K suffix allowed)  or n is ALL to  indicate log\n", MIN_BUFLEN, MAX_BUFLEN );
   printf( "\t     all bytes from buffers\n" );
   printf( "\t-C f indicates enable (f = ON) or disable (f = OFF) tracing control\n" );
   printf( "\t-D g indicates trace specific DOSCALLS API groups, where g is either\n" );
   printf( "\t     ALL[,NOgrp]... or grp[,grp]... and grp is one of the following:" );

   /*
    * Display table of DOSCALLS API groups
    */
   for ( i = 0; i < sizeof( szDosGroups ) / sizeof( PCHAR ); i++ )
   {
      /*
       * Display up to ten groups per line from table of DOSCALLS API groups
       */
      if ( !( i % 10 ) )
      {
         printf( "\n\t       " );
      }

      printf( "%-6s", szDosGroups[ i ] );
   }
   putchar( '\n' );

   /*
    * Display additional trace customization helping information
    */
   printf( "\t-E f indicates  enable  (f = ON) or  disable  (f = OFF)  logging  of\n" );
   printf( "\t     EBCDIC translation of character buffers\n" );
   printf( "\t-F n indicates log  maximum of  n  bytes  before  log  file wrapping\n" );
   printf( "\t     occurs,  where %d ó n ó %d (K or M suffixes allowed) or\n", MIN_FILLEN, MAX_FILLEN );
   printf( "\t     n  is ALL  to indicate  log all  information  without  log file\n" );
   printf( "\t     wrapping\n" );
   printf( "\t-G g indicates  trace specific  PMGPI API groups,  where g is either\n" );
   printf( "\t     ALL[,NOgrp]... or grp[,grp]... and grp is one of the following:" );

   /*
    * Display table of PMGPI API groups
    */
   for ( i = 0; i < sizeof( szGpiGroups ) / sizeof( PCHAR ); i++ )
   {
      /*
       * Display up to ten groups per line from table of PMGPI API groups
       */
      if ( !( i % 10 ) )
      {
         printf( "\n\t       " );
      }

      printf( "%-6s", szGpiGroups[ i ] );
   }
   putchar( '\n' );

   /*
    * Display remainder of trace customization helping information
    */
   printf( "\t-I f indicates enable (f = ON) or disable (f = OFF) interception  of\n" );
   printf( "\t     dynamic API calls\n" );
   printf( "\t-L n indicates log level n information, where %d ó n ó %d:\n", MIN_TRCLVL, MAX_TRCLVL );
   printf( "\t       1 indicates log API entry/exit information\n" );
   printf( "\t       2 indicates log API parameters\n" );
   printf( "\t       3 indicates log API parameter contents\n" );
   printf( "\t-Q   indicates query current trace customization options\n" );
   printf( "\t-T f indicates enable (f = ON) or disable (f = OFF) time stamping of\n" );
   printf( "\t     API entries and exits\n" );
#ifdef T_USERHOOK
   printf( "\t-U s indicates user hook of the format  DLLNAME.HOOKNAME or NONE for\n" );
   printf( "\t     no user hook\n" );
#endif
   printf( "\t-W g indicates  trace specific  PMWIN API groups,  where g is either\n" );
   printf( "\t     ALL[,NOgrp]... or grp[,grp]... and grp is one of the following:" );

   /*
    * Display table of PMWIN API groups
    */
   for ( i = 0; i < sizeof( szWinGroups ) / sizeof( PCHAR ); i++ )
   {
      /*
       * Display up to ten groups per line from table of PMWIN API groups
       */
      if ( !( i % 10 ) )
      {
         printf( "\n\t       " );
      }

      printf( "%-6s", szWinGroups[ i ] );
   }
   putchar( '\n' );

   /*
    * Display trace customization note
    */
   printf( "\nNote:" );
   printf( "\tDefault trace customization option settings are:\n" );
   printf( "\t-A %s", strlen( DEF_ALTDIR ) ? DEF_ALTDIR : "NONE" );
   printf( DEF_BUFLENInt ? " -B %d" : " -B ALL", DEF_BUFLENInt );
   printf( " -C %s -D ", FLAGSET( DEF_TRCFLGInt, F_CONTROL ) ? "ON" : "OFF" );
   DisplayDOSCALLSGroups( DEF_DOSGRPInt );
   printf( " -E %s", FLAGSET( DEF_TRCFLGInt, F_EBCDIC ) ? "ON" : "OFF" );
   printf( DEF_FILLENInt ? " -F %d -G " : " -F ALL -G ", DEF_FILLENInt );
   DisplayPMGPIGroups( DEF_GPIGRPInt );
   printf( " -I %s -L %d -T %s", FLAGSET( DEF_TRCFLGInt, F_INTERCEPT ) ? "ON" : "OFF", DEF_TRCLVLInt, FLAGSET( DEF_TRCFLGInt, F_TIMESTAMP ) ? "ON" : "OFF" );
#ifdef T_USERHOOK
   printf( " -U %s", strlen( DEF_ALTDIR ) ? DEF_ALTDIR : "NONE" );
#endif
   printf( " -W " );
   DisplayPMWINGroups( DEF_WINGRPInt );
   putchar( '\n' );

   /*
    * Display trace control helping information separator
    */
   printf( "\n----------------------------- Trace Controller -----------------------------\n" );

   /*
    * Display trace control helping information
    */
   printf( "\nUsage:\n" );
   printf( "\t%s -PAUSE|-RESUME\n", szAppName );
   printf( "Where:\n" );
   printf( "\t-PAUSE  indicates pause API tracing\n" );
   printf( "\t-RESUME indicates resume API tracing\n" );

   /*
    * Display trace control note
    */
   printf( "\nNote:" );
   printf( "\tThese  options  have no  affect unless  tracing control  is  enabled\n" );
   printf( "\t(-C ON) prior to starting a trace-enabled application.\n" );

   /*
    * Display trace summarization helping information separator
    */
   printf( "\n----------------------------- Trace Summarizer -----------------------------\n" );

   /*
    * Display trace summarization helping information
    */
   printf( "\nUsage:\n" );
   printf( "\t%s -S file\n", szAppName );
   printf( "Where:\n" );
   printf( "\t-S   indicates summarize API tracing\n" );
   printf( "\tfile indicates name of trace information file to be trace summarized\n" );

   /*
    * Terminate program
    */
   exit( NO_ERROR );

}  /* Usage() */

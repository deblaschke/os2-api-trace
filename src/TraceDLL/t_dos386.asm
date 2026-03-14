;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  File name    : T_DOS386.ASM                                                 ;
;                                                                              ;
;  Title        : OS/2 API trace DLL DOSCALLS assembly routines                ;
;                                                                              ;
;  Author       : Dave Blaschke                                                ;
;                 deblaschke@yahoo.com                                         ;
;                                                                              ;
;  Description  : DOSCALLS   assembly   routines:   Trc_Dos16CreateThread  and ;
;                 Trc_Dos32UnwindException.                                    ;
;                                                                              ;
;  History      : Version Date    Item                                         ;
;                 2.30.29 21Jun96 Fixed Dos32UnwindException bug               ;
;                 2.45.30 22Jun01 Fixed Dos32UnwindException bug               ;
;                 2.45.32 28Jun01 Added -U option                              ;
;                 2.45.35 02Dec03 Fixed Dos16CreateThread bug                  ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .386P

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Constants (from OS2TRACE.H and T_COMMON.H)                                  ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

D_XCPT                          EQU     4000H   ; DOSCALLS exception group
DT_PEXCEPTIONREGISTRATIONRECORD EQU     92      ; PEXCEPTIONREGISTRATIONRECORD data type
DT_PEXCEPTIONREPORTRECORD       EQU     93      ; PEXCEPTIONREPORTRECORD data type
DT_PVOID                        EQU     211     ; PVOID data type
L_PARMS                         EQU     2       ; API parameters trace level

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  External function declarations                                              ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFDEF T_16BIT
EXTRN   C_POSTDOS16CREATETHREAD:FAR
EXTRN   C_PREDOS16CREATETHREAD:FAR
EXTRN   DOSCREATETHREAD:FAR
ENDIF
EXTRN   DOS32UNWINDEXCEPTION:NEAR
EXTRN   T_FreeLog:NEAR
IFDEF T_USERHOOK
EXTRN   T_HookEntry:NEAR
ENDIF
EXTRN   T_LogAPIEntry:NEAR
EXTRN   T_LogParameter:NEAR
EXTRN   T_LogPEXCEPTIONREPORTRECORD:NEAR
EXTRN   T_LogPFN:NEAR
EXTRN   T_LogPVOID:NEAR
EXTRN   T_WaitLog:NEAR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  External variable declarations                                              ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

EXTRN   T_DosAPIGrp:DWORD
EXTRN   T_TraceLvl:DWORD
IFDEF T_USERHOOK
EXTRN   T_UserHook:DWORD
ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Segment definitions                                                         ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE32  SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32  ENDS
DATA32  SEGMENT DWORD USE32 PUBLIC 'DATA'
DATA32  ENDS
IFDEF T_16BIT
CODE16  SEGMENT WORD USE16 PUBLIC 'CODE'
CODE16  ENDS
ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Variable definitions                                                        ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DATA32  SEGMENT

szDos32UnwindException  DB      'Dos32UnwindException'
                        DB      0
DATA32  ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Trc_Dos16CreateThread()                                                     ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFDEF T_16BIT
        PUBLIC  TRC_DOS16CREATETHREAD

        ASSUME  CS:CODE16
CODE16  SEGMENT
        ALIGN   04H

TRC_DOS16CREATETHREAD   PROC FAR
        PUSH    BP                              ; Save old base pointer
        MOV     BP, SP                          ; BP = New base pointer

        PUSHF                                   ; Save flags register
        PUSH    AX                              ; Save AX
        PUSH    BX                              ; Save BX
        PUSH    CX                              ; Save CX
        PUSH    DX                              ; Save DX
        PUSH    SI                              ; Save SI
        PUSH    DI                              ; Save DI

        PUSH    [BP+ 4]                         ; Push parameter 1 (API caller) segment
        PUSH    [BP+ 2]                         ; Push parameter 1 (API caller) offset
        PUSH    [BP+16]                         ; Push PFNTHREAD parameter 2 segment
        PUSH    [BP+14]                         ; Push PFNTHREAD parameter 2 offset
        PUSH    [BP+12]                         ; Push PTID16 parameter 3 segment
        PUSH    [BP+10]                         ; Push PTID16 parameter 3 offset
        PUSH    [BP+ 8]                         ; Push PUSHORT16 parameter 4 segment
        PUSH    [BP+ 6]                         ; Push PUSHORT16 parameter 4 offset
        ; Pass pointer to parameters so that user hook can modify
        PUSH    SS                              ; Push parameter 5 (API parms) segment
        MOV     AX, BP                          ; AX = Current BP
        ADD     AX, 6                           ; AX = API parameters offset
        PUSH    AX                              ; Push parameter 5 (API parms) offset
        CALL    C_PREDOS16CREATETHREAD          ; Call C pre-trace API

        MOV     AX, [BP- 4]                     ; Reload AX for API
        MOV     BX, [BP- 6]                     ; Reload BX for API
        MOV     CX, [BP- 8]                     ; Reload CX for API
        MOV     DX, [BP-10]                     ; Reload DX for API
        MOV     SI, [BP-12]                     ; Reload SI for API
        MOV     DI, [BP-14]                     ; Reload DI for API
        PUSH    [BP-2]                          ; Save flags register
        POPF                                    ; Reload flags register for API
        PUSH    [BP+16]                         ; Push PUSHORT16 parameter 3 segment
        PUSH    [BP+14]                         ; Push PUSHORT16 parameter 3 offset
        PUSH    [BP+12]                         ; Push PTID16 parameter 2 segment
        PUSH    [BP+10]                         ; Push PTID16 parameter 2 offset
        PUSH    [BP+ 8]                         ; Push PFNTHREAD parameter 1 segment
        PUSH    [BP+ 6]                         ; Push PFNTHREAD parameter 1 offset
        CALL    DOSCREATETHREAD                 ; Call API

        PUSH    AX                              ; Save return code
        ; Pass pointer to return code so that user hook can modify
        PUSH    SS                              ; Push parameter 1 (return code) segment
        MOV     AX, SP                          ; AX = Current BP
        ADD     AX, 2                           ; AX = APIRET16 offset
        PUSH    AX                              ; Push parameter 1 (return code) offset
        PUSH    [BP+12]                         ; Push PTID16 parameter 2 segment
        PUSH    [BP+10]                         ; Push PTID16 parameter 2 offset
        ; Pass pointer to parameters so that user hook can modify
        PUSH    SS                              ; Push parameter 3 (API parms) segment
        MOV     AX, BP                          ; AX = Current BP
        ADD     AX, 6                           ; AX = API parameters offset
        PUSH    AX                              ; Push parameter 3 (API parms) offset
        CALL    C_POSTDOS16CREATETHREAD         ; Call C post-trace API

        POP     AX                              ; Restore return code

        POP     DI                              ; Restore DI
        POP     SI                              ; Restore SI
        POP     DX                              ; Restore DX
        POP     CX                              ; Restore CX
        POP     BX                              ; Restore BX
        ADD     SP, 2                           ; Don't restore AX
        POPF                                    ; Restore flags register

        MOV     SP, BP                          ; SP = New base pointer
        POP     BP                              ; Restore old base pointer
        RETF    12
TRC_DOS16CREATETHREAD   ENDP

CODE16  ENDS
ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Trc_Dos32UnwindException()                                                  ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        PUBLIC  Trc_Dos32UnwindException

        ASSUME  CS:FLAT, DS:FLAT
CODE32  SEGMENT
        ALIGN   04H

Trc_Dos32UnwindException        PROC
        PUSH    EBP                             ; Save old base pointer
        MOV     EBP, ESP                        ; EBP = New base pointer
        PUSHFD                                  ; Save flags register
        PUSHAD                                  ; Save all general registers

IFDEF T_MINSTACK

;  if ( LOGDOSAPI( D_XCPT ) )
;  {
        TEST    T_DosAPIGrp, D_XCPT
        JZ      SHORT NotD_XCPT

;     T_WaitLog( );
        CALL    T_WaitLog

;     T_LogAPIEntry( szDos32UnwindException );
        MOV     EAX, OFFSET FLAT:szDos32UnwindException
        call    T_LogAPIEntry

;     if ( LOGLEVEL( L_PARMS ) )
;     {
        CMP     T_TraceLvl, L_PARMS
        JC      SHORT NotL_PARMS

;        T_LogParameter( 1L, DT_PEXCEPTIONREGISTRATIONRECORD );
        MOV     EDX, DT_PEXCEPTIONREGISTRATIONRECORD
        MOV     EAX, 1
        CALL    T_LogParameter

;        T_LogPVOID( Parm1 );
        MOV     EAX, DWORD PTR [EBP+8]
        CALL    T_LogPVOID

;        T_LogParameter( 2L, DT_PVOID );
        MOV     EDX, DT_PVOID
        MOV     EAX, 2
        CALL    T_LogParameter

;        T_LogPFN( (PFN)Parm2 );
        MOV     EAX, DWORD PTR [EBP+12]
        CALL    T_LogPFN

;        T_LogParameter( 3L, DT_PEXCEPTIONREPORTRECORD );
        MOV     EDX, DT_PEXCEPTIONREPORTRECORD
        MOV     EAX, 3
        CALL    T_LogParameter

;        T_LogPEXCEPTIONREPORTRECORD( Parm3 );
        MOV     EAX, DWORD PTR [EBP+16]
        CALL    T_LogPEXCEPTIONREPORTRECORD

;     }
NotL_PARMS:

;     HOOKENTRY( szDos32SetExceptionHandler, &Parm1 );
IFDEF T_USERHOOK
        CMP     T_UserHook, 0
        JE      SHORT NoUserHook
        LEA     EDX, [EBP+08H]
        MOV     EAX, OFFSET FLAT:szDos32UnwindException
        CALL    T_HookEntry
NoUserHook:
ENDIF

;     T_FreeLog( );
        CALL    T_FreeLog

;  }
NotD_XCPT:

ELSE

;  if ( LOGDOSAPI( D_XCPT ) )
;  {
        TEST    T_DosAPIGrp, D_XCPT
        JZ      SHORT NotD_XCPT

;     T_WaitLog( );
        CALL    T_WaitLog

;     T_LogAPIEntry( szDos32UnwindException );
        PUSH    OFFSET FLAT:szDos32UnwindException
        CALL    T_LogAPIEntry
        ADD     ESP, 4

;     if ( LOGLEVEL( L_PARMS ) )
;     {
        CMP     T_TraceLvl, L_PARMS
        JC      SHORT NotL_PARMS

;        T_LogParameter( 1L, DT_PEXCEPTIONREGISTRATIONRECORD );
        PUSH    DT_PEXCEPTIONREGISTRATIONRECORD
        PUSH    1
        CALL    T_LogParameter
        ADD     ESP, 8

;        T_LogPVOID( Parm1 );
        PUSH    DWORD PTR [EBP+8]
        CALL    T_LogPVOID
        ADD     ESP, 4

;        T_LogParameter( 2L, DT_PVOID );
        PUSH    DT_PVOID
        PUSH    2
        CALL    T_LogParameter
        ADD     ESP, 8

;        T_LogPFN( (PFN)Parm2 );
        PUSH    DWORD PTR [EBP+12]
        CALL    T_LogPFN
        ADD     ESP, 4

;        T_LogParameter( 3L, DT_PEXCEPTIONREPORTRECORD );
        PUSH    DT_PEXCEPTIONREPORTRECORD
        PUSH    3
        CALL    T_LogParameter
        ADD     ESP, 8

;        T_LogPEXCEPTIONREPORTRECORD( Parm3 );
        PUSH    DWORD PTR [EBP+16]
        CALL    T_LogPEXCEPTIONREPORTRECORD
        ADD     ESP, 4

;     }
NotL_PARMS:

;     HOOKENTRY( szDos32SetExceptionHandler, &Parm1 );
IFDEF T_USERHOOK
        CMP     T_UserHook, 0
        JE      SHORT NoUserHook
        LEA     EAX, [EBP+8]
        PUSH    EAX
        PUSH    OFFSET FLAT:szDos32UnwindException
        CALL    T_HookEntry
        ADD     ESP, 8
NoUserHook:
ENDIF

;     T_FreeLog( );
        CALL    T_FreeLog

;  }
NotD_XCPT:

ENDIF

        POPAD                                   ; Restore all general registers
        POPFD                                   ; Restore flags register
        MOV     ESP, EBP                        ; ESP = New base pointer
        POP     EBP                             ; Restore old base pointer

        JMP     DOS32UNWINDEXCEPTION            ; Jump to OS/2 API
Trc_Dos32UnwindException        ENDP

CODE32  ENDS
END

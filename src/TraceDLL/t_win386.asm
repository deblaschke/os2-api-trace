;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  File name    : T_WIN386.ASM                                                 ;
;                                                                              ;
;  Title        : OS/2 API trace DLL PMWIN assembly routines                   ;
;                                                                              ;
;  Author       : Dave Blaschke                                                ;
;                 deblaschke@yahoo.com                                         ;
;                                                                              ;
;  Description  : PMWIN     assembly     routines:       Trc_Win16Catch    and ;
;                 Trc_Win16SetErrorInfo.                                       ;
;                                                                              ;
;  History      : Version Date    Item                                         ;
;                 2.40.26 24Jun97 Added 16-bit Win APIs                        ;
;                 2.40.74 27Oct98 Added WinSetErrorInfo                        ;
;                 2.45.31 26Jun01 Added  capability  of building  32-bit  only ;
;                                 trace DLLs                                   ;
;                 2.45.36 03Dec03 Fixed WinSetErrorInfo user hook bug          ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .386P

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  External function declarations                                              ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFDEF T_16BIT
EXTRN   C_POSTWIN16SETERRORINFO:FAR
ENDIF
EXTRN   C_PostWin32SetErrorInfo:NEAR
IFDEF T_16BIT
EXTRN   C_PREWIN16SETERRORINFO:FAR
ENDIF
EXTRN   C_PreWin32SetErrorInfo:NEAR
IFDEF T_16BIT
EXTRN   C_WIN16CATCH:FAR
EXTRN   _WINSETERRORINFO:FAR
ENDIF
EXTRN   WIN32SETERRORINFO:NEAR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Structure declaration                                                       ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CatchBuf        STRUC                           ; From pmwin\applsup.asm
cb_ret  DD      ?
cb_bp   DW      ?
cb_sp   DW      ?
cb_si   DW      ?
cb_di   DW      ?
cb_hds  DW      ?
cb_hss  DW      ?
CatchBuf        ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Segment definitions                                                         ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE32  SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32  ENDS
IFDEF T_16BIT
CODE16  SEGMENT WORD USE16 PUBLIC 'CODE'
CODE16  ENDS
ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Trc_Win16Catch()                                                            ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFDEF T_16BIT
        PUBLIC  TRC_WIN16CATCH

        ASSUME  CS:CODE16
CODE16  SEGMENT
        ALIGN   04H

TRC_WIN16CATCH  PROC FAR
        PUSH    BP                              ; Save old base pointer
        MOV     BP, SP                          ; BP = New base pointer

        PUSH    SS                              ; Save SS for local CatchBuf
        PUSH    DS                              ; Save DS for local CatchBuf
        PUSH    DI                              ; Save DI for local CatchBuf
        PUSH    SI                              ; Save SI for local CatchBuf
        MOV     AX, SP                          ; AX = Current SP
        ADD     AX, 8                           ; AX = SP for local CatchBuf
        PUSH    AX                              ; Save SP for local CatchBuf
        PUSH    WORD PTR [BP]                   ; Save BP for local CatchBuf
        PUSH    WORD PTR [BP+4]                 ; Save CS for local CatchBuf
        PUSH    WORD PTR [BP+2]                 ; Save IP for local CatchBuf
        PUSH    WORD PTR [BP+8]                 ; Push parameter 1 (user CatchBuf) segment
        PUSH    WORD PTR [BP+6]                 ; Push parameter 1 (user CatchBuf) offset
        PUSH    SS                              ; Push parameter 2 (local CatchBuf) segment
        MOV     AX, SP                          ; AX = Current SP
        ADD     AX, 6                           ; AX = Local CatchBuf offset
        PUSH    AX                              ; Push parameter 2 (local CatchBuf) offset
        PUSH    WORD PTR [BP]                   ; Push old base pointer
        POP     BP                              ; Restore old base pointer
        CALL    C_WIN16CATCH                    ; Call C trace API
        ADD     SP, TYPE CatchBuf               ; Remove local CatchBuf

        POP     BP                              ; Restore old base pointer
        RET     4
TRC_WIN16CATCH  ENDP

CODE16  ENDS
ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Trc_Win16SetErrorInfo()                                                     ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFDEF T_16BIT
        PUBLIC  TRC_WIN16SETERRORINFO

        ASSUME  CS:CODE16
CODE16  SEGMENT
        ALIGN   04H

TRC_WIN16SETERRORINFO   PROC FAR
        PUSH    BP                              ; Save old base pointer
        MOV     BP, SP                          ; BP = New base pointer

        PUSHF                                   ; Save flags register
        PUSH    AX                              ; Save AX
        PUSH    BX                              ; Save BX
        PUSH    CX                              ; Save CX
        PUSH    DX                              ; Save DX
        PUSH    SI                              ; Save SI
        PUSH    DI                              ; Save DI

        SUB     SP, 2                           ; Make space for local variable

        PUSH    [BP+ 4]                         ; Push parameter 1 (API caller) segment
        PUSH    [BP+ 2]                         ; Push parameter 1 (API caller) offset
        PUSH    [BP+ 8]                         ; Push ERRORID parameter 2 severity
        PUSH    [BP+ 6]                         ; Push ERRORID parameter 2 error code
        PUSH    [BP+10]                         ; Push USHORT parameter 3
        MOV     AX, BP                          ; AX = New base pointer
        ADD     AX, 12                          ; AX = Variable args pointer
        PUSH    SS                              ; Push parameter 4 (variable args) segment
        PUSH    AX                              ; Push parameter 4 (variable args) offset
        ; Pass pointer to parameters so that user hook can modify
        PUSH    SS                              ; Push parameter 5 (API parms) segment
        SUB     AX, 6                           ; AX = API parameters offset
        PUSH    AX                              ; Push parameter 5 (API parms) offset
        CALL    C_PREWIN16SETERRORINFO          ; Call C pre-trace API

        MOV     [BP-16], AX                     ; Save size of API parms
        SUB     SP, AX                          ; Make space for API parms

        MOV     CX, AX                          ; CX = Size of API parms
        SHR     CX, 1                           ; CX = Num of API parm WORDs
        MOV     SI, BP                          ; SI = New base pointer
        ADD     SI, 6                           ; SI = Parms pointer
        MOV     DI, SP                          ; DI = Local API parms
        REP     MOVSW                           ; Copy parms locally

        MOV     AX, [BP- 4]                     ; Reload AX for API
        MOV     BX, [BP- 6]                     ; Reload BX for API
        MOV     CX, [BP- 8]                     ; Reload CX for API
        MOV     DX, [BP-10]                     ; Reload DX for API
        MOV     SI, [BP-12]                     ; Reload SI for API
        MOV     DI, [BP-14]                     ; Reload DI for API
        PUSH    [BP-2]                          ; Save flags register
        POPF                                    ; Reload flags register for API
        CALL    _WINSETERRORINFO                ; Call API

        PUSH    DX                              ; Save ERRORID severity
        PUSH    AX                              ; Save ERRORID error code
        ; Pass pointer to return code so that user hook can modify
        PUSH    SS                              ; Push parameter 1 (return code) segment
        MOV     AX, SP                          ; AX = Current SP
        ADD     AX, 2                           ; AX = ERRORID offset
        PUSH    AX                              ; Push parameter 1 (return code) offset
        ; Pass pointer to parameters so that user hook can modify
        PUSH    SS                              ; Push parameter 2 (API parms) segment
        MOV     AX, BP                          ; AX = Current BP
        ADD     AX, 6                           ; AX = API parms offset
        PUSH    AX                              ; Push parameter 2 (API parms) offset
        CALL    C_POSTWIN16SETERRORINFO         ; Call C post-trace API
        POP     AX                              ; Restore ERRORID error code
        POP     DX                              ; Restore ERRORID severity
        MOV     [BP-10], DX                     ; Save ERRORID severity for exit
        MOV     [BP- 4], AX                     ; Save ERRORID error code for exit

        MOV     AX, [BP-16]                     ; AX = Size of API parms
        ADD     SP, AX                          ; Clean off all API parms

        ADD     SP, 2                           ; Clean off local variable

        POP     DI                              ; Restore DI
        POP     SI                              ; Restore SI
        POP     DX                              ; Restore DX
        POP     CX                              ; Restore CX
        POP     BX                              ; Restore BX
        POP     AX                              ; Restore AX
        POPF                                    ; Restore flags register

        MOV     SP, BP                          ; SP = New base pointer
        POP     BP                              ; Restore old base pointer
        RETF
TRC_WIN16SETERRORINFO   ENDP

CODE16  ENDS
ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Trc_Win32SetErrorInfo()                                                     ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        PUBLIC  Trc_Win32SetErrorInfo

        ASSUME  CS:FLAT, DS:FLAT, ES:FLAT
CODE32  SEGMENT
        ALIGN   04H

Trc_Win32SetErrorInfo   PROC
        PUSH    EBP                             ; Save old base pointer
        MOV     EBP, ESP                        ; EBP = New base pointer

        PUSHFD                                  ; Save flags register
        PUSH    EAX                             ; Save EAX
        PUSH    EBX                             ; Save EBX
        PUSH    ECX                             ; Save ECX
        PUSH    EDX                             ; Save EDX
        PUSH    ESI                             ; Save ESI
        PUSH    EDI                             ; Save EDI

        SUB     ESP, 4                          ; Make space for local variable

        MOV     EAX, EBP                        ; EAX = New base pointer
        ADD     EAX, 8                          ; EAX = API parameters pointer
        PUSH    EAX                             ; Push parameter 5 (API parms)
        ; Pass pointer to parameters so that user hook can modify
        ADD     EAX, 8                          ; EAX = Variable args pointer
        PUSH    EAX                             ; Push parameter 4 (variable args)
        PUSH    [EBP+12]                        ; Push ULONG parameter 3
        PUSH    [EBP+ 8]                        ; Push ERRORID parameter 2
        PUSH    [EBP+ 4]                        ; Push parameter 1 (API caller)
        CALL    C_PreWin32SetErrorInfo          ; Call C pre-trace API
        ADD     ESP, 20                         ; Clean off all parms

        MOV     [EBP-32], EAX                   ; Save size of API parms
        SUB     ESP, EAX                        ; Make space for API parms

        MOV     ECX, EAX                        ; ECX = Size of API parms
        SHR     ECX, 2                          ; ECX = Num of API parm DWORDs
        MOV     ESI, EBP                        ; ESI = New base pointer
        ADD     ESI, 8                          ; ESI = Parms pointer
        MOV     EDI, ESP                        ; EDI = Local API parms
        REP     MOVSD                           ; Copy parms locally

        MOV     EAX, [EBP- 8]                   ; Reload EAX for API
        MOV     EBX, [EBP-12]                   ; Reload EBX for API
        MOV     ECX, [EBP-16]                   ; Reload ECX for API
        MOV     EDX, [EBP-20]                   ; Reload EDX for API
        MOV     ESI, [EBP-24]                   ; Reload ESI for API
        MOV     EDI, [EBP-28]                   ; Reload EDI for API
        PUSH    [EBP-4]                         ; Save flags register
        POPFD                                   ; Reload flags register for API
        CALL    WIN32SETERRORINFO               ; Call API

        PUSH    EAX                             ; Save ERRORID
        ; Pass pointer to parameters so that user hook can modify
        MOV     EAX, EBP                        ; EAX = Current EBP
        ADD     EAX, 8                          ; EAX = API parms pointer
        PUSH    EAX                             ; Push parameter 2 (API parms)
        ; Pass pointer to return code so that user hook can modify
        MOV     EAX, ESP                        ; EAX = Current ESP
        ADD     EAX, 4                          ; EAX = ERRORID pointer
        PUSH    EAX                             ; Push parameter 1 (return code)
        CALL    C_PostWin32SetErrorInfo         ; Call C post-trace API
        ADD     ESP, 8                          ; Clean off all parms
        POP     EAX                             ; Restore ERRORID
        MOV     [EBP-8], EAX                    ; Save ERRORID for exit

        MOV     EAX, [EBP-32]                   ; EAX = Size of API parms
        ADD     ESP, EAX                        ; Clean off all API parms

        ADD     ESP, 4                          ; Clean off local variable

        POP     EDI                             ; Restore EDI
        POP     ESI                             ; Restore ESI
        POP     EDX                             ; Restore EDX
        POP     ECX                             ; Restore ECX
        POP     EBX                             ; Restore EBX
        POP     EAX                             ; Restore EAX
        POPFD                                   ; Restore flags register

        MOV     ESP, EBP                        ; ESP = New base pointer
        POP     EBP                             ; Restore old base pointer
        RET
Trc_Win32SetErrorInfo   ENDP

CODE32  ENDS

END

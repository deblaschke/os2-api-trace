;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  File name    : T_M__386.ASM                                                 ;
;                                                                              ;
;  Title        : OS/2 API trace DLL MSG assembly routines                     ;
;                                                                              ;
;  Author       : Dave Blaschke                                                ;
;                 deblaschke@yahoo.com                                         ;
;                                                                              ;
;  Description  : MSG     assembly    routines:        A_DOS32IQUERYMESSAGECP, ;
;                 A_DOS32TRUEGETMESSAGE,        Trc_Dos32GetMessage,       and ;
;                 Trc_Dos32QueryMessageCP.                                     ;
;                                                                              ;
;  History      : Version Date    Item                                         ;
;                 2.30.00 12Jun95 Created (supported LX format  and 32-bit Dos ;
;                                 APIs only)                                   ;
;                 2.30.12 25Aug95 Fixed Dos32QueryMessageCP bug                ;
;                 2.30.13 27Aug95 Added OS/2 for PowerPC support               ;
;                 2.40.73 26Oct98 Fixed    Dos32GetMessage/Dos32QueryMessageCP ;
;                                 bug                                          ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .386P

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  External function declarations                                              ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

EXTRN   C_Dos32GetMessage:NEAR
EXTRN   C_Dos32QueryMessageCP:NEAR
EXTRN   DOS32IQUERYMESSAGECP:NEAR
EXTRN   DOS32TRUEGETMESSAGE:NEAR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Segment definition                                                          ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE32  SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32  ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  A_DOS32IQUERYMESSAGECP()                                                    ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        PUBLIC  A_DOS32IQUERYMESSAGECP

        ASSUME  CS:FLAT
CODE32  SEGMENT
        ALIGN   04H

A_DOS32IQUERYMESSAGECP  PROC
        PUSH    EBP                             ; Save old base pointer
        MOV     EBP, ESP                        ; EBP = New base pointer

        PUSH    [EBP+24]                        ; Push PVOID parameter 5
        PUSH    [EBP+20]                        ; Push PULONG parameter 4
        PUSH    [EBP+16]                        ; Push PSZ parameter 3
        PUSH    [EBP+12]                        ; Push ULONG parameter 2
        PUSH    [EBP+ 8]                        ; Push PCHAR parameter 1
        CALL    DOS32IQUERYMESSAGECP            ; Call OS/2 API
        ADD     ESP, 16                         ; Clean off all parms but one

        MOV     ESP, EBP                        ; ESP = New base pointer
        POP     EBP                             ; Restore old base pointer
        RET
A_DOS32IQUERYMESSAGECP  ENDP

CODE32  ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  A_DOS32TRUEGETMESSAGE()                                                     ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        PUBLIC  A_DOS32TRUEGETMESSAGE

        ASSUME  CS:FLAT
CODE32  SEGMENT
        ALIGN   04H

A_DOS32TRUEGETMESSAGE   PROC
        PUSH    EBP                             ; Save old base pointer
        MOV     EBP, ESP                        ; EBP = New base pointer

        PUSH    [EBP+36]                        ; Push PULONG parameter 8
        PUSH    [EBP+32]                        ; Push PSZ parameter 7
        PUSH    [EBP+28]                        ; Push ULONG parameter 6
        PUSH    [EBP+24]                        ; Push ULONG parameter 5
        PUSH    [EBP+20]                        ; Push PCHAR parameter 4
        PUSH    [EBP+16]                        ; Push ULONG parameter 3
        PUSH    [EBP+12]                        ; Push PCHAR * parameter 2
        PUSH    [EBP+ 8]                        ; Push PVOID parameter 1
        CALL    DOS32TRUEGETMESSAGE             ; Call OS/2 API
        ADD     ESP, 28                         ; Clean off all parms but one

        MOV     ESP, EBP                        ; ESP = New base pointer
        POP     EBP                             ; Restore old base pointer
        RET
A_DOS32TRUEGETMESSAGE   ENDP

CODE32  ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Trc_Dos32GetMessage()                                                       ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        PUBLIC  Trc_Dos32GetMessage

        ASSUME  CS:FLAT
CODE32  SEGMENT
        ALIGN   04H

Trc_Dos32GetMessage     PROC
        PUSH    EBP                             ; Save old base pointer
        MOV     EBP, ESP                        ; EBP = New base pointer

        PUSH    [EBP+36]                        ; Push PULONG parameter 9
        PUSH    [EBP+32]                        ; Push PSZ parameter 8
        PUSH    [EBP+28]                        ; Push ULONG parameter 7
        PUSH    [EBP+24]                        ; Push ULONG parameter 6
        PUSH    [EBP+20]                        ; Push PCHAR parameter 5
        PUSH    [EBP+16]                        ; Push ULONG parameter 4
        PUSH    [EBP+12]                        ; Push PCHAR * parameter 3
        PUSH    [EBP+ 8]                        ; Push PVOID parameter 2
        PUSH    [EBP+ 4]                        ; Push ULONG parameter 1
        CALL    C_Dos32GetMessage               ; Call C trace API
        ADD     ESP, 36                         ; Clean off all parms

        MOV     ESP, EBP                        ; ESP = New base pointer
        POP     EBP                             ; Restore old base pointer
        RET     4                               ; Clean off message segment ptr
Trc_Dos32GetMessage     ENDP

CODE32  ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                              ;
;  Trc_Dos32QueryMessageCP()                                                   ;
;                                                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        PUBLIC  Trc_Dos32QueryMessageCP

        ASSUME  CS:FLAT
CODE32  SEGMENT
        ALIGN   04H

Trc_Dos32QueryMessageCP PROC
        PUSH    EBP                             ; Save old base pointer
        MOV     EBP, ESP                        ; EBP = New base pointer

        PUSH    [EBP+24]                        ; Push PVOID parameter 6
        PUSH    [EBP+20]                        ; Push PULONG parameter 5
        PUSH    [EBP+16]                        ; Push PSZ parameter 4
        PUSH    [EBP+12]                        ; Push ULONG parameter 3
        PUSH    [EBP+ 8]                        ; Push PCHAR parameter 2
        PUSH    [EBP+ 4]                        ; Push ULONG parameter 1
        CALL    C_Dos32QueryMessageCP           ; Call C trace API
        ADD     ESP, 24                         ; Clean off all parms

        MOV     ESP, EBP                        ; ESP = New base pointer
        POP     EBP                             ; Restore old base pointer
        RET     4                               ; Clean off message segment ptr
Trc_Dos32QueryMessageCP ENDP

CODE32  ENDS
END

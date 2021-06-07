
;; This is an add-on for gbdk's rand.s

    .area   _BSS

    ; alternate state vars so that two sequences can be run in parallel
.randhi_b::
    .ds 0x01
.randlo_b::
    .ds 0x01
.randtemp::
    .ds 0x01


    .area   _GSINIT

    ;; Clear swaprand global variables
    XOR A
    LD  (#.randhi_b),A
    LD  (#.randlo_b),A
    LD  (#.randtemp),A


    .area   _BASE
    ;; Swap random number state vars to the alternate set
_swaprand::         ; Non banked
                ; Low Byte
    LD  A,(.randlo) ; Primary -> Temp
    LD  (.randtemp),A
    LD  A,(.randlo_b)   ; Secondary -> Primary
    LD  (.randlo),A
    LD  A,(.randtemp)   ; Temp -> Secondary
    LD  (.randlo_b),A
                ; Hi byte
    LD  A,(.randhi) ; Primary -> Temp
    LD  (.randtemp),A
    LD  A,(.randhi_b)   ; Secondary -> Primary
    LD  (.randhi),A
    LD  A,(.randtemp)   ; Temp -> Secondary
    LD  (.randhi_b),A
    RET

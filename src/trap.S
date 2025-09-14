.text
.globl traps
.extern internal_handler
.extern timer_handler
.extern external_handler


.balign 4
traps:
    j internal
    j timer
.skip 28
    j external

# make a macro for pushAll and popAll
.macro pushAll
    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr
.endm

.macro popAll
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256
.endm

internal:
    pushAll

    addi fp, sp, 256
    call internal_handler

    popAll
    sret

timer:
    pushAll

    addi fp, sp, 256
    call timer_handler

    popAll
    sret

external:
    pushAll

    addi fp, sp, 256
    call external_handler

    popAll
    sret

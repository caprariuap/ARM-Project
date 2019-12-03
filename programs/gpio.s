ldr r0,=0x20200000
ldr r2, [r0, #4]
mov r1, #1
lsl r1, #18
orr r1, r1, r2
str r1, [r0, #4]
mov r2, #1
lsl r2, #21
mov r1, #1
lsl r1, #16
str r1, [r0, #40]
loop:
sub r2, r2, #1
cmp r2, #0
bne loop
str r1, [r0, #28]
mov r2, #1
lsl r2, #21
loop2:
sub r2, r2, #1
cmp r2, #0
bne loop2
andeq r0, r0, r0

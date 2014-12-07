
ldr r0,=0x20200000
mov r1,#1
lsl r1,#18
str r1,[r0,#4]

mov r1,#1
lsl r1,#16

mov r2,#20
off:

str r1,[r0,#40]

mov r3,#0x500000
offtime:
sub r3,#1
cmp r3,#0
bne offtime

str r1,[r0,#28]

mov r3,#0x500000
ontime:
sub r3,#1
cmp r3,#0
bne ontime

sub r2,#1
cmp r2,#0
bne off

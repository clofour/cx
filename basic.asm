bits 64
default rel
segment .data
msg db 'Hello World!', 0xd, 0xa, 0
segment .text
global main
extern printf
extern ExitProcess
main:
push rbp
mov rbp, rsp
sub rsp, 32
msg db 'Hello World!', 0xd, 0xa, 0
xor rax, rax
call ExitProcess

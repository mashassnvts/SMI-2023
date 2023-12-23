.586P
.model flat, stdcall
includelib libucrt.lib
includelib kernel32.lib
includelib ..\Debug\Library.lib
ExitProcess PROTO :DWORD

EXTRN CONCAT: proc
EXTRN LINELEN: proc
EXTRN RANDOM: proc
EXTRN SQRT: proc
EXTRN OutNumber: proc
EXTRN OutLine: proc
EXTRN OutBool: proc
EXTRN OutNumberLn: proc
EXTRN OutLineLn: proc
EXTRN OutBoolLn: proc

.stack 4096

.const
	null_division BYTE "Exception: деление на ноль", 0
	lit1	SWORD 2
	lit2	SWORD 3
	lit3	BYTE "Смена логина проведена успешно!", 0
	lit4	BYTE "Ваш новый логин: ", 0
	lit5	BYTE "Результат: ", 0
	lit6	SWORD 6
	lit7	BYTE "Результат вычисления выражения отрицательный? - ", 0
	lit8	SWORD -8
	lit9	SWORD 5
	lit10	SWORD 0
	lit11	BYTE "Результат был: ", 0
	lit12	BYTE "Ivanov Ivan", 0
	lit13	BYTE "Ваш баланс (в $): ", 0
	lit14	SWORD 10
	lit15	SWORD 1
	lit16	BYTE "Квадратный корень из 10 равен 4? - ", 0
	lit17	SWORD 4
	lit18	BYTE "Да", 0
	lit19	BYTE "Нет", 0
	lit20	BYTE "Длина строки равна 0? - ", 0
	lit21	BYTE "abcdef", 0
	lit22	BYTE "Hello, ", 0
	lit23	BYTE "it's SMI-2023!", 0

.data
	buffer BYTE 256 dup(0)
	power_i	SWORD 0
	power_res	SWORD 0
	program_resPow	SWORD 0
	program_expr	SWORD 0
	program_resNeg	SWORD 0
	program_login	DWORD ?
	program_balance	SWORD 0
	program_resSqrt	SWORD 0
	program_lineA	DWORD ?
	program_lengthA	SWORD 0
	program_resA	SWORD 0
	program_resConcat	DWORD ?

.code

power PROC power_a : SWORD, power_b : SWORD
	push lit1
	pop power_i
	push power_a
	push lit2
	pop ax
	pop bx
	mul bx
	push ax
	push power_b
	push lit1
	pop ax
	pop bx
	add ax, bx
	push ax
	pop bx
	pop ax
	sub ax, bx
	push ax
	pop power_res
	push power_res
	jmp local0
local0:
	pop eax
	ret
power ENDP

yourLogin PROC yourLogin_login : DWORD
	push offset lit3
	call OutLineLn
	push offset lit4
	call OutLine
	push yourLogin_login
	call OutLineLn
local1:
	pop eax
	ret
	ret
yourLogin ENDP

program PROC
	push offset lit5
	call OutLine
	push lit6
	push lit2
	pop dx
	pop dx
	movsx eax, lit2
	push eax
	movsx eax, lit6
	push eax
	call power
	push ax
	pop program_resPow
	movsx eax, program_resPow
	push eax
	call OutNumberLn
	push offset lit7
	call OutLine
	push lit8
	push lit1
	pop bx
	pop ax
	cmp bx, 0
	je nulldiv
	cwd
	idiv bx
	push ax
	push lit9
	push lit1
	pop bx
	pop ax
	cmp bx, 0
	je nulldiv
	cwd
	idiv bx
	push dx
	pop bx
	pop ax
	sub ax, bx
	push ax
	pop program_expr
	push program_expr
	push lit10
	pop bx
	pop ax
	cmp ax, bx
	jle l0
	jg l1
l0:
	mov ax, 1
	push ax
	jmp endofexpr0
l1:
	mov ax, 0
	push ax

endofexpr0:
	pop ax
	cmp ax, 0
	je l2
	jne l3
l2:
	mov ax, 0
	push ax
	jmp endofexpr1
l3:
	mov ax, 1
	push ax

endofexpr1:
	pop program_resNeg
	movsx eax, program_resNeg
	push eax
	call OutBoolLn
	push offset lit11
	call OutLine
	movsx eax, program_expr
	push eax
	call OutNumberLn
	push offset lit12
	pop program_login
	push program_login
	call yourLogin
	push offset lit13
	call OutLine
	push lit14
	push lit15
	pop dx
	pop dx
	movsx eax, lit15
	push eax
	movsx eax, lit14
	push eax
	call RANDOM
	push ax
	push lit9
	pop ax
	pop bx
	add ax, bx
	push ax
	pop program_balance
	movsx eax, program_balance
	push eax
	call OutNumberLn
	push offset lit16
	call OutLine
	push lit14
	pop dx
	movsx eax, lit14
	push eax
	call SQRT
	push ax
	pop program_resSqrt
	mov ax, program_resSqrt
	cmp ax, lit17
	je m0
	jne m1
m0:
	push offset lit18
	call OutLineLn
	jmp e0
m1:
	push offset lit19
	call OutLineLn
e0:
	push offset lit20
	call OutLine
	push offset lit21
	pop program_lineA
	push program_lineA
	pop dx
	push program_lineA
	call LINELEN
	push ax
	pop program_lengthA
	push program_lengthA
	push lit10
	pop bx
	pop ax
	cmp ax, bx
	jne l4
	je l5
l4:
	mov ax, 1
	push ax
	jmp endofexpr2
l5:
	mov ax, 0
	push ax

endofexpr2:
	pop ax
	cmp ax, 0
	je l6
	jne l7
l6:
	mov ax, 0
	push ax
	jmp endofexpr3
l7:
	mov ax, 1
	push ax

endofexpr3:
	pop program_resA
	mov ax, program_resA
	cmp ax, 0
	jnz m2
	jz m3
m2:
	push offset lit19
	call OutLineLn
	jmp e1
m3:
	push offset lit18
	call OutLineLn
e1:
	push offset lit22
	push offset lit23
	pop dx
	pop dx
	push offset lit23
	push offset lit22
	push offset buffer
	call CONCAT
	push eax
	pop program_resConcat
	push program_resConcat
	call OutLineLn
theend:
	push 0
	call ExitProcess
nulldiv:
	push offset null_division
	call OutLineLn
	push -1
	call ExitProcess
program ENDP
end program
# 中断和异常处理函数的入口
# 中断处理函数会在IDT中为相应的中断/异常设置处理程序
# 中断/异常的行为参见i386手册
.globl vec0; vec0: pushl $0; jmp asm_do_irq
.globl vec1; vec1: pushl $0; pushl $1; jmp asm_do_irq
.globl vec2; vec2: pushl $0; pushl $2; jmp asm_do_irq
.globl vec3; vec3: pushl $0; pushl $3; jmp asm_do_irq
.globl vec4; vec4: pushl $0; pushl $4; jmp asm_do_irq
.globl vec5; vec5: pushl $0; pushl $5; jmp asm_do_irq
.globl vec6; vec6: pushl $0; pushl $6; jmp asm_do_irq
.globl vec7; vec7: pushl $0; pushl $7; jmp asm_do_irq
.globl vec8; vec8:           pushl $8; jmp asm_do_irq
.globl vec9; vec9: pushl $0; pushl $9; jmp asm_do_irq
.globl vec10; vec10:         pushl $10; jmp asm_do_irq
.globl vec11; vec11:         pushl $11; jmp asm_do_irq
.globl vec12; vec12:         pushl $12; jmp asm_do_irq
.globl vec13; vec13:         pushl $13; jmp asm_do_irq

.globl vecsys; vecsys: push $0; pushl $0x80; jmp asm_do_irq

.globl irq0; irq0: push $0; pushl $1000; jmp asm_do_irq
.globl irq1; irq1: push $0; pushl $1001; jmp asm_do_irq
.globl irq14; irq14: push $0; pushl $1014; jmp asm_do_irq

.globl irq_empty; irq_empty: pushl $-1; jmp asm_do_irq

# ！调用C编写的中断处理程序(见irq/irq_handle.c)！

#define SEG_KERNEL_CODE 1
#define SEG_KERNEL_DATA 2
#define DPL_KERNEL 0
#define SELECTOR_KERNEL(s)	( ((s) << 3) | DPL_KERNEL )

.globl asm_do_irq
.extern irq_handle
asm_do_irq:
#	subl $8,%esp
	cli
	pushl %ds
	pushl %es
	pushl %eax
	movw $(SELECTOR_KERNEL(SEG_KERNEL_DATA)), %ax
	movw %ax, %es
	movw %ax, %ds
	popl %eax
	pushal

	pushl %esp			# ???
	call irq_handle
	addl $4, %esp

	popal
	popl %es
	popl %ds
	addl $0x8, %esp
	sti
  	iret

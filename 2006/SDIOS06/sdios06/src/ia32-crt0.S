//
// File:  src/test/ia32-crt0.S
//
// Description: Simple startupcode
//

.text

	.global _start
_start:
	/* dont' load the stack pointer for our small stack any more! */

	/* save environment given from elfexec. the third param is at -8 because
	there is no return address on the stack. */ 
    mov 8(%esp), %ebx
	mov %ebx, environ
	
	/* initialize libc */
	call __libc_init

	/* Start the constructors of global objects (supporting C++) */
    leal __section_ctors_start, %ebx
2:
	cmp $__section_ctors_end, %ebx
    je 3f
	call *(%ebx)
	add $4,%ebx
	jmp 2b
3:

	/* push a return address to catch main ending (should never happen) */
	pushl	$___return_from_main
	jmp	main /* call main() */

___return_from_main:
	pushl	%eax				/* save return code as a parameter to exit */
		
	/* run the destructors of global objects (supporting C++) */
	leal __section_dtors_start, %ebx
5:
	cmp $__section_dtors_end, %ebx
	je 6f
	call *(%ebx)
	add $4,%ebx
	jmp 5b
6:

	/* Kill ourself */
	call exit

	/* Enter kdebug (this is what never should happen) 
	WARNING: only L4Ka::Pistachio KDebug enter */
        int     $3
        jmp     8f
        mov     $7f, %eax
	.section .rodata
7:      .asciz "***System stopped***"
	.previous
8:      jmp 6b /* Loop forever on the enter_kdebug */
	
.data
	/* save envp pointer to environment here */
	.global environ
environ:
	.long 0
		
	/* symbols which are expected to exist */
	.global __heap_start_ptr

__heap_start_ptr:
	.long __elf_end

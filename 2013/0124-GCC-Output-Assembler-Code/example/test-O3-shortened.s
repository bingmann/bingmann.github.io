[... header ...]
  56              		.p2align 4,,15
  57              	.globl some_function
  59              	some_function:
  60              	.LFB22:
  61              		.file 1 "test.c"
   1:test.c        **** #include <stdio.h>
   2:test.c        **** 
   3:test.c        **** int some_function(int a)
   4:test.c        **** {
  62              		.loc 1 4 0
  63              		.cfi_startproc
  64              	.LVL0:
   5:test.c        ****     a *= 5;     // a is not really needed.
   6:test.c        ****     return 42;
   7:test.c        **** }
  65              		.loc 1 7 0
  66 0000 B82A0000 		mov	eax, 42	#,
  66      00
  67 0005 C3       		ret
  68              		.cfi_endproc
  69              	.LFE22:
  71              		.section	.rodata.str1.1,"aMS",@progbits,1
  72              	.LC0:
  73 0000 54686973 		.string	"This is a test program."
  73      20697320 
  73      61207465 
  73      73742070 
  73      726F6772 
  74              		.text
  75 0006 662E0F1F 		.p2align 4,,15
  75      84000000 
  75      0000
  76              	.globl main
  78              	main:
  79              	.LFB23:
   8:test.c        **** 
   9:test.c        **** int main(int argc, char* argv[])
  10:test.c        **** {
  80              		.loc 1 10 0
  81              		.cfi_startproc
  82              	.LVL1:
  83 0010 4883EC08 		sub	rsp, 8	#,
  84              	.LCFI0:
  85              		.cfi_def_cfa_offset 16
  86              		.file 2 "/usr/include/bits/stdio2.h"
   1:/usr/include/bits/stdio2.h **** /* Checking macros for stdio functions. */
[... more lines from stdio2.h ...]
 102:/usr/include/bits/stdio2.h **** __extern_always_inline int
 103:/usr/include/bits/stdio2.h **** printf (__const char *__restrict __fmt, ...)
 104:/usr/include/bits/stdio2.h **** {
 105:/usr/include/bits/stdio2.h ****   return __printf_chk (__USE_FORTIFY_LEVEL - 1, __fmt, __va_arg_pack ());
  87              		.loc 2 105 0
  88 0014 BF000000 		mov	edi, OFFSET FLAT:.LC0	#,
  88      00
  89              	.LVL2:
  90 0019 E8000000 		call	puts	#
  90      00
  91              	.LVL3:
  11:test.c        ****     int variableA = 10;
  12:test.c        **** 
  13:test.c        ****     printf("This is a test program.\n");
  14:test.c        **** 
  15:test.c        ****     variableA += 5;
  16:test.c        ****     variableA += some_function(20);
  17:test.c        ****     
  18:test.c        ****     return variableA;
  19:test.c        **** }
  92              		.loc 1 19 0
  93 001e B8390000 		mov	eax, 57	#,
  93      00
  94 0023 4883C408 		add	rsp, 8	#,
  95              	.LCFI1:
  96              		.cfi_def_cfa_offset 8
  97 0027 C3       		ret
  98              		.cfi_endproc
  99              	.LFE23:
 101              	.Letext0:

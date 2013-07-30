   1              		.file	"test.c"
   2              		.intel_syntax noprefix
   3              	# GNU C (Gentoo 4.5.4 p1.1, pie-0.4.7) version 4.5.4 (x86_64-pc-linux-gnu)
   4              	#	compiled by GNU C version 4.5.4, GMP version 5.0.2, MPFR version 3.1.1, MPC version 0.8.2
   5              	# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
   6              	# options passed:  test.c -D_FORTIFY_SOURCE=2 -masm=intel -mtune=generic
   7              	# -march=x86-64 -g -fverbose-asm
   8              	# options enabled:  -falign-loops -fargument-alias
   9              	# -fasynchronous-unwind-tables -fauto-inc-dec -fbranch-count-reg -fcommon
  10              	# -fdelete-null-pointer-checks -fdwarf2-cfi-asm -fearly-inlining
  11              	# -feliminate-unused-debug-types -ffunction-cse -fgcse-lm -fident
  12              	# -finline-functions-called-once -fira-share-save-slots
  13              	# -fira-share-spill-slots -fivopts -fkeep-static-consts
  14              	# -fleading-underscore -fmath-errno -fmerge-debug-strings
  15              	# -fmove-loop-invariants -fpeephole -freg-struct-return
  16              	# -fsched-critical-path-heuristic -fsched-dep-count-heuristic
  17              	# -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
  18              	# -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
  19              	# -fsched-stalled-insns-dep -fshow-column -fsigned-zeros
  20              	# -fsplit-ivs-in-unroller -ftrapping-math -ftree-cselim -ftree-forwprop
  21              	# -ftree-loop-im -ftree-loop-ivcanon -ftree-loop-optimize
  22              	# -ftree-parallelize-loops= -ftree-phiprop -ftree-pta -ftree-reassoc
  23              	# -ftree-scev-cprop -ftree-slp-vectorize -ftree-vect-loop-version
  24              	# -funit-at-a-time -funwind-tables -fvect-cost-model -fverbose-asm
  25              	# -fzero-initialized-in-bss -m128bit-long-double -m64 -m80387
  26              	# -maccumulate-outgoing-args -malign-stringops -mfancy-math-387
  27              	# -mfp-ret-in-387 -mfused-madd -mglibc -mieee-fp -mmmx -mno-sse4
  28              	# -mpush-args -mred-zone -msse -msse2 -mtls-direct-seg-refs
  29              	
  37              	.Ltext0:
  38              	# Compiler executable checksum: 76a20b9f1070721afce61d667b1c5940
  39              	
  40              	.globl some_function
  42              	some_function:
  43              	.LFB0:
  44              		.file 1 "test.c"
   1:test.c        **** #include <stdio.h>
   2:test.c        **** 
   3:test.c        **** int some_function(int a)
   4:test.c        **** {
  45              		.loc 1 4 0
  46              		.cfi_startproc
  47 0000 55       		push	rbp	#
  48              	.LCFI0:
  49              		.cfi_def_cfa_offset 16
  50 0001 4889E5   		mov	rbp, rsp	#,
  51              		.cfi_offset 6, -16
  52              	.LCFI1:
  53              		.cfi_def_cfa_register 6
  54 0004 897DFC   		mov	DWORD PTR [rbp-4], edi	# a, a
   5:test.c        ****     a *= 5;     // a is not really needed.
  55              		.loc 1 5 0
  56 0007 8B55FC   		mov	edx, DWORD PTR [rbp-4]	# tmp60, a
  57 000a 89D0     		mov	eax, edx	# tmp61, tmp60
  58 000c C1E002   		sal	eax, 2	# tmp61,
  59 000f 01D0     		add	eax, edx	# tmp62, tmp60
  60 0011 8945FC   		mov	DWORD PTR [rbp-4], eax	# a, tmp62
   6:test.c        ****     return 42;
  61              		.loc 1 6 0
  62 0014 B82A0000 		mov	eax, 42	# D.3288,
  62      00
   7:test.c        **** }
  63              		.loc 1 7 0
  64 0019 C9       		leave
  65              	.LCFI2:
  66              		.cfi_def_cfa 7, 8
  67 001a C3       		ret
  68              		.cfi_endproc
  69              	.LFE0:
  71              		.section	.rodata
  72              	.LC0:
  73 0000 54686973 		.string	"This is a test program."
  73      20697320 
  73      61207465 
  73      73742070 
  73      726F6772 
  74              		.text
  75              	.globl main
  77              	main:
  78              	.LFB1:
   8:test.c        **** 
   9:test.c        **** int main(int argc, char* argv[])
  10:test.c        **** {
  79              		.loc 1 10 0
  80              		.cfi_startproc
  81 001b 55       		push	rbp	#
  82              	.LCFI3:
  83              		.cfi_def_cfa_offset 16
  84 001c 4889E5   		mov	rbp, rsp	#,
  85              		.cfi_offset 6, -16
  86              	.LCFI4:
  87              		.cfi_def_cfa_register 6
  88 001f 4883EC20 		sub	rsp, 32	#,
  89 0023 897DEC   		mov	DWORD PTR [rbp-20], edi	# argc, argc
  90 0026 488975E0 		mov	QWORD PTR [rbp-32], rsi	# argv, argv
  11:test.c        ****     int variableA = 10;
  91              		.loc 1 11 0
  92 002a C745FC0A 		mov	DWORD PTR [rbp-4], 10	# variableA,
  92      000000
  12:test.c        **** 
  13:test.c        ****     printf("This is a test program.\n");
  93              		.loc 1 13 0
  94 0031 BF000000 		mov	edi, OFFSET FLAT:.LC0	#,
  94      00
  95 0036 E8000000 		call	puts	#
  95      00
  14:test.c        **** 
  15:test.c        ****     variableA += 5;
  96              		.loc 1 15 0
  97 003b 8345FC05 		add	DWORD PTR [rbp-4], 5	# variableA,
  16:test.c        ****     variableA += some_function(20);
  98              		.loc 1 16 0
  99 003f BF140000 		mov	edi, 20	#,
  99      00
 100 0044 E8000000 		call	some_function	#
 100      00
 101 0049 0145FC   		add	DWORD PTR [rbp-4], eax	# variableA, D.3285
  17:test.c        ****     
  18:test.c        ****     return variableA;
 102              		.loc 1 18 0
 103 004c 8B45FC   		mov	eax, DWORD PTR [rbp-4]	# D.3286, variableA
  19:test.c        **** }
 104              		.loc 1 19 0
 105 004f C9       		leave
 106              	.LCFI5:
 107              		.cfi_def_cfa 7, 8
 108 0050 C3       		ret
 109              		.cfi_endproc
 110              	.LFE1:
 112              	.Letext0:

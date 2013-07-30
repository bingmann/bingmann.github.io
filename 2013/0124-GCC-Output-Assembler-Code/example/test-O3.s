   1              		.file	"test.c"
   2              		.intel_syntax noprefix
   3              	# GNU C (Gentoo 4.5.4 p1.1, pie-0.4.7) version 4.5.4 (x86_64-pc-linux-gnu)
   4              	#	compiled by GNU C version 4.5.4, GMP version 5.0.2, MPFR version 3.1.1, MPC version 0.8.2
   5              	# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
   6              	# options passed:  test.c -D_FORTIFY_SOURCE=2 -march=core2 -mcx16 -msahf
   7              	# -mpopcnt -msse4.2 --param l1-cache-size=32 --param l1-cache-line-size=64
   8              	# --param l2-cache-size=8192 -mtune=core2 -masm=intel -g -O3 -fverbose-asm
   9              	# options enabled:  -falign-loops -fargument-alias
  10              	# -fasynchronous-unwind-tables -fauto-inc-dec -fbranch-count-reg
  11              	# -fcaller-saves -fcommon -fcprop-registers -fcrossjumping
  12              	# -fcse-follow-jumps -fdefer-pop -fdelete-null-pointer-checks
  13              	# -fdwarf2-cfi-asm -fearly-inlining -feliminate-unused-debug-types
  14              	# -fexpensive-optimizations -fforward-propagate -ffunction-cse -fgcse
  15              	# -fgcse-after-reload -fgcse-lm -fguess-branch-probability -fident
  16              	# -fif-conversion -fif-conversion2 -findirect-inlining -finline
  17              	# -finline-functions -finline-functions-called-once
  18              	# -finline-small-functions -fipa-cp -fipa-cp-clone -fipa-pure-const
  19              	# -fipa-reference -fipa-sra -fira-share-save-slots -fira-share-spill-slots
  20              	# -fivopts -fkeep-static-consts -fleading-underscore -fmath-errno
  21              	# -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
  22              	# -fomit-frame-pointer -foptimize-register-move -foptimize-sibling-calls
  23              	# -fpeephole -fpeephole2 -fpredictive-commoning -freg-struct-return
  24              	# -fregmove -freorder-blocks -freorder-functions -frerun-cse-after-loop
  25              	# -fsched-critical-path-heuristic -fsched-dep-count-heuristic
  26              	# -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
  27              	# -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
  28              	# -fsched-stalled-insns-dep -fschedule-insns2 -fshow-column -fsigned-zeros
  29              	# -fsplit-ivs-in-unroller -fsplit-wide-types -fstrict-aliasing
  30              	# -fstrict-overflow -fthread-jumps -ftoplevel-reorder -ftrapping-math
  31              	# -ftree-builtin-call-dce -ftree-ccp -ftree-ch -ftree-copy-prop
  32              	# -ftree-copyrename -ftree-cselim -ftree-dce -ftree-dominator-opts
  33              	# -ftree-dse -ftree-forwprop -ftree-fre -ftree-loop-im -ftree-loop-ivcanon
  34              	# -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop -ftree-pre
  35              	# -ftree-pta -ftree-reassoc -ftree-scev-cprop -ftree-sink
  36              	# -ftree-slp-vectorize -ftree-sra -ftree-switch-conversion -ftree-ter
  37              	# -ftree-vect-loop-version -ftree-vectorize -ftree-vrp -funit-at-a-time
  38              	# -funswitch-loops -funwind-tables -fvar-tracking
  39              	# -fvar-tracking-assignments -fvect-cost-model -fverbose-asm
  40              	# -fzero-initialized-in-bss -m128bit-long-double -m64 -m80387
  41              	# -maccumulate-outgoing-args -malign-stringops -mcx16 -mfancy-math-387
  42              	# -mfp-ret-in-387 -mfused-madd -mglibc -mieee-fp -mmmx -mpopcnt -mpush-args
  43              	# -mred-zone -msahf -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -mssse3
  44              	# -mtls-direct-seg-refs
  45              	
  53              	.Ltext0:
  54              	# Compiler executable checksum: 76a20b9f1070721afce61d667b1c5940
  55              	
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
   1:/usr/include/bits/stdio2.h **** /* Checking macros for stdio functions.
   2:/usr/include/bits/stdio2.h ****    Copyright (C) 2004, 2005, 2007, 2008 Free Software Foundation, Inc.
   3:/usr/include/bits/stdio2.h ****    This file is part of the GNU C Library.
   4:/usr/include/bits/stdio2.h **** 
   5:/usr/include/bits/stdio2.h ****    The GNU C Library is free software; you can redistribute it and/or
   6:/usr/include/bits/stdio2.h ****    modify it under the terms of the GNU Lesser General Public
   7:/usr/include/bits/stdio2.h ****    License as published by the Free Software Foundation; either
   8:/usr/include/bits/stdio2.h ****    version 2.1 of the License, or (at your option) any later version.
   9:/usr/include/bits/stdio2.h **** 
  10:/usr/include/bits/stdio2.h ****    The GNU C Library is distributed in the hope that it will be useful,
  11:/usr/include/bits/stdio2.h ****    but WITHOUT ANY WARRANTY; without even the implied warranty of
  12:/usr/include/bits/stdio2.h ****    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  13:/usr/include/bits/stdio2.h ****    Lesser General Public License for more details.
  14:/usr/include/bits/stdio2.h **** 
  15:/usr/include/bits/stdio2.h ****    You should have received a copy of the GNU Lesser General Public
  16:/usr/include/bits/stdio2.h ****    License along with the GNU C Library; if not, write to the Free
  17:/usr/include/bits/stdio2.h ****    Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  18:/usr/include/bits/stdio2.h ****    02111-1307 USA.  */
  19:/usr/include/bits/stdio2.h **** 
  20:/usr/include/bits/stdio2.h **** #ifndef _STDIO_H
  21:/usr/include/bits/stdio2.h **** # error "Never include <bits/stdio2.h> directly; use <stdio.h> instead."
  22:/usr/include/bits/stdio2.h **** #endif
  23:/usr/include/bits/stdio2.h **** 
  24:/usr/include/bits/stdio2.h **** extern int __sprintf_chk (char *__restrict __s, int __flag, size_t __slen,
  25:/usr/include/bits/stdio2.h **** 			  __const char *__restrict __format, ...) __THROW;
  26:/usr/include/bits/stdio2.h **** extern int __vsprintf_chk (char *__restrict __s, int __flag, size_t __slen,
  27:/usr/include/bits/stdio2.h **** 			   __const char *__restrict __format,
  28:/usr/include/bits/stdio2.h **** 			   _G_va_list __ap) __THROW;
  29:/usr/include/bits/stdio2.h **** 
  30:/usr/include/bits/stdio2.h **** #ifdef __va_arg_pack
  31:/usr/include/bits/stdio2.h **** __extern_always_inline int
  32:/usr/include/bits/stdio2.h **** __NTH (sprintf (char *__restrict __s, __const char *__restrict __fmt, ...))
  33:/usr/include/bits/stdio2.h **** {
  34:/usr/include/bits/stdio2.h ****   return __builtin___sprintf_chk (__s, __USE_FORTIFY_LEVEL - 1,
  35:/usr/include/bits/stdio2.h **** 				  __bos (__s), __fmt, __va_arg_pack ());
  36:/usr/include/bits/stdio2.h **** }
  37:/usr/include/bits/stdio2.h **** #elif !defined __cplusplus
  38:/usr/include/bits/stdio2.h **** # define sprintf(str, ...) \
  39:/usr/include/bits/stdio2.h ****   __builtin___sprintf_chk (str, __USE_FORTIFY_LEVEL - 1, __bos (str), \
  40:/usr/include/bits/stdio2.h **** 			   __VA_ARGS__)
  41:/usr/include/bits/stdio2.h **** #endif
  42:/usr/include/bits/stdio2.h **** 
  43:/usr/include/bits/stdio2.h **** __extern_always_inline int
  44:/usr/include/bits/stdio2.h **** __NTH (vsprintf (char *__restrict __s, __const char *__restrict __fmt,
  45:/usr/include/bits/stdio2.h **** 		 _G_va_list __ap))
  46:/usr/include/bits/stdio2.h **** {
  47:/usr/include/bits/stdio2.h ****   return __builtin___vsprintf_chk (__s, __USE_FORTIFY_LEVEL - 1,
  48:/usr/include/bits/stdio2.h **** 				   __bos (__s), __fmt, __ap);
  49:/usr/include/bits/stdio2.h **** }
  50:/usr/include/bits/stdio2.h **** 
  51:/usr/include/bits/stdio2.h **** #if defined __USE_BSD || defined __USE_ISOC99 || defined __USE_UNIX98
  52:/usr/include/bits/stdio2.h **** 
  53:/usr/include/bits/stdio2.h **** extern int __snprintf_chk (char *__restrict __s, size_t __n, int __flag,
  54:/usr/include/bits/stdio2.h **** 			   size_t __slen, __const char *__restrict __format,
  55:/usr/include/bits/stdio2.h **** 			   ...) __THROW;
  56:/usr/include/bits/stdio2.h **** extern int __vsnprintf_chk (char *__restrict __s, size_t __n, int __flag,
  57:/usr/include/bits/stdio2.h **** 			    size_t __slen, __const char *__restrict __format,
  58:/usr/include/bits/stdio2.h **** 			    _G_va_list __ap) __THROW;
  59:/usr/include/bits/stdio2.h **** 
  60:/usr/include/bits/stdio2.h **** # ifdef __va_arg_pack
  61:/usr/include/bits/stdio2.h **** __extern_always_inline int
  62:/usr/include/bits/stdio2.h **** __NTH (snprintf (char *__restrict __s, size_t __n,
  63:/usr/include/bits/stdio2.h **** 		 __const char *__restrict __fmt, ...))
  64:/usr/include/bits/stdio2.h **** {
  65:/usr/include/bits/stdio2.h ****   return __builtin___snprintf_chk (__s, __n, __USE_FORTIFY_LEVEL - 1,
  66:/usr/include/bits/stdio2.h **** 				   __bos (__s), __fmt, __va_arg_pack ());
  67:/usr/include/bits/stdio2.h **** }
  68:/usr/include/bits/stdio2.h **** # elif !defined __cplusplus
  69:/usr/include/bits/stdio2.h **** #  define snprintf(str, len, ...) \
  70:/usr/include/bits/stdio2.h ****   __builtin___snprintf_chk (str, len, __USE_FORTIFY_LEVEL - 1, __bos (str), \
  71:/usr/include/bits/stdio2.h **** 			    __VA_ARGS__)
  72:/usr/include/bits/stdio2.h **** # endif
  73:/usr/include/bits/stdio2.h **** 
  74:/usr/include/bits/stdio2.h **** __extern_always_inline int
  75:/usr/include/bits/stdio2.h **** __NTH (vsnprintf (char *__restrict __s, size_t __n,
  76:/usr/include/bits/stdio2.h **** 		  __const char *__restrict __fmt, _G_va_list __ap))
  77:/usr/include/bits/stdio2.h **** {
  78:/usr/include/bits/stdio2.h ****   return __builtin___vsnprintf_chk (__s, __n, __USE_FORTIFY_LEVEL - 1,
  79:/usr/include/bits/stdio2.h **** 				    __bos (__s), __fmt, __ap);
  80:/usr/include/bits/stdio2.h **** }
  81:/usr/include/bits/stdio2.h **** 
  82:/usr/include/bits/stdio2.h **** #endif
  83:/usr/include/bits/stdio2.h **** 
  84:/usr/include/bits/stdio2.h **** #if __USE_FORTIFY_LEVEL > 1
  85:/usr/include/bits/stdio2.h **** 
  86:/usr/include/bits/stdio2.h **** extern int __fprintf_chk (FILE *__restrict __stream, int __flag,
  87:/usr/include/bits/stdio2.h **** 			  __const char *__restrict __format, ...);
  88:/usr/include/bits/stdio2.h **** extern int __printf_chk (int __flag, __const char *__restrict __format, ...);
  89:/usr/include/bits/stdio2.h **** extern int __vfprintf_chk (FILE *__restrict __stream, int __flag,
  90:/usr/include/bits/stdio2.h **** 			   __const char *__restrict __format, _G_va_list __ap);
  91:/usr/include/bits/stdio2.h **** extern int __vprintf_chk (int __flag, __const char *__restrict __format,
  92:/usr/include/bits/stdio2.h **** 			  _G_va_list __ap);
  93:/usr/include/bits/stdio2.h **** 
  94:/usr/include/bits/stdio2.h **** # ifdef __va_arg_pack
  95:/usr/include/bits/stdio2.h **** __extern_always_inline int
  96:/usr/include/bits/stdio2.h **** fprintf (FILE *__restrict __stream, __const char *__restrict __fmt, ...)
  97:/usr/include/bits/stdio2.h **** {
  98:/usr/include/bits/stdio2.h ****   return __fprintf_chk (__stream, __USE_FORTIFY_LEVEL - 1, __fmt,
  99:/usr/include/bits/stdio2.h **** 			__va_arg_pack ());
 100:/usr/include/bits/stdio2.h **** }
 101:/usr/include/bits/stdio2.h **** 
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

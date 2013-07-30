/*********************************************************************
 *                
 * Copyright (C) 2003,  Karlsruhe University
 *                
 * File path:     amd64-port.h
 * Description:   AMD64 port I/O
 *                
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *                
 * $Id: amd64-port.h,v 1.2 2003/09/24 19:06:28 skoglund Exp $
 *                
 ********************************************************************/
#ifndef __USER__LIB__IO__IA32_PORT_H__
#define __USER__LIB__IO__IA32_PORT_H__

extern inline L4_Word8_t inb(L4_Word_t port)
{
    L4_Word8_t tmp;

    if (port < 0x100) /* GCC can optimize this if constant */
	__asm__ __volatile__ ("inb %w1, %0" :"=al"(tmp) :"dN"(port));
    else
	__asm__ __volatile__ ("inb %%dx, %0" :"=al"(tmp) :"d"(port));

    return tmp;
}

extern inline void outb(L4_Word_t port,  L4_Word8_t val)
{
    if (port < 0x100) /* GCC can optimize this if constant */
	__asm__ __volatile__ ("outb %1, %w0" : :"dN"(port), "al"(val));
    else
	__asm__ __volatile__ ("outb %1, %%dx" : :"d"(port), "al"(val));
}

#endif /* !__USER__LIB__IO__IA32_PORT_H__ */

/*********************************************************************
 *                
 * Copyright (C) 2004,  Karlsruhe University
 *                
 * File path:     elf.cc
 * Description:   Simple ELF loader
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
 * $Id: elf.cc,v 1.2.2.1 2004/06/02 22:51:31 skoglund Exp $
 *                
 ********************************************************************/
#include <config.h>

#include <l4io.h>
#include <l4/bootinfo.h>

//#define ELF_32on64
//#define ELF_64on32

#include "kickstart.h"
#include "elf.h"
#include "lib.h"

template<typename T>
inline T min(T v1, T v2)
{
	if(v1 < v2)
		return v1;
		
	return v2;
}

template<typename T>
inline T max(T v1, T v2)
{
	if(v1 > v2)
		return v1;
	
	return v2;
}

bool elf_load32 (L4_Word_t file_start,
		 L4_Word_t file_end,
		 L4_Word_t *memory_start,
		 L4_Word_t *memory_end,
		 L4_Word_t *entry);

bool elf_find_sections32 (L4_Word_t addr,
			  L4_Boot_SimpleExec_t * exec);

bool elf_load64 (L4_Word_t file_start,
		 L4_Word_t file_end,
		 L4_Word_t *memory_start,
		 L4_Word_t *memory_end,
		 L4_Word_t *entry);

bool elf_find_sections64 (L4_Word_t addr,
			  L4_Boot_SimpleExec_t * exec);



bool __elf_func(elf_load) (L4_Word_t file_start,
			   L4_Word_t file_end,
			   L4_Word_t *memory_start,
			   L4_Word_t *memory_end,
			   L4_Word_t *entry)
{
    // Pointer to ELF file header
    ehdr_t* eh = (ehdr_t*) file_start;

    // Is it an executable?
    if (eh->type != 2)
    {
        // No. Bail out
        l4_printf("  No executable\n");
        return false;
    }

    // Is the address of the PHDR table valid?
    if (eh->phoff == 0)
    {
        // No. Bail out
        l4_printf("Wrong PHDR table offset\n");
        return false;
    }

        
    l4_printf("   => %p\n", (void *)(L4_Word_t)eh->entry);

    // Locals to find the enclosing memory range of the loaded file
    L4_Word_t max_addr =  0U;
    L4_Word_t min_addr = ~0U;

    // Walk the program header table
    for (L4_Word_t i = 0; i < eh->phnum; i++)
    {
        // Locate the entry in the program header table
        phdr_t* ph = (phdr_t*)(L4_Word_t)
	    (file_start + eh->phoff + eh->phentsize * i);
        
        // Is it to be loaded?
        if (ph->type == PT_LOAD)
        {
            l4_printf("  (%p,%p)->%p\n",
                   (void *)(L4_Word_t) (file_start + ph->offset),
		   (void *)(L4_Word_t) ph->fsize,
		   (void *)(L4_Word_t) ph->paddr);
            // Copy bytes from "file" to memory - load address
            memcopy(ph->paddr, file_start + ph->offset, ph->fsize);
	    // Zero "non-file" bytes
	    memset (ph->paddr + ph->fsize, 0, ph->msize - ph->fsize);
            // Update min and max
            min_addr = min(min_addr, ph->paddr);
            max_addr = max(max_addr, ph->paddr + max(ph->msize, ph->fsize));
        }
        
    }

    // Write back the values into the caller-provided locations
    *memory_start = min_addr;
    *memory_end = max_addr;
    *entry = eh->entry;

    // Signal successful loading
    return true;
}


bool __elf_func(elf_find_sections) (L4_Word_t addr,
				    L4_Boot_SimpleExec_t * exec)
{
    // Pointer to ELF file header
    ehdr_t * eh = (ehdr_t *) addr;

    if (eh->type != 2)
	// Not an executable file.
	return false;

    if (eh->phoff == 0)
	// No program headers.
        return false;

    // Initialize a local bootinfo record
    memset ((L4_Word_t) exec, 0, sizeof (*exec));
    exec->type = L4_BootInfo_SimpleExec;
    exec->version = 1;
    exec->initial_ip = eh->entry;
    exec->offset_next = sizeof (*exec);

    if (eh->phoff != 0 && eh->shoff == 0)
    {
	/*
	 * We only have a program headers.  Walk all program headers
	 * and try to figure out what the sections are.
	 */

	for (L4_Word_t i = 0; i < eh->phnum; i++)
	{
	    phdr_t * ph = (phdr_t *) (L4_Word_t) (addr + eh->phoff + eh->phentsize * i);

	    // Assume that a segment without write permissions is .text
	    if ((ph->flags & PF_W) == 0)
	    {
		exec->text_pstart = ph->paddr;
		exec->text_vstart = ph->paddr;
		exec->text_size   = ph->fsize;
	    }
	    else
	    {
		exec->data_pstart = ph->paddr;
		exec->data_vstart = ph->paddr;
		exec->data_size   = ph->fsize;
	    }

	    if (ph->msize > ph->fsize)
	    {
		// Looks like a bss section
		exec->bss_pstart = ph->paddr + ph->fsize;
		exec->bss_vstart = ph->vaddr + ph->fsize;
		exec->bss_size   = ph->msize - ph->fsize;
	    }
	}

	return true;
    }

    /*
     * If we have the section headers we can try to figure out the
     * real .text, .data, and .bss segments by inspecting the section
     * type and flags.
     */

    L4_Word_t tlow = ~0UL, thigh = 0;
    L4_Word_t dlow = ~0UL, dhigh = 0;
    L4_Word_t blow = ~0UL, bhigh = 0;
    for (L4_Word_t i = 0; i < eh->shnum; i++)
    {
	shdr_t * sh = (shdr_t *) (L4_Word_t)
	    (addr + eh->shoff + eh->shentsize * i);

	if (sh->type == SHT_PROGBITS)
	{
	    /* Also include read-only sections into .text */
	    if ((sh->flags & SHF_ALLOC) &&
		((sh->flags & SHF_EXECINSTR) || (~sh->flags & SHF_WRITE)))
	    {
		if (sh->addr < tlow)
		    tlow = sh->addr;
		if (sh->addr + sh->size > thigh)
		    thigh = sh->addr + sh->size;
	    }
	    /* Other writable sections are counted as .data */
	    else if ((sh->flags & SHF_ALLOC) &&
		     (sh->flags & SHF_WRITE))
	    {
		if (sh->addr < dlow)
		    dlow = sh->addr;
		if (sh->addr + sh->size > dhigh)
		    dhigh = sh->addr + sh->size;
	    }
	}
	else if (sh->type == SHT_NOBITS)
	{
	    /* Assume that all nobits sections are .bss */
	    if (sh->addr < blow)
		blow = sh->addr;
	    if (sh->addr + sh->size > bhigh)
		bhigh = sh->addr + sh->size;
	}
    }

    /*
     * Translate the virtual addresses of the sections to physical
     * addresses using the segments in the program header.
     */

#define INSEGMENT(a)	(a >= ph->vaddr && a < (ph->vaddr + ph->msize))
#define PADDR(a)	(ph->paddr + a - ph->vaddr)

    for (L4_Word_t i = 0; i < eh->phnum; i++)
    {
	phdr_t * ph = (phdr_t *) (L4_Word_t)
	    (addr + eh->phoff + eh->phentsize * i);

	if (INSEGMENT (tlow))
	{
	    exec->text_pstart = PADDR (tlow);
	    exec->text_vstart = tlow;
	    exec->text_size = thigh - tlow;
	}
	if (INSEGMENT (dlow))
	{
	    exec->data_pstart = PADDR (dlow);
	    exec->data_vstart = dlow;
	    exec->data_size = dhigh - dlow;
	}
	if (INSEGMENT (blow))
	{
	    exec->bss_pstart = PADDR (blow);
	    exec->bss_vstart = blow;
	    exec->bss_size = bhigh - blow;
	}
    }

    return true;
}


#if !defined(ELF_32on64) && !defined(ELF_64on32) 

/**
 * ELF-load an ELF memory image
 *
 * @param file_start    First byte of source ELF image in memory
 * @param file_end      First byte behind source ELF image in memory
 * @param memory_start  Pointer to address of first byte of loaded image
 * @param memory_end    Pointer to address of first byte behind loaded image
 * @param entry         Pointer to address of entry point
 *
 * This function ELF-loads an ELF image that is located in memory.  It
 * interprets the program header table and copies all segments marked
 * as load. It stores the lowest and highest+1 address used by the
 * loaded image as well as the entry point into caller-provided memory
 * locations.
 *
 * @returns false if ELF loading failed, true otherwise.
 */
bool elf_load (L4_Word_t file_start,
	       L4_Word_t file_end,
	       L4_Word_t *memory_start,
	       L4_Word_t *memory_end,
	       L4_Word_t *entry)
{
    // Pointer to ELF file header
    ehdr_t* eh = (ehdr_t*) file_start;

    // Is it an ELF file? Check ELF magic
    if (!((eh->ident[0] == 0x7F) &&
          (eh->ident[1] == 'E') &&
          (eh->ident[2] == 'L') &&
          (eh->ident[3] == 'F')))
    {
        return false;
    }

#if defined(L4_32BIT) || defined(ALSO_ELF32)
    if (eh->is_32bit ())
	return elf_load32 (file_start, file_end,
			   memory_start, memory_end, entry);
#endif

#if defined(L4_64BIT) || defined(ALSO_ELF64)
    if (eh->is_64bit ())
	return elf_load64 (file_start, file_end,
			   memory_start, memory_end, entry);
#endif

    return false;
}


/**
 * Find section address and sizes of ELF file
 *
 * @param addr		Address of ELF file
 * @param exec		Pointer to bootinfo exec structure
 *
 * This functions tries to figure out the location (virtual and
 * physical) and size of the .text, .data and .bss sections.  The
 * location is derived from the section headers or, if there are no
 * section headers, from the program headers.
 *
 * @returns true if able to find sections, false otherwise
 */
bool elf_find_sections (L4_Word_t addr,
			L4_Boot_SimpleExec_t * exec)
{
    // Pointer to ELF file header
    ehdr_t * eh = (ehdr_t *) addr;

    // Is it an ELF file? Check ELF magic
    if (!((eh->ident[0] == 0x7F) &&
          (eh->ident[1] == 'E') &&
          (eh->ident[2] == 'L') &&
          (eh->ident[3] == 'F')))
    {
	// Not an ELF file.
        return false;
    }

#if defined(L4_32BIT) || defined(ELF_32on64)
    if (eh->is_32bit ())
	return elf_find_sections32 (addr, exec);
#endif

#if defined(L4_64BIT) || defined(ELF_64on32)
    if (eh->is_64bit ())
	return elf_find_sections64 (addr, exec);
#endif

    return false;
}

#endif

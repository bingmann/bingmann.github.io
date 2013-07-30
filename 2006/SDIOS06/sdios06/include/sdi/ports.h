/**
 * $Id: ports.h 69 2006-07-17 23:23:17Z sdi2 $
 * 
 * Inline assembly routines for accessing IO-Ports
 * @author matze
 */
#ifndef PORT_H_
#define PORT_H_

#include <stdint.h>

static inline uint8_t inb(const uint16_t port)
{
    uint8_t val;

    __asm__ __volatile__ ("inb  %w1, %0" : "=a"(val) : "dN"(port));

    return val;
}

static inline void outb(const uint16_t port, const uint8_t val)
{
    __asm__ __volatile__ ("outb %0, %w1" : : "a"(val), "dN"(port));
}

static inline uint16_t inw(const uint16_t port)
{
    uint16_t val;

    __asm__ __volatile__ ("inw %w1, %0" : "=a"(val) : "d"(port));
    
    return val;
}

static inline void outw(const uint16_t port, const uint16_t val)
{
    __asm__ __volatile__ ("outw %0, %w1" : : "a"(val), "d"(port));
}

static inline uint32_t inl(const uint16_t port)
{
    uint32_t val;

    __asm__ __volatile__ ("inl %w1, %0" : "=a"(val) : "d"(port));
    
    return val;
}

static inline void outl(const uint16_t port, const uint32_t val)
{
    __asm__ __volatile__ ("outl %0, %w1" : : "a"(val), "d"(port));
}

#endif

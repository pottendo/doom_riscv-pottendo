// -*-c++-*-
// $Id$
//
// File:		c64-linux.c
// Date:		Fri Dec 16 09:38:08 2022
// Author:		pottendo (pottendo)
// 
// Abstract:
//      
//
// Modifications:
// 	$Log$
//
#include <stdio.h>

#ifndef ZEPHYR
#include <fcntl.h>
#include <sys/mman.h>
#endif

#include <unistd.h>
#include <iostream>
#include "c64-lib.h"

c64::c64()
{
    mem = vic = cia1 = cia2 = sid = oc_ctrl = nullptr;
    mem = map_memory(c64_physaddress, 0x10000);
    if (mem == (unsigned char*) -1)
	perror("map_memory failed.");
    else
    {
	vic = &mem[0xd000];
	cia1 = &mem[0xdc00];
	cia2 = &mem[0xdd00];
	sid = &mem[0xd400];
	oc_ctrl = &mem[0xdf00];
    }
    
}

c64::~c64()
{
    gfx(VICBank0, VICModeText, 1);
#ifndef ZEPHYR    
    munmap(mem, 0x10000);
#endif    
}

void c64::gfx(c64_consts bank, c64_consts mode, uint8_t vram)
{
    if (!vic)
    {
	log_msg("vic not mapped.\n");
	return;
    }
    cia2[0] &= 0b11111100;
    cia2[0] |= bank;
    switch(mode)
    {
    case VICModeGfxMC:
	vic[VIC::CR2] |= 0b00010000;
    case VICModeGfxHR:
	vic[VIC::CR1] |= 0b00100000;
	if (vram > 15)
	    log_msg("%s: illegal VRAM: 0x%02x\n", __FUNCTION__, vram);
	vic[VIC::MEM] &= 0b00001111;
	vic[VIC::MEM] |= (vram << 4);
	break;
    case VICModeText:
	vic[VIC::CR1] &= 0b11011111;
	vic[VIC::CR2] &= 0b11101111;
	if (vram > 15)
	    log_msg("%s: illegal VRAM: 0x%02x\n", __FUNCTION__, vram);
	vic[VIC::MEM] &= 0b00001111;
	vic[VIC::MEM] |= (vram << 4);
	break;
    default:
	log_msg("%s: unkown VIC mode %d.\n", __FUNCTION__, mode);
	break;
    }
}

unsigned char *
c64::map_memory(off_t offset, size_t len)
{
#ifndef ZEPHYR    
    // Truncate offset to a multiple of the page size, or mmap will fail.
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t page_offset = offset - page_base;

    int fd = open("/dev/mem", (O_RDWR | O_DSYNC));
    unsigned char *mem = (unsigned char *) mmap(NULL, page_offset + len, PROT_READ | PROT_WRITE,
						MAP_SHARED, fd, page_base);
    if (mem == MAP_FAILED) {
        perror("Can't map memory");
        return (unsigned char *)-1;
    }
#else
    mem = (unsigned char *)offset;
#endif    
    return mem;
}

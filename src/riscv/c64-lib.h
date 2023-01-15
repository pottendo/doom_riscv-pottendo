/*  -*-c-*-
 * File:		c64-linux.h
 * Date:		Fri Dec 16 09:39:01 2022
 * Author:		pottendo (pottendo)
 *  
 * Abstract:
 *     some aux functions for c64 @linux orange cart
 * 
 * Modifications:
 * $Log$
 */

#ifndef __c64_linux_h__
#define __c64_linux_h__
#include <stdint.h>

#define log_msg printf

typedef enum {
    VICBank0 = 3, VICBank1 = 2, VICBank2 = 1, VICBank3 = 0,
    VICModeGfxHR = 4, VICModeGfxMC = 5, VICModeText = 6
} c64_consts;

namespace VIC
{
    const uint8_t BoC = 0x20;
    const uint8_t BgC = 0x21;
    const uint8_t CR1 = 0x11;
    const uint8_t CR2 = 0x16;
    const uint8_t MEM = 0x18;
    enum { BLACK = 0, WHITE, RED, CYAN, PURPLE, GREEN, BLUE, YELLOW,
	   ORANGE, BROWN, PINK, DARK_GREY, GREY, LIGHT_GREEN, LIGHT_BLUE, LIGHT_GREY };
};

class c64
{
#ifdef CONFIG_BOARD_ORANGECART // Fixme: should be done via DTS query
    const uint32_t c64_physaddress = 0x00000000;  // as defined in basesoc.py, RVCop64
#else
    const uint32_t c64_physaddress = 0x0f000000;  // as defined in basesoc.py, RVCop64
#endif    
    
    unsigned char *map_memory(off_t addr, size_t len);
    unsigned char *mem, *vic, *cia1, *cia2, *sid, *oc_ctrl;

  public:
    c64();
    ~c64();

    unsigned char *get_mem(void) { return mem; }
    void gfx(c64_consts bank, c64_consts mode, uint8_t vram);
    void screencols(uint8_t bo, uint8_t bg) { vic[VIC::BoC] = bo; vic[VIC::BgC] = bg; }

};

#endif /* __c64_linux_h__ */

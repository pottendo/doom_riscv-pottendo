/*
 * i_video.c
 *
 * Video system support code
 *
 * Copyright (C) 2021 Sylvain Munaut
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

//#include <stdint.h>
#include <string.h>

#include "doomdef.h"

#include "i_system.h"
#include "v_video.h"
#include "i_video.h"

#include "config.h"

#include <sys/stat.h>
#include <sys/mman.h>
#ifdef LUCKFOX
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#endif
#include <unistd.h>
//#include <stdint.h>

uint16_t *tft_canvas; // must not be static?!

static uint32_t video_pal[3][256];

#ifdef LUCKFOX
static int fd;
static int img_w, img_h;
#endif

extern void init_cv(void);

void
I_InitGraphics(void)
{
    /* Don't need to do anything really ... */

    /* Ok, maybe just set gamma default */
    usegamma = 1;

#ifdef LUCKFOX
    struct fb_var_screeninfo fb_var;

    printf("%s: mapping fb...\n", __FUNCTION__);

    fd = open("/dev/fb0", O_RDWR);
    if (fd <= 0)
    {
        printf("open TFT framebuffer, errno = %d\n", errno);
        return;
    }

    ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
    img_w = fb_var.xres;
    img_h = fb_var.yres;
    printf("%s: img = %dx%d, %dx%d\n", __FUNCTION__, img_w, img_h, SCREENWIDTH, SCREENHEIGHT);
    tft_canvas = (uint16_t *)mmap(NULL, (img_w * img_h) << 1, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (tft_canvas == MAP_FAILED)
    {
        printf("mmap failed - errno = %d\n", errno);
        tft_canvas = NULL;
        return;
    }
    memset(tft_canvas, 0, (img_w * img_h) << 1);
//    init_cv();
#endif    
}

void
I_ShutdownGraphics(void)
{
    /* Don't need to do anything really ... */
}


void
I_SetPalette(byte* palette)
{
    //static volatile uint32_t * const video_pal = (void*)(VID_PAL_BASE);

    for (int i=0 ; i<256 ; i++) {
	video_pal[0][i] = gammatable[usegamma][*palette++];
	video_pal[1][i] = gammatable[usegamma][*palette++];
	video_pal[2][i] = gammatable[usegamma][*palette++];
    }
}


void
I_UpdateNoBlit(void)
{
//    printf("%s: 1\n", __FUNCTION__);
}

void
I_FinishUpdate (void)
{
    int x, y, pos;
    /* Copy from RAM buffer to frame buffer */
    for (x = 0; x < SCREENWIDTH; x++)
    {
	for (y = 0; y < SCREENHEIGHT; y++)
	{
	    pos = y * SCREENWIDTH + x ;
	    tft_canvas[pos] = (uint16_t)
		(video_pal[2][screens[0][pos]] & (0x1f << 3)) << 8 | 
		(video_pal[1][screens[0][pos]] & (0x3f << 2)) << 3 | 
		((video_pal[0][screens[0][pos]] >> 3) & 0x1f);
	}
    }
    /* Very crude FPS measure (time to render 100 frames */
#if 0
    static int frame_cnt = 0;
    static int tick_prev = 0;

    if (++frame_cnt == 100)
    {
	int tick_now = I_GetTime();
	printf("%d\n", tick_now - tick_prev);
	tick_prev = tick_now;
	frame_cnt = 0;
    }
#endif
}


void
I_WaitVBL(int count)
{
    return;
    /* Buys-Wait for VBL status bit */
    static volatile uint32_t * const video_state = (void*)(VID_CTRL_BASE);
    while (!(video_state[0] & (1<<16)));
}


void
I_ReadScreen(byte* scr)
{
    /* FIXME: Would have though reading from VID_FB_BASE be better ...
     *        but it seems buggy. Not sure if the problem is in the
     *        gateware
     */
    memcpy(
	scr,
	screens[0],
	SCREENHEIGHT * SCREENWIDTH
	);
}


#if 0	/* WTF ? Not used ... */
void
I_BeginRead(void)
{
}

void
I_EndRead(void)
{
}
#endif

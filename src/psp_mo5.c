/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <stdio.h>
#include <zlib.h>
#include "SDL.h"

#include <stdlib.h>
#include <stdio.h>

#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "global.h"
#include "psp_mo5.h"
#include "psp_sdl.h"


void
psp_mo5_hard_reset()
{
  Initprog();
  Ejectmemo();
  Ejectk7();
  Ejectfd();

  if (MO5.mo5_disk_mode) {
    mo5_load_dos();
  }
}

void
psp_mo5_soft_reset()
{
  Initprog();
}

void
psp_mo5_eject()
{
  Ejectmemo();

  if (MO5.mo5_disk_mode) {
    mo5_load_dos();
  }
}

int
psp_mo5_rewind_k7()
{
  Rewindk7();
  return 0;
}

int
psp_mo5_get_k7_index()
{
  return GetK7Index();
}

int 
psp_mo5_load_rom(char *filename) 
{
  return Loadmemo(filename);
}

/* LUDO: */
void
mo5_synchronize(void)
{
  static u32 nextclock = 1;

  if (nextclock) {
    u32 curclock;
    do {
     curclock = SDL_GetTicks();
    } while (curclock < nextclock);
    nextclock = curclock + (u32)( 1000 / MO5.mo5_speed_limiter);
  }
}

void
mo5_update_fps()
{
  static u32 next_sec_clock = 0;
  static u32 cur_num_frame = 0;
  cur_num_frame++;
  u32 curclock = SDL_GetTicks();
  if (curclock > next_sec_clock) {
    next_sec_clock = curclock + 1000;
    MO5.mo5_current_fps = cur_num_frame;
    cur_num_frame = 0;
  }
}

void
psp_mo5_backup_to_blit(void)
{
  uint* src_vram = (uint *)back_surface->pixels;
  uint* dst_vram = (uint *)blit_surface->pixels;
  src_vram += 8 * PSP_LINE_SIZE;
  dst_vram += 4 + (4 * MO5_WIDTH);
  int y;
  int x;
  for (y = 0; y < (MO5_HEIGHT - 16); y++) {
    for (x = 0; x < (320 >> 1); x++) {
      dst_vram[x] = src_vram[x];
    }
    dst_vram += (MO5_WIDTH  >> 1);
    src_vram += (PSP_LINE_SIZE >> 1);
  }
}

void
psp_mo5_display()
{
  if (MO5.psp_skip_cur_frame <= 0) {

    MO5.psp_skip_cur_frame = MO5.psp_skip_max_frame;

    if (psp_kbd_is_danzeff_mode()) {
      danzeff_moveTo(-10, -65);
      danzeff_render();
    }

    if (MO5.mo5_view_fps) {
      char buffer[32];
      sprintf(buffer, "%3d", (int)MO5.mo5_current_fps);
      psp_sdl_fill_print(0, 0, buffer, 0xffffff, 0 );
    }
    psp_sdl_flip();
  
    if (psp_screenshot_mode) {
      psp_screenshot_mode--;
      if (psp_screenshot_mode <= 0) {
        psp_sdl_save_screenshot();
        psp_screenshot_mode = 0;
      }
    }

  } else if (MO5.psp_skip_max_frame) {
    MO5.psp_skip_cur_frame--;
  }

  if (MO5.mo5_speed_limiter) {
	  mo5_synchronize();
  }

  if (MO5.mo5_view_fps) {
    mo5_update_fps();
  }
  
}

void
psp_mo5_initialize()
{
  InitScreen();
  Init6809registerpointers();
  Hardreset();
  psp_mo5_hard_reset();
}

# define FREQ_6809 1000000  // 1 MHz 

void
psp_mo5_main_loop()
{
  Run();
}

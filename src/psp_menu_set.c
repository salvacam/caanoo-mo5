/*
 *  Copyright (C) 2009 Ludovic Jacomme (ludovic.jacomme@gmail.com)
 *
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
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#include "global.h"
#include "psp_sdl.h"
#include "psp_kbd.h"
#include "psp_menu.h"
#include "psp_fmgr.h"
#include "psp_menu_kbd.h"
#include "psp_menu_set.h"

extern SDL_Surface *back_surface;

# define MENU_SET_SOUND         0
# define MENU_SET_JOYSTICK      1
# define MENU_SET_SPEED_LIMIT   2
# define MENU_SET_SKIP_FPS      3
# define MENU_SET_VIEW_FPS      4
# define MENU_SET_RENDER        5
# define MENU_SET_ANALOG        6
# define MENU_SET_AUTOFIRE      7
# define MENU_SET_DISK_MODE     8

# define MENU_SET_LOAD          9
# define MENU_SET_SAVE         10
# define MENU_SET_RESET        11
                                 
# define MENU_SET_BACK         12

# define MAX_MENU_SET_ITEM (MENU_SET_BACK + 1)

  static menu_item_t menu_list[] =
  {
    { "Sound enable       :"},
    { "Active Joystick    :"},
    { "Speed limiter      :"},
    { "Skip frame         :"},
    { "Display fps        :"},
    { "Render mode        :"},
    { "Joystick as cursor :"},
    { "Auto fire period   :"},
    { "Disk mode          :"},
    { "Load settings"       },
    { "Save settings"       },
    { "Reset settings"      },
    { "Back to Menu"        }
  };

  static int cur_menu_id = MENU_SET_LOAD;

  static int mo5_snd_enable     = 0;
  static int mo5_view_fps       = 0;
  static int mo5_speed_limiter  = 50;
  static int mo5_render_mode    = 0;
  static int psp_joystick_cursor   = 1;
  static int psp_active_joystick  = 0;
  static int mo5_skip_fps       = 0;
  static int mo5_auto_fire_period  = 0;
  static int mo5_disk_mode         = 1;

static void
psp_settings_menu_reset(void);

static void 
psp_display_screen_settings_menu(void)
{
  char buffer[64];
  int menu_id = 0;
  int color   = 0;
  int x       = 0;
  int y       = 0;
  int y_step  = 0;

  psp_sdl_blit_help();
  
  x      = 10;
  y      =  5;
  y_step = 10;
  
  for (menu_id = 0; menu_id < MAX_MENU_SET_ITEM; menu_id++) {
    color = PSP_MENU_TEXT_COLOR;
    if (cur_menu_id == menu_id) color = PSP_MENU_SEL_COLOR;

    psp_sdl_back2_print(x, y, menu_list[menu_id].title, color);

    if (menu_id == MENU_SET_SOUND) {
      if (mo5_snd_enable) strcpy(buffer,"yes");
      else                 strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_VIEW_FPS) {
      if (mo5_view_fps) strcpy(buffer,"on ");
      else              strcpy(buffer,"off");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_ANALOG) {
      if (psp_joystick_cursor) strcpy(buffer,"yes");
      else                     strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_AUTOFIRE) {
      sprintf(buffer,"%d", mo5_auto_fire_period+1);
      string_fill_with_space(buffer, 7);
      psp_sdl_back2_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_JOYSTICK) {
      if (psp_active_joystick) strcpy(buffer,"player 2");
      else                     strcpy(buffer,"player 1 ");
      string_fill_with_space(buffer, 10);
      psp_sdl_back2_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_DISK_MODE) {
      if (mo5_disk_mode) strcpy(buffer,"yes");
      else                strcpy(buffer,"no ");
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(140, y, buffer, color);
      y += y_step;
    } else
    if (menu_id == MENU_SET_SKIP_FPS) {
      sprintf(buffer,"%d", mo5_skip_fps);
      string_fill_with_space(buffer, 4);
      psp_sdl_back2_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_SPEED_LIMIT) {
      if (mo5_speed_limiter == 0) strcpy(buffer,"no");
      else sprintf(buffer, "%d fps", mo5_speed_limiter);
      string_fill_with_space(buffer, 10);
      psp_sdl_back2_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_RENDER) {

      strcpy(buffer, "normal");
      psp_sdl_back2_print(140, y, buffer, color);
      string_fill_with_space(buffer, 13);
      psp_sdl_back2_print(140, y, buffer, color);
    } else
    if (menu_id == MENU_SET_RESET) {
      y += y_step;
    }

    y += y_step;
  }

  psp_menu_display_save_name();
}

static void
psp_settings_menu_render(int step)
{
  if (step > 0) {
    if (mo5_render_mode < MO5_LAST_RENDER) mo5_render_mode++;
    else                                       mo5_render_mode = 0;
  } else {
    if (mo5_render_mode > 0) mo5_render_mode--;
    else                       mo5_render_mode = MO5_LAST_RENDER;
  }
}

static void
psp_settings_menu_autofire(int step)
{
  if (step > 0) {
    if (mo5_auto_fire_period < 19) mo5_auto_fire_period++;
  } else {
    if (mo5_auto_fire_period >  0) mo5_auto_fire_period--;
  }
}

static void
psp_settings_menu_skip_fps(int step)
{
  if (step > 0) {
    if (mo5_skip_fps < 25) mo5_skip_fps++;
  } else {
    if (mo5_skip_fps > 0) mo5_skip_fps--;
  }
}

static void
psp_settings_menu_limiter(int step)
{
  if (step > 0) {
    if (mo5_speed_limiter < 60) mo5_speed_limiter++;
    else                          mo5_speed_limiter  = 0;
  } else {
    if (mo5_speed_limiter >  0) mo5_speed_limiter--;
    else                          mo5_speed_limiter  = 60;
  }
}

static void
psp_settings_menu_init(void)
{
  mo5_snd_enable     = MO5.mo5_snd_enable;
  mo5_render_mode    = MO5.mo5_render_mode;
  mo5_speed_limiter  = MO5.mo5_speed_limiter;
  mo5_skip_fps       = MO5.psp_skip_max_frame;
  mo5_view_fps       = MO5.mo5_view_fps;
  psp_joystick_cursor  = MO5.psp_joystick_cursor;
  psp_active_joystick  = MO5.psp_active_joystick;
  mo5_disk_mode        = MO5.mo5_disk_mode;
  mo5_auto_fire_period = MO5.mo5_auto_fire_period;
}

static void
psp_settings_menu_load(int format)
{
  int ret;

  ret = psp_fmgr_menu(format);
  if (ret ==  1) /* load OK */
  {
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(170, 110, "File loaded !", 
                       PSP_MENU_NOTE_COLOR);
    psp_sdl_flip();
    sleep(1);
    psp_settings_menu_init();
  }
  else 
  if (ret == -1) /* Load Error */
  {
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(170, 110, "Can't load file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_sdl_flip();
    sleep(1);
  }
}

static void
psp_settings_menu_validate(void)
{
  /* Validate */
  MO5.mo5_snd_enable    = mo5_snd_enable;
  MO5.mo5_render_mode   = mo5_render_mode;
  MO5.mo5_speed_limiter = mo5_speed_limiter;
  MO5.mo5_view_fps      = mo5_view_fps;
  MO5.psp_joystick_cursor = psp_joystick_cursor;
  MO5.psp_active_joystick = psp_active_joystick;
  MO5.psp_skip_max_frame  = mo5_skip_fps;
  MO5.psp_skip_cur_frame  = 0;
  MO5.mo5_disk_mode       = mo5_disk_mode;
  MO5.mo5_auto_fire_period  = mo5_auto_fire_period;
}


static void
psp_settings_menu_save()
{
  int error;

  psp_settings_menu_validate();
  error = mo5_save_settings();

  if (! error) /* save OK */
  {
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(170, 110, "File saved !", 
                       PSP_MENU_NOTE_COLOR);
    psp_sdl_flip();
    sleep(1);
  }
  else 
  {
    psp_display_screen_settings_menu();
    psp_sdl_back2_print(170, 110, "Can't save file !", 
                       PSP_MENU_WARNING_COLOR);
    psp_sdl_flip();
    sleep(1);
  }
}

void
psp_settings_menu_reset(void)
{
  psp_display_screen_settings_menu();
  psp_sdl_back2_print(170, 110, "Reset Settings !", 
                     PSP_MENU_WARNING_COLOR);
  psp_sdl_flip();
  mo5_default_settings();
  psp_settings_menu_init();
  sleep(1);
}

int 
psp_settings_menu(void)
{
  gp2xCtrlData c;
  long        new_pad;
  long        old_pad;
  int         last_time;
  int         end_menu;

  psp_kbd_wait_no_button();

  old_pad   = 0;
  last_time = 0;
  end_menu  = 0;

  psp_settings_menu_init();

  while (! end_menu)
  {
    psp_display_screen_settings_menu();
    psp_sdl_flip();

    while (1)
    {
      gp2xCtrlPeekBufferPositive(&c, 1);
      gp2xCtrlConvertKeyboardSym( &c );
      c.Buttons &= PSP_ALL_BUTTON_MASK;

      if (c.Buttons) break;
    }

    new_pad = c.Buttons;

    if ((old_pad != new_pad) || ((c.TimeStamp - last_time) > PSP_MENU_MIN_TIME)) {
      last_time = c.TimeStamp;
      old_pad = new_pad;

    } else continue;

    if ((c.Buttons & GP2X_CTRL_RTRIGGER) == GP2X_CTRL_RTRIGGER) {
      psp_settings_menu_reset();
      end_menu = 1;
    } else
    if ((new_pad == GP2X_CTRL_LEFT ) || 
        (new_pad == GP2X_CTRL_RIGHT) ||
        (new_pad == GP2X_CTRL_CROSS) || 
        (new_pad == GP2X_CTRL_CIRCLE))
    {
      int step = 0;

      if (new_pad & GP2X_CTRL_RIGHT) {
        step = 1;
      } else
      if (new_pad & GP2X_CTRL_LEFT) {
        step = -1;
      }

      switch (cur_menu_id ) 
      {
        case MENU_SET_SOUND      : mo5_snd_enable = ! mo5_snd_enable;
        break;              
        case MENU_SET_VIEW_FPS    : mo5_view_fps     = ! mo5_view_fps;
        break;              
        case MENU_SET_DISK_MODE  : mo5_disk_mode = ! mo5_disk_mode;
        break;              
        case MENU_SET_SPEED_LIMIT : psp_settings_menu_limiter( step );
        break;              
        case MENU_SET_AUTOFIRE  : psp_settings_menu_autofire( step );
        break;              
        case MENU_SET_SKIP_FPS   : psp_settings_menu_skip_fps( step );
        break;              
        case MENU_SET_ANALOG     : psp_joystick_cursor = ! psp_joystick_cursor;
        break;              
        case MENU_SET_JOYSTICK   : psp_active_joystick = ! psp_active_joystick;
        break;              
        case MENU_SET_RENDER     : psp_settings_menu_render( step );
        break;              
        case MENU_SET_LOAD       : psp_settings_menu_load(FMGR_FORMAT_SET);
                                   old_pad = new_pad = 0;
        break;              
        case MENU_SET_SAVE       : psp_settings_menu_save();
                                   old_pad = new_pad = 0;
        break;                     
        case MENU_SET_RESET      : psp_settings_menu_reset();
        break;                     
        case MENU_SET_BACK       : end_menu = 1;
        break;                     
      }

    } else
    if(new_pad & GP2X_CTRL_UP) {

      if (cur_menu_id > 0) cur_menu_id--;
      else                 cur_menu_id = MAX_MENU_SET_ITEM-1;

    } else
    if(new_pad & GP2X_CTRL_DOWN) {

      if (cur_menu_id < (MAX_MENU_SET_ITEM-1)) cur_menu_id++;
      else                                     cur_menu_id = 0;

    } else  
    if(new_pad & GP2X_CTRL_SQUARE) {
      /* Cancel */
      end_menu = -1;
    } else 
    if((new_pad & GP2X_CTRL_SELECT) == GP2X_CTRL_SELECT) {
      /* Back to MO5 */
      end_menu = 1;
    }
  }
 
  if (end_menu > 0) {
    psp_settings_menu_validate();
  }

  psp_kbd_wait_no_button();

  psp_sdl_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_sdl_flip();
  psp_sdl_clear_screen( PSP_MENU_BLACK_COLOR );
  psp_sdl_flip();

  return 1;
}


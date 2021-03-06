#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#include "gp2x_psp.h"
#include "gp2x_cpu.h"
#include <time.h>

# ifndef CLK_TCK
# define CLK_TCK  CLOCKS_PER_SEC
# endif

#define CPU_CLOCK_IDLE 60
#define CPU_CLOCK_STD 200

# define MO5_RENDER_NORMAL     0
# define MO5_LAST_RENDER       0

# define MO5_WIDTH  336
# define MO5_HEIGHT 216

# define SNAP_WIDTH   (MO5_WIDTH/2)
# define SNAP_HEIGHT  (MO5_HEIGHT/2)

# define MAX_PATH   256
# define MO5_MAX_SAVE_STATE 5

#define TRUE 1
#define FALSE 0


typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned int BOOL;

#include <SDL.h>

  typedef struct MO5_save_t {

    SDL_Surface    *surface;
    char            used;
    char            thumb;
    time_t          date;

  } MO5_save_t;

  typedef struct MO5_t {

    MO5_save_t mo5_save_state[MO5_MAX_SAVE_STATE];

    char mo5_save_name[MAX_PATH];
    char mo5_home_dir[MAX_PATH];
    int  psp_screenshot_id;
    int  psp_joystick_cursor;
    int  mo5_view_fps;
    int  mo5_current_fps;
    int  mo5_snd_enable;
    int  mo5_render_mode;
    int  psp_active_joystick;
    int  psp_skip_max_frame;
    int  psp_skip_cur_frame;
    int  mo5_speed_limiter;
    int  mo5_auto_fire;
    int  mo5_auto_fire_pressed;
    int  mo5_auto_fire_period;
    int  mo5_disk_mode;

  } MO5_t;

  extern MO5_t MO5;


//END_LUDO:
  extern void mo5_default_settings(void);
  extern int mo5_save_settings(void);
  extern int mo5_load_settings();
  extern int mo5_load_file_settings(char *FileName);

  extern void mo5_update_save_name(char *Name);
  extern void reset_save_name();
  extern void mo5_kbd_load(void);
  extern int mo5_kbd_save(void);
  extern void emulator_reset(void);
  extern int mo5_load_rom(char *FileName, int zip_format);

  extern int mo5_load_k7(char *FileName);
  extern int mo5_load_fd(char *FileName);
  extern int mo5_load_state(char *FileName);
  extern int mo5_snapshot_save_slot(int save_id);
  extern int mo5_snapshot_load_slot(int load_id);
  extern int mo5_snapshot_del_slot(int save_id);

  extern int psp_screenshot_mode;

#ifdef __cplusplus
}
#endif

#endif

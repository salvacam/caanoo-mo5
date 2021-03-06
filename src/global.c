/*
 *  Copyright (C) 2006 Ludovic Jacomme (ludovic.jacomme@gmail.com)
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
#include <string.h>
#include <stdlib.h>

#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <zlib.h>
#include "SDL.h"
#include "global.h"
#include "psp_mo5.h"
#include "psp_fmgr.h"
#include "psp_kbd.h"

  MO5_t MO5;

  int psp_screenshot_mode = 0;

void
psp_global_initialize()
{
  memset(&MO5, 0, sizeof(MO5_t));
  getcwd(MO5.mo5_home_dir,MAX_PATH);
  mo5_default_settings();
}

int 
SDL_main(int argc, char **argv)
{
  psp_global_initialize();
  psp_sdl_init();

  mo5_update_save_name("");
  mo5_load_settings();

  psp_sdl_black_screen();
  psp_mo5_initialize();
  psp_run_load_file();
  psp_mo5_main_loop();

  psp_sdl_exit(0);

  return 0;
}

//LUDO:
void
mo5_default_settings()
{
  //LUDO:
  MO5.mo5_snd_enable    = 1;
  MO5.mo5_render_mode   = MO5_RENDER_NORMAL;
  MO5.mo5_speed_limiter = 50;
  MO5.mo5_auto_fire = 0;
  MO5.mo5_auto_fire_period = 6;
  MO5.mo5_auto_fire_pressed = 0;
  MO5.psp_joystick_cursor  = 1;
  MO5.psp_active_joystick  = 0;
  MO5.psp_screenshot_id   = 0;
  MO5.mo5_disk_mode       = 1;
  MO5.mo5_view_fps        = 0;

}

static int
loc_mo5_save_settings(char *chFileName)
{
  FILE* FileDesc;
  int   error = 0;

  FileDesc = fopen(chFileName, "w");
  if (FileDesc != (FILE *)0 ) {

    fprintf(FileDesc, "psp_joystick_cursor=%d\n" , MO5.psp_joystick_cursor);
    fprintf(FileDesc, "psp_skip_max_frame=%d\n" , MO5.psp_skip_max_frame);
    fprintf(FileDesc, "mo5_snd_enable=%d\n"   , MO5.mo5_snd_enable);
    fprintf(FileDesc, "mo5_render_mode=%d\n"  , MO5.mo5_render_mode);
    fprintf(FileDesc, "mo5_speed_limiter=%d\n", MO5.mo5_speed_limiter);
    fprintf(FileDesc, "mo5_auto_fire_period=%d\n", MO5.mo5_auto_fire_period);
    fprintf(FileDesc, "mo5_view_fps=%d\n"     , MO5.mo5_view_fps);
    fprintf(FileDesc, "mo5_disk_mode=%d\n"     , MO5.mo5_disk_mode);

    fclose(FileDesc);

  } else {
    error = 1;
  }

  return error;
}

int
mo5_save_settings(void)
{
  char  FileName[MAX_PATH+1];
  int   error;

  error = 1;

  snprintf(FileName, MAX_PATH, "%s/set/%s.set", MO5.mo5_home_dir, MO5.mo5_save_name);
  error = loc_mo5_save_settings(FileName);

  return error;
}

static int
loc_mo5_load_settings(char *chFileName)
{
  char  Buffer[512];
  char *Scan;
  unsigned int Value;
  FILE* FileDesc;

  FileDesc = fopen(chFileName, "r");
  if (FileDesc == (FILE *)0 ) return 0;

  while (fgets(Buffer,512, FileDesc) != (char *)0) {

    Scan = strchr(Buffer,'\n');
    if (Scan) *Scan = '\0';
    /* For this #@$% of windows ! */
    Scan = strchr(Buffer,'\r');
    if (Scan) *Scan = '\0';
    if (Buffer[0] == '#') continue;

    Scan = strchr(Buffer,'=');
    if (! Scan) continue;

    *Scan = '\0';
    Value = atoi(Scan+1);

    if (!strcasecmp(Buffer,"psp_joystick_cursor")) MO5.psp_joystick_cursor = Value;
    else
    if (!strcasecmp(Buffer,"psp_skip_max_frame")) MO5.psp_skip_max_frame = Value;
    else
    if (!strcasecmp(Buffer,"mo5_snd_enable"))   MO5.mo5_snd_enable = Value;
    else
    if (!strcasecmp(Buffer,"mo5_render_mode"))  MO5.mo5_render_mode = Value;
    else
    if (!strcasecmp(Buffer,"mo5_speed_limiter"))  MO5.mo5_speed_limiter = Value;
    else
    if (!strcasecmp(Buffer,"mo5_view_fps"))  MO5.mo5_view_fps = Value;
    else
    if (!strcasecmp(Buffer,"mo5_auto_fire_period")) MO5.mo5_auto_fire_period = Value;
    else
    if (!strcasecmp(Buffer,"mo5_disk_mode"))     MO5.mo5_disk_mode = Value;
  }

  fclose(FileDesc);

  return 0;
}

int
mo5_load_settings()
{
  char  FileName[MAX_PATH+1];
  int   error;

  error = 1;

  snprintf(FileName, MAX_PATH, "%s/set/%s.set", MO5.mo5_home_dir, MO5.mo5_save_name);
  error = loc_mo5_load_settings(FileName);

  return error;
}

int
mo5_load_file_settings(char *FileName)
{
  return loc_mo5_load_settings(FileName);
}

static int 
loc_load_rom(char *TmpName)
{
  int error = psp_mo5_load_rom(TmpName);
  return error;
}

void
mo5_update_save_name(char *Name)
{
  char        TmpFileName[MAX_PATH];
  struct stat aStat;
  int         index;
  char       *SaveName;
  char       *Scan1;
  char       *Scan2;

  SaveName = strrchr(Name,'/');
  if (SaveName != (char *)0) SaveName++;
  else                       SaveName = Name;

  if (!strncasecmp(SaveName, "sav_", 4)) {
    Scan1 = SaveName + 4;
    Scan2 = strrchr(Scan1, '_');
    if (Scan2 && (Scan2[1] >= '0') && (Scan2[1] <= '5')) {
      strncpy(MO5.mo5_save_name, Scan1, MAX_PATH);
      MO5.mo5_save_name[Scan2 - Scan1] = '\0';
    } else {
      strncpy(MO5.mo5_save_name, SaveName, MAX_PATH);
    }
  } else {
    strncpy(MO5.mo5_save_name, SaveName, MAX_PATH);
  }

  if (MO5.mo5_save_name[0] == '\0') {
    strcpy(MO5.mo5_save_name,"default");
  }

  for (index = 0; index < MO5_MAX_SAVE_STATE; index++) {
    MO5.mo5_save_state[index].used  = 0;
    memset(&MO5.mo5_save_state[index].date, 0, sizeof(time_t));
    MO5.mo5_save_state[index].thumb = 0;

    snprintf(TmpFileName, MAX_PATH, "%s/save/sav_%s_%d.sta", MO5.mo5_home_dir, MO5.mo5_save_name, index);
    if (! stat(TmpFileName, &aStat)) 
    {
      MO5.mo5_save_state[index].used = 1;
      MO5.mo5_save_state[index].date = aStat.st_mtime;
      snprintf(TmpFileName, MAX_PATH, "%s/save/sav_%s_%d.png", MO5.mo5_home_dir, MO5.mo5_save_name, index);
      if (! stat(TmpFileName, &aStat)) 
      {
        if (psp_sdl_load_thumb_png(MO5.mo5_save_state[index].surface, TmpFileName)) {
          MO5.mo5_save_state[index].thumb = 1;
        }
      }
    }
  }
}

void
reset_save_name()
{
  mo5_update_save_name("");
}

void
mo5_kbd_load(void)
{
  char        TmpFileName[MAX_PATH + 1];
  struct stat aStat;

  snprintf(TmpFileName, MAX_PATH, "%s/kbd/%s.kbd", MO5.mo5_home_dir, MO5.mo5_save_name );
  if (! stat(TmpFileName, &aStat)) {
    psp_kbd_load_mapping(TmpFileName);
  }
}

int
mo5_kbd_save(void)
{
  char TmpFileName[MAX_PATH + 1];
  snprintf(TmpFileName, MAX_PATH, "%s/kbd/%s.kbd", MO5.mo5_home_dir, MO5.mo5_save_name );
  return( psp_kbd_save_mapping(TmpFileName) );
}


//Load Functions

typedef struct {
   char *pchZipFile;
   char *pchExtension;
   char *pchFileNames;
   char *pchSelection;
   int iFiles;
   unsigned int dwOffset;
} t_zip_info;

t_zip_info zip_info;

typedef unsigned int dword;
typedef unsigned char byte;
typedef unsigned short word;

#define ERR_FILE_NOT_FOUND       13
#define ERR_FILE_BAD_ZIP         14
#define ERR_FILE_EMPTY_ZIP       15
#define ERR_FILE_UNZIP_FAILED    16

FILE *pfileObject;
char *pbGPBuffer = NULL;

static dword
loc_get_dword(byte *buff)
{
  return ( (((dword)buff[3]) << 24) |
           (((dword)buff[2]) << 16) |
           (((dword)buff[1]) <<  8) |
           (((dword)buff[0]) <<  0) );
}

static void
loc_set_dword(byte *buff, dword value)
{
  buff[3] = (value >> 24) & 0xff;
  buff[2] = (value >> 16) & 0xff;
  buff[1] = (value >>  8) & 0xff;
  buff[0] = (value >>  0) & 0xff;
}

static word
loc_get_word(byte *buff)
{
  return( (((word)buff[1]) <<  8) |
          (((word)buff[0]) <<  0) );
}


int 
zip_dir(t_zip_info *zi)
{
   int n, iFileCount;
   long lFilePosition;
   dword dwCentralDirPosition, dwNextEntry;
   word wCentralDirEntries, wCentralDirSize, wFilenameLength;
   byte *pbPtr;
   char *pchStrPtr;
   dword dwOffset;

   iFileCount = 0;
   if ((pfileObject = fopen(zi->pchZipFile, "rb")) == NULL) {
      return ERR_FILE_NOT_FOUND;
   }

   if (pbGPBuffer == (char *)0) {
     pbGPBuffer = (char *)malloc( sizeof(byte) * 128*1024); 
   }

   wCentralDirEntries = 0;
   wCentralDirSize = 0;
   dwCentralDirPosition = 0;
   lFilePosition = -256;
   do {
      fseek(pfileObject, lFilePosition, SEEK_END);
      if (fread(pbGPBuffer, 256, 1, pfileObject) == 0) {
         fclose(pfileObject);
         return ERR_FILE_BAD_ZIP; // exit if loading of data chunck failed
      }
      pbPtr = (byte*)(pbGPBuffer + (256 - 22)); // pointer to end of central directory (under ideal conditions)
      while (pbPtr != (byte *)pbGPBuffer) {
         if (loc_get_dword(pbPtr) == 0x06054b50) { // check for end of central directory signature
            wCentralDirEntries = loc_get_word(pbPtr + 10);
            wCentralDirSize = loc_get_word(pbPtr + 12);
            dwCentralDirPosition = loc_get_dword(pbPtr + 16);
            break;
         }
         pbPtr--; // move backwards through buffer
      }
      lFilePosition -= 256; // move backwards through ZIP file
   } while (wCentralDirEntries == 0);
   if (wCentralDirSize == 0) {
      fclose(pfileObject);
      return ERR_FILE_BAD_ZIP; // exit if no central directory was found
   }
   fseek(pfileObject, dwCentralDirPosition, SEEK_SET);
   if (fread(pbGPBuffer, wCentralDirSize, 1, pfileObject) == 0) {
      fclose(pfileObject);
      return ERR_FILE_BAD_ZIP; // exit if loading of data chunck failed
   }

   pbPtr = (byte *)pbGPBuffer;
   if (zi->pchFileNames) {
      free(zi->pchFileNames); // dealloc old string table
   }
   zi->pchFileNames = (char *)malloc(wCentralDirSize); // approximate space needed by using the central directory size
   pchStrPtr = zi->pchFileNames;

   for (n = wCentralDirEntries; n; n--) {
      wFilenameLength = loc_get_word(pbPtr + 28);
      dwOffset = loc_get_dword(pbPtr + 42);
      dwNextEntry = wFilenameLength + loc_get_word(pbPtr + 30) + loc_get_word(pbPtr + 32);
      pbPtr += 46;
      char *pchThisExtension = zi->pchExtension;
      while (*pchThisExtension != '\0') { // loop for all extensions to be checked
         if (strncasecmp((char *)pbPtr + (wFilenameLength - 4), pchThisExtension, 4) == 0) {
            strncpy(pchStrPtr, (char *)pbPtr, wFilenameLength); // copy filename from zip directory
            pchStrPtr[wFilenameLength] = 0; // zero terminate string
            pchStrPtr += wFilenameLength+1;
            loc_set_dword((byte*)pchStrPtr, dwOffset);
            pchStrPtr += 4;
            iFileCount++;
            break;
         }
         pchThisExtension += 4; // advance to next extension
      }
      pbPtr += dwNextEntry;
   }
   fclose(pfileObject);

   if (iFileCount == 0) { // no files found?
      return ERR_FILE_EMPTY_ZIP;
   }

   zi->iFiles = iFileCount;
   return 0; // operation completed successfully
}

int 
zip_extract(char *pchZipFile, char *pchFileName, dword dwOffset, char *ext)
{
   int iStatus, iCount;
   dword dwSize;
   byte *pbInputBuffer, *pbOutputBuffer;
   FILE *pfileOut, *pfileIn;
   z_stream z;

   strcpy(pchFileName, MO5.mo5_home_dir);
   strcat(pchFileName, "/unzip.");
   strcat(pchFileName, ext);

   if (!(pfileOut = fopen(pchFileName, "wb"))) {
      return ERR_FILE_UNZIP_FAILED; // couldn't create output file
   }
   if (pbGPBuffer == (char *)0) {
     pbGPBuffer = (char *)malloc( sizeof(byte) * 128*1024); 
   }
   pfileIn = fopen(pchZipFile, "rb"); // open ZIP file for reading
   fseek(pfileIn, dwOffset, SEEK_SET); // move file pointer to beginning of data block
   fread(pbGPBuffer, 30, 1, pfileIn); // read local header
   dwSize = loc_get_dword((byte *)(pbGPBuffer + 18)); // length of compressed data
   dwOffset += 30 + loc_get_word((byte *)(pbGPBuffer + 26)) + loc_get_word((byte *)(pbGPBuffer + 28));
   fseek(pfileIn, dwOffset, SEEK_SET); // move file pointer to start of compressed data

   pbInputBuffer = (byte *)pbGPBuffer; // space for compressed data chunck
   pbOutputBuffer = pbInputBuffer + 16384; // space for uncompressed data chunck
   z.zalloc = (alloc_func)0;
   z.zfree = (free_func)0;
   z.opaque = (voidpf)0;
   iStatus = inflateInit2(&z, -MAX_WBITS); // init zlib stream (no header)
   do {
      z.next_in = pbInputBuffer;
      if (dwSize > 16384) { // limit input size to max 16K or remaining bytes
         z.avail_in = 16384;
      } else {
         z.avail_in = dwSize;
      }
      z.avail_in = fread(pbInputBuffer, 1, z.avail_in, pfileIn); // load compressed data chunck from ZIP file
      while ((z.avail_in) && (iStatus == Z_OK)) { // loop until all data has been processed
         z.next_out = pbOutputBuffer;
         z.avail_out = 16384;
         iStatus = inflate(&z, Z_NO_FLUSH); // decompress data
         iCount = 16384 - z.avail_out;
         if (iCount) { // save data to file if output buffer is full
            fwrite(pbOutputBuffer, 1, iCount, pfileOut);
         }
      }
      dwSize -= 16384; // advance to next chunck
   } while ((dwSize > 0) && (iStatus == Z_OK)) ; // loop until done
   if (iStatus != Z_STREAM_END) {
      return ERR_FILE_UNZIP_FAILED; // abort on error
   }
   iStatus = inflateEnd(&z); // clean up
   fclose(pfileIn);
   fclose(pfileOut);

   return 0; // data was successfully decompressed
}

int
mo5_load_rom(char *FileName, int zip_format)
{
  char *pchPtr;
  char *scan;
  char  SaveName[MAX_PATH+1];
  char  TmpFileName[MAX_PATH + 1];
  dword n;
  int   format;
  int   error;

  error = 1;

  if (zip_format) {

    zip_info.pchZipFile   = FileName;
    zip_info.pchExtension = ".rom";

    if (!zip_dir(&zip_info)) 
    {
      pchPtr = zip_info.pchFileNames;
      for (n = zip_info.iFiles; n != 0; n--) 
      {
        format = psp_fmgr_getExtId(pchPtr);
        if (format == FMGR_FORMAT_ROM) break;
        pchPtr += strlen(pchPtr) + 5; // skip offset
      }
      if (n) {
        strncpy(SaveName,pchPtr,MAX_PATH);
        scan = strrchr(SaveName,'.');
        if (scan) *scan = '\0';
        mo5_update_save_name(SaveName);
        zip_info.dwOffset = loc_get_dword((byte *)(pchPtr + (strlen(pchPtr)+1)));
        if (!zip_extract(FileName, TmpFileName, zip_info.dwOffset, scan+1)) {
          error = loc_load_rom(TmpFileName);
          remove(TmpFileName);
        }
      }
    }

  } else {
    strncpy(SaveName,FileName,MAX_PATH);
    scan = strrchr(SaveName,'.');
    if (scan) *scan = '\0';
    mo5_update_save_name(SaveName);
    error = loc_load_rom(FileName);
  }

  if (! error ) {
    mo5_kbd_load();
    mo5_load_settings();
  }

  return error;
}

int
mo5_load_k7(char *FileName)
{
  char  SaveName[MAX_PATH+1];
  char *scan;

  int error = Loadk7( FileName );
  if (! error) {
    strncpy(SaveName,FileName,MAX_PATH);
    scan = strrchr(SaveName,'.');
    if (scan) *scan = '\0';
    mo5_update_save_name( SaveName );
    mo5_kbd_load();
    mo5_load_settings();
  }
  return error;
}

int
mo5_load_fd(char *FileName)
{
  char  SaveName[MAX_PATH+1];
  char *scan;

  int error = Loadfd( FileName );
  if (! error) {
    strncpy(SaveName,FileName,MAX_PATH);
    scan = strrchr(SaveName,'.');
    if (scan) *scan = '\0';
    mo5_update_save_name( SaveName );
    mo5_kbd_load();
    mo5_load_settings();
  }
  return error;
}

static int
loc_load_state(char *filename)
{
  int error;
  error = ! dcmo5_load_state(filename);
  return error;
}

int
mo5_load_dos()
{
  return loc_load_state("./dos.stz");
}

int
mo5_load_state(char *FileName)
{
  char *pchPtr;
  char *scan;
  char  SaveName[MAX_PATH+1];
  char  TmpFileName[MAX_PATH + 1];
  dword n;
  int   format;
  int   error;

  error = 1;

  strncpy(SaveName,FileName,MAX_PATH);
  scan = strrchr(SaveName,'.');
  if (scan) *scan = '\0';
  mo5_update_save_name(SaveName);
  error = loc_load_state(FileName);

  if (! error ) {
    mo5_kbd_load();
    mo5_load_settings();
  }

  return error;
}

static int
loc_mo5_save_state(char *filename)
{
  int error;
  error = ! dcmo5_save_state(filename);
  return error;
}

int
mo5_snapshot_save_slot(int save_id)
{
  char      FileName[MAX_PATH+1];
  struct stat aStat;
  int       error;

  error = 1;

  if (save_id < MO5_MAX_SAVE_STATE) {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.stz", MO5.mo5_home_dir, MO5.mo5_save_name, save_id);
    error = loc_mo5_save_state(FileName);
    if (! error) {
      if (! stat(FileName, &aStat)) 
      {
        MO5.mo5_save_state[save_id].used  = 1;
        MO5.mo5_save_state[save_id].thumb = 0;
        MO5.mo5_save_state[save_id].date  = aStat.st_mtime;
        snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.png", MO5.mo5_home_dir, MO5.mo5_save_name, save_id);
        if (psp_sdl_save_thumb_png(MO5.mo5_save_state[save_id].surface, FileName)) {
          MO5.mo5_save_state[save_id].thumb = 1;
        }
      }
    }
  }

  return error;
}

int
mo5_snapshot_load_slot(int load_id)
{
  char  FileName[MAX_PATH+1];
  int   error;

  error = 1;

  if (load_id < MO5_MAX_SAVE_STATE) {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.stz", MO5.mo5_home_dir, MO5.mo5_save_name, load_id);
    error = loc_load_state(FileName);
  }
  return error;
}

int
mo5_snapshot_del_slot(int save_id)
{
  char  FileName[MAX_PATH+1];
  struct stat aStat;
  int   error;

  error = 1;

  if (save_id < MO5_MAX_SAVE_STATE) {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.stz", MO5.mo5_home_dir, MO5.mo5_save_name, save_id);
    error = remove(FileName);
    if (! error) {
      MO5.mo5_save_state[save_id].used  = 0;
      MO5.mo5_save_state[save_id].thumb = 0;
      memset(&MO5.mo5_save_state[save_id].date, 0, sizeof(time_t));

      snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.png", MO5.mo5_home_dir, MO5.mo5_save_name, save_id);
      if (! stat(FileName, &aStat)) 
      {
        remove(FileName);
      }
    }
  }

  return error;
}


void
mo5_treat_command_key(int mo5_idx)
{
  int new_render;

  switch (mo5_idx) 
  {
    case MO5_C_FPS: MO5.mo5_view_fps = ! MO5.mo5_view_fps;
    break;
    case MO5_C_JOY: MO5.psp_joystick_cursor = ! MO5.psp_joystick_cursor;
    break;
    case MO5_C_RENDER: 
      psp_sdl_black_screen();
      new_render = MO5.mo5_render_mode + 1;
      if (new_render > MO5_LAST_RENDER) new_render = 0;
      MO5.mo5_render_mode = new_render;
    break;
    case MO5_C_LOAD: psp_main_menu_load_current();
    break;
    case MO5_C_SAVE: psp_main_menu_save_current(); 
    break;
    case MO5_C_RESET: 
       psp_sdl_black_screen();
       psp_mo5_soft_reset(); 
       reset_save_name();
    break;
    case MO5_C_AUTOFIRE: 
       kbd_change_auto_fire(! MO5.mo5_auto_fire);
    break;
    case MO5_C_DECFIRE: 
      if (MO5.mo5_auto_fire_period > 0) MO5.mo5_auto_fire_period--;
    break;
    case MO5_C_INCFIRE: 
      if (MO5.mo5_auto_fire_period < 19) MO5.mo5_auto_fire_period++;
    break;
    case MO5_C_SCREEN: psp_screenshot_mode = 10;
    break;
  }
}

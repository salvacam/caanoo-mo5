
    Welcome to CaanooMO5

Original Author of MO5

  Daniel Coulom   (see http://dcmo5.free.fr/)

Author of the Caanoo and PSP port versions

  Ludovic Jacomme also known as Zx-81 (see http://zx81.zx81.free.fr/)


1. INTRODUCTION
   ------------

DCMO5 is one of the best emulator of the Thomson MO5 home computer running on
MacOS, Windows and Unix. The emulator faithfully imitates the MO5 model.

CaanooMO5 is a port on Caanoo of the version 11 of DCMO5.

Thanks to Raven for the graphics stuff.

This package is under GPL Copyright, read COPYING file for more information
about it.


2. INSTALLATION
   ------------

Unzip the zip file, and copy the content of the directory game to your
SD memory.

For any comments or questions on this version, please visit 
http://zx81.zx81.free.fr or http://www.gp32x.com/


3. CONTROL
   ------------

3.1 - Virtual keyboard

  In the MO5 emulator window, there are three different mapping 
  (standard, left trigger, and right Trigger mappings). 
  You can toggle between while playing inside the emulator using 
  the two trigger keys.

    -------------------------------------
    Caanoo       MO5          (standard)
  
    A          Space
    B          ENTER
    Y          1
    X          Joystick Fire
    Up         Up
    Down       Down
    Left       Left 
    Right      Right

    -------------------------------------
    Caanoo        MO5   (left trigger)
  
    A          FPS
    B          Swap joystick / cursor
    Y          Load state
    X          Save state
    Up         Joystick Up
    Down       Joystick Down
    Left       Joystick Left 
    Right      Joystick Right

    -------------------------------------
    Caanoo        MO5   (right trigger)
  
    A          STOP 
    B          ENTER
    Y          DELETE
    X          Auto-fire
    Up         Joystick Up
    Down       Joystick Down
    Left       Dec auto-fire
    Right      Inc auto-fire
  
    Joystick   Up, Down, Left, Rigth
    
    Press Select  to enter in emulator main menu.
    Press Start   open/close the On-Screen keyboard

  In the main menu

    RTrigger   Reset the emulator

    Y   Go Up directory
    X   Valid
    B   Valid
    A   Go Back to the emulator window

  The On-Screen Keyboard of "Danzel" and "Jeff Chen"

    Use the stick to choose one of the 9 squares, and
    use A, B, X, Y to choose one of the 4 letters 
    of the highlighted square.

    Use LTrigger and RTrigger to see other 9 squares 
    figures.


4. LOADING ROM FILES (ROM)
   ------------

If you want to load rom image in your emulator, you have to put your rom file
(with .zip or .rom file extension) on your Caanoo in the 'roms' directory. 

Then, while inside MO5 emulator, just press SELECT to enter in the emulator main
menu, choose "Load ROM", and then using the file selector choose one rom file to
load in your emulator.

To eject the ROM you will need to select Hard Reset option in the main menu.

5. LOADING TAPE FILES (K7)
------------

If you want to load tape image in the virtual drive of your emulator, you have
to put your tape file (with .k7 file extension) on your Caanoo memory stick in the
'k7' directory. 

Then, while inside MO5 emulator, just press SELECT to enter in the emulator main
menu, choose "Load K7", and then using the file selector choose one tape file to
load in your emulator.

To run the game of your tape file, you have to use the virtual keyboard 
(press START key) and type the MO5 command 'LOADM"",,R' followed by ENTER (Y).

You can also use directly the shortcut in the emulator menu (Command LOADM
option)

To RUN BASIC program, you have to type the MO5 command 'RUN""' followed by
ENTER (Y)

You can also use directly the shortcut in the emulator menu (Command RUN option)

It may happen that you need to rewind the tape using the Rewind K7 menu ...

The command 'LOAD""' (without M) is sometimes needed, when LOADM command doesn't
work.

If you use the MS Disk Basic (disk mode enabled), then you will have to put the
string "CASS:" in your 'LOADM', 'LOAD' or 'RUN' command as follows :

  LOADM"CASS:",,R 
  LOAD"CASS:",,R
  RUN"CASS:"

6. LOADING DISK FILES (FD)
------------

If you want to load floppy image in the virtual disk drive of your emulator, you
have to put your disk file (with .fd file extension) on your Caanoo in 'disc' directory. 

Then, while inside the emulator, just press SELECT to enter in the emulator main
menu, choose "Load Disc" and then using the file selector choose one disc file
to load in your emulator.

Use the same commands as described in the "Load tape" section.

The command AUTO is useful to run automatically the RUN"AUTO.BAT" command used
to most of all disk games.

If you want to specify the command to run for given games then you can do it in
the run.txt, using the following syntax :

  tapename=LOAD"CASS:RunName",,R
  tapename=LOADM"CASS:RunName",,R
  tapename=RUN"CASS:RunName"
  diskname=LOAD"RunName",,R
  diskname=LOADM"RunName",,R
  diskname=RUN"RunName"

7. LOADING SNAPSHOT FILES (STZ)
------------

I've modified original MO5 emulator to add a save state feature. The save state
format is specific to CaanooMO5 and PSPMO5, but it might be useful to run 
previously loaded games (using K7 and disk menu).

 
8. LOADING KEY MAPPING FILES
 ------------

For given games, the default keyboard mapping between Caanoo Keys and MO5 keys, is
not suitable, and the game can't be played on CaanooMO5.

To overcome the issue, you can write your own mapping file. Using notepad for
example you can edit a file with the .kbd extension and put it in the kbd
directory.

For the exact syntax of those mapping files, have a look on files generated in
the kbd directory (default.kbd etc ...).

After writting such keyboard mapping file, you can load them using the main menu
inside the emulator.

If the keyboard filename is the same as the rom file (.rom) then when you load
this rom file, the corresponding keyboard file is automatically loaded.

You can now use the Keyboard menu and edit, load and save your keyboard mapping
files inside the emulator. The Save option save the .kbd file in the kbd
directory using the "Game Name" as filename.  The game name is displayed on the
right corner in the emulator menu.

   
9. COMPILATION
   ------------

It has been developped under Linux using gcc with GPH-SDK.  To rebuild the
homebrew run the Makefile in the src archive.

In the src/doc directory you will find original readme and license files of
DCMO5.


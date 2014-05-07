FAS1 (FlashAdvance Slot-1) v1.7b
by Smiths (smiths/AT/emuholic/DOT/com)
http://www.emuholic.com
Source SVN @ http://code.google.com/p/fas1/
===========

based off of tftpds v2.5 by Sten Larsson (stonebone@planetunreal.com) and Gustav Munkby (grddev@gmail.com)
http://www.itstud.chalmers.se/~larssten/nds/
http://forum.gbadev.org/viewtopic.php?t=8682

----------------
+ ABOUT
+ HISTORY
+ USAGE
+ SRAM SUPPORT
+ BUGS/LIMITATIONS
----------------

About
-----
Old FlashAdvance cartridges for the Gameboy Advance used a Parallel Port device for flashing files to the cartridge from a PC. It's hard to find a newer PC (especially a laptop) with a Parallel Port. Based off of tftpds, FAS1 (FlashAdvance Slot-1) lets you place Gameboy Advance games in a folder "GBA" on a Slot-1 device (M3/R4/SuperCard/G6) and flash those files to a FlashAdvance 256 cartridge (only model tested) sitting in Slot-2 of your Nintendo DS.

Like tftpds, it uses Jeff Frohwein's GBA FLinker tool to do the writing, writing at a rate of 9.5s/meg (Flash Advance, Nintendo cartridges are slower). A full file (32 megs) takes about 4.8 minutes.

Also, it allows you to backup Bank 1 of your SRAM (64KB) to the cartridge in Slot-1. Restoring to the Flash Advance's Bank 1 is also supported.

History
------
v1.7b - Nintendo 128MB Support
+ Got 128MB Nintendo AGB Dev Cards working
+ Many thanks to nono14 for testing
+ Attempted to get 256MB Nintendo cards working, only erasing is possible due to lack of documentation
+ Code cleanup and optimization since it hadn't been touched in 6 years

v1.5 - DABURU SUPIIDO!
+ Write rate cut to 9.5s/meg!!!
+ That's right... 2X SPEED!
+ Added GBA Frame support for L+R+START Booting
 - Place 15bit BMP file named GBAFRAME.BMP in the /GBA folder

v1.4 - Boot me Back
+ Added Slot-2 booting with L+R+START

v1.2 - SRAM Sub-menu
+ SRAM restoring added to separate menu
+ Scans for SRAM .SAV files in /SAVES directory

v1.0 - Initial release
+ Flashes selected ROM to FAPro cartridge
+ Backs up SRAM bank 1 to Slot-1 device
+ Restores SRAM bank 1 from specific file


Usage
-----
1) FAS1flasher.nds can be put wherever you launch homebrew from on your Slot-1 Device. 

2) GBA ROMs (with extensions .gba [games] and .bin [Pogoshell] should be placed in a subfolder called "GBA" on your Slot-1 Device.

3) Launch FAS1Flasher.nds and you'll be presented with a menu on the bottom screen showing GBA ROMs found on your Slot-1 Device.

4) Selecting a title in the bottom menu will present you with on-screen information about the file, along with an estimated writing time.

5) Press "Start" to begin flashing the file (currently it flashes it at the first position of the FlashAdvance cartridge (offset 0). Later versions may let you change the starting position to create a multi-rom cartridge. Honestly, you should be using PogoShell for multi-rom compilations anyway.


SRAM Support
------
Pressing "Y" will switch between the ROM flashing and SAVE flashing menus. Saves must be put in a "SAVES" folder in the root of your Slot-1 device.  Saves are selected like ROMs, and are written to the first 64KB bank of SRAM on the FlashAdvance Pro cartridge.

Also, pressing "Select" will back up SRAM Bank 1 to a file called "BANK1.SAV" in a "SAVES" folder on your Slot-1 device.

Bugs/Limitations
------
Like tftpds, it only has been tested/writes to FlashAdvance Pro (aka Turbo FA). Support for other carts could be implemented. If you have any other flashcart, implement support for it and please submit the code! Those cartridges are still useful!

F2A Support is in UCON64, and the f2a.c file can be found here:
http://www.koders.com/c/fidC5AEE87302951FF4854331F1B326B3ADEEC05D48.aspx
Anyone willing to help on that?

FlashCart writing speed seems to be capped by the FLinker routines. Everything seems to have to be done a byte at a time, but it turns out GUI updates slow down the writing... who knew???

Also, seeing as how it's been forever since I've coded, and basically am re-learning everything based on what I need done... the source is probably very messy(???).

Thanks
------
Sten Larsson 	       For getting the ball rolling with tftpds for flashing
and Gustav Munkby      from within the DS, and the fwgui program.

Jeff Frohwein          Flashcart code from GBA FLinker
                       http://www.devrs.com/gba/software.php#misc

GuyFawkes	       Emuholic allows me to whore free stuff. Costis and JB helped
Costis		       in #emuholic with random things as my head began to hurt.
JustBurn

AceKard Team	       If I wasn't adding FAS1 support to the AceKard GUI, I wouldn't
 		       have altered the GUI updating speed and noticed the increase

AceKard Contributors   All the random source modifications you all made greatly helped
		       in adding more to this program

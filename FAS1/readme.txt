FAS1 (FlashAdvance Slot-1) v1.0
by Smiths (smiths/AT/emuholic/DOT/com)
http://www.emuholic.com
===========

based off of tftpds v2.5 by Sten Larsson (stonebone@planetunreal.com) and Gustav Munkby (grddev@gmail.com)
http://www.itstud.chalmers.se/~larssten/nds/
http://forum.gbadev.org/viewtopic.php?t=8682

About
-----
Old FlashAdvance cartridges for the Gameboy Advance used a Parallel Port device for flashing files to the cartridge from a PC. It's hard to find a newer PC (especially a laptop) with a Parallel Port. Based off of tftpds, FAS1 (FlashAdvance Slot-1) lets you place Gameboy Advance games in a folder "GBA" on a Slot-1 device (M3/R4/SuperCard/G6) and flash those files to a FlashAdvance 256 cartridge (only model tested) sitting in Slot-2 of your Nintendo DS.

Like tftpds, it uses Jeff Frohwein's GBA FLinker tool to do the writing, writing at a rate of 18s/meg. A full file (32 megs) takes about 9.5 minutes.

Also, it allows you to backup Bank 1 of your SRAM (64KB) to the cartridge in Slot-1. Restoring to the Flash Advance's Bank 1 will be implemented shortly.

Usage
-----
1) FAS1flasher.nds can be put wherever you launch homebrew from on your Slot-1 Device. 

2) GBA ROMs (with extensions .gba [games] and .bin [Pogoshell] should be placed in a subfolder called "GBA" on your Slot-1 Device.

3) Launch FAS1Flasher.nds and you'll be presented with a menu on the bottom screen showing GBA ROMs found on your Slot-1 Device.

4) Selecting a title in the bottom menu will present you with on-screen information about the file, along with an estimated writing time.

5) Press "Start" to begin flashing the file (currently it flashes it at the first position of the FlashAdvance cartridge (offset 0). Later versions may let you change the starting position to create a multi-rom cartridge. Honestly, you should be using PogoShell for multi-rom compilations anyway.

Also, pressing "Select" will back up SRAM Bank 1 to a file called "BANK1.SAV" in the root of your Slot-1 device.

Bugs/Limitations
------
Like tftpds, it only has been tested/writes to FlashAdvance Pro (aka Turbo FA). Support for other carts could be implemented. cartlib.c has the information for other cartridges, and flashcartfile.cpp contains the "file->write" command which is called from the main.cpp WriteRom() function.
If you have any other flashcart, implement support for it and please submit the code! Those cartridges are still useful!

Also, seeing as how it's been forever since I've coded, and basically am re-learning everything based on what I need done... the source is probably very messy(???).


Thanks
------
Sten Larsson 	       For getting the ball rolling with tftpds for flashing
and Gustav Munkby      from within the DS, and the fwgui program.

Jeff Frohwein          Flashcart code from GBA FLinker
                       http://www.devrs.com/gba/software.php#misc

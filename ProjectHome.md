Old FlashAdvance cartridges for the Gameboy Advance used a Parallel Port device for flashing files to the cartridge from a PC. It's hard to find a newer PC (especially a laptop) with a Parallel Port.
Based off of tftpds, FAS1 (FlashAdvance Slot-1) lets you place Gameboy Advance games in a folder "GBA" on a Slot-1 device (M3/[R4](https://code.google.com/p/fas1/source/detail?r=4)/SuperCard/G6) and flash those files to a FlashAdvance 256 cartridge (only model tested) sitting in Slot-2 of your Nintendo DS.

Like tftpds, it uses Jeff Frohwein's GBA FLinker tool to do the writing, writing at a rate of **9.5s/meg**. A full file (32 megs) takes about 4.8 minutes. This is comparable to the EZ Flash 3in1's NOR flashing, which is about 9s/meg! Hooray for speed!

Also, it allows you to backup Bank 1 of your SRAM (64KB) to the cartridge in Slot-1.
It also allows you to flash a 64KB save file from Slot-1 to the FlashAdvance cartridge.

PLEASE NOTE: THIS DOES **NOT** WORK WITH FLASH2ADVANCE (F2A) CARTRIDGES
The library for flashing a f2a cart is in ucon64, and can be found here:
http://www.koders.com/c/fidC5AEE87302951FF4854331F1B326B3ADEEC05D48.aspx

I'm sure someone can play with that and update FAS1 for f2a support. :)

> ![http://home.comcast.net/~gbatools/fas11.png](http://home.comcast.net/~gbatools/fas11.png)  ![http://home.comcast.net/~gbatools/fas12.png](http://home.comcast.net/~gbatools/fas12.png)
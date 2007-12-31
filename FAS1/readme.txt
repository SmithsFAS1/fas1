tftpds v2.5
===========
by Sten Larsson (stonebone@planetunreal.com)
and Gustav Munkby (grddev@gmail.com)
http://www.itstud.chalmers.se/~larssten/nds/
http://forum.gbadev.org/viewtopic.php?t=8682


About
-----
tftpds is a Nintendo DS application that recieves a file over
the wireless network and programs your Flash Advance Pro flash cart
inserted in the GBA slot. It can then boot .ds.gba and .gba-files.
It can also read/write sram.


Usage
-----
1. You need to have your Wifi Connection settings saved to firmware using
   for example Mario Kart DS.

2. Boot tftpds like any other homebrew application on the Nintendo DS.

3. Wait for "TFTP server ready..."

4. Transfer files to/from your PC using a TFTP client.
   Examples using the tftp client from:
   http://www.tftp-server.com/tftp-client.html
 
   a. Sending

      This writes the file 1 Megabyte from the beginning of the cart:

        tftp -b1432 192.168.0.2 put tftpds.ds.gba rom/100000/

      This writes the file tetattds.sav to the sram:

        tftp -b1432 192.168.0.2 put tetattds.sav ram

   b. Retrieving

      This retrieves the sram:

        tftp -b1432 192.168.0.2 get ram tetattds.sav

5. The cart will be scanned for things that looks like bootable files.
   Click on a file on the touch screen to boot it. The files will be
   displayed as

     <title>.<ds.gba or nds or gba> (<offset in hex>)


Paths
-----
* To access flash cart:
  /rom/<offset in hex>/<any filename>

  The offset must be a multiple of 0x40000 bytes (256 kilobytes), because
  this is the erase block size on the flash cart.
  Examples: C0000, 100000

* To access sram:
  /ram/<any filename>


Blocksize
---------
The parameter "-b1432" above specifies the blocksize. It affects the
transfer speed, and you can try different values. The value 1432 is suggested
by RFC 1783, the document describing the protocol, and there is probably
no point in using anything higher. Another value to try is 1024. The default
value is 512.


Gba menu
--------
tftpds.ds.gba can be booted on a gba. It will then display a simple menu which
allows you to boot gba programs on the flash cart. This is useful if you want
to play a gba game on you Gameboy Player or to play a game that makes use of
the gba link port (like PocketNES). Typically you will have tftpds.ds.gba as
the first thing on your flash cart, and it will now work on both ds and gba.

Implementation details: This works by appending a small multiboot menu rom to
the end of tftpds.ds.gba. A custom loader is used with dsbuild that detects
that it's a gba, copies the rom image to ram, and branches. This is possible
since the loader is executed on ARM7, the same cpu as on the gba, and both
Passme/Flashme and the gba boots the cart in the same way (by branching to
0x8000000).


Known issues
------------
* Only Flash Advance Pro (aka Turbo FA) is supported. Support for other carts
  could be implemented but I don't have any to test. If you have any other
  flashcart, implement support for it and send the changes to me, and then I
  will include it in the next release. If you've got GBAMP, SuperCard, etc.
  you should try bafio's "wifitransfer" instead:
  http://bafio.drunkencoders.com/

* Cannot retrieve files from flash cart.

* There is an error printed after transfer when using the client in the
  examples. It should be harmless though.

* There is currently no way to delete files from sram


Changelog
---------
2.5 (?)
  * Removed save system
  * Compiles with current libnds

2.4 beta (20070107)
  * Added save system
  * Implemented "blocksize" option.
  * Rewrote code for booting programs. It's much simpler now.
  * Disables wifi before booting
  * Fixed code so that it compiles with cvs version of libnds.

2.3 (20060801)
  * Updated to work with latest devkitPro and dswifi
  * Fixed a bug that erased too much on the cart if the file was a
    multiple of 256 kb
  * The path that curl sends seems to have changed
  * You can now scroll the list of boot items

2.1 (20060325)
  * Added gba menu. It's now possible to boot on a gba.

2.0 (20060303)
  * A GUI which lists files on cart
  * Boots gba files
  * Read and write sram
  * Using paths to select where to write
  * Fixed a bug in dswifi which prevented large files from working correctly

1.0 (20060222)
  * Initial release


Thanks
------
sgstair                For the wifi library
                       http://akkit.org/dswifi/

Jeff Frohwein          Flashcart code from GBA FLinker
                       http://www.devrs.com/gba/software.php#misc

bafio                  Info on how to read the firmware settings
                       http://forum.gbadev.org/viewtopic.php?p=68956#68956

Infantile Paralysiser  Info on how to boot a file (MoonShell sources)
                       http://mdxonline.dyndns.org/archives/nds/

Martin Korth           The best source of information about the Nintendo DS
                       http://nocash.emubase.de/gbatek.htm

NDSTech Wiki           Information about flash carts
                       http://www.bottledlight.com/ds/

Patrick Kwong          Information about the "blocksize" option.

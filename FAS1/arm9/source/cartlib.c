#include <stdio.h>
#include <nds.h>

// *** GBA flash cart support routines in GCC ***
//  This library allows programming FA/Visoly (both Turbo
// and non-Turbo) and official Nintendo flash carts. They
// can be used with the Flash Linker or can be called
// from GBA code to allow in-system flash cart programming.
// NOTE: ALL OF THESE ROUTINES MUST BE LOCATED IN GBA RAM
// IF THIS LIBRARY IS USED FOR IN-SYSTEM PROGRAMMING.
//
// by Jeff Frohwein, Started 2001-Aug-29
//
//  v1.0 - 2001-Sept-25 - Original release
//  v1.1 - 2001-Nov-13  - Slightly modified SetVisolyBackupRWMode by removing >>1.
//
// Routines -
//
//  void SetFAFlashRWMode (void)
//
//      Used to enable modifications of FA/Visoly cart flash chip(s)
//     and also used for CartTypeDetect routine. YOU MUST CALL THIS
//     ROUTINE BEFORE USING ANY OF THE OTHER FA/VISOLY ROUTINES.
//
//  u8 CartTypeDetect (void)
//      Return a value indicating type of cart installed:
//       0x00 = Hudson Cart,   0x2e = Standard ROM
//       0xe2 = N Flash Cart,  0xff = Unknown
//       0x17 = FA 64M,        0x96 = Turbo FA 64M
//       0x18 = FA 128M,       0x97 = Turbo FA 128M
//
//  u32 EraseNintendoFlashBlocks (u32 StartAddr, u32 BlockCount)
//
//      Erase official Nintendo flash cart blocks.
//       Ex: EraseNintendoFlashBlocks (0x8000000, 1);  // erase block 1
//           EraseNintendoFlashBlocks (0x8010000, 2);  // erase blocks 2 & 3
//
//  u32 EraseNonTurboFABlocks (u32 StartAddr, u32 BlockCount)
//
//      Erase older (non-Turbo) Visoly flash cart blocks.
//       Ex: EraseNonTurboFABlocks (0x8000000, 1);  // erase block 1
//           EraseNonTurboFABlocks (0x8020000, 2);  // erase blocks 2 & 3
//
//  u32 EraseTurboFABlocks (u32 StartAddr, u32 BlockCount)
//
//      Erase newer (Turbo) Visoly flash cart blocks.
//       Ex: EraseTurboFABlocks (0x8000000, 1);  // erase block 1
//           EraseTurboFABlocks (0x8040000, 2);  // erase blocks 2 & 3
//
//  u32 WriteNintendoFlashCart (u32 SrcAddr, u32 FlashAddr, u32 Length)
//
//      Write 2 x Length bytes to official Nintendo flash cart.
//       Ex: WriteNintendoFlashCart (SrcAddr, 0x8000000, 2)  // write 4 bytes
//
//  u32 WriteNonTurboFACart (u32 SrcAddr, u32 FlashAddr, u32 Length)
//
//      Write 32 x Length bytes to older (non-Turbo) Visoly flash cart.
//       Ex: WriteNonTurboFACart (SrcAddr, 0x8000000, 2)  // write 64 bytes
//
//  u32 WriteTurboFACart (u32 SrcAddr, u32 FlashAddr, u32 Length)
//
//      Write 64 x Length bytes to newer (Turbo) Visoly flash cart.
//       Ex: WriteTurboFACart (SrcAddr, 0x8000000, 2)  // write 128 bytes
//
// To reduce the library size and remove support for any
// of the following types, comment out one or more of
// the following lines using //
#define NONTURBO_FA_SUPPORT 1      // Visoly Non-Turbo flash carts
#define TURBO_FA_SUPPORT 1         // Visoly Turbo flash carts
#define NOA_FLASH_CART_SUPPORT 1   // Official Nintendo flash carts
//#define SET_CL_SECTION 1           // Enable setting code section for cartlib

//
//
//

#ifdef TURBO_FA_SUPPORT
 #define COMMON_FA_SUPPORT 1
#endif
#ifdef NONTURBO_FA_SUPPORT
 #define COMMON_FA_SUPPORT 1
#endif

#ifdef FLINKER
 // FLinker programming defines
 #define _MEM_INC 1
 #define FP_TIMEOUT1 0x4000
 #define FP_TIMEOUT2 0x8000
 #define FP_TIMEOUT3 0x80000
 #define _CART_START  0
 #define FLINKER_SET         l40226c ()
 #define READ_NTURBO_SR(a,b) WriteFlash (a, INTEL28F_READSR); \
                             outpb (SPPCtrlPort, 0);          \
                             b = (PPReadWord() & 0xff)
 #define READ_NTURBO_S(a)    outpb (SPPCtrlPort, 0);          \
                             a = (PPReadWord() & 0xff)
 #define READ_TURBO_SR(a)    WriteFlash (0, INTEL28F_READSR); \
                             PPWriteWord (INTEL28F_READSR);   \
                             outpb (SPPCtrlPort, 0);          \
                             a = PPReadWord() & 0xff;         \
                             a += (PPReadWord() & 0xff) << 8
 #define READ_TURBO_S(a)     outpb (SPPCtrlPort, 0);          \
                             a = PPReadWord() & 0xff;         \
                             a += (PPReadWord() & 0xff) << 8
 #define READ_TURBO_S2(a,b,c) outpb (SPPCtrlPort, 0);          \
                              b = PPReadWord () & 0x80;        \
                              c = PPReadWord () & 0x80
 #define WRITE_FLASH_NEXT(a,b) PPWriteWord (b)
 #define SET_CART_ADDR(a)    SetCartAddr (a);                 \
                             l4021d0 (3)
 #define CTRL_PORT_0         outpb (SPPCtrlPort, 0)
 #define CTRL_PORT_1         outpb (SPPCtrlPort, 1)

void WriteRepeat (int addr, int data, int count)
   {
   int i;
   for (i=0; i<count; i++)
      WriteFlash (addr, data);
   }

#else
 // GBA in-system programming defines
 #define _MEM_INC 2
 #define FP_TIMEOUT1 0x40000      // Probably could be MUCH smaller
 #define FP_TIMEOUT2 0x80000      // Probably could be MUCH smaller
 #define FP_TIMEOUT3 0x800000     // Probably could be MUCH smaller
 #define INTEL28F_BLOCKERASE 0x20
 #define INTEL28F_CLEARSR    0x50
 #define INTEL28F_CONFIRM    0xD0
 #define INTEL28F_QUIRY      0x98
 #define INTEL28F_READARRAY  0xff
 #define INTEL28F_READSR     0x70
 #define INTEL28F_RIC        0x90
 #define INTEL28F_WRTOBUF    0xe8

 #define SHARP28F_BLOCKERASE 0x20
 #define SHARP28F_CONFIRM    0xD0
 #define SHARP28F_WORDWRITE  0x10
 #define SHARP28F_READARRAY  0xff
// typedef     volatile unsigned char           vu8;
// typedef     volatile unsigned short int      vu16;
// typedef     volatile unsigned int            vu32;
// typedef     volatile unsigned long long int  vu64;

// typedef     unsigned char           u8;
// typedef     unsigned short int      u16;
// typedef     unsigned int            u32;
// typedef     unsigned long long int  u64;

 #define _CART_START 0x8000000
 #define _BACKUP_START 0xe000000
 #define FLINKER_SET         {}
 #define READ_NTURBO_SR(a,b) *(vu16 *)a = INTEL28F_READSR;     \
                             b = *(vu16 *)a
 #define READ_NTURBO_S(a)    a = *(vu16 *)_CART_START
 #define READ_TURBO_SR(a)    *(vu16 *)_CART_START = INTEL28F_READSR;     \
                             *(vu16 *)(_CART_START+2) = INTEL28F_READSR; \
                             a = *(vu16 *)_CART_START & 0xff;            \
                             a += (*(vu16 *)(_CART_START+2) & 0xff) << 8
 #define READ_TURBO_S(a)     a = *(vu16 *)_CART_START & 0xff;            \
                             a += (*(vu16 *)(_CART_START+2) & 0xff) << 8
 #define READ_TURBO_S2(a,b,c) b = *(vu16 *)a & 0x80;            \
                              c = *(vu16 *)(a+2) & 0x80
 #define WRITE_FLASH_NEXT(a,b) WriteFlash (a, b)
 #define SET_CART_ADDR(a)    {}
 #define CTRL_PORT_0         {}
 #define CTRL_PORT_1         {}
 #define CL_SECTION __attribute__ ((section (".iwram")))

  #ifdef SET_CL_SECTION
 // Prototypes to allow placing routines in any section
 void WriteFlash (u32 addr, u16 data) CL_SECTION;
 u16 ReadFlash (u32 addr) CL_SECTION;
 void WriteRepeat (u32 addr, u16 data, u16 count) CL_SECTION;
 void VisolyModePreamble (void) CL_SECTION;
 void SetVisolyFlashRWMode (void) CL_SECTION;
 void SetVisolyBackupRWMode (int i) CL_SECTION;
 u8 CartTypeDetect (void) CL_SECTION;
 u32 EraseNintendoFlashBlocks (u32 StartAddr, u32 BlockCount) CL_SECTION;
 u32 EraseNintendoFlashBlocks256 (u32 StartAddr, u32 BlockCount) CL_SECTION;
 u32 EraseNonTurboFABlocks (u32 StartAddr, u32 BlockCount) CL_SECTION;
 u32 EraseTurboFABlocks (u32 StartAddr, u32 BlockCount) CL_SECTION;
 u32 WriteNintendoFlashCart (u32 SrcAddr, u32 FlashAddr, u32 Length) CL_SECTION;
 u32 WriteNintendoFlashCart256 (u32 SrcAddr, u32 FlashAddr, u32 Length) CL_SECTION;
 u32 WriteNonTurboFACart (u32 SrcAddr, u32 FlashAddr, u32 Length) CL_SECTION;
 u32 WriteTurboFACart (u32 SrcAddr, u32 FlashAddr, u32 Length) CL_SECTION;
  #endif

void WriteFlash (u32 addr, u16 data) { *(vu16 *)addr = data; }
u16 ReadFlash (u32 addr) { return(*(vu16 *)addr); }

void WriteRepeat (u32 addr, u16 data, u16 count)
   {
   u16 i;
   for (i=0; i<count; i++)
      *(vu16 *)(_CART_START + (addr << 1)) = data;
   }
#endif

#ifdef COMMON_FA_SUPPORT
void VisolyModePreamble (void)  // 402438
   {
   FLINKER_SET;
   WriteRepeat (0x987654, 0x5354, 1);
   WriteRepeat ( 0x12345, 0x1234, 500);
   WriteRepeat (  0x7654, 0x5354, 1);
   WriteRepeat ( 0x12345, 0x5354, 1);
   WriteRepeat ( 0x12345, 0x5678, 500);
   WriteRepeat (0x987654, 0x5354, 1);
   WriteRepeat ( 0x12345, 0x5354, 1);
   WriteRepeat (0x765400, 0x5678, 1);
   WriteRepeat ( 0x13450, 0x1234, 1);
   WriteRepeat ( 0x12345, 0xabcd, 500);
   WriteRepeat (0x987654, 0x5354, 1);
   }

void SetVisolyFlashRWMode (void)
   {
   VisolyModePreamble ();
   WriteRepeat (0xf12345, 0x9413, 1);
   }

void SetVisolyBackupRWMode (int i)                     // 402550
   {
   VisolyModePreamble ();
   WriteRepeat (0xa12345, i, 1);
   }
#endif

// Cart Type Detect
//   Return a value indicating type of cart installed:
//     0xdc = Hudson Cart,   0x2e = Standard ROM Cart
//     0xe2 = N Flash Cart,  0xff = Unknown
//     0x17 = FA 64M,        0x96 = Turbo FA 64M
//     0x18 = FA 128M,       0x97 = Turbo FA 128M

u8 CartTypeDetect (void)
   {
   u8 type = 0xff;
   u16 Manuf, Device;

   WriteFlash (_CART_START, INTEL28F_RIC);  // Read Identifier codes from flash.
                                            // Works for intel 28F640J3A & Sharp LH28F320BJE.
   Manuf = ReadFlash(_CART_START);
   Device = ReadFlash(_CART_START+_MEM_INC);
	printf("Manuf:i-0x%x\n", Manuf);
	printf("Device:i-0x%x\n", Device);

   switch (Manuf)
      {
      case 0 :                          // Hudson Cart
         type = 0xdc;
         break;
      case 0x2e :                       // Standard ROM
         type = Manuf;
         break;
      case 0x89 :                       // Intel chips
         switch (Device)
            {
            case 0x16 :                 // i28F320J3A
            case 0x17 :                 // i28F640J3A
            case 0x18 :                 // i28F128J3A
               type = Device;
               break;
            default :
               // Check to see if this is a Visoly "Turbo" cart
               Device = ReadFlash(_CART_START+_MEM_INC+_MEM_INC);
               switch (Device)
                  {
                  case 0x16 :           // 2 x i28F320J3A
                  case 0x17 :           // 2 x i28F640J3A
                  case 0x18 :           // 2 x i28F128J3A
                     type = Device + 0x80;
                     break;
                  }
            }
         break;
      case 0xb0 :                       // Sharp chips
         switch (Device)
            {
            case 0xe2:
               type = Device;
               break;
            case 0xb0:
               type = Device;
               break;
            }
         break;
      }
   WriteFlash (_CART_START, INTEL28F_READARRAY);  // Set flash to normal read mode
   return (type);
   }

#ifdef NOA_FLASH_CART_SUPPORT
// Erase official Nintendo flash cart blocks
// Function returns true if erase was successful.
// Each block represents 64k bytes.

u32 EraseNintendoFlashBlocks (u32 StartAddr, u32 BlockCount)
   {
   int i=0;
   int j,k;

   for (k = 0; k < BlockCount; k++)
      {
      i = StartAddr + (k * 32768 * _MEM_INC);

		//printf("EraseNintendoBlocks:i-0x%x|bc-0x%x\n", i, BlockCount);
		//printf("EraseNintendoBlocks:j-0x%x|k-0x%x\n", j, k);

      do { READ_NTURBO_SR(i,j); } while ((j & 0x80)==0);
      WriteFlash (i, SHARP28F_BLOCKERASE);          // Erase a 64k byte block
      WriteFlash (i, SHARP28F_CONFIRM);             // Comfirm block erase
      }

		//printf("for loop exited[i:%i|bc:0x%x]\n", i, BlockCount);
   
   do { READ_NTURBO_SR(i,j); } while ((j & 0x80)==0);
	  //printf("Passed final READ_NTURBO section\n");

   WriteFlash (i, SHARP28F_READARRAY);             // Set normal read mode
	  //printf("Passed SHARP28F_READARRAY section\n");

   return(1);
   }

u32 EraseNintendoFlashBlocks256 (u32 StartAddr, u32 BlockCount)
   {
   int i=0;
   int j,k;

   for (k = 0; k < BlockCount; k++)
      {
      i = StartAddr + (k * 32768 * _MEM_INC);

	  do { READ_NTURBO_SR(i,j); } while ((j & 0x80)==0);
      WriteFlash (i, SHARP28F_BLOCKERASE);          // Erase a 64k byte block
      WriteFlash (i, SHARP28F_CONFIRM);             // Comfirm block erase
      }
  
   do { READ_NTURBO_SR(i,j); } while ((j & 0x80)==0);

   WriteFlash (i, SHARP28F_READARRAY);             // Set normal read mode

   return(1);
   }
   
#endif

#ifdef NONTURBO_FA_SUPPORT
// Erase older (non-Turbo) FA/Visoly flash cart blocks
// (Single flash chip)
// Function returns true if erase was successful.
// Each block represents 128k bytes.

u32 EraseNonTurboFABlocks (u32 StartAddr, u32 BlockCount)
   {
   u16 k;
   u16 Ready = 1;
   u32 i = 0;
   u32 Timeout;

   for (k = 0; k < BlockCount; k++)
      {
      i = StartAddr + (k * 65536 * _MEM_INC);

      Ready = 0;
      Timeout = FP_TIMEOUT2;

      while ((Ready == 0) && (Timeout != 0))
         {
         READ_NTURBO_SR(_CART_START,Ready);
         Ready &= 0x80;
         Timeout--;
         }

      if (Ready)
         {
         WriteFlash (i, INTEL28F_BLOCKERASE);          // Erase a 128k byte block
         Ready = 0;
         Timeout = FP_TIMEOUT3;

         while ((!Ready) && (Timeout != 0))
            {
            READ_NTURBO_S(Ready);
            Ready = (Ready == 0x80);
            Timeout--;
            }

         if (Ready)
            {
            WriteFlash (i, INTEL28F_CONFIRM);          // Comfirm block erase
            Ready = 0;
            Timeout = FP_TIMEOUT3;

            while ((!Ready) && (Timeout != 0))
               {
               READ_NTURBO_S(Ready);
               Ready = (Ready == 0x80);
               Timeout--;
               }

            if (Ready)
               {
               READ_NTURBO_SR(_CART_START,Ready);
               Ready = (Ready == 0x80);

               if (!Ready)
                  break;
               }
            else
               break;
            }
         else
            break;
         }
      else
         break;
      }

   if (!Ready)
      {
      WriteFlash (i, INTEL28F_CLEARSR);    // Clear flash status register
      }

   WriteFlash (i, INTEL28F_READARRAY);  // Set flash to normal read mode
   WriteFlash (i, INTEL28F_READARRAY);  // Set flash to normal read mode

   return (Ready != 0);
   }
#endif

#ifdef TURBO_FA_SUPPORT
// Erase newer (Turbo) FA/Visoly flash cart blocks
// (Dual chip / Interleave)
// Function returns true if erase was successful.
// Each block represents 256k bytes.

u32 EraseTurboFABlocks (u32 StartAddr, u32 BlockCount)
   {
   u16 j,k;
   u16 done1,done2;
   u16 Ready = 1;
   u32 i = 0;
   u32 Timeout;

   for (k = 0; k < BlockCount; k++)
      {
      i = StartAddr + (k * 131072 * _MEM_INC);

      Ready = 0;
      Timeout = FP_TIMEOUT2;

      while ((!Ready) && (Timeout != 0))
         {
         READ_TURBO_SR(j);
         Ready = (j == 0x8080);
         Timeout--;
         }

      if (Ready)
         {
         done1 = 0;
         done2 = 0;
         Ready = 0;
         Timeout = FP_TIMEOUT3;

         while ((!Ready) && (Timeout != 0))
            {
            if (done1 == 0) WriteFlash (i, INTEL28F_BLOCKERASE);       // Erase a 128k byte block in flash #1
            if (done2 == 0) WriteFlash (i+_MEM_INC, INTEL28F_BLOCKERASE);       // Erase a 128k byte block in flash #2

            READ_TURBO_S2(_CART_START,done1,done2);
            Ready = ((done1+done2) == 0x100);

            Timeout--;
            }

         if (Ready)
            {
            WriteFlash (i, INTEL28F_CONFIRM);          // Comfirm block erase in flash #1
            WriteFlash (i+_MEM_INC, INTEL28F_CONFIRM);            // Comfirm block erase in flash #2

            Ready = 0;
            Timeout = FP_TIMEOUT3;
            j = 0;

            while (((j & 0x8080) != 0x8080) && (Timeout != 0))
               {
               READ_TURBO_S(j);
               Ready = (j == 0x8080);

               Timeout--;
               }

            if (!Ready)
               {
               printf("timeout3\n");
               break;
               }
            }
         else
            {
            printf("timeout2\n");
            break;
            }
         }
      else
         {
         printf("timeout1\n");
         break;
         }
      }

   if (!Ready)
      {
      WriteFlash (i, INTEL28F_CLEARSR);
      WriteFlash (i+_MEM_INC, INTEL28F_CLEARSR);
      }

   WriteFlash (_CART_START, INTEL28F_READARRAY);
   WriteFlash (_CART_START+_MEM_INC, INTEL28F_READARRAY);
   WriteFlash (_CART_START, INTEL28F_READARRAY);
   WriteFlash (_CART_START+_MEM_INC, INTEL28F_READARRAY);

   return (Ready != 0);
   }
#endif

#ifdef NOA_FLASH_CART_SUPPORT
// Write 2 x Length bytes to official Nintendo flash cart.
// Function returns true if write was successful.

u32 WriteNintendoFlashCart (u32 SrcAddr, u32 FlashAddr, u32 Length)
   {
   int j;
   int LoopCount = 0;

   while (LoopCount < Length)
      {
		//printf("WNFC:i-0x%x|l-0x%x\n",LoopCount,Length);
		//printf("WNFC:fa-0x%x|srca-0x%x\n",FlashAddr,SrcAddr);
		//printf("\n");


      do { READ_NTURBO_SR(FlashAddr,j); } while ((j & 0x80)==0);

      WriteFlash (FlashAddr, SHARP28F_WORDWRITE);
      WriteFlash (FlashAddr, *(u16 *)SrcAddr);
      SrcAddr += 2;
      FlashAddr += _MEM_INC;
      LoopCount++;
      }

   do { READ_NTURBO_SR(FlashAddr,j); } while ((j & 0x80)==0);

   WriteFlash (_CART_START, SHARP28F_READARRAY);
//   CTRL_PORT_0;

   return (1);
   }

u32 WriteNintendoFlashCart256 (u32 SrcAddr, u32 FlashAddr, u32 Length)
   {
   int j;
   int LoopCount = 0;

   while (LoopCount < Length)
      {
		//printf("WNFC:i-0x%x|l-0x%x\n",LoopCount,Length);
		//printf("WNFC:fa-0x%x|srca-0x%x\n",FlashAddr,SrcAddr);
		//printf("\n");


      do { READ_NTURBO_SR(FlashAddr,j); } while ((j & 0x80)==0);

      WriteFlash (FlashAddr, SHARP28F_WORDWRITE);
      WriteFlash (FlashAddr, *(u16 *)SrcAddr);
      SrcAddr += 2;
      FlashAddr += _MEM_INC;
      LoopCount++;
      }

   do { READ_NTURBO_SR(FlashAddr,j); } while ((j & 0x80)==0);

   WriteFlash (_CART_START, SHARP28F_READARRAY);
//   CTRL_PORT_0;

   return (1);
   }
   
#endif

#ifdef NONTURBO_FA_SUPPORT
// Write 32 x Length bytes to older (non-Turbo) FA/Visoly flash cart.
// Function returns true if write was successful.

u32 WriteNonTurboFACart (u32 SrcAddr, u32 FlashAddr, u32 Length)
   {
   int Ready = 0;
   int Timeout = 0;
   int LoopCount = 0;

   while (LoopCount < Length)
      {
      Ready = 0;
      Timeout = FP_TIMEOUT1;

      while ((Ready == 0) && (Timeout != 0))
         {
         WriteFlash (FlashAddr, INTEL28F_WRTOBUF);
         READ_NTURBO_S(Ready);
         Ready &= 0x80;

         Timeout--;
         }

      if (Ready)
         {
         int i;

         WriteFlash (FlashAddr, 15);              // Write 15+1 16bit words

         SET_CART_ADDR(FlashAddr);

         for (i=0; i<16; i++)
            {
            WRITE_FLASH_NEXT(FlashAddr,*(u16 *)SrcAddr);
            SrcAddr += 2;
            FlashAddr += _MEM_INC;
            }

         WRITE_FLASH_NEXT(FlashAddr,INTEL28F_CONFIRM);

         Ready = 0;
         Timeout = FP_TIMEOUT1;

         while ((Ready == 0) && (Timeout != 0))
            {
            READ_NTURBO_SR(_CART_START,i);
            Ready = i & 0x80;

            Timeout--;
            }

         if (Ready)
            {
            if (i & 0x7f)
               {
               // One or more status register error bits are set
               CTRL_PORT_1;
               WriteFlash (0, INTEL28F_CLEARSR);
               Ready = 0;
               break;
               }
            }
         else
            {
            CTRL_PORT_1;
            WriteFlash (0, INTEL28F_CLEARSR);
            break;
            }
         }
      else
         {
         break;
         }

      LoopCount++;
      }
   WriteFlash (_CART_START, INTEL28F_READARRAY);  // Set flash to normal read mode
   WriteFlash (_CART_START, INTEL28F_READARRAY);  // Set flash to normal read mode
   return (Ready != 0);
   }
#endif

#ifdef TURBO_FA_SUPPORT
// Write 64 x Length bytes to newer (Turbo) FA/Visoly flash cart.
// Function returns true if write was successful.

u32 WriteTurboFACart (u32 SrcAddr, u32 FlashAddr, u32 Length)
   {
   int i,k;
   int done1,done2;
   int Timeout;
   int Ready = 0;
   int LoopCount = 0;

   while (LoopCount < Length)
      {
      done1 = 0;
      done2 = 0;
      Ready = 0;
      Timeout = 0x4000;

      while ((!Ready) && (Timeout != 0))
         {
         if (done1 == 0) WriteFlash (FlashAddr, INTEL28F_WRTOBUF);
         if (done2 == 0) WriteFlash (FlashAddr+_MEM_INC, INTEL28F_WRTOBUF);

         SET_CART_ADDR(FlashAddr);
         READ_TURBO_S2(FlashAddr,done1,done2);

         Ready = ((done1+done2) == 0x100);

         Timeout--;
         }

      if (Ready)
         {
         WriteFlash (FlashAddr, 15);              // Write 15+1 16bit words
         WRITE_FLASH_NEXT(FlashAddr+_MEM_INC,15);           // Write 15+1 16bit words

         SET_CART_ADDR(FlashAddr);

         for (i=0; i<32; i++)
            {
            WRITE_FLASH_NEXT(FlashAddr,*(u16 *)SrcAddr);
            SrcAddr += 2;
            FlashAddr += _MEM_INC;
            }
         WRITE_FLASH_NEXT(FlashAddr,INTEL28F_CONFIRM);
         WRITE_FLASH_NEXT(FlashAddr+_MEM_INC,INTEL28F_CONFIRM);

         Ready = 0;
         Timeout = 0x4000;
         k = 0;

         while (((k & 0x8080) != 0x8080) && (Timeout != 0))
            {
            READ_TURBO_S(k);
            Ready = (k == 0x8080);

            Timeout--;
            }

         if (!Ready)
            break;
         }
      else
         break;
      LoopCount++;
      }

   WriteFlash (_CART_START, INTEL28F_READARRAY);
   CTRL_PORT_0;
   WriteFlash (_CART_START+_MEM_INC, INTEL28F_READARRAY);
   CTRL_PORT_0;

   if (!Ready)
      {
      WriteFlash (_CART_START, INTEL28F_CLEARSR);
      WriteFlash (_CART_START+_MEM_INC, INTEL28F_CLEARSR);
      }
   return (Ready != 0);
   }
#endif



// code from http://www.bottledlight.com/ds/index.php/Hardware/FlashCartridges
void VisolySetFlashBaseAddress(u32 offset)
{
	u16 base = ((offset >> 22) & 3) | ((offset >> 22) & 4) | ((offset >> 12) & 0x3F8);
	VisolyModePreamble();
	WriteRepeat(0xB5AC97, base, 1);
}

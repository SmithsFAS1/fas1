#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern void VisolyModePreamble (void);
extern void WriteRepeat (u32 addr, u16 data, u16 count);
extern void SetVisolyFlashRWMode (void);
extern u8 CartTypeDetect (void);
extern u32 EraseTurboFABlocks (u32 StartAddr, u32 BlockCount);
extern u32 WriteTurboFACart(u32 SrcAddr, u32 FlashAddr, u32 Length);

extern void VisolySetFlashBaseAddress(u32 offset);

#ifdef __cplusplus
}
#endif

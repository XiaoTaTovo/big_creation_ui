#ifndef UI_CONFIG_H
#define UI_CONFIG_H
/* Edit this switch, then rebuild both projects and download the new Merge.bin.
 * 0: portrait (170x320, MADCTL direction 2)
 * 1: landscape (320x170, MADCTL direction 1)
 * Build script can override with -DUI_LANDSCAPE=0/1.
 * Runtime rotation is intentionally unsupported while DMA is active.
 */
#ifndef UI_LANDSCAPE
#define UI_LANDSCAPE 0
#endif
#if UI_LANDSCAPE != 0 && UI_LANDSCAPE != 1
#error UI_LANDSCAPE_must_be_0_or_1
#endif
#define LCD_NATIVE_W 170U
#define LCD_NATIVE_H 320U
#if UI_LANDSCAPE
#define LCD_W LCD_NATIVE_H
#define LCD_H LCD_NATIVE_W
#define LCD_ORIENTATION 1U
#define LCD_OFFSET_X 0U
#define LCD_OFFSET_Y 35U
#else
#define LCD_W LCD_NATIVE_W
#define LCD_H LCD_NATIVE_H
#define LCD_ORIENTATION 2U
#define LCD_OFFSET_X 35U
#define LCD_OFFSET_Y 0U
#endif
#define UI_TEST_UPDATE_MS 100U
#define UI_TEST_PAGE_MS 8000U
#endif

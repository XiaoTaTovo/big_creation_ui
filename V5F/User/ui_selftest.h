#ifndef UI_SELFTEST_H
#define UI_SELFTEST_H
/* Requires a default LVGL display and tick. Call from LVGL/main context only.
 * Start replaces any prior self-test; stop frees only its own objects/timer.
 */
void ui_selftest_start(void);
void ui_selftest_stop(void);
#endif

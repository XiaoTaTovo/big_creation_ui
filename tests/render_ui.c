#include "lvgl.h"
#include "ui_selftest.h"
#include "ui_config.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
static uint16_t frame[LCD_W*LCD_H];
static uint16_t buf[LCD_W*10];
static unsigned flushes;
static void flush(lv_display_t *d,const lv_area_t *a,uint8_t *pixels)
{
    assert(a->x1>=0 && a->y1>=0 && a->x2<LCD_W && a->y2<LCD_H);
    uint16_t *p=(uint16_t*)pixels;
    for(int y=a->y1;y<=a->y2;y++) for(int x=a->x1;x<=a->x2;x++)frame[y*LCD_W+x]=*p++;
    ++flushes;lv_display_flush_ready(d);
}
static void save(const char *path)
{
    FILE *f=fopen(path,"wb");assert(f);
    fprintf(f,"P6\n%d %d\n255\n",LCD_W,LCD_H);
    for(unsigned i=0;i<LCD_W*LCD_H;i++) {
        unsigned v=frame[i];unsigned char rgb[3]={(v>>11)*255/31,((v>>5)&63)*255/63,(v&31)*255/31};
        fwrite(rgb,1,3,f);
    }
    fclose(f);
}
int main(int argc,char **argv)
{
    assert(argc==3);lv_init();
    lv_display_t *d=lv_display_create(LCD_W,LCD_H);
    lv_display_set_color_format(d,LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(d,buf,NULL,sizeof(buf),LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(d,flush);
    uint32_t initial=lv_obj_get_child_count(lv_screen_active());
    ui_selftest_start();ui_selftest_start();
    assert(lv_obj_get_child_count(lv_screen_active())==initial+1);
    for(int i=0;i<5;i++){lv_tick_inc(100);lv_timer_handler();}
    save(argv[1]);
    for(int i=0;i<90;i++){lv_tick_inc(100);lv_timer_handler();}
    save(argv[2]);assert(flushes>30);
    for(int i=0;i<600;i++){lv_tick_inc(100);lv_timer_handler();}
    ui_selftest_stop();ui_selftest_stop();
    assert(lv_obj_get_child_count(lv_screen_active())==initial);
    puts("PASS: bounds, rendering, 69.5s updates, repeated start/stop");return 0;
}

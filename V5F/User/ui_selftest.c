#include "ui_selftest.h"
#include "ui_config.h"
#include "lvgl.h"
#include <stdint.h>

static lv_obj_t *root, *status, *bar, *counter;
static lv_timer_t *timer;
static uint32_t started, page_started, ticks;
static unsigned page;
static int w, h;
static lv_point_precise_t arrow_pts[5];

static lv_obj_t *box(lv_obj_t *parent, int x, int y, int bw, int bh, uint32_t color)
{
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_style_all(o);
    lv_obj_set_scrollable(o, false);
    lv_obj_set_pos(o, x, y); lv_obj_set_size(o, bw, bh);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    return o;
}
static lv_obj_t *text(int x, int y, int width, const char *s)
{
    lv_obj_t *o = lv_label_create(root);
    lv_obj_set_pos(o,x,y); lv_obj_set_width(o,width);
    lv_label_set_long_mode(o,LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(o,&lv_font_montserrat_14,0);
    lv_obj_set_style_text_color(o,lv_color_white(),0);
    lv_label_set_text(o,s); return o;
}
static void create_page(void)
{
    lv_obj_clean(root); status=bar=counter=NULL;
    /* Draw border inside the visible region and mark all four corners. */
    box(root,1,1,w-2,1,0xFFFFFF); box(root,1,h-2,w-2,1,0xFFFFFF);
    box(root,1,1,1,h-2,0xFFFFFF); box(root,w-2,1,1,h-2,0xFFFFFF);
    text(4,4,24,"TL");text(w-25,4,23,"TR");
    text(4,h-20,24,"BL");text(w-25,h-20,23,"BR");
    lv_obj_t *title=text(30,5,w-60,page?"DYNAMIC":"STATIC");
    lv_obj_set_style_text_align(title,LV_TEXT_ALIGN_CENTER,0);
    if(!page) {
        static const uint32_t colors[]={0xFF0000,0x00FF00,0x0000FF,0xFFFFFF,0x000000};
        int unit=(w-16)/5;
        for(unsigned i=0;i<5;i++) box(root,8+i*unit,28,unit-2,18,colors[i]);
        text(8,50,w-16,"0123456789 ABC");
        text(8,70,w-16,"12.00 V   1.67 A");
        lv_obj_t *rounded=box(root,8,94,44,24,0x22AACC);
        lv_obj_set_style_radius(rounded,7,0);
        lv_obj_t *dot=box(root,64,94,22,22,0x22CC55);
        lv_obj_set_style_radius(dot,LV_RADIUS_CIRCLE,0);
        arrow_pts[0]=(lv_point_precise_t){98,106};
        arrow_pts[1]=(lv_point_precise_t){w-12,106};
        arrow_pts[2]=(lv_point_precise_t){w-22,96};
        arrow_pts[3]=(lv_point_precise_t){w-12,106};
        arrow_pts[4]=(lv_point_precise_t){w-22,116};
        lv_obj_t *arrow=lv_line_create(root);
        lv_line_set_points(arrow,arrow_pts,5);
        lv_obj_set_style_line_color(arrow,lv_color_white(),0);
        lv_obj_set_style_line_width(arrow,2,0);
        text(8,123,w-16,UI_LANDSCAPE?"320 x 170  LAND":"170 x 320  PORT");
    } else {
        status=text(8,30,w-16,"WAITING");
        counter=text(8,55,w-16,"999");
        box(root,8,85,w-16,16,0x304050);
        bar=box(root,8,85,1,16,0x22CC55);
        text(8,111,w-16,"TEST DATA ONLY");
    }
}
static void update(lv_timer_t *t)
{
    (void)t;
    uint32_t now=lv_tick_get(); ++ticks;
    if((uint32_t)(now-page_started)>=UI_TEST_PAGE_MS) {
        page^=1U;page_started=now;create_page();
    }
    if(page) {
        const char *words[]={"WAITING","READY","WAITING","--"};
        lv_label_set_text(status,words[(ticks/10U)%4U]);
        const char *numbers[]={"999","1000","1","12.00 V"};
        lv_label_set_text_fmt(counter,"%s  %lus",numbers[(ticks/10U)%4U],
                              (unsigned long)((uint32_t)(now-started)/1000U));
        unsigned step=ticks%40U; if(step>20U) step=40U-step;
        lv_obj_set_width(bar,1+(w-17)*(int)step/20);
    }
}
void ui_selftest_stop(void)
{
    if(timer) {lv_timer_delete(timer);timer=NULL;}
    if(root) {lv_obj_delete(root);root=NULL;}
    status=bar=counter=NULL;
}
void ui_selftest_start(void)
{
    ui_selftest_stop();
    lv_display_t *d=lv_display_get_default();
    if(!d) return;
    w=lv_display_get_horizontal_resolution(d);
    h=lv_display_get_vertical_resolution(d);
    root=box(lv_screen_active(),0,0,w,h,0x101820);
    ticks=page=0; started=page_started=lv_tick_get();
    create_page(); timer=lv_timer_create(update,UI_TEST_UPDATE_MS,NULL);
}

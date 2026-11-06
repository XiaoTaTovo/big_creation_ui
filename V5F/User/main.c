#include "debug.h"
#include "lcd.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "ui_selftest.h"

static void LVGL_TIM2_Init(void)
{
    RCC_ClocksTypeDef clocks = {0};
    TIM_TimeBaseInitTypeDef tim = {0};

    RCC_GetClocksFreq(&clocks);

    /* H417 �� TIM2 ʹ�� HB1 ����ʱ�� */
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM2, ENABLE);

    TIM_Cmd(TIM2, DISABLE);

    /*
     * ������Ƶ�ʷ�Ƶ�� 1MHz��
     * ÿ����һ��Ϊ 1us������ 1000 ��Ϊ 1ms��
     * Ҫ��ʱ��Ƶ���ܱ� 1000000 ������
     */
    tim.TIM_Prescaler =
        (uint16_t)(clocks.HCLK_Frequency / 1000000UL - 1);
    tim.TIM_Period = 1000 - 1;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM2, &tim);

    TIM_SetCounter(TIM2, 0);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}


int main(void)
{
    SystemAndCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    LCD_Init();
    LCD_LED_SET;
    lv_init();
    lv_port_disp_init();
    LVGL_TIM2_Init();
    ui_selftest_start();
    while(1) {
        lv_port_disp_poll();
        lv_timer_handler();
        Delay_Ms(5);
    }
}

/*********************************************************************
 * File Name          : config.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : file for Config.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__

/* LCD Screen Definition */
//170ÁÐ£¬320ÐÐ

#include "ui_config.h"

/* Buffer Definition */
#define COLOR_BYTE        (2u)
#define FILL_BUFFER_COUNT (256u * 1u)
#define FILL_BUFFER_SIZE  (FILL_BUFFER_COUNT * COLOR_BYTE)

/* SPI Flash Speed Definition */
#define SPI_NORMAL_MODE   1
#define SPI_FAST_MODE     2
#define SPI_SPEED_MODE    SPI_NORMAL_MODE

/* SPI LCD_SCREEN GPIO Definition */


#define LCD_LED_RCC       RCC_HB2Periph_GPIOD
#define LCD_LED_PORT      GPIOD
#define LCD_LED_PIN       (GPIO_Pin_8)

#define LCD_DC_RCC       RCC_HB2Periph_GPIOB
#define LCD_DC_PORT       GPIOB
#define LCD_DC_PIN        (GPIO_Pin_6)

#define LCD_RST_RCC       RCC_HB2Periph_GPIOB
#define LCD_RST_PORT      GPIOB
#define LCD_RST_PIN       (GPIO_Pin_5)

#define LCD_CS_RCC       RCC_HB2Periph_GPIOB
#define LCD_CS_PORT       GPIOB
#define LCD_CS_PIN        (GPIO_Pin_7)

/* SPI1 alternate-function pin selection. Change all five fields together. */
#define LCD_SCK_RCC RCC_HB2Periph_GPIOA
#define LCD_SCK_PORT GPIOA
#define LCD_SCK_PIN GPIO_Pin_5
#define LCD_SCK_SOURCE GPIO_PinSource5
#define LCD_SCK_AF GPIO_AF5
#define LCD_MOSI_RCC RCC_HB2Periph_GPIOA
#define LCD_MOSI_PORT GPIOA
#define LCD_MOSI_PIN GPIO_Pin_7
#define LCD_MOSI_SOURCE GPIO_PinSource7
#define LCD_MOSI_AF GPIO_AF5
#endif

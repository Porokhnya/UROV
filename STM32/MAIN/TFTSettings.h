#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// НАСТРОЙКИ TFT
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_BUTTON_COLORS WHITE, LGRAY, WHITE, RED, BLUE // цвета для кнопок
#define TFT_BUTTON_COLORS2 WHITE, LGRAY, WHITE, RED, 0x4DC9 // цвета для кнопок
#define TFT_BACK_COLOR BLACK // цвет заливки экрана
#define TFT_FONT_COLOR WHITE // цвет шрифта по умолчанию
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// touch settings
// настройки тачскрина
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_TOUCH_CLK_PIN 6
#define TFT_TOUCH_CS_PIN 5
#define TFT_TOUCH_DIN_PIN 4
#define TFT_TOUCH_DOUT_PIN 3
#define TFT_TOUCH_IRQ_PIN 2
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_INIT_DELAY 0 // сколько миллисекунд (при необходимости) ждать перед отсылкой первых команд на дисплей
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_ORIENTATION LANDSCAPE // ориентация экрана
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TOUCH_PRECISION PREC_HI // чувствительность тача
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ФАЙЛ НАСТРОЕК ПРОШИВКИ
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "RS485.h"
#include "FileUtils.h"
#include <Wire.h>
#include "TinyVector.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// версия ПО
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SOFTWARE_VERSION                              "v.12.03.21"


//#define _MY_DEBUG // закомментировать для выключения отладочной информации
//#define _SETTINGS_LOAD_DEBUG // отладочный режим загрузки настроек из EEPROM (выводит ошибки чтения заголовков записей)
#define DEBUG_SERIAL Serial // какой Serial использовать для дебаг-режима


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// тип используемой EEPROM (МИНИМУМ 50 Кб объёмом, т.е. МИНИМУМ AT24C1024)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define EEPROM_CLASS AT24C1024 // поддерживаемые типы: AT24C128, AT24C256, AT24C512, AT24C1024 

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// пересчет напряжения в ток
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define COEFF_1  5.0 // первый коэффициент по пересчёту тока
#define CURRENT_NUM_SAMPLES 10 // за сколько измерений вычислять ток для главного экрана
#define CURRENT_MIN_TREAT_AS_ZERO  100 // минимальное значение тока, которое интерпретируется как 0
#define  CURRENT_DIVIDER  1000 // делитель для пересчёта напряжения в ток

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// НАСТРОЙКИ SD
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define USE_SDFAT // РАСКОММЕНТИРОВАТЬ, ЕСЛИ НАДО ИСПОЛЬЗОВАТЬ SdFat вместо SdFatSdio
#define SDCARD_CS_PIN 52 // номер пина Chip Select для SD-модуля (АКТУАЛЬНО ПРИ ИСПОЛЬЗОВАНИИ SdFat)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// проверка быстродействия SD
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MIN_SD_WRITE_SPEED 1500 // минимальная допустимая скорость записи на SD (Кб/с)
#define MIN_SD_READ_SPEED 3000 // минимальная допустимая скорость чтения с SD (Кб/с)
// если проверки SD не пройдены - будет мигать красный светодиод на передней панели с интервалом, указанным ниже
#define BAD_SD_BLINK_INTERVAL 500 // интервал мигания светодиода индикации плохой SD-карточки, миллисекунд

// МЕНЯТЬ ИМЯ РЕЗУЛЬТИРУЮЩЕГО ФАЙЛА ЗДЕСЬ, ЕСЛИ ХОЧЕТСЯ ЗАПУСТИТЬ ТЕСТ ПРИ ПЕРЕЗАЛИВКЕ ПРОШИВКИ, НО ФАЙЛ С РЕЗУЛЬТАТАМИ НА SD УЖЕ ЕСТЬ !!!
#define BENCH_RESULTS_FILENAME "SPEED.RES" // имя файла с результатами проверки SD (создаётся только при прохождении всех тестов на чтение/запись)

#define DISABLE_SAVE_BENCH_FILE // раскомментировать, если не надо сохранять результаты проверки быстродействия на SD в файл

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// выключение модулей из прошивки
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define _RTC_OFF // выключить работу с часами реального времени
//#define _RS485_OFF // выключить RS-485 на время тестирования
//#define _ADC_OFF // выключить АЦП на время тестирования
#define _YIELD_OFF // выключить функцию yield на время тестирования
//#define _COM_COMMANDS_OFF // выключить обработку команд из COM-порта на время тестирования
//#define _DRAW_GRID_OFF // выключить отрисовку сетки графика на время тестирования
//#define _SD_OFF // выключить поддержку SD на время тестирования
//#define _DELAYED_EVENT_OFF // отключить CoreDelayedEvent на время тестирования
//#define _CORE_TEMP_OFF // отключить периодическое получение температуры ядра на время тестирования
//#define _CURRENT_COLLECT_OFF // выключить сбор информации по току из прошивки

//#define _SCREEN_2_OFF // отключить второй экран на время тестирования
//#define _SCREEN_3_OFF // отключить экран перехода в меню настройки, на время тестирования
//#define _SCREEN_4_OFF // отключить экран меню установки даты/времени, на время тестирования
//#define _SCREEN_5_OFF // отключить создание экрана установки времени на время тестирования
//#define _SCREEN_6_OFF // отключить создание экрана установки даты на время тестирования

//#define _DISABLE_MAIN_SCREEN_BUTTONS // убрать кнопки с главного экрана, на время тестирования
//#define _DISABLE_DRAW_RAM_ON_SCREEN // убрать вывод свободной памяти на главном экране
//#define _DISABLE_DRAW_TIME // не рисовать время на главном экране
//#define _DISABLE_DRAW_VOLTAGE // не рисовать вольтаж на главном экране
//#define _DISABLE_DRAW_SOFTWARE_VERSION // не рисовать версию ПО на главном экране

//#define DISABLE_CATCH_ENCODER_DIRECTION // не определять направление движения энкодера

//#define IGNORE_ROD_POSITION // ЗАКОММЕНТИРОВАТЬ, ЕСЛИ НЕ НАДО ИГНОРИРОВАТЬ ПОЗИЦИЮ ШТАНГИ ПРИ СРАВНЕНИИ ЭТАЛОНОВ !!!

// если эта настройка раскомментирована, то при сравнении прерывания с эталоном сравнивается только количество импульсов.
// иначе - сравнивается, помимо этого - ещё и дельта времени по каждому импульсу
#define ETHALON_COMPARE_ONLY_PULSES_COUNT
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SERIAL_SPEED            115200 // скорость работы с Serial
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки пинов прерываний
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// все настройки пинов энкодера - должны соответствовать одному порту! Например, если ENCODER_PIN1 == PC4, то настройка ENCODER_PORT должна быть GPIOC, а настройка
// ENCODER_PIN_A - должна быть GPIO_PIN_4 !!!
#define ENCODER_PIN1                    PC4 // номер пина, на котором ловятся импульсы от энкодера штанги
#define ENCODER_PIN2                    PC2 // номер пина 2, на котором ловятся импульсы от энкодера штанги
#define ENCODER_PORT                    GPIOC // порт, на который подключены выводы энкодера
#define ENCODER_PIN_A                   GPIO_PIN_4 // пин вывода А энкодера
#define ENCODER_PIN_B                   GPIO_PIN_2 // пин вывода B энкодера

#define ENCODER_INTERRUPT_LEVEL         RISING // по какому уровню ловим прерывания

#define INTERRUPT_MAX_IDLE_TIME         300000ul    // время по умолчанию, в микросекундах, после получения последнего импульса и до момента, когда будет принято решение, что импульсы на входе закончились
#define DEFAULT_ROD_MOVE_LENGTH         50 // величина перемещения штанги по умолчанию, мм


#define INTERRUPT_SCREEN_SEE_TIME       6000    // сколько миллисекунд демонстрировать экран с графиками прерываний?
#define INTERRUPT_CHART_X_POINTS        150      // кол-во точек по X на графике прерываний
#define INTERRUPT_CHART_Y_POINTS        100      // кол-во точек по Y на графике прерываний
#define INTERRUPT_CHART_X_COORD         5         // Х-координата графика прерываний на экране
#define INTERRUPT_CHART_Y_COORD         120       // стартовая Y-координата графика прерываний на экране
#define CHART_POINTS_COUNT              150			 // кол-во точек на графике синусоиды на главном экране

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки получения значений превью осциллограммы тока
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CURRENT_COEFF_DEFAULT             3160 // коэффициент пересчёта по току (по умолчанию), в тысячных долях
#define CURRENT_TIMER_PERIOD              8 // период таймера сбора информации по осциллограммам тока, кол-во прерываний Timer3 (период 250us)
#define CURRENT_AVG_SAMPLES               10   // количество семплов для усреднения показаний с АЦП, перед помещением их в список превью осциллограммы по току
#define CURRENT_LIST_SIZE                 20     // количество записей в списке превью осциллограмм тока

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки детектирования значений тока при срабатывании релейной защиты
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// при срабатывании релейной защиты начинается сбор информации по значению тока. Если не было импульсов с энкодера, и ток не превышен - это ЛОЖНОЕ срабатывание защиты,
// и выходной сигнал на линии в этом случае - НЕ выдаётся. Если не было импульсов с энкодера, и ток превышен - это авария, и на выходных линиях появляется сигнал.
// настройки ниже отвечают за время сбора информации, по умолчанию это 20 семплов через 1 миллисекунду, то есть за 20 миллисекунд соберётся по 20 значений АЦП
// по каждому из каналов.

// усреднение происходит следующим образом: ищется минимальное и максимальное значение АЦП, вычисляется размах, и этот размах делится на количество семплов.

#define CURRENT_PEAK_NUM_SAMPLES          20      // количество семплов для усреднения значения тока
#define CURRENT_PEAK_TIMER_PERIOD         4    // период таймера сбора информации по значению тока, кол-во прерываний Timer3 (период 250us)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// стартовые координаты верхнего/левого угла сетки
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define INTERRUPT_CHART_GRID_X_START    5 
#define INTERRUPT_CHART_GRID_Y_START    20


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка светодиодов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LED_ON_LEVEL          HIGH			 // уровень включения светодиодов
#define LED_READY             PE5        // Индикация светодиодом "Готов"
#define LED_FAILURE           PE4        // Индикация светодиодом "Авария"
#define LED_TEST              PE6        // Индикация светодиодом "Тест"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка линий реле и выходного состояния
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define RELAY_PIN                 PB0                // пин, на котором ловятся импульсы срабатывания защиты
#define RELAY_TRIGGER_LEVEL       LOW         // уровень, при котором срабатывает защита

#define STATUS_LINE               PE0              // линия, на которой устанавливается нужный уровень при аварии

#define STATUS_ALARM_LEVEL        HIGH     // какой уровень устанавливать на STATUS_LINE при аварии

#define MAX_PULSES_TO_CATCH       5000 // сколько импульсов ловим на энкодере максимально?
#define INTERRUPT_SKIP_COUNTER    1 // через какое кол-во импульсов записывать прерывание в список? Например, если 1 - то каждый импульс, 2 - через один импульс, 3 - каждый третий импульс и т.п.
#define PREDICT_ENABLED     // включено ли предсказание срабатывания защиты по импульсам? ЗАКОММЕНТИРОВАТЬ СТРОКУ, если выключено
#define PREDICT_PULSES  5 // какое кол-во импульсов регистрировать для предсказания, прежде чем делать вывод - было ли срабатывание защиты
#define PREDICT_TIME    10000 // время, в микросекундах, между первым и последним импульсом для предсказаний. Если между крайними импульсами прошло меньше указанного времени - защита сработала


/*
как только у нас на RELAY_PIN какой-то уровень, начинаем считать РМС в течение какого-то времени
сравнивать с жестко прошитой базовой величиной
при отклонении больше чем на N процентов:
смотрим состояние нижнего концевика: если он изменил положение - это нормально. если не изменил - то это авария.
смотрим состояние верхнего концевика в течение какого-то времени (должен за это время сработать). Если он не изменил состояние - то это авария,
если изменил - то всё хорошо.
*/


//TODO: ПОД ПЕРЕПИСЫВАНИЕ??? ЭТИ НАСТРОЙКИ УЖЕ НЕАКТУАЛЬНЫ, ВРОДЕ БЫ
#define RELAY_WANT_DATA_AFTER       15000 // через какое максимальное кол-во микросекунд должны поступить данные прерываний после срабатывания реле защиты
#define RELAY_DATA_GAP              100     // гистерезис времени, в микросекундах, для принятия решения - попадает ли интервал между импульсом с реле и началом прерываний в нужный диапазон
#define ACS_SIGNAL_DELAY            50      // задержка после срабатывания защиты до выдачи импульса на АСУ ТП, миллисекунд
#define ACS_SIGNAL_LEVEL            HIGH    // какой уровень выдавать на АСУ ТП при срабатывании защиты


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка кнопок
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define BUTTON_RED                  PE11                  // Кнопка №1, красная
#define BUTTON_BLUE                 PE12                  // Кнопка №2, синяя
#define BUTTON_YELLOW               PE13                   // Кнопка №3, жёлтая

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Выход на АСУ ТП
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define out_asu_tp1                 PE0                //Выход на АСУ ТП №1
#define out_asu_tp2                 PE1                //Выход на АСУ ТП №2
#define out_asu_tp3                 PE2                //Выход на АСУ ТП №3
#define out_asu_tp4                 PE3                //Выход на АСУ ТП №4
#define asu_tp_level                HIGH               // уровень на выходе АСУ ТП при срабатывании защиты

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Вход токового трансформатора
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define current_transformer1        PA4      // Вход токового трансформатора №1
#define current_transformer2        PA5      // Вход токового трансформатора №2
#define current_transformer3        PA6      // Вход токового трансформатора №3

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CAN шина
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define ID0_Out                     PB9                   //         
#define ID1_Out                     PB7                   //
#define IDE0_In                     PC7                   //
#define IDE1_In                     PD8                   //
#define PPS_Out                     PC6                   //

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Контроль источников питания
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define power_3V3                   PA7               // Контроль источника питания +3.3в
#define power_5V0                   PE15               // Контроль источника питания +5.0в

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// НАСТРОЙКИ RS485
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define Upr_RS485                     PD11                // Вход управлениея шиной RS485  
#define RS485_SERIAL                  Serial2 // какой Serial используем для RS-485
#define RS485_SPEED                   57600 // какая скорость для RS-485
#define RS485_READING_TIMEOUT         1000 // таймаут на чтение входящих данных, миллисекунд
#define RS485_PING_PACKET_FREQUENCY   1000 // через сколько миллисекунд модуль посылает пакет проверки связи?

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// НАСТРОЙКИ MODBUS
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MODBUS_SERIAL           Serial4       // какой Serial для общения с MODBUS
#define MODBUS_BAUD             57600   // скорость Serial для работы с MODBUS

// регистры MODBUS

#define MODBUS_REG_PULSES                         40000 // регистр для количества импульсов
#define MODBUS_REG_PULSES_DELTA                   40001 // регистр для дельты импульсов
#define MODBUS_REG_ETHALON_PULSES_DELTA1          40002 // регистр 1 для дельты импульсов эталона
#define MODBUS_REG_ETHALON_PULSES_DELTA2          40003 // регистр 2 для дельты импульсов эталона
#define MODBUS_REG_MOTORESOURCE1                  40004 // регистр 1 для моторесурса
#define MODBUS_REG_MOTORESOURCE2                  40005 // регистр 2 для моторесурса
#define MODBUS_REG_MOTORESOURCE_MAX1              40006 // регистр 1 для максимального моторесурса
#define MODBUS_REG_MOTORESOURCE_MAX2              40007 // регистр 2 для максимального моторесурса
#define MODBUS_REG_TLOW_BORDER1                   40008 // регистр 1 для нижнего порога трансформатора
#define MODBUS_REG_TLOW_BORDER2                   40009 // регистр 2 для нижнего порога трансформатора
#define MODBUS_REG_THIGH_BORDER1                  40010 // регистр 1 для верхнего порога трансформатора
#define MODBUS_REG_THIGH_BORDER2                  40011 // регистр 2 для верхнего порога трансформатора
#define MODBUS_REG_RDELAY1                        40012 // регистр 1 для задержки реле
#define MODBUS_REG_RDELAY2                        40013 // регистр 2 для задержки реле
#define MODBUS_REG_ACSDELAY                       40014 // регистр 1 для задержки ACS
#define MODBUS_REG_SKIPC1                         40015 // регистр 1 для пропуска импульсов
#define MODBUS_REG_SKIPC2                         40016 // регистр 2 для пропуска импульсов
#define MODBUS_REG_CCOEFF1                        40017 // регистр 1 для коэффициента тока
#define MODBUS_REG_CCOEFF2                        40018 // регистр 2 для коэффициента тока
#define MODBUS_REG_ASUTPFLAGS                     40019 // регистр для флагов АСУ ТП
#define MODBUS_REG_MAXIDLETIME1                   40020 // регистр 1 для времени ожидания окончания импульсов
#define MODBUS_REG_MAXIDLETIME2                   40021 // регистр 2 для времени ожидания окончания импульсов
#define MODBUS_REG_RODMOVELEN1                    40022 // регистр 1 для величины перемещения штанги
#define MODBUS_REG_RODMOVELEN2                    40023 // регистр 2 для величины перемещения штанги
#define MODBUS_REG_SAVECHANGES                    40024 // регистр для флага сохранения настроек

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// НАСТРОЙКИ TFT
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_BUTTON_COLORS WHITE, LGRAY, WHITE, RED, BLUE // цвета для кнопок
#define TFT_BUTTON_COLORS2 WHITE, LGRAY, WHITE, RED, 0x4DC9 // цвета для кнопок
#define TFT_BACK_COLOR BLACK // цвет заливки экрана
#define TFT_FONT_COLOR WHITE // цвет шрифта по умолчанию

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_INIT_DELAY                0 // сколько миллисекунд (при необходимости) ждать перед отсылкой первых команд на дисплей
#define TFT_ORIENTATION               LANDSCAPE // ориентация экрана
#define TFT_reset                     PD3                 // Назначение pin сброса TFT дисплея.

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки тачскрина
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Тач-скрин сидит на SPI2 !!!

#define TFT_TOUCH_CS_PIN        PB12

#define  TOUCH_IRQ_Pin       GPIO_PIN_5   //  Определяем вывод информирующий о нажатии на тачскрин
#define  TOUCH_IRQ_GPIO_Port GPIOC        //  Определяем порт  TOUCH_IRQ_Pin
#define  TOUCH_PRESCALER     SPI_BAUDRATEPRESCALER_256 // прескалер для тача

#define  TOUCH_SCALE_X 220               // Рамер по горизонтали X
#define  TOUCH_SCALE_Y 176               // Рамер по вертикали  Y


#define  TOUCH_MIN_RAW_X 440    // Реальная минимальная величина по оси X (подстроить под конкретный дисплей)
#define  TOUCH_MAX_RAW_X 3820    // Реальная максимальная величина по оси X(подстроить под конкретный дисплей)

#define  TOUCH_MIN_RAW_Y 440     // Реальная минимальная величина по оси Y (подстроить под конкретный дисплей)
#define  TOUCH_MAX_RAW_Y 3840   // Реальная максимальная величина по оси Y(подстроить под конкретный дисплей)



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// концевики (индуктивные)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define ROD_ENDSTOP_UP                PC0          // Концевик позиции штанги №1, вверху
#define ROD_ENDSTOP_DOWN              PC1          // Концевик позиции штанги №1, внизу
#define ENDSTOPS_INT_LEVEL            RISING // уровень для прерывания
#define ENDSTOP_FREQUENCY             450 // рабочая частота импульсов концевика, Герц
#define ENDSTOP_HISTERESIS            30 // гистерезис частоты, Герц


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки АЦП
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
ch7:A0
ch6:A1
ch5:A2
ch4:A3  Вход токового трансформатора №3 (4)
ch3:A4  Вход токового трансформатора №2 (3)
ch2:A5  Вход токового трансформатора №1 (2)
ch1:A6  Вход индуктивного датчика №1 тест исправности датчика (1)
ch0:A7  Вход индуктивного датчика №2 тест исправности датчика (0)
ch10:A8 Вход индуктивного датчика №3 тест исправности датчика (5)
ch11:A9 Измерение =200В  (6)
ch12:A10 Измерение 3V3   (7)
ch13:A11 Измерение +5V   (8)
*/

#define NUMBER_OF_BUFFERS 6                                            // Установить количество буферов
#define SAMPLING_RATE 21000   // период таймера 3
#define NUM_CHANNELS 4   // Установить количество аналоговых входов
#define ADC_BUFFER_SIZE 200*NUM_CHANNELS                                   // Определить размер буфера хранения измеряемого сигнала     


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка логов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOGS_DIRECTORY                              "/LOG" // папка, в которой лежат логи
#define ETHALONS_DIRECTORY                          "/ETL" // папка, где лежат эталоны
#define ETHALON_NAME_PREFIX                         "/et" // префикс имени файла эталона
#define ETHALON_UP_POSTFIX                          "up" // постфикс для имени файла эталона, поднятие штанги
#define ETHALON_DOWN_POSTFIX                        "dwn" // постфикс для имени файла эталона, опускание штанги
#define ETHALON_FILE_EXT                            ".etl" // расширение файла эталона
#define ETHALON_RECORD_TIMEOUT	                    30000 // таймаут, миллисекунд, при записи эталона с конфигуратора
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DATA_MEASURE_THRESHOLD                      2000 // через сколько миллисекунд обновлять показания с линий вольтажа и температуру с часов
#define VOLTAGE_THRESHOLD                           10 // порог изменений, в процентах, для принятия решения о том, хороший вольтаж или нет
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки EEPROM
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// заголовки записей, которым нужны префиксы
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define RECORD_HEADER1 0xD0
#define RECORD_HEADER2 0xEF
#define RECORD_HEADER3 0x18

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// адреса хранения в EEPROM
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define COUNT_PULSES_STORE_ADDRESS                   20 // по какому адресу храним переменную для кол-ва импульсов канала 1 (4 байта)
#define CHANNEL_PULSES_DELTA_ADDRESS                 40 // адрес хранения дельты импульсов канала 1 (3 байта)
#define MOTORESOURCE_STORE_ADDRESS                   60 // адрес хранения кол-ва срабатываний защиты канала 1 (6 байт)
#define MOTORESOURCE_MAX_STORE_ADDRESS               80 // адрес хранения макс. кол-ва срабатываний защиты канала 1 (6 байт)
#define UUID_STORE_ADDRESS                            130 // адрес хранения уникального идентификатора контроллера (32 байта идентификатор + 3 байта префикс = 35 байт)
#define TRANSFORMER_LOW_BORDER_STORE_ADDRESS          170 // адрес хранения нижнего порога токового трансформатора (6 байт)
#define TRANSFORMER_HIGH_BORDER_STORE_ADDRESS         180 // адрес хранения верхнего порога токового трансформатора (6 байт)
#define RELAY_DELAY_STORE_ADDRESS                     190 // адрес хранения задержки после срабатывания защиты и до начала импульсов (6 байт)
#define ACS_DELAY_STORE_ADDRESS                       200 // адрес хранения задержки после срабатывания защиты и до начала выдачи сигнала АСУ ТП (4 байта)
#define SKIP_COUNTER_STORE_ADDRESS                    210 // адрес хранения настройки пропуска импульсов (6 байт)
#define CURRENT_COEFF_STORE_ADDRESS                   220 // адрес хранения настройки коэффициента по току (6 байт)
#define ETHALON_DELTA_STORE_ADDRESS                   230 // адрес хранения настройки дельты времени импульсов для сравнения с эталоном (6 байт)
#define ASU_TP_SIGNALS_ADDRESS                        240 // адрес хранения настройки флагов выдачи внешних сигналов на АСУ ТП (3 байта)
#define MAX_IDLE_TIME_STORE_ADDRESS                   250 // адрес хранения времени ожидания окончания импульсов энкодера  (6 байт)
#define ROD_MOVE_LENGTH_STORE_ADDRESS                 260 // адрес хранения величины перемещения привода штанги, мм (6 байт)
#define MODBUS_SLAVE_ADDRESS                          270 // адрес хранения Slave ID MODBUS (3 байта)

#define EEPROM_LAST_3_DATA_ADDRESS                    2048 // адрес начала хранения последних трёх срабатываний в EEPROM (ПОКА ИСПОЛЬЗУЕТСЯ ТОЛЬКО ОДНО СРАБАТЫВАНИЕ !!!)
#define EEPROM_LAST_3_RECORD_SIZE                     45000ul // максимальный размер одной записи срабатывания, для хранения в EEPROM 45 000 байт на одну запись максимум)
//--------------------------------------------------------------------------------------------------------------------------------
// НАСТРОЙКИ ВНЕШНЕГО ВАТЧДОГА (актуально при раскомментированной команде USE_EXTERNAL_WATCHDOG)
//--------------------------------------------------------------------------------------------------------------------------------
#define USE_EXTERNAL_WATCHDOG // использовать или нет внешний ватчдог?
#define WATCHDOG_REBOOT_PIN PC13 // номер пина, на котором будут меняться уровни
#define WATCHDOG_NORMAL_LEVEL HIGH // уровень на ноге, который держится WATCHDOG_WORK_INTERVAL миллисекунд
#define WATCHDOG_TRIGGERED_LEVEL LOW // уровень импульса на ноге, который держится WATCHDOG_PULSE_DURATION миллисекунд
#define WATCHDOG_WORK_INTERVAL 5000 // через сколько миллисекунд на ноге будет уровень WATCHDOG_TRIGGERED_LEVEL
#define WATCHDOG_PULSE_DURATION 600 // сколько миллисекунд держать уровень WATCHDOG_TRIGGERED_LEVEL на ноге


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// время в миллисекундах, после которого идёт переключение на главный экран, если какой-то экран висит долгое время
#define RESET_TO_MAIN_SCREEN_DELAY                    60000
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// частота мигания надписью моторесурса, если он исчерпан, миллисекунд
#define MOTORESOURCE_BLINK_DURATION                   800

// через сколько процентов до максимального мигать надписью моторесурса (например, если уканано 10, то будет мигать после 90% выработки)
#define MOTORESOURCE_BLINK_PERCENTS                   10
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// кол-во кадров в секунду для отображения синусоид
#define CHART_FPS                                     30
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки порогов токового трансформатора по умолчанию
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TRANSFORMER_HIGH_DEFAULT_BORDER               3500 // верхний порог
#define TRANSFORMER_LOW_DEFAULT_BORDER                2500 // нижний порог
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// дальше лазить - НЕОСМОТРИТЕЛЬНО !!!!
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// ||
// \/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _RS485_OFF
extern RS485 rs485;
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern TwoWire Wire1;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void SwitchRS485MainHandler(bool on); // включаем/выключаем обработчик по умолчанию
extern bool HasRS485Link; // флаг, есть ли связь по RS-485
extern void updateExternalWatchdog();
extern void Test_SD(Stream* outS, bool withBenchFile, bool dontReadSavedBenchFile);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _MY_DEBUG
  #define DBG(s) { DEBUG_SERIAL << (s); DEBUG_SERIAL.flush(); }
  #define DBGLN(s) { DEBUG_SERIAL << (s) << ENDL; DEBUG_SERIAL.flush(); }
//  #define DBG(s) { delayMicroseconds(1000); }
//  #define DBGLN(s) { delayMicroseconds(1000); }

#else
  #define DBG(s) (void) 0
  #define DBGLN(s) (void) 0
#endif
#define ENDL '\n'
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T> inline Stream& operator << (Stream &s, T n) { s.print(n); return s; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CORE_COMMAND_GET F("GET=") // префикс для команды получения данных из ядра
#define CORE_COMMAND_SET F("SET=") // префикс для команды сохранения данных в ядро
#define CORE_COMMAND_ANSWER_OK F("OK=") // какой префикс будет посылаться в ответ на команду получения данных и её успешной отработке
#define CORE_COMMAND_ANSWER_ERROR F("ER=") // какой префикс будет посылаться в ответ на команду получения данных и её неуспешной отработке
#define CORE_COMMAND_PARAM_DELIMITER '|' // разделитель параметров
#define CORE_END_OF_DATA F("[END]")
#define CORE_COMMAND_DONE F("DONE")
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_EXTERNAL_WATCHDOG
  typedef enum
  {
    WAIT_FOR_TRIGGERED,
    WAIT_FOR_NORMAL 
  } ExternalWatchdogState;
  
  typedef struct
  {
    uint16_t timer;
    ExternalWatchdogState state;
  } ExternalWatchdogSettings;
#endif
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//extern Vector<uint8_t> LastTriggeredInterruptRecord;
extern int8_t LastTriggeredInterruptRecordIndex;

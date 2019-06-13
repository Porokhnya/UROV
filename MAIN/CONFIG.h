#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define _DEBUG // закомментировать для выключения отладочной информации

//#define _ADC_OFF // выключить АЦП на время тестирования
//#define _YIELD_OFF // выключить функцию yield на время тестирования
//#define _COM_COMMANDS_OFF // выключить обработку команд из COM-порта на время тестирования
//#define _DRAW_GRID_OFF // выключить отрисовку сетки графика на время тестирования
//#define _SD_OFF // выключить поддержку SD на время тестирования
//#define _RMS_OFF // отключить проверку РМС на время тестирования
//#define _DELAYED_EVENT_OFF // отключить CoreDelayedEvent на время тестирования
//#define _CORE_TEMP_OFF // отключить периодическое получение температуры ядра на время тестирования

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

//#define _FAKE_CHART_DRAW // рисовать тестовый график из 200 линий вместо актуального, на время тестирования
#define FAKE_POINTS_TO_GENERATE 200 // сколько точек фейкового графика генерировать

//#define _PRINT_INTERRUPT_DATA // выводить данные списка прерываний в Serial или нет?

//#define IGNORE_ROD_POSITION // ЗАКОММЕНТИРОВАТЬ, ЕСЛИ НЕ НАДО ИГНОРИРОВАТЬ ПОЗИЦИЮ ШТАНГИ ПРИ СРАВНЕНИИ ЭТАЛОНОВ !!!
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SERIAL_SPEED 115200 // скорость работы с Serial
#define I2C_SPEED 400000UL // скорость работы I2C, герц
#define DS3231_WIRE_NUMBER 1 // номер I2C для часов (0 - первый I2C, 1 - второй I2C)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки пинов прерываний
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define ENCODER_PIN1 12 // номер пина, на котором ловятся импульсы от энкодера штанги
#define ENCODER_INTERRUPT_LEVEL RISING // по какому уровню ловим прерывания
#define ENCODER_PIN2 8 // номер пина 2, на котором ловятся импульсы от энкодера штанги

#define INTERRUPT_RESERVE_RECORDS 500 // на сколько импульсов первоначально резервировать памяти в списках прерываний, для оптимизации работы с памятью
#define INTERRUPT_MAX_IDLE_TIME 1200000    // время, в микросекундах, после получения последнего импульса и до момента, когда будет принято решение, что импульсы на входе закончились
#define INTERRUPT_SCREEN_SEE_TIME 6000    // сколько миллисекунд демонстрировать экран с графиками прерываний?
#define INTERRUPT_CHART_X_POINTS 150      // кол-во точек по X на графике прерываний
#define INTERRUPT_CHART_Y_POINTS 100      // кол-во точек по Y на графике прерываний
#define INTERRUPT_CHART_X_COORD 5         // Х-координата графика прерываний на экране
#define INTERRUPT_CHART_Y_COORD 120       // стартовая Y-координата графика прерываний на экране
#define CHART_POINTS_COUNT 150			 // кол-во точек на графике синусоиды на главном экране

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// стартовые координаты верхнего/левого угла сетки
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define INTERRUPT_CHART_GRID_X_START 5 
#define INTERRUPT_CHART_GRID_Y_START 20


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка светодиодов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LED_ON_LEVEL HIGH			 // уровень включения светодиодов
#define LED_READY  37                // Индикация светодиодом "Готов"
#define LED_FAILURE  36              // Индикация светодиодом "Авария"
#define LED_TEST  38                 // Индикация светодиодом "Тест"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка линий реле и выходного состояния
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define RELAY_PIN 11                // пин, на котором ловятся импульсы срабатывания защиты
#define RELAY_INTERRUPT_LEVEL RISING // по какому фронту ловим прерывания
#define RMS_COMPUTE_TIME 40 // кол-во миллисекунд, в течение которых мы считаем РМС
#define RMS_ETHALON_VAL 1860	// эталонное среднее значение РМС
#define RMS_HISTERESIS_PERCENTS 20 // допустимое значение отклонения РМС от эталонного
#define STATUS_LINE 50              // линия, на которой устанавливается нужный уровень при аварии
#define STATUS_ALARM_LEVEL HIGH     // какой уровень устанавливать на STATUS_LINE при аварии

/*
как только у нас на RELAY_PIN какой-то уровень, начинаем считать РМС в течение какого-то времени
сравнивать с жестко прошитой базовой величиной
при отклонении больше чем на N процентов:
смотрим состояние нижнего концевика: если он изменил положение - это нормально. если не изменил - то это авария.
смотрим состояние верхнего концевика в течение какого-то времени (должен за это время сработать). Если он не изменил состояние - то это авария,
если изменил - то всё хорошо.
*/


//TODO: ПОД ПЕРЕПИСЫВАНИЕ??? ЭТИ НАСТРОЙКИ УЖЕ НЕАКТУАЛЬНЫ, ВРОДЕ БЫ
#define RELAY_WANT_DATA_AFTER 15000 // через какое максимальное кол-во микросекунд должны поступить данные прерываний после срабатывания реле защиты
#define RELAY_DATA_GAP      100     // гистерезис времени, в микросекундах, для принятия решения - попадает ли интервал между импульсом с реле и началом прерываний в нужный диапазон
#define ACS_SIGNAL_DELAY  50      // задержка после срабатывания защиты до выдачи импульса на АСУ ТП, миллисекунд
#define ACS_SIGNAL_LEVEL  HIGH    // какой уровень выдавать на АСУ ТП при срабатывании защиты


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка кнопок
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define BUTTON_RED  39                  // Кнопка №1, красная
#define BUTTON_BLUE1  40                  // Кнопка №2, синяя
#define BUTTON_BLUE2  41                  // Кнопка №3, синяя
#define BUTTON_YELLOW  9                   // Кнопка №4, жёлтая

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Выход на АСУ ТП
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define out_asu_tp1 42                //Выход на АСУ ТП №1
#define out_asu_tp2 43                //Выход на АСУ ТП №2
#define out_asu_tp3 44                //Выход на АСУ ТП №3
#define out_asu_tp4 50                //Выход на АСУ ТП №4

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Вход токового трансформатора
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define current_transformer1  A5      // Вход токового трансформатора №1
#define current_transformer2  A4      // Вход токового трансформатора №2
#define current_transformer3  A3      // Вход токового трансформатора №3

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CAN шина
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define ID0_Out 45                   //         
#define ID1_Out 46                   //
#define IDE0_In 47                   //
#define IDE1_In 48                   //
#define PPS_Out 49                   //

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Контроль источников питания
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define power_200  A9                // Контроль источника питания 200в
#define power_3V3  A10               // Контроль источника питания +3.3в
#define power_5V0  A11               // Контроль источника питания +5.0в

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// RS485
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define Upr_RS485  51                // Вход управлениея шиной RS485  


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SD-карта
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SD_CS_PIN 10                 // пин CS для SD-карты

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_reset 34                 // Назначение pin сброса TFT дисплея.


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// концевики (индуктивные)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define ROD_ENDSTOP_UP 7          // Концевик позиции штанги №1, вверху
#define ROD_ENDSTOP_DOWN 35          // Концевик позиции штанги №1, внизу
#define ENDSTOPS_INT_LEVEL RISING // уровень для прерывания
#define ENDSTOP_FREQUENCY 450 // рабочая частота импульсов концевика, Герц
#define ENDSTOP_HISTERESIS 30 // гистерезис частоты, Герц
//#define ROD_ENDSTOP_TRIGGERED LOW // уровень, когда концевик штанги сработал


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки АЦП
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NUM_CHANNELS 9   // Установить количество аналоговых входов
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

#define ADC_CHANNELS ADC_CHER_CH0 | ADC_CHER_CH1 | ADC_CHER_CH2 | ADC_CHER_CH3 | ADC_CHER_CH4 | ADC_CHER_CH10 | ADC_CHDR_CH11 | ADC_CHDR_CH12 | ADC_CHDR_CH13  //
#define ADC_CHANNELS_DIS  ADC_CHDR_CH5 | ADC_CHDR_CH6 | ADC_CHDR_CH7  // Отключить не используемые входа
#define BUFFER_SIZE 200*NUM_CHANNELS                                   // Определить размер буфера хранения измеряемого сигнала     
#define NUMBER_OF_BUFFERS 6                                            // Установить количество буферов
//#define VOLT_REF        (2400)                                       // Величина опорного напряжения
/* The maximal digital value */
//#define ADC_RESOLUTION		12                                // Разрядность АЦП (максимальная для DUE)



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка логов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOGS_DIRECTORY "/LOG" // папка, в которой лежат логи
#define ETHALONS_DIRECTORY "/ETL" // папка, где лежат эталоны
#define ETHALON_NAME_PREFIX "/et" // префикс имени файла эталона
#define ETHALON_UP_POSTFIX "up" // постфикс для имени файла эталона, поднятие штанги
#define ETHALON_DOWN_POSTFIX "dwn" // постфикс для имени файла эталона, опускание штанги
#define ETHALON_FILE_EXT ".etl" // расширение файла эталона
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DATA_MEASURE_THRESHOLD 2000 // через сколько миллисекунд обновлять показания с линий вольтажа и температуру с часов
#define VOLTAGE_THRESHOLD 10 // порог изменений, в процентах, для принятия решения о том, хороший вольтаж или нет
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
#define COUNT_PULSES_STORE_ADDRESS1 10 // по какому адресу храним переменную для кол-ва импульсов канала 1 (2 байта)
#define CHANNEL_PULSES_DELTA_ADDRESS1 16 // адрес хранения дельты импульсов канала 1 (1 байт)
#define MOTORESOURCE_STORE_ADDRESS1  100 // адрес хранения кол-ва срабатываний защиты канала 1 (4 байта)
#define MOTORESOURCE_MAX_STORE_ADDRESS1  112 // адрес хранения макс. кол-ва срабатываний защиты канала 1 (4 байта)
#define UUID_STORE_ADDRESS  130 // адрес хранения уникального идентификатора контроллера (32 байта идентификатор + 3 байта - префикс = 35 байт)
#define TRANSFORMER_LOW_BORDER_STORE_ADDRESS 170 // адрес хранения нижнего порога токового трансформатора (4 байта)
#define TRANSFORMER_HIGH_BORDER_STORE_ADDRESS 174 // адрес хранения верхнего порога токового трансформатора (4 байта)
#define RELAY_DELAY_STORE_ADDRESS 178 // адрес хранения задержки после срабатывания защиты и до начала импульсов (4 байта)
#define ACS_DELAY_STORE_ADDRESS 182 // адрес хранения задержки после срабатывания защиты и до начала выдачи сигнала АСУ ТП (2 байта)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// версия ПО
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SOFTWARE_VERSION "v.0.1a"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// время в миллисекундах, после которого идёт переключение на главный экран, если какой-то экран висит долгое время
#define RESET_TO_MAIN_SCREEN_DELAY 60000
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// частота мигания надписью моторесурса, если он исчерпан, миллисекунд
#define MOTORESOURCE_BLINK_DURATION 800

// через сколько процентов до максимального мигать надписью моторесурса (например, если уканано 10, то будет мигать после 90% выработки)
#define MOTORESOURCE_BLINK_PERCENTS 10
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// кол-во кадров в секунду для отображения синусоид
#define CHART_FPS 30
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// дельта времени импульсов (микросекунд) при сравнении эталона с пачкой пойманных импульсов
#define ETHALON_COMPARE_DELTA 500
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки порогов токового трансформатора по умолчанию
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TRANSFORMER_HIGH_DEFAULT_BORDER 3500 // верхний порог
#define TRANSFORMER_LOW_DEFAULT_BORDER 2500 // нижний порог
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------



















#define NVIC_PriorityGroup_0   ((uint32_t)0x700)
#define NVIC_PriorityGroup_1   ((uint32_t)0x600)
#define NVIC_PriorityGroup_2   ((uint32_t)0x500)
#define NVIC_PriorityGroup_3   ((uint32_t)0x400)
#define NVIC_PriorityGroup_4   ((uint32_t)0x300)







#ifdef _DEBUG
#define DBG(s) { Serial << (s); }
  #define DBGLN(s) { Serial << (s) << ENDL; }
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


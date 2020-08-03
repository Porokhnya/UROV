#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define _DEBUG                           // закомментировать для выключения отладочной информации

#define _INTERRUPTS_OFF                 // выключить обработку прерываний от энкодера штанги и концевиков
//#define _ADC_OFF                       // выключить АЦП на время тестирования
//#define _YIELD_OFF                     // выключить функцию yield на время тестирования
//#define _COM_COMMANDS_OFF              // выключить обработку команд из COM-порта на время тестирования
//#define _DRAW_GRID_OFF                 // выключить отрисовку сетки графика на время тестирования
//#define _SD_OFF                        // выключить поддержку SD на время тестирования
//#define _RMS_OFF                       // отключить проверку РМС на время тестирования
//#define _DELAYED_EVENT_OFF             // отключить CoreDelayedEvent на время тестирования
//#define _CORE_TEMP_OFF                 // отключить периодическое получение температуры ядра на время тестирования

//#define _SCREEN_2_OFF                  // отключить второй экран на время тестирования
//#define _SCREEN_3_OFF                  // отключить экран перехода в меню настройки, на время тестирования
//#define _SCREEN_4_OFF                  // отключить экран меню установки даты/времени, на время тестирования
//#define _SCREEN_5_OFF                  // отключить создание экрана установки времени на время тестирования
//#define _SCREEN_6_OFF                  // отключить создание экрана установки даты на время тестирования

//#define _DISABLE_MAIN_SCREEN_BUTTONS   // убрать кнопки с главного экрана, на время тестирования
//#define _DISABLE_DRAW_RAM_ON_SCREEN    // убрать вывод свободной памяти на главном экране
//#define _DISABLE_DRAW_TIME             // не рисовать время на главном экране
//#define _DISABLE_DRAW_VOLTAGE          // не рисовать вольтаж на главном экране
//#define _DISABLE_DRAW_SOFTWARE_VERSION // не рисовать версию ПО на главном экране

//#define _FAKE_CHART_DRAW               // рисовать тестовый график из 200 линий вместо актуального, на время тестирования
#define FAKE_POINTS_TO_GENERATE 200      // сколько точек фейкового графика генерировать

//#define _PRINT_INTERRUPT_DATA          // выводить данные списка прерываний в Serial или нет?

//#define IGNORE_ROD_POSITION            // ЗАКОММЕНТИРОВАТЬ, ЕСЛИ НЕ НАДО ИГНОРИРОВАТЬ ПОЗИЦИЮ ШТАНГИ ПРИ СРАВНЕНИИ ЭТАЛОНОВ !!!
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SERIAL_SPEED 115200              // скорость работы с Serial
#define I2C_SPEED 400000UL               // скорость работы I2C, герц
#define DS3231_WIRE_NUMBER 1             // номер I2C для часов (0 - первый I2C, 1 - второй I2C)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
// настройки экранных кнопок
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
#define BUTTON_WIDTH  280 // ширина кнопок в меню
#define BUTTON_HEIGHT  40 // высота кнопок в меню


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
// НАСТРОЙКА ЗАПУСКА ТЕСТА ПО ЧЁРНОЙ КНОПКЕ
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
#define RELAY_LINE_ALL_WAIT_INTERVAL      1000  // пауза после включения всех линий и до включения шунта 1 (миллисекунд)
#define RELAY_SHUNT1_WAIT_INTERVAL        50  // пауза после включения шунта 1 до включения шунта 2 (миллисекунд)
#define RELAY_SHUNT2_WAIT_INTERVAL        50   // пауза после включения шунта 2 до включения реле аварии (миллисекунд)
#define RELAY_PROTECT_WAIT_INTERVAL       50    // пауза после включения реле аварии до старта генерации импульсов (миллисекунд)
#define RELAY_SHUNT2_OFF_INTERVAL         20   // пауза после генерации импульсов до отключения шунта 2 (миллисекунд)
#define RELAY_SHUNT1_OFF_INTERVAL         20   // пауза после отключения шунта 2 до отключения шунта 1 (миллисекунд)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ПИЩАЛКА
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// уровень включения пищалки
#define BUZZER_LEVEL HIGH

// пин пищалки
#define BUZZER_PIN 67

// длительность писка, миллисекунд
#define BUZZER_DURATION 60

#define BUZZER_TIMER Timer5 // таймер для пищалки


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// НАСТРОЙКА ТАЙМЕРА ОБНОВЛЕНИЯ ГЕНЕРАТОРОВ ИМПУЛЬСОВ
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define GEN_TIMER Timer4                    // какой таймер используется (Timer0, Timer1 и т.п.)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ДАТЧИКИ ТЕМПЕРАТУРЫ DS18B20
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DS18B20_PINS 55, 56 // номера пинов для DS18B20, через запятую (могут быть одинаковы, тогда датчики висят на одной линии)
#define TEMPERATURE_ALERT_BORDER 70 // при превышении этой уставки температуры будут выключаться РЕЛЕ ШУНТОВ
#define TEMPERATURE_UPDATE_INTERVAL 2000 // интервал, в миллисекундах, между обновлениями температуры с датчиков

// координаты отрисовки данных датчиков
#define SENSOR_1_DRAW_X 5 // X-координата отрисовки данных первого датчика
#define SENSOR_1_DRAW_Y 150 // Y-координата отрисовки данных первого датчика

#define SENSOR_2_DRAW_X 5 // X-координата отрисовки данных второго датчика
#define SENSOR_2_DRAW_Y 170 // Y-координата отрисовки данных второго датчика

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки пинов прерываний
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define ENCODER_PIN1 73                  // номер пина, на котором ловятся импульсы от энкодера штанги
#define ENCODER_INTERRUPT_LEVEL RISING   // по какому уровню ловим прерывания
#define ENCODER_PIN2 73                  // номер пина 2, на котором ловятся импульсы от энкодера штанги

#define INTERRUPT_RESERVE_RECORDS 1000    // на сколько импульсов первоначально резервировать памяти в списках прерываний, для оптимизации работы с памятью
#define INTERRUPT_MAX_IDLE_TIME 1200000  // время, в микросекундах, после получения последнего импульса и до момента, когда будет принято решение, что импульсы на входе закончились
#define INTERRUPT_SCREEN_SEE_TIME 6000   // сколько миллисекунд демонстрировать экран с графиками прерываний?
#define INTERRUPT_CHART_X_POINTS 150     // кол-во точек по X на графике прерываний
#define INTERRUPT_CHART_Y_POINTS 100     // кол-во точек по Y на графике прерываний
#define INTERRUPT_CHART_X_COORD 5        // Х-координата графика прерываний на экране
#define INTERRUPT_CHART_Y_COORD 140      // стартовая Y-координата графика прерываний на экране
#define CHART_POINTS_COUNT 150			 // кол-во точек на графике синусоиды на главном экране

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// стартовые координаты верхнего/левого угла сетки
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define INTERRUPT_CHART_GRID_X_START 5 
#define INTERRUPT_CHART_GRID_Y_START 20


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка линий реле и выходного состояния
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define RELAY_PIN 73                     // пин, на котором ловятся импульсы срабатывания защиты
#define RELAY_INTERRUPT_LEVEL RISING     // по какому фронту ловим прерывания
#define RMS_COMPUTE_TIME 40              // кол-во миллисекунд, в течение которых мы считаем РМС
#define RMS_ETHALON_VAL 1860	         // эталонное среднее значение РМС
#define RMS_HISTERESIS_PERCENTS 20       // допустимое значение отклонения РМС от эталонного
#define STATUS_LINE 50                   // линия, на которой устанавливается нужный уровень при аварии
#define STATUS_ALARM_LEVEL HIGH          // какой уровень устанавливать на STATUS_LINE при аварии

/*
как только у нас на RELAY_PIN какой-то уровень, начинаем считать РМС в течение какого-то времени
сравнивать с жестко прошитой базовой величиной
при отклонении больше чем на N процентов:
смотрим состояние нижнего концевика: если он изменил положение - это нормально. если не изменил - то это авария.
смотрим состояние верхнего концевика в течение какого-то времени (должен за это время сработать). Если он не изменил состояние - то это авария,
если изменил - то всё хорошо.
*/


//TODO: ПОД ПЕРЕПИСЫВАНИЕ??? ЭТИ НАСТРОЙКИ УЖЕ НЕАКТУАЛЬНЫ, ВРОДЕ БЫ
#define RELAY_WANT_DATA_AFTER 15000     // через какое максимальное кол-во микросекунд должны поступить данные прерываний после срабатывания реле защиты
#define RELAY_DATA_GAP      100         // гистерезис времени, в микросекундах, для принятия решения - попадает ли интервал между импульсом с реле и началом прерываний в нужный диапазон
#define ACS_SIGNAL_DELAY  50            // задержка после срабатывания защиты до выдачи импульса на АСУ ТП, миллисекунд
#define ACS_SIGNAL_LEVEL  HIGH          // какой уровень выдавать на АСУ ТП при срабатывании защиты


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка кнопок на передней панели Формирователя токов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define BUTTON_RED    51            // Кнопка №1, красная    Подать ток на фазу "А"
#define BUTTON_BLUE   13            // Кнопка №2, синяя      Подать ток на фазу "А"
#define BUTTON_GREY   12            // Кнопка №3, серая      Подать ток на фазу "С"
#define BUTTON_BLACK  11            // Кнопка №4, черная     Старт теста
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Выход на управление реле 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define RELAY_ON_LEVEL  HIGH            // уровень включения реле
#define RELAY_DRAW_COORD_X 290          // X-координата начала отрисовки состояния реле на главном экране
#define RELAY_DRAW_COORD_Y 260          // Y-координата начала отрисовки состояния реле на главном экране
#define RELAY_BOX_SIZE      40          // размер стороны квадрата для отрисовки состояния реле на главном экране

#define rele_lineA    50                // Реле RL2 подачи тока на фазу "А"
#define rele_lineB    42                // Реле RL1 подачи тока на фазу "B"
#define rele_lineC    43                // Реле RL3 подачи тока на фазу "C"
#define rele_lineAll  46                // Реле RL6 подачи тока на на все фазы
#define rele_shunt1   44                // Реле RL4 шунт 1 подачи тока
#define rele_shunt2   45                // Реле RL5 шунт 2 подачи тока
#define rele_protect1 62                // Реле RL0 формирование сингнала срабатывания защиты
#define rele_protect2 63                // Реле RL7 формирование сингнала срабатывания защиты (резерв)


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Выход формирователя сигналов энкодера
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define enkoderA      53                // формирователь сигналов энкодера линия А    
#define enkoderB      49                // формирователь сигналов энкодера линия В    


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Вход троб сигнала
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define strob1       54                 // Вход троб сигнала (резерв)   
#define strob2       48                 // Вход троб сигнала (резерв)   


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Вход токового трансформатора
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define current_transformer1  A5      // Вход токового трансформатора №1
#define current_transformer2  A4      // Вход токового трансформатора №2
#define current_transformer3  A3      // Вход токового трансформатора №3


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Контроль источников питания
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define power_200   A9               // Контроль источника питания 200в
//#define power_3V3  A10               // Контроль источника питания +3.3в
//#define power_5V0  A11               // Контроль источника питания +5.0в



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SD-карта
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SD_CS_PIN 52                 // пин CS для SD-карты

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFT
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TFT_reset 41                 // Назначение pin сброса TFT дисплея.


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// концевики (индуктивные)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define ROD_ENDSTOP_UP 72            // Концевик позиции штанги №1, вверху
#define ROD_ENDSTOP_DOWN 72          // Концевик позиции штанги №1, внизу
#define ENDSTOPS_INT_LEVEL RISING    // уровень для прерывания
#define ENDSTOP_FREQUENCY 450        // рабочая частота импульсов концевика, Герц
#define ENDSTOP_HISTERESIS 30        // гистерезис частоты, Герц
//#define ROD_ENDSTOP_TRIGGERED LOW  // уровень, когда концевик штанги сработал


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки АЦП
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NUM_CHANNELS 3   // Установить количество аналоговых входов
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


#define ADC_CHANNELS  ADC_CHER_CH2 | ADC_CHER_CH3 | ADC_CHER_CH4
#define ADC_CHANNELS_DIS   ADC_CHER_CH0 | ADC_CHER_CH1 | ADC_CHDR_CH5 | ADC_CHDR_CH6 | ADC_CHDR_CH7| ADC_CHER_CH10| ADC_CHDR_CH11 | ADC_CHDR_CH12 | ADC_CHDR_CH13   // Отключить не используемые входа
#define BUFFER_SIZE 250*NUM_CHANNELS                                   // Определить размер буфера хранения измеряемого сигнала     
#define NUMBER_OF_BUFFERS 4                                            // Установить количество буферов
//#define VOLT_REF        (2400)                                       // Величина опорного напряжения
/* The maximal digital value */
//#define ADC_RESOLUTION		12                                     // Разрядность АЦП (максимальная для DUE)



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка логов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOGS_DIRECTORY "/LOG"         // папка, в которой лежат логи
#define ETHALONS_DIRECTORY "/ETL"     // папка, где лежат эталоны
#define ETHALON_NAME_PREFIX "/et"     // префикс имени файла эталона
#define ETHALON_UP_POSTFIX "up"       // постфикс для имени файла эталона, поднятие штанги
#define ETHALON_DOWN_POSTFIX "dwn"    // постфикс для имени файла эталона, опускание штанги
#define ETHALON_FILE_EXT ".etl"       // расширение файла эталона
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DATA_MEASURE_THRESHOLD 2000   // через сколько миллисекунд обновлять показания с линий вольтажа и температуру с часов
#define VOLTAGE_THRESHOLD 10          // порог изменений, в процентах, для принятия решения о том, хороший вольтаж или нет
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
#define COUNT_PULSES_STORE_ADDRESS1 10              // по какому адресу храним переменную для кол-ва импульсов канала 1 (2 байта)
#define CHANNEL_PULSES_DELTA_ADDRESS1 16            // адрес хранения дельты импульсов канала 1 (1 байт)
#define MOTORESOURCE_STORE_ADDRESS1  100            // адрес хранения кол-ва срабатываний защиты канала 1 (4 байта)
#define MOTORESOURCE_MAX_STORE_ADDRESS1  112        // адрес хранения макс. кол-ва срабатываний защиты канала 1 (4 байта)
#define UUID_STORE_ADDRESS  130                     // адрес хранения уникального идентификатора контроллера (32 байта идентификатор + 3 байта - префикс = 35 байт)
#define TRANSFORMER_LOW_BORDER_STORE_ADDRESS 170    // адрес хранения нижнего порога токового трансформатора (4 байта)
#define TRANSFORMER_HIGH_BORDER_STORE_ADDRESS 174   // адрес хранения верхнего порога токового трансформатора (4 байта)
#define RELAY_DELAY_STORE_ADDRESS 178               // адрес хранения задержки после срабатывания защиты и до начала импульсов (4 байта)
#define ACS_DELAY_STORE_ADDRESS 182                 // адрес хранения задержки после срабатывания защиты и до начала выдачи сигнала АСУ ТП (2 байта)
#define CHART_PERIOD_STORE_ADDRESS 200              // адрес хранения настройки периода таймера для генерации графика работы энкодера (7 байт)
#define CHART_PULSE_WIDTH_STORE_ADDRESS 210         // адрес хранения настройки ширины импульса для генерации графика работы энкодера (7 байт)
#define CHART_WORK_TIME_STORE_ADDRESS 220           // адрес хранения настройки времени работы графика для генерации графика работы энкодера (7 байт)

// адрес хранения привязок DS18B20 к адресам, максимум 100 записей, одна запись - 10 байт, т.е. отводится 1000 байт на это дело
#define DS18B20_BINDING_ADDRESS 2200

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// версия ПО
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SOFTWARE_VERSION "v.0.1a"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// время в миллисекундах, после которого идёт переключение на главный экран, если какой-то экран висит долгое время
#define RESET_TO_MAIN_SCREEN_DELAY 3600000
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
#define TRANSFORMER_HIGH_DEFAULT_BORDER 3500    // верхний порог
#define TRANSFORMER_LOW_DEFAULT_BORDER 2500     // нижний порог
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NO_TEMPERATURE_DATA -128 // нет данных с датчика температуры
















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

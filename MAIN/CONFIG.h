#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define _DEBUG // закомментировать для выключения отладочной информации
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки пинов прерываний
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define INTERRUPT1_PIN 7
#define INTERRUPT2_PIN 8
#define INTERRUPT3_PIN 35

#define INTERRUPT_MAX_IDLE_TIME 900000    // время, в микросекундах, после получения последнего импульса и до момента, когда будет принято решение, что импульсы на входе закончились
#define INTERRUPT_SCREEN_SEE_TIME 60000    // сколько миллисекунд демонстрировать экран с графиками прерываний?
#define INTERRUPT_CHART_X_POINTS 150      // кол-во точек по X на графике прерываний
#define INTERRUPT_CHART_Y_POINTS 100      // кол-во точек по Y на графике прерываний
#define INTERRUPT_CHART_X_COORD 5         // Х-координата графика прерываний на экране
#define INTERRUPT_CHART_Y_COORD 120       // стартовая Y-координата графика прерываний на экране

// стартовые координаты верхнего/левого угла сетки
#define INTERRUPT_CHART_GRID_X_START 5 
#define INTERRUPT_CHART_GRID_Y_START 20
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка светодиодов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LED_ON_LEVEL HIGH // уровень включения светодиодов

#define LED_READY  37                // Индикация светодиодом "Готов"
#define LED_FAILURE  36              // Индикация светодиодом "Авария"
#define LED_TEST  38                 // Индикация светодиодом "Тест"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка кнопок
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define BUTTON_RED  39                  // Кнопка №1, красная
#define BUTTON_BLUE1  40                  // Кнопка №2, синяя
#define BUTTON_BLUE2  41                  // Кнопка №3, синяя
#define BUTTON_YELLOW  9                   // Кнопка №4, жёлтая
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройка логов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOGS_DIRECTORY "/LOG" // папка, в которой лежат логи
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define VOLTAGE_MEASURE_THRESHOLD 1000 // через сколько миллисекунд обновлять показания с линий вольтажа, для показа на главном экране
#define VOLTAGE_THRESHOLD 10 // порог изменений, в процентах, для принятия решения о том, хороший вольтаж или нет
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки EEPROM
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define COUNT_PULSES_STORE_ADDRESS1 10 // по какому адресу храним переменную для кол-ва импульсов канала 1 (2 байта)
#define COUNT_PULSES_STORE_ADDRESS2 12 // по какому адресу храним переменную для кол-ва импульсов канала 2 (2 байта)
#define COUNT_PULSES_STORE_ADDRESS3 14 // по какому адресу храним переменную для кол-ва импульсов канала 3 (2 байта)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

































#ifdef _DEBUG
  #define DBG(s) { Serial << (s); Serial.flush(); }
  #define DBGLN(s) { Serial << (s) << ENDL; Serial.flush(); }
#else
  #define DBG(s) (void) 0
  #define DBGLN(s) (void) 0
#endif
#define ENDL '\n'
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T> inline Stream& operator << (Stream &s, T n) { s.print(n); return s; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


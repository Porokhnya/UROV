#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// НАСТРОЙКИ ПРОШИВКИ
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SERIAL_SPEED                      57600        // скорость работы UART (должна быть одинакова в прошивках мастера и слейва!)
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// закомментировать для выключения отладочного режима (выводит в DEBUG_SERIAL отладочную информацию на скорости SERIAL_SPEED бод)
// ВНИМАНИЕ!!! ПРИ ВКЛЮЧЕННОМ ОТЛАДОЧНОМ РЕЖИМЕ, если для RS485 используется DEBUG_SERIAL - RS-485 РАБОТАТЬ НЕ БУДЕТ!!!
#define _DEBUG 

#define DEBUG_SERIAL                      Serial      // какой Serial использовать для отладки


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки RS-485
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define RS485_DE_PIN                      A0          // номер пина управления режимом приёма/передачи MAX485
#define RS485_READING_TIMEOUT             200          // таймаут чтения из шины RS485, миллисекунд
#define RS485_SERIAL                      Serial1      // какой аппаратный UART использовать для RS-485


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки обработки прерываний
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define RELAY_PIN                         11          // пин, на котором ловятся импульсы срабатывания защиты
#define RELAY_TRIGGER_LEVEL               HIGH        // уровень, при котором срабатывает защита
#define INTERRUPT_BEGIN_DELAY             5000        // время ожидания (в микросекундах) между срабатыванием защиты до старта сбора информации с энкодера
#define INTERRUPT_MAX_IDLE_TIME           1200000     // время, в микросекундах, после получения последнего импульса с энкодера и до момента, когда будет принято решение, что импульсы на входе закончились
#define INTERRUPT_RESERVE_RECORDS         50          // на сколько записей (каэдая запись - 4 байта) резервировать список прерываний c энкодера

#define ENCODER_PIN1                      2           // номер пина, на котором ловятся импульсы от энкодера штанги
#define ENCODER_INTERRUPT_LEVEL           RISING      // по какому уровню ловим прерывания энкодера

#define ENDSTOP_UP_PIN                    7           // Концевик позиции штанги, вверху
#define ENDSTOP_DOWN_PIN                  8           // Концевик позиции штанги, внизу
#define ENDSTOP_TRIGGER_LEVEL             HIGH        // уровень, на который проверяем в процессе опроса концевиков
#define ENDSTOP_FREQUENCY                 450         // рабочая частота импульсов концевика, Герц
#define ENDSTOP_HISTERESIS                30          // гистерезис частоты, Герц

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// дальше лазить - НЕОСМОТРИТЕЛЬНО !!!!
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _DEBUG
  #define DBG(s) { DEBUG_SERIAL.print((s)); }
  #define DBGLN(s) { DEBUG_SERIAL.println((s)); }
#else
  #define DBG(s) (void) 0
  #define DBGLN(s) (void) 0
#endif
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


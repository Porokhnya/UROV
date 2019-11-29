#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------------------------------------------
#include <STM32RTC.h>      // Часы
//--------------------------------------------------------------------------------------------------------------------------------------
struct DS3231Time // данные по текущему времени
{
  uint8_t second; // секунда (0-59)
  uint8_t minute; // минута (0-59)
  uint8_t hour; // час (0-23)
  uint8_t dayOfWeek; // день недели (1 - понедельник и т.д.)
  uint8_t dayOfMonth; // день месяца (0-31)
  uint8_t month; // месяц(1-12)
  uint16_t year; // формат - ХХХХ

  static bool isLeapYear(uint16_t year);
  uint32_t unixtime(void);
  static DS3231Time fromUnixtime(uint32_t timeInput);
  
  uint16_t date2days(uint16_t _year, uint8_t _month, uint8_t _day);
  long time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);

  DS3231Time addDays(long days);
  
}; 
//--------------------------------------------------------------------------------------------------------------------------------------
struct DS3231Temperature // структура показаний с модуля часов
{
  int8_t Value; // значение градусов (-128 - 127)
  uint8_t Fract; // сотые доли градуса (значение после запятой)
};
//--------------------------------------------------------------------------------------------------------------------------------------
class _RealtimeClock
{
  public:
    _RealtimeClock();
    void setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year);
    void setTime(const DS3231Time& time);


    const char* getDayOfWeekStr(const DS3231Time& t);
    const char* getTimeStr(const DS3231Time& t);
    const char* getDateStr(const DS3231Time& t);

    DS3231Time getTime(); 

    DS3231Temperature getTemperature();
    
    void begin();
        
  private:
    static char workBuff[12]; // буфер под дату/время
    
  
};
//--------------------------------------------------------------------------------------------------------------------------------------
extern _RealtimeClock RealtimeClock;
//--------------------------------------------------------------------------------------------------------------------------------------



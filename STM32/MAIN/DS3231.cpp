#include "DS3231.h"
#include "ConfigPin.h"
//--------------------------------------------------------------------------------------------------------------------------------------
_RealtimeClock RealtimeClock;
char _RealtimeClock::workBuff[12] = {0};
const uint8_t daysArray [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
//--------------------------------------------------------------------------------------------------------------------------------------
uint32_t DS3231Time::unixtime(void)
{
  uint32_t u;

  u = time2long(date2days(this->year, this->month, this->dayOfMonth), this->hour, this->minute, this->second);
  u += 946684800; // + 01.01.2000 00:00:00 unixtime

  return u;
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Time DS3231Time::addDays(long days)
{
  uint32_t ut = unixtime();
  long diff = days*86400;
  ut += diff;  
  return fromUnixtime(ut);
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Time DS3231Time::fromUnixtime(uint32_t time)
{
  DS3231Time result;

  uint8_t year;
  uint8_t month, monthLength;
  unsigned long days;

  result.second = time % 60;
  time /= 60; // now it is minutes
  result.minute = time % 60;
  time /= 60; // now it is hours
  result.hour = time % 24;
  time /= 24; // now it is days
  
  year = 0;  
  days = 0;
  while((unsigned)(days += (isLeapYear(year) ? 366 : 365)) <= time) {
    year++;
  }
  result.year = year + 1970; // year is offset from 1970 
  
  days -= isLeapYear(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0
  
  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) 
  {
    if (month==1) 
    { // february
      if (isLeapYear(year)) 
      {
        monthLength=29;
      } 
      else 
      {
        monthLength=28;
      }
    } 
    else 
    {
      monthLength = pgm_read_byte(daysArray + month); //monthDays[month];
    }
    
    if (time >= monthLength) 
    {
      time -= monthLength;
    } 
    else 
    {
        break;
    }
  }
  result.month = month + 1;  // jan is month 1  
  result.dayOfMonth = time + 1;     // day of month  

    int dow;
    byte mArr[12] = {6,2,2,5,0,3,5,1,4,6,2,4};
    dow = (result.year % 100);
    dow = dow*1.25;
    dow += result.dayOfMonth;
    dow += mArr[result.month-1];
    
    if (isLeapYear(result.year) && (result.month<3))
     dow -= 1;
     
    while (dow>7)
     dow -= 7;

   result.dayOfWeek = dow;

  return result;
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool DS3231Time::isLeapYear(uint16_t year)
{
  return (year % 4 == 0);
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint16_t DS3231Time::date2days(uint16_t _year, uint8_t _month, uint8_t _day)
{
    _year = _year - 2000;

    uint16_t days16 = _day;

    for (uint8_t i = 1; i < _month; ++i)
    {
        days16 += pgm_read_byte(daysArray + i - 1);
    }

    if ((_month == 2) && isLeapYear(_year))
    {
        ++days16;
    }

    return days16 + 365 * _year + (_year + 3) / 4 - 1;
}
//--------------------------------------------------------------------------------------------------------------------------------------
long DS3231Time::time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds)
{    
    return ((days * 24L + hours) * 60 + minutes) * 60 + seconds;
}
//--------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_INTERNAL_CLOCK
//--------------------------------------------------------------------------------------------------------------------------------------
_RealtimeClock::_RealtimeClock()
{
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Temperature _RealtimeClock::getTemperature()
{
  DS3231Temperature t;
  return t;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void _RealtimeClock::begin(uint8_t wireNum)
{

  STM32RTC& rtc = STM32RTC::getInstance(); 
  // Select RTC clock source: LSI_CLOCK, LSE_CLOCK or HSE_CLOCK.
  // By default the LSI is selected as source.
  rtc.setClockSource(STM32RTC::LSE_CLOCK);     // 
  rtc.begin(); // initialize RTC 24H format
    
}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Time _RealtimeClock::getTime()
{
  STM32RTC& rtc = STM32RTC::getInstance(); 
  DS3231Time result;

  uint32_t subSeconds;
   rtc.getTime((uint8_t*)&(result.hour), (uint8_t*)&(result.minute), (uint8_t*)&(result.second),&subSeconds);

   uint8_t y;
   rtc.getDate((uint8_t*)&(result.dayOfWeek), (uint8_t*)&(result.dayOfMonth), (uint8_t*)&(result.month),&y);
   result.year = 2000+y;

  return result;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void _RealtimeClock::setTime(const DS3231Time& time)
{
  setTime(time.second, time.minute, time.hour, time.dayOfWeek, time.dayOfMonth, time.month,time.year);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void _RealtimeClock::setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year)
{
  STM32RTC& rtc = STM32RTC::getInstance(); 

  while(year > 100) // приводим к диапазону 0-99
  {
    year -= 100;
  }
  
  rtc.setTime(hour, minute, second);  
  rtc.setDate(dayOfWeek,dayOfMonth,month, year);
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* _RealtimeClock::getDayOfWeekStr(const DS3231Time& t)
{
  static const char* dow[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  return dow[t.dayOfWeek-1];
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* _RealtimeClock::getTimeStr(const DS3231Time& t)
{
  char* writePtr = workBuff;
  
  if(t.hour < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.hour/10) + '0';

  *writePtr++ = (t.hour % 10) + '0';

 *writePtr++ = ':';

 if(t.minute < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.minute/10) + '0';

 *writePtr++ = (t.minute % 10) + '0';

 *writePtr++ = ':';

 if(t.second < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.second/10) + '0';

 *writePtr++ = (t.second % 10) + '0';

 *writePtr = 0;

 return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* _RealtimeClock::getDateStr(const DS3231Time& t)
{
  char* writePtr = workBuff;
  if(t.dayOfMonth < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.dayOfMonth/10) + '0';
  *writePtr++ = (t.dayOfMonth % 10) + '0';

  *writePtr++ = '.';

  if(t.month < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.month/10) + '0';
  *writePtr++ = (t.month % 10) + '0';

  *writePtr++ = '.';

  *writePtr++ = (t.year/1000) + '0';
  *writePtr++ = (t.year % 1000)/100 + '0';
  *writePtr++ = (t.year % 100)/10 + '0';
  *writePtr++ = (t.year % 10) + '0';  

  *writePtr = 0;

  return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
#else
//--------------------------------------------------------------------------------------------------------------------------------------
_RealtimeClock::_RealtimeClock()
{
    wireInterface = &Wire;
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t _RealtimeClock::dec2bcd(uint8_t val)
{
  return( (val/10*16) + (val%10) );
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t _RealtimeClock::bcd2dec(uint8_t val)
{
  return( (val/16*10) + (val%16) );
}
//--------------------------------------------------------------------------------------------------------------------------------------
void _RealtimeClock::setTime(const DS3231Time& time)
{
  setTime(time.second, time.minute, time.hour, time.dayOfWeek, time.dayOfMonth, time.month,time.year);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void _RealtimeClock::setTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint16_t year)
{

  while(year > 100) // приводим к диапазону 0-99
  {
    year -= 100;
  }
 
  wireInterface->beginTransmission(DS3231Address);
  
  wireInterface->write(0); // указываем, что начинаем писать с регистра секунд
  wireInterface->write(dec2bcd(second)); // пишем секунды
  wireInterface->write(dec2bcd(minute)); // пишем минуты
  wireInterface->write(dec2bcd(hour)); // пишем часы
  wireInterface->write(dec2bcd(dayOfWeek)); // пишем день недели
  wireInterface->write(dec2bcd(dayOfMonth)); // пишем дату
  wireInterface->write(dec2bcd(month)); // пишем месяц
  wireInterface->write(dec2bcd(year)); // пишем год
  
  wireInterface->endTransmission(true);  

  delay(10); // немного подождём для надёжности

}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Temperature _RealtimeClock::getTemperature()
{

     DS3231Temperature res;
      
     union int16_byte {
           int i;
           byte b[2];
       } rtcTemp;
         
      wireInterface->beginTransmission(DS3231Address);
      wireInterface->write(0x11);
      if(wireInterface->endTransmission(true) != 0) // ошибка
        return res;
    
      if(wireInterface->requestFrom(DS3231Address, 2) == 2)
      {
        rtcTemp.b[1] = wireInterface->read();
        rtcTemp.b[0] = wireInterface->read();
    
        long tempC100 = (rtcTemp.i >> 6) * 25;
    
        res.Value = tempC100/100;
        res.Fract = abs(tempC100 % 100);
        
      }
      
      return res;

}
//--------------------------------------------------------------------------------------------------------------------------------------
DS3231Time _RealtimeClock::getTime()
{
  static DS3231Time t;
  static uint32_t lastRequestTime = 0;
  
  if(!lastRequestTime)
    memset(&t,0,sizeof(t));


     if(!lastRequestTime || ( (millis() - lastRequestTime) > 999) )
     {
        wireInterface->beginTransmission(DS3231Address);
        wireInterface->write(0); // говорим, что мы собираемся читать с регистра 0
        
        if(wireInterface->endTransmission(true) != 0) // ошибка
          return t;
        
        if(wireInterface->requestFrom(DS3231Address, 7) == 7) // читаем 7 байт, начиная с регистра 0
        {
            t.second = bcd2dec(wireInterface->read() & 0x7F);
            if(t.second > 59)
              t.second = 59;
            
            t.minute = bcd2dec(wireInterface->read());
            if(t.minute > 59)
              t.minute = 59;
            
            t.hour = bcd2dec(wireInterface->read() & 0x3F);
            if(t.hour > 23)
              t.hour = 23;
            
            t.dayOfWeek = bcd2dec(wireInterface->read());
            t.dayOfWeek = constrain(t.dayOfWeek,1,7);
            
            t.dayOfMonth = bcd2dec(wireInterface->read());
            t.dayOfMonth = constrain(t.dayOfMonth,1,31);
            
            t.month = bcd2dec(wireInterface->read());
            t.month = constrain(t.month,1,12);
            
            t.year = bcd2dec(wireInterface->read());     
            t.year += 2000; // приводим время к нормальному формату
        } // if

        lastRequestTime = millis();
     }
      
      return t;

}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* _RealtimeClock::getDayOfWeekStr(const DS3231Time& t)
{
  uint8_t idx = t.dayOfWeek;
  if(idx > 0)
    --idx;

  if(idx > 6)
    idx = 6;
    
  static const char* dow[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  return dow[idx];
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* _RealtimeClock::getTimeStr(const DS3231Time& t)
{
  char* writePtr = workBuff;
  
  if(t.hour < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.hour/10) + '0';

  *writePtr++ = (t.hour % 10) + '0';

 *writePtr++ = ':';

 if(t.minute < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.minute/10) + '0';

 *writePtr++ = (t.minute % 10) + '0';

 *writePtr++ = ':';

 if(t.second < 10)
  *writePtr++ = '0';
 else
  *writePtr++ = (t.second/10) + '0';

 *writePtr++ = (t.second % 10) + '0';

 *writePtr = 0;

 return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
const char* _RealtimeClock::getDateStr(const DS3231Time& t)
{
  char* writePtr = workBuff;
  if(t.dayOfMonth < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.dayOfMonth/10) + '0';
  *writePtr++ = (t.dayOfMonth % 10) + '0';

  *writePtr++ = '.';

  if(t.month < 10)
    *writePtr++ = '0';
  else
    *writePtr++ = (t.month/10) + '0';
  *writePtr++ = (t.month % 10) + '0';

  *writePtr++ = '.';

  *writePtr++ = (t.year/1000) + '0';
  *writePtr++ = (t.year % 1000)/100 + '0';
  *writePtr++ = (t.year % 100)/10 + '0';
  *writePtr++ = (t.year % 10) + '0';  

  *writePtr = 0;

  return workBuff;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void _RealtimeClock::begin(uint8_t wireNumber)
{
    wireInterface = &Wire;
}
//--------------------------------------------------------------------------------------------------------------------------------------
#endif


#include "Endstop.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Endstop::Endstop(uint8_t _pin, uint8_t _level,uint32_t _minInterval, uint32_t _maxInterval) 
{
  pin = _pin;
  level = _level;
  minInterval = _minInterval;
  maxInterval = _maxInterval;
  lastTrigTime = 0;
  state = false;
  machineState = esWaitForTrig;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Endstop::begin()
{
  pinMode(pin, level == LOW ? INPUT_PULLUP : INPUT);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Endstop::update()
{
  switch(machineState)
  {
    case esWaitForTrig: // ждём уровня срабатывания
    {
       if(digitalRead(pin) == level) // поймали нужный уровень
        {
          uint32_t now = micros();
          
          if(lastTrigTime) // был предыдущий уровень
          {
            uint32_t intervalTime = now - lastTrigTime;
            lastTrigTime = now;

           // если интервал между двумя уровнями укладывается в границы частоты концевика - он не сработал
           if(intervalTime >= minInterval && intervalTime <= maxInterval)
              state = false; // не сработал
           else
              state = true; // сработал
            
          }
          else
          {
            lastTrigTime = now; // первый импульс
          }

          // раз мы поймали уровень срабатывания - нам надо его игнорировать до тех пор,
          // пока мы не поймаем уровень несрабатывания. Поэтому переключаемся на другую ветку
          machineState = esWaitForRelease;
          
        } // if(digitalRead(pin) == level)      
    }
    break; // esWaitForTrig

    case esWaitForRelease: // ждём уровня несрабатывания
    {
      if(digitalRead(pin) != level) // поймали уровень несрабатывания, переключаемся на ожидание уровня срабатывания, как только мы его поймаем - интервал между двумя уровнями и будет частотой
      {
        machineState = esWaitForTrig;
      }      
    }
    break; // esWaitForRelease
    
  } // switch
  
 
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


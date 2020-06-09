//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "FileUtils.h"
#include "CONFIG.h"
#include "PulsesGen.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const uint8_t IMPULSE_PIN = 100; // НОМЕР ПИНА, НА КОТОРОМ БУДУТ ГЕНЕРИРОВАТЬСЯ ИМПУЛЬСЫ
const uint8_t PULSE_ON_LEVEL = HIGH; // УРОВЕНЬ ВКЛЮЧЕННОГО ИМПУЛЬСА
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ImpulseGeneratorClass ImpulseGenerator(IMPULSE_PIN);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ImpulseGeneratorClass::ImpulseGeneratorClass(uint8_t p)
{
  pin = p;
  workMode = igNothing;
  pList = NULL;
  memAddress = 0;
  memCount = 0;
  lastMicros = 0;
  listIterator = 0;
  pauseTime = 0;
  currentPinLevel = !PULSE_ON_LEVEL;
  done = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::pinConfig()
{
  pinMode(pin,OUTPUT);
  currentPinLevel = !PULSE_ON_LEVEL;
  digitalWrite(pin,currentPinLevel);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t ImpulseGeneratorClass::getNextPauseTime(bool& done)
{
  done = false;
  uint32_t result = 0;
  uint8_t* ptr = (uint8_t*)&result;

 switch(workMode)
  {
    case igNothing: 
    {
      done = true;
    }
    break;
    
    case igFile:
    {
        if(file.isOpen())
        {
            for(size_t i=0;i<sizeof(result);i++)
            {
              int iCh = file.read();
              if(iCh == -1) // конец файла или ошибка чтения
              {
                done = true;
                break;
              }
              else
              {
                *ptr++ = (uint8_t) iCh;
              }
            } // for
            
        }
        else
        {
          done = true;
        }
    }
    break;

    case igEEPROM:
    {
      if(!memCount) // все записи из EEPROM вычитаны
      {
        done = true;
      }
      else
      {
          for(size_t i=0;i<sizeof(result);i++)
          {
              *ptr++ = Settings.read(memAddress);
              memAddress++;
              
          } // for
          
          if(memCount)
          {
            memCount--;
          }
      } // else
    }
    break;

    case igList:
    {
      if(!pList || listIterator >= pList->size()) // весь список пробежали
      {
        done = true;        
      }
      else
      {
        result = (*pList)[listIterator];
        listIterator++;
      }
    }
    break;
  }

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::wipe()
{
  switch(workMode)
  {
    case igNothing: 
    break;
    
    case igFile:
    {
        if(file.isOpen())
        {
          file.close();
        }
    }
    break;

    case igEEPROM:
    {
      memAddress = 0;
      memCount = 0;
    }
    break;

    case igList:
    {
      pList = NULL;
      listIterator = 0;
    }
    break;
  }

  pauseTime = 0;
  lastMicros = 0;
  
  workMode = igNothing;
  
  currentPinLevel = !PULSE_ON_LEVEL;
  digitalWrite(pin,currentPinLevel);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::start(const String& fileName)
{
  wipe();
  pinConfig();

#ifndef _SD_OFF
  if(!file.open(fileName.c_str(),O_READ))
    return;  

  file.rewind();
  
  workMode = igFile;
  done = false;

#endif // _SD_OFF

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::start(int memAddr)
{
    pinConfig();
  
    memAddress = memAddr;
    memCount = 0;

    uint8_t* ptr = (uint8_t*)&memCount;
    for(size_t i=0;i<sizeof(memCount);i++)
    {
      *ptr++ = Settings.read(memAddress);
      memAddress++;
    }

  workMode = igEEPROM;
  done = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::start(const Vector<uint32_t>& list)
{
  pinConfig();
  
  pList = &list;
  listIterator = 0;
  workMode = igList;
  done = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::stop()
{
  wipe();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::update()
{
  if(workMode == igNothing || done) // не работаем никак, или уже закончили
  {
    return;
  }
  
  if(micros() - lastMicros >= pauseTime) // время паузы между сменой уровня вышло
  {
    pauseTime = getNextPauseTime(done);
    
    if(!done) // ещё не закончили работу, время паузы вышло, меняем уровень на пине
    {
      currentPinLevel = !currentPinLevel;
      digitalWrite(pin,currentPinLevel);

      lastMicros = micros(); // не забываем, что надо засечь текущее время
    }
    else // работа закончена, список импульсов кончился
    {
      wipe();
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


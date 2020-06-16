//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "FileUtils.h"
#include "CONFIG.h"
#include "PulsesGen.h"
#include "Settings.h"
#include "DueTimer.h"
#include "digitalWriteFast.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const uint8_t IMPULSE_PIN_A = 53;    // НОМЕР ПИНА A, НА КОТОРОМ БУДУТ ГЕНЕРИРОВАТЬСЯ ИМПУЛЬСЫ
const uint8_t IMPULSE_PIN_B = 49;    // НОМЕР ПИНА B, НА КОТОРОМ БУДУТ ГЕНЕРИРОВАТЬСЯ ИМПУЛЬСЫ
const uint8_t PULSE_ON_LEVEL = HIGH; // УРОВЕНЬ ВКЛЮЧЕННОГО ИМПУЛЬСА
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ImpulseGeneratorClass ImpulseGeneratorA(IMPULSE_PIN_A);
ImpulseGeneratorClass ImpulseGeneratorB(IMPULSE_PIN_B);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile bool timerAttached = false;
volatile bool timerStarted = false;
volatile uint8_t timerUsed = 0;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void genUpdate()
{  
  ImpulseGeneratorA.update();
  ImpulseGeneratorB.update();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ImpulseGeneratorClass::ImpulseGeneratorClass(uint8_t p)
{
  pin = p;
  
  workMode = igNothing;
  pList = NULL;
  listIterator = 0;
  
//  memAddress = 0;
//  memCount = 0;
  lastMicros = 0;
  pauseTime = 0;
//  currentPinLevel = !PULSE_ON_LEVEL;
  done = false;
  inUpdateFlag = false;
  machineState = onBetweenPulses;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::pinConfig()
{
  pinModeFast(pin,OUTPUT);
  //currentPinLevel = !PULSE_ON_LEVEL;
  digitalWriteFast(pin,!PULSE_ON_LEVEL);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::timerStart()
{
    timerUsed++;
    if(!timerStarted)
    {
      timerStarted = true;
      GEN_TIMER.start();
    }  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::timerStop()
{
  if(timerUsed)
  {
    timerUsed--;
  }
  
  if(!timerUsed)
  {
     GEN_TIMER.stop();
     timerStarted = false;
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::timerConfig()
{
    if(!timerAttached)
    {
      timerAttached = true;
      GEN_TIMER.attachInterrupt(genUpdate);
      GEN_TIMER.setPeriod(GEN_TIMER_PERIOD);
    }

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t ImpulseGeneratorClass::getNextPauseTime(bool& done)
{
  done = false;
  uint32_t result = 0;
 // uint8_t* ptr = (uint8_t*)&result;

 switch(workMode)
  {
    case igNothing: 
    {
      done = true;
    }
    break;
/*    
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
*/
    case igExternalList: // внешний список
    case igInternalList: // внутренний список
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
  timerStop();

  internalList.clear(); // очищаем внутренний список
  pList = NULL;
  listIterator = 0;
  
/*  
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
*/
  pauseTime = 0;
  lastMicros = 0;
  
  workMode = igNothing;
  
  //currentPinLevel = !PULSE_ON_LEVEL;  
  digitalWriteFast(pin,!PULSE_ON_LEVEL);
  machineState = onBetweenPulses;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::prepare(const String& fileName)
{
  wipe();
  /*
  pinConfig();
  timerConfig();
  */


#ifndef _SD_OFF

  SdFile file;
  
  if(!file.open(fileName.c_str(),O_READ))
    return;  

  file.rewind();

  bool canContinue = true;
  
  while(canContinue)
  {
          uint32_t record = 0;
          uint8_t* ptr = (uint8_t*)&record;
          
          for(size_t i=0;i<sizeof(record);i++)
          {
            int iCh = file.read();
            if(iCh == -1) // конец файла или ошибка чтения
            {
              canContinue = false;
              break;
            }
            else
            {
              *ptr++ = (uint8_t) iCh;
            }
          } // for
      if(canContinue)
      {
          internalList.push_back(record);
      }
  } // while


  file.close();

  if(internalList.size())
  {
    workMode = igInternalList;
    pList = &internalList;
    listIterator = 0;
  }
  
  /*
  workMode = igFile;
  done = false;
  lastMicros = micros(); // не забываем, что надо засечь текущее время

  timerStart();
  */

#endif // _SD_OFF

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::prepare(int memAddr)
{
  wipe();

  uint32_t totalRecords = 0;
  uint8_t* ptr = (uint8_t*)&totalRecords;
  for(size_t i=0;i<sizeof(totalRecords);i++)
  {
    *ptr++ = Settings.read(memAddr);
    memAddr++;
  }

  if(totalRecords > 0)
  {
      for(uint32_t i=0;i<totalRecords;i++)
      {
        uint32_t record = 0; 
        ptr = (uint8_t*)&record;

        for(size_t k=0;k<sizeof(record);k++)
        {
          *ptr++ = Settings.read(memAddr);
          memAddr++;
        }

        internalList.push_back(record);
      } // for
  } // if

  if(internalList.size())
  {
    workMode = igInternalList;
    pList = &internalList;
    listIterator = 0;    
  }
  
  /*
    pinConfig();
    timerConfig();
  
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
  lastMicros = micros(); // не забываем, что надо засечь текущее время

  timerStart();
  */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::prepare(const Vector<uint32_t>& list)
{
    wipe();
    
    if(list.size())
    {
      pList = &list;
      listIterator = 0;
      workMode = igExternalList;
    }
    
  /*
  pinConfig();
  timerConfig();
  
  pList = &list;
  listIterator = 0;
  workMode = igList;
  done = false;
  lastMicros = micros(); // не забываем, что надо засечь текущее время

  timerStart();
  */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::start()
{
  if(workMode == igNothing)
  {
    return;
  }

  pinConfig();
  timerConfig();
  done = false;

  pauseTime = getNextPauseTime(done);
  machineState = onBetweenPulses;
  
  lastMicros = micros(); // не забываем, что надо засечь текущее время
  timerStart();
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::stop()
{
  wipe();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::update()
{
  if(inUpdateFlag || workMode == igNothing)// || done) // не работаем никак, или уже закончили
  {
    return;
  }

  inUpdateFlag = true;

  // проверяем состояние конечного автомата
  switch(machineState)
  {
      case onHighLevel: // обрабатываем высокий уровень на пине
      {
        if(micros() - lastMicros >= PULSE_WIDTH) // вышло время удержания высокого уровня на пине
        {
          digitalWriteFast(pin,!PULSE_ON_LEVEL); // низкий уровень на пин
          
          if(!done) // получаем следующее время паузы
          {
            pauseTime = getNextPauseTime(done);
            machineState = onBetweenPulses; // переключаемся на ожидание паузы между импульсами
            lastMicros = micros(); // не забываем, что надо засечь текущее время
          } // if
          else
          {
            // всё, работа закончена, это был последний импульс, поскольку предыдущий вызов getNextPauseTime выставил done в true
            wipe();
          }
        }
      }
      break; // onHighLevel

      case onBetweenPulses: // находимся в паузе между импульсами
      {
        if(micros() - lastMicros >= pauseTime) // время паузы между импульсами вышло
        {
            digitalWriteFast(pin,PULSE_ON_LEVEL); // высокий уровень на пин
            machineState = onHighLevel; // переключаемся в ветку ожидания окончания высокого уровня на пине
            lastMicros = micros(); // не забываем, что надо засечь текущее время
        }
      }
      break; // onBetweenPulses
  } // switch

  /*
  
  if(micros() - lastMicros >= pauseTime) // время паузы между сменой уровня вышло
  {
    
    pauseTime = getNextPauseTime(done);
    
    if(!done) // ещё не закончили работу, время паузы вышло, меняем уровень на пине
    {
      
      //currentPinLevel = !currentPinLevel;      
      //digitalWriteFast(pin,currentPinLevel);
      
      digitalWriteFast(pin,PULSE_ON_LEVEL);
      delayMicroseconds(PULSE_WIDTH);
      digitalWriteFast(pin,!PULSE_ON_LEVEL);

      lastMicros = micros(); // не забываем, что надо засечь текущее время

    }
    else // работа закончена, список импульсов кончился
    {
      wipe();
    }
  }
  */

  inUpdateFlag = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


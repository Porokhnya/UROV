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
//volatile bool timerAttached = false;
//volatile bool timerStarted = false;
//volatile uint8_t timerUsed = 0;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void genUpdate()
{  
  
  if(ImpulseGeneratorA.isDone())// && ImpulseGeneratorB.isDone())
  {
    return;
  }
  ImpulseGeneratorA.update();
//  ImpulseGeneratorB.update();

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ImpulseGeneratorClass::ImpulseGeneratorClass(uint8_t p)
{
  pin = p;
  
  workMode = igNothing;
  pList = &internalList;
  listIterator = 0;
  
  lastMicros = 0;
  pauseTime = 0;
  
  done = false;
  inUpdateFlag = false;
  machineState = onBetweenPulses;

  stopped = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::pinConfig()
{
  pinModeFast(pin,OUTPUT);
  digitalWriteFast(pin,!PULSE_ON_LEVEL);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
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
*/
uint32_t ImpulseGeneratorClass::getNextPauseTime(bool& done)
{
  done = false;
  uint32_t result = 0;

 switch(workMode)
  {
    case igNothing: // ничего не делаем
    {
      done = true;
      stopped = true;
    }
    break;
    
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
//  timerStop();
  
  listIterator = 0;
  
  pauseTime = 0;
  lastMicros = 0;
  
  digitalWriteFast(pin,!PULSE_ON_LEVEL);
  machineState = onBetweenPulses;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::prepare(const String& fileName)
{
  wipe();


#ifndef _SD_OFF

  SdFile file;
  
  if(!file.open(fileName.c_str(),O_READ))
    return;  

  file.rewind();

  bool canContinue = true;

  internalList.clear();
  
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
  else
  {
    workMode = igNothing;
  }
  

#endif // _SD_OFF

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::prepare(int memAddr)
{
  wipe();

  internalList.clear();

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
  else
  {
    workMode = igNothing;
  }

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::prepare(const Vector<uint32_t>& list)
{
    wipe();

    internalList.clear();
    
    if(list.size())
    {
      pList = &list;
      listIterator = 0;
      workMode = igExternalList;
    }
    else
    {
      workMode = igNothing;
    }    
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::start()
{
  if(workMode == igNothing)
  {
    return;
  }

  #ifdef _DEBUG

    if(!pList)
    {
      DBGLN("pList is NULL, AHTUNG PARTIZANEN !!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }

    DBGLN("-----------------------------------------------------");
    DBG("COUNT OF PULSES FOR GENERATOR IS: ");
    if(pList)
    {
      DBGLN(pList->size());
    }
    else
    {
      DBGLN(0);
    }
    DBGLN("LIST OF PULSES FOR GENERATOR IS:");
    DBGLN("-----------------------------------------------------");

    if(pList)
    {
      for(size_t i=0;i<pList->size();i++)
      {
        DBGLN((*pList)[i]);
      }
    }   
    Serial.flush();
    
  #endif // _DEBUG

  pinConfig();
//  timerConfig();
  listIterator = 0;

  pauseTime = getNextPauseTime(done);

  DBG("FIRST PAUSE TIME IS: ");
  DBGLN(pauseTime);
  

  DBGLN("-----------------------------------------------------");
  DBGLN("");
  
  #ifdef _DEBUG
    Serial.flush();
  #endif
  
  lastMicros = micros(); // не забываем, что надо засечь текущее время
//  timerStart();

  machineState = onBetweenPulses;
  done = false;
  stopped = false;
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::stop()
{
  stopped = true;
  wipe();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ImpulseGeneratorClass::update()
{
  if(inUpdateFlag || workMode == igNothing || stopped) // не работаем никак, или уже закончили
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
            stop();
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

  inUpdateFlag = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


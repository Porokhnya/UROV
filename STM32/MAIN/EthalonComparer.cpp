//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "EthalonComparer.h"
#include "CONFIG.h"
#include "ConfigPin.h"
#include "Settings.h"
#include "FileUtils.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EthalonCompareResult EthalonComparer::Compare(InterruptTimeList& list, uint8_t channelNumber, EthalonCompareNumber& compareNumber, String& fileName)//InterruptTimeList& ethalonData)
{
    DBGLN("");
    DBG(F("Сравниваем список импульсов #"));
    DBG(channelNumber);
    DBGLN(F(" с эталоном..."));

    compareNumber = ecnNoEthalon;
    
    InterruptTimeList ethalonData;
    ethalonData.clear();
    
    if(list.size() < 2)
    {
      DBGLN(F("пустой исходный список, нет ничего к сравнению"));
      return COMPARE_RESULT_NoSourcePulses; // нет исходных данных в списке
    }


  // сравниваем кол-во импульсов на канал
  uint16_t skipCounter = Settings.getSkipCounter();
  uint16_t channelPulses = Settings.getPulses() / (skipCounter ? skipCounter : 1);
  uint16_t channelDelta = Settings.getPulsesDelta();

  uint16_t minPulses = channelPulses - channelDelta;
  uint16_t maxPulses = channelPulses + channelDelta;

  DBG("minPulses="); DBGLN(minPulses);
  DBG("maxPulses="); DBGLN(maxPulses);
  DBG("list.size()="); DBGLN(list.size());



  // смотрим позицию штанги
  RodDirection rodPos = Settings.getRodDirection();// (channelNumber);
  
 #ifndef IGNORE_ROD_POSITION
    if(rpBroken == rodPos)
    {
      DBGLN(F("детектирована поломка штанги!"));
      return COMPARE_RESULT_RodBroken;      // штанга поломана
    }
 #endif
      
  // загружаем файл эталона
  
   fileName = "";
   fileName = ETHALONS_DIRECTORY;
   fileName += ETHALON_NAME_PREFIX;
  
   fileName += channelNumber;

  #ifndef IGNORE_ROD_POSITION  
    if(rodPos == rpUp) // штанга в верхней позиции, значит, она поднималась
    {
      fileName += ETHALON_UP_POSTFIX;
      switch(channelNumber)
      {
        case 0:
          compareNumber = ecnE1up;
        break;
        case 1:
          compareNumber = ecnE2up;
        break;
        case 2:
          compareNumber = ecnE3up;
        break;
      }
    }
    else
    {
      fileName += ETHALON_DOWN_POSTFIX;
      switch(channelNumber)
      {
        case 0:
          compareNumber = ecnE1down;
        break;
        case 1:
          compareNumber = ecnE2down;
        break;
        case 2:
          compareNumber = ecnE3down;
        break;
      }
    }
  #else
  {
    fileName += ETHALON_UP_POSTFIX;
      switch(channelNumber)
      {
        case 0:
          compareNumber = ecnE1up;
        break;
        case 1:
          compareNumber = ecnE2up;
        break;
        case 2:
          compareNumber = ecnE3up;
        break;
      }    
  }
  #endif
  
  fileName += ETHALON_FILE_EXT;
  
  if(!SD_CARD.exists(fileName.c_str()))
  {
    DBGLN(F("не найден файл эталона!"));

    compareNumber = ecnNoEthalon;
    return COMPARE_RESULT_NoEthalonFound; // не найдено эталона для канала
  }

  PAUSE_ADC; // останавливаем АЦП
  
  SdFile file;
  file.open(fileName.c_str(),FILE_READ);

  uint32_t ethalonPulsesCount = 0; // количество импульсов эталона
  
  if(file.isOpen())
  {
    file.rewind(); // переходим на начало файла
    uint32_t fSize = file.fileSize();

    #ifndef ETHALON_COMPARE_ONLY_PULSES_COUNT // только если сказали - сравнивать и времена импульсов, не только их количество   
      uint32_t curRec;
      while(file.available())
      {
        int readResult = file.read(&curRec,sizeof(curRec));
        if(readResult == -1 || size_t(readResult) < sizeof(curRec))
        {
          break;
        }
    
          ethalonData.push_back(curRec);
      }

      ethalonPulsesCount = ethalonData.size();
    #else
      // сравниваем только кол-во импульсов
      ethalonPulsesCount = fSize/sizeof(uint32_t);
    #endif // ETHALON_COMPARE_ONLY_PULSES_COUNT
    
    file.close();
  }
  else
  {
    DBGLN(F("не удалось открыть файл эталона!"));

    compareNumber = ecnNoEthalon;
    return COMPARE_RESULT_NoEthalonFound; // не найдено эталона для канала
  }
  
  // прочитали эталон, теперь можно сравнивать. Кол-во импульсов у нас находится в пределах настроенной дельты,
  // и нам надо сравнить импульсы, находящиеся в списке list, с импульсами, находящимися в списке ethalon.


  if(!(list.size() >= minPulses && list.size() <= maxPulses))
  {
    DBGLN(F("кол-во импульсов в списке выходит за границы допустимых - несовпадание с эталоном!"));
    return COMPARE_RESULT_MismatchEthalon; // результат не соответствует эталону, поскольку кол-во импульсов расходится с настройками
  }

  DBG(F("Кол-во импульсов эталона: "));
  DBGLN(ethalonPulsesCount);

  DBG(F("Кол-во собранных импульсов: "));
  DBGLN(list.size());

  #ifndef ETHALON_COMPARE_ONLY_PULSES_COUNT

  // сравниваем времена импульсов
  
  // для начала вычисляем, сколько импульсов сравнивать
  size_t toCompare = min(ethalonPulsesCount,list.size());

  DBG(F("Импульсов к сравнению: "));
  DBGLN(toCompare);

  uint32_t ethalonCompareDelta = Settings.getEthalonPulseDelta();

  // потом проходим по каждому импульсу
  
  for(size_t i=1;i<toCompare;i++)
  {
    uint32_t ethalonPulseDuration = ethalonData[i] - ethalonData[i-1];
    uint32_t passedPulseDuration = list[i] - list[i-1];

    DBG("#");
    DBG(i-1);
    DBG(", ");
    DBG(ethalonPulseDuration);
    DBG("=");
    DBGLN(passedPulseDuration);
    
    uint32_t lowVal;    
    uint32_t highVal = ethalonPulseDuration + ethalonCompareDelta;

    if(ethalonPulseDuration >= ethalonCompareDelta)
    {
      lowVal = ethalonPulseDuration - ethalonCompareDelta;
    }
    else
    {
      lowVal = 0;
    }

    // и если длительность между импульсами различается от эталонной больше, чем на нужную дельту - эталон не совпадает
    if(!(passedPulseDuration >= lowVal && passedPulseDuration <= highVal))
    {
      DBGLN(F("найдено несовпадение с эталоном!"));
      return COMPARE_RESULT_MismatchEthalon;
    }
  }
  #else
    // сравниваем только количество импульсов
    int32_t dt = list.size() - ethalonPulsesCount;

    if(abs(dt) > maxPulses - minPulses)
    {
      // кол-во импульсов в эталоне отличается на кол-во импульсов прерывания больше, чем на дельту
      return COMPARE_RESULT_MismatchEthalon;
    }
  #endif // ETHALON_COMPARE_ONLY_PULSES_COUNT

  DBGLN(F("полное совпадение с эталоном!"));
  DBGLN("");
  return COMPARE_RESULT_MatchEthalon;    // результат соответствует эталону
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

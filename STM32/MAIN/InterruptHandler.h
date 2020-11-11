#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "CONFIG.h"
#include "TinyVector.h"
#include "DS3231.h"
#include "ADCSampler.h"
//--------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<uint32_t> InterruptTimeList;
//--------------------------------------------------------------------------------------------------------------------------------------
// структура-подписчик обработчика прерываний. Имея такой интерфейс - всегда можно переназначить вывод результатов серий измерений
// от одного обработчика прерываний в разных подписчиков, например: в обычном режиме показывается график при срабатывании прерываний,
// в режиме записи эталона - показывается другой экран, который регистрирует себя в качестве временного обработчика результатов
// серий прерываний, и что-то там с ними делает; по выходу с экрана обработчиком результатов прерываний опять назначается экран с 
// графиками. Т.е. имеем гибкий инструмент, кмк.
//--------------------------------------------------------------------------------------------------------------------------------------
// результат сравнения списка прерываний с эталоном
//--------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  COMPARE_RESULT_NoSourcePulses, // нет исходных данных в списке
  COMPARE_RESULT_NoEthalonFound, // не найдено эталона для канала
  COMPARE_RESULT_RodBroken,      // штанга поломана
  COMPARE_RESULT_MatchEthalon,    // результат соответствует эталону
  COMPARE_RESULT_MismatchEthalon, // результат не соответствует эталону
  
} EthalonCompareResult;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  ecnNoEthalon,
  
  ecnE1up,
  ecnE1down,

  ecnE2up,
  ecnE2down,

  ecnE3up,
  ecnE3down,

  
} EthalonCompareNumber;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	dirUp,
	dirDown
} EthalonDirection;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  msIdle, // нормальный режим работы
  msWaitHandleInterrupts, // ждём до начала сбора прерываний
  msHandleInterrupts, // собираем прерывания
  msWaitGuardRelease, // ждём, пока концевик срабатывания защиты не переключится в разомкнутое состояние
  
} MachineState;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct InterruptEventSubscriber
{
  // вызывается, когда прерывания на нужном номере завершены, и накоплена статистика
  virtual void OnInterruptRaised(CurrentOscillData* oscData, EthalonCompareResult result) = 0;

};
//--------------------------------------------------------------------------------------------------------------------------------------
class InterruptHandlerClass
{
 public:
  InterruptHandlerClass();

   void begin();
   void update();
   void pause();
   void resume();

   void setSubscriber(InterruptEventSubscriber* h);
   InterruptEventSubscriber* getSubscriber();
   bool informSubscriber(CurrentOscillData* oscData, EthalonCompareResult compareResult);
   
   static void writeToLog(int32_t dataArrivedTime, DS3231Time& tm, CurrentOscillData* oscData, InterruptTimeList& lst1, EthalonCompareResult res1, EthalonCompareNumber num1, /*InterruptTimeList& ethalonData1*/const String& ethalonFileName, bool toEEPROM=false);


   static void normalizeList(InterruptTimeList& list);

private:

  bool hasAlarm;
  
   static uint32_t writeLogRecord(int32_t dataArrivedTime, CurrentOscillData* oscData, InterruptTimeList& _list, EthalonCompareResult compareResult, EthalonCompareNumber num, /*InterruptTimeList& ethalonData*/const String& ethalonFileName, bool toEEPROM=false, uint32_t curEEPROMWriteAddress=0);


};
//--------------------------------------------------------------------------------------------------------------------------------------
extern InterruptHandlerClass InterruptHandler;
extern InterruptTimeList InterruptData;
//--------------------------------------------------------------------------------------------------------------------------------------

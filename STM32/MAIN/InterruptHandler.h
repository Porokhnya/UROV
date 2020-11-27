#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "CONFIG.h"
#include "TinyVector.h"
#include "DS3231.h"
#include "ADCSampler.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<uint32_t> InterruptTimeList;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class DirectionInfoData
{
  public:
    DirectionInfoData()
    {
      
    }

    InterruptTimeList times;
    Vector<uint8_t> directions;

    void clear()
    {
      noInterrupts();
        times.empty();
        directions.empty();
      interrupts();
    }
    void add(uint8_t direction, uint32_t timer)
    {
      times.push_back(timer);
      directions.push_back(direction);
    }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// результат сравнения списка прерываний с эталоном
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
  msHandlePeakReason, // ветка обработки причины срабатывания "превышение по току"
  msHandleRelayReason, // ветка обработки причины срабатывания "срабатывание внешней защиты"
  msWaitForCollectEncoderPulses, // ветка обработки окончания сбора информации с энкодера
  msWaitGuardRelease, // ждём, пока концевик срабатывания защиты не переключится в разомкнутое состояние
  msWaitForNotCurrentPeaks, // ждём, когда прекратится превышение по току
  
} MachineState;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct InterruptEventSubscriber
{
  // вызывается, когда прерывания на нужном номере завершены, и накоплена статистика
  virtual void OnInterruptRaised(CurrentOscillData* oscData, EthalonCompareResult result) = 0;

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
   
   static void writeToLog(DirectionInfoData& directionData, uint16_t previewCount, int32_t dataArrivedTime, DS3231Time& tm, CurrentOscillData* oscData, InterruptTimeList& lst1, EthalonCompareResult res1, EthalonCompareNumber num1, const String& ethalonFileName, bool toEEPROM=false);


   static void normalizeList(InterruptTimeList& list);
   static void normalizeList(InterruptTimeList& list, uint32_t dirOffset);

private:

  bool hasAlarm;
  
   static uint32_t writeLogRecord(DirectionInfoData& directionData, uint16_t previewCount, int32_t dataArrivedTime, CurrentOscillData* oscData, InterruptTimeList& _list, EthalonCompareResult compareResult, EthalonCompareNumber num, const String& ethalonFileName, bool toEEPROM=false, uint32_t curEEPROMWriteAddress=0);


};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern InterruptHandlerClass InterruptHandler;
extern InterruptTimeList InterruptData;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


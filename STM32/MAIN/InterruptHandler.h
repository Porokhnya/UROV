#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "CONFIG.h"
#include "TinyVector.h"
#include "DS3231.h"
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
struct CurrentOscillData // данные по току, по трём каналам
{
	void clear() // очищает данные без освобождения памяти под них
	{
		times.empty();
		data1.empty();
		data2.empty();
		data3.empty();
	}

	void erase() // очищает данные вместе с памятью, выделенной под них
	{
		times.clear();
		data1.clear();
		data2.clear();
		data3.clear();
	}
	InterruptTimeList times; // время занесения записи, micros()

	// данные по АЦП
	InterruptTimeList data1;
	InterruptTimeList data2;
	InterruptTimeList data3;

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct InterruptEventSubscriber
{
  // вызывается, когда прерывания на нужном номере завершены, и накоплена статистика
  virtual void OnInterruptRaised(const CurrentOscillData& oscData, const InterruptTimeList& list, EthalonCompareResult result) = 0;

  // вызывается, когда есть хотя бы один список с прерываниями - закончен
  virtual void OnHaveInterruptData() = 0;

 // virtual void OnTimeBeforeInterruptsBegin(uint32_t tm, bool hasTime) = 0;
};
//--------------------------------------------------------------------------------------------------------------------------------------
class InterruptHandlerClass
{
 public:
  InterruptHandlerClass();

   void begin();
   void update();

   void setSubscriber(InterruptEventSubscriber* h);
   InterruptEventSubscriber* getSubscriber();
   void informSubscriber(CurrentOscillData& oscData, InterruptTimeList& list, EthalonCompareResult compareResult, uint32_t timeBeforeInterruptsBegin, uint32_t relayTriggeredTime);
   
   static void writeLogRecord(CurrentOscillData& oscData, InterruptTimeList& _list, EthalonCompareResult compareResult, EthalonCompareNumber num, InterruptTimeList& ethalonData);
   static void writeToLog(uint32_t dataArrivedTime, DS3231Time& tm, CurrentOscillData& oscData, InterruptTimeList& lst1, EthalonCompareResult res1, EthalonCompareNumber num1, InterruptTimeList& ethalonData1);
   static void writeRodPositionToLog(uint8_t channelNumber);

   // ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
   static void startCollectCurrentData();
   static void stopCollectCurrentData();
   static CurrentOscillData& getCurrentData();
   // ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

   static void normalizeList(InterruptTimeList& list);

private:

  bool hasAlarm;

};
//--------------------------------------------------------------------------------------------------------------------------------------
extern InterruptHandlerClass InterruptHandler;
//--------------------------------------------------------------------------------------------------------------------------------------


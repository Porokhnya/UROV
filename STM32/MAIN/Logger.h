#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "FileUtils.h"
#include "DS3231.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  recordInterruptInfoBegin,   // начало записи по прерыванию в логе

    recordInterruptTime,        // время прерывания
    recordSystemTemperature,    // температура системы

    recordInterruptRecordBegin,// начало записи для канала
    
      recordChannelNumber,       // запись номера канала
      recordRodPosition,         // позиция штанги для канала
      recordMoveTime,            // время движения штанги для канала
      recordMotoresource,        // моторесурс канала
      recordEthalonNumber,       // номер эталона, с которым сравнивали
      recordCompareResult,       // результат сравнения с эталоном

      recordInterruptDataBegin,  // начало данных прерывания
      recordInterruptDataEnd,    // конец данных прерывания

      recordEthalonDataFollow,  // следом идут данные эталона, с которым сравнивали

    recordInterruptRecordEnd, // конец записи для канада
  
  recordInterruptInfoEnd,    // конец записи по прерыванию

  recordRelayTriggeredTime, // время срабатывания защиты

  recordChannelInductiveSensorState, // состояние индуктивного датчика канала

  recordOscDataFollow, // идут данные по току для канала
  recordDataArrivedTime, // смещение от начала записей по току до начала данных по прерыванию, миллисекунд
  recordPreviewCount, // кол-во записей в превью по току
  recordRodMoveLength, // длина перемещения штанги, мм
    
} LogRecordTypes;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class LoggerClass
{
  public:

    LoggerClass();

    void write(uint8_t* data,size_t dataLength);
    void pause();
    void resume();

    String getCurrentLogFileName() { return logFileName; }

  private:

    DS3231Time lastWriteTime;
    SdFile workFile;
    bool bPaused;
    String logFileName;
    
    void doOpenFile(DS3231Time& tm);
    bool openWorkFile();
    void closeWorkFile();

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern LoggerClass Logger;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

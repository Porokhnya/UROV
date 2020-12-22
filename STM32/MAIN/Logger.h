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

    recordInterruptRecordEnd, // конец записи для канала
  
  recordInterruptInfoEnd,    // конец записи по прерыванию

  recordRelayTriggeredTime, // время срабатывания защиты

  recordChannelInductiveSensorState, // состояние индуктивного датчика канала

  recordOscDataFollow, // идут данные по току для канала
  recordDataArrivedTime, // смещение от начала записей по току до начала данных по прерыванию, миллисекунд
  recordPreviewCount, // кол-во записей в превью по току
  recordRodMoveLength, // длина перемещения штанги, мм
  recordDirectionData,  // информация об изменениях направления вращения штанги
    
} LogRecordTypes;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class LoggerClass // класс записи логов на SD
{
  public:

    LoggerClass(); // конструктор

    void write(uint8_t* data,size_t dataLength); // записывает данные в файл
    void pause(); // пауза
    void resume(); // возобновление рабты

    String getCurrentLogFileName() { return logFileName; } // возвращает имя текущего лог-файла

  private:

    DS3231Time lastWriteTime;
    SdFile workFile;
    bool bPaused;
    String logFileName;
    
    void doOpenFile(DS3231Time& tm); // открывает лог-файл, делает его имя соответственно с текущей датой
    bool openWorkFile(); // открывает лог-файл
    void closeWorkFile(); // закрывает лог-файл

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern LoggerClass Logger; // экземпляр класса
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

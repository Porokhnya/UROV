#pragma once

#include <Arduino.h>
#include "CONFIG.h"
#include "TinyVector.h"

typedef Vector<uint32_t> UInt32Vector;
typedef Vector<uint16_t> UInt16Vector;

/*
typedef enum
{
  checkCurrentBorder, // проверяем пороги
  waitForBorderAlert, // ждём, чтобы проверить порог ещё раз
  collectCurrentData, // собираем данные по току
  waitForTakeData, // ждём, когда данные заберут
  
} WatchCurrentState;
*/

struct CurrentOscillData // данные по току, по трём каналам
{

  private:

    int32_t firstRecordIndex; // индекс самой ранней записи
  
  public:

    // максимальное кол-во записей по току в списке
    static const uint32_t MAX_RECORDS = CURRENT_LIST_SIZE;//COUNT_OF_FULL_CURRENT_LIST + COUNT_OF_CURRENT_PREVIEW_RECORDS;


  void clear() // очищает данные без освобождения памяти под них
  {    
    firstRecordIndex = 0;
    
    times.clear();
    data1.clear();
    data2.clear();
    data3.clear();
        
  }

  void init()
  {    
   clear();
  }

  CurrentOscillData normalize();

  uint32_t earlierRecordTime()
  {
    if(times.size() > 0)
    {
      return times[firstRecordIndex];
    }
    return 0xFFFFFFFF;
   
  }

  void add(uint32_t tm, uint16_t channel1, uint16_t channel2, uint16_t channel3)
  {
    if(times.size() >= MAX_RECORDS)
    {
      // достигли конца списка, надо начинать сначала.
      // для этого увеличиваем указатель первой записи в списке, и пишем
      // на нужное место переданные данные.

      size_t writeIndex = times.size() - (times.size() - firstRecordIndex);

      // теперь пишем по нужному адресу
      times[writeIndex] = tm;
      data1[writeIndex] = channel1;
      data2[writeIndex] = channel2;
      data3[writeIndex] = channel3;
      
      // увеличиваем указатель самой ранней записи
      firstRecordIndex++;
      if(firstRecordIndex >= MAX_RECORDS)
      {
        firstRecordIndex = 0; 
      }


      return;
    }
    
    times.push_back(tm);
    data1.push_back(channel1);
    data2.push_back(channel2);
    data3.push_back(channel3);
    
  }
  
  UInt32Vector times; // время занесения записи, micros()

  // данные по АЦП
  UInt16Vector data1;
  UInt16Vector data2;
  UInt16Vector data3;

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class ADCSampler 
{
  public:
    ADCSampler();
    void begin();
    void end();
    void handleInterrupt();
    bool available();

    // установка порогов компаратора
    void setLowBorder(uint32_t val);
    void setHighBorder(uint32_t val);
    
    uint16_t* getADCBuffer(int *bufferLength); // возвращает заполненный буфер измерений АЦП

    void reset(); // сбрасывает признак готовности данных

    void pause();
    void resume();

    // разрешает или запрещает собирать данные по току
    void setCanCollectCurrentData(bool val)
    {
      canCollectCurrentData = val;
    }

    // возвращает список данных по осциллограмме тока, чистя локальный
    CurrentOscillData getListOfCurrent();

/*
	void startComputeRMS();
	void getComputedRMS(uint32_t& result1, uint32_t& result2, uint32_t& result3);
*/


  private:

/*
    volatile WatchCurrentState machineState; // состояние конечного автомата
    void savePreviewOfCurrent(uint16_t raw1, uint16_t raw2, uint16_t raw3);
    bool hasBorderAlert(uint16_t raw1, uint16_t raw2, uint16_t raw3);
*/  

    bool putAVG(uint16_t raw1, uint16_t raw2, uint16_t raw3);
    void getAVG(uint16_t& avg1, uint16_t& avg2, uint16_t& avg3);
    
    volatile bool dataReady;
    uint16_t adcBuffer[NUMBER_OF_BUFFERS][ADC_BUFFER_SIZE];

    uint32_t _compare_High = TRANSFORMER_HIGH_DEFAULT_BORDER;                                  // Верхний порог компаратора АЦП
    uint32_t _compare_Low = TRANSFORMER_LOW_DEFAULT_BORDER;                                   // Нижний порог компаратора АЦП

    volatile uint16_t tempADCBuffer[NUM_CHANNELS]; // буфер одного измерения АЦП
    uint16_t workingBufferIndex; // индекс буфера, который заполняем
    uint16_t filledBufferIndex; // индекс заполненного данными буфера
    uint16_t countOfSamples; // кол-во проделанных измерений


    bool canCollectCurrentData;
    CurrentOscillData oscillData;
    uint32_t currentOscillTimer;


//    bool compareTimerEnabled; // флаг, что мы в режиме таймера проверки превышения верхнего порога компаратора
//    uint32_t compareTimer; // таймер ожидания верхнего порога компаратора
};

extern ADCSampler adcSampler;

class ADCStopper
{
  public:
    ADCStopper();
    ~ADCStopper();
  
};

#define PAUSE_ADC ADCStopper _stopper

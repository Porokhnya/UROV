#pragma once

#include <Arduino.h>
#include "CONFIG.h"
#include "TinyVector.h"

typedef Vector<uint32_t> UInt32Vector;
typedef Vector<uint16_t> UInt16Vector;

typedef enum
{
  checkCurrentBorder, // проверяем пороги
  waitForBorderAlert, // ждём, чтобы проверить порог ещё раз
  collectCurrentData, // собираем данные по току
  waitForTakeData, // ждём, когда данные заберут
  
} WatchCurrentState;

struct CurrentOscillData // данные по току, по трём каналам
{
  public:

    static const uint32_t MAX_RECORDS = COUNT_OF_FULL_CURRENT_LIST + COUNT_OF_CURRENT_PREVIEW_RECORDS;


  void clear() // очищает данные без освобождения памяти под них
  {    
    
    times.clear();
    data1.clear();
    data2.clear();
    data3.clear();
        
  }

  void init()
  {    
   clear();
  }

  uint32_t earlierRecord()
  {
    if(times.size() > 0)
    {
      return times[0];
    }
    return 0xFFFFFFFF;
   
  }

  void add(uint32_t tm, uint16_t channel1, uint16_t channel2, uint16_t channel3)
  {
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

    CurrentOscillData getListOfCurrent()
    {
      CurrentOscillData result = oscillData;
      machineState = checkCurrentBorder;
      return result;
    }

/*
	void startComputeRMS();
	void getComputedRMS(uint32_t& result1, uint32_t& result2, uint32_t& result3);
*/


  private:

    volatile WatchCurrentState machineState; // состояние конечного автомата
    void savePreviewOfCurrent(uint16_t raw1, uint16_t raw2, uint16_t raw3);
    bool hasBorderAlert(uint16_t raw1, uint16_t raw2, uint16_t raw3);

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


    CurrentOscillData oscillData;
    uint32_t currentOscillTimer;


    bool compareTimerEnabled; // флаг, что мы в режиме таймера проверки превышения верхнего порога компаратора
    uint32_t compareTimer; // таймер ожидания верхнего порога компаратора
/*
	  volatile bool rmsComputeMode;
	  volatile uint32_t rmsData1, rmsData2, rmsData3;
	  volatile uint32_t rmsStartComputeTime;
*/    
};

extern ADCSampler adcSampler;

class ADCStopper
{
  public:
    ADCStopper();
    ~ADCStopper();
  
};

#define PAUSE_ADC ADCStopper _stopper

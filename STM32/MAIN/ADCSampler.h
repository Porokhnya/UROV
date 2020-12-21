#pragma once

#include <Arduino.h>
#include "CONFIG.h"
#include "TinyVector.h"

typedef Vector<uint32_t> UInt32Vector;
typedef Vector<uint16_t> UInt16Vector;


struct CurrentOscillData // данные по току
{
  CurrentOscillData()
  {
    
  }
  
  void clear() // очищает данные
  {        
    times.clear();
    data1.clear();
    data2.clear();
    data3.clear();
        
  }  


  void add(uint32_t tm, uint16_t channel1, uint16_t channel2, uint16_t channel3) // добавляет данные в списки
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

  CurrentOscillData& operator=(const CurrentOscillData& rhs) // оператор присваивания
  {
    if(this == &rhs)
    {
      return *this;
    }

      times = rhs.times;
      data1 = rhs.data1;
      data2 = rhs.data2;
      data3 = rhs.data3;

      return *this;
  } 

  uint32_t earlierRecordTime() // время самой ранней записи
  {
    if(times.size() > 0)
    {
      return times[0];
    }
    return 0xFFFFFFFF;
   
  }   
    
};


struct CurrentCircularBuffer // кольцевой буфер данных по току, по трём каналам
{

  private:

    uint32_t firstRecordIndex; // индекс самой ранней записи
    uint32_t writeIterator;
  
  public:

    CurrentCircularBuffer()
    {
      firstRecordIndex = 0;
      writeIterator = 0;
      recordsCount = 0; 
    }


  void clear() // очищает данные
  {    
    firstRecordIndex = 0;
    writeIterator = 0;
    recordsCount = 0;        
  }

  void init()
  {    
   clear();
  }

  CurrentCircularBuffer normalize();

  void add(uint32_t tm, uint16_t channel1, uint16_t channel2, uint16_t channel3) // добавляет данные в списки
  {
      // теперь пишем по нужному адресу
    if(recordsCount >= CURRENT_LIST_SIZE)
    {
      noInterrupts();
      // достигли конца списка, надо начинать сначала.
      // для этого увеличиваем указатель первой записи в списке, и пишем
      // на нужное место переданные данные.

      size_t writeIndex = recordsCount - (recordsCount - firstRecordIndex);

      // теперь пишем по нужному адресу
      times[writeIndex] = tm;
      data1[writeIndex] = channel1;
      data2[writeIndex] = channel2;
      data3[writeIndex] = channel3;
      
      // увеличиваем указатель самой ранней записи
      firstRecordIndex++;
      if(firstRecordIndex >= CURRENT_LIST_SIZE)
      {
        firstRecordIndex = 0; 
      }
      interrupts();

      return;
    }

    noInterrupts();
    times[writeIterator] = (tm);
    data1[writeIterator] = (channel1);
    data2[writeIterator] = (channel2);
    data3[writeIterator] = (channel3);

    writeIterator++;
    recordsCount++;

    if(recordsCount >=  CURRENT_LIST_SIZE)
    {
      recordsCount = CURRENT_LIST_SIZE;
    }

    if(writeIterator >= CURRENT_LIST_SIZE)
    {
      writeIterator = 0;
    } 
    interrupts();
   
    
  }

    volatile uint32_t recordsCount;
    volatile uint32_t times[CURRENT_LIST_SIZE];
    volatile uint16_t data1[CURRENT_LIST_SIZE];
    volatile uint16_t data2[CURRENT_LIST_SIZE];
    volatile uint16_t data3[CURRENT_LIST_SIZE];

  CurrentCircularBuffer& operator=(const CurrentCircularBuffer& rhs) // оператор присваивания
  {
    
    if(this == &rhs)
    {
      return *this;
    }

noInterrupts();
      memcpy((void*)times,(void*)rhs.times,sizeof(times));      
      memcpy((void*)data1,(void*)rhs.data1,sizeof(data1));      
      memcpy((void*)data2,(void*)rhs.data2,sizeof(data2));      
      memcpy((void*)data3,(void*)rhs.data3,sizeof(data3));      

      firstRecordIndex = rhs.firstRecordIndex;
      writeIterator = rhs.writeIterator;
      recordsCount = rhs.recordsCount;
interrupts();

      return *this;
  }

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ADCSampler // класс обработки данных с АЦП
{
  private:

    volatile bool _stopped; // флаг остановки
  
  public:
  
    ADCSampler(); // конструктор
    void begin(); // начинаем работу
    void end(); // заканчиваем работу
    void handleInterrupt(); // обрабатываем данные с АЦП
    bool available(); // признак готовности данных

    // установка порогов компаратора
    void setLowBorder(uint32_t val);
    uint32_t getLowBorder() { return _compare_Low; }
    
    void setHighBorder(uint32_t val);
    uint32_t getHighBorder() { return _compare_High; }
    
    uint16_t* getADCBuffer(int *bufferLength); // возвращает заполненный буфер измерений АЦП

    void reset(); // сбрасывает признак готовности данных

    void pause(bool withNoInterrupts=true); // пауза
    void resume(bool withNoInterrupts=true); // возобновление работы

    // разрешает или запрещает собирать данные превью по току
    void stopCollectPreview();
    void startCollectPreview();
    
    // возвращает список данных по осциллограмме тока, чистя локальный
    CurrentOscillData getListOfCurrent(uint16_t& previewCount, bool withNoInterrupts=true);


    // проверяет, собрана ли информация по среднему значению тока на каналах за определённый период времени
    bool currentPeakDataAvailable();

    // возвращает средние значения по току на каналах, собранные в течение определённого кол-ва времени
    void getCurrentPeakData(uint16_t& channel1, uint16_t& channel2, uint16_t& channel3);

    // разрешает или запрещает собирать данные по току в обычный, не кольцевой буфер
    void startCollectCurrent();
    void stopCollectCurrent();


  private:


    // помещение данных в списки усреднения
    bool putAVG(volatile uint16_t& samplesCounter, volatile uint16_t* arr1, volatile uint16_t* arr2, volatile uint16_t* arr3, uint16_t raw1, uint16_t raw2, uint16_t raw3);
    // получение среднего из списков усреднения
    void getAVG(volatile uint16_t* arr1, volatile uint16_t* arr2, volatile uint16_t* arr3, uint16_t& avg1, uint16_t& avg2, uint16_t& avg3);
    
    volatile bool dataReady; // флаг готовности данных
    uint16_t adcBuffer[NUMBER_OF_BUFFERS][ADC_BUFFER_SIZE]; // служебные буфера с данными АЦП

    volatile uint32_t _compare_High = TRANSFORMER_HIGH_DEFAULT_BORDER;                                  // Верхний порог компаратора АЦП
    volatile uint32_t _compare_Low = TRANSFORMER_LOW_DEFAULT_BORDER;                                   // Нижний порог компаратора АЦП

    volatile uint16_t tempADCBuffer[NUM_CHANNELS]; // буфер одного измерения АЦП
    volatile uint16_t workingBufferIndex; // индекс буфера, который заполняем
    volatile uint16_t filledBufferIndex; // индекс заполненного данными буфера
    volatile uint16_t countOfSamples; // кол-во проделанных измерений


    volatile bool canCollectCurrentPreviewData; // флаг сбора превью по току
    CurrentCircularBuffer currentPreviewData; // данные превью по току
    volatile uint32_t currentPreviewOscillTimer; // таймер превью по току


    // усреднённые данные по току на каналах
    volatile uint16_t currentPeakBuffers[2][3];
    volatile uint8_t currentPeakBufferIndex;
    volatile bool currentPeakDataReady;


    volatile bool canCollectCurrent; // флаг сбора информации по току
    volatile uint32_t currentTimer; // таймер по току
    UInt32Vector currentTimes; // список времён получения данных по току
    UInt16Vector currentChannel1; // данные по току, канал №1
    UInt16Vector currentChannel2; // данные по току, канал №2
    UInt16Vector currentChannel3; // данные по току, канал №3
    


};

extern ADCSampler adcSampler; // экземпляр класса работы с АЦП

class ADCStopper // класс-хэлпер для удобной приостановки/возобновления работы с АЦП
{
  public:
    ADCStopper();
    ~ADCStopper();
  
};

// определения для автоматической остановки АЦП в зоне видимости, и возобновление работы с АЦП при выходе из зоны видимости
#define PAUSE_ADC ADCStopper _stopper

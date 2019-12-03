#pragma once

#include <Arduino.h>
#include "Config.h"


class ADCSampler 
{
  public:
    ADCSampler();
    void begin();
    void end();
    void handleInterrupt();
    bool available();

    // установка порогов компаратора
    void setLowBorder(uint32_t val) {_compare_Low = val; } 
    void setHighBorder(uint32_t val) {_compare_High = val; } 
    
    uint16_t* getADCBuffer(int *bufferLength); // возвращает заполненный буфер измерений АЦП

    void reset(); // сбрасывает признак готовности данных

	void startComputeRMS();
	void getComputedRMS(uint32_t& result1, uint32_t& result2, uint32_t& result3);



  private:
  
    volatile bool dataReady;
    uint16_t adcBuffer[NUMBER_OF_BUFFERS][ADC_BUFFER_SIZE];


	  uint32_t _compare_High = TRANSFORMER_HIGH_DEFAULT_BORDER;                                  // Верхний порог компаратора АЦП
	  uint32_t _compare_Low = TRANSFORMER_LOW_DEFAULT_BORDER;                                   // Нижний порог компаратора АЦП


    volatile uint16_t tempADCBuffer[NUM_CHANNELS]; // буфер одного измерения АЦП
    uint16_t workingBufferIndex; // индекс буфера, который заполняем
    uint16_t filledBufferIndex; // индекс заполненного данными буфера
    uint16_t countOfSamples; // кол-во проделанных измерений

	  volatile bool rmsComputeMode;
	  volatile uint32_t rmsData1, rmsData2, rmsData3;
	  volatile uint32_t rmsStartComputeTime;
};

extern ADCSampler adcSampler;



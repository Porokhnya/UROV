#ifndef ADCSAMPLER_H
#define ADCSAMPLER_H

#include <Arduino.h>
#include "Config.h"

#define NUM_CHANNELS 9   // Установить количество аналоговых входов
/*
ch7:A0
ch6:A1
ch5:A2
ch4:A3  Вход токового трансформатора №3 (4)
ch3:A4  Вход токового трансформатора №2 (3)
ch2:A5  Вход токового трансформатора №1 (2)
ch1:A6  Вход индуктивного датчика №1 тест исправности датчика (1)
ch0:A7  Вход индуктивного датчика №2 тест исправности датчика (0)
ch10:A8 Вход индуктивного датчика №3 тест исправности датчика (5)
ch11:A9 Измерение =200В  (6)
ch12:A10 Измерение 3V3   (7)
ch13:A11 Измерение +5V   (8)
*/

#define ADC_CHANNELS ADC_CHER_CH0 | ADC_CHER_CH1 | ADC_CHER_CH2 | ADC_CHER_CH3 | ADC_CHER_CH4 | ADC_CHER_CH10 | ADC_CHDR_CH11 | ADC_CHDR_CH12 | ADC_CHDR_CH13  //
#define ADC_CHANNELS_DIS  ADC_CHDR_CH5 | ADC_CHDR_CH6 | ADC_CHDR_CH7  // Отключить не используемые входа
#define BUFFER_SIZE 200*NUM_CHANNELS                                   // Определить размер буфера хранения измеряемого сигнала     
#define NUMBER_OF_BUFFERS 6                                            // Установить количество буферов
//#define VOLT_REF        (2400)                                       // Величина опорного напряжения
/* The maximal digital value */
//#define ADC_RESOLUTION		12                                // Разрядность АЦП (максимальная для DUE)

//***************** Новые настройки АЦП *******************************



//*********************************************************************






class ADCSampler {
  public:
    ADCSampler();
    void begin(unsigned int samplingRate);
    void end();
    void handleInterrupt();
    bool available();

    void setLowBorder(uint32_t val) {_compare_Low = val; } 
    void setHighBorder(uint32_t val) {_compare_High = val; } 
    
	bool available_compare();
    unsigned int getSamplingRate();
    uint16_t* getFilledBuffer(int *bufferLength);
    void readBufferDone();
	volatile bool dataHigh;                                    // Признак превышения порога компаратора АЦП
//	unsigned int samplingRate = 3000;                          // Частота вызова (стробирования) АЦП 50мс
  private:
    unsigned int samplingRate;
    volatile bool dataReady;
    uint16_t adcBuffer[NUMBER_OF_BUFFERS][BUFFER_SIZE];
    unsigned int adcDMAIndex;                                  //!< This hold the index of the next DMA buffer
    unsigned int adcTransferIndex;                             //!< This hold the last filled buffer
	uint32_t _compare_High = TRANSFORMER_HIGH_DEFAULT_BORDER;                                  // Верхний порог компаратора АЦП
	uint32_t _compare_Low = TRANSFORMER_LOW_DEFAULT_BORDER;                                   // Нижний порог компаратора АЦП

};

extern ADCSampler adcSampler;


#endif /* ADCSAMPLER_H */


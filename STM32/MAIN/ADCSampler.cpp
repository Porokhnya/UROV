
#include "ADCSampler.h"
#include "CONFIG.h"
#include "Feedback.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ADCStopper::ADCStopper()
{
  adcSampler.pause(); // останавливаем АЦП
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ADCStopper::~ADCStopper()
{
  adcSampler.resume(); // запускаем АЦП
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ADCSampler adcSampler;
static ADC_HandleTypeDef hadc1;
static DMA_HandleTypeDef hdma_adc1;
static TIM_HandleTypeDef htim3 = {0};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MX_DMA_Init(void)  // Enable DMA controller clock
{
DBGLN("MX_DMA_Init START.");

  // DMA controller clock enable 
  __HAL_RCC_DMA2_CLK_ENABLE();

  // DMA interrupt init 
  // DMA2_Stream0_IRQn interrupt configuration 
  HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);  

DBGLN("MX_DMA_Init END.");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MX_ADC1_Init(void) // Init ADC1
{

DBGLN("MX_ADC1_Init START.");

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();  
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      return;
    }
    

  /* USER CODE BEGIN ADC1_Init 0 */


  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;                     // Запретить непрерывное измерение. Запускаем измерение по таймеру Т3
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;  // В качестве внешнего события мы можем выбрать таймер (Т3)
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  // Смещение данных в право  
  hadc1.Init.NbrOfConversion = 4;                              // Количество измеряемых каналов.
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    return;
  }

  
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  /*
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
   return;
  }
  */
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_4;        // Подключить аналоговый вход PA4 (трансформатор №1)
  sConfig.Rank = 1;                       // Номер очереди измерения каналов 
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES; // Время измерения (заряда измерительного конденсатора)
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return;
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_5;    // Подключить аналоговый вход PA5 (трансформатор №2)
  sConfig.Rank = 2;                   // Номер очереди измерения каналов 
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return;
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_6;  // Подключить аналоговый вход PA6 (трансформатор №3)
  sConfig.Rank = 3;                 // Номер очереди измерения каналов 
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return;
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_7;   // Подключить аналоговый вход PA7 (измерение 3,3 вольта деленное на 2)
  sConfig.Rank = 4;                  // Номер очереди измерения каналов 
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return;
  }

    __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);
 
 
DBGLN("MX_ADC1_Init END.");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MX_TIM3_Init(void)
{
  DBGLN("MX_TIM3_Init START.");
  
    /* TIM3 interrupt Init */
  HAL_NVIC_SetPriority( TIM3_IRQn, 0, 0 );
  HAL_NVIC_EnableIRQ  ( TIM3_IRQn );


  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;                            // Применяем таймер №3
  htim3.Init.Prescaler = 0;                         // Первый делитель. Тактовая частота 84 мГц (168000000/2)
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;      // Счет вверх
  htim3.Init.Period = SAMPLING_RATE;                // Второй делитель. Период следования прерываний 260мкс. 
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;// 
  
  //TODO: РУГАЕТСЯ НА ЭТУ СТРОЧКУ !!!
  //htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  // Работа с теневым регистром, в этой версии библиотеки HAL не применяется.

   /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

  
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    return;
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;    // Внутренний источник тактирования
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    return;
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;  //
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    return;
  }
  

 DBGLN("MX_TIM3_Init END.");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern "C"  void TIM3_IRQHandler(void) // обработчик тика таймера
{
  
  HAL_TIM_IRQHandler(&htim3);

  // вызываем обработчик прерывания, для накопления сэмплов АЦП
	adcSampler.handleInterrupt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _CURRENT_COLLECT_OFF
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile uint16_t avgPreviewSamplesDone = 0; // кол-во собранных семплов для усреднения
// списки для усреднения
volatile uint16_t avgPreviewChannel1[CURRENT_AVG_SAMPLES] = {0};
volatile uint16_t avgPreviewChannel2[CURRENT_AVG_SAMPLES] = {0};
volatile uint16_t avgPreviewChannel3[CURRENT_AVG_SAMPLES] = {0};

volatile uint16_t avgCurrentSamplesDone = 0; // кол-во собранных семплов для усреднения
// списки для усреднения
volatile uint16_t avgCurrentChannel1[CURRENT_AVG_SAMPLES] = {0};
volatile uint16_t avgCurrentChannel2[CURRENT_AVG_SAMPLES] = {0};
volatile uint16_t avgCurrentChannel3[CURRENT_AVG_SAMPLES] = {0};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // _CURRENT_COLLECT_OFF
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ADCSampler::ADCSampler()
{
  dataReady = false;
  filledBufferIndex = 0;
  workingBufferIndex = 0;
  countOfSamples = 0;
  currentPreviewOscillTimer = 0;
  canCollectCurrentPreviewData = true;
  _stopped = false;

  canCollectCurrentPeak = false;
  currentPeakDataReady = false;
  currentPeakTimer = 0;
  currentPeakTimerPeriod = 0;
  currentPeakNumSamples = 0;

  canCollectCurrent = false;
  currentTimer = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::startCollectCurrent()
{
  noInterrupts();
  
  currentTimes.clear();
  currentChannel1.clear();
  currentChannel2.clear();
  currentChannel3.clear();

  // копируем часть уже накопленных сэмплов из списков превью по току в список постоянного накопления по току      
  currentTimer = currentPreviewOscillTimer;//micros();
  memcpy((void*)avgCurrentChannel1,(void*)avgPreviewChannel1,sizeof(avgCurrentChannel1));
  memcpy((void*)avgCurrentChannel2,(void*)avgPreviewChannel2,sizeof(avgCurrentChannel2));
  memcpy((void*)avgCurrentChannel3,(void*)avgPreviewChannel3,sizeof(avgCurrentChannel3));

  #ifndef _CURRENT_COLLECT_OFF
    avgCurrentSamplesDone = avgPreviewSamplesDone;
  #endif

  canCollectCurrent = true;
  
  interrupts();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::stopCollectCurrent()
{
  noInterrupts();
    canCollectCurrent = false;
  interrupts();  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::startDetectCurrentPeak(uint32_t numSamples,uint32_t timerPeriod)
{
  PAUSE_ADC; // останавливаем АЦП на время
  currentPeakDataReady = false;
  currentPeakTimerPeriod = timerPeriod;
  currentPeakNumSamples = numSamples;

  // очищаем списки
  currentPeakChannel1.clear();
  currentPeakChannel2.clear();
  currentPeakChannel3.clear();

  // запускаем сбор информации
  currentPeakTimer = 0;//micros();
  canCollectCurrentPeak = true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ADCSampler::currentPeakDataAvailable()
{
  return currentPeakDataReady;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::getCurrentPeakData(uint16_t& avg1, uint16_t& avg2, uint16_t& avg3)
{
  avg1 = avg2 = avg3 = 0;
  
  PAUSE_ADC; // останавливаем АЦП на время
  
  if(!currentPeakDataReady)
  {
    return;
  }

  // вычисляем средние значения
 uint32_t chMin1 = 0xFFFFFFFF;
  uint32_t chMin2 = 0xFFFFFFFF;
  uint32_t chMin3 = 0xFFFFFFFF;

  uint32_t chMax1 = 0;
  uint32_t chMax2 = 0;
  uint32_t chMax3 = 0;
  
  for(uint16_t i=0;i<currentPeakNumSamples;i++)
  {
    chMin1 = min(chMin1,currentPeakChannel1[i]);
    chMin2 = min(chMin2,currentPeakChannel2[i]);
    chMin3 = min(chMin3,currentPeakChannel3[i]);

    chMax1 = max(chMax1,currentPeakChannel1[i]);
    chMax2 = max(chMax2,currentPeakChannel2[i]);
    chMax3 = max(chMax3,currentPeakChannel3[i]);
  }

  if(chMin1 == 0xFFFFFFFF)
  {
    chMin1 = chMax1;
  }

  if(chMin2 == 0xFFFFFFFF)
  {
    chMin2 = chMax2;
  }

  if(chMin3 == 0xFFFFFFFF)
  {
    chMin3 = chMax3;
  }

  avg1 = chMax1/currentPeakNumSamples - chMin1/currentPeakNumSamples;
  avg2 = chMax2/currentPeakNumSamples - chMin2/currentPeakNumSamples;
  avg3 = chMax3/currentPeakNumSamples - chMin3/currentPeakNumSamples;

  // сбрасываем флаг готовности данных, потому что его уже получили - и обработали
  currentPeakDataReady = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::setLowBorder(uint32_t val) 
{
  _compare_Low = val; 
} 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::setHighBorder(uint32_t val) 
{
  _compare_High = val; 
} 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::begin()
{  
DBGLN("ADCSampler::begin START.");  

  _stopped = false;
  dataReady = false;

  currentPreviewData.init();
  currentPreviewOscillTimer = 0;//micros();
  canCollectCurrentPreviewData = true;
  
  #ifndef _CURRENT_COLLECT_OFF
    avgPreviewSamplesDone = 0;
    avgCurrentSamplesDone = 0;
  #endif

  filledBufferIndex = 0;
  workingBufferIndex = 0;
  
  countOfSamples = 0;  
  memset(adcBuffer,0,sizeof(adcBuffer));
  
 MX_DMA_Init();
 MX_ADC1_Init();
 MX_TIM3_Init();  


 //HAL_ADCEx_Calibration_Start(&hadc1); 
  
  // говорим АЦП собирать данные по каналам в наш буфер
 HAL_ADC_Start_DMA(&hadc1,(uint32_t*) &tempADCBuffer,NUM_CHANNELS);
  
  // запускаем таймер
  HAL_TIM_Base_Start_IT ( &htim3 ); 
 

 
DBGLN("ADCSampler::begin END.");   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::end()
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ADCSampler::putAVG(volatile uint16_t& samplesCounter, volatile uint16_t* arr1, volatile uint16_t* arr2, volatile uint16_t* arr3,  uint16_t raw1, uint16_t raw2, uint16_t raw3)
{
#ifndef _CURRENT_COLLECT_OFF
  
      arr1[samplesCounter] = raw1;
      arr2[samplesCounter] = raw2;
      arr3[samplesCounter] = raw3;

      samplesCounter++;
      
      if(samplesCounter >= CURRENT_AVG_SAMPLES)
      {
          samplesCounter = 0;
         // HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);
          return true;
      }
#endif // #ifndef _CURRENT_COLLECT_OFF      

 return false;     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::getAVG(volatile uint16_t* arr1, volatile uint16_t* arr2, volatile uint16_t* arr3, uint16_t& avg1, uint16_t& avg2, uint16_t& avg3)
{
  avg1 = avg2 = avg3 = 0;
  
#ifndef _CURRENT_COLLECT_OFF  

  uint32_t chMin1 = 0xFFFFFFFF;
  uint32_t chMin2 = 0xFFFFFFFF;
  uint32_t chMin3 = 0xFFFFFFFF;

  uint32_t chMax1 = 0;
  uint32_t chMax2 = 0;
  uint32_t chMax3 = 0;
  
  for(uint16_t i=0;i<CURRENT_AVG_SAMPLES;i++)
  {
    chMin1 = min(chMin1,arr1[i]);
    chMin2 = min(chMin2,arr2[i]);
    chMin3 = min(chMin3,arr3[i]);

    chMax1 = max(chMax1,arr1[i]);
    chMax2 = max(chMax2,arr2[i]);
    chMax3 = max(chMax3,arr3[i]);
  }

  if(chMin1 == 0xFFFFFFFF)
  {
    chMin1 = chMax1;
  }

  if(chMin2 == 0xFFFFFFFF)
  {
    chMin2 = chMax2;
  }

  if(chMin3 == 0xFFFFFFFF)
  {
    chMin3 = chMax3;
  }

  avg1 = chMax1/CURRENT_AVG_SAMPLES - chMin1/CURRENT_AVG_SAMPLES;
  avg2 = chMax2/CURRENT_AVG_SAMPLES - chMin2/CURRENT_AVG_SAMPLES;
  avg3 = chMax3/CURRENT_AVG_SAMPLES - chMin3/CURRENT_AVG_SAMPLES;

  #endif // #ifndef _CURRENT_COLLECT_OFF
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::stopCollectPreview()
{
  noInterrupts();
      canCollectCurrentPreviewData = false;  
  interrupts();     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::startCollectPreview()
{
  noInterrupts();
      currentPreviewData.clear();
      #ifndef _CURRENT_COLLECT_OFF
      avgPreviewSamplesDone = 0;
      #endif
      currentPreviewOscillTimer = 0;  
      canCollectCurrentPreviewData = true;
  interrupts();     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CurrentOscillData ADCSampler::getListOfCurrent(uint16_t& previewCount,bool withNoInterrupts)
{
    pause(withNoInterrupts);

  // возвращаем нормализованный список, упорядоченный по времени
  CurrentOscillData result;
  
  CurrentCircularBuffer normList = currentPreviewData.normalize();
  previewCount = normList.recordsCount;

//  Serial.print("ADC SOURCE SIZE: "); Serial.println(currentPreviewData./*times.size()*/recordsCount);
//  Serial.print("ADC PREVIEW SIZE: "); Serial.println(normList./*times.size()*/recordsCount);
  
  // очищаем локальный список осциллограмм тока
  currentPreviewData.clear();

  // помещаем данные по превью тока в список результатов
  for(size_t i=0;i<normList.recordsCount/*.times.size()*/;i++)
  {
    result.add(normList.times[i], normList.data1[i], normList.data2[i], normList.data3[i]);
//    Serial.print("ADC PREVIEW: "); Serial.println(normList.times[i]);
  }

  // помещаем данные по списку тока в список результатов
  for(size_t i=0;i<currentTimes.size();i++)
  {
    result.add(currentTimes[i], currentChannel1[i], currentChannel2[i], currentChannel3[i]);
  }


  // очищаем локальные списки по току
  currentTimes.clear();
  currentChannel1.clear();
  currentChannel2.clear();
  currentChannel3.clear();
  
  #ifndef _CURRENT_COLLECT_OFF
  avgPreviewSamplesDone = 0;
  avgCurrentSamplesDone = 0;
  #endif
  
  resume(withNoInterrupts);
  
  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CurrentCircularBuffer CurrentCircularBuffer::normalize()
{
  // вот тут надо скопировать буфер так, чтобы учитывать индекс первой записи
  CurrentCircularBuffer result;

  if(/*times.size()*/recordsCount < CURRENT_LIST_SIZE)
  {
    result = *this;
  }
  else
  {
    // кол-во записей уже достигло максимального, надо учитывать индекс первой записи, и от него идти
    size_t readIndex = firstRecordIndex;
    size_t writeIndex = 0;

    for(size_t i=0;i</*times.size()*/recordsCount;i++)
    {
      /*
        result.times.push_back(times[readIndex]);
        result.data1.push_back(data1[readIndex]);
        result.data2.push_back(data2[readIndex]);
        result.data3.push_back(data3[readIndex]);
      */
      
        result.times[writeIndex] = (times[readIndex]);
        result.data1[writeIndex] = (data1[readIndex]);
        result.data2[writeIndex] = (data2[readIndex]);
        result.data3[writeIndex] = (data3[readIndex]);
        writeIndex++;        

        readIndex++;
        if(readIndex >= recordsCount)//times.size())
        {
          readIndex = 0;
        }
    } // for

      result.firstRecordIndex = 0;
      result.writeIterator = 0;
      result.recordsCount = recordsCount;
  }


  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::handleInterrupt()
{
  // код обработки данных, поступающих с АЦП
  
  if(_stopped) // остановлены
  {
    return;
  }


    // заполняем буфер данными одного измерения
    uint16_t writeIndex = countOfSamples*NUM_CHANNELS;
    
    for(int i=0;i<NUM_CHANNELS;i++)
    {
        adcBuffer[workingBufferIndex][writeIndex++] = tempADCBuffer[i];
    } // for

      // данные по осциллограмме тока

      // считаем показания по трём каналам        
        uint32_t raw1 = 0;
        uint32_t raw2 = 0;
        uint32_t raw3 = 0;

        float currentCoeff = Settings.getCurrentCoeff();
        currentCoeff /= 1000; // у нас в тысячных долях

        raw1 = (COEFF_1*(tempADCBuffer[0]))/currentCoeff;
        raw2 = (COEFF_1*(tempADCBuffer[1]))/currentCoeff;
        raw3 = (COEFF_1*(tempADCBuffer[2]))/currentCoeff;


        // проверяем - надо ли собирать информацию по току за определённый интервал?
        if(canCollectCurrentPeak)
        {
            if(/*micros() - */++currentPeakTimer >= currentPeakTimerPeriod)
            {
              currentPeakChannel1.push_back(raw1);
              currentPeakChannel2.push_back(raw2);
              currentPeakChannel3.push_back(raw3);

              if(currentPeakChannel1.size() >= currentPeakNumSamples)
              {
                // закончили сбор информации
                canCollectCurrentPeak = false;

                // выставили флаг, что данные доступны
                currentPeakDataReady = true;
                
              }

              currentPeakTimer = 0;//micros();
            }
          
        } // if(canCollectCurrentPeak)

        // тут собираем данные по осциллограмме тока
        #ifndef _CURRENT_COLLECT_OFF

        // проверяем, можем ли мы помещать данные по току в обычный, не кольцевой буфер?
        if(canCollectCurrent)
        {
         if(/*micros() - */++currentTimer >= CURRENT_TIMER_PERIOD)
          {            
            if(putAVG(avgCurrentSamplesDone, avgCurrentChannel1, avgCurrentChannel2, avgCurrentChannel3, raw1,raw2,raw3))
            {
              uint16_t avg1,avg2,avg3;
              getAVG(avgCurrentChannel1, avgCurrentChannel2, avgCurrentChannel3, avg1,avg2,avg3);
              
              currentTimes.push_back(micros());
              currentChannel1.push_back(avg1);
              currentChannel2.push_back(avg2);
              currentChannel3.push_back(avg3);
            }
              currentTimer = 0;//micros();
          } 
        } // if(canCollectCurrent)


        // можем ли мы собирать превью в кольцевой буфер?
        if(canCollectCurrentPreviewData)
        {
          if(/*micros() - */++currentPreviewOscillTimer >= CURRENT_TIMER_PERIOD)
          {
            if(putAVG(avgPreviewSamplesDone, avgPreviewChannel1, avgPreviewChannel2, avgPreviewChannel3, raw1,raw2,raw3))
            {
              uint16_t avg1,avg2,avg3;
              getAVG(avgPreviewChannel1, avgPreviewChannel2, avgPreviewChannel3, avg1,avg2,avg3);
              
              currentPreviewData.add(micros(),avg1,avg2,avg3);
            }
              currentPreviewOscillTimer = 0;//micros();
          }
        } // canCollectCurrentPreviewData
        
        #endif // _CURRENT_COLLECT_OFF

    countOfSamples++;

    if(countOfSamples >= ADC_BUFFER_SIZE/NUM_CHANNELS)
    {
      // буфер заполнили      
      countOfSamples = 0; // обнуляем кол-во сэмплов
      
      filledBufferIndex = workingBufferIndex; // запоминаем, какой буфер мы заполнили
      
      workingBufferIndex++; // перемещаемся на заполнение следующего буфера
      
      if(workingBufferIndex >= NUMBER_OF_BUFFERS) // если закончили заполнять все буфера - перемещаемся на старт
      {
        workingBufferIndex = 0;
      }


      dataReady = true; // Данные сформированы
      
      
    } // if(countOfSamples >= ADC_BUFFER_SIZE/NUM_CHANNELS)

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::pause(bool withNoInterrupts)
{
  if(withNoInterrupts)
  {
    noInterrupts();  
  }
  bool thisStopped = _stopped;
  
  if(withNoInterrupts)
  {
    interrupts();
  }
  
  if(thisStopped)
  {
    return;
  }

  if(withNoInterrupts)
  {
    noInterrupts();
  }
  _stopped = true;

  if(withNoInterrupts)
  {
    interrupts();
  }
  
  // останавливаем таймер
  HAL_NVIC_DisableIRQ  ( TIM3_IRQn );
  HAL_NVIC_ClearPendingIRQ(TIM3_IRQn);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::resume(bool withNoInterrupts)
{
 if(withNoInterrupts)
  {
    noInterrupts();  
  }
  bool thisStopped = _stopped;
  
  if(withNoInterrupts)
  {
    interrupts();
  }

  if(!thisStopped)
  {
    return;
  }

  if(withNoInterrupts)
  {
    noInterrupts();
  }
  _stopped = false;

  if(withNoInterrupts)
  {
    interrupts();
  }

  // запускаем таймер
  HAL_NVIC_EnableIRQ  ( TIM3_IRQn );
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ADCSampler::available()
{
  return dataReady;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t* ADCSampler::getADCBuffer(int *bufferLength)
{
  *bufferLength = ADC_BUFFER_SIZE;
  return adcBuffer[filledBufferIndex];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::reset()
{
  dataReady = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

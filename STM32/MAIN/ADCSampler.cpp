
#include "ADCSampler.h"
#include "CONFIG.h"
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
ADCSampler::ADCSampler()
{
	rmsComputeMode = false;
	rmsStartComputeTime = 0;
  dataReady = false;
  filledBufferIndex = 0;
  workingBufferIndex = 0;
  countOfSamples = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::begin()
{  
DBGLN("ADCSampler::begin START.");  

  dataReady = false;

  filledBufferIndex = 0;
  workingBufferIndex = 0;
  
  countOfSamples = 0;  
  memset(adcBuffer,0,sizeof(adcBuffer));
  
 //TODO: КОД АЦП ДЛЯ STM32 !!!

 MX_DMA_Init();
 MX_ADC1_Init();
 MX_TIM3_Init();
 
 // говорим АЦП собирать данные по каналам в наш буфер
 HAL_ADC_Start_DMA(&hadc1,(uint32_t*) &tempADCBuffer,NUM_CHANNELS);


 // запускаем таймер
// HAL_TIM_Base_Start(&htim3);
  HAL_TIM_Base_Start_IT ( &htim3 ); 
 
DBGLN("ADCSampler::begin END.");   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::startComputeRMS()
{
	rmsData1 = 0;
	rmsData2 = 0;
	rmsData3 = 0;
	rmsStartComputeTime = millis();
	rmsComputeMode = true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::getComputedRMS(uint32_t& result1, uint32_t& result2, uint32_t& result3)
{
	rmsComputeMode = false;
	result1 = rmsData1;
	result1 = rmsData2;
	result1 = rmsData3;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::end()
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ADCSampler::handleInterrupt()
{
  //TODO: Код АЦП для STM32!!!


  /*
    Буфер у нас для четырёх каналов, индексы:

    0 - Аналоговый вход трансформатора №1
    1 - Аналоговый вход трансформатора №2
    2 - Аналоговый вход трансформатора №3
    3 - Аналоговый вход контроль питания 3.3в

     тут мы должны заполнять текущий буфер данными. У нас один буфер имеет размерность ADC_BUFFER_SIZE, которая
     определяется как 200*NUM_CHANNELS, где NUM_CHANNELS = 4.

     т.е. для каждого канала у нас - ADC_BUFFER_SIZE/NUM_CHANNELS измерений.
     измерения располагаются последовательно по каналам.

   */

    // заполняем буфер данными одного измерения
    uint16_t writeIndex = countOfSamples*NUM_CHANNELS;
    
    for(int i=0;i<NUM_CHANNELS;i++)
    {
        adcBuffer[workingBufferIndex][writeIndex++] = tempADCBuffer[i];
    } // for

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


      if (rmsComputeMode)
        {
          // нас попросили считать РМС, считаем
      
          int bufferLength = 0;
          uint16_t* cBuf = adcSampler.getADCBuffer(&bufferLength);
          uint16_t countOfPoints = bufferLength / NUM_CHANNELS;
      
          uint32_t serie1=0, serie2=0, serie3=0;
      
          for (int i = 0; i < bufferLength; i = i + NUM_CHANNELS) // получить результат измерения поканально, с интервалом 3
          {
            serie1 += cBuf[i + 0];        // Данные 1 графика  (красный)
            serie2 += cBuf[i + 1];        // Данные 2 графика  (синий)
            serie3 += cBuf[i + 2];        // Данные 3 графика  (желтый)
          }
      
          // получаем средние значения за серию
          serie1 /= countOfPoints;
          serie2 /= countOfPoints;
          serie3 /= countOfPoints;
      
          // складываем полученные средние с ранее высчитанными, и делим на 2, поскольку в высчитанном значении у нас тоже хранится среднее
          if (rmsData1)
          {
            rmsData1 += serie1;
            rmsData1 /= 2;
          }
          else
            rmsData1 = serie1;
      
          if (rmsData2)
          {
            rmsData2 += serie2;
            rmsData2 /= 2;
          }
          else
            rmsData2 = serie2;
      
          if (rmsData3)
          {
            rmsData3 += serie3;
            rmsData3 /= 2;
          }
          else
            rmsData3 = serie3;
      
          if (millis() - rmsStartComputeTime > RMS_COMPUTE_TIME)
          {
            // время подсчёта вышло, больше не считаем
            rmsComputeMode = false;
            rmsStartComputeTime = 0;
          }
      
        } // if(rmsComputeMode)


      dataReady = true; // Данные сформированы
      
      
    } // if(countOfSamples >= ADC_BUFFER_SIZE/NUM_CHANNELS)

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




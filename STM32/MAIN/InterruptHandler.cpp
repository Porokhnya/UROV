#include "InterruptHandler.h"
#include "InterruptScreen.h"
#include "Endstops.h"
#include "Feedback.h"
#include "FileUtils.h"
#include "Logger.h"
#include "Settings.h"
#include "DelayedEvents.h"
#include "ADCSampler.h"
//--------------------------------------------------------------------------------------------------------------------------------------
InterruptHandlerClass InterruptHandler;
//--------------------------------------------------------------------------------------------------------------------------------------
// список времён срабатываний прерываний на энкодере штанги
InterruptTimeList list1;
//--------------------------------------------------------------------------------------------------------------------------------------
volatile bool hasEncoderInterrupt = false;
volatile uint32_t lastEncoderInterruptTime = 0;

volatile uint32_t relayTriggeredTime = 0; // время, когда защита сработала
volatile bool hasRelayTriggered = false;
DS3231Time trigTime; // время срабатывания защиты

volatile uint32_t timeBeforeInterruptsBegin = 0; // время от срабатывания реле защиты до первого прерывания
volatile bool hasRelayTriggeredTime = false; // флаг, что было срабатывание реле защиты перед пачкой прерываний

// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
volatile uint32_t currentOscillTimer = 0; // таймер для сбора информации по току
volatile bool currentOscillTimerActive = false; // флаг активности таймера сбора информации по току
CurrentOscillData oscillData; // информация по току
// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

#ifndef _RMS_OFF
volatile bool wantComputeRMS = false; // флаг, что мы должны подсчитать РМС
volatile bool inComputeRMSMode = false; // флаг, что мы считаем РМС
volatile uint32_t rmsStartComputeTime = 0; // начало времени подсчёта РМС
volatile bool computeRMSCalled = false; // флаг, что мы попросили АЦП подсчитать РМС
#endif // _RMS_OFF

volatile bool downEndstopTriggered = false; // состояние нижнего концевика на момент срабатывания защиты
//--------------------------------------------------------------------------------------------------------------------------------------
InterruptEventSubscriber* subscriber = NULL;
//--------------------------------------------------------------------------------------------------------------------------------------
void EncoderPulsesHandler() // обработчик импульсов энкодера
{
    uint32_t now = micros();
    list1.push_back(now);

	hasEncoderInterrupt = true;
	lastEncoderInterruptTime = now;

  if(list1.size() < 2)
  {
    timeBeforeInterruptsBegin = (now - relayTriggeredTime);
  }

#ifndef DISABLE_CATCH_ENCODER_DIRECTION
		// определяем направление вращения энкодера.
	  if (digitalRead(ENCODER_PIN2))
	  {
		  // по часовой
		  Settings.setRodDirection(rpUp);
	  }
	  else
	  {
		  // против часовой
		  Settings.setRodDirection(rpDown);
	  }
#endif
    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void computeRMS()
{
#ifndef _RMS_OFF
	if (computeRMSCalled)
		return;

	computeRMSCalled = true;
	// считаем РМС
	adcSampler.startComputeRMS();
#endif // _RMS_OFF
}
//--------------------------------------------------------------------------------------------------------------------------------------
void checkRMS()
{
#ifndef _RMS_OFF

	// получаем подсчитанное РМС
	uint32_t rmsComputed1, rmsComputed2, rmsComputed3;
	adcSampler.getComputedRMS(rmsComputed1, rmsComputed2, rmsComputed3);

	computeRMSCalled = false;

	// получаем текущее состояние нижнего концевика, оно должно измениться.
	bool thiDownEndstopTriggered = RodDownEndstopTriggered(true);

	//тут проверяем РМС

	uint32_t rmsEthalonVal = RMS_ETHALON_VAL; // 100% значение РМС
	float hist = ((1.0f*RMS_ETHALON_VAL) / 100)*RMS_HISTERESIS_PERCENTS;
	uint32_t rmsHisteresis = hist; // гистерезис РМС

	bool hasAlarm = abs(rmsComputed1 - rmsEthalonVal) >= rmsHisteresis ||
		abs(rmsComputed2 - rmsEthalonVal) >= rmsHisteresis ||
		abs(rmsComputed3 - rmsEthalonVal) >= rmsHisteresis;


	if (hasAlarm)
	{
		// если нижний концевик не изменил положения - это авария!
		bool hasEndstopAlarm = (!thiDownEndstopTriggered && thiDownEndstopTriggered == downEndstopTriggered) ||
			(downEndstopTriggered && thiDownEndstopTriggered == downEndstopTriggered);

		if (hasEndstopAlarm)
		{
			// авария
			Feedback.alarm(true);
		}
	}

#endif // _RMS_OFF
}
//--------------------------------------------------------------------------------------------------------------------------------------
volatile bool relayTriggeredAtStart = true;
//--------------------------------------------------------------------------------------------------------------------------------------
void RelayTriggered()
{
	if (relayTriggeredAtStart) // убираем первое срабатывание при старте
	{
		relayTriggeredAtStart = false;
		return;
	}
  // запоминаем время срабатывания защиты
  relayTriggeredTime = micros();
  trigTime = RealtimeClock.getTime();
  hasRelayTriggered = true;
  hasRelayTriggeredTime = true;

#ifndef _RMS_OFF
  wantComputeRMS = true; // говорим, что надо посчитать РМС
#endif // _RMS_OFF

  timeBeforeInterruptsBegin = 0; // сбрасываем время до начала импульсов

  // сохраняем состояние нижнего концевика 
  downEndstopTriggered = RodDownEndstopTriggered(false);

}
//--------------------------------------------------------------------------------------------------------------------------------------
InterruptHandlerClass::InterruptHandlerClass()
{
  subscriber = NULL;
  hasAlarm = false;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::begin()
{
  // резервируем память
  list1.reserve(INTERRUPT_RESERVE_RECORDS);

#if (RELAY_INTERRUPT_LEVEL == RISING)
  pinMode(RELAY_PIN, INPUT_PULLUP);
#else
  pinMode(RELAY_PIN, INPUT);
#endif


  // настраиваем первый выход энкодера на чтение
#if (ENCODER_INTERRUPT_LEVEL == RISING)
  pinMode(ENCODER_PIN1, INPUT_PULLUP);
#else
  pinMode(ENCODER_PIN1, INPUT);
#endif

  // настраиваем второй выход энкодера на чтение
  pinMode(ENCODER_PIN2, INPUT);

  // ждём, пока устаканится питание
  delay(50);

  // взводим прерывание на входе срабатывания защиты
  attachInterrupt((RELAY_PIN), RelayTriggered, RELAY_INTERRUPT_LEVEL);

  // считаем импульсы на штанге по прерыванию
  attachInterrupt((ENCODER_PIN1),EncoderPulsesHandler, ENCODER_INTERRUPT_LEVEL);

}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::normalizeList(InterruptTimeList& list)
{
  size_t sz = list.size();
  
  if(sz < 2)
    return;

  // нормализуем список относительно первого значения
  uint32_t first = list[0];
  list[0] = 0;

  for(size_t i=1;i<sz;i++)
  {
    list[i] = (list[i] - first);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
int InterruptHandlerClass::writeLogRecord(uint32_t dataArrivedTime, CurrentOscillData& oscData, InterruptTimeList& _list, EthalonCompareResult compareResult
, EthalonCompareNumber num, InterruptTimeList& ethalonData, bool toEEPROM, int curEEPROMWriteAddress)
{

  int written = 0;
  
  if(_list.size() < 2) // ничего в списке прерываний нет
    return written;

 const uint8_t CHANNEL_NUM = 0;
 AT24CX* eeprom = Settings.getEEPROM();

 uint8_t workBuff[5] = {0};

  workBuff[0] = recordInterruptRecordBegin;

  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,1);
    written++;
    curEEPROMWriteAddress++;
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,1);
    #endif
  }
  
  // пишем номер канала, для которого сработало прерывание
  workBuff[0] = recordChannelNumber;
  workBuff[1] = CHANNEL_NUM; // ВСЕГДА ПЕРВЫЙ КАНАЛ !!!

  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,2);
    written += 2;
    curEEPROMWriteAddress += 2;
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,2);  
    #endif
  }
  
  // пишем положение штанги
  RodDirection rodPos = Settings.getRodDirection();// (channelNumber);

  workBuff[0] = recordRodPosition;
  workBuff[1] = rodPos;

  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,2);
    written += 2;
    curEEPROMWriteAddress += 2;
  }
  else
  {
    #ifndef _SD_OFF  
      Logger.write(workBuff,2);
    #endif // _SD_OFF
  }  

  // пишем время движения штанги  
  uint32_t moveTime = _list[_list.size()-1] - _list[0];
  workBuff[0] = recordMoveTime;
  memcpy(&(workBuff[1]),&moveTime,4);

  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,5);
    written += 5;
    curEEPROMWriteAddress += 5;    
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,5);
    #endif
  }
  

  // пишем кол-во срабатываний канала
  uint32_t motoresource = Settings.getMotoresource(CHANNEL_NUM); // ВСЕГДА ПЕРВЫЙ КАНАЛ

  workBuff[0] = recordMotoresource;
  memcpy(&(workBuff[1]),&motoresource,4);

  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,5);
    written += 5;
    curEEPROMWriteAddress += 5;    
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,5);  
    #endif
  }

  // пишем номер эталона, с которым сравнивали
  workBuff[0] = recordEthalonNumber;
  workBuff[1] = num;
  
  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,2);
    written += 2;
    curEEPROMWriteAddress += 2;        
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,2);
    #endif
  }
  
  // пишем результат сравнения с эталоном для канала
  workBuff[0] = recordCompareResult;
  workBuff[1] = compareResult;
  
  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,2);
    written += 2;
    curEEPROMWriteAddress += 2;        
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,2);
    #endif
  }

// пишем время, когда пошли данные, относительно начала сбора данных по току
  workBuff[0] = recordDataArrivedTime;
  memcpy(&(workBuff[1]),&dataArrivedTime,4);

  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,5);
    written += 5;
    curEEPROMWriteAddress += 5;        
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,5);    
    #endif
  }

  // пишем список прерываний
  if(_list.size() > 1)
  {
    // есть список прерываний
   workBuff[0] = recordInterruptDataBegin;
   uint16_t dataLen = _list.size();
   memcpy(&(workBuff[1]),&dataLen,2);

   if(toEEPROM)
   {
    eeprom->write(curEEPROMWriteAddress,workBuff,3);
    written += 3;
    curEEPROMWriteAddress += 3;
    
    eeprom->write(curEEPROMWriteAddress,(uint8_t*) _list.pData(), _list.size()*sizeof(uint32_t));
    written += _list.size()*sizeof(uint32_t);
    curEEPROMWriteAddress += _list.size()*sizeof(uint32_t);
   }
   else
   {
    #ifndef _SD_OFF
    Logger.write(workBuff,3);
    Logger.write((uint8_t*) _list.pData(), _list.size()*sizeof(uint32_t));
    #endif
   }

   workBuff[0] = recordInterruptDataEnd;
   
   if(toEEPROM)
   {
    eeprom->write(curEEPROMWriteAddress,workBuff,1);
    written += 1;
    curEEPROMWriteAddress += 1;            
   }
   else
   {
    #ifndef _SD_OFF
    Logger.write(workBuff,1);
    #endif
   }
  }

  if(ethalonData.size() > 1)
  {
   // пишем данные эталона, с которым сравнивали
   workBuff[0] = recordEthalonDataFollow;
   uint16_t dataLen = ethalonData.size();
   memcpy(&(workBuff[1]),&dataLen,2);

   if(toEEPROM)
   {
    eeprom->write(curEEPROMWriteAddress,workBuff,3);
    written += 3;
    curEEPROMWriteAddress += 3;
    
    eeprom->write(curEEPROMWriteAddress,(uint8_t*) ethalonData.pData(), ethalonData.size()*sizeof(uint32_t));
    written += ethalonData.size()*sizeof(uint32_t);
    curEEPROMWriteAddress += ethalonData.size()*sizeof(uint32_t);
    
   }
   else
   {
    #ifndef _SD_OFF
    Logger.write(workBuff,3);
    Logger.write((uint8_t*) ethalonData.pData(), ethalonData.size()*sizeof(uint32_t));    
    #endif
   }
  }

  // пишем данные по току
  if (oscData.times.size() > 1)
  {
	  workBuff[0] = recordOscDataFollow;
	  uint16_t dataLen = oscData.times.size();
	  memcpy(&(workBuff[1]), &dataLen, 2);

    if(toEEPROM)
    {
    eeprom->write(curEEPROMWriteAddress,workBuff,3);
    written += 3;
    curEEPROMWriteAddress += 3;
    
    eeprom->write(curEEPROMWriteAddress,(uint8_t*) oscData.times.pData(), oscData.times.size()*sizeof(uint32_t));
    written += oscData.times.size()*sizeof(uint32_t);
    curEEPROMWriteAddress += oscData.times.size()*sizeof(uint32_t);

    eeprom->write(curEEPROMWriteAddress,(uint8_t*) oscData.data1.pData(), oscData.data1.size()*sizeof(uint32_t));
    written += oscData.data1.size()*sizeof(uint32_t);
    curEEPROMWriteAddress += oscData.data1.size()*sizeof(uint32_t);

    eeprom->write(curEEPROMWriteAddress,(uint8_t*) oscData.data2.pData(), oscData.data2.size()*sizeof(uint32_t));
    written += oscData.data2.size()*sizeof(uint32_t);
    curEEPROMWriteAddress += oscData.data2.size()*sizeof(uint32_t);

    eeprom->write(curEEPROMWriteAddress,(uint8_t*) oscData.data3.pData(), oscData.data3.size()*sizeof(uint32_t));
    written += oscData.data3.size()*sizeof(uint32_t);
    curEEPROMWriteAddress += oscData.data3.size()*sizeof(uint32_t);
      
    }
    else
    {
      #ifndef _SD_OFF
	    Logger.write(workBuff, 3);
	    Logger.write((uint8_t*)oscData.times.pData(), oscData.times.size() * sizeof(uint32_t));
	    Logger.write((uint8_t*)oscData.data1.pData(), oscData.data1.size() * sizeof(uint32_t));
	    Logger.write((uint8_t*)oscData.data2.pData(), oscData.data2.size() * sizeof(uint32_t));
	    Logger.write((uint8_t*)oscData.data3.pData(), oscData.data3.size() * sizeof(uint32_t));
     #endif
    }
  }

  // заканчиваем запись
  workBuff[0] = recordInterruptRecordEnd;
  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,1);
    written += 1;
    curEEPROMWriteAddress += 1;                
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,1);
    #endif
  }


return written;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::writeToLog(
  uint32_t dataArrivedTime, 
  DS3231Time& tm,
	CurrentOscillData& oscData,
	InterruptTimeList& lst1, 
	EthalonCompareResult res1, 
	EthalonCompareNumber num1,
	InterruptTimeList& ethalonData1,
  bool toEEPROM
)
{

  uint8_t workBuff[10] = {0};
  int eepromAddress = EEPROM_LAST_3_DATA_ADDRESS;
  int recordStartAddress = 0;
  int recordTotalLength = 0;
  
  AT24CX* eeprom = Settings.getEEPROM();
  

  if(toEEPROM)
  {
    // вычисляем адрес для записи в EEPROM
    // сначала смотрим, под каким индексом записывать?
    uint8_t idx = 0;
    int header1 = eeprom->read(eepromAddress);
    int header2 = eeprom->read(eepromAddress+1);
    int header3 = eeprom->read(eepromAddress+2);
    
    if(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2 && header3 == RECORD_HEADER3)
    {
      // прочитали текущий индекс, и инкрементировали его
      idx = eeprom->read(eepromAddress+3);
      idx++;
      if(idx > 2) // пишем только последние 3 срабатывания
      {
        idx = 0;
      }

      // записали новый индекс
      eeprom->write(eepromAddress+3,idx);
    }
    else
    {
      // нет записей, неправильные заголовки, надо записать
      eeprom->write(eepromAddress,RECORD_HEADER1);
      eeprom->write(eepromAddress+1,RECORD_HEADER2);
      eeprom->write(eepromAddress+2,RECORD_HEADER3);
      eeprom->write(eepromAddress+3,idx);
    }

    // теперь надо просчитать смещение для старта начала записи. При этом от начального адреса пропускаем 4 байта (адрес хранения индекса текущей записи)
    eepromAddress = EEPROM_LAST_3_DATA_ADDRESS + 4 + idx*EEPROM_LAST_3_RECORD_SIZE;

    // сначала записываем заголовок для нашей записи
    eeprom->write(eepromAddress,RECORD_HEADER1);
    eeprom->write(eepromAddress+1,RECORD_HEADER2);
    eeprom->write(eepromAddress+2,RECORD_HEADER3);
    eepromAddress += 3;

    // мы теперь на начале данных записи, надо пропустить 4 байта (куда мы потом запишем длину записи), и сохранить указатель на начало записи
    recordStartAddress = eepromAddress;
    eepromAddress += 4; // теперь можем писать данные, начиная с этого адреса
    
  }

  workBuff[0] = recordInterruptInfoBegin;

  if(toEEPROM)
  {
    // потом пишем заголовок начала данных
    eeprom->write(eepromAddress,workBuff,1);
    eepromAddress++;
    recordTotalLength++;
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,1);
    #endif
  }
  
  // пишем время срабатывания прерывания
  workBuff[0] = recordInterruptTime;
  workBuff[1] = tm.dayOfMonth;
  workBuff[2] = tm.month;
  memcpy(&(workBuff[3]),&(tm.year),2);
  workBuff[5] = tm.hour;
  workBuff[6] = tm.minute;
  workBuff[7] = tm.second;

  if(toEEPROM)
  {
    eeprom->write(eepromAddress,workBuff,8);
    eepromAddress += 8;
    recordTotalLength += 8;
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,8);
    #endif
  }


  // пишем температуру системы
  DS3231Temperature temp = Settings.getTemperature();

  workBuff[0] = recordSystemTemperature;
  workBuff[1] = temp.Value;
  workBuff[2] = temp.Fract;

  if(toEEPROM)
  {
    eeprom->write(eepromAddress,workBuff,3);
    eepromAddress += 3;  
    recordTotalLength += 3;  
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,3);
    #endif
  }
  
  // теперь смотрим, в каких списках есть данные, и пишем записи в лог
  if(lst1.size() > 1)
  {
    int written = writeLogRecord(dataArrivedTime,oscData,lst1,res1,num1, ethalonData1,toEEPROM,eepromAddress);
    eepromAddress += written;
    recordTotalLength += written;
  } // if


    workBuff[0] = recordInterruptInfoEnd;

    if(toEEPROM)
    {
       eeprom->write(eepromAddress,workBuff,1);
       recordTotalLength++;

       // и не забываем записать всю длину сохранённых данных !!!
       eeprom->write(recordStartAddress,(uint8_t*)&recordTotalLength,4);
    }
    else
    {
      #ifndef _SD_OFF
      Logger.write(workBuff,1);
      #endif
    }

}
//--------------------------------------------------------------------------------------------------------------------------------------
// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::startCollectCurrentData()
{
#ifndef CURRENT_OSCILL_OFF
	// сработала защита, нам надо собирать данные по току с определённым интервалом
	if (!currentOscillTimerActive)
	{
		oscillData.clear();
		currentOscillTimerActive = true;
		currentOscillTimer = micros();
	}
#endif // #ifndef CURRENT_OSCILL_OFF
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::stopCollectCurrentData()
{
	currentOscillTimerActive = false;
}
//--------------------------------------------------------------------------------------------------------------------------------------
CurrentOscillData& InterruptHandlerClass::getCurrentData()
{
	return oscillData;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::update()
{

	#ifndef CURRENT_OSCILL_OFF
	// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
	if (currentOscillTimerActive)
	{
		// просто собираем информацию по току через указанные промежутки времени
		if (adcSampler.available())
		{
			// есть данные по АЦП, проверяем таймер
			if (micros() - currentOscillTimer >= CURRENT_OSCILL_FREQ)
			{
				// промежуток времени прошёл, собираем данные с АЦП, по всем трём каналам
				uint32_t cT = micros();

				int bufferLength = 0;
				uint16_t* cBuf = adcSampler.getADCBuffer(&bufferLength);    // Получить буфер с данными

				uint16_t countOfPoints = bufferLength / NUM_CHANNELS;
				
				uint32_t raw1 = 0;
				uint32_t raw2 = 0;
				uint32_t raw3 = 0;

          /*
            Буфер у нас для четырёх каналов, индексы:
        
            0 -  Аналоговый вход трансформатора №1
            1 - Аналоговый вход трансформатора №2
            2 -  Аналоговый вход трансформатора №3
            3 - Аналоговый вход контроль питания 3.3в
        
        */  

				for (int i = 0; i < bufferLength; i = i + NUM_CHANNELS)                // получить результат измерения поканально, с интервалом 3
				{

					raw1 += cBuf[i + 0];                          // Данные 1 графика  (красный)
					raw2 += cBuf[i + 1];                          // Данные 2 графика  (синий)
					raw3 += cBuf[i + 2];                          // Данные 3 графика  (желтый)

				} // for

				raw1 /= countOfPoints;
				raw2 /= countOfPoints;
				raw3 /= countOfPoints;

				// посчитали среднее, заносим в список
				oscillData.times.push_back(cT);
				oscillData.data1.push_back(raw1);
				oscillData.data2.push_back(raw2);
				oscillData.data3.push_back(raw3);

				currentOscillTimer = micros();
				adcSampler.reset(); // сбрасываем признак готовности данных

			} // if


		} // if (adcSampler.available())

	} // if(currentOscillTimerActive)
	  // ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //
	#endif // #ifndef CURRENT_OSCILL_OFF


  static bool inProcess = false;

  noInterrupts();
    bool thisHasEncoderInterrupt = hasEncoderInterrupt;
    uint32_t thisLastEncoderInterruptTime = lastEncoderInterruptTime;
    
    bool thisHasRelayTriggered = hasRelayTriggered;
    uint32_t thisRelayTriggeredTime = relayTriggeredTime;

#ifndef _RMS_OFF
	bool thisWantComputeRMS = wantComputeRMS;
	wantComputeRMS = false;
#endif // _RMS_OFF

  interrupts();

#ifndef _RMS_OFF
  if (thisWantComputeRMS) // надо считать РМС
  {
	//  DBGLN(F("Надо считать RMS!"));
	  thisWantComputeRMS = false;
	  inComputeRMSMode = true;

	  rmsStartComputeTime = 0;
	  // считаем РМС
	  computeRMS();

  } // if(wantComputeRMS)

  if (inComputeRMSMode)
  {
	  if (millis() - rmsStartComputeTime > RMS_COMPUTE_TIME)
	  {
	//	  DBGLN(F("RMS собрано, проверяем!"));
		  inComputeRMSMode = false;
		  // время подсчёта РМС вышло, надо проверять
		  checkRMS(); // проверяем РМС
	  }

  }
#endif // _RMS_OFF


  // проверяем факт срабатывания защиты
  if(thisHasRelayTriggered)
  {

	// защита сработала, надо считать РМС !!!
//	DBGLN(F("СРАБОТАЛО РЕЛЕ ЗАЩИТЫ!"));

#ifndef _RMS_OFF
	wantComputeRMS = true;

	if (!rmsStartComputeTime)
		rmsStartComputeTime = millis();
#endif // _RMS_OFF

    // было прерывание срабатывания защиты - проверяем время c момента срабатывания
    if(micros() - thisRelayTriggeredTime >= Settings.getRelayDelay())
    {      
      // время ожидания прошло
	//	DBGLN(F("Время ожидания после срабатывания реле вышло, продолжаем..."));
      // проверяем - если данные в одном из списков есть - ничего не делаем.
      // если ни в одном из списков нет данных - значит, это авария.
      // в любом другом случае флаг аварии выставится после того, как будет принято решение
      // о том, что пачки импульсов закончились.
      
      noInterrupts();

       hasRelayTriggered = false;
       relayTriggeredTime = micros();
	   hasAlarm = !(list1.size());
       
       if(hasAlarm)
       {
        // есть тревога, надо подождать окончания прерываний c энкодера
        thisHasEncoderInterrupt = true;
        thisLastEncoderInterruptTime = micros();

		hasEncoderInterrupt = true;
		lastEncoderInterruptTime = micros();

        timeBeforeInterruptsBegin = micros() - thisRelayTriggeredTime;
       }

      interrupts();      

	  // обновляем моторесурс, т.к. было срабатывание защиты
	  uint32_t motoresource = Settings.getMotoresource(0);
	  motoresource++;
	  Settings.setMotoresource(0, motoresource);

      // выставляем флаг аварии, в зависимости от наличия данных в списках
      if(hasAlarm)
      {
	//	  DBGLN(F("Взведён флаг аварии!"));
		  // сделал именно так, поскольку флаг аварии сбрасывать нельзя, плюс могут понадобиться дополнительные действия
        Feedback.alarm(true);
      }
    } // if

	// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
	// сработала защита, нам надо собирать данные по току с определённым интервалом
	startCollectCurrentData();	
	// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //
    
  } // if(thisHasRelayTriggered)


  // работаем с энкодером, а именно - ожидаем окончание сбора с него данных

  if (!thisHasEncoderInterrupt || inProcess)
  {
	  return;
  }
  
      if(!(micros() - thisLastEncoderInterruptTime > INTERRUPT_MAX_IDLE_TIME)) // ещё не вышло максимальное время ожидания окончания прерываний на энкодере
      {
        return;
      }

    noInterrupts();

      inProcess = true;
	  hasEncoderInterrupt = false;
      
      InterruptTimeList copyList1 = list1; // копируем данные в локальный список
      // вызываем не clear, а empty, чтобы исключить лишние переаллокации памяти
      list1.empty();

	  // копируем данные по току в локальный список
	  stopCollectCurrentData();
	  CurrentOscillData copyOscillData = oscillData;
	  oscillData.clear();

          
    interrupts();



    // вычисляем смещение от начала записи по току до начала поступления данных
    uint32_t datArrivTm = 0;
    if(copyOscillData.times.size() > 0 && copyList1.size() > 0)
    {
      datArrivTm = copyList1[0] - copyOscillData.times[0];
    }

	// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
	// нормализуем список времен записей по току
	InterruptHandlerClass::normalizeList(copyOscillData.times);
	// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

	// здесь мы получили список прерываний, и можно с ним что-то делать
     InterruptHandlerClass::normalizeList(copyList1);

     
     EthalonCompareResult compareRes1 = COMPARE_RESULT_NoSourcePulses;

	 EthalonCompareNumber compareNumber1;
	 InterruptTimeList ethalonData1;
     
    bool needToLog = false;

    // теперь смотрим - надо ли нам самим чего-то обрабатывать?
    if(copyList1.size() > 1)
    {
	//	DBG("Прерывание содержит данные: ");
  //    DBGLN(copyList1.size());

      // зажигаем светодиод "ТЕСТ"
      Feedback.testDiode();

      needToLog = true;
        
       // здесь мы можем обрабатывать список сами - в нём ЕСТЬ данные
       compareRes1 = EthalonComparer::Compare(copyList1, 0,compareNumber1, ethalonData1);

       if(compareRes1 == COMPARE_RESULT_MatchEthalon)
        {}
       else if(compareRes1 == COMPARE_RESULT_MismatchEthalon || compareRes1 == COMPARE_RESULT_RodBroken)
       {
          Feedback.failureDiode();
          Feedback.alarm();
       }
    }
	else
	{
	//	DBGLN(F("Прерывание НЕ содержит данных!!!"));
	}
    

    if(needToLog)
    {
#ifndef _SD_OFF
	//	DBGLN(F("Надо сохранить в лог, пишем на SD!"));
      // надо записать в лог дату срабатывания системы
      InterruptHandlerClass::writeToLog(datArrivTm, trigTime, copyOscillData,copyList1, compareRes1, compareNumber1, ethalonData1);
#endif // !_SD_OFF
    } // needToLog
    

    // если в каком-то из списков есть данные - значит, одно из прерываний сработало,
    // в этом случае мы должны сообщить обработчику, что данные есть. При этом мы
    // не в ответе за то, что делает сейчас обработчик - пускай сам разруливает ситуацию
    // так, как нужно ему.

    bool wantToInformSubscriber = ( hasAlarm || (copyList1.size() > 1));

    if(wantToInformSubscriber)
    { 
	//	DBGLN(F("Надо уведомить подписчика прерываний!"));
      if(subscriber)
      {
		//  DBGLN(F("Подписчик найден!"));
        noInterrupts();
        uint32_t thisTm = timeBeforeInterruptsBegin;
        bool thisHasRelayTriggeredTime = hasRelayTriggeredTime;
        
        timeBeforeInterruptsBegin = 0;
        hasRelayTriggeredTime = false;
        relayTriggeredTime = micros();
        interrupts();

#ifdef _FAKE_CHART_DRAW
		////////////////////////////////////////////////////////////////////////////////////
		// тут тупо пытаемся сделать кучу данных в списке
		////////////////////////////////////////////////////////////////////////////////////

		const int TO_GENERATE = FAKE_POINTS_TO_GENERATE; // сколько тестовых точек генерировать?
		copyList1.clear();
		copyList1.reserve(TO_GENERATE);
		uint32_t val = 0;
		uint32_t spacer = 0;

		while (copyList1.size() < TO_GENERATE)
		{
			val += spacer;
			spacer++;
			copyList1.push_back(val);
		}
		////////////////////////////////////////////////////////////////////////////////////
#endif // _FAKE_CHART_DRAW

		// уведомляем подписчика
		informSubscriber(copyOscillData,copyList1, compareRes1, thisTm, thisHasRelayTriggeredTime);

      } // if(subscriber)
      else
      {
		// подписчика нет, просто очищаем переменные
	//	  DBGLN(F("!!! ПОДПИСЧИКА НЕТ !!!"));
        noInterrupts();
        timeBeforeInterruptsBegin = 0;
        relayTriggeredTime = micros();
        interrupts();
      }
      
    }    

	// всё обработали
    inProcess = false;

}
//--------------------------------------------------------------------------------------------------------------------------------------
InterruptEventSubscriber* InterruptHandlerClass::getSubscriber()
{
	return subscriber;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::setSubscriber(InterruptEventSubscriber* h)
{  
  // устанавливаем подписчика результатов прерываний.
  subscriber = h;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::informSubscriber(CurrentOscillData& oscData, InterruptTimeList& list, EthalonCompareResult compareResult, uint32_t timeBeforeInterruptsBegin, uint32_t relayTriggeredTime)
{
	if (subscriber)
	{
		//DBGLN(F("Subscriber exists!"));

		//subscriber->OnTimeBeforeInterruptsBegin(timeBeforeInterruptsBegin, relayTriggeredTime);
		subscriber->OnInterruptRaised(oscData, list, compareResult);
		// сообщаем обработчику, что данные в каком-то из списков есть
		subscriber->OnHaveInterruptData();

		//DBGLN(F("Subscriber informed!"));
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------


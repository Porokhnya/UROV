#include "InterruptHandler.h"
#include "InterruptScreen.h"
#include "Endstops.h"
#include "Feedback.h"
#include "DS3231.h"
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

volatile uint32_t timeBeforeInterruptsBegin = 0; // время от срабатывания реле защиты до первого прерывания
volatile bool hasRelayTriggeredTime = false; // флаг, что было срабатывание реле защиты перед пачкой прерываний

volatile bool wantComputeRMS = false; // флаг, что мы должны подсчитать РМС
volatile bool inComputeRMSMode = false; // флаг, что мы считаем РМС
volatile uint32_t rmsStartComputeTime = 0; // начало времени подсчёта РМС
volatile bool computeRMSCalled = false; // флаг, что мы попросили АЦП подсчитать РМС
volatile bool downEndstopTriggered = false; // состояние нижнего концевика на момент срабатывания защиты
//--------------------------------------------------------------------------------------------------------------------------------------
InterruptEventSubscriber* subscriber = NULL;
//--------------------------------------------------------------------------------------------------------------------------------------
void setEncoderInterruptFlag()
{
	hasEncoderInterrupt = true;
	lastEncoderInterruptTime = micros();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void EncoderPulsesHandler() // обработчик импульсов энкодера
{
    uint32_t now = micros();
    list1.push_back(now);

	setEncoderInterruptFlag();

  if(list1.size() < 2)
  {
    timeBeforeInterruptsBegin = (micros() - relayTriggeredTime);
  }

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
    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void computeRMS()
{
	if (computeRMSCalled)
		return;

	computeRMSCalled = true;
	// считаем РМС
	adcSampler.startComputeRMS();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void checkRMS()
{
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

}
//--------------------------------------------------------------------------------------------------------------------------------------
/*
//DEPRECATED:
void MakeAcsSignalDecision(void* param)
{
  // принимаем решение - выдавать ли сигнал на АСУ ТП.
  // сигнал НЕ выдаётся, если:
  // положение штанги - поломана
  // неисправность индуктивных датчиков
  // неисправность параметров питания
  // нет ни одного импульса прерываний

  // проверяем положение штанги
  for(uint8_t i=0;i<NUM_RODS;i++)
  {
    if(ConfigPin::getRodPosition(i) == rpBroken)
    {
      // одна из штанг поломана
      return;
    }
  }

  // проверяем исправность индуктивных датчиков
  for(uint8_t i=0;i<NUM_RODS;i++)
  {
    if(Settings.getInductiveSensorState(i) != 1)
    {
      // один из индуктивных датчиков неисправен
      return;
    }
  }


  // проверяем параметры питания
  
  // Контроль источника питания +3.3в
  VoltageData vData = Settings.get3V3Voltage();
  
  float threshold = (3.3/100)*VOLTAGE_THRESHOLD;
  float lowBorder = 3.3 - threshold;
  float highBorder = 3.3 + threshold;
  
  if(vData.voltage >= lowBorder && vData.voltage <= highBorder)
  {
  }
  else
  {
    // неисправность питания +3.3в
    return;
  }
  
  // Контроль источника питания +5.0в
  vData = Settings.get5Vvoltage();        
  threshold = (5.0/100)*VOLTAGE_THRESHOLD;
  lowBorder = 5.0 - threshold;
  highBorder = 5.0 + threshold;
  
  if(vData.voltage >= lowBorder && vData.voltage <= highBorder)
  {
  }
  else
  {
    // неисправность питания +5.0в
    return;
  }    
  
  // Контроль источника питания 200в 
  vData = Settings.get200Vvoltage();      
  threshold = (200.0/100)*VOLTAGE_THRESHOLD;
  lowBorder = 200.0 - threshold;
  highBorder = 200.0 + threshold;
  
  
  if(vData.voltage >= lowBorder && vData.voltage <= highBorder)
  {      
  }
  else
  {
    // неисправность питания 200в
    return;
    
  }

  // проверяем наличие данных в списке
  noInterrupts();
  bool hasData = list1.size();//DEPRECATED: || list2.size() || list3.size();
  interrupts();

  if(!hasData)
  {
    // нет ни одного импульса прерываний
    return;
  }



  // все условия выполнены, можем подавать сигнал на АСУ ТП
  digitalWrite(out_asu_tp1,ACS_SIGNAL_LEVEL);
    
}
*/
//--------------------------------------------------------------------------------------------------------------------------------------
void RelayTriggered()
{
  // запоминаем время срабатывания защиты
  relayTriggeredTime = micros();
  hasRelayTriggered = true;
  hasRelayTriggeredTime = true;
  wantComputeRMS = true; // говорим, что надо посчитать РМС
  timeBeforeInterruptsBegin = 0; // сбрасываем время до начала импульсов

  // сохраняем состояние нижнего концевика 
  downEndstopTriggered = RodDownEndstopTriggered(false);

  // взводим отложенное событие
  //DEPRECATED: 
  //uint32_t raiseDelay = Settings.getACSDelay()*1000;
  //CoreDelayedEvent.raise(raiseDelay,MakeAcsSignalDecision,NULL);
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

  NVIC_SetPriorityGrouping(NVIC_PriorityGroup_1);

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
  delay(10);

  // взводим прерывание на входе срабатывания защиты
  attachInterrupt(digitalPinToInterrupt(RELAY_PIN), RelayTriggered, RELAY_INTERRUPT_LEVEL);

  // считаем импульсы на штанге по прерыванию
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN1),EncoderPulsesHandler, ENCODER_INTERRUPT_LEVEL);

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
void InterruptHandlerClass::writeRodPositionToLog(uint8_t channelNumber)
{
 // пишем положение штанги
	RodDirection rodPos = Settings.getRodDirection();// (channelNumber);

  uint8_t workBuff[2] = {0};
  workBuff[0] = recordRodPosition;
  workBuff[1] = rodPos;
  
  Logger.write(workBuff,2);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::writeLogRecord(uint8_t channelNumber, InterruptTimeList& _list, EthalonCompareResult compareResult, EthalonCompareNumber num, InterruptTimeList& ethalonData)
{
  if(_list.size() < 2) // ничего в списке прерываний нет
    return;

 uint8_t workBuff[5] = {0};

  workBuff[0] = recordInterruptRecordBegin;
  Logger.write(workBuff,1);
  
  // пишем номер канала, для которого сработало прерывание
  workBuff[0] = recordChannelNumber;
  workBuff[1] = channelNumber;
  Logger.write(workBuff,2);
  
  // пишем положение штанги
  writeRodPositionToLog(channelNumber);

  // пишем время движения штанги  
  uint32_t moveTime = _list[_list.size()-1] - _list[0];
  workBuff[0] = recordMoveTime;
  memcpy(&(workBuff[1]),&moveTime,4);
  Logger.write(workBuff,5);
  

  // пишем кол-во срабатываний канала
  uint32_t motoresource = Settings.getMotoresource(channelNumber);
  motoresource++;
  Settings.setMotoresource(channelNumber,motoresource);

  workBuff[0] = recordMotoresource;
  memcpy(&(workBuff[1]),&motoresource,4);
  Logger.write(workBuff,5);  

  // пишем номер эталона, с которым сравнивали
  workBuff[0] = recordEthalonNumber;
  workBuff[1] = num;
  Logger.write(workBuff,2);
  
  // пишем результат сравнения с эталоном для канала
  workBuff[0] = recordCompareResult;
  workBuff[1] = compareResult;
  Logger.write(workBuff,2);

  // пишем список прерываний
  if(_list.size() > 1)
  {
    // есть список прерываний
   workBuff[0] = recordInterruptDataBegin;
   uint16_t dataLen = _list.size();
   memcpy(&(workBuff[1]),&dataLen,2);
   Logger.write(workBuff,3);

   Logger.write((uint8_t*) _list.pData(), _list.size()*sizeof(uint32_t));
   workBuff[0] = recordInterruptDataEnd;
   Logger.write(workBuff,1);
  }

  if(ethalonData.size() > 1)
  {
   // пишем данные эталона, с которым сравнивали
   workBuff[0] = recordEthalonDataFollow;
   uint16_t dataLen = ethalonData.size();
   memcpy(&(workBuff[1]),&dataLen,2);
   Logger.write(workBuff,3);

   Logger.write((uint8_t*) ethalonData.pData(), ethalonData.size()*sizeof(uint32_t));    
  }

  // заканчиваем запись
  workBuff[0] = recordInterruptRecordEnd;
  Logger.write(workBuff,1);
    
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::writeToLog(
	InterruptTimeList& lst1, 
	EthalonCompareResult res1, 
	EthalonCompareNumber num1,
	InterruptTimeList& ethalonData1
)
{

  uint8_t workBuff[10] = {0};

  workBuff[0] = recordInterruptInfoBegin;
  Logger.write(workBuff,1);
  
  // пишем время срабатывания прерывания
  DS3231Time tm = RealtimeClock.getTime();

  workBuff[0] = recordInterruptTime;
  workBuff[1] = tm.dayOfMonth;
  workBuff[2] = tm.month;
  memcpy(&(workBuff[3]),&(tm.year),2);
  workBuff[5] = tm.hour;
  workBuff[6] = tm.minute;
  workBuff[7] = tm.second;
  
  Logger.write(workBuff,8);

  // пишем температуру системы
  DS3231Temperature temp = Settings.getTemperature();

  workBuff[0] = recordSystemTemperature;
  workBuff[1] = temp.Value;
  workBuff[2] = temp.Fract;
  
  Logger.write(workBuff,3);
  
  // теперь смотрим, в каких списках есть данные, и пишем записи в лог
  if(lst1.size() > 1)
  {
    writeLogRecord(0,lst1,res1,num1, ethalonData1); 
  } // if


    workBuff[0] = recordInterruptInfoEnd;
    Logger.write(workBuff,1);
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::update()
{

  static bool inProcess = false;

  noInterrupts();
    bool thisHasEncoderInterrupt = hasEncoderInterrupt;
    uint32_t thisLastEncoderInterruptTime = lastEncoderInterruptTime;
    
    bool thisHasRelayTriggered = hasRelayTriggered;
    uint32_t thisRelayTriggeredTime = relayTriggeredTime;

	bool thisWantComputeRMS = wantComputeRMS;
	wantComputeRMS = false;

  interrupts();

  if (thisWantComputeRMS) // надо считать РМС
  {
	  DBGLN(F("Want to compute RMS!"));
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
		  DBGLN(F("RMS compute done, check RMS!"));
		  inComputeRMSMode = false;
		  // время подсчёта РМС вышло, надо проверять
		  checkRMS(); // проверяем РМС
	  }

  }



  // проверяем факт срабатывания защиты
  if(thisHasRelayTriggered)
  {

	// защита сработала, надо считать РМС !!!
	DBGLN(F("RELAY TRIGGERED!"));

	wantComputeRMS = true;

	if (!rmsStartComputeTime)
		rmsStartComputeTime = millis();

    // было прерывание срабатывания защиты - проверяем время c момента срабатывания
    if(micros() - thisRelayTriggeredTime >= Settings.getRelayDelay())
    {      
      // время ожидания прошло
		DBGLN(F("Relay delay check..."));
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

      // выставляем флаг аварии, в зависимости от наличия данных в списках
      if(hasAlarm)
      {
		  DBGLN(F("HAS ALARM FLAG!"));
		  // сделал именно так, поскольку флаг аварии сбрасывать нельзя, плюс могут понадобиться дополнительные действия
        Feedback.alarm(true);
      }
    } // if
    
  } // if


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
      
      InterruptTimeList copyList1 = list1;
      // вызываем не clear, а empty, чтобы исключить лишние переаллокации памяти
      list1.empty();
          
    interrupts();

	// здесь мы получили список прерываний, и можно с ним что-то делать
     InterruptHandlerClass::normalizeList(copyList1);
     EthalonCompareResult compareRes1 = COMPARE_RESULT_NoSourcePulses;

	 EthalonCompareNumber compareNumber1;
	 InterruptTimeList ethalonData1;
     
    bool needToLog = false;

    // теперь смотрим - надо ли нам самим чего-то обрабатывать?
    if(copyList1.size() > 1)
    {
      DBG("INTERRUPT #1 HAS SERIES OF DATA: ");
      DBGLN(copyList1.size());

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
		DBGLN(F("INTERRUPT LIST HAS NO DATA!!!"));
	}
    

    if(needToLog)
    {
		DBGLN(F("Want to log, write to SD!"));
      // надо записать в лог дату срабатывания системы
      InterruptHandlerClass::writeToLog(copyList1, compareRes1, compareNumber1, ethalonData1);     
    } // needToLog
    

    // если в каком-то из списков есть данные - значит, одно из прерываний сработало,
    // в этом случае мы должны сообщить обработчику, что данные есть. При этом мы
    // не в ответе за то, что делает сейчас обработчик - пускай сам разруливает ситуацию
    // так, как нужно ему.

    bool wantToInformSubscriber = ( hasAlarm || (copyList1.size() > 1));

    if(wantToInformSubscriber)
    { 
		DBGLN(F("Want to inform subscriber!"));
      if(subscriber)
      {
		  DBGLN(F("Subscriber exists!"));
        noInterrupts();
        uint32_t thisTm = timeBeforeInterruptsBegin;
        bool thisHasRelayTriggeredTime = hasRelayTriggeredTime;
        
        timeBeforeInterruptsBegin = 0;
        hasRelayTriggeredTime = false;
        relayTriggeredTime = micros();
        interrupts();

        subscriber->OnTimeBeforeInterruptsBegin(thisTm, thisHasRelayTriggeredTime);
                
        subscriber->OnInterruptRaised(copyList1, compareRes1);        
         // сообщаем обработчику, что данные в каком-то из списков есть
         subscriber->OnHaveInterruptData();

		 DBGLN(F("Subscriber call done."));
      }
      else
      {
		// подписчика нет, просто очищаем переменные
		  DBGLN(F("NO SUBSСRIBER FOUND!!!"));
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
void InterruptHandlerClass::setSubscriber(InterruptEventSubscriber* h)
{  
  // устанавливаем подписчика результатов прерываний.
  subscriber = h;
}
//--------------------------------------------------------------------------------------------------------------------------------------

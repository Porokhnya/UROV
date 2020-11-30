#include "InterruptHandler.h"
#include "InterruptScreen.h"
#include "Endstops.h"
#include "Feedback.h"
#include "FileUtils.h"
#include "Logger.h"
#include "Settings.h"
#include "DelayedEvents.h"
#include "ADCSampler.h"
#include "RelayGuard.h"
//--------------------------------------------------------------------------------------------------------------------------------------
InterruptHandlerClass InterruptHandler;
CurrentOscillData     OscillData; // данные по току, актуальные на момент прерывания
//--------------------------------------------------------------------------------------------------------------------------------------
InterruptTimeList InterruptData; // список времён срабатываний прерываний на энкодере штанги
MachineState machineState = msIdle; // состояние конечного автомата
volatile bool canHandleEncoder = false; // флаг, что мы можем собирать прерывания с энкодера
volatile uint32_t encoderTimer = 0; // служебный таймер для обработки энкодера
DS3231Time relayTriggeredTime; // время срабатывания защиты
volatile bool downEndstopTriggered = false; // состояние нижнего концевика на момент срабатывания защиты
//--------------------------------------------------------------------------------------------------------------------------------------
volatile uint16_t interruptSkipCounter = 0; // счётчик пойманных импульсов, для пропуска лишних
volatile bool paused = false; // флаг, что обработчик - на паузе
//--------------------------------------------------------------------------------------------------------------------------------------
#define REASON_RELAY  1 // причина старабывания - внешнее реле
#define REASON_PREDICT 2 // причина срабатывания - предсказания
#define REASON_PEAK   3 // причина срабатывания - превышение по току

volatile uint8_t trigReason = 0; // причина срабатывания
volatile uint32_t trigReasonTimer = 0; // таймер отсчёта от причины срабатывания
volatile bool asuTPAlarmFlag = false; // флаг, что это авария, которую надо зафиксировать на выходной линии АСУ ТП
volatile bool peakAlarmFlag = false; // флаг, что было зафиксировано превышение по току
volatile bool relayTrigCatched = false; // флаг, что было зафиксировано срабатывание внешней защиты

volatile uint32_t lastPeakDetectedTimer = 0; // таймер последнего превышения по току
//--------------------------------------------------------------------------------------------------------------------------------------
volatile bool canCatchInitialRotationDirection = false; // флаг, что мы должны засечь и сохранить первоначальное направление движения штанги
volatile uint8_t initialDirection = 0xFF;       // первоначальное направление движения штанги
volatile uint8_t lastKnownDirection = 0xFF;     // последнее известное направление движения штанги
DirectionInfoData DirectionInfo;  // список изменений направления вращения энкодера
volatile bool aFlag = 0;
volatile bool bFlag = 0;
volatile uint8_t rotationDirection = 0xFF;
volatile uint8_t transitionState = 0; // таблица переходов энкодера
//--------------------------------------------------------------------------------------------------------------------------------------
bool hasRelayTriggered()
{

  if(RelayGuard.isTriggered())
  {
    relayTriggeredTime = RealtimeClock.getTime(); // запоминаем время срабатывания входа релейной защиты
    
    // сохраняем состояние нижнего концевика 
    downEndstopTriggered = RodDownEndstopTriggered(false);

    return true;
  }

  return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------
// ПРЕДСКАЗАНИЯ
//--------------------------------------------------------------------------------------------------------------------------------------
#ifdef PREDICT_ENABLED
//--------------------------------------------------------------------------------------------------------------------------------------
volatile bool predictEnabledFlag = true; // флаг, что мы можем собирать информацию о предсказаниях срабатывания защиты
InterruptTimeList predictList; // список для предсказаний
volatile bool predictTriggeredFlag = false; // флаг срабатывания предсказания
//--------------------------------------------------------------------------------------------------------------------------------------
void predictOff() // выключаем предсказание
{
  if(predictEnabledFlag)
  {
    predictEnabledFlag = false; // отключаем сбор предсказаний
    predictList.clear(); // очищаем список предсказаний
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void predictOn() // включаем предсказание
{
  if(!predictEnabledFlag)
  {
    predictEnabledFlag = true; // включаем сбор предсказаний
    predictList.clear(); // очищаем список предсказаний
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool predictTriggered() // возвращает флаг срабатывания предсказания, однократно (т.е. флаг срабатывания предсказания сбрасывается перед выходом из функции)
{
  bool f = predictTriggeredFlag;
  if(f)
  {
    noInterrupts();
    predictTriggeredFlag = false;  
    interrupts();
  }
  return f;
}
//--------------------------------------------------------------------------------------------------------------------------------------
#endif // PREDICT_ENABLED
//--------------------------------------------------------------------------------------------------------------------------------------
void copyPredictToList() // копируем предсказания в список срабатывания
{
  noInterrupts();

    InterruptData.empty(); // очищаем список прерываний

    #ifdef PREDICT_ENABLED
    // тут копируем полученные в предсказании импульсы в список
    for(size_t k=0;k<predictList.size();k++)
    {
      InterruptData.push_back(predictList[k]);
    }  
    #endif // PREDICT_ENABLED
            
  interrupts();
}
//--------------------------------------------------------------------------------------------------------------------------------------
InterruptEventSubscriber* subscriber = NULL; // подписчик для обработки результатов пачки прерываний
//--------------------------------------------------------------------------------------------------------------------------------------
uint8_t GetRotationDirection() // возвращает направление движения энкодера
{
  return rotationDirection;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void saveTransitionState()
{
  uint8_t aState = digitalRead(ENCODER_PIN1);
  uint8_t bState = digitalRead(ENCODER_PIN2);

  transitionState <<= 1;
  transitionState |= aState;
  transitionState <<= 1;
  transitionState |= bState;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void handleDirection()
{
    if(transitionState == 11 || transitionState == 14)
    {
      //CW!
      rotationDirection = rpUp; // clockwise
    }
    else if(transitionState == 7 || transitionState == 13)
    {
      //CCW!
      rotationDirection = rpDown; // counter-clockwise
    }
    else
    {
      //UNKNOWN!
      rotationDirection = 0xFF;
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void  CheckRotationDirectionA() // определяет направление движения энкодера, вызывается для пина А энкодера
{
  noInterrupts();
/*  
  uint8_t aState = digitalRead(ENCODER_PIN1);
  uint8_t bState = digitalRead(ENCODER_PIN2);

  if(aState && bState && aFlag)
  {
    rotationDirection = rpDown;
    bFlag = 0;
    aFlag = 0;
  }
  else if (bState) 
  {
    bFlag = 1;
  }
*/
  aFlag = 1;
  saveTransitionState();

  if(bFlag)
  {
    aFlag = 0;
    bFlag = 0;
    handleDirection();
    transitionState = 0;
  }
  interrupts();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CheckRotationDirectionB() // прерывание на пине В энкодера
{
  noInterrupts();
/*
  uint8_t aState = digitalRead(ENCODER_PIN1);
  uint8_t bState = digitalRead(ENCODER_PIN2);

  if (aState && bState && bFlag) 
  { 
    rotationDirection = rpUp;
    bFlag = 0;
    aFlag = 0;
  }
  else if (aState)
  {
    aFlag = 1;
  }
*/
  bFlag = 1;
  saveTransitionState();

  if(aFlag)
  {
    aFlag = 0;
    bFlag = 0;
    handleDirection();
    transitionState = 0;
  }  
  interrupts();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void EncoderPulsesHandler() // обработчик импульсов энкодера на пине А
{
  CheckRotationDirectionA();
  
  if(paused) // на паузе
  {
    return;
  }

  // тут проверяем, надо ли пропустить N импульсов
  uint32_t toSkip = Settings.getSkipCounter();

  if(toSkip > 1) // каждый первый - пропускать бессмысленно.
  {
      interruptSkipCounter++;
      if(interruptSkipCounter % toSkip)
      {
         // надо пропустить
         return;
      }
      else
      {
        interruptSkipCounter = 0;
      }
  }
  else
  {
    interruptSkipCounter = 0;
  }
  
  #ifdef PREDICT_ENABLED // включены предсказания?
  
  if(predictEnabledFlag && !predictTriggeredFlag) // можем делать предсказания о срабатывании защиты
  {
    // включено предсказание срабатывания защиты по импульсам
    
    if(predictList.size() < PREDICT_PULSES)
    {
      predictList.push_back(micros()); // сохраняем время импульса в нашем списке
    }

    if(predictList.size() >= PREDICT_PULSES) // накопили достаточное количество импульсов
    {
      // список наполнился, можем делать предсказания
      uint32_t first = predictList[0];
      uint32_t last = predictList[predictList.size()-1];
      
      if(last - first <= PREDICT_TIME) // время между крайними импульсами укладывается в настройку
      {
        // предсказание сработало
        predictTriggeredFlag = true;
      }
      else
      {
        // предсказание не сработало, просто чистим список
        predictList.clear();        
      }
    }
  } // predictEnabledFlag
  
  #endif // PREDICT_ENABLED

  if(!canHandleEncoder || InterruptData.size() >= MAX_PULSES_TO_CATCH) // не надо собирать импульсы с энкодера
  {
    return;
  }
  
    uint32_t now = micros();
    InterruptData.push_back(now);
    encoderTimer = now; // обновляем значение времени, когда было последнее срабатывание энкодера  


    #ifndef DISABLE_CATCH_ENCODER_DIRECTION

      if(canCatchInitialRotationDirection)
      {
        uint8_t dir = GetRotationDirection();
        if(dir != 0xFF)
        {
          canCatchInitialRotationDirection = false;
  
          // определяем направление вращения энкодера.
          initialDirection = dir; //digitalRead(ENCODER_PIN2) ? rpUp : rpDown;
          lastKnownDirection = initialDirection;      
          Settings.setRodDirection((RodDirection)initialDirection);
        }
        
      } // canCatchInitialRotationDirection
      else
      {
         // тут проверяем, не изменилось ли направление вращения энкодера?
         uint8_t curDirection = GetRotationDirection(); //digitalRead(ENCODER_PIN2) ? rpUp : rpDown;
         
         if(curDirection != 0xFF && (curDirection != lastKnownDirection) )
         {
           // направление вращения энкодера изменилось, надо сохранить информацию об этом
           lastKnownDirection = curDirection;
           DirectionInfo.add(lastKnownDirection, micros()); 
         }
      }
    #endif
       
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
  InterruptData.reserve(MAX_PULSES_TO_CATCH);


  // настраиваем первый выход энкодера на чтение
/*  
#if (ENCODER_INTERRUPT_LEVEL == RISING)
  pinMode(ENCODER_PIN1, INPUT_PULLUP);
#else
  pinMode(ENCODER_PIN1, INPUT);
#endif
*/

  // настраиваем второй выход энкодера на чтение
 // pinMode(ENCODER_PIN2, INPUT_PULLUP);

  // ждём, пока устаканится питание
  delay(50);
  

  // считаем импульсы на штанге по прерыванию
  attachInterrupt((ENCODER_PIN1),EncoderPulsesHandler, ENCODER_INTERRUPT_LEVEL);
  attachInterrupt((ENCODER_PIN2),CheckRotationDirectionB, ENCODER_INTERRUPT_LEVEL);

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
void InterruptHandlerClass::normalizeList(InterruptTimeList& list, uint32_t dirOffset)
{
  size_t sz = list.size();
  
  if(sz < 1 || !dirOffset)
    return;

  // нормализуем список, отнимая от всех значений указанное

  for(size_t i=0;i<sz;i++)
  {
    list[i] = (list[i] - dirOffset);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
uint32_t InterruptHandlerClass::writeLogRecord(DirectionInfoData& directionData, uint16_t previewCount, int32_t dataArrivedTime, CurrentOscillData* oscData, InterruptTimeList& _list, EthalonCompareResult compareResult
, EthalonCompareNumber num, /*InterruptTimeList& ethalonData*/const String& ethalonFileName, bool toEEPROM, uint32_t curEEPROMWriteAddress)
{

  uint32_t written = 0;

  /*
  if(_list.size() < 2) // ничего в списке прерываний нет
  {
    return written;
  }
*/

 const uint8_t CHANNEL_NUM = 0;
 EEPROM_CLASS* eeprom = Settings.getEEPROM();

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
  RodDirection rodPos = Settings.getRodDirection();

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
  uint32_t moveTime = 0;
  if(_list.size() > 1)
  {
    moveTime = _list[_list.size()-1] - _list[0];
  }
  
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
  uint32_t motoresource = Settings.getMotoresource(); // ВСЕГДА ПЕРВЫЙ КАНАЛ

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

  // пишем количество превью в списках по току
  workBuff[0] = recordPreviewCount;
  memcpy(&(workBuff[1]),&previewCount,2);

  if(toEEPROM)
  {
    eeprom->write(curEEPROMWriteAddress,workBuff,3);
    written += 3;
    curEEPROMWriteAddress += 3;    
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,3);    
    #endif
  }  


  // пишем длину перемещения штанги
  workBuff[0] = recordRodMoveLength;
  uint32_t rml = Settings.getRodMoveLength();
  memcpy(&(workBuff[1]),&rml,4);

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

   if(SD_CARD.exists(ethalonFileName.c_str()))   
   {
      SdFile file;
      file.open(ethalonFileName.c_str(),FILE_READ);
      if(file.isOpen())
      {
        file.rewind();
        uint32_t fSize = file.fileSize();

       // пишем данные эталона, с которым сравнивали
       workBuff[0] = recordEthalonDataFollow;
       uint16_t dataLen = fSize/sizeof(uint32_t);//ethalonData.size();
       memcpy(&(workBuff[1]),&dataLen,2);

       if(toEEPROM)
       {
        eeprom->write(curEEPROMWriteAddress,workBuff,3);
        written += 3;
        curEEPROMWriteAddress += 3;
         while(file.available())
         {
            uint8_t b = file.read();
            eeprom->write(curEEPROMWriteAddress,b);
            written++;
            curEEPROMWriteAddress++;

          //  LastTriggeredInterruptRecord.push_back(b);
         }
        
       } // toEEPROM
       else
       {
        #ifndef _SD_OFF
        Logger.write(workBuff,3);
        //Logger.write((uint8_t*) ethalonData.pData(), ethalonData.size()*sizeof(uint32_t));

        while(file.available())
        {
          uint8_t b = (uint8_t) file.read();
          Logger.write(&b,1);
        }

        #endif
       } // else       
            

        file.close();
      }
   } // if(SD_CARD.exists(ethalonFileName.c_str()))   
  

  // пишем данные по току
  if (oscData->times.size() > 1)
  {
	  workBuff[0] = recordOscDataFollow;
	  uint16_t dataLen = oscData->times.size();
	  memcpy(&(workBuff[1]), &dataLen, 2);

    if(toEEPROM)
    {
    eeprom->write(curEEPROMWriteAddress,workBuff,3);
    written += 3;
    curEEPROMWriteAddress += 3;
 
    eeprom->write(curEEPROMWriteAddress,(uint8_t*) oscData->times.pData(), oscData->times.size()*sizeof(uint32_t));
    written += oscData->times.size()*sizeof(uint32_t);
    curEEPROMWriteAddress += oscData->times.size()*sizeof(uint32_t);

    eeprom->write(curEEPROMWriteAddress,(uint8_t*) oscData->data1.pData(), oscData->data1.size()*sizeof(uint16_t));
    written += oscData->data1.size()*sizeof(uint16_t);
    curEEPROMWriteAddress += oscData->data1.size()*sizeof(uint16_t);


    eeprom->write(curEEPROMWriteAddress,(uint8_t*) oscData->data2.pData(), oscData->data2.size()*sizeof(uint16_t));
    written += oscData->data2.size()*sizeof(uint16_t);
    curEEPROMWriteAddress += oscData->data2.size()*sizeof(uint16_t);

    eeprom->write(curEEPROMWriteAddress,(uint8_t*) oscData->data3.pData(), oscData->data3.size()*sizeof(uint16_t));
    written += oscData->data3.size()*sizeof(uint16_t);
    curEEPROMWriteAddress += oscData->data3.size()*sizeof(uint16_t);
      
    }
    else
    {
      #ifndef _SD_OFF
	    Logger.write(workBuff, 3);
	    Logger.write((uint8_t*)oscData->times.pData(), oscData->times.size() * sizeof(uint32_t));
	    Logger.write((uint8_t*)oscData->data1.pData(), oscData->data1.size() * sizeof(uint16_t));
	    Logger.write((uint8_t*)oscData->data2.pData(), oscData->data2.size() * sizeof(uint16_t));
	    Logger.write((uint8_t*)oscData->data3.pData(), oscData->data3.size() * sizeof(uint16_t));
     #endif
    }
  }


  // пишем данные по изменения направления вращения энкодера
  if (directionData.times.size() > 0)
  {
    workBuff[0] = recordDirectionData;
    uint16_t dataLen = directionData.times.size();
    memcpy(&(workBuff[1]), &dataLen, 2);

    if(toEEPROM)
    {
    eeprom->write(curEEPROMWriteAddress,workBuff,3);
    written += 3;
    curEEPROMWriteAddress += 3;
 
    eeprom->write(curEEPROMWriteAddress,(uint8_t*) directionData.times.pData(), directionData.times.size()*sizeof(uint32_t));
    written += directionData.times.size()*sizeof(uint32_t);
    curEEPROMWriteAddress += directionData.times.size()*sizeof(uint32_t);

    eeprom->write(curEEPROMWriteAddress,(uint8_t*) directionData.directions.pData(), directionData.directions.size()*sizeof(uint8_t));
    written += directionData.directions.size()*sizeof(uint8_t);
    curEEPROMWriteAddress += directionData.directions.size()*sizeof(uint8_t);

    }
    else
    {
      #ifndef _SD_OFF
      Logger.write(workBuff, 3);
      Logger.write((uint8_t*)directionData.times.pData(), directionData.times.size() * sizeof(uint32_t));
      Logger.write((uint8_t*)directionData.directions.pData(), directionData.directions.size() * sizeof(uint8_t));
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
  DirectionInfoData& directionData,
  uint16_t previewCount,
  int32_t dataArrivedTime, 
  DS3231Time& tm,
	CurrentOscillData* oscData,
	InterruptTimeList& lst1, 
	EthalonCompareResult res1, 
	EthalonCompareNumber num1,
	//InterruptTimeList& ethalonData1,
  const String& ethalonFileName,
  bool toEEPROM
)
{
  EEPROM_CLASS* eeprom = Settings.getEEPROM();

  if(toEEPROM && !eeprom)
  {
    return;
  }

  PAUSE_ADC; // останавливаем АЦП

  uint8_t workBuff[10] = {0};
  uint32_t eepromAddress = EEPROM_LAST_3_DATA_ADDRESS;
  uint32_t recordStartAddress = 0;
  uint32_t recordTotalLength = 0;
  
  uint8_t idx = 0;
  

  if(toEEPROM)
  {
//    LastTriggeredInterruptRecord.clear(); // очищаем список срабатывания
    LastTriggeredInterruptRecordIndex = -1;
    
    // вычисляем адрес для записи в EEPROM
    // сначала смотрим, под каким индексом записывать?
    int header1 = eeprom->read(eepromAddress);
    int header2 = eeprom->read(eepromAddress+1);
    int header3 = eeprom->read(eepromAddress+2);
    
    if(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2 && header3 == RECORD_HEADER3)
    {
      // прочитали текущий индекс, и инкрементировали его
      /*
      idx = eeprom->read(eepromAddress+3);
      idx++;
      if(idx > 2) // пишем только последние 3 срабатывания
      {
        idx = 0;
      }

      // записали новый индекс
      eeprom->write(eepromAddress+3,idx);
      */
      idx = 0; //TODO: ПОКА ТОЛЬКО ОДНА ЗАПИСЬ !!!
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

   // LastTriggeredInterruptRecord.push_back(workBuff[0]);
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
/*
    for(uint8_t c=0;c<8;c++)
    {
      LastTriggeredInterruptRecord.push_back(workBuff[c]);
    }
*/    
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
/*
    for(uint8_t c=0;c<3;c++)
    {
      LastTriggeredInterruptRecord.push_back(workBuff[c]);
    }    
*/    
  }
  else
  {
    #ifndef _SD_OFF
    Logger.write(workBuff,3);
    #endif
  }
  
  // теперь смотрим, в каких списках есть данные, и пишем записи в лог
  //if(lst1.size() > 1)
  {
    uint32_t written = writeLogRecord(directionData, previewCount, dataArrivedTime,oscData,lst1,res1,num1, /*ethalonData1*/ ethalonFileName,toEEPROM,eepromAddress);
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
/*
      for(uint8_t c=0;c<1;c++)
      {
        LastTriggeredInterruptRecord.push_back(workBuff[c]);
      }       
*/      
    }
    else
    {
      #ifndef _SD_OFF
      Logger.write(workBuff,1);
      #endif
    }

  if(toEEPROM)
  {
      LastTriggeredInterruptRecordIndex = idx; // сохранили индекс последнего срабатывания до тех пор, пока его данные не запросят извне    
  }

}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::pause()
{
  if(paused) // уже на паузе
  {
    return;
  }

  noInterrupts();
  paused = true;
  interrupts();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::resume()
{
  if(!paused) // не на паузе
  {
    return;
  }
  
  noInterrupts();
  
  // поскольку мы были на паузе - начинаем сначала
  #ifdef PREDICT_ENABLED
    predictEnabledFlag = true; // флаг, что мы можем собирать информацию о предсказаниях срабатывания защиты
    predictList.clear(); // список для предсказаний
    predictTriggeredFlag = false; // флаг срабатывания предсказания
  #endif

  paused = false;
  InterruptData.empty();
  DirectionInfo.clear(); // очищаем список направлений движений
  machineState = msIdle; // состояние конечного автомата
  canHandleEncoder = false; // флаг, что мы можем собирать прерывания с энкодера
  downEndstopTriggered = false; // состояние нижнего концевика на момент срабатывания защиты  
  OscillData.clear();
  interrupts();  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void InterruptHandlerClass::update()
{

  if(paused) // на паузе
  {
    return;
  }

  /*
    Логика следующая: мы постоянно проверяем превышение по току, вход внешней защиты, собираем предсказания.

    Если детектировано одно из вышеперечисленного, то мы:
      1. Сохраняем причину детектирования;
      2. Переключаемся в другую ветку конечного автомата;
      3. В этой ветке смотрим:
      
        3.1. Если причина - превышение порога тока, то:
        
          1. Если внешняя защита не сработала в течение какого-то времени - это авария;
          2. Если импульсов с энкодера не было в течение какого-то времени - это авария;
          
        3.2. Если причина - срабатывание внешней защиты, то:

          1. Если ток не был превышен - это ложное срабатывание защиты;
          2. Если не было импульсов с энкодера в течение какого-то времени - это авария;
          3. Если ток превышен - это авария;

        3.3. Если причина - предсказания (т.е. штанга двигается), то:

          1. Собираем импульсы со штанги;
          2. Как только сбор закончен, то смотрим:
            1. Если несовпадение с эталоном - это авария.
   */

  // проверяем состояние конечного автомата
  switch(machineState)
  {
    case msIdle: // нормальный режим работы, проверяем, что там творится на линиях    
    {
        if(adcSampler.currentPeakDataAvailable())
        {
           // есть данные по значению тока
            uint32_t highBorder = adcSampler.getHighBorder(); // получаем верхний порог по току
            uint16_t currentPeakChannel1 = 0,currentPeakChannel2 = 0,currentPeakChannel3 = 0;
           
            // получаем данные по току
            adcSampler.getCurrentPeakData(currentPeakChannel1,currentPeakChannel2,currentPeakChannel3);


            // проверяем - есть ли превышение по току?
             bool hasCurrentPeakHighBorderAlarm = currentPeakChannel1 >= highBorder || currentPeakChannel2 >= highBorder || currentPeakChannel3 >= highBorder;
/*
              Serial.print("border = ");
              Serial.print(highBorder);
              Serial.print(", current = ");
              Serial.println(currentPeakChannel1);
*/
             if(hasCurrentPeakHighBorderAlarm)
             {
              // есть превышение по току
            //  Serial.print(F("PEAK DETECTED, delay = "));
           //   Serial.println(Settings.getRelayDelay());

                #ifdef PREDICT_ENABLED
                  predictOff(); // отключаем сбор предсказаний
                #endif     

                copyPredictToList(); // копируем предсказания в общий список

                // сохраняем время срабатывания защиты
                relayTriggeredTime = RealtimeClock.getTime();
                 // сохраняем состояние нижнего концевика, с выключением прерываний
                downEndstopTriggered = RodDownEndstopTriggered(true);                

                noInterrupts();
                
                trigReason = REASON_PEAK; // причина срабатывания - превышение по току
                trigReasonTimer = micros(); // запоминаем время срабатывания причины
                
                encoderTimer = micros();
                canHandleEncoder = true; // разрешаем обработчику прерываний энкодера собирать информацию
                canCatchInitialRotationDirection = true; // говорим, чтобы засекли первоначальное направление движения штанги

                machineState = msHandlePeakReason; // переключаемся на ветку обработки причины срабатывания "превышение по току"
                relayTrigCatched = hasRelayTriggered(); // сохраняем флаг срабатывание внешней защиты
                asuTPAlarmFlag = false; // сбрасываем флаг аварии

                 // запрещаем собирать данные превью по току
                adcSampler.stopCollectPreview();
                // и говорим АЦП, чтобы собирало данные по току до момента окончания событий
                adcSampler.startCollectCurrent();

                interrupts();
                
              
             } // if(hasCurrentPeakHighBorderAlarm)

        } // if(adcSampler.currentPeakDataAvailable())
        else
        if(hasRelayTriggered())
        {
        //  Serial.println(F("EXTERNAL SIGNAL DETECTED !!!"));
       
        #ifdef PREDICT_ENABLED
          predictOff(); // отключаем сбор предсказаний
        #endif     

        copyPredictToList(); // копируем предсказания в общий список

        noInterrupts();
        
        trigReason = REASON_RELAY; // причина срабатывания - срабатывание внешней защиты
        trigReasonTimer = micros(); // запоминаем время срабатывания причины
        
        encoderTimer = micros();
        canHandleEncoder = true; // разрешаем обработчику прерываний энкодера собирать информацию
        canCatchInitialRotationDirection = true; // говорим, чтобы засекли первоначальное направление движения штанги

        machineState = msHandleRelayReason; // переключаемся на ветку обработки причины срабатывания "срабатывание внешней защиты"
        relayTrigCatched = true; // сохраняем флаг срабатывание внешней защиты
        asuTPAlarmFlag = false; // сбрасываем флаг аварии
        peakAlarmFlag = false; // сбрасываем флаг детектирования превышения по току

         // запрещаем собирать данные превью по току
        adcSampler.stopCollectPreview();
        // и говорим АЦП, чтобы собирало данные по току до момента окончания событий
        adcSampler.startCollectCurrent();

        interrupts();
          
        } // if(hasRelayTriggered())
        #ifdef PREDICT_ENABLED
         else
         if(predictTriggered()) // сработало предсказание?
         {
      //    Serial.println(F("PREDICT DETECTED !!!"));

          #ifdef PREDICT_ENABLED
            predictOff(); // отключаем сбор предсказаний
          #endif     

        copyPredictToList(); // копируем предсказания в общий список

        // сохраняем время срабатывания защиты
        relayTriggeredTime = RealtimeClock.getTime();
         // сохраняем состояние нижнего концевика, с выключением прерываний
        downEndstopTriggered = RodDownEndstopTriggered(true);        

        noInterrupts();
        
        trigReason = REASON_PREDICT; // причина срабатывания - срабатывание предсказаний
        trigReasonTimer = micros(); // запоминаем время срабатывания причины
        
        encoderTimer = micros();
        canHandleEncoder = true; // разрешаем обработчику прерываний энкодера собирать информацию
        canCatchInitialRotationDirection = true; // говорим, чтобы засекли первоначальное направление движения штанги

        machineState = msWaitForCollectEncoderPulses; // переключаемся на ветку ожидания окончания импульсов с энкодера
        relayTrigCatched = hasRelayTriggered(); // сохраняем флаг срабатывание внешней защиты
        asuTPAlarmFlag = false; // сбрасываем флаг аварии

         // запрещаем собирать данные превью по току
        adcSampler.stopCollectPreview();
        // и говорим АЦП, чтобы собирало данные по току до момента окончания событий
        adcSampler.startCollectCurrent();

        interrupts();
        
         } // if(predictTriggered())
        #endif // #ifdef PREDICT_ENABLED
    }
    break; // msIdle

    case msHandleRelayReason: // ветка обработки причины срабатывания "срабатывание внешней защиты"
    {
        if(adcSampler.currentPeakDataAvailable())
        {
           // есть данные по значению тока
            uint32_t highBorder = adcSampler.getHighBorder(); // получаем верхний порог по току
            uint16_t currentPeakChannel1 = 0,currentPeakChannel2 = 0,currentPeakChannel3 = 0;
           
            // получаем данные по току
            adcSampler.getCurrentPeakData(currentPeakChannel1,currentPeakChannel2,currentPeakChannel3);

            // проверяем - есть ли превышение по току?
             peakAlarmFlag = peakAlarmFlag || (currentPeakChannel1 >= highBorder || currentPeakChannel2 >= highBorder || currentPeakChannel3 >= highBorder); 
             
        } // if(adcSampler.currentPeakDataAvailable())

        if(micros() - trigReasonTimer >= Settings.getRelayDelay())
        {
          // прошло время ожидания, надо анализировать, что произошло.
          // если есть импульсы с энкодера или превышен ток - это наш случай.
          // если же ток не превышен и нет импульсов с энкодера - это ложное срабатывание защиты, переключаемся в ветку ожидания отщёлкивания концевика внешней защиты

          noInterrupts();
            size_t catchedPulses = InterruptData.size();
          interrupts();

          // если ток превышен  - это авария
          asuTPAlarmFlag = peakAlarmFlag;

          // если есть импульсы энкодера - их надо собирать.
          // если нет импульсов энкодера и ток не превышен - это ложное срабатывание защиты
          if(catchedPulses > 0)
          {
            // даём поработать обработчику энкодера некоторое время ещё
            encoderTimer = micros();
            machineState = msWaitForCollectEncoderPulses;                 
          } // if(catchedPulses > 0)
          else
          {
            // импульсов с энкодера нет, надо посмотреть - есть ли превышение тока?
            if(peakAlarmFlag)
            {
                // превышение тока есть, надо подождать - может, пойдут импульсы
                // даём поработать обработчику энкодера некоторое время ещё
                encoderTimer = micros();
                machineState = msWaitForCollectEncoderPulses;                 
            } // if(peakAlarmFlag)
            else
            {
              // превышения тока нет, импульсов с энкодера нет - это ложное срабатывание защиты
              
              noInterrupts();
                canHandleEncoder = false; // выключаем обработку импульсов энкодера
                #ifdef PREDICT_ENABLED
                predictOn(); // включаем сбор предсказаний          
                #endif
              interrupts();

             // переключаемся на ветку ожидания отщёлкивания концевика защиты
              machineState = msWaitGuardRelease;
              
            } // !peakAlarmFlag
             
          } // else
        
        } // if(micros() - trigReasonTimer >= Settings.getRelayDelay())
    }
    break; // msHandleRelayReason

    case msHandlePeakReason: // ветка обработки причины срабатывания "превышение по току"
    {
      relayTrigCatched = relayTrigCatched || hasRelayTriggered(); // сохраняем флаг срабатывание внешней защиты
      
      // проверяем - прошло ли время задержки для проверки срабатывания защиты?
      if(micros() - trigReasonTimer >= Settings.getRelayDelay())
      {
    //    Serial.println("msHandlePeakReason DONE !!!");
        
          noInterrupts();
            size_t catchedPulses = InterruptData.size();
          interrupts();

          // проверяем - сработала ли защита?
          if(!relayTrigCatched)
          {
              // срабатывания внешней защиты не было в течение определённого времени - это авария
              asuTPAlarmFlag = true;              
          } // !relayTrigCatched

          // проверяем - были ли импульсы с энкодера?
          if(catchedPulses < 1)
          {
            // импульсов с энкодера не было - это авария
            asuTPAlarmFlag = true;
            
          } // catchedPulses < 1

          // даём поработать обработчику энкодера некоторое время ещё
          encoderTimer = micros();
          machineState = msWaitForCollectEncoderPulses;
          
      } // if(micros() - trigReasonTimer >= Settings.getRelayDelay())
    }
    break; // msHandlePeakReason

    case msWaitForCollectEncoderPulses: // ждём окончания сбора импульсов с энкодера, в этой ветке идёт запись в лог и выдача событий подписчику
    {
      noInterrupts();      
          uint32_t thisTimer = encoderTimer; // копируем значение времени последнего прерывания с энкодера локально
          size_t catchedPulses = InterruptData.size();
      interrupts();

      if(micros() - thisTimer >= Settings.getMaxIdleTime()) // прошло максимальное время для сбора импульсов, т.е. последний импульс с энкодера был очень давно
      {

      //  Serial.println("START WORK WITH INTERRUPT, STAGE 1!"); Serial.flush();

        PAUSE_ADC; // останавливаем АЦП на время

        pause(); // ставим на паузу
              
        noInterrupts();
          canHandleEncoder = false; // выключаем обработку импульсов энкодера
          #ifdef PREDICT_ENABLED
          predictOn(); // включаем сбор предсказаний          
          #endif
        interrupts(); 
        

 //      Serial.println("STAGE 2"); Serial.flush();

        uint8_t asuTpFlags = Settings.getAsuTpFlags();

        // обновляем моторесурс, т.к. было срабатывание защиты
        uint32_t motoresource = Settings.getMotoresource();
        motoresource++;
        Settings.setMotoresource(motoresource);
        

//        Serial.println("STAGE 3"); Serial.flush();

        // проверяем, авария ли?
        hasAlarm = !catchedPulses || asuTPAlarmFlag;
        
        // выставляем флаг аварии, в зависимости от наличия данных в списках
        if(hasAlarm)
        {
  //        Serial.println("STAGE ALARM"); Serial.flush();
          Feedback.setFailureLineLevel(); // говорим на выходящей линии, что это авария
        }    


//        Serial.println("STAGE 4"); Serial.flush();


        noInterrupts();
        
           // заканчиваем сбор данных по току, копируем данные по току в локальный список
           
            // говорим АЦП прекратить собирать данные по току
            adcSampler.stopCollectCurrent();

           uint16_t previewCount;
           OscillData.clear();
           OscillData = adcSampler.getListOfCurrent(previewCount);//false);

          // разрешаем собирать данные превью по току
          adcSampler.startCollectPreview();
           
        
        interrupts();



  //      Serial.println("STAGE 5"); Serial.flush();
        

        // вычисляем смещение от начала записи по току до начала поступления данных
        
        int32_t datArrivTm = 0;
        if(OscillData.times.size() > 0 && catchedPulses > 0)
        {          
          // высчитываем дату поступления данных
          datArrivTm = 250ul*CURRENT_TIMER_PERIOD * CURRENT_AVG_SAMPLES * previewCount;
        }


//        Serial.println("STAGE 6"); Serial.flush();

        // нормализуем список времен записей по току
        normalizeList(OscillData.times);

        // нормализуем список времён для направлений движения штанги
        uint32_t dirOffset = 0;
        if(InterruptData.size() > 0)
        {
          dirOffset = InterruptData[0];
        }
        normalizeList(DirectionInfo.times, dirOffset);

         // нормализуем список прерываний
         normalizeList(InterruptData);

//         Serial.println("STAGE 7"); Serial.flush();

         // начинаем работать со списком прерываний
         EthalonCompareResult compareRes1 = COMPARE_RESULT_NoSourcePulses;
         EthalonCompareNumber compareNumber1;
         String ethalonFileName;

          bool needToLog = false;

          // зажигаем светодиод "ТЕСТ" (желтый)
          Feedback.testDiode();

        // теперь смотрим - надо ли нам самим чего-то обрабатывать?
        //if(catchedPulses > 1)
        {
//          Serial.println("STAGE TEST DIODE ON"); Serial.flush();
          
//            DBG("Прерывание содержит данные: ");
//            DBGLN(catchedPulses);
    


          if(asuTpFlags & 1) // только если флаг выдачи сигнала в первую линию АСУ ТП - установлен
          {
              // Формируем сигнал срабатывания системы на выводах АСУ ТП 
              // №1 - НО контакта: в схему УРОВ (срабатывание УРОВ) (параллельно желтому светодиоду)
              digitalWrite(out_asu_tp1,asu_tp_level);
          }
    
          needToLog = true; // говорим, что надо записать в лог


 // ЗАКОММЕНТИРОВАЛ АВТОСОХРАНЕНИЕ ЭТАЛОНА
 
          // проверяем, есть ли у нас эталоны на SD? Если нет - создаём их.
          // эталон движения вверх
//        if(!FileUtils::isEthalonExists(0,true))
//          {
//            FileUtils::saveEthalon(0,true,InterruptData);
//          }
          
          // эталон движения вниз
//          if(!FileUtils::isEthalonExists(0,false))
//          {
//            FileUtils::saveEthalon(0,false,InterruptData);
//          }
            
           // здесь мы можем обрабатывать список сами - в нём ЕСТЬ данные
           compareRes1 = EthalonComparer::Compare(InterruptData, 0,compareNumber1, ethalonFileName);//ethalonData1);


 //           Serial.println("STAGE 8"); Serial.flush();
    
           if(compareRes1 == COMPARE_RESULT_MatchEthalon)
            {}
           else if(compareRes1 == COMPARE_RESULT_MismatchEthalon || compareRes1 == COMPARE_RESULT_RodBroken)
           {
//              Serial.println("STAGE ALARM & FAILURE"); Serial.flush();
              Feedback.failureDiode(); // зажигаем светодиод АВАРИЯ
              Feedback.setFailureLineLevel(); // говорим на выходящей линии, что это авария
              
              
           }
        } // if(catchedPulses > 1)

        // проверяем, надо ли выдавать сигнал на линии АСУ ТП?
        if(asuTPAlarmFlag)
        {
              if(asuTpFlags & 4) // только если флаг выдачи сигнала в третью линию АСУ ТП - установлен
              {
                // Формируем сигнал срабатывания системы на выводах АСУ ТП
                // №3 - НО контакт: «неисправность выключателя» (параллельно красному светодиоду. При выходе параметров кривой движения за допустимые границы)
                digitalWrite(out_asu_tp3,asu_tp_level);
              }
          
        }

            if(needToLog)
            { 
              //TODO: ПОКА ТЕСТОВЫЕ ДАННЫЕ ПО ИЗМЕНЕНИЮ НАПРАВЛЕНИЯ ДВИЖЕНИЯ ШТАНГИ, ЗАКОММЕНТИРОВАТЬ!!!
               if(InterruptData.size() > 10)
               {
                  DirectionInfo.clear();
                  
                  uint8_t countOfFakeRecords = 4;
                  size_t offset = InterruptData.size() / countOfFakeRecords; // N записей
                  RodDirection dir = Settings.getRodDirection(); // получили первоначальное направление вращения штанги

                  // последовательно меняем направления на противоположные
                  for(uint8_t i=1;i<=countOfFakeRecords;i++)
                  {
                    if(dir == rpUp)
                    {
                      dir = rpDown;
                    }
                    else
                    {
                      dir = rpUp;
                    }

                    size_t recIdx = i*offset - 1;
                    uint32_t timeVal = InterruptData[recIdx];

                    DirectionInfo.add(dir,timeVal);
                  } // for
                
               }
              // КОНЕЦ ТЕСТОВЫХ ДАННЫХ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              
           //   Serial.println("STAGE WRITE TO LOG BEGIN"); Serial.flush();            
              // записываем последнее срабатывание в EEPROM
              writeToLog(DirectionInfo, previewCount, datArrivTm, relayTriggeredTime, &OscillData,InterruptData, compareRes1, compareNumber1, ethalonFileName,true);
              
              #ifndef _SD_OFF
                  //  DBGLN(F("Надо сохранить в лог, пишем на SD!"));
                  // надо записать в лог дату срабатывания системы
                  writeToLog(DirectionInfo, previewCount, datArrivTm, relayTriggeredTime, &OscillData,InterruptData, compareRes1, compareNumber1, ethalonFileName);
              #endif // !_SD_OFF

//              Serial.println("STAGE WRITE TO LOG END"); Serial.flush();
              
            } // needToLog


        bool wantToInformSubscriber = subscriber != NULL; //(catchedPulses > 1);

        if(wantToInformSubscriber)
        { 
          //  DBGLN(F("Надо уведомить подписчика прерываний!"));
          if(subscriber)
          {
         //   Serial.println("STAGE INFORM SUBSCRIBER BEGIN"); Serial.flush();
            //  DBGLN(F("Подписчик найден!"));  
              
            // уведомляем подписчика
            informSubscriber(&OscillData,compareRes1);

      //      Serial.println("STAGE INFORM SUBSCRIBER END"); Serial.flush();
    
          } // if(subscriber)
          else
          {
//            Serial.println("STAGE RESUME BEGIN 1"); Serial.flush();
            resume(); // подписчика нет, просто начинаем сначала            
//            Serial.println("STAGE RESUME END 1"); Serial.flush();
          }
          
        }   // if(wantToInformSubscriber)
        else
        {
//          Serial.println("STAGE RESUME BEGIN 2"); Serial.flush();
          resume(); // подписчика нет, просто начинаем сначала
//          Serial.println("STAGE RESUME END 2"); Serial.flush();
        }
                

        // переключаемся на ветку ожидания отщёлкивания концевика защиты
        machineState = msWaitGuardRelease;

      }            
    }
    break; // msWaitForCollectEncoderPulses

    case msWaitGuardRelease:
    {
      // ждём отщёлкивания концевика защит
      if(!hasRelayTriggered())
      {
     //   Serial.println("GUARD RELEASED!");
         // концевик разомкнут, переходим в режим ожидания срабатывания защиты
         relayTrigCatched = false; // сбрасываем флаг срабатывания реле

         lastPeakDetectedTimer = micros();
         peakAlarmFlag = false;
         machineState = msWaitForNotCurrentPeaks;
      }
      
    }
    break; // msWaitGuardRelease

    case msWaitForNotCurrentPeaks: // ждём, когда прекратится превыщение по току
    {
        if(adcSampler.currentPeakDataAvailable())
        {
          // есть данные по значению тока
            uint32_t highBorder = adcSampler.getHighBorder(); // получаем верхний порог по току
            uint16_t currentPeakChannel1 = 0,currentPeakChannel2 = 0,currentPeakChannel3 = 0;

           noInterrupts();
            // получаем данные по току
            adcSampler.getCurrentPeakData(currentPeakChannel1,currentPeakChannel2,currentPeakChannel3);
            interrupts();

            // проверяем - есть ли превышение по току?
             peakAlarmFlag  = (currentPeakChannel1 >= highBorder || currentPeakChannel2 >= highBorder || currentPeakChannel3 >= highBorder);

             if(peakAlarmFlag)
             {
              lastPeakDetectedTimer = micros();
             }
             
        } // if(adcSampler.currentPeakDataAvailable())

        if(micros() - lastPeakDetectedTimer >= 1000000ul) // ждём секунду после последнего детектированного пика по току, и если его нету - переключаемся на нормальный режим работы
        {
  //        Serial.println("No peaks over 1 second, switch to normal work mode.");
          machineState = msIdle;
        }
    }
    break; // msWaitForNotCurrentPeaks

    
  } // switch(machineState)
  
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
bool InterruptHandlerClass::informSubscriber(CurrentOscillData* oscData, EthalonCompareResult compareResult)
{
	if (subscriber)
	{
		//DBGLN(F("Subscriber exists!"));

    // сообщаем обработчику, что данные по срабатыванию есть
		subscriber->OnInterruptRaised(oscData, compareResult);

		//DBGLN(F("Subscriber informed!"));
   return true;
	}

 return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------

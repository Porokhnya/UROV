//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
 STM32F103CBT6
 */
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ВСЕ НАСТРОЙКИ - В CONFIG.h !!!!
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "CONFIG.h"
#include "RS485.h"
#include "TinyVector.h"
#include "Endstop.h"
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  msIdle, // нормальный режим работы
  msWaitHandleInterrupts, // ждём до начала сбора прерываний
  msHandleInterrupts, // собираем прерывания
  msWaitGuardRelease, // ждём, пока концевик срабатывания защиты не переключится в разомкнутое состояние
  
} MachineState;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<uint32_t> DWordVector;
typedef Vector<uint8_t> ByteVector;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RS485 rs485(RS485_SERIAL,RS485_DE_PIN,RS485_READING_TIMEOUT); // класс работы с RS-485
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MachineState machineState = msIdle; // состояние конечного автомата
volatile uint32_t timer = 0; // служебный таймер

uint32_t pingID = 0; // ID пакета пинга
uint32_t lastPacketSentAt = 0; // когда был послан последний пакет?

volatile bool canHandleEncoder = false; // флаг, что мы можем собирать прерывания с энкодера

bool waitForACK = false;
uint32_t ackStartTime = 0;
uint8_t retransmitAttempts = 0;
RS485PacketType queuePacketType;

DWordVector encoderList; // список прерываний с энкодера
ByteVector rs485DataPacket, rs485QueuePacket; // список данных, который мы отправляем по RS-485
  
// наши концевики
const uint32_t minInterval = 1000000.0 / (1.*(ENDSTOP_FREQUENCY - ENDSTOP_HISTERESIS));
const uint32_t maxInterval = 1000000.0 / (1.*(ENDSTOP_FREQUENCY + ENDSTOP_HISTERESIS));  
Endstop  endstopUp(ENDSTOP_UP_PIN, ENDSTOP_TRIGGER_LEVEL,minInterval,maxInterval);
Endstop  endstopDown(ENDSTOP_DOWN_PIN, ENDSTOP_TRIGGER_LEVEL,minInterval,maxInterval);
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_TEST_INDICATION
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool indicationActive = false;
uint32_t indicationTimer = 0;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void indicate()
{
  if(indicationActive)
  {
    return;
  }
  digitalWrite(INDICATION_PIN,INDICATION_ON_LEVEL);
  indicationActive = true;
  indicationTimer = millis();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void updateIndicate()
{
  if(!indicationActive)
  {
    return;
  }

  if(millis() - indicationTimer >= INDICATION_DURATION)
  {
    indicationActive = false;
    digitalWrite(INDICATION_PIN,!(INDICATION_ON_LEVEL));
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // ENABLE_TEST_INDICATION
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EncoderPulsesHandler() // обработчик импульсов энкодера
{
  
  if(!canHandleEncoder) // не надо собирать импульсы с энкодера
  {
    return;
  }
  
    uint32_t now = micros();
    encoderList.push_back(now);
    timer = now; // обновляем значение таймера
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void createRS485Packet(DWordVector& list, ByteVector& resultPacket) // создаём пакет для отправки по RS-485
{
  resultPacket.empty();

  // у нас длина пакета в байтах - кол-во прерываний*4 + два байта под состояние верхнего и нижнего концевика
  resultPacket.reserve(list.size()*sizeof(uint32_t) + 2);

  //тут заполняем пакет данными по концевикам
  
  // первый байт в пакете - состояние верхнего концевика на момент окончания сбора данных
  bool eUpTrig = endstopUp.isTriggered();
  resultPacket.push_back(eUpTrig);

  // второй байт в пакете - состояние нижнего концевика на момент окончания сбора данных
  bool eDownTrig = endstopDown.isTriggered();
  resultPacket.push_back(eDownTrig);

  //DBG(F("UP ENDSTOP STATE: "));
  //DBGLN(eUpTrig);

  //DBG(F("DOWN ENDSTOP STATE: "));
  //DBGLN(eDownTrig);
  
  // заполняем пакет данными по прерываниям, копируем их побайтово
  for(size_t i=0;i<list.size();i++)
  {
    uint8_t* dt = (uint8_t*) &(list[i]);
    
    for(size_t k=0;k<sizeof(uint32_t);k++)
    {
        resultPacket.push_back(*dt++);
    }
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void normalizeList(DWordVector& list)
{
  size_t sz = list.size();
  
  if(sz < 2)
  {
    return;
  }

  // нормализуем список относительно первого значения
  uint32_t first = list[0];
  list[0] = 0;

  for(size_t i=1;i<sz;i++)
  {
    list[i] = (list[i] - first);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ON_RS485_INCOMING_DATA(RS485* Sender) // событие - получены входящие данные по RS-475
{
    // получили входящий пакет по RS-485
    uint8_t* data;
    RS485Packet packet = rs485.getDataReceived(data);

/*    

    DBG(F("INCOMING RS-475 PACKET CATCHED, TYPE="));
    DBG(packet.packetType);
    DBG(F(", DATA LENGTH="));
    DBGLN(packet.dataLength);
*/

  if(packet.packetType == rs485ACK) // пришёл ответ о получении пакета
  {
    waitForACK = false;       // сбрасываем флаг ожидания ответа
    rs485QueuePacket.empty();  // освобождаем пакет в очереди
  }

  rs485.clearReceivedData(); // очищаем за собой
    
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool hasRelayTriggered()
{
  return digitalRead(RELAY_PIN) == RELAY_TRIGGER_LEVEL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void updateEndstops()
{
  endstopUp.update();
  endstopDown.update();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sendPingPacket()
{
    if(millis() - lastPacketSentAt >= RS485_PING_PACKET_FREQUENCY)
    {
      DBGLN(F("SEND PING PACKET!"));
      
      // пришла пора отсылать пакет пинга контроллеру
      pingID++;
      rs485.send(rs485Ping,(const uint8_t*)&pingID, sizeof(pingID));
      lastPacketSentAt = millis();

      #ifdef ENABLE_TEST_INDICATION
          indicate();
      #endif      
    }  
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  #ifdef _DEBUG
    DEBUG_SERIAL.begin(SERIAL_SPEED);
  #endif

  // настраиваем светодиод индикации
  #ifdef ENABLE_TEST_INDICATION
    pinMode(INDICATION_PIN,OUTPUT);
    digitalWrite(INDICATION_PIN,!(INDICATION_ON_LEVEL));
  #endif

  // настраиваем вход защиты
  pinMode(RELAY_PIN,
  #if (RELAY_TRIGGER_LEVEL == LOW)
    INPUT_PULLUP
  #else
    INPUT
  #endif
  );  

  // резервируем память
  encoderList.reserve(MAX_PULSES_TO_CATCH);

  // настраиваем концевики
  DBGLN(F("Setup endstops..."));
  endstopUp.begin();
  endstopDown.begin();

  // настраиваем RS-485
  RS485_SERIAL.begin(SERIAL_SPEED);
  rs485.begin();

  // считаем импульсы на штанге по прерыванию
  attachInterrupt(ENCODER_PIN1,EncoderPulsesHandler, ENCODER_INTERRUPT_LEVEL);  

  DBGLN(F("Ready."));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{

  #ifdef ENABLE_TEST_INDICATION
    updateIndicate();
  #endif

  // проверяем, пришёл ли пакет подтверждения?
  if(waitForACK)
  {
      if(retransmitAttempts <= RETRANSMITS_COUNT)
      {
          if(millis() - ackStartTime >= ACK_PACKET_WAIT_TIME)
          {
              if(rs485QueuePacket.size()) // есть пакет к отсылу
              {
                retransmitAttempts++;
                
                rs485.send(queuePacketType,(const uint8_t*)rs485QueuePacket.pData(),rs485QueuePacket.size());
                
                ackStartTime = millis();    // запоминаем, когда мы отослали пакет, и через сколько максимум надо попробовать его переслать повторно                
                lastPacketSentAt = millis(); // запоминаем время отсыла последнего пакета

                #ifdef ENABLE_TEST_INDICATION
                    indicate();
                #endif                
              }
              else
              {
                waitForACK = false; // пустой пакет к отсылу
              }
          }
      }
      else
      {
        // закончились попытки переслать пакет
        waitForACK = false;
      }
  }

  // проверяем состояние конечного автомата
  switch(machineState)
  {
    case msIdle:
    {
      // в режиме ожидания, проверяем, не сработало ли реле защиты?
      if(hasRelayTriggered())
      {

        timer = micros(); // запоминаем время срабатывания реле защиты

        DBGLN(F("RELAY TRIGGERED, WAIT FOR PULSES BEGIN..."));
        
        // реле защиты сработало !!!
        // посылаем пакет с сообщением "ЕСТЬ СРАБАТЫВАНИЕ ЗАЩИТЫ" контроллеру        
        pingID++;
        rs485.send(rs485HasInterrupt,(const uint8_t*)&pingID, sizeof(pingID));
        lastPacketSentAt = millis();
                 
        //переключаемся на ветку сбора данных по прерываниям, с энкодера
        machineState = msWaitHandleInterrupts;

        #ifdef ENABLE_TEST_INDICATION
            indicate();
        #endif        
      }
      else
      {
        // реле защиты не сработало, отсылаем пакет пинга
        sendPingPacket();
      }
    }
    break; // msIdle

    case msWaitHandleInterrupts:
    {
      // ждём начала импульсов с энкодера
      if(micros() - timer >= INTERRUPT_BEGIN_DELAY)
      {
        DBGLN(F("WAIT DONE, COLLECT ENCODER PULSES..."));
                
        noInterrupts();
          encoderList.empty(); // очищаем список прерываний
          timer = micros();
          canHandleEncoder = true; // разрешаем обработчику прерываний энкодера собирать информацию
          machineState = msHandleInterrupts; // можем собирать прерывания с энкодера
        interrupts(); 

        ///////////////////////////////////////////////////
        // считаем импульсы на штанге по прерыванию
        //attachInterrupt(ENCODER_PIN1,EncoderPulsesHandler, ENCODER_INTERRUPT_LEVEL);  
        ///////////////////////////////////////////////////
           
      }
    }
    break; // msWaitHandleInterrupts

    case msHandleInterrupts:
    {
      // собираем прерывания с энкодера
      uint32_t nowMicros = micros();
      
      noInterrupts();
      
          uint32_t thisTimer = timer; // копируем значение времени последнего прерывания с энкодера локально
         // size_t catchedPulses = encoderList.size(); // сколько импульсов уже поймали?
          bool isCollectDone = (nowMicros - thisTimer >= INTERRUPT_MAX_IDLE_TIME);// || catchedPulses >= MAX_PULSES_TO_CATCH);
          if(isCollectDone)
          {
            canHandleEncoder = false; // выключаем обработку испульсов энкодера
          }
      interrupts();
      
      if(isCollectDone)
      {        
         ///////////////////////////////////////////////////
        // detachInterrupt(ENCODER_PIN1); // снимаем прерывание с пина энкодера
         ///////////////////////////////////////////////////

        DBG(F("INTERRUPT DONE, CATCHED PULSES: "));
        DBGLN(encoderList.size());
        
        // нормализуем список
        normalizeList(encoderList);


        // готовим данные для отсыла по RS-475
        createRS485Packet(encoderList,rs485DataPacket);

        // отправляем пакет с данными по прерыванию
        rs485.send(rs485InterruptData,(const uint8_t*)rs485DataPacket.pData(),rs485DataPacket.size());

        // говорим, что мы ждём подтверждения получения пакета с данными
        waitForACK = true;

        // копируем отсылаемый пакет в очередь
        rs485QueuePacket = rs485DataPacket;
        queuePacketType = rs485InterruptData;

        // запоминаем, сколько у нас попыток ретрансмита, и когда мы его начали
        ackStartTime = millis();
        retransmitAttempts = 0;

        // чистим локальный список после отсыла данных
        rs485DataPacket.empty(); 

        // не забываем обновить время отсыла крайнего пакета
        lastPacketSentAt = millis();

        // переключаемся на ветку ожидания отщёлкивания концевика защиты
        machineState = msWaitGuardRelease;


        #ifdef ENABLE_TEST_INDICATION
            indicate();
        #endif        
      }

    }
    break; // msHandleInterrupts

    case msWaitGuardRelease:
    {
      // ждём отщёлкивания концевика защиты
      if(!hasRelayTriggered())
      {
         // концевик разомкнут, переходим в режим ожидания срабатывания защиты
         machineState = msIdle;
      }
      else
      {
        sendPingPacket(); // отсылаем пакет пинга
      }
      
    } // msWaitGuardRelease
    break;
    
  } // switch


  // обновляем концевики
  updateEndstops();


  // обновляем RS-485
  rs485.update();
 
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void yield()
{
  // обновляем концевики
  updateEndstops();
  
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

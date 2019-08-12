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

DWordVector encoderList; // список прерываний с энкодера
ByteVector rs485DataPacket; // список данных, который мы отправляем по RS-485
  
// наши концевики
const uint32_t minInterval = 1000000.0 / (1.*(ENDSTOP_FREQUENCY - ENDSTOP_HISTERESIS));
const uint32_t maxInterval = 1000000.0 / (1.*(ENDSTOP_FREQUENCY + ENDSTOP_HISTERESIS));  
Endstop  endstopUp(ENDSTOP_UP_PIN, ENDSTOP_TRIGGER_LEVEL,minInterval,maxInterval);
Endstop  endstopDown(ENDSTOP_DOWN_PIN, ENDSTOP_TRIGGER_LEVEL,minInterval,maxInterval);
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
    return;

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
/*    
    uint8_t* data;
    RS485Packet packet = rs485.getDataReceived(data);


    DBG(F("INCOMING RS-475 PACKET CATCHED, TYPE="));
    DBG(packet.packetType);
    DBG(F(", DATA LENGTH="));
    DBGLN(packet.dataLength);
*/

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
void setup() 
{
  #ifdef _DEBUG
    DEBUG_SERIAL.begin(SERIAL_SPEED);
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
  encoderList.reserve(INTERRUPT_RESERVE_RECORDS);

  // настраиваем концевики
  DBGLN(F("Setup endstops..."));
  endstopUp.begin();
  endstopDown.begin();

  // настраиваем RS-485
  RS485_SERIAL.begin(SERIAL_SPEED);
  rs485.begin();

  // считаем импульсы на штанге по прерыванию
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN1),EncoderPulsesHandler, ENCODER_INTERRUPT_LEVEL);  

  DBGLN(F("Ready."));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{

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
      }
      else
      {
        // реле защиты не сработало, тут можем проверять - как давно мы посылали пакет пинга контроллеру?
        if(millis() - lastPacketSentAt >= RS485_PING_PACKET_FREQUENCY)
        {
          DBGLN(F("SEND PING PACKET!"));
          
          // пришла пора отсылать пакет пинга контроллеру
          pingID++;
          rs485.send(rs485Ping,(const uint8_t*)&pingID, sizeof(pingID));
          lastPacketSentAt = millis();
        }
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
          canHandleEncoder = true;
          machineState = msHandleInterrupts; // можем собирать прерывания с энкодера
        interrupts();       
      }
    }
    break; // msWaitHandleInterrupts

    case msHandleInterrupts:
    {
      // собираем прерывания с энкодера
      noInterrupts();
      uint32_t thisTimer = timer; // копируем значение времени последнего прерывания с энкодера локально
      interrupts();
      
      if(micros() - thisTimer >= INTERRUPT_MAX_IDLE_TIME)
      {
        noInterrupts();
          canHandleEncoder = false; // выключаем обработку испульсов энкодера
        interrupts();

        DBG(F("INTERRUPT DONE, CATCHED PULSES: "));
        DBGLN(encoderList.size());
        
        // нормализуем список
        normalizeList(encoderList);

        /*
        #ifdef _DEBUG
          // выводим для теста данные в Serial
          DBGLN(F("<<<< LIST BEGIN"));
          for(size_t i=0;i<encoderList.size();i++)
          {
            DBGLN(encoderList[i]);
          }
          DBGLN(F("<<<< LIST END"));
        #endif
        */

        // готовим данные для отсыла по RS-475
        createRS485Packet(encoderList,rs485DataPacket);

        // отправляем пакет с данными по прерыванию
        rs485.send(rs485InterruptData,(const uint8_t*)rs485DataPacket.pData(),rs485DataPacket.size());

        // чистим локальный список после отсыла данных
        rs485DataPacket.empty(); 

        // не забываем обновить время отсыла крайнего пакета
        lastPacketSentAt = millis();

        // переключаемся на ветку ожидания отщёлкивания концевика защиты
        machineState = msWaitGuardRelease;
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

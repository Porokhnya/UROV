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
  msIdle,
  msWaitHandleInterrupts,
  msHandleInterrupts,
  
} MachineState;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RS485 rs485(RS485_SERIAL,RS485_DE_PIN,RS485_READING_TIMEOUT);
bool hasIncomingRS485Data = false; // флаг, что мы получили входящий запрос по RS-485
bool hasGuardTriggered = false; // флаг, что у нас сработала защита
MachineState machineState = msIdle; // состояние конечного автомата
volatile uint32_t timer = 0; // служебный таймер
volatile bool canHandleEncoder = false; // флаг, что мы можем собирать прерывания с энкодера
typedef Vector<uint32_t> InterruptTimeList;
InterruptTimeList encoderList; // список прерываний с энкодера
Vector<uint8_t> rs485DataPacket; // список данных, который мы отправляем по RS-485
  
InterruptTimeList fakeList; // тестовый список прерываний
Vector<uint8_t> fakePacket; // тестовый пакет для RS-485


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
void createRS485Packet(InterruptTimeList& list, Vector<uint8_t>& resultPacket)
{
  resultPacket.empty();
  resultPacket.reserve(list.size()*sizeof(uint32_t) + 3);

  // первый байт в пакете - признак того, что у нас срабатывала защита
  resultPacket.push_back(hasGuardTriggered);

  //тут заполняем пакет данными по концевикам
  
  // второй байт в пакете - состояние верхнего концевика на момент окончания сбора данных
  bool eUpTrig = endstopUp.isTriggered();
  resultPacket.push_back(eUpTrig);

  // третий байт в пакете - состояние нижнего концевика на момент окончания сбора данных
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
void normalizeList(InterruptTimeList& list)
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
    hasIncomingRS485Data = true; // взводим флаг
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void handleRS485Packet(const RS485Packet& packet, const uint8_t* data) // обрабатываем входящий пакет, в зависимости от его типа
{
    // тут работаем с пакетом в зависимости от его типа
    
    switch(packet.packetType)
    {
      case rs485Ping: // пакет пинга
      {
        // запрос пинга, в data - 4 байта ID пакета пинга, отправляем их назад, плюс сообщаем, есть ли у нас данные по прерываниям
        uint32_t pingID;
        memcpy(&pingID,data,sizeof(pingID));

     //   DBG(F("PING PACKET RECEIVED, ID="));
     //   DBGLN(pingID);

        RS485PongPacket answer;
        answer.pingID = pingID;
        answer.hasGuardTriggered = hasGuardTriggered;
        rs485.send(rs485Pong,(const uint8_t*)&answer,sizeof(answer));

    //    DBGLN(F("PING ANSWER WAS SENT."));

        // данные отослали, в следующий раз мастер, если у нас есть данные по прерываниям - попросит нас отдать ему список прерываний
      }
      break; // rs485Ping

      case rs485InterruptDataRequest: // мастер запросил данные по прерыванию
      {
       // DBGLN(F("MASTER ASKS FOR DATA..."));
        
          // отправляем пакет с данными по прерыванию
          rs485.send(rs485InterruptDataAnswer,(const uint8_t*)rs485DataPacket.pData(),rs485DataPacket.size());

          hasGuardTriggered = false; // сбрасываем флаг, что зажита сработала
          // чистим локальный список после отсыла данных
          rs485DataPacket.empty();

         // DBGLN(F("INTERRUPT DATA WAS SENT."));
      }
      break; // rs485InterruptDataRequest

      case rs485TestInterrupt:
      {
        // мастер попросил отправить тестовый массив с данными
        rs485.send(rs485InterruptDataAnswer,(const uint8_t*)fakePacket.pData(),fakePacket.size());        
      }
      break; // rs485TestInterrupt
    } // switch 
  
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
  

  // создаём тестовый массив данных
  
  const int sz = 50;
  fakeList.reserve(sz);
  
  for(int k=0;k<sz;k++)
    fakeList.push_back(k*10);
    
  createRS485Packet(fakeList,fakePacket);
  fakeList.clear();
  
  // конец создания тестового массива данных

  DBGLN(F("Ready."));
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{

  switch(machineState)
  {
    case msIdle:
    {
      // в режиме ожидания, проверяем, не сработало ли реле?
      if(hasRelayTriggered())
      {
        DBGLN(F("RELAY TRIGGERED, WAIT FOR PULSES BEGIN..."));
        // реле защиты сработало, переключаемся на ветку сбора данных по прерываниям, с энкодера
        timer = micros();
        machineState = msWaitHandleInterrupts;
      }
    }
    break; // msIdle

    case msWaitHandleInterrupts:
    {
      // ждём начала импульсов с энкодера
      if(micros() - timer > INTERRUPT_BEGIN_DELAY)
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
      
      if(micros() - thisTimer > INTERRUPT_MAX_IDLE_TIME)
      {
        noInterrupts();
          canHandleEncoder = false; // выключаем обработку испульсов энкодера
        interrupts();

        DBGLN(F("INTERRUPT DONE!!!"));

        // нормализуем список
        normalizeList(encoderList);

        // готовим данные для отсыла по RS-475
        createRS485Packet(encoderList,rs485DataPacket);

        DBG(F("CATCHED PULSES: "));
        DBGLN(encoderList.size());

        #ifdef _DEBUG
          // выводим для теста данные в Serial
          DBGLN(F("<<<< LIST BEGIN"));
          for(size_t i=0;i<encoderList.size();i++)
          {
            DBGLN(encoderList[i]);
          }
          DBGLN(F("<<<< LIST END"));
        #endif

        // взводим флаг, что сработала защита
        hasGuardTriggered = true;

        // переключаемся на ветку ожидания новых прерываний
        machineState = msIdle;
      }

    }
    break; // msHandleInterrupts
    
  } // switch


  // обновляем концевики
  updateEndstops();

  
  rs485.update(); // обновляем RS-485

  if(hasIncomingRS485Data)
  {
    
    hasIncomingRS485Data = false;
    
    // получили входящий пакет по RS-485
    uint8_t* data;
    RS485Packet packet = rs485.getDataReceived(data);

    DBG(F("INCOMING RS-475 PACKET CATCHED, TYPE="));
    DBG(packet.packetType);
    DBG(F(", DATA LENGTH="));
    DBGLN(packet.dataLength);

    handleRS485Packet(packet,data); // обрабатываем входящий пакет
    
  } // hasIncomingRS485Data
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void yield()
{
  // обновляем концевики
  updateEndstops();
  
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

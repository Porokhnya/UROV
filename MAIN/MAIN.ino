#include <Arduino.h>
#include "CONFIG.h"
#include "UTFTMenu.h"
#include "DS3231.h"               // подключаем часы
#include "ConfigPin.h"
#include "AT24CX.h"
#include "InterruptHandler.h"
#include "DelayedEvents.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// подключаем наши экраны
#include "Screen1.h"              // Главный экран
#include "Screen2.h"              // Вызов меню настроек
#include "Screen3.h"              //
#include "Screen4.h"              // Вызов меню установки времени и даты
#include "Screen5.h"              // Вызов установки времени
#include "Screen6.h"              // Вызов установки даты
#include "InterruptScreen.h"      // экран с графиком прерывания
#include "Buttons.h"              // наши железные кнопки
#include "Feedback.h"             // обратная связь (диоды и прочее)
#include "FileUtils.h"
#include "Settings.h"
#include "CoreCommandBuffer.h"
#include <Wire.h>
#include "Endstops.h"
#include "RS485.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t screenIdleTimer = 0;
bool setupDone = false;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void screenAction(AbstractTFTScreen* screen)
{
	// какое-то действие на экране произошло.
	// тут просто сбрасываем таймер ничегонеделанья.
	screenIdleTimer = millis();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// RS-485
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RS485 rs485(RS485_SERIAL,Upr_RS485,RS485_READING_TIMEOUT);
uint32_t rs485WaitTimer = 0;
typedef enum
{
  rs485Normal,
  rs485WaitAnswer,
  rs485HandlePacket,
  
} RS485State;
RS485State rs485State = rs485Normal;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ON_RS485_INCOMING_DATA(RS485* Sender)
{
  rs485State =  rs485HandlePacket;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void processRS485()
{
  rs485.update();

  switch(rs485State)
  {
    case rs485Normal: // нормальный режим работы, надо отослать пакет пинга
    {
        if(millis() - rs485WaitTimer >= RS485_PING_FREQ)
        {
          static uint32_t pingID = 0;
          ++pingID;

          rs485.send(rs485Ping,(const uint8_t*)&pingID,sizeof(pingID));
          
          rs485State = rs485WaitAnswer;
          rs485WaitTimer = millis();
        }
    }
    break; // rs485Normal

    case rs485WaitAnswer: // ждём ответа
    {
      if(millis() - rs485WaitTimer >= RS485_ANSWER_TIMEOUT)
      {
          //TODO: ответа не получили !!!
          DBGLN(F("[RS-485] NO ANSWER FROM MODULE !!!"));

          rs485WaitTimer = millis();
          rs485State = rs485Normal;
      }
    }
    break; // rs485WaitAnswer

    case rs485HandlePacket:
    {
        // надо обработать пакет
        uint8_t* data;
        RS485Packet packet = rs485.getDataReceived(data);

        switch(packet.packetType)
        {
            case rs485Pong:
            {
				DBGLN(F("[RS-485] RECEIVE PONT PACKET FROM MODULE!"));
					
              // это ответ от модуля - есть ли прерывания?

              RS485PongPacket* pp = (RS485PongPacket*) data;

              if(pp->hasGuardTriggered)
              {
                DBGLN(F("[RS-485] GUARD TRIGGERED, WANT TO GET INTERRUPT DATA!!!!"));

                // посылаем запрос на данные прерывания
                uint8_t dummy;
                rs485.send(rs485InterruptDataRequest,(const uint8_t*)&dummy,sizeof(dummy));
                rs485WaitTimer = millis();
                rs485State = rs485WaitAnswer;
              }
			  else
			  {
				  // переключаемся на нормальный режим работы
				  rs485WaitTimer = millis();
				  rs485State = rs485Normal;
			  }
            }
            break; // rs485Pong

            case rs485InterruptDataAnswer:
            {
                // пришёл ответ на запрос данных прерывания
				DBGLN(F("[RS-485] RECEIVE INTERRUPT DATA FROM MODULE!!!"));

               // парсим пакет
               // первый байт в пакете - признак того, что у нас срабатывала защита
               bool hasGuardTriggered = *data++;
               if(hasGuardTriggered)
               {
                  // второй байт в пакете - состояние верхнего концевика на момент окончания сбора данных
                  bool endstopUpTriggered = *data++;

                  // третий байт в пакете - состояние нижнего концевика на момент окончания сбора данных
                  bool endstopDownTriggered = *data++;

                  // потом идёт список прерываний
                  // три байта - заголовок, потом N записей по 4 байта. Высчитываем это из длины пакета в байтах
                  uint16_t recordsCount = (packet.dataLength - 3)/sizeof(uint32_t);
                  uint32_t* rec = (uint32_t*) data;

                  DBG(F("[RS-485] INTERRUPTS COUNT: "));
                  DBGLN(recordsCount);

                  // сохраняем записи
                  Vector<uint32_t> interruptsList;
                  interruptsList.reserve(recordsCount);

                  for(uint16_t i=0;i<recordsCount;i++)
                  {
                      interruptsList.push_back(*rec++);
                  }

                  // выводим их для теста
                  #ifdef _DEBUG
                    DBGLN(F("-----INTERRUPTS LIST FROM MODULE -----"));
                    for(size_t i=0;i<interruptsList.size();i++)
                    {
                      DBGLN(interruptsList[i]);
                    }
                    DBGLN(F("-----INTERRUPTS LIST END -----"));
                  #endif

                  //TODO: ТУТ ЧТО-ТО ДЕЛАЕМ СО СПИСКОМ ПРЕРЫВАНИЙ !!!
                
               } // if(hasGuardTriggered)

               // переключаемся на нормальный режим работы
               rs485WaitTimer = millis();
               rs485State = rs485Normal;
            }
            break; // rs485InterruptDataAnswer

			default:
			{
				DBG(F("[RS-485] UNKNOWN PACKET TYPE: "));
				DBGLN(packet.packetType);

				// переключаемся на нормальный режим работы
				rs485WaitTimer = millis();
				rs485State = rs485Normal;
			}
			break;
          
        } // switch
    }
    break; // rs485HandlePacket


  } // switch
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(SERIAL_SPEED);
  while(!Serial);

  DBGLN(F("Init I2C..."));
  // на первом I2C у нас память висит
  Wire.begin();
  Wire.setClock(I2C_SPEED);

  // на втором I2C могут висеть часы
  #if DS3231_WIRE_NUMBER == 1
  Wire1.begin();
  Wire1.setClock(I2C_SPEED);
  #endif
  
  ConfigPin::setI2CPriority(5);
  DBGLN(F("I2C inited."));

  NVIC_SetPriorityGrouping(NVIC_PriorityGroup_1);
  Serial.setInterruptPriority(2);

  ConfigPin::setup();
  
  DBGLN(F("Init settings..."));
  Settings.begin();
  DBGLN(F("Settings inited."));
  
  DBGLN(F("Init RTC..."));
  RealtimeClock.begin(DS3231_WIRE_NUMBER);           // запускаем их на шине I2C 1 (SDA1, SCL1);
 // RealtimeClock.setTime(0,1,11,1,7,2,2018);


  DBGLN(F("Init endstops..."));
  SetupEndstops();

  // инициализируем RS-485
  RS485_SERIAL.begin(RS485_SPEED);
  rs485.begin();

#ifndef _SD_OFF

  DBGLN(F("INIT SD..."));
  if (SDInit::InitSD())
  {
	  DBGLN(F("SD inited."));
  }
  else
  {
	  DBGLN(F("ОШИБКА ИНИЦИАЛИЗАЦИИ SD!!"));
  }
#endif // !_SD_OFF
  

  DBGLN(F("Init screen..."));
  Screen.setup();

  DBGLN(F("Add screen1...")); 
  Screen.addScreen(Screen1::create());           // первый экран покажется по умолчанию

#ifndef _SCREEN_2_OFF
  DBGLN(F("Add screen2..."));
  // добавляем второй экран
  Screen.addScreen(Screen2::create());
#endif // !_SCREEN_2_OFF

#ifndef _SCREEN_3_OFF
  DBGLN(F("Add screen3..."));
  // добавляем третий экран. Переход в меню настройки
  Screen.addScreen(Screen3::create());
#endif // !_SCREEN_3_OFF

#ifndef _SCREEN_4_OFF
  DBGLN(F("Add screen4..."));
  // добавляем четвертый экран. Меню установки даты и времени
  Screen.addScreen(Screen4::create());
#endif // !_SCREEN_4_OFF

#ifndef _SCREEN_5_OFF
  DBGLN(F("Add screen5..."));
  // добавляем 5 экран. Установка времени
  Screen.addScreen(Screen5::create());
#endif // !_SCREEN_5_OFF

#ifndef _SCREEN_6_OFF
  DBGLN(F("Add screen6..."));
  // добавляем 6 экран. Установка даты
  Screen.addScreen(Screen6::create());
#endif // !_SCREEN_6_OFF

  DBGLN(F("Add interrupt screen..."));
  // добавляем экран с графиком прерываний
  Screen.addScreen(InterruptScreen::create());

  // переключаемся на первый экран
  Screen.switchToScreen("Main");

  // настраиваем обратную связь (информационные диоды и пр.)
  Feedback.begin();

  // настраиваем железные кнопки
  Buttons.begin();

  // поднимаем наши прерывания
  InterruptHandler.begin();

  screenIdleTimer = millis();
  Screen.onAction(screenAction);

  DBGLN(F("Inited."));

  Serial.print(F("UROV "));
  Serial.println(SOFTWARE_VERSION);

  setupDone = true;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{

#ifndef _DELAYED_EVENT_OFF
  CoreDelayedEvent.update();
#endif // _DELAYED_EVENT_OFF

  Settings.update();
  
  // обновляем кнопки
  Buttons.update();

  Screen.update();

  InterruptHandler.update();

  // проверяем, какой экран активен. Если активен главный экран - сбрасываем таймер ожидания. Иначе - проверяем, не истекло ли время ничегонеделанья.
  AbstractTFTScreen* activeScreen = Screen.getActiveScreen();
  if(activeScreen == mainScreen)
  {
    screenIdleTimer = millis();
  }
  else
  {
      if(millis() - screenIdleTimer > RESET_TO_MAIN_SCREEN_DELAY)
      {
		  DBGLN(F("ДОЛГОЕ БЕЗДЕЙСТВИЕ, ПЕРЕКЛЮЧАЕМСЯ НА ГЛАВНЫЙ ЭКРАН!"));
        screenIdleTimer = millis();
        Screen.switchToScreen(mainScreen);
      }
  } // else


#ifndef _COM_COMMANDS_OFF
  // обрабатываем входящие команды
  CommandHandler.handleCommands();
#endif // _COM_COMMANDS_OFF


  // обновляем RS-485
  processRS485();


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool nestedYield = false;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _YIELD_OFF
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void yield()
{  
  if(nestedYield || !setupDone)
    return;
    
 nestedYield = true;
 
   // обновляем прерывания
   InterruptHandler.update();

#ifndef _DELAYED_EVENT_OFF
   CoreDelayedEvent.update();
#endif // _DELAYED_EVENT_OFF

   Buttons.update();


 nestedYield = false;
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // _YIELD_OFF
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


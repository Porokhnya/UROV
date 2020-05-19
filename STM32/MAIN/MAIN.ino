//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
 BLACK F407VG
 Serial commanication: SerialUART1
 Upload method: STLink
 */
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "CONFIG.h"
#include "TFTMenu.h"
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
TwoWire Wire1 = TwoWire(I2C2, PB11, PB10); // второй I2C
Vector<uint8_t> LastTriggeredInterruptRecord; // список последнего сработавшего прерывания
bool isBadSDDetected = false;
bool isBadSDLedOn = false;
uint32_t badSDBlinkTimer = 0;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_EXTERNAL_WATCHDOG
  ExternalWatchdogSettings watchdogSettings;
#endif
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
// РАБОТА С RS-485
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RS485 rs485(RS485_SERIAL,Upr_RS485,RS485_READING_TIMEOUT);
uint32_t rs485RelayTriggeredTime = 0; // время срабатывания защиты
uint32_t rs485DataArrivedTime = 0;
DS3231Time rsRelTrigTime; // время срабатывания защиты
bool HasRS485Link = false;
uint32_t lastRS485PacketSeenAt = 0;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void processInterruptFromModule(uint32_t dataArrivedTime, DS3231Time& tm, InterruptTimeList& interruptsList, bool endstopUpTriggered, bool endstopDownTriggered)
{
	// тут обрабатываем результаты срабатывания защиты от модуля

	// обновляем моторесурс, т.к. было срабатывание защиты
	//DBGLN(F("processInterruptFromModule: INC motoresource!"));
	uint32_t motoresource = Settings.getMotoresource(0);
	motoresource++;
	Settings.setMotoresource(0, motoresource);

	bool hasAlarm = !interruptsList.size(); // авария, если в списке нет данных
	if (hasAlarm)
	{
	//	DBGLN(F("processInterruptFromModule: HAS ALARM FLAG!"));
		// сделал именно так, поскольку флаг аварии сбрасывать нельзя, плюс могут понадобиться дополнительные действия
		Feedback.alarm(true);
	}


	// начинаем сравнивать с эталоном
	EthalonCompareResult compareRes1 = COMPARE_RESULT_NoSourcePulses;

	EthalonCompareNumber compareNumber1;
	InterruptTimeList ethalonData1;

	bool needToLog = false;

	// теперь смотрим - надо ли нам самим чего-то обрабатывать?
	if (interruptsList.size() > 1)
	{
	//	DBG("processInterruptFromModule: INTERRUPT HAS DATA COUNT: ");
	//	DBGLN(interruptsList.size());

		// зажигаем светодиод "ТЕСТ"
		Feedback.testDiode();

		needToLog = true;

		// здесь мы можем обрабатывать список сами - в нём ЕСТЬ данные
		compareRes1 = EthalonComparer::Compare(interruptsList, 0, compareNumber1, ethalonData1);

		if (compareRes1 == COMPARE_RESULT_MatchEthalon)
		{
	//		DBGLN(F("processInterruptFromModule: MATCH ETHALON!"));
		}
		else if (compareRes1 == COMPARE_RESULT_MismatchEthalon || compareRes1 == COMPARE_RESULT_RodBroken)
		{
	//		DBGLN(F("processInterruptFromModule: MISMATCH ETHALON!"));
			Feedback.failureDiode();
			Feedback.alarm();
		}
	}
	else
	{
	//	DBGLN(F("processInterruptFromModule: INTERRUPT HAS NO DATA!!!"));
	}

	// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
	// получаем список данных по току
	CurrentOscillData oscillData = InterruptHandlerClass::getCurrentData();
	// нормализуем список времён
	InterruptHandlerClass::normalizeList(oscillData.times);
	// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

	bool wantToInformSubscriber = (hasAlarm || (interruptsList.size() > 1));

	if (wantToInformSubscriber)
	{
	//	DBGLN(F("processInterruptFromModule: WANT TO INFORM SUBSCRIBER!"));
		InterruptHandler.informSubscriber(oscillData,interruptsList, compareRes1/*, millis() - rs485RelayTriggeredTime, rs485RelayTriggeredTime*/);

   // обновляем экран, чтобы график появился сразу
   Screen.update();
	}

  if (needToLog)
  {
    // записываем в EEPROM
    InterruptHandlerClass::writeToLog(dataArrivedTime, tm, oscillData,interruptsList, compareRes1, compareNumber1, ethalonData1,true);
    
#ifndef _SD_OFF
  //  DBGLN(F("processInterruptFromModule: WANT TO LOG ON SD!"));

    // надо записать в лог на SD дату срабатывания системы
    InterruptHandlerClass::writeToLog(dataArrivedTime, tm, oscillData,interruptsList, compareRes1, compareNumber1, ethalonData1);

#endif // !_SD_OFF
  } // needToLog

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OnRS485IncomingData(RS485* Sender)
{
  // пришёл пакет от модуля по RS-485 (не обязательно от модуля, но в целом - пришёл какой-то пакет от кого-то)

  HasRS485Link = true; // обновляем флаг, что есть связь
  lastRS485PacketSeenAt = millis();

	uint8_t* data;
	RS485Packet packet = rs485.getDataReceived(data);

	switch (packet.packetType)
	{
		case rs485Ping: // сообщение вида "ПРОВЕРКА СВЯЗИ", посылает модуль периодически для проверки связи
		{
			// пришёл пакет пинга
		//	DBGLN(F("[RS-485] RECEIVE PING PACKET FROM MODULE !!!"));
		}
		break; // rs485Ping

		case rs485HasInterrupt: // сообщение вида "ЕСТЬ СРАБАТЫВАНИЕ ЗАЩИТЫ", посылает модуль по факту срабатывания защиты
		{
		//	DBGLN(F("[RS-485] HAS INTERRUPT FROM MODULE !!!"));

			rs485RelayTriggeredTime = millis(); // запоминаем время срабатывания защиты
      rsRelTrigTime = RealtimeClock.getTime();

			// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
			InterruptHandlerClass::startCollectCurrentData(); // начинаем собирать данные по току
			// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

			//TODO: ТУТ МОЖНО ЧТО-ТО ДЕЛАТЬ ПО ФАКТУ СРАБАТЫВАНИЯ ЗАЩИТЫ, НАПРИМЕР - НАЧИНАТЬ ПРОВЕРКУ ТОКОВЫХ ТРАНСФОРМАТОРОВ.
		}
		break; // rs485HasInterrupt

		case rs485InterruptData: // сообщение вида "ДАННЫЕ ПО ПРЕРЫВАНИЮ", посылает модуль по факту собирания списка прерываний
		{
		//	DBGLN(F("[RS-485] HAS INTERRUPT LIST FROM MODULE !!!"));

				// пришли данные по прерыванию, собираем их

				// первый байт в пакете - состояние верхнего концевика на момент окончания сбора данных
				bool endstopUpTriggered = *data++;

				// второй байт в пакете - состояние нижнего концевика на момент окончания сбора данных
				bool endstopDownTriggered = *data++;

				// потом идёт список прерываний
				// 2 байта - служебный заголовок пакета (концевик 1 + концевик 2), потом N записей по 4 байта. Высчитываем это из длины пакета в байтах
				uint16_t recordsCount = (packet.dataLength - 2) / sizeof(uint32_t);
				uint32_t* rec = (uint32_t*)data;

		//		DBG(F("[RS-485] INTERRUPTS COUNT: "));
		//		DBGLN(recordsCount);

				// сохраняем записи
				InterruptTimeList interruptsList;
				interruptsList.reserve(recordsCount);

				for (uint16_t i = 0; i<recordsCount; i++)
				{
					interruptsList.push_back(*rec++);
				}

       // запоминаем время, когда данные пришли
       rs485DataArrivedTime = millis();

       uint32_t dataArriveTime = rs485DataArrivedTime - rs485RelayTriggeredTime;


				// выводим их для теста
/*
#ifdef _MY_DEBUG
				DBGLN(F("-----INTERRUPTS LIST FROM MODULE -----"));
				for (size_t i = 0; i<interruptsList.size(); i++)
				{
					DBGLN(interruptsList[i]);
				}
				DBGLN(F("-----INTERRUPTS LIST END -----"));
#endif
*/
				// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
				// говорим, что хватит нам собирать данные по току
				InterruptHandlerClass::stopCollectCurrentData();
				// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

				// обрабатываем список прерываний  
				processInterruptFromModule(dataArriveTime, rsRelTrigTime,interruptsList, endstopUpTriggered, endstopDownTriggered);
		}
		break; // rs485InterruptData

		default:
		{
			//DBG(F("[RS-485] MAIN HANDLER, UNHANDLED PACKET TYPE: "));
			//DBGLN(packet.packetType);
		}
		break;

	} // switch

	rs485.clearReceivedData(); // очищаем входящие данные
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SwitchRS485MainHandler(bool on)
{
	if (!on) // нас выключили
	{
	//	DBGLN(F("Main handler, release RS-485..."));

		rs485.setHandler(NULL);
		rs485.clearReceivedData();

//		DBGLN(F("Main handler, RS-485 released."));
	}
	else // включили
	{
//		DBGLN(F("Main handler, own RS-485..."));

		rs485.setHandler(OnRS485IncomingData);

//		DBGLN(F("Main handler, RS-485 owned."));
	}

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_EXTERNAL_WATCHDOG
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void updateExternalWatchdog()
{
  static unsigned long watchdogLastMillis = millis();
  unsigned long watchdogCurMillis = millis();

  uint16_t dt = watchdogCurMillis - watchdogLastMillis;
  watchdogLastMillis = watchdogCurMillis;

      watchdogSettings.timer += dt;
      switch(watchdogSettings.state)
      {
        case WAIT_FOR_TRIGGERED:
        {
          if(watchdogSettings.timer >= WATCHDOG_WORK_INTERVAL)
          {
            watchdogSettings.timer = 0;
            watchdogSettings.state = WAIT_FOR_NORMAL;
            digitalWrite(WATCHDOG_REBOOT_PIN, WATCHDOG_TRIGGERED_LEVEL);
          }
        }
        break;

        case WAIT_FOR_NORMAL:
        {
          if(watchdogSettings.timer >= WATCHDOG_PULSE_DURATION)
          {
            watchdogSettings.timer = 0;
            watchdogSettings.state = WAIT_FOR_TRIGGERED;
            digitalWrite(WATCHDOG_REBOOT_PIN, WATCHDOG_NORMAL_LEVEL);
          }          
        }
        break;
      }  
  
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // USE_EXTERNAL_WATCHDOG
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(SERIAL_SPEED);
  while(!Serial && millis() < 2000);

  DBGLN(F("Init I2C..."));
  
  //Wire.begin();  
  Wire1.begin();
  
  DBGLN(F("I2C inited."));

  ConfigPin::setup();

  #ifdef USE_EXTERNAL_WATCHDOG
    pinMode(WATCHDOG_REBOOT_PIN,OUTPUT);
    digitalWrite(WATCHDOG_REBOOT_PIN,WATCHDOG_NORMAL_LEVEL);
    watchdogSettings.timer = 0;
    watchdogSettings.state = WAIT_FOR_TRIGGERED;
  #endif  
  
  
  DBGLN(F("Init settings..."));
  Settings.begin();
  DBGLN(F("Settings inited."));
  
  DBGLN(F("Init RTC..."));
  RealtimeClock.begin(); 
 // RealtimeClock.setTime(0,1,11,1,7,2,2018);
  DBGLN(F("RTC inited."));


  DBGLN(F("Init endstops..."));
  SetupEndstops();
  DBGLN(F("Endstops inited."));


  // настраиваем обратную связь (информационные диоды и пр.)
  Feedback.begin();


  DBGLN(F("Init RS-485..."));
  // инициализируем RS-485
  RS485_SERIAL.begin(RS485_SPEED);
  rs485.setHandler(OnRS485IncomingData);
  rs485.begin();
  DBGLN(F("RS-485 inited."));


#ifndef _SD_OFF

  DBGLN(F("INIT SD..."));
  if (SDInit::InitSD())
  {
	  DBGLN(F("SD inited."));
  }
  else
  {
	  DBGLN(F("SD INIT ERROR!!!"));
  }

  SDSpeedResults sdSpeed = SDInit::MeasureSpeed(&Serial);
  isBadSDDetected = !sdSpeed.testSucceeded || sdSpeed.writeSpeed < MIN_SD_WRITE_SPEED || sdSpeed.readSpeed < MIN_SD_READ_SPEED;  
  
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


  // настраиваем железные кнопки
  Buttons.begin();

  // поднимаем наши прерывания
  InterruptHandler.begin();

  screenIdleTimer = millis();
  Screen.onAction(screenAction);

  DBGLN(F("Inited."));

  CommandHandler.getVER(&Serial);

  setupDone = true;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{

  if(isBadSDDetected) // если детектирована плохая SD-карта
  {
    if(millis() - badSDBlinkTimer >= BAD_SD_BLINK_INTERVAL)
    {
      isBadSDLedOn = !isBadSDLedOn;
      Feedback.failureDiode(isBadSDLedOn);
      badSDBlinkTimer = millis();
    }
  } // if
  

  #ifndef _DELAYED_EVENT_OFF
    CoreDelayedEvent.update();
  #endif // _DELAYED_EVENT_OFF
  
   #ifdef USE_EXTERNAL_WATCHDOG
     updateExternalWatchdog();
   #endif // USE_EXTERNAL_WATCHDOG

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
		 // DBGLN(F("ДОЛГОЕ БЕЗДЕЙСТВИЕ, ПЕРЕКЛЮЧАЕМСЯ НА ГЛАВНЫЙ ЭКРАН!"));
        screenIdleTimer = millis();
        Screen.switchToScreen(mainScreen);
      }
  } // else


#ifndef _COM_COMMANDS_OFF
  // обрабатываем входящие команды
  CommandHandler.handleCommands();
#endif // _COM_COMMANDS_OFF


  // обновляем RS-485
  rs485.update();

  if (millis() - lastRS485PacketSeenAt >= (RS485_PING_PACKET_FREQUENCY)*3)
  {
    HasRS485Link = false; // долго не было пакетов по RS-485
    lastRS485PacketSeenAt = millis(); // чтобы часто не дёргать
  }

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

   #ifdef USE_EXTERNAL_WATCHDOG
     updateExternalWatchdog();
   #endif // USE_EXTERNAL_WATCHDOG

 nestedYield = false;
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // _YIELD_OFF
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


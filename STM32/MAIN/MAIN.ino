//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
 Плата:                 BLACK F407VG/VE/ZE/XG boards
 Specific board:        UROV board (LACK F407VG, vector table=0xC000)
 USB:                   Serial [Virtual COM port, PA11/PA12 pins]
 Serial commanication:  SerialUART1
 Upload method:         STLink [Automatic serial=SerialUSB, address=0x800C000]
 */
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h> // подключение главного заголовочного файла
#include "CONFIG.h" // подключаем файл конфигурации
#include "TFTMenu.h" // подключаем обработчик TFT-экрана
#include "DS3231.h"  // подключаем часы
#include "ConfigPin.h" // подключаем настройки выводов
#include "AT24CX.h" // подключаем работу с EEPROM-памятью
#include "InterruptHandler.h" // подключаем обработчик прерываний
#include "DelayedEvents.h" // подключаем обработчик отложенных событий
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// подключаем наши экраны
#include "Screen1.h"              // Главный экран
#include "Screen2.h"              // Вызов меню настроек
#include "Screen3.h"              // Экран №3
#include "Screen4.h"              // Вызов меню установки времени и даты
#include "Screen5.h"              // Вызов установки времени
#include "Screen6.h"              // Вызов установки даты
#include "InterruptScreen.h"      // экран с графиком прерывания
#include "Buttons.h"              // наши железные кнопки
#include "Feedback.h"             // обратная связь (диоды и прочее)
#include "FileUtils.h"            // работа с файловой системой на SD-карте
#include "Settings.h"             // класс настроек
#include "CoreCommandBuffer.h"    // обработчик входящих по UART команд
#include <Wire.h>                 // Класс работы с I2C
#include "Endstops.h"             // Работа с концевиками
#include "RS485.h"                // Работа с RS-485
#include "RelayGuard.h"           // Работа с релейным входом
#include "ModbusHandler.h"        // Работа с MODBUS
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TwoWire Wire1 = TwoWire(I2C2, PB11, PB10); // второй I2C
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Vector<uint8_t> LastTriggeredInterruptRecord; // список последнего сработавшего прерывания
int8_t LastTriggeredInterruptRecordIndex = -1; // индекс последнего сработавшего прерывания, сохраненный в EEPROM
bool isBadSDDetected = false;   // флаг обнаружения неисправности SD (медленная работа по записи и чтению)
bool isBadSDLedOn = false;      // флаг включения светодиода индикации неисправности SD
uint32_t badSDBlinkTimer = 0;   // таймер мигания светодиодом при неисправности SD
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_EXTERNAL_WATCHDOG
  ExternalWatchdogSettings watchdogSettings; // настройки внешнего ватчдога
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t screenIdleTimer = 0; // таймер ожидания простоя экрана
bool setupDone = false;       // флаг завершения обработки функции setup()
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void screenAction(AbstractTFTScreen* screen) // вызывается при осуществлении действия с экраном (нажатии на тачскрин)
{
	// какое-то действие на экране произошло.
	// тут просто сбрасываем таймер ничегонеделанья.
	screenIdleTimer = millis();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// РАБОТА С RS-485
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _RS485_OFF
RS485 rs485(RS485_SERIAL,Upr_RS485,RS485_READING_TIMEOUT); // экземпляр класса для обработки RS-485. Параметры - указатель на UART, номер вывода для управления приёмом-передачей, таймаут на чтение.
#endif

uint32_t rs485RelayTriggeredTime = 0; // время срабатывания защиты
uint32_t rs485DataArrivedTime = 0; // время прихода данных с внешнего модуля
DS3231Time rsRelTrigTime; // время срабатывания защиты
bool HasRS485Link = false;  // флаг, что с модулем установлена связь по RS-485
uint32_t lastRS485PacketSeenAt = 0; // время получения последнего пакета по RS-485
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void processInterruptFromModule(int32_t dataArrivedTime, DS3231Time& tm, bool endstopUpTriggered, bool endstopDownTriggered) // обработчик входящих по RS-485 данных от модуля
{
	// тут обрабатываем результаты срабатывания защиты от модуля

	// обновляем моторесурс, т.к. было срабатывание защиты
	//DBGLN(F("processInterruptFromModule: INC motoresource!"));
	uint32_t motoresource = Settings.getMotoresource();
	motoresource++;

 // сохраняем новый моторесурс
	Settings.setMotoresource(motoresource);

	bool hasAlarm = !InterruptData.size(); // авария, если в списке нет данных
	if (hasAlarm) // если установлен флаг аварии
	{
	//	DBGLN(F("processInterruptFromModule: HAS ALARM FLAG!"));
		// сделал именно так, поскольку флаг аварии сбрасывать нельзя, плюс могут понадобиться дополнительные действия
		Feedback.setFailureLineLevel(); // говорим на выходящей линии, что это авария
	}


	// начинаем сравнивать с эталоном
	EthalonCompareResult compareRes1 = COMPARE_RESULT_NoSourcePulses;

	EthalonCompareNumber compareNumber1;
	//InterruptTimeList ethalonData1;
 String ethalonFileName;

	bool needToLog = false;

	// теперь смотрим - надо ли нам самим чего-то обрабатывать?
	if (InterruptData.size() > 1)
	{
	//	DBG("processInterruptFromModule: INTERRUPT HAS DATA COUNT: ");
	//	DBGLN(interruptsList.size());

		// зажигаем светодиод "ТЕСТ"
		Feedback.testDiode();

    // устанавливаем флаг, что надо сохранить в лог-файлы
		needToLog = true;

		// здесь мы можем обрабатывать список сами - в нём ЕСТЬ данные
		compareRes1 = EthalonComparer::Compare(InterruptData, 0, compareNumber1, ethalonFileName);//ethalonData1);

		if (compareRes1 == COMPARE_RESULT_MatchEthalon) // если прерывание совпало с эталоном
		{
	//		DBGLN(F("processInterruptFromModule: MATCH ETHALON!"));
      // ничего не делаем
		}
   // иначе, если прерывание не совпало с эталоном, или штанга поломана
		else if (compareRes1 == COMPARE_RESULT_MismatchEthalon || compareRes1 == COMPARE_RESULT_RodBroken)
		{
	//		DBGLN(F("processInterruptFromModule: MISMATCH ETHALON!"));
      
			Feedback.failureDiode(); // зажигаем светодиод "авария"
			Feedback.setFailureLineLevel(); // говорим на выходящей линии, что это авария
		}
	}
	else // иначе - нет данных по прерыванию
	{
	//	DBGLN(F("processInterruptFromModule: INTERRUPT HAS NO DATA!!!"));
  
	}

	// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
	// получаем список данных по току
 uint16_t previewCount;
	CurrentOscillData oscillData = adcSampler.getListOfCurrent(previewCount);
	// нормализуем список времён
	InterruptHandlerClass::normalizeList(oscillData.times);
	// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

  // смотрим - можем ли мы уведомлять подписчика на событие прихода прерывания?
	bool wantToInformSubscriber = (hasAlarm || (InterruptData.size() > 1));

	if (wantToInformSubscriber) // если можем уведомлять подписчика, то
	{
	//	DBGLN(F("processInterruptFromModule: WANT TO INFORM SUBSCRIBER!"));
		InterruptHandler.informSubscriber(&oscillData,compareRes1); // уведомляем его, передавая ему данные

   // обновляем экран, чтобы график появился сразу
   Screen.update();
	}

  // пустой список направлений движения энкодера
  DirectionInfoData emptyDirectionInfo;

  if (needToLog) // если надо сохранить лог, то
  {
    // записываем в EEPROM
    InterruptHandlerClass::writeToLog(emptyDirectionInfo, previewCount, dataArrivedTime, tm, &oscillData,InterruptData, compareRes1, compareNumber1, /*ethalonData1*/ethalonFileName,true);
    
#ifndef _SD_OFF
  //  DBGLN(F("processInterruptFromModule: WANT TO LOG ON SD!"));

    // надо записать в лог на SD дату срабатывания системы
    InterruptHandlerClass::writeToLog(emptyDirectionInfo, previewCount, dataArrivedTime, tm, &oscillData,InterruptData, compareRes1, compareNumber1, ethalonFileName);//ethalonData1);

#endif // !_SD_OFF
  } // needToLog

  // возобновляем работу обработчика прерываний
  InterruptHandler.resume();

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OnRS485IncomingData(RS485* Sender) // обработчик события "есть входящие данные по RS-485"
{
#ifndef _RS485_OFF  
  // пришёл пакет от модуля по RS-485 (не обязательно от модуля, но в целом - пришёл какой-то пакет от кого-то)

  HasRS485Link = true; // обновляем флаг, что есть связь
  lastRS485PacketSeenAt = millis(); // обновляем время получения пакета

  // получаем пакет с данными
	uint8_t* data;
	RS485Packet packet = rs485.getDataReceived(data);

	switch (packet.packetType) // смотрим тип пакета
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

      // останавливаем обработку прерываний у нас
      InterruptHandler.pause();

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
       InterruptData.empty();

				for (uint16_t i = 0; i<recordsCount; i++)
				{
					InterruptData.push_back(*rec++);
				}

       // запоминаем время, когда данные пришли
       rs485DataArrivedTime = millis();

       int32_t dataArriveTime = rs485DataArrivedTime - rs485RelayTriggeredTime;


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
				// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

				// обрабатываем список прерываний  
				processInterruptFromModule(dataArriveTime, rsRelTrigTime, endstopUpTriggered, endstopDownTriggered);
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
#endif // #ifndef _RS485_OFF 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SwitchRS485MainHandler(bool on) // вкл/выкл обработчик RS-485
{
#ifndef _RS485_OFF
  
	if (!on) // нас выключили
	{
	//	DBGLN(F("Main handler, release RS-485..."));

    // выключаем обработчик
		rs485.setHandler(NULL);
		rs485.clearReceivedData();

//		DBGLN(F("Main handler, RS-485 released."));
	}
	else // включили
	{
//		DBGLN(F("Main handler, own RS-485..."));

    // включаем обработчик
		rs485.setHandler(OnRS485IncomingData);

//		DBGLN(F("Main handler, RS-485 owned."));
	}
#endif // #ifndef _RS485_OFF
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_EXTERNAL_WATCHDOG
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void updateExternalWatchdog() // обновляем внешний ватчдог
{

  // таймеры для фиксирования промежутка времени между текущим и последним вызовами функции
  static unsigned long watchdogLastMillis = millis();
  unsigned long watchdogCurMillis = millis();

  // получаем дельту времени
  uint16_t dt = watchdogCurMillis - watchdogLastMillis;
  watchdogLastMillis = watchdogCurMillis;

      // учитываем эту дельту в настройках
      watchdogSettings.timer += dt;
      
      switch(watchdogSettings.state) // смотрим состояние работы с внешним ватчдогом
      {
        case WAIT_FOR_TRIGGERED: // ждём установки уровня "сброс"
        {
          if(watchdogSettings.timer >= WATCHDOG_WORK_INTERVAL) // если интервал времени - вышел, то
          {
            watchdogSettings.timer = 0; // сбрасываем таймер
            watchdogSettings.state = WAIT_FOR_NORMAL; // говорим, что надо ждать установки уровня "нормальный"
            digitalWrite(WATCHDOG_REBOOT_PIN, WATCHDOG_TRIGGERED_LEVEL); // устанавливаем уровень "сброс"
          }
        }
        break;

        case WAIT_FOR_NORMAL: // ждём установки уровня "нормальный"
        {
          if(watchdogSettings.timer >= WATCHDOG_PULSE_DURATION) // если интервал таймера вышел, то
          {
            watchdogSettings.timer = 0; // сбрасываем таймер
            watchdogSettings.state = WAIT_FOR_TRIGGERED; // говорим, что надо ждать установки уровня "сброс"
            digitalWrite(WATCHDOG_REBOOT_PIN, WATCHDOG_NORMAL_LEVEL); // устанавливаем уровень "нормальный"
          }          
        }
        break;
      }  
  
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // USE_EXTERNAL_WATCHDOG
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() // настройка системы в работу
{
  // поднимаем первый UART
  Serial.begin(SERIAL_SPEED);
  while(!Serial && millis() < 2000);

  #ifdef USE_EXTERNAL_WATCHDOG
    // настраиваем внешний ватчдог
    pinMode(WATCHDOG_REBOOT_PIN,OUTPUT);
    digitalWrite(WATCHDOG_REBOOT_PIN,WATCHDOG_NORMAL_LEVEL);
    watchdogSettings.timer = 0;
    watchdogSettings.state = WAIT_FOR_TRIGGERED;
  #endif 


  // поднимаем I2C
  DBGLN(F("Init I2C..."));  
  Wire1.begin();  
  DBGLN(F("I2C inited."));

#ifndef _RTC_OFF  
  // поднимаем часы реального времени
  DBGLN(F("Init RTC..."));
  RealtimeClock.begin(); 
 // RealtimeClock.setTime(0,1,11,1,7,2,2018);
  DBGLN(F("RTC inited."));
#endif // #ifndef _RTC_OFF


  // настраиваем вывода
  ConfigPin::setup();

  DBGLN(F("Init settings..."));
  // загружаем сохранённые в EEPROM настройки
  Settings.begin();
  DBGLN(F("Settings inited."));



  DBGLN(F("Init screen..."));
  // настраиваем TFT-экран
  Screen.setup();

 DBGLN(F("Add screen1...")); 
  // добавляем первый экран
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

  // запоминаем время настройки экрана
  screenIdleTimer = millis();
  // привязываем обработчик нажатия на тачсктин к нашей функции
  Screen.onAction(screenAction);



#ifndef _SD_OFF

  DBGLN(F("INIT SD..."));
  // инициализация SD
  if (SDInit::InitSD())
  {
    DBGLN(F("SD inited."));
  }
  else
  {
    DBGLN(F("SD INIT ERROR!!!"));
  }
  // тестируем SD-карту
  Test_SD(
  &Serial,
  #ifdef DISABLE_SAVE_BENCH_FILE
  false // no save bench results
  #else
  true // save bench results
  #endif
  ,false // read saved bench file, if exists, and return results    
    );

  // настраиваем флаг успешности тестирования SD
  isBadSDDetected = !sdSpeed.testSucceeded || sdSpeed.writeSpeed < MIN_SD_WRITE_SPEED || sdSpeed.readSpeed < MIN_SD_READ_SPEED;  
  
#endif // !_SD_OFF   

 



  DBGLN(F("Init endstops..."));
  // настраиваем концевики
  SetupEndstops();
  DBGLN(F("Endstops inited."));


  // настраиваем обратную связь (информационные диоды и пр.)
  Feedback.begin();


#ifndef _RS485_OFF
  DBGLN(F("Init RS-485..."));
  // инициализируем RS-485
  RS485_SERIAL.begin(RS485_SPEED);
  rs485.setHandler(OnRS485IncomingData);
  rs485.begin();
  DBGLN(F("RS-485 inited."));
#endif // #ifndef _RS485_OFF  
  


  DBGLN(F("Init MODBUS..."));
  // поднимаем MODBUS
  Modbus.begin();
  DBGLN(F("MODBUS inited."));  

 

  // переключаемся на первый экран
  Screen.switchToScreen("Main");

  // настраиваем железные кнопки
  Buttons.begin();

// поднимаем АЦП
#ifndef _ADC_OFF

  adcSampler.setLowBorder(Settings.getTransformerLowBorder());
  adcSampler.setHighBorder(Settings.getTransformerHighBorder());
  
  adcSampler.begin();  
#endif  


  // настраиваем вход релейной защиты
  RelayGuard.begin(RELAY_PIN, 5, // 5 миллисекунд на подавление дребезга
 #if (RELAY_TRIGGER_LEVEL == LOW)
    true
  #else
    false
  #endif  
  );

  // поднимаем наши прерывания
  InterruptHandler.begin();


  DBGLN(F("Inited."));

  // выподим в UART версию прошивки
  CommandHandler.getVER(&Serial);

  // говорим, что настройка окончена
  setupDone = true;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() // основная функция работы программы
{
  if(isBadSDDetected) // если детектирована плохая SD-карта
  {
    if(millis() - badSDBlinkTimer >= BAD_SD_BLINK_INTERVAL) // если пора мигать светодиодом, то
    {
      isBadSDLedOn = !isBadSDLedOn; // меняем флаг его состояния
      Feedback.failureDiode(isBadSDLedOn); // выставляем светодиоду состояние
      badSDBlinkTimer = millis(); // обновляем таймер мигания
    }
  } // if

  Modbus.update(); // обновляем подсистему MODBUS

  #ifndef _DELAYED_EVENT_OFF
    // обновляем отложенные события
    CoreDelayedEvent.update();
  #endif // _DELAYED_EVENT_OFF
  
   #ifdef USE_EXTERNAL_WATCHDOG
     // обновляем внешний ватчдог
     updateExternalWatchdog();
   #endif // USE_EXTERNAL_WATCHDOG

  // обновляем настройки
  Settings.update();
  
  // обновляем кнопки
  Buttons.update();

  // обновляем TFT-экран
  Screen.update();

  // обновляем вход релейной защиты
  RelayGuard.update();
  
  // обновляем обработчик прерываний
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


#ifndef _RS485_OFF
  // обновляем RS-485
  rs485.update();
#endif // #ifndef _RS485_OFF  

  if (millis() - lastRS485PacketSeenAt >= (RS485_PING_PACKET_FREQUENCY)*3) // если очень долго не было пакетов по RS-485 - сбрасываем флаг наличия связи по RS-485
  {
    HasRS485Link = false; // долго не было пакетов по RS-485
    lastRS485PacketSeenAt = millis(); // чтобы часто не дёргать
  }

/*
  //////////// test begin //////
  static uint32_t t = millis();
  if(millis() - t >= 5000)
  {
    Serial.println("CURRENT DATA:");
    
    CurrentOscillData od = adcSampler.getListOfCurrent();
    for(size_t i=0;i<od.data1.size();i++)
    {
      Serial.println(od.data1[i]);
    }
    Serial.println();
    t = millis();
  }
  /////////// test end ///////
*/ 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool nestedYield = false; // влаг вложенных вызовов функции yiels
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _YIELD_OFF
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void yield() // функция, вызывающаяся из любого места прошивки, выполняющего длительные блокирующие операции. Преднахначена для обновления критических мест системы
{  
  if(nestedYield || !setupDone) // если был вложенный вызов, или ещё не закончена работа функции setup - ничего не делаем
  {
    return; // возврат из функции
  }
    
 nestedYield = true; // устанавливаем флаг вложенного вызова
 
   // обновляем прерывания
   InterruptHandler.update();

#ifndef _DELAYED_EVENT_OFF
   // обновляем отложенные события
   CoreDelayedEvent.update();
#endif // _DELAYED_EVENT_OFF

   // обновляем кнопки
   Buttons.update();

   #ifdef USE_EXTERNAL_WATCHDOG
     // обновляем внешний ватчдог
     updateExternalWatchdog();
   #endif // USE_EXTERNAL_WATCHDOG

 nestedYield = false; // сбрасываем флаг вложенного вызова
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // _YIELD_OFF
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Size of read/write.
const size_t BUF_SIZE = 32768;

// File size in MB where MB = 1,000,000 bytes.
const uint32_t FILE_SIZE_MB = 5;

// Write pass count.
const uint8_t WRITE_COUNT = 1;

// Read pass count.
const uint8_t READ_COUNT = 1;
//==============================================================================
// End of configuration constants.
//------------------------------------------------------------------------------
// File size in bytes.
const uint32_t FILE_SIZE = 1000000UL * FILE_SIZE_MB;

uint8_t sdTestBuf[BUF_SIZE]; // буфер для тестирования SD

// file system

//SdFatSdio SD_CARD;


// test file
SdFile file;

// Serial output stream
ArduinoOutStream cout(Serial);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SD_OUT(s) if(outS) { outS->print(s); }
#define SD_OUTLN(s) if(outS) { outS->println(s); }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Test_SD(Stream* outS, bool withBenchFile, bool dontReadSavedBenchFile) // функция тестирования SD
{
  PAUSE_ADC; // останавливаем АЦП

  // вспомогательные переменныт
  float s;
  uint32_t t;
  uint32_t maxLatency;
  uint32_t minLatency;
  uint32_t totalLatency;
  uint32_t writeSpeed;
  uint32_t readSpeed;

  // Discard any input.


/*
  if (!SD_CARD.begin()) {
    SD_CARD.initErrorHalt();
  }
*/
  SD_OUTLN(("[SD TEST] begin..."));

  // инициализируем результаты тестирования
  memset(&sdSpeed,0,sizeof(sdSpeed));

  // принудительно переинициализируем SD
  SDInit::sdInitFlag = false;
  SDInit::InitSD();


  if(!SDInit::sdInitResult) // не удалось инициализировать SD
  {
    SD_OUTLN(("[SD TEST] card not found!"));
    return; // возврат
  }


  SD_OUT(("Type is FAT")); SD_OUTLN(int(SD_CARD.vol()->fatType()));
  SD_OUT(("Card size: ")); SD_OUT(SD_CARD.card()->cardSize()*512E-9);
  SD_OUTLN((" GB (GB = 1E9 bytes)"));


if(withBenchFile && !dontReadSavedBenchFile) // если попросили работать с сохранённым результатом тестирования, и надо его читать, то
{

  
  if(file.open(BENCH_RESULTS_FILENAME,O_READ)) // если открыли файл с результатами тестирования, то
  {
    file.rewind(); // на начало файла

     SD_OUTLN(("[SD TEST] found old bench file, read test results..."));

     // читаем данные
     file.read(&sdSpeed,sizeof(sdSpeed));
     // закрываем файл
     file.close();

    // показываем статистику
    showSDStats(sdSpeed,outS);
    return; // возврат
  }
} // if(withBenchFile)  

   // open or create file - truncate existing file.
   SD_OUTLN(("[SD TEST] create test file..."));

  // имя файла с результатами тестирования, формат - 8.3
  const char* benchfilename = "sd_bnc.dat";

  // если не удалось открыть файл, то
  if (!file.open(benchfilename, O_CREAT | O_TRUNC | O_RDWR)) {
    SD_OUTLN(("[SD TEST] create failed !!!"));

    adcSampler.resume(); // возобновляем АЦП
    return; // возврат
  }

  SD_OUTLN(("[SD TEST] test file created."));

  // показываем сообщение на экране
  Vector<const char*> lines;
  lines.push_back("");
  lines.push_back("");
  lines.push_back("ИДЁТ ТЕСТ SD!");
  lines.push_back("ПОДОЖДИТЕ...");
  MessageBox->show(lines,NULL);
  Screen.update();
  
  // fill sdTestBuf with known data
  for (uint16_t i = 0; i < (BUF_SIZE - 2); i++) {
    sdTestBuf[i] = 'A' + (i % 26);
  }
  sdTestBuf[BUF_SIZE - 2] = '\r';
  sdTestBuf[BUF_SIZE - 1] = '\n';

  //cout << F("File size ") << FILE_SIZE_MB << F(" MB\n");
  //cout << F("Buffer size ") << BUF_SIZE << F(" bytes\n");
  

SD_OUTLN(("Starting write test, please wait..."));


  // начинаем тест записи
  uint32_t n = FILE_SIZE / sizeof(sdTestBuf);
  //cout << F("write speed and latency") << endl;
  //cout << F("speed,max,min,avg") << endl;
  //cout << F("KB/Sec,usec,usec,usec") << endl;
  for (uint8_t nTest = 0; nTest < WRITE_COUNT; nTest++) {
    file.truncate(0);
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    t = millis();

    for (uint32_t i = 0; i < n; i++) {
      uint32_t m = micros();
      if (file.write(sdTestBuf, sizeof(sdTestBuf)) != sizeof(sdTestBuf)) {
        SD_OUTLN(("[SD TEST] write failed!!!"));
        file.close();
        Screen.switchToScreen("Main");

        return;
      }


      m = micros() - m;
      if (maxLatency < m) {
        maxLatency = m;
      }
      if (minLatency > m) {
        minLatency = m;
      }
      totalLatency += m;
    }

    file.sync();
    t = millis() - t;
    s = file.fileSize();
    //cout << s / t << ',' << maxLatency << ',' << minLatency;
    //cout << ',' << totalLatency / n << endl;
  }
  
  writeSpeed = s / t; // вычисляем скорость записи

  //cout << endl << F("Starting read test, please wait.") << endl;
  //cout << endl << F("read speed and latency") << endl;
  //cout << F("speed,max,min,avg") << endl;
  //cout << F("KB/Sec,usec,usec,usec") << endl;

  // начинаем тест чтения
  for (uint8_t nTest = 0; nTest < READ_COUNT; nTest++) {
    file.rewind();
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    t = millis();
    for (uint32_t i = 0; i < n; i++) {
      sdTestBuf[BUF_SIZE - 1] = 0;
      uint32_t m = micros();
      int32_t nr = file.read(sdTestBuf, sizeof(sdTestBuf));
      if (nr != sizeof(sdTestBuf)) {
        SD_OUTLN(("[SD TEST] read failed!!!"));
        file.close();
        Screen.switchToScreen("Main");

        return;
      }
      m = micros() - m;
      if (maxLatency < m) {
        maxLatency = m;
      }
      if (minLatency > m) {
        minLatency = m;
      }
      totalLatency += m;
      if (sdTestBuf[BUF_SIZE - 1] != '\n') {
        SD_OUTLN(("[SD TEST] data check error!"));
      }
    }
    s = file.fileSize();
    t = millis() - t;
  /*  cout << s / t << ',' << maxLatency << ',' << minLatency;
    cout << ',' << totalLatency / n << endl;*/
  }
  readSpeed = s / t; // вычисляем скорость чтения

  file.close(); // закрываем файл

  //Serial.println(writeSpeed);
  //Serial.println(readSpeed);

  sdSpeed.numPasses = WRITE_COUNT;
  sdSpeed.writeSpeed = writeSpeed;
  sdSpeed.readSpeed = readSpeed;
  sdSpeed.testSucceeded = true;

  if (!SD_CARD.remove(benchfilename)) // удаляем файл
  {
    SD_OUTLN("[SD TEST] delete failed!");
  }


  showSDStats(sdSpeed,outS); // показываем статистику

  if(withBenchFile) // если надо сохранить результаты тестирования
  {
    if (file.open(BENCH_RESULTS_FILENAME, O_CREAT | O_TRUNC | O_RDWR)) // открываем файл
    {
      // теперь записываем результаты тестирования в файл, чтобы потом повторно не дёргаться
      file.truncate(0);
      file.write(&sdSpeed, sizeof(sdSpeed));
      file.sync();
      file.close();
    }  
  } // if(withBenchFile)

  SD_OUTLN(("[SD TEST] done."));

  Screen.switchToScreen("Main"); // переключаемся на главный экран
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


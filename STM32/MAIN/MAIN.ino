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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Vector<uint8_t> LastTriggeredInterruptRecord; // список последнего сработавшего прерывания
int8_t LastTriggeredInterruptRecordIndex = -1; // индекс последнего сработавшего прерывания, сохраненный в EEPROM
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
#ifndef _RS485_OFF
RS485 rs485(RS485_SERIAL,Upr_RS485,RS485_READING_TIMEOUT);
#endif

uint32_t rs485RelayTriggeredTime = 0; // время срабатывания защиты
uint32_t rs485DataArrivedTime = 0;
DS3231Time rsRelTrigTime; // время срабатывания защиты
bool HasRS485Link = false;
uint32_t lastRS485PacketSeenAt = 0;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void processInterruptFromModule(int32_t dataArrivedTime, DS3231Time& tm, bool endstopUpTriggered, bool endstopDownTriggered)
{
	// тут обрабатываем результаты срабатывания защиты от модуля

	// обновляем моторесурс, т.к. было срабатывание защиты
	//DBGLN(F("processInterruptFromModule: INC motoresource!"));
	uint32_t motoresource = Settings.getMotoresource();
	motoresource++;
	Settings.setMotoresource(motoresource);

	bool hasAlarm = !InterruptData.size(); // авария, если в списке нет данных
	if (hasAlarm)
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

		needToLog = true;

		// здесь мы можем обрабатывать список сами - в нём ЕСТЬ данные
		compareRes1 = EthalonComparer::Compare(InterruptData, 0, compareNumber1, ethalonFileName);//ethalonData1);

		if (compareRes1 == COMPARE_RESULT_MatchEthalon)
		{
	//		DBGLN(F("processInterruptFromModule: MATCH ETHALON!"));
		}
		else if (compareRes1 == COMPARE_RESULT_MismatchEthalon || compareRes1 == COMPARE_RESULT_RodBroken)
		{
	//		DBGLN(F("processInterruptFromModule: MISMATCH ETHALON!"));
			Feedback.failureDiode();
			Feedback.setFailureLineLevel(); // говорим на выходящей линии, что это авария
		}
	}
	else
	{
	//	DBGLN(F("processInterruptFromModule: INTERRUPT HAS NO DATA!!!"));
	}

	// ИЗМЕНЕНИЯ ПО ТОКУ - НАЧАЛО //
	// получаем список данных по току
	CurrentOscillData oscillData = adcSampler.getListOfCurrent();
	// нормализуем список времён
	InterruptHandlerClass::normalizeList(oscillData.times);
	// ИЗМЕНЕНИЯ ПО ТОКУ - КОНЕЦ //

	bool wantToInformSubscriber = (hasAlarm || (InterruptData.size() > 1));

	if (wantToInformSubscriber)
	{
	//	DBGLN(F("processInterruptFromModule: WANT TO INFORM SUBSCRIBER!"));
		InterruptHandler.informSubscriber(&oscillData,compareRes1);

   // обновляем экран, чтобы график появился сразу
   Screen.update();
	}

  if (needToLog)
  {
    // записываем в EEPROM
    InterruptHandlerClass::writeToLog(dataArrivedTime, tm, &oscillData,InterruptData, compareRes1, compareNumber1, /*ethalonData1*/ethalonFileName,true);
    
#ifndef _SD_OFF
  //  DBGLN(F("processInterruptFromModule: WANT TO LOG ON SD!"));

    // надо записать в лог на SD дату срабатывания системы
    InterruptHandlerClass::writeToLog(dataArrivedTime, tm, &oscillData,InterruptData, compareRes1, compareNumber1, ethalonFileName);//ethalonData1);

#endif // !_SD_OFF
  } // needToLog

  // возобновляем работу обработчика прерываний
  InterruptHandler.resume();

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OnRS485IncomingData(RS485* Sender)
{
#ifndef _RS485_OFF  
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
			//	InterruptHandlerClass::stopCollectCurrentData();
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
void SwitchRS485MainHandler(bool on)
{
#ifndef _RS485_OFF
  
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
#endif // #ifndef _RS485_OFF
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

  #ifdef USE_EXTERNAL_WATCHDOG
    pinMode(WATCHDOG_REBOOT_PIN,OUTPUT);
    digitalWrite(WATCHDOG_REBOOT_PIN,WATCHDOG_NORMAL_LEVEL);
    watchdogSettings.timer = 0;
    watchdogSettings.state = WAIT_FOR_TRIGGERED;
  #endif 


  DBGLN(F("Init I2C..."));  
  Wire1.begin();  
  DBGLN(F("I2C inited."));

#ifndef _RTC_OFF  
  DBGLN(F("Init RTC..."));
  RealtimeClock.begin(); 
 // RealtimeClock.setTime(0,1,11,1,7,2,2018);
  DBGLN(F("RTC inited."));
#endif // #ifndef _RTC_OFF


  ConfigPin::setup();

  DBGLN(F("Init settings..."));
  Settings.begin();
  DBGLN(F("Settings inited."));


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
  
  screenIdleTimer = millis();
  Screen.onAction(screenAction);



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
  Test_SD(
  &Serial,
  #ifdef DISABLE_SAVE_BENCH_FILE
  false // no save bench results
  #else
  true // save bench results
  #endif
  ,false // read saved bench file, if exists, and return results    
    );
    
  isBadSDDetected = !sdSpeed.testSucceeded || sdSpeed.writeSpeed < MIN_SD_WRITE_SPEED || sdSpeed.readSpeed < MIN_SD_READ_SPEED;  
  
#endif // !_SD_OFF   

 




  DBGLN(F("Init endstops..."));
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

  // поднимаем наши прерывания
  InterruptHandler.begin();


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


#ifndef _RS485_OFF
  // обновляем RS-485
  rs485.update();
#endif // #ifndef _RS485_OFF  

  if (millis() - lastRS485PacketSeenAt >= (RS485_PING_PACKET_FREQUENCY)*3)
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
bool nestedYield = false;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _YIELD_OFF
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void yield()
{  
  if(nestedYield || !setupDone)
  {
    return;
  }
    
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

uint8_t sdTestBuf[BUF_SIZE];

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
void Test_SD(Stream* outS, bool withBenchFile, bool dontReadSavedBenchFile)
{
  PAUSE_ADC; // останавливаем АЦП
  
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
  
  memset(&sdSpeed,0,sizeof(sdSpeed));

  // принудительно переинициализируем SD
  SDInit::sdInitFlag = false;
  SDInit::InitSD();


  if(!SDInit::sdInitResult) // не удалось инициализировать SD
  {
    SD_OUTLN(("[SD TEST] card not found!"));
    return;
  }


  SD_OUT(("Type is FAT")); SD_OUTLN(int(SD_CARD.vol()->fatType()));
  SD_OUT(("Card size: ")); SD_OUT(SD_CARD.card()->cardSize()*512E-9);
  SD_OUTLN((" GB (GB = 1E9 bytes)"));


if(withBenchFile && !dontReadSavedBenchFile)
{

  
  if(file.open(BENCH_RESULTS_FILENAME,O_READ))
  {
    file.rewind();

     SD_OUTLN(("[SD TEST] found old bench file, read test results..."));

     file.read(&sdSpeed,sizeof(sdSpeed));
     file.close();

    showSDStats(sdSpeed,outS);
    return;
  }
} // if(withBenchFile)  

   // open or create file - truncate existing file.
   SD_OUTLN(("[SD TEST] create test file..."));

  const char* benchfilename = "sd_bnc.dat";
  
  if (!file.open(benchfilename, O_CREAT | O_TRUNC | O_RDWR)) {
    SD_OUTLN(("[SD TEST] create failed !!!"));

    adcSampler.resume();
    return;
  }

  SD_OUTLN(("[SD TEST] test file created."));

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

  // do write test
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
  writeSpeed = s / t;

  //cout << endl << F("Starting read test, please wait.") << endl;
  //cout << endl << F("read speed and latency") << endl;
  //cout << F("speed,max,min,avg") << endl;
  //cout << F("KB/Sec,usec,usec,usec") << endl;

  // do read test
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
  readSpeed = s / t;

  file.close();

  //Serial.println(writeSpeed);
  //Serial.println(readSpeed);

  sdSpeed.numPasses = WRITE_COUNT;
  sdSpeed.writeSpeed = writeSpeed;
  sdSpeed.readSpeed = readSpeed;
  sdSpeed.testSucceeded = true;

  if (!SD_CARD.remove(benchfilename))
  {
    SD_OUTLN("[SD TEST] delete failed!");
  }


  showSDStats(sdSpeed,outS);

  if(withBenchFile)
  {
    if (file.open(BENCH_RESULTS_FILENAME, O_CREAT | O_TRUNC | O_RDWR)) 
    {
      // теперь записываем результаты тестирования в файл, чтобы потом повторно не дёргаться
      file.truncate(0);
      file.write(&sdSpeed, sizeof(sdSpeed));
      file.sync();
      file.close();
    }  
  } // if(withBenchFile)

  SD_OUTLN(("[SD TEST] done."));

  Screen.switchToScreen("Main");
}

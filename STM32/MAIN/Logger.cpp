//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Logger.h"
#include "CONFIG.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LoggerClass Logger;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LoggerClass::LoggerClass()
{
  memset(&lastWriteTime,0xFF,sizeof(lastWriteTime));
  bPaused = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::doOpenFile(DS3231Time& tm)
{
  lastWriteTime = tm;
  SD_CARD.mkdir(LOGS_DIRECTORY);

 // формируем имя файла ггггммдд.log. (год,месяц,день)
  
  logFileName = LOGS_DIRECTORY;
  if(!logFileName.endsWith("/"))
    logFileName += "/";
  
  logFileName += tm.year;
  if(tm.month < 10)
    logFileName += '0';
  logFileName += tm.month;

 if(tm.dayOfMonth < 10)
  logFileName += '0';
 logFileName += tm.dayOfMonth;

  logFileName += F(".LOG");

  DBG(F("ТЕКУЩИЙ ЛОГ-ФАЙЛ: "));
  DBGLN(logFileName);

  workFile.open(logFileName.c_str(),FILE_WRITE);
  
  if(workFile.isOpen())
  {
    workFile.seekEnd(0);          
  }
   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool LoggerClass::openWorkFile()
{
   static uint32_t lastMillis = 0;
   
  if(workFile.isOpen())
  {
    if(millis() - lastMillis >= 60000ul) // раз в минуту проверяем, чаще - смысла нет
    {
      // файл уже открыт, проверяем, надо ли создать новый?
      DS3231Time tm = RealtimeClock.getTime();
      if(tm.dayOfMonth != lastWriteTime.dayOfMonth)
      {
        // наступил новый день месяца, надо переоткрыть файл
        closeWorkFile();
        doOpenFile(tm);
      }

      lastMillis = millis();
    }
  }
  else
  {
    // файл не открыт, открываем его впервые
    DS3231Time tm = RealtimeClock.getTime();
    doOpenFile(tm);
    lastMillis = millis();
  }

  return workFile.isOpen();
  
  /*  
  
  closeWorkFile();

 // пишем в лог-файл дату/время срабатывания системы
  SD_CARD.mkdir(LOGS_DIRECTORY);

  DS3231Time tm = RealtimeClock.getTime();

  // формируем имя файла ггггммдд.log. (год,месяц,день)
  String logFileName;
  
  logFileName = LOGS_DIRECTORY;
  if(!logFileName.endsWith("/"))
    logFileName += "/";
  
  logFileName += tm.year;
  if(tm.month < 10)
    logFileName += '0';
  logFileName += tm.month;

 if(tm.dayOfMonth < 10)
  logFileName += '0';
 logFileName += tm.dayOfMonth;

  logFileName += F(".LOG");

  DBG(F("ТЕКУЩИЙ ЛОГ-ФАЙЛ: "));
  DBGLN(logFileName);

  workFile.open(logFileName.c_str(),FILE_WRITE);  
  return workFile.isOpen();
  */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::closeWorkFile()
{
  if(workFile.isOpen())
  {
    workFile.close();
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::pause()
{
  bPaused = true;
  closeWorkFile();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::resume()
{
  bPaused = false;
  openWorkFile();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LoggerClass::write(uint8_t* data,size_t dataLength)
{
  if(bPaused) // на паузе, ничего не пишем
  {
    return;
  }
  
	if (!SDInit::sdInitResult)
	{
		DBGLN(F("SD не инициализировано!!!"));
		return;
	}

  if(!data || !dataLength)
  {
    DBGLN(F("LoggerClass::write - нечего записывать!"));
    return;    
  }

	if (!openWorkFile())
	{
		DBGLN(F("ОШИБКА ОТКРЫТИЯ ФАЙЛА НА SD !!!"));
		return;
	}

  workFile.write(data,dataLength);
  workFile.sync();

//  closeWorkFile();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

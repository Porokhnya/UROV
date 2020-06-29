//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Logger.h"
#include "DS3231.h"
#include "CONFIG.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LoggerClass Logger;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LoggerClass::LoggerClass()
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool LoggerClass::openWorkFile()
{
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
void LoggerClass::write(uint8_t* data,size_t dataLength)
{
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

  closeWorkFile();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


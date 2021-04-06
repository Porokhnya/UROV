//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ModbusHandler.h"
#include "Settings.h"
#include "ADCSampler.h"
#include "DS3231.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ModbusHandler Modbus;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MBUS_32_BIT(REG1,REG2) (  (((uint32_t)mbusRegBank.get(REG1)) << 16) | ( mbusRegBank.get(REG2) ) )
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ModbusHandler::ModbusHandler()
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::setID(uint8_t id)
{
  //CONFIG_SERIAL.print("setID=");
  //CONFIG_SERIAL.println(id);
  
  mbusRegBank.setId(id); // устанавливаем ID для слейва MODBUS  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::set(uint16_t reg, uint16_t val)
{
  mbusRegBank.set(reg,val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t ModbusHandler::get(uint16_t reg)
{
  return mbusRegBank.get(reg);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::setup()
{

  setID(1);
  
  mbusRegBank.add(MODBUS_REG_PULSES);
  mbusRegBank.add(MODBUS_REG_PULSES_DELTA);
  mbusRegBank.add(MODBUS_REG_ETHALON_PULSES_DELTA1);
  mbusRegBank.add(MODBUS_REG_ETHALON_PULSES_DELTA2);
  mbusRegBank.add(MODBUS_REG_MOTORESOURCE1);
  mbusRegBank.add(MODBUS_REG_MOTORESOURCE2);
  mbusRegBank.add(MODBUS_REG_MOTORESOURCE_MAX1);
  mbusRegBank.add(MODBUS_REG_MOTORESOURCE_MAX2);
  mbusRegBank.add(MODBUS_REG_TLOW_BORDER1);
  mbusRegBank.add(MODBUS_REG_TLOW_BORDER2);
  mbusRegBank.add(MODBUS_REG_THIGH_BORDER1);
  mbusRegBank.add(MODBUS_REG_THIGH_BORDER2);
  mbusRegBank.add(MODBUS_REG_RDELAY1);
  mbusRegBank.add(MODBUS_REG_RDELAY2);
  mbusRegBank.add(MODBUS_REG_ACSDELAY);
  mbusRegBank.add(MODBUS_REG_SKIPC1);
  mbusRegBank.add(MODBUS_REG_SKIPC2);
  mbusRegBank.add(MODBUS_REG_CCOEFF1);
  mbusRegBank.add(MODBUS_REG_CCOEFF2);
  mbusRegBank.add(MODBUS_REG_ASUTPFLAGS);
  mbusRegBank.add(MODBUS_REG_MAXIDLETIME1);
  mbusRegBank.add(MODBUS_REG_MAXIDLETIME2);
  mbusRegBank.add(MODBUS_REG_RODMOVELEN1);
  mbusRegBank.add(MODBUS_REG_RODMOVELEN2);  
  
  mbusRegBank.add(MODBUS_REG_SAVECHANGES);  // регистр флага сохранения настроек, ПОСЛЕДНИЙ ИЗ РЕГИСТРОВ НАСТРОЕК


  // регистры времени
  mbusRegBank.add(MODBUS_REG_YEAR);
  mbusRegBank.add(MODBUS_REG_MONTH);
  mbusRegBank.add(MODBUS_REG_DAY);
  mbusRegBank.add(MODBUS_REG_HOUR);
  mbusRegBank.add(MODBUS_REG_MINUTE);
  mbusRegBank.add(MODBUS_REG_SECOND);

  // теперь идут служебные настройки
  mbusRegBank.add(MODBUS_REG_FILE_SIZE1);
  mbusRegBank.add(MODBUS_REG_FILE_SIZE2);
  mbusRegBank.add(MODBUS_REG_FUNCTION_NUMBER);
  mbusRegBank.add(MODBUS_REG_READY_FLAG);
  mbusRegBank.add(MODBUS_REG_CONTINUE_FLAG);
  mbusRegBank.add(MODBUS_REG_FILE_FLAGS);
  mbusRegBank.add(MODBUS_REG_DATA_LENGTH);

  
  for(uint16_t i=(MODBUS_REG_DATA); i< (MODBUS_REG_DATA)+(MODBUS_PACKET_LENGTH);i++)
  {
    mbusRegBank.add(i);
  }
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::begin()
{
  mbusSlave._device = &mbusRegBank; // говорим устройству, где наш банк с регистрами

  mbusSlave.setRxTxSwitch(MODBUS_RX_TX_SWITCH); // установка вывода для переключения приема/передачи по RS-485

  MODBUS_SERIAL.begin(MODBUS_BAUD);
  MODBUS_SERIAL.flush();
  while(MODBUS_SERIAL.available()) MODBUS_SERIAL.read();

  mbusSlave.setBaud(MODBUS_BAUD);
  mbusSlave.setSerial(&MODBUS_SERIAL);                       // Подключение к протоколу MODBUS
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::checkForChanges()
{
  // устанавливаем в регистры время контроллера
  static uint32_t setTimeTimer = 0;
  if(millis() - setTimeTimer >= 1000ul)
  {
    DS3231Time tm = RealtimeClock.getTime();

    set(MODBUS_REG_YEAR,tm.year);
    set(MODBUS_REG_MONTH,tm.month);
    set(MODBUS_REG_DAY,tm.dayOfMonth);
    set(MODBUS_REG_HOUR,tm.hour);
    set(MODBUS_REG_MINUTE,tm.minute);
    set(MODBUS_REG_SECOND,tm.second);

    setTimeTimer = millis();
  }

  
  if(mbusRegBank.get(MODBUS_REG_SAVECHANGES) == 1) // попросили сохранить все настройки, пришедшие с modbus
  {
    set(MODBUS_REG_SAVECHANGES,0); // сбрасываем флаг

    uint16_t pulses =  get(MODBUS_REG_PULSES);
    if(pulses != Settings.getPulses())
    {
      Settings.setPulses(pulses);
    }
  
    uint32_t ethDelta = MBUS_32_BIT(MODBUS_REG_ETHALON_PULSES_DELTA1,MODBUS_REG_ETHALON_PULSES_DELTA2);
    if(ethDelta != Settings.getEthalonPulseDelta())
    {
      Settings.setEthalonPulseDelta(ethDelta);
    }
  
    uint8_t pDelta = get(MODBUS_REG_PULSES_DELTA);
    if(pDelta != Settings.getPulsesDelta())
    {
      Settings.setPulsesDelta(pDelta);
    }
  
    uint32_t mres = MBUS_32_BIT(MODBUS_REG_MOTORESOURCE1,MODBUS_REG_MOTORESOURCE2);
    if(mres != Settings.getMotoresource())
    {
      Settings.setMotoresource(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_MOTORESOURCE_MAX1,MODBUS_REG_MOTORESOURCE_MAX2);
    if(mres != Settings.getMotoresourceMax())
    {
      Settings.setMotoresourceMax(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_TLOW_BORDER1,MODBUS_REG_TLOW_BORDER2);
    if(mres != Settings.getTransformerLowBorder())
    {
      Settings.setTransformerLowBorder(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_THIGH_BORDER1,MODBUS_REG_THIGH_BORDER2);
    if(mres != Settings.getTransformerHighBorder())
    {
      Settings.setTransformerHighBorder(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_RDELAY1,MODBUS_REG_RDELAY2);
    if(mres != Settings.getRelayDelay())
    {
      Settings.setRelayDelay(mres);
    }
  
    uint16_t acs =  get(MODBUS_REG_ACSDELAY);
    if(acs != Settings.getACSDelay())
    {
      Settings.setACSDelay(acs);
    }  
  
    mres = MBUS_32_BIT(MODBUS_REG_SKIPC1,MODBUS_REG_SKIPC2);
    if(mres != Settings.getSkipCounter())
    {
      Settings.setSkipCounter(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_CCOEFF1,MODBUS_REG_CCOEFF2);
    if(mres != Settings.getCurrentCoeff())
    {
      Settings.setCurrentCoeff(mres);
    }
  
    uint8_t asuf =  get(MODBUS_REG_ASUTPFLAGS);
    if(asuf != Settings.getAsuTpFlags())
    {
      Settings.setAsuTpFlags(asuf);
    }  
  
    mres = MBUS_32_BIT(MODBUS_REG_MAXIDLETIME1,MODBUS_REG_MAXIDLETIME2);
    if(mres != Settings.getMaxIdleTime())
    {
      Settings.setMaxIdleTime(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_RODMOVELEN1,MODBUS_REG_RODMOVELEN2);  
    if(mres != Settings.getRodMoveLength())
    {
      Settings.setRodMoveLength(mres);
    }

  } // если было запрошено сохранение настроек


  // тут смотрим, запрошена ли мастером какая-либо функция?
  request = (MBusFunction) get(MODBUS_REG_FUNCTION_NUMBER);
  
  set(MODBUS_REG_FUNCTION_NUMBER,0); // сбрасываем флаг запрошенной функции, чтобы повторно не проверять

  switch(request)
  {
    case mbusNone: // никакой функции не запрошено
    break;

    case mbusListFiles: // запрошен список файлов в директории, это может быть как первичный вызов, так и перезапрос на следующий пункт списка
    {

      // получаем имя директории
      String dirName = getPassedFileName();
      
      // прочитали имя директории, надо выводить список файлов в ней
      do_mbusListFiles(dirName);
    }
    break; // mbusListFiles

    case mbusFileContent:
    {
      String fileName = getPassedFileName();
      do_mbusFileContent(fileName);
    }
    break; // mbusFileContent

    case mbusDeleteFile: // запрошего удаление файла
    {
      String fileName = getPassedFileName();
      do_mbusDeleteFile(fileName);
    }
    break;

    case mbusSetDeviceTime: // запрошена установка времени прибора
    {
      do_mbusSetDeviceTime();
    }
    break;
    
  } // switch
  

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::do_mbusSetDeviceTime()
{
    uint16_t year = get(MODBUS_REG_DATA);
    uint8_t month = get(MODBUS_REG_DATA+1);
    uint8_t day = get(MODBUS_REG_DATA+2);

    uint8_t hour = get(MODBUS_REG_DATA+3);
    uint8_t minute = get(MODBUS_REG_DATA+4);
    uint8_t second = get(MODBUS_REG_DATA+5);
    
     // вычисляем день недели
    int16_t dow;
    uint8_t mArr[12] = {6,2,2,5,0,3,5,1,4,6,2,4};
    dow = (year % 100);
    dow = dow*1.25;
    dow += day;
    dow += mArr[month-1];
    
    if (((year % 4)==0) && (month<3))
     dow -= 1;
     
    while (dow>7)
     dow -= 7; 

    RealtimeClock.setTime(second, minute, hour, dow, day, month, year);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String ModbusHandler::getPassedFileName()
{
      uint16_t dataLen = get(MODBUS_REG_DATA_LENGTH);
      String passedName; // имя файла
      uint16_t numRegs = dataLen/2; // количество регистров к чтению
      bool hasLastByte = false;
      
      if(dataLen%2)
      {
        numRegs++; // есть один байт в конце, нечётный
        hasLastByte = true;
      }

      // читаем регистры, содержащие имя файла
      uint16_t readedBytes = 0;
      
      for(uint16_t i=MODBUS_REG_DATA;i<(MODBUS_REG_DATA+numRegs);i++)
      {
        uint16_t reg = get(i);
        char highVal = (char)((reg & 0xFF00) >> 8);
        char lowVal = (char)(reg & 0x00FF);

        // это последний регистр к чтению?
        bool lastReg = i == (MODBUS_REG_DATA+numRegs)-1;

        if(lastReg) // последний регистр к чтению
        {
          // тут может быть один байт
          if(hasLastByte)
          {
            // есть всего один байт в регистре, младший
            passedName += lowVal;
          }
          else
          {
            // надо прочитать его целиком
            passedName += highVal;
            passedName += lowVal;
          }
        }
        else // регистр полностью из двух байт
        {
            passedName += highVal;
            passedName += lowVal;          
        }
        
        
      } // for  

      return passedName;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::do_mbusDeleteFile(const String& fileName)
{
 if(!SDInit::sdInitResult)
  {
    // карта не инициализирована
    
        // устанавливаем флаг перезапроса мастером в 0
        set(MODBUS_REG_CONTINUE_FLAG,0);

        // устанавливаем длину данных в 0
        set(MODBUS_REG_DATA_LENGTH,0);
        
        // устанавливаем флаг готовности данных
        set(MODBUS_REG_READY_FLAG,1);

        // всё, дальше мастер сам разберётся, что делать

    return;
  }  

  // удаляем файл
  FileUtils::deleteFile(fileName);

  // устанавливаем флаг выполнения операции
    set(MODBUS_REG_READY_FLAG,1);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::do_mbusFileContent(const String& fileName)
{
  set(MODBUS_REG_FILE_FLAGS,0); // длина файла - 0
  set(MODBUS_REG_READY_FLAG,0);

  if(!SDInit::sdInitResult)
  {
    // карта не инициализирована
    
        // устанавливаем флаг перезапроса мастером в 0
        set(MODBUS_REG_CONTINUE_FLAG,0);

        // устанавливаем длину данных в 0
        set(MODBUS_REG_DATA_LENGTH,0);
        
        // устанавливаем флаг готовности данных
        set(MODBUS_REG_READY_FLAG,1);

        entry.close(); // закрываем текущий файл

        // всё, дальше мастер сам разберётся, что делать

    return;
  }

    PAUSE_ADC; // останавливаем АЦП

    // открываем файл
    if(!entry.isOpen())
    {
        entrySize = 0;
        
        if(entry.open(fileName.c_str(),O_READ))
        {
          entrySize = entry.fileSize();
        }      
    }

  if(!entry.isOpen())
  {
        // не удалось открыть файл, сообщаем об этом мастеру

        // устанавливаем флаг перезапроса мастером в 0
        set(MODBUS_REG_CONTINUE_FLAG,0);

        // устанавливаем длину данных в 0
        set(MODBUS_REG_DATA_LENGTH,0);
        
        // устанавливаем флаг готовности данных
        set(MODBUS_REG_READY_FLAG,1);

        // всё, дальше мастер сам разберётся, что делать
        
        return; // возврат  
  }

   // устанавливаем длину всего файла в регистры
  set(MODBUS_REG_FILE_SIZE1,(word)(entrySize >> 16));
  set(MODBUS_REG_FILE_SIZE2,(word)(entrySize & 0xffff));

   // теперь читаем данные файла в пакет, пока не забъём весь пакет данными
   uint16_t regNum = MODBUS_REG_DATA; // номер регистра, куда пишем
   uint8_t regCntr = 0; // счётчик байт, готовых к записи в текущий регистр
   uint16_t regData = 0; // байты для записи в текущий регистр
   uint16_t readedBytes = 0; // количество прочитанных байт данных
   bool errorRead = false;

   for(uint16_t i=0;i<(MODBUS_PACKET_LENGTH)*2;i++)
   {
      // читаем очередной байт из файла
      int iCh = entry.read();
      if(iCh == -1)
      {
        // в файле больше нет данных
        errorRead = true;
        
        // закрываем файл
        entry.close();
        
        break; // выходим из цикла
      }
      else
      {
        // в файле ещё есть данные
        readedBytes++; // увеличиваем количество прочитанных байт

        // получаем текущий байт данных
        uint8_t curByte = (uint8_t) iCh;

        // записываем его в промежуточные данные регистра
        regData <<= 8;
        regData |= curByte;
        regCntr++;

        // смотрим, не пора ли записать в регистр?
        if(regCntr  > 1)
        {
          // два байта были сформированы как значение регистра, можно писать
          set(regNum,regData);

          // увеличиваем номер регистра
          regNum++;

          // сбрасываем промежуточные данные
          regData = 0;
          regCntr = 0;
        }
      } // else
    
   } // for

    // устанавливаем флаг перезапроса мастером - если данные прочитаны полным пакетом - то, возможно, там они ещё остались
    set(MODBUS_REG_CONTINUE_FLAG, (!errorRead && readedBytes >= (MODBUS_PACKET_LENGTH)*2 ) ? 1 : 0 );

    // устанавливаем длину данных в количество прочитанных ранее байт
    set(MODBUS_REG_DATA_LENGTH,readedBytes);
    
    // устанавливаем флаг готовности данных
    set(MODBUS_REG_READY_FLAG,1);
   
   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::do_mbusListFiles(const String& dirName)
{
  set(MODBUS_REG_FILE_FLAGS,0); // нет данных по файлу
  set(MODBUS_REG_READY_FLAG,0);
  
  if(!SDInit::sdInitResult)
  {
    // карта не инициализирована
    
        // устанавливаем флаг перезапроса мастером в 0
        set(MODBUS_REG_CONTINUE_FLAG,0);

        // устанавливаем длину данных в 0
        set(MODBUS_REG_DATA_LENGTH,0);
        
        // устанавливаем флаг готовности данных
        set(MODBUS_REG_READY_FLAG,1);

        entry.close(); // закрываем текущий файл

        // всё, дальше мастер сам разберётся, что делать

    return;
  }
  
  PAUSE_ADC; // останавливаем АЦП
  
  if(!root.isOpen())
  {
    // пытаемся открыть папку
      if(root.open(dirName.c_str(),O_READ))
      {
        root.rewind(); // открыли папку, перематываем на начало
      }
      
  } // if(!root.isOpen())

  if(!root.isOpen())
  {
        // не удалось открыть папку, сообщаем об этом мастеру

        // устанавливаем флаг перезапроса мастером в 0
        set(MODBUS_REG_CONTINUE_FLAG,0);

        // устанавливаем длину данных в 0
        set(MODBUS_REG_DATA_LENGTH,0);
        
        // устанавливаем флаг готовности данных
        set(MODBUS_REG_READY_FLAG,1);

        entry.close(); // закрываем текущий файл

        // всё, дальше мастер сам разберётся, что делать
        
        return; // возврат  
  }

  // тут получаем данные файла
  bool entryOpened = entry.openNext(&root,O_READ);

  if(entryOpened)
  {
  
        String fileName = FileUtils::getFileName(entry);
        set(MODBUS_REG_FILE_FLAGS, entry.isDir() ? 2 : 1); // установка признаков файла (2 - папка, 1 - файл)
      
        // теперь сохраняем в регистры нужные состояния, и рапортуем мастеру, что данные по имени файла - готовы
      
        // флаг перезапроса устанавливаем, пусть перезапросит, раз открыли текущий файл
        set(MODBUS_REG_CONTINUE_FLAG,1);
        
        // длина данных
        uint16_t dataLen = fileName.length();
        set(MODBUS_REG_DATA_LENGTH,dataLen);
        
        // данные, заполняем ими регистры, упаковывая данные в слова
        uint16_t startReg = MODBUS_REG_DATA;
        uint16_t numRegs = dataLen/2;
        if(dataLen%2)
        {
          numRegs++;
        }
      
        const char* ptr = fileName.c_str();
      
        for(uint16_t i=startReg;i<startReg+numRegs;i++)
        {
          // в i - номер текущего регистра, куда надо записать данные
          const char highVal = *ptr++;
          const char lowVal = *ptr++;
      
          if(!lowVal)
          {
            // дошли до конца строки, всего один байт в регистр записать
            set(i,highVal);
          }
          else
          {
            // два байта в регистр записать
            uint16_t val = (uint8_t) highVal;
            val <<= 8;
            val |= (uint8_t) lowVal;
      
            set(i,val);
          }
        } // for

  } // if(entryOpened)
  else
  {
    // всё, кончились файлы в запрошенной папке
    set(MODBUS_REG_CONTINUE_FLAG,0);
    set(MODBUS_REG_DATA_LENGTH,0);
  }
  
  // флаг готовности данных
  set(MODBUS_REG_READY_FLAG,1);
  
  entry.close(); // закрываем файл

  if(!entryOpened) // если нет больше файлов - закрываем дескриптор папки
  {
    root.close();
  }
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::update()
{
  
  mbusSlave.run(); // обновляем состояние MODBUS
  checkForChanges(); // проверяем на изменения, пришедшие извне
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


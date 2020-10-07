//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "SDFakeFilesInterceptor.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define OUR_LAST_3_FAKE_FILE_NAME F("LAST_3.LOG") // имя нашего файла с последними тремя срабатываниями
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SDFakeFilesClass SDFakeFiles;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SDFakeFilesClass::SDFakeFilesClass()
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SDFakeFilesClass::addToLS(const String& requestedDirName, Stream* pStream)
{
  String strLogsFir = LOGS_DIRECTORY;
  if(strLogsFir.startsWith("/"))
  {
    strLogsFir.remove(0,1);
  }

  if(requestedDirName.endsWith(strLogsFir))  // это папка с логами, туда надо добавить наш файл
  {
    pStream->println(OUR_LAST_3_FAKE_FILE_NAME);
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SDFakeFilesClass::isOurFakeFile(const String& fileName)
{
  return fileName.endsWith(OUR_LAST_3_FAKE_FILE_NAME);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SDFakeFilesClass::getFileSize(const String& fileName)
{
  if(fileName.endsWith(OUR_LAST_3_FAKE_FILE_NAME)) // это файл с последними тремя срабатываниями
  {    
    uint32_t result = 0;

    EEPROM_CLASS* eeprom = Settings.getEEPROM();
    if(!eeprom)
    {
      return 0;
    }

    for(uint8_t i=0;i<3;i++)
    {
      // вычисляем начало очередной записи в EEPROM
      uint32_t eepromAddress = EEPROM_LAST_3_DATA_ADDRESS + 4 + i*EEPROM_LAST_3_RECORD_SIZE;

      // читаем заголовки
      uint8_t header1, header2, header3;
      header1 = eeprom->read(eepromAddress++);
      header2 = eeprom->read(eepromAddress++);
      header3 = eeprom->read(eepromAddress++);

      // проверяем заголовки
      if(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2 && header3 == RECORD_HEADER3)
      {
        // запись валидная, в следующих четырёх байтах - будет длина данных
        uint32_t recordLength;
        eeprom->read(eepromAddress,(uint8_t*)&recordLength,4);

        // плюсуем к конечному результату
        result += recordLength;
      }
    } // for

    return result;
  } // if(fileName.endsWith(OUR_LAST_3_FAKE_FILE_NAME))

  return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SDFakeFilesClass::printFakeFile(const String& fileName, Stream* pStream)
{
  if(fileName.endsWith(OUR_LAST_3_FAKE_FILE_NAME)) // это файл с последними тремя срабатываниями
  {

    EEPROM_CLASS* eeprom = Settings.getEEPROM();
    if(!eeprom)
    {
      return;
    }

    for(uint8_t i=0;i<3;i++)
    {
       // вычисляем начало очередной записи в EEPROM
      uint32_t eepromAddress = EEPROM_LAST_3_DATA_ADDRESS + 4 + i*EEPROM_LAST_3_RECORD_SIZE;

      // читаем заголовки
      uint8_t header1, header2, header3;
      header1 = eeprom->read(eepromAddress++);
      header2 = eeprom->read(eepromAddress++);
      header3 = eeprom->read(eepromAddress++);

      // проверяем заголовки
      if(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2 && header3 == RECORD_HEADER3)
      {
        // запись валидная, в следующих четырёх байтах - будет длина данных
        uint32_t recordLength;
        eeprom->read(eepromAddress,(uint8_t*)&recordLength,4);

        // пропускаем длину данных, следом пойдут сами данные
        eepromAddress += 4;

        // теперь читаем файл
        for(uint32_t iter=0;iter<recordLength;iter++)
        {
          pStream->write(eeprom->read(eepromAddress++));
        } // for
        
      }
    } // for
    
  } // файл с тремя последними срабатываниями защиты
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

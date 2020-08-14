//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "FileUtils.h"
#include "CONFIG.h"
#include "DS3231.h"
#include "TFTMenu.h"
#include "ADCSampler.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SDSpeedResults sdSpeed;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_SDFAT
  SdFat SD_CARD;
#else
  SdFatSdio SD_CARD;
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SDInit::sdInitFlag = false;
bool SDInit::sdInitResult = false;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FileUtils
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setFileDateTime(uint16_t* date, uint16_t* time) 
{
  DS3231Time tm = RealtimeClock.getTime();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(tm.year, tm. month, tm. dayOfMonth);

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(tm.hour, tm. minute, tm. second);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FileUtils::isEthalonExists(uint8_t channel, bool isUpMove)
{
  if(!SDInit::sdInitResult)
  {
    return false;
  }  

  String fileName = ETHALONS_DIRECTORY;
  fileName += ETHALON_NAME_PREFIX;
  fileName += channel;

  if(isUpMove)
  {
    fileName += ETHALON_UP_POSTFIX;
  }
  else
  {
    fileName += ETHALON_DOWN_POSTFIX;
  }

   fileName += ETHALON_FILE_EXT;

   return SD_CARD.exists(fileName.c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FileUtils::saveEthalon(uint8_t channel, bool isUpMove, Vector<uint32_t>& list)
{
  if(!SDInit::sdInitResult)
  {
    return;
  }
  
  PAUSE_ADC; // останавливаем АЦП  

   SD_CARD.mkdir(ETHALONS_DIRECTORY);

  String fileName = ETHALONS_DIRECTORY;
  fileName += ETHALON_NAME_PREFIX;
  fileName += channel;

  if(isUpMove)
  {
    fileName += ETHALON_UP_POSTFIX;
  }
  else
  {
    fileName += ETHALON_DOWN_POSTFIX;
  }

   fileName += ETHALON_FILE_EXT;

  DBG(F("WRITE ETHALON TO FILE "));
  DBGLN(fileName);

  SdFile file;
  file.open(fileName.c_str(),FILE_WRITE | O_CREAT | O_TRUNC);
  
  if(file.isOpen())
  {
    DBG(F("WRITE ETHALON DATA, RECORDS COUNT: "));
    DBGLN(list.size());

    for(size_t i=0;i<list.size();i++)
    {
      uint32_t val = list[i];
      file.write(&val,sizeof(val));
    }

    file.close();
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FileUtils::deleteFile(const String& fileName)
{
  if(!SDInit::sdInitResult)
  {
    return;
  }
  PAUSE_ADC; // останавливаем АЦП
  SD_CARD.remove(fileName.c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t FileUtils::getFileSize(const String& fileName)
{
  if(!SDInit::sdInitResult)
  {
    return 0;
  }

  PAUSE_ADC;
      
  uint32_t result = 0;
  SdFile f;
  if(f.open(fileName.c_str(),O_READ))
  {
    result = f.fileSize();
    f.close();
  }

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int FileUtils::CountFiles(const String& dirName)
{
  if(!SDInit::sdInitResult)
  {
    return 0;
  }

  PAUSE_ADC; // останавливаем АЦП

  SdFile root;
  if(!root.open(dirName.c_str(),O_READ))
  {
    return 0;
  }

  int result = 0;
  root.rewind();

  SdFile entry;
  while(entry.openNext(&root,O_READ))
  {
    entry.close();
    result++;  
  }

  root.close();
  return result;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FileUtils::SendToStream(Stream& s, const String& fileName)
{
  FileUtils::SendToStream(&s,fileName);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FileUtils::SendToStream(Stream* s, const String& fileName)
{
  if(!SDInit::sdInitResult)
  {
    return;
  }

  PAUSE_ADC; // останавливаем АЦП
      
  SdFile file;
  file.open(fileName.c_str(),FILE_READ);

    if(file.isDir())
    {
      file.close();
      return;
    }  

  if(file.isOpen())
  {
     while(1)
    {
      int iCh = file.read();
      if(iCh == -1)
      {
        break;
      }

        s->write((uint8_t) iCh);
    }

    file.close();
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String FileUtils::getFileName(SdFile &f)
{
    PAUSE_ADC; // останавливаем АЦП
  
    char nameBuff[50] = {0};
    f.getName(nameBuff,50);
    return nameBuff;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FileUtils::printFilesNames(const String& dirName, bool recursive, Stream* outStream)
{  
  if(!SDInit::sdInitResult)
  {
    return;
  }
      
  const char* dirP = dirName.c_str(); 

  PAUSE_ADC; // останавливаем АЦП

  SdFile root;
  if(!root.open(dirP,O_READ))
  {
    return;
  }

  root.rewind();

  SdFile entry;
  while(entry.openNext(&root,O_READ))
  {
    if(entry.isDir())
    {
      String currentDirName =  FileUtils::getFileName(entry);
      outStream->print(currentDirName);
      outStream->println(F("\t<DIR>"));
      
      if(recursive)
      {
        String subPath = dirName + "/";
        subPath += currentDirName;
        FileUtils::printFilesNames(subPath,recursive, outStream);      
      }
    }
    else
    {      
      outStream->println(FileUtils::getFileName(entry));
    }
    entry.close();
  } // while


  root.close();
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SDInit
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SDInit::InitSD()
{
#ifndef _SD_OFF

  if(SDInit::sdInitFlag)
  {
    DBGLN(F("[SD] ALREADY INITED!"));
    return SDInit::sdInitResult;
  }

  DBGLN(F("[SD] begin..."));
  
  SDInit::sdInitFlag = true;
  
  #ifdef USE_SDFAT
  
    SDInit::sdInitResult = SD_CARD.begin(SDCARD_CS_PIN);
    if(!SDInit::sdInitResult)
    {
      delay(50);
      SDInit::sdInitResult = SD_CARD.begin(SDCARD_CS_PIN, SPI_HALF_SPEED);
      if(!SDInit::sdInitResult)
        {
          delay(50);
          SDInit::sdInitResult = SD_CARD.begin(SDCARD_CS_PIN, SPI_QUARTER_SPEED); // пробуем инициализировать SD-модуль
        }
    }
  #else
  
    delay(50);
    SDInit::sdInitResult = SD_CARD.begin();
    
  #endif
  
  
 // SdFile::dateTimeCallback(setFileDateTime);

  DBG(F("[SD] inited? "));
  DBGLN(SDInit::sdInitResult ? "true" : "false");
  
  return SDInit::sdInitResult;
#else
	return false;
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showSDStats(const SDSpeedResults& info, Stream* showIn)
{
  if(showIn == NULL)
  {
    return;
  }

  showIn->print(F("TEST PASSED ? "));
  if(info.testSucceeded)
  {
    showIn->println(F("YES"));
  }
  else
  {
    showIn->println(F("NO"));
  }

  showIn->print(F("NUM PASSES: "));
  showIn->println(info.numPasses);

  showIn->print(F("WRITE SPEED: "));
  showIn->println(info.writeSpeed);
/*
  showIn->print(F("MIN WRITE LATENCY: "));
  showIn->println(info.minWriteLatency);

  showIn->print(F("MAX WRITE LATENCY: "));
  showIn->println(info.maxWriteLatency);

  showIn->print(F("AVG WRITE LATENCY: "));
  showIn->println(info.avgWriteLatency);
*/
  showIn->print(F("READ SPEED: "));
  showIn->println(info.readSpeed);
/*
  showIn->print(F("MIN READ LATENCY: "));
  showIn->println(info.minReadLatency);

  showIn->print(F("MAX READ LATENCY: "));
  showIn->println(info.maxReadLatency);

  showIn->print(F("AVG READ LATENCY: "));
  showIn->println(info.avgReadLatency);
 */ 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "FileUtils.h"
#include "CONFIG.h"
#include "DS3231.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SdFatSdio SD_CARD;
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
void FileUtils::deleteFile(const String& fileName)
{
  if(!SDInit::sdInitResult)
    return;
      
  SD_CARD.remove(fileName.c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t FileUtils::getFileSize(const String& fileName)
{
  if(!SDInit::sdInitResult)
    return 0;
      
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
    return 0;

  SdFile root;
  if(!root.open(dirName.c_str(),O_READ))
    return 0;

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
    return;
      
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
        break;

        s->write((uint8_t) iCh);
    }

    file.close();
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String FileUtils::getFileName(SdFile &f)
{
    char nameBuff[50] = {0};
    f.getName(nameBuff,50);
    return nameBuff;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FileUtils::printFilesNames(const String& dirName, bool recursive, Stream* outStream)
{  
  if(!SDInit::sdInitResult)
    return;
      
  const char* dirP = dirName.c_str(); 

  SdFile root;
  if(!root.open(dirP,O_READ))
    return;

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
  SDInit::sdInitResult = SD_CARD.begin();//(SD_CS_PIN,SPI_HALF_SPEED);
  SdFile::dateTimeCallback(setFileDateTime);

  DBG(F("[SD] inited? "));
  DBGLN(sdInitResult ? "true" : "false");
  
  return SDInit::sdInitResult;
#else
    DBGLN(F("[SD] SD-card disabled in firmware!"));
	return false;
#endif;
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

  showIn->print(F("MIN WRITE LATENCY: "));
  showIn->println(info.minWriteLatency);

  showIn->print(F("MAX WRITE LATENCY: "));
  showIn->println(info.maxWriteLatency);

  showIn->print(F("AVG WRITE LATENCY: "));
  showIn->println(info.avgWriteLatency);

  showIn->print(F("READ SPEED: "));
  showIn->println(info.readSpeed);

  showIn->print(F("MIN READ LATENCY: "));
  showIn->println(info.minReadLatency);

  showIn->print(F("MAX READ LATENCY: "));
  showIn->println(info.maxReadLatency);

  showIn->print(F("AVG READ LATENCY: "));
  showIn->println(info.avgReadLatency);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Size of read/write.
const size_t BUF_SIZE = 50;//32768;

// File size in MB where MB = 1,000,000 bytes.
const uint32_t FILE_SIZE_MB = 1;

// Test pass count.
const uint8_t TEST_COUNT = 2;

// File size in bytes.
const uint32_t FILE_SIZE = 1000000UL*FILE_SIZE_MB;
uint8_t buf[BUF_SIZE];

#define BENCH_FILENAME "bench.spb"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SDSpeedResults SDInit::MeasureSpeed(Stream* intermediateResultsOutStream)
{
  SDSpeedResults results;
  memset(&results,0,sizeof(results));


  if(intermediateResultsOutStream != NULL)
  {
      intermediateResultsOutStream->println(F("[SD TEST] begin..."));
  }

  if(!sdInitResult) // не удалось инициализировать SD
  {
      if(intermediateResultsOutStream != NULL)
      {
          intermediateResultsOutStream->println(F("[SD TEST] card not found!"));
      }

    return results;
  }

  SdFile file;
  
  if(file.open(BENCH_FILENAME,O_READ))
  {
    file.rewind();

      if(intermediateResultsOutStream != NULL)
      {
          intermediateResultsOutStream->println(F("[SD TEST] found old bench file, read test results..."));
      }

     file.read(&results,sizeof(results));
     file.close();

    showSDStats(results,intermediateResultsOutStream);

    return results;
  }


  // файла со старой статистикой нету, начинаем измерять...
  if (!file.open(BENCH_FILENAME, O_CREAT | O_TRUNC | O_RDWR)) 
  {
      if(intermediateResultsOutStream != NULL)
      {
          intermediateResultsOutStream->println(F("[SD TEST] can't create bench file!"));
      }

    return results;
  }

  // fill buf with known data
  for (uint16_t i = 0; i < (BUF_SIZE-2); i++) 
  {
    buf[i] = 'A' + (i % 26);
  }
  
  buf[BUF_SIZE-2] = '\r';
  buf[BUF_SIZE-1] = '\n';
  
  // начинаем старт записи
  float s;
  uint32_t t;
  uint32_t maxLatency;
  uint32_t minLatency;
  uint32_t totalLatency;

  uint32_t speedAccum = 0;
  uint32_t maxLatencyAccum = 0;
  uint32_t minLatencyAccum = 0;
  uint32_t avgLatencyAccum = 0;
 
  
  if(intermediateResultsOutStream != NULL)
  {
      intermediateResultsOutStream->println(F("[SD TEST] starting write test, please wait..."));
  }  
  
  uint32_t n = FILE_SIZE/sizeof(buf);

  for (uint8_t nTest = 0; nTest < TEST_COUNT; nTest++) 
  {
    file.truncate(0);
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    t = millis();
    
    for (uint32_t i = 0; i < n; i++) 
    {
      uint32_t m = micros();
      
      long written = file.write(buf, sizeof(buf));
      
    //  DBG(F("WRITE TO FILE, BYTES: "));
  //    DBGLN(sizeof(buf));
      
  //    DBG(F("File.write returns: "));
 //     DBGLN(written);

      if (written != sizeof(buf)) 
      {
          if(intermediateResultsOutStream != NULL)
          {
              intermediateResultsOutStream->println(F("[SD TEST] write failed!"));
          }
        file.close();
        return results;
      }
      m = micros() - m;
      if (maxLatency < m) 
      {
        maxLatency = m;
      }
      if (minLatency > m) 
      {
        minLatency = m;
      }
      totalLatency += m;
    }
    file.sync();
    t = millis() - t;
    s = file.fileSize();
    
    speedAccum += s/t;
    maxLatencyAccum += maxLatency;
    minLatencyAccum += minLatency;
    avgLatencyAccum += totalLatency/n;
    
  } // for

  results.numPasses = TEST_COUNT;
  results.writeSpeed = speedAccum/results.numPasses;
  results.maxWriteLatency = maxLatencyAccum/results.numPasses;
  results.minWriteLatency = minLatencyAccum/results.numPasses;
  results.avgWriteLatency = avgLatencyAccum/results.numPasses;


  // начинаем тест чтения
  
  speedAccum = 0;
  maxLatencyAccum = 0;
  minLatencyAccum = 0;
  avgLatencyAccum = 0;


  if(intermediateResultsOutStream != NULL)
  {
      intermediateResultsOutStream->println(F("[SD TEST] starting read test, please wait..."));
  }

  for (uint8_t nTest = 0; nTest < TEST_COUNT; nTest++) 
  {
    file.rewind();
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    t = millis();
    for (uint32_t i = 0; i < n; i++) 
    {
      buf[BUF_SIZE-1] = 0;
      uint32_t m = micros();
      int32_t nr = file.read(buf, sizeof(buf)); 
      if (nr != sizeof(buf)) 
      {   
          if(intermediateResultsOutStream != NULL)
          {
              intermediateResultsOutStream->println(F("[SD TEST] read failed!"));
          }
        file.close();
        return results;
      }
      m = micros() - m;
      if (maxLatency < m) 
      {
        maxLatency = m;
      }
      if (minLatency > m) 
      {
        minLatency = m;
      }
      totalLatency += m;
      
      if (buf[BUF_SIZE-1] != '\n') 
      {
          if(intermediateResultsOutStream != NULL)
          {
              intermediateResultsOutStream->println(F("[SD TEST] data check error!"));
          }        
      }
    } // for
    
    s = file.fileSize();
    t = millis() - t;

    speedAccum += s/t;
    maxLatencyAccum += maxLatency;
    minLatencyAccum += minLatency;
    avgLatencyAccum += totalLatency/n;    
    
  } // for

  results.readSpeed = speedAccum/results.numPasses;
  results.maxReadLatency = maxLatencyAccum/results.numPasses;
  results.minReadLatency = minLatencyAccum/results.numPasses;
  results.avgReadLatency = avgLatencyAccum/results.numPasses;
   
  results.testSucceeded = true;

  // теперь записываем результаты тестирования в файл, чтобы потом повторно не дёргаться
  file.truncate(0);
  file.write(&results, sizeof(results));
  file.sync();
  file.close();
  
  showSDStats(results,intermediateResultsOutStream);
  
  return results;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "FileUtils.h"
#include "CONFIG.h"
#include "DS3231.h"
#include "TFTMenu.h"
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
void FileUtils::deleteFile(const String& fileName)
{
  if(!SDInit::sdInitResult)
  {
    return;
  }
      
  SD_CARD.remove(fileName.c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t FileUtils::getFileSize(const String& fileName)
{
  if(!SDInit::sdInitResult)
  {
    return 0;
  }
      
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
  {
    return;
  }
      
  const char* dirP = dirName.c_str(); 

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
const size_t BUF_SIZE = 32768;

// File size in MB where MB = 1,000,000 bytes.
const uint32_t FILE_SIZE_MB = 5;

// Write/read pass count.
const uint8_t TEST_COUNT = 2;

// File size in bytes.
const uint32_t FILE_SIZE = 1000000UL*FILE_SIZE_MB;
uint8_t sdTestBuf[BUF_SIZE];

ArduinoOutStream cout(Serial);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SD_OUT(s) if(outS) { outS->print(s); }
#define SD_OUTLN(s) if(outS) { outS->println(s); }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SDSpeedResults SDInit::MeasureSpeed(Stream* outS, bool withBenchFile, bool dontReadSavedBenchFile)
{

  Vector<const char*> lines;
  lines.push_back("");
  lines.push_back("");
  lines.push_back("ИДЁТ ТЕСТ SD!");
  lines.push_back("ПОДОЖДИТЕ...");
  MessageBox->show(lines,NULL);
  Screen.update();

  
  SDSpeedResults results;
  memset(&results,0,sizeof(results));


  SD_OUTLN(F("[SD TEST] begin..."));

  InitSD();


  if(!SDInit::sdInitResult) // не удалось инициализировать SD
  {
    Screen.switchToScreen("Main");    
    SD_OUTLN(F("[SD TEST] card not found!"));
    return results;
  }

  SdFile file;
  
  float s;
  uint32_t t;
  uint32_t maxLatency;
  uint32_t minLatency;
  uint32_t totalLatency;

  uint32_t speedAccum = 0;
  uint32_t maxLatencyAccum = 0;
  uint32_t minLatencyAccum = 0;
  uint32_t avgLatencyAccum = 0;  

  SD_OUT(F("Type is FAT")); SD_OUTLN(int(SD_CARD.vol()->fatType()));
  SD_OUT(F("Card size: ")); SD_OUT(SD_CARD.card()->cardSize()*512E-9);
  SD_OUTLN(F(" GB (GB = 1E9 bytes)"));



if(withBenchFile && !dontReadSavedBenchFile)
{

  if(file.open(BENCH_RESULTS_FILENAME,O_READ))
  {
    file.rewind();

     SD_OUTLN(F("[SD TEST] found old bench file, read test results..."));

     file.read(&results,sizeof(results));
     file.close();

    showSDStats(results,outS);

    Screen.switchToScreen("Main");

    return results;
  }
} // if(withBenchFile)


SD_OUTLN(F("[SD TEST] create test file..."));

  // open or create file - truncate existing file.
  if (!file.open("bench.dat", O_CREAT | O_TRUNC | O_RDWR)) 
  {
    SD_OUTLN(F("[SD TEST] create failed !!!"));

    Screen.switchToScreen("Main");
    return results;
  }

SD_OUTLN(F("[SD TEST] test file created."));  

  // fill buf with known data
  for (uint16_t i = 0; i < (BUF_SIZE-2); i++) {
    sdTestBuf[i] = 'A' + (i % 26);
  }
  sdTestBuf[BUF_SIZE-2] = '\r';
  sdTestBuf[BUF_SIZE-1] = '\n';

//  cout << F("File size ") << FILE_SIZE_MB << F(" MB\n");
//  cout << F("Buffer size ") << BUF_SIZE << F(" bytes\n");
  SD_OUTLN(F("Starting write test, please wait..."));

  // do write test
  uint32_t n = FILE_SIZE/sizeof(sdTestBuf);
//  cout <<F("write speed and latency") << endl;
//  cout << F("speed,max,min,avg") << endl;
//  cout << F("KB/Sec,usec,usec,usec") << endl;
  for (uint8_t nTest = 0; nTest < TEST_COUNT; nTest++) {
    file.truncate(0);
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    t = millis();
    for (uint32_t i = 0; i < n; i++) {
      uint32_t m = micros();
      if (file.write(sdTestBuf, sizeof(sdTestBuf)) != sizeof(sdTestBuf)) {
        SD_OUTLN(F("[SD TEST] write failed!!!"));
        file.close();
        
        Screen.switchToScreen("Main");
        return results;
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
//    cout << s/t <<',' << maxLatency << ',' << minLatency;
//    cout << ',' << totalLatency/n << endl;

    speedAccum += s/t;
    maxLatencyAccum += maxLatency;
    minLatencyAccum += minLatency;
    avgLatencyAccum += totalLatency/n;    
  }
  SD_OUTLN(F("Starting read test, please wait..."));
//  cout << endl <<F("read speed and latency") << endl;
//  cout << F("speed,max,min,avg") << endl;
//  cout << F("KB/Sec,usec,usec,usec") << endl;


  results.numPasses = TEST_COUNT;
  results.writeSpeed = speedAccum/results.numPasses;
  results.maxWriteLatency = maxLatencyAccum/results.numPasses;
  results.minWriteLatency = minLatencyAccum/results.numPasses;
  results.avgWriteLatency = avgLatencyAccum/results.numPasses;  

  // do read test

  speedAccum = 0;
  maxLatencyAccum = 0;
  minLatencyAccum = 0;
  avgLatencyAccum = 0;
    
  for (uint8_t nTest = 0; nTest < TEST_COUNT; nTest++) {
    file.rewind();
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    t = millis();
    for (uint32_t i = 0; i < n; i++) {
      sdTestBuf[BUF_SIZE-1] = 0;
      uint32_t m = micros();
      int32_t nr = file.read(sdTestBuf, sizeof(sdTestBuf)); 
      if (nr != sizeof(sdTestBuf)) {   
        SD_OUTLN(F("[SD TEST] read failed !!!"));
        file.close();

        Screen.switchToScreen("Main");
        return results;
      }
      m = micros() - m;
      if (maxLatency < m) {
        maxLatency = m;
      }
      if (minLatency > m) {
        minLatency = m;
      }
      totalLatency += m;
      if (sdTestBuf[BUF_SIZE-1] != '\n') {
        SD_OUTLN(F("[SD TEST] data check error!"));
      }
    }
    s = file.fileSize();
    t = millis() - t;
//    cout << s/t <<',' << maxLatency << ',' << minLatency;
//    cout << ',' << totalLatency/n << endl;

    speedAccum += s/t;
    maxLatencyAccum += maxLatency;
    minLatencyAccum += minLatency;
    avgLatencyAccum += totalLatency/n;        
  }

  results.readSpeed = speedAccum/results.numPasses;
  results.maxReadLatency = maxLatencyAccum/results.numPasses;
  results.minReadLatency = minLatencyAccum/results.numPasses;
  results.avgReadLatency = avgLatencyAccum/results.numPasses;
   
  results.testSucceeded = true;
  
//  cout << endl << F("Done") << endl;
  file.close();

  showSDStats(results,outS);

if(withBenchFile)
{
  if (file.open(BENCH_RESULTS_FILENAME, O_CREAT | O_TRUNC | O_RDWR)) 
  {
    // теперь записываем результаты тестирования в файл, чтобы потом повторно не дёргаться
    file.truncate(0);
    file.write(&results, sizeof(results));
    file.sync();
    file.close();
  }  
} // if(withBenchFile)

  SD_OUTLN(F("[SD TEST] done."));

  Screen.switchToScreen("Main");
  return results;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


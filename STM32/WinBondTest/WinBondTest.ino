#include <RamDisk.h>
#include <WinBondRAM.h>
#include<SPIFlash.h>
//------------------------------------------------------------------------------
// закомментировать, если НЕ НАДО форматировать память под FAT перед стартом
#define FORMAT_RAM_AT_START
//------------------------------------------------------------------------------
SPIFlash flash(PB0);
WinBondRAM ram;
RamVolume vol;
RamFile file;
char buf[40];
//------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200);
  delay(100);

  
  // prints title with ending line break
  Serial.println(F("Setup start"));

  SPI.stm32SetMOSI(PB5);
  SPI.stm32SetMISO(PB4);
  SPI.stm32SetSCK(PB3);
   
  delay(2000);
  Serial.println("EXTERNAL FLASH TEST");

  flash.begin();  

  // передаём класс работы с памятью в интерфейс работы с RamDisk  
  ram.begin(flash);

  getID();

  #ifdef FORMAT_RAM_AT_START
  Serial.println("Start FLASH formatting.");

    // use defaults:
    // totalBlocks: entire RAM
    // dirBlocks: 4  (64 entries)
    // clusterSizeBlocks: 1 (one 512 byte block per cluster)

    Serial.println("Format started, please wait...");

    if (!vol.format(&ram)) 
	  {
      Serial.println(F("Format failed!"));
      return;
    }

    Serial.println("Format done."); 
  #endif //  FORMAT_RAM_AT_START

  // инициализируем память
  Serial.println("Init FLASH...");
  
  if (!vol.init(&ram)) 
  {
    Serial.println(F("Init fail!"));
    return;
  }
  
  vol.printInfo(&Serial);
  if (!file.open("TEST.TXT", O_CREAT | O_RDWR | O_TRUNC)) {
    Serial.println(F("Can't open file TEST.TXT!"));
    return;
  }
  file.println("Hello RamDisk!");
  file.println("Line to test fgets");
  file.rewind();
  int lineNumber = 0;
  // Read file line at a time.
  while (file.fgets(buf, sizeof(buf)) > 0) {
    Serial.print(++lineNumber);
    Serial.print(F(": "));
    Serial.print(buf);
  }
  file.close();
  Serial.println();
  Serial.println(F("Done!"));
}
//------------------------------------------------------------------------------
void loop() {}
//------------------------------------------------------------------------------
void printLine()
{
  Serial.println(F("----------------------------------------"));
}
//------------------------------------------------------------------------------
void clearprintBuffer(char *bufPtr)
{
  for (uint8_t i = 0; i < 128; i++) {
    *bufPtr++ = 0;
  }
}
//------------------------------------------------------------------------------
void getID() {
  char printBuffer[128];
  printLine();
  for (uint8_t i = 0; i < 68; i++) {
    Serial.print(F(" "));
  }
  Serial.print(F("SPIFlash Library version"));
#ifdef LIBVER
  uint8_t _ver, _subver, _bugfix;
  flash.libver(&_ver, &_subver, &_bugfix);
  clearprintBuffer(&printBuffer[1]);
  sprintf(printBuffer, ": %d.%d.%d", _ver, _subver, _bugfix);
  Serial.println(printBuffer);
#else
  Serial.println(F("< 2.5.0"));
#endif
  printLine();

  for (uint8_t i = 0; i < 80; i++) {
    Serial.print(F(" "));
  }
  Serial.println(F("Get ID"));
  printLine();
  uint8_t b1, b2;
  uint16_t b3;
  uint32_t JEDEC = flash.getJEDECID();
  uint32_t maxPage = flash.getMaxPage();
  uint32_t capacity = flash.getCapacity();
  b1 = (JEDEC >> 16);
  b2 = (JEDEC >> 8);
  b3 = (JEDEC >> 0);


  printLine();
  //---------------------------------------------------------------------------------------------//

  clearprintBuffer(&printBuffer[1]);
  sprintf(printBuffer, "\t\t\tJEDEC ID: %04lxh", JEDEC);
  Serial.println(printBuffer);
  clearprintBuffer(&printBuffer[1]);
  sprintf(printBuffer, "\t\t\tManufacturer ID: %02xh\n\t\t\tMemory Type: %02xh\n\t\t\tCapacity: %lu bytes\n\t\t\tMaximum pages: %lu", b1, b2, capacity, maxPage);
  Serial.println(printBuffer);
}


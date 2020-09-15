#include <RamDisk.h>
#include <WinBondRAM.h>

WinBondRAM ram;
RamVolume vol;
RamFile file;
char buf[40];
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  
  // Инициализируем память.
  // Первый параметр - пин CS
  // второй параметр - пин MOSI
  // третий параметр - пин MISO
  // четвертый параметр - пин SCK
  ram.begin(PC13,PA7,PA6,PA5);

  Serial.print(F("Format (Y/N): "));
  while (!Serial.available());
  char c = toupper(Serial.read());
  Serial.println(c);

  if (c == 'Y') {
    // use defaults:
    // totalBlocks: entire RAM
    // dirBlocks: 4  (64 entries)
    // clusterSizeBlocks: 1 (one 512 byte block per cluster)
    if (!vol.format(&ram)) {
      Serial.println(F("format failed"));
      return;
    }
  } 
  else if (c != 'N') {
    Serial.println(F("Invalid entry"));
    return;
  }
  if (!vol.init(&ram)) {
    Serial.println(F("init fail"));
    return;
  }
  vol.printInfo(&Serial);
  if (!file.open("TEST.TXT", O_CREAT | O_RDWR | O_TRUNC)) {
    Serial.println(F("open fail"));
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

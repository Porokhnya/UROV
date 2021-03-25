#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>

#include "CONFIG.h"
#include "FileUtils.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// работа с MODBUS
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  mbusNone = 0,     // нет функции
  mbusListFiles,    // запрошен список файлов в директории
  mbusFileContent,  // запрошено содержимое файла
  
} MBusFunction; // коды функций
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ModbusHandler
{
  public:

    ModbusHandler();

    void setup();
    void begin();
    void update();


    // установка значения регистра
    void set(uint16_t reg, uint16_t val);

    // получение значения регистра
    uint16_t get(uint16_t reg);

    // установка ID слейва
    void setID(uint8_t id);

private:


  modbusDevice mbusRegBank;
  modbusSlave mbusSlave;


  void checkForChanges();


  // возвращает переданное мастером имя файла
  String getPassedFileName();
  
  // работа с запрошенными функциями
  MBusFunction request;
  void do_mbusListFiles(const String& dirName);
  void do_mbusFileContent(const String& fileName);
  

  // для работы со списком файлов
  SdFile root;
  SdFile entry;
  uint32_t entrySize; // размер файла
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern ModbusHandler Modbus;

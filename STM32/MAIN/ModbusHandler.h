#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
*/
#include "CONFIG.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// работа с MODBUS
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ModbusHandler
{
  public:

    ModbusHandler();
    
    void begin();
    void update();


private:

  //modbusDevice mbusRegBank;
  //modbusSlave mbusSlave;
    
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern ModbusHandler Modbus;
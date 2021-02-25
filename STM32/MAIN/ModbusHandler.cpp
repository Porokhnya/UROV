//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ModbusHandler.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ModbusHandler Modbus;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ModbusHandler::ModbusHandler()
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::begin()
{
  
  mbusRegBank.setId(MODBUS_SLAVE_ID); // устанавливаем ID для слейва MODBUS
  mbusRegBank.add(MODBUS_REG_NUM);  // добавляем наш тестовый регистр

  mbusSlave._device = &mbusRegBank; // говорим устройству, где наш банк с регистрами
  mbusSlave.setSerial(MODBUS_SERIAL_NUMBER,MODBUS_BAUD);                       // Подключение к протоколу MODBUS
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::update()
{
  mbusSlave.run(); // обновляем состояние MODBUS
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


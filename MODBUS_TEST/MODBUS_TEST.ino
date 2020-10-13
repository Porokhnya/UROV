//--------------------------------------------------------------------------------------------------------------------------------------
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
//--------------------------------------------------------------------------------------------------------------------------------------
// настройки прошивки
//--------------------------------------------------------------------------------------------------------------------------------------
#define MODBUS_SLAVE_ID         100     // ID слейва MODBUS
#define MODBUS_REG_NUM          4000    // номер тестового регистра для MODBUS
#define MODBUS_SERIAL_NUMBER    2       // номер Serial для общения с MODBUS (0 - Serial, 1 - Serial1, 2 - Serial2, 3 - Serial3)
#define MODBUS_BAUD             57600   // скорость Serial для работы с MODBUS
//--------------------------------------------------------------------------------------------------------------------------------------
modbusDevice mbusRegBank;
modbusSlave mbusSlave;
//--------------------------------------------------------------------------------------------------------------------------------------
void initRegBank()
{
  mbusRegBank.setId(MODBUS_SLAVE_ID);               // MODBUS Slave ID
  mbusRegBank.add(MODBUS_REG_NUM);

  mbusSlave._device = &mbusRegBank;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  initRegBank();
  mbusSlave.setSerial(MODBUS_SERIAL_NUMBER,MODBUS_BAUD);                       // Подключение к протоколу MODBUS

}
//--------------------------------------------------------------------------------------------------------------------------------------
void loop()
{
  mbusSlave.run();

  // тестовая работа с регистром MODBUS, каждую секунду меняем состояние регистра на противоположное
  static bool bRegState = true;
  static uint32_t timer = millis();
  if(millis() - timer >= 1000)
  {
    bRegState = !bRegState;
    mbusRegBank.set(MODBUS_REG_NUM,bRegState);
    timer = millis();
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void yield()
{
  mbusSlave.run();
}
//--------------------------------------------------------------------------------------------------------------------------------------

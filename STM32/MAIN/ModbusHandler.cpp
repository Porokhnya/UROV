//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ModbusHandler.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ModbusHandler Modbus;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MBUS_32_BIT(REG1,REG2) (  (((uint32_t)mbusRegBank.get(REG1)) << 16) | ( mbusRegBank.get(REG2) ) )
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ModbusHandler::ModbusHandler()
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::setID(uint8_t id)
{
  //TODO: ИСПРАВИТЬ КАК БЫЛО !!!!
  mbusRegBank.setId(1);//id); // устанавливаем ID для слейва MODBUS  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::set(uint16_t reg, uint16_t val)
{
  mbusRegBank.set(reg,val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t ModbusHandler::get(uint16_t reg)
{
  return mbusRegBank.get(reg);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::setup()
{
  
  mbusRegBank.add(MODBUS_REG_PULSES);
  mbusRegBank.add(MODBUS_REG_PULSES_DELTA);
  mbusRegBank.add(MODBUS_REG_ETHALON_PULSES_DELTA1);
  mbusRegBank.add(MODBUS_REG_ETHALON_PULSES_DELTA2);
  mbusRegBank.add(MODBUS_REG_MOTORESOURCE1);
  mbusRegBank.add(MODBUS_REG_MOTORESOURCE2);
  mbusRegBank.add(MODBUS_REG_MOTORESOURCE_MAX1);
  mbusRegBank.add(MODBUS_REG_MOTORESOURCE_MAX2);
  mbusRegBank.add(MODBUS_REG_TLOW_BORDER1);
  mbusRegBank.add(MODBUS_REG_TLOW_BORDER2);
  mbusRegBank.add(MODBUS_REG_THIGH_BORDER1);
  mbusRegBank.add(MODBUS_REG_THIGH_BORDER2);
  mbusRegBank.add(MODBUS_REG_RDELAY1);
  mbusRegBank.add(MODBUS_REG_RDELAY2);
  mbusRegBank.add(MODBUS_REG_ACSDELAY);
  mbusRegBank.add(MODBUS_REG_SKIPC1);
  mbusRegBank.add(MODBUS_REG_SKIPC2);
  mbusRegBank.add(MODBUS_REG_CCOEFF1);
  mbusRegBank.add(MODBUS_REG_CCOEFF2);
  mbusRegBank.add(MODBUS_REG_ASUTPFLAGS);
  mbusRegBank.add(MODBUS_REG_MAXIDLETIME1);
  mbusRegBank.add(MODBUS_REG_MAXIDLETIME2);
  mbusRegBank.add(MODBUS_REG_RODMOVELEN1);
  mbusRegBank.add(MODBUS_REG_RODMOVELEN2);  




  
  mbusRegBank.add(MODBUS_REG_SAVECHANGES);  // регистр флага сохранения настроек
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::begin()
{

  setID(1);

  mbusSlave._device = &mbusRegBank; // говорим устройству, где наш банк с регистрами

  mbusSlave.setRxTxSwitch(MODBUS_RX_TX_SWITCH); // установка вывода для переключения приема/передачи по RS-485

  MODBUS_SERIAL.begin(MODBUS_BAUD);
  MODBUS_SERIAL.flush();
  
  mbusSlave.setSerial(&MODBUS_SERIAL,MODBUS_BAUD);                       // Подключение к протоколу MODBUS
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::checkForChanges()
{
  if(mbusRegBank.get(MODBUS_REG_SAVECHANGES) == 1) // попросили сохранить все настройки, пришедшие с modbus
  {
    set(MODBUS_REG_SAVECHANGES,0); // сбрасываем флаг

    uint8_t pulses =  mbusRegBank.get(MODBUS_REG_PULSES);
    if(pulses != Settings.getPulses())
    {
      Settings.setPulses(pulses);
    }
  
    uint32_t ethDelta = MBUS_32_BIT(MODBUS_REG_ETHALON_PULSES_DELTA1,MODBUS_REG_ETHALON_PULSES_DELTA2);
    if(ethDelta != Settings.getEthalonPulseDelta())
    {
      Settings.setEthalonPulseDelta(ethDelta);
    }
  
    uint8_t pDelta = mbusRegBank.get(MODBUS_REG_PULSES_DELTA);
    if(pDelta != Settings.getPulsesDelta())
    {
      Settings.setPulsesDelta(pDelta);
    }
  
    uint32_t mres = MBUS_32_BIT(MODBUS_REG_MOTORESOURCE1,MODBUS_REG_MOTORESOURCE2);
    if(mres != Settings.getMotoresource())
    {
      Settings.setMotoresource(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_MOTORESOURCE_MAX1,MODBUS_REG_MOTORESOURCE_MAX2);
    if(mres != Settings.getMotoresourceMax())
    {
      Settings.setMotoresourceMax(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_TLOW_BORDER1,MODBUS_REG_TLOW_BORDER2);
    if(mres != Settings.getTransformerLowBorder())
    {
      Settings.setTransformerLowBorder(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_THIGH_BORDER1,MODBUS_REG_THIGH_BORDER2);
    if(mres != Settings.getTransformerHighBorder())
    {
      Settings.setTransformerHighBorder(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_RDELAY1,MODBUS_REG_RDELAY2);
    if(mres != Settings.getRelayDelay())
    {
      Settings.setRelayDelay(mres);
    }
  
    uint16_t acs =  mbusRegBank.get(MODBUS_REG_ACSDELAY);
    if(acs != Settings.getACSDelay())
    {
      Settings.setACSDelay(acs);
    }  
  
    mres = MBUS_32_BIT(MODBUS_REG_SKIPC1,MODBUS_REG_SKIPC2);
    if(mres != Settings.getSkipCounter())
    {
      Settings.setSkipCounter(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_CCOEFF1,MODBUS_REG_CCOEFF2);
    if(mres != Settings.getCurrentCoeff())
    {
      Settings.setCurrentCoeff(mres);
    }
  
    uint8_t asuf =  mbusRegBank.get(MODBUS_REG_ASUTPFLAGS);
    if(asuf != Settings.getAsuTpFlags())
    {
      Settings.setAsuTpFlags(acs);
    }  
  
    mres = MBUS_32_BIT(MODBUS_REG_MAXIDLETIME1,MODBUS_REG_MAXIDLETIME2);
    if(mres != Settings.getMaxIdleTime())
    {
      Settings.setMaxIdleTime(mres);
    }
  
    mres = MBUS_32_BIT(MODBUS_REG_RODMOVELEN1,MODBUS_REG_RODMOVELEN2);  
    if(mres != Settings.getRodMoveLength())
    {
      Settings.setRodMoveLength(mres);
    }

  } // если было запрошено сохранение настроек
  

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ModbusHandler::update()
{
  
  mbusSlave.run(); // обновляем состояние MODBUS
//TODO: ИСПРАВИТЬ, КАК БЫЛО !!!
//  checkForChanges(); // проверяем на изменения, пришедшие извне
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


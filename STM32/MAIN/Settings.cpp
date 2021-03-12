//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Settings.h"
#include "CONFIG.h"
#include "ConfigPin.h"
#include "ADCSampler.h"
#include "ModbusHandler.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass Settings;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass::SettingsClass() // конструктор
{
  // инициализируем все настройки по умолчанию
  eeprom = NULL;
  timer = DATA_MEASURE_THRESHOLD;
  
  voltage3V3.raw = voltage5V.raw = voltage200V.raw = 0;
  voltage3V3.voltage = voltage5V.voltage = voltage200V.voltage = 0;

  relayDelay = RELAY_WANT_DATA_AFTER;
  acsDelay = ACS_SIGNAL_DELAY;

  currentCoeff = CURRENT_COEFF_DEFAULT; // коэффициент тока по умолчанию
  channelPulses = 0;
  channelDelta = 0;
  motoresource = 0;
  motoresourceMax = 0;
  transformerHighBorder = TRANSFORMER_HIGH_DEFAULT_BORDER;
  transformerLowBorder = TRANSFORMER_LOW_DEFAULT_BORDER;
  skipCounter = INTERRUPT_SKIP_COUNTER;

  asuTpFlags = 0xFF; // выдаём сигналы на все линии АСУ ТП

#ifndef DISABLE_CATCH_ENCODER_DIRECTION
  rodDirection = rpBroken;
#else
  rodDirection = rpUp;
#endif

  modbus_slave_id = 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getUUID(const char* passedUUID) // возвращаем уникальный ID контроллера
{
    String savedUUID;

    if(!eeprom)
    {
      return savedUUID;
    }

    // читаем заголовки
    uint32_t addr = UUID_STORE_ADDRESS;
    uint8_t header1 = eeprom->read(addr); addr++;
    uint8_t header2 = eeprom->read(addr); addr++;
    uint8_t header3 = eeprom->read(addr); addr++;

    if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2 && header3 == RECORD_HEADER3)) // если заголовки плохие - то сохраняем переданный ID как уникальный
    {
      savedUUID = passedUUID;

      addr = UUID_STORE_ADDRESS;
      eeprom->write(addr,RECORD_HEADER1); addr++;
      eeprom->write(addr,RECORD_HEADER2); addr++;
      eeprom->write(addr,RECORD_HEADER3); addr++;

      uint8_t written = 0;
      for(size_t i=0;i<savedUUID.length();i++)
      {
        eeprom->write(addr,savedUUID[i]); 
        addr++;
        written++;
      }

      for(int i=written;i<32;i++)
      {
         eeprom->write(addr,'\0'); 
         addr++;
      }

      return savedUUID;
    }

    // есть сохранённый GUID, читаем его
    for(int i=0;i<32;i++)
    {
      char ch = (char) eeprom->read(addr); addr++;;
      if(!ch)
      {
        break;
      }
      savedUUID += ch;
    }

    return savedUUID;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write8(uint32_t addr, uint8_t val) // пишем один байт в память, с заголовками
{
  if(!eeprom)
  {
    return;    
  }

    uint32_t writeaddr = addr;
    eeprom->write(writeaddr,RECORD_HEADER1);writeaddr++;
    eeprom->write(writeaddr,RECORD_HEADER2);writeaddr++;
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeaddr,writePtr,sizeof(val)); 
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write16(uint32_t addr, uint16_t val) // пишем два байта в память, с заголовками
{
  if(!eeprom)
  {
    return;    
  }

    uint32_t writeaddr = addr;
    eeprom->write(writeaddr,RECORD_HEADER1);writeaddr++;
    eeprom->write(writeaddr,RECORD_HEADER2);writeaddr++;
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeaddr,writePtr,sizeof(val)); 
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write32(uint32_t addr, uint32_t val) // пишем 4 байта в память, с заголовками
{
  if(!eeprom)
  {
    return;    
  }

    uint32_t writeaddr = addr;
    eeprom->write(writeaddr,RECORD_HEADER1);writeaddr++;
    eeprom->write(writeaddr,RECORD_HEADER2);writeaddr++;
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeaddr,writePtr,sizeof(val)); 
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SettingsClass::read8(uint32_t addr, uint8_t& val) // читаем 1 байт из памяти
{
  val = 0xFF;
  
  if(!eeprom)
  {
    return false;
  }

  // читаем заголовки
  uint32_t readAddr = addr;
  uint8_t header1, header2;
  header1 = eeprom->read(readAddr);readAddr++;
  header2 = eeprom->read(readAddr);readAddr++;

  if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2) // если заголовки хорошие, то читаем сохранённое значение
  {
     uint8_t* writePtr = (uint8_t*)&val;
     eeprom->read(readAddr,writePtr,sizeof(val));
     return true;
  }
  #ifdef _SETTINGS_LOAD_DEBUG
  else
  {
      Serial.print("Settings:read8 - BAD HEADER AT ADDRESS ");
      Serial.println(addr);
  }
  #endif

  return false;      
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SettingsClass::read16(uint32_t addr, uint16_t& val) // читаем два байта из памяти
{
  val = 0xFFFF;
  
  if(!eeprom)
  {
    return false;
  }

  // читаем заголовки
  uint32_t readAddr = addr;
  uint8_t header1, header2;
  header1 = eeprom->read(readAddr);readAddr++;
  header2 = eeprom->read(readAddr);readAddr++;

  if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2) // если заголовки хорошие - читаем сохранённое значение
  {
     uint8_t* writePtr = (uint8_t*)&val;
     eeprom->read(readAddr,writePtr,sizeof(val));
     return true;//(val != 0xFFFF);
  }
  #ifdef _SETTINGS_LOAD_DEBUG
  else
  {
      Serial.print("Settings:read16 - BAD HEADER AT ADDRESS ");
      Serial.println(addr);
  }
  #endif

  return false;      
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SettingsClass::read32(uint32_t addr, uint32_t& val) // читаем 4 байта из памяти
{
  val = 0xFFFFFFFF;
  
  if(!eeprom)
  {
    return false;
  }

  // читаем заголовки
  uint32_t readAddr = addr;
  uint8_t header1, header2;
  header1 = eeprom->read(readAddr);readAddr++;
  header2 = eeprom->read(readAddr);readAddr++;

  if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2) // если заголовки хорошие - читаем сохранённое значение
  {
     uint8_t* writePtr = (uint8_t*)&val;
     eeprom->read(readAddr,writePtr,sizeof(val));
     return true;//(val != 0xFFFFFFFF);
  }
  #ifdef _SETTINGS_LOAD_DEBUG
  else
  {
      Serial.print("Settings:read32 - BAD HEADER AT ADDRESS ");
      Serial.println(addr);
  }
  #endif

  return false;      
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::reloadSettings() // перезагружаем настройки
{

  if(eeprom)
  {
    Modbus.setup(); // настраиваем регистры MODBUS
    
     // читаем задержку реле
     if(!read32(RELAY_DELAY_STORE_ADDRESS,relayDelay))
     {
       relayDelay = RELAY_WANT_DATA_AFTER;
     }  

      Modbus.set(MODBUS_REG_RDELAY1,(word)(relayDelay >> 16));
      Modbus.set(MODBUS_REG_RDELAY2,(word)(relayDelay & 0xffff));

      // читаем задержку АСУ
      if(!read16(ACS_DELAY_STORE_ADDRESS,acsDelay))
      {
        acsDelay = ACS_SIGNAL_DELAY;
      }

      Modbus.set(MODBUS_REG_ACSDELAY,acsDelay);

      // читаем коэффициент тока
      if(!read32(CURRENT_COEFF_STORE_ADDRESS,currentCoeff))
      {
        currentCoeff = CURRENT_COEFF_DEFAULT;
      }

      Modbus.set(MODBUS_REG_CCOEFF1,(word)(currentCoeff >> 16));
      Modbus.set(MODBUS_REG_CCOEFF2,(word)(currentCoeff & 0xffff));
      
      // читаем пропуск
      if(!read32(SKIP_COUNTER_STORE_ADDRESS,skipCounter))
      {
        skipCounter = INTERRUPT_SKIP_COUNTER;
      }

      Modbus.set(MODBUS_REG_SKIPC1,(word)(skipCounter >> 16));
      Modbus.set(MODBUS_REG_SKIPC2,(word)(skipCounter & 0xffff));
      

      // читаем сохранённые импульсы
      if(!read16(COUNT_PULSES_STORE_ADDRESS,channelPulses))
      {
        channelPulses = 0;
      }

      Modbus.set(MODBUS_REG_PULSES,channelPulses);

      // читаем моторесурс
      if(!read32(MOTORESOURCE_STORE_ADDRESS,motoresource))
      {
        motoresource = 0;
      }

      Modbus.set(MODBUS_REG_MOTORESOURCE1,(word)(motoresource >> 16));
      Modbus.set(MODBUS_REG_MOTORESOURCE2,(word)(motoresource & 0xffff));

      // читаем максимальный моторесурс
      if(!read32(MOTORESOURCE_MAX_STORE_ADDRESS,motoresourceMax))
      {
        motoresourceMax = 0;
      }

      Modbus.set(MODBUS_REG_MOTORESOURCE_MAX1,(word)(motoresourceMax >> 16));
      Modbus.set(MODBUS_REG_MOTORESOURCE_MAX2,(word)(motoresourceMax & 0xffff));

      // читаем дельту импульсов
     if(!read8(CHANNEL_PULSES_DELTA_ADDRESS,channelDelta))
     {
      channelDelta = 0;
     }

      Modbus.set(MODBUS_REG_PULSES_DELTA,channelDelta);
     
      // читаем верхнюю границу трансформатора
      if(!read32(TRANSFORMER_HIGH_BORDER_STORE_ADDRESS,transformerHighBorder))
      {
        transformerHighBorder = TRANSFORMER_HIGH_DEFAULT_BORDER;
      }

      Modbus.set(MODBUS_REG_THIGH_BORDER1,(word)(transformerHighBorder >> 16));
      Modbus.set(MODBUS_REG_THIGH_BORDER2,(word)(transformerHighBorder & 0xffff));

      // читаем нижнюю границу трансформатора
      if(!read32(TRANSFORMER_LOW_BORDER_STORE_ADDRESS,transformerLowBorder))
      {
        transformerLowBorder = TRANSFORMER_LOW_DEFAULT_BORDER;
      }

      Modbus.set(MODBUS_REG_TLOW_BORDER1,(word)(transformerLowBorder >> 16));
      Modbus.set(MODBUS_REG_TLOW_BORDER2,(word)(transformerLowBorder & 0xffff));

       // читаем дельту времени импульса для эталона
      if(!read32(ETHALON_DELTA_STORE_ADDRESS,ethalonPulseDelta))
      {
        ethalonPulseDelta = 50;
      }

      Modbus.set(MODBUS_REG_ETHALON_PULSES_DELTA1,(word)(ethalonPulseDelta >> 16));
      Modbus.set(MODBUS_REG_ETHALON_PULSES_DELTA2,(word)(ethalonPulseDelta & 0xffff));

      // читаем флаги выдачи сигналов на АСУ ТП
      if(!read8(ASU_TP_SIGNALS_ADDRESS,asuTpFlags))
      {
        asuTpFlags = 0xFF; // выдаём сигналы на все линии
      }

      Modbus.set(MODBUS_REG_ASUTPFLAGS,asuTpFlags);
      
       // читаем время ожидания
      if(!read32(MAX_IDLE_TIME_STORE_ADDRESS,maxIdleTime))
      {
        maxIdleTime = INTERRUPT_MAX_IDLE_TIME;
      }

      Modbus.set(MODBUS_REG_MAXIDLETIME1,(word)(maxIdleTime >> 16));
      Modbus.set(MODBUS_REG_MAXIDLETIME2,(word)(maxIdleTime & 0xffff));

       // читаем длину перемещения штанги
      if(!read32(ROD_MOVE_LENGTH_STORE_ADDRESS,rodMoveLength))
      {
        rodMoveLength = DEFAULT_ROD_MOVE_LENGTH;
      }
      
      Modbus.set(MODBUS_REG_RODMOVELEN1,(word)(rodMoveLength >> 16));
      Modbus.set(MODBUS_REG_RODMOVELEN2,(word)(rodMoveLength & 0xffff));


       // читаем дельту импульсов
     if(!read8(MODBUS_SLAVE_ADDRESS,modbus_slave_id))
     {
      modbus_slave_id = 1;
     }

      Modbus.setID(modbus_slave_id);
      
  } // if(eeprom)  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::begin() // начинаем работу
{
  eeprom = new EEPROM_CLASS(Wire1);
  reloadSettings();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::set3V3RawVoltage(uint16_t raw)
{
   voltage3V3.raw = raw;  
   voltage3V3.voltage = voltage3V3.raw*(3.3 / 4096 * 2);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::set5VRawVoltage(uint16_t raw)
{
  voltage5V.raw = raw; 
  voltage5V.voltage = voltage5V.raw*(2.4 / 4096 * 2);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::set200VRawVoltage(uint16_t raw)
{
  voltage200V.raw = raw; 
  voltage200V.voltage = voltage200V.raw*(2.4 / 4096 * 100);    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::update() // обновляемся
{
#ifndef _CORE_TEMP_OFF

  uint32_t now = millis();
  
  if(now - timer > DATA_MEASURE_THRESHOLD)
  {
    timer = now;
    coreTemp = RealtimeClock.getTemperature();  
  }

#endif // !_CORE_TEMP_OFF  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setRodMoveLength(uint32_t val)
{
  rodMoveLength = val;
  write32(ROD_MOVE_LENGTH_STORE_ADDRESS,rodMoveLength);

  Modbus.set(MODBUS_REG_RODMOVELEN1,(word)(rodMoveLength >> 16));
  Modbus.set(MODBUS_REG_RODMOVELEN2,(word)(rodMoveLength & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::getAsuTpFlags()
{
  return asuTpFlags;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setModbusSlaveID(uint8_t val)
{
  modbus_slave_id = val;
  write8(MODBUS_SLAVE_ADDRESS,modbus_slave_id);

  Modbus.setID(modbus_slave_id);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setAsuTpFlags(uint8_t val)
{
  asuTpFlags = val;
  write8(ASU_TP_SIGNALS_ADDRESS,asuTpFlags);

  Modbus.set(MODBUS_REG_ASUTPFLAGS,asuTpFlags);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getEthalonPulseDelta()
{
  return ethalonPulseDelta;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setEthalonPulseDelta(uint32_t val)
{
  ethalonPulseDelta = val;
  write32(ETHALON_DELTA_STORE_ADDRESS,ethalonPulseDelta);

  Modbus.set(MODBUS_REG_ETHALON_PULSES_DELTA1,(word)(ethalonPulseDelta >> 16));
  Modbus.set(MODBUS_REG_ETHALON_PULSES_DELTA2,(word)(ethalonPulseDelta & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMaxIdleTime(uint32_t val)
{
  maxIdleTime = val;
  write32(MAX_IDLE_TIME_STORE_ADDRESS,maxIdleTime);  

  Modbus.set(MODBUS_REG_MAXIDLETIME1,(word)(maxIdleTime >> 16));
  Modbus.set(MODBUS_REG_MAXIDLETIME2,(word)(maxIdleTime & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getCurrentCoeff()
{
  return currentCoeff;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setCurrentCoeff(uint32_t val)
{
  currentCoeff = val;
  write32(CURRENT_COEFF_STORE_ADDRESS,currentCoeff);

  Modbus.set(MODBUS_REG_CCOEFF1,(word)(currentCoeff >> 16));
  Modbus.set(MODBUS_REG_CCOEFF2,(word)(currentCoeff & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getRelayDelay()
{
  return relayDelay;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setRelayDelay(uint32_t val)
{
  relayDelay = val;
  write32(RELAY_DELAY_STORE_ADDRESS,relayDelay);

  Modbus.set(MODBUS_REG_RDELAY1,(word)(relayDelay >> 16));
  Modbus.set(MODBUS_REG_RDELAY2,(word)(relayDelay & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::getACSDelay()
{
  return acsDelay;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setACSDelay(uint16_t val)
{
  acsDelay = val;
  write16(ACS_DELAY_STORE_ADDRESS,acsDelay);

  Modbus.set(MODBUS_REG_ACSDELAY,acsDelay);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setSkipCounter(uint32_t val)
{
  skipCounter = val;
  write32(SKIP_COUNTER_STORE_ADDRESS,skipCounter);

  Modbus.set(MODBUS_REG_SKIPC1,(word)(skipCounter >> 16));
  Modbus.set(MODBUS_REG_SKIPC2,(word)(skipCounter & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getTransformerLowBorder()
{
  return transformerLowBorder;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getTransformerHighBorder()
{
  return transformerHighBorder;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setTransformerLowBorder(uint32_t val)
{
  transformerLowBorder = val;
  adcSampler.setLowBorder(val);
  
  write32(TRANSFORMER_LOW_BORDER_STORE_ADDRESS,transformerLowBorder);

  Modbus.set(MODBUS_REG_TLOW_BORDER1,(word)(transformerLowBorder >> 16));
  Modbus.set(MODBUS_REG_TLOW_BORDER2,(word)(transformerLowBorder & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setTransformerHighBorder(uint32_t val)
{
  transformerHighBorder = val;
  adcSampler.setHighBorder(val);

  write32(TRANSFORMER_HIGH_BORDER_STORE_ADDRESS,transformerHighBorder);

  Modbus.set(MODBUS_REG_THIGH_BORDER1,(word)(transformerHighBorder >> 16));
  Modbus.set(MODBUS_REG_THIGH_BORDER2,(word)(transformerHighBorder & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getMotoresource()
{
  return motoresource; 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMotoresource(uint32_t val)
{
  motoresource = val;
  write32(MOTORESOURCE_STORE_ADDRESS,motoresource);  

  Modbus.set(MODBUS_REG_MOTORESOURCE1,(word)(motoresource >> 16));
  Modbus.set(MODBUS_REG_MOTORESOURCE2,(word)(motoresource & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getMotoresourceMax()
{
  return motoresourceMax;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMotoresourceMax(uint32_t val)
{
  motoresourceMax = val;
  write32(MOTORESOURCE_MAX_STORE_ADDRESS,motoresourceMax);

  Modbus.set(MODBUS_REG_MOTORESOURCE_MAX1,(word)(motoresourceMax >> 16));
  Modbus.set(MODBUS_REG_MOTORESOURCE_MAX2,(word)(motoresourceMax & 0xffff));
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::getPulses()
{
  return channelPulses; 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setPulses(uint16_t val)
{  
  channelPulses = val;
  write16(COUNT_PULSES_STORE_ADDRESS,channelPulses);

  Modbus.set(MODBUS_REG_PULSES,channelPulses);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::getPulsesDelta()
{
  return channelDelta;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setPulsesDelta(uint8_t val)
{
  channelDelta = val;
  write8(CHANNEL_PULSES_DELTA_ADDRESS,channelDelta);

  Modbus.set(MODBUS_REG_PULSES_DELTA,channelDelta);
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

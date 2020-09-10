//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Settings.h"
#include "CONFIG.h"
#include "ConfigPin.h"
#include "ADCSampler.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass Settings;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SettingsClass::SettingsClass()
{
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

#ifndef DISABLE_CATCH_ENCODER_DIRECTION
  rodDirection = rpBroken;
#else
  rodDirection = rpUp;
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::getUUID(const char* passedUUID)
{
    String savedUUID;

    if(!eeprom)
    {
      return savedUUID;
    }
    
    uint16_t addr = UUID_STORE_ADDRESS;
    uint8_t header1 = eeprom->read(addr); addr++;
    uint8_t header2 = eeprom->read(addr); addr++;
    uint8_t header3 = eeprom->read(addr); addr++;

    if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2 && header3 == RECORD_HEADER3))
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
      savedUUID += (char) eeprom->read(addr); addr++;
    }

    return savedUUID;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write8(int addr, uint8_t val)
{
  if(!eeprom)
  {
    return;    
  }

    int writeaddr = addr;
    eeprom->write(writeaddr,RECORD_HEADER1);writeaddr++;
    eeprom->write(writeaddr,RECORD_HEADER2);writeaddr++;
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeaddr,writePtr,sizeof(val)); 
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write16(int addr, uint16_t val)
{
  if(!eeprom)
  {
    return;    
  }

    int writeaddr = addr;
    eeprom->write(writeaddr,RECORD_HEADER1);writeaddr++;
    eeprom->write(writeaddr,RECORD_HEADER2);writeaddr++;
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeaddr,writePtr,sizeof(val)); 
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::write32(int addr, uint32_t val)
{
  if(!eeprom)
  {
    return;    
  }

    int writeaddr = addr;
    eeprom->write(writeaddr,RECORD_HEADER1);writeaddr++;
    eeprom->write(writeaddr,RECORD_HEADER2);writeaddr++;
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeaddr,writePtr,sizeof(val)); 
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SettingsClass::read8(int addr, uint8_t& val)
{
  val = 0xFF;
  
  if(!eeprom)
  {
    return false;
  }

  int readAddr = addr;
  uint8_t header1, header2;
  header1 = eeprom->read(readAddr);readAddr++;
  header2 = eeprom->read(readAddr);readAddr++;

  if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2)
  {
     uint8_t* writePtr = (uint8_t*)&val;
     eeprom->read(readAddr,writePtr,sizeof(val));
     return true;//(val != 0xFF);
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
bool SettingsClass::read16(int addr, uint16_t& val)
{
  val = 0xFFFF;
  
  if(!eeprom)
  {
    return false;
  }

  int readAddr = addr;
  uint8_t header1, header2;
  header1 = eeprom->read(readAddr);readAddr++;
  header2 = eeprom->read(readAddr);readAddr++;

  if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2)
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
bool SettingsClass::read32(int addr, uint32_t& val)
{
  val = 0xFFFFFFFF;
  
  if(!eeprom)
  {
    return false;
  }

  int readAddr = addr;
  uint8_t header1, header2;
  header1 = eeprom->read(readAddr);readAddr++;
  header2 = eeprom->read(readAddr);readAddr++;

  if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2)
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
void SettingsClass::reloadSettings()
{

  if(eeprom)
  {
     // читаем задержку реле
     if(!read32(RELAY_DELAY_STORE_ADDRESS,relayDelay))
     {
       relayDelay = RELAY_WANT_DATA_AFTER;
     }   

      // читаем задержку АСУ
      if(!read16(ACS_DELAY_STORE_ADDRESS,acsDelay))
      {
        acsDelay = ACS_SIGNAL_DELAY;
      }

      // читаем коэффициент тока
      if(!read32(CURRENT_COEFF_STORE_ADDRESS,currentCoeff))
      {
        currentCoeff = CURRENT_COEFF_DEFAULT;
      }
      
      // читаем пропуск
      if(!read32(SKIP_COUNTER_STORE_ADDRESS,skipCounter))
      {
        skipCounter = INTERRUPT_SKIP_COUNTER;
      }

      // читаем сохранённые импульсы
      if(!read16(COUNT_PULSES_STORE_ADDRESS,channelPulses))
      {
        channelPulses = 0;
      }

      // читаем моторесурс
      if(!read32(MOTORESOURCE_STORE_ADDRESS,motoresource))
      {
        motoresource = 0;
      }

      // читаем максимальный моторесурс
      if(!read32(MOTORESOURCE_MAX_STORE_ADDRESS,motoresourceMax))
      {
        motoresourceMax = 0;
      }

      // читаем дельту импульсов
     if(!read8(CHANNEL_PULSES_DELTA_ADDRESS,channelDelta))
     {
      channelDelta = 0;
     }
     
      // читаем верхнюю границу трансформатора
      if(!read32(TRANSFORMER_HIGH_BORDER_STORE_ADDRESS,transformerHighBorder))
      {
        transformerHighBorder = TRANSFORMER_HIGH_DEFAULT_BORDER;
      }

      // читаем нижнюю границу трансформатора
      if(!read32(TRANSFORMER_LOW_BORDER_STORE_ADDRESS,transformerLowBorder))
      {
        transformerLowBorder = TRANSFORMER_LOW_DEFAULT_BORDER;
      }

       // читаем дельту времени импульса для эталона
      if(!read32(ETHALON_DELTA_STORE_ADDRESS,ethalonPulseDelta))
      {
        ethalonPulseDelta = 50;
      }
      
  } // if(eeprom)  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::begin()
{
#ifndef _WIRE1_OFF  
  eeprom = new EEPROM_CLASS(Wire1);
#endif  

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
void SettingsClass::update()
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
uint32_t SettingsClass::getEthalonPulseDelta()
{
  return ethalonPulseDelta;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setEthalonPulseDelta(uint32_t val)
{
  ethalonPulseDelta = val;
  write32(ETHALON_DELTA_STORE_ADDRESS,ethalonPulseDelta);
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
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setSkipCounter(uint32_t val)
{
  skipCounter = val;
  write32(SKIP_COUNTER_STORE_ADDRESS,skipCounter);
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
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setTransformerHighBorder(uint32_t val)
{
  transformerHighBorder = val;
  adcSampler.setHighBorder(val);

  write32(TRANSFORMER_HIGH_BORDER_STORE_ADDRESS,transformerHighBorder);
  
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
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

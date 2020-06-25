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
void SettingsClass::begin()
{
#ifndef _WIRE1_OFF  
  eeprom = new AT24C128(Wire1);
#endif  

  if(eeprom)
  {
      int readAddr = RELAY_DELAY_STORE_ADDRESS;
      uint8_t header1, header2;
      header1 = eeprom->read(readAddr++);
      header2 = eeprom->read(readAddr++);
    
      if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2)
      {
        uint8_t* writePtr = (uint8_t*)&relayDelay;
        eeprom->read(readAddr,writePtr,sizeof(uint32_t));
      }
      else
      {
        relayDelay = RELAY_WANT_DATA_AFTER;
      }
    
      readAddr = ACS_DELAY_STORE_ADDRESS;
      header1 = eeprom->read(readAddr++);
      header2 = eeprom->read(readAddr++);
    
      if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2)
      {
        uint8_t* writePtr = (uint8_t*)&acsDelay;
        eeprom->read(readAddr,writePtr,sizeof(uint16_t));
      }
      else
      {
        acsDelay = ACS_SIGNAL_DELAY;
      }

      readAddr = CURRENT_COEFF_STORE_ADDRESS;
      header1 = eeprom->read(readAddr++);
      header2 = eeprom->read(readAddr++);
    
      if(RECORD_HEADER1 == header1 && RECORD_HEADER2 == header2)
      {
        uint8_t* writePtr = (uint8_t*)&currentCoeff;
        eeprom->read(readAddr,writePtr,sizeof(uint32_t));
      }
      else
      {
        currentCoeff = CURRENT_COEFF_DEFAULT;      
      }
    
      skipCounter = readSkipCounter();
  
  } // if(eeprom)
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
uint32_t SettingsClass::getCurrentCoeff()
{
  return currentCoeff;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setCurrentCoeff(uint32_t val)
{
  currentCoeff = val;

  if(eeprom)
  {
      int writeaddr = CURRENT_COEFF_STORE_ADDRESS;
      eeprom->write(writeaddr++,RECORD_HEADER1);
      eeprom->write(writeaddr++,RECORD_HEADER2);
      uint8_t* writePtr = (uint8_t*)&val;
      eeprom->write(writeaddr,writePtr,sizeof(uint32_t)); 
  }

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

  if(eeprom)
  {
      int writeaddr = RELAY_DELAY_STORE_ADDRESS;
      eeprom->write(writeaddr++,RECORD_HEADER1);
      eeprom->write(writeaddr++,RECORD_HEADER2);
      uint8_t* writePtr = (uint8_t*)&val;
      eeprom->write(writeaddr,writePtr,sizeof(uint32_t)); 
  }

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

  if(eeprom)
  {
    int writeaddr = ACS_DELAY_STORE_ADDRESS;
    eeprom->write(writeaddr++,RECORD_HEADER1);
    eeprom->write(writeaddr++,RECORD_HEADER2);
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeaddr,writePtr,sizeof(uint16_t)); 
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::readSkipCounter()
{
  if(!eeprom)
  {
    return INTERRUPT_SKIP_COUNTER;
  }

  int readAddr = SKIP_COUNTER_STORE_ADDRESS;
  uint8_t header1 = eeprom->read(readAddr++);
  uint8_t header2 = eeprom->read(readAddr++);

  if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2))
  {
    return INTERRUPT_SKIP_COUNTER;
  }
  
  uint32_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(readAddr,writePtr,sizeof(uint32_t));
  
  if(result == 0xFFFFFFFF)
    result = INTERRUPT_SKIP_COUNTER;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setSkipCounter(uint32_t val)
{
  skipCounter = val;

  if(eeprom)
  {
    int writeAddr = SKIP_COUNTER_STORE_ADDRESS;
    eeprom->write(writeAddr++,RECORD_HEADER1);
    eeprom->write(writeAddr++,RECORD_HEADER2);
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeAddr,writePtr,sizeof(uint32_t)); 
  }

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getTransformerLowBorder()
{
  if(!eeprom)
  {
    return TRANSFORMER_LOW_DEFAULT_BORDER;
  }

  int readAddr = TRANSFORMER_LOW_BORDER_STORE_ADDRESS;
  uint8_t header1 = eeprom->read(readAddr++);
  uint8_t header2 = eeprom->read(readAddr++);

  if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2))
  {
    return TRANSFORMER_LOW_DEFAULT_BORDER;
  }

  uint32_t result = 0;
  
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(readAddr,writePtr,sizeof(uint32_t));
  
  if(result == 0xFFFFFFFF)
    result = TRANSFORMER_LOW_DEFAULT_BORDER;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getTransformerHighBorder()
{
  if(!eeprom)
  {
    return TRANSFORMER_HIGH_DEFAULT_BORDER;
  }

  int readAddr = TRANSFORMER_HIGH_BORDER_STORE_ADDRESS;
  uint8_t header1 = eeprom->read(readAddr++);
  uint8_t header2 = eeprom->read(readAddr++);

  if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2))
  {
    return TRANSFORMER_HIGH_DEFAULT_BORDER;
  }
  
  uint32_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(readAddr,writePtr,sizeof(uint32_t));
  
  if(result == 0xFFFFFFFF)
    result = TRANSFORMER_HIGH_DEFAULT_BORDER;

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setTransformerLowBorder(uint32_t val)
{
  if(eeprom)
  {
    int writeAddr = TRANSFORMER_LOW_BORDER_STORE_ADDRESS;
    eeprom->write(writeAddr++,RECORD_HEADER1);
    eeprom->write(writeAddr++,RECORD_HEADER2);
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeAddr,writePtr,sizeof(uint32_t)); 
  }

   adcSampler.setLowBorder(val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setTransformerHighBorder(uint32_t val)
{
  if(eeprom)
  {
    int writeAddr = TRANSFORMER_HIGH_BORDER_STORE_ADDRESS;
    eeprom->write(writeAddr++,RECORD_HEADER1);
    eeprom->write(writeAddr++,RECORD_HEADER2);
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(writeAddr,writePtr,sizeof(uint32_t));
  }

   adcSampler.setHighBorder(val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getMotoresource(uint8_t channelNum)
{
  if(!eeprom)
  {
    return 0;
  }
  
  uint16_t addr = 0;
  
  switch(channelNum)
  {
	default:
		addr = MOTORESOURCE_STORE_ADDRESS1;
    break;
    
  }

  uint8_t header1 = eeprom->read(addr++);
  uint8_t header2 = eeprom->read(addr++);  

  if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2))
  {
    setMotoresource(channelNum,0);
    return 0;
  }  

  uint32_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(addr,writePtr,sizeof(uint32_t));

  if(result == 0xFFFFFFFF)
  {
    result = 0;
    setMotoresource(channelNum,result);
  }

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMotoresource(uint8_t channelNum, uint32_t val)
{
  if(!eeprom)
  {
    return;
  }
  
  uint16_t addr = 0;
  switch(channelNum)
  {
	default:
		addr = MOTORESOURCE_STORE_ADDRESS1;
    break;
    
  }
    eeprom->write(addr++,RECORD_HEADER1);
    eeprom->write(addr++,RECORD_HEADER2);
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(addr,writePtr,sizeof(uint32_t));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t SettingsClass::getMotoresourceMax(uint8_t channelNum)
{
  if(!eeprom)
  {
    return 0;
  }
  
  uint16_t addr = 0;
  switch(channelNum)
  {
	default:
		addr = MOTORESOURCE_MAX_STORE_ADDRESS1;
    break;
    
  }

  uint8_t header1 = eeprom->read(addr++);
  uint8_t header2 = eeprom->read(addr++);  

  if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2))
  {
    setMotoresourceMax(channelNum,0);
    return 0;
  }  

  uint32_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(addr,writePtr,sizeof(uint32_t));

  if(result == 0xFFFFFFFF)
  {
    result = 0;
    setMotoresourceMax(channelNum,result);
  }

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setMotoresourceMax(uint8_t channelNum, uint32_t val)
{
  if(!eeprom)
  {
    return;
  }
  
  uint16_t addr = 0;
  switch(channelNum)
  {
	default:
		addr = MOTORESOURCE_MAX_STORE_ADDRESS1;
    break;
    
  }

    eeprom->write(addr++,RECORD_HEADER1);
    eeprom->write(addr++,RECORD_HEADER2);
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(addr,writePtr,sizeof(uint32_t));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::getChannelPulses(uint8_t channelNum)
{
  if(!eeprom)
  {
    return 0;
  }
  
  uint16_t addr = 0;
  switch(channelNum)
  {
	default:
		addr = COUNT_PULSES_STORE_ADDRESS1;
    break;
    
  }

  uint8_t header1 = eeprom->read(addr++);
  uint8_t header2 = eeprom->read(addr++);  

  if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2))
  {
    setChannelPulses(channelNum,0);
    return 0;
  }  

  uint16_t result = 0;
  uint8_t* writePtr = (uint8_t*)&result;
  eeprom->read(addr,writePtr,sizeof(uint16_t));

  if(result == 0xFFFF)
  {
    result = 0;
    setChannelPulses(channelNum,result);
  }

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setChannelPulses(uint8_t channelNum, uint16_t val)
{
  if(!eeprom)
  {
    return;
  }
  uint16_t addr = 0;
  switch(channelNum)
  {
	default:
		addr = COUNT_PULSES_STORE_ADDRESS1;
    break;
    
  }
  
    eeprom->write(addr++,RECORD_HEADER1);
    eeprom->write(addr++,RECORD_HEADER2);
    
    uint8_t* writePtr = (uint8_t*)&val;
    eeprom->write(addr,writePtr,sizeof(uint16_t));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t SettingsClass::getChannelDelta(uint8_t channelNum)
{
  if(!eeprom)
  {
    return 0;  
  }
  uint16_t addr = 0;
  switch(channelNum)
  {
	default:
		addr = CHANNEL_PULSES_DELTA_ADDRESS1;
    break;
    
  }

  uint8_t header1 = eeprom->read(addr++);
  uint8_t header2 = eeprom->read(addr++);  

  if(!(header1 == RECORD_HEADER1 && header2 == RECORD_HEADER2))
  {
    setChannelDelta(channelNum,0);
    return 0;
  }  

  uint8_t result = eeprom->read(addr);

  if(result == 0xFF)
  {
    result = 0;
    setChannelDelta(channelNum,result);
  }

  return result;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setChannelDelta(uint8_t channelNum, uint8_t val)
{
  if(!eeprom)
  {
    return;
  }
  
  uint16_t addr = 0;
  switch(channelNum)
  {
	default:
		addr = CHANNEL_PULSES_DELTA_ADDRESS1;
    break;
    
  }

  eeprom->write(addr++,RECORD_HEADER1);
  eeprom->write(addr++,RECORD_HEADER2);
  eeprom->write(addr,val);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#include "RS485.h"
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------
RS485::RS485(Stream& s, uint8_t de,uint32_t tmout)
{
	dePin = de;
	receiveTimeout = tmout;
	workStream = &s;
	writePtr = 0;
	rsPacketPtr = (uint8_t*) &rs485Packet;
	dataReceived = NULL;
	dataHandler = NULL;
}
//--------------------------------------------------------------------------------------------------
RS485::~RS485()
{
  delete [] dataReceived;
}
//--------------------------------------------------------------------------------------------------
void RS485::begin()
{
  DBGLN(F("RS485: begin."));
  pinMode(dePin,OUTPUT);
  switchToReceive();
}
//--------------------------------------------------------------------------------------------------
void RS485::switchToSend()
{
   digitalWrite(dePin,HIGH); // переводим контроллер RS-485 на передачу
}
//--------------------------------------------------------------------------------------------------
void RS485::switchToReceive()
{    
  digitalWrite(dePin,LOW); // переводим контроллер RS-485 на приём
}
//--------------------------------------------------------------------------------------------------
void RS485::send(RS485PacketType packetType, const uint8_t* data, uint16_t dataLength)
{
 // DBGLN(F("RS485: send data..."));
  
  RS485Packet outPacket;
  outPacket.packetType = packetType;
  outPacket.dataLength = dataLength;
  outPacket.dataCrc = crc8(data,dataLength);

  uint8_t* p = (uint8_t*)&outPacket;
  outPacket.packetCrc = crc8(p,sizeof(RS485Packet) - 1);

  switchToSend();

  workStream->write(p,sizeof(RS485Packet));
  workStream->write(data,dataLength);

  waitTransmitComplete();
  
  switchToReceive();

 // DBGLN(F("RS485: data was sent."));
  
}
//--------------------------------------------------------------------------------------------------
void RS485::waitTransmitComplete()
{
  workStream->flush(); 
}
//--------------------------------------------------------------------------------------------------
RS485Packet RS485::getDataReceived(uint8_t* &data)
{
    data = dataReceived;
    return rs485Packet;
}
//--------------------------------------------------------------------------------------------------
void RS485::clearReceivedData()
{
	delete[] dataReceived;
	dataReceived = NULL;
	rsPacketPtr = (uint8_t*)&rs485Packet;
	writePtr = 0;
	memset(&rs485Packet, 0, sizeof(rs485Packet));
}
//--------------------------------------------------------------------------------------------------
void RS485::update()
{
  switchToReceive();  
  
    while(workStream->available())
    {
      rsPacketPtr[writePtr++] = (uint8_t) workStream->read();
           
      if(gotRS485Packet())
      {
        processRS485Packet();
      }
    } // while 

  
}
//--------------------------------------------------------------------------------------------------
bool RS485::gotRS485Packet()
{
  // проверяем, есть ли у нас полный RS-485 пакет
  if(writePtr > ( sizeof(RS485Packet)-1 ))
  {
      // вычитали N байт из потока, надо проверить - не пакет ли это?
      if(!(rs485Packet.stx1 == STX1 && rs485Packet.stx2 == STX2))
      {
         // заголовок неправильный, ищем возможное начало пакета
         uint8_t readPtr = 0;
         bool startPacketFound = false;
         while(readPtr < sizeof(RS485Packet))
         {
           if(rsPacketPtr[readPtr] == STX1)
           {
            startPacketFound = true;
            break;
           }
            readPtr++;
         } // while
    
         if(!startPacketFound) // не нашли начало пакета
         {
            writePtr = 0; // сбрасываем указатель чтения и выходим
            return false;
         }
    
         if(readPtr == 0)
         {
          // стартовый байт заголовка найден, но он в нулевой позиции, следовательно - что-то пошло не так
            writePtr = 0; // сбрасываем указатель чтения и выходим
            return false;       
         } // if
    
         // начало пакета найдено, копируем всё, что после него, перемещая в начало буфера
         
         uint8_t thisWritePtr = 0;
         uint8_t bytesWritten = 0;
         while(readPtr < sizeof(RS485Packet) )
         {
          rsPacketPtr[thisWritePtr++] = rsPacketPtr[readPtr++];
          bytesWritten++;
         }

    
         writePtr = bytesWritten; // запоминаем, куда писать следующий байт

         return false;
             
      } // if
      else
      {
        // заголовок совпал, проверяем жопку
        if(!(rs485Packet.etx1 == ETX1 && rs485Packet.etx2 == ETX2))
        {
          // окончание неправильное, сбрасываем указатель чтения и выходим
          writePtr = 0;
          return false;
        }
  
        // жопка совпала, проверяем CRC пакета
        // данные мы получили, сразу обнуляем указатель записи, чтобы не забыть
        writePtr = 0;
    
        // проверяем контрольную сумму
        byte crc = crc8(rsPacketPtr,sizeof(RS485Packet) - 1);
        if(crc != rs485Packet.packetCrc)
        {
          // не сошлось, игнорируем
          DBGLN(F("RS485: BAD PACKET CRC!!!"));
          return false;
        }
        
        // CRC сошлось, пакет валидный
        return true;              
      } // else
  } // if
  
  return false;
}
//--------------------------------------------------------------------------------------------------
bool RS485::processRS485Packet()
{
   bool receiveResult = false;
   
    // у нас в пакете лежит длина данных, надо их вычитать из потока
//    DBG(F("RS485: DATA TO READ: "));
 //   DBGLN(rs485Packet.dataLength);

    uint16_t readed = 0;
    delete [] dataReceived;
    dataReceived = new uint8_t[rs485Packet.dataLength];
    memset(dataReceived,0,rs485Packet.dataLength);
    
    uint32_t startReadingTime = millis();
    bool hasTimeout = false;
    
    while(readed < rs485Packet.dataLength)
    {
      while(workStream->available() && readed < rs485Packet.dataLength)
      {
        dataReceived[readed++] = workStream->read();
        startReadingTime = millis();
      }

      if(millis() - startReadingTime >= receiveTimeout) // таймаут чтения
      {
        //DBGLN(F("RS485: RECEIVE TIMEOUT!!!"));
        hasTimeout = true;
        break;
      }
    } // while

    bool isCrcGood = false;
    
    if(rs485Packet.dataLength)
    {
        uint8_t dataCrc = crc8(dataReceived,rs485Packet.dataLength);
        
        if(dataCrc == rs485Packet.dataCrc)
        {
          isCrcGood = true;
		  /*
          DBG(F("RS485: DATA RECEIVED = "));
          #ifdef _DEBUG
            DEBUG_SERIAL.write(dataReceived,rs485Packet.dataLength);
            DEBUG_SERIAL.println();
          #endif        
		  */
        }
        else
        {
          DBGLN(F("RS485: BAD DATA CRC!!!"));
        }
    } // if(rs485Packet.dataLength)
    else
    {
      isCrcGood = true; // нет данных в пакете 
    }

   receiveResult = isCrcGood && !hasTimeout;
   if (receiveResult)
   {
	   if(dataHandler)
		   dataHandler(this);
   }
   else
   {
	   clearReceivedData();
   }
        
  return receiveResult;
}
//--------------------------------------------------------------------------------------------------
uint8_t RS485::crc8(const uint8_t *addr, uint16_t len)
{
  uint8_t crc = 0;
  while (len--) 
    {
    uint8_t inbyte = *addr++;
    for (uint8_t i = 8; i; i--)
      {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) 
        crc ^= 0x8C;
      inbyte >>= 1;
      }  // end of for
    }  // end of while
  return crc;  
}
//--------------------------------------------------------------------------------------------------



#pragma once
//--------------------------------------------------------------------------------------------------
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------
class RS485;
//--------------------------------------------------------------------------------------------------
extern "C" {
  void ON_RS485_INCOMING_DATA(RS485* Sender);
}
//--------------------------------------------------------------------------------------------------
#define STX1 0xAB
#define STX2 0xBA
#define ETX1 0xDE
#define ETX2 0xAD
//--------------------------------------------------------------------------------------------------
typedef enum
{
  rs485Ping = 10, // сообщение вида "ПРОВЕРКА СВЯЗИ", посылает модуль периодически для проверки связи
  rs485HasInterrupt, // сообщение вида "ЕСТЬ СРАБАТЫВАНИЕ ЗАЩИТЫ", посылает модуль по факту срабатывания защиты
  rs485InterruptData, // сообщение вида "ДАННЫЕ ПО ПРЕРЫВАНИЮ", посылает модуль по факту собирания списка прерываний
  
} RS485PacketType;
//--------------------------------------------------------------------------------------------------
#pragma pack(push,1)
struct RS485Packet
{
  uint8_t stx1;
  uint8_t stx2;
  uint8_t packetType;
  uint16_t dataLength;
  uint8_t dataCrc;
  uint8_t etx1;
  uint8_t etx2;
  uint8_t packetCrc;

  RS485Packet()
  {
    stx1 =  STX1;
    stx2 =  STX2;

    etx1 =  ETX1;
    etx2 =  ETX2;

    dataLength = 0;
    dataCrc = 0;
    packetCrc = 0;   
  }
  
};
#pragma pack(pop)
//--------------------------------------------------------------------------------------------------
class RS485
{
  public:
    RS485(Stream& s, uint8_t dePin, uint32_t receiveTimeout);
    ~RS485();    
    
    void begin();
    void update();

    void switchToSend();
    void switchToReceive();

    // отправляет пакет нужного типа
    void send(RS485PacketType packetType, const uint8_t* data, uint16_t dataLength);
 
    RS485Packet getDataReceived(uint8_t* &data);
    void clearReceivedData();
    
    uint8_t crc8(const uint8_t *addr, uint16_t len);

  private:
    
    void waitTransmitComplete();
    uint8_t dePin;
    Stream* workStream;

    uint8_t writePtr;
    RS485Packet rs485Packet;
    uint8_t* rsPacketPtr;
    uint32_t receiveTimeout;
   
    uint8_t* dataReceived;

    bool gotRS485Packet();
    bool processRS485Packet();
};
//--------------------------------------------------------------------------------------------------


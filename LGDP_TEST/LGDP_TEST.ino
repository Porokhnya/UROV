#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <Arduino.h>

/* raspberry pi pinout
 * +-----+-----+---------+------+---+-Model B1-+---+------+---------+-----+-----+
 * | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 * +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 * |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 * |   2 |   8 |   SDA.1 |   IN | 1 |  3 || 4  |   |      | 5v      |     |     |
 * |   3 |   9 |   SCL.1 |   IN | 1 |  5 || 6  |   |      | 0v      |     |     |
 * |   4 |   7 | GPIO. 7 |   IN | 1 |  7 || 8  | 1 | ALT0 | TxD     | 15  | 14  |
 * |     |     |      0v |      |   |  9 || 10 | 1 | ALT0 | RxD     | 16  | 15  |
 * |  17 |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO. 1 | 1   | 18  |
 * |  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |
 * |  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
 * |     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO. 5 | 5   | 24  |
 * |  10 |  12 |    MOSI |   IN | 0 | 19 || 20 |   |      | 0v      |     |     |
 * |   9 |  13 |    MISO |   IN | 0 | 21 || 22 | 0 | IN   | GPIO. 6 | 6   | 25  |
 * |  11 |  14 |    SCLK |   IN | 0 | 23 || 24 | 1 | IN   | CE0     | 10  | 8   |
 * |     |     |      0v |      |   | 25 || 26 | 1 | IN   | CE1     | 11  | 7   |
 * +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 * | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 * +-----+-----+---------+------+---+-Model B1-+---+------+---------+-----+-----+
 */
//gpios=reset:4,dc:18,wr:17,cs:27,db00:23,db01:24,db02:10,db03:9,db04:25,db05:11,db06:8,db07:7

//wiring pi pin definitions
#define CS PD7
#define reset PE6 //physical pin 7  HW-Reset active low
#define RS PD13    //physical pin 12 REGISTER SELECT Low: selects the index/status register. High: selects a control register.
#define WR PD5    //physical pin 11 write strobe (active low)
#define RD PD4    //physical pin 13 Read strobe (active Low)
#define DB0 PD14   //physical pin 16
#define DB1 PD15   //physical pin 18
#define DB2 PD0  //physical pin 19
#define DB3 PD1  //physical pin 21
#define DB4 PE7   //physical pin 22    
#define DB5 PE8  //physical pin 23
#define DB6 PE9  //physical pin 24
#define DB7 PE10  //physical pin 26

/*
 * The LGDP4524 has a 16-bit index register (IR); 
 * an 18-bit write-data register (WDR); 
 * and an 18-bit read-data register (RDR). 
 * 
 * The IR is the register to store index information from control registers and the
 * internal GRAM. 
 * The WDR is the register to temporarily store data to be written to control registers and
 * the internal GRAM. 
 * The RDR is the register to temporarily store data read from the GRAM. 
 * Data from the MPU to be written to the internal GRAM are first written to the WDR and then automatically written
 * to the internal GRAM in internal operation. 
 * Data are read via the RDR from the internal GRAM.
 * Therefore, invalid data are read out to the data bus when the LGDP4524 read the first data from the
 * internal GRAM. Valid data are read out after the LGDP4524 performs the second read operation.
 * 
 * 
 * RAM/instruction data read -> 16 bits MSB-byte first
 * RAM data write -> 18 bits
 * 
 * 
 */

/*
uint16_t read_control_register(void)
{
    
    pinMode (DB0, INPUT) ;
    pinMode (DB1, INPUT) ;
    pinMode (DB2, INPUT) ;
    pinMode (DB3, INPUT) ;
    pinMode (DB4, INPUT) ;
    pinMode (DB5, INPUT) ;
    pinMode (DB6, INPUT) ;
    pinMode (DB7, INPUT) ;//set inputs
    
    digitalWrite (WR, HIGH);
    digitalWrite (RS, HIGH);
    
    delayMicroseconds(1);
    digitalWrite (RD, LOW);      //reading after RD LOW
    delayMicroseconds(1);
    
    uint16_t control_reg = 0;
    //MSB read first
    if (digitalRead(DB7)) control_reg |= 0x8000;
    if (digitalRead(DB6)) control_reg |= 0x4000;
    if (digitalRead(DB5)) control_reg |= 0x2000;
    if (digitalRead(DB4)) control_reg |= 0x1000;
    if (digitalRead(DB3)) control_reg |= 0x0800;
    if (digitalRead(DB2)) control_reg |= 0x0400;
    if (digitalRead(DB1)) control_reg |= 0x0200;
    if (digitalRead(DB0)) control_reg |= 0x0100;
    
    digitalWrite (RD, HIGH);
    delayMicroseconds(1);
    digitalWrite (RD, LOW);
    delayMicroseconds(1);
    
    if (digitalRead(DB7)) control_reg |= 0x80;
    if (digitalRead(DB6)) control_reg |= 0x40;
    if (digitalRead(DB5)) control_reg |= 0x20;
    if (digitalRead(DB4)) control_reg |= 0x10;
    if (digitalRead(DB3)) control_reg |= 0x08;
    if (digitalRead(DB2)) control_reg |= 0x04;
    if (digitalRead(DB1)) control_reg |= 0x02;
    if (digitalRead(DB0)) control_reg |= 0x01;
    
    digitalWrite (RD, HIGH);
    
    return control_reg;
}
*/

void write_control_register(uint16_t reg_v)
{
    
    
    pinMode (DB0, OUTPUT) ;
    pinMode (DB1, OUTPUT) ;
    pinMode (DB2, OUTPUT) ;
    pinMode (DB3, OUTPUT) ;
    pinMode (DB4, OUTPUT) ;
    pinMode (DB5, OUTPUT) ;
    pinMode (DB6, OUTPUT) ;
    pinMode (DB7, OUTPUT) ;

    digitalWrite(CS,LOW);
    
    digitalWrite (RD, HIGH);
    digitalWrite (RS, HIGH);
    
    //MSB first
    if (reg_v & 0x8000) digitalWrite (DB7, HIGH); else digitalWrite (DB7, LOW);
    if (reg_v & 0x4000) digitalWrite (DB6, HIGH); else digitalWrite (DB6, LOW);
    if (reg_v & 0x2000) digitalWrite (DB5, HIGH); else digitalWrite (DB5, LOW);
    if (reg_v & 0x1000) digitalWrite (DB4, HIGH); else digitalWrite (DB4, LOW);
    if (reg_v & 0x0800) digitalWrite (DB3, HIGH); else digitalWrite (DB3, LOW);
    if (reg_v & 0x0400) digitalWrite (DB2, HIGH); else digitalWrite (DB2, LOW);
    if (reg_v & 0x0200) digitalWrite (DB1, HIGH); else digitalWrite (DB1, LOW);
    if (reg_v & 0x0100) digitalWrite (DB0, HIGH); else digitalWrite (DB0, LOW);
    
    
    delayMicroseconds(1);
    digitalWrite (WR, LOW);
    delayMicroseconds(1);
    digitalWrite (WR, HIGH);
    
    if (reg_v & 0x80) digitalWrite (DB7, HIGH); else digitalWrite (DB7, LOW);
    if (reg_v & 0x40) digitalWrite (DB6, HIGH); else digitalWrite (DB6, LOW);
    if (reg_v & 0x20) digitalWrite (DB5, HIGH); else digitalWrite (DB5, LOW);
    if (reg_v & 0x10) digitalWrite (DB4, HIGH); else digitalWrite (DB4, LOW);
    if (reg_v & 0x08) digitalWrite (DB3, HIGH); else digitalWrite (DB3, LOW);
    if (reg_v & 0x04) digitalWrite (DB2, HIGH); else digitalWrite (DB2, LOW);
    if (reg_v & 0x02) digitalWrite (DB1, HIGH); else digitalWrite (DB1, LOW);
    if (reg_v & 0x01) digitalWrite (DB0, HIGH); else digitalWrite (DB0, LOW);
    
    delayMicroseconds(1);
    digitalWrite (WR, LOW);
    delayMicroseconds(1);
    digitalWrite (WR, HIGH);

    digitalWrite(CS,HIGH);
    
}

void set_index_register(uint8_t index_R)
{
    digitalWrite(CS,LOW);
    
    digitalWrite (RS, LOW);
    digitalWrite (RD, HIGH);
    
    delayMicroseconds(1);
    digitalWrite (WR, LOW);
    delayMicroseconds(1);
    digitalWrite (WR, HIGH);    //transfer first byte ; value = doesnt matter
    
    
    if (index_R & 0x80) digitalWrite (DB7, HIGH); else digitalWrite (DB7, LOW);
    if (index_R & 0x40) digitalWrite (DB6, HIGH); else digitalWrite (DB6, LOW);
    if (index_R & 0x20) digitalWrite (DB5, HIGH); else digitalWrite (DB5, LOW);
    if (index_R & 0x10) digitalWrite (DB4, HIGH); else digitalWrite (DB4, LOW);
    if (index_R & 0x08) digitalWrite (DB3, HIGH); else digitalWrite (DB3, LOW);
    if (index_R & 0x04) digitalWrite (DB2, HIGH); else digitalWrite (DB2, LOW);
    if (index_R & 0x02) digitalWrite (DB1, HIGH); else digitalWrite (DB1, LOW);
    if (index_R & 0x01) digitalWrite (DB0, HIGH); else digitalWrite (DB0, LOW);
    
    delayMicroseconds(1);
    digitalWrite (WR, LOW);
    delayMicroseconds(1);
    digitalWrite (WR, HIGH);
    delayMicroseconds(1);

    digitalWrite(CS,HIGH);
    
}

void sync_data_trans_8bit (void)
{
  digitalWrite(CS,LOW);
    
    pinMode (DB0, OUTPUT) ;
    pinMode (DB1, OUTPUT) ;
    pinMode (DB2, OUTPUT) ;
    pinMode (DB3, OUTPUT) ;
    pinMode (DB4, OUTPUT) ;
    pinMode (DB5, OUTPUT) ;
    pinMode (DB6, OUTPUT) ;
    pinMode (DB7, OUTPUT) ;
    
    digitalWrite (DB7, LOW);
    digitalWrite (DB6, LOW);
    digitalWrite (DB5, LOW);
    digitalWrite (DB4, LOW);
    digitalWrite (DB3, LOW);
    digitalWrite (DB2, LOW);
    digitalWrite (DB1, LOW);
    digitalWrite (DB0, LOW);
    
    digitalWrite (RS, LOW);
    digitalWrite (RD, HIGH);
    
    digitalWrite (WR, LOW);
    delay(1);
    digitalWrite (WR, HIGH);    //1
    delay(1);
    digitalWrite (WR, LOW);
    delay(1);
    digitalWrite (WR, HIGH);   //2
    delay(1);
    digitalWrite (WR, LOW);
    delay(1);
    digitalWrite (WR, HIGH);    //3
    delay(1);
    digitalWrite (WR, LOW);
    delay(1);
    digitalWrite (WR, HIGH);    //4
    delay(1);

  digitalWrite(CS,HIGH);
}


void lcd_init (void)
{
    
    //  set_index_register(0x01);
    //  write_control_register(0x001B); // set Driver Output Control  
    //  set_index_register(0x02);
    // write_control_register(0x0600); // set 1 line inversion  //FLD = "01" //display flackert sonst
    set_index_register(0x03);
    write_control_register(0x8030); // TRI=1
    //   set_index_register(0x04);
    //   write_control_register(0x0000);
    //   set_index_register(0x08);
    //   write_control_register(0x0207); // set the back porch and front porch
    //   set_index_register(0x09);
    //  write_control_register(0x0000); //SET PTS -> 00
    //   set_index_register(0x0C);
    //   write_control_register(0x0000);
    
    //power supply on sequence--------------------------------------------
    set_index_register(0x11);
    write_control_register(0x0110); //VC=VCi * 1 // DC1 = DC0 = 1
    set_index_register(0x12);       //VRH = 
    write_control_register(0x001D); //PON = 1
    set_index_register(0x13);
    write_control_register(0x0C43); //VCM = VREG1OUT x 0.735    //VDV = VREG1OUT x 0.96
    
    set_index_register(0x10);
    write_control_register(0x0134); //DK = 1 /BT = 1    //AP -> 1
    
    delay(200); //stabilizing
    
    
    set_index_register(0x13);
    write_control_register(0x2C43); //add VCOMG = 1
    
    set_index_register(0x10);
    write_control_register(0x0130); //add DK = 0
    
    
    delay(100); //stabilizing
    
    
    set_index_register(0x0B);
    write_control_register(0x012C); //44 clocks per line // fosc/2  // REV BIT ??
    set_index_register(0x71);
    write_control_register(0x0040); //T8CL = 1
    set_index_register(0x72);
    write_control_register(0x0020); //MVCI = 1
    set_index_register(0x10);
    write_control_register(0x4130); //SAP = 1.0
    
    //display on sequence--------------------------------------------
    set_index_register(0x07);
    write_control_register(0x0001); //D1-D0 = 1
    
    delay(200);
    
    write_control_register(0x0021); //D1-D0 = 1 //GON= 1
    write_control_register(0x0023); //D1-D0 = 11 //GON= 1
    delay(200);
    
    write_control_register(0x1033); //D1-D0 = 11 //GON= 1 // DTE = 1 // REV = 1
    
   }

void WRITE_GRAM_DATA(uint8_t red, uint8_t green, uint8_t blue)
{
    
    digitalWrite(CS,LOW);
    
    digitalWrite (RD, HIGH);
    digitalWrite (RS, HIGH);
    
    if (red & 0x20) digitalWrite (DB7, HIGH); else digitalWrite (DB7, LOW);
    if (red & 0x10) digitalWrite (DB6, HIGH); else digitalWrite (DB6, LOW);
    if (red & 0x8) digitalWrite (DB5, HIGH); else digitalWrite (DB5, LOW);
    if (red & 0x4) digitalWrite (DB4, HIGH); else digitalWrite (DB4, LOW);
    if (red & 0x2) digitalWrite (DB3, HIGH); else digitalWrite (DB3, LOW);
    if (red & 0x1) digitalWrite (DB2, HIGH); else digitalWrite (DB2, LOW);
    
    delayMicroseconds(1);
    digitalWrite (WR, LOW);
    delayMicroseconds(1);
    digitalWrite (WR, HIGH);
    
    if (green & 0x20) digitalWrite (DB7, HIGH); else digitalWrite (DB7, LOW);
    if (green & 0x10) digitalWrite (DB6, HIGH); else digitalWrite (DB6, LOW);
    if (green & 0x8) digitalWrite (DB5, HIGH); else digitalWrite (DB5, LOW);
    if (green & 0x4) digitalWrite (DB4, HIGH); else digitalWrite (DB4, LOW);
    if (green & 0x2) digitalWrite (DB3, HIGH); else digitalWrite (DB3, LOW);
    if (green & 0x1) digitalWrite (DB2, HIGH); else digitalWrite (DB2, LOW);
    
    delayMicroseconds(1);
    digitalWrite (WR, LOW);
    delayMicroseconds(1);
    digitalWrite (WR, HIGH);
    
    if (blue & 0x20) digitalWrite (DB7, HIGH); else digitalWrite (DB7, LOW);
    if (blue & 0x10) digitalWrite (DB6, HIGH); else digitalWrite (DB6, LOW);
    if (blue & 0x8) digitalWrite (DB5, HIGH); else digitalWrite (DB5, LOW);
    if (blue & 0x4) digitalWrite (DB4, HIGH); else digitalWrite (DB4, LOW);
    if (blue & 0x2) digitalWrite (DB3, HIGH); else digitalWrite (DB3, LOW);
    if (blue & 0x1) digitalWrite (DB2, HIGH); else digitalWrite (DB2, LOW);
    
    delayMicroseconds(1);
    digitalWrite (WR, LOW);
    delayMicroseconds(1);
    digitalWrite (WR, HIGH);

    digitalWrite(CS,HIGH);
}

uint16_t convert_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    
    uint16_t data=0;
    
    
    
    data |= ((red & 0x3F) << 11);
    data |= ((green & 0x38) << 8);
    data |= ((green & 7) << 5);
    data |= (blue & 0x3F);
    
    return data;
}

void draw_testimage()
{
    
    set_index_register(0x22);   //set GRAM data
    
    uint8_t x,y;
    
    
    
    for (x=0; x<220; x++){
        for (y=0; y<176 ; y++){
            
            if (x>60 && x<80)
                WRITE_GRAM_DATA(0xFF,0,0); 
            else if (x>100 && x<120)
                WRITE_GRAM_DATA(0,0xFF,0);
            else if (x>140 && x<160)
                WRITE_GRAM_DATA(0,0,0xFF);
            else
                WRITE_GRAM_DATA(0xFF,0xFF,0xFF);
            
        }
        
        
    }
    
    
    
}

void setup()
{

    pinMode(CS,OUTPUT);
    digitalWrite(CS,HIGH);

    pinMode(PE5,OUTPUT);
    digitalWrite(PE5,HIGH);
    
    
    pinMode (reset, OUTPUT) ;
    pinMode (RS, OUTPUT) ;
    pinMode (WR, OUTPUT) ;
    pinMode (RD, OUTPUT) ;
    pinMode (DB0, OUTPUT) ;
    pinMode (DB1, OUTPUT) ;
    pinMode (DB2, OUTPUT) ;
    pinMode (DB3, OUTPUT) ;
    pinMode (DB4, OUTPUT) ;
    pinMode (DB5, OUTPUT) ;
    pinMode (DB6, OUTPUT) ;
    pinMode (DB7, OUTPUT) ;
    digitalWrite (WR, HIGH);
    digitalWrite (RD, HIGH);//setting up pins
    
    digitalWrite (reset, LOW); //hw-reset
    delay(3);   //millisec delay
    digitalWrite (reset, HIGH); //wake up from reset
    delay (333);
    
    //uint16_t chipid=0;
    
    
    //set_index_register(0);
    //chipid=read_control_register();
    
    
    
    // printf ("\nchipID= %#x\n",chipid);
    
    lcd_init();
    delay (33);
    
    draw_testimage();
    
  
}

void loop()
{
  digitalWrite(PE5,!digitalRead(PE5));
  draw_testimage();
  delay(100);
}

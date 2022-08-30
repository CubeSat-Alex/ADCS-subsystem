#include "createData.h"
#include<EEPROM.h>
#define led 7

// variable for SPI
uint8_t sendData[200];
uint8_t receiveData[200];
const byte  OBC = 0x01, ADCS = 0x10, TT = 0x11; //SUB SYSTEM ADDRESS
const byte Read = 0x06, Write = 0x07, ACK = 0X02, Ping = 0x00; // TYPE COMMAND
String data , data_received;
char* carac_data;
int csend = 0, creveive = 0, lreceive = 0, endData = 0, x = 0;
bool startSend = false, Print = false, getData = false;
///////////////////////////
void setup() {
  Serial.begin(115200);
  // have to send on master in, slave out
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode
  SPCR |= bit(SPE);//Enable SPI 
  // turn on interrupts
  SPCR |= bit(SPIE); // Enable Interrupt register
  servo_setup();
  Gyro_setup();
  pinMode(led, OUTPUT);
}
// SPI interrupt routine
ISR (SPI_STC_vect)
{
  if ((SPDR == 0xc0))
  {
    startSend = false;
    endData++;
  }
  if (endData > 0)
  {
    receiveData[creveive++] = SPDR;
  }
  if (endData == 2)
  { lreceive = creveive;// lreceive
    creveive = 0;
    Print = true;
    endData = 0;
  }
  if (SPDR == 0x23)
  {
    csend = 0;
    startSend = true;
  }

  if (startSend)
  {
    SPDR = sendData[csend++];
  }

} // end of interrupt service routine (ISR) SPI_STC_vect






void loop() {
  curr_angX = map(servox.read(), 750, 2250, 0, 120);
  curr_angY = map(servoy.read(), 750, 2250, 0, 120);
  if (Print)
  {

    for (int i = 0; i < lreceive; i++)
    {

      Serial.print(receiveData[i]);
      Serial.print(" ,/");
    }

    data_received = dismantPacket();

    Serial.println(data_received);
    if (data_received.length() == 1) // lenght == 1
    { carac_data = data_received.c_str();
      data = get_Data(carac_data[0]);
    } else {
      String x = "", y = ""; bool first = true;
      for ( int i = 0 ; i < data_received.length(); i++) {
        if (first) {
          if (data_received[i] != ',') {
            x.concat(data_received[i]);
          }

          else {
            des_angX = x.toInt();
            first = false;
          }
        }
        else {
          y.concat(data_received[i]);
        }

      }
      des_angY =   y.toInt();
      EEPROM.write(0, des_angX);//desired angle X
      EEPROM.write(1, des_angY);//desired angle X
      data = "OK";
    }

    buildPacket(data, Write, ADCS, OBC);
    Print = false;

  }
  if ( abs(EEPROM.read(0)- des_angX) > 5 ) {
  des_angX = EEPROM.read(0);
  }
  if (abs(EEPROM.read(1)- des_angY) > 5 ) {
  des_angY = EEPROM.read(1);
  }
  servox_motion(curr_angX, des_angX);
  servoy_motion(curr_angY, des_angY);
  Serial.print("curr ang: ");
  Serial.println(curr_angX);
  Serial.print("curr ang: ");
  Serial.println(curr_angY);
  Serial.print("desired x : ");
  Serial.println(des_angX);
  Serial.print("desired y : ");
  Serial.println(des_angY);
}
void buildPacket(String msg, uint8_t type, uint8_t dest, uint8_t src) // convert data to packet using SSP protocol (simple serial protocol)
{
  x = msg.length(); // length of data or msg
  uint8_t packet[x + 7], data[x], crc_buf[x + 3];
  strcpy(data, msg.c_str());
  packet[1] = dest;
  packet[2] = src;
  packet[3] = type;
  for (int i = 0; i < x; i++)
  {
    packet[i + 4] = data[i];
  }

  for (int i = 0; i < x + 3; i++)
    crc_buf[i] = packet[i + 1];
  packet[x + 6] = 0xc0; //add FEND in first index and last index of packet.
  packet[0] = 0xc0;
  uint16_t crc_r = CalCrc(0x0000, crc_buf, sizeof(crc_buf) / sizeof(crc_buf[0]), 0x1021); // calculate crc negative false with initializer 0x0000
  byte a, b, c, d;// for append crc in packet 
  a = crc_r % 16;
  crc_r = int(crc_r / 16);
  b = crc_r % 16;
  crc_r = int(crc_r / 16);
  c = crc_r % 16;
  crc_r = int(crc_r / 16);
  d = crc_r % 16;
  packet[x + 4] = (a | (b << 4));
  packet[x + 5] = (c | (d << 4));
  for (int i = 0; i < x + 7; i++) // copy packet in sendData array for sending it
  {
    sendData[i] = packet[i];

  }
}

String dismantPacket() // function for extract data from receiveing packet .
{
  String data_received;
  uint8_t crc_buf[lreceive - 4];

  if (receiveData[0] == 0xc0 && receiveData[lreceive - 1] == 0xc0) // check FEND
  {
    if (receiveData[1] == ADCS && receiveData[2] == OBC) //CHECK DESTINATION AND SOURCE
      for (int i = 0; i < lreceive - 4; i++)
        crc_buf[i] = receiveData[i + 1];
    uint16_t crc_r = CalCrc(0x0000, crc_buf, sizeof(crc_buf) / sizeof(crc_buf[0]), 0x1021);
    byte a, b, c, d;
    a = crc_r % 16;
    crc_r = int(crc_r / 16);
    b = crc_r % 16;
    crc_r = int(crc_r / 16);
    c = crc_r % 16;
    crc_r = int(crc_r / 16);
    d = crc_r % 16;
    if ( receiveData[lreceive - 3] == (a | (b << 4)) && receiveData[lreceive - 2] == (c | (d << 4))) // check crc then get data
    {
      for (int i = 4; i < lreceive - 3; i++)
      {
        data_received.concat(char(receiveData[i]));
      }

    }

  }


  return data_received;
}

uint16_t CalCrc(uint16_t crc_initial, uint8_t* buf, uint16_t len, uint16_t POLY)
{
  unsigned int byte ;
  unsigned char k;
  unsigned short ACC, TOPBIT;
  unsigned short remainder = crc_initial;
  TOPBIT = 0x8000;
  for (byte = 0; byte < len; ++byte)
  {
    ACC = (unsigned)buf[byte] << 8;
    remainder ^= ACC;
    // printf("new remainder = %u \n",remainder);
    for (k = 8; k > 0; --k)
    {
      if (remainder & TOPBIT)
      {
        remainder = remainder << 1;
        remainder = remainder ^ POLY;
      }
      else
      {
        remainder = (remainder << 1);
      }
    }
  }
  remainder = remainder ^ 0x0000;
  return remainder;
}

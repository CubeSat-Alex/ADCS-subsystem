#include "servo.h"
#define led 7
int curr_angX, curr_angY, des_angX, des_angY;
void(* resetFunc) (void) = 0; //declare reset function @ address 0

String get_Data(char command)
{
  String data;

  Serial.println(command);
  switch (command)
  {
    case 'A':
     mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      gyrox = map(ax, -17000, 17000, -90, 90) ;
    gyroy = map(ay, -17000, 17000, -90, 90);
    gyroz = map(az, -17000, 17000, -90, 90);  
  curr_angX = map(servox.read(), 750, 2250, 0, 120);
  curr_angY = map(servoy.read(), 750, 2250, 0, 120);
   if (curr_angX < 0) curr_angX = 0; // for handle error in first write when setup servo motor
  if (curr_angY < 0) curr_angY = 0;
      data = "{\"X\":"+String(gyrox)+","+"\"Y\":"+String(gyroy)+","+"\"Z\":"+String(gyroz)+",\"A\":"+String(ax)+"}";
      break;
    case 'Z':     //  ping
      data = "OK";
      break;
    case 'X':
      data = "OK";
      digitalWrite(led, HIGH);
      break;
    case 'Y':
      data = "OK";
      digitalWrite(led, LOW);
      break;
    case 'R':
      data = "reset";
      resetFunc();  //call reset
      break;
    default:
      data = "ERROR";
  }
  return data;
}

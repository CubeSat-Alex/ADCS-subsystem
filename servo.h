#include <ServoTimer2.h>
#include "GYRO.h"
ServoTimer2 servox;
ServoTimer2 servoy;// create two object for each servo motor

void servo_setup()
{ //
  Serial.println("servo intialized");
  servox.attach(6); // configuration servo motors with PWM pins
  servoy.attach(5);
}

void servox_motion(int  curr_ang, int des_ang) //pass angle as int
{
  curr_ang = map(servox.read(), 750, 2250, 0, 120);
  if ((curr_ang - des_ang) < -5) //make tolerance
  {Serial.println(des_ang);
    while ( (curr_ang < des_ang) && (des_ang < 120) && (des_ang > -1)){
      servox.write(servox.read() + 15);
      delay(50);
      
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      Serial.print("curr_angX after motor: ");
      Serial.println(curr_ang);
       gyrox = map(ax, -17000, 17000, -90, 90) ;
      // EEPROM.write(0, gyrox);
      curr_ang = map(servox.read(), 750, 2250, 0, 120);
    }
  }
  else if ((curr_ang - des_ang) > 5)
  { Serial.println("else");
    while ( (curr_ang > des_ang) && (des_ang < 120) && (des_ang> -1) )
    {
      servox.write(servox.read() - 15);
      delay(50);
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      Serial.print("curr_angX after motor: ");
      Serial.println(curr_ang);
      gyrox = map(ax, -17000, 17000, -90, 90) ;
      //EEPROM.write(0, gyrox);
      curr_ang = map(servox.read(), 750, 2250, 0, 120);
    }
  }
  //   return gyrox;
}

void servoy_motion(int  curr_ang, int des_ang) //pass angle as int
{
  curr_ang = map(servoy.read(), 750, 2250, 0, 120);
  if ((curr_ang - des_ang) < -5)
  {
    while ( (curr_ang <= des_ang) && (des_ang < 120) && (des_ang > -1) )
    {
      servoy.write(servoy.read() + 15);
      delay(50);
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      Serial.print("curr_angY after motor: ");
      Serial.println(curr_ang);
      gyroy = map(ay, -17000, 17000, -90, 90) ;
      curr_ang = map(servoy.read(), 750, 2250, 0, 120);
    }
  }
  else if ((curr_ang - des_ang) > 5)
  {
    while ( (curr_ang > des_ang) && (des_ang < 120) && (des_ang > -1) )
    {
      servoy.write(servoy.read() - 15);
      delay(50);
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      Serial.print("curr_angY after motor: ");
      Serial.println(curr_ang);
      gyroy = map(ay, -17000, 17000, -90, 90) ;
      curr_ang = map(servoy.read(), 750, 2250, 0, 120);
    }

  }
}

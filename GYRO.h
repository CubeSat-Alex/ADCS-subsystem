#include <MPU6050.h>

#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"


MPU6050 mpu;
 
int16_t ax, ay, az,gx, gy, gz;
int gyrox,gyroy,gyroz;
 
void Gyro_setup()
{
    Wire.begin();
    mpu.initialize();
    Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");

}

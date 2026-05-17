#include "mpu6050_utils.h"

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
Mpu6050 mpu;

/* Setup function */
void setup() 
{
    Serial.begin(9600);
    initMPU(&mpu); 
    setAccelRange(&mpu, ACCEL_2G);
    setGyroRange(&mpu, GYRO_250DPS);
}

/* Main loop */
void loop()
{
    /* Read raw data from MPU6050 */
    Wire.beginTransmission(mpu.i2c_addr);
    Wire.write(0x3B);  /* starting with register 0x3B (ACCEL_XOUT_H) */
    Wire.endTransmission(false);

    /* Request 14 registers starting from 0x3B (ACCEL_XOUT_H) */
    Wire.requestFrom((uint8_t)mpu.i2c_addr, (size_t)14, (bool)true);  /* Request a total of 14 registers */
    AcX = read16bit();  /* 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) */
    AcY = read16bit();  /* 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L) */
    AcZ = read16bit();  /* 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L) */
    Tmp = read16bit();  /* 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L) */
    GyX = read16bit();  /* 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L) */
    GyY = read16bit();  /* 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L) */
    GyZ = read16bit();  /* 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L) */

    /* Accelerometer values */
    Serial.print(" | AcX = "); 
    Serial.print(accelRawToG(&mpu, AcX));
    Serial.print(" | AcY = "); 
    Serial.print(accelRawToG(&mpu, AcY));
    Serial.print(" | AcZ = "); 
    
    /* Gyroscope values */
    Serial.print(accelRawToG(&mpu, AcZ));
    Serial.print(" | GyX = "); 
    Serial.print(gyroRawToDPS(&mpu, GyX));
    Serial.print(" | GyY = "); 
    Serial.print(gyroRawToDPS(&mpu, GyY));
    Serial.print(" | GyZ = "); 
    Serial.print(gyroRawToDPS(&mpu, GyZ));
    
    /* Temperature value */
    Serial.print(" | Tmp = "); 
    Serial.println(Tmp/340.00+36.53);  /* Temperature formula from the datasheet of MPU6050 */

    delay(500);
}

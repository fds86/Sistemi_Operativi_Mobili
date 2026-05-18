#include "mpu6050_utils.h"

/**
 * @file main.cpp
 * @brief ESP32 firmware example for acquiring MPU6050 data over I2C.
 */

int16_t AcX,AcY,AcZ,GyX,GyY,GyZ,Tmp; /**< Raw sensor data */
Mpu6050 mpu;

/**
 * @brief Initialize serial output and configure the MPU6050 sensor.
 */
void setup() 
{
    Serial.begin(9600);
    initMPU(&mpu); 
    setAccelRange(&mpu, ACCEL_2G);
    setGyroRange(&mpu, GYRO_250DPS);
}

/**
 * @brief Read sensor values and print acceleration, gyroscope, and temperature.
 */
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
    Serial.print(accelRawToG(&mpu, AcZ));
    
    /* Gyroscope values */
    Serial.print(" | GyX = "); 
    Serial.print(gyroRawToDPS(&mpu, GyX));
    Serial.print(" | GyY = "); 
    Serial.print(gyroRawToDPS(&mpu, GyY));
    Serial.print(" | GyZ = "); 
    Serial.print(gyroRawToDPS(&mpu, GyZ));
    
    /* Temperature value */
    Serial.print(" | Tmp = "); 
    Serial.println(tempRawToCelsius(Tmp));

    delay(500);
}

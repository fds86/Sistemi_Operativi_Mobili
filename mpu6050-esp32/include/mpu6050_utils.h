#ifndef MPU6050_UTILS_H
#define MPU6050_UTILS_H

/**
 * @file mpu6050_utils.h
 * @brief Utility functions and constants for MPU6050 on ESP32.
 */

#include <Arduino.h>
#include <Wire.h>

/* MPU6050 I2C address (default) */
#define MPU6050_ADDR 0x68  /**< MPU6050 I2C address */

/* Default I2C pins for ESP32 */
#define SDA_PIN 21  /**< Default SDA pin for ESP32 */
#define SCL_PIN 22  /**< Default SCL pin for ESP32 */   

/* Accelerometer sensitivity (AFS_SEL) */
#define ACCEL_2G  0  /**< Accelerometer range: ±2g */
#define ACCEL_4G  1  /**< Accelerometer range: ±4g */
#define ACCEL_8G  2  /**< Accelerometer range: ±8g */
#define ACCEL_16G 3  /**< Accelerometer range: ±16g */

/* Gyroscope sensitivity (FS_SEL) */
#define GYRO_250DPS  0  /**< Gyroscope range: ±250°/s */
#define GYRO_500DPS  1  /**< Gyroscope range: ±500°/s */
#define GYRO_1000DPS 2  /**< Gyroscope range: ±1000°/s */
#define GYRO_2000DPS 3  /**< Gyroscope range: ±2000°/s */

/** @brief Runtime configuration and state for the MPU6050 sensor. */
typedef struct 
{
    uint8_t accel_range;  /**< Accelerometer range */
    uint8_t gyro_range;   /**< Gyroscope range */
    uint8_t i2c_addr;     /**< I2C address */
} Mpu6050;

/**
 * @brief Initialize the MPU-6050 sensor.
 * 
 * This function initializes the I2C communication and configures
 * the MPU-6050 sensor to use the internal 8MHz oscillator.
 * @param[in,out] mpu Pointer to the MPU6050 structure.
 */
inline void initMPU(Mpu6050* mpu) 
{
    mpu->i2c_addr = MPU6050_ADDR;
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.beginTransmission(mpu->i2c_addr);
    Wire.write(0x6B);  /* PWR_MGMT_1 register */
    Wire.write(0);     /* set to zero (Internal 8MHz oscillator) */
    Wire.endTransmission(true);
}

/**
 * @brief Set the accelerometer range.
 * 
 * @param[in,out] mpu Pointer to the MPU6050 structure.
 * @param[in] afs_sel Accelerometer range (ACCEL_2G, ACCEL_4G, ACCEL_8G, ACCEL_16G).
 */
inline void setAccelRange(Mpu6050* mpu, uint8_t afs_sel) 
{
    mpu->accel_range = afs_sel;
    Wire.beginTransmission(mpu->i2c_addr);
    Wire.write(0x1C); /* ACCEL_CONFIG register */
    Wire.write((afs_sel & 0x03) << 3);
    Wire.endTransmission(true);
}

/**
 * @brief Set the gyroscope range.
 * 
 * @param[in,out] mpu Pointer to the MPU6050 structure.
 * @param[in] fs_sel Gyroscope range (GYRO_250DPS, GYRO_500DPS, GYRO_1000DPS, GYRO_2000DPS).
 */
inline void setGyroRange(Mpu6050* mpu, uint8_t fs_sel) 
{
    mpu->gyro_range = fs_sel;
    Wire.beginTransmission(mpu->i2c_addr);
    Wire.write(0x1B); /* GYRO_CONFIG register */
    Wire.write((fs_sel & 0x03) << 3);
    Wire.endTransmission(true);
}

/**
 * @brief Convert raw accelerometer value to g.
 * 
 * @param[in] mpu Pointer to the MPU6050 structure.
 * @param[in] raw Raw accelerometer value.
 * @return Accelerometer value in g.
 */
inline float accelRawToG(const Mpu6050* mpu, int16_t raw) 
{
    float scale;
    switch (mpu->accel_range) 
    {
        case ACCEL_2G:  
            scale = 16384.0; 
            break;
        case ACCEL_4G:  
            scale = 8192.0;  
            break;
        case ACCEL_8G:  
            scale = 4096.0;  
            break;
        case ACCEL_16G: 
            scale = 2048.0;  
            break;
        default:        
            scale = 16384.0; 
            break;
    }

    return raw / scale;
}
    
/**
 * @brief Convert raw gyroscope value to degrees per second (DPS).
 * 
 * @param[in] mpu Pointer to the MPU6050 structure.
 * @param[in] raw Raw gyroscope value.
 * @return Gyroscope value in DPS.
 */
inline float gyroRawToDPS(const Mpu6050* mpu, int16_t raw) 
{
    float scale;
    switch (mpu->gyro_range) 
    {
        case GYRO_250DPS:  
            scale = 131.0;  
            break;
        case GYRO_500DPS:  
            scale = 65.5;   
            break;
        case GYRO_1000DPS: 
            scale = 32.8;   
            break;
        case GYRO_2000DPS: 
            scale = 16.4;   
            break;
        default:           
            scale = 131.0;  
            break;
    }

    return raw / scale;
}

/**
 * @brief Convert raw temperature value to degrees Celsius.
 * 
 * @param[in] raw Raw temperature value.
 * @return Temperature value in degrees Celsius.
 */
inline float tempRawToCelsius(int16_t raw) 
{
    return ((float)raw) / 340.0f + 36.53f;  /* Temperature formula from the MPU6050 datasheet */
}
    
/**
 * @brief Read 16-bit value from the I2C bus.
 */
inline int16_t read16bit() 
{
  uint8_t high = Wire.read();
  uint8_t low = Wire.read();
  return (int16_t)((high << 8) | low);
}
    
#endif // MPU6050_UTILS_H
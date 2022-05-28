#include "MPU9250.h"
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <SD.h>

MPU9250 mpu;
Adafruit_BMP280 bmp; // I2C
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();


void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(2000);

    Serial.print("Initializing SD card...");

    
    if (!SD.begin(4)) {
      Serial.println("initialization failed!");
      while (1);
    }
    
    

    Serial.println("initialization done.");
    
    if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }
    
    
    while ( !Serial ) delay(100);   // wait for native usb

    
    
    Serial.println(F("BMP280 test"));
    unsigned status;
    status = bmp.begin();

    print_status(status);
    
    
      /* Default settings from datasheet. */
    
    
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    bmp_temp->printSensorDetails();

    
}

void loop() {
  
    if (mpu.update()) {
        static uint32_t prev_ms = millis();
        if (millis() > prev_ms + 25) {
            print_roll_pitch_yaw();
            prev_ms = millis();
        }
    }
}

void print_roll_pitch_yaw() {
  
    sensors_event_t temp_event, pressure_event;
    bmp_temp->getEvent(&temp_event);
    bmp_pressure->getEvent(&pressure_event);

    Serial.print(F("Temperature = "));
    Serial.print(temp_event.temperature);
    Serial.println(" *C");
  
    Serial.print(F("Pressure = "));
    Serial.print(pressure_event.pressure);
    Serial.println(" hPa");
    
    
    File sense_bmp = SD.open("ccc.txt", FILE_WRITE);
    if (sense_bmp) {
      sense_bmp.print(pressure_event.pressure);
      sense_bmp.print(",");
      sense_bmp.print(temp_event.temperature);
      sense_bmp.println();
      sense_bmp.close();
    }
    else {
      Serial.println("ERROR FAILED TO WRITE");
    }
    
    
    Serial.print("Yaw, Pitch, Roll: ");
    Serial.print(mpu.getYaw(), 2);
    Serial.print(", ");
    Serial.print(mpu.getPitch(), 2);
    Serial.print(", ");
    Serial.println(mpu.getRoll(), 2);
    Serial.println();

    
    File sense_mpu = SD.open("sense_mpu.txt", FILE_WRITE);
    if (sense_mpu) {
      sense_mpu.print(mpu.getYaw());
      sense_mpu.print(",");
      sense_mpu.print(mpu.getPitch());
      sense_mpu.print(",");
      sense_mpu.print(mpu.getRoll());
      sense_mpu.println();
      sense_mpu.close();
    }
    
    else {
      Serial.println("ERROR FAILED TO WRITE");
    }
    
    
    
    delay(10);
}


void print_status(unsigned status) {
  if (!status) {
      Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                        "try a different address!"));
      Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
    }
}

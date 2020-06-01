/*!
 * @file Adafruit_BME680.h
 *
 * Adafruit BME680 temperature, humidity, barometric pressure and gas sensor driver
 *
 * This is the documentation for Adafruit's BME680 driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit BME680 breakout: https://www.adafruit.com/products/3660
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Ladyada for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef __BME680_H__
#define __BME680_H__

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "bme680.h"


/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define BME680_DEFAULT_ADDRESS       (0x77)     ///< The default I2C address
/*=========================================================================*/
#define BME680_DEFAULT_SPIFREQ       (1000000)  ///< The default SPI Clock speed



/*
class Adafruit_BME680_Unified : public Adafruit_Sensor
{
public:
    Adafruit_BME680_Unified(int32_t sensorID = -1);

    bool  begin(uint8_t addr = BME680_ADDRESS);
    void  getTemperature(float *temp);
    void  getPressure(float *pressure);
    float pressureToAltitude(float seaLevel, float atmospheric, float temp);
    float seaLevelForAltitude(float altitude, float atmospheric, float temp);
    void  getEvent(sensors_event_t*);
    void  getSensor(sensor_t*);

  private:
    uint8_t   _i2c_addr;
    int32_t   _sensorID;
};

*/

/** Adafruit_BME680 Class for both I2C and SPI usage.
 *  Wraps the Bosch library for Arduino usage
 */

class Adafruit_BME680
{
  public:
    Adafruit_BME680(int8_t cspin = -1);
    Adafruit_BME680(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);

    bool  begin(uint8_t addr = BME680_DEFAULT_ADDRESS);
    float readTemperature(void);
    float readPressure(void);
    float readHumidity(void);
    uint32_t readGas(void);
    float readAltitude(float seaLevel);

    bool setTemperatureOversampling(uint8_t os);
    bool setPressureOversampling(uint8_t os);
    bool setHumidityOversampling(uint8_t os);
    bool setIIRFilterSize(uint8_t fs);
    bool setGasHeater(uint16_t heaterTemp, uint16_t heaterTime);

    /// Perform a reading in blocking mode.
    bool performReading(void);

    /** @brief Begin an asynchronous reading.
     *  @return When the reading would be ready as absolute time in millis().
     */
    unsigned long beginReading(void);

    /** @brief End an asynchronous reading.
     *  @return Whether success.
     *
     *  If the asynchronous reading is still in progress, block until it ends.
     *  If no asynchronous reading has started, this is equivalent to performReading().
     */
    bool endReading(void);

    /// Temperature (Celsius) assigned after calling performReading() or endReading()
    float temperature;
    /// Pressure (Pascals) assigned after calling performReading() or endReading()
    float pressure;
    /// Humidity (RH %) assigned after calling performReading() or endReading()
    float humidity;
    /// Gas resistor (ohms) assigned after calling performReading() or endReading()
    float gas_resistance;
  private:

    bool _filterEnabled, _tempEnabled, _humEnabled, _presEnabled, _gasEnabled;
    uint8_t _i2caddr;
    int32_t _sensorID;
    int8_t _cs;
    unsigned long _meas_end;

    uint8_t spixfer(uint8_t x);

    struct bme680_dev gas_sensor;
};

#endif

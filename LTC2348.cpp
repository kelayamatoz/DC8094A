/*! @file
    @ingroup LTC2348
    Library for LTC2348: 18-bit, 8 channel Simultaneous Sampling SAR ADC
*/
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2348.h"

#define VREF 4.096
#define POW2_18 262143
#define POW2_17 131072

// Setting input range of all channels - 0V to 0.5 Vref with no gain compression (SS2 = 0, SS1 = 0, SS0 = 1)
struct Config_Word_Struct CWSTRUCT = { 7, 7, 7, 7, 7, 7, 7, 7};

// Calculates the voltage from ADC output data depending on the channel configuration
float LTC2348_voltage_calculator(int32_t data, uint8_t channel)
{
  float voltage;
  uint8_t CW;
  switch (channel)
  {
    case 0:
      CW = CWSTRUCT.LTC2348_CHAN0_CONFIG;
      break;
    case 1:
      CW = CWSTRUCT.LTC2348_CHAN1_CONFIG;
      break;
    case 2:
      CW = CWSTRUCT.LTC2348_CHAN2_CONFIG;
      break;
    case 3:
      CW = CWSTRUCT.LTC2348_CHAN3_CONFIG;
      break;
    case 4:
      CW = CWSTRUCT.LTC2348_CHAN4_CONFIG;
      break;
    case 5:
      CW = CWSTRUCT.LTC2348_CHAN5_CONFIG;
      break;
    case 6:
      CW = CWSTRUCT.LTC2348_CHAN6_CONFIG;
      break;
    case 7:
      CW = CWSTRUCT.LTC2348_CHAN7_CONFIG;
      break;
  }

  switch (CW)
  {
    case 0:
      voltage = 0;
      break;   // Disable Channel
    case 1:
      voltage = (float)data * (1.25 * VREF / 1.000) / POW2_18;
      break;
    case 2:
      {
        voltage = (float)data * (1.25 * VREF / 1.024) / POW2_17;
        if (voltage > 5.12)
          voltage -= 10.24;
        break;
      }
    case 3:
      {
        voltage = (float)data * (1.25 * VREF / 1.000) / POW2_17;
        if (voltage > 5.12)
          voltage -= 10.24;
        break;
      }
    case 4:
      voltage = (float)data * (2.50 * VREF / 1.024) / POW2_18;
      break;
    case 5:
      voltage = (float)data * (2.50 * VREF / 1.000) / POW2_18;
      break;
    case 6:
      {
        voltage = (float)data * (2.50 * VREF / 1.024) / POW2_17;
        if (voltage > 10.24)
          voltage -= 20.48;
        break;
      }
    case 7:
      {
        voltage = (float)data * (2.50 * VREF / 1.000) / POW2_17;
        if (voltage > 10.24)
          voltage -= 20.48;
        break;
      }
  }
  return voltage;
}

// Creates 24-bit configuration word for the 8 channels.
uint32_t LTC2348_create_config_word()
{
  uint32_t config_word;
  config_word = (uint32_t)CWSTRUCT.LTC2348_CHAN7_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN6_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN5_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN4_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN3_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN2_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN1_CONFIG;
  config_word = (config_word << 3) | (uint32_t)CWSTRUCT.LTC2348_CHAN0_CONFIG;

  return config_word;
}

// Transmits 24 bits (3 bytes) of configuration information and
// reads back 24 bytes of data (3 bytes/ 24 bits for each channel)
// 24 bits: 18 bit data + 3 bit config + 3 bit channel number
// Read back is done in a new cycle
void LTC2348_write(uint8_t Result[24])
{
  int i, j;
  uint32_t config_word;
  config_word = LTC2348_create_config_word();

  output_low(QUIKEVAL_CS);                  //! Pull CS low

  SPI.transfer((uint8_t)(config_word >> 16));   //! Transferring byte 2
  SPI.transfer((uint8_t)(config_word >> 8));    //! Transferring byte 1
  SPI.transfer((uint8_t)config_word);         //! Transferring byte 0

  output_high(QUIKEVAL_CS);                 //! Pull CS high
  output_low(QUIKEVAL_CS);

  for (i = 0; i < 24; ++i)
    Result[i] = SPI.transfer(0); //! Receiving 24B data by transferring null information

  output_high(QUIKEVAL_CS);
}

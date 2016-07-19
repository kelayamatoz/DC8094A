#ifndef LTC2348_H
#define LTC2348_H

//! SoftSpan Configuration Bits for 8 channels (3 bits each)
struct Config_Word_Struct
{
  uint8_t LTC2348_CHAN0_CONFIG : 3;
  uint8_t LTC2348_CHAN1_CONFIG : 3;
  uint8_t LTC2348_CHAN2_CONFIG : 3;
  uint8_t LTC2348_CHAN3_CONFIG : 3;
  uint8_t LTC2348_CHAN4_CONFIG : 3;
  uint8_t LTC2348_CHAN5_CONFIG : 3;
  uint8_t LTC2348_CHAN6_CONFIG : 3;
  uint8_t LTC2348_CHAN7_CONFIG : 3;
};

//! Calculates the voltage from ADC output data depending on the channel configuration
float LTC2348_voltage_calculator(int32_t data,
                                 uint8_t channel
                                );

//! Creates 24-bit configuration word for the 8 channels.
uint32_t LTC2348_create_config_word();

//! Transmits 24 bits (3 bytes) of configuration information and
//! reads back 24 bytes of data (3 bytes/ 24 bits for each channel)
//! 24 bits: 18 bit data + 3 bit config + 3 bit channel number
//! Read back is done in a new cycle
void LTC2348_write(uint8_t Result[24]
                  );

#endif

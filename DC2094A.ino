#include <Arduino.h>
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <Wire.h>
#include <stdint.h>
#include <SPI.h>
#include "LTC2348.h"

// "CONFIGURATION SETTINGS (Vref = 4.096V)"
// "|Config Number| SS2 | SS1 | SS0 | ANALOG INPUT RANGE      | DIGITAL COMPRESSION | RESULT BINARY FORMAT |"
// "|-------------|-----------------|-------------------------|---------------------|----------------------|"
// "|      0      |  0  |  0  |  0  | Disable Channel         | N/A                 | All Zeros            |"
// "|      1      |  0  |  0  |  1  | 0 - 1.25 Vref           | 1                   | Straight Binary      |"
// "|      2      |  0  |  1  |  0  | -1.25 Vref - +1.25 Vref | 1/1.024             | Two's Complement     |"
// "|      3      |  0  |  1  |  1  | -1.25 Vref - +1.25 Vref | 1                   | Two's Complement     |"
// "|      4      |  1  |  0  |  0  | 0 - 2.5 Vref            | 1/1.024             | Straight Binary      |"
// "|      5      |  1  |  0  |  1  | 0 - 2.5 Vref            | 1                   | Straight Binary      |"
// "|      6      |  1  |  1  |  0  | -2.5 Vref - +2.5 Vref   | 1/1.024             | Two's Complement     |"
// "|      7      |  1  |  1  |  1  | -2.5 Vref - +2.5 Vref   | 1                   | Two's Complement     |"

// Macros
#define  CONFIG_WORD_POSITION  0X07

// Global variable
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected
uint8_t channel;

extern Config_Word_Struct CWSTRUCT;

// Function declarations
void display_adc_output();
void menu2_display_channel_CW();
void changeCW();

//! Initialize Linduino
void setup()
{
  uint8_t value = 0;
  uint8_t *p = &value;
  char demo_name[] = "LTC2348-18";  //! Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();              //! Initializes Linduino I2C port.
  quikeval_SPI_init();            //! Initializes Linduino SPI port.

//  Serial.begin(115200);             //! Initialize the serial port to the PC
  Serial.begin(230400);		// doubling the rate 
  demo_board_connected = discover_DC2094(demo_name);
  if (!demo_board_connected)
  {
	  Serial.println(F("Error: cannot connect!"));
  }

  i2c_read_byte(0x20, &value);      // 0x20 is the port address for i/o expander for I2C.
  value = value & 0x7F;             // P7 = WRIN = 0
  value = value | 0x04;             // P2 = WRIN2 = 1
  i2c_write_byte(0x20, value);

  quikeval_SPI_connect();           //! Connects to main SPI port
  changeCW();
}

//! Repeats Linduino Loop
void loop()
{
  int8_t user_command;                 // The user input command
  uint8_t acknowledge = 0;
  if (Serial.available())             // Check for user input
  {
	  display_adc_output();
  }
}

//! Read the ID string from the EEPROM and determine if the correct board is connected.
//! Returns 1 if successful, 0 if not successful
uint8_t discover_DC2094(char *demo_name)
{
  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;

  if (!strcmp(demo_board.product_name, demo_name))
  {
   return 1;
  }
  else
  {
    Serial.print("Demo board ");
    Serial.print(demo_name);
    Serial.print(" not found, \nfound ");
    Serial.print(demo_board.name);
    Serial.println(" instead. \nConnect the correct demo board, then press the reset button.");
    return 0;
  }
}

//! Displays the ADC output and calculated voltage for all channels
void display_adc_output()
{
  uint8_t i,k,pos;
  uint8_t channel_selected;
  uint8_t *p;
  uint8_t Result[24];
  float voltage;
  float voltageResults[8];
  uint32_t code;
  union LT_union_int32_4bytes data;
  data.LT_uint32 = 0;
  uint8_t j = 0;
  LTC2348_write(Result);    //discard the first reading
  LTC2348_write(Result);

  for (i = 0; i < 24; i = i+3)
  {
    data.LT_byte[2] = Result[i];
    data.LT_byte[1] = Result[i+1];
    data.LT_byte[0] = Result[i+2];

    channel = (data.LT_uint32 & 0x38) >> 3;
//    Serial.print(F("\nChannel      : "));
//    Serial.println(channel);

    code = (data.LT_uint32 & 0xFFFFC0) >> 6;
//    Serial.print(F("Data         : 0x"));
//    Serial.println(code, HEX);

//    Serial.print(F("Voltage      : "));
    voltage = LTC2348_voltage_calculator(code, channel);
    voltageResults[j] = voltage;
//    Serial.print(voltage, 6);
//    Serial.println(F(" V"));
    j++;
  }
  // format: float float float float float float float float \n
  for (k = 0; k < 8; k ++)
  {
    Serial.print(voltageResults[k], 6);
    if (k == 7)
    {
      Serial.println("");  
    }
    else 
    {
      Serial.print(F(" "));
    }
  }
}

void changeCW()
{
  uint8_t configNum;
  configNum = 7;
  CWSTRUCT.LTC2348_CHAN0_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN1_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN2_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN3_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN4_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN5_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN6_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN7_CONFIG = configNum;
}

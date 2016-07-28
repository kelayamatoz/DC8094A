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

// Macros
#define  CONFIG_WORD_POSITION  0X07

// Global variable
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected
uint8_t channel;

extern Config_Word_Struct CWSTRUCT;

// Function declarations
void display_adc_output();
void changeCW();

//! Initialize Linduino
void setup()
{
  uint8_t value = 0;
  uint8_t *p = &value;
  char demo_name[] = "LTC2348-18";  //! Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();              //! Initializes Linduino I2C port.
  quikeval_SPI_init();            //! Initializes Linduino SPI port.

  Serial.begin(230400);             //! Initialize the serial port to the PC

  demo_board_connected = discover_DC2094(demo_name);
  if (!demo_board_connected)
  {
    Serial.println("Error: cannot see the demo board");
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
  display_adc_output();
}

uint8_t discover_DC2094(char *demo_name)
{
  Serial.print(F("\nChecking EEPROM contents..."));
  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);

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

void display_adc_output()
{
  uint8_t i, pos;
  uint8_t channel_selected;
  uint8_t *p;
  uint8_t Result[24];
  float voltage;
  uint32_t code;
  union LT_union_int32_4bytes data;
  data.LT_uint32 = 0;

  LTC2348_write(Result);    //discard the first reading
  LTC2348_write(Result);

  for (i = 0; i < 24; i = i+3)
  {
    data.LT_byte[2] = Result[i];
    data.LT_byte[1] = Result[i+1];
    data.LT_byte[0] = Result[i+2];

    channel = (data.LT_uint32 & 0x38) >> 3;
    code = (data.LT_uint32 & 0xFFFFC0) >> 6;
    voltage = LTC2348_voltage_calculator(code, channel);
    Serial.print(voltage, 6);
    Serial.print(F(":"));
    if (i == 21)
    {
      Serial.println(F(" "));  
    }
//    Serial.println(F(" V"));
  }
}

void changeCW()
{
  uint8_t configNum = 7;
  CWSTRUCT.LTC2348_CHAN0_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN1_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN2_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN3_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN4_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN5_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN6_CONFIG = configNum;
  CWSTRUCT.LTC2348_CHAN7_CONFIG = configNum;
}

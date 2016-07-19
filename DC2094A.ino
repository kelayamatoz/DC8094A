// driver for driving the ADC
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
void menu1_display_adc_output();
void menu2_display_channel_CW();
void menu3_changeCW();

void setup()
{
  uint8_t value = 0;
  uint8_t *p = &value;
  char demo_name[] = "LTC2348-18";  
  quikeval_I2C_init();
  quikeval_SPI_init();

  Serial.begin(115200);
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*****************************************************************\n"));
  
  demo_board_connected = discover_DC2094(demo_name);
  if (demo_board_connected)
  {
    Serial.print(F("\connection established"));
  }

  i2c_read_byte(0x20, &value);      // 0x20 is the port address for i/o expander for I2C.
  delay(100);
  value = value & 0x7F;             // P7 = WRIN = 0
  value = value | 0x04;             // P2 = WRIN2 = 1
  i2c_write_byte(0x20, value);
  delay(100);

  quikeval_SPI_connect();           // Connects to main SPI port
}

void loop()
{
  int8_t user_command;                 // The user input command
  uint8_t acknowledge = 0;
  if (Serial.available())             // Check for user input
  {
    if (acknowledge)
      Serial.println(F("***** I2C ERROR *****"));
  }
}

uint8_t discover_DC2094(char *demo_name)
{
  Serial.print(F("\nChecking EEPROM contents..."));
  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);
  
  if (!strcmp(demo_board.product_name, demo_name))
  {
    Serial.println("demo_board established");
    Serial.println(F("Demo board connected"));
    Serial.println(F("Press Enter to Continue"));
    read_int();
    return 1;
  }
  else
  {
    Serial.print("Demo board ");
    Serial.print(demo_name);
    Serial.print(" not found, \nfound ");
    Serial.print(demo_board.name);
    return 0;
  }
}


//! Displays the ADC output and calculated voltage for all channels
void menu1_display_adc_output()
{
  uint8_t i, pos;
  uint8_t channel_selected;
  uint8_t *p;
  uint8_t Result[24];
  float voltage;
  uint32_t code;
  union LT_union_int32_4bytes data;
  data.LT_uint32 = 0;

  Serial.print("\nOutputing readings from all 8 channels ");
  LTC2348_write(Result);    //discard the first reading
  LTC2348_write(Result);
  Serial.println("ALL");
  for (i = 0; i < 24; i = i+3)
  {
    data.LT_byte[2] = Result[i];
    data.LT_byte[1] = Result[i+1];
    data.LT_byte[0] = Result[i+2];
    channel = (data.LT_uint32 & 0x38) >> 3;
    Serial.print(F("\nChannel      : "));
    Serial.println(channel);

    code = (data.LT_uint32 & 0xFFFFC0) >> 6;
    Serial.print(F("Data         : 0x"));
    Serial.println(code, HEX);

    Serial.print(F("Voltage      : "));
    voltage = LTC2348_voltage_calculator(code, channel);
    Serial.print(voltage, 6);
    Serial.println(F(" V"));

    Serial.print(F("Config Number: "));
    Serial.println(data.LT_byte[0] & CONFIG_WORD_POSITION);
  }
}

// Debugging: display the channel values configurations 
void menu2_display_channel_CW()
{
  uint8_t i, j;
  uint8_t channel;
  uint8_t Result[24];

  Serial.print("\nEnter the channel number (0 - 7, 8: ALL): ");
  channel = read_int();
  if (channel < 0)
    channel = 0;
  else if (channel > 8)
    channel = 8;

  LTC2348_write(Result);    //discard the first reading
  LTC2348_write(Result);

  if (channel == 8)
  {
    Serial.println("ALL");
    Serial.print("\nConfig number for each channel:");
    j = 0;
    for (i = 0; i < 8; ++i)
    {
      Serial.print("\n\nChannel      : ");
      Serial.println(i);
      Serial.print("Config Number: ");
      Serial.print(Result[j + 2] & CONFIG_WORD_POSITION);
      j = j + 3;
    }
    Serial.print("\n");
  }
  else
  {
    Serial.println(channel);
    Serial.print("Config Number: ");
    Serial.println(Result[channel * 3 + 2] & CONFIG_WORD_POSITION);
  }
}

//! Function to change the configuration setting
void menu3_changeCW()
{
  uint8_t i, j;
  uint8_t channel;
  uint8_t configNum;
  uint8_t Result[24];

  Serial.print("\nEnter the channel number (0 - 7, 8: ALL): ");
  channel = read_int();
  if (channel < 0)
    channel = 0;
  else if (channel > 8)
    channel = 8;

  if (channel == 8)
    Serial.println("ALL");
  else
    Serial.println(channel);

  Serial.print("Enter the configuration number in decimal: ");
  configNum = read_int();
  Serial.println(configNum);

  if (channel == 8)
  {
    CWSTRUCT.LTC2348_CHAN0_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN1_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN2_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN3_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN4_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN5_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN6_CONFIG = configNum;
    CWSTRUCT.LTC2348_CHAN7_CONFIG = configNum;
  }
  else
  {
    switch (channel)
    {
      case 0:
        CWSTRUCT.LTC2348_CHAN0_CONFIG = configNum;
        break;
      case 1:
        CWSTRUCT.LTC2348_CHAN1_CONFIG = configNum;
        break;
      case 2:
        CWSTRUCT.LTC2348_CHAN2_CONFIG = configNum;
        break;
      case 3:
        CWSTRUCT.LTC2348_CHAN3_CONFIG = configNum;
        break;
      case 4:
        CWSTRUCT.LTC2348_CHAN4_CONFIG = configNum;
        break;
      case 5:
        CWSTRUCT.LTC2348_CHAN5_CONFIG = configNum;
        break;
      case 6:
        CWSTRUCT.LTC2348_CHAN6_CONFIG = configNum;
        break;
      case 7:
        CWSTRUCT.LTC2348_CHAN7_CONFIG = configNum;
        break;
    }
  }

  Serial.print(F("\nCONFIGURATION CHANGED!"));
}

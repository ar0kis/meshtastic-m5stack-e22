#define I2C_SDA 21
#define I2C_SCL 22

// M5Stack Core Basic power controller. IP5306 exposes a coarse four-step
// fuel gauge (25/50/75/100%) over I2C address 0x75.
#define M5STACK_IP5306

#define BUTTON_PIN 38

#define USE_SX1262
#define SX126X_MAX_POWER 22
#define SX126X_DIO3_TCXO_VOLTAGE 1.8

#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_CS 26
#define LORA_RESET 25
#define LORA_DIO1 36
#define LORA_DIO2 35

#define SX126X_CS LORA_CS
#define SX126X_DIO1 LORA_DIO1
#define SX126X_BUSY LORA_DIO2
#define SX126X_RESET LORA_RESET
#define SX126X_RXEN 2
#define SX126X_TXEN 5

#undef GPS_RX_PIN
#undef GPS_TX_PIN
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_UBLOX

#define HAS_SPI_TFT 1
#define ILI9341_DRIVER
#define TFT_HEIGHT 240
#define TFT_WIDTH 320
#define TFT_OFFSET_X 0
#define TFT_OFFSET_Y 0
#define TFT_BUSY -1
#define USE_TFTDISPLAY 1
#define SCREEN_TRANSITION_FRAMERATE 1
#define ILI9341_SPI_HOST VSPI_HOST

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include "SimpleTimer.h"

//configuration

#include "config.h"

AdafruitIO_Feed *pressure = io.feed("pressure");
AdafruitIO_Feed *altitude = io.feed("altitude");

// Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

 #define TFT_SCLK 13
 #define TFT_MOSI 11

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define SEALEVELPRESSURE_HPA (1013.25)

// Variables
float t;
float p;
float a;

Adafruit_BMP3XX bmp;

// this int will hold the current count for our sketch
int count = 0;

void setup(void) {
  Serial.begin(115200);

  //Adafruit IO

  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  //ST7735 set up

  Serial.print(F("Hello! ST77xx TFT Test"));

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  tft.setSPISpeed(27000000);

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  Serial.println("done");
  delay(1000);

  //BMP388 set up

 if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
  //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
  //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
  Serial.println("Could not find a valid BMP3 sensor, check wiring!");
  while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);


}

void sendSensor()
{

  if (! bmp.performReading()) 
  {
  Serial.println("Failed to perform reading :(");
  return;
  }
  t = bmp.temperature;
  p = bmp.pressure / 100.0;  // in hPa
  a = bmp.readAltitude(SEALEVELPRESSURE_HPA);

  pressure->save(p); 
  delay(2000);
  altitude->save(a);

  tft.setTextSize(1);
  tft.println("pressure: ");
  tft.print(p, 6);
  tft.println(" hPa");
  tft.println("altitude: ");
  tft.print(a, 6);
  tft.println(" m");
delay(5000);

  Serial.print("Pressure = ");
  Serial.print(p);
  Serial.println(" hPa");
}

void loop() {
tft.fillScreen(ST77XX_BLACK);
tft.setCursor(0, 0);
tft.setTextSize(2);
tft.println("ESE 2180");
tft.setTextSize(1);
tft.println();
sendSensor();
}

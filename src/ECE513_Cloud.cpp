/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "c:/Users/YZ/Documents/ECE513_Proj/ECE513_Cloud/src/ECE513_Cloud.ino"
#include "Particle.h"
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

// Sensor Setup
void setup();
void loop();
void RGB_color(int red_light_value, int green_light_value, int blue_light_value);
#line 7 "c:/Users/YZ/Documents/ECE513_Proj/ECE513_Cloud/src/ECE513_Cloud.ino"
MAX30105 particleSensor;
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //int_time at which the last beat occurred

float beatsPerMinute;
int beatAvg;


// The following line is optional, but it allows your code to run
// even when not cloud connected
SYSTEM_THREAD(ENABLED);

// This allows for USB serial debug logs
SerialLogHandler logHandler;

// Forward declarations (functions used before they're implemented)
int getSensor();

// This is how often to read the sensor (every 1 second)
std::chrono::milliseconds sensorCheckPeriod = 1s;

// This keeps track of the last int_time we published
unsigned long lastSensorCheckMs;

// The is is the variable where the sensor value is stored.
int heartbeat;

// LED
int red_light_pin= D3;
int green_light_pin = D4;
int blue_light_pin = D5;
int int_time = 1000;

void setup()
{
    // LED Pin Defined
    pinMode(red_light_pin, OUTPUT);
    pinMode(green_light_pin, OUTPUT);
    pinMode(blue_light_pin, OUTPUT);

    // MX30105 Setup
    Serial.begin(115200);
    Serial.println("Initializing...");

    
    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
      Serial.println("MAX30105 was not found. Please check wiring/power. ");
      while (1);
    }
    Serial.println("Place your index finger on the sensor with steady pressure.");

    particleSensor.setup(); //Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

    // Particle Cloud Setup
    Particle.variable("heartbeat", heartbeat);
    

}

void loop()
{
  /* For RGB LED Test
  RGB_color(255, 0, 0); // Red
  delay(int_time);
  RGB_color(0, 255, 0); // Green
  delay(int_time);
  RGB_color(0, 0, 255); // Blue
  delay(int_time);
  RGB_color(255, 255, 125); // Raspberry
  delay(int_time);
  RGB_color(0, 255, 255); // Cyan
  delay(int_time);
  RGB_color(255, 0, 255); // Magenta
  delay(int_time);
  RGB_color(255, 255, 0); // Yellow
  delay(int_time);
  */
  
  RGB_color(30, 0, 0); 
  
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  heartbeat = beatsPerMinute;
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");
  Serial.print("\n");    
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}
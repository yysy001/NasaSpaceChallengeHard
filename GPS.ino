#include <WiFi.h>

#include <HTTPClient.h>

#include <HardwareSerial.h>
#include <TinyGPS++.h>

#include <Adafruit_BMP085.h>

#include "DHT.h"

#include "utils.h"

// Definitions
#define DHT_PIN 33
#define DHT_TYPE DHT21

#define GPS_RX_PIN 35
#define GPS_TX_PIN 34

#define MQ7_PIN 32

 
// Main Objects
//// Sensors
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_BMP085 bmp180;
MQ_7 mq_7(MQ7_PIN);

//// GPS Object
TinyGPSPlus gps;

float currentLat;
float currentLng;

//// Connections
HardwareSerial neo6m(2);
WiFiManager wifiManager("yerson", "char5524");
SupabaseManager supabaseManager("https://vhdoekfqjwwvsklzhzca.supabase.co/rest/v1/weatherdata", "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InZoZG9la2Zxand3dnNrbHpoemNhIiwicm9sZSI6ImFub24iLCJpYXQiOjE3Mjc2MzA3MTcsImV4cCI6MjA0MzIwNjcxN30.PKihHezo0djNd1_XNEqeWDwYaxMsESOSN7w4DXGgXhM");

// Setups
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 5000; // 20000ms = 20s

// Local variables 
//// DHT 21 
float h; // %
float t; // °C

//// BMP180
float p, a;
float m_s_n_m = 2335;

//// MQ
float qa;

void setup()
{
    // Serial and GPS initialization
    Serial.begin(115200);
    neo6m.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); // Inicializar HardwareSerial
    Serial.println("GPS inicializado");

    wifiManager.connect();

    // Sensors initialization
    dht.begin();

    if (!bmp180.begin()) {
	    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    }
}

void loop()
{
    // Handle GPS data
    smartdelay_gps(1000);
    currentLat = 0.0;
    currentLng = 0.0;

    // Handle DHT data
    h = dht.readHumidity();    // %
    t = dht.readTemperature(); // °C

    // Handle BMP data
    p = bmp180.readPressure();
    a = bmp180.readAltitude(101500);

    qa = mq_7.readQA();

    Serial.print(" H: ");
    Serial.print(h);
    Serial.print(" T: ");
    Serial.print(t);
    Serial.print(" P: ");
    Serial.print(p);
    Serial.print(" A: ");
    Serial.print(a);
    Serial.print(" QA: ");
    Serial.print(qa);
    Serial.println();

    if (gps.location.isValid())
    {
        currentLat = gps.location.lat();
        currentLng = gps.location.lng();

        Serial.print("Latitud: ");
        Serial.println(currentLat, 6);
        Serial.print("Longitud: ");
        Serial.println(currentLng, 6);
        Serial.println(); 
    }
    else
    {
        Serial.println("Esperando señal GPS...");
    }

    // Update Supabase data every 20 seconds
    if (millis() - lastUpdateTime >= updateInterval) {
      if (_currentLat != 0.0 || _currentLng != 0.0) {
        supabaseManager.updateData(currentLat, currentLng, p, a, t, h, qa);  
      } else {
        supabaseManager.updateData(0.0, 0.0, 0.0,0.0,0.0,0.0,0.0);  
      }
      lastUpdateTime = millis(); 
    }
}

static void smartdelay_gps(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (neo6m.available())
            gps.encode(neo6m.read());
    } while (millis() - start < ms);
}

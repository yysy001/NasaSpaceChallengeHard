#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// GPS Pins
#define gpsRxPin 5
#define gpsTxPin 4
SoftwareSerial neo6m(gpsTxPin, gpsRxPin);

// Interval for updating the database (20 seconds)
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 20000; // 20000ms = 20s

// Clase para manejar la conexión WiFi
class WiFiManager {
  private:
    const char* ssid;
    const char* password;

  public:
    WiFiManager(const char* ssid, const char* password) {
      this->ssid = ssid;
      this->password = password;
    }

    void connect() {
      Serial.print("Conectando a ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      Serial.println("\nWiFi conectado.");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }

    bool isConnected() {
      return (WiFi.status() == WL_CONNECTED);
    }
};

// Clase para manejar la actualización de datos en Supabase
class SupabaseManager {
  private:
    String apiUrl;
    String apiKey;
    WiFiClientSecure client;
    HTTPClient https;

  public:
    SupabaseManager(String apiUrl, String apiKey) {
      this->apiUrl = apiUrl;
      this->apiKey = apiKey;
      client.setInsecure();
    }

    void updateData(float lat, float lng, String name = "dronepos") {
      if (WiFi.status() == WL_CONNECTED) {
        // Crear la conexión HTTPS con la URL de la API y la tabla weatherdata
        https.begin(client, apiUrl + "?id=eq.1");

        // Añadir los headers necesarios para la solicitud
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Prefer", "return=representation");
        https.addHeader("apikey", apiKey);
        https.addHeader("Authorization", "Bearer " + apiKey);

        // Crear el cuerpo del mensaje en formato JSON
        String postData = "{\"name\":\"" + name + "\",\"lat\":" + String(lat, 6) + ",\"log\":" + String(lng, 6) + "}";

        // Enviar la solicitud PATCH para actualizar la fila con id = 1
        int httpCode = https.PATCH(postData);

        // Imprimir el código de respuesta y el contenido de la respuesta
        String payload = https.getString();
        Serial.println("HTTP Code: " + String(httpCode));
        Serial.println("Response: " + payload);

        // Terminar la conexión HTTPS
        https.end();
      } else {
        Serial.println("Error en la conexión WiFi.");
      }
    }
};

// GPS object
TinyGPSPlus gps;
WiFiManager wifiManager("yerson", "char5524");
SupabaseManager supabaseManager("https://vhdoekfqjwwvsklzhzca.supabase.co/rest/v1/weatherdata", "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InZoZG9la2Zxand3dnNrbHpoemNhIiwicm9sZSI6ImFub24iLCJpYXQiOjE3Mjc2MzA3MTcsImV4cCI6MjA0MzIwNjcxN30.PKihHezo0djNd1_XNEqeWDwYaxMsESOSN7w4DXGgXhM");

void setup() {
  // Serial and GPS initialization
  Serial.begin(115200);
  neo6m.begin(9600);
  Serial.println("GPS inicializado");

  // Conectar a la red WiFi
  wifiManager.connect();
}

void loop() {
  // Handle GPS data
  smartdelay_gps(1000);
  float currentLat = 0.0;
  float currentLng = 0.0;

  if (gps.location.isValid()) {
    // Si la ubicación GPS es válida, obtenemos los valores
    currentLat = gps.location.lat();
    currentLng = gps.location.lng();

    // Imprimir latitud y longitud en el monitor serial
    Serial.print("Latitud: ");
    Serial.println(currentLat, 6);
    Serial.print("Longitud: ");
    Serial.println(currentLng, 6);
    Serial.println("");  // Nueva línea para separación visual
  } else {
    // Si la ubicación no es válida, establecer valores en 0
    Serial.println("Esperando señal GPS...");
  }

  // Update Supabase data every 20 seconds
  if (millis() - lastUpdateTime >= updateInterval) {
    if (currentLat != 0.0 || currentLng != 0.0) {
      supabaseManager.updateData(currentLat, currentLng);  // Solo actualiza si latitud o longitud no es 0
    } else {
      supabaseManager.updateData(0.0, 0.0);  // Actualiza con 0 en ambos campos si no hay señal GPS
    }
    lastUpdateTime = millis();  // Reinicia el temporizador
  }
}

static void smartdelay_gps(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}

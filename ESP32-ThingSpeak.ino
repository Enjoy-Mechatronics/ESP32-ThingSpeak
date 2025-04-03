#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Replace with your ThingSpeak API Key
String serverName = "http://api.thingspeak.com/update?api_key=REPLACE_WITH_YOUR_API_KEY";

// Timer setup
unsigned long lastTime = 0;
const unsigned long timerDelay = 3000; // 3 seconds delay

// DHT Sensor setup
#define DHT_PIN 26           // Pin connected to the DHT sensor (change if necessary)
#define DHT_TYPE DHT11       // DHT11 or DHT22 (change if you're using a DHT11)

DHT dht(DHT_PIN, DHT_TYPE); // Initialize DHT sensor

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WiFi, IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize the DHT sensor
  dht.begin();

  // Initial message for clarity
  Serial.println("Timer set to 10 seconds. It will take 10 seconds before publishing the first reading.");
}

void loop() {
  // Check if it's time to send a new HTTP request
  if (millis() - lastTime >= timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      // Read temperature and humidity from DHT sensor
      float temperature = dht.readTemperature();  // Get temperature in Celsius
      float humidity = dht.readHumidity();       // Get humidity percentage

      // Check if reading was successful
      if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }

      // Construct the server path with temperature and humidity data
      String serverPath = serverName + "&field1=" + String(temperature) + "&field2=" + String(humidity);

      // Start HTTP request
      http.begin(client, serverPath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);  // Print the response payload
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);  // Error code if request failed
      }

      // End the HTTP request and free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }

    // Update lastTime to manage the delay
    lastTime = millis();
  }
}

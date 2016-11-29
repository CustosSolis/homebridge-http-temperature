/*
  Fabrice Masachs
  ESP8266 + DS18B20 
*/

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 5 on the Arduino
#define ONE_WIRE_BUS 5

String readString;

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Replace with your network details
const char* ssid = "Fabrice's Wi-Fi Network";
const char* password = "62236223";

// Web Server on port 80
WiFiServer server(80);

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  // Start up the library
  sensors.begin();
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

// runs over and over again
void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (readString.length() < 100) {
          //store characters to string 
          readString += c; 
          //Serial.print(c);
        }
          
        if (c == '\n' && blank_line) {
          sensors.requestTemperatures(); // Send the command to get temperatures
          
          // Read temperature as Celsius (the default)
          float t = (sensors.getTempCByIndex(0));
          // Read temperature as Fahrenheit
          float f = (sensors.getTempFByIndex(0));

          // Check if any reads failed and exit early (to try again).
          if (isnan(t) || isnan(f)) {
            Serial.println("Failed to read from DS18B20 sensor!");
            strcpy(celsiusTemp,"Failed");
            strcpy(fahrenheitTemp, "Failed");
          } else {
            // You can delete the following Serial.print's, it's just for debugging purposes
            Serial.print("Temperature: ");
            Serial.print(t);
            Serial.println(" *C");
            Serial.print("Temperature: ");
            Serial.print(f);
            Serial.println(" *F");
          }

          /*
          // HTML
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          
          // your actual web page that displays temperature and humidity
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head></head>");
          client.println("<body>");
          client.println("<h1>ESP8266 - DS18B20</h1>");
          client.println("<h3>Temperature in Celsius: ");
          client.println(t);
          client.println("&deg;C</h3>");
          client.println("<h3>Temperature in Fahrenheit: ");
          client.println(f);
          client.println("&deg;F</h3>");
          client.println("</body></html>");
          */
          
          // JSON
          client.println("HTTP/1.1 200 OK");          
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();

          client.print("{");
          client.print("\"temperature\":");
          client.print(t);
          //client.print(fahrenheitTemperature);
          client.print("}");
          client.println();
            
          break;
        }
        
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }
    
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}

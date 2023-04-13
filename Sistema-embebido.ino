#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

 
//#ifndef Pins_Arduino_h
 #define Pins_Arduino_h

// http client resources
String serverAddress = "https://javier-mu.vercel.app/"; // change url
const int serverPort = 80; // Server port

//  firebase car
const char* idValue;
const char* bussyValue;

/*
#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)
*/
/*
static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;
*/

/*
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2; LED
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t RX   = 3;
static const uint8_t TX   = 1;
*/

/************************* Pin Definition *********************************/

#define Ledd1 13 //renta (azul)
#define Led2 12 // indicador MQTT / actualizar 5 secs (verde)
#define Led3 27 // indicador puerta (rojo)
#define Led4 14 // indicador encendido motor (amarillo)
int led_state = HIGH;

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "A1hackdogs"
#define WLAN_PASS       "1234567890"

/************************* Adafruit.io Se5000tup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "rnajera"
#define AIO_KEY       "aio_WCQG45bSAru0CJIam3n5wiIY1htc"
/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe Led1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Led1");


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);

  pinMode(Ledd1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led4, OUTPUT);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  

  delay(500);

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  Serial.println("Accesing...");
  
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(Ledd1, led_state);
    led_state = !led_state;
    delay(200);
    Serial.print(".");
  }
  Serial.println("Conectado!");
  digitalWrite(Ledd1, LOW);
  digitalWrite(Ledd1, HIGH);
  delay(2000);
  digitalWrite(Ledd1, LOW);
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  
  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Led1);

}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    
    if (subscription == &Led1) {
      int Led1_State = atoi((char *)Led1.lastread);
      if (Led1_State == 1){
        
        Serial.print(F("Got: "));
        Serial.println((char *)Led1.lastread);
        delay(3000);
        digitalWrite(Ledd1, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);  
        digitalWrite(Ledd1, LOW);
        delay(100);    
      } else if (Led1_State == 2) {
        Serial.print(F("Got: "));
        Serial.println((char *)Led1.lastread);
        digitalWrite(Led3, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);  
        digitalWrite(Led3, LOW);
        delay(100); 
      } else if (Led1_State == 3) {
        Serial.print(F("Got: "));
        Serial.println((char *)Led1.lastread);
        digitalWrite(Led4, HIGH);   // turn the LED on (HIGH is the voltage level)
      } else if (Led1_State == 4) {
        Serial.print(F("Got: "));
        Serial.println((char *)Led1.lastread);
        digitalWrite(Led4, LOW);   // turn the LED on (HIGH is the voltage level)
      }
    }
  }
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  
}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.println("Connecting to MQTT... ");

  uint8_t retries = 6;
  digitalWrite(Led2, HIGH);
  delay(200);
  digitalWrite(Led2, LOW);
  delay(200);
  digitalWrite(Led2, HIGH);
  delay(200);
  digitalWrite(Led2, LOW);
  delay(200);
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection");
    mqtt.disconnect();
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(Led2, HIGH);
  delay(2000);
  digitalWrite(Led2, LOW);
}

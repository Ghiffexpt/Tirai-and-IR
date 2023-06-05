                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        /*
  Basic ESP8266 MQTT example
  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.
  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <W5500lwIP.h>

#define DHTPIN 4
#define CSPIN 16

Wiznet5500lwIP eth(CSPIN);
DHT dht (DHTPIN, DHT22);

int ledState = LOW;
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
unsigned long previousMillis = 0;
const long interval = 2000;
float temp, humi;
float suhu;
int ON_OFF = 0;

// Update these with values suitable for your network.

WiFiClient espClient;
PubSubClient client(espClient);
IPAddress localIP;
String a ;
unsigned long now ;
const char* mqtt_server = "159.223.61.133";
unsigned long lastMsg = 0;
unsigned long msg22 = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


void setup_eth() {

  delay(10);
  // We start by connecting to a WiFi network
  //  Serial.println();
  //  Serial.print(ESP.getFullVersion());
  //  Serial.println();
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setFrequency(4000000);

  eth.setDefault(); // use ethernet for default route
  int present = eth.begin(mac);
  if (!present) {
    //    Serial.println("no ethernet hardware present");
    while (1);
  }

  //  Serial.print("connecting ethernet");
  while (!eth.connected()) {
    //    Serial.print(".");
    digitalWrite(2, LOW);
    delay(500);
    digitalWrite(2, HIGH);
    delay(500);
    now = millis();
    if (now - lastMsg > 10000) {
      lastMsg = now;
      ESP.reset();
    }

  }
  //  Serial.println();
  //  Serial.print("ethernet ip address: ");
  //  Serial.println(eth.localIP());
  //  Serial.print("ethernet subnetMask: ");
  //  Serial.println(eth.subnetMask());
  //  Serial.print("ethernet gateway: ");
  //  Serial.println(eth.gatewayIP());
}

void callback(char* topic, byte * payload, unsigned int length) {

  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  String buff;
  charToString(topic, buff);

  if (buff == "SML/V30/CNTRL") {
    Serial.println(msg);
  }
  if (buff == "SML/V30/REQ") {
    client.publish ("SML/V30/Suhu", a.c_str()); //a.c_atr() untuk mengubah nilai string menjadi char array

  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "node-redxx1";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), "mecharoot", "mecharnd595")) {
      digitalWrite(2, LOW);
      // Once connected, publish an announcement...
      client.subscribe("SML/V30/CNTRL");
      client.subscribe("SML/V30/REQ");



    } else {

      digitalWrite(2, HIGH);
      // Wait 5 seconds before retrying
      delay(2000);
      digitalWrite(2, LOW);
      delay(2000);
      now = millis();
      if (now - lastMsg > 10000) {
        lastMsg = now;
        ESP.reset();
      }
    }
  }
}

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  setup_eth();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float Humidi = dht.readHumidity();
  float Temper = dht.readTemperature();


  now = millis();

  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;

    //sensor suhu

    if (isnan(humi) || isnan(temp)) {
      //      Serial.print("DHT22 tidak terbaca.... ");
      return;
    }
    else if (Temper != suhu) {
      a = ""; //untuk menampung data yg ingin kirim
      // menjadi { "suhu": Temper,"kelembaban": Humidi};
      a += "{\"suhu\":\"" ;            //untuk memunculkan nilai suhu & kelembaban secara bersamaan
      a += Temper ;                    // data temperature
      a += "\",\"kelembaban\":\"";    //dengan menggunakan string (\) agar tanda kutip biasa berada di dalam tanda kutip
      a += Humidi;                    // data kelembaban
      a += "\"}";
      sprintf (msg, "%s", a);  //(%s) adalah untuk memunculkan data dari variable string
      suhu = Temper;
    }
  }
}


void charToString(char S[], String & D)
{
  String rc(S);
  D = rc;
}

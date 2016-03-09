//Adapted from PubSubClient ESP8266 example

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Fill these in
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);

char msg[11] = "\0";
unsigned long off_time = 0;

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(D1, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  for (int i = 0; i < (length < 10 ? length : 10); i++) {
    msg[i] = payload[i];
  }
  msg[length < 10 ? length : 10] = '\0';

  unsigned long duration = strtol(msg, NULL, 10);

  if (duration == 0) {
    pinMode(D1, INPUT);
    off_time = 0;
  }
  else {
    pinMode(D1, OUTPUT);
    off_time = millis() + duration;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected resubscribe
      client.subscribe("lights/beacon");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (off_time != 0 && millis() > off_time) {
    pinMode(D1, INPUT);
  }
}

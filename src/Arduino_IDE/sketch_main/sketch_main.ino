#include <PubSubClient.h>

#include <ESP32Pinout.h>
#include <WiFiConnection.h>

#define PIN_LED_WIFI_FEEDBACK D21

#define DEVICE_ESPTOUCHv2_PASSWORD "2893701982730182"

#define MQTT_HOST "MQTT_BROKER_HERE"
#define MQTT_PORT MQTT_PORT_HERE
#define MQTT_USER "MQTT_USER_HERE"
#define MQTT_PASSWORD "MQTT_PASSWORD_HERE"
#define MQTT_SECRET_HASH "MQTT_SECRET_HASH_HERE"

#define MQTT_TOPIC_CHANGE_DEVICE_STATUS(mac) ("/alarmouse/mqtt/se/" + String(MQTT_SECRET_HASH) + "/control/status/" + mac).c_str()

bool wifi_configurated = false;
WiFiConnection wifi_connection = WiFiConnection(DEVICE_ESPTOUCHv2_PASSWORD);

WiFiClient wiFiClient;
PubSubClient MQTTClient(wiFiClient);

void mqtt_connect_and_subscribe();
void on_mqtt_message_callback(char*,byte*,unsigned int);

void setup()
{
  Serial.begin(115200);

  MQTTClient.setServer(MQTT_HOST, MQTT_PORT);
  MQTTClient.setCallback(on_mqtt_message_callback);

  pinMode(PIN_LED_WIFI_FEEDBACK, OUTPUT);
}

void loop()
{
  if (!wifi_configurated)
    wifi_configurated = wifi_connection.waitSmartConfig();
  else 
    if (!wifi_connection.connected()) 
      wifi_connection.reconnect();
    else 
      if (!MQTTClient.connected()) mqtt_connect_and_subscribe();

  MQTTClient.loop();

  digitalWrite(PIN_LED_WIFI_FEEDBACK, wifi_connection.connected());

  delay(50);
}

void mqtt_connect_and_subscribe() {
  bool _connected = MQTTClient.connect(
    "alarmouse_mqtt_client@2023" +  random(300),
    MQTT_USER,
    MQTT_PASSWORD
  );

  if (!_connected) return;

  MQTTClient.subscribe(MQTT_TOPIC_CHANGE_DEVICE_STATUS(wifi_connection.getMacAddress()));
}

void on_mqtt_message_callback(char* topic, byte* payload, unsigned int size) {
  Serial.print("[MSG RECEBIDA] Topico: ");
  Serial.print(topic);
  Serial.print(" / Mensagem: ");
  for (int i = 0; i < size; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

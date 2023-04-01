/*
  THIRD-PARTY LIBRARIES
*/
#include <PubSubClient.h>


/*
  CUSTOM LIBRARIES
*/
#include <ESP32Pinout.h>
#include <WiFiConnection.h>
#include <AlarmouseDevice.h>


/*
  DEFINE - PIN
*/
#define PIN_LED_WIFI_FEEDBACK D21
#define PIN_SENSOR_HC_SR501 D5
#define PIN_ALARM D18


/*
  DEFINE - CONFIG
*/
#define DEVICE_ESPTOUCHv2_PASSWORD "2893701982730182"


/*
  DEFINE - MQTT CREDENTIALS
*/
#define MQTT_HOST "MQTT_BROKER_HERE"
#define MQTT_PORT MQTT_PORT_HERE
#define MQTT_USER "MQTT_USER_HERE"
#define MQTT_PASSWORD "MQTT_PASSWORD_HERE"
#define MQTT_SECRET_HASH "MQTT_SECRET_HASH_HERE"


/* 
  DEFINE - MQTT TOPICS
*/
#define MQTT_TOPIC_CHANGE_DEVICE_STATUS(mac) ("/alarmouse/mqtt/se/" + String(MQTT_SECRET_HASH) + "/control/status/" + mac).c_str()


/*
  PROTOTYPES
*/
void mqtt_connect_and_subscribe();
void on_wifi_event_callback(WiFiEvent_t);
void on_mqtt_message_callback(char*,byte*,unsigned int);


/*
  GLOBAL VARIABLES
*/
WiFiConnection wifiConnection = WiFiConnection(
  DEVICE_ESPTOUCHv2_PASSWORD,
  on_wifi_event_callback
);
WiFiClient wiFiClient;
PubSubClient MQTTClient(wiFiClient);


AlarmouseDevice alarmouse = AlarmouseDevice(
  PIN_SENSOR_HC_SR501,
  PIN_ALARM
);

void setup()
{
  Serial.begin(115200);

  MQTTClient.setServer(MQTT_HOST, MQTT_PORT);
  MQTTClient.setCallback(on_mqtt_message_callback);

  pinMode(PIN_LED_WIFI_FEEDBACK, OUTPUT);
}

void loop()
{
  if (!alarmouse.configurated())
    alarmouse.setIsConfigurated(wifiConnection.waitSmartConfig());
  else 
    if (!wifiConnection.connected()) 
      wifiConnection.reconnect();
    else 
      if (!MQTTClient.connected()) mqtt_connect_and_subscribe();

  MQTTClient.loop();
  alarmouse.loop();

  digitalWrite(PIN_LED_WIFI_FEEDBACK, wifiConnection.connected());

  delay(50);
}


void mqtt_connect_and_subscribe() {
  bool _connected = MQTTClient.connect(
    "alarmouse_mqtt_client@2023" +  random(300),
    MQTT_USER,
    MQTT_PASSWORD
  );

  if (!_connected) return;

  MQTTClient.subscribe(MQTT_TOPIC_CHANGE_DEVICE_STATUS(wifiConnection.getMacAddress()));
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

void on_wifi_event_callback(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      uint8_t rvd_data[36] = { 0 };
      esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data));
      Serial.write(rvd_data, sizeof(rvd_data));
      break;
  }
}

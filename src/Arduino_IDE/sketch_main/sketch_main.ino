/*
  THIRD-PARTY LIBRARIES
*/
#include <stdarg.h>
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
#define PIN_LED_MQTT_FEEDBACK D19
#define PIN_SENSOR_HC_SR501 D5
#define PIN_ALARM D18


/*
  DEFINE - CONFIG
*/
#define UUID_V4_LENGTH 36
#define DEVICE_ESPTOUCHv2_PASSWORD "2893701982730182"


/*
  DEFINE - MQTT CREDENTIALS
*/
#define MQTT_HOST "MQTT_BROKER_HERE"
#define MQTT_PORT MQTT_PORT_HERE
#define MQTT_USER "MQTT_USER_HERE"
#define MQTT_PASSWORD "MQTT_PASSWORD_HERE"
#define MQTT_SECRET_HASH "MQTT_SECRET_HASH_HERE"
#define MQTT_PUBLIC_HASH "MQTT_PUBLIC_HASH_HERE"


/* 
  DEFINE - MQTT TOPICS
*/
#define MQTT_TOPIC_CHANGE_DEVICE_STATUS(mac) ("/alarmouse/mqtt/se/" + String(MQTT_SECRET_HASH) + "/control/status/" + mac).c_str()
#define MQTT_TOPIC_CONFIGURE_DEVICE(id) ("/alarmouse/mqtt/em/" + String(MQTT_PUBLIC_HASH) + "/device/configure/" + String(id)).c_str()


/*
  PROTOTYPES
*/
bool is_uuid_v4(char*);
void mqtt_connect_and_subscribe();
void on_wifi_event_callback(WiFiEvent_t);
void on_device_event_callback(DeviceEvent);
void publish_json(const char*,size_t,const char*,...);
void on_mqtt_message_callback(char*,byte*,unsigned int);


/*
  GLOBAL VARIABLES
*/
char* owner_id = NULL;
bool _publish_first_configuration = false;
WiFiConnection wifiConnection = WiFiConnection(
  DEVICE_ESPTOUCHv2_PASSWORD,
  on_wifi_event_callback
);
WiFiClient wiFiClient;
PubSubClient MQTTClient(wiFiClient);
AlarmouseDevice alarmouse = AlarmouseDevice(
  PIN_SENSOR_HC_SR501,
  PIN_ALARM,
  on_device_event_callback
);

void setup() {
  Serial.begin(115200);

  MQTTClient.setServer(MQTT_HOST, MQTT_PORT);
  MQTTClient.setCallback(on_mqtt_message_callback);

  pinMode(PIN_LED_WIFI_FEEDBACK, OUTPUT);
  pinMode(PIN_LED_MQTT_FEEDBACK, OUTPUT);
}

void loop() {
  if (!alarmouse.configurated())
    alarmouse.setIsConfigurated(wifiConnection.waitSmartConfig());
  else 
    if (!wifiConnection.connected()) 
      wifiConnection.reconnect();
    else 
      if (!MQTTClient.connected()) mqtt_connect_and_subscribe();

  if (_publish_first_configuration && MQTTClient.connected()) {
    publish_json(
      MQTT_TOPIC_CONFIGURE_DEVICE(owner_id), 
      35,
      "{\"macAddress\":\"%s\"}",
      wifiConnection.getMacAddress().c_str()
    );
    _publish_first_configuration = false;
  }

  MQTTClient.loop();
  alarmouse.loop();

  digitalWrite(PIN_LED_WIFI_FEEDBACK, wifiConnection.connected());
  digitalWrite(PIN_LED_MQTT_FEEDBACK, MQTTClient.connected());

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
  if (
    strcmp(topic, MQTT_TOPIC_CHANGE_DEVICE_STATUS(wifiConnection.getMacAddress())) == 0
  ) {
    alarmouse.statusChangedByExternal(payload[0]);
    return;
  }
}

void publish_json(const char* topic, size_t max_size, const char* pattern, ...) {
  char buffer[max_size];

  va_list args;
  va_start(args, pattern);

  vsnprintf(buffer, sizeof(buffer), pattern, args);
  buffer[sizeof(buffer) / sizeof(buffer[0]) - 1] = '\0';
  
  va_end(args);

  MQTTClient.publish(topic, buffer);
}

void on_wifi_event_callback(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      uint8_t rvd_data[UUID_V4_LENGTH + 1] = { 0 };
      esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data));
      owner_id = (char*) malloc(sizeof(char) * UUID_V4_LENGTH);
      memcpy(owner_id, rvd_data, UUID_V4_LENGTH + 1);
      if (is_uuid_v4(owner_id))
        _publish_first_configuration = true;
      else {
        free(owner_id);
        alarmouse.resetConfig();
        wifiConnection.resetSmartConfig();
        _publish_first_configuration = false;
      }
      break;
  }
}

bool is_uuid_v4(char* uuid) {
  if (strlen(uuid) != UUID_V4_LENGTH) return false;

  if (uuid[14] != '4') return false;

  if (uuid[8] != '-' || uuid[13] != '-' || uuid[18] != '-' || uuid[23] != '-')
    return false;
  
  char c = uuid[19];
  if (c != '8' && c != '9' && c != 'a' && c != 'b') return false;
  
  for (int i = 0; i < UUID_V4_LENGTH; i++) {
    if (i == 8 || i == 13 || i == 18 || i == 23) continue; 
    if (!isxdigit(uuid[i])) return false;
  }
  
  return true;
}

void on_device_event_callback(DeviceEvent event) {

}
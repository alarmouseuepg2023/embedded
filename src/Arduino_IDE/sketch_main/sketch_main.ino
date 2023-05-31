/*
  THIRD-PARTY LIBRARIES
*/
#include <stdarg.h>
#include <PubSubClient.h>


/*
  CUSTOM LIBRARIES
*/
#include <Debounce.h>
#include <ESP32Pinout.h>
#include <WiFiConnection.h>
#include <AlarmouseDevice.h>
#include <MQTTCredentials.h>
#include <MQTTPublishTaskQueue.h>


/*
  DEFINE - PIN
*/
#define PIN_BTN_RESET_WIFI D18
#define PIN_LED_WIFI_FEEDBACK D21
#define PIN_LED_MQTT_FEEDBACK D19
#define PIN_SENSOR_HC_SR501 D2
#define PIN_ALARM D4
#define PIN_RF_CONTROL D5


/*
  DEFINE - CONFIG
*/
#define UUID_V4_LENGTH 36
#define DEVICE_ESPTOUCHv2_PASSWORD "2893701982730182"


/* 
  DEFINE - MQTT TOPICS
*/
#define MQTT_TOPIC_PUB_CHANGE_DEVICE_STATUS ("/alarmouse/mqtt/es/" + String(MQTT_SECRET_HASH) + "/control/status/change").c_str()
#define MQTT_TOPIC_FAILED_STATUS_CHANGED_ATTEMPT ("/alarmouse/mqtt/es/" + String(MQTT_SECRET_HASH) + "/control/status/change/failed").c_str()
#define MQTT_TOPIC_RECEIVE_STATUS_CHANGES(mac) ("/alarmouse/mqtt/se/" + String(MQTT_SECRET_HASH) + "/control/status/change/waiting_ack/" + mac).c_str()
#define MQTT_TOPIC_PUB_GET_CURRENT_STATUS ("/alarmouse/mqtt/es/" + String(MQTT_SECRET_HASH) + "/control/status/get").c_str()
#define MQTT_TOPIC_SUB_GET_CURRENT_STATUS(mac) ("/alarmouse/mqtt/se/" + String(MQTT_SECRET_HASH) + "/control/status/get/" + mac).c_str()
#define MQTT_TOPIC_ERROR_AT_CREATE_DEVICE(mac) ("/alarmouse/mqtt/se/" + String(MQTT_SECRET_HASH) + "/control/error/create_device/" + mac).c_str()
#define MQTT_TOPIC_CONFIGURE_DEVICE(id) ("/alarmouse/mqtt/em/" + String(MQTT_PUBLIC_HASH) + "/device/configure/" + String(id)).c_str()
#define MQTT_TOPIC_CHANGE_WIFI ("/alarmouse/mqtt/se/" + String(MQTT_SECRET_HASH) + "/control/wifi").c_str()

/*
  PROTOTYPES
*/
void cpu1Task(void*);
bool is_uuid_v4(char*);
void mqtt_connect_and_subscribe();
void on_btn_reset_wifi_callback();
void on_wifi_event_callback(WiFiEvent_t);
void on_device_event_callback(DeviceEvent);
void on_mqtt_message_callback(char*,byte*,unsigned int);


/*
  GLOBAL VARIABLES
*/
String macAddress = "";
char* owner_id = NULL;
bool wifi_has_reset_by_btn = false;
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
Debounce btnResetWifi = Debounce(
  PIN_BTN_RESET_WIFI,
  3000,
  on_btn_reset_wifi_callback
);
Debounce rfControlDebounce = Debounce(
  PIN_RF_CONTROL,
  1000,
  alarmouse.onRfControlPressedHandler
);
MQTTPublishTaskQueue mqttPublishTaskQueue = MQTTPublishTaskQueue();

void setup() {
  Serial.begin(115200);

  alarmouse.setup();
  wifiConnection.setup();

  MQTTClient.setServer(MQTT_HOST, MQTT_PORT);
  MQTTClient.setCallback(on_mqtt_message_callback);

  pinMode(PIN_LED_WIFI_FEEDBACK, OUTPUT);
  pinMode(PIN_LED_MQTT_FEEDBACK, OUTPUT);

  xTaskCreatePinnedToCore(cpu1Task, "cpu1Task", 4096, NULL, 1, NULL, 1);
}

void cpu1Task(void* parameter) {
  while (true) {
    btnResetWifi.loop();
    rfControlDebounce.loop();

    if (MQTTClient.connected() && mqttPublishTaskQueue.hasQueuedMessage()) {
      mqtt_message_t message = mqttPublishTaskQueue.dequeue();
      MQTTClient.publish(message.topic, message.buffer);
    }

    delay(50);
  }
}

void loop() {
  if (!wifiConnection.hasWifiCredentials())
    wifiConnection.waitSmartConfig();
  else 
    if (!wifiConnection.connected()) 
      wifiConnection.reconnect();
    else 
      if (!MQTTClient.connected()) mqtt_connect_and_subscribe();

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

  MQTTClient.subscribe(MQTT_TOPIC_RECEIVE_STATUS_CHANGES(macAddress));
  MQTTClient.subscribe(MQTT_TOPIC_ERROR_AT_CREATE_DEVICE(macAddress));
  MQTTClient.subscribe(MQTT_TOPIC_SUB_GET_CURRENT_STATUS(macAddress));
}

void on_mqtt_message_callback(char* topic, byte* payload, unsigned int size) {
  if (
    strcmp(topic, MQTT_TOPIC_RECEIVE_STATUS_CHANGES(macAddress)) == 0 ||
    strcmp(topic, MQTT_TOPIC_SUB_GET_CURRENT_STATUS(macAddress)) == 0
  ) {
    alarmouse.statusChangedByExternal((char)payload[0]);
    return;
  }

  if (
    strcmp(topic, MQTT_TOPIC_ERROR_AT_CREATE_DEVICE(macAddress)) == 0
  ) {
    wifiConnection.resetSmartConfig();
    return;
  }
}

void on_wifi_event_callback(WiFiEvent_t event) {
  if (event == SYSTEM_EVENT_STA_GOT_IP) {
    macAddress = wifiConnection.getMacAddress();
    alarmouse.setIsConfigurated();

    mqttPublishTaskQueue.enqueue(
      MQTT_TOPIC_PUB_GET_CURRENT_STATUS, 
      35,
      "{\"macAddress\":\"%s\"}",
      macAddress.c_str()
    );
    
    if (wifi_has_reset_by_btn) {
      wifi_has_reset_by_btn = false;
      String _ssid = wifiConnection.getSsid();
      
      mqttPublishTaskQueue.enqueue(
        MQTT_TOPIC_CHANGE_WIFI, 
        44 + _ssid.length() + 1,
        "{\"macAddress\":\"%s\",\"ssid\":\"%s\"}",
        macAddress.c_str(),
        _ssid.c_str()
      );

      return;
    }
    
    if (USE_FLASH_MEMORY) return;

    uint8_t rvd_data[UUID_V4_LENGTH + 1] = { 0 };
    esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data));
    owner_id = (char*) malloc(sizeof(char) * UUID_V4_LENGTH);
    memcpy(owner_id, rvd_data, UUID_V4_LENGTH + 1);
    
    if (!is_uuid_v4(owner_id)) {
      wifiConnection.resetSmartConfig();
      return;
    }

    mqttPublishTaskQueue.enqueue(
      MQTT_TOPIC_CONFIGURE_DEVICE(owner_id),
      35,
      "{\"macAddress\":\"%s\"}",
      macAddress.c_str()
    );
    
    return;
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
  if (event == DeviceEvent::STATUS_CHANGED) {
    mqttPublishTaskQueue.enqueue(
      MQTT_TOPIC_PUB_CHANGE_DEVICE_STATUS,
      48,
      "{\"macAddress\":\"%s\",\"status\":\"%d\"}",
      macAddress.c_str(),
      static_cast<int>(alarmouse.getStatus())
    );
    return;
  }

  if (event == DeviceEvent::FAILED_STATUS_CHANGED_ATTEMPT) {
    mqttPublishTaskQueue.enqueue(
      MQTT_TOPIC_FAILED_STATUS_CHANGED_ATTEMPT,
      48,
      "{\"macAddress\":\"%s\",\"status\":\"%d\"}",
      macAddress.c_str(),
      static_cast<int>(alarmouse.getStatus())
    );
    return;
  }
}

void on_btn_reset_wifi_callback() {
  wifi_has_reset_by_btn = true;
  wifiConnection.resetSmartConfig();
  alarmouse.triggerAlarm2UserFeedback();
}
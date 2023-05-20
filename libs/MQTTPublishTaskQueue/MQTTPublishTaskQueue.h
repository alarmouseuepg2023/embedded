#ifndef MQTT_PUBLISH_TASK_QUEUE_H
#define MQTT_PUBLISH_TASK_QUEUE_H

#include <Arduino.h>
#include <freertos/queue.h>

#define MAX_QUEUED_MESSAGES 3


struct mqtt_message_t {
  const char* topic;
  const char* buffer;
};


class MQTTPublishTaskQueue {

private:
  QueueHandle_t queue;

public:
  MQTTPublishTaskQueue();
  ~MQTTPublishTaskQueue();
  bool hasQueuedMessage();
  mqtt_message_t pop();
  void push(const char*,size_t,const char*,...);

};


#endif
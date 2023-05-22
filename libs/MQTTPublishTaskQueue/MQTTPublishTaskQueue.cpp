#include "MQTTPublishTaskQueue.h"


MQTTPublishTaskQueue::MQTTPublishTaskQueue() {
  this->queue = xQueueCreate(MAX_QUEUED_MESSAGES, sizeof(mqtt_message_t));
}

MQTTPublishTaskQueue::~MQTTPublishTaskQueue() {
  vQueueDelete(this->queue);
}

bool MQTTPublishTaskQueue::hasQueuedMessage() {
  return uxQueueMessagesWaiting(this->queue) > 0;
}

void MQTTPublishTaskQueue::enqueue(const char* topic, size_t max_size, const char* pattern, ...) {
  char buffer[max_size];

  va_list args;
  va_start(args, pattern);

  vsnprintf(buffer, sizeof(buffer), pattern, args);
  buffer[sizeof(buffer) / sizeof(buffer[0]) - 1] = '\0';
  
  va_end(args);

  mqtt_message_t message = {
    .topic=strdup(topic),
    .buffer=strdup(buffer)
  };

  xQueueSendToBack(this->queue, &message, portMAX_DELAY);
}

mqtt_message_t MQTTPublishTaskQueue::dequeue() {
  mqtt_message_t message;
  xQueueReceive(this->queue, &message, portMAX_DELAY);

  return message;
}

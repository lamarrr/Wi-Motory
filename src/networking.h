/**
 * @file main.cc
 * @author Basit Ayantunde (rlamarrr@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-05-26
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef MARVIN_NETWORKING
#define MARVIN_NETWORKING

#include <AsyncMqttClient.h>
#include <WiFi.h>


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include "config.h"
#include "motor.h"
#include "utils.h"

AsyncMqttClient mqttClient;
TimerHandle_t kWiFiPTimer;
TimerHandle_t kMQTTPTimer;

constexpr uint8_t kNotificationLed{3};

TimerHandle_t kBlinkNotificationTimer{};

void BlinkTimer(TimerHandle_t) {
  static uint8_t state{0};
  state = !state;

  digitalWrite(kNotificationLed, state);
}

void WiFiPersistenceTimer(TimerHandle_t timer) {
  if (WIFI_PASSWORD != nullptr) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  } else {
    WiFi.begin(WIFI_SSID);
  }
}
void MQTTPersistenceTimer(TimerHandle_t timer) { mqttClient.connect(); }

void WiFiEventCallback(WiFiEvent_t event) {
  utils::Print("---- WiFi-event: ", event);

  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      utils::Print("WiFi Connected");
      utils::Print("\tIP Address: ", WiFi.localIP());
      utils::Print("\tMAC Address: ", WiFi.macAddress());
      utils::Print("\tRSSI: ", WiFi.RSSI(), " DB");
      utils::Print("Connecting to MQTT Broker");
      xTimerStart(kMQTTPTimer, 0);
      xTimerStart(kBlinkNotificationTimer, 0);
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      utils::Print("WiFi Disconnected");
      xTimerStart(kWiFiPTimer, 0);
      xTimerStop(kBlinkNotificationTimer, 0);
      break;
  }
}

void MQTTConnectCallback(bool sessionPresent) {
  utils::Print("Connected to MQTT Broker");
  utils::Print(
      "Subscribing to Topics: " OBSTACLE_TOPIC MOTORY_TOPIC NECK_TOPIC);
  uint16_t motory_sub_packet_id = mqttClient.subscribe(MOTORY_TOPIC, 2);
  uint16_t obstacle_sub_packet_id = mqttClient.subscribe(OBSTACLE_TOPIC, 2);
  uint16_t neck_sub_packet_id = mqttClient.subscribe(NECK_TOPIC, 2);
  utils::Print("\tSubscription Packet IDs:\n\t\tMotory: ", motory_sub_packet_id,
               " Obstacle: ", obstacle_sub_packet_id,
               " Neck: ", neck_sub_packet_id);
}

void MQTTDisconnectCallback(AsyncMqttClientDisconnectReason reason) {
  utils::Print("MQTT Disconnected");

  switch (reason) {
    case AsyncMqttClientDisconnectReason::ESP8266_NOT_ENOUGH_SPACE:
      utils::Print("\tReason: ESP8266 Not Enough Space");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
      utils::Print("\tReason: Identifier Rejected");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
      utils::Print("\tReason: Malformed Credentials");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
      utils::Print("\tReason: Not Authorized");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
      utils::Print("\tReason: Server Unavailable");
      break;
    case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
      utils::Print("\tReason: Unacceptable Protocol Version");
      break;
    case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
      utils::Print("\tReason: TCP Disconnected");
      xTimerStart(kWiFiPTimer, 0);
      break;

    case AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT:

      utils::Print("\tReason: TLS Bad Fingerutils::Print");
      break;
  }
  // WiFi still connected? Start Yourself Up
  if (WiFi.isConnected()) xTimerStart(kMQTTPTimer, 0);
}

void MQTTSubscribeCallback(uint16_t packetId, uint8_t qos) {
  utils::Print("Subscribed to Topic");
  utils::Print("\tPacket ID: ", packetId);
  utils::Print("\tQOS: ", qos);
}

void MQTTUnsubscribeCallback(uint16_t packetId) {
  utils::Print("Unsubscribed from Topic");
}

void MQTTMessageCallback(char* topic, const char* payload,
                         AsyncMqttClientMessageProperties properties,
                         size_t len, size_t index, size_t total) {
  utils::Print("Received Payload");
  utils::Print("\tTopic: ", topic);
  utils::Print("\tQOS: ", properties.qos);
  utils::Print("\tDup: ", properties.dup);
  utils::Print("\tRetain: ", properties.retain);
  utils::Print("\tLength: ", len);
  utils::Print("\tIndex: ", index);
  utils::Print("\tTotal: ", total);
  char* str = new char[len + 1];
  memcpy(str, payload, len * sizeof(char));
  str[len] = '\0';

  utils::Print("\tPayload: ", str);

  if (strcmp(topic, MOTORY_TOPIC) == 0) {
    utils::Print("Executing Motory");
    ExecuteMotory(str);
  } else if (strcmp(topic, NECK_TOPIC) == 0) {
  } else if (strcmp(topic, OBSTACLE_TOPIC) == 0) {
  } else {
    utils::Print("Got Unrecognized Command");
  }

  delete[] str;
}

void MQTTPublishCallback(uint16_t packetId) {
  utils::Print("Publish Acknowledged to Topic");
  utils::Print("Packet ID: ", packetId);
}

void NetworkSetup() {
  pinMode(kNotificationLed, OUTPUT);
  kBlinkNotificationTimer = xTimerCreate(
      "BlinkNotificationTask", pdMS_TO_TICKS(500), true, nullptr, BlinkTimer);

  kWiFiPTimer = xTimerCreate("WiFiPersistenceTimer", pdMS_TO_TICKS(500), false,
                             nullptr, WiFiPersistenceTimer);

  kMQTTPTimer = xTimerCreate("MQTTPersistenceTimer", pdMS_TO_TICKS(500), false,
                             nullptr, MQTTPersistenceTimer);

  if (WiFi.status() == WL_CONNECTED) WiFi.disconnect();
  WiFi.mode(WIFI_MODE_STA);
  WiFi.onEvent(WiFiEventCallback);

  mqttClient.onConnect(MQTTConnectCallback);
  mqttClient.onDisconnect(MQTTDisconnectCallback);
  mqttClient.onSubscribe(MQTTSubscribeCallback);
  mqttClient.onUnsubscribe(MQTTUnsubscribeCallback);
  mqttClient.onMessage(MQTTMessageCallback);
  mqttClient.onPublish(MQTTPublishCallback);
  mqttClient.setServer(MQTT_HOST_IP, MQTT_HOST_PORT);

  mqttClient.setClientId(MQTT_NODE_ID);

  // to allow for details to set
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

#endif
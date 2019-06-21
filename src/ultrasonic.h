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
#ifndef MARVIN_ULTRASONIC
#define MARVIN_ULTRASONIC
#include "Arduino.h"
#include "Servo.h"


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

namespace ultrasonic {
constexpr uint8_t KServoPin{16};
constexpr uint8_t kTriggerPin{22};
constexpr uint8_t kEchoPin{1};
constexpr double kMaxDetectionRange{0.5};  // metres
constexpr double S_US{1e6};
// speed of sound at 25*c
constexpr double kSoundSpeed{346.18};  // metres per second
constexpr uint64_t kDetectionTimeOut =
    static_cast<uint64_t>(kMaxDetectionRange / kSoundSpeed * S_US *
                          2.0);  // microseconds, back and forth

Servo kServo;

TimerHandle_t kRangingTimer;

// metres
double ReadDistance() {
  digitalWrite(kTriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(kTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(kTriggerPin, LOW);
  int64_t duration =
      pulseIn(kEchoPin, HIGH, kDetectionTimeOut);  // microseconds

  return (duration * kSoundSpeed / S_US) / 2.0;
}

// run at 250ms interval
void RangingTimer(TimerHandle_t) {
  static bool forward = true;
  int angle = 60;

  auto distance = ultrasonic::ReadDistance();
  utils::Print("Distance: ", distance);
  if (forward) {
    angle += 20;
  } else {
    angle -= 20;
  }
  ultrasonic::kServo.write(angle);
  if (forward) {
    if (angle >= 110) {
      forward = false;
    }

  } else {
    if (angle <= 40) forward = true;
  }
  // TODO(lamarrr): obstacle lower than 90* of bot?
  // if (angle >= 90) {
  if (distance > 0) {
    utils::Print("Obstacle at ", angle);
  }
  //}
  // publish if and only ig obstacle ahead
  // spec sheet 0.17 s / 60* @ 4.8v
  // 30* -> 0.8

  // notify
}  // namespace ultrasonic

void SetUp() {
  kRangingTimer = xTimerCreate("RangingTimer", pdMS_TO_TICKS(80), true, nullptr,
                               RangingTimer);
  pinMode(KServoPin, OUTPUT);
  pinMode(kTriggerPin, OUTPUT);
  pinMode(kEchoPin, INPUT);
  digitalWrite(KServoPin, LOW);
  // ydigitalWrite(kTriggerPin, LOW);
  xTimerStart(kRangingTimer, 0);
  kServo.attach(KServoPin, -1, 0, 180);
}

}  // namespace ultrasonic
#endif
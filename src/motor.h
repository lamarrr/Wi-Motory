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
#ifndef MARVIN_MOTOR

#define MARVIN_MOTOR
#include <Arduino.h>
#include "utils.h"

constexpr uint32_t kPWMFrequency{30000};
constexpr uint8_t kPWMBitResolution{8};
struct MotorPins {
 public:
  uint8_t enable_pin{};
  uint8_t pin_f{};
  uint8_t pin_b{};
  uint8_t pwm_channel{};
  MotorPins() = default;
  MotorPins(uint8_t enable_pin, uint8_t pin_f, uint8_t pin_b)
      : enable_pin{enable_pin}, pin_f{pin_f}, pin_b{pin_b} {}
};

struct MotorState {
  uint8_t pin_f : 1;
  uint8_t pin_b : 1;
  uint16_t speed;
  MotorState() = default;

  MotorState(uint8_t speed, uint8_t pin_f, uint8_t pin_b)
      : speed{speed}, pin_f{pin_f}, pin_b{pin_b} {}
};

class Motor {
 public:
  MotorPins pins{};
  MotorState state{};

  Motor(const MotorPins pins, const MotorState state)
      : pins{pins}, state{state} {}

  void SetUp(void) {
    static uint8_t curr_pwm_channel{0};

    pinMode(pins.enable_pin, OUTPUT);
    pinMode(pins.pin_f, OUTPUT);
    pinMode(pins.pin_b, OUTPUT);

    ledcSetup(curr_pwm_channel, kPWMFrequency, kPWMBitResolution);

    ledcAttachPin(pins.enable_pin, curr_pwm_channel);
    pins.pwm_channel = curr_pwm_channel;

    curr_pwm_channel++;
  }

  void Write(void) {
    ledcWrite(pins.pwm_channel, state.speed);
    digitalWrite(pins.pin_b, state.pin_b);
    digitalWrite(pins.pin_f, state.pin_f);
  }
};

namespace motors {
Motor l{{15, 2, 4}, {0, 0, 0}};

Motor r{{13, 12, 14}, {0, 0, 0}};

void SetUp() {
  l.SetUp();
  r.SetUp();
}
};  // namespace motors

enum class MotorDirection : uint8_t {
  Forward = 1,
  Backward = 2,
  ClockWise = 4,
  AntiClockWise = 8,
  Stop = 16
};

void ExecuteMotory(const char* args) {
  int direction{};
  int speed{};

  sscanf(args, "%d:%d", &direction, &speed);

  utils::Print("Direction: ", direction, " Speed: ", speed);

  switch (static_cast<MotorDirection>(direction)) {
    //
    case MotorDirection::Forward:
      utils::Print("Called Forward");
      motors::l.state.pin_f = 1;
      motors::l.state.pin_b = 0;
      motors::l.state.speed = speed;

      motors::r.state.pin_f = 1;
      motors::r.state.pin_b = 0;
      motors::r.state.speed = speed;

      motors::l.Write();
      motors::r.Write();
      break;

    //
    case MotorDirection::Backward:

      utils::Print("Called Backward");
      motors::l.state.pin_f = 0;
      motors::l.state.pin_b = 1;
      motors::l.state.speed = speed;

      motors::r.state.pin_f = 0;
      motors::r.state.pin_b = 1;
      motors::r.state.speed = speed;

      motors::l.Write();
      motors::r.Write();
      break;

    //
    case MotorDirection::ClockWise:

      utils::Print("Called Clockwise");
      motors::l.state.pin_f = 1;
      motors::l.state.pin_b = 0;
      motors::l.state.speed = speed;

      motors::r.state.pin_f = 0;
      motors::r.state.pin_b = 1;
      motors::r.state.speed = speed;

      motors::l.Write();
      motors::r.Write();
      break;

    //
    case MotorDirection::AntiClockWise:

      utils::Print("Called Anti-Clockwise");
      motors::l.state.pin_f = 0;
      motors::l.state.pin_b = 1;
      motors::l.state.speed = speed;

      motors::r.state.pin_f = 1;
      motors::r.state.pin_b = 0;
      motors::r.state.speed = speed;

      motors::l.Write();
      motors::r.Write();
      break;

    //
    case MotorDirection::Stop:

      utils::Print("Called Stop");
      motors::l.state.pin_f = 0;
      motors::l.state.pin_b = 0;
      motors::l.state.speed = 0;

      motors::r.state.pin_f = 0;
      motors::r.state.pin_b = 0;
      motors::r.state.speed = 0;

      motors::l.Write();
      motors::r.Write();
      break;
  }
}

#endif
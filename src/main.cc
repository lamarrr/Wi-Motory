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
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include "motor.h"
#include "networking.h"
#include "ultrasonic.h"

void setup() {
  Serial.begin(9600);
  motors::SetUp();
  NetworkSetup();
  ultrasonic::SetUp();

  // */
}

void loop() {}

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
#ifndef MARVIN_UTILS
#define MARVIN_UTILS

#include <Arduino.h>
#include <iostream>

namespace utils {
void Print() { std::cout << std::endl; }

template <typename T, typename... U>
void Print(const T &t, const U &... u) {
  std::cout << t;
  Print(u...);
}
}  // namespace utils
#endif
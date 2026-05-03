#pragma once

#include <cstdint>

#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace yeelight_remote {

enum Command : uint8_t {
  PRESS = 0x01,
  PRESS_AND_ROTATE_RIGHT = 0x02,
  PRESS_AND_ROTATE_LEFT = 0x03,
  ROTATE_RIGHT = 0x04,
  ROTATE_LEFT = 0x05,
  LONG_PRESS = 0x06,
};

enum Position : uint8_t {
  ID = 2,
  COMMAND = 4,
  PARITY = 8,
};

class YeelightRemote : public Component, public uart::UARTDevice {
 public:
  void dump_config() override;
  void loop() override;

  Trigger<> *get_press_trigger() { return &this->press_trigger_; }
  Trigger<> *get_double_press_trigger() { return &this->double_press_trigger_; }
  Trigger<> *get_long_press_trigger() { return &this->long_press_trigger_; }
  Trigger<> *get_left_trigger() { return &this->left_trigger_; }
  Trigger<> *get_right_trigger() { return &this->right_trigger_; }
  Trigger<> *get_press_left_trigger() { return &this->press_left_trigger_; }
  Trigger<> *get_press_right_trigger() { return &this->press_right_trigger_; }

 protected:
  Command message_command_{0};
  uint8_t message_parity_{0};
  uint8_t message_size_{0};
  uint8_t previous_message_id_{0};
  uint32_t last_press_time_ms_{0};
  bool is_reading_message_{false};
  bool is_press_pending_{false};

  void handle_byte_(uint8_t incoming_byte);
  void handle_command_();
  void handle_pending_press_();
  void handle_press_();

  Trigger<> press_trigger_;
  Trigger<> long_press_trigger_;
  Trigger<> double_press_trigger_;
  Trigger<> left_trigger_;
  Trigger<> right_trigger_;
  Trigger<> press_left_trigger_;
  Trigger<> press_right_trigger_;
};

}  // namespace yeelight_remote
}  // namespace esphome

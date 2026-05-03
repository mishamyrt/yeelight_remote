#include "yeelight_remote.h"

#include <cstdint>

#include "esphome/core/log.h"

namespace esphome {
namespace yeelight_remote {

static const char *TAG = "yeelight-remote";
static const uint8_t REMOTE_MESSAGE_START = 0x5A;

void YeelightRemote::dump_config() { ESP_LOGCONFIG(TAG, ""); }

void YeelightRemote::loop() {
  while (this->available()) {
    uint8_t read_byte = this->read();
    this->handle_byte_(read_byte);
  }
  this->handle_pending_press_();
}

void YeelightRemote::handle_byte_(uint8_t incoming_byte) {
  if (incoming_byte == REMOTE_MESSAGE_START && !this->is_reading_message_) {
    ESP_LOGD(TAG, "Got message start");
    this->is_reading_message_ = true;
    this->message_size_ = 0;
    this->message_parity_ = 0;
  }
  if (!this->is_reading_message_) {
    return;
  }

  this->message_size_ += 1;

  switch (this->message_size_) {
    case Position::ID:
      ESP_LOGD(TAG, "Found message id: %d", incoming_byte);
      if (this->previous_message_id_ == incoming_byte) {
        ESP_LOGD(TAG, "This is the same as the previous message, skipping");
        this->is_reading_message_ = false;
      } else {
        this->previous_message_id_ = incoming_byte;
      }
      break;
    case Position::COMMAND:
      ESP_LOGD(TAG, "Found command: %d", incoming_byte);
      this->message_command_ = static_cast<Command>(incoming_byte);
      break;
    case Position::PARITY:
      this->is_reading_message_ = false;  // Parity byte is last
      const uint8_t parity = this->message_parity_ % 255;
      ESP_LOGD(TAG, "Parity: got %d, have %d", incoming_byte, parity);
      if (parity == incoming_byte) {
        this->handle_command_();
      } else {
        ESP_LOGD(TAG, "Parity mismatch, skipping");
      }

      return;
  }

  this->message_parity_ += incoming_byte;
}

void YeelightRemote::handle_command_() {
  switch (this->message_command_) {
    case Command::PRESS:
      ESP_LOGD(TAG, "Triggering press");
      this->handle_press_();
      break;
    case Command::LONG_PRESS:
      ESP_LOGD(TAG, "Triggering long press");
      this->long_press_trigger_.trigger();
      break;
    case Command::PRESS_AND_ROTATE_RIGHT:
      ESP_LOGD(TAG, "Triggering press and rotate right");
      this->press_right_trigger_.trigger();
      break;
    case Command::PRESS_AND_ROTATE_LEFT:
      ESP_LOGD(TAG, "Triggering press and rotate left");
      this->press_left_trigger_.trigger();
      break;
    case Command::ROTATE_RIGHT:
      ESP_LOGD(TAG, "Triggering rotate right");
      this->right_trigger_.trigger();
      break;
    case Command::ROTATE_LEFT:
      ESP_LOGD(TAG, "Triggering rotate left");
      this->left_trigger_.trigger();
      break;
  }
}

void YeelightRemote::handle_pending_press_() {
  if (!this->is_press_pending_) {
    return;
  }

  const uint32_t now = millis();
  const uint32_t elapsed = now - this->last_press_time_ms_;
  if (elapsed > this->double_press_timeout_ms_) {
    this->is_press_pending_ = false;
    ESP_LOGD(TAG, "Triggering press");
    this->press_trigger_.trigger();
  }
}

void YeelightRemote::handle_press_() {
  const uint32_t now_ms = millis();
  const uint32_t elapsed = now_ms - this->last_press_time_ms_;

  if (this->is_press_pending_ && elapsed <= this->double_press_timeout_ms_) {
    ESP_LOGD(TAG, "Triggering double press");
    this->is_press_pending_ = false;
    this->double_press_trigger_.trigger();
    return;
  }

  this->is_press_pending_ = true;
  this->last_press_time_ms_ = now_ms;

  ESP_LOGD(TAG, "Press pending...");
}

}  // namespace yeelight_remote
}  // namespace esphome

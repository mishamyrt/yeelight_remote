#include "yeelight_remote.h"
#include "esphome/core/log.h"

namespace esphome {
namespace yeelight_remote {

static const char *TAG = "yeelight-remote";

static const uint8_t REMOTE_COMMAND_PRESS = 0x01;
static const uint8_t REMOTE_COMMAND_PRESS_AND_ROTATE_RIGHT = 0x02;
static const uint8_t REMOTE_COMMAND_PRESS_AND_ROTATE_LEFT = 0x03;
static const uint8_t REMOTE_COMMAND_ROTATE_RIGHT = 0x04;
static const uint8_t REMOTE_COMMAND_ROTATE_LEFT = 0x05;
static const uint8_t REMOTE_COMMAND_LONG_PRESS = 0x06;

static const uint8_t REMOTE_MESSAGE_START = 0x5A;
static const uint8_t REMOTE_POSITION_COMMAND = 3;
static const uint8_t REMOTE_POSITION_PARITY = 7;


void YeelightRemote::dump_config() {
    ESP_LOGCONFIG(TAG, "");
}

void YeelightRemote::loop() {
    while (this->available()) {
        uint8_t read_byte = this->read();
        this->handle_char_(read_byte);
    }
}

void YeelightRemote::handle_char_(uint8_t read_byte) {
    if (read_byte == REMOTE_MESSAGE_START && !this->is_in_message_) {
        ESP_LOGD(TAG, "Got message start");
        this->is_in_message_ = true;
        this->message_size_ = 0;
        this->parity_ = 0;
    }
    if (!this->is_in_message_) {
        return;
    }

    if (this->message_size_ == 1) {
        ESP_LOGD(TAG, "Found message id: %d", read_byte);
        if (this->previous_message_id_ == read_byte) {
            ESP_LOGD(TAG, "This is the same as the previous message, so skipping this");
            this->is_in_message_ = false;
        }
        this->previous_message_id_ = read_byte;
    } else if (this->message_size_ == REMOTE_POSITION_COMMAND) {
        ESP_LOGD(TAG, "Found command: %d", read_byte);
        this->command_ = read_byte;
    } else if (this->message_size_ == REMOTE_POSITION_PARITY) {
        ESP_LOGD(TAG, "Parity got of: %d", read_byte);
        ESP_LOGD(TAG, "Calculated parity of: %d", this->parity_ % 255);
        if (this->parity_ % 255 == read_byte) {
            switch (this->command_) {
                case REMOTE_COMMAND_PRESS:
                    this->handle_press_();
                    break;
                case REMOTE_COMMAND_PRESS_AND_ROTATE_RIGHT:
                    this->handle_press_and_rotate_right_();
                    break;
                case REMOTE_COMMAND_PRESS_AND_ROTATE_LEFT:
                    this->handle_press_and_rotate_left_();
                    break;
                case REMOTE_COMMAND_ROTATE_RIGHT:
                    this->handle_rotate_right_();
                    break;
                case REMOTE_COMMAND_ROTATE_LEFT:
                    this->handle_rotate_left_();
                    break;
                case REMOTE_COMMAND_LONG_PRESS:
                    this->handle_long_press_();
                    break;
            }
        } else {
            ESP_LOGD(TAG, "Parity is incorrect, skipping message");
        }
        this->is_in_message_ = false;
    }

    this->parity_ += read_byte;
    this->message_size_ += 1;
}

void YeelightRemote::handle_press_() {
    ESP_LOGD(TAG, "Press");
    this->press_trigger_->trigger();
}

void YeelightRemote::handle_long_press_() {
    ESP_LOGD(TAG, "Long press");
    this->long_press_trigger_->trigger();
}

void YeelightRemote::handle_rotate_left_() {
    ESP_LOGD(TAG, "Left");
    this->left_trigger_->trigger();
}

void YeelightRemote::handle_rotate_right_() {
    ESP_LOGD(TAG, "Right");
    this->right_trigger_->trigger();
}

void YeelightRemote::handle_press_and_rotate_left_() {
    ESP_LOGD(TAG, "Press Left");
    this->press_left_trigger_->trigger();
}

void YeelightRemote::handle_press_and_rotate_right_() {
    ESP_LOGD(TAG, "Press Right");
    this->press_right_trigger_->trigger();
}

} // namespace yeelight_remote
} // namespace esphome

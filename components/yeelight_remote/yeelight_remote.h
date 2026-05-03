#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace yeelight_remote {
    class YeelightRemote : public Component, public uart::UARTDevice {

    public:
        void dump_config() override;
        void loop() override;

        Trigger<> *get_press_trigger() const { return press_trigger_; }
        Trigger<> *get_double_press_trigger() const { return double_press_trigger_; }
        Trigger<> *get_long_press_trigger() const { return long_press_trigger_; }
        Trigger<> *get_left_trigger() const { return left_trigger_; }
        Trigger<> *get_right_trigger() const { return right_trigger_; }
        Trigger<> *get_press_left_trigger() const { return press_left_trigger_; }
        Trigger<> *get_press_right_trigger() const { return press_right_trigger_; }

    protected:
        uint32_t last_press_time_ = 0;
        uint8_t parity_ = 0;
		uint8_t previous_message_id_ = 0;
        uint8_t message_size_ = 0;
		uint8_t command_ = 0;
		bool is_in_message_ = false;
		bool press_pending_ = false;

        void handle_char_(uint8_t read_byte);
		void handle_pending_press_();
        void handle_press_();

		void fire_press_();
		void fire_double_press_();
		void fire_long_press_();
		void fire_rotate_left_();
		void fire_rotate_right_();
		void fire_press_and_rotate_left_();
		void fire_press_and_rotate_right_();

		Trigger<> *press_trigger_ = new Trigger<>();
		Trigger<> *long_press_trigger_ = new Trigger<>();
		Trigger<> *double_press_trigger_ = new Trigger<>();
		Trigger<> *left_trigger_ = new Trigger<>();
		Trigger<> *right_trigger_ = new Trigger<>();
		Trigger<> *press_left_trigger_ = new Trigger<>();
		Trigger<> *press_right_trigger_ = new Trigger<>();
    };
}
}

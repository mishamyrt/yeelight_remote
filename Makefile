.PHONY: format compile

format:
	find components/ -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i

compile:
	esphome compile testdata/yeelight_screen_light_bar_pro.yaml

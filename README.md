# Yeelight LED Screen Light Bar remote support for ESPHome

Contains component for Yeelight YLTD001/YTLD003 remote.

## Usage

1. Flash your Yeelight LED Screen Light Bar with esphome following instructions from [dckiller51 repository](https://github.com/dckiller51/esphome-yeelight-led-screen-light-bar).
2. Add this repo as `external_components` to configuration:
  ```yaml
  external_components:
    - source:
        type: git
        url: https://github.com/mishamyrt/yeelight_remote
      components: [ yeelight_remote ]
  ```
3. Configure remote:
  ```yaml
  uart:
    - id: remote_bus
      baud_rate: 4800
      rx_pin: GPIO2
  
  yeelight_remote:
    double_press_timeout: 500
    on_press:
      - light.toggle: light1
    on_left:
      then:
  	  - light.dim_relative:
  	    id: light1
  		relative_brightness: -5%
    on_right:
      then:
  	  - light.dim_relative:
  	    id: light1
  		relative_brightness: 5%
    on_long_press:
      - light.toggle: ring
    on_press_left:
      - 
    on_press_right:
      - 
    on_double_press:
      - 
  ```

## License

MIT

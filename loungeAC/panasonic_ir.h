#include "esphome.h"
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "ir_Panasonic.h"


const uint16_t kIrLed = 19;  // ESP8266 GPIO pin to use. Recommended: 0 (D3).
IRPanasonicAc ac(kIrLed, false, false);  //(pin, inverted, use_modulation)

class PanasonicAC : public Component, public Climate {
 public:
  void setup() override {
    ac.begin();
    ac.on();
    ac.setTemp(21);
    ac.setFan(kPanasonicAcFanAuto);  //Min,Med,Max,Auto
    ac.setMode(kPanasonicAcHeat);
    ac.setSwingVertical(false);
    ac.setSwingHorizontal(false);
  }

  climate::ClimateTraits traits() {
    auto traits = climate::ClimateTraits();
    traits.set_supports_current_temperature(true);
    //traits.set_supports_auto_mode(true);
    //traits.set_supports_cool_mode(true);
    //traits.set_supports_heat_mode(true);
    traits.set_supports_two_point_target_temperature(false);
    //traits.set_supports_away(false);
    traits.set_visual_min_temperature(16);
    traits.set_visual_max_temperature(30);
    traits.set_visual_temperature_step(1.f);

    traits.set_supports_auto_mode(true);
    traits.set_supports_cool_mode(true);
    traits.set_supports_heat_mode(true);
    traits.set_supports_fan_only_mode(true);    //fan only mode is really just thermostat at 27
    traits.set_supports_dry_mode(true);
    //traits.set_supports_away(true);   //economy

    traits.set_supports_fan_mode_auto(true);
    traits.set_supports_fan_mode_low(true);
    traits.set_supports_fan_mode_medium(true);
    traits.set_supports_fan_mode_high(true);

    traits.set_supports_swing_mode_off(true);   //auto?
    //traits.set_supports_swing_mode_both(true);
    traits.set_supports_swing_mode_vertical(true);
    //traits.set_supports_swing_mode_horizontal(true);

    return traits;
  }
  void control(const ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      // User requested mode change
      ClimateMode mode = *call.get_mode();
      // Send mode to hardware
      switch(mode) {
        case CLIMATE_MODE_HEAT:
          ac.on();
          ac.setMode(kPanasonicAcHeat);
          break;
        case CLIMATE_MODE_COOL:
          ac.on();
          ac.setMode(kPanasonicAcCool);
          break;
        case CLIMATE_MODE_AUTO:
          ac.on();
          ac.setMode(kPanasonicAcAuto);
          break;
        case CLIMATE_MODE_FAN_ONLY:  //fan only
          ac.on();
          //this->target_temperature = kPanasonicAcFanModeTemp;
          ac.setMode(kPanasonicAcFan);
          break;
        case CLIMATE_MODE_DRY:
          ac.on();
          ac.setMode(kPanasonicAcDry);
          break;
        case CLIMATE_MODE_OFF:
          ac.off();
          break;
      }
    // Publish updated state
    this->mode = mode;
    this->publish_state();
    }
    if (call.get_target_temperature().has_value()) {
      // User requested target temperature change
      float temp = *call.get_target_temperature();
      // Send target temp to climate
      ac.setTemp(temp);
      this->target_temperature = temp;
      this->publish_state();
    }
    /*if (call.get_away().has_value()) {
      bool awayMode = *call.get_away();
      // Send target temp to climate
      ac.setTemp(awayMode);
      this->away = awayMode;
      this->publish_state();
    }*/
    if (call.get_fan_mode().has_value()) {
      ClimateFanMode fanMode = *call.get_fan_mode();
      switch(fanMode) {
        case CLIMATE_FAN_AUTO:
          ac.setFan(kPanasonicAcFanAuto);
          break;
        case CLIMATE_FAN_LOW:
          ac.setFan(kPanasonicAcFanMin);
          break;
        case CLIMATE_FAN_MEDIUM:
          ac.setFan(kPanasonicAcFanMed);
          break;
        case CLIMATE_FAN_HIGH:
          ac.setFan(kPanasonicAcFanMax);
          break;
      }
      this->fan_mode = fanMode;
      this->publish_state();
    }
    if (call.get_swing_mode().has_value()) {
      ClimateSwingMode swingMode = *call.get_swing_mode();
      switch(swingMode) {
        case CLIMATE_SWING_OFF:
          ac.setSwingVertical(kPanasonicAcSwingVMiddle);
          ac.setSwingHorizontal(kPanasonicAcSwingHMiddle);
          break;
        case CLIMATE_SWING_BOTH:
          ac.setSwingVertical(kPanasonicAcSwingVAuto);
          ac.setSwingHorizontal(kPanasonicAcSwingHAuto);
          break;
        case CLIMATE_SWING_VERTICAL:
          ac.setSwingVertical(kPanasonicAcSwingVAuto);
          ac.setSwingHorizontal(kPanasonicAcSwingHMiddle);
          break;
        case CLIMATE_SWING_HORIZONTAL:
          ac.setSwingVertical(kPanasonicAcSwingVMiddle);
          ac.setSwingHorizontal(kPanasonicAcSwingHAuto);
          break;
      }
      this->swing_mode = swingMode;
      this->publish_state();
    }

    ac.send();
  }
};
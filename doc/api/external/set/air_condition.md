# /api/external/set/air_condition

## Classification

- Behavior: Service
- DataType: tier4_external_api_msgs/srv/SetAirCondition

## Description

エアコンの制御を実施する

## Requirement

- エアコンの制御を実施すること。
- `temperature_mode=TEMPERATURE_CELSIUS` は 18.0 から 32.0 まで 0.5 刻み、`TEMPERATURE_FAHRENHEIT` は 60 から 85 の整数とすること。
- `TEMPERATURE_LO` は最大冷房、`TEMPERATURE_HI` は最大暖房とし、この2つでは `temperature` を使わないこと。
- 指示値を車両側で受理できない場合は失敗を返すこと。

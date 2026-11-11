# /api/external/get/air_condition

## Classification

- Behavior: Topic
- DataType: tier4_external_api_msgs/msg/AirConditionStatus

## Description

エアコンの設定情報を取得する。

## Requirement

- エアコンの設定情報が取得できること。
- `TEMPERATURE_LO` は最大冷房、`TEMPERATURE_HI` は最大暖房とすること。この2つでは `temperature` を使わないこと。
- `temperature_mode=TEMPERATURE_UNKNOWN` は、車両が温度を対応表へ戻せないことを表すこと。このとき `temperature` は使わないこと。

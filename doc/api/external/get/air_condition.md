# /api/external/get/air_condition

## Classification

- Behavior: Topic
- DataType: tier4_external_api_msgs/msg/AirConditionStatus

## Description

接続中の車両の設定温度と HVAC (1st Row) Auto 状態を取得する。

## Requirement

- `/vehicle/status/air_condition` を同じ型のまま `/api/external/get/air_condition` へ中継すること。
- 車両状態を受信する前は publish しないこと。
- `temperature="unknown"` かつ `unit=0` は、車両が温度を対応表へ戻せないことを表すこと。
- `enabled` は HVAC 1st Row Auto が ON のとき true、それ以外は false とすること。

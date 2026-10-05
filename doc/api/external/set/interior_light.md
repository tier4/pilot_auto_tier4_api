# /api/external/set/interior_light

## Classification

- Behavior: Service
- DataType: tier4_external_api_msgs/srv/SetInteriorLight

## Description

室内灯の制御を実施する。

## Requirement

- グローバルオーバーヘッドライトの ON/OFF を実施すること。
- `enabled` が true のとき ON、false のとき OFF とすること。

# /api/external/get/interior_light

## Classification

- Behavior: Topic
- DataType: tier4_external_api_msgs/msg/InteriorLightStatus

## Description

室内灯の状態を取得する

## Requirement

- グローバルオーバーヘッドライトの ON/OFF が取得できること。
- `enabled` が true のとき ON、false のとき OFF であること。

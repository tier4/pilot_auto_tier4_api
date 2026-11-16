# /api/external/get/hazard_alert

## Classification

- Behavior: Topic
- DataType: tier4_external_api_msgs/msg/HazardAlert

## Description

自車周辺を方位で分割した領域(セクタ)ごとに、衝突・逸脱・近接の警報を取得する

## Requirement

- セクタごとの危険度と、その発生源が取得できること。
- 配信されないことは、警報が無いことを表さないこと。

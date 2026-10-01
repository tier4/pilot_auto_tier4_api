# /api/external/set/air_condition

## Classification

- Behavior: Service
- DataType: tier4_external_api_msgs/srv/SetAirCondition

## Description

接続中の車両の設定温度と HVAC (1st Row) Auto の ON/OFF を同時に更新する。車両固有の要求値への変換は車両インターフェースが行う。

## Requirement

- `/vehicle/air_condition/command` へ同じ型のまま転送し、その応答を返すこと。
- 摂氏は `unit=1`、華氏は `unit=2` とし、温度は文字列で指定すること。
- `"LO"` は最大冷房、`"HI"` は最大暖房とし、この2つでは `unit` を使わないこと。
- `enabled=true` を Auto ON、`false` を Auto OFF とし、設定温度と同時に更新すること。
- 車両が受理できない温度は、Auto を含めて反映せず失敗を返すこと。
- 成功は車両インターフェースが command を受理したことを表す。実車への反映は `/api/external/get/air_condition` で確認すること。

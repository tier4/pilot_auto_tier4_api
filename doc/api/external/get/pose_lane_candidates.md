# /api/external/get/pose_lane_candidates

## Classification

- Behavior: Service
- DataType: tier4_external_api_msgs/srv/GetPoseLaneCandidates

## Description

車両の現在位置において、現在レーンとして許容可能なレーン（lanelet）の候補を取得する。
[/api/routing/set_route](https://docs.autoware.org/main/design/autoware-architecture-v1/interfaces/ad-api/list/api/routing/set_route/) でルートを設定する際に、ルートの始点となるレーンを選ぶ目的で使用する。

## Requirement

- 自己位置を基準として、現在レーンとして許容可能なレーンの候補をすべて取得できること。
- 取得した候補の中から始点レーンを選んでルートを設定した場合、planning がそのレーンを現在レーンとして認識すること（[制約](#constraints)を満たす場合に限る）。
- rule-based の planner（behavior_path_planner）と AI ベースの planner（diffusion_planner / ml_planner）のどちらの構成でも、上記を満たすこと。

## Interface

```txt
# Request
---
# Response
tier4_external_api_msgs/ResponseStatus status
int64[] lanelet_ids
```

| Name        | Type             | Description                                        |
| ----------- | ---------------- | -------------------------------------------------- |
| status      | `ResponseStatus` | 処理結果。                                         |
| lanelet_ids | `int64[]`        | 現在レーンとして許容可能な lanelet の ID。順不同。 |

## Behavior

次の条件をすべて満たす lanelet を候補として返す。

| 条件                           | 初期値     |
| ------------------------------ | ---------- |
| 車両位置が lanelet 内にある    | 距離 0 m   |
| lanelet の向きと車両の向きの差 | 45° 以内   |
| lanelet 中心線からの距離       | 1.0 m 未満 |

- 閾値はパラメータで設定する。
- 候補の順序に意味はなく、優先度などの付加情報も返さない。

処理結果は次のとおり。

| 状況                                                         | status.code | lanelet_ids |
| ------------------------------------------------------------ | ----------- | ----------- |
| 候補あり                                                     | SUCCESS     | 候補の ID   |
| 候補なし                                                     | SUCCESS     | 空          |
| 自己位置の未初期化、地図の未受信など、計算に必要な情報がない | ERROR       | 空          |

## Constraints

- クライアントは、取得した候補の中から始点レーンを 1 つ選び、ルートの最初の segment の preferred_primitive に置くこと。
  - AI ベースの planner は各 segment の preferred_primitive しか参照しないため、alternatives に置くだけでは始点として認識されない。
- ルートには、始点レーン以外に、車両位置と重なる lanelet を含めないこと。
  - 重なりの判定は 2D で行い、lanelet の向きや高さは問わない。
- ルートの始点レーン以外の preferred lanelet が、車両位置の近くを通らないこと。
  - planner によっては、向きを見ずに、間引いた中心線の点との距離で始点を選ぶ。そのため、重なっていなくても近くを通る lanelet が始点として選ばれる場合がある。
- 上記を満たさない場合、planning が始点レーン以外を現在レーンとして選ぶ可能性がある。その結果、rule-based の構成では走行を開始できず、AI ベースの構成では誤ったレーンを前提に走行を開始する。
  - 周回ルート、交差点内からの発進、立体交差、対向車線を通るルートでは特に注意すること。
  - この API は、この制約が満たされているかどうかを検証しない。
- 自己位置推定の精度はこの API では考慮しない。自己位置推定が安定していることは、クライアントが確認すること。
- 判定は呼び出し時点の自己位置で行う。候補を取得してからルートを設定するまでの間に車両が動かないようにすることは、クライアントの責務とする。

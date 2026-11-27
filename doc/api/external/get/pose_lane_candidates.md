# /api/external/get/pose_lane_candidates

## Classification

- Behavior: Service
- DataType: tier4_external_api_msgs/srv/GetPoseLaneCandidates

## Description

車両の現在位置において、現在レーンとして許容可能なレーン（lanelet）の候補を取得する。
[/api/routing/set_route](https://docs.autoware.org/main/design/autoware-architecture-v1/interfaces/ad-api/list/api/routing/set_route/) でルートを設定する際に、ルートの始点となるレーンを選ぶ目的で使用する。

## Requirement

- 自己位置を基準として、現在レーンとして許容可能なレーンの候補をすべて取得できること。
- 取得した候補の中から始点レーンを選んでルートを設定した場合、現在レーンの判定によって走行開始が妨げられないこと（[制約](#constraints)を満たす場合に限る）。

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

- クライアントは、取得した候補の中から始点レーンを 1 つ選び、ルートの最初の segment に置くこと。
- ルートには、始点レーン以外に、車両位置と重なる lanelet を含めないこと。
  - 重なりの判定は 2D で行い、lanelet の向きや高さは問わない。
  - 走行開始前の現在レーンは、ルートに含まれるすべての lanelet から 2D の距離のみで選ばれる。そのため、車両位置と重なる lanelet がほかにあると、始点レーン以外が現在レーンとして選ばれ、走行を開始できない場合がある。
  - 周回ルート、交差点内からの発進、立体交差、対向車線を通るルートでは特に注意すること。
  - この API は、この制約が満たされているかどうかを検証しない。
- 自己位置推定の精度はこの API では考慮しない。自己位置推定が安定していることは、クライアントが確認すること。
- 判定は呼び出し時点の自己位置で行う。候補を取得してからルートを設定するまでの間に車両が動かないようにすることは、クライアントの責務とする。

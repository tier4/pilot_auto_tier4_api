# Pose Lane Candidates API (Draft)

> Status: 検討中
> Last updated: 2026-11-27

## 背景

ルート設定に AD API の [`/api/routing/set_route`](https://docs.autoware.org/main/design/autoware-architecture-v1/interfaces/ad-api/list/api/routing/set_route/) を使用する場合、ルートの lanelet segments を指定するために始点にあたるレーンを自分で決める必要がある。通常、ルートの始点は現在の車両位置であり、現状はレーンの計算をクライアント側で行っている。一方、Autoware は planning component が「車両がルート上のどのレーンにいるか」を独自に判定しているため、判定ロジックが二重化して異なる結果を返す可能性がある。クライアントが選んだ始点レーンが planning の判定と一致しない場合、ルートは設定できても走行を開始できない（rule-based 構成）、または planning が誤ったレーンを前提に走行を開始してしまう（AI planner 構成）。これを解決するために「車両の現在位置として許容可能なレーンのリスト」を Autoware から取得する API を追加する。クライアントはこのリストの中から始点レーンを選んでルートを作成することが可能となる。

## 要求

- R1: 現在の車両位置において、ルートの始点（現在レーン）として planning が許容するレーンのリストを取得できること。
- R2: このリストに含まれるレーンを始点としてルートを設定すれば（「利用上の制約」を満たす場合）、planning の各モジュールがそのレーンを現在レーンとして認識すること。
  - rule-based 構成では、認識が食い違うと走行を開始できない。
  - AI planner 構成では、認識が食い違っても走行開始は止まらず、誤ったレーンを前提に経路が生成される。
- R3: rule-based 構成（behavior_path_planner）と AI planner 構成（diffusion_planner / ml_planner）の両方で、R1 と R2 を満たすこと。

## 仕様

### インターフェース

| 項目     | 内容                                                |
| -------- | --------------------------------------------------- |
| Name     | `/api/external/get/pose_lane_candidates`            |
| Behavior | Service                                             |
| DataType | `tier4_external_api_msgs/srv/GetPoseLaneCandidates` |

### データ型

```txt
# Request
# (なし。自己位置を基準に計算する)
---
# Response
tier4_external_api_msgs/ResponseStatus status
int64[] lanelet_ids  # 許容可能な lanelet の ID（順不同）
```

<!--
- 既存の external API の Service に合わせて、`ResponseStatus` を status に使う。
- lanelet ID の型は、AD API の `LaneletPrimitive.id` と合わせて `int64` にする。
- 将来、位置指定が必要になった場合は、Request に位置指定を有効にするフラグと pose を追加する。
  - フラグのデフォルト（false）では自己位置を使うため、既存のクライアントとの互換性を保てる。

  ```txt
  # 将来の拡張イメージ
  bool use_pose          # true: pose を基準にする / false: 自己位置を基準にする
  geometry_msgs/PoseStamped pose
  ```
-->

### 振る舞い

- 自己位置（現在の車両位置）を基準に、ルートの始点として許容可能な lanelet を判定し、該当するものをすべて返す。
- 返す順序に意味は持たせない。推奨度や確度などの付加情報も明確に必要なケースが出るまで含めない。
- 判定基準は planning の各判定のうち最も厳しいもの（またはそれらの積集合）でよい。詳細は「判定基準」を参照。

| 状況                                   | status.code | lanelet_ids |
| -------------------------------------- | ----------- | ----------- |
| 判定成功・候補あり                     | SUCCESS     | 候補の ID   |
| 判定成功・候補なし                     | SUCCESS     | 空          |
| 自己位置が未初期化                     | ERROR       | 空          |
| 地図が未受信                           | ERROR       | 空          |
| その他、計算に必要な情報が揃っていない | ERROR       | 空          |

### 利用上の制約

- クライアントは、この API が返した候補の中から始点レーンを 1 つ選び、それを **ルートの最初の segment の preferred_primitive** に置く。
  - AI planner は各 segment の preferred_primitive しか参照しないため、primitives（alternatives）に置くだけでは始点として認識されない。
- **ルートには、始点レーン以外に、車両位置と重なる lanelet（2D、向きや高さは問わない）を含めないこと。**
- **ルートの始点レーン以外の preferred lanelet が、車両位置の近くを通らないこと。**
  - diffusion_planner は、向きを見ずに、間引いた中心線の点との距離で始点を選ぶ。そのため、重なっていなくても近くを通る lanelet が始点として選ばれる可能性がある。
  - どこまで離れていればよいかの目安は、始点レーンの長さによって変わる（「走行開始前の現在レーン判定」を参照）。
- 上記の制約を満たさないと、planning が始点レーン以外を現在レーンとして選ぶ可能性がある。その結果、rule-based 構成では走行開始できず、AI planner 構成では誤ったレーンを前提に走行を開始する。
  - 周回ルート、交差点内からの発進、立体交差、対向車線を通るルートで特に注意が必要。
- この API は、上記の制約が満たされているかどうかを検証しない。
- 自己位置推定の精度はこの API では考慮しない。自己位置推定が安定していることの確認は、クライアントの責務とする。
- この API は呼び出した時点の自己位置で判定する。リストを取得してから `set_route` を呼ぶまでの間に車両が動かないことの保証（停車中に呼ぶ、動いた場合は取得し直す、など）は、クライアントの責務とする。

### 判定条件（初版）

次の条件をすべて満たす lanelet を候補とする。rule-based 構成と AI planner 構成のどちらの判定よりも厳しい条件である（「判定基準」を参照）。

- 閾値はパラメータとして手動で設定する（planning とは連動しない）。
- 将来的には、planning から閾値や判定結果を取得する方法を検討する。

| 条件                           | 初期値     | パラメータ名（仮）          |
| ------------------------------ | ---------- | --------------------------- |
| 車両位置が lanelet 内にある    | 距離 0 m   | `dist_threshold`            |
| lanelet の向きと車両の向きの差 | 45° 以内   | `yaw_threshold`             |
| lanelet 中心線からの距離       | 1.0 m 未満 | `centerline_dist_threshold` |

## 確認事項

以下は、クライアントの運用に依存するため、クライアントに判断・確認してもらいたい点。

- Q1: 「利用上の制約」（始点レーンを preferred_primitive に置く、始点レーン以外の lanelet が車両位置と重ならず近くも通らない）を守れるか。
  - 周回ルート、交差点内からの発進、立体交差、対向車線を通るルートを使う予定はあるか。
  - 守るのが難しい場合、作成したルートが走行開始できるかを検証する API（「走行開始前の現在レーン判定」の選択肢 2）が必要か。
- Q2: 判定条件の厳しさは運用上問題ないか。
  - 中心線から 1.0 m 以上ずれて停車している場合や、車両の向きが lanelet と 45° 以上ずれている場合は、候補が空になりルートを設定できない。
  - 候補が空のときに、クライアント側でどう扱うか（運転者に通知する、手動運転で位置を直す、など）。
- Q3: 候補が複数ある場合に、順不同で付加情報のないリストから始点レーンを 1 つ選べるか。
  - 選ぶ基準がない、または API 側で順序や付加情報が必要な場合は教えてほしい。
- Q4: 自己位置以外の位置を基準に候補を取得したいユースケースはあるか（初版は自己位置のみ）。
- Q5: 次の点をクライアントの責務とすることを受け入れられるか。
  - 自己位置推定が安定していることの確認。
  - 候補を取得してから `set_route` を呼ぶまでの間に車両が動かないようにすること。

## 設計資料

### Planning の現在レーン判定（調査結果）

対象は次の 2 つの構成とする。

- rule-based 構成: behavior_path_planner と scenario_selector を使う従来の構成。
- AI planner 構成: diffusion_planner または ml_planner を使う構成（launch 引数 `planning_setting` で選択）。
  - behavior_path_planner と scenario_selector は起動しない。
  - 処理の流れは AI planner → trajectory_modifier → trajectory_selector で、safety_planner がバックアップの経路を生成する。

#### rule-based 構成

mission_planner (`autoware_core/planning/autoware_mission_planner`)

- `set_route` で segments が指定された場合、車両位置と始点レーンの整合性はチェックしていない（状態チェックと reroute の安全性チェックのみ）。そのため不整合なルートでも設定は成功してしまう。

behavior_path_planner (`planner_manager`)

- 初期化時 (`resetCurrentRouteLanelet`): `RouteHandler::getClosestLaneletWithinRoute` で、ルート内で最も近い lanelet を距離・角度の制約なしで選ぶ。
- 更新時 (`updateCurrentRouteLanelet`): `getClosestRouteLaneletFromLanelet` / `getClosestLaneletWithConstrainsWithinRoute` に `ego_nearest_dist_threshold` と `ego_nearest_yaw_threshold` を渡して判定する。
  - デフォルトは 3.0 m と 1.046 rad (60°)（`nearest_search.param.yaml`）。

scenario_selector

- `isAlongLane`: ルート内の lanelet に対し、距離 0.0 m、yaw 45° の制約で最近傍を探し、さらに中心線からの距離が 1.0 m 未満であることを判定する。
- 結果は LaneDriving / Parking のシナリオ選択に影響する。

#### AI planner 構成

mission_planner (`autoware_mission_planner_universe`)

- rule-based 構成と同じく、ルート設定時に車両位置と始点レーンの整合性はチェックしていない。
- `RouteHandler` の最近傍 lanelet の関数も、rule-based 構成と同じ実装である。

diffusion_planner (`preprocessing/lane_segments.cpp` の `select_route_segment_indices`)

- ルートの全 segment の **preferred_primitive のみ** を対象に、車両位置に最も近いものを始点とし、そこからルート順に先の lanelet をモデルに入力する。
- 距離は、中心線を 20 点に間引いた点との **3D の点間距離**。ポリゴンとの距離ではない。
- **向きは見ない**。距離の上限もない。距離が同じ場合は、ルート上で前にある segment が選ばれる。
- 毎周期選び直す（前回の結果を引き継がない）。

ml_planner (`preprocessing/items/map.cpp` の `select_route_segment_indices`)

- diffusion_planner と同じく、ルート全体の preferred_primitive のみを対象に毎周期選び直す。
- 距離は **2D のポリゴン距離**（車両位置が内側なら 0）。
- 向きの差が 60° 以内（コード内の定数）の lanelet から選ぶ。該当がなければ、向きの制限なしで選び直す。
- 距離の上限はない。距離が同じ場合は、ルート上で前にある segment が選ばれる。

safety_planner（バックアップの経路生成）

- 最初（ルートの受信時）に、ルートの **全 primitives** から 2D のポリゴン距離が最も近い lanelet を選ぶ。向きは、距離が同じ場合の選択にだけ使う。距離の上限はない。
- 以降は、選んだ lanelet から連続性を保って追従する（距離 6.0 m、向き 90° の制約）。

trajectory_modifier / trajectory_selector

- 現在レーンの判定はしない。車両や trajectory がルートやレーン上にあるかを確認するフィルタもない。

operation_mode_transition_manager

- 停車中の走行開始時に確認するのは、trajectory 上に車両から 1.5 m・45° 以内の点があることだけ。
- そのため、誤ったレーンを前提にした trajectory でも、車両の近くから始まっていれば走行開始は止まらない。

#### まとめ

- 現在レーンの判定は、構成ごと・モジュールごとに対象（preferred のみ / 全 primitives）、距離（2D / 3D、ポリゴン / 点）、向き、継続性が異なる。
- どのモジュールにも距離の上限がない。車両がルートから外れていても、最も近い lanelet に黙って割り当てる。
- API は、これらすべてを満たす厳しめの判定にする（「判定基準」を参照）。

### 判定基準

- 要求は「リストから選べば走行を開始できること」であり、planning の判定と完全に一致している必要はない。
- そのため、planning 各モジュールの判定より厳しい判定とする。
  - 各モジュールの閾値が統一されていない問題も、全モジュールの条件を満たす厳しい判定にすることで吸収する。
- 初版の条件は、車両位置を含む lanelet（距離 0 m）、かつ向きの差 45° 以内、かつ中心線からの距離 1.0 m 未満とする。
  - rule-based 構成の scenario_selector の `isAlongLane` と、behavior_path_planner の制約（3.0 m / 60°）の両方を満たす。
  - AI planner 構成の ml_planner（60°）と safety_planner（6.0 m / 90°）の制約も満たす。
  - diffusion_planner は向きも距離の上限も見ないため、条件の厳しさでは対応できない。ルートの形で対応する（「利用上の制約」を参照）。
- 厳しくしすぎると、実際には走行できる状況でも候補が空になり、ルートを設定できなくなる（可用性とのトレードオフ）。
  - 例: 中心線から 1.0 m 以上ずれて停車している場合（「クライアントへの確認事項」Q2）。
- 判定ロジックと閾値の一元化
  - API 側で判定を再実装すると、planning のパラメータやロジックが変わったときに乖離する。
  - 初版は閾値をパラメータとして手動で設定する。planning と同じ関数・パラメータを使う構成（パラメータの共有方法、実装場所）は将来検討する。

### 走行開始前の現在レーン判定

始点レーンを正しく選んでも、ルートの他の部分によっては、planning が始点レーン以外を現在レーンとして選ぶ場合がある。これが「利用上の制約」の根拠である。

#### rule-based 構成（behavior_path_planner）の動作

- 自動運転モードで Autoware が制御していない間（＝走行開始前）は、毎周期 `resetCurrentRouteLanelet` が呼ばれる（`behavior_path_planner_node.cpp`）。
  - 現在レーンは、前回の結果を引き継がずに毎回選び直される。
- `resetCurrentRouteLanelet` は `RouteHandler::getClosestLaneletWithinRoute` を使う。
  - 対象は **ルートに含まれる全 lanelet**（全 segment の preferred と alternatives）であり、始点 segment に限定されない。
  - 判定は **2D のポリゴン距離のみ**。yaw（向き）、高さ（z）、ルート上の順序は考慮しない。
  - 車両位置を含む lanelet が複数ある場合は、いずれも距離 0 となる。そのときは R-tree の探索順で最初に見つかったものが選ばれ、どれが選ばれるかは事実上不定。
- 自動運転開始後は `updateCurrentRouteLanelet` に切り替わる。前回の現在レーンからの連続性と、距離・yaw の制約を使って追従する。
  - そのため、問題になるのは主に **走行開始前の初期選択** である。

#### AI planner 構成の動作

- diffusion_planner と ml_planner は、走行開始の前後を問わず、毎周期ルート全体の preferred lanelet から始点を選び直す。
  - 走行中も初期選択と同じ判定が続くため、ルートの後半が車両の近くを通る場合は、走行中にも始点の取り違えが起こりうる。
- diffusion_planner は、中心線を 20 点に間引いた点との距離で選ぶ。
  - 始点レーン A が長いと点の間隔が広がり、A 上の車両から A の最も近い点までの距離が大きくなる。例えば長さ 100 m なら点の間隔は約 5.3 m で、最大で約 2.6 m に車線内の横ずれが加わる。
  - そのため、ルートの後半にある別の lanelet B の点が、A の点より車両に近くなることがある。B は A と重なっている必要はなく、隣の車線でもよい。
- safety_planner は最初に一度だけ全 primitives から選び、以降は追従する。初期選択の問題は rule-based 構成と同じだが、走行中に取り違えることはない。

#### 問題になるケース

クライアントが API の候補から始点レーン A を選んでも、ルートの後半に、車両位置を含む、または A より車両に近い別の lanelet B が含まれていると、現在レーンが B と判定されうる。AI planner 構成（diffusion_planner）では、B が車両位置と重なっていなくても、近くを通るだけで起こりうる。

- ループ／周回ルート: 出発地点を後で再び通過し、その際に A と重なる別の lanelet を通る。
- 交差点内の発進: 交差点内では lanelet が重なっている。ルートが同じ交差点を後で別方向に通過する。
- 立体交差: 高架下で発進し、ルートが後で高架上を通る。2D 判定のため z の違いを区別できない。
- 逆方向の lanelet: ルートが後で対向車線を戻ってくる（U ターンを含むルートなど）。rule-based 構成と diffusion_planner は向きを見ないため区別できない。
  - rule-based 構成では、車両が車線境界付近にいる場合に問題になる。
  - diffusion_planner では、重なっていなくても隣の車線であれば問題になりうる。

B が現在レーンになったときの結果は、構成によって異なる。

- rule-based 構成: 経路がルート後半の位置から生成される、向きが合わずに経路が作れない、などの結果になり、走行開始できない。
- AI planner 構成: 走行開始は止まらない。AI planner には車両がいないレーンから始まるルート情報が入力され、safety_planner も誤ったレーンを基準に経路を生成する。誤った前提のまま走行を開始するため、rule-based 構成より危険である。

#### 対応方針

検討した選択肢は次のとおり。

1. **対象外とし、制約として明記する**
   - 例: 「ルートには、始点レーン以外に車両位置と重なる lanelet を含めないこと」。
   - 実装コストはないが、クライアントがこれを判定するには結局地図計算が必要になる。
2. **ルート設定前の検証 API を追加する**
   - 作成したルート（segments）を渡すと、走行開始前に現在レーンとして選ばれる lanelet が始点レーンと一致するかを判定する。
   - planning と同じ `getClosestLaneletWithinRoute` を使えば、判定を一致させやすい。
3. **候補 API の Response に注意情報を加える**
   - 例: 候補ごとに「ルートに含めると干渉する lanelet」（車両位置と重なる他の lanelet）を返す。
   - 他の lanelet との関係は返せるが、ルート全体を見ないと最終的な判定はできない。
4. **planning 側を修正する**
   - 例: 初期選択で yaw の制約を使う、始点 segment を優先する、など。
   - 根本的な解決になるが、upstream での議論が必要になる。
   - AI planner 構成は、取り違えても走行開始が止まらないため、rule-based 構成より修正の必要性が高い。

→ 当面は 1 とし、「利用上の制約」に明記する。2 や 4 は、クライアントの回答（「クライアントへの確認事項」Q1）に応じて検討する。

### その他の方針

- 基準位置
  - 初版は自己位置専用とする。位置指定は、必要になったときに Request にフラグを追加して対応する（「データ型」を参照）。
- 該当レーンが複数ある場合
  - 交差点内で lanelet が重なっている場合や、隣接レーンにまたがっている場合など。
  - 初版では全候補を順不同で返す。順序付けや付加情報は将来の拡張とする。
- 該当なしの場合
  - 車線外や駐車場（parking シナリオ）にいる場合などは、空リストで成功として返す。
- 必要な情報が揃っていない場合
  - 自己位置推定の未初期化や地図の未受信など、計算に必要な情報が揃っていない場合はエラーとして返す。
- 自己位置推定の精度、取得タイミング
  - 自己位置推定が不安定な場合（初期化済みだが精度が低い場合）や、リストを取得してから set_route を呼ぶまでに車両が動いた場合は、API では扱わずクライアントの責務とする（「利用上の制約」を参照）。

## 決定事項ログ

| 日付       | 内容                                                                                                                         |
| ---------- | ---------------------------------------------------------------------------------------------------------------------------- |
| 2026-11-27 | 初版発行                                                                                                                     |
| 2026-11-27 | AI planner 構成の調査結果を反映。API は rule-based 構成と AI planner 構成の両方を対象とする（R3）。R2 と利用上の制約を更新。 |

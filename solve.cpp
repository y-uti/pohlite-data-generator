#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

// 人数と費用を保持する構造体
struct resource {
  long amount; // 人数
  long cost;   // 費用
  resource(long amount, long cost) : amount(amount), cost(cost) {}
};

// 枝刈りの閾値判定用の関数オブジェクト
struct amount_greater_equal_value {
  bool operator() (const resource& r, long value) const {
    return r.amount >= value;
  }
};

// 下請け会社整列用の関数オブジェクト
struct amount_greater_than {
  bool operator() (const resource& a, const resource& b) const {
    return a.amount > b.amount || (a.amount == b.amount && a.cost < b.cost);
  }
};

int main()
{
  const long LONG_MIN = std::numeric_limits<long>::min();
  const long LONG_MAX = std::numeric_limits<long>::max();

  int m; // プロジェクトに必要な人数
  int n; // 下請け会社数
  std::cin >> m >> n;

  std::vector<resource> companies; // 下請け会社
  companies.reserve(n);
  for (int i = 0; i < n; ++i) {
    long qi; // 各下請け会社の人員数
    long ri; // 各下請け会社への発注費用
    std::cin >> qi >> ri;
    companies.push_back(resource(qi, ri));
  }

  // 下請け会社を人員数の降順に整列する。同一の場合は発注費用の昇順とする
  std::sort(companies.begin(), companies.end(), amount_greater_than());

  // companies[i] 以降の全社による合計人員数
  std::vector<long> totals(n + 1, 0);
  long acc = 0;
  for (int i = n - 1; i >= 0; --i) {
    totals[i] = acc += companies[i].amount;
  }

  // 最良解。反復処理を進めながら更新する
  long answer = LONG_MAX;

  // companies[i - 1] までの各社を用いて実現可能な値のリスト
  std::vector<resource> curr;
  curr.push_back(resource(0, 0));               // 初期値
  curr.push_back(resource(LONG_MIN, LONG_MIN)); // 番兵

  for (int i = 0; i < n; ++i) {

    // ここで curr は (末尾の番兵を除いて) 以下のループ不変条件を満たす
    //   * 先頭要素について
    //       curr[0].amount < m
    //       curr[0].cost   < answer
    //   * 隣接する要素について
    //       curr[k + 1].amount < curr[k].amount
    //       curr[k + 1].cost   < curr[k].cost
    //   * 末尾要素について
    //       curr[tail] >= m - totals[i]

    const long ci_a = companies[i].amount;
    const long ci_c = companies[i].cost;

    // pit (Pass-case ITerator): companies[i] に発注しない場合
    //
    // pend は curr[x] + totals[i + 1] < m を満たす先頭要素を指す
    // 意味: pend 以降は、残り全社に発注しても m に届かないので次回以降の反復では不要
    std::vector<resource>::const_iterator pit = curr.begin();
    const std::vector<resource>::const_iterator pend =
      std::lower_bound(curr.begin(), curr.end(), m - totals[i + 1], amount_greater_equal_value());

    // oit (Order-case ITerator): companies[i] に発注する場合
    //
    // oit は curr[x] + ci_a < m を満たす先頭要素を指す
    // 意味: oit の手前までは、この発注で m に到達するので次回以降の反復では不要
    std::vector<resource>::const_iterator oit =
      std::lower_bound(curr.begin(), curr.end(), m - ci_a, amount_greater_equal_value());
    const std::vector<resource>::const_iterator oend = curr.end();

    // この発注で m に到達するものがあり、それが answer を改善するならば更新する
    if (oit != curr.begin()) {
      const long candidate = (oit - 1)->cost + ci_c;
      if (candidate < answer) {
        answer = candidate;
      }
    }

    // 次回反復のための値を next に積んでいく
    // pit->amount と (oit->amount + ci_a) を比較しながら iterator を進めることで、
    // ループ不変条件を満たす next を整列処理なしで高速に構築する
    std::vector<resource> next;
    next.reserve((pend - pit) + (oend - oit)); // oend == curr.end() なので番兵を含む

    // limit は next の末尾要素の cost を保持する
    // next に積もうとする値の cost が limit 以上ならば、それは現在の末尾要素よりも
    // 少ない人員で費用が高いということなので、積まずに捨てる
    long limit = answer;

    while (true) {
      // pend は一般に番兵を指さないので、pit == pend のときは LONG_MIN に置き換える
      const long pit_a = pit != pend ? pit->amount : LONG_MIN;
      const long pit_c = pit->cost;
      const long oit_a = oit->amount + ci_a; // 発注するので ci_a を加える
      const long oit_c = oit->cost + ci_c;   // 発注するので ci_c を加える
      if (pit_a < oit_a) {
        // [while ループの終了条件]
        // oit が番兵を指していれば oit_a == LONG_MIN + ci_a
        // それにも関わらず pit_a < oit_a が成立するのは pit_a == LONG_MIN のときだけ
        if (++oit == oend) {
          break;
        }
        if (oit_c < limit) {
          next.push_back(resource(oit_a, oit_c));
          limit = oit_c;
        }
      } else if (pit_a == oit_a) {
        const long min_cost = std::min(pit_c, oit_c);
        if (min_cost < limit) {
          next.push_back(resource(pit_a, min_cost));
          limit = min_cost;
        }
        ++pit;
        ++oit;
      } else {
        if (pit_c < limit) {
          next.push_back(*pit);
          limit = pit_c;
        }
        ++pit;
      }
    }

    // 末尾に番兵を積み直して curr に代入する
    next.push_back(resource(LONG_MIN, LONG_MIN));
    curr.swap(next); // next は不要なので代入演算子を使わず swap で高速化する
  }

  std::cout << answer << std::endl;

  return EXIT_SUCCESS;
}

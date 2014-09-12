/*
 * 深さ優先探索による解法
 *
 * 下記の実装を参考にしたもの
 * https://gist.github.com/wonderful-panda/a019722476a5acfd8c6b
 */

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

// 人数と費用を保持する構造体
struct resource {
    long amount; // 人数
    long cost;   // 費用
    resource(const long amount, const long cost) : amount(amount), cost(cost) {}
};

// ソルバー
class solver {
private:

    const long target;                     // プロジェクトに必要な人数
    const std::vector<resource> companies; // 下請け会社

    // 下請け会社整列用の関数オブジェクト
    struct unitprice_less_than {
        bool operator() (const resource& a, const resource& b) const {
            const double up_a = static_cast<double>(a.cost) / a.amount;
            const double up_b = static_cast<double>(b.cost) / b.amount;
            return up_a < up_b || (up_a == up_b && a.amount > b.amount);
        }
    };

    // least_cost の結果を保持するオブジェクト
    struct memo {
        std::vector<resource>::const_iterator it;
        long cost;
        long count;
        long expected;
        memo(const std::vector<resource>::const_iterator it)
            : it(it), cost(0), count(0), expected(std::numeric_limits<long>::max()) {}
        memo(const std::vector<resource>::const_iterator it, const long cost, const long count)
            : it(it), cost(cost), count(count), expected(-1) {}
    };

public:

    // コンストラクタ
    // 下請け会社を人員あたり費用の昇順に整列する。同一の場合は人員数の降順とする
    solver(const long target, const std::vector<resource>& companies)
        : target(target)
        , companies(sort_companies(companies))
    {
    }

    // 問題を解く
    long solve()
    {
        long answer = std::numeric_limits<long>::max();
        solve_recursive(target, 0, companies.begin(), initial_memo(), answer);

        return answer;
    }

private:

    static std::vector<resource> sort_companies(std::vector<resource> companies)
    {
        std::stable_sort(companies.begin(), companies.end(), unitprice_less_than());
        return companies;
    }

    void solve_recursive(
        const long target,
        const long cost,
        const std::vector<resource>::const_iterator it,
        memo m,
        long& best)
    {
        if (target <= 0) {
            if (cost < best) {
                best = cost;
            }
            return;
        }

        if (m.expected == -1) {
            m = least_cost(m);
        }

        if (m.expected < best) {
            solve_recursive(target - it->amount, cost + it->cost, it + 1, m, best);
            solve_recursive(target, cost, it + 1, next_memo(m, it), best);
        }
    }

    memo least_cost(memo m) const
    {
        while (m.it != companies.end()) {
            if (m.it->amount < m.count) {
                m.cost += m.it->cost;
                m.count -= m.it->amount;
                ++m.it;
            } else {
                return update_expected(m);
            }
        }
        return not_reached();
    }

    memo initial_memo() const
    {
        return memo(companies.begin(), 0, target);
    }

    memo next_memo(const memo& m, const std::vector<resource>::const_iterator it) const
    {
        return memo(m.it, m.cost - it->cost, m.count + it->amount);
    }

    memo update_expected(memo m) const
    {
        m.expected = m.cost + m.count * m.it->cost / m.it->amount;
        return m;
    }

    memo not_reached() const
    {
        return memo(companies.end());
    }
};

int main()
{
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

    const long answer = solver(m, companies).solve();

    std::cout << answer << std::endl;

    return EXIT_SUCCESS;
}

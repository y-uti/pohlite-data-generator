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
        solve_recursive(target, 0, companies.begin(), answer);

        return answer;
    }

private:

    static std::vector<resource> sort_companies(std::vector<resource> companies)
    {
        std::stable_sort(companies.begin(), companies.end(), unitprice_less_than());
        return companies;
    }

    void solve_recursive(const long target, const long cost, const std::vector<resource>::const_iterator it, long& best)
    {
        if (target <= 0) {
            if (cost < best) {
                best = cost;
            }
            return;
        }

        const long expected = least_cost(it, cost, target);
        if (expected < best) {
            solve_recursive(target - it->amount, cost + it->cost, it + 1, best);
            solve_recursive(target, cost, it + 1, best);
        }
    }

    long least_cost(std::vector<resource>::const_iterator it, long cost, long count) const
    {
        while (it != companies.end()) {
            if (it->amount < count) {
                cost += it->cost;
                count -= it->amount;
                ++it;
            } else {
                cost += count * it->cost / it->amount;
                return cost;
            }
        }
        return std::numeric_limits<long>::max();
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

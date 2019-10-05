#include <algorithm>
#include <experimental/iterator>
// #include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

struct Study {
    double cost;
    double cost_multiplier;
};

struct StudyLayer {
    int layer_id;
    Study study;
    int size;

    struct PrereqInfo {
        std::optional<int> number_unlocked_per_prev_layer;
        int prev_layer_index_offset = -1;
    } prereq_info;
};

struct LayerResult {
    int layer_id;
    int quantity;
};

struct Result {
    std::vector<LayerResult> layer_results;
    double cost;
};

template<typename... Ts>
void debug(const Ts&... ts) {
    // (std::cout << ... << ts) << '\n';
}

std::string to_path_string(const std::vector<LayerResult>& layer_results) {
    using std::begin;
    using std::end;

    std::ostringstream oss;
    for (const auto& result : layer_results) {
        oss << result.quantity << "x" << result.layer_id << " ";
    }
    return oss.str();
}

void process_layer(
    const std::vector<StudyLayer>& tree,
    int index,
    std::vector<Result>& results,
    std::vector<LayerResult>& ongoing_result,
    double cost_so_far,
    double cost_multiplier_so_far
) {
    if (index >= tree.size()) return;

    const auto& layer = tree[index];

    // Case of not buying any in this layer and continuing on.
    // Especially important for 302.
    ongoing_result.push_back({layer.layer_id, 0});
    process_layer(tree, index + 1, results, ongoing_result, cost_so_far, cost_multiplier_so_far);
    ongoing_result.pop_back();

    auto max = [&] {
        if (layer.prereq_info.number_unlocked_per_prev_layer) {
            auto prev_layer_quantity = ongoing_result.at(ongoing_result.size() + layer.prereq_info.prev_layer_index_offset).quantity;
            auto max_from_prev = *layer.prereq_info.number_unlocked_per_prev_layer * prev_layer_quantity;
            return std::min(layer.size, max_from_prev);
        }

        return layer.size;
    }();
    for (int quantity = 1; quantity <= max; ++quantity) {
        cost_so_far += layer.study.cost * cost_multiplier_so_far;
        cost_multiplier_so_far *= layer.study.cost_multiplier;

        ongoing_result.push_back({layer.layer_id, quantity});
        results.push_back({ongoing_result, cost_so_far});
        process_layer(tree, index + 1, results, ongoing_result, cost_so_far, cost_multiplier_so_far);
        ongoing_result.pop_back();
    }
}

auto get_path_costs(const std::vector<StudyLayer>& tree) -> std::vector<Result> {
    std::vector<Result> results;
    results.reserve(10000);

    std::vector<LayerResult> ongoing_result;
    process_layer(tree, 0, results, ongoing_result, 0, 1);
    return results;
}

int main() {
    std::vector<StudyLayer> layers{
        // {ID, {cost, mult}, size, {unlocks per prev, prev index offset}}
        {240, {1e71, 1}, 1},
        {250, {2e71, 2.5}, 3, {3}},
        {260, {5e71, 6}, 6, {2}},
        {270, {2.74e76, 2}, 3, {3, -2}},
        {280, {6.86e76, 4}, 2, {2}},
        {290, {2.14e77, 1}, 2, {2, -2}},
        {300, {8.57e77, 2}, 2, {1}},
        {302, {2.68e78, 1<<17}, 1, {1, -4}},
        {310, {8.57e77, 64}, 2, {1, -2}},
        {320, {2.68e76, 2}, 2, {1}},

        // Replicants
        {322, {9.32e77, 2}, 1, {1, -3}},
        {330, {1.02e79, 2}, 2, {2}},
        {340, {1.02e79, 1}, 2, {1}}, // 342, 343
        {345, {9.54e78, 1}, 2, {1}}, // 341, 344
        {351, {2.12e79, 4}, 1, {1}},
	
	// Emperor Dimensions
	{360, {1.59e79, 1}, 2, {2}},
	{370, {2.12e79, 2}, 2, {1}},
	{375, {6.62e79, 2}, 2, {1}}, // 372, 383
	{380, {6.62e79, 1}, 2, {1}}, // 381, 382
	{390, {8.28e79, 1}, 2, {1}},
	{392, {8.28e79, 1}, 1, {1}}
    };

    using std::begin;
    using std::end;

    auto path_costs = get_path_costs(layers);
    std::sort(begin(path_costs), end(path_costs), [](const auto& lhs, const auto& rhs) {
        return lhs.cost < rhs.cost;
    });

    std::cout << std::scientific << std::setprecision(2);
    double last_cost = 1;
    int display_count = 0;
    for (const auto& path_cost : path_costs) {
        if (path_cost.cost / last_cost > 1.01) {
            std::cout << path_cost.cost << ": " << to_path_string(path_cost.layer_results) << "\n";
            last_cost = path_cost.cost;
            ++display_count;
        }
    }

    std::cout << display_count << " out of " << path_costs.size() << " shown\n";
}

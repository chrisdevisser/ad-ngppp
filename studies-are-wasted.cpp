#include <algorithm>
#include <experimental/iterator>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct Study {
    int id;
    double cost;
    double cost_multiplier;
    std::vector<int> prereq_id_options;
};

struct Node {
    Study study;
    std::vector<int> reachable_ids;
};

struct Graph {
    Node start;
};

struct PathCost {
    std::unordered_set<int> id_path;
    double cost;
};

template<typename... Ts>
void debug(const Ts&... ts) {
    // (std::cout << ... << ts) << '\n';
}

std::string to_path_string(const std::unordered_set<int>& id_path) {
    using std::begin;
    using std::end;

    std::vector<int> path(begin(id_path), end(id_path));
    std::sort(begin(path), end(path));

    std::ostringstream oss;
    std::copy(begin(path), end(path), std::experimental::ostream_joiner(oss, ','));
    return oss.str();
}

auto get_path_costs(const std::unordered_map<int, Node>& tree, int start_id) -> std::vector<PathCost> {
    std::vector<PathCost> path_costs;
    std::unordered_set<int> visited_ids;

    double cost = 0;
    double cost_multiplier = 1;

    std::function<void(const Node&)> process = [&](const Node& node) {
        if (visited_ids.count(node.study.id)) {
            debug("Already visited ", node.study.id, " - returning");
            return;
        }

        debug("Visiting ", node.study.id);
        cost += node.study.cost * cost_multiplier;
        cost_multiplier *= node.study.cost_multiplier;
        visited_ids.insert(node.study.id);

        path_costs.push_back({visited_ids, cost});
        debug("Added path: ", to_path_string(visited_ids));

        for (auto id : node.reachable_ids) {
            debug("Trying out ", id, " as next node");
            bool has_prereq = std::any_of(begin(node.study.prereq_id_options), end(node.study.prereq_id_options), [&](auto prereq_id) {
                return visited_ids.count(prereq_id);
            });

            if (has_prereq or node.study.prereq_id_options.empty()) {
                process(tree.at(id));
            } else {
                debug("Skipping, ", id, " because no prereq is met");
            }
        }

        visited_ids.erase(node.study.id);
        cost_multiplier /= node.study.cost_multiplier;
        cost -= node.study.cost;

        debug("Backtracking through ", node.study.id);
    };

    process(tree.at(start_id));
    return path_costs;
}

std::vector<int> concat(std::initializer_list<std::vector<int>> vecs) {
    using std::begin;
    using std::end;

    std::vector<int> result;
    for (const auto& vec : vecs) {
        result.insert(end(result), begin(vec), end(vec));
    }

    return result;
}

int main() {
    std::vector layer250s{251, 252, 253};
    std::vector layer260s{261, 262, 263, 264, 265, 266};
    std::vector layer270s{271, 272, 273};
    std::vector layer280s{281, 282};
    std::vector layer290s{291, 292};
    std::vector layer300s{301, 302, 303};
    std::vector layer310s{311, 312};
    std::vector layer320s{321, 323};

    auto layer250s_260s = concat({layer250s, layer260s});
    auto layer260s_270s = concat({layer260s, layer270s});
    auto layer270s_280s = concat({layer270s, layer280s});
    auto layer280s_290s_302 = concat({layer280s, layer290s, {302}});
    auto layer290s_300s = concat({layer290s, layer300s});
    auto layer300s_310s = concat({layer300s, layer310s});
    auto layer310s_320s = concat({layer310s, layer320s});

    std::unordered_map<int, Node> mastery_tree{
        // {ID, {{ID, cost, mult, {prereq options}}, {reachable IDs}}
        {241, {{241, 1e71, 1}, layer250s}},
        {251, {{251, 2e71, 2.5, {241}}, layer250s_260s}},
        {252, {{252, 2e71, 2.5, {241}}, concat({layer250s_260s, layer270s})}},
        {253, {{253, 2e71, 2.5, {241}}, layer250s_260s}},
        {261, {{261, 5e71, 6, {251}}, layer260s_270s}},
        {262, {{262, 5e71, 6, {251}}, layer260s_270s}},
        {263, {{263, 5e71, 6, {252}}, layer260s_270s}},
        {264, {{264, 5e71, 6, {252}}, layer260s_270s}},
        {265, {{265, 5e71, 6, {253}}, layer260s_270s}},
        {266, {{266, 5e71, 6, {253}}, layer260s_270s}},
        {271, {{271, 2.74e76, 2, {252}}, layer270s_280s}},
        {272, {{272, 2.74e76, 2, {252}}, concat({layer270s, layer280s_290s_302})}},
        {273, {{273, 2.74e76, 2, {252}}, layer270s_280s}},
        {281, {{281, 6.86e76, 4, {271, 272}}, layer280s_290s_302}},
        {282, {{282, 6.86e76, 4, {272, 273}}, layer280s_290s_302}},
        {291, {{291, 2.14e77, 1, {272}}, /*layer290s_300s*/}},
        {292, {{292, 2.14e77, 1, {272}}, /*layer290s_300s*/}},
        // {301, {{301, 8.57e77, 2, {291}}, layer300s_310s}},
        {302, {{302, 8.57e77, 2, {272}}, /*layer300s_310s*/}},
        // {303, {{303, 8.57e77, 2, {292}}, layer300s_310s}},
        // {311, {{311, 8.57e77, 64, {301}}, layer310s_320s}},
        // {312, {{312, 8.57e77, 64, {303}}, layer310s_320s}},
        // {321, {{321, 2.68e76, 2, {311}}, layer320s}},
        // {323, {{323, 2.68e76, 2, {312}}, layer320s}}
    };

    using std::begin;
    using std::end;

    auto path_costs = get_path_costs(mastery_tree, 241);
    std::sort(begin(path_costs), end(path_costs), [](const auto& lhs, const auto& rhs) {
        return lhs.cost < rhs.cost;
    });

    std::cout << std::scientific << std::setprecision(2);
    double last_cost = 1;
    for (const auto& path_cost : path_costs) {
        if (path_cost.cost / last_cost > 1.01) {
            std::cout << path_cost.cost << ": " << to_path_string(path_cost.id_path) << "|0\n";
            last_cost = path_cost.cost;
        }
    }
}
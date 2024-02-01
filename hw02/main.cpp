 #include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <climits>
#include <bits/stdc++.h>

/* limits to check on input */
#define LABEL_LOWER_LIMIT 0
#define POINTS_LOWER_LIMIT 2
#define POINTS_UPPER_LIMIT 1000000
#define TRACKS_UPPER_LIMIT 3500000

using namespace std;

struct point {
    bool is_cp = false;
    vector<int> neighbors;

    int component_id;
};

/* input */
int points_number, tracks_number, cp_peak;
vector<point> points;

/* auxiliary parameters */
int cp_area, counter = 0, components_number = 0;
vector<point> components, transposed_components;
vector<int> components_points_number;

/* output */
int visited_areas_number = 0, survival_kits_number = 0;

int invalid_input();

/* find strongly connected components */
void strongly_connected_components();
void scc_util(int point_id, vector<int>& disc, vector<int>& low,
              vector<bool>& in_stack, stack<int>& scc_stack);

/* find the longest paths between components */
void components_longest_path();
void dfs(int component_id, vector<bool>& visited, vector<int>& dp, vector<set<int>>& paths, vector<point>& dfs_components);


int main() {
    // speed up cin
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // read parameters
    cin >> points_number >> tracks_number >> cp_peak;
    if (points_number < POINTS_LOWER_LIMIT || points_number > POINTS_UPPER_LIMIT ||
        tracks_number > TRACKS_UPPER_LIMIT ||
        cp_peak < LABEL_LOWER_LIMIT || cp_peak > points_number - 1)
        return invalid_input();

    points.resize(points_number);
    points[cp_peak].is_cp = true;

    // read paths
    for (int t = 0; t < tracks_number; t++) {
        int p1, p2;
        cin >> p1 >> p2;
        if (p1 < LABEL_LOWER_LIMIT || p1 > points_number - 1 ||
            p2 < LABEL_LOWER_LIMIT || p2 > points_number - 1)
            return invalid_input();

        points[p1].neighbors.emplace_back(p2);
    }

    // find strongly connected components
    strongly_connected_components();

    components.resize(components_number);
    transposed_components.resize(components_number);
    components_points_number.resize(components_number);

    // create components and transposed components graph
    for (const point& current_point : points) {
        components_points_number[current_point.component_id]++;

        if (current_point.is_cp) {
            components[current_point.component_id].is_cp = true;
            cp_area = current_point.component_id;
        }

        for (int neighbor_id : current_point.neighbors) {
            point neighbor_point = points[neighbor_id];

            if (current_point.component_id != neighbor_point.component_id) {
                components[current_point.component_id].neighbors.push_back(neighbor_point.component_id);
                transposed_components[neighbor_point.component_id].neighbors.push_back(current_point.component_id);
            }
        }
    }

    // remove duplicated neighbors from components and transposed components graph
    for (int c = 0; c < components_number; c++) {
        auto component_neighbors = components[c].neighbors;
        sort( component_neighbors.begin(), component_neighbors.end(), greater<int>() );
        component_neighbors.erase( unique( component_neighbors.begin(), component_neighbors.end() ), component_neighbors.end() );
        components[c].neighbors = component_neighbors;

        auto transposed_component_neighbors = transposed_components[c].neighbors;
        sort( transposed_component_neighbors.begin(), transposed_component_neighbors.end() );
        transposed_component_neighbors.erase( unique( transposed_component_neighbors.begin(), transposed_component_neighbors.end() ), transposed_component_neighbors.end() );
        transposed_components[c].neighbors = transposed_component_neighbors;
    }

    // find the longest paths between components
    components_longest_path();

    // output the result
    cout << visited_areas_number << " " << survival_kits_number << "\n";

    return EXIT_SUCCESS;
}

void dfs(int component_id, vector<bool>& visited, vector<int>& dp, vector<set<int>>& paths, vector<point>& dfs_components) {
    visited[component_id] = true;

    point current_component = dfs_components[component_id];
    for (int neighbor_id : current_component.neighbors) {
        if (!visited[neighbor_id]) dfs(neighbor_id, visited, dp, paths, dfs_components);

        if (dp[component_id] < dp[neighbor_id] + 1) {
            dp[component_id] = dp[neighbor_id] + 1;
            paths[component_id].clear();
            paths[component_id].insert(component_id);

            set<int> neighbor_path = paths[neighbor_id];
            for (int component : neighbor_path) {
                paths[component_id].insert(component);
            }
        } else if (dp[component_id] == dp[neighbor_id] + 1) {
            set<int> neighbor_path = paths[neighbor_id];
            for (int component : neighbor_path) {
                paths[component_id].insert(component);
            }
        }
    }
}

 void components_longest_path() {
    vector<int> dp(components_number, 1);
    vector<bool> visited(components_number, false);
    vector<set<int>> paths(components_number);

    vector<int> transposed_dp(components_number, 1);
    vector<bool> transposed_visited(components_number, false);
    vector<set<int>> transposed_paths(components_number);

     for (int i = 0; i < components_number; i++) {
         paths[i].insert(i);
         transposed_paths[i].insert(i);
     }

     dfs(cp_area, visited, dp, paths, components);
     dfs(cp_area, transposed_visited, transposed_dp, transposed_paths, transposed_components);

     for (int component : paths[cp_area]) {
        survival_kits_number += components_points_number[component];
     }

     for (int component : transposed_paths[cp_area]) {
         survival_kits_number += components_points_number[component];
     }

     survival_kits_number -= components_points_number[cp_area];

     visited_areas_number = dp[cp_area] + transposed_dp[cp_area] - 1;
 }

void scc_util(int point_id, vector<int>& disc, vector<int>& low,
              vector<bool>& in_stack, stack<int>& scc_stack) {

    disc[point_id] = low[point_id] = counter++;
    scc_stack.push(point_id);
    in_stack[point_id] = true;

    point current_point = points[point_id];
    for (int neighbor_id : current_point.neighbors) {
        if (disc[neighbor_id] == -1) {
            scc_util(neighbor_id, disc, low, in_stack, scc_stack);
            low[point_id] = min(low[point_id], low[neighbor_id]);
        } else if (in_stack[neighbor_id]) {
            low[point_id] = min(low[point_id], low[neighbor_id]);
        }
    }

    int top_id;
    if (low[point_id] == disc[point_id]) {

        while (scc_stack.top() != point_id) {
            top_id = (int)scc_stack.top();
            in_stack[top_id] = false;
            scc_stack.pop();

            points[top_id].component_id = components_number;
        }

        top_id = (int)scc_stack.top();
        in_stack[top_id] = false;
        scc_stack.pop();

        points[top_id].component_id = components_number;

        components_number++;
    }
}

void strongly_connected_components() {
    vector<int> disc(points_number, -1);
    vector<int> low(points_number, -1);
    vector<bool> in_stack(points_number, false);
    stack<int> scc_stack;

    for (int p = 0; p < points_number; p++)
        if (disc[p] == -1) scc_util(p, disc, low,
                                    in_stack, scc_stack);
}

int invalid_input() {
    printf("Error: Invalid input!\n");
    return EXIT_FAILURE;
}

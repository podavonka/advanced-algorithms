#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <climits>
#include <bits/stdc++.h>

/* limits to check on input */
#define ORDER_LOWER_LIMIT 2
#define ORDER_UPPER_LIMIT 500
#define SIZE_UPPER_LIMIT 10000
#define WEIGHT_LIMIT INT_MAX
#define FIRST_VERTEX 1

using namespace std;

struct vertex {
    int id;
    int class_id;
    int depth;
    vector<pair<int, int>> neighbors;
};

struct pair_comparator {
    constexpr bool operator()(
            pair<int, int> const& a,
            pair<int, int> const& b)
    const noexcept {
        return a.second > b.second;
    }
};

/* input */
int graph_order, graph_size;
vector<vertex> vertices;

/* auxiliary parameters */
int max_depth = 0;
int max_class = 0;
long long current_weight = 0;
vector<vector<vertex>> classes;
vector<vertex> class_neighbors;

/* output */
long long min_weight = LLONG_MAX;

/* utilities */
int invalid_input();
void print_graph(const vector<vertex>& graph);

/* algorithm functions */
void bfs(int current);
void dfs_util(int v, vector<bool> &visited);
void connected_components(int current_depth);
long long prim_mst(const vertex& start, int size, int order, vector<vertex> graph);
long long prim_mst_depths(const vertex& start, int size, int order, vector<vertex> graph);

int main() {
    // input graph parameters
    cin >> graph_order >> graph_size;
    if (graph_order < ORDER_LOWER_LIMIT || graph_order > ORDER_UPPER_LIMIT
        || graph_size < graph_order - 1 || graph_size > SIZE_UPPER_LIMIT)
        return invalid_input();

    // initialization
    vector<vertex> init(graph_order);
    vertices.resize(graph_order);

    // fill
    for (int r = 0; r < graph_size; r++) {
        int v1, v2, weight;
        cin >> v1 >> v2 >> weight;
        if (weight > WEIGHT_LIMIT || v1 < FIRST_VERTEX || v1 > graph_order
            || v2 < FIRST_VERTEX || v2 > graph_order)
            return invalid_input();

        init[v1 - 1].id = v1;
        init[v1 - 1].class_id = 0;
        init[v1 - 1].depth = 0;
        init[v1 - 1].neighbors.emplace_back(v2, weight);

        init[v2 - 1].id = v2;
        init[v2 - 1].class_id = 0;
        init[v2 - 1].depth = 0;
        init[v2 - 1].neighbors.emplace_back(v1, weight);
    }


    for (int k = 0; k < graph_order; k++) {
        classes.clear();
        class_neighbors.clear();

        vertices = init;
        max_depth = 0;
        max_class = 0;
        current_weight = 0;

        bfs(k+1);

        for (int d = 0; d < max_depth + 1; d++) connected_components(d);

        class_neighbors.resize(max_class);
        for (int c = 0; c < max_class; c++) {
            class_neighbors[c].id = c + 1;
            class_neighbors[c].class_id = 0;
            class_neighbors[c].depth = 0;
        }

        vector<int> min_neighbors_row(max_class, WEIGHT_LIMIT);
        vector<vector<int>> min_neighbors(max_class, min_neighbors_row);
        for (int v = 0; v < graph_order; v++) {
            int class_id = vertices[v].class_id;
            int class_depth = vertices[v].depth;
            class_neighbors[class_id].depth = class_depth;

            for (auto n: vertices[v].neighbors) {
                int neighbor_class_id = vertices[n.first - 1].class_id;
                int neighbor_depth = vertices[n.first - 1].depth;

                if (neighbor_class_id == class_id || class_depth == neighbor_depth) continue;

                int min = min_neighbors[class_id][neighbor_class_id];
                if (n.second < min) min_neighbors[class_id][neighbor_class_id] = n.second;
            }
        }

        for (int c = 0; c < max_class; c++) {
            vertex current_class = class_neighbors[c];
            for (int n = 0; n < min_neighbors[c].size(); n++) {
                if (min_neighbors[c][n] < WEIGHT_LIMIT)
                    current_class.neighbors.emplace_back(n + 1, min_neighbors[c][n]);
            }
            class_neighbors[c] = current_class;
        }

        current_weight += prim_mst_depths(class_neighbors[0], max_class, max_class, class_neighbors);

        if (current_weight < min_weight) min_weight = current_weight;
    }

    cout << min_weight;

    return EXIT_SUCCESS;
}

long long prim_mst_depths(const vertex& start, int size, int order, vector<vertex> graph) {
    vector<vertex> spanning_tree(size);
    spanning_tree[0] = start;

    vector<bool> visited(order, false);
    visited[start.id-1] = true;

    int processed_vertices = 1;
    long long weight = 0;

    while (processed_vertices < size) {
        pair<int, int> min_edge(-1,WEIGHT_LIMIT);
        for (int s = 0; s < processed_vertices; s++) {
            for (auto n : spanning_tree[s].neighbors) {
                if (graph[n.first - 1].class_id != spanning_tree[s].class_id || visited[n.first - 1]) {
                    continue;
                }
                if (n.second < min_edge.second && graph[n.first-1].depth > spanning_tree[s].depth) min_edge = n;
            }
        }
        if (min_edge.first == -1) break;
        weight += min_edge.second;
        spanning_tree[processed_vertices] = graph[min_edge.first-1];
        visited[min_edge.first-1] = true;
        processed_vertices++;
    }

    return weight;
}

long long prim_mst(const vertex& start, int size, int order, vector<vertex> graph) {
    vector<vertex> spanning_tree(size);
    spanning_tree[0] = start;

    vector<bool> visited(order, false);

    vector<long long> value(graph.size(), WEIGHT_LIMIT);
    priority_queue<pair<int, int>, vector<pair<int, int>>, pair_comparator> q;
    q.emplace(start.id, 0);

    int processed_vertices = 1;
    long long total_weight = 0;

    value[start.id-1]=0;
    while (!q.empty()) {
        vertex start_vertex = vertices[q.top().first-1];
        visited[start_vertex.id-1] = true;
        q.pop();
        for (auto neighbor : start_vertex.neighbors) {
            int vertex = neighbor.first;
            int weight = neighbor.second;
            if (!visited[vertex-1] && value[vertex-1] > weight && graph[vertex - 1].class_id == start_vertex.class_id) {
                value[vertex-1] = weight;
                spanning_tree[processed_vertices] = start_vertex;
                q.emplace(vertex, value[vertex-1]);
            }
        }
    }

    for_each(value.begin(), value.end(), [&](int weight) {
        if (weight != WEIGHT_LIMIT) {
            total_weight += weight;
        }
    });

    return total_weight;
}

void connected_components(int current_depth) {
    vector<bool> visited(graph_order, false);

    for (int v = 0; v < graph_order; v++) {
        if (vertices[v].depth != current_depth) {
            visited[v] = true;
            continue;
        }
        if (!visited[v]) {
            dfs_util(v, visited);
            if (classes[max_class].size() > 1)
                current_weight += prim_mst(classes[max_class][0], classes[max_class].size(), graph_order, vertices);
            max_class++;
        }
    }
}

void dfs_util(int v, vector<bool> &visited) {
    visited[v] = true;
    vertices[v].class_id = max_class;

    if (max_class == classes.size()) {
        vector<vertex> new_class {vertices[v]};
        classes.emplace_back(new_class);
    } else if (max_class < classes.size()) {
        classes[max_class].emplace_back(vertices[v]);
    }

    for (auto p : vertices[v].neighbors) {
        vertex current_vertex = vertices[p.first-1];
        if (current_vertex.depth == vertices[v].depth && !visited[current_vertex.id-1]) {
            dfs_util(current_vertex.id-1,visited);
        }
    }
}

void bfs(int current) {
    vector<bool> visited(graph_order, false);
    list<int> queue;

    visited[current-1] = true;
    queue.push_back(current);

    while (!queue.empty()) {
        current = queue.front();
        vertex current_vertex = vertices[current-1];
        queue.pop_front();

        int current_depth = current_vertex.depth;
        if (current_depth > max_depth) max_depth = current_depth;

        for (auto p : current_vertex.neighbors) {
            vertex v = vertices[p.first-1];
            if (!visited[v.id-1]) {
                visited[v.id-1] = true;
                v.depth = current_depth + 1;
                if (v.depth > max_depth) max_depth = v.depth;
                queue.push_back(v.id);
                vertices[p.first-1] = v;
            }
        }
    }
}

void print_graph(const vector<vertex>& graph) {
    for (auto & v : graph) {
        cout << v.id << "." << v.depth << " <" << v.class_id << "> --> ";
        for (auto p : v.neighbors) {
            cout << p.first << "-" << p.second << " ";
        }
        cout << "\n";
    }
}

int invalid_input() {
    printf("Error: Invalid input!\n");
    return EXIT_FAILURE;
}

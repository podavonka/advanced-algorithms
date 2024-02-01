#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

/* limits to check on input */
#define SERVERS_LOWER_LIMIT 4
#define SERVERS_UPPER_LIMIT 30
#define PLANNED_CONNECTIONS_UPPER_LIMIT 100
#define FAST_SERVERS_LOWER_LIMIT 0

using namespace std;

struct server {
    bool is_fast = false;

    int degree_fast = 0;
    int degree_not_fast = 0;

    bool operator<(const server& a) const {
        return tie(degree_fast, degree_not_fast) < tie(a.degree_fast, a.degree_not_fast);
    }

    bool operator==(const server& a) const {
        return degree_fast == a.degree_fast && degree_not_fast == a.degree_not_fast;
    }
};

struct topology_change {
    int network_A_server_1;
    int network_A_server_2;
    int network_B_server_1;
    int network_B_server_2;

    topology_change(int A1, int A2, int B1, int B2) :
        network_A_server_1(A1), network_A_server_2(A2),
        network_B_server_1(B1), network_B_server_2(B2) {}
};

/* input */
int servers_number, planned_connections_number, fast_servers_number;
vector<server> network_A, network_B;
vector<vector<int>> connections_A, connections_B;

/* auxiliary parameters */
map<tuple<bool, int, int>, vector<int>> server_groups_A;
map<tuple<bool, int, int>, vector<int>> server_groups_B;

/* output */
vector<topology_change> connection_fix;

int invalid_input();

vector<int> flatten_vector(const vector<vector<int>>& vec_vec);
vector<vector<int>> get_map_values(const map<tuple<bool, int, int>, vector<int>>& map);

void edit_network_A(int s1, int s2);
void reset_network_A(int s1, int s2);
void edit_network_B(int s1, int s2);
void reset_network_B(int s1, int s2);

void find_identical_topology();
bool is_identical_topology();
bool compare_degrees(vector<server> cmp_network_A, vector<server> cmp_network_B);
bool is_isomorphism(const vector<int>& groups_A, const vector<int>& groups_B);
bool find_mapping(const vector<int>& groups_A, vector<vector<int>>& groups_B, int group_id);

int main() {
    // speed up cin
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // read parameters
    cin >> servers_number >> planned_connections_number >> fast_servers_number;
    if (servers_number < SERVERS_LOWER_LIMIT || servers_number > SERVERS_UPPER_LIMIT ||
        planned_connections_number < servers_number - 1 || planned_connections_number > PLANNED_CONNECTIONS_UPPER_LIMIT ||
        fast_servers_number < FAST_SERVERS_LOWER_LIMIT || fast_servers_number > servers_number)
        return invalid_input();


    network_A.resize(servers_number);
    network_B.resize(servers_number);

    vector<int> connections_A_rows(servers_number, 0);
    vector<int> connections_B_rows(servers_number, 0);
    connections_A.resize(servers_number, connections_A_rows);
    connections_B.resize(servers_number, connections_B_rows);

    // read network #1
    for (int s = 0; s < fast_servers_number; s++) {
        int fast_server_id;
        cin >> fast_server_id;
        network_A[fast_server_id].is_fast = true;
    }

    for (int c = 0; c < planned_connections_number - 1; c++) {
        int s1, s2;
        cin >> s1 >> s2;

        if (network_A[s2].is_fast) network_A[s1].degree_fast++;
        else network_A[s1].degree_not_fast++;

        if (network_A[s1].is_fast) network_A[s2].degree_fast++;
        else network_A[s2].degree_not_fast++;

        connections_A[s1][s2] = 1;
        connections_A[s2][s1] = 1;
    }

    // read network #2
    for (int s = 0; s < fast_servers_number; s++) {
        int fast_server_id;
        cin >> fast_server_id;
        network_B[fast_server_id].is_fast = true;
    }

    for (int c = 0; c < planned_connections_number + 1; c++) {
        int s1, s2;
        cin >> s1 >> s2;

        if (network_B[s2].is_fast) network_B[s1].degree_fast++;
        else network_B[s1].degree_not_fast++;

        if (network_B[s1].is_fast) network_B[s2].degree_fast++;
        else network_B[s2].degree_not_fast++;

        connections_B[s1][s2] = 1;
        connections_B[s2][s1] = 1;
    }

    find_identical_topology();

    for (auto fix : connection_fix) {
        cout << fix.network_A_server_1 << " " << fix.network_A_server_2 << " " <<
                fix.network_B_server_1 << " " << fix.network_B_server_2 << "\n";
    }

    return EXIT_SUCCESS;
}

void find_identical_topology() {
    for (int row_A = 0; row_A < servers_number; row_A++) {
        for (int col_A = row_A + 1; col_A < servers_number; col_A++) {
            if (connections_A[row_A][col_A] == 0) {
                edit_network_A(row_A, col_A);

                for (int row_B = 0; row_B < servers_number; row_B++) {
                    for (int col_B = row_B + 1; col_B < servers_number; col_B++) {
                        if (connections_B[row_B][col_B] == 1) {
                            edit_network_B(row_B, col_B);

                            if (is_identical_topology()) {
                                topology_change change(row_A, col_A, row_B, col_B);
                                connection_fix.emplace_back(change);
                            }

                            reset_network_B(row_B, col_B);
                        }
                    }
                }

                reset_network_A(row_A, col_A);
            }
        }
    }
}

bool is_identical_topology() {
    // check degrees identity
    if (!(compare_degrees(network_A, network_B))) return false;

    // refactor groups
    vector<vector<int>> server_groups_values_A = get_map_values(server_groups_A);
    vector<vector<int>> server_groups_values_B = get_map_values(server_groups_B);

    return find_mapping(flatten_vector(server_groups_values_A),
                        server_groups_values_B,
                        0);
}

bool find_mapping(const vector<int>& groups_A, vector<vector<int>>& groups_B, int group_id) {
    if (group_id == groups_B.size())
        return is_isomorphism(groups_A, flatten_vector(groups_B));

    do {
        if (find_mapping(groups_A, groups_B, group_id + 1)) return true;
    } while (next_permutation(groups_B[group_id].begin(), groups_B[group_id].end()));

    return false;
}

bool is_isomorphism(const vector<int>& groups_A, const vector<int>& groups_B) {
    for (int i = 0; i < servers_number; i++) {
        for (int j = i + 1; j < servers_number; j++) {
            if (connections_A[groups_A[i]][groups_A[j]] != connections_B[groups_B[i]][groups_B[j]]) {
                return false;
            }
        }
    }
    return true;
}

bool compare_degrees(vector<server> cmp_network_A, vector<server> cmp_network_B) {
    std::sort(cmp_network_A.begin(), cmp_network_A.end());
    std::sort(cmp_network_B.begin(), cmp_network_B.end());

    if (!(cmp_network_A == cmp_network_B)) return false;
    return true;
}

void edit_network_A(int s1, int s2) {
    // remove a connection
    connections_A[s1][s2] = 1;
    connections_A[s2][s1] = 1;

    // edit services degrees
    if (network_A[s2].is_fast) network_A[s1].degree_fast++;
    else network_A[s1].degree_not_fast++;

    if (network_A[s1].is_fast) network_A[s2].degree_fast++;
    else network_A[s2].degree_not_fast++;

    // group servers by speed and degrees
    int s_id = 0;
    for (auto s : network_A) {
        server_groups_A[make_tuple(s.is_fast, s.degree_fast, s.degree_not_fast)].push_back(s_id);
        s_id++;
    }
}

void reset_network_A(int s1, int s2) {
    // add a connection
    connections_A[s1][s2] = 0;
    connections_A[s2][s1] = 0;

    // edit services degrees
    if (network_A[s2].is_fast) network_A[s1].degree_fast--;
    else network_A[s1].degree_not_fast--;

    if (network_A[s1].is_fast) network_A[s2].degree_fast--;
    else network_A[s2].degree_not_fast--;

    // clear grouping map
    server_groups_A.clear();
}

void edit_network_B(int s1, int s2) {
    // remove a connection
    connections_B[s1][s2] = 0;
    connections_B[s2][s1] = 0;

    // edit services degrees
    if (network_B[s2].is_fast) network_B[s1].degree_fast--;
    else network_B[s1].degree_not_fast--;

    if (network_B[s1].is_fast) network_B[s2].degree_fast--;
    else network_B[s2].degree_not_fast--;

    // group servers by speed and degrees
    int s_id = 0;
    for (auto s : network_B) {
        server_groups_B[make_tuple(s.is_fast, s.degree_fast, s.degree_not_fast)].push_back(s_id);
        s_id++;
    }
}

void reset_network_B(int s1, int s2) {
    // add a connection
    connections_B[s1][s2] = 1;
    connections_B[s2][s1] = 1;

    // edit services degrees
    if (network_B[s2].is_fast) network_B[s1].degree_fast++;
    else network_B[s1].degree_not_fast++;

    if (network_B[s1].is_fast) network_B[s2].degree_fast++;
    else network_B[s2].degree_not_fast++;

    // clear grouping map
    server_groups_B.clear();
}

vector<vector<int>> get_map_values(const map<tuple<bool, int, int>, vector<int>>& map) {
    vector<vector<int>> values;
    for (const auto& key_value : map) {
        values.push_back(key_value.second);
    }
    return values;
}

vector<int> flatten_vector(const vector<vector<int>>& vec_vec) {
    vector<int> vec;
    for (const auto& i : vec_vec) {
        for (auto j : i) {
            vec.push_back(j);
        }
    }
    return vec;
}

int invalid_input() {
    printf("Error: Invalid input!\n");
    return EXIT_FAILURE;
}

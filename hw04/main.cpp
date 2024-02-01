#include <iostream>
#include <unordered_map>
#include <string>
#include <stack>

#define LS_LOWER_LIMIT 2
#define LT_UPPER_LIMIT 10000
#define L1_LOWER_LIMIT 1

using namespace std;

struct node {
    int occurrences = 0;
    node* children[2] = {nullptr, nullptr};
};

/* auxiliary parameters */
node *root, *current_node;

/* output */
int rcd = 0, leaves = 1, paths = 0;

int invalid_input();

int elementary_similarity(const string& t, int length);
void restricted_composite_distance(const string& t, int l1, int l2);
void build_dictionary_tree(const string& s, int l1, int l2);

int main() {
    // speed up cin
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // input
    int LS, LT, L1, L2;
    string S, T;

    // read parameters
    cin >> LS >> LT >> L1 >> L2;
    if (LS < LS_LOWER_LIMIT || LT > LT_UPPER_LIMIT ||
        L1 < L1_LOWER_LIMIT || L2 < L1)
        return invalid_input();

    // read strings
    cin >> S >> T;

    // compute RCD(S, T, L1, L2)
    build_dictionary_tree(S, L1, L2);
    restricted_composite_distance(T, L1, L2);

    cout << rcd << " " << leaves << " " << paths << endl;

    return 0;
}

int elementary_similarity(const string& t, int length) {
    for (char ch : t) {
        if (current_node->children[ch - '0'] == nullptr) return 0;
        current_node = current_node->children[ch - '0'];
    }
    return current_node->occurrences * length;
}

void restricted_composite_distance(const string& t, int l1, int l2) {
    for (int i = 0; i <= (int) t.length(); ++i) {
        current_node = root;
        for (int length = l1; length <= l2; ++length) {
            if (i + length <= t.length()) {
                int similarity;
                if (length == l1) similarity = elementary_similarity(t.substr(i, length), length);
                else similarity = elementary_similarity(t.substr(i+length-1, 1), length);
                if (similarity == 0) break;
                rcd += similarity;
            }
        }
    }
}

void build_dictionary_tree(const string& s, int l1, int l2) {
    root = new node();
    for (int i = 0; i <= (int) s.length(); ++i) {
        for (int length = l2; length >= l1; --length) {
            if (i + length <= s.length()) {
                node* node = root;
                std::string substring = s.substr(i, length);
                for (int path = 0; path < substring.length(); path++) {
                    char ch = substring[path];
                    if (node->children[0] == nullptr && node->children[1] == nullptr) {
                        leaves--;
                        paths -= path;
                    }
                    if (node->children[ch - '0'] == nullptr) {
                        node->children[ch - '0'] = new struct node;
                        leaves++;
                        paths = paths + path + 1;
                    }
                    node = node->children[ch - '0'];
                    node->occurrences++;
                }

                break;
            }
        }
    }
}

int invalid_input() {
    printf("Error: Invalid input!\n");
    return EXIT_FAILURE;
}

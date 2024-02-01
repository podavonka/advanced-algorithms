#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <map>
#include <set>

#define N_LOWER_LIMIT 1
#define N_UPPER_LIMIT 130
#define F_LOWER_LIMIT 3
#define F_UPPER_LIMIT 20
#define D_UPPER_LIMIT 130
#define T_LOWER_LIMIT 1
#define T_UPPER_LIMIT 140000

using namespace std;

int N, F;
string T;

map<int, vector<string>> D;
set<int> substr_lengths;

int invalid_input();

int feasible_factorization();
int min_hamming_distance(const string& word, const vector<string>& dictionary);
int hamming_distance(const string& substr, const string& d);

int main() {
    // speed up cin
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // input parameters
    cin >> N >> F;
    if (N < N_LOWER_LIMIT || N > N_UPPER_LIMIT || F < F_LOWER_LIMIT || F > F_UPPER_LIMIT)
        return invalid_input();

    // initialize short words
    for (int i = 1; i < F; ++i) {
        substr_lengths.insert(i);
    }

    // input dictionary words
    for (int i = 0; i < N; ++i) {
        string D_word;
        cin >> D_word;

        if (D_word.length() < F || D_word.length() > D_UPPER_LIMIT)
            return invalid_input();

        D[(int) D_word.length()].push_back(D_word);
        substr_lengths.insert((int) D_word.length());
    }

    // input string T
    cin >> T;
    if (T.length() < T_LOWER_LIMIT || T.length() > T_UPPER_LIMIT)
        return invalid_input();

    // do feasible factorization
    int result = feasible_factorization();
    cout << result << endl;

    return 0;
}

int feasible_factorization() {
    vector<int> dp_dict_words(T.length() + 1, INT_MAX);
    vector<int> dp_short_words(T.length() + 1, INT_MAX);

    dp_dict_words[0] = 0;
    dp_short_words[0] = 0;

    for (int destination = 1; destination <= T.length(); ++destination) {
        for (int length : substr_lengths) {
            if (destination < length) break;

            if (length < F && dp_dict_words[destination - length] == INT_MAX) continue;
            if (dp_short_words[destination - length] == INT_MAX && dp_dict_words[destination - length] == INT_MAX) continue;

            if (length < F) {
                int hdd = dp_dict_words[destination - length];
                dp_short_words[destination] = min(dp_short_words[destination], hdd + 1);
            } else {
                int hdd = min(dp_short_words[destination - length], dp_dict_words[destination - length]);
                dp_dict_words[destination] = min(dp_dict_words[destination], hdd + min_hamming_distance(T.substr(destination - length, length), D[length]));
            }
        }
    }

    return min(dp_short_words[T.length()], dp_dict_words[T.length()]);
}

int min_hamming_distance(const string& word, const vector<string>& dictionary) {
    int min_distance = INT_MAX;
    for (const string& d : dictionary) {
        if (word.size() == d.size()) {
            min_distance = min(min_distance, hamming_distance(word, d));
        }
    }
    return min_distance;
}

int hamming_distance(const string& substr, const string& d) {
    int distance = 0;
    for (size_t i = 0; i < substr.size(); ++i) {
        if (substr[i] != d[i]) {
            ++distance;
        }
    }
    return distance;
}

int invalid_input() {
    printf("Error: Invalid input!\n");
    return EXIT_FAILURE;
}

#include <iostream>
#include <vector>
#include <valarray>
#include <cmath>

#define F_LOWER_LIMIT 1
#define F_UPPER_LIMIT 8
#define M_MAX_LOWER_LIMIT 2
#define M_MAX_UPPER_LIMIT 1000000000
#define X0_LOWER_LIMIT 0

using namespace std;

int F, M_max;
long long x0, x1;

vector<int> prime_numbers_product{1, 2, 6, 30, 210, 2310, 30030, 510510};
vector<long long> prime_numbers;

int result = 0;

int invalid_input();

void sieve_of_eratosthenes();

void count_feasible_LCGs();
long long find_prime_factors(int next_number_id, long long factors_product, vector<long long>& factors);
void find_LCG_parameters(int next_factor_id, long long M, long long factors_product, vector<long long>& factors);

int main() {
    // speed up cin
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // read input parameters
    cin >> F >> M_max >> x0 >> x1;
    if (F < F_LOWER_LIMIT || F > F_UPPER_LIMIT || M_max < M_MAX_LOWER_LIMIT ||
        M_max > M_MAX_UPPER_LIMIT || x0 < X0_LOWER_LIMIT || x1 >= M_max) return invalid_input();

    count_feasible_LCGs();

    cout << result << endl;

    return 0;
}

void find_LCG_parameters(int next_factor_id, long long M, long long factors_product, vector<long long>& factors) {
    if (M > M_max) return;

    if (M > max(x0, x1)) {
        long long A = M % 4 == 0 ? factors_product * 2 + 1 : factors_product + 1;
        if (A < M) {
            long long C = (x1 - A * x0) % M;
            if (C < 0) C += M;
            if (__gcd(M, C) == 1) result++;
        }
    }

    for (int factor_id = next_factor_id; factor_id < factors.size(); ++factor_id) {
        find_LCG_parameters(factor_id, M * factors[factor_id], factors_product, factors);
    }
}

long long find_prime_factors(int next_number_id, long long factors_product, vector<long long>& factors) {
    if (factors.size() == F) {
        find_LCG_parameters(0, factors_product, factors_product, factors);
        return factors_product;
    }

    long long previous_factors_product = factors_product;
    for (int number_id = next_number_id; number_id < prime_numbers.size() - F; ++number_id) {
        factors.push_back(prime_numbers[number_id]);
        if (prime_numbers[number_id] * prime_numbers[number_id + 1] * factors_product * (F - factors.size()) * 2 <= M_max &&
            previous_factors_product * 2 <= M_max) {
            previous_factors_product = find_prime_factors(number_id + 1,
                                                          factors_product * prime_numbers[number_id],
                                                          factors);
        }
        factors.pop_back();
    }

    return factors_product;
}

void count_feasible_LCGs() {
    sieve_of_eratosthenes();

    vector<long long> factors;
    find_prime_factors(0, 1, factors);
}

void sieve_of_eratosthenes() {
    vector<bool> prime(M_max + 1, true);

    for (int i = 2; i <= M_max / prime_numbers_product[F-1]; ++i) {
        if (prime[i]) {
            prime_numbers.push_back(i);
            for (int j = i; j <= M_max / prime_numbers_product[F-1]; j += i)
                prime[j] = false;
        }
    }
}

int invalid_input() {
    printf("Error: Invalid input!\n");
    return EXIT_FAILURE;
}

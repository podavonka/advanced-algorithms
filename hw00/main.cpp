#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>

#define INPUT_PARAMETERS 1
#define STICKS_LOWER_LIMIT 1

#define DIMENSION 2
#define X 0
#define Y 1
#define COORDINATES_LOWER_LIMIT 0

int sticks_number;
int **sticks;

int invalid_input(void);
int **init_2d_array(int **array, int rows, int columns);

int calculate_fence_length();

int main(int argc, char *argv[]) {
    if ((scanf("%d", &sticks_number)) != INPUT_PARAMETERS
        || sticks_number < STICKS_LOWER_LIMIT) return invalid_input();

    sticks = init_2d_array(sticks, sticks_number, DIMENSION);

    for (int s = 0; s < sticks_number; s++) {
        if ((scanf("%d %d", &sticks[s][X], &sticks[s][Y])) != DIMENSION
            || sticks[s][X] < COORDINATES_LOWER_LIMIT || sticks[s][Y] < COORDINATES_LOWER_LIMIT)
            return invalid_input();
    }

    int result = calculate_fence_length();
    std::cout << result << "\n";

    return EXIT_SUCCESS;
}

int calculate_fence_length() {
    double result = 0;

    for (int curr = 0, next = 1; next < sticks_number; curr++, next++) {
        result += sqrt(pow(sticks[next][X] - sticks[curr][X],2) + pow(sticks[next][Y] - sticks[curr][Y],2));
        if (next == sticks_number - 1)
            result += sqrt(pow(sticks[0][X] - sticks[next][X],2) + pow(sticks[0][Y] - sticks[next][Y],2));
    }

    return ceil(result  * 5);
}

int **init_2d_array(int **array, int rows, int columns) {
    array = (int **) malloc(rows * sizeof(int *));
    int columns_size = columns * sizeof(int);
    for (int r = 0; r < rows; r++) {
        array[r] = (int *) malloc(columns_size);
        memset(array[r], 0, columns_size);
    }
    return array;
}

int invalid_input(void) {
    printf("Error: Invalid input!\n");
    return EXIT_FAILURE;
}
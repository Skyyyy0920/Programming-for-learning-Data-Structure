#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define T_WIDTH 10
#define T_HEIGHT 20
#define T_DEAD_LINE 15

int G_BOARD[T_HEIGHT][T_WIDTH] = { 0 };
int G_SCORE = 0;

int ARR_BLOCKS[7][4][4][4] = {
    // I
    {{{1,1,1,1},{0}},{{1},{1},{1},{1}},{{1,1,1,1},{0}},{{1},{1},{1},{1}}},
    // T
    {{{0,1,0},{1,1,1}},{{1,0},{1,1},{1,0}},{{1,1,1},{0,1,0}},{{0,1},{1,1},{0,1}}},
    // O
    {{{1,1},{1,1}},{{1,1},{1,1}},{{1,1},{1,1}},{{1,1},{1,1}}},
    // J
    {{{1,0,0},{1,1,1}},{{1,1},{1,0},{1,0}},{{1,1,1},{0,0,1}},{{0,1},{0,1},{1,1}}},
    // L
    {{{0,0,1},{1,1,1}},{{1,0},{1,0},{1,1}},{{1,1,1},{1,0,0}},{{1,1},{0,1},{0,1}}},
    // S
    {{{0,1,1},{1,1,0}},{{1,0},{1,1},{0,1}},{{0,1,1},{1,1,0}},{{1,0},{1,1},{0,1}}},
    // Z
    {{{1,1,0},{0,1,1}},{{0,1},{1,1},{1,0}},{{1,1,0},{0,1,1}},{{0,1},{1,1},{1,0}}}
};

int getBlockIndex(char type) {
    switch (type) {
    case 'I': return 0;
    case 'T': return 1;
    case 'O': return 2;
    case 'J': return 3;
    case 'L': return 4;
    case 'S': return 5;
    case 'Z': return 6;
    default: return -1;
    }
}

int isPlace(int block[4][4], int x, int y) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (block[i][j])
                if (y + i >= T_HEIGHT || x + j < 0 || x + j >= T_WIDTH || G_BOARD[y + i][x + j])
                    return 0;
    return 1;
}

int dropY(int block[4][4], int x) {
    int y = 0;
    while (isPlace(block, x, y + 1)) y++;
    return y;
}

void placeBlock(int block[4][4], int x, int y) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (block[i][j])
                G_BOARD[y + i][x + j] = 1;
}

int clearLines() {
    int cleared = 0;
    for (int i = 0; i < T_HEIGHT; i++) {
        int full = 1;
        for (int j = 0; j < T_WIDTH; j++)
            if (!G_BOARD[i][j]) full = 0;
        if (full) {
            cleared++;
            for (int k = i; k > 0; k--)
                memcpy(G_BOARD[k], G_BOARD[k - 1], sizeof(G_BOARD[k]));
            memset(G_BOARD[0], 0, sizeof(G_BOARD[0]));
        }
    }
    if (cleared == 1) return 100;
    if (cleared == 2) return 300;
    if (cleared == 3) return 500;
    if (cleared >= 4) return 800;
    return 0;
}

int isDead() {
    for (int j = 0; j < T_WIDTH; j++)
        if (G_BOARD[T_DEAD_LINE][j]) return 1;
    return 0;
}

void clearStdout() {
    fflush(stdout);
}

void getColumnHeights(int heights[T_WIDTH]) {
    for (int j = 0; j < T_WIDTH; j++) {
        heights[j] = 0;
        for (int i = 0; i < T_HEIGHT; i++) {
            if (G_BOARD[i][j]) {
                heights[j] = T_HEIGHT - i;
                break;
            }
        }
    }
}

int countHoles() {
    int holes = 0;
    for (int j = 0; j < T_WIDTH; j++) {
        int found = 0;
        for (int i = 0; i < T_HEIGHT; i++) {
            if (G_BOARD[i][j]) found = 1;
            else if (found) holes++;
        }
    }
    return holes;
}

double evaluateBoard(int lines_cleared) {
    int heights[T_WIDTH];
    getColumnHeights(heights);

    int max_height = 0, sum_height = 0, bumpiness = 0;
    for (int i = 0; i < T_WIDTH; i++) {
        sum_height += heights[i];
        if (heights[i] > max_height)
            max_height = heights[i];
        if (i > 0)
            bumpiness += abs(heights[i] - heights[i - 1]);
    }

    int holes = countHoles();
    return -0.510 * max_height - 0.356 * holes + 0.760 * lines_cleared - 0.184 * bumpiness;
}

void start(char cur, char next) {
    int index = getBlockIndex(cur);
    double bests = -1e9;
    int bestr = 0, best_x = 0;

    for (int rot = 0; rot < 4; rot++) {
        int(*block)[4] = ARR_BLOCKS[index][rot];
        for (int x = -2; x <= T_WIDTH; x++) {
            int y = dropY(block, x);
            if (!isPlace(block, x, y)) continue;

            int temp[T_HEIGHT][T_WIDTH];
            memcpy(temp, G_BOARD, sizeof(G_BOARD));
            placeBlock(block, x, y);
            int lines = clearLines();
            double score_estimate = evaluateBoard(lines);
            memcpy(G_BOARD, temp, sizeof(G_BOARD));

            if (score_estimate > bests) {
                bests = score_estimate;
                bestr = rot;
                best_x = x;
            }
        }
    }

    int final_y = dropY(ARR_BLOCKS[index][bestr], best_x);
    placeBlock(ARR_BLOCKS[index][bestr], best_x, final_y);
    G_SCORE += clearLines();

    printf("%d %d\n", bestr, best_x);
    printf("%d\n", G_SCORE);
    clearStdout();
}

int main() {
    char cur, next;
    scanf(" %c %c", &cur, &next);
    while (1) {
        start(cur, next);

        char buf[10];
        if (scanf("%s", buf) != 1) break;
        if (buf[0] == 'E') break;
        if (buf[0] == 'X') {
            start(next, 'X');
            break;
        }
        cur = next;
        next = buf[0];
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 15
#define WALL '#'
#define EMPTY ' '

int main() {
    srand(time(NULL));
    
    char map[SIZE][SIZE];
    
    for(int row = 0; row < SIZE; row++) {
        for(int col = 0; col < SIZE; col++) {
            map[row][col] = EMPTY;
        }
    }
    
    for(int i = 0; i < SIZE; i++) {
        map[0][i] = WALL;
        map[SIZE-1][i] = WALL;
        map[i][0] = WALL;
        map[i][SIZE-1] = WALL;
    }
    
    int wallsPlaced = 0;
    while(wallsPlaced < 30) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        if(row > 0 && row < SIZE-1 && col > 0 && col < SIZE-1) {
            if(map[row][col] == EMPTY) {
                map[row][col] = WALL;
                wallsPlaced++;
            }
        }
    }
    
    printf("\n");
    for(int row = 0; row < SIZE; row++) {
        for(int col = 0; col < SIZE; col++) {
            printf("%c ", map[row][col]);
        }
        printf("\n");
    }
    
    return 0;
}

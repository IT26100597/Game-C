#include <stdio.h>

#define SIZE 15
#define WALL '#'
#define EMPTY ' '

int main() {
    char map[SIZE][SIZE];
    
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            map[i][j] = EMPTY;
        }
    }
    
    for(int i = 0; i < SIZE; i++) {
        map[0][i] = WALL;     
        map[SIZE-1][i] = WALL; 
        map[i][0] = WALL;      
        map[i][SIZE-1] = WALL; 
    }
    
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            printf("%c ", map[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}

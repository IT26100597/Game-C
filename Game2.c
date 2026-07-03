#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 15
#define WALL '#'
#define EMPTY ' '

typedef struct {
    int x, y;
    int health;
    int score;
    int keys;
    char symbol;
} Player;

void movePlayer(Player *p, char map[SIZE][SIZE], char direction) {
    int newX = p->x;
    int newY = p->y;
    
    if(direction == 'W' || direction == 'w') newY = p->y - 1;
    else if(direction == 'S' || direction == 's') newY = p->y + 1;
    else if(direction == 'A' || direction == 'a') newX = p->x - 1;
    else if(direction == 'D' || direction == 'd') newX = p->x + 1;
    else {
        printf("Invalid!\n");
        return;
    }
    
    if(newX < 0 || newX >= SIZE || newY < 0 || newY >= SIZE) {
        printf("Out of bounds!\n");
        return;
    }
    
    if(map[newY][newX] == WALL) {
        printf("Wall!\n");
        return;
    }
    
    map[p->y][p->x] = EMPTY;
    p->x = newX;
    p->y = newY;
    map[p->y][p->x] = p->symbol;
    printf("Moved to (%d,%d)\n", p->x, p->y);
}

int main() {
    srand(time(NULL));
    
    char map[SIZE][SIZE];
    char input[10];
    int playing = 1;
    
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
    
    Player player1;
    player1.symbol = '1';
    player1.health = 100;
    player1.score = 0;
    player1.keys = 0;
    
    int row, col;
    do {
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while(map[row][col] != EMPTY);
    
    player1.y = row;
    player1.x = col;
    map[row][col] = player1.symbol;
    
    while(playing == 1) {
        printf("\nHealth: %d  Score: %d  Keys: %d\n", 
               player1.health, player1.score, player1.keys);
        
        printf("\n");
        for(int row = 0; row < SIZE; row++) {
            for(int col = 0; col < SIZE; col++) {
                printf("%c ", map[row][col]);
            }
            printf("\n");
        }
        
        printf("\nMove (WASD, Q to quit): ");
        scanf("%s", input);
        
        if(input[0] == 'Q' || input[0] == 'q') {
            playing = 0;
            printf("Bye!\n");
        } else {
            movePlayer(&player1, map, input[0]);
        }
    }
    
    return 0;
}

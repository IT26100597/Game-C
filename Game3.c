#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 15
#define WALL '#'
#define EMPTY ' '
#define TREASURE 'T'
#define HEALTH_PACK 'H'
#define KEY 'K'
#define DOOR 'D'

int hiddenTraps[SIZE][SIZE];

typedef struct {
    int x, y;
    int health;
    int score;
    int keys;
    char symbol;
} Player;

void processTile(Player *p, char map[SIZE][SIZE]) {
    if(hiddenTraps[p->y][p->x] == 1) {
        p->health -= 20;
        printf("💀 TRAP! -20 HP (Health: %d)\n", p->health);
        hiddenTraps[p->y][p->x] = 0;
        if(p->health <= 0) {
            p->health = 0;
            printf("💀 Dead!\n");
            map[p->y][p->x] = EMPTY;
            return;
        }
    }
    
    char tile = map[p->y][p->x];
    
    switch(tile) {
        case TREASURE:
            p->score += 10;
            printf("💰 Treasure! +10 (Score: %d)\n", p->score);
            map[p->y][p->x] = EMPTY;
            break;
        case HEALTH_PACK:
            p->health += 20;
            if(p->health > 100) p->health = 100;
            printf("💊 Health! HP: %d\n", p->health);
            map[p->y][p->x] = EMPTY;
            break;
        case KEY:
            p->keys++;
            printf("🔑 Key! Keys: %d\n", p->keys);
            map[p->y][p->x] = EMPTY;
            break;
        case DOOR:
            if(p->keys > 0) {
                p->keys--;
                printf("🚪 Door open! Keys left: %d\n", p->keys);
                map[p->y][p->x] = EMPTY;
            } else {
                printf("🔒 Locked! Need key!\n");
            }
            break;
    }
}

void movePlayer(Player *p, char map[SIZE][SIZE], char direction) {
    int newX = p->x, newY = p->y;
    
    if(direction == 'W' || direction == 'w') newY = p->y - 1;
    else if(direction == 'S' || direction == 's') newY = p->y + 1;
    else if(direction == 'A' || direction == 'a') newX = p->x - 1;
    else if(direction == 'D' || direction == 'd') newX = p->x + 1;
    else return;
    
    if(newX < 0 || newX >= SIZE || newY < 0 || newY >= SIZE) return;
    if(map[newY][newX] == WALL) return;
    if(map[newY][newX] == DOOR && p->keys <= 0) {
        printf("Locked!\n");
        return;
    }
    
    map[p->y][p->x] = EMPTY;
    p->x = newX;
    p->y = newY;
    map[p->y][p->x] = p->symbol;
    processTile(p, map);
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
        int row = rand() % SIZE, col = rand() % SIZE;
        if(row > 0 && row < SIZE-1 && col > 0 && col < SIZE-1) {
            if(map[row][col] == EMPTY) {
                map[row][col] = WALL;
                wallsPlaced++;
            }
        }
    }
    
    int treasuresPlaced = 0;
    while(treasuresPlaced < 12) {
        int row = rand() % SIZE, col = rand() % SIZE;
        if(map[row][col] == EMPTY) {
            map[row][col] = TREASURE;
            treasuresPlaced++;
        }
    }
    
    int healthPlaced = 0;
    while(healthPlaced < 5) {
        int row = rand() % SIZE, col = rand() % SIZE;
        if(map[row][col] == EMPTY) {
            map[row][col] = HEALTH_PACK;
            healthPlaced++;
        }
    }
    
    int keysPlaced = 0;
    while(keysPlaced < 3) {
        int row = rand() % SIZE, col = rand() % SIZE;
        if(map[row][col] == EMPTY) {
            map[row][col] = KEY;
            keysPlaced++;
        }
    }
    
    int doorsPlaced = 0;
    while(doorsPlaced < 3) {
        int row = rand() % SIZE, col = rand() % SIZE;
        if(map[row][col] == EMPTY) {
            map[row][col] = DOOR;
            doorsPlaced++;
        }
    }
    
    for(int row = 0; row < SIZE; row++) {
        for(int col = 0; col < SIZE; col++) {
            hiddenTraps[row][col] = 0;
        }
    }
    
    int trapsPlaced = 0;
    while(trapsPlaced < 10) {
        int row = rand() % SIZE, col = rand() % SIZE;
        if(row > 0 && row < SIZE-1 && col > 0 && col < SIZE-1) {
            if(map[row][col] == EMPTY) {
                hiddenTraps[row][col] = 1;
                trapsPlaced++;
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
    
    printf("\n=== QUEST FOR THE LOST TREASURE ===\n");
    printf("Collect treasures, avoid traps!\n\n");
    
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
        } else {
            movePlayer(&player1, map, input[0]);
        }
    }
    
    printf("\nFinal Score: %d\n", player1.score);
    printf("Thanks for playing!\n");
    return 0;
}

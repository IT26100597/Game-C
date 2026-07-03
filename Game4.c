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
#define MAX_PLAYERS 2

int hiddenTraps[SIZE][SIZE];

typedef struct {
    char name[20];
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
            printf("💀 %s died!\n", p->name);
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
                printf("🚪 Door open! Keys: %d\n", p->keys);
                map[p->y][p->x] = EMPTY;
            } else {
                printf("🔒 Locked!\n");
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

void placePlayer(Player *p, char map[SIZE][SIZE]) {
    int row, col;
    do {
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while(map[row][col] != EMPTY);
    
    p->y = row;
    p->x = col;
    p->health = 100;
    p->score = 0;
    p->keys = 0;
    map[row][col] = p->symbol;
}

int main() {
    srand(time(NULL));
    
    char map[SIZE][SIZE];
    char input[20];
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
    
    Player players[MAX_PLAYERS];
    
    printf("=== QUEST FOR THE LOST TREASURE ===\n");
    printf("2 Players!\n\n");
    
    for(int i = 0; i < MAX_PLAYERS; i++) {
        printf("Player %d name: ", i+1);
        scanf("%s", players[i].name);
        players[i].symbol = '1' + i;
        placePlayer(&players[i], map);
    }
    
    int currentPlayer = 0;
    
    while(playing == 1) {
        if(players[currentPlayer].health <= 0) {
            currentPlayer = (currentPlayer + 1) % MAX_PLAYERS;
            continue;
        }
        
        Player *p = &players[currentPlayer];
        
        printf("\n========================================\n");
        printf("  %s's Turn (Player %c)\n", p->name, p->symbol);
        printf("========================================\n");
        printf("Health: %d  Score: %d  Keys: %d\n", 
               p->health, p->score, p->keys);
        
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
            break;
        }
        
        for(int i = 0; i < strlen(input) && i < 4; i++) {
            if(p->health > 0) {
                movePlayer(p, map, input[i]);
            }
        }
        
        int treasuresLeft = 0;
        for(int row = 0; row < SIZE; row++) {
            for(int col = 0; col < SIZE; col++) {
                if(map[row][col] == TREASURE) treasuresLeft++;
            }
        }
        
        if(treasuresLeft == 0) {
            printf("\n🎉 ALL TREASURES FOUND! 🎉\n");
            playing = 0;
            break;
        }
        
        int alive = 0;
        for(int i = 0; i < MAX_PLAYERS; i++) {
            if(players[i].health > 0) alive++;
        }
        if(alive == 0) {
            printf("\n💀 ALL PLAYERS DEAD! 💀\n");
            playing = 0;
            break;
        }
        
        currentPlayer = (currentPlayer + 1) % MAX_PLAYERS;
    }
    
    printf("\n=== GAME OVER ===\n");
    for(int i = 0; i < MAX_PLAYERS; i++) {
        if(players[i].health > 0) {
            players[i].score += players[i].health / 2;
        }
        printf("%s: %d points\n", players[i].name, players[i].score);
    }
    
    return 0;
}

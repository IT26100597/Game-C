
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
    else {
        printf("Invalid direction!\n");
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
    if(map[newY][newX] == DOOR && p->keys <= 0) {
        printf("Locked! Need key!\n");
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

void printMap(char map[SIZE][SIZE]) {
    printf("\n   ");
    for(int col = 0; col < SIZE; col++) {
        printf("%2d", col);
    }
    printf("\n");
    
    for(int row = 0; row < SIZE; row++) {
        printf("%2d ", row);
        for(int col = 0; col < SIZE; col++) {
            printf(" %c", map[row][col]);
        }
        printf("\n");
    }
}

void initializeMap(char map[SIZE][SIZE]) {
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
}

void placeItems(char map[SIZE][SIZE], char item, int count) {
    int placed = 0;
    while(placed < count) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        if(row > 0 && row < SIZE-1 && col > 0 && col < SIZE-1) {
            if(map[row][col] == EMPTY) {
                map[row][col] = item;
                placed++;
            }
        }
    }
}

void placeHiddenTraps(char map[SIZE][SIZE]) {
    for(int row = 0; row < SIZE; row++) {
        for(int col = 0; col < SIZE; col++) {
            hiddenTraps[row][col] = 0;
        }
    }
    
    int placed = 0;
    while(placed < 10) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        if(row > 0 && row < SIZE-1 && col > 0 && col < SIZE-1) {
            if(map[row][col] == EMPTY) {
                hiddenTraps[row][col] = 1;
                placed++;
            }
        }
    }
}

int countTreasures(char map[SIZE][SIZE]) {
    int count = 0;
    for(int row = 0; row < SIZE; row++) {
        for(int col = 0; col < SIZE; col++) {
            if(map[row][col] == TREASURE) {
                count++;
            }
        }
    }
    return count;
}

void saveGame(Player players[], int playerCount, char map[SIZE][SIZE]){

    FILE *fp = fopen("savegame.dat", "wb");
    
    if(fp == NULL) {
        printf("❌ Error creating save file!\n");
        return;
    }
    
    fwrite(&playerCount, sizeof(int), 1, fp);           
    fwrite(players, sizeof(Player), playerCount, fp);   
    fwrite(map, sizeof(char), SIZE * SIZE, fp);       
    fwrite(hiddenTraps, sizeof(int), SIZE * SIZE, fp);  
    
    fclose(fp);
    printf("✅ Game saved successfully to savegame.dat\n");
}

int loadGame(Player players[], int *playerCount, char map[SIZE][SIZE]) {
    FILE *fp = fopen("savegame.dat", "rb");
    
    if(fp == NULL) {
        printf("❌ No save file found. Starting new game.\n");
        return 0;  
    }
    
    fread(playerCount, sizeof(int), 1, fp);          
    fread(players, sizeof(Player), *playerCount, fp);   
    fread(map, sizeof(char), SIZE * SIZE, fp);         
    fread(hiddenTraps, sizeof(int), SIZE * SIZE, fp);   
    
    fclose(fp);
    printf("✅ Game loaded successfully from savegame.dat\n");
    return 1;  
}

void showResults(Player players[], int playerCount) {
    printf("\n============================================\n");
    printf("              GAME OVER\n");
    printf("============================================\n");
    
    for(int i = 0; i < playerCount; i++) {
        if(players[i].health > 0) {
            int bonus = players[i].health / 2;
            players[i].score += bonus;
            printf("%s: +%d HP bonus\n", players[i].name, bonus);
        }
    }
    
    for(int i = 0; i < playerCount - 1; i++) {
        for(int j = i + 1; j < playerCount; j++) {
            if(players[i].score < players[j].score) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }
    
    printf("\n--- FINAL SCORES ---\n");
    for(int i = 0; i < playerCount; i++) {
        printf("%d. %s: %d points\n", i+1, players[i].name, players[i].score);
    }
    
    if(playerCount > 1 && players[0].score == players[1].score) {
        printf("\n🏆 TIE between %s and %s!\n", players[0].name, players[1].name);
    } else {
        printf("\n🏆 WINNER: %s with %d points!\n", players[0].name, players[0].score);
    }
}

int main() {
    srand(time(NULL));
    
    char map[SIZE][SIZE];
    char input[20];
    int playing = 1;
    int playerCount = 2;
    int loaded = 0;
    int currentPlayer = 0;
    int treasuresLeft = 12;
    
    Player players[MAX_PLAYERS];
    
    printf("\n============================================\n");
    printf("  QUEST FOR THE LOST TREASURE\n");
    printf("  Day 5: Save & Load System\n");
    printf("============================================\n");
    printf("\n");
    printf("1. Start New Game\n");
    printf("2. Load Saved Game\n");
    printf("Enter choice (1 or 2): ");
    
    int choice;
    scanf("%d", &choice);
    
    if(choice == 2) {
        loaded = loadGame(players, &playerCount, map);
    }
    
    if(!loaded) {
        initializeMap(map);
        
        placeItems(map, WALL, 30);
        placeItems(map, TREASURE, 12);
        placeItems(map, HEALTH_PACK, 5);
        placeItems(map, KEY, 3);
        placeItems(map, DOOR, 3);
        placeHiddenTraps(map);
        
        printf("\nHow many players? (1-2): ");
        scanf("%d", &playerCount);
        
        if(playerCount < 1 || playerCount > 2) {
            printf("Invalid. Using 2 players.\n");
            playerCount = 2;
        }
        
        for(int i = 0; i < playerCount; i++) {
            printf("Enter name for Player %d: ", i+1);
            scanf("%s", players[i].name);
            players[i].symbol = '1' + i;  
            placePlayer(&players[i], map);
        }
        
        printf("\n✅ New game started!\n");
    }
    
    while(playing == 1) {
        if(players[currentPlayer].health <= 0) {
            currentPlayer = (currentPlayer + 1) % playerCount;
            continue;
        }
        
        Player *p = &players[currentPlayer];
        
        printf("\n============================================\n");
        printf("  %s's Turn (Player %c)\n", p->name, p->symbol);
        printf("============================================\n");
        printf("❤️  Health: %d/100\n", p->health);
        printf("⭐ Score: %d\n", p->score);
        printf("🔑 Keys: %d\n", p->keys);
        
        printMap(map);
        
        printf("\nMove (WASD, Q=quit, S=save): ");
        scanf("%s", input);
        
        if(input[0] == 'Q' || input[0] == 'q') {
            playing = 0;
            printf("\nGame ended.\n");
            break;
        }
        
        if(input[0] == 'S' || input[0] == 's') {
            saveGame(players, playerCount, map);
            continue;  
        }
        
        for(int i = 0; i < strlen(input) && i < 4; i++) {
            if(p->health > 0) {
                movePlayer(p, map, input[i]);
            }
        }
        
        treasuresLeft = countTreasures(map);
        if(treasuresLeft == 0) {
            printf("\n🎉 ALL TREASURES COLLECTED! 🎉\n");
            playing = 0;
            break;
        }
        
        int alive = 0;
        for(int i = 0; i < playerCount; i++) {
            if(players[i].health > 0) alive++;
        }
        if(alive == 0) {
            printf("\n💀 ALL PLAYERS DEAD! 💀\n");
            playing = 0;
            break;
        }
        
        currentPlayer = (currentPlayer + 1) % playerCount;
    }
    
    showResults(players, playerCount);
    printf("\nThanks for playing!\n");
    
    return 0;
}

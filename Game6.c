#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>


#define GRID_SIZE 15              
#define WALL_CHAR '#'             
#define EMPTY_CHAR ' '            
#define GOLD_CHAR 'T'            
#define MEDIC_CHAR 'H'       
#define LOCKPICK_CHAR 'K'   
#define GATE_CHAR 'D'       
#define MAX_HEROES 3             
#define LOG_CAPACITY 100          
#define MOVES_LIMIT 4             


int hiddenDangers[GRID_SIZE][GRID_SIZE];  
int roundCounter = 0;                     
char eventHistory[LOG_CAPACITY][200];    
int logWriteSpot = 0;                    
int totalEvents = 0;                      
int gameIsFinished = 0;              


typedef struct {
    char heroName[30];        
    int posX;              
    int posY;                
    int healthPoints;         
    int scorePoints;          
    int keyCount;             
    char displaySymbol;
    
    int stepsTaken;           
    int goldFound;            
    int trapsHit;             
    int damageReceived;   
    int medicsUsed;           
    int keysPickedUp;         
    int gatesOpened;          
} GameHero;


void addEventToLog(char *message);
void displayRecentEvents();
void saveLogToFile();

void createEmptyGrid(char grid[GRID_SIZE][GRID_SIZE]);
void buildBorderWalls(char grid[GRID_SIZE][GRID_SIZE]);
void placeRandomWalls(char grid[GRID_SIZE][GRID_SIZE]);
void placeGameItems(char grid[GRID_SIZE][GRID_SIZE]);
void hideTraps(char grid[GRID_SIZE][GRID_SIZE]);

void createNewHero(GameHero *hero, char symbol, char *name);
void placeHeroOnMap(GameHero *hero, char grid[GRID_SIZE][GRID_SIZE]);

void handleTileEffects(GameHero *hero, char grid[GRID_SIZE][GRID_SIZE]);
void moveHero(GameHero *hero, char grid[GRID_SIZE][GRID_SIZE], char direction);

void showGameControls();
void displayMap(char grid[GRID_SIZE][GRID_SIZE]);
void showHeroStatus(GameHero *hero);

void saveGameProgress(GameHero heroes[], int heroCount, char grid[GRID_SIZE][GRID_SIZE]);
int loadGameProgress(GameHero heroes[], int *heroCount, char grid[GRID_SIZE][GRID_SIZE]);

void showFinalScores(GameHero heroes[], int heroCount);
void showHeroStatistics(GameHero heroes[], int heroCount);

void addEventToLog(char *message) {
    strcpy(eventHistory[logWriteSpot], message);
    logWriteSpot = (logWriteSpot + 1) % LOG_CAPACITY;
    if(totalEvents < LOG_CAPACITY) {
        totalEvents++;
    }
}

void displayRecentEvents() {
    if(totalEvents == 0) {
        return;
    }
    
    printf("\n--- Recent Events ---\n");
    
    int startIndex = 0;
    int count = 5;
    
    if(totalEvents < 5) {
        startIndex = 0;
        count = totalEvents;
    } else {
        startIndex = (logWriteSpot - 5 + LOG_CAPACITY) % LOG_CAPACITY;
    }
    
    for(int i = 0; i < count; i++) {
        int index = (startIndex + i) % LOG_CAPACITY;
        printf("%s\n", eventHistory[index]);
    }
}

void saveLogToFile() {
    FILE *logFile = fopen("gamelog.txt", "w");
    if(logFile == NULL) {
        printf("Warning: Could not save log file!\n");
        return;
    }
    
    fprintf(logFile, "============================================\n");
    fprintf(logFile, "  QUEST FOR THE LOST TREASURE - GAME LOG\n");
    fprintf(logFile, "============================================\n\n");
    
    int startIndex = 0;
    if(totalEvents >= LOG_CAPACITY) {
        startIndex = logWriteSpot;
    }
    
    for(int i = 0; i < totalEvents; i++) {
        int index = (startIndex + i) % LOG_CAPACITY;
        fprintf(logFile, "%s\n", eventHistory[index]);
    }
    
    fclose(logFile);
    printf("✅ Game log saved to gamelog.txt\n");
}


void createEmptyGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for(int row = 0; row < GRID_SIZE; row++) {
        for(int col = 0; col < GRID_SIZE; col++) {
            grid[row][col] = EMPTY_CHAR;
        }
    }
}

void buildBorderWalls(char grid[GRID_SIZE][GRID_SIZE]) {
    for(int i = 0; i < GRID_SIZE; i++) {
        grid[0][i] = WALL_CHAR;              
        grid[GRID_SIZE-1][i] = WALL_CHAR;    
        grid[i][0] = WALL_CHAR;              
        grid[i][GRID_SIZE-1] = WALL_CHAR;    
    }
}

void placeRandomWalls(char grid[GRID_SIZE][GRID_SIZE]) {
    int wallsPlaced = 0;
    
    while(wallsPlaced < 30) {
        int randomRow = rand() % GRID_SIZE;
        int randomCol = rand() % GRID_SIZE;
        
        if(randomRow == 0 || randomRow == GRID_SIZE-1 || 
           randomCol == 0 || randomCol == GRID_SIZE-1) {
            continue;
        }
        
        if(grid[randomRow][randomCol] == EMPTY_CHAR) {
            grid[randomRow][randomCol] = WALL_CHAR;
            wallsPlaced++;
        }
    }
}

void placeGameItems(char grid[GRID_SIZE][GRID_SIZE]) {
    int placed = 0;
    while(placed < 12) {
        int r = rand() % GRID_SIZE;
        int c = rand() % GRID_SIZE;
        if(grid[r][c] == EMPTY_CHAR) {
            grid[r][c] = GOLD_CHAR;
            placed++;
        }
    }
    
    placed = 0;
    while(placed < 5) {
        int r = rand() % GRID_SIZE;
        int c = rand() % GRID_SIZE;
        if(grid[r][c] == EMPTY_CHAR) {
            grid[r][c] = MEDIC_CHAR;
            placed++;
        }
    }
    
    placed = 0;
    while(placed < 3) {
        int r = rand() % GRID_SIZE;
        int c = rand() % GRID_SIZE;
        if(grid[r][c] == EMPTY_CHAR) {
            grid[r][c] = LOCKPICK_CHAR;
            placed++;
        }
    }
    
    placed = 0;
    while(placed < 3) {
        int r = rand() % GRID_SIZE;
        int c = rand() % GRID_SIZE;
        if(grid[r][c] == EMPTY_CHAR) {
            grid[r][c] = GATE_CHAR;
            placed++;
        }
    }
}

void hideTraps(char grid[GRID_SIZE][GRID_SIZE]) {
    for(int row = 0; row < GRID_SIZE; row++) {
        for(int col = 0; col < GRID_SIZE; col++) {
            hiddenDangers[row][col] = 0;
        }
    }
    
    int trapsPlaced = 0;
    while(trapsPlaced < 10) {
        int r = rand() % GRID_SIZE;
        int c = rand() % GRID_SIZE;
        
        if(r == 0 || r == GRID_SIZE-1 || c == 0 || c == GRID_SIZE-1) {
            continue;
        }
        
        if(grid[r][c] == EMPTY_CHAR && hiddenDangers[r][c] == 0) {
            hiddenDangers[r][c] = 1;
            trapsPlaced++;
        }
    }
}


void createNewHero(GameHero *hero, char symbol, char *name) {
    strcpy(hero->heroName, name);
    hero->displaySymbol = symbol;
    hero->healthPoints = 100;
    hero->scorePoints = 0;
    hero->keyCount = 0;
    
    hero->stepsTaken = 0;
    hero->goldFound = 0;
    hero->trapsHit = 0;
    hero->damageReceived = 0;
    hero->medicsUsed = 0;
    hero->keysPickedUp = 0;
    hero->gatesOpened = 0;
}

void placeHeroOnMap(GameHero *hero, char grid[GRID_SIZE][GRID_SIZE]) {
    int r, c;
    do {
        r = rand() % GRID_SIZE;
        c = rand() % GRID_SIZE;
    } while(grid[r][c] != EMPTY_CHAR);
    
    hero->posY = r;
    hero->posX = c;
    grid[r][c] = hero->displaySymbol;
}


void handleTileEffects(GameHero *hero, char grid[GRID_SIZE][GRID_SIZE]) {
    char message[200];
    
    if(hiddenDangers[hero->posY][hero->posX] == 1) {
        hero->healthPoints -= 20;
        hero->trapsHit++;
        hero->damageReceived += 20;
        hiddenDangers[hero->posY][hero->posX] = 0; 
        
        sprintf(message, "[R%d] %s triggered a trap! (-20 HP)", 
                roundCounter, hero->heroName);
        addEventToLog(message);
        
        printf("💀 BOOM! Hidden trap! You lost 20 HP (Now: %d)\n", 
               hero->healthPoints);
        
        if(hero->healthPoints <= 0) {
            hero->healthPoints = 0;
            sprintf(message, "[R%d] %s has died!", roundCounter, hero->heroName);
            addEventToLog(message);
            printf("💀 %s has died!\n", hero->heroName);
            grid[hero->posY][hero->posX] = EMPTY_CHAR;
            return;
        }
    }
    
    char tile = grid[hero->posY][hero->posX];
    
    if(tile == GOLD_CHAR) {
        hero->scorePoints += 10;
        hero->goldFound++;
        sprintf(message, "[R%d] %s found treasure! (+10 points)", 
                roundCounter, hero->heroName);
        addEventToLog(message);
        printf("💰 Found treasure! +10 points (Total: %d)\n", hero->scorePoints);
        grid[hero->posY][hero->posX] = EMPTY_CHAR;
    }
    
    else if(tile == MEDIC_CHAR) {
        hero->healthPoints += 20;
        if(hero->healthPoints > 100) hero->healthPoints = 100;
        hero->medicsUsed++;
        sprintf(message, "[R%d] %s used health pack (HP: %d)", 
                roundCounter, hero->heroName, hero->healthPoints);
        addEventToLog(message);
        printf("💊 Found health pack! HP restored to %d\n", hero->healthPoints);
        grid[hero->posY][hero->posX] = EMPTY_CHAR;
    }
    
    else if(tile == LOCKPICK_CHAR) {
        hero->keyCount++;
        hero->keysPickedUp++;
        sprintf(message, "[R%d] %s found key (Now: %d)", 
                roundCounter, hero->heroName, hero->keyCount);
        addEventToLog(message);
        printf("🔑 Found key! Now have %d\n", hero->keyCount);
        grid[hero->posY][hero->posX] = EMPTY_CHAR;
    }
    
    else if(tile == GATE_CHAR) {
        if(hero->keyCount > 0) {
            hero->keyCount--;
            hero->gatesOpened++;
            sprintf(message, "[R%d] %s unlocked a door", 
                    roundCounter, hero->heroName);
            addEventToLog(message);
            printf("🚪 Door unlocked! Used 1 key\n");
            grid[hero->posY][hero->posX] = EMPTY_CHAR;
        } else {
            printf("🔒 Door is locked! You need a key!\n");
        }
    }
}


void moveHero(GameHero *hero, char grid[GRID_SIZE][GRID_SIZE], char direction) {
    int newX = hero->posX;
    int newY = hero->posY;
    
    if(direction == 'W' || direction == 'w') {
        newY--;  
    } else if(direction == 'S' || direction == 's') {
        newY++;
    } else if(direction == 'A' || direction == 'a') {
        newX--;  
    } else if(direction == 'D' || direction == 'd') {
        newX++;  
    } else {
        printf("Invalid direction! Use W, A, S, D\n");
        return;
    }
    
    if(newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) {
        printf("Can't go there - that's outside the map!\n");
        return;
    }
    
    if(grid[newY][newX] == WALL_CHAR) {
        printf("Can't go there - that's a wall!\n");
        return;
    }
    
    if(grid[newY][newX] == GATE_CHAR && hero->keyCount <= 0) {
        printf("The door is locked! You need a key!\n");
        return;
    }
    
    grid[hero->posY][hero->posX] = EMPTY_CHAR;
    hero->posX = newX;
    hero->posY = newY;
    grid[hero->posY][hero->posX] = hero->displaySymbol;
    hero->stepsTaken++;
    
    handleTileEffects(hero, grid);
}


void showGameControls() {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║            GAME CONTROLS              ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  W  = Move UP            ║  Q  = Quit ║\n");
    printf("║  S  = Move DOWN          ║  S  = Save ║\n");
    printf("║  A  = Move LEFT          ║            ║\n");
    printf("║  D  = Move RIGHT         ║            ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("\n");
}

void displayMap(char grid[GRID_SIZE][GRID_SIZE]) {
    printf("\n");
    
    printf("   ");
    for(int col = 0; col < GRID_SIZE; col++) {
        printf("%2d", col);
    }
    printf("\n");
    
    for(int row = 0; row < GRID_SIZE; row++) {
        printf("%2d ", row);
        for(int col = 0; col < GRID_SIZE; col++) {
            printf(" %c", grid[row][col]);
        }
        printf("\n");
    }
}

void showHeroStatus(GameHero *hero) {
    printf("❤️  Health: %d/100  ", hero->healthPoints);
    printf("⭐ Score: %d  ", hero->scorePoints);
    printf("🔑 Keys: %d\n", hero->keyCount);
}


void saveGameProgress(GameHero heroes[], int heroCount, char grid[GRID_SIZE][GRID_SIZE]) {
    FILE *saveFile = fopen("savegame.dat", "wb");
    if(saveFile == NULL) {
        printf("Error: Could not save game!\n");
        return;
    }
    
    fwrite(&heroCount, sizeof(int), 1, saveFile);
    fwrite(heroes, sizeof(GameHero), heroCount, saveFile);
    fwrite(grid, sizeof(char), GRID_SIZE * GRID_SIZE, saveFile);
    fwrite(hiddenDangers, sizeof(int), GRID_SIZE * GRID_SIZE, saveFile);
    fwrite(&roundCounter, sizeof(int), 1, saveFile);
    
    fclose(saveFile);
    printf("✅ Game saved successfully!\n");
    addEventToLog("Game was saved");
}

int loadGameProgress(GameHero heroes[], int *heroCount, char grid[GRID_SIZE][GRID_SIZE]) {
    FILE *saveFile = fopen("savegame.dat", "rb");
    if(saveFile == NULL) {
        return 0;  
    }
    
    fread(heroCount, sizeof(int), 1, saveFile);
    
    if(*heroCount < 1 || *heroCount > MAX_HEROES) {
        fclose(saveFile);
        return 0;  
    }
    
    fread(heroes, sizeof(GameHero), *heroCount, saveFile);
    fread(grid, sizeof(char), GRID_SIZE * GRID_SIZE, saveFile);
    fread(hiddenDangers, sizeof(int), GRID_SIZE * GRID_SIZE, saveFile);
    fread(&roundCounter, sizeof(int), 1, saveFile);
    
    fclose(saveFile);
    return 1;  
}


void showFinalScores(GameHero heroes[], int heroCount) {
    char message[200];
    
    printf("\n============================================\n");
    printf("           FINAL SCORES\n");
    printf("============================================\n");
    
    printf("\n--- HP Bonus ---\n");
    for(int i = 0; i < heroCount; i++) {
        if(heroes[i].healthPoints > 0) {
            int bonus = heroes[i].healthPoints / 2;
            heroes[i].scorePoints += bonus;
            printf("%s gets +%d bonus points for surviving!\n", 
                   heroes[i].heroName, bonus);
            sprintf(message, "%s got %d HP bonus points", 
                    heroes[i].heroName, bonus);
            addEventToLog(message);
        }
    }
    
    for(int i = 0; i < heroCount - 1; i++) {
        for(int j = i + 1; j < heroCount; j++) {
            if(heroes[i].scorePoints < heroes[j].scorePoints) {
                GameHero temp = heroes[i];
                heroes[i] = heroes[j];
                heroes[j] = temp;
            }
        }
    }
    
    printf("\n--- Leaderboard ---\n");
    for(int i = 0; i < heroCount; i++) {
        char status[20];
        if(heroes[i].healthPoints > 0) {
            strcpy(status, "ALIVE");
        } else {
            strcpy(status, "DEAD");
        }
        printf("%d. %s (P%c): %d points [%s]\n", 
               i+1, heroes[i].heroName, heroes[i].displaySymbol, 
               heroes[i].scorePoints, status);
    }
    
    printf("\n--- Result ---\n");
    if(heroCount > 1 && heroes[0].scorePoints == heroes[1].scorePoints) {
        printf("🏆 TIE between %s and %s!\n", 
               heroes[0].heroName, heroes[1].heroName);
        addEventToLog("Game ended in a tie!");
    } else {
        printf("🏆 WINNER: %s with %d points!\n", 
               heroes[0].heroName, heroes[0].scorePoints);
        sprintf(message, "Winner: %s with %d points", 
                heroes[0].heroName, heroes[0].scorePoints);
        addEventToLog(message);
    }
}

void showHeroStatistics(GameHero heroes[], int heroCount) {
    printf("\n============================================\n");
    printf("         HERO STATISTICS\n");
    printf("============================================\n");
    
    for(int i = 0; i < heroCount; i++) {
        printf("\n--- %s (P%c) ---\n", 
               heroes[i].heroName, heroes[i].displaySymbol);
        printf("  Moves Made:      %d\n", heroes[i].stepsTaken);
        printf("  Treasures Found: %d\n", heroes[i].goldFound);
        printf("  Traps Triggered: %d\n", heroes[i].trapsHit);
        printf("  Damage Taken:    %d\n", heroes[i].damageReceived);
        printf("  Health Packs:    %d\n", heroes[i].medicsUsed);
        printf("  Keys Collected:  %d\n", heroes[i].keysPickedUp);
        printf("  Doors Unlocked:  %d\n", heroes[i].gatesOpened);
        printf("  Final Score:     %d\n", heroes[i].scorePoints);
    }
}


int main() {
    srand(time(NULL));  
    
    char grid[GRID_SIZE][GRID_SIZE];
    GameHero heroes[MAX_HEROES];
    int heroCount = 0;
    int gameLoaded = 0;
    
    char message[200];
    
    
    printf("\n");
    printf("============================================\n");
    printf("    QUEST FOR THE LOST TREASURE\n");
    printf("    A Game of Adventure and Danger!\n");
    printf("============================================\n");
    printf("\n");
    
    
    printf("What would you like to do?\n");
    printf("  1. Start a new adventure\n");
    printf("  2. Continue a saved game\n");
    printf("Choice: ");
    int menuChoice;
    scanf("%d", &menuChoice);
    
    if(menuChoice == 2) {
        gameLoaded = loadGameProgress(heroes, &heroCount, grid);
        if(gameLoaded) {
            printf("✅ Game loaded successfully!\n");
            sprintf(message, "Game loaded - Round %d", roundCounter);
            addEventToLog(message);
        } else {
            printf("No save file found. Starting new game.\n");
        }
    }
    
    
    if(!gameLoaded) {
        createEmptyGrid(grid);
        buildBorderWalls(grid);
        placeRandomWalls(grid);
        placeGameItems(grid);
        hideTraps(grid);
        
        printf("\nHow many adventurers? (1-3): ");
        scanf("%d", &heroCount);
        
        if(heroCount < 1 || heroCount > MAX_HEROES) {
            printf("Invalid number. Using 2 players.\n");
            heroCount = 2;
        }
        
        for(int i = 0; i < heroCount; i++) {
            char name[30];
            printf("Name for Adventurer %d: ", i+1);
            scanf("%s", name);
            char symbol = '1' + i;  
            createNewHero(&heroes[i], symbol, name);
            placeHeroOnMap(&heroes[i], grid);
        }
        
        roundCounter = 0;
        sprintf(message, "New game started with %d players", heroCount);
        addEventToLog(message);
    }
    
    
    int gameRunning = 1;
    int currentHero = 0;
    char userInput[20];
    
    while(gameRunning == 1) {
        if(heroes[currentHero].healthPoints <= 0) {
            currentHero = (currentHero + 1) % heroCount;
            continue;
        }
        
        GameHero *current = &heroes[currentHero];
        
        
        printf("\n============================================\n");
        printf("           ROUND %d (Part B)\n", roundCounter);
        printf("============================================\n");
        printf("%s (Player %c)'s Turn\n", current->heroName, current->displaySymbol);
        showHeroStatus(current);
        
        displayMap(grid);
        displayRecentEvents(); 
        showGameControls();
        
        
        printf("Enter your moves (WASD, Q=quit, S=save): ");
        scanf("%s", userInput);
        
        if(userInput[0] == 'Q' || userInput[0] == 'q') {
            printf("\nAre you sure you want to quit? (y/n): ");
            char confirm;
            scanf(" %c", &confirm);
            if(confirm == 'y' || confirm == 'Y') {
                printf("Game ended by player.\n");
                addEventToLog("Game ended by player");
                gameRunning = 0;
                break;
            }
            continue;
        }
        
        if(userInput[0] == 'S' || userInput[0] == 's') {
            saveGameProgress(heroes, heroCount, grid);
            continue;
        }
        
        if(strlen(userInput) > MOVES_LIMIT) {
            printf("Too many moves! Max %d. Turn cancelled.\n", MOVES_LIMIT);
            sprintf(message, "[R%d] %s entered too many moves (turn cancelled)", 
                    roundCounter, current->heroName);
            addEventToLog(message);
        } else {
            for(int i = 0; i < strlen(userInput); i++) {
                if(current->healthPoints <= 0) break;
                moveHero(current, grid, userInput[i]);
            }
        }
        
        
        int treasuresLeft = 0;
        for(int row = 0; row < GRID_SIZE; row++) {
            for(int col = 0; col < GRID_SIZE; col++) {
                if(grid[row][col] == GOLD_CHAR) {
                    treasuresLeft++;
                }
            }
        }
        
        if(treasuresLeft == 0) {
            printf("\n🎉 ALL TREASURES COLLECTED! ADVENTURE COMPLETE! 🎉\n");
            addEventToLog("All treasures collected! Game Over!");
            gameRunning = 0;
            break;
        }
        
        int aliveCount = 0;
        for(int i = 0; i < heroCount; i++) {
            if(heroes[i].healthPoints > 0) aliveCount++;
        }
        
        if(aliveCount == 0) {
            printf("\n💀 ALL HEROES HAVE FALLEN! 💀\n");
            addEventToLog("All heroes dead! Game Over!");
            gameRunning = 0;
            break;
        }
        
        
        currentHero = (currentHero + 1) % heroCount;
        if(currentHero == 0) {
            roundCounter++;  
            
            printf("\nSave game? (y/n): ");
            char saveChoice[10];
            scanf("%s", saveChoice);
            if(saveChoice[0] == 'Y' || saveChoice[0] == 'y') {
                saveGameProgress(heroes, heroCount, grid);
            }
        }
    }
    
    printf("\n");
    printf("============================================\n");
    printf("           ADVENTURE OVER\n");
    printf("============================================\n");
    
    showFinalScores(heroes, heroCount);
    
    showHeroStatistics(heroes, heroCount);
    
    saveLogToFile();
    
    printf("\nThanks for playing!\n");
    return 0;
}

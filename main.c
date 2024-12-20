#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define MAP_SIZE 15
#define MAX_LENGTH 100

// Structures
typedef struct {
    int x, y;
} Position;

typedef struct {
    Position body[MAX_LENGTH];
    int length;
    int direction; // 0: Up, 1: Right, 2: Down, 3: Left
} Snake;

// Global Variables
int map[MAP_SIZE][MAP_SIZE];
Snake snake;
Position fruit;
Position powerUp1;
Position powerUp2;
int hasPowerUp1 = 0;
int hasPowerUp2 = 0;

// Function Prototypes
void initializeGame();
void initializeMap();
void initializeSnake();
void placeFruit();
void placePowerUps();
void displayMap();
void moveSnake();
int checkCollision();
void activatePowerUp1();
void activatePowerUp2();
void gameOver();
void sleepMs(int ms);

int main() {
    char input;
    int isRunning = 1;

    initializeGame();

    while (isRunning) {
        if (_kbhit()) {
            input = _getch();
            switch (input) {
                case 72: snake.direction = 0; break; // Up
                case 77: snake.direction = 1; break; // Right
                case 80: snake.direction = 2; break; // Down
                case 75: snake.direction = 3; break; // Left
                case '1': if (hasPowerUp1) activatePowerUp1(); break;
                case '2': if (hasPowerUp2) activatePowerUp2(); break;
            }
        }

        moveSnake();

        if (checkCollision()) {
            gameOver();
            isRunning = 0;
        }

        displayMap();
        sleepMs(200); // Adjust speed
    }

    return 0;
}

// Function Definitions
void initializeGame() {
    initializeMap();
    initializeSnake();
    placeFruit();
    placePowerUps();
}

void initializeMap() {
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            map[i][j] = 0;
        }
    }
}

void initializeSnake() {
    snake.length = 3;
    snake.direction = 1; // Moving right
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = MAP_SIZE / 2;
        snake.body[i].y = MAP_SIZE / 2 - i;
        map[MAP_SIZE / 2][MAP_SIZE / 2 - i] = 1;
    }
}

void placeFruit() {
    do {
        fruit.x = rand() % MAP_SIZE;
        fruit.y = rand() % MAP_SIZE;
    } while (map[fruit.x][fruit.y] != 0);

    map[fruit.x][fruit.y] = 2; // Represent fruit with 2
}

void placePowerUps() {
    do {
        powerUp1.x = rand() % MAP_SIZE;
        powerUp1.y = rand() % MAP_SIZE;
    } while (map[powerUp1.x][powerUp1.y] != 0);

    do {
        powerUp2.x = rand() % MAP_SIZE;
        powerUp2.y = rand() % MAP_SIZE;
    } while (map[powerUp2.x][powerUp2.y] != 0);

    map[powerUp1.x][powerUp1.y] = 3; // Represent powerUp1 with 3
    map[powerUp2.x][powerUp2.y] = 4; // Represent powerUp2 with 4
}

void displayMap() {
    system("cls");
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (map[i][j] == 1) printf("o"); // Snake body
            else if (map[i][j] == 2) printf("F"); // Fruit
            else if (map[i][j] == 3) printf("1"); // PowerUp1
            else if (map[i][j] == 4) printf("2"); // PowerUp2
            else printf("."); // Empty space
        }
        printf("\n");
    }
}

void moveSnake() {
    Position nextHead = snake.body[0];

    switch (snake.direction) {
        case 0: nextHead.x--; break; // Up
        case 1: nextHead.y++; break; // Right
        case 2: nextHead.x++; break; // Down
        case 3: nextHead.y--; break; // Left
    }

    // Handle map boundaries (cyclic movement)
    if (nextHead.x < 0) nextHead.x = MAP_SIZE - 1;
    if (nextHead.x >= MAP_SIZE) nextHead.x = 0;
    if (nextHead.y < 0) nextHead.y = MAP_SIZE - 1;
    if (nextHead.y >= MAP_SIZE) nextHead.y = 0;

    // Check for fruit
    if (nextHead.x == fruit.x && nextHead.y == fruit.y) {
        snake.length++;
        placeFruit();
    } else {
        Position tail = snake.body[snake.length - 1];
        map[tail.x][tail.y] = 0; // Clear tail
    }

    // Move snake
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0] = nextHead;

    map[nextHead.x][nextHead.y] = 1;
}

int checkCollision() {
    Position head = snake.body[0];
    for (int i = 1; i < snake.length; i++) {
        if (head.x == snake.body[i].x && head.y == snake.body[i].y) {
            return 1; // Collision with body
        }
    }
    return 0;
}

void activatePowerUp1() {
    hasPowerUp1 = 0;
    for (int i = snake.length - 1; i > 1; i--) {
        snake.body[i] = snake.body[i - 2];
    }
}

void activatePowerUp2() {
    hasPowerUp2 = 0;
    // Handle shield logic here
}

void gameOver() {
    printf("Game Over! Press 'y' to restart or any other key to exit.\n");
    char choice = _getch();
    if (choice == 'y' || choice == 'Y') {
        initializeGame();
    } else {
        exit(0);
    }
}

void sleepMs(int ms) {
    Sleep(ms);
}

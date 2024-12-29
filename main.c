#include <stdio.h>
#include <stdlib.h>
<<<<<<< HEAD
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
=======
#include <stdbool.h>
#include <conio.h> // Pour _kbhit() et _getch() sur Windows

#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h> // Pour usleep() sur Linux/MacOS
#define SLEEP(ms) usleep((ms) * 1000)
#endif

#define WIDTH 15
#define HEIGHT 15
#define INITIAL_LENGTH 3

// Directions
#define UP 'z'
#define LEFT 'q'
#define DOWN 's'
#define RIGHT 'd'

// Structure pour représenter une position
typedef struct {
    int x;
    int y;
} Position;

// Structure pour représenter le serpent
typedef struct Snake {
    Position *body; // Allocation dynamique
    int length;
    int capacity; // Capacité dynamique
    char direction;
    void (*initialize)(struct Snake* snake, char map[HEIGHT][WIDTH]);
    bool (*move)(struct Snake* snake, struct Game* game);
} Snake;

// Structure pour représenter le jeu
typedef struct Game {
    char map[HEIGHT][WIDTH];
    struct Snake snake;
    Position food;
    void (*initializeMap)(struct Game* game);
    void (*generateFood)(struct Game* game);
    void (*displayMap)(struct Game* game);
    int score; // Ajout du score
} Game;

// Fonction pour initialiser la carte
void initializeMap(struct Game* game) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                game->map[i][j] = '#';
            } else {
                game->map[i][j] = ' ';
            }
        }
    }
}

// Fonction pour afficher le score
void displayScore(struct Game* game) {
    printf("Score: %d\n", game->score);
}

// Fonction pour sauvegarder le score dans un fichier
void saveScoreToFile(int score) {
    FILE *file = fopen("score.txt", "a");
    if (file != NULL) {
        fprintf(file, "Score: %d\n", score);
        fclose(file);
    } else {
        printf("Impossible d'ouvrir le fichier pour enregistrer le score.\n");
    }
}

// Fonction pour réinitialiser la carte à chaque étape
void resetMap(struct Game* game) {
    for (int i = 1; i < HEIGHT - 1; i++) {
        for (int j = 1; j < WIDTH - 1; j++) {
            game->map[i][j] = ' ';
        }
    }

    game->map[game->food.y][game->food.x] = 'o';
    for (int i = 0; i < game->snake.length; i++) {
        game->map[game->snake.body[i].y][game->snake.body[i].x] = '*';
    }
}

// Fonction pour initialiser le serpent
void initializeSnake(struct Snake* snake, char map[HEIGHT][WIDTH]) {
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    snake->length = INITIAL_LENGTH;
    snake->capacity = INITIAL_LENGTH + 100;
    snake->body = (Position*)malloc(snake->capacity * sizeof(Position));
    if (snake->body == NULL) {
        printf("Erreur d'allocation mémoire pour le serpent.\n");
        exit(1);
    }

    snake->direction = RIGHT;
    for (int i = 0; i < snake->length; i++) {
        snake->body[i].x = startX - i;
        snake->body[i].y = startY;
        map[startY][startX - i] = '*';
    }
}

// Fonction pour agrandir dynamiquement la capacité du serpent
void expandSnakeCapacity(Snake* snake) {
    snake->capacity *= 2;
    snake->body = (Position*)realloc(snake->body, snake->capacity * sizeof(Position));
    if (snake->body == NULL) {
        printf("Erreur lors de l'extension de la mémoire du serpent.\n");
        exit(1);
    }
}

// Fonction pour générer de la nourriture
void generateFood(struct Game* game) {
    do {
        game->food.x = rand() % (WIDTH - 2) + 1;
        game->food.y = rand() % (HEIGHT - 2) + 1;
    } while (game->map[game->food.y][game->food.x] == '*');
    game->map[game->food.y][game->food.x] = 'o';
}

// Fonction pour afficher la carte
void displayMap(struct Game* game) {
    system("clear"); // Pour Linux/MacOS, utilisez "cls" pour Windows
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c ", game->map[i][j]);
        }
        printf("\n");
    }
    displayScore(game);
}

// Fonction pour lire et mettre à jour la direction
void updateDirection(Snake* snake) {
    if (_kbhit()) {
        char input = _getch();
        if ((input == UP && snake->direction != DOWN) ||
            (input == DOWN && snake->direction != UP) ||
            (input == LEFT && snake->direction != RIGHT) ||
            (input == RIGHT && snake->direction != LEFT)) {
            snake->direction = input;
        }
    }
}

// Fonction pour déplacer le serpent
bool moveSnake(struct Snake* snake, struct Game* game) {
    updateDirection(snake);
    Position next = snake->body[0];

    if (snake->direction == UP) next.y--;
    else if (snake->direction == DOWN) next.y++;
    else if (snake->direction == LEFT) next.x--;
    else if (snake->direction == RIGHT) next.x++;

    // Vérifie si le serpent frappe un mur
    if (next.x <= 0 || next.x >= WIDTH - 1 || next.y <= 0 || next.y >= HEIGHT - 1) {
        return false; // Fin du jeu
    }

    for (int i = 0; i < snake->length; i++) {
        if (snake->body[i].x == next.x && snake->body[i].y == next.y) {
            return false;
        }
    }

    bool ateFood = (next.x == game->food.x && next.y == game->food.y);
    if (ateFood && snake->length >= snake->capacity) {
        expandSnakeCapacity(snake);
    }

    for (int i = snake->length; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0] = next;
    if (ateFood) {
        snake->length++;
        game->generateFood(game);
        game->score++;
    }
    return true;
}

int main() {
    struct Game game;
    game.score = 0;
    game.initializeMap = initializeMap;
    game.generateFood = generateFood;
    game.displayMap = displayMap;

    game.snake.initialize = initializeSnake;
    game.snake.move = moveSnake;

    game.initializeMap(&game);
    game.snake.initialize(&game.snake, game.map);
    game.generateFood(&game);

    while (true) {
        resetMap(&game);
        game.displayMap(&game);
        if (!game.snake.move(&game.snake, &game)) break;
        SLEEP(200);
    }
    printf("Game Over! Score final: %d\n", game.score); // Message de game over
    saveScoreToFile(game.score); // Sauvegarde le score avant de quitter
    free(game.snake.body);
    return 0;
}
>>>>>>> dimitri

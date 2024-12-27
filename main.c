#include <stdio.h>
#include <stdlib.h>
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
    Position body[WIDTH * HEIGHT];
    int length;
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
    snake->direction = RIGHT;
    for (int i = 0; i < snake->length; i++) {
        snake->body[i].x = startX - i;
        snake->body[i].y = startY;
        map[startY][startX - i] = '*';
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

// Fonction pour déplacer le serpent
bool moveSnake(struct Snake* snake, struct Game* game) {
    Position next = snake->body[0];

    if (snake->direction == UP) {
        next.y--;
        if (next.y <= 0) next.y = HEIGHT - 2;
    } else if (snake->direction == DOWN) {
        next.y++;
        if (next.y >= HEIGHT - 1) next.y = 1;
    } else if (snake->direction == LEFT) {
        next.x--;
        if (next.x <= 0) next.x = WIDTH - 2;
    } else if (snake->direction == RIGHT) {
        next.x++;
        if (next.x >= WIDTH - 1) next.x = 1;
    }

    for (int i = 0; i < snake->length; i++) {
        if (snake->body[i].x == next.x && snake->body[i].y == next.y) {
            return false;
        }
    }

    bool ateFood = (next.x == game->food.x && next.y == game->food.y);
    Position lastTail = snake->body[snake->length - 1];
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0] = next;
    game->map[next.y][next.x] = '*';

    if (!ateFood) {
        game->map[lastTail.y][lastTail.x] = ' ';
    } else {
        if (snake->length < WIDTH * HEIGHT) {
            snake->length++;
        }
        game->generateFood(game);
        game->score++; // Augmenter le score
    }

    return true;
}

void updateDirection(struct Snake* snake) {
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

int main() {
    struct Game game;
    game.score = 0; // Initialiser le score
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
        updateDirection(&game.snake);
        if (!game.snake.move(&game.snake, &game)) {
            printf("Game Over!\n");
            saveScoreToFile(game.score); // Sauvegarder le score
            printf("Appuyez sur une touche pour quitter...\n");
            _getch();
            break;
        }
        SLEEP(200);
    }

    return 0;
}
s
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h> // Gere les touche clavier pour windwos

#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#endif

#define WIDTH 15 // taille horisontal de la carte
#define HEIGHT 15 // taille vertical de la carte
#define INITIAL_LENGTH 3 // longuer de depart du serpent

// Directions des touche pour se deplacer
#define UP 'z' // touche pour monter
#define LEFT 'q' // touche pour aller a gauche
#define DOWN 's' // touche pour descendre
#define RIGHT 'd' // touche pour aller a droite

// Représent une position en x et y
typedef struct {
    int x; // coordonée horisontale
    int y; // coordonée vertical
} Position;

// Structure pour le serpen
typedef struct Snake {
    Position *body; // position dynamique du corps
    int length; // longeur actuel
    int capacity; // capassité max
    char direction; // direction actuel du serpent
    void (*initialize)(struct Snake* snake, char map[HEIGHT][WIDTH]); // initialisation
    bool (*move)(struct Snake* snake, struct Game* game); // fonction pour deplacement
} Snake;

// Gere l'état du jeu
typedef struct Game {
    char map[HEIGHT][WIDTH]; // La carte du jeu
    struct Snake snake; // Le serpent
    Position food; // La position de la nouriture
    void (*initializeMap)(struct Game* game); // initialise la carte
    void (*generateFood)(struct Game* game); // cree la nouriture
    void (*displayMap)(struct Game* game); // affiche la carte
    int score; // Le score du jeu
} Game;

// Rempli la carte avec des murs et des espaces
void initializeMap(struct Game* game) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                game->map[i][j] = '#'; // murs autour de la carte
            } else {
                game->map[i][j] = ' '; // espace vide pour le jeu
            }
        }
    }
}

// Affiche le score actuel
void displayScore(struct Game* game) {
    printf("Score: %d\n", game->score); // montre le score
}

// Sauvegarde le score dans un fichier
void saveScoreToFile(int score) {
    FILE *file = fopen("score.txt", "a"); // ouvre le fichier en mode ajout
    if (file != NULL) {
        fprintf(file, "Score: %d\n", score); // ecrit le score
        fclose(file); // ferme le fichier
    } else {
        printf("Impossible d'ouvrir le fichier pour enregistrer le score.\n"); // message d'erreur
    }
}

// Efface et redessine la carte a chaque coup
void resetMap(struct Game* game) {
    for (int i = 1; i < HEIGHT - 1; i++) {
        for (int j = 1; j < WIDTH - 1; j++) {
            game->map[i][j] = ' '; // efface les espace vide
        }
    }

    game->map[game->food.y][game->food.x] = 'o'; // place la nouriture
    for (int i = 0; i < game->snake.length; i++) {
        game->map[game->snake.body[i].y][game->snake.body[i].x] = '*'; // place le serpent
    }
}

// Initialise le serpent au centre
void initializeSnake(struct Snake* snake, char map[HEIGHT][WIDTH]) {
    int startX = WIDTH / 2; // position x au milieu
    int startY = HEIGHT / 2; // position y au milieu
    snake->length = INITIAL_LENGTH; // longuer de base
    snake->capacity = INITIAL_LENGTH + 100; // capaciter initial + marge
    snake->body = (Position*)malloc(snake->capacity * sizeof(Position)); // allocation memoire pour le corps
    if (snake->body == NULL) {
        printf("Erreur d'allocation mémoire pour le serpent.\n");
        exit(1); // quitte en cas d'erreur memoire
    }

    snake->direction = RIGHT; // commence a aller a droite
    for (int i = 0; i < snake->length; i++) {
        snake->body[i].x = startX - i; // position initiale du serpent
        snake->body[i].y = startY;
        map[startY][startX - i] = '*'; // affiche sur la carte
    }
}

// Augment la capacité du serpent si nécéssaire
void expandSnakeCapacity(Snake* snake) {
    snake->capacity *= 2; // double la capacité
    snake->body = (Position*)realloc(snake->body, snake->capacity * sizeof(Position)); // realloc pour plus de place
    if (snake->body == NULL) {
        printf("Erreur lors de l'extension de la mémoire du serpent.\n");
        exit(1); // quitte si la memoire echoue
    }
}

// Genere la nouriture dans un endroit libre
void generateFood(struct Game* game) {
    do {
        game->food.x = rand() % (WIDTH - 2) + 1; // x aleatoire
        game->food.y = rand() % (HEIGHT - 2) + 1; // y aleatoire
    } while (game->map[game->food.y][game->food.x] == '*'); // evite de placer sur le serpent
    game->map[game->food.y][game->food.x] = 'o'; // ajoute la nouriture
}

// Affiche la carte
void displayMap(struct Game* game) {
    system("cls"); // Pour Windows
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c ", game->map[i][j]); // affiche chaque case
        }
        printf("\n"); // saut de ligne
    }
    displayScore(game); // affiche le score
}

// Lis les touches et met a jour la direction
void updateDirection(Snake* snake) {
    if (_kbhit()) {
        char input = _getch(); // recupere la touche
        if ((input == UP && snake->direction != DOWN) || // verifie la direction
            (input == DOWN && snake->direction != UP) ||
            (input == LEFT && snake->direction != RIGHT) ||
            (input == RIGHT && snake->direction != LEFT)) {
            snake->direction = input; // met a jour la direction
        }
    }
}

// Déplace le serpent, verifi les collision et si il mange
bool moveSnake(struct Snake* snake, struct Game* game) {
    updateDirection(snake);
    Position next = snake->body[0]; // position suivante

    if (snake->direction == UP) next.y--;
    else if (snake->direction == DOWN) next.y++;
    else if (snake->direction == LEFT) next.x--;
    else if (snake->direction == RIGHT) next.x++;

    // Si il tape un mur fin de jeux
    if (next.x <= 0 || next.x >= WIDTH - 1 || next.y <= 0 || next.y >= HEIGHT - 1) {
        return false; // termine le jeu
    }

    for (int i = 0; i < snake->length; i++) {
        if (snake->body[i].x == next.x && snake->body[i].y == next.y) {
            return false; // collision avec le corp
        }
    }

    bool ateFood = (next.x == game->food.x && next.y == game->food.y); // verifie si il mange
    if (ateFood && snake->length >= snake->capacity) {
        expandSnakeCapacity(snake); // agrandis si necessaire
    }

    for (int i = snake->length; i > 0; i--) {
        snake->body[i] = snake->body[i - 1]; // deplace chaque segment
    }

    snake->body[0] = next; // met a jour la tete
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
    printf("Game Over! Score final: %d\n", game.score);
    saveScoreToFile(game.score);
    free(game.snake.body);
    return 0;
}

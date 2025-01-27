#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Dimensions de la grille
#define WIDTH 30
#define HEIGHT 15
#define INITIAL_LENGTH 3

// Caractères pour les éléments
#define WALL '#'
#define EMPTY ' '
#define SNAKE '*'
#define FOOD 'o'
#define POWER_UP 'P'

// Directions
typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

// Position
typedef struct {
    int x;
    int y;
} Position;

// Serpent
typedef struct {
    Position *body;           // Tableau contenant les positions du serpent
    int length;               // Longueur actuelle du serpent
    int capacity;             // Capacité maximale du tableau
    Direction direction;      // Direction actuelle du serpent
    bool invincible;          // Indique si le serpent est invincible
    int invincibility_timer;  // Temps restant d'invincibilité
} Snake;

// Jeu
typedef struct {
    char map[HEIGHT][WIDTH];  // Carte du jeu
    Snake snake;              // Serpent
    Position food;            // Position de la nourriture
    Position power_up;        // Position du power-up
    bool has_power_up;        // Indique si un power-up est présent
    int score;                // Score du joueur
} Game;

// Prototypes des fonctions
void initializeMap(Game *game);
void initializeSnake(Game *game);
void generateFood(Game *game);
void generatePowerUp(Game *game);
void resetMap(Game *game);
void displayMap(Game *game);
bool moveSnake(Game *game);
void updateDirection(Snake *snake, int input);
void applyPowerUp(Game *game);

// Fonction principale
int main() {
    // Initialisation de ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);  // Activation des flèches directionnelles
    timeout(200);          // Délai pour les entrées utilisateur

    // Initialisation du jeu
    srand(time(NULL));
    Game game;
    game.score = 0;
    game.has_power_up = false;

    initializeMap(&game);
    initializeSnake(&game);
    generateFood(&game);

    // Boucle principale du jeu
    while (true) {
        resetMap(&game);       // Mise à jour de la carte
        displayMap(&game);     // Affichage de la carte

        int input = getch();   // Récupération de l'entrée utilisateur
        updateDirection(&game.snake, input); // Mise à jour de la direction

        if (!moveSnake(&game)) {  // Déplacement du serpent
            break; // Fin du jeu si collision
        }

        // Génération aléatoire d'un power-up (1 chance sur 10 par tour)
        if (!game.has_power_up && rand() % 10 == 0) {
            generatePowerUp(&game);
        }
    }

    // Fin du jeu
    endwin();
    printf("Game Over! Score final: %d\n", game.score);
    free(game.snake.body); // Libération de la mémoire allouée pour le serpent
    return 0;
}

// Initialisation de la carte
void initializeMap(Game *game) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            // Ajout de murs en bordure et d'espaces vides ailleurs
            game->map[i][j] = (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) ? WALL : EMPTY;
        }
    }
}

// Initialisation du serpent
void initializeSnake(Game *game) {
    Snake *snake = &game->snake;
    snake->length = INITIAL_LENGTH; // Longueur initiale du serpent
    snake->capacity = INITIAL_LENGTH + 100; // Capacité initiale du tableau
    snake->body = malloc(snake->capacity * sizeof(Position)); // Allocation dynamique du tableau
    snake->direction = RIGHT; // Direction initiale
    snake->invincible = false;
    snake->invincibility_timer = 0;

    // Placement initial au centre de la carte
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    for (int i = 0; i < snake->length; i++) {
        snake->body[i].x = startX - i;
        snake->body[i].y = startY;
    }
}

// Génération de la nourriture
void generateFood(Game *game) {
    do {
        game->food.x = rand() % (WIDTH - 2) + 1;
        game->food.y = rand() % (HEIGHT - 2) + 1;
    } while (game->map[game->food.y][game->food.x] != EMPTY);
}

// Génération d'un power-up
void generatePowerUp(Game *game) {
    do {
        game->power_up.x = rand() % (WIDTH - 2) + 1;
        game->power_up.y = rand() % (HEIGHT - 2) + 1;
    } while (game->map[game->power_up.y][game->power_up.x] != EMPTY);

    game->has_power_up = true;
}

// Réinitialisation de la carte (mise à jour des positions des éléments)
void resetMap(Game *game) {
    for (int i = 1; i < HEIGHT - 1; i++) {
        for (int j = 1; j < WIDTH - 1; j++) {
            game->map[i][j] = EMPTY;
        }
    }
    for (int i = 0; i < game->snake.length; i++) {
        game->map[game->snake.body[i].y][game->snake.body[i].x] = SNAKE;
    }
    game->map[game->food.y][game->food.x] = FOOD;
    if (game->has_power_up) {
        game->map[game->power_up.y][game->power_up.x] = POWER_UP;
    }
}

// Affichage de la carte
void displayMap(Game *game) {
    clear();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvaddch(i, j, game->map[i][j]); // Affichage caractère par caractère
        }
    }
    mvprintw(HEIGHT, 0, "Score: %d", game->score); // Affichage du score
    if (game->snake.invincible) {
        mvprintw(HEIGHT + 1, 0, "Invincible: %d moves left", game->snake.invincibility_timer);
    }
    refresh();
}

// Gestion du déplacement du serpent
bool moveSnake(Game *game) {
    Snake *snake = &game->snake;

    // Calcul de la nouvelle position de la tête
    Position next = snake->body[0];
    switch (snake->direction) {
        case UP: next.y--; break;
        case DOWN: next.y++; break;
        case LEFT: next.x--; break;
        case RIGHT: next.x++; break;
    }

    // Collision avec les murs
    if (!snake->invincible && (next.x <= 0 || next.x >= WIDTH - 1 || next.y <= 0 || next.y >= HEIGHT - 1)) {
        return false;
    }

    // Collision avec le corps du serpent
    for (int i = 0; i < snake->length; i++) {
        if (!snake->invincible && snake->body[i].x == next.x && snake->body[i].y == next.y) {
            return false;
        }
    }

    // Collision avec la nourriture
    if (next.x == game->food.x && next.y == game->food.y) {
        if (snake->length >= snake->capacity) {
            snake->capacity *= 2;
            snake->body = realloc(snake->body, snake->capacity * sizeof(Position));
        }
        game->score++;
        snake->length++;
        generateFood(game);
    }

    // Collision avec le power-up
    if (game->has_power_up && next.x == game->power_up.x && next.y == game->power_up.y) {
        applyPowerUp(game);
        game->has_power_up = false;
    }

    // Déplacement du corps du serpent
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = next; // Mise à jour de la tête

    // Réduction du temps d'invincibilité
    if (snake->invincible && --snake->invincibility_timer <= 0) {
        snake->invincible = false;
    }

    return true;
}

// Mise à jour de la direction en fonction de l'entrée utilisateur
void updateDirection(Snake *snake, int input) {
    Direction newDirection = snake->direction;
    if (input == KEY_UP) newDirection = UP;
    else if (input == KEY_DOWN) newDirection = DOWN;
    else if (input == KEY_LEFT) newDirection = LEFT;
    else if (input == KEY_RIGHT) newDirection = RIGHT;

    // Empêche les directions opposées
    if ((newDirection == UP && snake->direction != DOWN) ||
        (newDirection == DOWN && snake->direction != UP) ||
        (newDirection == LEFT && snake->direction != RIGHT) ||
        (newDirection == RIGHT && snake->direction != LEFT)) {
        snake->direction = newDirection;
    }
}

// Application des effets du power-up
void applyPowerUp(Game *game) {
    Snake *snake = &game->snake;
    int power = rand() % 4; // 4 types de power-ups

    switch (power) {
        case 0: // Double déplacement (simulé par un score bonus)
            game->score += 2;
            break;
        case 1: // Réduction de taille
            if (snake->length > 1) snake->length--;
            break;
        case 2: // Invincibilité
            snake->invincible = true;
            snake->invincibility_timer = 5;
            break;
        case 3: // Téléportation
            snake->body[0].x = rand() % (WIDTH - 2) + 1;
            snake->body[0].y = rand() % (HEIGHT - 2) + 1;
            break;
    }
}

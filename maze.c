#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIM 100
#define MIN_DIM 5
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

typedef struct __Coord {
    int x;
    int y;
} coord;

typedef struct __Maze {
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

int create_maze(maze *this, int height, int width) {
    if (height < MIN_DIM || height > MAX_DIM || width < MIN_DIM || width > MAX_DIM)
        return 1;
    
    this->height = height;
    this->width = width;
    this->map = (char **)malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        this->map[i] = (char *)malloc((width + 1) * sizeof(char));  // +1 for null-terminator
    }
    return 0;
}

void free_maze(maze *this) {
    for (int i = 0; i < this->height; i++) {
        free(this->map[i]);
    }
    free(this->map);
}

int get_width(FILE *file) {
    char buffer[MAX_DIM + 2];  // Allow space for newline and null terminator
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        return 0;  // Return 0 if no data could be read
    }

    size_t len = strcspn(buffer, "\n");
    buffer[len] = '\0';

    rewind(file);

    if (len < MIN_DIM || len > MAX_DIM) {
        return 0;
    }
    return (int)len;
}

int get_height(FILE *file) {
    int height = 0;
    char buffer[MAX_DIM + 2];
    int expected_width = get_width(file);

    if (expected_width == 0) {
        return 0;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (buffer[strcspn(buffer, "\n")] != '\0') {
            buffer[strcspn(buffer, "\n")] = '\0';
        }
        if ((int)strlen(buffer) == expected_width) {
            height++;
        } else {
            return 0;
        }
    }
    if (height < MIN_DIM || height > MAX_DIM) {
        return 0;
    }
    rewind(file);
    return height;
}

int read_maze(maze *this, FILE *file) {
    char buffer[MAX_DIM + 2];
    int y = 0;
    while (fgets(buffer, sizeof(buffer), file) && y < this->height) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) != this->width) return 1;
        strcpy(this->map[y], buffer);
        
        for (int x = 0; x < this->width; x++) {
            if (buffer[x] == 'S') {
                this->start.x = x;
                this->start.y = y;
            } else if (buffer[x] == 'E') {
                this->end.x = x;
                this->end.y = y;
            }
        }
        y++;
    }
    return (y != this->height);
}

void print_maze(maze *this, coord *player) {
    printf("\n");
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if (player->x == j && player->y == i)
                printf("X");
            else
                printf("%c", this->map[i][j]);
        }
        printf("\n");
    }
}

void move(maze *this, coord *player, char direction) {
    int newX = player->x;
    int newY = player->y;
    switch (direction) {
        case 'W': case 'w':
            newY--;
            break;
        case 'S': case 's':
            newY++;
            break;
        case 'A': case 'a':
            newX--;
            break;
        case 'D': case 'd':
            newX++;
            break;
    }
    if (newX >= 0 && newX < this->width && newY >= 0 && newY < this->height && this->map[newY][newX] != '#') {
        player->x = newX;
        player->y = newY;
    } else {
        printf("Invalid move!\n");
    }
}

int has_won(maze *this, coord *player) {
    return player->x == this->end.x && player->y == this->end.y;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./maze <mazefile path>\n");
        return EXIT_ARG_ERROR;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FILE_ERROR;
    }

    int width = get_width(file);
    int height = get_height(file);
    if (width == 0 || height == 0) {
        fprintf(stderr, "Invalid maze dimensions or inconsistent line widths.\n");
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    maze *this_maze = malloc(sizeof(maze));
    if (!this_maze || create_maze(this_maze, height, width) != 0) {
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    if (read_maze(this_maze, file) != 0) {
        fprintf(stderr, "Error reading maze data.\n");
        fclose(file);
        free_maze(this_maze);
        return EXIT_MAZE_ERROR;
    }

    fclose(file);
    coord player = this_maze->start;
    char command;

    while (!has_won(this_maze, &player)) {
        scanf(" %c", &command);
        if (command == 'M' || command == 'm') {
            print_maze(this_maze, &player);
        } else {
            move(this_maze, &player, command);
        }
    }

    printf("Congratulations, you've won!\n");
    free_maze(this_maze);
    return EXIT_SUCCESS;
}

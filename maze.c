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
    this->map = malloc(height * sizeof(char *));
    if (!this->map)
        return 1;
    for (int i = 0; i < height; i++) {
        this->map[i] = malloc(width * sizeof(char));
        if (!this->map[i])
            return 1;
    }
    this->height = height;
    this->width = width;
    return 0;
}

void free_maze(maze *this) {
    for (int i = 0; i < this->height; i++) {
        free(this->map[i]);
    }
    free(this->map);
    free(this);
}

int get_width(FILE *file) {
    char line[MAX_DIM + 2];  // Consider newline and null-terminator
    fgets(line, sizeof(line), file);
    return strlen(line) - 1;  // Subtract one for newline
}

int get_height(FILE *file) {
    char line[MAX_DIM + 2];
    int height = 0;
    while (fgets(line, sizeof(line), file)) {
        height++;
    }
    rewind(file);  // Reset file pointer after counting
    return height;
}

int read_maze(maze *this, FILE *file) {
    for (int i = 0; i < this->height; i++) {
        fgets(this->map[i], this->width + 2, file);  // +2 for newline and null-terminator
        if (this->map[i][strlen(this->map[i]) - 1] == '\n')
            this->map[i][strlen(this->map[i]) - 1] = '\0';  // Remove newline
        for (int j = 0; j < this->width; j++) {
            if (this->map[i][j] == 'S') {
                this->start.x = j;
                this->start.y = i;
            } else if (this->map[i][j] == 'E') {
                this->end.x = j;
                this->end.y = i;
            }
        }
    }
    return 0;
}

void print_maze(maze *this, coord *player) {
    printf("\n");
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if (player->x == j && player->y == i) {
                printf("X");
            } else {
                printf("%c", this->map[i][j]);
            }
        }
        printf("\n");
    }
}

void move(maze *this, coord *player, char direction) {
    int new_x = player->x, new_y = player->y;
    switch (direction) {
        case 'W': case 'w':
            new_y--; break;
        case 'S': case 's':
            new_y++; break;
        case 'A': case 'a':
            new_x--; break;
        case 'D': case 'd':
            new_x++; break;
        default:
            return;  // Ignore invalid commands
    }
    if (new_x >= 0 && new_x < this->width && new_y >= 0 && new_y < this->height &&
        this->map[new_y][new_x] != '#') {
        player->x = new_x;
        player->y = new_y;
    } else {
        printf("Invalid move!\n");
    }
}

int has_won(maze *this, coord *player) {
    return player->x == this->end.x && player->y == this->end.y;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mazefile path>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error opening file\n");
        return EXIT_FILE_ERROR;
    }

    maze *this_maze = malloc(sizeof(maze));
    if (!this_maze) {
        fclose(f);
        return EXIT_FILE_ERROR;
    }

    this_maze->width = get_width(f);
    this_maze->height = get_height(f);
    if (!create_maze(this_maze, this_maze->height, this_maze->width) && !read_maze(this_maze, f)) {
        coord player = this_maze->start;
        char command;
        do {
            if (has_won(this_maze, &player)) {
                printf("Congratulations! You have escaped the maze.\n");
                break;
            }
            command = getchar();
            if (command == 'M' || command == 'm') {
                print_maze(this_maze, &player);
            } else {
                move(this_maze, &player, command);
            }
        } while (command != EOF && command != 'Q' && command != 'q');
    } else {
        fprintf(stderr, "Failed to initialize or read maze\n");
        free(this_maze);
        fclose(f);
        return EXIT_MAZE_ERROR;
    }

    free_maze(this_maze);
    fclose(f);
    return EXIT_SUCCESS;
}

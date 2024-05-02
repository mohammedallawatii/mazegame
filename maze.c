#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIM 100
#define MIN_DIM 5

#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

int create_maze(maze *this, int height, int width)
{
    this->height = height;
    this->width = width;
    this->map = (char **)malloc(sizeof(char *) * height);
    if (this->map == NULL)
        return 1;
    for (int i = 0; i < height; i++)
    {
        this->map[i] = (char *)malloc(sizeof(char) * (width + 1));
        if (this->map[i] == NULL)
            return 1;
    }
    return 0;
}

void free_maze(maze *this)
{
    for (int i = 0; i < this->height; i++)
    {
        free(this->map[i]);
    }
    free(this->map);
}

int get_width(FILE *file)
{
    int width = 0;
    fseek(file, 0, SEEK_SET);
    char c;
    while ((c = fgetc(file)) != EOF && c != '\n')
    {
        width++;
    }
    return width;
}

int get_height(FILE *file)
{
    int height = 1;
    char c;

    fseek(file, 0, SEEK_SET);
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            height++;
        }
    }
    return height;
}

int read_maze(maze *this, FILE *file)
{
    int width = get_width(file);
    int height = get_height(file);

    if (width < MIN_DIM || width > MAX_DIM || height < MIN_DIM || height > MAX_DIM) {
        return 1;
    }

    if (create_maze(this, height, width) != 0) {
        return 1;
    }

    fseek(file, 0, SEEK_SET);
    char buffer[width + 2];
    for (int i = 0; i < height; i++) {
        if (fgets(buffer, sizeof(buffer), file)) {
            buffer[strcspn(buffer, "\r\n")] = '\0';
            strncpy(this->map[i], buffer, width + 1);
            for (int j = 0; j < width; j++) {
                if (this->map[i][j] == 'S') {
                    this->start.x = j;
                    this->start.y = i;
                } else if (this->map[i][j] == 'E') {
                    this->end.x = j;
                    this->end.y = i;
                }
            }
        }
    }

    return 0;
}

void print_maze(maze *this, coord *player)
{
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (player->x == j && player->y == i)
            {
                printf("X");
            }
            else
            {
                printf("%c", this->map[i][j]);
            }
        }
        printf("\n");
    }
}

void move(maze *this, coord *player, char direction)
{
    int newX = player->x;
    int newY = player->y;

    switch (direction)
    {
    case 'W':
    case 'w':
        newY--;
        break;
    case 'A':
    case 'a':
        newX--;
        break;
    case 'S':
    case 's':
        newY++;
        break;
    case 'D':
    case 'd':
        newX++;
        break;
    default:
        printf("Invalid direction! Use WASD.\n");
        return;
    }

    if (newX < 0 || newX >= this->width || newY < 0 || newY >= this->height)
    {
        printf("Can't move in that direction!\n");
        return;
    }

    if (this->map[newY][newX] == '#')
    {
        printf("Can't move through walls!\n");
        return;
    }

    player->x = newX;
    player->y = newY;
}

int has_won(maze *this, coord *player)
{
    return (player->x == this->end.x && player->y == this->end.y);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <mazefile path>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return EXIT_FILE_ERROR;
    }

    maze this_maze;
    if (read_maze(&this_maze, f) != 0)
    {
        printf("Invalid maze!\n");
        fclose(f);
        return EXIT_MAZE_ERROR;
    }
    fclose(f);

    coord player = this_maze.start;

    print_maze(&this_maze, &player);

    char input;
    do
    {
        printf("\nEnter your move (W/A/S/D/M for map): ");
        scanf(" %c", &input);
        if (input == 'M' || input == 'm')
        {
            print_maze(&this_maze, &player);
        }
        else
        {
            move(&this_maze, &player, input);
        }
    } while (!has_won(&this_maze, &player));

    printf("\nCongratulations! You've won!\n");

    free_maze(&this_maze);

    return EXIT_SUCCESS;
}


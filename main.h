#ifndef MAIN_H
#define MAIN_H

#define WIDTH 52
#define HEIGHT 27
#define SNAKE "o"
#define SNAKE_HEAD "O"
#define SNAKE_LENGTH 3
#define APPLE "x"
#define BORDER "#"

typedef struct snake_tile {
  int x;
  int y;
  struct snake_tile *prev;
} snake_tile;

struct snake_tile *create_snake_tile(int x, int y);
void append(struct snake_tile **snake, int x, int y);
void push(struct snake_tile **snake, int x, int y);
void destroy_snake(struct snake_tile *snake_head);

struct snake_tile *create_snake(struct snake_tile *snake_head, int *x_dir,
                                int *y_dir);
void update(struct snake_tile *snake, int *x_dir, int *y_dir, int *death,
            int *apple_x, int *apple_y, int *score);
void determine_direction(int *x_dir, int *y_dir);
void draw_field(struct snake_tile *snake, int apple_x, int apple_y, int score);
int is_dead(struct snake_tile *snake);
int is_on_snake(struct snake_tile *snake, int apple_x, int apple_y);
void apple_eaten(struct snake_tile *snake, int *apple_x, int *apple_y);

#endif
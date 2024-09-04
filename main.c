#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

struct snake_tile *create_snake(struct snake_tile *snake_head, int *x_dir,
                                int *y_dir);
void update(struct snake_tile *snake, int *x_dir, int *y_dir, int *death,
            int *apple_x, int *apple_y, int *score);
void determine_direction(int *x_dir, int *y_dir);
void draw_field(struct snake_tile *snake, int apple_x, int apple_y, int score);
int is_dead(struct snake_tile *snake);
int is_on_snake(struct snake_tile *snake, int apple_x, int apple_y);
void apple_eaten(struct snake_tile *snake, int *apple_x, int *apple_y);

int main() {
  struct snake_tile *snake_head = NULL;

  srand(time(NULL));

  int death = 0, x_dir, y_dir;
  int score = 0;
  snake_head = create_snake(snake_head, &x_dir, &y_dir);

  int apple_x = rand() % (WIDTH - 2) + 1, apple_y = rand() % (HEIGHT - 2) + 1;

  initscr();
  curs_set(FALSE);
  cbreak();
  noecho();

  while (death == 0) {
    draw_field(snake_head, apple_x, apple_y, score);
    update(snake_head, &x_dir, &y_dir, &death, &apple_x, &apple_y, &score);
  }
  endwin();
  return 0;
}

struct snake_tile *create_snake_tile(int x, int y) {
  struct snake_tile *new_tile =
      (struct snake_tile *)calloc(1, sizeof(struct snake_tile));
  if (new_tile != NULL) {
    new_tile->x = x;
    new_tile->y = y;
    new_tile->prev = NULL;
  } else {
    printw("[ERROR] Memory allocation failed for these coordinates: (%d, %d)",
           x, y);
  }
  return new_tile;
}

void append(struct snake_tile **snake, int x, int y) {
  struct snake_tile *new_tile = create_snake_tile(x, y);
  if (*snake != NULL) {
    struct snake_tile *last_tile = *snake;
    while (last_tile->prev != NULL) {
      last_tile = last_tile->prev;
    }
    last_tile->prev = new_tile;
  } else {
    *snake = new_tile;
  }
}

void push(struct snake_tile **snake, int x, int y) {
  struct snake_tile *new_tile = create_snake_tile(x, y);
  new_tile->prev = *snake;
  *snake = new_tile;
}

void update(struct snake_tile *snake, int *x_dir, int *y_dir, int *death,
            int *apple_x, int *apple_y, int *score) {
  struct snake_tile *temp = snake;
  int next_x = snake->x, next_y = snake->y;

  determine_direction(x_dir, y_dir);

  if (snake->x + *x_dir == *apple_x && snake->y + *y_dir == *apple_y) {
    apple_eaten(snake, apple_x, apple_y);
    (*score)++;
  }

  snake->x += *x_dir;
  snake->y += *y_dir;
  snake = snake->prev;

  while (snake != NULL) {
    int temp_next_x, temp_next_y;
    temp_next_x = snake->x;
    temp_next_y = snake->y;
    snake->x = next_x;
    snake->y = next_y;
    next_x = temp_next_x;
    next_y = temp_next_y;
    snake = snake->prev;
  }

  snake = temp;
  *death = is_dead(snake);
}

void determine_direction(int *x_dir, int *y_dir) {
  timeout(200);
  char move = getch();

  switch (move) {
  case 'w':
    if (*x_dir == 0 && *y_dir == 1) {
      break;
    }
    *x_dir = 0;
    *y_dir = -1;
    break;
  case 's':
    if (*x_dir == 0 && *y_dir == -1) {
      break;
    }
    *x_dir = 0;
    *y_dir = 1;
    break;
  case 'a':
    if (*x_dir == 1 && *y_dir == 0) {
      break;
    }
    *x_dir = -1;
    *y_dir = 0;
    break;
  case 'd':
    if (*x_dir == -1 && *y_dir == 0) {
      break;
    }
    *x_dir = 1;
    *y_dir = 0;
    break;
  default:
    break;
  }
}

void draw_field(struct snake_tile *snake, int apple_x, int apple_y, int score) {
  mvprintw(HEIGHT / 2, WIDTH + 15, "Score: %d", score);
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (i == 0 || j == 0 || i == HEIGHT - 1 || j == WIDTH - 1) {
        mvprintw(i, j, BORDER);
      } else {
        mvprintw(i, j, " ");
      }
    }
  }

  mvprintw(apple_y, apple_x, APPLE);

  struct snake_tile *temp = snake;
  int is_head = 1;
  while (temp != NULL) {
    if (is_head) {
      mvprintw(temp->y, temp->x, SNAKE_HEAD);
      is_head = 0;
    } else {
      mvprintw(temp->y, temp->x, SNAKE);
    }
    temp = temp->prev;
  }
}

int is_dead(struct snake_tile *snake) {
  struct snake_tile *temp = snake;
  int died = 0;
  if (temp->x == 0 || temp->x == WIDTH - 1 || temp->y == 0 ||
      temp->y == HEIGHT - 1) {
    died = 1;
  }
  const struct snake_tile *head = temp;
  temp = temp->prev;
  while (temp != NULL && !died) {
    if (head->x == temp->x && head->y == temp->y) {
      died = 1;
    } else {
      temp = temp->prev;
    }
  }
  return died;
}

int is_on_snake(struct snake_tile *snake, int apple_x, int apple_y) {
  int on_snake = 0;
  struct snake_tile *temp = snake;
  while (temp->prev != NULL && !on_snake) {
    if (temp->x == apple_x && temp->y == apple_y) {
      on_snake = 1;
    }
    temp = temp->prev;
  }
  return on_snake;
}

void apple_eaten(struct snake_tile *snake, int *apple_x, int *apple_y) {
  struct snake_tile *last_tile = snake;
  struct snake_tile *second_last_tile = snake;

  while (last_tile->prev != NULL) {
    last_tile = last_tile->prev;
  }

  while (second_last_tile->prev != last_tile) {
    second_last_tile = second_last_tile->prev;
  }

  if (last_tile->x > second_last_tile->x) {
    append(&snake, last_tile->x + 1, last_tile->y);
  } else if (last_tile->x < second_last_tile->x) {
    append(&snake, last_tile->x - 1, last_tile->y);
  } else if (last_tile->y > second_last_tile->y) {
    append(&snake, last_tile->x, last_tile->y + 1);
  } else if (last_tile->y < second_last_tile->y) {
    append(&snake, last_tile->x, last_tile->y - 1);
  }

  *apple_x = rand() % (WIDTH - 2) + 1;
  *apple_y = rand() % (HEIGHT - 2) + 1;
  while (is_on_snake(snake, *apple_x, *apple_y)) {
    *apple_x = rand() % (WIDTH - 2) + 1;
    *apple_y = rand() % (HEIGHT - 2) + 1;
  }
}

struct snake_tile *create_snake(struct snake_tile *snake_head, int *x_dir,
                                int *y_dir) {
  int snake_head_x = rand() % (WIDTH - 2) + SNAKE_LENGTH;
  int snake_head_y = rand() % (HEIGHT - 2) + SNAKE_LENGTH;
  push(&snake_head, snake_head_x, snake_head_y);
  int chosen_direction = rand() % 4;
  // 0 - BODY UP, 1 - BODY RIGHT, 2 - BODY DOWN, 3 - BODY LEFT
  switch (chosen_direction) {
  case 0:
    for (int i = 1; i < SNAKE_LENGTH; i++) {
      append(&snake_head, snake_head_x, snake_head_y - i);
    }
    *x_dir = 0;
    *y_dir = 1;
    break;
  case 1:
    for (int i = 1; i < SNAKE_LENGTH; i++) {
      append(&snake_head, snake_head_x + i, snake_head_y);
    }
    *x_dir = -1;
    *y_dir = 0;
    break;
  case 2:
    for (int i = 1; i < SNAKE_LENGTH; i++) {
      append(&snake_head, snake_head_x, snake_head_y + i);
    }
    *x_dir = 0;
    *y_dir = -1;
    break;
  case 3:
    for (int i = 1; i < SNAKE_LENGTH; i++) {
      append(&snake_head, snake_head_x - i, snake_head_y);
    }
    *x_dir = 1;
    *y_dir = 0;
    break;
  }
  return snake_head;
}

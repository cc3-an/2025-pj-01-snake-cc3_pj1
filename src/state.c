#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);
static char* read_really_long_line(FILE* file);

/* Tarea 1 */
game_state_t* create_default_state() {
    game_state_t *state = malloc(sizeof(game_state_t));
    if (!state) return NULL;

    state->num_rows = 18;
    state->board = malloc(sizeof(char *) * state->num_rows);
    if (!state->board) {
        free(state);
        return NULL;
    }

    char *default_board[] = {
        "####################",
        "#                  #",
        "# d>D    *         #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "####################"
    };

    unsigned int i = 0;
    while (i < state->num_rows) {
        size_t len = strlen(default_board[i]);
        state->board[i] = malloc(len + 1);
        if (!state->board[i]) {
            unsigned int j = 0;
            while (j < i) {
                free(state->board[j]);
                j++;
            }
            free(state->board);
            free(state);
            return NULL;
        }
        strcpy(state->board[i], default_board[i]);
        i++;
    }

    state->num_snakes = 1;
    state->snakes = malloc(sizeof(snake_t));
    if (!state->snakes) {
        for (unsigned int i = 0; i < state->num_rows; i++) free(state->board[i]);
        free(state->board);
        free(state);
        return NULL;
    }

    state->snakes[0].tail_row = 2;
    state->snakes[0].tail_col = 2;
    state->snakes[0].head_row = 2;
    state->snakes[0].head_col = 4;
    state->snakes[0].live = true;

    return state;
}

/* Tarea 2 */
void free_state(game_state_t* state) {
    if (!state) return;

    if (state->board) {
        for (unsigned int i = 0; i < state->num_rows; i++) {
            free(state->board[i]);
        }
        free(state->board);
    }

    if (state->snakes) {
        free(state->snakes);
    }

    free(state);
}

/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
    if (!state || !fp) return;
    for (unsigned int i = 0; i < state->num_rows; i++) {
        fprintf(fp, "%s\n", state->board[i]);
    }
}

void save_board(game_state_t* state, char* filename) {
    if (!state || !filename) return;
    FILE* archivo = fopen(filename, "w");
    if (!archivo) return;
    print_board(state, archivo);
    fclose(archivo);
}

/* Tarea 4.1 */
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
    return state->board[row][col];
}

static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
    state->board[row][col] = ch;
}

static bool is_tail(char c) {
    return c == 'w' || c == 'a' || c == 's' || c == 'd';
}

static bool is_head(char c) {
    return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}

static bool is_snake(char c) {
    return is_tail(c) || c == '^' || c == '<' || c == 'v' || c == '>' || is_head(c);
}

static char body_to_tail(char c) {
  if (c == '^') {
      return 'w';
  } else if (c == 'v') {
      return 's';
  } else if (c == '>') {
      return 'd';
  } else if (c == '<') {
      return 'a';
  } else {
      return '?';
  }
}

static char head_to_body(char c) {
    switch (c) {
        case 'W': return '^';
        case 'A': return '<';
        case 'S': return 'v';
        case 'D': return '>';
        default: return '?';
    }
}

static unsigned int get_next_row(unsigned int cur_row, char c) {

    if (c == '^' || c == 'W' || c == 'w') return cur_row - 1;
    if (c == 'v' || c == 'S' || c == 's') return cur_row + 1;

    return cur_row;
}

static unsigned int get_next_col(unsigned int cur_col, char c) {

    if (c == '<' || c == 'A' || c == 'a') return cur_col - 1;
    if (c == '>' || c == 'D' || c == 'd') return cur_col + 1;

    return cur_col;
}

/* Tarea 4.2 */
static char next_square(game_state_t* state, unsigned int snum) {

    snake_t snake = state->snakes[snum];

    char head = get_board_at(state, snake.head_row, snake.head_col);

    unsigned int newRow = get_next_row(snake.head_row, head);
    unsigned int newColumn = get_next_col(snake.head_col, head);

    return get_board_at(state, newRow, newColumn);
}

/* Tarea 4.3 */
static void update_head(game_state_t* state, unsigned int snum) {
    snake_t* snake = &state->snakes[snum];
    char head = get_board_at(state, snake->head_row, snake->head_col);
    char body = head_to_body(head);
    set_board_at(state, snake->head_row, snake->head_col, body);
    unsigned int newRow = get_next_row(snake->head_row, head);
    unsigned int newColumn = get_next_col(snake->head_col, head);
    set_board_at(state, newRow, newColumn, head);
    snake->head_row = newRow;
    snake->head_col = newColumn;
}

/* Tarea 4.4 */
static void update_tail(game_state_t* state, unsigned int snum) {
    snake_t* snake = &state->snakes[snum];

    unsigned int tailRow = snake->tail_row;
    unsigned int tailColumn = snake->tail_col;

    char tail = get_board_at(state, tailRow, tailColumn);
    set_board_at(state, tailRow, tailColumn, ' ');

    unsigned int newRow = get_next_row(tailRow, tail);
    unsigned int newColumn = get_next_col(tailColumn, tail);

    char next = get_board_at(state, newRow, newColumn);
    char new_tail = body_to_tail(next);

    set_board_at(state, newRow, newColumn, new_tail);
    snake->tail_row = newRow;
    snake->tail_col = newColumn;
}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t*)) {

    for (unsigned int i = 0; i < state->num_snakes; i++) {
        if (!state->snakes[i].live) continue;
        char next = next_square(state, i);
        if (next == '#' || is_snake(next)) {
            state->snakes[i].live = false;
            set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
            continue;
        }
        if (next == '*') {
            update_head(state, i);
            add_food(state);
            continue;
        }
        update_head(state, i);
        update_tail(state, i);
    }
}

/**
 * Tarea 5: función extra que lee líneas largas, no utiliza getline.
 */
static char* read_really_long_line(FILE* file) {

    size_t buffsize = 128;
    size_t length = 0;
    char* buffer = malloc(buffsize);

    if (!buffer) return NULL;
    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {
        if (length + 1 >= buffsize) {
          buffsize *= 2;
            char* temporary = realloc(buffer, buffsize);
            if (!temporary) {
                free(buffer);
                return NULL;
            }
            buffer = temporary;
        }
        buffer[length++] = (char)c;
    }
    if (c == EOF && length == 0) {
        free(buffer);
        return NULL;
    }
    buffer[length] = '\0';
    if (length > 0 && buffer[length - 1] == '\r') {
        buffer[length - 1] = '\0';
    }
    return buffer;
}

/* Tarea 5: carga el tablero de archivo usando read_really_long_line */
game_state_t* load_board(char* filename) {

    FILE* archivo = fopen(filename, "r");
    if (!archivo) return NULL;
    char** temp_board = NULL;
    unsigned int num_rows = 0;
    char* line;


    while ((line = read_really_long_line(archivo)) != NULL) {
        char** temporary = realloc(temp_board, sizeof(char*) * (num_rows + 1));
        if (!temporary) {
            free(line);
            for (unsigned int i = 0; i < num_rows; i++) free(temp_board[i]);
            free(temp_board);
            fclose(archivo);
            return NULL;
        }


        temp_board = temporary;
        temp_board[num_rows++] = line;
    }
    fclose(archivo);
    game_state_t* state = malloc(sizeof(game_state_t));
    if (!state) {
        for (unsigned int i = 0; i < num_rows; i++) free(temp_board[i]);
        free(temp_board);
        return NULL;
    }
    state->num_rows = num_rows;

    state->board = temp_board;

    state->num_snakes = 0;

    state->snakes = NULL;
    return state;
}

/* Tarea 6.1 */
static void find_head(game_state_t* state, unsigned int snum) {

    snake_t* snake = &state->snakes[snum];
    unsigned int row = snake->tail_row;

    unsigned int column = snake->tail_col;
    char current = get_board_at(state, row, column);

    while (!is_head(current)) {
        row = get_next_row(row, current);
        column = get_next_col(column, current);
        current = get_board_at(state, row, column);
    }

    snake->head_row = row;
    snake->head_col = column;
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {

    unsigned int count = 0;

    for (unsigned int i = 0; i < state->num_rows; i++) {
        for (unsigned int j = 0; state->board[i][j]; j++) {
            if (is_tail(state->board[i][j])) {
                count++;
            }
        }
    }

    state->num_snakes = count;
    if (count == 0) {
        state->snakes = NULL;
        return state;
    }

    state->snakes = malloc(count * sizeof *state->snakes);
    if (!state->snakes) {
        state->num_snakes = 0;
        return state;
    }

    unsigned int indice = 0;
    for (unsigned int i = 0; i < state->num_rows; i++) {
        for (unsigned int j = 0; state->board[i][j]; j++) {
            if (is_tail(state->board[i][j])) {
                snake_t* snake = &state->snakes[indice];
                snake->tail_row = i;
                snake->tail_col = j;
                snake->live     = true;
                find_head(state, indice);
                indice++;
            }
        }
    }

    return state;
}

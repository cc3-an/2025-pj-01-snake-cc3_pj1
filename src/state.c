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

/* Tarea 1 */
game_state_t* create_default_state() {

    game_state_t *state = malloc(sizeof(game_state_t));

    state->num_rows = 18;

    state->board = malloc(sizeof(char *) * state->num_rows);

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

    int i = 0;
    while (i < state->num_rows) {

        size_t len = strlen(default_board[i]);

        state->board[i] = malloc(len + 1);

        strcpy(state->board[i], default_board[i]);

        i++;
    }

    state->num_snakes = 1;
    state->snakes = malloc(sizeof(snake_t));

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
  return;
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
    if ( fp == NULL ||state == NULL ) {
      return;
  }

  unsigned int i = 0;
  while (i < state->num_rows) {
      fprintf(fp, "%s\n", state->board[i]);
      i++;
  }
  
}


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* file = fopen(filename, "w");
  print_board(state, file);
  fclose(file);
}

/* Tarea 4.1 */


/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  return c == 'w' || c == 's' || c == 'd'|| c == 'a';

}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';

}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return is_tail(c)|| c == '^' || c == '<' || c == 'v' || c == '>' || is_head(c) ;
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^') return 'w';

  else if (c == 'v') return 's';

  else if (c == '>') return 'd';

  else if (c == '<') return 'a';

  else return '?';
  
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  switch (c) {

    case 'S': return 'v';

    case 'D': return '>';

    case 'A': return '<';

    case 'W': return '^';
    default: return '?';
  }
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == '^' || c == 'W' || c == 'w' ) return cur_row - 1;

  if (c == 's'|| c == 'v'  || c == 'S') return cur_row + 1;

  return cur_row;
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == 'a'  || c == '<' || c == 'A') return cur_col - 1;

  if (c == '>' || c == 'D' || c == 'd') return cur_col + 1;

  return cur_col;
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
static char next_square(game_state_t* state, unsigned int snum) {

  snake_t snake = state->snakes[snum];

  char head = get_board_at(state, snake.head_row, snake.head_col);

  unsigned int next_row = get_next_row(snake.head_row, head);

  unsigned int next_col = get_next_col(snake.head_col, head);

  return get_board_at(state, next_row, next_col);
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  snake_t* snake = &state->snakes[snum];

  char head = get_board_at(state, snake->head_row, snake->head_col);

  char body = head_to_body(head);

  set_board_at(state, snake->head_row, snake->head_col, body);

  unsigned int new_row = get_next_row(snake->head_row, head);

  unsigned int new_column = get_next_col(snake->head_col, head);

  set_board_at(state, new_row, new_column, head);

  snake->head_row = new_row;
  
  snake->head_col = new_column;
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  snake_t* snake = &state->snakes[snum];

  unsigned int row = snake->tail_row;
  unsigned int col = snake->tail_col;

  char tail = get_board_at(state, row, col);

  set_board_at(state, row, col, ' ');

  unsigned int next_row = get_next_row(row, tail);
  unsigned int next_col = get_next_col(col, tail);

  char next_char = get_board_at(state, next_row, next_col);
  char new_tail = body_to_tail(next_char);

  set_board_at(state, next_row, next_col, new_tail);

  snake->tail_row = next_row;
  snake->tail_col = next_col;
}




/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
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



/* Tarea 5 */
game_state_t* load_board(char* filename) {
  FILE* archivo = fopen(filename, "r");
  if (!archivo) return NULL;

  char buffer[1024];
  unsigned int num_rows = 0;
  char** temp_board = NULL;

  while (fgets(buffer, sizeof(buffer), archivo)) {
      size_t len = strlen(buffer);

      if (len > 0 && buffer[len - 1] == '\n') {
          buffer[len - 1] = '\0';
          len--;
      }

      temp_board = realloc(temp_board, sizeof(char*) * (num_rows + 1));
      temp_board[num_rows] = malloc(len + 1);
      strcpy(temp_board[num_rows], buffer);
      num_rows++;
  }

  fclose(archivo);

  game_state_t* state = malloc(sizeof(game_state_t));
  state->num_rows = num_rows;
  state->board = temp_board;
  state->num_snakes = 0;
  state->snakes = NULL;

  return state;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) {
  snake_t* snake = &state->snakes[snum];
  unsigned int row = snake->tail_row;
  unsigned int col = snake->tail_col;
  char current = get_board_at(state, row, col);

  while (!is_head(current)) {
      row = get_next_row(row, current);
      col = get_next_col(col, current);
      current = get_board_at(state, row, col);
  }

  snake->head_row = row;
  snake->head_col = col;
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  unsigned int capacidad = 4;
  state->snakes = malloc(sizeof(snake_t) * capacidad);
  state->num_snakes = 0;

  for (unsigned int i = 0; i < state->num_rows; i++) {
      for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
          if (is_tail(state->board[i][j])) {
              // Expandir capacidad si es necesario
              if (state->num_snakes >= capacidad) {
                  capacidad *= 2;
                  state->snakes = realloc(state->snakes, sizeof(snake_t) * capacidad);
              }

              snake_t* snake = &state->snakes[state->num_snakes];
              snake->tail_row = i;
              snake->tail_col = j;
              snake->live = true;

              find_head(state, state->num_snakes);

              state->num_snakes++;
          }
      }
  }

  return state;
}

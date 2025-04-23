#include <stdio.h>
#include <string.h>

#include "snake_utils.h"
#include "state.h"

int main(int argc, char* argv[]) {
  char* in_filename = NULL;
  char* out_filename = NULL;
  game_state_t* state = NULL;

  // Parsea los argumentos recibidos
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i < argc - 1) {
      in_filename = argv[i + 1];
      i++;
      continue;
    }
    if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
      out_filename = argv[i + 1];
      i++;
      continue;
    }
    fprintf(stderr, "Usage: %s [-i filename] [-o filename]\n", argv[0]);
    return 1;
  }

  // NO MODIFIQUEN NADA ARRIBA DE ESTA LINEA.

  /* Tarea 7 */

  // Leer el tablero de un archivo, o crear un tablero por defecto.
  if (in_filename != NULL) {
    // TODO: cargar el tablero de in_filename
    // TODO: Si el archivo no existe, retornar -1
    // TODO: Despues llamar a initialize_snakes en el estado creado
    state = load_board(in_filename);
    if (state == NULL) {
      fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", in_filename);
      return -1;
    }
    state = initialize_snakes(state);
  } else {
    // TODO: Cargar el estado por defecto.
    state = create_default_state();
  }

  // TODO: Actualizar el estado. Utilizar la funcion deterministic_food
  // (esta ya ha sido creada en snakes_utils.h) para agregar comida al
  // tablero)
  update_state(state, deterministic_food);
  if (out_filename != NULL) {
    save_board(state, out_filename);
  } else {
    print_board(state, stdout);
  }

  free_state(state);
  return 0;
}

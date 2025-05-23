#include "tdas/extra.h"
#include "tdas/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_ESCENARIOS 100

// Estructura para representar un ítem
typedef struct {
    char nombre[50];
    int valor;
    int peso;
} Item;

// Estructura para representar un escenario del grafo
typedef struct Escenario {
    int id;
    char nombre[50];
    char descripcion[100];
    Item* items[10];
    int numItems;
    int conexiones[4];        // 0: arriba, 1: abajo, 2: izquierda, 3: derecha
    int es_final;
    struct Escenario* vecinos[4]; // punteros reales a escenarios conectados
} Escenario;

typedef struct {
    Escenario* escenarioActual;
    List* inventario;
    int tiempoRestante;
    int puntajeTotal;
} Jugador;


// Lista global de escenarios
Escenario* escenarios[MAX_ESCENARIOS];
int numEscenarios = 0;

/**
 * Carga escenarios desde un archivo CSV y construye el grafo
 */
void leer_escenarios() {
    FILE* archivo = fopen("data/graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    char** campos;

    // Leer encabezado
    campos = leer_linea_csv(archivo, ',');

    // Leer cada línea
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Escenario* e = malloc(sizeof(Escenario));
        e->id = atoi(campos[0]);
        strcpy(e->nombre, campos[1]);
        strcpy(e->descripcion, campos[2]);
        e->numItems = 0;
        e->es_final = atoi(campos[8]);

        // Leer ítems
        List* items = split_string(campos[3], ";");
        for (char* item = list_first(items); item != NULL; item = list_next(items)) {
            List* valores = split_string(item, ",");
            Item* it = malloc(sizeof(Item));
            strcpy(it->nombre, list_first(valores));
            it->valor = atoi(list_next(valores));
            it->peso = atoi(list_next(valores));
            e->items[e->numItems++] = it;

            list_clean(valores);
            free(valores);
        }
        list_clean(items);
        free(items);

        // Leer conexiones por ID
        e->conexiones[0] = atoi(campos[4]); // arriba
        e->conexiones[1] = atoi(campos[5]); // abajo
        e->conexiones[2] = atoi(campos[6]); // izquierda
        e->conexiones[3] = atoi(campos[7]); // derecha

        escenarios[numEscenarios++] = e;
    }

    fclose(archivo);

    // Enlazar vecinos (punteros reales)
    for (int i = 0; i < numEscenarios; i++) {
        Escenario* e = escenarios[i];
        for (int d = 0; d < 4; d++) {
            int idVecino = e->conexiones[d];
            if (idVecino == -1) {
                e->vecinos[d] = NULL;
                continue;
            }

            // Buscar nodo con ese ID
            for (int j = 0; j < numEscenarios; j++) {
                if (escenarios[j]->id == idVecino) {
                    e->vecinos[d] = escenarios[j];
                    break;
                }
            }
        }
    }

    printf("Se cargaron %d escenarios exitosamente.\n", numEscenarios);
}


int calcular_peso_total(List* inventario) {
    int total = 0;
    for (Item* it = list_first(inventario); it != NULL; it = list_next(inventario)) {
        total += it->peso;
    }
    return total;
}


 
void jugar(Jugador* jugador) {
    while (jugador->tiempoRestante > 0) {
        Escenario* e = jugador->escenarioActual;

        printf("\n=== %s ===\n", e->nombre);
        printf("%s\n", e->descripcion);
        printf("Tiempo restante: %d\n", jugador->tiempoRestante);
        printf("Inventario (total %d kg, %d pts):\n", 
            calcular_peso_total(jugador->inventario),
            jugador->puntajeTotal);

        for (Item* it = list_first(jugador->inventario); it != NULL; it = list_next(jugador->inventario)) {
            printf("  - %s (%d pts, %d kg)\n", it->nombre, it->valor, it->peso);
        }

        printf("Items disponibles en este escenario:\n");
        for (int i = 0; i < e->numItems; i++) {
            printf("  %d. %s (%d pts, %d kg)\n", i + 1, e->items[i]->nombre, e->items[i]->valor, e->items[i]->peso);
        }

        printf("\nAcciones disponibles:\n");
        printf("1. Recoger items\n");
        printf("2. Descartar items\n");
        printf("3. Avanzar\n");
        printf("4. Reiniciar\n");
        printf("5. Salir\n");

        printf("Seleccione una opcion: ");
        int opcion;
        scanf("%d", &opcion);
        if (opcion == 1) {
            printf("Item a recoger (numero): ");
            int idx;
            scanf("%d", &idx);
            if (idx > 0 && idx <= e->numItems) {
                list_pushBack(jugador->inventario, e->items[idx - 1]);
                jugador->puntajeTotal += e->items[idx - 1]->valor;

                // Eliminar el ítem del escenario
                for (int j = idx - 1; j < e->numItems - 1; j++) {
                    e->items[j] = e->items[j + 1];
                }
                e->numItems--;
                jugador->tiempoRestante -= 1;
            }

        } else if (opcion == 2) {
            printf("Item a descartar (nombre): ");
            char nombre[50];
            scanf(" %[^\n]s", nombre);

            Item* it = list_first(jugador->inventario);
            while (it) {
                if (strcmp(it->nombre, nombre) == 0) {
                    jugador->puntajeTotal -= it->valor;
                    list_popCurrent(jugador->inventario);
                    jugador->tiempoRestante -= 1;
                    break;
                }
                it = list_next(jugador->inventario);
            }

        } else if (opcion == 3) {
            printf("Direcciones disponibles:\n");
            char* dirs[] = {"Arriba", "Abajo", "Izquierda", "Derecha"};
            for (int i = 0; i < 4; i++) {
                if (e->vecinos[i]) {
                    printf("%d. %s -> %s\n", i + 1, dirs[i], e->vecinos[i]->nombre);
                }
            }
            printf("Ingrese direccion (1-4): ");
            int dir;
            scanf("%d", &dir);
            dir--;

            if (e->vecinos[dir]) {
                jugador->escenarioActual = e->vecinos[dir];
                int peso = calcular_peso_total(jugador->inventario);
                int tiempo = ceil((peso + 1) / 10.0);
                jugador->tiempoRestante -= tiempo;

                if (jugador->escenarioActual->es_final) {
                    printf("¡Has llegado al escenario final!\n");
                    printf("Puntaje final: %d\n", jugador->puntajeTotal);
                    return;
                }
            }

        } else if (opcion == 4) {
            jugador->escenarioActual = escenarios[0];
            jugador->tiempoRestante = 10;
            jugador->puntajeTotal = 0;
            list_clean(jugador->inventario);
            printf("Juego reiniciado.\n");

        } else if (opcion == 5) {
            printf("Saliendo del juego.\n");
            return;
        } else {
            printf("Opcion invalida.\n");
      }
    }

    printf("¡Se acabo el tiempo! Puntaje final: %d\n", jugador->puntajeTotal);
}



// Mostrar todos los escenarios cargados (opcional para depuración)
void mostrar_escenarios() {
    for (int i = 0; i < numEscenarios; i++) {
        Escenario* e = escenarios[i];
        printf("ID: %d\n", e->id);
        printf("Nombre: %s\n", e->nombre);
        printf("Descripcion: %s\n", e->descripcion);
        printf("Items:\n");
        for (int j = 0; j < e->numItems; j++) {
            printf("  - %s (%d pts, %d kg)\n", e->items[j]->nombre, e->items[j]->valor, e->items[j]->peso);
        }

        char* dirs[4] = {"Arriba", "Abajo", "Izquierda", "Derecha"};
        for (int d = 0; d < 4; d++) {
            if (e->vecinos[d]) {
                printf("%s: %s (ID %d)\n", dirs[d], e->vecinos[d]->nombre, e->vecinos[d]->id);
            }
        }

        if (e->es_final)
            printf("Escenario Final: Sí\n");
        printf("-----------------------------\n");
    }
}

int main() {
    leer_escenarios();
    mostrar_escenarios(); // Solo para verificar por ahora
    Jugador jugador;
    jugador.escenarioActual = escenarios[0]; // escenario inicial
    jugador.tiempoRestante = 10;
    jugador.puntajeTotal = 0;
    jugador.inventario = list_create();

    jugar(&jugador);
    return 0;
}

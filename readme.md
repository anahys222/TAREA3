
# AUTOR
Anahys Vera


# GraphQuest

GraphQuest es un juego de aventuras en C basado en un sistema de escenarios interconectados representado como un grafo. El jugador debe recolectar ítems, moverse estratégicamente y gestionar su tiempo para alcanzar el escenario final con el mayor puntaje posible.

---

## 🛠️ Cómo compilar y ejecutar
Para compilar y ejecutar el juego primero debemos compilar:
````
gcc tdas/*.c tarea3.c -Wno-unused-result -o tarea3

Y luego ejecutar:
````
./tarea3


### Requisitos:
- GCC o cualquier compilador compatible con C


### Ejemplo de uso:
Se cargaron 5 escenarios exitosamente.

=== Bosque Encantado ===
Un bosque lleno de misterios y criaturas mágicas.
Tiempo restante: 10
Inventario (total 0 kg, 0 pts):

Ítems disponibles en este escenario:
  1. Varita mágica (30 pts, 3 kg)
  2. Pocion curativa (10 pts, 1 kg)

Acciones disponibles:
1. Recoger ítems
2. Descartar ítems
3. Avanzar
4. Reiniciar
5. Salir
Seleccione una opción: 1
Ítem a recoger (número): 1

...

¡Has llegado al escenario final!
Puntaje final: 110

## 🚫 Funcionalidades no implementadas o limitaciones

- No se guarda el progreso del jugador ni se permite cargar partidas.
- No se validan completamente todos los errores posibles del archivo CSV.
- El juego es para un solo jugador y no tiene niveles de dificultad.

---

## 🤝 Contribuciones

Este proyecto fue desarrollado íntegramente por **Anahys Vera**.


/* 
git add .
git commit -m "."
git push
*/


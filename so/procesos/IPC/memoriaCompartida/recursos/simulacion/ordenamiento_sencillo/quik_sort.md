# Algoritmo de ordenamiento Quik sort

Este algoritmo divide y venceras:

1. Tomamos un pivote
2. acomodamos los elementos pequeños a la izquierda
3. acomodamos los elemento grandes a la derecha del pivote
4. Se usa la recursion en en las parciciones.

```none
ejemplo

5 3 7 6 8 4 9 6
              |
tomamos de pivote el ultimo elemento

i = -1

5 3 7 6 8 4 9 6
|                    [5<6] ok i = 0

5 3 7 6 8 4 9 6
  |                  [3 < 6] ok i = 1

5 3 7 6 8 4 9 6
    |                [7 < 6] no "marcamos" i = 2


5 3 7 6 8 4 9 6
    * |               [6 < 6 ] no , i = 3


5 3 7 6 8 4 9 6
   [*]* |           [8 < 6] no , i = 4

5 3 7 6 8 4 9 6
    * * * |         [4 < 6] ok , i = 5  "swap"

5 3 4 6 8 7 9 6
      * * * |       [9 < 6] no

5 3 4 6 8 7 9 6
      * * *   |    end swap al primer *

5 3 4 6 8 7 9 6
      |

ordenar(arr, inicio , pivote - 1);    ordenar(arr , pivote + 1 , fin );
5 3 4                                   8 7 9 6

...                                     6 8 7 9

3 5 4
3 4 5






```

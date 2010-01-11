#include <basic.inc>

#define DRUM(volumen, adsr_num, tiempo,  frecuencia) \
        vol   volumen     :                          \
        adsr  adsr_num    :                          \
        times tiempo      :                          \
        drums frecuencia  :


#define PLATO(fr)                                    \
        DRUM(0, 3, 8, fr)  :                         \
        DRUM(8, 12, 8, fr) :                         \
        DRUM(5, 5, 5, fr)  :

#define PRUEBA         T 100 O(octava) V(7) L(32) C# D& E F G A B


var volumen, octava, duracion

duracion = 16





channel psg1
        volumen = 4
        octava = 3
        loop 0
        PRUEBA
        endl
        endc


channel psg2
        volumen = 3
        octava = 6
        loop 0                  ;Lo necesita el driver
        T 100 O4 V7 L32 C# D& E F G A B
        loop 0
        PRUEBA
        endl
        endc

channel psg3
        loop 0
        PLATO(0)   L2 R
        PLATO(2)   L2 R
        PLATO(4)   L2 R
        PLATO(8)   L2 R
        PLATO(16)  L2 R
        PLATO(31)  L2 R
        endl
        endc

write

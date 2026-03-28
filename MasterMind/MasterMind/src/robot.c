#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "robot.h"

int est_chiffre_valide(int chiffre) {
    return (chiffre >= 0 && chiffre <= 9);
}

void generer_code_robot(int code[], int niveau) {
    
    static int initialise = 0;
    if (!initialise) {
        srand((unsigned int)time(NULL));
        initialise = 1;
    }
    
    if (niveau == 1) {
        int chiffres_disponibles[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int nb_disponibles = 10;
        
        for (int i = 0; i < 5; i++) {
            int index = rand() % nb_disponibles;
            code[i] = chiffres_disponibles[index];
            
            for (int j = index; j < nb_disponibles - 1; j++) {
                chiffres_disponibles[j] = chiffres_disponibles[j + 1];
            }
            nb_disponibles--;
        }
        
    } else {
        for (int i = 0; i < 5; i++) {
            code[i] = rand() % 10;
        }
    }
    
    printf("[DEBUG] Code robot: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", code[i]);
    }
    printf("\n");
}


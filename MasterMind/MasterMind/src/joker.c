#include <stdio.h>
#include "joker.h"
#include "joueur.h"

void utiliser_joker(Joueur *joueur, int code_secret[]) {
    int choix;
    
    printf("\n=== MENU JOKER ===\n");
    printf("1. Voir un chiffre (-3 essais)\n");
    printf("2. Exclure un chiffre (-5 essais)\n");
    printf("Votre choix: ");
    scanf("%d", &choix);
    
    if(choix == 1) {
        int position;
        printf("Position à voir (1-%d): ", CODE_LENGTH);
        scanf("%d", &position);
        
        if(position >= 1 && position <= CODE_LENGTH) {
            printf("Position %d: %d\n", position, code_secret[position-1]);
            joueur->score += 3;
            printf("+3 essais de pénalité!\n");
        } else {
            printf("Position invalide!\n");
            return;
        }
        
    } else if(choix == 2) {
        int chiffre;
        printf("Chiffre à vérifier (0-9): ");
        scanf("%d", &chiffre);
        
        if(chiffre < 0 || chiffre > 9) {
            printf("Chiffre invalide!\n");
            return;
        }
        
        int trouve = 0;
        for(int i = 0; i < CODE_LENGTH; i++) {
            if(code_secret[i] == chiffre) {
                trouve = 1;
                break;
            }
        }
        
        if(trouve) {
            printf("Le chiffre %d EST dans le code\n", chiffre);
        } else {
            printf("Le chiffre %d N'EST PAS dans le code\n", chiffre);
        }
        
        joueur->score += 5;
        printf("+5 essais de pénalité!\n");
        
    } else {
        printf("Choix invalide!\n");
        return;
    }
    
    joueur->joker_utilise = 1;
    printf("Joker utilisé! Nouveau total: %d essais\n", joueur->score);
}

int proposer_joker(Joueur *joueur, int code_secret[]) {
    if(joueur->joker_utilise) {
        return 0;
    }
    
    if(joueur->score >= 10) {
        printf("\n--- JOKER DISPONIBLE ---\n");
        printf("Voulez-vous utiliser un joker? (1=Oui, 0=Non): ");
        
        int reponse;
        scanf("%d", &reponse);
        
        if(reponse == 1) {
            utiliser_joker(joueur, code_secret);
            return 1;
        }
    }
    
    return 0; 
}


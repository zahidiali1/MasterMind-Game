#include <stdio.h>
#include <string.h>
#include "joueur.h"


void creer_joueur(Joueur *j, char *nom) {
    strcpy(j->nom, nom);
    j->score = 0;
    j->niveau = 1;
    j->joker_utilise = 0;
    
    for(int i = 0; i < CODE_LENGTH; i++) {
        j->code_secret[i] = -1;
    }
    
    printf("Joueur %s créé.\n", j->nom);
}

void afficher_joueur(Joueur j) {
    printf("\n=== JOUEUR ===\n");
    printf("Nom: %s\n", j.nom);
    printf("Score: %d essais\n", j.score);
    printf("Niveau: %s\n", j.niveau == 1 ? "Débutant" : "Expert");
    printf("Joker utilisé: %s\n", j.joker_utilise ? "Oui" : "Non");
    
    if(j.code_secret[0] != -1) {
        printf("Code secret: ");
        for(int i = 0; i < CODE_LENGTH; i++) {
            printf("%d ", j.code_secret[i]);
        }
        printf("\n");
    }
}

void saisir_code(Joueur *j, int niveau) {
    j->niveau = niveau;
    
    printf("\n%s, entrez le code secret (%d chiffres):\n", j->nom, CODE_LENGTH);
    printf("Niveau: %s\n", niveau == 1 ? "DEBUTANT (chiffres différents)" : "EXPERT");
    
    for(int i = 0; i < CODE_LENGTH; i++) {
        int valide = 0;
        
        while(!valide) {
            printf("Chiffre %d: ", i+1);
            scanf("%d", &j->code_secret[i]);
            
            if(j->code_secret[i] < 0 || j->code_secret[i] > 9) {
                printf("ERREUR: 0-9 seulement!\n");
                continue;
            }
            
            if(niveau == 1) {
                int doublon = 0;
                for(int k = 0; k < i; k++) {
                    if(j->code_secret[k] == j->code_secret[i]) {
                        doublon = 1;
                        printf("ERREUR: Chiffre déjà utilisé!\n");
                        break;
                    }
                }
                if(doublon) continue;
            }
            
            valide = 1;
        }
    }
    
    printf("Code secret enregistré!\n");
}

int verifier_code(Joueur j, int code[]) {
    int bien = 0, mal = 0;
    
    int secret_utilise[CODE_LENGTH] = {0};
    int code_utilise[CODE_LENGTH] = {0};
    
    for(int i = 0; i < CODE_LENGTH; i++) {
        if(code[i] == j.code_secret[i]) {
            bien++;
            secret_utilise[i] = 1;
            code_utilise[i] = 1;
        }
    }
    
    for(int i = 0; i < CODE_LENGTH; i++) {
        if(!code_utilise[i]) {
            for(int k = 0; k < CODE_LENGTH; k++) {
                if(!secret_utilise[k] && code[i] == j.code_secret[k]) {
                    mal++;
                    secret_utilise[k] = 1;
                    break;
                }
            }
        }
    }
    
    printf("Résultat: %d bien placé(s), %d mal placé(s)\n", bien, mal);
    return (bien == CODE_LENGTH) ? 1 : 0;
}

int tentative_valide(int tentative[]) {
    for (int i = 0; i < CODE_LENGTH; i++) {
        for (int j = i + 1; j < CODE_LENGTH; j++) {
            if (tentative[i] == tentative[j]) {
                return 0;
            }
        }
    }
    return 1;
}

void saisir_tentative_sans_doublons(int tentative[]) {
    int valide;
    
    do {
        valide = 1;
        printf("Entrez votre code (%d chiffres DIFFÉRENTS):\n", CODE_LENGTH);
        
        for (int i = 0; i < CODE_LENGTH; i++) {
            printf("Chiffre %d: ", i + 1);
            scanf("%d", &tentative[i]);
            
            while (tentative[i] < 0 || tentative[i] > 9) {
                printf("ERREUR: 0-9 seulement! Réessayez: ");
                scanf("%d", &tentative[i]);
            }
        }
        
        if (!tentative_valide(tentative)) {
            printf("ERREUR: Les chiffres doivent être tous différents!\n");
            printf("Veuillez réessayer.\n");
            valide = 0;
        }
        
    } while (!valide);
}

void saisir_tentative(int tentative[CODE_LENGTH]) {
    printf("Entrez votre tentative (chiffres de 0 à 9, répétitions autorisées):\n");
    
    for (int i = 0; i < CODE_LENGTH; i++) {
        do {
            printf("Chiffre %d: ", i + 1);
            scanf("%d", &tentative[i]);
            
            if (tentative[i] < 0 || tentative[i] > 9) {
                printf("Erreur: entrez un chiffre entre 0 et 9!\n");
            }
        } while (tentative[i] < 0 || tentative[i] > 9);
    }
    
    printf("Vous avez saisi: ");
    for (int i = 0; i < CODE_LENGTH; i++) {
        printf("%d ", tentative[i]);
    }
    printf("\n");
}


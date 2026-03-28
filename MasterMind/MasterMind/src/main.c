#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "joueur.h"
#include "score.h"
#include "joker.h"
#include "robot.h"

void afficher_menu(void);
void jouer_partie_amicale(void);
void jouer_partie_robot(void);

int main(void) {
    int choix;
    
    do {
        afficher_menu();
        scanf("%d", &choix);
        
        switch(choix) {
            case 1:
                jouer_partie_amicale();
                break;
            case 2:
                jouer_partie_robot();
                break;
            case 3:
                afficher_meilleurs_scores();
                printf("\nAppuyez sur Entrée...");
                getchar(); getchar();
                break;
            case 4:
                printf("\nMerci d'avoir joué! Au revoir!\n");
                break;
            default:
                printf("Choix invalide!\n");
        }
        
        system("clear");
        
    } while(choix != 4);
    
    return 0;
}

void afficher_menu(void) {
    printf("╔══════════════════════════════╗\n");
    printf("║      MASTERMIND              ║\n");
    printf("╠══════════════════════════════╣\n");
    printf("║ 1. Nouvelle partie (Amicale) ║\n");
    printf("║ 2. Nouvelle partie (VS Robot)║\n");
    printf("║ 3. Meilleurs scores          ║\n");
    printf("║ 4. Quitter                   ║\n");
    printf("╚══════════════════════════════╝\n");
    printf("Votre choix: ");
}

void jouer_partie_amicale(void) {
    Joueur joueur1, joueur2;
    char nom1[50], nom2[50];
    int niveau;
    
    printf("\nJOUEUR 1 (crée le code), entrez votre nom : ");
    scanf("%s", nom1);
    creer_joueur(&joueur1, nom1);
    
    printf("JOUEUR 2 (devine le code), entrez votre nom : ");
    scanf("%s", nom2);
    creer_joueur(&joueur2, nom2);
    
    printf("\nChoisissez le niveau:\n");
    printf("1. Niveau 1 (chiffres différents)\n");
    printf("2. Niveau 2 (chiffres peuvent se répéter)\n");
    printf("Votre choix: ");
    scanf("%d", &niveau);
    
    system("clear");
    printf("=== %s, entrez le code secret ===\n", joueur1.nom);
    saisir_code(&joueur1, niveau);
    
    printf("\nAppuyez sur Entrée pour cacher l'écran...");
    getchar();
    getchar();
    system("clear");
    
    printf("=== %s, devinez le code ===\n", joueur2.nom);
    printf("Vous avez %d essais maximum.\n", MAX_TENTATIVES);
    
    int gagne = 0;
    joueur2.score = 0;
    
    while(joueur2.score < MAX_TENTATIVES && !gagne) {
            joueur2.score++;
            
            printf("\n--- Essai %d/%d ---\n", joueur2.score, MAX_TENTATIVES);
            
            if(joueur2.score >= 10 && joueur2.joker_utilise == 0) {
                int choix;
                printf("Voulez-vous utiliser un joker? (1=Oui/2=Non) : ");
                scanf(" %d", &choix);
                
                if(choix == 1 ) {
                    utiliser_joker(&joueur2, joueur1.code_secret);
                }
            }
            
            int tentative[CODE_LENGTH];
        
        if (niveau == 1) {
            saisir_tentative_sans_doublons(tentative);
        } else {
            saisir_tentative(tentative);
        }
            gagne = verifier_code(joueur1, tentative);
        
        if(gagne) {
            printf("\n FÉLICITATIONS %s! \n", joueur2.nom);
            printf("Vous avez trouvé en %d essais!\n", joueur2.score);
            
            enregistrer_score(joueur2, niveau, 0);
            
            int sauvegarder;
            printf("\nVoulez-vous sauvegarder votre score? (1=Oui/2=Non) : ");
            scanf(" %d", &sauvegarder);
                        
            if(sauvegarder == 1) {
                enregistrer_score(joueur2, niveau, 0);
                printf("Score sauvegardé avec succès!\n");
            } else {
                printf("Score non sauvegardé.\n");
            }
        } else {
            printf("Essais restants: %d\n", MAX_TENTATIVES - joueur2.score);
        }
    }
    
    if(!gagne) {
        printf("\nDÉSOLÉ %s! Vous avez perdu.\n", joueur2.nom);
        printf("Le code secret était: ");
        for(int i = 0; i < CODE_LENGTH; i++) {
            printf("%d ", joueur1.code_secret[i]);
        }
        printf("\n");
    }
    
    printf("\nAppuyez sur Entrée pour continuer...");
    getchar();
    getchar();
}

void jouer_partie_robot(void) {
    Joueur joueur_humain, joueur_robot;
    char nom_humain[50];
    int niveau;
    
    printf("\n=== MODE JOUEUR vs ROBOT ===\n");
    
    printf("Entrez votre nom : ");
    scanf("%s", nom_humain);
    creer_joueur(&joueur_humain, nom_humain);
    joueur_humain.joker_utilise = 0;
    
    strcpy(joueur_robot.nom, "ROBOT");
    joueur_robot.score = 0;
    
    printf("\nChoisissez le niveau:\n");
    printf("1. Niveau 1 (chiffres différents)\n");
    printf("2. Niveau 2 (chiffres peuvent se répéter)\n");
    printf("Votre choix: ");
    scanf("%d", &niveau);
    
    int code_robot[CODE_LENGTH];
    generer_code_robot(code_robot, niveau);
    
    for(int i = 0; i < CODE_LENGTH; i++) {
        joueur_robot.code_secret[i] = code_robot[i];
    }
    
    system("clear");
    printf("=== %s vs ROBOT ===\n", joueur_humain.nom);
    printf("Le robot a généré un code secret!\n");
    printf("Vous avez %d essais maximum.\n", MAX_TENTATIVES);
    
    int gagne = 0;
    joueur_humain.score = 0;
    
    while(joueur_humain.score < MAX_TENTATIVES && !gagne) {
            joueur_humain.score++;
            
            printf("\n--- Essai %d/%d ---\n", joueur_humain.score, MAX_TENTATIVES);
            
            if(joueur_humain.score >= 10 && joueur_humain.joker_utilise == 0) {
                int choix;
                printf("Voulez-vous utiliser un joker? (1=Oui/2=Non) : ");
                scanf(" %d", &choix);
                
                if(choix == 1) {
                    utiliser_joker(&joueur_humain, joueur_robot.code_secret);
                }
            }
            
            int tentative[CODE_LENGTH];
            if (niveau == 1) {
                saisir_tentative_sans_doublons(tentative);
            } else {
                saisir_tentative(tentative);
            }
        
            gagne = verifier_code(joueur_robot, tentative);
            
        
        if(gagne) {
            printf("\n FÉLICITATIONS %s! \n", joueur_humain.nom);
            printf("Vous avez battu le robot en %d essais!\n", joueur_humain.score);
            
            int sauvegarder;
            printf("\nVoulez-vous sauvegarder votre score? (1=Oui/0=Non) : ");
            scanf(" %d", &sauvegarder);
                        
            if(sauvegarder == 1) {
                enregistrer_score(joueur_humain, niveau, 1);
                printf("Score sauvegardé avec succès!\n");
            } else {
                printf("Score non sauvegardé.\n");
            }
            
        } else {
            printf("Essais restants: %d\n", MAX_TENTATIVES - joueur_humain.score);
        }
    }
    
    if(!gagne) {
        printf("\nDÉSOLÉ %s! Vous avez perdu.\n", joueur_humain.nom);
        printf("Le code secret était: ");
        for(int i = 0; i < CODE_LENGTH; i++) {
            printf("%d ", joueur_robot.code_secret[i]);
        }
        printf("\n");
    }
    
    printf("\nAppuyez sur Entrée pour continuer...");
    getchar();
    getchar();
}

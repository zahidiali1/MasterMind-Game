#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "score.h"
#include "joueur.h"


#ifdef __APPLE__
    #define SCORES_FILE "../mastermind_scores.txt"
#else
    #define SCORES_FILE "../data/scores.txt"
#endif

void enregistrer_score(Joueur joueur, int niveau, int contre_robot) {
    FILE *fichier = fopen(SCORES_FILE, "a"); 
    
    if (!fichier) {
        printf("Erreur: impossible d'ouvrir %s\n", SCORES_FILE);
        return;
    }
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", t);
    
    fprintf(fichier, "%s %d %d %d %s\n",
            joueur.nom, joueur.score, niveau, contre_robot, date_str);
    
    fclose(fichier);
    printf("Score sauvegardé dans %s\n", SCORES_FILE);
}

void afficher_meilleurs_scores(void) {
    FILE *fichier = fopen(SCORES_FILE, "r");
    
    if(fichier == NULL) {
        printf("\nAucun score enregistré.\n");
        return;
    }
    
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║           MEILLEURS SCORES             ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  Rang  Nom             Essais  Niveau  ║\n");
    printf("╠════════════════════════════════════════╣\n");
    
    Joueur scores[100];
    int nb = 0;
    char nom[50];
    int essais, niveau;
    
    while(fscanf(fichier, "%s %d %d", nom, &essais, &niveau) != EOF) {
        strcpy(scores[nb].nom, nom);
        scores[nb].score = essais;
        scores[nb].niveau = niveau;
        nb++;
    }
    
    fclose(fichier);
    
    for(int i = 0; i < nb-1; i++) {
        for(int j = i+1; j < nb; j++) {
            if(scores[i].score > scores[j].score) {
                Joueur temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
    
    int limite = (nb < 10) ? nb : 10;
    
    for(int i = 0; i < limite; i++) {
        const char *niv = (scores[i].niveau == 1) ? "Déb" : "Exp";
        printf("║  %2d.   %-12s   %2d      %3s   ║\n",
               i+1, scores[i].nom, scores[i].score, niv);
    }
    
    printf("╚══════════════════════════════════════╝\n");
}

void trier_scores(void) {
    
}

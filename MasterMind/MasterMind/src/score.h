#ifndef SCORE_H
#define SCORE_H

#include "joueur.h"

#define MAX_SCORES 100
#define FICHIER_SCORES "scores.txt"

typedef struct {
    Joueur joueurs[MAX_SCORES];
    int nb_scores;
} ListeScores;

void enregistrer_score(Joueur j, int niveau, int contre_robot);
void afficher_meilleurs_scores(void);
void trier_scores(void);

#endif

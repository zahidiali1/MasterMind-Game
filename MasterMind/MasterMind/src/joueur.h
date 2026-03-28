#ifndef JOUEUR_H
#define JOUEUR_H

#define MAX_NOM 50
#define CODE_LENGTH 5
#define MAX_TENTATIVES 20

typedef struct {
    char nom[MAX_NOM];
    int score;
    int niveau;
    int code_secret[CODE_LENGTH];
    int joker_utilise;
} Joueur;

void creer_joueur(Joueur *j, char *nom);
void afficher_joueur(Joueur j);
void saisir_code(Joueur *j, int niveau);
int verifier_code(Joueur j, int code[]);
int tentative_valide(int tentative[]);
void saisir_tentative_sans_doublons(int tentative[]);
void saisir_tentative(int tentative[CODE_LENGTH]);

#endif


#ifndef FONCTIONNES_H
#define FONCTIONNES_H
#include "GAMME.h"
#include "pile.h"
#include <time.h>
#include <stdlib.h>

// demander a l'utilisateur de donner nombre des pieces dans chaque pile
Game DepartDuJeu(int n);
void AfficherKalah();
int MenuPrincipal();
Game HandTake(int j, Game gamme);
//afficher l'etat actuele de jeu
Game Jouer1(int j,Game gamme);
Game Jouer2(int j,Game gamme);
int ChoixPile(int i);
void AfficherGamme(Game gamme);
int Randomly();
int LesPileEstVide(joueur j);
int GameFinish(int i,Game gamme);
void PlayingNewGame(Game gamme);
void ContinuePlaying(Game gamme);
Game Recuperer(int i,Game gamme);
//void Enregistrer(int i, Game gamme);
//Game  Readrecent();
void whowin(Game gamme);
#endif // FONCTIONNES_H

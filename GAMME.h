#ifndef GAMME_H
#define GAMME_H
#include "pile.h"

// definition de jj
typedef struct
{
    pile kalah;
    pile PilesJoueur[6];
}joueur;
typedef struct
{
    joueur Joueur[2];
    pile hand;
}Game;


#endif

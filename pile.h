#ifndef PILE_H
#define PILE_H
#include "pile.h"
#include <SDL2/SDL.h>
#include<unistd.h>
/*
typedef struct _list
{
    int element;
    struct _list *suivant;
}liste;
*/

typedef struct
{
    //liste * T;
    int NombreActuel ;
    int NOMBREMAX;
}pile;

pile PileVide();

pile Empiler(pile p);
int PileEstVide(pile p);
int PileEstPlein(pile p);
pile Depiler(pile p);
pile EmpilerNFois(pile p,int n);

#endif // PILE_H

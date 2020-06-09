#include <stdio.h>
#include <stdlib.h>
#include "fonctionnes.h"

int main(int argc,char *argv[])
{
   
 
  Game gamme;
  
     int choix;
    choix = MenuPrincipal();
    
    switch (choix)
    {
    case 1:
         PlayingNewGame(gamme);
        break;
    case 2 :
         ContinuePlaying(gamme);
    default:
           return EXIT_SUCCESS;
        break;
    }

}

/* dans ce dossier j'ai ecrire plusieur fonctions qui sert a manipuller les pille
don je définit une type abstrait 
empiler : pile,char -> pile
pile vide : void ->pile
pile_est_vide : pile  -> int
depiler : pile -> pile
Piliernfois

*/
#include <stdio.h>
#include <stdlib.h>
#include "./pile.h"


pile PileVide()
{
    pile p;
    p.NOMBREMAX = 121;
    p.NombreActuel = 0;
   // p.T = NULL;
    return p;
}
pile Empiler(pile p)
{ 
     //  int i;
      // liste * t = p.T;
   if( p.NombreActuel >= p.NOMBREMAX )
  {
      
     printf("error la pile est plein  %d %d  \n",p.NombreActuel,p.NOMBREMAX);
     return p;  
       } 
  /*  switch (p.NombreActuel)
    {
    case 0:
      /*  p.T = (liste *) malloc(sizeof(liste));
        if (p.T == NULL)
        {
            printf("error malloc \n");
            SDL_Delay(1200);
            system("exit");
        }
        p.T->element = 0;
        
        break;
    
    default:
        
        for ( i = 0; i <= p.NombreActuel; i++)
        {
            t = t->suivant;
        }
          t = (liste *) malloc(sizeof(liste));
        if (t == NULL)
        {
            printf("error malloc \n");
            SDL_Delay(1200);
            system("exit");
        }
        t->element = 0;
        t->suivant = NULL;
        break;
    }*/
    
              
     p.NombreActuel++;
  return p;
}

int PileEstVide(pile p)
{
    return !(p.NombreActuel) ;
}
int PileEstPlein(pile p)
{
    return !(p.NombreActuel < p.NOMBREMAX) ;
}

pile Depiler(pile p)
{
    
    switch (p.NombreActuel)
    {
    case 0 :    printf("la pile est vide \n");
                return p;
                  break;
    case 1 : p = PileVide();
             break;
    default:
                      p.NombreActuel--;
                      break;
    }                      
  return p;
}
pile EmpilerNFois(pile p,int n)
{
    int i;
    //printf("%d \n",n);
    for(i = 0 ; i < n;i++)
    {
        p = Empiler(p);
    }
    return p;
}

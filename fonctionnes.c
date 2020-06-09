#include<stdio.h>
#include <stdlib.h>
#include "fonctionnes.h"
int DERNIERPILE;
int DERNIERJOUEUR;
Game DepartDuJeu(int n)
{
  Game game;
  int i,j;
    game.hand = PileVide();
    for(i = 0 ; i < 2 ;i++)
    {
         game.Joueur[i].kalah = PileVide();
        for( j = 0; j < 6 ;j++)
        {
          // parcourant les 6 piles en remplace par p a n pieces
         game.Joueur[i].PilesJoueur[j] = PileVide();
         game.Joueur[i].PilesJoueur[j] = EmpilerNFois(game.Joueur[i].PilesJoueur[j] , n);
        }
    }
    return game;

}

void AfficherKalah()
{
  
    printf("\n***********************************************************************************************************\n");
    printf("\nKK	KK	        A         	LL	                    A                 HH       HH   \nKK    KK	      AA AA             LL                        AA AA               HH       HH   \nKK  KK               AA   AA            LL                       AA   AA              HH       HH   \nKKKK                AAAAAAAAA           LL                      AAAAAAAAA             HHHHHHHHHHH   \nKK  KK             AA       AA          LL                     AA       AA            HH       HH   \nKK    KK          AA         AA         LL         LL         AA         AA           HH       HH   \nKK      KK       AA           AA        LLLLLLLLLLLLL	     AA           AA          HH       HH   \n");
    printf("***********************************************************************************************************\n");
    printf("\n");
}

int MenuPrincipal()
{
 int choix;
    
    AfficherKalah();
    printf("    I-   pour jouer a nouveau tapez 1\n");
    printf("    II-  pour terminer la dernier jeux taper 2 \n ");
    printf("   III- pour quiter tapez 3 \n");
    printf("Entrer votre choix : ");
    scanf("%d",&choix);
    printf("\n");
    if (choix<=0 || choix>3)
    {
     printf("choix incorrect ressayer a nouveau\n");
     SDL_Delay(1500);
     system("cls");
     choix = MenuPrincipal();
    }
    return choix ;
}

void AfficherGamme(Game game)
{

    int i;
  
     printf("         I           1           2           3           4           5           6\n");
     printf("        ___________________________________________________________________________\n");
      printf("        [%d]",game.Joueur[0].kalah.NombreActuel);
    for (size_t i = 0; i < 6; i++)
    {
       printf("         [%d]",game.Joueur[0].PilesJoueur[i].NombreActuel);
    }
    printf("\n");
    printf("\n");
    printf("                   ________________________________________________________________________________________\n");             
    printf("         ");
    for (size_t i = 0; i < 6; i++)
    {
       printf("           [%d]",game.Joueur[1].PilesJoueur[i].NombreActuel);
    }
    printf("           [%d]",game.Joueur[1].kalah.NombreActuel);
    printf("\n                   ________________________________________________________________________________________\n");
    printf("\n                     7             8             9            10            11            12            II\n");
    printf("%n");
      

    
}


Game HandTake(int j,Game gamme)
{
   int i = j/7;
   switch (i)
   {
   case 0:
          if( j<=0 || j >= 13 ||PileEstVide(gamme.Joueur[i].PilesJoueur[j-1]) ) 
        {
           printf(" la choix et incorrect ressayer a neouvau \n");
           return gamme;
        }
     break;
   
   default:
        if( j<=0 || j >= 13 ||PileEstVide(gamme.Joueur[i].PilesJoueur[j-7]) ) 
        {
           printf(" la choix et incorrect ressayer a neouvau \n");
           return gamme;
        }
     break;
   }
  
  //*******
  if( j <= 6){
  gamme.hand = gamme.Joueur[i].PilesJoueur[j-1]; //prendre la pile choisie
  gamme.Joueur[i].PilesJoueur[j-1] = PileVide(); // remplacer par  vide 
  }else
  {
  gamme.hand = gamme.Joueur[i].PilesJoueur[j-7]; //prendre la pile choisie
  gamme.Joueur[i].PilesJoueur[j-7] = PileVide();
  }
   return gamme;
}

int ChoixPile(int i)
{
   int j;
   switch (i)
   {
   case 0 :
      printf(" joueur 1  :");
      scanf("%d",&j);
      printf("\n");
           if (!(j>0 && j < 7 ))
            {
              printf(" voux avez vhoisie la pile de votre adversaire resayer a nouveau \n");
              j = ChoixPile(i);
            }
       break;
   case 1 :
      printf(" joueur 2  :");
      scanf("%d",&j);
      printf("\n");
           if (!(j<13 && j > 6))
            {
             printf(" voux avez vhoisie la pile de votre adversaire resayer a nouveau \n");
             j = ChoixPile(i);
            }
      break;
   default:
   break;
   }
   return j;
}


Game Jouer1(int j,Game gamme)
{  
  
     gamme = HandTake(j,gamme);
     if (j <= 6)
     {
       j--;
     }else
     {
       j++;
     }
     while (!PileEstVide(gamme.hand))
     {
      DERNIERPILE  = j;
      switch (j)
      {
      case 0 :
               gamme.Joueur[0].kalah =Empiler(gamme.Joueur[0].kalah);
               gamme.hand = Depiler(gamme.hand);
               j = 7;
               break;
      case 1 :
      case 2 :
      case 3 :
      case 4 :
      case 5 :
      case 6 :gamme.Joueur[0].PilesJoueur[j-1] = Empiler(gamme.Joueur[0].PilesJoueur[j-1]);
              gamme.hand = Depiler(gamme.hand);
              j--;
              break;
      case 7 :
      case 8 :
      case 9 :
      case 10 :
      case 11 :
      case 12 :gamme.Joueur[1].PilesJoueur[j-7] = Empiler(gamme.Joueur[1].PilesJoueur[j-7]);
               gamme.hand = Depiler(gamme.hand);
               j++;
               break;
      case 13 : j = 6;
                break;
      default:
                break;
      }
      if(gamme.hand.NombreActuel == 0) break;
    }
 
  return gamme;
}

Game Jouer2(int j,Game gamme)
{

  gamme = HandTake(j,gamme);
     if (j <= 6)
     {
       j--;
     }else
     {
       j++;
     }
    
     while (!PileEstVide(gamme.hand))
     {
      DERNIERPILE  = j;
      switch (j)
      {
      case 0 :j = 7;
               break;
      case 1 :
      case 2 :
      case 3 :
      case 4 :
      case 5 :
      case 6 :gamme.Joueur[0].PilesJoueur[j-1] = Empiler(gamme.Joueur[0].PilesJoueur[j-1]);
              gamme.hand = Depiler(gamme.hand);
              j--;
              break;
      case 7 :
      case 8 :
      case 9 :
      case 10 :
      case 11 :
      case 12 :gamme.Joueur[1].PilesJoueur[j-7] = Empiler(gamme.Joueur[1].PilesJoueur[j-7]);
               gamme.hand = Depiler(gamme.hand);
               j++;
               break;
      case 13 : gamme.Joueur[1].kalah =Empiler(gamme.Joueur[1].kalah);
                gamme.hand = Depiler(gamme.hand);
                j = 6;
                break;
      default:
                break;
      }
      if(gamme.hand.NombreActuel == 0) break;
    }
   
    
  return gamme;
}
int Randomly(){
        srand(time(NULL)); 
        int r = rand() % 2;
  return r;
}
int LesPileEstVide(joueur j)
{
  int i;
  for ( i = 0; i < 6 ; i++)
  {
    if (!PileEstVide( j.PilesJoueur[i])) return 0;
  }

  return 1;

}
int GameFinish(int i,Game gamme)
{
  
  if (LesPileEstVide(gamme.Joueur[i]) && PileEstVide(gamme.hand))
  {
    return 1;
  }else
  {
    return 0;
  }
  
  
}
Game TomberSurVide(int j,Game gamme)
{
  int i = j/7,p1,p2,p3;
  
  switch (i)
  {
  case 0:
        if (gamme.Joueur[0].PilesJoueur[j - 1].NombreActuel  == 1 )
               {
                 if ( !PileEstVide(gamme.Joueur[1].PilesJoueur[j - 1]))
                 {
                   p1 = gamme.Joueur[0].PilesJoueur[j - 1].NombreActuel;
                   p2 = gamme.Joueur[1].PilesJoueur[ j -1].NombreActuel;
                   p3 = gamme.hand.NombreActuel;
                   gamme.Joueur[0].PilesJoueur[DERNIERPILE - 1] = PileVide();
                   gamme.Joueur[1].PilesJoueur[DERNIERPILE - 1] = PileVide();
                   gamme.hand = PileVide();
                  gamme.Joueur[0].kalah = EmpilerNFois(gamme.Joueur[0].kalah,p1+p2+p3);
                 
                 }
               }
    
    break;
    case 1 :
              if (gamme.Joueur[1].PilesJoueur[j - 7].NombreActuel  == 1 )
               {
                 if (!PileEstVide(gamme.Joueur[0].PilesJoueur[j - 7 ]))
                 {
                   p1 = gamme.Joueur[0].PilesJoueur[j - 7].NombreActuel;
                   p2 = gamme.Joueur[1].PilesJoueur[j - 7].NombreActuel;
                   p3 = gamme.hand.NombreActuel;
                   gamme.Joueur[0].PilesJoueur[j - 7 ] = PileVide();
                   gamme.Joueur[1].PilesJoueur[j - 7] = PileVide();
                   gamme.hand = PileVide();
                   gamme.Joueur[1].kalah = EmpilerNFois(gamme.Joueur[1].kalah,p1 + p2 + p3);
                 }
                 
               }
    break;
  default:
    break;
  }
       
  return gamme;
  
}
 Game SaveEtRetour(Game gamme)
{
  int choix; 
    printf("1- si vous Voullez sauvgarder cette tour \n");
    printf("2- si vous reviendre a la tourne precedent \n");
    printf("AUtre pour continue \n");
    scanf("%d",&choix);
    printf("\n");
    if (choix == 1)
    {
      //Enregistrer(DERNIERPILE,gamme);
    }
    if (choix == 2)
    {
      //gamme = Readrecent();
    }
    return gamme;
}
Game Recupurer(int i,Game gamme)
{
  int j,p;
  for ( j = 0; j < 6; j++)
  {
    gamme.hand = EmpilerNFois(gamme.hand,gamme.Joueur[i].PilesJoueur[j].NombreActuel);
    gamme.hand,gamme.Joueur[i].PilesJoueur[j] = PileVide();
  }
  return gamme;
}
void PlayingNewGame(Game gamme){
        int NOMBREGRAVIER;
       do
       {
         printf("entrer le nombre des Graviers inferieur a 10 \n");
         scanf("%d",&NOMBREGRAVIER);
       } while ( NOMBREGRAVIER <= 0 || NOMBREGRAVIER > 10);
       
       
        system("cls");
        gamme = DepartDuJeu(NOMBREGRAVIER);
        int j,i = Randomly();
       
        while (!GameFinish(!i,gamme)){
           
        switch (i)
        {
        case 0 :
             // Enregistrer(DERNIERJOUEUR,gamme);
        rejouer0 :system("cls");
                   AfficherGamme(gamme);
               do
              {
                j = ChoixPile(i);
              } while (PileEstVide(gamme.Joueur[i].PilesJoueur[j-1]));
              
              gamme =  Jouer1(j,gamme);
               //******** replay conditions 
                if (DERNIERPILE == 0 && !GameFinish(i,gamme))
                {
                 goto rejouer0;
               }
               gamme = TomberSurVide(DERNIERPILE,gamme);
                i = 1;
              /* gamme = SaveEtRetour(gamme);
                SDL_Delay(5000);
               if (DERNIERJOUEUR != i)
               {
                 i = 0;
                 goto rejouer0;
               }*/
              //**************************
            
                  
              break;
        case 1 :
                //Enregistrer(DERNIERPILE,gamme);
        rejouer1 :system("cls");
                AfficherGamme(gamme);
              do
              {
                j = ChoixPile(i);
              } while (PileEstVide(gamme.Joueur[i].PilesJoueur[j-7]));
              
            
              gamme =  Jouer2(j,gamme);
              
             //******** replay conditions 
              
               if (DERNIERPILE == 13 && !GameFinish(i,gamme))
               {
                 goto rejouer1;
               }
                gamme = TomberSurVide(DERNIERPILE,gamme);
                  i = 0;
            //    gamme = SaveEtRetour(gamme);
              
              /*  
                SDL_Delay(5000);
                if (DERNIERJOUEUR != i)
               {
                 i = 1;
                 goto rejouer0;
               }*/
              //**************************
              
              break;
        default:
          break;
        }
        }
        
        
        // le joueur recuper ces piles
        gamme = Recuperer(i,gamme);
        gamme.Joueur[i].kalah = EmpilerNFois(gamme.Joueur[i].kalah,gamme.hand.NombreActuel);
        gamme.hand = PileVide();
        AfficherGamme(gamme);
        printf("\n\n    fin de jeux merci \n");
        whowin(gamme);

}

void ContinuePlaying(Game gamme)
{
  /*int i,j;
  gamme = Readrecent();
  i = DERNIERJOUEUR = DERNIERPILE/7;
  
  
        while (!GameFinish(!i,gamme)){
           
        switch (i)
        {
        case 0 :
             //  Enregistrer(DERNIERPILE,gamme);
        rejouer0 :system("cls");
                   AfficherGamme(gamme);
               do
              {
                j = ChoixPile(i);
              } while (PileEstVide(gamme.Joueur[i].PilesJoueur[j-1]));
              
              gamme =  Jouer1(j,gamme);
               //******** replay conditions 
              
                
                if (DERNIERPILE == 0 && !GameFinish(i,gamme))
                {
                 goto rejouer0;
               }
               gamme = TomberSurVide(DERNIERPILE,gamme);
                i = 1;
              /* gamme = SaveEtRetour(gamme);
               
               if (DERNIERJOUEUR != i)
               {
                 i = 0;
                 goto rejouer0;
               }
               
               
              //**************************
            
                  
              break;
        case 1 :
               // Enregistrer(DERNIERJOUEUR,gamme);
        rejouer1 :system("cls");
                AfficherGamme(gamme);
              do
              {
                j = ChoixPile(i);
              } while (PileEstVide(gamme.Joueur[i].PilesJoueur[j-7]));
              gamme =  Jouer2(j,gamme);
              
             //******** replay conditions 
              
               if (DERNIERPILE == 13 && !GameFinish(i,gamme))
               {
                 goto rejouer1;
               }
                gamme = TomberSurVide(DERNIERPILE,gamme);
              /*  gamme = SaveEtRetour(gamme);
                i = 0;
                
                if (DERNIERJOUEUR != i)
               {
                 i = 1;
                 goto rejouer0;
               }
              //**************************
              
              break;
        default:
          break;
        }
        }
        
        
        // le joueur recuper ces piles
        gamme = Recuperer(i,gamme);
        gamme.Joueur[i].kalah = EmpilerNFois(gamme.Joueur[i].kalah,gamme.hand.NombreActuel);
        gamme.hand = PileVide();
        AfficherGamme(gamme);
        // affichafe de le gangnant 
        printf("\n\n    fin de jeux merci \n");
        printf(" the winner is %d ",i);
        SDL_Delay(10000);
        whowin(gamme);*/
}
/*void Enregistrer(int i, Game gamme)
{ 
  FILE * F = fopen("enregetrement","wb");
  if (F == NULL)
  {
    printf("error d'enregestrement de jeux \n");
    system("exit");
  }
  fwrite(&gamme,sizeof(gamme),1,F);
  fwrite(&i,sizeof(int),1,F);
  fclose(F);
}
Game  Readrecent()
{
  FILE *F;
  Game gamme;
  int i,returncode;
   F = fopen("enregetrement","rb");
  if (F == NULL)
  {
    printf("error d'enregestrement de jeux \n");
    system("exit");
  }
  returncode = fread(&gamme,sizeof(gamme),1,F);
  if (returncode == EOF )
  {
    printf("problem de lecteur \n");
  }
  
  returncode = fread(&i,sizeof(int),1,F);
   if (returncode == EOF )
  {
    printf("problem de lecteur \n");
  }

  DERNIERJOUEUR = i;
  return gamme;
}*/
void whowin(Game gamme)
{
  if ( gamme.Joueur[0].kalah.NombreActuel > gamme.Joueur[1].kalah.NombreActuel)
  {
    printf(" joueur 1 t'as gangner \n");
    SDL_Delay(5000);
  }else if (gamme.Joueur[0].kalah.NombreActuel < gamme.Joueur[1].kalah.NombreActuel)
  {
       printf(" joueur 2 t'as gangner \n");
      SDL_Delay(5000);
  }else
  {
    printf(" y a une egalité \n");
  }
}


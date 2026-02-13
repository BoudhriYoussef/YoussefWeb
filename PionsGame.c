#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define taille 9

typedef struct Plateau {
    char matrice[taille][taille];
} PLATEAU;

typedef struct Joueur {
    char couleur;
    int nb_pions;
} JOUEUR;

/* ========================= */

void afficher_plateau(PLATEAU *PLT){
    printf("\n   1 2 3 4 5 6 7 8 9\n");
    for (int i = 0; i < taille; i++){
        printf("%c  ", 'A' + i);
        for (int j = 0; j < taille; j++){
            printf("%c ", PLT->matrice[i][j]);
        }
        printf("\n");
    }
}

void init_joueur(JOUEUR* J , char color ){
    J->couleur = color;
    J->nb_pions = 18;
}

void remplir_plateau(PLATEAU* PLT){
    for (int i = 0; i < taille; i++){
        for (int j = 0; j < taille; j++){
            if (i < 2) PLT->matrice[i][j] = 'N';
            else if (i > 6) PLT->matrice[i][j] = 'B';
            else PLT->matrice[i][j] = '.';
        }
    }
}

/* ---------- AJOUT : vérification du chemin ---------- */
int chemin_libre(PLATEAU *PLT, int ligneDepart, int colonneDepart, int ligneArrivée, int colonneArrivée){
    int pas_ligne = 0;
    int pas_colonne = 0;

    if (ligneArrivée > ligneDepart) pas_ligne = 1;
    else if (ligneArrivée < ligneDepart) pas_ligne = -1;

    if (colonneArrivée > colonneDepart) pas_colonne = 1;
    else if (colonneArrivée < colonneDepart) pas_colonne = -1;

    int i = ligneDepart + pas_ligne;
    int j = colonneDepart + pas_colonne;

    while (i != ligneArrivée || j != colonneArrivée){
        if (PLT->matrice[i][j] != '.')
            return 0;
        i += pas_ligne;
        j += pas_colonne;
    }

    return 1;
}

int deplacement_valide(PLATEAU PLT,JOUEUR J,int ligneDepart,int colonneDepart ,int ligneArrivée ,int colonneArrivée ){

    if (ligneArrivée < 0 || ligneArrivée >= taille || colonneArrivée < 0 || colonneArrivée >= taille) return 0;

    if (PLT.matrice[ligneDepart][colonneDepart] != J.couleur) return 0;

    if (PLT.matrice[ligneArrivée][colonneArrivée] != '.') return 0;

    // on élimine tout les déplacement diagonales !!
    if (ligneDepart != ligneArrivée && colonneDepart != colonneArrivée) return 0;

    // pas de déplacement fait 
    if (ligneDepart == ligneArrivée && colonneDepart == colonneArrivée) return 0;

    //le cas de saut d'un pion adjacent (adversaire ou ennemi) situé dans la meme colonne (dernier cas possible)
    if (abs(ligneArrivée-ligneDepart)==2){
        return (PLT.matrice[ligneArrivée][colonneArrivée]=='.') ; //on ne tient pas compte de ce qu'il y a dans la position adjacente (soit vide soit adversaire soit ennemi on peut deplacer)
    }

    //le cas de saut d'un pion adjacent(adversaire ou ennemi) situé dans la meme ligne (dernier cas possible)
    if (abs(colonneArrivée-colonneDepart)==2){ 
        return (PLT.matrice[ligneArrivée][colonneArrivée]=='.') ; //on ne tient pas compte de ce qu'il y a dans la position adjacente (soit vide soit adversaire soit ennemi on peut deplacer)
    }
    
    //cas de glissment dans un chemin vide (sans saut d'un pion adjacent)
    if (ligneArrivée-ligneDepart !=2 || colonneArrivée-colonneDepart !=2 ){
        return (chemin_libre(&PLT, ligneDepart,colonneDepart,ligneArrivée,colonneArrivée)); // si chemin est libre donc glissement possible
    }

    return 0; // si le deplacement ne correspond à aucun cas des cas ci dessus on retourne 0 pour debuger la compilation
    
}

int recherche_indice(char c){
    if (c >= 'A' && c <= 'I') return c - 'A';
    if (c >= '1' && c <= '9') return c - '1';
    return -1;
}

void conversion(char positionDeplacement[],int *ligneDepart,int *colonneDepart,int *ligneArrivée,int *colonneArrivée){
    *ligneDepart  = recherche_indice(positionDeplacement[0]);
    *colonneDepart = recherche_indice(positionDeplacement[1]);
    *ligneArrivée  = recherche_indice(positionDeplacement[3]);
    *colonneArrivée = recherche_indice(positionDeplacement[4]);
}

void deplacer_pion(PLATEAU *PLT,int ligneDepart,int colonneDepart,int ligneArrivée,int colonneArrivée){
    PLT->matrice[ligneArrivée][colonneArrivée] = PLT->matrice[ligneDepart][colonneDepart];
    PLT->matrice[ligneDepart][colonneDepart] = '.';
}

void compter_pions(PLATEAU *PLT, JOUEUR *J1, JOUEUR *J2){
    J1->nb_pions = 0;
    J2->nb_pions = 0;
    for (int i = 0; i < taille; i++){
        for (int j = 0; j < taille; j++){
            if (PLT->matrice[i][j] == J1->couleur) J1->nb_pions++;
            else if (PLT->matrice[i][j] == J2->couleur) J2->nb_pions++;
        }
    }
}

void capture_pions(PLATEAU *PLT, JOUEUR *J1, JOUEUR *J2, int ligneArrivee, int colonneArrivee, int *nb_oppose){
    int VecL[4] = {-1, 1, 0, 0};
    int VecC[4] = {0, 0, -1, 1};
    int total_captures = 0;

    for (int k = 0; k < 4; k++){
        int i = ligneArrivee + VecL[k];
        int j = colonneArrivee + VecC[k];
        *nb_oppose = 0;

        while (i >= 0 && i < 9 && j >= 0 && j < 9 && PLT->matrice[i][j] == J2->couleur){
            (*nb_oppose)++;
            i += VecL[k];
            j += VecC[k];
        }

        if (*nb_oppose > 0 &&
            i >= 0 && i < 9 && j >= 0 && j < 9 &&
            PLT->matrice[i][j] == J1->couleur){

            i = ligneArrivee + VecL[k];
            j = colonneArrivee + VecC[k];

            while (PLT->matrice[i][j] == J2->couleur){
                PLT->matrice[i][j] = '.';
                J2->nb_pions--;
                total_captures++;
                i += VecL[k];
                j += VecC[k];
            }
        }
    }

    if (total_captures > 0){
        char *couleur_nom = (J2->couleur == 'N') ? "noir" : "blanc";
        if (total_captures == 1)
            printf("Capture d'un pion %s!\n", couleur_nom);
        else
            printf("Capture de %d pions %ss!\n", total_captures, couleur_nom);
    }
}

int verif_fin_jeu(JOUEUR J1,JOUEUR J2){
    return (J1.nb_pions <= 5 || J2.nb_pions <= 5);
}

void annoner_vainqueur(JOUEUR J1,JOUEUR J2){
    if (J1.nb_pions > J2.nb_pions)
        printf("\nLe gagnant est le joueur 1 (%c)\n", J1.couleur);
    else
        printf("\nLe gagnant est le joueur 2 (%c)\n", J2.couleur);
}

/* ========================= */

int main(){
    PLATEAU PLT;
    JOUEUR J1;
    JOUEUR J2;
    int fin_jeu = 0;
    int nb_oppose;

    init_joueur(&J1 , 'N');
    init_joueur(&J2 , 'B');
    remplir_plateau(&PLT);
    afficher_plateau(&PLT);

    while (!fin_jeu){

        char positionsDeplacement[10];
        int ligneDepart, colonneDepart, ligneArrivée, colonneArrivée;

        printf("\nJoueur B : ");

        int coup_trouve = 0;
        while (!coup_trouve) {
            if (fgets(positionsDeplacement, sizeof(positionsDeplacement), stdin) == NULL) {
                printf("\nErreur de lecture ou fin de fichier.\n");
                fin_jeu = 1;
                break;
            }
            positionsDeplacement[strcspn(positionsDeplacement,"\n")] = '\0';
            conversion(positionsDeplacement,&ligneDepart,&colonneDepart,&ligneArrivée,&colonneArrivée);

            if (deplacement_valide(PLT,J2,ligneDepart,colonneDepart,ligneArrivée,colonneArrivée)){
                coup_trouve = 1;
            } else {
                printf("Coup invalide, suivant: ");
            }
        }

        if (fin_jeu) break;

        deplacer_pion(&PLT,ligneDepart,colonneDepart,ligneArrivée,colonneArrivée);
        capture_pions(&PLT,&J2,&J1,ligneArrivée,colonneArrivée,&nb_oppose);
        compter_pions(&PLT, &J1, &J2);
        afficher_plateau(&PLT);
        printf("Pions N=%d, B=%d\n", J1.nb_pions, J2.nb_pions);

        fin_jeu = verif_fin_jeu(J1,J2);
        if (fin_jeu) break;

        printf("\nJoueur N : ");

        coup_trouve = 0;
        while (!coup_trouve) {
            if (fgets(positionsDeplacement, sizeof(positionsDeplacement), stdin) == NULL) {
                printf("\nErreur de lecture ou fin de fichier.\n");
                fin_jeu = 1;
                break;
            }
            positionsDeplacement[strcspn(positionsDeplacement,"\n")] = '\0';
            conversion(positionsDeplacement,&ligneDepart,&colonneDepart,&ligneArrivée,&colonneArrivée);

            if (deplacement_valide(PLT,J1,ligneDepart,colonneDepart,ligneArrivée,colonneArrivée)){
                coup_trouve = 1;
            } else {
                printf("Coup invalide, suivant: ");
            }
        }

        if (fin_jeu) break;

        deplacer_pion(&PLT,ligneDepart,colonneDepart,ligneArrivée,colonneArrivée);
        capture_pions(&PLT,&J1,&J2,ligneArrivée,colonneArrivée,&nb_oppose);
        compter_pions(&PLT, &J1, &J2);
        afficher_plateau(&PLT);
        printf("Pions N=%d, B=%d\n", J1.nb_pions, J2.nb_pions);

        fin_jeu = verif_fin_jeu(J1,J2);
    }

    annoner_vainqueur(J1,J2);
    return 0;
}

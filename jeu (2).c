#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define taille 9
#define VIDE '.'
#define NOIR 'N'
#define BLANC 'B'
#define SEUIL_FIN 5


//lES STRUCTURES //

/* structure de donnée Plateau
 * matrice : tableau  9x9 de caractères
 *           chaque case contient :
 *           'N' (pion noir), 'B' (pion blanc) ou '.' (case vide)
 */

typedef struct Plateau {
    char matrice[taille][taille];
} PLATEAU;

/* structure de donnée Joueur
 * couleur : caractère : 'N' ou 'B'
 * nb_pions : entier : nombre de pions du joueur sur le plateau
 */

typedef struct Joueur {
    char couleur;
    int nb_pions;
} JOUEUR;

//LES FONCTIONS ET ACTIONS//

/* action afficher_plateau
 * Rôle : afficher l’état actuel du plateau de jeu
 * Paramètres: donnée : PLATEAU PLT
 *             locales: i : entier:Indice de ligne pour l'affichage
 *                      j : entier : Indice de colonne pour l'affichage
 *
 */

void afficher_plateau(PLATEAU *PLT){
    // Affichage de l'entête des colonnes (1 à 9)
    printf("\n   1 2 3 4 5 6 7 8 9\n");
    // Boucle sur chaque ligne du plateau
    for (int i = 0; i < taille; i++){
        printf("%c  ", 'A' + i);

        // Boucle sur chaque colonne de la ligne i
        for (int j = 0; j < taille; j++){
            // Affiche le contenu de la case ('.' = vide, 'N' = pion noir, 'B' = pion blanc)
            printf("%c ", PLT->matrice[i][j]);
        }
        printf("\n");
    }
}

/* action init_joueur
 * Rôle : initialiser un joueur (couleur + nombre de pions)
 * donnée/résultat : JOUEUR J
 * donnée : caractère: color
 */

void init_joueur(JOUEUR* J , char color ){
    J->couleur = color;
    J->nb_pions = 18;
}

/* action remplir_plateau
 * Rôle : initialiser le plateau :
 *       - 2 premières lignes : pions noirs
 *       - 2 dernières lignes : pions blancs
 *       - le reste : cases vides
 * Paramètres:Résultat : PLATEAU PLT
 *            Locales:i : entier{Indice de ligne pour parcourir de la matrice }
 *                    j : entier {Indice de colonne pour parcourir la matrice }
 */

void remplir_plateau(PLATEAU* PLT){
    // Boucle sur chaque ligne du plateau (indices 0 à 8)
    for (int i = 0; i < taille; i++){
        // Boucle sur chaque colonne de la ligne i (indices 0 à 8)
        for (int j = 0; j < taille; j++){

            // Les deux premières lignes (i = 0 ou 1) : pions noirs
            if (i < 2) PLT->matrice[i][j] = NOIR;
            // Les deux dernières lignes (i = 7 ou 8) : pions blancs
            else if (i > 6) PLT->matrice[i][j] = BLANC;
             // Les lignes intermédiaires : cases vides
            else PLT->matrice[i][j] = VIDE;
        }
    }
}

//Validation du coup //

/* fonction chemin_libre qui retourne un entier pour désigner vrai ou faux
 * Rôle : vérifier que toutes les cases entre départ et arrivée sont vides
 * Paramètres:Données: : PLATEAU *PLT
 *                       ligneDepart, colonneDepart, ligneArrivée, colonneArrivée:entier
 *            Locales: pas_ligne:entier{Indique le sens vertical du déplacement (+1=bas, -1=haut, 0=pas de mouvement)}
 *                     pas_colonne:entier {Indique le sens horizontal du déplacement (+1=droite, -1=gauche, 0=pas de mouvement)}
 *                     i:entier {Ligne actuelle parcourue pour vérifier si le chemin est libre}
 *                     j:entier {Colonne actuelle parcourue pour vérifier si le chemin est libre}
 */

int chemin_libre(PLATEAU *PLT, int ligneDepart, int colonneDepart, int ligneArrivée, int colonneArrivée){
    int pas_ligne = 0;   // pas sur les lignes : +1 = descendre, -1 = monter, 0 = pas de déplacement vertical
    int pas_colonne = 0; // entier : +1 si déplacement vers la droite, -1 si vers la gauche, 0 si pas de mouvement horizontal

    // Calcul du pas vertical
    if (ligneArrivée > ligneDepart)
        pas_ligne = 1;  //on descend
    else if (ligneArrivée < ligneDepart)
        pas_ligne = -1;  //on monte

    // Calcul du pas horizontal
    if (colonneArrivée > colonneDepart) pas_colonne = 1; // vers la droite
    else if (colonneArrivée < colonneDepart) pas_colonne = -1;  //vers la gauche

    // Initialisation des indices pour parcourir le chemin
    int i = ligneDepart + pas_ligne;
    int j = colonneDepart + pas_colonne;

    // Parcours du chemin entre départ et arrivée
    while (i != ligneArrivée || j != colonneArrivée){
        if (PLT->matrice[i][j] != VIDE)
            return 0;   //chemin bloquee
        i += pas_ligne;
        j += pas_colonne;
    }

    return 1;  //chemin libre
}


/* fonction deplacement_valide qui retourne un entier (1 ou 0) pour désigner vrai ou faux
 * Rôle : vérifier si un déplacement respecte les règles du jeu
 * paramètres :
 * Données:
 *   - PLT : PLATEAU
 *           plateau de jeu
 *   - J : JOUEUR
 *           joueur qui tente le déplacement
 *   - ligneDepart : entier
 *           ligne de départ du pion
 *   - colonneDepart : entier
 *           colonne de départ du pion
 *   - ligneArrivée : entier
 *           ligne d’arrivée du pion
 *   - colonneArrivée : entier
 *           colonne d’arrivée du pion
 */
int deplacement_valide(PLATEAU PLT,JOUEUR J,int ligneDepart,int colonneDepart ,int ligneArrivée ,int colonneArrivée ){

    //condition dimensions plateau
    if (ligneArrivée < 0 || ligneArrivée >= taille || colonneArrivée < 0 || colonneArrivée >= taille){
        printf("Vous avez saisir des valeurs hors du cadre du plateau");
        return 0;
    }
    //vérifier que le joueur déplace son propre pion
    if (PLT.matrice[ligneDepart][colonneDepart] != J.couleur){
        printf("Vous venez de déplacer un pion qui n'est pas le votre");
        return 0;
    }
    //vérifier que la case d'arrivée est vide
    if (PLT.matrice[ligneArrivée][colonneArrivée] != VIDE){
        printf("La case d'arrivée n'est pas vide");
        return 0;
    }
    //Déplacement horizontal
    if (ligneDepart != ligneArrivée && colonneDepart != colonneArrivée){
        printf("Vous avez fait un déplacement diagonal, qui est interdit");
        return 0;
    }
    //Pas de déplacement(même cases de départ et arrivée)
    if (ligneDepart == ligneArrivée && colonneDepart == colonneArrivée){
        printf("Vous ne changez pas de case");
        return 0;
    }

    return 1;
}


 /* action: nature_deplacement
 * Rôle : afficher la nature de déplacement effectué par un pion
 *        - 1 : déplacement simple (1 case)
 *        - 2 : saut (2 cases avec pion intermédiaire)
 *        - 3 : déplacement long/glissé (plus de 2 cases, chemin libre)
 *
 * Paramètres: Données:
 *                      PLT : PLATEAU (structure) : le plateau de jeu
 *                      ligneDepart : entier : indice de la ligne de départ (0..8)
 *                      colonneDepart : entier : indice de la colonne de départ (0..8)
 *                      ligneArrivee : entier : indice de la ligne d'arrivée (0..8)
 *                      colonneArride typevee : entier : indice de la colonne d'arrivée (0..8)
 *             Locales: distance: entier{Calcule le nombre de cases entre la case de départ et la case d’arrivée. Toujours positif}
 *                      ligneMilieu:entier{Ligne de la case intermédiaire si le déplacement est un saut de 2 cases}
 *                      colonneMilieu:entier{Colonne de la case intermédiaire si le déplacement est un saut de 2 cases}
 */

void nature_deplacement(PLATEAU PLT, inboolt ligneDepart, int colonneDepart, int ligneArrivee, int colonneArrivee) {

    int distance;        // entier : nombre de cases entre départ et arrivée (toujours positif)
    int ligneMilieu;     // entier : ligne de la case intermédiaire pour un saut de 2 cases
    int colonneMilieu;   // entier : colonne de la case intermédiaire pour un saut de 2 cases

   //calcul de la distnace
    if (ligneDepart == ligneArrivee) {
        // déplacement horizontal : distance = différence de colonnes
        distance = colonneArrivee - colonneDepart;
    } else {
        // déplacement vertical : distance = différence de lignes
        distance = ligneArrivee - ligneDepart;
    }

    // rendre la distance positive
    if (distance < 0) {
        distance = -distance;
    }

    //cas 1: Déplacement simple
    if (distance == 1) {
        printf("Déplacement simple effectué\n");  // pion se déplace d'une seule case --> déplacement simple
    }

    //cas2: saut de 2 cases
    else if (distance == 2) {
        // déterminer la case intermédiaire
        ligneMilieu = (ligneDepart + ligneArrivee) / 2;
        colonneMilieu = (colonneDepart + colonneArrivee) / 2;

        if (PLT.matrice[ligneMilieu][colonneMilieu] != VIDE) {
            printf("Saut effectué.\n"); // saut valide si la case intermédiaire est occupée(non vide)
        }
    }

   //cas3: déplacement long et glissé
    else {
        // vérifier que toutes les cases entre départ et arrivée sont vides
        if (chemin_libre(&PLT, ligneDepart, colonneDepart, ligneArrivee, colonneArrivee) == 1) {
            printf("Déplacement glissé effectué\n"); // chemin libre
        }
    }
}


/* fonction recherche_indice
 * Rôle : convertir un caractère de position en indice de tableau (0..8)
 * Paramètre :c : caractère : caractère à convertir ('A'..'I' ou '1'..'9')
 *
 */

int recherche_indice(char c){
    if (c >= 'A' && c <= 'I')   // Si c'est une lettre de A à I
        return c - 'A';          // Convertit en indice 0-8

    if (c >= '1' && c <= '9')   // Si c'est un chiffre 1-9
        return c - '1';          // Convertit en indice 0-8

    return -1;
}


/* action conversion
 * Rôle : convertir une chaîne de déplacement "A1,B2" en fonction des indices de tableau (0..8)
 * Paramètres :Données:positionDeplacement : chaîne de caractères] : chaîne de caractères saisie par le joueur (ex: "A1,B2")
 *             Résultats: ligneDepart, colonneDepart, ligneArrivee, colonneArrivee: entier
 *
 */

void conversion(char positionDeplacement[],
                int *ligneDepart,
                int *colonneDepart,
                int *ligneArrivée,
                int *colonneArrivée) {

    // Conversion de la lettre de départ en indice de ligne (0..8)
    *ligneDepart  = recherche_indice(positionDeplacement[0]);

    // Conversion du chiffre de départ en indice de colonne (0..8)
    *colonneDepart = recherche_indice(positionDeplacement[1]);

    // Conversion de la lettre d'arrivée en indice de ligne (0..8)
    *ligneArrivée  = recherche_indice(positionDeplacement[3]);

    // Conversion du chiffre d'arrivée en indice de colonne (0..8)
    *colonneArrivée = recherche_indice(positionDeplacement[4]);
}



/* action deplacer_pion
 * Rôle : déplacer un pion sur le plateau d'une case de départ à une case d'arrivée
 * Paramètres :
 *      PLT : PLATEAU :plateau de jeu
 *      Données:ligneDepart : entier : indice de ligne de départ (0..8)
 *              colonneDepart : entier : indice de colonne de départ (0..8)
 *              ligneArrivée : entier : indice de ligne d'arrivée (0..8)
 *              colonneArrivée : entier : indice de colonne d'arrivée (0..8)
 */

void deplacer_pion(PLATEAU *PLT,
                   int ligneDepart,
                   int colonneDepart,
                   int ligneArrivée,
                   int colonneArrivée) {

    // Copier le pion de la case de départ vers la case d'arrivée
    PLT->matrice[ligneArrivée][colonneArrivée] = PLT->matrice[ligneDepart][colonneDepart];

    // Vider la case de départ (remplacer par '.')
    PLT->matrice[ligneDepart][colonneDepart] = VIDE;
}




/* action capture_pions
 * Rôle : vérifier si le dernier pion déplacé capture des pions adverses
 *        et mettre à jour le plateau et le nombre de pions du joueur adverse.
 * Paramètres :
 *      PLT : PLATEAU : le plateau de jeu
 *      J1  : JOUEUR : joueur courant (qui a déplacé son pion)
 *      J2  : JOUEUR : joueur adverse (dont les pions peuvent être capturés)
 *      ligneArrivee : entier : indice de la ligne où le pion a été déplacé
 *      colonneArrivee : entier : indice de la colonne où le pion a été déplacé
 * Locales :
 *      VecL : tableau de 4 entiers { -1, 1, 0, 0 } : vecteurs pour parcourir les lignes (haut, bas, rien, rien)
 *      VecC : tableau de 4 entiers { 0, 0, -1, 1 } : vecteurs pour parcourir les colonnes (rien, rien, gauche, droite)
 *      total_captures : entier : nombre total de pions capturés dans ce déplacement
 *      i, j : entier : indices pour parcourir les cases adjacentes
 */
void capture_pions(PLATEAU *PLT, JOUEUR *J1, JOUEUR *J2, int ligneArrivee, int colonneArrivee) {

    int VecL[4] = {-1, 1, 0, 0}; // déplacement vertical : haut, bas
    int VecC[4] = {0, 0, -1, 1}; // déplacement horizontal : gauche, droite
    int total_captures = 0;       // compteur total des pions capturés

    // Parcourir les 4 directions autour du pion
    for (int k = 0; k < 4; k++) {
        int i = ligneArrivee + VecL[k];   // première case dans la direction k
        int j = colonneArrivee + VecC[k];
        int nb_adverses = 0;              // compteur des pions adverses consécutifs

        // Compter les pions adverses consécutifs
        while (i >= 0 && i < taille && j >= 0 && j < taille && PLT->matrice[i][j] == J2->couleur) {
            nb_adverses++;
            i += VecL[k];
            j += VecC[k];
        }

        // Si la ligne se termine par un pion du joueur courant, capture des pions adverses
        if (nb_adverses > 0 && i >= 0 && i < taille && j >= 0 && j < taille && PLT->matrice[i][j] == J1->couleur) {
            i = ligneArrivee + VecL[k];
            j = colonneArrivee + VecC[k];

            // Supprimer les pions adverses capturés
            while (PLT->matrice[i][j] == J2->couleur) {
                PLT->matrice[i][j] = VIDE;
                J2->nb_pions--;        // mettre à jour le nombre de pions du joueur adverse
                total_captures++;      // incrémenter le total de captures
                i += VecL[k];
                j += VecC[k];
            }
        }
    }

    // Affichage des captures
    if (total_captures > 0) {
        char *couleur_nom = (J2->couleur == NOIR) ? "noir" : "blanc";
        if (total_captures == 1)
            printf("Capture d'un pion %s\n", couleur_nom);
        else
            printf("Capture de %d pions %ss\n", total_captures, couleur_nom);
    }
}


/* fonction verif_fin_jeu  qui retourne un entier (1 ou 0) pour désigner vrai ou faux
 * Rôle : vérifier si la partie est terminée
 *        La partie se termine dès qu'un joueur a 5 pions ou moins
 * Paramètres :
 *      J1 : JOUEUR : structure contenant la couleur et le nombre de pions du joueur 1
 *      J2 : JOUEUR : structure contenant la couleur et le nombre de pions du joueur 2
 */
int verif_fin_jeu(JOUEUR J1, JOUEUR J2){
    return (J1.nb_pions <= 5 || J2.nb_pions <= 5);
}


/* fonction annoncer_vainqueur
 * Rôle : afficher le gagnant d
 * Paramètres :
 *      J1 : JOUEUR : structure contenant la couleur et le nombre de pions du joueur 1
 *      J2 : JOUEUR : structure contenant la couleur et le nombre de pions du joueur 2
 */

void annoncer_vainqueur(JOUEUR J1,JOUEUR J2){
    if (J1.nb_pions > J2.nb_pions)
        printf("\nLe gagnant est le joueur 1 (%c)\n", J1.couleur);
    else
        printf("\nLe gagnant est le joueur 2 (%c)\n", J2.couleur);
}

//FONCTION PRINCIPALE

int main(){
    //Déclarations des variables
    PLATEAU PLT;
    JOUEUR J1;
    JOUEUR J2;

    int fin_jeu = 0;  // Indicateur de fin de partie
    char positionsDeplacement[6];  //// Chaîne saisie par le joueur pour son déplacement
    int ligneDepart, colonneDepart, ligneArrivée, colonneArrivée;  // Indices de départ et d'arrivée (lignes et colonnes)

    init_joueur(&J1 , NOIR);   // Initialiser le joueur 1 (pions noirs)
    init_joueur(&J2 , BLANC);   // Initialiser le joueur 2 (pions blancs)

    remplir_plateau(&PLT);  //remplir le plateau
    afficher_plateau(&PLT); //afficher le plateau initial

    //Boucle principale de la partie
    while (!fin_jeu) {

       //TOUR JOUEUR Blanc
       printf("\nJoueur B : ");
       int coup_trouve = 0;  //le coup valide pour le joueur courant

       // Boucle jusqu'à ce que le joueur B saisisse un coup valide
       while (!coup_trouve) {
           scanf(" %5s", positionsDeplacement);//lecture

           // Conversion de la chaîne en indices de tableau
           conversion(positionsDeplacement,
                      &ligneDepart, &colonneDepart,
                      &ligneArrivée, &colonneArrivée);

           // Vérification de la validité du déplacement
           if (deplacement_valide(PLT, J2,
               ligneDepart, colonneDepart,
               ligneArrivée, colonneArrivée)) {

               coup_trouve = 1;

            }
            else {
                printf("Coup invalide, suivant: ");

            }

    }

    // Déterminer la nature du déplacement (simple, saut, glissé)
    nature_deplacement(PLT, ligneDepart, colonneDepart, ligneArrivée, colonneArrivée);

    // Déplacer le pion sur le plateau
    deplacer_pion(&PLT,
                  ligneDepart, colonneDepart,
                  ligneArrivée, colonneArrivée);


    // Capture des pions adverses
    capture_pions(&PLT, &J2, &J1,
                   ligneArrivée, colonneArrivée);



    // Affichage du plateau et des scores après le tour
    afficher_plateau(&PLT);
    printf("Pions N=%d, B=%d\n", J1.nb_pions, J2.nb_pions);

    // Vérifier si la partie est terminée
    fin_jeu = verif_fin_jeu(J1, J2);

    //TOUR JOUEUR Noir
            if (!fin_jeu) {

                printf("\nJoueur N : ");
                coup_trouve = 0;

                while (!coup_trouve) {

                    scanf(" %5s", positionsDeplacement);

                    conversion(positionsDeplacement,
                            &ligneDepart, &colonneDepart,
                            &ligneArrivée, &colonneArrivée);

                    if (deplacement_valide(PLT, J1,
                                        ligneDepart, colonneDepart,
                                        ligneArrivée, colonneArrivée)) {
                        coup_trouve = 1;
                    }
                    else {
                        printf("Coup invalide, suivant: ");
                    }
                }

                 // Nature du déplacement
                nature_deplacement(PLT, ligneDepart, colonneDepart, ligneArrivée, colonneArrivée);



                // Déplacement du pion sur le plateau
                deplacer_pion(&PLT,
                            ligneDepart, colonneDepart,
                            ligneArrivée, colonneArrivée);

                // Capture des pions adverses
                capture_pions(&PLT, &J1, &J2,
                            ligneArrivée, colonneArrivée);



                // Affichage du plateau et des scores après le tour
                afficher_plateau(&PLT);
                printf("Pions N=%d, B=%d\n", J1.nb_pions, J2.nb_pions);

                // Vérification de fin de partie
                fin_jeu = verif_fin_jeu(J1, J2);
            }
        }

        //Annoncer le gagnant
        annoncer_vainqueur(J1, J2);
        return 0;
}


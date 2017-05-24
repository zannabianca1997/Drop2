/**
  \file
  \authors Giuseppe Zanichelli Bruno Bucciotti
  \brief assign2: file di implementazione drop
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "drop.h"
#include "myrand.h"

/** inizializza la matrice mat settando tutti i valori a EMPTY (stavolta non è globale!)

   \param mat puntatore alla matrice
   \param N numero di righe
   \param M numero di colonne
*/
void init_matrix (char** mat, unsigned n, unsigned m)
{
    //Se il puntatore è nullo esci
    if (mat==NULL) return;

    /*
    //La riga di Empty viene inizializzata
    char riga[M];
    int i, j;
    for (j=0; j<M; j++)
    {
    riga[j] = EMPTY;
    }
    //La riga di Empty viene ricopiata su ogni colonna della matrice
    for (i=0; i<N; i++)
    {
        memcpy(mat[i], riga, sizeof(riga));
    }
    */

    /* valori inconsistenti */
    if ( n == 0 || m == 0 ) {
      fprintf(stderr,"print_matrix: 0 colonne o righe.\n");
      return;
    }

    /* matrice vuota */
    if ( mat == NULL ) {
      fprintf(stderr,"Matrice vuota.\n");
      return;
    }

    int i;
    int j;

    for (i=0;i<n; i++) {
       for (j=0;j<m; j++){
        mat[i][j] = EMPTY;
       }
    }
}

/** libera la memoria occupata dalla matrice e mette a NULL il puntatore pmat

    \param pmat puntatore al puntatore alla matrice da liberare
    \param N numero di righe
 */
void free_matrix (char*** pmat, unsigned N)
{
    int i;
    for (i=0; i<N; i++)
    {
	//Libera le righe
	free((*pmat)[i]);
    }
    //Libera matrice e suo puntatore
    free(*pmat);
    (*pmat)=NULL;
}

/* scorciatoie per controllare se la cella è libera o occupata */
#define IS_FULL(x) ((x) == FULL || (x) == OBSTACLE)
#define IS_EMPTY(x) ((x) == EMPTY)

/* estrazione numeri casuali */
#define RND_2_RGT()  (my_rand() & 1) // estrae un numero tra 1 e 0 (bitwise operator, sempre più veloci)
#define RND_2_LFT()  ((my_rand() & 1) - 1) // estrae un numero tra 1 e 0 (bitwise operator, sempre più veloci)
#define RND_2_APART() ((my_rand() & 1) * 2 - 1) // estrae un numero fra {-1, +1}
#define RND_3()  ((my_rand() % 3) - 1) // aggiunge un errore dell'un per mille a favore dello 0, ma corregge quello di 1/32 del generatore (...)


/** calcola la caduta della prossima particella restituendo le coordinate (i,j) del prossimo elemento dell'area da mettere a FULL

   \param next_i l'indirizzo della variabile dove scrivere il valore della coordinata i
   \param next_j l'indirizzo della variabile dove scrivere il valore della coordinata j
   \param ad tipo di adiacenza con cui stiamo lavorando (croce, diagonale o entrambe)
   \param mat puntatore alla matrice
   \param N numero di righe della matrice
   \param M numero di colonne della matrice

   \SPECIFICA
   il prossimo elemento da riempire viene calcolato simulando la caduta di una particella
   a partire dalla posizione P0 di coordinate ( 0, M/2 ),
   ad ogni passo, se mi trovo nella posizione Pi = (i,j) considero l'insieme U delle celle non ancora piene fra la tre celle
              (i+1,j-1) (i+1,j) (i+1,j-1)
   e scelgo in modo equiprobabile fra le celle in U, ad esempio nel caso in cui le tre celle contengano rispettivamente
              EMPTY EMPTY FULL
   U = { (i+1,j-1), (i+1,j) }
   e scelgo fra i due elementi di U con probabilità 1/2.

   La caduta della particella si arresta quando si verifica uno dei due seguenti casi:
   1) sono arrivato all'ultima riga (la N-1) (quindi mi sedimento sul fondo)
   2) U e' vuoto
   3) ho almeno una cella piena nell'intorno di (i,j) definito dall'adiacenza "ad" ovvero
      a) nessuno (NONE): in questo caso mi fermo solo se (i+1,j), (i+1,j-1) e (i+1,j+1) sono gia' piene (U e' vuoto)
      b) croce (CROSS):
                (i-1,j)
     (i,j-1)     <i,j>    (i,j+1)
                (i+1,j)
      c) diagonale (DIAGONAL)
     (i-1,j-1)            (i-1,j+1)
                 <i,j>
     (i+1,j-1)            (i+1,j+1)
     d) croce e diagonale insieme (BOTH)
     (i-1,j-1)   (i-1,j)        (i-1,j+1)
      (i,j-1)     <i,j>          (i,j+1)
     (i+1,j-1)   (i+1,j)        (i+1,j+1)

   In tutti i casi --> restituisco in *next_i
   il valore di i e in *next_j il valore di j

   \retval 0 se le coordinate sono state calcolate correttamente
   \retval -1 se il punto di caduta iniziale P0
    al centro della prima riga è già FULL
    in questo caso non viene modificato il valore di *next_i *next_j

*/
int step (int* next_i, int* next_j, adj_t ad, char** mat, int N, int M)
{
    /* posizione iniziale */
    int P0_I = 0;
    int P0_J = (M / 2); // divisione intera, essendo entrambi interi

    // controllo se la simulazione è finita
    if(IS_FULL(mat[P0_I][P0_J]))
        return -1; //e finisce qui

    /* ----- SIMULAZIONE ----- */

    // setto le variabili posizione alla posizione iniziale
    int i = P0_I;
    int j = P0_J;

    //queste servono per segnarsi quali celle sono libere
    int left,  right, below;

    // Questo ciclo simula la caduta (i aumenta a ogni giro).
    for(;i<N-1;i++)
    {
        /*controllo le adiacenze*/
        //Controllo per Both e Diagonal
        if ((ad == DIAGONAL || ad == BOTH) &&
            ( ( i>0 && j > 0    && IS_FULL(mat[i-1][j-1]))  ||
              ( i>0 && j < M-1  && IS_FULL(mat[i-1][j+1]))  ||
              ( j > 0           && IS_FULL(mat[i+1][j-1]))  ||
              ( j < M-1         && IS_FULL(mat[i+1][j+1]))
            ) ) break;
        if ((ad == CROSS || ad == BOTH) &&//In questo modo Both equivale a controllare Cross e Diagonal
            ( ( i > 0     && IS_FULL(mat[i-1][j]))  ||
              ( i < N-1   && IS_FULL(mat[i+1][j]))  ||
              ( j > 0     && IS_FULL(mat[i][j-1]))  ||
              ( j < M-1   && IS_FULL(mat[i][j+1]))
            ) ) break;

        left  = (j > 0   && IS_EMPTY(mat[i+1][j-1])); // true se la cella sotto a sinistra è libera
        right = (j < M-1 && IS_EMPTY(mat[i+1][j+1])); // true se la cella sotto a destra è libera
        below = (j < M-1 && IS_EMPTY(mat[i+1][j])); // true se la cella sotto è libera

        if(left)
            if(right)
                if(below)
	                j += RND_3(); // tutti e tre liberi: scelgo tra le tre possibilità
                else
                    j += RND_2_APART(); // liberi tranne che al centro
            else
                if(below)
	                j += RND_2_LFT(); // liberi sotto e sinistra
                else
                    j -= 1; // libero solo a sinistra
        else
            if(right)
                if(below)
	                j += RND_2_RGT(); //liberi sotto e destra
                else
                    j += 1; //libero solo a destra
            else
                if(!below)
                    break; // adj era NONE e sotto sono tutti pieni: interrompi il ciclo senza aumentare i

            // ultimo caso solo below è vero, j resta invariata e i aumenta
    }

    *next_i = i;
    *next_j = j;
    return 0;
}


/** legge la rappresentazione di un ostacolo come stringa di 4 estremi interi
    (es. "0 0 3 4" rapresenta i due estremi (0,0) superiore sinistro e (3,4) inferiore destro)
    e crea sullo heap la corrispondente struttura obstacle_t
    controllando anche che le due coordinate siano consistenti ovvero detta (si,sj) la coordinata in alto a sinistra e (di,dj)
    quella in basso a destra deve essere vero che
    si <= di && sj <= dj

    \param s la stringa contenente gli estremi (es. "0 0 3 4")

    \retval p il puntatore alla nuova struttura obstacle_t creata (se la conversione ha avuto successo)
    \retval NULL altrimenti
*/
obstacle_t * string_to_obstacle (char * s)
{
    int i;
    int pos[4] = {0, 0, 0, 0}; /* conterrà i 4 numeri */
    for (i=0; i<4; i++) /* loop sui 4 numeri */
    {
	/* strtol prende la stringa s (primo parmtro), isola il numero che compare all'inizio e lo ritorna
	 come long (poi faccio cast a int)
	il secondo parametro è un puntatore su cui viene salvato il pezzo restante di stringa. 10 è la base decimale
	Per fortuna gli spazi bianchi iniziali vengono saltati (letto dalla documentazione e testato) */
	pos[i] = (int) strtol(s, &s, 10);
    }
    if (pos[0] <= pos[2] && pos[1] <= pos[3] &&
        pos[0] >= 0 && pos[2] >= 0 &&
        pos[1] >= 0 && pos[3] >= 0) /* faccio il test richiesto */
    {
        obstacle_t *obstacle = malloc(sizeof(obstacle_t));
        if (obstacle == NULL) return NULL; /* si spera che non capiti */

        obstacle->s_i = pos[0];
        obstacle->s_j = pos[1];
        obstacle->d_i = pos[2];
        obstacle->d_j = pos[3];

        return obstacle;
    }
    return NULL;//Altrimenti
}


/** crea la rappresentazione di un ostacolo come stringa con i 4 estremi (es. "0 0 3 4" rapresenta i due estremi (0,0) superiore sinistro e (3,4) inferiore destro)
    \param po puntatore all'ostacolo
    \param s puntatore all'array di caratteri dove scrivere la stringa risultato della conversione
    \param n lunghezza dell'array s

    \retval s il puntatore al primo carattere della stringa (se la conversione ha avuto successo)
    \retval NULL altrimenti
*/
char * obstacle_to_string (obstacle_t * po, char* s, int n)
{
    sprintf(s, "%d %d %d %d", po->s_i, po->s_j, po->d_i, po->d_j);
    return (*s);
}

/** inserisce nella matrice di caduta l'ostacolo s marcando gli elementi corrispondenti all'ostacolo con OBSTACLE
  \param s puntatore all'ostacolo da inserire
   \param mat puntatore alla matrice
   \param n numero di righe
   \param m numero di colonne

  \retval 0 se tutto è andato bene
  \retval -1 se l'ostacolo è incompatibile con l'area di caduta (es. le coordinate sono maggiori del numero di righe/colonne)
*/
int put_obstacle_in_matrix (obstacle_t * s,char ** mat, unsigned n, unsigned m){

    if(s->s_i <  0 || s->s_j <  0) return -1;
    if(s->d_i >= n || s->d_j >= m) return -1;

    int i; int j;

    for(i = s->s_i; i <= s->d_i; i++){
        for(j = s->s_j; j <= s->d_j; j++){
            mat[i][j] = OBSTACLE;
        }
    }
    return 0;
}

/** compara due obstacle

     L'ostacolo [(x1,y1),(t1,q1)] precede [(x2,y2),(t2,q2)] se vale che
    se x1!= x2 e x1 < x2 oppure
    se x1 == x2 e y1!= y2 e y1 < y2 oppure
    se x1 == x2 e y1 == y2 e t1!= t2 e t1 < t2 oppure
    se x1 == x2 e y1 == y2 e t1 == t2 e q1!= q2 e q1 < q2


  \param a primo obstacle
  \param b secondo obstacle

  \retval 1 se a > b
  \retval 0 altrimenti
*/
int maggiore_di(obstacle_t* a, obstacle_t* b){
  if (a->s_i > b->s_i)
    return 1;
  if (a->s_i < b->s_i)
    return 0;
  if (a->s_j > b->s_j)
    return 1;
  if (a->s_j < b->s_j)
    return 0;
  if (a->d_i > b->d_i)
    return 1;
  if (a->d_i < b->d_i)
    return 0;
  if (a->d_j > b->d_j)
    return 1;
  return 0; // anche in caso di parità
}

/** inserisce un ostacolo nella lista mantenendo l'ordinamento crescente
  \param p l'ostacolo da inserire (viene inserito direttamente senza effettuare copie)
  \param l il puntatore alla testa della lista

  \retval l il puntatore alla nuova testa della lista (dopo l'inserimento)

*/
lista_t * put_obstacle_in_list (obstacle_t* p,lista_t* l){
    if(l != NULL && maggiore_di(p, l->pobj)){ // è più avanti
        l->next = put_obstacle_in_list(p, l->next) //inseriscilo nel seguito
        return l; //tutto bene
    }
    // altrimenti va in testa ( o coda, se ci siamo già arrivati )
    lista_t * n_list;
    n_list = malloc(sizeof(lista_t));
    if(n_list == NULL) return NULL; // non so come fare di meglio. Dovrebbe dare errore
    n_list->pobj = p;
    n_list->next = l; // connetto al seguito (o a null, se finisce qui)
    return n_list;
}

/** libera la memoria occupata dalla lista mettendo a NULL il puntatore alla lista
   \param plist puntatore al puntatore della lista da deallocare
*/
void free_list (lista_t ** plist){
    *plist = NULL;
}

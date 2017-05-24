#include <stdio.h>
#include <time.h>
#include <string.h>

#include "drop.h"
#include "myrand.h"

// dichiafazione del tipo booleano e dei suoi valori true e false
typedef enum { false, true } bool;

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
      fprintf("Matrice vuota.\n");
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
#define RND_2_ADJ()  (my_rand() & 1) // estrae un numero tra 1 e 0 (bitwise operator, sempre più veloci)
#define RND_2_APART() ((my_rand() & 1) * 2 - 1) // estrae un numero fra {-1, +1}
#define RND_3()  (my_rand() % 3) // aggiunge un errore dell'un per mille a favore dello 0, ma corregge quello di 1/32 del generatore (...)


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
    bool left,  right, below;

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
              ( j > 0           && IS_FULL(mat[i][j-1]))  ||
              ( j < M-1         && IS_FULL(mat[i][j+1]))
            ) ) break;

        left  = (j > 0   && IS_EMPTY(mat[i+1][j-1])); // true se la cella sotto a sinistra è libera
        right = (j < M-1 && IS_EMPTY(mat[i+1][j+1])); // true se la cella sotto a destra è libera
	below = (j < M-1 && IS_EMPTY(mat[i+1][j])); // true se la cella sotto è libera

        if(left)
            if(right)
		if(below)
	                j += RND_3() - 1; // tutti e tre liberi: scelgo tra le tre possibilità
		else
			j += RND_2_APART(); // liberi tranne che al centro
            else
		if(below)
	                j += RND_2_ADJ() - 1; // liberi sotto e sinistra
		else
			j -= 1; // libero solo a sinistra
        else
            if(right)
		if(below)
	                j += RND_2_ADJ(); //liberi sotto e destra
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
    int pos[4] = { 0 }; /* conterrà i 4 numeri */
    for (i=0; i<4; i++) /* loop sui 4 numeri */
    {
	/* strtol prende la stringa s (primo parmtro), isola il numero che compare all'inizio e lo ritorna come long (poi faccio cast a int)
	il secondo parametro è un puntatore su cui viene salvato il pezzo restante di stringa. 10 è la base decimale
	Per fortuna gli spazi bianchi iniziali vengono saltati (letto dalla documentazione e testato) */
	pos[i] = (int) strtol(s, &s, 10);
    }
    if (pos[0]<pos[2] && pos[1]<pos[3]) /* faccio il test richiesto */
    {
	obstacle_t *obstacle = malloc(sizeof(obstacle_t));
	if (!obstacle) return NULL; /* si spera che non capiti */
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
    sprintf(s, "%d %d %d %d", po.s_i, po.s_j, po.d_i, po.d_j);
    return (*s);
}

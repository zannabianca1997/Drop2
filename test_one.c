/*
  \file
  \authors informatica (CDS Fisica) 2016/17
  \brief assegnamento2: primo file di test
 */
#include <stdio.h>
#include <stdlib.h>
#include "drop.h"

/* numeri di righe/colonne (Test1) */
#define N1 10
#define M1 20
#define N2 1000
#define M2 2000

/* test di creazione, inizializzazione, stampa e cancellazione matrici */
int main (void) {
  /* conterra' il puntatore alla matrice */
  char ** a,**b;

  /* Test 1.0: test valori malformati */
  init_matrix(NULL,N1,M1);
  init_matrix(a,0,M1);


  /* Test 1.1 */
  fprintf(stdout,"Test 1.1: \n");

  a = new_matrix(N1,M1);
  
  /* inizializzazione della matrice a EMPTY*/
  init_matrix(a,N1,M1);

  /* stampa */
  fprint_matrix(stdout,a,N1,M1);
  free_matrix(&a,N1);
  if (a != NULL) {
      fprintf(stderr,"Test 1.1: a puntatore non nullo \n");
    return EXIT_FAILURE;
  }
    /* Test 1.2 */
  fprintf(stdout,"Test 1.2: \n");

  a = new_matrix(N2,M2);
  b = new_matrix(N1,M2);
  
  /* inizializzazione della matrice a EMPTY*/
  init_matrix(a,N2,M2);
  init_matrix(b,N1,M2);

  /* stampa */
  fprint_matrix(stdout,a,N2,M2);
  fprint_matrix(stdout,b,N1,M2);
  free_matrix(&a,N2);
  free_matrix(&b,N1);
  
  if ( a != NULL || b != NULL ) {
    fprintf(stderr,"Test 1.1: a o b puntatore non nullo \n");
    return EXIT_FAILURE;
  }
  
  return 0;
}

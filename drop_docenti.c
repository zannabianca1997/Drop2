/**
  \file
  \author inf 2016/17
  \brief assegnamento 2: funzioni di allocazione e stampa fornite dai docenti
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "drop.h"

char** new_matrix (unsigned n, unsigned m)  {
  char ** a;
  int i;

  if ( n == 0 || m == 0 ) {
    errno = EINVAL;
    return NULL;
  }

  /* allocazione vettore puntatori alle righe */
  if ( ( a = malloc(n * sizeof (char *) ) ) == NULL )  {
    return NULL;
  }

  /* inizializzo (serve per il rollback) */
  for (i=0;i<n; i++)
    a[i]=NULL;

  /* allocazione righe*/
  for (i=0;i<n; i++)
    if ( (a[i]=malloc(m * sizeof (char) ) ) == NULL )  {
      /* rollback */
      int j;
      for(j=0;j<i;j++)
        free(a[j]);
      free(a);
      errno=ENOMEM;
      return NULL;
    }
  return a;
}


void fprint_matrix (FILE * f, char ** a, unsigned n, unsigned m) {
  int i,j;

  /* file non utilizzabile */
  if ( f == NULL ) {
    fprintf(stderr,"print_matrix: File NULL.\n");
    return;
  }

  /* valori inconsistenti */
  if ( n == 0 || m == 0 ) {
    fprintf(stderr,"print_matrix: 0 colonne o righe.\n");
    return;
  }

  /* matrice vuota */
  if ( a == NULL ) {
    fprintf(f,"Matrice vuota.\n");
    return;
  }

  /* stampa matrice */
  for (i=0;i<n; i++) {
    for (j=0;j<m; j++)
      if ( a[i][j] == EMPTY ) fprintf(f, "%c", EMPTY );
      else if ( a[i][j] == FULL ) fprintf(f, "%c", FULL );
      else if ( a[i][j] == OBSTACLE) fprintf(f, "%c", OBSTACLE );
      else fprintf(f, "?"); /* carattere ignoto */
    fprintf(f, "\n" );
  }
  fprintf(f, "\n" );

}








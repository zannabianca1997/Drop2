/**
  \file
  \authors Giuseppe Zanichelli Bruno Bucciotti
  \brief Generatore di numeri casuali(ASSOLUTAMENTE NON SICURO), implementando un Linear Congruence Generator.
*/

#include "myrand.h"

/*
Per semplicità implemento un tipo di generatore casuale estremamente semplice, un Linear Congruence Generator.
Se avrò tempo lo migliorerò.

Il LCG funziona così:

Ci sono tre parametri: m, a, c, con  0 < m, 0 < a < m, 0<= c < m.
Inoltre un seed iniziale X_0, con 0 <= X_0 < m (comunque verrà fatto il modulo)

Dato X_i l'ultimo risultato del generatore, il successivo è:
X_{i+1} = (a*X_n + c) % m

Per avere il periodo completo del generatore serve che:
1. m e c siano primi fra loro
2. a-1 sia divisibile per tutti i fattori primi di m
3. a-1 sia divisibile per 4 se m è divisibile per 4

Inoltre il numero da ritornare deve essere compreso fra 0-9999, sarà perciò usato X_i % 1000.
Utilizzare 1000 come m permetterebbe di evitare questo problema ma darebbe un periodo (1000) decisamente troppo corto.

Come M verrà usata una potenza di 2: questo renderà più efficiente l'operazione di modulo.
*/

#define M_EXPONENT 15 // vedi sotto(commento su lastnumber) per spiegazioni sul perchè così piccolo
#define A 21345 // controllato per le condizioni 2. e 3.
#define C 1 // coprimo con M


/* contiene l'ultimo numero generato, vorrei usare un uint32_t, avendo molti generatori un periodo di 2^32.
Purtroppo non potendo modificare l'header sono costretto a usare int, che è assicurato di avere "al minimo" 16 bit, di cui uno viene perso per il segno.
Siamo quindi costretti a usare come m solo 2^15, corrispondente a 32768, che comunque nelle simulazioni di cui si parla nei test non viene raggiunto.
Ovviamente per la maggior parte dei calcolatori sarà invece a 32/64 bit, ma per la portabilità sono costretto a sprecare gli altri.
*/
int lastnumber;

/* Macro per fare il modulo con m. Essendo m una potenza di due, è sufficente estrarre gli ultimi M_EXPONENT bit*/
#define M_MASK ((1 << M_EXPONENT) - 1)
#define MOD_M(x) ((x) & M_MASK)


/** Inizializza il generatore di numeri casuali con un seme, essendo il LCG estremamente semplice e con un solo int come valore, basta inserirlo nell'ultimo valore generato
  \param seed il valore del seme

*/
void my_srand(int seed)
{
    lastnumber = MOD_M(seed); // inizializzo, assicurandomi che il risultato sia valido
}


/** genera il prossimo valore a partire dal valore contenuto nella variabile globale lastnumber, ne restituisce il modulo 1000.

    \retval n il prossimo numero della sequenza pseudocasuale compreso fra 0 e 9999

*/
int my_rand(void)
{
    lastnumber = MOD_M(A*lastnumber + C);
    return (lastnumber % 1000); // optimization is crying right now :(
    //TODO: Questo passaggio lascia inoltre a desiderare. Essendo 2^15 non zero modulo 1000, i  numeri da 0 a 768 hanno circa 1/32 di probabilità in più di venire estratti. Da riparare.
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define N 1000

/*
 * TODO: per semplicità assumiamo che il testo sia formatatto in maniera corretta. Implementare funzione che elimina tutti gli eventuali doppi spazi
*/

/*
 * Si è deciso di creare la lista di caratteri entranti in una riga al fine di non sovraccaricare la memoria
 */

typedef struct {
    char *parola;
    unsigned short size;
    struct Parola* next;
} Parola;

Parola* componiLista(Parola*, Parola**, char*, int, int*);
Parola* creaNodo(char*);
bool isEscapeChar(char);
Parola* pop(Parola**);
Parola* push(Parola*, Parola*, Parola**);

int main() {
    char buff[N] = "string.h e' l'header file della libreria standard del C che contiene definizioni di macro, costanti e dichiarazioni di funzioni e tipi usati non solo nella manipolazione delle stringhe ma anche nella manipolazione della memoria.\n\0";
    int L = 15;
    int i = 0;
    Parola *testa = NULL;
    Parola *coda = NULL;

    while (i < strlen(buff)) {
        Parola* testa_riga = NULL;
        Parola* coda_riga = NULL;
        int k;
        testa = componiLista(testa, &coda, &(buff[i]), L, &k);
        /* L'indice viene spostato indietro nel caso si sono lette lettere ma la parola non era finita */
        i += L  - (L - k);
        if(buff[i] == ' ') {
            i++;
        }
        int caratteri = 0, n_words = 0;
        bool printable = false;
        while (!printable) {
            Parola* nodo = pop(&testa);
            /* Se ancora non raggiungo L caratteri stampabili accodo la parola nella lista delle parole della riga */
            if (nodo != NULL && caratteri + nodo->size < L) {
                caratteri += nodo->size + 1;            // TODO: controllare il comportamento qui. Potrebbe accodare uno spazio alla fine dell'ultima parola della riga
                testa_riga = push(nodo, testa_riga, &coda_riga);
                n_words++;
            }
            /* Se i caratteri coprono perfettamente la lunghezza L significa che tutte le parole tra loro hanno uno spazio */
            else if ( (nodo == NULL && caratteri == L) || (nodo != NULL && caratteri + nodo->size == L) ) {
                if (nodo != NULL && caratteri + nodo->size == L) {
                    n_words++;
                }
                /* Stampiamo tutte le parole con uno spazio tra esse */
                while (n_words > 0) {
                    Parola *p = pop(&testa_riga);
                    printf("%s", p->parola);
                    if (n_words != 1) {
                        printf(" ");
                    } else {
                        printf("\n");
                    }
                    free(p);
                    n_words--;
                }
                printable = true;
            }
            /* Se i caratteri sono maggiori rispetto alla lunghezza L della riga significa che dovrà esserci una giustificazione */
            else {
                int space_to_place = L - caratteri;
                int space_each_world = space_to_place / (n_words-1);
                while(n_words > 1) {
                    Parola *p = pop(&testa_riga);
                    printf("%s ", p->parola);
                    for(int i = 0; i < space_each_world; i++) {
                        printf(" ");
                    }
                    space_to_place -= space_each_world;
                    free(p);
                    n_words--;
                }
                Parola *p = pop(&testa_riga);
                for (int j = 0; j < space_to_place; ++j) {
                    printf(" ");
                }
                printf("%s\n", p->parola);
                free(p);
                n_words--;
                printable = true;
            }
        }

    }

    return 0;
}

/**
 * Crea una lista di parole da un buffer. Dal buffer verranno letti massimo L caratteri
 * @param buffer buffer dal quale leggere i caratteri
 * @param L massimo numero di caratteri da leggere. Rappresenta la lunghezza della riga
 * @param k eventuali caratteri letti in più
 * @return la testa della lista
 */
Parola* componiLista(Parola* testa, Parola** coda, char* buffer, int L, int* k) {
    int caratteri = 0;          // numero di caratteri letti fino ad un certo istante
    int is_word = 0, i = 0, j = 0;
    *k = -1;
    char temp[L+1];
    memset(temp, 0, sizeof(temp));
    while (caratteri < L) {
        if((int)buffer[i] >= 33 && (int)buffer[i] <= 133) {
            if (*k == -1) {
                *k = i;
            }
            temp[j] = buffer[i];
            caratteri++;
            is_word = 1;
        }
        // creiamo la parola
        else if( is_word && isEscapeChar(buffer[i]) ) {
            Parola *p = creaNodo(temp);
            memset(temp, 0, sizeof(temp));
            /* Se la testa è null significa che il nodo è il primo della lista */
            if (testa == NULL) {
                testa = p;
                *coda = p;
            }
            /* Altrimenti bisogna aggiungere il nodo alla coda */
            else {
                (*coda)->next = p;
                *coda = p;
            }
            j = -1;
            *k = -1;
        }
        i++;
        j++;
    }

    /* Se ho letto L caratteri e is_word è settata significa che stavo leggendo una parola.
     * Se incontro un carattere di escape significa che la parola è effettivamente finita e può essere salvata */
    if ( is_word && isEscapeChar(buffer[i]) ) {
        Parola *p = creaNodo(temp);
        if (testa == NULL) {
            testa = p;
            *coda = p;
        }
            /* Altrimenti bisogna aggiungere il nodo alla coda */
        else {
            (*coda)->next = p;
            *coda = p;
        }
        *k = 15;
    }
    /* Se la parola non è ancora finita si setta il numero di caratteri letti che non formano una parola completa */

    return testa;
}

/**
 * Alloca uno spazio di memoria per una struttura di tipo Parola e ne ritorna il relativo puntatore all'indirizzo di
 * memoria
 * @param buff stringa da memorizzare all'interno del campo 'parola' della struct
 * @return il puntatore alla zona di memoria dove è stata allocata la struct, NULL in caso di errore
 */
Parola* creaNodo(char *buff) {
    Parola *p = (Parola*) malloc(sizeof(Parola));
    if (p != NULL) {
        int len = strlen(buff);
        p->parola = (char *) malloc(len + 1);
        strncpy(p->parola, buff, len);
        p->parola[len] = '\0';
        p->size = len;
        p->next = NULL;
    }
    return p;
}

/**
 * Controlla se un determinato carattere passato come paramentro è un carattere tra '\0', '\n' e ' '
 * @param c carattere da controllare
 * @return true se appartiene all'insieme dei caratteri non ammessi, false altrimenti
 */
bool isEscapeChar(char c) {
    if (c == ' ' || c == '\0' || c == '\n') {
        return true;
    }
    return false;
}

/**
 * Estrae un nodo dalla testa della lista
 * @param testa la testa della lista dalla quale si vuole estrarre
 * @return il nodo 'Parola' se presente, altrimenti NULL
 */
Parola* pop(Parola** testa) {
    if(*testa == NULL) {
        return NULL;
    }
    Parola* temp = *testa;
    *testa = (*testa)->next;
    return temp;
}

/**
 * Inserisce un nodo 'Parola' all'interno di una lista
 * @param p nodo che si vuole inserire
 * @param testa testa della lista nella quale si vuole inserire la 'Parola'
 * @param coda coda della lista nella quale si vuole inserire la 'Parola'
 * @return la testa della lista
 */
Parola* push(Parola* p, Parola* testa, Parola** coda) {
    if (testa == NULL) {
        testa = p;
        *coda = testa;
    }
    else {
        (*coda)->next = p;
        *coda = p;
    }
    return testa;
}
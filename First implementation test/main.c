#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <wchar.h>
#define N 1000

/*
 * TODO: per semplicità assumiamo che il testo sia formattato in maniera corretta. Implementare funzione che elimina tutti gli eventuali doppi spazi
*/

/*
 * TODO: conto prima le parole perché reallocando l'array mano a mano potrei avere aree di memoria diverse e perdere tutti i nodi già inseriti se non li copio di volta in volta, questo crea una complessità computazionale maggiore. La funzione realloc non garantisce che l'area di memoria rimanga sempre la stessa
 */


/**
 * La struttura definisce il modo in cui verrà memorizzata una parola
 * @var parola è la stringa da memorizzare come una parola. Consiste in un array.
 * @var size indica la dimensione della stringa memorizzata. è di tipo unsigned short int perchè le parole devono avere una lunghezza > 0
 */
typedef struct {
    char *parola;
    unsigned short size;
} Parola;

/**
 * La struttura definisce il modo in cui verrà memorizzata una riga da stampare
 * @var riga è la riga, sotto forma di stringa, da stampare
 * @var size è la dimensione delle stringa da stampare
 * @var next è un puntatore che referenzia alla prossima riga da stampare
 */
typedef struct {
    char *riga;
    unsigned short size;
    struct Riga *next;
} Riga;

void build_array(Parola** parole, int n_word, char *buff);
void free_buff(void* buff, int size);
bool isEscapeChar(char c);
Parola* crea_nodo(char* buff);
int count_words(char* buff);
Riga* full_Justyfy(Parola** parole, int n_words, int L, Riga *testa, Riga **coda, int *nRighe);
bool check_word_length(char *parola, int L);
Riga* left_justify(Parola** parole, int i, int j, int numbersOfSpaces, Riga *testa, Riga *coda, int rowLenght);
Riga* middle_justify(Parola** parole, int i, int j, int numberOfSpaces, Riga *testa, Riga *coda, int rowLenght);
Riga* crea_riga(char *buff);
Riga* push(Riga *element, Riga *testa, Riga **coda);
void stampa_lista(Riga *testa);
Riga** build_array_righe(Riga *testa, int nRighe);
void format_page(Riga **righe, int nRighe, int colonne);
FILE *create_file(char *name);


int main() {
    char buff[N] = "string.h e' l'header file della libreria standard del C che contiene definizioni di macro, costanti e dichiarazioni di funzioni e tipi usati non solo nella manipolazione delle stringhe ma anche nella manipolazione della memoria.\n\0";
    Parola** parole = NULL;
    int n_parole;
    int L;
    Riga* testa_righe = NULL;
    Riga* coda_righe = NULL;
    int nRighe = 0;

    printf("Inserisci larghezza di una riga: ");
    scanf("%d", &L);

    n_parole = count_words(buff);
    parole = (Parola**) malloc(sizeof(Parola)*n_parole);

    build_array(parole, n_parole, buff);
    testa_righe = full_Justyfy(parole, n_parole, L, testa_righe, &coda_righe,&nRighe);

    //printf("==========================\n");
    //stampa_lista(testa_righe);
    Riga **righe = build_array_righe(testa_righe, nRighe);
    printf("==========================\n");
    format_page(righe, nRighe, 4);


}

/**
 * Inserisci all'interno di un array di puntatori a struct 'Parola' una quantità determinata di parole
 * @param parole array dove inserire le varie strutture di tipo 'Parola'
 * @param n_words numero di elementi dell'array (corrisponde al numero di parole)
 * @param buff il buffer dal quale leggere le parole
 */
void build_array(Parola** parole, int n_words, char *buff) {
    char temp[N];
    free_buff(temp, N);
    bool is_word = false;
    int current_n_words = 0;
    for (int i = 0, j = 0; current_n_words < n_words;) {
        if (!isEscapeChar(buff[i])) {
            if (!is_word) {
                is_word = 1;
            }
            temp[j] = buff[i];
            j++;
        }
        else if (isEscapeChar(buff[i]) && is_word) {
            Parola *p = crea_nodo(temp);
            current_n_words++;
            parole[current_n_words - 1] = p;
            free_buff(temp, strlen(temp));
            is_word = false;
            j = 0;
        }
        i++;
    }
}

/**
 * Pulisce un buffer inizializzandolo completamente a 0
 * @param buff buffer da pulire
 * @param size dimensione del buffer da pulire
 */
void free_buff(void* buff, int size) {
    memset(buff, 0, size);
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
 * Alloca uno spazio in memoria per contenere una struttura di tipo 'Parola'
 * @param buff
 * @return
 */
Parola* crea_nodo(char* buff) {
    Parola* nodo = NULL;
    nodo = (Parola *) malloc(sizeof(Parola));

    int len_word = strlen(buff);
    char* parola = (char*) malloc(len_word);
    strcpy(parola, buff);
    nodo->parola = parola;
    nodo->size = len_word;

    return nodo;
}

/**
 * Conta il numero di parole all'interno di un buffer
 * @param buff buffer dal quale leggere le parole
 * @return il numero di parole contenute nel buffer
 */
int count_words(char* buff) {
    int n_words = 0;
    bool is_word = false;
    for (int i = 0; buff[i] != '\0'; ++i) {
        if (!(isEscapeChar(buff[i]))) {
            if (!is_word) {
                is_word = true;
            }
        } else if (isEscapeChar(buff[i]) && is_word) {
            n_words++;
        }
    }
    return n_words;
}

/**
 * Si occupa della giustificazione di un testo passato come array di parole
 * @param parole array di puntattori a struct di tipo 'Parola'
 * @param n_words numero di parole all'interno dell'array
 * @param L indica la lunghezza massima della riga
 */
Riga* full_Justyfy(Parola** parole, int n_words, int L, Riga *testa, Riga **coda, int *nRighe) {
    // L'idea è quella di usare due indici che indicano la prima parola da inserire all'interno della riga e la parola da inserire nella riga successiva da inserire all'interno della riga
    int i = 0;
    while (i < n_words) {
        int j = i + 1;
        // lineLenght: indica la quantità degli effettivi caratteri da stampare
        int lineLenght = parole[i]->size;
        check_word_length(parole[i]->parola, L);
        // j - i - 1: mi fa capire quanti spazi minimi devo inserire
        while (j < n_words && (lineLenght + parole[j]->size + (j - i - 1) < L) ) {
            lineLenght += parole[j]->size;
            j++;
        }

        int nSpaces = L - lineLenght;
        int numberOfWords = j - i;
        // Se la condizione è verificata devo semplicemente fare una giustificazione verso sinistra
        if (numberOfWords == 1 || j >= n_words) {
            testa = left_justify(parole, i, j, nSpaces, testa, &(*coda), L);
        }
        else {
            testa = middle_justify(parole, i, j, nSpaces, testa, &(*coda), L);
        }
        (*nRighe)++;
        i = j;
    }
    return testa;
}

/**
 * Verifica se una parola può essere contenuta totalmente all'interno di una riga vuota lunga L
 * @param parola stringa da verificare
 * @param L lunghezza della riga
 * @return true se la parola può essere inserita, altrimenti uccide il processo
 */
bool check_word_length(char *parola, int L) {
    if (strlen(parola) > L) {
        printf("%s è una parola inammissibile: è più lunga di %d caratteri", parola, L);
        fflush(stdout);     // forzo la scrittura prima di uccidere il processo
        kill(getpid(), SIGKILL);
        return false;
    }
    return true;
}

/**
 * Permette di stampare una stringa allineandola a sinistra. Funziona come una printf classica
 * @param parole array di parole di tipo 'Parola'
 * @param i indice della prima parola da stampare
 * @param j indice dell'ultima parola da stampare
 * @param numberOfSpaces numero di spazi totali da partizionare all'interno della riga
 */

Riga* left_justify(Parola** parole, int i, int j, int numbersOfSpaces, Riga *testa, Riga *coda, int rowLenght) {

    char riga[rowLenght+1];
    free_buff(riga, rowLenght+1);
    char *ptr = riga;                           // necessario per concatenare le varie parole nella riga senza perdere la testa dell'array di char

    if (j - i == 1) {
        ptr += sprintf(ptr, "%s", parole[i]->parola);
        printf("%s", parole[i]->parola);
        for (int k = 0; k < numbersOfSpaces; ++k) {
            printf(" ");
            ptr += sprintf(ptr, " ");
        }
    }
    else {
        for (;i < j ; ++i) {
            ptr += sprintf(ptr, "%s", parole[i]->parola);
            printf("%s", parole[i]->parola);
            if (i+1 != j) {
                printf(" ");
                ptr += sprintf(ptr, " ");
            }
        }
    }

    printf("\n");

    Riga *r = crea_riga(riga);
    testa = push(r, testa, &(*coda));
    return testa;
}

/**
 * Si occupa di formattare una riga giustificadola completamente
 * @param parole array di puntatotori a parole di tipo 'Parola'
 * @param i indice indicante la posizione nell'array della prima parola della riga da stampare
 * @param j indice indicante la posizione nell'array della parola successiva all'ultima da stampare
 * @param numberOfSpaces numero di spazi totali da partizionare all'interno della riga
 */
Riga* middle_justify(Parola** parole, int i, int j, int numberOfSpaces, Riga *testa, Riga *coda, int rowLenght) {

    char riga[rowLenght+1];
    free_buff(riga, rowLenght+1);
    char *ptr = riga;                                       // necessario per concatenare le varie parole nella riga senza perdere la testa dell'array di char

    int spaceNeeded = j - i - 1;                            // numero di spazi minimi necessari (uno tra ogni parola), rappresenta anche in quante parti bisogna dividere gli spazi totali
    int effectiveSpace = numberOfSpaces / spaceNeeded;      // quanti spazi vanno inseriti tra due parole
    int extraSpace = numberOfSpaces % spaceNeeded;          // potrebbero dover essere redistribuiti alcuni spazi perchè la quantità di spazi totali è un numero dispari

    for (; i < j; ++i) {
        printf("%s", parole[i]->parola);
        ptr += sprintf(ptr, "%s", parole[i]->parola);
        for (int k = 0; k < effectiveSpace && numberOfSpaces > 0; ++k, --numberOfSpaces) {
            printf(" ");
            ptr += sprintf(ptr, " ");
        }
        if (extraSpace > 0) {
            printf(" ");
            ptr += sprintf(ptr, " ");
            extraSpace--;
            numberOfSpaces--;
        }
    }
    printf("\n");

    Riga *r = crea_riga(riga);
    testa = push(r, testa, &(*coda));
    return testa;
}

/**
 * Alloca uno spazio in memoria per memorizzare una struttura di tipo 'Riga'
 * @param buff stringa terminante con carattere '\0' da memorizzare sotto forma di riga
 * @return il puntatore di tipo 'Riga' all'area di memoria allocata
 */
Riga *crea_riga(char *buff) {
    Riga* riga = malloc(sizeof(Riga));
    if (riga == NULL) {
        printf("Impossibile creare un nodo 'Riga'!");
        fflush(stdout);
        kill(getpid(), SIGSEGV);
    }
    riga->size = strlen(buff) + 1;
    char *line = (char*) malloc(riga->size);
    if (line == NULL) {
        printf("Impossibile riservare spazio per la stringa da stampare.");
        fflush(stdout);
        kill(getpid(), SIGSEGV);
    }
    free_buff(line, riga->size);
    strncpy(line, buff, riga->size - 1);
    riga->riga = line;
    riga->next = NULL;
    return riga;
}

/**
 * Permette di inserire un elemento di tipo 'Riga' all'interno di una lista a puntatori (lista implementata con politica FIFO)
 * @param element elemento generico da inserire all'interno di una lista
 * @param testa puntatore al primo elemento della lista
 * @param coda puntatore all'ultimo elemento della lista (passato per referenza)
 * @return la testa della lista (la funzione ritorna anche la coda della lista per mezzo del doppio puntatore)
 */
Riga* push(Riga *element, Riga *testa, Riga **coda) {
    if (testa == NULL) {
        testa = element;
    } else {
        (*coda)->next = element;
    }
    *coda = element;
    return testa;
}

/**
 * Dato il puntatore alla testa di una lista di tipo 'Riga', la funzione si occupa di stampare tutti gli elementi all'interno della lista
 * @param testa puntatore alla testa della lista
 */
void stampa_lista(Riga *testa) {
    Riga *temp = testa;
    while (temp != NULL) {
        printf("%s\n", temp->riga);
        temp = temp->next;
    }
}

/**
 * Permette di creare un array di puntatori a elmenti di tipo 'Riga' a partire da una lista di righe
 * @param testa primo elemento della lista a puntatori
 * @param nRighe numero di righe totali da posizionare nell'array
 * @return un puntatore di tipo riga se il sistema operativo riesce ad allocare la memoria, NULL viceversa
 */
Riga** build_array_righe(Riga *testa, int nRighe) {
    Riga **righe = malloc(sizeof(Riga) * nRighe);
    if (righe == NULL) {
        return NULL;
    }
    Riga *temp = testa;
    int i = 0;
    while (temp != NULL) {
        righe[i] = temp;
        temp = temp->next;
        righe[i]->next = NULL;          // non serve più il collegamento al nodo successivo
        i++;
    }
    return righe;
}

/**
 * Permette di formattare le righe all'interno di una pagina
 * @param righe array di puntatori a struct di tipo 'Riga'
 * @param nRighe lunghezza dell'array
 * @param colonne numero di colonne nelle quali si vogliono disporre le linee
 */
void format_page(Riga **righe, int nRighe, int colonne) {

    int n_righe_colonna;
    int righe_extra;

    if (nRighe < colonne) {
        n_righe_colonna = nRighe;
        colonne = 1;
    }
    else {
        n_righe_colonna = nRighe / colonne;
        righe_extra = nRighe % colonne;
        if (righe_extra > 0) {
            colonne++;
        }
    }

    FILE *file = create_file("prova.txt\0");

    for (int i = 0; i < n_righe_colonna; ++i) {
        for (int j = 0; j < colonne; ++j) {
            printf("%s", righe[n_righe_colonna*j+i]->riga);
            fprintf(file, "%s", righe[n_righe_colonna*j+i]->riga);
            if (j != colonne-1) {
                printf(" | ");
                fprintf(file, " | ");
            }
        }
        printf("\n");
        fprintf(file, "\n");
    }
    fclose(file);
}

/**
 * Permette di creare un file con un nome specificato in input.
 * @param name nome da dare al file
 * @return puntatore al file se riesce a crearlo, NULL altrimenti
 */
FILE *create_file(char *name) {
    FILE *fp;
    fp = fopen(name, "w");
    return fp;
}
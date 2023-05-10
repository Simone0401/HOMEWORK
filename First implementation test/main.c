#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#define N 1000

/*
 * TODO: per semplicità assumiamo che il testo sia formatatto in maniera corretta. Implementare funzione che elimina tutti gli eventuali doppi spazi
*/

/*
 * TODO: conto prima le parole perché reallocando l'array mano a mano potrei avere aree di memoria diverse e perdre tutti i nodi già inseriti se non li copio di volta in volta, questo crea una complessità computazionale maggiore. La funzione realloc non garantisce che l'area di memoria rimanga sempre la stessa
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
void full_Justyfy(Parola** parole, int n_words, int L, Riga *testa, Riga *coda, int *nRighe);
bool check_word_length(char *parola, int L);
void left_justify(Parola** parole, int i, int j, int numbersOfSpaces, Riga *testa, Riga *coda, int *nRighe);
void middle_justify(Parola** parole, int i, int j, int numberOfSpaces, Riga *testa, Riga *coda, int *nRighe);

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
    full_Justyfy(parole, n_parole, L);


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
void full_Justyfy(Parola** parole, int n_words, int L, Riga *testa, Riga *coda, int *nRighe) {
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
            left_justify(parole, i, j, nSpaces);
        }
        else {
            middle_justify(parole, i, j, nSpaces);
        }
        i = j;
    }
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
void left_justify(Parola** parole, int i, int j, int numbersOfSpaces, Riga *testa, Riga *coda, int *nRighe) {

    if (j - i == 1) {
        printf("%s", parole[i]->parola);
        for (int k = 0; k < numbersOfSpaces; ++k) {
            printf(" ");
        }
    }
    else {
        for (;i < j ; ++i) {
            printf("%s", parole[i]->parola);
            if (i+1 != j) {
                printf(" ");
            }
        }
    }

    printf("\n");
}

/**
 * Si occupa di formattare una riga giustificadola completamente
 * @param parole array di puntatotori a parole di tipo 'Parola'
 * @param i indice indicante la posizione nell'array della prima parola della riga da stampare
 * @param j indice indicante la posizione nell'array della parola successiva all'ultima da stampare
 * @param numberOfSpaces numero di spazi totali da partizionare all'interno della riga
 */
void middle_justify(Parola** parole, int i, int j, int numberOfSpaces, Riga *testa, Riga *coda, int *nRighe) {
    int spaceNeeded = j - i - 1;                            // numero di spazi minimi necessari (uno tra ogni parola), rappresenta anche in quante parti bisogna dividere gli spazi totali
    int effectiveSpace = numberOfSpaces / spaceNeeded;      // quanti spazi vanno inseriti tra due parole
    int extraSpace = numberOfSpaces % spaceNeeded;          // potrebbero dover essere redistribuiti alcuni spazi perchè la quantità di spazi totali è un numero dispari

    for (; i < j; ++i) {
        printf("%s", parole[i]->parola);
        for (int k = 0; k < effectiveSpace; ++k) {
            printf(" ");
        }
        if (extraSpace > 0) {
            printf(" ");
            extraSpace--;
        }
    }
    printf("\n");
}
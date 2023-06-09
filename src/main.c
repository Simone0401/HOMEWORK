#include "justificationTools.h"
#include "../dist/argtable3.h"
#include <sys/types.h>
#include <sys/wait.h>

bool validate_parameter(int L, int colonne, int righe, int dist);
void processo_formattazione(int pipefd[], unsigned long buffsize, unsigned int L, unsigned int colonne, unsigned int linee, unsigned int distanza, const char* outfile);
void processo_scrittura(int pipefd[], const char* outfile);
int multiprocesso(unsigned int L, unsigned int colonne, unsigned int linee, unsigned int distanza, const char* ofile, const char* infile, int openOut);
int monoprocesso(unsigned int L, unsigned int colonne, unsigned int linee, unsigned int distanza, const char* ofile, const char* infile, int openOut);

int main(int argc, char **argv) {

    struct arg_file *outfile  = arg_file0("o",	"outfile",	"<output>",	"output file (default è \"output.txt\")");
    struct arg_lit  *help     = arg_lit0("h",	"help",                    	"stampa questo help e esce");
    struct arg_lit  *version  = arg_lit0(NULL,	"version",                 	"stampa la versione del programma e esce");
    struct arg_int  *lRiga    = arg_int0("r", 	"lungRiga", 	"<n>", 		"la lunghezza di una riga, deve essere un numero intero positivo (default 26)");
    struct arg_int  *nColonne = arg_int1("c", 	"colonne", 	"<n>", 		"il numero di colonne per pagina, deve essere un numero intero positivo (obbligatorio)");
    struct arg_int  *nLinee   = arg_int1("l", 	"numRighe", 	"<n>", 		"il numero di righe per colonna, deve essere un numero intero positivo (obbligatorio)");
    struct arg_int  *dist     = arg_int0("d", 	"distanza", 	"<n>", 		"la distanza tra una colonna e l'altra, deve essere un numero intero positivo (default 3)");
    struct arg_lit  *openOut  = arg_lit0("p", 	"printout", 			"visualizza su STDOUT il file prodotto");
    struct arg_lit  *mulProc  = arg_lit0(NULL, "mP", "consente di avviare la versione multiprocesso del programma (di default viene avviata la versione monoprocesso)");
    struct arg_file *infile   = arg_file1(NULL, NULL, 		NULL, 		"input file (obbligatorio)");
    struct arg_end  *end      = arg_end(20);
    void* argtable[] = {outfile, help, version, lRiga, nColonne, nLinee, dist, openOut, mulProc, infile, end};
    const char* progname = "justify";
    int nerrors;
    int exitcode = 0;

    /* Verifica che gli elementi di argtable[] siano stati allocati con successo */
    if (arg_nullcheck(argtable) != 0)
    {
        /* Elementi NULL rilevati, qualche allocazione non è andata a buon fine */
        printf("%s: memoria insufficiente\n",progname);
        return 1;
    }

    nerrors = arg_parse(argc, argv, argtable);

    /* Caso speciale: '--help' ha la precedenza rispetta al reporting di errori */
    if (help->count > 0) {
        printf("Uso: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Questo programma permette la generazione di un file di testo formattato con giustificazione\n"
               "completa e diviso in colonne a partire da un semplice file di testo in input. Justify accetta parametri come\n"
               "la lunghezza desiderata di una riga, il numero di colonne per pagina e il numero di righe\n"
               "per colonna; in base a questi dati genererà un file in output composto da una, o più, pagine.\n"
               "Ogni pagina è delimitata dai caratteri \\n %%%%%% \\n\n"
               "Il programma manterrà la divisione in paragrafi del testo originale (un paragrafo è\n"
               "delimitato da '\\n').\n"
               "I parametri accettati sono i seguenti:\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        exitcode = 0;
        goto exit;
    }

    /* Caso speciale: '--version' ha la precedenza rispetta al reporting di errori */
    if (version->count > 0)
    {
        printf("'%s' è un programma che permette di giustificare testi in formato 'txt' in colonne con "
               "giustificazione completa\n", progname);
        printf("Giugno 2023, Argento Simone\n");
        exitcode=0;
        goto exit;
    }

    /* Se il parser ritorna qualche errore lo stampo e termino il programma */
    if (nerrors > 0)
    {
        /* Stampa degli errori dettagliati */
        arg_print_errors(stdout, end, progname);
        printf("Prova '%s --help' per avere maggiori informazioni.\n", progname);
        exitcode=1;
        goto exit;
    }

    /* caso speciale: non sono stati forniti parametri */
    if (argc==1)
    {
        printf("Prova '%s --help' per maggiori informazioni.\n", progname);
        exitcode=0;
        goto exit;
    }

    unsigned int L = 26, colonne, linee, distanza = 3;
    char *ofile = malloc(sizeof("../output_file/output.txt")+1);
    strcpy(ofile, "../output_file/output.txt\0");

    colonne = nColonne->ival[0];
    linee = nLinee->ival[0];

    if (lRiga->count > 0) {
        L = lRiga->ival[0];
    }

    if (dist->count > 0) {
        distanza = dist->ival[0];
    }

    if (outfile->count > 0) {
        ofile = (char*) realloc(ofile, strlen(outfile->filename[0]) + 1);
        if (ofile == NULL) {
            goto exit;
        }
        free_buff(ofile, strlen(outfile->filename[0]) + 1);
        strcpy(ofile, outfile->filename[0]);
    }

    if (!validate_parameter(L, colonne, linee, distanza)) {
        printf("%s: sono stati forniti dei valori non ammissibili\n", progname);
        printf("Prova '%s --help' per maggiori informazioni.\n", progname);
        exitcode = 1;
        goto exit;
    }

    if (mulProc->count > 0) {
        exitcode = multiprocesso(L, colonne, linee, distanza, ofile, infile->filename[0], openOut->count);
    } else {
        exitcode = monoprocesso(L, colonne, linee, distanza, ofile, infile->filename[0], openOut->count);
    }

exit:
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    return exitcode;
}

/**
 * Permette di verificare la correttezza dei vari parametri in termini numerici
 * @param L lunghezza di ogni riga della colonna
 * @param colonne numero di colonne
 * @param righe numero di righe per colonna
 * @param dist distanza tra due colonne
 * @return true se i valori sono ok, false altrimenti
 */
bool validate_parameter(int L, int colonne, int righe, int dist){
    if (L <= 0) {
        return false;
    }
    if (colonne < 1) {
        return false;
    }
    if (righe < 0) {
        return false;
    }
    if (dist < 1) {
        return false;
    }
    return true;
}

/**
 * Si occupa del codice da far eseguire al processo di formattazione delle pagine
 * @param pipefd il file descriptor della pipe
 * @param buffsize dimensione del buffer che dovrà contenere il testo letto dal file
 * @param L lunghezza di ogni riga della colonna
 * @param colonne numero di colonne per pagina
 * @param linee numero di righe per colonna
 * @param distanza numero di spazi tra una coonna e l'altra
 * @param outfile è il path del file di output
 */
void processo_formattazione(int pipefd[], unsigned long buffsize, unsigned int L, unsigned int colonne, unsigned int linee, unsigned int distanza, const char *outfile) {
    char *buff = (char *) malloc(sizeof(char)*buffsize);
    if (buff == NULL) {
        printf("Errore nel processo di formattazione della pagina!\n");
        return;
    }

    /* Leggo il contenuto del file passato dal processo padre */
    read(pipefd[0], buff, buffsize);
    close(pipefd[0]);

    Parola** parole = NULL;
    unsigned int n_parole;
    Riga* testa_righe = NULL;
    Riga* coda_righe = NULL;
    unsigned int nRighe = 0;

    n_parole = count_words(buff);
    parole = (Parola**) malloc(sizeof(Parola)*n_parole);
    build_array(parole, n_parole, buff, L);
    testa_righe = full_Justyfy(parole, n_parole, L, testa_righe, &coda_righe, &nRighe);

    Riga **righe = build_array_righe(testa_righe, nRighe);

    int pipefd2[2];

    if (pipe(pipefd2) == -1) {
        printf("Errore nella comunicazione con il processo di scrittura su file\n");
        goto exit_figlio;
    }

    pid_t pid = fork();

    if (pid == -1) {
        printf("Errore nella creazione del processo di scrittura su file\n");
        goto exit_figlio;
    } else if (pid == 0) {
        close(pipefd2[1]);
        processo_scrittura(pipefd2, outfile);
        exit(EXIT_SUCCESS);
    } else {
        close(pipefd2[0]);
        format_page_multiprocess(righe, nRighe, colonne, linee, distanza, L, pipefd2, pid);
        close(pipefd[1]);
        wait(NULL);
        free_list(righe, sizeof(Riga), nRighe);
        exit(EXIT_SUCCESS);
    }

exit_figlio:
    exit(EXIT_SUCCESS);
}

/**
 * Si occupa del codice da far eseguire al processo che deve scrivere sul file output
 * @param pipefd il file descriptor della pipe nella quale transitano le stringhe da stampare sul file
 * @param outfile il path del file di output
 */
void processo_scrittura(int pipefd[], const char* outfile) {
    unsigned int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    free_buff(buffer, BUFFER_SIZE);
    ssize_t bytesRead;

    FILE *file = create_file(outfile);
    if (file == NULL) {
        printf("ERRORE APERUTRA FILE!\n");
        fflush(stdout);
        kill(getpid(), SIGKILL);
    }
    int outfilefd = fileno(file);

    while ((bytesRead = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
        /* Stampa i dati letti dalla pipe sul file di output */
        write(outfilefd, buffer, (int)bytesRead);
    }

exit_scrittura:
    fclose(file);
    close(pipefd[0]);
}

/**
 * Permette di avviare la versione multiprocesso del programma
 * @param L lunghezza di ogni riga
 * @param colonne numero di colonne desiderate
 * @param linee numero di righe per colonna
 * @param distanza numero di spazi tra due colonne
 * @param ofile nome del file di output
 * @param infile nome del file di input
 * @param openOut se settato a 1 indica che si vuole stampare il file direttamente su Shell
 */
int multiprocesso(unsigned int L, unsigned int colonne, unsigned int linee, unsigned int distanza, const char* ofile, const char* infile, int openOut) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        printf("Errore nella creazione della pipe. Uscita in corso ...\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        printf("Errore nella creazione del processo di formattazione\n");
        return 1;
    } else if (pid == 0) {
        /* è il processo figlio, chiudo l'estremità di scrittura nella pipe del processo figlio */
        close(pipefd[1]);
        unsigned long buffsize;
        read(pipefd[0], &buffsize, sizeof(buffsize));
        processo_formattazione(pipefd, buffsize, L, colonne, linee, distanza, ofile);
        exit(EXIT_SUCCESS);
    } else {
        /* Siamo nel processo padre, si deve occupare di leggere il contenuto del file in input. */
        /* Chiudo l'estremità di lettura nella pipe del processo padre. */
        close(pipefd[0]);

        char *buff = NULL;
        FILE *input = fopen(infile, "r");
        buff = read_from_file(input);
        fclose(input);

        unsigned long buffsize = strlen(buff);
        write(pipefd[1], &buffsize, sizeof(buffsize));
        write(pipefd[1], buff, buffsize);
        close(pipefd[1]);

        int status;
        wait(&status);
        if (WIFSIGNALED(status) && (WTERMSIG(status) == SIGKILL)) {
            return 1;
        }
        free_buff(buff, sizeof(buff));
        free(buff);

        if (openOut > 0) {
            FILE *print = fopen(ofile, "r");
            buff = read_from_file(print);
            printf("%s", buff);
            fclose(print);
        } else {
            printf("FILE CREATO CORRETTAMENTE!\nVai nella cartella 'output_file' per vedere il risultato\n");
        }
    }
    return 0;
}

/**
 * Permette di avviare la versione monoprocesso del programma
 * @param L lunghezza di ogni riga
 * @param colonne numero di colonne desiderate
 * @param linee numero di righe per colonna
 * @param distanza numero di spazi tra due colonne
 * @param ofile nome del file di output
 * @param infile nome del file di input
 * @param openOut se settato a 1 indica che si vuole stampare il file direttamente su Shell
 */
int monoprocesso(unsigned int L, unsigned int colonne, unsigned int linee, unsigned int distanza, const char* ofile, const char* infile, int openOut) {
    char *buff = NULL;
    Riga* testa_righe = NULL;
    Riga* coda_righe = NULL;
    int nRighe = 0, n_parole;
    Parola** parole = NULL;

    FILE *input = fopen(infile, "r");
    buff = read_from_file(input);
    fclose(input);

    n_parole = count_words(buff);
    parole = (Parola**) malloc(sizeof(Parola)*n_parole);

    build_array(parole, n_parole, buff, L);
    testa_righe = full_Justyfy(parole, n_parole, L, testa_righe, &coda_righe, &nRighe);

    Riga **righe = build_array_righe(testa_righe, nRighe);
    format_page(righe, nRighe, colonne, linee, distanza, L, ofile);
    free_list(righe, sizeof(Riga), nRighe);

    if (openOut > 0) {
        FILE *print = fopen(ofile, "r");
        buff = read_from_file(print);
        printf("%s", buff);
        fclose(input);
    } else {
        printf("FILE CREATO CORRETTAMENTE!\nVai nella cartella 'output_file' per vedere il risultato\n");
    }

    return 0;
}
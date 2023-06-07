#include "justificationTools.h"
#include "../dist/argtable3.h"

int main(int argc, char **argv) {

    struct arg_file *outfile  = arg_file0("o",	"outfile",	"<output>",	"output file (default è \"output.txt\")");
    struct arg_lit  *help     = arg_lit0("h",	"help",                    	"stampa questo help e esce");
    struct arg_lit  *version  = arg_lit0(NULL,	"version",                 	"stampa la versione del programma e esce");
    struct arg_int  *lRiga    = arg_int0("r", 	"lungRiga", 	"<n>", 		"la lunghezza di una riga (default 26)");
    struct arg_int  *nColonne = arg_int1("c", 	"colonne", 	"<n>", 		"il numero di colonne per pagina (obbligatorio)");
    struct arg_int  *nLinee   = arg_int1("l", 	"numRighe", 	"<n>", 		"il numero di righe per colonna (obbligatorio)");
    struct arg_int  *dist     = arg_int0("d", 	"distanza", 	"<n>", 		"la distanza tra una colonna e l'altra (default 3)");
    struct arg_lit  *openOut  = arg_lit0("p", 	"printout", 			"visualizza su STDOUT il file prodotto");
    struct arg_file *infile   = arg_file1(NULL, NULL, 		NULL, 		"input file (obbligatorio)");
    struct arg_end  *end      = arg_end(20);
    void* argtable[] = {outfile, help, version, lRiga, nColonne, nLinee, dist, openOut, infile, end};
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
               "giustificazione completa\n",progname);
        printf("Giugno 2023, Argento Simone\n");
        exitcode=0;
        goto exit;
    }

    /* Se il parser ritorna qualche errore lo stampo e termino il programma */
    if (nerrors > 0)
    {
        /* Stampa degli errori dettagliati */
        arg_print_errors(stdout,end,progname);
        printf("Prova '%s --help' per avere maggiori informazioni.\n",progname);
        exitcode=1;
        goto exit;
    }

    /* caso speciale: non sono stati forniti parametri */
    if (argc==1)
    {
        printf("Prova '%s --help' per maggiori informazioni.\n",progname);
        exitcode=0;
        goto exit;
    }

    Parola** parole = NULL;
    int n_parole;
    int L = 26, colonne, linee, distanza = 3;
    Riga* testa_righe = NULL;
    Riga* coda_righe = NULL;
    int nRighe = 0;
    char *ofile = malloc(sizeof("../output_file/output.txt")+1);
    strcpy(ofile, "../output_file/output.txt\0");

    char *buff = NULL;
    FILE *input = fopen(infile->filename[0], "r");
    buff = read_from_file(input);
    fclose(input);

    colonne = nColonne->ival[0];
    linee = nLinee->ival[0];

    if (lRiga->count > 0) {
        L = lRiga->ival[0];
    }

    if (dist->count > 0) {
        distanza = dist->ival[0];
    }

    if (outfile->count > 0) {
        ofile = (char*) realloc(ofile, sizeof(outfile->filename[0]) + 1);
        if (ofile == NULL) {
            goto exit;
        }
        free_buff(ofile, sizeof(outfile->filename[0]) + 1);
        strcpy(ofile, outfile->filename[0]);
    }

    n_parole = count_words(buff);
    parole = (Parola**) malloc(sizeof(Parola)*n_parole);

    build_array(parole, n_parole, buff, L);
    testa_righe = full_Justyfy(parole, n_parole, L, testa_righe, &coda_righe, &nRighe);

    Riga **righe = build_array_righe(testa_righe, nRighe);
    format_page(righe, nRighe, colonne, linee, distanza, L, ofile);
    free_list(righe, sizeof(Riga), nRighe);

    if (openOut->count > 0) {
        FILE *print = fopen(ofile, "r");
        buff = read_from_file(print);
        printf("%s", buff);
        fclose(input);
    } else {
        printf("FILE CREATO CORRETTAMENTE!");
    }

    exit:
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    return exitcode;
}

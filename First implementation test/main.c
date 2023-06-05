#include "justificationTools.h"

int main() {
    Parola** parole = NULL;
    int n_parole;
    int L, colonne, linee, distanza;
    Riga* testa_righe = NULL;
    Riga* coda_righe = NULL;
    int nRighe = 0;

    char *buff = NULL;
    FILE *input = fopen("../../input2.txt", "r");
    buff = read_from_file(input);
    fclose(input);

    printf("Inserisci larghezza di una riga: ");
    scanf("%d", &L);
    printf("Inserisci numero di colonne per pagina: ");
    scanf("%d", &colonne);
    printf("Inserisci numero di righe per colonna: ");
    scanf("%d", &linee);
    printf("Inserisci distanza desiderata tra una colonna e l'altra: ");
    scanf("%d", &distanza);

    n_parole = count_words(buff);
    parole = (Parola**) malloc(sizeof(Parola)*n_parole);

    build_array(parole, n_parole, buff, L);
    testa_righe = full_Justyfy(parole, n_parole, L, testa_righe, &coda_righe,&nRighe);

    Riga **righe = build_array_righe(testa_righe, nRighe);
    format_page(righe, nRighe, colonne, linee, distanza, L);
    free_list(righe, sizeof(Riga), nRighe);
    printf("FILE CREATO CORRETTAMENTE!");
    return 0;
}
# JUSTIFICATION TOOL
<sub>_"Per una corretta visualizzazione grafica del file si consiglia la lettura direttamente tramite GitHub"_<sub>

Il progetto si pone l'obiettivo di fornire un programma, lanciabile tramite Shell, in grado di formattare un file di testo codificato in
formato ASCII o UTF-8 in un file di testo giustificato completamente e diviso in colonne.
L'utilizzatore potrà lanciare il comando fornendo alcuni parametri di input (_verrano analizzati in seguito_) e otterà un nuovo file, con
estensione '_.txt_', che manterrà la divisione in paragrafi del testo originale ma sarà formattato in modo giustificato su più pagine e
diviso per colonne. In breve si può dire che il nuovo file avrà l'aspetto di "alcune pagine di un giornale".
Nel proseguio del documento verranno mostrate alcune immaggini esemplicative.

## Requisiti

Verranno elencati di seguito i requisiti per la compilazione e successiva esecuzione del programma:

- Compilatore C: **gcc**. Il makefile utilizza questo compilatore, non è garantito il funzionamento con compilatori diversi.
- Librerie di sistema: **stdio.h**, **string.h**, **stdlib.h**, **stdbool.h**, **signal.h**, **unistd.h**, **sys/wait.h**.
- Libreria esterna per analisi dei parametri passati tramite Shell: **argtable3.h**. (_https://www.argtable.org/_).
- Sistema operativo: 6.0.0-kali3-amd64 (il programma dovrebbe funzionare su qualsiasi distro Unix ma non si garantisce il corretto funzionamento).

## Installazione

Verranno di seguito elencati i passaggi per l'installazione e l'esecuzione del programma:

1. Clonare il repository `https://github.com/Simone0401/HOMEWORK.git` oppure scaricare la release v1.1 al seguente link https://github.com/Simone0401/HOMEWORK/releases
2. Compilare il codice sorgente: `gcc -Wall -Wextra -o src/justify src/main.c dist/argtable3.c -lm`. Si consiglia di utilizzare il Makefile con il comando `make`, esso genererà in automatico l'eseguibile e lo posizionerà in src/.
3. Eseguire il programma: spostarsi nella cartella src/ ed eseguire `./justify`

## Utilizzo

Si consiglia, una volta compilato il file sorgente, di eseguire il comando `./justify --help`, in questo modo si avrà una panoramica completa del funzionamento e delle
possibili opzioni passabili al programma. Riassumendo:

1. Eseguire il programma: `./justify --help`
2. Seguire le istruzioni a schermo per interagire con il programma, fornendo gli opportuni parametri necessari
3. Assicurarsi di avere un file di testo. Nella directory _output_file_ del progetto vi sono alcuni esempi per il testing.

Un esempio di esecuzione è mostrato nelle immagini seguenti.

**Manuale dei parametri**

<img width="748" alt="Help" src="https://github.com/Simone0401/HOMEWORK/assets/79322230/08622064-c4fe-478c-afe8-d114df8c778c">


**Esempio di comando con parametri e relativa esecuzione**


<img width="748" alt="Parametri" src="https://github.com/Simone0401/HOMEWORK/assets/79322230/a9828551-fe8c-4864-9f76-a0501c2677cb">


<img width="748" alt="Output_file" src="https://github.com/Simone0401/HOMEWORK/assets/79322230/e9ca7b25-3b29-4851-9d19-fcddfd6daf12">

  
Nel caso in cui ci fossero parole del testo più lunghe rispetto alla lunghezza massima scelta per ogni riga verrà restituito il seguente errore:
  

<img width="641" alt="Errore" src="https://github.com/Simone0401/HOMEWORK/assets/79322230/30b71a9e-b8f7-411d-b094-60a768e8be8a">


## Struttura del Progetto

Verrà di seguito descritta brevemente la struttura dei file e delle cartelle del progetto:

- `src/`: Contiene i file sorgenti del progetto. In particolare il main.c e la libreria _justificationTools.h_ contenente tutte le funzioni per la logica del programma.
- `dist/`: contiene tutti i file necessari per l'utilizzo della libreria _argtable3_.
- `input_file/`: contiene alcuni testi di esempio con i quali poter provare il funzionamento del programma.
- `output_file/`: i file di output, di cui non è specificato il percorso come parametro, verranno salvati qui. **NB: se non viene indicato un percorso e un nome per i file di output il programma salverà il file di output come _output.txt_. Questo andrà a sovrascrivere il precedente file già nominato _output.txt_.
- `License`: è il file rappresentante la Licenza del programma.
- `README`: questo file.
- `Makefile`: per compilare velocemente il programma utilizzando il comando **make**. 

## Contributi

Per segnalazione di eventuali bug si pregà di utilizzare la sezione **Issues** di gitHub al seguente link: https://github.com/Simone0401/HOMEWORK/issues 

## Licenza

Il progetto è distribuito con la licenza MIT. Maggiori informazioni possono essere trovate nel file `LICENSE`.

## Contatti

- Simone Argento: [argento.1939925@studenti.uniroma1.it](mailto:argento.1939925@studenti.uniroma1.it)
- Progetto Sistemi Operativi 2 - _Università di Roma La Sapienza_
- Docente corso: Prof. Paolo Zuliani

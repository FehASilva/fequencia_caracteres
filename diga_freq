#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_LINE 1001
#define ASCII_START 32
#define ASCII_END 126
#define ASCII_RANGE (ASCII_END + 1)

typedef struct {
    int ascii;
    int freq;
} CharFreq;

int compare(const void *a, const void *b) {
    CharFreq *x = (CharFreq *)a;
    CharFreq *y = (CharFreq *)b;
    if (x->freq != y->freq) return x->freq - y->freq;
    return x->ascii - y->ascii;
}

int main() {
    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, stdin) != NULL) {
        int freq[ASCII_RANGE] = {0};
        int len = strlen(line);

        // Remove newline se existir
        if (line[len - 1] == '\n') line[--len] = '\0';

        // Contagem paralela dos caracteres
        #pragma omp parallel for shared(freq)
        for (int i = 0; i < len; i++) {
            unsigned char c = line[i];
            if (c >= ASCII_START && c <= ASCII_END) {
                #pragma omp atomic
                freq[c]++;
            }
        }

        // Preparar array para ordenação
        CharFreq list[ASCII_RANGE];
        int count = 0;
        for (int i = ASCII_START; i <= ASCII_END; i++) {
            if (freq[i] > 0) {
                list[count].ascii = i;
                list[count].freq = freq[i];
                count++;
            }
        }

        // Ordenar
        qsort(list, count, sizeof(CharFreq), compare);

        // Imprimir resultado
        for (int i = 0; i < count; i++) {
            printf("%d %d\n", list[i].ascii, list[i].freq);
        }

        // Separação entre casos
        printf("\n");
    }

    return 0;
}

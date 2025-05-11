/*
Trabalho 1: Frequência de Caracteres (SSC0903 - CAD - 2025/1) 
Autores:
Felipe Aparecido da Silva – NUSP: 11954502
Felipi Adenildo Soares Sousa – NUSP: 10438790
Francisco Jose de Sousa Filho – NUSP: 11200313
Viktor Sergio Ferreira – NUSP: 11800570
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Definindo constantes
#define MAX_LINE 1001
#define ASCII_START 32
#define ASCII_END 126
#define ASCII_RANGE (ASCII_END + 1)

// Definição da estrutura contendo código ASCII e frequência
typedef struct{
    int ascii;
    int freq;
} CharFreq;

// Função de comparação usada para ordenar os caracteres primeiro pela frequência e depois pelo valor ASCII
int compare(const void *a, const void *b){
    CharFreq *x = (CharFreq *)a;
    CharFreq *y = (CharFreq *)b;

    // Ordena pela frequência
    if (x->freq != y->freq) return x->freq - y->freq;

    // Ordena pelo valor ASCII, se necessário.
    return x->ascii - y->ascii;
}

// Função que mescla (merge) dois subvetores já ordenados
void merge(CharFreq *arr, int l, int m, int r, CharFreq *temp) {
    int i = l, j = m, k = l;

    // Comparação entre os dois subvetores e cópia ordenada para temp
    while (i < m && j < r){
        if (compare(&arr[i], &arr[j]) <= 0)
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    // Copia os elementos restantes (se houver) do subvetor esquerdo
    while (i < m) temp[k++] = arr[i++];

    // Copia os elementos restantes (se houver) do subvetor direito
    while (j < r) temp[k++] = arr[j++];

    // Copia os elementos mesclados de temp de volta para arr
    for (i = l; i < r; i++) arr[i] = temp[i];
}

// Função recursiva de ordenação merge sort paralela
// 'depth' controla a profundidade da recursão paralela
void merge_sort_parallel(CharFreq *arr, int l, int r, CharFreq *temp, int depth) {
    // Caso base: vetor de 1 ou nenhum elemento já está ordenado
    if (r - l <= 1) return;

    int m = (l + r) / 2;

    // Enquanto houver profundidade permitida, cria tarefas paralelas
    if (depth > 0) {
        #pragma omp task shared(arr, temp)
        merge_sort_parallel(arr, l, m, temp, depth - 1);

        #pragma omp task shared(arr, temp)
        merge_sort_parallel(arr, m, r, temp, depth - 1);

        #pragma omp taskwait
    } else {
        // Se profundidade esgotada, executa de forma sequencial
        merge_sort_parallel(arr, l, m, temp, 0);
        merge_sort_parallel(arr, m, r, temp, 0);
    }

    // Mescla os subvetores ordenados
    merge(arr, l, m, r, temp);
}

int main(){
    char line[MAX_LINE];

    // Lê linha por linha da entrada padrão
    while (fgets(line, MAX_LINE, stdin) != NULL){
        int freq[ASCII_RANGE] = {0}; // Array zerado para armazenar as frequências dos caracteres
        int len = strlen(line);
        if (line[len - 1] == '\n') line[len - 1] = '\0';

        // Contagem paralela das frequências de caracteres
        #pragma omp parallel for shared(freq)
        for (int i = 0; i < len; i++){
            unsigned char c = line[i];
            if (c >= ASCII_START && c <= ASCII_END){
                #pragma omp atomic // Incremento de forma atômica (evita condição de corrida)
                freq[c]++;
            }
        }

        // Arrays de CharFreq, array temporário para ordenação e contador de caracteres diferentes encontrados
        CharFreq list[ASCII_RANGE];
        CharFreq temp[ASCII_RANGE];
        int count = 0;

        // Preenche a lista com os caracteres que têm frequência maior que 0
        for (int i = ASCII_START; i <= ASCII_END; i++){
            if (freq[i] > 0) {
                list[count].ascii = i;
                list[count].freq = freq[i];
                count++;
            }
        }

        // Ordena a lista usando merge sort paralelo com profundidade controlada
        #pragma omp parallel
        {
            #pragma omp single
            merge_sort_parallel(list, 0, count, temp, 4);
        }

        // Imprime o resultado
        for (int i = 0; i < count; i++){
            printf("%d %d\n", list[i].ascii, list[i].freq);
        }
        printf("\n");
    }

    return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ceps_dados.h"

#define SEED 0x12345678
#define TOTAL_BUCKETS 6100

typedef enum { HASH_LINEAR, HASH_DUPLO } TipoHash;

typedef struct {
    uintptr_t *table;
    int size;
    int max;
    int limite;
    uintptr_t deleted;
    char *(*get_key)(void *);
    TipoHash tipo;
} thash;

uint32_t hashf(const char *str, uint32_t h) {
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

uint32_t hash2(const char *str) {
    uint32_t h = 0;
    for (; *str; ++str) {
        h = (h * 33) ^ (uint32_t)(*str);
    }
    h = (h << 1) | 1;
    return h;
}

int hash_constroi(thash *h, int nbuckets, char *(*get_key)(void *), TipoHash tipo) {
    h->table = calloc(sizeof(void *), nbuckets + 1);
    if (!h->table) return EXIT_FAILURE;
    h->max = nbuckets + 1;
    h->size = 0;
    h->limite = (int)(0.7 * h->max);
    h->deleted = (uintptr_t)&(h->size);
    h->get_key = get_key;
    h->tipo = tipo;
    return EXIT_SUCCESS;
}

int hash_insere(thash *h, void *bucket) {
    uint32_t hash = hashf(h->get_key(bucket), SEED);
    uint32_t step = (h->tipo == HASH_DUPLO) ? hash2(h->get_key(bucket)) : 1;
    int pos = hash % h->max;
    while (h->table[pos] != 0 && h->table[pos] != h->deleted) {
        pos = (pos + step) % h->max;
    }
    h->table[pos] = (uintptr_t)bucket;
    h->size++;
    return EXIT_SUCCESS;
}

void *hash_busca(thash h, const char *key) {
    uint32_t hash = hashf(key, SEED);
    uint32_t step = (h.tipo == HASH_DUPLO) ? hash2(key) : 1;
    int pos = hash % h.max;
    while (h.table[pos] != 0) {
        if (h.table[pos] != h.deleted && strcmp(h.get_key((void *)h.table[pos]), key) == 0)
            return (void *)h.table[pos];
        pos = (pos + step) % h.max;
    }
    return NULL;
}

void hash_apaga(thash *h) {
    for (int i = 0; i < h->max; i++) {
        if (h->table[i] != 0 && h->table[i] != h->deleted)
            free((void *)h->table[i]);
    }
    free(h->table);
}

typedef struct {
    char prefixo[6];
    char cidade[100];
    char estado[3];
} tcep_prefixo;

char *get_key(void *reg) {
    return ((tcep_prefixo *)reg)->prefixo;
}

void *aloca_prefixo(const char *prefixo, const char *cidade, const char *estado) {
    tcep_prefixo *p = malloc(sizeof(tcep_prefixo));
    strncpy(p->prefixo, prefixo, 5); p->prefixo[5] = '\0';
    strncpy(p->cidade, cidade, sizeof(p->cidade) - 1); p->cidade[sizeof(p->cidade) - 1] = '\0';
    strncpy(p->estado, estado, 2); p->estado[2] = '\0';
    return p;
}

void consulta_por_cep(thash *h, const char *cep) {
    char prefixo[6];
    strncpy(prefixo, cep, 5); prefixo[5] = '\0';
    tcep_prefixo *res = hash_busca(*h, prefixo);
    if (res != NULL)
        printf("%s => %s/%s\n", prefixo, res->cidade, res->estado);
}

void insere6100() {
    thash h;
    hash_constroi(&h, 6100, get_key, HASH_DUPLO);
    for (int i = 0; i < total_ceps; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    hash_apaga(&h);
}

void insere1000() {
    thash h;
    hash_constroi(&h, 1000, get_key, HASH_DUPLO);
    for (int i = 0; i < total_ceps; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    hash_apaga(&h);
}

int main() {
    clock_t start, end;
    double tempo;

    start = clock();
    insere6100();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo para insercao com 6100 buckets: %f segundos\n", tempo);

    start = clock();
    insere1000();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo para insercao com 1000 buckets: %f segundos\n", tempo);

    return 0;
}

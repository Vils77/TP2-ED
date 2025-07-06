#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

bool consulta_por_cep(thash *h, const char *cep) {
    bool T = true;

    char prefixo[6];
    strncpy(prefixo, cep, 5); prefixo[5] = '\0';
    tcep_prefixo *res = hash_busca(*h, prefixo);
    if (res != NULL)
        return T;
}


void busca10() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.10);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca20() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.20);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca30() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.30);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca40() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.40);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca50() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.50);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca60() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.60);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca70() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.70);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca80() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.80);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca90() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.90);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}

void busca99() {
    thash h;
    hash_constroi(&h, TOTAL_BUCKETS, get_key, HASH_DUPLO);
    int limite = (int)(total_ceps * 0.99);
    for (int i = 0; i < limite; i++) {
        hash_insere(&h, aloca_prefixo(ceps[i].prefixo, ceps[i].cidade, ceps[i].estado));
    }
    for (int i = 0; i < limite; i++) {
        consulta_por_cep(&h, ceps[i].prefixo);
    }
    hash_apaga(&h);
}


int main() {

    clock_t start, end;
    double tempo;

    start = clock();
    busca10();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca10: %f segundos\n", tempo);

    start = clock();
    busca20();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca20: %f segundos\n", tempo);

    start = clock();
    busca30();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca30: %f segundos\n", tempo);

    start = clock();
    busca40();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca40: %f segundos\n", tempo);

    start = clock();
    busca50();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca50: %f segundos\n", tempo);

    start = clock();
    busca60();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca60: %f segundos\n", tempo);

    start = clock();
    busca70();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca70: %f segundos\n", tempo);

    start = clock();
    busca80();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca80: %f segundos\n", tempo);

    start = clock();
    busca90();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca90: %f segundos\n", tempo);

    start = clock();
    busca99();
    end = clock();
    tempo = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Tempo da busca99: %f segundos\n", tempo);

    return 0;
}

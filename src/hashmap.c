#include "hashmap.h"

size_t h(char c) { return (size_t)c; }

size_t *generate_power_arr(size_t n, size_t p) {
    size_t *power = malloc((n + 1) * sizeof(size_t));
    size_t i;

    power[0] = 1;

    for (i = 1; i <= n; ++i) {
        power[i] = power[i - 1] * p;
    }

    return power;
}

size_t hash(const char *str) {
    size_t res = 0;
    size_t i;

    for (i = 0; i < strlen(str); ++i) {
        res += power_arr[i] * h(str[i]);
    }

    return res;
}

bool add_hm(char *c, char *value, data_item *hash_map, size_t size) {
    size_t p, stop;
    p = stop = hash(c) % size;

    if (hash_map[p].key == NULL) {
        hash_map[p].key = c;
        hash_map[p].value = value;
        return true;
    }

    ++p;
    p %= size;
    while (p != stop) {
        if (hash_map[p].key == NULL) {
            hash_map[p].key = c;
            hash_map[p].value = value;
            return true;
        }

        ++p;
        p %= size;
    }

    return false;
}

data_item *find_hm(char *c, data_item *hash_map, size_t size) {
    size_t p, stop;
    p = stop = hash(c) % size;

    if (hash_map[p].key && !strcmp(hash_map[p].key, c)) {
        return &hash_map[p];
    }

    ++p;
    p %= size;
    while (p != stop) {
        if (hash_map[p].key && !strcmp(hash_map[p].key, c)) {
            return &hash_map[p];
        }

        ++p;
        p %= size;
    }

    return NULL;
}

bool remove_hm(const char *c, data_item *hash_map, size_t size) {
    size_t p, stop;
    p = stop = hash(c) % size;

    if (!strcmp(hash_map[p].key, c)) { 
        free(hash_map[p].key);
        free(hash_map[p].value);
        hash_map[p].key = NULL;
        return true;
    }

    ++p;
    p %= size;
    while (p != stop) {
        if (!strcmp(hash_map[p].key, c)) {
            free(hash_map[p].key);
            free(hash_map[p].value);
            hash_map[p].key = NULL;
            return true;
        }

        ++p;
        p %= size;
    }

    return false;
}

data_item *init_hashmap() {
    data_item *hash_map;
    hash_map = malloc(HASH_MAP_SIZE * sizeof(data_item));
    size_t i;

    for (i = 0; i < HASH_MAP_SIZE; ++i) {
        hash_map[i].key = NULL;
    }

    return hash_map;
}

void free_hasmap(data_item *hash_map) {
    int i;
    for (i = 0; i < HASH_MAP_SIZE; ++i) {
        free(hash_map[i].key);
        free(hash_map[i].value);
    }
    free(hash_map);
}

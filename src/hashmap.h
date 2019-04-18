#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_MAP_SIZE 256
#define POWER_ARR_SIZE 70
#define P 37

struct s_data_item {
    char *key;
    char *value;
};

typedef struct s_data_item data_item;

size_t h(char c);

size_t *power_arr;

size_t *generate_power_arr(size_t n, size_t p);

size_t hash(const char *str);

bool add_hm(char *c, char *value, data_item *hash_map, size_t size);

data_item *find_hm(char *c, data_item *hash_map, size_t size);

bool remove_hm(const char *c, data_item *hash_map, size_t size);

data_item *init_hashmap();

void free_hasmap(data_item *hash_map);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "benchmark.h"
#define inline
#include "htable.h"

#define HASH_SIZE 127

struct myitem {
	int value;
	struct hash_entry entry;
};

struct hash_table mytable;

inline int comp(const void *i, const void *j, size_t len)
{
  return *(int *)i - *(int *)j;
}

int test_int(int N, const unsigned *data)
{
	struct hash_entry *hentry;
	struct myitem *tmp;
	unsigned int len;
	int i;

	hash_table_init(&mytable, HASH_SIZE, comp);
	for (i = 0; i < N; ++i) {
		len = sizeof(data[i]);
		if ((hentry = hash_table_lookup_key(&mytable, (char *)&data[i], len)) == NULL){
			tmp = (struct myitem *)malloc(sizeof(struct myitem));
			memset(tmp, 0, sizeof(struct myitem));
			tmp->value = i;
			hash_table_insert(&mytable, &tmp->entry, (char *)&data[i], len);
		} else {
			hash_table_del_entry(&mytable, hentry);
			tmp = hash_entry(hentry, struct myitem, entry);
			hash_entry_finit(hentry);
			free(tmp);
		}
	}
	return mytable.num_elems;
}

int test_str(int N, char * const *data)
{
	struct hash_entry *hentry;
	struct myitem *tmp;
	unsigned int len;
	int i;

	hash_table_init(&mytable, HASH_SIZE, NULL);
	for (i = 0; i < N; ++i) {
		len = strlen(data[i]);
		if ((hentry = hash_table_lookup_key(&mytable, data[i], len)) == NULL){
			tmp = (struct myitem *)malloc(sizeof(struct myitem));
			memset(tmp, 0, sizeof(struct myitem));
			tmp->value = i;
			hash_table_insert(&mytable, &tmp->entry, data[i], len);
		} else {
			hash_table_del_entry(&mytable, hentry);
			tmp = hash_entry(hentry, struct myitem, entry);
			hash_entry_finit(hentry);
			free(tmp);
		}
	}
	return mytable.num_elems;
}

int main(int argc, char *argv[])
{
	return udb_benchmark(argc, argv, test_int, test_str);
}

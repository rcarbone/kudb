#ifndef __HTABLE_H
#define __HTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "list.h"

#define DENSITY 0.80

typedef int (*keycmp_ptr) (const void *, const void *, size_t);

struct hash_entry {
	struct list_head   list;
	unsigned char      *key;
	unsigned int       keylen;
};

struct hash_table {
	struct hash_entry  *table;
	unsigned int       buckets;
	unsigned int       num_elems;
	keycmp_ptr         keycmp;
	/* private variables */
	unsigned int       __ht_i;
	struct list_head   *pos;
};

static void rehash(struct hash_table *h);

uint32_t __hash(const char * data, int len) 
{
	int i;
	uint32_t hash;

	hash = UINT32_C(2166136261);
	for (i=0; i < len; i++) {
		hash = (UINT32_C(16777619) * hash) ^ data[i];
	}
	return hash;
}

static inline int hash_table_hash_code(const struct hash_table *t,
				       const char *key, unsigned int len)
{
	return (__hash(key, len) % t->buckets);
}

static inline int hash_entry_init(struct hash_entry *e,
				  const unsigned char *str, unsigned int len)
{
	INIT_LIST_HEAD(&(e->list));

	if (str) {
		if ((e->key = (unsigned char *)malloc(len)) == NULL)
			return -1;
		memcpy(e->key, str, len);
		e->keylen = len;
	} 
	return 0;
}

static inline void hash_entry_finit(struct hash_entry *e)
{
	if (e->key)
		free(e->key);
	e->keylen = 0;
}

static inline int hash_table_init(struct hash_table *h, unsigned int b, keycmp_ptr keycmp)
{
	h->buckets = b;
	h->num_elems = 0;

	if ((h->table = (struct hash_entry *)malloc(sizeof(struct hash_entry) * b)) == NULL)
		return -1;

	for (--b; b != 0; --b) {
		hash_entry_init(&(h->table[b]), NULL, 0);
	}
	hash_entry_init(&(h->table[0]), NULL, 0);
	
	if (keycmp)
		h->keycmp = keycmp;
	else
		h->keycmp = &memcmp;

	return 0;
}

static inline void hash_table_finit(struct hash_table *h)
{
	if (h->table)
		free(h->table);
	h->buckets = 0;
	h->num_elems = 0;
}

/* insert_hash_table()
 * @h: &struct hash_table hash table to insert hash_entry into
 * @e: &struct hash_entry
 * Description: inserts @e into @h using @e->key as key. not thread-safe.
 */
void hash_table_insert(struct hash_table *h,
		       struct hash_entry *e,
		       const unsigned char *key, unsigned int len)
{
	unsigned int n;
	float alpha;
	
	alpha = (float) h->num_elems / h->buckets;

	if (alpha > DENSITY)
		rehash(h);
	hash_entry_init(e, key, len);
	n = __hash(key, len) % h->buckets;
	list_add(&(e->list), &(h->table[n].list));
	h->num_elems++;
}

/* hash_table_lookup_key()
 * @h: hash table to look into
 * @str: the key to look for
 * @len: length of the key
 * Description: looks up the hash table for the presence of key. 
 * Returns: returns a pointer to the hash_entry that matches the key. otherise returns NULL.
 * Notes: in the presence of duplicate keys the function returns the first hash_entry found.
 * 		  function is not safe from delections. 
 * 		  function is not thread safe. 
 */
struct hash_entry *hash_table_lookup_key(const struct hash_table *h,
					 const unsigned char *str,
					 unsigned int len)
{
//	unsigned int key = hash_table_hash_code(h, str, len);
	unsigned int key = __hash(str, len) % h->buckets;	
	struct hash_entry *tmp;
	struct list_head *pos;

	list_for_each(pos, &(h->table[key].list)) {
		tmp = list_entry(pos, struct hash_entry, list);

		if ((tmp->keylen == len)
		    && (h->keycmp(tmp->key, str, tmp->keylen) == 0))
			return tmp;
	}
	return NULL;
}


/* same as hash_table_lookup_key() but this function takes a valid hash_entry as input.
 * a valid hash_entry is the one that has key, len set appropriately. in other words, a
 * hash_entry that is the output of hash_entry_init()
 */
static inline struct hash_entry *hash_table_lookup_hash_entry(const struct
							      hash_table *h,
							      const struct
							      hash_entry *e)
{
	return (hash_table_lookup_key(h, e->key, e->keylen));
}


struct hash_entry *hash_table_del_key(struct hash_table *h, const char *str,
				      unsigned int len)
{
	struct hash_entry *e;

	if ((e = hash_table_lookup_key(h, str, len)) == NULL)
		return NULL;

	list_del_init(&(e->list));
	h->num_elems--;

	return e;
}

static inline struct hash_entry *hash_table_del_hash_entry(struct hash_table *h,
							   struct hash_entry *e)
{
	return (hash_table_del_key(h, e->key, e->keylen));
}

/**
 * hash_entry - get the user data for this entry
 * @ptr:	the &struct hash_entry pointer
 * @type:	the type of the user data (e.g. struct my_data) embedded in this entry
 * @member:	the name of the hash_entry within the struct (e.g. entry)
 */
#define hash_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*
 * @hentry: &struct hash_entry
 * @htable: &struct hash_table
 */
#define hash_table_for_each(hentry, htable)	\
	for	((htable)->__ht_i=0; ((htable)->__ht_i < (htable)->buckets); ++((htable)->__ht_i))	\
		for(((htable)->pos= (htable)->table[(htable)->__ht_i].list.next);		\
				((htable)->pos != &((htable)->table[(htable)->__ht_i].list)) &&	\
				((hentry) = ((struct hash_entry *)((char *)((htable)->pos)-(unsigned long)(&((struct hash_entry *)0)->list))) );	\
				(htable)->pos= (htable)->pos->next)

/*
 * @hentry: &struct hash_entry
 * @htable: &struct hash_table
 * @pos: &struct list_head
 * @hti: unsigned int
 */
 #define hash_table_for_each_safe(hentry, htable, pos, hti)	\
	for	((hti)=0; ((hti) < (htable)->buckets); ++(hti))	\
		for(((pos)= (htable)->table[(hti)].list.next);		\
				((pos) != &((htable)->table[(hti)].list)) &&	\
				((hentry) = ((struct hash_entry *)((char *)((pos))-(unsigned long)(&((struct hash_entry *)0)->list))) );	\
				(pos)= (pos)->next)

/*******/
/*******/

static inline void hash_table_move(struct hash_table *h, struct hash_entry *e)
{
	unsigned int n;

	n = __hash(e->key, e->keylen) % h->buckets;
	list_add(&(e->list), &(h->table[n].list));
}


inline struct hash_entry *hash_table_del_entry(struct hash_table *h,
							   struct hash_entry *e)
{
	if (e == NULL)
		return NULL;
	
	list_del_init(&(e->list));
	h->num_elems--;
	
	return e;
}

static void rehash(struct hash_table *h)
{
	struct hash_entry **all_entry;
	struct hash_entry *hentry;
	unsigned int ns, hti;

	ns = h->buckets * 2 + 1;

	if ((all_entry = (struct hash_entry **)malloc(sizeof(struct hash_entry *) 
												  * (h->num_elems + 1))) == NULL){
		printf("No memory for rehash\n");
		exit(1);
	}
	hti = 0;
	/* Copy hash table */
	hash_table_for_each(hentry, h){
		all_entry[hti++] = hentry;	
	}
	/* new table */
	if ((h->table = 
		 (struct hash_entry *)realloc(h->table, 
									  sizeof(struct hash_entry) * ns)) == NULL){
		printf("No memory\n");
		exit(1);
	}
	h->buckets = ns;
    /* init the table */
	for (hti = 0; hti < ns; hti++) {
		hash_entry_init(&(h->table[hti]), NULL, 0);
	}
	/* insert in the table */
	for (hti = 0; hti < h->num_elems; hti++){
		hash_table_move(h, all_entry[hti]);
	}
	free(all_entry);
}

#endif

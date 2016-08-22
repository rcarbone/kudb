/*
Copyright (c) 2003-2008, Troy D. Hanson     http://uthash.sourceforge.net
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h> /* memcmp,strlen */
#include <stddef.h> /* ptrdiff_t */

#ifndef UTHASH_H
#define UTHASH_H 

/* C++ requires extra stringent casting */
#if defined __cplusplus
#define TYPEOF(x) (typeof(x))
#else
#define TYPEOF(x)
#endif


#define uthash_fatal(msg) exit(-1)        /* fatal error (out of memory,etc) */
#define uthash_bkt_malloc(sz) malloc(sz)  /* malloc fcn for UT_hash_bucket's */
#define uthash_bkt_free(ptr) free(ptr)    /* free fcn for UT_hash_bucket's   */
#define uthash_tbl_malloc(sz) malloc(sz)  /* malloc fcn for UT_hash_table    */
#define uthash_tbl_free(ptr) free(ptr)    /* free fcn for UT_hash_table      */

#define uthash_noexpand_fyi(tbl)          /* can be defined to log noexpand  */
#define uthash_expand_fyi(tbl)            /* can be defined to log expands   */

/* initial number of buckets */
#define HASH_INITIAL_NUM_BUCKETS 32      /* initial number of buckets        */
#define HASH_INITIAL_NUM_BUCKETS_LOG2 5  /* lg2 of initial number of buckets */
#define HASH_BKT_CAPACITY_THRESH 10      /* expand when bucket count reaches */

/* calculate the element whose hash handle address is hhe */
#define ELMT_FROM_HH(tbl,hhp) ((void*)(((char*)hhp) - (tbl)->hho))

#define HASH_FIND(hh,head,keyptr,keylen_in,out)                               \
do {                                                                          \
  out=TYPEOF(out)head;                                                        \
  if (head) {                                                                 \
     (head)->hh.tbl->key = (char*)(keyptr);                                   \
     (head)->hh.tbl->keylen = keylen_in;                                      \
     HASH_FCN((head)->hh.tbl->key,(head)->hh.tbl->keylen,                     \
             (head)->hh.tbl->num_buckets,                                     \
             (head)->hh.tbl->hash_scratch, (head)->hh.tbl->bkt,               \
             (head)->hh.tbl->i, (head)->hh.tbl->j,(head)->hh.tbl->k);         \
     HASH_FIND_IN_BKT((head)->hh.tbl, hh,                                     \
                      (head)->hh.tbl->buckets[ (head)->hh.tbl->bkt],          \
                      keyptr,keylen_in,out);                                  \
  }                                                                           \
} while (0)

#define HASH_ADD(hh,head,fieldname,keylen_in,add)                             \
        HASH_ADD_KEYPTR(hh,head,&add->fieldname,keylen_in,add)
 
#define HASH_ADD_KEYPTR(hh,head,keyptr,keylen_in,add)                         \
do {                                                                          \
 add->hh.next = NULL;                                                         \
 add->hh.key = (char*)keyptr;                                                 \
 add->hh.keylen = keylen_in;                                                  \
 if (!(head)) {                                                               \
    head = add;                                                               \
    (head)->hh.prev = NULL;                                                   \
    (head)->hh.tbl = (UT_hash_table*)uthash_tbl_malloc(                       \
                    sizeof(UT_hash_table));                                   \
    if (!((head)->hh.tbl))  { uthash_fatal( "out of memory"); }               \
    memset((head)->hh.tbl, 0, sizeof(UT_hash_table));                         \
    (head)->hh.tbl->tail = &(add->hh);                                        \
    (head)->hh.tbl->num_buckets = HASH_INITIAL_NUM_BUCKETS;                   \
    (head)->hh.tbl->log2_num_buckets = HASH_INITIAL_NUM_BUCKETS_LOG2;         \
    (head)->hh.tbl->hho = (char*)(&add->hh) - (char*)(add);                   \
    (head)->hh.tbl->buckets = (UT_hash_bucket*)uthash_bkt_malloc(             \
            HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket));          \
    if (! (head)->hh.tbl->buckets) { uthash_fatal( "out of memory"); }        \
    memset((head)->hh.tbl->buckets, 0,                                        \
            HASH_INITIAL_NUM_BUCKETS*sizeof(struct UT_hash_bucket));          \
 } else {                                                                     \
    (head)->hh.tbl->tail->next = add;                                         \
    add->hh.prev = ELMT_FROM_HH((head)->hh.tbl, (head)->hh.tbl->tail);        \
    (head)->hh.tbl->tail = &(add->hh);                                        \
 }                                                                            \
 (head)->hh.tbl->num_items++;                                                 \
 add->hh.tbl = (head)->hh.tbl;                                                \
 (head)->hh.tbl->key = (char*)keyptr;                                         \
 (head)->hh.tbl->keylen = keylen_in;                                          \
 HASH_FCN((head)->hh.tbl->key,(head)->hh.tbl->keylen,                         \
         (head)->hh.tbl->num_buckets,                                         \
         (add)->hh.hashv,                                                     \
         (head)->hh.tbl->bkt,                                                 \
         (head)->hh.tbl->i, (head)->hh.tbl->j, (head)->hh.tbl->k );           \
 HASH_ADD_TO_BKT(hh,(head)->hh.tbl->buckets[(head)->hh.tbl->bkt],add);        \
 HASH_EMIT_KEY(hh,head,keyptr,keylen_in);                                     \
 HASH_FSCK(hh,head);                                                          \
} while(0)

#define HASH_TO_BKT( hashv, num_bkts, bkt ) bkt = ((hashv) & ((num_bkts) - 1))

/* delete "delptr" from the hash table.
 * "the usual" patch-up process for the app-order doubly-linked-list.
 * The use of tbl->hh_del below deserves special explanation.
 * These used to be expressed using (delptr) but that led to a bug
 * if someone used the same symbol for the head and deletee, like
 *  HASH_DELETE(hh,users,users);
 * We want that to work, but by changing the head (users) below
 * we were forfeiting our ability to further refer to the deletee (users)
 * in the patch-up process. Solution: use scratch space in the table to
 * copy the deletee pointer, then the latter references are via that
 * scratch pointer rather than through the repointed (users) symbol.
 */
#define HASH_DELETE(hh,head,delptr)                                           \
do {                                                                          \
    if ( ((delptr)->hh.prev == NULL) && ((delptr)->hh.next == NULL) )  {      \
        uthash_bkt_free((head)->hh.tbl->buckets );                            \
        uthash_tbl_free((head)->hh.tbl);                                      \
        head = NULL;                                                          \
    } else {                                                                  \
        (head)->hh.tbl->hh_del = &((delptr)->hh);                             \
        if ((delptr) == ELMT_FROM_HH((head)->hh.tbl,(head)->hh.tbl->tail)) {  \
            (head)->hh.tbl->tail =                                            \
                (UT_hash_handle*)((char*)((delptr)->hh.prev) +                \
                (head)->hh.tbl->hho);                                         \
        }                                                                     \
        if ((delptr)->hh.prev) {                                              \
            ((UT_hash_handle*)((char*)((delptr)->hh.prev) +                   \
                    (head)->hh.tbl->hho))->next = (delptr)->hh.next;          \
        } else {                                                              \
            head = TYPEOF(head)((delptr)->hh.next);                           \
        }                                                                     \
        if ((head)->hh.tbl->hh_del->next) {                                   \
            ((UT_hash_handle*)((char*)(head)->hh.tbl->hh_del->next +          \
                    (head)->hh.tbl->hho))->prev =                             \
                    (head)->hh.tbl->hh_del->prev;                             \
        }                                                                     \
        HASH_TO_BKT( (head)->hh.tbl->hh_del->hashv,                           \
                     (head)->hh.tbl->num_buckets,                             \
                     (head)->hh.tbl->bkt);                                    \
        HASH_DEL_IN_BKT(hh,(head)->hh.tbl->buckets[(head)->hh.tbl->bkt],      \
                (head)->hh.tbl->hh_del);                                      \
        (head)->hh.tbl->num_items--;                                          \
    }                                                                         \
    HASH_FSCK(hh,head);                                                       \
} while (0)


/* convenience forms of HASH_FIND/HASH_ADD/HASH_DEL */
#define HASH_FIND_STR(head,findstr,out)                                       \
    HASH_FIND(hh,head,findstr,strlen(findstr),out)
#define HASH_ADD_STR(head,strfield,add)                                       \
    HASH_ADD(hh,head,strfield,strlen(add->strfield),add)
#define HASH_FIND_INT(head,findint,out)                                       \
    HASH_FIND(hh,head,findint,sizeof(int),out)
#define HASH_ADD_INT(head,intfield,add)                                       \
    HASH_ADD(hh,head,intfield,sizeof(int),add)
#define HASH_DEL(head,delptr)                                                 \
    HASH_DELETE(hh,head,delptr)

/* HASH_FSCK checks hash integrity on every add/delete when HASH_DEBUG is defined.
 * This is for uthash developer only; it compiles away if HASH_DEBUG isn't defined.
 * This function misuses fields in UT_hash_table for its bookkeeping variables.
 */
#ifdef HASH_DEBUG
#define HASH_OOPS(...) do { fprintf(stderr,__VA_ARGS__); exit(-1); } while (0)
#define HASH_FSCK(hh,head)                                                    \
do {                                                                          \
    if (head) {                                                               \
        (head)->hh.tbl->keylen = 0;   /* item counter */                      \
        for(    (head)->hh.tbl->bkt_i = 0;                                    \
                (head)->hh.tbl->bkt_i < (head)->hh.tbl->num_buckets;          \
                (head)->hh.tbl->bkt_i++)                                      \
        {                                                                     \
            (head)->hh.tbl->ideal_chain_maxlen = 0; /*bkt item counter*/      \
            (head)->hh.tbl->thh =                                             \
            (head)->hh.tbl->buckets[(head)->hh.tbl->bkt_i].hh_head;           \
            (head)->hh.tbl->key = NULL;  /* hh_prev */                        \
            while ((head)->hh.tbl->thh) {                                     \
               if ((head)->hh.tbl->key !=                                     \
                   (char*)((head)->hh.tbl->thh->hh_prev)) {                   \
                   HASH_OOPS("invalid hh_prev %x, actual %x\n",               \
                    (head)->hh.tbl->thh->hh_prev,                             \
                    (head)->hh.tbl->key );                                    \
               }                                                              \
               (head)->hh.tbl->ideal_chain_maxlen++;                          \
               (head)->hh.tbl->key = (char*)((head)->hh.tbl->thh);            \
               (head)->hh.tbl->thh = (head)->hh.tbl->thh->hh_next;            \
            }                                                                 \
            (head)->hh.tbl->keylen +=                                         \
               (head)->hh.tbl->ideal_chain_maxlen;                            \
            if ((head)->hh.tbl->buckets[(head)->hh.tbl->bkt_i].count          \
               !=  (head)->hh.tbl->ideal_chain_maxlen) {                      \
               HASH_OOPS("invalid bucket count %d, actual %d\n",              \
                (head)->hh.tbl->buckets[(head)->hh.tbl->bkt_i].count,         \
                (head)->hh.tbl->ideal_chain_maxlen);                          \
            }                                                                 \
        }                                                                     \
        if ((head)->hh.tbl->keylen != (head)->hh.tbl->num_items) {            \
            HASH_OOPS("invalid hh item count %d, actual %d\n",                \
                (head)->hh.tbl->num_items, (head)->hh.tbl->keylen );          \
        }                                                                     \
        /* traverse hh in app order; check next/prev integrity, count */      \
        (head)->hh.tbl->keylen = 0;   /* item counter */                      \
        (head)->hh.tbl->key = NULL;  /* app prev */                           \
        (head)->hh.tbl->thh =  &(head)->hh;                                   \
        while ((head)->hh.tbl->thh) {                                         \
           (head)->hh.tbl->keylen++;                                          \
           if ((head)->hh.tbl->key !=(char*)((head)->hh.tbl->thh->prev)) {    \
              HASH_OOPS("invalid prev %x, actual %x\n",                       \
                    (head)->hh.tbl->thh->prev,                                \
                    (head)->hh.tbl->key );                                    \
           }                                                                  \
           (head)->hh.tbl->key = ELMT_FROM_HH((head)->hh.tbl,                 \
                                              (head)->hh.tbl->thh);           \
           (head)->hh.tbl->thh = ( (head)->hh.tbl->thh->next ?                \
             (UT_hash_handle*)((char*)((head)->hh.tbl->thh->next) +           \
                               (head)->hh.tbl->hho)                           \
                                 : NULL );                                    \
        }                                                                     \
        if ((head)->hh.tbl->keylen != (head)->hh.tbl->num_items) {            \
            HASH_OOPS("invalid app item count %d, actual %d\n",               \
                (head)->hh.tbl->num_items, (head)->hh.tbl->keylen );          \
        }                                                                     \
    }                                                                         \
} while (0)
#else
#define HASH_FSCK(hh,head) 
#endif

/* When compiled with -DHASH_EMIT_KEYS, length-prefixed keys are emitted to 
 * the descriptor to which this macro is defined for tuning the hash function.
 * The app can #include <unistd.h> to get the prototype for write(2). */
#ifdef HASH_EMIT_KEYS
#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen)                                \
    (head)->hh.tbl->keylen = fieldlen;                                        \
    write(HASH_EMIT_KEYS, &((head)->hh.tbl->keylen), sizeof(int));            \
    write(HASH_EMIT_KEYS, keyptr, fieldlen);          
#else 
#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen)                    
#endif

/* default to Jenkins unless specified e.g. DHASH_FUNCTION=HASH_SAX */
#ifdef HASH_FUNCTION 
#define HASH_FCN HASH_FUNCTION
#else
#define HASH_FCN HASH_JEN
#endif

/* The Bernstein hash function, used in Perl prior to v5.6 */
#define HASH_BER(key,keylen,num_bkts,hash,bkt,i,j,k)                          \
  hash = 0;                                                                   \
  while (keylen--)  hash = (hash * 33) + *key++;                              \
  bkt = hash & (num_bkts-1);          


/* SAX/FNV/OAT/JEN hash functions are macro variants of those listed at 
 * http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx */
#define HASH_SAX(key,keylen,num_bkts,hash,bkt,i,j,k)                          \
  hash = 0;                                                                   \
  for(i=0; i < keylen; i++)                                                   \
      hash ^= (hash << 5) + (hash >> 2) + key[i];                             \
  bkt = hash & (num_bkts-1);          

#define HASH_FNV(key,keylen,num_bkts,hash,bkt,i,j,k)                          \
  hash = 2166136261UL;                                                        \
  for(i=0; i < keylen; i++)                                                   \
      hash = (hash * 16777619) ^ key[i];                                      \
  bkt = hash & (num_bkts-1);          
 
#define HASH_OAT(key,keylen,num_bkts,hash,bkt,i,j,k)                          \
  hash = 0;                                                                   \
  for(i=0; i < keylen; i++) {                                                 \
      hash += key[i];                                                         \
      hash += (hash << 10);                                                   \
      hash ^= (hash >> 6);                                                    \
  }                                                                           \
  hash += (hash << 3);                                                        \
  hash ^= (hash >> 11);                                                       \
  hash += (hash << 15);                                                       \
  bkt = hash & (num_bkts-1);          

#define HASH_JEN_MIX(a,b,c)                                                   \
{                                                                             \
  a -= b; a -= c; a ^= ( c >> 13 );                                           \
  b -= c; b -= a; b ^= ( a << 8 );                                            \
  c -= a; c -= b; c ^= ( b >> 13 );                                           \
  a -= b; a -= c; a ^= ( c >> 12 );                                           \
  b -= c; b -= a; b ^= ( a << 16 );                                           \
  c -= a; c -= b; c ^= ( b >> 5 );                                            \
  a -= b; a -= c; a ^= ( c >> 3 );                                            \
  b -= c; b -= a; b ^= ( a << 10 );                                           \
  c -= a; c -= b; c ^= ( b >> 15 );                                           \
}

#define HASH_JEN(key,keylen,num_bkts,hash,bkt,i,j,k)                          \
  hash = 0xfeedbeef;                                                          \
  i = j = 0x9e3779b9;                                                         \
  k = keylen;                                                                 \
  while (k >= 12) {                                                           \
    i +=    (key[0] + ( (unsigned)key[1] << 8 )                               \
        + ( (unsigned)key[2] << 16 )                                          \
        + ( (unsigned)key[3] << 24 ) );                                       \
    j +=    (key[4] + ( (unsigned)key[5] << 8 )                               \
        + ( (unsigned)key[6] << 16 )                                          \
        + ( (unsigned)key[7] << 24 ) );                                       \
    hash += (key[8] + ( (unsigned)key[9] << 8 )                               \
        + ( (unsigned)key[10] << 16 )                                         \
        + ( (unsigned)key[11] << 24 ) );                                      \
                                                                              \
     HASH_JEN_MIX(i, j, hash);                                                \
                                                                              \
     key += 12;                                                               \
     k -= 12;                                                                 \
  }                                                                           \
  hash += keylen;                                                             \
  switch ( k ) {                                                              \
     case 11: hash += ( (unsigned)key[10] << 24 );                            \
     case 10: hash += ( (unsigned)key[9] << 16 );                             \
     case 9:  hash += ( (unsigned)key[8] << 8 );                              \
     case 8:  j += ( (unsigned)key[7] << 24 );                                \
     case 7:  j += ( (unsigned)key[6] << 16 );                                \
     case 6:  j += ( (unsigned)key[5] << 8 );                                 \
     case 5:  j += key[4];                                                    \
     case 4:  i += ( (unsigned)key[3] << 24 );                                \
     case 3:  i += ( (unsigned)key[2] << 16 );                                \
     case 2:  i += ( (unsigned)key[1] << 8 );                                 \
     case 1:  i += key[0];                                                    \
  }                                                                           \
  HASH_JEN_MIX(i, j, hash);                                                   \
  bkt = hash & (num_bkts-1);          


/* key comparison function; return 0 if keys equal */
#define HASH_KEYCMP(a,b,len) memcmp(a,b,len) 

/* iterate over items in a known bucket to find desired item */
#define HASH_FIND_IN_BKT(tbl,hh,head,keyptr,keylen_in,out)                    \
out = TYPEOF(out)((head.hh_head) ? ELMT_FROM_HH(tbl,head.hh_head) : NULL);    \
while (out) {                                                                 \
    if (out->hh.keylen == keylen_in) {                                        \
        if ((HASH_KEYCMP(out->hh.key,keyptr,keylen_in)) == 0) break;          \
    }                                                                         \
    out= TYPEOF(out)((out->hh.hh_next) ?                                      \
                     ELMT_FROM_HH(tbl,out->hh.hh_next) : NULL);               \
}

/* add an item to a bucket  */
#define HASH_ADD_TO_BKT(hh,head,add)                                          \
 head.count++;                                                                \
 add->hh.hh_next = head.hh_head;                                              \
 add->hh.hh_prev = NULL;                                                      \
 if (head.hh_head) head.hh_head->hh_prev = &add->hh;                          \
 head.hh_head=&add->hh;                                                       \
 if (head.count >= ((head.expand_mult+1) * HASH_BKT_CAPACITY_THRESH)          \
     && add->hh.tbl->noexpand != 1) {                                         \
       HASH_EXPAND_BUCKETS(add->hh.tbl)                                       \
 }

/* remove an item from a given bucket */
#define HASH_DEL_IN_BKT(hh,head,hh_del)                                       \
    (head).count--;                                                           \
    if ((head).hh_head == hh_del) {                                           \
      (head).hh_head = hh_del->hh_next;                                       \
    }                                                                         \
    if (hh_del->hh_prev) {                                                    \
        hh_del->hh_prev->hh_next = hh_del->hh_next;                           \
    }                                                                         \
    if (hh_del->hh_next) {                                                    \
        hh_del->hh_next->hh_prev = hh_del->hh_prev;                           \
    }                                                                

/* Bucket expansion has the effect of doubling the number of buckets
 * and redistributing the items into the new buckets. Ideally the
 * items will distribute more or less evenly into the new buckets
 * (the extent to which this is true is a measure of the quality of
 * the hash function as it applies to the key domain). 
 * 
 * With the items distributed into more buckets, the chain length
 * (item count) in each bucket is reduced. Thus by expanding buckets
 * the hash keeps a bound on the chain length. This bounded chain 
 * length is the essence of how a hash provides constant time lookup.
 * 
 * The calculation of tbl->ideal_chain_maxlen below deserves some
 * explanation. First, keep in mind that we're calculating the ideal
 * maximum chain length based on the *new* (doubled) bucket count.
 * In fractions this is just n/b (n=number of items,b=new num buckets).
 * Since the ideal chain length is an integer, we want to calculate 
 * ceil(n/b). We don't depend on floating point arithmetic in this
 * hash, so to calculate ceil(n/b) with integers we could write
 * 
 *      ceil(n/b) = (n/b) + ((n%b)?1:0)
 * 
 * and in fact a previous version of this hash did just that.
 * But now we have improved things a bit by recognizing that b is
 * always a power of two. We keep its base 2 log handy (call it lb),
 * so now we can write this with a bit shift and logical AND:
 * 
 *      ceil(n/b) = (n>>lb) + ( (n & (b-1)) ? 1:0)
 * 
 * Actually bucket expansion is so expensive that this is a micro-
 * optimization. But it is more optimal, and more elegant this way. :-)
 */
#define HASH_EXPAND_BUCKETS(tbl)                                              \
    tbl->new_buckets = (UT_hash_bucket*)uthash_bkt_malloc(                    \
             2 * tbl->num_buckets * sizeof(struct UT_hash_bucket));           \
    if (!tbl->new_buckets) { uthash_fatal( "out of memory"); }                \
    memset(tbl->new_buckets, 0,                                               \
            2 * tbl->num_buckets * sizeof(struct UT_hash_bucket));            \
    tbl->ideal_chain_maxlen =                                                 \
       (tbl->num_items >> (tbl->log2_num_buckets+1)) +                        \
       ((tbl->num_items & ((tbl->num_buckets*2)-1)) ? 1 : 0);                 \
    tbl->nonideal_items = 0;                                                  \
    for(tbl->bkt_i = 0; tbl->bkt_i < tbl->num_buckets; tbl->bkt_i++)          \
    {                                                                         \
        tbl->thh = tbl->buckets[ tbl->bkt_i ].hh_head;                        \
        while (tbl->thh) {                                                    \
           tbl->hh_nxt = tbl->thh->hh_next;                                   \
           HASH_TO_BKT( tbl->thh->hashv, tbl->num_buckets*2, tbl->bkt);       \
           tbl->newbkt = &(tbl->new_buckets[ tbl->bkt ]);                     \
           if (++(tbl->newbkt->count) > tbl->ideal_chain_maxlen) {            \
             tbl->nonideal_items++;                                           \
             tbl->newbkt->expand_mult = tbl->newbkt->count /                  \
                                        tbl->ideal_chain_maxlen;              \
           }                                                                  \
           tbl->thh->hh_prev = NULL;                                          \
           tbl->thh->hh_next = tbl->newbkt->hh_head;                          \
           if (tbl->newbkt->hh_head) tbl->newbkt->hh_head->hh_prev =          \
                tbl->thh;                                                     \
           tbl->newbkt->hh_head = tbl->thh;                                   \
           tbl->thh = tbl->hh_nxt;                                            \
        }                                                                     \
    }                                                                         \
    tbl->num_buckets *= 2;                                                    \
    tbl->log2_num_buckets++;                                                  \
    uthash_bkt_free( tbl->buckets );                                          \
    tbl->buckets = tbl->new_buckets;                                          \
    tbl->ineff_expands = (tbl->nonideal_items > (tbl->num_items >> 1))        \
      ? (tbl->ineff_expands+1) : 0;                                           \
    if (tbl->ineff_expands > 1) {                                             \
        tbl->noexpand=1;                                                      \
        uthash_noexpand_fyi(tbl);                                             \
    }                                                                         \
    uthash_expand_fyi(tbl);                                         


/* This is an adaptation of Simon Tatham's O(n log(n)) mergesort */
/* Note that HASH_SORT assumes the hash handle name to be hh. 
 * HASH_SRT was added to allow the hash handle name to be passed in. */
#define HASH_SORT(head,cmpfcn) HASH_SRT(hh,head,cmpfcn)
#define HASH_SRT(hh,head,cmpfcn)                                              \
  if (head) {                                                                 \
      (head)->hh.tbl->insize = 1;                                             \
      (head)->hh.tbl->looping = 1;                                            \
      (head)->hh.tbl->list = &((head)->hh);                                   \
      while ((head)->hh.tbl->looping) {                                       \
          (head)->hh.tbl->p = (head)->hh.tbl->list;                           \
          (head)->hh.tbl->list = NULL;                                        \
          (head)->hh.tbl->tale = NULL;                                        \
          (head)->hh.tbl->nmerges = 0;                                        \
          while ((head)->hh.tbl->p) {                                         \
              (head)->hh.tbl->nmerges++;                                      \
              (head)->hh.tbl->q = (head)->hh.tbl->p;                          \
              (head)->hh.tbl->psize = 0;                                      \
              for ( (head)->hh.tbl->i = 0;                                    \
                    (head)->hh.tbl->i  < (head)->hh.tbl->insize;              \
                    (head)->hh.tbl->i++ ) {                                   \
                  (head)->hh.tbl->psize++;                                    \
                  (head)->hh.tbl->q =                                         \
                      (UT_hash_handle*)(((head)->hh.tbl->q->next) ?           \
                      ((void*)((char*)((head)->hh.tbl->q->next) +             \
                      (head)->hh.tbl->hho)) : NULL);                          \
                  if (! ((head)->hh.tbl->q) ) break;                          \
              }                                                               \
              (head)->hh.tbl->qsize = (head)->hh.tbl->insize;                 \
              while (((head)->hh.tbl->psize > 0) ||                           \
                      (((head)->hh.tbl->qsize > 0) && (head)->hh.tbl->q )) {  \
                  if ((head)->hh.tbl->psize == 0) {                           \
                      (head)->hh.tbl->e = (head)->hh.tbl->q;                  \
                      (head)->hh.tbl->q =                                     \
                          (UT_hash_handle*)(((head)->hh.tbl->q->next) ?       \
                          ((void*)((char*)((head)->hh.tbl->q->next) +         \
                          (head)->hh.tbl->hho)) : NULL);                      \
                      (head)->hh.tbl->qsize--;                                \
                  } else if ( ((head)->hh.tbl->qsize == 0) ||                 \
                             !((head)->hh.tbl->q) ) {                         \
                      (head)->hh.tbl->e = (head)->hh.tbl->p;                  \
                      (head)->hh.tbl->p =                                     \
                          (UT_hash_handle*)(((head)->hh.tbl->p->next) ?       \
                          ((void*)((char*)((head)->hh.tbl->p->next) +         \
                          (head)->hh.tbl->hho)) : NULL);                      \
                      (head)->hh.tbl->psize--;                                \
                  } else if ((                                                \
                      cmpfcn(TYPEOF(head)                                     \
                            (ELMT_FROM_HH((head)->hh.tbl,(head)->hh.tbl->p)), \
                            TYPEOF(head)                                      \
                            (ELMT_FROM_HH((head)->hh.tbl,(head)->hh.tbl->q))) \
                             ) <= 0) {                                        \
                      (head)->hh.tbl->e = (head)->hh.tbl->p;                  \
                      (head)->hh.tbl->p =                                     \
                          (UT_hash_handle*)(((head)->hh.tbl->p->next) ?       \
                          ((void*)((char*)((head)->hh.tbl->p->next) +         \
                          (head)->hh.tbl->hho)) : NULL);                      \
                      (head)->hh.tbl->psize--;                                \
                  } else {                                                    \
                      (head)->hh.tbl->e = (head)->hh.tbl->q;                  \
                      (head)->hh.tbl->q =                                     \
                          (UT_hash_handle*)(((head)->hh.tbl->q->next) ?       \
                          ((void*)((char*)((head)->hh.tbl->q->next) +         \
                                   (head)->hh.tbl->hho)) : NULL);             \
                      (head)->hh.tbl->qsize--;                                \
                  }                                                           \
                  if ( (head)->hh.tbl->tale ) {                               \
                      (head)->hh.tbl->tale->next = (((head)->hh.tbl->e) ?     \
                      ELMT_FROM_HH((head)->hh.tbl,(head)->hh.tbl->e) : NULL); \
                  } else {                                                    \
                      (head)->hh.tbl->list = (head)->hh.tbl->e;               \
                  }                                                           \
                  (head)->hh.tbl->e->prev = (((head)->hh.tbl->tale) ?         \
                   ELMT_FROM_HH((head)->hh.tbl,(head)->hh.tbl->tale) : NULL); \
                  (head)->hh.tbl->tale = (head)->hh.tbl->e;                   \
              }                                                               \
              (head)->hh.tbl->p = (head)->hh.tbl->q;                          \
          }                                                                   \
          (head)->hh.tbl->tale->next = NULL;                                  \
          if ( (head)->hh.tbl->nmerges <= 1 ) {                               \
              (head)->hh.tbl->looping=0;                                      \
              (head)->hh.tbl->tail = (head)->hh.tbl->tale;                    \
              (head) = TYPEOF(head)ELMT_FROM_HH((head)->hh.tbl,               \
                                                (head)->hh.tbl->list);        \
          }                                                                   \
          (head)->hh.tbl->insize *= 2;                                        \
      }                                                                       \
      HASH_FSCK(hh,head);                                                     \
 }

/* obtain a count of items in the hash */
#define HASH_COUNT(head) HASH_CNT(hh,head) 
#define HASH_CNT(hh,head) (head?(head->hh.tbl->num_items):0)

typedef struct UT_hash_bucket {
   struct UT_hash_handle *hh_head;
   unsigned count;

   /* expand_mult is normally set to 0. In this situation, the max chain length
    * threshold is enforced at its default value, HASH_BKT_CAPACITY_THRESH. (If
    * the bucket's chain exceeds this length, bucket expansion is triggered). 
    * However, setting expand_mult to a non-zero value delays bucket expansion
    * (that would be triggered by additions to this particular bucket)
    * until its chain length reaches a *multiple* of HASH_BKT_CAPACITY_THRESH.
    * (The multiplier is simply expand_mult+1). The whole idea of this
    * multiplier is to reduce bucket expansions, since they are expensive, in
    * situations where we know that a particular bucket tends to be overused.
    * It is better to let its chain length grow to a longer yet-still-bounded
    * value, than to do an O(n) bucket expansion too often. 
    */
   unsigned expand_mult;

} UT_hash_bucket;

typedef struct UT_hash_table {
   UT_hash_bucket *buckets;
   unsigned num_buckets, log2_num_buckets;
   unsigned num_items;
   struct UT_hash_handle *tail; /* tail hh in app order, for fast append    */
   ptrdiff_t hho; /* hash handle offset (byte pos of hash handle in element */

   /* in an ideal situation (all buckets used equally), no bucket would have
    * more than ceil(#items/#buckets) items. that's the ideal chain length. */
   unsigned ideal_chain_maxlen;

   /* nonideal_items is the number of items in the hash whose chain position
    * exceeds the ideal chain maxlen. these items pay the penalty for an uneven
    * hash distribution; reaching them in a chain traversal takes >ideal steps */
   unsigned nonideal_items;

   /* ineffective expands occur when a bucket doubling was performed, but 
    * afterward, more than half the items in the hash had nonideal chain
    * positions. If this happens on two consecutive expansions we inhibit any
    * further expansion, as it's not helping; this happens when the hash
    * function isn't a good fit for the key domain. When expansion is inhibited
    * the hash will still work, albeit no longer in constant time. */
   unsigned ineff_expands, noexpand;

   /* scratch */
   unsigned hash_scratch, bkt, bkt_i, keylen, i, j, k;
   char *key;
   struct UT_hash_handle *thh, *hh_nxt, *hh_del;

   /* scratch for bucket expansion */
   UT_hash_bucket *new_buckets, *newbkt;

   /* scratch for sort */
   int looping,nmerges,insize,psize,qsize;
   struct UT_hash_handle *p, *q, *e, *list, *tale;

} UT_hash_table;


typedef struct UT_hash_handle {
   struct UT_hash_table *tbl;
   void *prev;                       /* prev element in app order      */
   void *next;                       /* next element in app order      */
   struct UT_hash_handle *hh_prev;   /* previous hh in bucket order    */
   struct UT_hash_handle *hh_next;   /* next hh in bucket order        */
   void *key;                        /* ptr to enclosing struct's key  */
   int keylen;                       /* enclosing struct's key len     */
   unsigned hashv;                   /* result of hash-fcn(key)        */
} UT_hash_handle;

#endif /* UTHASH_H */

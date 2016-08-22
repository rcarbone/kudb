/******************************************************************************
 *
 * Copyright (C) 2008 Jason Evans <jasone@canonware.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice(s), this list of conditions and the following disclaimer
 *    unmodified other than the allowable addition of one or more
 *    copyright notices.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice(s), this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 *
 * cpp macro implementation of treaps.
 *
 * Usage:
 *
 *   (Optional, see assert(3).)
 *   #define NDEBUG
 *
 *   (Required.)
 *   #include <stdint.h> (For uint32_t.)
 *   #include <assert.h>
 *   #include <trp.h>
 *   trp(...)
 *   trp_gen(...)
 *   ...
 *
 ******************************************************************************/

#ifndef TRP_H_
#define	TRP_H_

#define	HASH_LITTLE_ENDIAN	true
#define	HASH_BIG_ENDIAN		false
#include "lookup3.h"

/* Node structure. */
#define	trp_node(a_type)						\
struct {								\
    a_type *trpn_left;							\
    a_type *trpn_right;							\
}

/* Root structure. */
#define	trp(a_type)							\
struct {								\
    a_type *trp_root;							\
}

/* Left accessors. */
#define	trp_left_get(a_type, a_field, a_node)				\
    ((a_node)->a_field.trpn_left)
#define	trp_left_set(a_type, a_field, a_node, a_left) do {		\
    (a_node)->a_field.trpn_left = a_left;				\
} while (0)

/* Right accessors. */
#define	trp_right_get(a_type, a_field, a_node)				\
    ((a_node)->a_field.trpn_right)
#define	trp_right_set(a_type, a_field, a_node, a_right) do {		\
    (a_node)->a_field.trpn_right = a_right;				\
} while (0)

/* Priority accessors. */
#define	trp_prio_get(a_type, a_field, a_node)				\
    hashlittle((const uint32_t *)&(a_node), sizeof(void *), 0xf3278904U)

/* Node initializer. */
#define	trp_node_new(a_type, a_field, a_trp, a_node) do {		\
    trp_left_set(a_type, a_field, (a_node), NULL);			\
    trp_right_set(a_type, a_field, (a_node), NULL);			\
} while (0)

/* Tree initializer. */
#define	trp_new(a_type, a_field, a_seed, a_trp) do {			\
    (a_trp)->trp_root = NULL;						\
} while (0)

/* Internal utility macros. */
#define	trpn_first(a_type, a_field, a_root, r_node) do {		\
    (r_node) = (a_root);						\
    if ((r_node) == NULL) {						\
	return (NULL);							\
    }									\
    for (;								\
      trp_left_get(a_type, a_field, (r_node)) != NULL;			\
      (r_node) = trp_left_get(a_type, a_field, (r_node))) {		\
    }									\
} while (0)

#define	trpn_last(a_type, a_field, a_root, r_node) do {			\
    (r_node) = (a_root);						\
    if ((r_node) == NULL) {						\
	return (NULL);							\
    }									\
    for (;								\
      trp_right_get(a_type, a_field, (r_node)) != NULL;			\
      (r_node) = trp_right_get(a_type, a_field, (r_node))) {		\
    }									\
} while (0)

#define	trpn_rotate_left(a_type, a_field, a_node, r_node) do {		\
    (r_node) = trp_right_get(a_type, a_field, (a_node));		\
    trp_right_set(a_type, a_field, (a_node),				\
      trp_left_get(a_type, a_field, (r_node)));				\
    trp_left_set(a_type, a_field, (r_node), (a_node));			\
} while (0)

#define	trpn_rotate_right(a_type, a_field, a_node, r_node) do {		\
    (r_node) = trp_left_get(a_type, a_field, (a_node));			\
    trp_left_set(a_type, a_field, (a_node),				\
      trp_right_get(a_type, a_field, (r_node)));			\
    trp_right_set(a_type, a_field, (r_node), (a_node));			\
} while (0)

/*
 * The trp_gen() macro generates a type-specific treap implementation,
 * based on the above cpp macros.
 *
 * Arguments:
 *
 *   a_attr     : Function attribute for generated functions (ex: static).
 *   a_prefix   : Prefix for generated functions (ex: treap_).
 *   a_trp_type : Type for treap data structure (ex: treap_t).
 *   a_type     : Type for treap node data structure (ex: treap_node_t).
 *   a_field    : Name of treap node linkage (ex: treap_link).
 *   a_cmp      : Node comparison function name, with the following prototype:
 *                  int (a_cmp *)(a_type *a_node, a_type *a_other);
 *                                        ^^^^^^
 *                                     or a_key
 *                Interpretation of comparision function return values:
 *                  -1 : a_node <  a_other
 *                   0 : a_node == a_other
 *                   1 : a_node >  a_other
 *                In all cases, the a_node or a_key macro argument is the first
 *                argument to the comparison function, which makes it possible
 *                to write comparison functions that treat the first argument
 *                specially.
 *
 * Assuming the following setup:
 *
 *   typedef struct ex_node_s ex_node_t;
 *   struct ex_node_s {
 *       trp_node(ex_node_t) ex_link;
 *   };
 *   typedef trp(ex_node_t) ex_t;
 *   trp_gen(static, ex_, ex_t, ex_node_t, ex_link, ex_cmp)
 *
 * The following API is generated:
 *
 *   static void
 *   ex_new(ex_t *treap, uint32_t seed);
 *       Description: Initialize a treap structure.
 *       Args:
 *         treap: Pointer to an uninitialized treap object.
 *         seed : Pseudo-random number generator seed.  The seed value isn't
 *                very important, so if in doubt, pick a favorite number.
 *
 *   static ex_node_t *
 *   ex_first(ex_t *treap);
 *   static ex_node_t *
 *   ex_last(ex_t *treap);
 *       Description: Get the first/last node in the treap.
 *       Args:
 *         treap: Pointer to a initialized treap object.
 *       Ret: First/last node in the treap, or NULL if the treap is empty.
 *
 *   static ex_node_t *
 *   ex_next(ex_t *treap, ex_node_t *node);
 *   static ex_node_t *
 *   ex_prev(ex_t *treap, ex_node_t *node);
 *       Description: Get node's successor/predecessor.
 *       Args:
 *         treap: Pointer to a initialized treap object.
 *         node : A node in treap.
 *       Ret: node's successor/predecessor in treap, or NULL if node is
 *            last/first.
 *
 *   static ex_node_t *
 *   ex_search(ex_t *treap, ex_node_t *key);
 *       Description: Search for node that matches key.
 *       Args:
 *         treap: Pointer to a initialized treap object.
 *         key  : Search key.
 *       Ret: Node in treap that matches key, or NULL if no match.
 *
 *   static ex_node_t *
 *   ex_nsearch(ex_t *treap, ex_node_t *key);
 *   static ex_node_t *
 *   ex_psearch(ex_t *treap, ex_node_t *key);
 *       Description: Search for node that matches key.  If no match is found,
 *                    return what would be key's successor/predecessor, were
 *                    key in treap.
 *       Args:
 *         treap: Pointer to a initialized treap object.
 *         key  : Search key.
 *       Ret: Node in treap that matches key, or if no match, hypothetical
 *            node's successor/predecessor (NULL if no successor/predecessor).
 *
 *   static void
 *   ex_insert(ex_t *treap, ex_node_t *node);
 *       Description: Insert node into treap.
 *       Args:
 *         treap: Pointer to a initialized treap object.
 *         node : Node to be inserted into treap.
 *
 *   static void
 *   ex_remove(ex_t *treap, ex_node_t *node);
 *       Description: Remove node from treap.
 *       Args:
 *         treap: Pointer to a initialized treap object.
 *         node : Node in treap to be removed.
 *
 *   static ex_node_t *
 *   ex_iter(ex_t *treap, ex_node_t *start, ex_node_t *(*cb)(ex_t *,
 *     ex_node_t *, void *), void *arg);
 *   static ex_node_t *
 *   ex_reverse_iter(ex_t *treap, ex_node_t *start, ex_node *(*cb)(ex_t *,
 *     ex_node_t *, void *), void *arg);
 *       Description: Iterate forward/backward over treap, starting at node.
 *                    If the treap is modified, iteration must be immediately
 *                    terminated by the callback function that causes the
 *                    modification.
 *       Args:
 *         treap: Pointer to a initialized treap object.
 *         start: Node at which to start iteration, or NULL to start at
 *                first/last node.
 *         cb   : Callback function, which is called for each node during
 *                iteration.  Under normal circumstances the callback function
 *                should return NULL, which causes iteration to continue.  If a
 *                callback function returns non-NULL, iteration is immediately
 *                terminated and the non-NULL return value is returned by the
 *                iterator.  This is useful for re-starting iteration after
 *                modifying the treap.
 *         arg  : Opaque pointer passed to cb().
 *       Ret: NULL if iteration completed, or the non-NULL callback return value
 *            that caused termination of the iteration.
 */
#define	trp_gen(a_attr, a_prefix, a_trp_type, a_type, a_field, a_cmp)	\
a_attr void								\
a_prefix##new(a_trp_type *treap, uint32_t seed) {			\
    trp_new(a_type, a_field, seed, treap);				\
}									\
a_attr a_type *								\
a_prefix##first(a_trp_type *treap) {					\
    a_type *ret;							\
    trpn_first(a_type, a_field, treap->trp_root, ret);			\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##last(a_trp_type *treap) {					\
    a_type *ret;							\
    trpn_last(a_type, a_field, treap->trp_root, ret);			\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##next(a_trp_type *treap, a_type *node) {			\
    a_type *ret;							\
    if (trp_right_get(a_type, a_field, node) != NULL) {			\
	trpn_first(a_type, a_field, trp_right_get(a_type,		\
	  a_field, node), ret);						\
    } else {								\
	a_type *tnode = treap->trp_root;				\
	assert(tnode != NULL);						\
	ret = NULL;							\
	while (true) {							\
	    int cmp = (a_cmp)(node, tnode);				\
	    if (cmp < 0) {						\
		ret = tnode;						\
		tnode = trp_left_get(a_type, a_field, tnode);		\
	    } else if (cmp > 0) {					\
		tnode = trp_right_get(a_type, a_field, tnode);		\
	    } else {							\
		break;							\
	    }								\
	    assert(tnode != NULL);					\
	}								\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##prev(a_trp_type *treap, a_type *node) {			\
    a_type *ret;							\
    if (trp_left_get(a_type, a_field, node) != NULL) {			\
	trpn_last(a_type, a_field, trp_left_get(a_type,			\
	  a_field, node), ret);						\
    } else {								\
	a_type *tnode = treap->trp_root;				\
	assert(tnode != NULL);						\
	ret = NULL;							\
	while (true) {							\
	    int cmp = (a_cmp)(node, tnode);				\
	    if (cmp < 0) {						\
		tnode = trp_left_get(a_type, a_field, tnode);		\
	    } else if (cmp > 0) {					\
		ret = tnode;						\
		tnode = trp_right_get(a_type, a_field, tnode);		\
	    } else {							\
		break;							\
	    }								\
	    assert(tnode != NULL);					\
	}								\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##search(a_trp_type *treap, a_type *key) {			\
    a_type *ret;							\
    int cmp;								\
    ret = treap->trp_root;						\
    while (ret != NULL							\
      && (cmp = (a_cmp)(key, ret)) != 0) {				\
	if (cmp < 0) {							\
	    ret = trp_left_get(a_type, a_field, ret);			\
	} else {							\
	    ret = trp_right_get(a_type, a_field, ret);			\
	}								\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##nsearch(a_trp_type *treap, a_type *key) {			\
    a_type *ret;							\
    a_type *tnode = treap->trp_root;					\
    ret = NULL;								\
    while (tnode != NULL) {						\
	int cmp = (a_cmp)(key, tnode);					\
	if (cmp < 0) {							\
	    ret = tnode;						\
	    tnode = trp_left_get(a_type, a_field, tnode);		\
	} else if (cmp > 0) {						\
	    tnode = trp_right_get(a_type, a_field, tnode);		\
	} else {							\
	    ret = tnode;						\
	    break;							\
	}								\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##psearch(a_trp_type *treap, a_type *key) {			\
    a_type *ret;							\
    a_type *tnode = treap->trp_root;					\
    ret = NULL;								\
    while (tnode != NULL) {						\
	int cmp = (a_cmp)(key, tnode);					\
	if (cmp < 0) {							\
	    tnode = trp_left_get(a_type, a_field, tnode);		\
	} else if (cmp > 0) {						\
	    ret = tnode;						\
	    tnode = trp_right_get(a_type, a_field, tnode);		\
	} else {							\
	    ret = tnode;						\
	    break;							\
	}								\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##insert_recurse(a_type *cur_node, a_type *ins_node) {		\
    if (cur_node == NULL) {						\
	return (ins_node);						\
    } else {								\
	a_type *ret;							\
	int cmp = a_cmp(ins_node, cur_node);				\
	assert(cmp != 0);						\
	if (cmp < 0) {							\
	    a_type *left = a_prefix##insert_recurse(trp_left_get(a_type,\
	      a_field, cur_node), ins_node);				\
	    trp_left_set(a_type, a_field, cur_node, left);		\
	    if (trp_prio_get(a_type, a_field, left) <			\
	      trp_prio_get(a_type, a_field, cur_node)) {		\
		trpn_rotate_right(a_type, a_field, cur_node, ret);	\
	    } else {							\
		ret = cur_node;						\
	    }								\
	} else {							\
	    a_type *right = a_prefix##insert_recurse(trp_right_get(	\
	      a_type, a_field, cur_node), ins_node);			\
	    trp_right_set(a_type, a_field, cur_node, right);		\
	    if (trp_prio_get(a_type, a_field, right) <			\
	      trp_prio_get(a_type, a_field, cur_node)) {		\
		trpn_rotate_left(a_type, a_field, cur_node, ret);	\
	    } else {							\
		ret = cur_node;						\
	    }								\
	}								\
	return (ret);							\
    }									\
}									\
a_attr void								\
a_prefix##insert(a_trp_type *treap, a_type *node) {			\
    trp_node_new(a_type, a_field, treap, node);				\
    treap->trp_root = a_prefix##insert_recurse(treap->trp_root, node);	\
}									\
a_attr a_type *								\
a_prefix##remove_recurse(a_type *cur_node, a_type *rem_node) {		\
    int cmp = a_cmp(rem_node, cur_node);				\
    assert(cur_node != NULL);						\
    if (cmp == 0) {							\
	a_type *ret;							\
	a_type *left = trp_left_get(a_type, a_field, cur_node);		\
	a_type *right = trp_right_get(a_type, a_field, cur_node);	\
	if (left == NULL) {						\
	    if (right == NULL) {					\
		return (NULL);						\
	    }								\
	} else {							\
	    if (right == NULL || trp_prio_get(a_type, a_field, left) <	\
	      trp_prio_get(a_type, a_field, right)) {			\
		trpn_rotate_right(a_type, a_field, cur_node, ret);	\
		right = a_prefix##remove_recurse(cur_node, rem_node);	\
		trp_right_set(a_type, a_field, ret, right);		\
		return (ret);						\
	    }								\
	}								\
	trpn_rotate_left(a_type, a_field, cur_node, ret);		\
	left = a_prefix##remove_recurse(cur_node, rem_node);		\
	trp_left_set(a_type, a_field, ret, left);			\
	return (ret);							\
    } else if (cmp < 0) {						\
	a_type *left = a_prefix##remove_recurse(trp_left_get(a_type,	\
	  a_field, cur_node), rem_node);				\
	trp_left_set(a_type, a_field, cur_node, left);			\
	return (cur_node);						\
    } else {								\
	a_type *right = a_prefix##remove_recurse(trp_right_get(a_type,	\
	  a_field, cur_node), rem_node);				\
	trp_right_set(a_type, a_field, cur_node, right);		\
	return (cur_node);						\
    }									\
}									\
a_attr void								\
a_prefix##remove(a_trp_type *treap, a_type *node) {			\
    treap->trp_root = a_prefix##remove_recurse(treap->trp_root, node);	\
}									\
a_attr a_type *								\
a_prefix##iter_recurse(a_trp_type *treap, a_type *node,			\
  a_type *(*cb)(a_trp_type *, a_type *, void *), void *arg) {		\
    if (node == NULL) {							\
	return (NULL);							\
    } else {								\
	a_type *ret;							\
	if ((ret = a_prefix##iter_recurse(treap, trp_left_get(a_type,	\
	  a_field, node), cb, arg)) != NULL				\
	  || (ret = cb(treap, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##iter_recurse(treap, trp_right_get(a_type,	\
	  a_field, node), cb, arg));					\
    }									\
}									\
a_attr a_type *								\
a_prefix##iter_start(a_trp_type *treap, a_type *start, a_type *node,	\
  a_type *(*cb)(a_trp_type *, a_type *, void *), void *arg) {		\
    int cmp = a_cmp(start, node);					\
    if (cmp < 0) {							\
	a_type *ret;							\
	if ((ret = a_prefix##iter_start(treap, start,			\
	  trp_left_get(a_type, a_field, node), cb, arg)) != NULL	\
	  || (ret = cb(treap, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##iter_recurse(treap, trp_right_get(a_type,	\
	  a_field, node), cb, arg));					\
    } else if (cmp > 0) {						\
	return (a_prefix##iter_start(treap, start, trp_right_get(a_type,\
	  a_field, node), cb, arg));					\
    } else {								\
	a_type *ret;							\
	if ((ret = cb(treap, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##iter_recurse(treap, trp_right_get(a_type,	\
	  a_field, node), cb, arg));					\
    }									\
}									\
a_attr a_type *								\
a_prefix##iter(a_trp_type *treap, a_type *start, a_type *(*cb)(		\
  a_trp_type *, a_type *, void *), void *arg) {				\
    if (start != NULL) {						\
	return (a_prefix##iter_start(treap, start, treap->trp_root, cb,	\
	  arg));							\
    } else {								\
	return (a_prefix##iter_recurse(treap, treap->trp_root, cb,	\
	  arg));							\
    }									\
}									\
a_attr a_type *								\
a_prefix##reverse_iter_recurse(a_trp_type *treap, a_type *node,		\
  a_type *(*cb)(a_trp_type *, a_type *, void *), void *arg) {		\
    if (node == NULL) {							\
	return (NULL);							\
    } else {								\
	a_type *ret;							\
	if ((ret = a_prefix##reverse_iter_recurse(treap,		\
	  trp_right_get(a_type, a_field, node), cb, arg)) != NULL	\
	  || (ret = cb(treap, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##reverse_iter_recurse(treap,			\
	  trp_left_get(a_type, a_field, node), cb, arg));		\
    }									\
}									\
a_attr a_type *								\
a_prefix##reverse_iter_start(a_trp_type *treap, a_type *start,		\
  a_type *node, a_type *(*cb)(a_trp_type *, a_type *, void *),		\
  void *arg) {								\
    int cmp = a_cmp(start, node);					\
    if (cmp > 0) {							\
	a_type *ret;							\
	if ((ret = a_prefix##reverse_iter_start(treap, start,		\
	  trp_right_get(a_type, a_field, node), cb, arg)) != NULL	\
	  || (ret = cb(treap, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##reverse_iter_recurse(treap,			\
	  trp_left_get(a_type, a_field, node), cb, arg));		\
    } else if (cmp < 0) {						\
	return (a_prefix##reverse_iter_start(treap, start,		\
	  trp_left_get(a_type, a_field, node), cb, arg));		\
    } else {								\
	a_type *ret;							\
	if ((ret = cb(treap, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##reverse_iter_recurse(treap,			\
	  trp_left_get(a_type, a_field, node), cb, arg));		\
    }									\
}									\
a_attr a_type *								\
a_prefix##reverse_iter(a_trp_type *treap, a_type *start, a_type *(*cb)(	\
  a_trp_type *, a_type *, void *), void *arg) {				\
    if (start != NULL) {						\
	return (a_prefix##reverse_iter_start(treap, start,		\
	  treap->trp_root, cb, arg));					\
    } else {								\
	return (a_prefix##reverse_iter_recurse(treap, treap->trp_root,	\
	  cb, arg));							\
    }									\
}
#endif /* TRP_H_ */

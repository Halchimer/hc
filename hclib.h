/*
 *  HC Library is halchimer's utility C library
 *
 *  define H_DEFINITIONS for library implementation
 *
 *  Modules included are :
 *
 *  H_ALL
 *
 *  H_TYPES
 *  H_ALLOCATORS
 *  H_COLLECTIONS
 *  H_HASH
 *  H_RANDOM
 *  H_STRING
 *  H_ITER
 *  H_BITSET
 *
 *  Parameters :
 *
 *  H_DEBUG for debug messages related to allocators, collections etc...
 */

#ifndef HCLIB_HCLIB_H
#define HCLIB_HCLIB_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef H_ALL
#define H_TYPES
#define H_ALLOCATORS
#define H_COLLECTIONS
#define H_HASH
#define H_RANDOM
#define H_STRING
#define H_ITER
#define H_BITSET
#endif

//
// Dependencies
//

#ifdef H_BITSET
#define H_TYPES
#endif

#ifdef H_RANDOM
#define H_TYPES
#define H_HASH
#endif

#ifdef H_HASH
#ifdef H_COLLECTIONS
#define H_ALLOCATORS
#endif
#endif

//
//  DECLARATIONS
//

#ifdef H_TYPES
    typedef char i8_t;
    typedef short i16_t;
    typedef int i32_t;
    typedef long long i64_t;
    typedef unsigned char u8_t;
    typedef unsigned short u16_t;
    typedef unsigned int u32_t;
    typedef unsigned long long u64_t;
#endif

#ifdef H_ALLOCATORS

typedef struct h_arena_t {
    size_t cap;
    size_t size;
    void* data;

#ifdef H_DEBUG
    char const* debug_name;
#endif
} h_arena_t;

h_arena_t *h_arena_create(
    size_t cap
    , char const* debug_name
    );
void h_arena_destroy(h_arena_t const *arena);
void *h_arena_alloc(h_arena_t *arena, size_t size);

#endif

#ifdef H_COLLECTIONS

typedef struct h_array_t {
    size_t size;
    size_t cap;
    size_t el_size;
    void* data;
} h_array_t;

void *h_array_get(h_array_t const *arr, size_t idx);
void h_array_set(h_array_t *arr, size_t idx, void *val);
void *h_array_push(h_array_t *arr, void *val);
void h_array_remove(h_array_t *arr, size_t idx);
void h_array_clear(h_array_t *arr);
void h_array_free(h_array_t *arr);

#define H_CREATE_ARRAY(type, size) (h_array_t){0, size, sizeof(type), calloc(size , sizeof(type))}
#define H_ARRAY_SET(type, arr, idx, val) ({type _v=(val); h_array_set((h_array_t*)&(arr), idx, &_v);})
#define H_ARRAY_GET(type, arr, idx) (*((type*)h_array_get((h_array_t*)&(arr), idx)))
#define H_ARRAY_PUSH(type, arr, val) ({type _v=(val); h_array_push((h_array_t*)&(arr), &_v);})

typedef struct h_link_t {
    struct h_link_t *next;
    void *data;
    size_t data_size;
} h_link_t;

h_link_t *h_create_link(size_t data_size);
h_link_t *h_enlink(h_link_t *head, size_t data_size);
h_link_t *h_enlink_same(h_link_t *head);
void h_free_link(h_link_t *head);

typedef struct h_queue_t {
    h_link_t *head;
    h_link_t *tail;
    size_t size;
    size_t data_size;
} h_queue_t;

h_queue_t h_create_queue(size_t data_size);
#define H_CREATE_QUEUE(type) h_create_queue(sizeof(type))

void h_enqueue(h_queue_t *queue, void *data);
#define H_ENQUEUE(type, queue, val) ({type _v=(val);h_enqueue((h_queue_t*)&(queue), &_v);})
void *h_dequeue(h_queue_t *queue);
#define H_DEQUEUE(type, queue) ({type *_vp=(type*)h_dequeue((h_queue_t*)&(queue));type _v = *_vp;free(_vp);_v;})
void h_queue_free(h_queue_t *queue);

#endif

#ifdef H_HASH

    typedef u32_t (h_hash_fn_t)(u32_t);

    u32_t h_pcg_hash(u32_t seed);

    u32_t h_hash(h_hash_fn_t *hash_fn, void *val, size_t size);

#ifdef H_COLLECTIONS

#ifndef H_HASHMAP_SLATE_FACTOR
#define H_HASHMAP_SLATE_FACTOR 1.5
#endif

    typedef bool (h_kcompare_fn_t)(void*,void*);

    typedef struct h_kvpairdef_t {
        size_t keysize;
        size_t valuesize;
    } h_kvpairdef_t;

    typedef struct h_kvpair_t {
        h_kvpairdef_t keytype;
        void *key;
        void *val;

        struct h_kvpair_t *next;
    } h_kvpair_t;

    typedef struct h_hashmap_t {
        h_kvpairdef_t keytype;
        size_t capacity;

        h_arena_t *arena;
        h_kvpair_t *buckets;
        h_array_t overflow;

        h_hash_fn_t *hash_fn;
        h_kcompare_fn_t *kcompare_fn;
    } h_hashmap_t;

    h_hashmap_t h_create_hashmap(h_kvpairdef_t kvpairdef, size_t capacity, h_hash_fn_t *hash_fn, h_kcompare_fn_t *kcompare_fn);
#define H_CREATE_HASHMAP(ktype, vtype, capacity, hashfn, compfn) h_create_hashmap((h_kvpairdef_t){.keysize=sizeof(ktype),.valuesize=sizeof(vtype)}, (capacity), (hashfn), (compfn))

    void *h_hashmap_put(h_hashmap_t *hashmap, void* key, void* val);
#define H_HASHMAP_PUT(hashmap, key, val) (typeof(val)*)({typeof(key) _k##__LINE__ = key;typeof(val) _v##__LINE__ = val;h_hashmap_put(&(hashmap), &(_k##__LINE__), &(_v##__LINE__));})

    void *h_hashmap_get(h_hashmap_t *hashmap, void* key);
#define H_HASHMAP_GET(hashmap, key) ({typeof(key) _k##__LINE__ = key;h_hashmap_get(&(hashmap), &(_k##__LINE__));})

    void h_hashmap_remove(h_hashmap_t *hashmap, void* key);
#define H_HASHMAP_REMOVE(hashmap, key) ({typeof(key) _k##__LINE__ = key;h_hashmap_remove(&(hashmap), &(_k##__LINE__));})

    void h_hashmap_clear(h_hashmap_t *hashmap);
    void h_hashmap_free(h_hashmap_t *hashmap);

    typedef struct h_hashset_t {
        size_t capacity;

        h_arena_t *arena;
        void *buckets;
        h_array_t overflow;

        h_hash_fn_t *hash_fn;
        h_kcompare_fn_t *kcompare_fn;
    } h_hashset_t;

#endif

#ifdef H_RANDOM

    float h_randf(u32_t seed);

    float h_randf_range(u32_t seed, float min, float max);

    i32_t h_randi(u32_t seed, i32_t min, i32_t max);

#endif

#ifdef H_STRING

    typedef char * h_cstr_t;

    typedef struct h_string_t {
        h_cstr_t cstr;
        size_t size;
    } h_string_t;

    h_string_t h_tostring(h_cstr_t cstr);
    h_cstr_t h_cstr(h_string_t str);

    bool h_string_eq_ptr(void* a, void* b);

#ifdef H_ALLOCATORS
    h_string_t h_arena_string_alloc_cstr(h_arena_t *arena, char *cstr);
    h_string_t h_arena_string_alloc_size(h_arena_t *arena, size_t size);
#define h_arena_string_alloc(arena, p) _Generic( p,\
char* : h_arena_string_alloc_cstr,\
size_t : h_arena_string_alloc_size\
)(arena, p)
#endif


#endif

#endif

#ifdef H_BITSET

    #define h_fixed_bitset_t(S) _BitInt(S)

    // Dynamic Bitset

    typedef u64_t h_bitset_word_t;

    typedef struct h_bitset_t {
        size_t size;
        h_bitset_word_t *words;
    } h_bitset_t;

    h_bitset_t h_create_bitset();

    void h_bitset_set(h_bitset_t *bitset, size_t idx);
    bool h_bitset_get(h_bitset_t *bitset, size_t idx);
    void h_bitset_clear_all(h_bitset_t *bitset);
    void h_bitset_clear(h_bitset_t *bitset, size_t idx);
    void h_bitset_flip(h_bitset_t *bitset, size_t idx);
    void h_bitset_free(h_bitset_t *bitset);

    bool h_bitset_any(h_bitset_t *bitset);
    void h_bitset_or(h_bitset_t *bitset, h_bitset_t *other);
    void h_bitset_and(h_bitset_t *bitset, h_bitset_t *other);
    void h_bitset_xor(h_bitset_t *bitset, h_bitset_t *other);

#endif

#ifdef H_ITER
    struct h_iter_t;
    typedef void* (h_iter_next_fn_t)(struct h_iter_t*);
    typedef bool (h_iter_hasnext_fn_t)(struct h_iter_t*);
    typedef struct h_iter_t {
        void *collection;
        void *state;

        h_iter_next_fn_t *next;
        h_iter_hasnext_fn_t *hasnext;
    } h_iter_t;

#ifdef H_COLLECTIONS
    h_iter_t h_array_iter(h_array_t *arr);
    void *h_array_next(h_iter_t *iter);
    bool h_array_hasnext(h_iter_t *iter);

    h_iter_t h_queue_iter(h_queue_t *queue);
    void *h_queue_next(h_iter_t *iter);
    bool h_queue_hasnext(h_iter_t *iter);

#endif

#ifdef H_BITSET
    h_iter_t h_bitset_iter(h_bitset_t *bitset);
    void *h_bitset_next(h_iter_t *iter);
    bool h_bitset_hasnext(h_iter_t *iter);
#endif

#define H_FOREACH(type, name, iter) \
    for(;iter.hasnext(&(iter));)\
        for(type name = *(type*)iter.next(&(iter)),*_once=&name; _once; _once=NULL)

#define H_FOREACH_PTR(type, name, iter) \
for(;iter.hasnext(&(iter));)\
for(type *name = (type*)iter.next(&(iter)),**_once=&name; _once; _once=NULL)

#endif

#ifdef H_DEFINITIONS

#ifdef H_ALLOCATORS

#ifdef H_DEBUG
    static h_array_t _debug_arena_registry;
    __attribute__((constructor))
    void h_debug_init_arena_registry() {
        _debug_arena_registry = H_CREATE_ARRAY(h_arena_t*, 32);
    }
#endif

    h_arena_t *h_arena_create(size_t cap
    , char const* debug_name
    ) {
        void *data = malloc(cap);

        if (!debug_name) debug_name = "Arena";

#ifdef H_DEBUG
        printf("Created arena '%s' with %zu bytes of capacity\n", debug_name , cap);
#endif

        h_arena_t *arena = calloc(1, sizeof(h_arena_t));
        *arena = (h_arena_t){cap, 0, data
        #ifdef H_DEBUG
        , debug_name
        #endif
        };

#ifdef H_DEBUG
        H_ARRAY_PUSH(h_arena_t*, _debug_arena_registry, arena);
#endif

        return arena;
    }
    void h_arena_destroy(h_arena_t const *arena) {
#ifdef H_DEBUG
        printf("Freeing arena '%s' with %zu bytes allocated\n", arena->debug_name ,arena->cap);

        for (int i=0;i<_debug_arena_registry.size;++i) {
            if (H_ARRAY_GET(h_arena_t*, _debug_arena_registry, i) == arena)
                h_array_remove(&_debug_arena_registry, i);
        }
#endif
        free(arena->data);
        free(arena);
    }
    void *h_arena_alloc(h_arena_t *arena, size_t size) {
        if (arena->size + size > arena->cap) {
#ifdef H_DEBUG
            fprintf(stderr,"Capacity of arena '%s' exceeded.", arena->debug_name);
#endif
            return NULL;
        }

        void *ptr = (char*)arena->data + arena->size;
        arena->size += size;

#ifdef H_DEBUG
        printf("Allocating %zu bytes in arena '%s' with a capacity of %zu bytes : %zu/%zu allocated\n", size, arena->debug_name, arena->cap, arena->size, arena->cap);
#endif

        return ptr;
    }

#endif

#ifdef H_COLLECTIONS

    void *h_array_get(h_array_t const *arr, size_t idx) {
        if (idx >= arr->size) {
            fprintf(stderr,"Index out of bounds : index %d for array of size %d.\n", idx, arr->size);
            return NULL;
        }
        return (char*)arr->data + idx * arr->el_size;
    }

    void h_array_set(h_array_t *arr, size_t idx, void *val) {
        if (idx >= arr->size) {
            if (idx > arr->cap) {
                arr->cap *= 2;
                arr->data = realloc(arr->data, arr->cap * arr->el_size);
            }
            arr->size = idx + 1;
        }
        memcpy((char*)arr->data + idx * arr->el_size, val, arr->el_size);
    }
    void *h_array_push(h_array_t *arr, void *val) {
        h_array_set(arr, arr->size, val);
        return h_array_get(arr, arr->size - 1);
    }

    void h_array_remove(h_array_t *arr, size_t idx) {
        memmove((char*)arr->data + idx * arr->el_size, (char*)arr->data + (idx + 1) * arr->el_size, (arr->size - idx - 1) * arr->el_size);
        arr->size--;
    }
    void h_array_clear(h_array_t *arr) {
        arr->size = 0;
        for (int i=0;i<arr->size*arr->el_size;++i)
            *((char*)arr->data + i) = 0;
    }
    void h_array_free(h_array_t *arr) {
        arr->cap = 0;
        arr->size = 0;
        free(arr->data);
        arr->data = NULL;
        arr->el_size = 0;
    }

    h_link_t *h_create_link(size_t data_size) {
        h_link_t *link = calloc(1, sizeof(h_link_t));
        link->data_size = data_size;
        link->next = NULL;
        link->data = calloc(1, data_size);
        return link;
    }
    h_link_t *h_enlink(h_link_t *head, size_t data_size) {
        if (!head) return NULL;
        h_link_t *link = head;
        while (link->next != NULL) link = link->next;
        link->next = h_create_link(data_size);
        return link->next;
    }
    h_link_t *h_enlink_same(h_link_t *head) {
        return h_enlink(head, head->data_size);
    }
    void h_free_link(h_link_t *head) {
        if (!head) return;
        h_array_t lnks_to_free = H_CREATE_ARRAY(h_link_t*, 32);
        h_link_t *link = head;
        H_ARRAY_PUSH(h_link_t*, lnks_to_free, link);
        while (link->next != NULL) {
            link = link->next;
            H_ARRAY_PUSH(h_link_t*, lnks_to_free, link);
        }
        while (lnks_to_free.size > 0) {
            h_link_t *lnk = H_ARRAY_GET(h_link_t*, lnks_to_free, lnks_to_free.size - 1);
            h_array_remove(&lnks_to_free, lnks_to_free.size - 1);
            free(lnk->data);
            free(lnk);
        }
    }

    h_queue_t h_create_queue(size_t data_size) {
        return (h_queue_t){NULL, NULL, 0, data_size};
    }
    void h_enqueue(h_queue_t *queue, void *data) {
        if (!data) return;
        if (!queue->head) {
            queue->head = h_create_link(queue->data_size);
            queue->tail = queue->head;
        }
        else
            queue->tail = h_enlink(queue->head, queue->data_size);

        memcpy(queue->tail->data, data, queue->data_size);

        queue->size++;
    }
    void *h_dequeue(h_queue_t *queue) {
        if (!queue->head) return NULL;

        h_link_t *lnk = queue->head;
        queue->head = lnk->next;
        queue->size--;
        void *out = lnk->data;
        free(lnk);
        return out;
    }

    void h_queue_free(h_queue_t *queue){
        h_free_link(queue->head);
        queue->head = NULL;
        queue->tail = NULL;
        queue->size = 0;
    }

#endif

#ifdef H_HASH
    u32_t h_pcg_hash(u32_t seed) {
        u32_t state = seed * 747796405u + 2891336453u;
        u32_t word = (state >> ((state >> 28) + 4)) * 277803737u;
        return (word >> 22u) ^ word;
    }

    u32_t h_hash(h_hash_fn_t *hash_fn, void *val, size_t size) {
        u32_t h = 0x811c9dc5u;
        for (size_t i=0;i<size;++i) {
            h ^= *((char*)val + i);
            h = hash_fn(h);
        }
    }

#ifdef H_COLLECTIONS

    h_hashmap_t h_create_hashmap(h_kvpairdef_t kvpairdef, size_t capacity, h_hash_fn_t *hash_fn, h_kcompare_fn_t *kcompare_fn) {
        h_hashmap_t hashmap = {.keytype = kvpairdef};
        hashmap.arena = h_arena_create(capacity * (kvpairdef.keysize + kvpairdef.valuesize) * H_HASHMAP_SLATE_FACTOR, "HashmapAllocator");
        hashmap.buckets = calloc(capacity, sizeof(h_kvpair_t));
        hashmap.overflow = H_CREATE_ARRAY(h_kvpair_t, 128);
        hashmap.hash_fn = hash_fn;
        hashmap.kcompare_fn = kcompare_fn;
        hashmap.capacity = capacity;

        return hashmap;
    }

    void *h_hashmap_put(h_hashmap_t *hashmap, void* key, void* val) {
        u32_t idx = h_hash(hashmap->hash_fn, key, hashmap->keytype.keysize)%hashmap->capacity;

        h_kvpair_t pair = (h_kvpair_t){
            hashmap->keytype,
            h_arena_alloc(hashmap->arena, hashmap->keytype.keysize),
            h_arena_alloc(hashmap->arena, hashmap->keytype.valuesize),
            NULL
        };

        memcpy(pair.key, key, hashmap->keytype.keysize);
        memcpy(pair.val, val, hashmap->keytype.valuesize);

        if (!hashmap->buckets[idx].key) {
            hashmap->buckets[idx] = pair;
            return pair.val;
        }
        h_kvpair_t *next = &hashmap->buckets[idx];
        while (next && next->next) next = next->next;

        //if (!next) return NULL;

        h_kvpair_t *npair = (h_kvpair_t *)H_ARRAY_PUSH(h_kvpair_t, hashmap->overflow, pair);
        next->next = npair;

        return pair.val;
    }

    void *h_hashmap_get(h_hashmap_t *hashmap, void* key) {
        u32_t idx = h_hash(hashmap->hash_fn, key, hashmap->keytype.keysize)%hashmap->capacity;

        h_kvpair_t *pair = &hashmap->buckets[idx];
        while (pair && pair->key &&!hashmap->kcompare_fn(key, pair->key)) pair = pair->next;
        return pair ? pair->val : NULL;
    }
    void h_hashmap_remove(h_hashmap_t *hashmap, void* key) {
        u32_t idx = h_hash(hashmap->hash_fn, key, hashmap->keytype.keysize)%hashmap->capacity;

        h_kvpair_t *pair = &hashmap->buckets[idx];

        if (hashmap->kcompare_fn(key, pair->key)) {
            if (pair->next)
                hashmap->buckets[idx] = *pair->next;
            else
                memset(pair, 0, sizeof(h_kvpair_t));
            return;
        }

        while (pair && !hashmap->kcompare_fn(key, pair->next->key)) pair = pair->next;
        if (pair->next)
            pair->next = pair->next->next;
    }
    void h_hashmap_clear(h_hashmap_t *hashmap) {
        h_hashmap_free(hashmap);

        *hashmap = h_create_hashmap(hashmap->keytype, hashmap->capacity, hashmap->hash_fn, hashmap->kcompare_fn);
    }
    void h_hashmap_free(h_hashmap_t *hashmap) {
        free(hashmap->buckets);
        h_arena_destroy(hashmap->arena);
        h_array_free(&hashmap->overflow);
    }
#endif
#endif

#ifdef H_RANDOM

    float h_randf(u32_t seed) {
        seed = h_pcg_hash(seed);
        return (float)seed / (float)(1u << 31);
    }

    float h_randf_range(u32_t seed, float min, float max) {
        return min + (max - min) * h_randf(seed);
    }

    i32_t h_randi(u32_t seed, i32_t min, i32_t max) {
        return min + (i32_t)(h_randf(seed) * (max - min + 1));
    }
#endif

#ifdef H_STRING

    h_string_t h_tostring(h_cstr_t cstr) {
        return (h_string_t){cstr, strlen(cstr)};
    }
    h_cstr_t h_cstr(h_string_t str) {
        return str.cstr;
    }

    bool h_string_eq_ptr(void* a, void* b) {
        h_string_t *str_a = (h_string_t*)a;
        h_string_t *str_b = (h_string_t*)b;
        return strcmp(str_a->cstr, str_b->cstr) == 0;
    }

#ifdef H_ALLOCATORS
    h_string_t h_arena_string_alloc_cstr(h_arena_t *arena, char *cstr) {
        size_t size = strlen(cstr) + 1;
        h_cstr_t cstr_ptr = h_arena_alloc(arena, size);
        memcpy(cstr_ptr, cstr, size);
        return (h_string_t){cstr_ptr, size};

    }
    h_string_t h_arena_string_alloc_size(h_arena_t *arena, size_t size) {
        h_cstr_t cstr_ptr = h_arena_alloc(arena, size);
        return (h_string_t){cstr_ptr, size};
    }

#endif

#endif

#ifdef H_ITER
#ifdef H_COLLECTIONS

    h_iter_t h_array_iter(h_array_t *arr) {
        return (h_iter_t){arr, arr->data, &h_array_next, &h_array_hasnext};
    }
    void *h_array_next(h_iter_t *iter) {
        h_array_t *arr = (h_array_t*)iter->collection;
        void *val = iter->state;
        if (iter->hasnext(iter))
            iter->state+=arr->el_size;
        return val;
    }
    bool h_array_hasnext(h_iter_t *iter) {
        h_array_t *arr = (h_array_t*)iter->collection;
        return iter->state < arr->data+arr->size*arr->el_size;
    }

    h_iter_t h_queue_iter(h_queue_t *queue) {
        return (h_iter_t){queue, queue->head, &h_queue_next, &h_queue_hasnext};
    }
    void *h_queue_next(h_iter_t *iter) {
        if (!iter->state) return NULL;
        void* data = ((h_link_t*)iter->state)->data;
        iter->state =((h_link_t*)iter->state)->next;
        return data;
    }
    bool h_queue_hasnext(h_iter_t *iter) {
        return (h_link_t*)iter->state != NULL;
    }

#endif

#ifdef H_BITSET
    h_iter_t h_bitset_iter(h_bitset_t *bitset) {
        return (h_iter_t){bitset, bitset->words, &h_bitset_next, &h_bitset_hasnext};
    }
    void *h_bitset_next(h_iter_t *iter) {
        void *val = iter->state;
        if (iter->hasnext(iter))
            iter->state+=sizeof(u64_t);
        return val;
    }
    bool h_bitset_hasnext(h_iter_t *iter) {
        h_bitset_t *bitset = (h_bitset_t*)iter->collection;
        return iter->state < bitset->words+bitset->size*sizeof(u64_t);
    }
#endif

#endif

#ifdef H_BITSET
    h_bitset_t h_create_bitset() {
        h_bitset_word_t *words = calloc(1, sizeof(h_bitset_word_t));
        return (h_bitset_t){1, words};
    }

    void h_bitset_set(h_bitset_t *bitset, size_t idx) {
        if (bitset->size == 0 || bitset->words == NULL)
            return;

        h_bitset_word_t word_idx = idx / (sizeof(h_bitset_word_t) * 8);
        h_bitset_word_t bit_idx = idx % (sizeof(h_bitset_word_t) * 8);

        if (word_idx >= bitset->size) {
            bitset->size*=2;
            bitset->words = realloc(bitset->words, bitset->size * sizeof(h_bitset_word_t));
        }

        bitset->words[word_idx] |= (1ULL << bit_idx);
    }
    bool h_bitset_get(h_bitset_t *bitset, size_t idx) {
        if (bitset->size == 0 || bitset->words == NULL)
            return false;

        if (idx >= bitset->size * (sizeof(h_bitset_word_t) * 8)) return false;

        h_bitset_word_t word_idx = idx / (sizeof(h_bitset_word_t) * 8);
        h_bitset_word_t bit_idx = idx % (sizeof(h_bitset_word_t) * 8);

        return (bitset->words[word_idx] & (1ULL << bit_idx));
    }
    void h_bitset_clear_all(h_bitset_t *bitset) {
        if (bitset->size == 0 || bitset->words == NULL)
            return;
        memset(bitset->words, 0, bitset->size * sizeof(h_bitset_word_t));
    }
    void h_bitset_clear(h_bitset_t *bitset, size_t idx) {
        if (bitset->size == 0 || bitset->words == NULL)
            return;

        h_bitset_word_t word_idx = idx / (sizeof(h_bitset_word_t) * 8);
        h_bitset_word_t bit_idx = idx % (sizeof(h_bitset_word_t) * 8);

        if (word_idx >= bitset->size) {
            bitset->size*=2;
            bitset->words = realloc(bitset->words, bitset->size * sizeof(h_bitset_word_t));
        }

        bitset->words[word_idx] &= ~(1ULL << bit_idx);
    }
    void h_bitset_flip(h_bitset_t *bitset, size_t idx) {
        if (bitset->size == 0 || bitset->words == NULL)
            return;

        h_bitset_word_t word_idx = idx / (sizeof(h_bitset_word_t) * 8);
        h_bitset_word_t bit_idx = idx % (sizeof(h_bitset_word_t) * 8);

        if (word_idx >= bitset->size) {
            bitset->size*=2;
            bitset->words = realloc(bitset->words, bitset->size * sizeof(h_bitset_word_t));
        }

        bitset->words[word_idx] ^= (1ULL << bit_idx);
    }
    void h_bitset_free(h_bitset_t *bitset) {
        free(bitset->words);
        bitset->words = NULL;
        bitset->size = 0;
    }

    bool h_bitset_any(h_bitset_t *bitset) {
        if (bitset->size == 0 || bitset->words == NULL)
            return false;

        for (int w=0;w<bitset->size;++w) {
            if (bitset->words[w] != 0) {
                return true;
            }
        }
        return false;
    }
    void h_bitset_or(h_bitset_t *bitset, h_bitset_t *other) {
        if (bitset->size == 0 || bitset->words == NULL) return;
        if (other->size == 0 || other->words == NULL) return;

        for (int w=0;w<bitset->size;++w) {
            if (w >= other->size) {
                bitset->words[w] |= 0ULL;
                continue;
            }
            bitset->words[w] |= other->words[w];
        }

    }
    void h_bitset_and(h_bitset_t *bitset, h_bitset_t *other) {
        if (bitset->size == 0 || bitset->words == NULL) return;
        if (other->size == 0 || other->words == NULL) return;

        for (int w=0;w<bitset->size;++w) {
            if (w >= other->size) {
                bitset->words[w] &= 0ULL;
                continue;
            }
            bitset->words[w] &= other->words[w];
        }
    }
    void h_bitset_xor(h_bitset_t *bitset, h_bitset_t *other) {
        if (bitset->size == 0 || bitset->words == NULL) return;
        if (other->size == 0 || other->words == NULL) return;

        for (int w=0;w<bitset->size;++w) {
            if (w >= other->size) {
                bitset->words[w] ^= 0ULL;
                continue;
            }
            bitset->words[w] ^= other->words[w];
        }
    }

#endif

#ifdef H_DEBUG
    __attribute__((destructor))
    void h_debug_end_warnings() {
#ifdef H_ALLOCATORS
        if (_debug_arena_registry.size > 0) {
            for (int ai=0;ai<_debug_arena_registry.size;++ai) {
                h_arena_t *arena = H_ARRAY_GET(h_arena_t*, _debug_arena_registry, ai);
                fprintf(stderr, "Arena '%s' with %zu bytes allocated has never been destroyed. Potential memory leak.\n", arena->debug_name, arena->cap);
            }
        }
#endif
    }
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif //HCLIB_HCLIB_H
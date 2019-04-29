/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _JOS_SECURE_PTR_H_
#define _JOS_SECURE_PTR_H_

typedef uint32_t j_sptr_t;

struct j_sptr_pool_t;
typedef struct j_sptr_pool_t *j_sptr_pool_h;

typedef enum {JSPTR_NORMAL, JSPTR_ODD, JSPTR_EVEN} j_sptr_pool_type_t;

j_sptr_pool_h j_secure_ptr_pool_init(j_sptr_pool_type_t type);
void j_secure_ptr_pool_uninit(j_sptr_pool_h sptr_pool);

/* Zero return value is an error */
j_sptr_t j_secure_ptr_create(j_sptr_pool_h sptr_pool, void *ptr);
result_t j_secure_ptr_domesticize(j_sptr_pool_h pool, j_sptr_t sptr, void *ptr);

#ifndef J_DEBUG
void j_secure_ptr_destroy(j_sptr_pool_h sptr_pool, j_sptr_t sptr);

void *j_secure_ptr_get(j_sptr_pool_h sptr_pool, j_sptr_t sptr);
#endif

/* Debug APIs version */
#ifdef J_DEBUG
void j_secure_ptr_destroy_d(j_sptr_pool_h sptr_pool, j_sptr_t sptr, char *file,
    int_t line);
#define j_secure_ptr_destroy(a, b) \
    j_secure_ptr_destroy_d(a, b, __FILE__, __LINE__)

void *j_secure_ptr_get_d(j_sptr_pool_h sptr_pool, j_sptr_t sptr, char *file,
    int_t line);
#define j_secure_ptr_get(a, b) j_secure_ptr_get_d(a, b, __FILE__, __LINE__)
#endif

#endif


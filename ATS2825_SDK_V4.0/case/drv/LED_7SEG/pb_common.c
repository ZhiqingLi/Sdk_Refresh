/* pb_common.c: Common support functions for pb_encode.c and pb_decode.c.
 *
 * 2014 Petteri Aimonen <jpa@kapsi.fi>
 */

#include "pb_common.h"

bool pb_field_iter_begin(pb_field_iter_t *iter, const pb_field_t *fields, void *dest_struct)
{
    iter->start = fields;
    iter->pos = fields;
    iter->required_field_index = 0;
    iter->dest_struct = dest_struct;
    iter->pData = (char *)dest_struct + iter->pos->data_offset;
    iter->pSize = (char *)iter->pData + iter->pos->size_offset;

    return (iter->pos->tag != 0);
}

bool pb_field_iter_next(pb_field_iter_t *iter)
{
    const pb_field_t *prev_field = iter->pos;

    if(prev_field->tag == 0) {
        /* Handle empty message types, where the first field is already the terminator.
         * In other cases, the iter->pos never points to the terminator. */
        return false;
    }

    iter->pos++;

    if(iter->pos->tag == 0) {
        /* Wrapped back to beginning, reinitialize */
        (void)pb_field_iter_begin(iter, iter->start, iter->dest_struct);
        return false;
    } else {
        /* Increment the pointers based on previous field size */
        size_t prev_size = prev_field->data_size;

        if(PB_HTYPE(prev_field->type) == PB_HTYPE_ONEOF &&
                PB_HTYPE(iter->pos->type) == PB_HTYPE_ONEOF &&
                iter->pos->data_offset == PB_SIZE_MAX) {
            /* Don't advance pointers inside unions */
            return true;
        } else if(PB_ATYPE(prev_field->type) == PB_ATYPE_STATIC &&
                  PB_HTYPE(prev_field->type) == PB_HTYPE_REPEATED) {
            /* In static arrays, the data_size tells the size of a single entry and
             * array_size is the number of entries */
            prev_size *= prev_field->array_size;
        } else if(PB_ATYPE(prev_field->type) == PB_ATYPE_POINTER) {
            /* Pointer fields always have a constant size in the main structure.
             * The data_size only applies to the dynamically allocated area. */
            prev_size = sizeof(void *);
        }

        if(PB_HTYPE(prev_field->type) == PB_HTYPE_REQUIRED) {
            /* Count the required fields, in order to check their presence in the
             * decoder. */
            iter->required_field_index++;
        }

        iter->pData = (char *)iter->pData + prev_size + iter->pos->data_offset;
        iter->pSize = (char *)iter->pData + iter->pos->size_offset;
        return true;
    }
}

bool pb_field_iter_find(pb_field_iter_t *iter, uint32_t tag)
{
    const pb_field_t *start = iter->pos;

    do {
        if(iter->pos->tag == tag &&
                PB_LTYPE(iter->pos->type) != PB_LTYPE_EXTENSION) {
            /* Found the wanted field */
            return true;
        }

        (void)pb_field_iter_next(iter);
    } while(iter->pos != start);

    /* Searched all the way back to start, and found nothing. */
    return false;
}

/* Implementations are from the Public Domain C Library (PDCLib). */
size_t strlen(const char *s)
{
    size_t rc = 0;
    while(s[rc]) {
        ++rc;
    }
    return rc;
}

void *memcpy(void *s1, const void *s2, size_t n)
{
    char *dest = (char *) s1;
    const char *src = (const char *) s2;
    while(n--) {
        *dest++ = *src++;
    }
    return s1;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char *) s;
    while(n--) {
        *p++ = (unsigned char) c;
    }
    return s;
}

#ifdef ENABLE_AUTO_LOAD_RODATA
static unsigned int cur_addr;

typedef void (*load_cb_t)(unsigned int addr);

static load_cb_t pb_load_cb;

void pb_load(unsigned int addr)
{
    addr = ((unsigned int)(addr) & 0xfffc0000);

    if(addr != cur_addr && pb_load_cb != NULL) {
        cur_addr = addr;
        pb_load_cb(addr);
    }
}

void pb_register_load_callback(unsigned int func)
{
    pb_load_cb = (load_cb_t)func;
}

#endif


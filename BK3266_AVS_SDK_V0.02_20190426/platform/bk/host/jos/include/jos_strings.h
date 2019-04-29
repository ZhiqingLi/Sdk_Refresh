/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef __JOS_STRINGS_H__
#define __JOS_STRINGS_H__

/*----------------------------
  JOS Strings API Header File
  ----------------------------*/

/* NOTE: The functions declared in this file are implemented and documented in
 *       jos/utils/jos_strings.c. */

/* String character encodings */
typedef enum {
    J_ST_ASCII,
    J_ST_UTF8,
    J_ST_UTF16,
    J_ST_UTF16BE, /* Only valid for @src_type parameters */
    J_ST_UTF16LE  /* Only valid for @src_type parameters */
} j_string_type_t;

/* JOS string */
typedef struct {
    j_string_type_t type;
    union {
        char     ascii[1];
        uint8_t  utf8[1];
        uint16_t utf16[1];
    } s;
} j_string_t;

/* JOS string list: an array of pointers to JOS strings */
typedef j_string_t **j_string_list_t;

char *j_string_get_ascii(j_string_t *str, char *dst_str, j_size_t dst_size);
uint8_t *j_string_get_utf8(j_string_t *str, uint8_t *dst_str,
    j_size_t dst_size);

uint16_t *j_utf8_to_utf16(uint8_t *src, j_size_t src_size);
uint8_t *j_utf16_to_utf8(uint16_t *src, j_string_type_t src_type,
    j_size_t src_size);

j_string_t *j_string_convert(j_string_t *src, j_string_type_t dest_type);

j_string_t *j_string_create(void *src, j_string_type_t src_type,
    j_string_type_t dest_type);

j_string_t *j_string_create_n(void *src, j_size_t src_size,
    j_string_type_t src_type, j_string_type_t dest_type);

void j_string_free(j_string_t *str);

#ifdef J_DEBUG
#define j_string_dup(str) j_string_dup_d(str, __FILE__, __LINE__)
j_string_t *j_string_dup_d(j_string_t *str, char *file, int_t line);
#else
j_string_t *j_string_dup(j_string_t *str);
#endif

j_size_t j_string_size(j_string_t *str);

char *j_stradd(char *strs[]);

void j_string_list_free(j_string_list_t str_list, uint32_t num);

#endif /* __JOS_STRINGS_H__ */


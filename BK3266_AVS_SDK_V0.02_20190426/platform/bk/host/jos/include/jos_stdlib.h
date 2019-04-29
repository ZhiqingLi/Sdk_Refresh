/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _JOS_STDLIB_H_
#define _JOS_STDLIB_H_

#define NUMERIC_VALUE_INVALID   0xFFFFFFFF
#define J_UNDEFINED_MASK       0

/* String Manipulation Functions */
uint32_t j_strtonum(const char *instr, uint8_t base, char **last_ptr);
bool_t   j_is_space(char c);
char    *j_strstr(const char *instr, const char *substr);
char    *j_strdup(const char *str);
uint32_t j_strnlen(char *str, uint32_t maxlen);
char    *j_numtostr(char *outstr, uint8_t base, uint32_t min_digits,
    uint32_t max_digits, uint32_t num);
uint32_t j_strlcpy(char *dst, const char *src, uint32_t n);
char    *j_strrchr(char *cs, char c);
char    *j_strlchr(char *cs, uint32_t len, char c);
char    *j_strchr(char *cs, char c);
int_t    j_tolower(int_t c);
char    *j_strltrim(char *str, uint32_t *len);
char    *j_strtrim(char *str, uint32_t *len, bool_t move_str);
char   **j_str_to_array(char *buf, uint32_t len, const char *delimiters,
    uint32_t delimiters_n, uint32_t *array_n);
int_t j_strcasecmp(const char *s1, const char *s2);

/* Memory Descriptor Manipulation */
void jsg_to_virt(sg_list_item_t *sg_list, void *_dst, uint32_t size);
void jvirt_to_sg(void *_src, sg_list_item_t *sg_list, uint32_t size);

#define J_CODE2STR_LAST (-1)
#define J_ENUM2STR(x) x, #x

typedef struct {
    int_t code;
    const char *str;
} j_code2str_t;

const char *j_code2str(int_t code, j_code2str_t codes_list[]);

typedef struct {
    uint32_t bit;
    const char *string;
} j_bit2str_t;

void j_bit2str_dump(uint32_t mask, j_bit2str_t bits_list[]);

/* Bit Manipulation Functions */
uint32_t uw_count_set_bits(uint32_t value);

#endif


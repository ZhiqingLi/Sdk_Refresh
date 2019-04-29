/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <includes.h>
#include <str_utils.h>
#include <jos/include/jos_stdlib.h>

char *j_strtok_r(char *str, const char *delim, char **saveptr)
{
    char *token = NULL;
    char *curr = NULL;
    int_t i = 0;

    if(!saveptr)
        return NULL;

    if(str)
        curr = str;
    else
        curr = *saveptr;

    /* scan input string */
    while (curr && *curr)
    {
        /* test current character against delimiters */
        for (i=0; delim[i]; i++)
        {
            if(*curr == delim[i])
            {
                /* if we have a token then return it */
                if(token)
                {
                    *curr = 0;
                    *saveptr = curr+1;
                    return token;
                }

                break;
            }
        }

        /* If curr is the first non delimiter we find, then its
         * the beginning of our token */
        if(!delim[i] && !token)
        {
            token = curr;
        }

        curr++;
    }

    /* reaching this point means we finished tokenizing the input string
     * which means we need to return the token (if any) now, and return
     * NULL for every next call */
    *saveptr = curr;
    return token;
}

/* Internal function used by snscanf() */
static bool_t parse_str_range(char **format, char *range, uint32_t size,
    bool_t *exclude)
{
    bool_t do_range = 0;
    unsigned char last = 0;
    char *range_beg = range;

    unsigned char *f = (*(unsigned char **)format)+1;

    if(*f == '^')
    {
        *exclude = 1;
        f++;
    }

    while (*f && (range-range_beg < size))
    {
        if((*f >= 'A' && *f <= 'Z') ||
            (*f >= 'a' && *f <= 'z') ||
            (*f >= '0' && *f <= '9'))
        {
            if(do_range)
            {
                while ((++last <= *f) && (range-range_beg < size))
                    *range++ = last;

                do_range = 0;
                last = 0;
            }
            else
            {
                last = *range = *f;
                range++;
            }

            f++;
            continue;
        }

        if(*f == '-' && last)
        {
            do_range = 1;
            f++;
            continue;
        }

        if(*f == ']')
        {
            *range = 0;
            if(os_strlen(range_beg))
            {
                *format = (char *)f;
                return 1;
            }
            else
                return 0;
        }

        *range = *f;
        last = 0;
        range++;
        f++;
    }

    return 0;
}

/*
 *   FUNCTION:   j_snscanf
 *
 *   General format string element: %[modifier]<directive>
 *
 *   This skinny implementation of sscanf adheres to the ANSI standard in the
 *   directives it does implement (see list below), with the following
 *   exceptions:
 *
 *   1. It is named sNscanf() since, for directives of unknown size, it
 *   requires a buffer size to be supplied in the argument list BEFORE the
 *   actual pointer to the buffer.
 *   Example:
 *      char str[80];
 *      j_snscanf(buf, "%s", sizeof(str), str);
 *
 *   2. It does not try to analyse the base of numeric values according to
 *   their prefixes, but rather infers the base from the directive
 *   (i.e. d=decimal, x=hex)
 *
 *   3. It does not recognise signed numbers, floating point numbers or any
 *   other variations defined in ANSI for parsing anything other than a simple
 *   unsigned value
 *
 *   4. The arguments provided for integer conversion MUST be 32-bit.
 *   data type modifiers such as h, l, etc are NOT supported.
 *
 *   5. If end_ptr is not NULL, snscanf will return a pointer to the next
 *   character that was to be processed, i.e. it will point to the character
 *   that did not match the format string, or to the terminating NULL.
 *
 *   Supported directives:
 *   - d or u = unsigned decimal number
 *   - x or X = unsigned hexadecimal number
 *   - s = string
 *   - [] = string composed of the characters specified within the brackets
 *   - [^] = string that does not contain any of the characters specified
 *           within the brackets
 *
 *   NOTE: for the [] & [^] directives, specifying ranges is also possible.
 *   ranges can be specified in lower case, upper case or numeric values, e.g:
 *      [a-zA-Z] = all letters
 *      [c-h2-5] = any string composed of the characters c,d,e,f,g,h,2,3,4,5
 *
 *   Supported modifiers (directive prefixes):
 *
 *   1. * = ignore element. e.g: "%*d" will parse the number from the input
 *   but does not use a scanf argument to write it into.
 *
 *   2. width = max array size. This is relevant only for s, [] & [^]
 *   directives. e.g: "%30s" will parse up to 30 characters into the target
 *   buffer provided
 *
 */
uint32_t j_snscanf(char *buf, char **end_ptr, char *format, ...)
{
    uint32_t args_parsed;
    va_list args;

    va_start(args, format);
    args_parsed = j_vsnscanf(buf, end_ptr, format, args);
    va_end(args);

    return args_parsed;
}

uint32_t j_vsnscanf(char *buf, char **end_ptr, char *format, va_list args)
{
    uint32_t args_parsed = 0;
    uint32_t max_size = UW_MAX_UINT32;
    uint32_t value = 0;
    uint32_t width = UW_MAX_UINT32;
    bool_t ignore = 0;
    bool_t str_range_do = 0;
    bool_t str_range_exclude = 0;
    char str_range[100];
    char *str_out = NULL;
    uint32_t *int_out = NULL;
    char *last_ptr;
    bool_t done = 0;
    enum { SCANF_ARG_DEC, SCANF_ARG_HEX, SCANF_ARG_STR, SCANF_ARG_SKIPWS,
        SCANF_ARG_MOD, SCANF_ARG_MATCH } arg_type = SCANF_ARG_MATCH;

    while (!done)
    {
        switch (arg_type)
        {
        case SCANF_ARG_DEC:
            value = j_strtonum(buf, 10, &last_ptr);
            if(value != NUMERIC_VALUE_INVALID)
            {
                if(!ignore)
                {
                    *int_out = value;
                    args_parsed++;
                }
                buf = last_ptr;
            }
            arg_type = SCANF_ARG_MATCH;
            break;

        case SCANF_ARG_HEX:
            value = j_strtonum(buf, 16, &last_ptr);
            if(value != NUMERIC_VALUE_INVALID)
            {
                if(!ignore)
                {
                    *int_out = value;
                    args_parsed++;
                }
                buf = last_ptr;
            }
            arg_type = SCANF_ARG_MATCH;
            break;

        case SCANF_ARG_STR:
            while (*buf && !j_is_space(*buf) && --max_size && width--)
            {
                if(str_range_do &&
                    (str_range_exclude ^ (!j_strchr(str_range, *buf))))
                {
                    if(!ignore)
                        args_parsed--;
                    break;
                }

                if(!ignore)
                {
                    *str_out = *buf;
                    str_out++;
                }

                buf++;
            }

            if(!ignore)
            {
                *str_out = 0;
                args_parsed++;
            }

            arg_type = SCANF_ARG_MATCH;
            break;

        case SCANF_ARG_SKIPWS:
            while (*buf && j_is_space(*buf))
                buf++;
            while (*format && j_is_space(*format))
                format++;
            arg_type = SCANF_ARG_MATCH;
            break;

        case SCANF_ARG_MOD:
            width = j_strtonum(format, 10, &last_ptr);
            if(width != NUMERIC_VALUE_INVALID)
            {
                format = last_ptr;
            }

            switch (*format)
            {
            case 'x':
            case 'X':
                if(!ignore)
                    int_out = va_arg(args, uint32_t *);
                arg_type = SCANF_ARG_HEX;
                break;

            case 'd':
            case 'u':
                if(!ignore)
                    int_out = va_arg(args, uint32_t *);
                arg_type = SCANF_ARG_DEC;
                break;

            case '[':
                str_range_do = parse_str_range(&format, str_range,
                    sizeof(str_range), &str_range_exclude);
                if(!str_range_do)
                {
                    return 0;
                }
                break;

            case 's':
                if(!ignore)
                {
                    max_size = va_arg(args, uint32_t);
                    str_out = va_arg(args, char *);
                }
                arg_type = SCANF_ARG_STR;
                break;

            case '*':
                ignore = 1;
                break;

            case '%':
                arg_type = SCANF_ARG_MATCH;
                format--;
                break;

            default:
                /* Unknown format directive */
                return 0;
            }
            format++;
            break;

        case SCANF_ARG_MATCH:
        default:
            max_size = UW_MAX_UINT32;
            width = UW_MAX_UINT32;
            ignore = 0;
            str_range_do = 0;
            str_range_exclude = 0;
            if(end_ptr)
                *end_ptr = buf;

            if(*format == '%')
            {
                arg_type = SCANF_ARG_MOD;
                format++;
                continue;
            }

            if(j_is_space(*format))
            {
                arg_type = SCANF_ARG_SKIPWS;
                continue;
            }

            if(!*buf || !*format || (*buf != *format))
                done = 1;

            buf++;
            format++;
            break;
        }
    }

    return args_parsed;
}


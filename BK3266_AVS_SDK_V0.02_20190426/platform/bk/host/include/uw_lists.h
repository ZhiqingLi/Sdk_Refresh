/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */
/*
 * Copyright (c) 1991, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef _UW_LISTS_H_
#define _UW_LISTS_H_

#ifndef _SYS_QUEUE_H_

/*
 * This file defines four types of data structures: singly-linked lists,
 * singly-linked tail queues, lists and tail queues.
 *
 * A singly-linked list is headed by a single forward pointer. The elements
 * are singly linked for minimum space and pointer manipulation overhead at
 * the expense of O(n) removal for arbitrary elements. New elements can be
 * added to the list after an existing element or at the head of the list.
 * Elements being removed from the head of the list should use the explicit
 * macro for this purpose for optimum efficiency. A singly-linked list may
 * only be traversed in the forward direction.  Singly-linked lists are ideal
 * for applications with large datasets and few or no removals or for
 * implementing a LIFO queue.
 *
 * A singly-linked tail queue is headed by a pair of pointers, one to the
 * head of the list and the other to the tail of the list. The elements are
 * singly linked for minimum space and pointer manipulation overhead at the
 * expense of O(n) removal for arbitrary elements. New elements can be added
 * to the list after an existing element, at the head of the list, or at the
 * end of the list. Elements being removed from the head of the tail queue
 * should use the explicit macro for this purpose for optimum efficiency.
 * A singly-linked tail queue may only be traversed in the forward direction.
 * Singly-linked tail queues are ideal for applications with large datasets
 * and few or no removals or for implementing a FIFO queue.
 *
 * A list is headed by a single forward pointer (or an array of forward
 * pointers for a hash table header). The elements are doubly linked
 * so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before
 * or after an existing element or at the head of the list. A list
 * may only be traversed in the forward direction.
 *
 * A tail queue is headed by a pair of pointers, one to the head of the
 * list and the other to the tail of the list. The elements are doubly
 * linked so that an arbitrary element can be removed without a need to
 * traverse the list. New elements can be added to the list before or
 * after an existing element, at the head of the list, or at the end of
 * the list. A tail queue may be traversed in either direction.
 *
 * For details on the use of these macros, see the queue(3) manual page.
 *
 *
 *                      SLIST   LIST    STAILQ  TAILQ
 * _HEAD                +       +       +       +
 * _HEAD_INITIALIZER    +       +       +       +
 * _ENTRY               +       +       +       +
 * _INIT                +       +       +       +
 * _EMPTY               +       +       +       +
 * _FIRST               +       +       +       +
 * _NEXT                +       +       +       +
 * _PREV                -       -       -       +
 * _LAST                -       -       +       +
 * _FOREACH             +       +       +       +
 * _FOREACH_REVERSE     -       -       -       +
 * _INSERT_HEAD         +       +       +       +
 * _INSERT_BEFORE       -       +       -       +
 * _INSERT_AFTER        +       +       +       +
 * _INSERT_TAIL         -       -       +       +
 * _REMOVE_HEAD         +       -       +       -
 * _REMOVE              -       +       -       +
 * _MOVE_HEAD           -       -       +       -
 *
 */

#define TAILQ_ENTRY(type)                                               \
struct {                                                                \
    struct type *tqe_next;  /* next element */                          \
    struct type **tqe_prev; /* address of previous next element */      \
}

#define TAILQ_HEAD(name, type)                                          \
struct name {                                                           \
    struct type *tqh_first; /* first element */                         \
    struct type **tqh_last; /* addr of last next element */             \
}

#define TAILQ_EMPTY(head)       ((head)->tqh_first == NULL)

#define TAILQ_FIRST(head)       ((head)->tqh_first)

#define TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)

#define TAILQ_PREV(elm, headname, field)                                \
    (*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))

#define TAILQ_REMOVE(head, elm, field) do {                             \
    if ((TAILQ_NEXT((elm), field)) != NULL)                             \
    {                                                                   \
        TAILQ_NEXT((elm), field)->field.tqe_prev =                      \
            (elm)->field.tqe_prev;                                      \
    }                                                                   \
    else                                                                \
    {                                                                   \
        (head)->tqh_last = (elm)->field.tqe_prev;                       \
    }                                                                   \
    *(elm)->field.tqe_prev = TAILQ_NEXT((elm), field);                  \
} while (0)

#define TAILQ_REMOVE_HEAD(head, field) do {                             \
    TAILQ_REMOVE(head, TAILQ_FIRST(head), field);                       \
} while (0)

#define TAILQ_FIND(list, item, type, next, found) do {                  \
    type curr_tf;                                                       \
    type next_tf;                                                       \
                                                                        \
    curr_tf = TAILQ_FIRST((list));                                      \
    while (curr_tf)                                                     \
    {                                                                   \
        next_tf = TAILQ_NEXT(curr_tf, next);                            \
        if (curr_tf == (item))                                          \
        {                                                               \
            found = 1;                                                  \
            break;                                                      \
        }                                                               \
        curr_tf = next_tf;                                              \
    }                                                                   \
} while (0)

#define TAILQ_FIND_AND_REMOVE(list, item, type, next, removed) do {     \
    type curr_tf;                                                       \
    type next_tf;                                                       \
                                                                        \
    removed = 0;                                                        \
    curr_tf = TAILQ_FIRST((list));                                      \
    while (curr_tf)                                                     \
    {                                                                   \
        next_tf = TAILQ_NEXT(curr_tf, next);                            \
        if (curr_tf == (item))                                          \
        {                                                               \
            TAILQ_REMOVE((list), curr_tf, next);                        \
            removed = 1;                                                \
            UNUSED_VAR(removed);                                        \
        }                                                               \
        curr_tf = next_tf;                                              \
    }                                                                   \
} while (0)

#define TAILQ_INSERT_TAIL(head, elm, field) do {                        \
    TAILQ_NEXT((elm), field) = NULL;                                    \
    (elm)->field.tqe_prev = (head)->tqh_last;                           \
    *(head)->tqh_last = (elm);                                          \
    (head)->tqh_last = &TAILQ_NEXT((elm), field);                       \
} while (0)

#define TAILQ_INSERT_HEAD(head, elm, field)  do {                       \
    if ((TAILQ_NEXT((elm), field) = TAILQ_FIRST((head))) != NULL)       \
    {                                                                   \
        TAILQ_FIRST((head))->field.tqe_prev = &TAILQ_NEXT((elm), field);\
    }                                                                   \
    else                                                                \
    {                                                                   \
        (head)->tqh_last = &TAILQ_NEXT((elm), field);                   \
    }                                                                   \
    TAILQ_FIRST((head)) = (elm);                                        \
    (elm)->field.tqe_prev = &TAILQ_FIRST((head));                       \
} while (0)

#define TAILQ_HEAD_INITIALIZER(head)                                    \
    { NULL, &(head).tqh_first }

#define TAILQ_INIT(head) do {                                           \
    TAILQ_FIRST((head)) = NULL;                                         \
    (head)->tqh_last = &TAILQ_FIRST((head));                            \
} while (0)

#define TAILQ_INSERT_AFTER(head, listelm, elm, field) do {              \
    if ((TAILQ_NEXT((elm), field) = TAILQ_NEXT((listelm), field)) != NULL) \
    {                                                                   \
        TAILQ_NEXT((elm), field)->field.tqe_prev = &TAILQ_NEXT((elm),   \
            field);                                                     \
    }                                                                   \
    else                                                                \
    {                                                                   \
        (head)->tqh_last = &TAILQ_NEXT((elm), field);                   \
    }                                                                   \
    TAILQ_NEXT((listelm), field) = (elm);                               \
    (elm)->field.tqe_prev = &TAILQ_NEXT((listelm), field);              \
} while (0)

#define TAILQ_INSERT_BEFORE(listelm, elm, field) do {                   \
    (elm)->field.tqe_prev = (listelm)->field.tqe_prev;                  \
    TAILQ_NEXT((elm), field) = (listelm);                               \
    *(listelm)->field.tqe_prev = (elm);                                 \
    (listelm)->field.tqe_prev = &TAILQ_NEXT((elm), field);              \
} while (0)

#define TAILQ_LAST(head, headname)                                      \
    (*(((struct headname *)((head)->tqh_last))->tqh_last))

#define TAILQ_FOREACH(var, head, field)                                 \
    for ((var) = TAILQ_FIRST((head));                                   \
        (var);                                                          \
        (var) = TAILQ_NEXT((var), field))

#define TAILQ_FOREACH_SAFE(var, head, field, tvar)                      \
    for ((var) = TAILQ_FIRST((head));                                   \
        (var) && ((tvar) = TAILQ_NEXT((var), field), 1);                \
        (var) = (tvar))

#define TAILQ_FOREACH_REVERSE(var, head, headname, field)               \
    for ((var) = TAILQ_LAST((head), headname);                          \
        (var);                                                          \
        (var) = TAILQ_PREV((var), headname, field))

/*
 * Singly-linked Tail queue declarations.
 */
#define STAILQ_HEAD(name, type)                                         \
    struct name {                                                       \
        struct type *stqh_first;/* first element */                     \
        struct type **stqh_last;/* addr of last next element */         \
    }

#define STAILQ_ENTRY(type)                                              \
    struct {                                                            \
        struct type *stqe_next; /* next element */                      \
    }

/*
 * Singly-linked Tail access methods
 */
#define STAILQ_EMPTY(head)      ((head)->stqh_first == NULL)
#define STAILQ_FIRST(head)      ((head)->stqh_first)
#define STAILQ_LAST(head, type, field)                                  \
    (STAILQ_EMPTY((head)) ?                                             \
        NULL :                                                          \
        ((type *)(void *)                                               \
        ((char *)((head)->stqh_last) - ((char *)&(((type *)0)->field)))))

#define STAILQ_NEXT(elm, field) ((elm)->field.stqe_next)

/*
 * Singly-linked Tail queue functions
 */
#define STAILQ_REMOVE_HEAD(head, field) do {                            \
    if ((STAILQ_FIRST((head)) =                                         \
        STAILQ_NEXT(STAILQ_FIRST((head)), field)) == NULL)              \
    (head)->stqh_last = &STAILQ_FIRST((head));                          \
} while (0)

#define STAILQ_INSERT_HEAD(head, elm, field) do {                       \
    if ((STAILQ_NEXT((elm), field) = STAILQ_FIRST((head))) == NULL)     \
    (head)->stqh_last = &STAILQ_NEXT((elm), field);                     \
    STAILQ_FIRST((head)) = (elm);                                       \
} while (0)

#define STAILQ_MOVE_HEAD(head_src, head_dst) do {                       \
    if (STAILQ_FIRST(head_src))                                         \
    {                                                                   \
        (head_dst)->stqh_first = (head_src)->stqh_first;                \
        (head_dst)->stqh_last = (head_src)->stqh_last;                  \
        STAILQ_INIT(head_src);                                          \
    }                                                                   \
    else                                                                \
    {                                                                   \
        STAILQ_INIT(head_dst);                                          \
    }                                                                   \
} while (0)

#define STAILQ_INSERT_TAIL(head, elm, field) do {                       \
    STAILQ_NEXT((elm), field) = NULL;                                   \
    *(head)->stqh_last = (elm);                                         \
    (head)->stqh_last = &STAILQ_NEXT((elm), field);                     \
} while (0)

#define STAILQ_FOREACH(var, head, field)                                \
    for ((var) = STAILQ_FIRST((head));                                  \
        (var);                                                          \
        (var) = STAILQ_NEXT((var), field))

#define STAILQ_INIT(head) do {                                          \
    STAILQ_FIRST((head)) = NULL;                                        \
    (head)->stqh_last = &STAILQ_FIRST((head));                          \
} while (0)

/*
 * List definitions.
 */
#ifdef LIST_HEAD
#undef LIST_HEAD
#endif
#define LIST_HEAD(name, type)                                           \
    struct name {                                                       \
        struct type *lh_first;  /* first element */                     \
    }

#define LIST_ENTRY(type)                                                \
    struct {                                                            \
        struct type *le_next;   /* next element */                      \
        struct type **le_prev;  /* address of previous next element */  \
    }

/*
 * List access methods.
 */
#define LIST_EMPTY(head)        ((head)->lh_first == NULL)
#define LIST_FIRST(head)        ((head)->lh_first)
#define LIST_NEXT(elm, field)   ((elm)->field.le_next)

/*
 * List functions.
 */
#define LIST_HEAD_INITIALIZER(head)  { NULL }

#define LIST_INIT(head) do {                                            \
    LIST_FIRST((head)) = NULL;                                          \
} while (0)

#define LIST_FOREACH(var, head, field)                                  \
    for ((var) = LIST_FIRST((head));                                    \
        (var);                                                          \
        (var) = LIST_NEXT((var), field))

#define LIST_FOREACH_SAFE(var, head, field, tvar)                       \
    for ((var) = LIST_FIRST((head));                                    \
        (var) && ((tvar) = LIST_NEXT((var), field), 1);                 \
        (var) = (tvar))

#define LIST_INSERT_AFTER(listelm, elm, field) do {                     \
    if ((LIST_NEXT((elm), field) = LIST_NEXT((listelm), field)) != NULL)\
    {                                                                   \
        LIST_NEXT((listelm), field)->field.le_prev =                    \
            &LIST_NEXT((elm), field);                                   \
    }                                                                   \
    LIST_NEXT((listelm), field) = (elm);                                \
    (elm)->field.le_prev = &LIST_NEXT((listelm), field);                \
} while (0)

#define LIST_INSERT_BEFORE(listelm, elm, field) do {                    \
    (elm)->field.le_prev = (listelm)->field.le_prev;                    \
    LIST_NEXT((elm), field) = (listelm);                                \
    *(listelm)->field.le_prev = (elm);                                  \
    (listelm)->field.le_prev = &LIST_NEXT((elm), field);                \
} while (0)

#define LIST_INSERT_HEAD(head, elm, field) do {                         \
    if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != NULL)         \
        LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field);   \
    LIST_FIRST((head)) = (elm);                                         \
    (elm)->field.le_prev = &LIST_FIRST((head));                         \
} while (0)

#define LIST_REMOVE(elm, field) do {                                    \
    if (LIST_NEXT((elm), field) != NULL)                                \
    {                                                                   \
        LIST_NEXT((elm), field)->field.le_prev =                        \
            (elm)->field.le_prev;                                       \
    }                                                                   \
    *(elm)->field.le_prev = LIST_NEXT((elm), field);                    \
} while (0)

/*
 * Singly-linked List definitions.
 */
#define SLIST_HEAD(name, type)                                          \
    struct name {                                                       \
        struct type *slh_first; /* first element */                     \
    }

#define SLIST_HEAD_INITIALIZER(head)                                    \
    { NULL }

#define SLIST_ENTRY(type)                                               \
    struct {                                                            \
        struct type *sle_next;  /* next element */                      \
    }

/*
 * Singly-linked List functions.
 */
#define SLIST_INIT(head) do {                                           \
    (head)->slh_first = NULL;                                           \
} while (0)

#define SLIST_INSERT_AFTER(slistelm, elm, field) do {                   \
    (elm)->field.sle_next = (slistelm)->field.sle_next;                 \
    (slistelm)->field.sle_next = (elm);                                 \
} while (0)

#define SLIST_INSERT_HEAD(head, elm, field) do {                        \
    (elm)->field.sle_next = (head)->slh_first;                          \
    (head)->slh_first = (elm);                                          \
} while (0)

#define SLIST_REMOVE_HEAD(head, field) do {                             \
    (head)->slh_first = (head)->slh_first->field.sle_next;              \
} while (0)

#define SLIST_REMOVE_AFTER(slistelm, field) do {                        \
    (slistelm)->field.sle_next =                                        \
        SLIST_NEXT(SLIST_NEXT((slistelm), field), field);               \
} while (0)

#define SLIST_FOREACH(var, head, field)                                 \
    for ((var) = (head)->slh_first; (var); (var) = (var)->field.sle_next)

/*
 * Singly-linked List access methods.
 */
#define SLIST_EMPTY(head)       ((head)->slh_first == NULL)
#define SLIST_FIRST(head)       ((head)->slh_first)
#define SLIST_NEXT(elm, field)  ((elm)->field.sle_next)

#endif
#endif


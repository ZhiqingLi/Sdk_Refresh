#include <stdio.h>
#include <string.h>
#include "kfifo.h"
#include "hal_trace.h"

static int is_power_of_2(unsigned int n)  
{  
    int r = 0;
    if (n != 0 && ((n & (n - 1)) == 0))
        r = 1;

    return r;
}

void kfifo_init(struct kfifo *fifo, unsigned char *buffer, unsigned int len)
{
    ASSERT(is_power_of_2(len), "kfifo_init : len %d not power of 2!!!!!", len);

    fifo->size = len;
    fifo->buffer = buffer;
    fifo->in = fifo->out = 0;
}

void kfifo_clean(struct kfifo *fifo)
{
    fifo->in = fifo->out = 0;
}

unsigned int kfifo_put(struct kfifo *fifo, unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    len = MIN(len, fifo->size - fifo->in + fifo->out); 
    __sync_synchronize();

    l = MIN(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
    memcpy(fifo->buffer, buffer + l, len - l);

    __sync_synchronize();
    fifo->in += len;

    return len;
}
unsigned int kfifo_get(struct kfifo *fifo, unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    len = MIN(len, fifo->in - fifo->out);
    __sync_synchronize();

    l = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    memcpy(buffer + l, fifo->buffer, len - l);

    __sync_synchronize();
    fifo->out += len;

    return len;
}
unsigned int kfifo_peek(struct kfifo *fifo, unsigned int len_want, unsigned char **buff1, unsigned char **buff2, unsigned int *len1, unsigned int *len2)
{
    unsigned int l,len;

    *buff1 = *buff2 = NULL;
    *len1 = *len2 = NULL;
    len = MIN(len_want, fifo->in - fifo->out);
    if (len < len_want) {
        return 0;
    }

    __sync_synchronize();
    l = MIN(len, fifo->size - (fifo->out & (fifo->size - 1)));
    *buff1 = fifo->buffer + (fifo->out & (fifo->size - 1));
    *len1 = l;
    if (l < len) {
        *buff2 = fifo->buffer;
        *len2 = len-l;
    }

    return len_want;
}
unsigned int kfifo_len(struct kfifo *fifo)
{
    return (fifo->in-fifo->out);
}

#if 0
struct kfifo test_kfifo;
unsigned char kfifo_buffer[32];
void kfifo_test(void)
{
    unsigned char b[10];
    kfifo_init(&test_kfifo, kfifo_buffer, 32);
    TRACE("init 32 : fifo size %d", test_kfifo.size);

    kfifo_put(&test_kfifo, "1234567890", 10);
    TRACE("put 10 : fifo  len %d", kfifo_len(&test_kfifo));

    kfifo_put(&test_kfifo, "abcdefghij", 10);
    TRACE("put 10 : fifo  len %d", kfifo_len(&test_kfifo));

    kfifo_put(&test_kfifo, "!@#$%^&*()", 10);
    TRACE("put 10 : fifo  len %d", kfifo_len(&test_kfifo));

    kfifo_put(&test_kfifo, "VVVV", 4);
    TRACE("put 4 : fifo  len %d", kfifo_len(&test_kfifo));

    kfifo_get(&test_kfifo, b, 2);
    TRACE("get 2 : fifo  len %d", kfifo_len(&test_kfifo));
    TRACE("data:");
    for(int i = 0; i < 2; ++i) {
        TRACE("%c-", b[i]);
    }
    TRACE(" ");
    kfifo_get(&test_kfifo, b, 10);
    TRACE("get 10 : fifo  len %d", kfifo_len(&test_kfifo));
    TRACE("data:");
    for(int i = 0; i < 10; ++i) {
        TRACE("%c-", b[i]);
    }
    TRACE(" ");
    kfifo_get(&test_kfifo, b, 10);
    TRACE("get 10 : fifo  len %d", kfifo_len(&test_kfifo));
    TRACE("data:");
    for(int i = 0; i < 10; ++i) {
        TRACE("%c-", b[i]);
    }
    TRACE(" ");
    kfifo_get(&test_kfifo, b, 10);
    TRACE("get 10 : fifo  len %d", kfifo_len(&test_kfifo));
    TRACE("data:");
    for(int i = 0; i < 10; ++i) {
        TRACE("%c-", b[i]);
    }
    TRACE(" ");
}
#endif

//#include <stdio.h>
#include "led_driver.h"
#include <pb_encode.h>
#include <pb_decode.h>
//#include "simple.pb.h"
#include "message.h"

#ifdef NANOPB_TEST
int test_nanopb(void)
{
    /* This is the buffer where we will store our message. */
    uint8_t buffer[128];
    size_t message_length;
    bool status;

    pb_ostream_t ostream;
    pb_istream_t istream;

    /* Encode our message */
    {
        /* Allocate space on the stack to store the message data.
         *
         * Nanopb generates simple struct definitions for all the messages.
         * - check out the contents of simple.pb.h!
         * It is a good idea to always initialize your structures
         * so that you do not have garbage data from RAM in there.
         */
        SimpleMessage message = SimpleMessage_init_zero;

        /* Create a stream that will write to our buffer. */
        drv_pb_ostream_from_buffer(buffer, sizeof(buffer), &ostream);

        /* Fill in the lucky number */
        message.lucky_number = 13;

        /* Now we are ready to encode the message! */
        status = drv_pb_encode(&ostream, SimpleMessage_fields, &message);
        message_length = ostream.bytes_written;

        /* Then just check for any errors.. */
        if(!status) {
            //printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
            libc_print("Encoding failed", 0, 0);
            return 1;
        }
    }

    /* Now we could transmit the message over network, store it in a file or
     * wrap it to a pigeon's leg.
     */

    /* But because we are lazy, we will just decode it immediately. */

    {
        /* Allocate space for the decoded message. */
        SimpleMessage message = SimpleMessage_init_zero;

        /* Create a stream that reads from the buffer. */
        drv_pb_istream_from_buffer(buffer, message_length, &istream);

        /* Now we are ready to decode the message. */
        status = drv_pb_decode(&istream, SimpleMessage_fields, &message);

        /* Check for errors... */
        if(!status) {
            //printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            libc_print("Decoding failed", 0, 0);
            return 1;
        }

        /* Print the data contained in the message. */
        //printf("Your lucky number was %d!\n", (int)message.lucky_number);
        libc_print("Your lucky number was", (int)message.lucky_number, 2);

    }

    return 0;
}
#endif

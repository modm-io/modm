/*
 * Copyright (c) 2022, Lucas Moesch
 * Copyright (c) 2022, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This file originated from the nanopb project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <pb_encode.h>
#include <pb_decode.h>
#include <simple.pb.hpp>
#include <complex.pb.hpp>

int main()
{
    Board::initialize();

    /* This is the buffer where we will store our message. */
    uint8_t buffer[128];
    size_t message_length;
    bool status;

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
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        /* Fill in the lucky number */
        message.lucky_number = 42;

        /* Now we are ready to encode the message! */
        status = pb_encode(&stream, SimpleMessage_fields, &message);
        modm_assert(status, "pb.enc", "Encoding SimpleMessage failed!");
        message_length = stream.bytes_written;
        modm_assert(message_length, "pb.len", "Empty SimpleMessage buffer!");
    }
    {
        /* Now we could transmit the message over network, store it in a file or
         * wrap it to a pigeon's leg.
         */

        /* But because we are lazy, we will just decode it immediately. */

        /* Allocate space for the decoded message. */
        SimpleMessage message = SimpleMessage_init_zero;

        /* Create a stream that reads from the buffer. */
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

        /* Now we are ready to decode the message. */
        status = pb_decode(&stream, SimpleMessage_fields, &message);
        modm_assert(status, "pb.dec", "Decoding SimpleMessage failed!");
        modm_assert(message.lucky_number == 42, "lucky_number", "Incorrect SimpleMessage values!");
    }

    {
        /* Allocate space on the stack to store the message data.
         *
         * Nanopb generates simple struct definitions for all the messages.
         * - check out the contents of simple.pb.h!
         * It is a good idea to always initialize your structures
         * so that you do not have garbage data from RAM in there.
         */
        ComplexMessage message = ComplexMessage_init_zero;

        /* Create a stream that will write to our buffer. */
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        /* Fill in the unlucky number */
        std::strcpy(message.query, "Hello World");
        message.unlucky_number = 13;
        message.toggle = true;
        message.value = 4.00012f;

        /* Now we are ready to encode the message! */
        status = pb_encode(&stream, ComplexMessage_fields, &message);
        modm_assert(status, "pb.enc", "Encoding ComplexMessage failed!");
        message_length = stream.bytes_written;
        modm_assert(message_length, "pb.len", "Empty ComplexMessage buffer!");
    }
    {
        /* Now we could transmit the message over network, store it in a file or
         * wrap it to a pigeon's leg.
         */

        /* But because we are lazy, we will just decode it immediately. */

        /* Allocate space for the decoded message. */
        ComplexMessage message = ComplexMessage_init_zero;

        /* Create a stream that reads from the buffer. */
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

        /* Now we are ready to decode the message. */
        status = pb_decode(&stream, ComplexMessage_fields, &message);
        modm_assert(status, "pb.dec", "Decoding ComplexMessage failed!");

        modm_assert(!strcmp(message.query, "Hello World"), "query", "Incorrect ComplexMessage values!");
        modm_assert(message.unlucky_number == 13, "unlucky_number", "Incorrect ComplexMessage values!");
        modm_assert(message.toggle == true, "toggle", "Incorrect ComplexMessage values!");
        modm_assert(int(message.value) == 4, "value", "Incorrect ComplexMessage values!");
    }

    while (true)
    {
        /* tumbleweed */
    }
    return 0;
}

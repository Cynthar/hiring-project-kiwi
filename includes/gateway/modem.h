#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MODEM_MAX_PAYLOAD_LENGTH 128

/**
 * Checks if there is an incoming packet from the backend available, if there is
 * writes a pointer to the packet to `*data`, the length of the packet to
 * `*length` and returns true; if there wasn't then returns false and doesn't
 * touch `data` or `length`. The maximum length of the buffer returned will be
 * MODEM_MAX_PAYLOAD_LENGTH.
 *
 * The buffer returned in `*data` is valid until the next time that
 * `modem_dequeue_incoming` is called.
 */
bool modem_dequeue_incoming(uint8_t const **data, size_t *length);

/**
 * Enqueues a packet to be sent to the backend, reads `length` bytes from
 * `data`. The data is copied out during this call so `data` can be reused as
 * soon as this returns. `length` must be less than MODEM_MAX_PAYLOAD_LENGTH.
 */
void modem_enqueue_outgoing(uint8_t const *data, size_t length);

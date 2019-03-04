#pragma once

#include <stdint.h>
#include <stdbool.h>

#define WIRELESS_PAYLOAD_LENGTH 32

/**
 * Checks if there is an incoming packet from a gateway available, if there is
 * writes the data of the packet to
 * `*data` and returns true; if there wasn't then returns false and doesn't
 * touch `data`.
 */
bool wireless_dequeue_incoming(uint8_t data[static WIRELESS_PAYLOAD_LENGTH]);

/**
 * Enqueues a packet to be sent to a gateway.  The data is copied out during
 * this call so `data` can be reused as soon as this returns.
 */
void wireless_enqueue_outgoing(uint8_t const data[static WIRELESS_PAYLOAD_LENGTH]);

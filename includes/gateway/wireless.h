#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "common/device.h"

#define WIRELESS_PAYLOAD_LENGTH 32

/**
 * Checks if there is an incoming packet from a sensor available, if there is
 * writes the id of the sensor to `*device_id`, the data of the packet to
 * `*data` and returns true; if there wasn't then returns false and doesn't
 * touch `device_id` or `data`.
 */
bool wireless_dequeue_incoming(
    device_id_t *device_id,
    uint8_t data[static WIRELESS_PAYLOAD_LENGTH]);

/**
 * Enqueues a packet to be sent to a sensor.  The data is copied out during
 * this call so `data` can be reused as soon as this returns.
 */
void wireless_enqueue_outgoing(
    device_id_t device_id,
    uint8_t const data[static WIRELESS_PAYLOAD_LENGTH]);

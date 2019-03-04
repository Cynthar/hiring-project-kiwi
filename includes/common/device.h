#pragma once

typedef union
{
  uint8_t bytes[16];
  uint32_t words[4];
} device_id_t;

/**
 * Get the current device's unique id.
 */
device_id_t get_device_id(void);

/**
 * Initiates a soft reset of the device.
 */
__attribute__((noreturn))
void reset_device(void);

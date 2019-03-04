#pragma once

#include <stdint.h>
#include <stdbool.h>

#define KI_TOKEN_LENGTH 16

typedef enum
{
  KI_STORE_SUCCESS,
  KI_STORE_ERROR_FULL,
  KI_STORE_ERROR_UNKNOWN,
} ki_store_result_t;

/**
 * Adds a token to the ki store, returns KI_STORE_SUCCESS if the token already
 * exists in the store or adding it was successful, returns an error code if
 * adding the token was unsuccessful.
 */
__attribute__((warn_unused_result))
ki_store_result_t ki_store_add(uint8_t const token[static KI_TOKEN_LENGTH]);

/**
 * Removes a token from the ki store, returns KI_STORE_SUCCESS if the token did
 * not exist in the store or removing it was successful, returns an error code
 * if removing the token was unsuccessful.
 */
__attribute__((warn_unused_result))
ki_store_result_t ki_store_remove(uint8_t const token[static KI_TOKEN_LENGTH]);

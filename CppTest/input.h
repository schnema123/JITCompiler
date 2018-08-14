#pragma once

#include <stdint.h>

#include "error.h"

typedef struct
{
	char instruction;
	int64_t operand;
} instruction;

#define MAX_NUM_INSTRCTIONS 256
typedef struct
{
	instruction instructions[MAX_NUM_INSTRCTIONS];
	size_t num_instructions;

	int start_value;
	int num_outputs;
} input_buffer;

error_t input_read(input_buffer *);
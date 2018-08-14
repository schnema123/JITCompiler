#pragma once

#include <stdint.h>

#include "input.h"

typedef int64_t(*function_ptr)(int64_t);

typedef struct
{
	uint8_t *buffer;
	size_t buffer_size;
	size_t current_size;

	function_ptr function_ptr;
} assembler_buffer;

error_t assembler_allocate_buffer(assembler_buffer *);
error_t assembler_assemble_instructions(assembler_buffer *, input_buffer *);
error_t assembler_finalize_buffer(assembler_buffer *);

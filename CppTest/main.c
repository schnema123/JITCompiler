#include <stdio.h>

#include "error.h"
#include "input.h"
#include "assembler.h"

int main(int argc, char **argv)
{
	input_buffer input_buffer = {};
	input_read(&input_buffer);

	assembler_buffer assembler_buffer = {};
	assembler_allocate_buffer(&assembler_buffer);
	assembler_assemble_instructions(&assembler_buffer, &input_buffer);
	assembler_finalize_buffer(&assembler_buffer);

	int64_t value = input_buffer.start_value;
	for (size_t i = 0; i <= input_buffer.num_outputs; i++)
	{
		printf("Term %lld: %lld\n", i, value);
		value = assembler_buffer.function_ptr(value);
	}

	getchar();
}
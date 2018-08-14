#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "input.h"
#include "error.h"

error_t parse_instructions(input_buffer *input_buffer, const char *instruction_line)
{
	size_t instruction_line_size = strlen(instruction_line) + 1;
	char instruction_line_cpy[instruction_line_size];
	strcpy_s(instruction_line_cpy, instruction_line_size, instruction_line);

	char *context;
	char *token = strtok_s(instruction_line_cpy, " ", &context);
	while (token)
	{
		instruction current_instruction = {};

		current_instruction.instruction = token[0];
		token++;
		current_instruction.operand = strtol(token, NULL, 10);

		input_buffer->instructions[input_buffer->num_instructions] = current_instruction;
		input_buffer->num_instructions++;

		token = strtok_s(NULL, " ", &context);
	}

	return ERROR_OK;
}

error_t input_read(input_buffer *input_buffer)
{
	FILE *f;
	errno_t file_open_state = fopen_s(&f, "input.txt", "r");

	if (file_open_state != 0)
		return ERROR_INPUT;

	char instruction_line[1024];
	char starting_value_line[1024];
	char num_outputs_line[1024];

	fgets(instruction_line, 1024, f);
	fgets(starting_value_line, 1024, f);
	fgets(num_outputs_line, 1024, f);

	fclose(f);

	parse_instructions(input_buffer, instruction_line);
	input_buffer->start_value = strtol(starting_value_line, NULL, 10);
	input_buffer->num_outputs = strtol(num_outputs_line, NULL, 10);

	return ERROR_OK;
}
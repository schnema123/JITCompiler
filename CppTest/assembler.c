#include "assembler.h"

#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.H>
#include <WinBase.h>

error_t assembler_allocate_buffer(assembler_buffer *buffer)
{
	// Get page size
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	DWORD page_size = system_info.dwPageSize;

	// Allocate size
	void *allocated_buffer = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (allocated_buffer == NULL)
		return ERROR_MEMORY;

	buffer->buffer = allocated_buffer;
	buffer->buffer_size = page_size;
	buffer->current_size = 0;

	return ERROR_OK;
}

static error_t assembler_add_bytes_to_buffer(assembler_buffer *buffer, const uint8_t *bytes, size_t bytes_size)
{
	size_t remaining_size
		= buffer->buffer_size - buffer->current_size;

	if (bytes_size > remaining_size)
		return ERROR_MEMORY;

	memcpy_s(buffer->buffer + buffer->current_size, remaining_size, bytes, bytes_size);
	buffer->current_size += bytes_size;

	return ERROR_OK;
}

const uint8_t add_rcx[] = {0x48, 0x03, 0xC1};
const uint8_t sub_rcx[] = {0x48, 0x2B, 0xC1};
const uint8_t imul_rcx[] = {0x48, 0x0F, 0xAF, 0xC1};
const uint8_t idiv_rcx[] = {0x48, 0xF7, 0xF9};

error_t assembler_assemble_instructions(assembler_buffer *buffer, input_buffer *input_buffer)
{
	// Initialize function state

	// Move input to RAX
	uint8_t move_input_to_rax[] = {0x48, 0x8B, 0xC1};
	assembler_add_bytes_to_buffer(buffer, move_input_to_rax, sizeof(move_input_to_rax));

	// For each instruction,
	// Move the operand to RCX and execute the instruction on RAX
	uint8_t move_to_rcx[] = {0x48, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	// A pointer to the above value FFFFFFFFFFFFFFFF
	// Edit this to change the value RCX gets set to
	uint64_t *value_rcx = (uint64_t*) (move_to_rcx + 2);

	for (size_t i = 0; i < input_buffer->num_instructions; i++)
	{
		const instruction *cur_instruction = input_buffer->instructions + i;

		*value_rcx = cur_instruction->operand;
		assembler_add_bytes_to_buffer(buffer, move_to_rcx, sizeof(move_to_rcx));

		switch (cur_instruction->instruction)
		{
		case '+':
			assembler_add_bytes_to_buffer(buffer, add_rcx, sizeof(add_rcx));
			break;
		case '-':
			assembler_add_bytes_to_buffer(buffer, sub_rcx, sizeof(sub_rcx));
			break;
		case '*':
			assembler_add_bytes_to_buffer(buffer, imul_rcx, sizeof(imul_rcx));
			break;
		case '/':
			assembler_add_bytes_to_buffer(buffer, idiv_rcx, sizeof(idiv_rcx));
			break;
		}
	}

	uint8_t ret[] = {0xC3};
	assembler_add_bytes_to_buffer(buffer, ret, sizeof(ret));

	return ERROR_OK;
}

error_t assembler_finalize_buffer(assembler_buffer *buffer)
{
	DWORD old_protect;
	if (VirtualProtect(buffer->buffer, buffer->buffer_size, PAGE_EXECUTE_READ, &old_protect) == FALSE)
		return ERROR_MEMORY;

	buffer->function_ptr = (function_ptr) buffer->buffer;

	return ERROR_OK;
}
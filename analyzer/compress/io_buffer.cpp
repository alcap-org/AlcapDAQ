//////////////////////////////////////////////////////////////////////
// 
// Bitwise I/O buffers.
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <assert.h>

#include "midas.h"

#include "mucap_compress.h"

void start_output_buffer(io_buffer * buffer, unsigned char *p)
{
  buffer->w = 0;
  buffer->w_bits = 32;
  buffer->start = (unsigned int *) p;
  buffer->p = buffer->start + 1;
  buffer->num_codes = 0;
}

int flush_output_buffer(io_buffer * buffer)
{
  // store the last word
  *((buffer->p)++) = buffer->w << buffer->w_bits;

  // remember the length of the buffer, in bytes and in codes
  *(buffer->start) = buffer->num_codes;

  //return ((unsigned int) buffer->p) - ((unsigned int) buffer->start);
  return (int) (((char *) buffer->p) - ((char *) buffer->start));
}

void
io_buffer_put(io_buffer * buffer, unsigned int code_bits, int code_length)
{

  assert(code_length > 0 && code_length < 32);

  // Does this symbol fit in our buffer word?
  if (code_length < buffer->w_bits) {
    // OK, so put it there...
    buffer->w = (buffer->w << code_length) | code_bits;
    buffer->w_bits -= code_length;
  } else if (code_length == buffer->w_bits) {
    // Only barely...write out the buffer word
    buffer->w = (buffer->w << code_length) | code_bits;
    *((buffer->p)++) = buffer->w;
    buffer->w_bits = 32;
  } else {
    // No, we'll have to split the code across two words.
    unsigned int first_code_bits =
	code_bits >> (code_length - buffer->w_bits);
    *((buffer->p)++) = (buffer->w << buffer->w_bits) | first_code_bits;
    buffer->w = code_bits;
    buffer->w_bits = 32 - code_length + buffer->w_bits;
  }
}

void start_input_buffer(io_buffer * buffer, unsigned char *p)
{
  buffer->w = 0;
  buffer->w_bits = 0;
  buffer->start = (unsigned int *) p;
  buffer->p = buffer->start + 1;
  buffer->num_codes = *(buffer->start);
}

unsigned int io_buffer_get(io_buffer * buffer, int num_bits)
{
  if (buffer->w_bits == 0) {
    buffer->w = *((buffer->p)++);
    buffer->w_bits = 32;
  }

  int w_bits = buffer->w_bits;

  unsigned int retval;
  if (num_bits <= w_bits) {
    retval = buffer->w >> (32 - num_bits);
    buffer->w = buffer->w << num_bits;
    buffer->w_bits = w_bits - num_bits;

  } else {
    unsigned int first_part =
	io_buffer_get(buffer, w_bits) << (num_bits - w_bits);
    retval = first_part | io_buffer_get(buffer, num_bits - w_bits);
  }

  return retval;
}


//////////////////////////////////////////////////////////////////////
// 
// Run-length encoding and decoding of bit patterns.
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "midas.h"

#include "mucap_compress.h"

static int rle_bits_setup_done = 0;
static char num_leading_0[256];
static char num_leading_1[256];
static unsigned char ones[9];

int num_leading_bits(unsigned char n, int bit_val)
{
  // 
  // Computes the number of leading 0s or 1s (selected by bit_val) in a
  // given byte.  For example, the number of leading 0s in 00010110 is
  // 3,
  // and the number of leading 1s is 0.
  // 

  int retval = 0;

  for (int i = 0; i < 8; i++) {
    int bit_i = (n >> (7 - i)) & 1;
    if (bit_i == bit_val) {
      retval++;
    } else {
      break;
    }
  }

  return retval;
}

void rle_bits_setup()
{
  if(rle_bits_setup_done) {
    return;
  }

  // 
  // Cache the number of leading 0s and 1s for all 8-bit values.
  // 
  for (int i = 0; i < 256; i++) {
    num_leading_0[i] = num_leading_bits(i, 0);
    num_leading_1[i] = num_leading_bits(i, 1);
  }

  // 
  // Cache the bit-strings consisting of all 1s, with lengths from 1 to
  // 8.
  // 
  short o = 0;
  for (int i = 0; i < 9; i++) {
    ones[i] = o;
    o = (o << 1) | 1;
  }
}

void rle_put(unsigned char b, rle_state * s, io_buffer * output)
{
  int bits = 8;

  while (bits > 0) {
    int nr;

    if (s->running_digit == 0) {
      nr = num_leading_0[b];
    } else {
      nr = num_leading_1[b];
    }

    if (nr > bits) {
      nr = bits;
    }

    s->run_length += nr;
    bits -= nr;

    if (bits != 0 || (!s->use_extension_word && 
 	             s->run_length >= s->huffman_length[s->running_digit]-8)) {
      b = b << nr;

      if(s->run_length < s->huffman_length[s->running_digit]) {
        huffman_put_symbol(&s->huffman[s->running_digit], output, s->run_length);
      } else {
        huffman_put_symbol(&s->huffman[s->running_digit], output, s->huffman_length[s->running_digit]);
        io_buffer_put(output,s->run_length, 24);
       
      }
      s->running_digit = !s->running_digit;
      s->run_length = 0;
    }
  }
}

int flush_rle(rle_state * s, io_buffer * output)
{
  if (s->run_length != 0) {
      if(s->run_length < s->huffman_length[s->running_digit]) {
        huffman_put_symbol(&s->huffman[s->running_digit], output, s->run_length);
      } else {
        huffman_put_symbol(&s->huffman[s->running_digit], output, s->huffman_length[s->running_digit]);
        io_buffer_put(output, s->run_length, 24);
      }
  }

  return flush_output_buffer(output);
}

int rle_get(io_buffer * input, rle_state * s)
{
  // 
  // Reverse the encoding performed by rle_bits.
  // 

  unsigned char output_word = 0;
  int output_bits = 8;

  while (1) {
    if (s->run_length == 0) {
      s->running_digit = !s->running_digit;
      int run_length =
	  huffman_get_symbol(&s->huffman[s->running_digit], input);

      if (run_length < 0) {
	return -1;
      } else if(run_length == s->huffman_length[s->running_digit]) {
        s->run_length = io_buffer_get(input, 24);
      } else {
	s->run_length = run_length;
      }
    }

    while (s->run_length > 0) {

      if (output_bits > s->run_length) {
	output_word = output_word << s->run_length;
	if (s->running_digit == 1) {
	  output_word |= ones[s->run_length];
	}
	output_bits -= s->run_length;
	s->run_length = 0;
      } else {
	output_word = output_word << output_bits;
	if (s->running_digit == 1) {
	  output_word |= ones[output_bits];
	}
	s->run_length -= output_bits;

	return output_word;
      }
    }

  }
}


//////////////////////////////////////////////////////////////////////
// 
// Run-length encoding and decoding of times.
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "midas.h"

#include "mucap_compress.h"
#include "mucap_structures.h"

huffman_table bits_huffman[2];
huffman_table time_huffman;
BOOL should_compress_tdc400 = TRUE;

void tdc400_optimize()
{
  huffman_optimize_tree(&time_huffman);
  huffman_optimize_tree(&bits_huffman[0]);
  huffman_optimize_tree(&bits_huffman[1]);
 
  save_huffman("/Compression/Lossless/TDC400/Huffman/Time", &time_huffman);
  save_huffman("/Compression/Lossless/TDC400/Huffman/Bits 0", &bits_huffman[0]);
  save_huffman("/Compression/Lossless/TDC400/Huffman/Bits 1", &bits_huffman[1]);
}

void tdc400_load()
{
  huffman_init_default(&time_huffman, 128);
  huffman_init_default(&bits_huffman[0], 128);
  huffman_init_default(&bits_huffman[1], 128);

  load_huffman("/Compression/Lossless/TDC400/Huffman/Time", &time_huffman);
  load_huffman("/Compression/Lossless/TDC400/Huffman/Bits 0", &bits_huffman[0]);
  load_huffman("/Compression/Lossless/TDC400/Huffman/Bits 1", &bits_huffman[1]);

  int size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/TDC400/Enabled", &should_compress_tdc400,
    &size, TID_BOOL, TRUE);
}

void binary_print(unsigned char c)
{
  for (int i = 0; i < 8; i++) {
    if (c & 0x80) {
      printf("X");
    } else {
      printf(".");
    }

    c = c << 1;
  }
}

void event_display(tdc400_word * input, int length)
{
  for (int i = 0; i < length; i++) {
    printf("%d ", input[i].time);

    for (int j = 0; j < 6; j++) {
      binary_print(tdc400_byte(input[i], j));
      printf(" ");
    }

    printf("\n");
  }

  printf("------------------------------------------------------------\n");
}

int encode_times(tdc400_word * input, int input_size, io_buffer * output)
{
  // 
  // Perform a variant of run-length encoding on the times in the TDC400
  // data, which are the most significant 16 bits in every 64 bit word.
  // 
  // Times typically come in blocks of consecutive words, where the
  // block 
  // length is generally of order 20.  We will store the time of the 
  // start of the block followed by the number of elements in the block.
  // 
  // More precisely, this is what we do:
  // - Take the difference of the first time of the current block
  // from the first time of the previous block.
  // - If the difference is less than 256, store a 0; otherwise, store a 1
  // 
  // - Store either 8 or 16 bits of time difference information.
  // - Store Huffman-coded run length.
  // 

  int run_length = 1;
  unsigned short prev_time = 0;
  unsigned short prev_run_time = 0;

  for (int i = 0; i < input_size; i++) {
    unsigned short this_time = input[i].time;

    if (this_time == prev_time + 1 && i != 0 && run_length < 127) {
      run_length++;
    } else {
      if (i != 0) {
#if 0
	if (run_length >= 8) {
	  event_display(&input[i - run_length], run_length);
	}
#endif
	huffman_put_symbol(&time_huffman, output, run_length);
      }
      run_length = 1;

      unsigned short this_run_time_diff = this_time - prev_run_time;
      if (this_run_time_diff < 256) {
	io_buffer_put(output, this_run_time_diff, 9);
      } else {
	io_buffer_put(output, 0x00010000 | this_run_time_diff, 17);
      }
      output->num_codes++;

      prev_run_time = this_time;
    }

    prev_time = this_time;
  }

  huffman_put_symbol(&time_huffman, output, run_length);

  return flush_output_buffer(output);
}

void decode_times(io_buffer * input, tdc400_word * output)
{
  // 
  // Reverse the encoding performed by encode_times.
  // 

  int num_words_in = input->num_codes / 2;
  int output_size = 0;
  unsigned short prev_run_time = 0;

  for (int i = 0; i < num_words_in; i++) {
    int first_time_length_flag = io_buffer_get(input, 1);
    unsigned short first_time;
    if (first_time_length_flag) {
      first_time = io_buffer_get(input, 16);
    } else {
      first_time = io_buffer_get(input, 8);
    }

    int run_length = huffman_get_symbol(&time_huffman, input);

    for (int j = 0; j < run_length; j++) {
      output[output_size++].time = first_time + prev_run_time + j;
    }

    prev_run_time += first_time;
  }
}

int encode_bits(tdc400_word * input, int input_size, io_buffer * output)
{
  // 
  // Perform bitwise run-length encoding on the bit mask portion of 
  // the TDC400 data.  For example, the following bit pattern:
  // 
  // t 000000000011100000000000000000000000000000000000 
  // t+1 000000000000110000000000000000000000000000000000
  // 
  // which is not altogether improbable, would be encoded as
  // 
  // 10, 3, 47, 2, 34
  // 
  // Then, store Huffman codes for the run lengths.  Use a separate
  // Huffman table for the "zero" runs and the "one" runs.
  // 

  rle_state s = { 0, 0, {128, 128}, 0, bits_huffman };

  for (int i = 0; i < input_size; i++) {
    for (int j = 0; j < 6; j++) {
      unsigned char b = tdc400_byte(input[i], j);
      rle_put(b, &s, output);
    }
  }

  return flush_rle(&s, output);
}

void decode_bits(io_buffer * input, tdc400_word * output, int output_size)
{
  rle_state s = { 1, 0, {128, 128}, 0, bits_huffman };

  for (int i = 0; i < output_size; i++) {
    for (int j = 0; j < 6; j++) {
      int b = rle_get(input, &s);
      if (b >= 0) {
	tdc400_byte(output[i], j) = (unsigned char) b;
      } else {
	return;
      }
    }
  }
}

int tdc400_compress(tdc400_word * input, int input_size,
		    unsigned char *output, int userParam)
{
  // 
  // Carry out lossless compression on the TPC data recorded by a
  // single TDC400.
  // 

  input_size /= sizeof(tdc400_word);

  rle_bits_setup();

  io_buffer output_buffer;

  // store size of input data
  int *uncompressed_size_p = (int *) output;
  *uncompressed_size_p = input_size;
  output += sizeof(int);

  // reserve space for the size of the compressed times
  int *compressed_time_size_p = (int *) output;
  output += sizeof(int);

  // compress times
  start_output_buffer(&output_buffer, output);
  int compressed_time_size =
      encode_times(input, input_size, &output_buffer);
  output += compressed_time_size;

  // store size of compressed times
  *compressed_time_size_p = compressed_time_size;

  // reserve space for the size of the compressed bit patterns
  int *compressed_bits_size_p = (int *) output;
  output += sizeof(int);

  // compress bit patterns
  start_output_buffer(&output_buffer, output);
  int compressed_bits_size =
      encode_bits(input, input_size, &output_buffer);
  output += compressed_time_size;

  // store size of compressed bit patterns 
  *compressed_bits_size_p = compressed_bits_size;

  return compressed_time_size + compressed_bits_size + 3 * sizeof(int);
}

int tdc400_expand(unsigned char *input, int input_size,
		  tdc400_word * output, int userParam)
{
  // 
  // Uncompress the data produced by tdc400_compress().
  // 

  rle_bits_setup();

  // Get uncompressed data size
  int *uncompressed_size_p = (int *) input;
  int uncompressed_size = *uncompressed_size_p;
  input += sizeof(int);
  input_size -= sizeof(int);

  // Get size of compressed time data
  int *compressed_time_size_p = (int *) input;
  int compressed_time_size = *compressed_time_size_p;
  input += sizeof(int);
  input_size -= sizeof(int);

  // Uncompress times
  io_buffer input_buffer;
  start_input_buffer(&input_buffer, input);
  decode_times(&input_buffer, output);
  input += compressed_time_size;
  input_size -= compressed_time_size;

  // Get size of compressed bit patterns
  int *compressed_bits_size_p = (int *) input;
  int compressed_bits_size = *compressed_bits_size_p;
  input += sizeof(int);
  input_size -= sizeof(int);

  // Uncompress bit patterns
  start_input_buffer(&input_buffer, input);
  decode_bits(&input_buffer, output, uncompressed_size);

  return uncompressed_size * sizeof(tdc400_word);
}


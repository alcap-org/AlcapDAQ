//////////////////////////////////////////////////////////////////////
// 
// Compressor compression :-)
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "midas.h"

#include "mucap_compress.h"
#include "mucap_structures.h"

huffman_table cmp_bits_huffman[2];
huffman_table cmp_channel_huffman;
BOOL should_compress_comp = TRUE;

void comp_optimize()
{
  huffman_optimize_tree(&cmp_channel_huffman);
  huffman_optimize_tree(&cmp_bits_huffman[0]);
  huffman_optimize_tree(&cmp_bits_huffman[1]);

  save_huffman("/Compression/Lossless/COMP/Huffman/Channel", &cmp_channel_huffman);
  save_huffman("/Compression/Lossless/COMP/Huffman/Bits 0", &cmp_bits_huffman[0]);
  save_huffman("/Compression/Lossless/COMP/Huffman/Bits 1", &cmp_bits_huffman[1]);
}

void comp_load()
{
  huffman_init_default(&cmp_channel_huffman, 256);
  huffman_init_default(&cmp_bits_huffman[0], 128);
  huffman_init_default(&cmp_bits_huffman[1], 128);

  load_huffman("/Compression/Lossless/COMP/Huffman/Channel", &cmp_channel_huffman);
  load_huffman("/Compression/Lossless/COMP/Huffman/Bits 0", &cmp_bits_huffman[0]);
  load_huffman("/Compression/Lossless/COMP/Huffman/Bits 1", &cmp_bits_huffman[1]);

  int size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/COMP/Enabled", &should_compress_comp,
    &size, TID_BOOL, TRUE);
}

static int sign_extend(int in, int bits)
{
  int sign_bit = (1 << (bits-1));
  if(in & sign_bit) {
    return in | ~(sign_bit - 1);
  } else {
    return in;
  }
}

int encode_cmp_bits(comp_word * input, int input_size, io_buffer * output)
{
  // 
  // Run-length encode the bits in the compressor data.  See the comment
  // at the beginning of encode_bits for an explanation of this process.
  // 

  int num_bit_words = 0;
  rle_state s = { 0, 0, {128, 128}, 0, cmp_bits_huffman };

  for (int i = 0; i < input_size; i++) {
    // This word may be a header/time word:
    if (num_bit_words == 0) {
      num_bit_words = input[i].r1.a + input[i].r1.b;
    } else {
      for (int j = 0; j < 4; j++) {

	unsigned char b = input[i].bytes[j];
	rle_put(b, &s, output);
      }
      num_bit_words--;
    }
  }

  return flush_rle(&s, output);
}

int encode_cmp_times(comp_word * input, int input_size, io_buffer * output)
{
  // 
  // Encode the FPGA numbers and times in the compressor data.  
  // 
  // 1. The B and A bits, which indicate the meaning of the
  // following words, are preserved.
  // 2. The FPGA numbers are Huffman coded, since some values are
  // probably more likely than others.
  // 3. The times are encoded as successive differences, which are
  // then stored on a variable scale.
  //

  int num_bit_words = 0;
  int prev_time = 0;

  for (int i = 0; i < input_size; i++) {
    // This word may be a header/time word:
    if (num_bit_words == 0) {
      num_bit_words = input[i].r1.a + input[i].r1.b;

      if (num_bit_words != 1 && num_bit_words != 2) {
        printf("Warning: corrupted compressor data.\n");
      }

      int channel = input[i].r1.fpga | input[i].r1.a << 6 | input[i].r1.b << 7;
      huffman_put_symbol(&cmp_channel_huffman, output, channel);

      int time_diff = input[i].r1.time - prev_time;

      if (time_diff < 8 && time_diff >= -8) {
	io_buffer_put(output, 0, 1);
	io_buffer_put(output, time_diff & 0xf, 4);
      } else if (time_diff < 512 && time_diff >= -512) {
	io_buffer_put(output, 2, 2);
	io_buffer_put(output, time_diff & 0x3ff, 10);
      } else {
	io_buffer_put(output, 3, 2);
	io_buffer_put(output, time_diff & 0xffffff, 24);
      }

      prev_time = input[i].r1.time;
    }

    else {
      num_bit_words--;
    }
  }

  return flush_output_buffer(output);
}

int cmp_compress(comp_word * input, int input_size, unsigned char *output, int userParam)
{
  // 
  // Encode compressor data, which are in the following format:
  // | B | A | FPGA number (6 bits) | Time (24 bits) |
  // ... followed by one or two 32-bit masks indicating which
  // channels
  // within the group have fired.  
  // 

  input_size /= sizeof(comp_word);

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
      encode_cmp_times(input, input_size, &output_buffer);
  output += compressed_time_size;

  // store size of compressed times
  *compressed_time_size_p = compressed_time_size;

  // reserve space for the size of the compressed bit patterns
  int *compressed_bits_size_p = (int *) output;
  output += sizeof(int);

  // compress bit patterns
  start_output_buffer(&output_buffer, output);
  int compressed_bits_size =
      encode_cmp_bits(input, input_size, &output_buffer);
  output += compressed_time_size;

  // store size of compressed bit patterns 
  *compressed_bits_size_p = compressed_bits_size;

  return compressed_time_size + compressed_bits_size + 3 * sizeof(int);
}

void decode_cmp_times(io_buffer * input, comp_word * output)
{
  // 
  // Reverse the encoding performed by encode_times.
  // 

  int num_words_in = input->num_codes;
  int output_size = 0;
  int prev_time = 0;

  for (int i = 0; i < num_words_in; i++) {

    int channel = huffman_get_symbol(&cmp_channel_huffman, input);
    output[output_size].r1.fpga = channel & 0x3f;
    output[output_size].r1.a = (channel >> 6) & 1;
    output[output_size].r1.b = (channel >> 7) & 1;

    int time_length_flag1 = io_buffer_get(input, 1);
    int time_diff;
    if (time_length_flag1 == 0) {
      time_diff = io_buffer_get(input, 4);
      time_diff = sign_extend(time_diff, 4);
    } else {
      int time_length_flag2 = io_buffer_get(input, 1);
      if (time_length_flag2 == 0) {
	time_diff = io_buffer_get(input, 10);
        time_diff = sign_extend(time_diff, 10);
      } else {
	time_diff = io_buffer_get(input, 24);
        time_diff = sign_extend(time_diff, 24);
      }
    }

    output[output_size].r1.time = (prev_time + time_diff) & 0xffffff;
    prev_time = output[output_size].r1.time;

    int num_bit_words =
	output[output_size].r1.a + output[output_size].r1.b;
    output_size += 1 + num_bit_words;
  }
}

void decode_cmp_bits(io_buffer * input, comp_word * output, int output_size)
{
  int num_bit_words = 0;

  rle_state s = { 1, 0, {128, 128}, 0, cmp_bits_huffman };

  for (int i = 0; i < output_size; i++) {
    if (num_bit_words == 0) {
      num_bit_words = output[i].r1.a + output[i].r1.b;
    } else {
      for (int j = 0; j < 4; j++) {
	output[i].bytes[j] = rle_get(input, &s);
      }
      num_bit_words--;
    }
  }
}

int cmp_expand(unsigned char *input, int input_size, comp_word * output, int userParam)
{
  // 
  // Uncompress the data produced by cmp_compress().
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
  decode_cmp_times(&input_buffer, output);
  input += compressed_time_size;
  input_size -= compressed_time_size;

  // Get size of compressed bit patterns
  int *compressed_bits_size_p = (int *) input;
  int compressed_bits_size = *compressed_bits_size_p;
  input += sizeof(int);
  input_size -= sizeof(int);

  // Uncompress bit patterns
  start_input_buffer(&input_buffer, input);
  decode_cmp_bits(&input_buffer, output, uncompressed_size);

  return uncompressed_size * sizeof(comp_word);
}

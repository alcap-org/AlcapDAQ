//////////////////////////////////////////////////////////////////////
// 
// CAEN TDC compression
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "midas.h"

#include "mucap_compress.h"
#include "mucap_structures.h"

huffman_table caen_channel_huffman;
BOOL should_compress_caen = TRUE;
BOOL should_strip_trailing_edge = TRUE;
BOOL should_strip_r1 = TRUE;

void caen_optimize()
{
  huffman_optimize_tree(&caen_channel_huffman);
  save_huffman("/Compression/Lossless/CAEN/Huffman/Channel", &caen_channel_huffman);
}

void caen_load()
{
  huffman_init_default(&caen_channel_huffman, 129);
  load_huffman("/Compression/Lossless/CAEN/Huffman/Channel", &caen_channel_huffman);

  int size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/CAEN/Enabled", &should_compress_caen, 
    &size, TID_BOOL, TRUE);

  size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/CAEN/Strip trailing edge", 
    &should_strip_trailing_edge, &size, TID_BOOL, TRUE);

  size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/CAEN/Strip R1",
    &should_strip_r1, &size, TID_BOOL, TRUE);
}

int encode_caen(caen_word * input, int input_size, io_buffer * output, 
  int *uncompressed_size_p)
{
  // Encode CAEN TDC data, which are in the following format:
  // 0 | channel (7 bits) | Start (1 bit) | 00 | Edge (1 bit) | time (20 bits)
  // 
  // 1. Optionally, trailing edges are discarded.
  // 2. An extended channel number, formed from the channel, start, 
  // and edge bits, is Huffman coded.
  // 3. Successive differences between times are constructed and encoded
  // with a variable number of bits.

  int strip_trailing_edge = should_strip_trailing_edge;
  int strip_r1 = should_strip_r1;
  int prev_time = 0;
  int uncompressed_size = 0;

  io_buffer_put(output, strip_trailing_edge, 1);
  io_buffer_put(output, strip_r1, 1);

  for (int i = 0; i < input_size; i++) {

    int channel = input[i].channel;
    if (input[i].r2 != 0 || input[i].start) {
#if 0
      printf("in datum %d : r1 %d channel %d start %d r2 %d edge %d time %d\n",
        i, input[i].r1, input[i].channel, input[i].start, input[i].r2,
        input[i].edge, input[i].time);
#endif
      channel = 128;
    } else if (strip_trailing_edge && input[i].edge == 0) {
      continue;
    }

    huffman_put_symbol(&caen_channel_huffman, output, channel);

    if(channel == 128) {
      int verbatim = *((int *) &input[i]);
      io_buffer_put(output, (verbatim >> 16) & 0xffff, 16);
      io_buffer_put(output, verbatim & 0xffff, 16);
    } else {

      if(!strip_r1) {
        io_buffer_put(output, input[i].r1, 1);
      }

      if (!strip_trailing_edge) {
        io_buffer_put(output, input[i].edge, 1);
      }

      int time_diff = input[i].time - prev_time;

      if (time_diff < 32 && time_diff >= -32) {
        io_buffer_put(output, 0, 1);
        io_buffer_put(output, time_diff & 0x3f, 6);
      } else if (time_diff < 512 && time_diff >= -512) {
        io_buffer_put(output, 2, 2);
        io_buffer_put(output, time_diff & 0x3ff, 10);
      } else {
        io_buffer_put(output, 3, 2);
        io_buffer_put(output, time_diff & 0xfffff, 20);
      }

      prev_time = input[i].time;
    }

    uncompressed_size++;
  }

  *uncompressed_size_p = uncompressed_size;

  return flush_output_buffer(output);
}

int caen_compress(caen_word * input, int input_size, unsigned char *output, int userParam)
{
  input_size /= sizeof(caen_word);

  rle_bits_setup();

  io_buffer output_buffer;

  // reserve space for size of uncompressed data (may be different from
  // input size, since some hits may be stripped)
  int *uncompressed_size_p = (int *) output;
  output += sizeof(int);

  // reserve space for the size of the compressed data
  int *compressed_time_size_p = (int *) output;
  output += sizeof(int);

  // compress times
  start_output_buffer(&output_buffer, output);
  int compressed_time_size =
      encode_caen(input, input_size, &output_buffer, uncompressed_size_p);
  output += compressed_time_size;

  // store size of compressed times
  *compressed_time_size_p = compressed_time_size;

  return compressed_time_size + 2 * sizeof(int);
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

void decode_caen(io_buffer * input, caen_word * output)
{
  int strip_trailing_edge = io_buffer_get(input, 1);
  int strip_r1 = io_buffer_get(input, 1);

  int prev_time = 0;

  int i = 0;
  while (1) {

    int channel = huffman_get_symbol(&caen_channel_huffman, input);

    if (channel < 0) {
      break;
    }

    if (channel == 128) {
      int v1 = io_buffer_get(input, 16);
      int v2 = io_buffer_get(input, 16);

      int verbatim = v2 | (v1 << 16);
      output[i] = *((caen_word *) &verbatim);
    } else {
      output[i].start = 0;
      output[i].channel = channel;

      if(!strip_r1) {
        output[i].r1 = io_buffer_get(input, 1);
      } else {
        output[i].r1 = 0;
      }
      output[i].r2 = 0;

      if (!strip_trailing_edge) {
        output[i].edge = io_buffer_get(input, 1);
      } else {
        output[i].edge = 1;
      }

      int time_length_flag1 = io_buffer_get(input, 1);
      int time_diff;
      if (time_length_flag1 == 0) {
        time_diff = io_buffer_get(input, 6);
        time_diff = sign_extend(time_diff, 6);
      } else {
        int time_length_flag2 = io_buffer_get(input, 1);
        if (time_length_flag2 == 0) {
  	  time_diff = io_buffer_get(input, 10);
          time_diff = sign_extend(time_diff, 10);
        } else {
  	  time_diff = io_buffer_get(input, 20);
          time_diff = sign_extend(time_diff, 20);
        }
      }

      output[i].time = (prev_time + time_diff) & 0xfffff;
      prev_time = output[i].time;
    }

    i++;
  }
}

int caen_expand(unsigned char *input, int input_size, caen_word * output, int userParam)
{
  // 
  // Uncompress the data produced by caen_compress().
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
  decode_caen(&input_buffer, output);
  input += compressed_time_size;
  input_size -= compressed_time_size;

  return uncompressed_size * sizeof(caen_word);
}

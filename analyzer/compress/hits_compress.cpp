//////////////////////////////////////////////////////////////////////
// 
// HITS bank compression (for use with "skimmed" data)
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <assert.h>

#include "midas.h"

#include "mucap_compress.h"
#include "mucap_structures.h"

huffman_table hits_parameter_huffman;
BOOL should_compress_hits = TRUE;

void hits_optimize()
{
  huffman_optimize_tree(&hits_parameter_huffman);
  save_huffman("/Compression/Lossless/HITS/Huffman/Parameter", &hits_parameter_huffman);
}

void hits_load()
{
  huffman_init_default(&hits_parameter_huffman, 322);
  load_huffman("/Compression/Lossless/HITS/Huffman/Parameter", &hits_parameter_huffman);

  int size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/HITS/Enabled", &should_compress_hits, 
    &size, TID_BOOL, TRUE);
}

int encode_hits(channel_hit * input, int input_size, io_buffer * output, 
  int *uncompressed_size_p)
{
  int prev_time = 0;
  int prev_parameter = 0;

  for (int i = 0; i < input_size; i++) {
    //printf("par %d time %f\n", input[i].parameter, input[i].time);

    int parameter = input[i].parameter;
    int parameter_diff = parameter - prev_parameter + 161;
    if(parameter_diff <= 0 || parameter_diff >= 322) {
      parameter_diff = 0;
    }
    huffman_put_symbol(&hits_parameter_huffman, output, parameter_diff);
    if(parameter_diff == 0) {
      io_buffer_put(output, parameter, 13);      
    }

    int time = (int) ((input[i].time+0.01)/1.25);
    int time_diff = time - prev_time;
    assert (time_diff >= 0);

    if (time_diff < 64) {
        io_buffer_put(output, 0, 1);
        io_buffer_put(output, time_diff & 0x3f, 6);
    } else if (time_diff < 4096) {
        io_buffer_put(output, 2, 2);
        io_buffer_put(output, time_diff & 0xfff, 12);
    } else {
        io_buffer_put(output, 3, 2);
        io_buffer_put(output, time_diff, 29);
    }

    prev_time = time;
    prev_parameter = parameter;
  }

  *uncompressed_size_p = input_size;

  return flush_output_buffer(output);
}

int hits_compress(channel_hit * input, int input_size, unsigned char *output, int userParam)
{
  input_size /= sizeof(channel_hit);

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
      encode_hits(input, input_size, &output_buffer, uncompressed_size_p);
  output += compressed_time_size;

  // store size of compressed times
  *compressed_time_size_p = compressed_time_size;

  return compressed_time_size + 2 * sizeof(int);
}

void decode_hits(io_buffer * input, channel_hit * output)
{
  int prev_time = 0;
  int prev_parameter = 0;

  int i = 0;
  while (1) {

    int parameter = 0;
    int parameter_diff = huffman_get_symbol(&hits_parameter_huffman, input);
    if (parameter_diff < 0) {
      break;
    } else if(parameter_diff == 0) {
      parameter = io_buffer_get(input, 13);
    } else {
      parameter = prev_parameter + parameter_diff - 161;
    }
    output[i].parameter = parameter;

    int time_length_flag1 = io_buffer_get(input, 1);
    int time_diff;
    if (time_length_flag1 == 0) {
        time_diff = io_buffer_get(input, 6);
    } else {
        int time_length_flag2 = io_buffer_get(input, 1);
        if (time_length_flag2 == 0) {
  	  time_diff = io_buffer_get(input, 12);
        } else {
  	  time_diff = io_buffer_get(input, 29);
        }
    }

    output[i].time = (prev_time + time_diff) * 1.25;
    prev_time = prev_time + time_diff;
    prev_parameter = parameter;

    //printf("par %d time %f\n", output[i].parameter, output[i].time);

    i++;
  }
}

int hits_expand(unsigned char *input, int input_size, channel_hit * output, int userParam)
{
  // 
  // Uncompress the data produced by hits_compress().
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
  decode_hits(&input_buffer, output);
  input += compressed_time_size;
  input_size -= compressed_time_size;

  return uncompressed_size * sizeof(channel_hit);
}

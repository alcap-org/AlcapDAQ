//////////////////////////////////////////////////////////////////////
// 
// Flash ADC compression
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "midas.h"

#include "mucap_compress.h"
#include "mucap_structures.h"

huffman_table fadc_run_huffman;
huffman_table fadc_small_huffman;
BOOL should_compress_fadc = TRUE;

const int num_fadc_channels = 4;

void fadc_optimize()
{
  huffman_optimize_tree(&fadc_run_huffman);
  huffman_optimize_tree(&fadc_small_huffman);

  save_huffman("/Compression/Lossless/FADC/Huffman/Run", &fadc_run_huffman);
  save_huffman("/Compression/Lossless/FADC/Huffman/Small", &fadc_small_huffman);
}

void fadc_load()
{
  huffman_init_default(&fadc_run_huffman, 32);
  huffman_init_default(&fadc_small_huffman, 9);

  load_huffman("/Compression/Lossless/FADC/Huffman/Run", &fadc_run_huffman);
  load_huffman("/Compression/Lossless/FADC/Huffman/Small", &fadc_small_huffman);

  int size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/FADC/Enabled", &should_compress_fadc,
    &size, TID_BOOL, TRUE);
}

int encode_fadc(fadc_word * input, int input_size, io_buffer * output)
{
  // Encode the flash ADC data by 
  // 1. Splitting the four channels.
  // 2. Taking successive differences between samples within a channel.
  // 3. Run-length encoding the differences.
  // 4. Huffman coding the results.

  int num_fadc_samples = input_size;

  int diffs[ /*num_fadc_samples */ 1024];

  for (int chan = 0; chan < num_fadc_channels; chan++) {
    int prev_sample = 0;

    for (int i = 0; i < num_fadc_samples; i++) {
      diffs[i] = ((int) input[i].channel[chan]) - prev_sample;
      prev_sample = input[i].channel[chan];
    }

    for (int i = 0; i < num_fadc_samples; i++) {

      if (diffs[i] == 0) {
	// count the run length
	int run_length = 0;
	while (i + run_length < num_fadc_samples && run_length < 31) {
	  if (diffs[i + run_length] == 0) {
	    run_length++;
	  } else {
	    break;
	  }
	}

	i += run_length - 1;

	io_buffer_put(output, 0, 1);
	huffman_put_symbol(&fadc_run_huffman, output, run_length);
      } else if (diffs[i] <= 4 && diffs[i] >= -4) {
	io_buffer_put(output, 2, 2);
	huffman_put_symbol(&fadc_small_huffman, output, diffs[i] + 4);
      } else {
	io_buffer_put(output, 3, 2);
	io_buffer_put(output, diffs[i] + 256, 9);
      }
    }
  }

  return flush_output_buffer(output);
}

int fadc_compress(fadc_word * input, int input_size, unsigned char *output, int userParam)
{
  input_size /= sizeof(fadc_word);

  rle_bits_setup();

  io_buffer output_buffer;

  // store size of input data
  int *uncompressed_size_p = (int *) output;
  *uncompressed_size_p = input_size;
  output += sizeof(int);

  // reserve space for the size of the compressed data
  int *compressed_time_size_p = (int *) output;
  output += sizeof(int);

  // compress samples
  start_output_buffer(&output_buffer, output);
  int compressed_size = encode_fadc(input, input_size, &output_buffer);
  output += compressed_size;

  // store size of compressed times
  *compressed_time_size_p = compressed_size;

  return compressed_size + 2 * sizeof(int);
}

int decode_fadc(io_buffer * input, fadc_word * output, int input_size)
{
  int num_fadc_samples = input_size;

  int diffs[ /*num_fadc_samples */ 1024];

  for (int chan = 0; chan < num_fadc_channels; chan++) {

    for (int i = 0; i < num_fadc_samples; i++) {
      int type_code_1 = io_buffer_get(input, 1);
      if (type_code_1 == 0) {
	int run_length = huffman_get_symbol(&fadc_run_huffman, input);

	for (int j = 0; j < run_length; j++) {
	  diffs[i + j] = 0;
	}

	i += run_length - 1;

	if (i < 0 || i >= num_fadc_samples) {
	  printf("WTF?\n");
	}


      } else {
	int type_code_2 = io_buffer_get(input, 1);
	if (type_code_2 == 0) {
	  diffs[i] = huffman_get_symbol(&fadc_small_huffman, input) - 4;
	} else {
	  diffs[i] = io_buffer_get(input, 9) - 256;
	}
      }
    }

    int prev_sample = 0;

    for (int i = 0; i < num_fadc_samples; i++) {
      output[i].channel[chan] = prev_sample + diffs[i];
      prev_sample = output[i].channel[chan];
    }
  }

  return num_fadc_samples;
}

int fadc_expand(unsigned char *input, int input_size, fadc_word * output, int userParam)
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

  // Get size of compressed data
  int *compressed_size_p = (int *) input;
  int compressed_size = *compressed_size_p;
  input += sizeof(int);
  input_size -= sizeof(int);

  // Uncompress data
  io_buffer input_buffer;
  start_input_buffer(&input_buffer, input);
  decode_fadc(&input_buffer, output, uncompressed_size);
  input += compressed_size;
  input_size -= compressed_size;

  return uncompressed_size * sizeof(fadc_word);
}

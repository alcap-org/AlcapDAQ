//////////////////////////////////////////////////////////////////////
// 
// Compression of "stacked" TPC data.
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "midas.h"

#include "mucap_compress.h"
#include "mucap_structures.h"

void myprint_TPCstack_interval(stack_elt *stackArray, long index1, long index2);
void print_TPCstack_interval(stack_elt *stackArray, long index1, long index2);

huffman_table stck_bits_huffman[2];
huffman_table stck_time_huffman;
BOOL should_compress_stck = TRUE;

void stck_optimize()
{
  huffman_optimize_tree(&stck_time_huffman);
  huffman_optimize_tree(&stck_bits_huffman[0]);
  huffman_optimize_tree(&stck_bits_huffman[1]);
 
  save_huffman("/Compression/Lossless/STCK/Huffman/Bits 0", &stck_bits_huffman[0]);
  save_huffman("/Compression/Lossless/STCK/Huffman/Bits 1", &stck_bits_huffman[1]);
}

void stck_load()
{
  huffman_init_default(&stck_time_huffman, 129);
  huffman_init_default(&stck_bits_huffman[0], 129);
  huffman_init_default(&stck_bits_huffman[1], 129);

  load_huffman("/Compression/Lossless/STCK/Huffman/Bits 0", &stck_bits_huffman[0]);
  load_huffman("/Compression/Lossless/STCK/Huffman/Bits 1", &stck_bits_huffman[1]);

  int size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/STCK/Enabled", &should_compress_stck,
    &size, TID_BOOL, TRUE);
}

int encode_times(stack_elt * input, int input_size, io_buffer * output)
{
  // 
  // Perform a variant of run-length encoding on the times in the STCK
  // data, which are the most significant 16 bits in every 64 bit word.
  // 
  // Times come in sequences of consecutive words, with sequences separated 
  // by a "-1" time; generally there are a whole lot of words, many of which 
  // are entirely zero padding.  We store the time  of the start of the block 
  // followed by the number of elements in the block.
  // 

  int run_length = 0;
  int prev_time = 0;
  unsigned short prev_run_time = 0;

  for (int i = 0; i < input_size; i++) {
    if (input[i].time != -1.0) {
      if(run_length == 0) {
        int this_time = (int) ((input[i].time+0.1)/200);
	int this_run_time_diff = this_time - prev_run_time;
        if (this_run_time_diff < 2048) {
  	  io_buffer_put(output, this_run_time_diff, 12);
        } else {
	  io_buffer_put(output, (1 << 22) | this_run_time_diff, 23);
        }
        output->num_codes++;

        prev_run_time = this_time;
      }
      run_length++;
    } else {
      if (run_length != 0) {
        if(run_length < 256) {
          io_buffer_put(output, run_length, 9);
	} else {
          io_buffer_put(output, (1 << 19) | run_length, 20);
	}
        run_length = 0;
      }
    }
  }

  if (run_length != 0) {
    if(run_length < 256) {
      io_buffer_put(output, run_length, 9);
    } else {
      io_buffer_put(output, (1 << 19) | run_length, 20);
    }
    run_length = 0;
  }

  return flush_output_buffer(output);
}

void decode_times(io_buffer * input, stack_elt * output)
{
  // 
  // Reverse the encoding performed by encode_times.
  // 

  int num_words_in = input->num_codes;
  int output_size = 0;
  unsigned short prev_run_time = 0;

  for (int i = 0; i < num_words_in; i++) {
    int first_time_length_flag = io_buffer_get(input, 1);
    int first_time;
    if (first_time_length_flag) {
      first_time = io_buffer_get(input, 22);
    } else {
      first_time = io_buffer_get(input, 11);
    }

    int run_length_flag = io_buffer_get(input, 1);
    int run_length;
    if (run_length_flag) {
      run_length = io_buffer_get(input, 19);
    } else {
      run_length = io_buffer_get(input, 8);
    }

    output[output_size++].time = -1;
    for (int j = 0; j < run_length; j++) {
      output[output_size++].time = (first_time + prev_run_time + j)*200;
    }

    prev_run_time += first_time;
  }
}

int encode_bits(stack_elt * input, int input_size, io_buffer * output)
{
  // 
  // the STCK data.  For example, the following bit pattern:
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

  rle_state s = { 0, 0, {128, 128}, 1, stck_bits_huffman };

  for(int i = 0; i < 3; i++) {
    for (int j = 0; j < input_size; j++) {
      for (int k = 0; k < 6; k++) {
        unsigned char b = input[j].thr[i].Aword[k] & 0xff;
        rle_put(b, &s, output);
        b = (input[j].thr[i].Aword[k] >> 8) & 0xff;
        rle_put(b, &s, output);
      }
    }

    for (int j = 0; j < input_size; j++) {
      for (int k = 0; k < 3; k++) {
        unsigned char b = input[j].thr[i].Sword[k] & 0xff;
        rle_put(b, &s, output);
        b = (input[j].thr[i].Sword[k] >> 8) & 0xff;
        rle_put(b, &s, output);
      }
    }
  }

  return flush_rle(&s, output);
}

void decode_bits(io_buffer * input, stack_elt * output, int output_size)
{
  rle_state s = { 1, 0, {128, 128}, 1, stck_bits_huffman };

  for(int i = 0; i < 3; i++) {
    for (int j = 0; j < output_size; j++) {
      for (int k = 0; k < 6; k++) {
        int b1 = rle_get(input, &s);
        int b2 = rle_get(input, &s);
	if(b1 >= 0 && b2 >= 0) {
          output[j].thr[i].Aword[k] = (b2 << 8) | b1;
        } else {
          return;
	}
      }
    }

    for (int j = 0; j < output_size; j++) {
      for (int k = 0; k < 3; k++) {
        int b1 = rle_get(input, &s);
        int b2 = rle_get(input, &s);
	if(b1 >= 0 && b2 >= 0) {
          output[j].thr[i].Sword[k] = (b2 << 8) | b1;
        } else {
          return;
	}
      }
    }

    for (int j = 0; j < output_size; j++) {
      bool bit = 0;
      for(int k = 0; k < 6; k++) {
        bit = bit || (output[j].thr[i].Aword[k] != 0);
      }
      for(int k = 0; k < 3; k++) {
        bit = bit || (output[j].thr[i].Sword[k] != 0);
      }
      output[j].thrbits[i] = bit;
    }
  }
}

int stck_compress(stack_elt * input, int input_size,
		    unsigned char *output, int userParam)
{
  // 
  // Carry out lossless compression on the stacked TPC data.
  // 

  input_size /= sizeof(stack_elt);

  //myprint_TPCstack_interval(input, 0, input_size-1);
 
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

int stck_expand(unsigned char *input, int input_size,
		  stack_elt * output, int userParam)
{
  // 
  // 
  // Uncompress the data produced by stck_compress().
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

  //myprint_TPCstack_interval(output, 0, uncompressed_size-1);

  return uncompressed_size * sizeof(stack_elt);
}

void myprint_TPCstack_interval(stack_elt *stackArray,
			     long index1, 
			     long index2)
{
  for(int i = index1; i <= index2; i++) {
#if 0
    printf("%f: \n", stackArray[i].time);
    for(int j = 0; j < 3; j++) {
      printf(" | %d | ", stackArray[i].thrbits[j]);
      for(int k = 0; k < 6; k++) {
        printf("0x%08x ", stackArray[i].thr[j].Aword[k]);
      }
      printf("* ");
      for(int k = 0; k < 3; k++) {
        printf("0x%08x ", stackArray[i].thr[j].Sword[k]);
      }
      printf("\n");
    }
#endif

    printf("%f: \n", stackArray[i].time);
    for(int j = 0; j < sizeof(stack_elt); j++) {
      printf("%d ", ((char *) &stackArray[i])[j]);
    }
    printf("\n");
  }
}

void print_TPCstack_interval(stack_elt *stackArray,
			     long index1, 
			     long index2)
{
  // Everything OK, so print requested section of STCK bank
  for (int index=index1; index<=index2; index++) {
    if (stackArray[index].time > 0) {
      // Anodes
      for (int section=1; section<=5; section++) {
	for (int anode=0; anode<16; anode++) {
          if (stackArray[index].thr[2].Aword[section-1] & (1 << anode)) {
	    printf("*");
	  } else {
	    if (stackArray[index].thr[1].Aword[section-1] & (1 << anode)) {
	      printf("#");
	    } else {
	      if (stackArray[index].thr[0].Aword[section-1] & (1 << anode)) {
		printf("."); 
	      } else {
		printf("0");
	      }
	    }
	  }
	}
      } // End loop over anodes

      // Strips
      printf(" ");
      for (int section=1; section<=3; section++) {
	int maxstrip;
	if (section == 3) {
	  maxstrip=3;
	} else {
	  maxstrip=16;
	}
	for (int strip=0; strip<maxstrip; strip++) {
	  if (stackArray[index].thr[2].Sword[section-1] & (1 << strip)) {
	    printf("*");
	  } else {
	    if (stackArray[index].thr[1].Sword[section-1] & (1 << strip)) {
	      printf("#");
	    } else {
	      if (stackArray[index].thr[0].Sword[section-1] & (1 << strip)) {
		printf(".");
	      } else {
		printf("0");
	      }
	    }
	  }
	}
      } // End loop over strips

      printf(" [%d].time = %0.0f ns\n",index,stackArray[index].time);
    } else {
      if (index != index2) {
	printf("         |10       |20       |30       |40       |50       ");
	printf("|60       |70   |76\n");
      }
      printf("----------------------------------------------------------");
      printf("----------------------------------------------------------");
      printf(" [%d].time = %0.0f\n", index, stackArray[index].time);
    }
  }
}

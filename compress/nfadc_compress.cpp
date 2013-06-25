//////////////////////////////////////////////////////////////////////
// 
// New Louvain/Berkeley Flash ADC compression
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "midas.h"

#include "mucap_compress.h"
#include "mucap_structures.h"

BOOL should_compress_nfadc = TRUE;

const int num_nfadc_channels = 4;

void nfadc_dump(int num_fadc_words, unsigned char *data);

void nfadc_optimize()
{
}

void nfadc_load()
{
  int size = sizeof(BOOL);
  db_get_value(hDB, 0, "/Compression/Lossless/NFADC/Enabled", &should_compress_nfadc,
    &size, TID_BOOL, TRUE);
}

int encode_nfadc(unsigned char * input, int input_size, io_buffer * output, int boardNumber)
{
  int num_words = input_size/10;

  // first deal with the timestamps
  int run_length = 0;
  int last_timestamp = -2;
  int last_adc = -2;
  int island_adc = 0;
  int island_timestamp = -1;
  int last_island_timestamp = 0;
  for(int j = 0; j < num_words; j++) {

    int adc = (input[j*10+0] >> 7) & 0x01;

    int timestamp =  ((input[j*10+0] & 0x7f) << 20) |
                      (input[j*10+1] << 12) |
                      (input[j*10+2] << 4) |
                      (input[j*10+3] >> 4);
    if(boardNumber < 128) {
      timestamp /= 2;
    }

    if(timestamp != last_timestamp + 1 || adc != last_adc || run_length == 255) {
      if(island_timestamp != -1) {
        io_buffer_put(output, island_adc, 1); 
        io_buffer_put(output, island_timestamp, 27); 
        io_buffer_put(output, run_length, 8);
      }

      island_adc = adc;
      island_timestamp = timestamp;
      run_length = 0;
    } 

    last_adc = adc;
    last_timestamp = timestamp;
    run_length++;
  }

  // final run
  if(island_timestamp != -1) {
    io_buffer_put(output, island_adc, 1); 
    io_buffer_put(output, island_timestamp, 27); 
    io_buffer_put(output, run_length, 8);
  }

  // now the FADC codes
  int lastSample = 0;
  for(int j = 0; j < num_words; j++) {
    int sample[4];
    bool overflowB0 = ((input[j*10+3] & 0x08) != 0);
    bool overflowA0 = ((input[j*10+3] & 0x04) != 0);
    bool overflowB1 = ((input[j*10+3] & 0x02) != 0);
    bool overflowA1 = ((input[j*10+3] & 0x01) != 0);
    sample[3] = (overflowB0 << 12) | (input[j*10+4] << 4) | (input[j*10+5] >> 4);  
    sample[2] = (overflowA0 << 12) | ((input[j*10+5] & 0xf) << 8) | (input[j*10+6]);
    sample[1] = (overflowB1 << 12) | (input[j*10+7] << 4) | (input[j*10+8] >> 4);
    sample[0] = (overflowA1 << 12) | ((input[j*10+8] & 0xf) << 8) | (input[j*10+9]);

    for(int k = 0; k < 4; k++) {
      int diff = sample[k] - lastSample;
      lastSample = sample[k]; 


      if(diff <= 7 && diff >= -7) { 
        io_buffer_put(output, diff+8, 4);
      } else {
        io_buffer_put(output, 0, 4);
        io_buffer_put(output, sample[k], 13);
      }
    } 
  }

  return flush_output_buffer(output);
}

int nfadc_compress(unsigned char *input, int input_size, unsigned char *output, int userParam)
{
  io_buffer output_buffer;

  // store size of input data
  int *uncompressed_size_p = (int *) output;
  *uncompressed_size_p = input_size;
  output += sizeof(int);

  // compress samples
  start_output_buffer(&output_buffer, output);
  int compressed_size = encode_nfadc(input, input_size, &output_buffer, userParam);
  output += compressed_size;

  return compressed_size + sizeof(int);
}

int decode_nfadc(io_buffer * input, unsigned char * output, int input_size, int boardNumber)
{
  int num_fadc_words = input_size/10;

  // first retrieve the timestamps
  int j = 0;
  while(j < num_fadc_words) {

    int adc = io_buffer_get(input, 1);
    int timestamp = io_buffer_get(input, 27);
    int run_length = io_buffer_get(input, 8);

    for(int k = 0; k < run_length; k++) {
      int t2 = timestamp + k;
      if(boardNumber < 128) {
        t2 *= 2;
      }
      output[(j+k)*10+0] = (adc << 7) | ((t2 >> 20) & 0x7f);
      output[(j+k)*10+1] = (t2 >> 12) & 0xff;
      output[(j+k)*10+2] = (t2 >> 4) & 0xff;
      output[(j+k)*10+3] = (t2 & 0xf) << 4;
    }
    j += run_length; 

  }

  // now the FADC codes
  int lastSample = 0;
  for(int j = 0; j < num_fadc_words; j++) {
    int sample[4];
    for(int k = 0; k < 4; k++) {
      int encodedDiff = io_buffer_get(input, 4) & 0xf;
      if(encodedDiff == 0)  {
        sample[k] = io_buffer_get(input, 13) & 0x1fff;
      } else {
        sample[k] = lastSample + encodedDiff - 8;
      }
      lastSample = sample[k];
    } 

    int overflows = ((sample[0] & 0x1000) ? 0x01 : 0) |
                    ((sample[1] & 0x1000) ? 0x02 : 0) |
                    ((sample[2] & 0x1000) ? 0x04 : 0) |
                    ((sample[3] & 0x1000) ? 0x08 : 0);
    output[j*10+3] |= overflows;
    output[j*10+4] = (sample[3] >> 4) & 0xff;
    output[j*10+5] = ((sample[3] & 0xf) << 4) | ((sample[2] >> 8) & 0xf);
    output[j*10+6] = (sample[2] & 0xff);
    output[j*10+7] = (sample[1] >> 4) & 0xff;
    output[j*10+8] = ((sample[1] & 0xf) << 4) | ((sample[0] >> 8) & 0xf);
    output[j*10+9] = (sample[0] & 0xff);
  }

  return input_size;
}

int nfadc_expand(unsigned char *input, int input_size, unsigned char * output, int userParam)
{
  // 
  // Uncompress the data produced by nfadc_compress().
  // 

  // Get uncompressed data size
  int *uncompressed_size_p = (int *) input;
  int uncompressed_size = *uncompressed_size_p;
  input += sizeof(int);
  input_size -= sizeof(int);

  // Uncompress data
  io_buffer input_buffer;
  start_input_buffer(&input_buffer, input);
  decode_nfadc(&input_buffer, output, uncompressed_size, userParam);

  return uncompressed_size; 
}

void nfadc_dump(int num_fadc_words, unsigned char *data)
{
  for(int i = 0; i < num_fadc_words; i++) {
    printf("%d: ", i);
    for(int j = 0; j < 10; j++) {
      printf("%02x ", data[i*10+j]);
    }
    printf("\n");
  }
}

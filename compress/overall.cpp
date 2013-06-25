#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <byteswap.h>
#include <ctype.h>
#include "midas.h"

// Bring in optimized Adler32 implementation from zlib
#include <zlib.h>

#include "mucap_structures.h"
#include "mucap_compress.h"

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define le32_to_cpu(i) (i)
#else
#define le32_to_cpu(i) bswap_32(i)
#endif

static int verbose = 0;

int read_midas_event(FILE * fp, char *pevent)
{
    int event_size, size_read;

    size_read = fread(pevent, 1, sizeof(EVENT_HEADER), fp);
    if (size_read != sizeof(EVENT_HEADER))
      return !SUCCESS;

    event_size = le32_to_cpu(((EVENT_HEADER *) pevent)->data_size);

    size_read = fread(pevent + sizeof(EVENT_HEADER), 1, event_size, fp);
    if (size_read != event_size)
      return !SUCCESS;

    bk_swap(pevent, 0);

    return SUCCESS;
}

typedef int (*comp_fcn)(void *input, int input_size, unsigned char *output, int userParam);

void compress_bank(void *input_data, int input_size, void *output_event,
   char *output_name, comp_fcn f, int userParam)

{
    unsigned char *compressed_bank;
    bk_create(output_event, output_name, TID_BYTE, &compressed_bank);

    // compress the bank
    int compressed_size = (*f) (input_data, input_size, compressed_bank, userParam);

    // compute checksum
    int *checksum_ptr = (int *) &compressed_bank[compressed_size];
    *checksum_ptr = adler32(0x5555, (unsigned char *) input_data, input_size);
    // printf("checksum = 0x%x\n", *checksum_ptr);

    bk_close(output_event, compressed_bank + compressed_size + sizeof(int));

    if(verbose) {
      printf("%s: %d %d %0.3f\n", output_name, input_size, compressed_size,
        ((double) compressed_size)/input_size); 
    }
}

void compress_event(void *input_event, void *output_event)
{
    // If it's not a 32-bit event, just copy from input to output
    BANK_HEADER *bank_header = (BANK_HEADER *)input_event;
    BOOL is_32 = (bank_header->flags & BANK_FORMAT_32BIT);

    if(!is_32) {
      memcpy(output_event, input_event, 
		 bank_header->data_size + sizeof(BANK_HEADER));
      return;
    }

    BANK32 *input_bank = NULL;
    void *input_data = NULL;

    while (1) {
	int input_size =
	    bk_iterate32(input_event, &input_bank, &input_data);
	if (input_bank == NULL) {
	    break;
	}

	char output_name[5];
	if (should_compress_tdc400 && !strncmp(input_bank->name, "TDC", 3)) {
	    sprintf(output_name, "CTD%c", input_bank->name[3]);
	    compress_bank(input_data, input_size, output_event,
			  output_name, (comp_fcn) tdc400_compress, 0);
	} else if (should_compress_fadc
		   && !strncmp(input_bank->name, "ADC", 3)) {
	    sprintf(output_name, "CAD%c", input_bank->name[3]);
	    compress_bank(input_data, input_size, output_event,
			  output_name, (comp_fcn) fadc_compress, 0);
	} else if (input_bank->name[0] == 'N' 
                   && input_bank->name[1] >= 'a'
                   && input_bank->name[1] <= 'h'
                   && isxdigit(input_bank->name[2]) 
                   && isxdigit(input_bank->name[3])) {
	    sprintf(output_name, "n%c%c%c", input_bank->name[1], 
                    input_bank->name[2], input_bank->name[3]);
            int board_number = 16*(input_bank->name[2]-'0') + (input_bank->name[3]-'0');
            if(board_number < 128 && should_compress_n2fadc_tpc) {
   	      compress_bank(input_data, input_size, output_event,
			  output_name, (comp_fcn) n2fadc_tpc_compress, board_number);
            } else if(should_compress_n2fadc_ndet) {
   	      compress_bank(input_data, input_size, output_event,
			  output_name, (comp_fcn) n2fadc_ndet_compress, board_number);
            }
	} else if (should_compress_caen
		   && !strncmp(input_bank->name, "CAE", 3)) {
	    sprintf(output_name, "CCA%c", input_bank->name[3]);
	    compress_bank(input_data, input_size, output_event,
			  output_name, (comp_fcn) caen_compress, 0);
	} else if (should_compress_comp
		   && !strncmp(input_bank->name, "CMP", 3)) {
	    sprintf(output_name, "CCM%c", input_bank->name[3]);
	    compress_bank(input_data, input_size, output_event,
			  output_name, (comp_fcn) cmp_compress, 0);
	} else {
	    // Otherwise: if the input and output events are not the same,
            // just copy the bank whole.
            if(input_event != output_event) {
	      char *output_data;
	      bk_create(output_event, input_bank->name, input_bank->type,
		        &output_data);
	      memcpy(output_data, input_data, input_size);
	      bk_close(output_event, output_data + input_size);
          }
	}
    }
}

void compress_event_skim(void *input_event, void *output_event, bool keep_etc)
{
    // If it's not a 32-bit event, just copy from input to output
    BANK_HEADER *bank_header = (BANK_HEADER *)input_event;
    BOOL is_32 = (bank_header->flags & BANK_FORMAT_32BIT);

    if(!is_32 || EVENT_ID(input_event) != 1) {
      memcpy(output_event, input_event,
                 bank_header->data_size + sizeof(BANK_HEADER));
      return;
    }

    BANK32 *input_bank = NULL;
    void *input_data = NULL;

    while (1) {
        int input_size =
            bk_iterate32(input_event, &input_bank, &input_data);
        if (input_bank == NULL) {
            break;
        }

        char output_name[5];
        if (!strncmp(input_bank->name, "RTPC", 4)) {
            compress_bank(input_data, input_size, output_event,
                          "CTPC", (comp_fcn) stck_compress, 0);
        } else if (!strncmp(input_bank->name, "RHIT", 4)) {
            compress_bank(input_data, input_size, output_event,
                          "HITZ", (comp_fcn) hits_compress, 0);
        } else if (!strncmp(input_bank->name, "SKIM", 4)) {
          // save bank of gskim triggers
            if(input_event != output_event) {
              char *output_data;
              bk_create(output_event, "GSKM", input_bank->type,
                        &output_data);
              memcpy(output_data, input_data, input_size);
              bk_close(output_event, output_data + input_size);
          }
        } else if (!strncmp(input_bank->name, "ADC", 3)) {
	    sprintf(output_name, "CAD%c", input_bank->name[3]);
	    compress_bank(input_data, input_size, output_event,
			  output_name, (comp_fcn) fadc_compress, 0);
        } else if (keep_etc && (!strncmp(input_bank->name, "LAD", 3) ||
//                              !strncmp(input_bank->name, "LSPK", 4) ||
                                !strncmp(input_bank->name, "CAET", 4)))
        {
            // Otherwise: if the input and output events are not the same,
            // just copy the bank whole.

            if(input_event != output_event) {
              char *output_data;
              bk_create(output_event, input_bank->name, input_bank->type,
                        &output_data);
              memcpy(output_data, input_data, input_size);
              bk_close(output_event, output_data + input_size);
          }
        }
    }
}

typedef int (*expand_fcn)(unsigned char *input, int input_size, void *output, int userParam);

void expand_bank(unsigned char *input_data, int input_size, void *output_event,
   char *output_name, expand_fcn f, int userParam)
{
    unsigned char *uncompressed_bank;
    int bank_type = TID_DWORD;
    if(output_name[0] == 'N') bank_type = TID_BYTE;
    bk_create(output_event, output_name, bank_type, &uncompressed_bank);

    // uncompress the data; "sizeof(int)" accounts for the checksum word
    int uncompressed_size = (*f) (input_data, input_size - sizeof(int), 
                                  uncompressed_bank, userParam);

    // verify the Adler32 checksum
    int checksum = adler32(0x5555, uncompressed_bank, uncompressed_size);
    int *checksum_ptr = (int *) &input_data[input_size-sizeof(int)];
    if(checksum != *checksum_ptr) {
      printf("*** CHECKSUM TEST FAILED: Bank %s (0x%x -> 0x%x) ***\n",
         output_name, *checksum_ptr, checksum); 
    } else {
//       printf("Checksum OK for bank %s (0x%x)\n", output_name, checksum);
    }

    bk_close(output_event, uncompressed_bank + uncompressed_size);
}

void expand_event(void *input_event, void *output_event)
{
    // If it's not a 32-bit event, just copy from input to output
    BANK_HEADER *bank_header = (BANK_HEADER *)input_event;
    BOOL is_32 = (bank_header->flags & BANK_FORMAT_32BIT);

    if(!is_32) {
      memcpy(output_event, input_event, 
		 bank_header->data_size + sizeof(BANK_HEADER));
      return;
    }

    BANK32 *input_bank = NULL;
    unsigned char *input_data = NULL;

    while (1) {
	int input_size =
	    bk_iterate32(input_event, &input_bank, &input_data);
	if (input_bank == NULL) {
	    break;
	}

	char output_name[5];
	if (!strncmp(input_bank->name, "CTD", 3)) {
	    sprintf(output_name, "TDC%c", input_bank->name[3]);
	    expand_bank(input_data, input_size, output_event,
		        output_name, (expand_fcn) tdc400_expand, 0);
	} else if(input_bank->name[0] == 'n' 
                   && input_bank->name[1] >= 'a'
                   && input_bank->name[1] <= 'h'
                   && isxdigit(input_bank->name[2]) 
                   && isxdigit(input_bank->name[3])) {
	    sprintf(output_name, "N%c%c%c", input_bank->name[1],
                        input_bank->name[2], input_bank->name[3]);
            int board_number = 16*(input_bank->name[2]-'0') + (input_bank->name[3]-'0');
            if(board_number < 128) {
	      expand_bank(input_data, input_size, output_event,
		        output_name, (expand_fcn) n2fadc_tpc_expand, board_number);
            } else {
	      expand_bank(input_data, input_size, output_event,
		        output_name, (expand_fcn) n2fadc_ndet_expand, board_number);
            }
	} else if (!strncmp(input_bank->name, "CAD", 3)) {
	    sprintf(output_name, "ADC%c", input_bank->name[3]);
	    expand_bank(input_data, input_size, output_event,
		        output_name, (expand_fcn) fadc_expand, 0);
	} else if (!strncmp(input_bank->name, "CCA", 3)) {
	    sprintf(output_name, "CAE%c", input_bank->name[3]);
	    expand_bank(input_data, input_size, output_event,
		        output_name, (expand_fcn) caen_expand, 0);
	} else if (!strncmp(input_bank->name, "CCM", 3)) {
	    sprintf(output_name, "CMP%c", input_bank->name[3]);
	    expand_bank(input_data, input_size, output_event,
		        output_name, (expand_fcn) cmp_expand, 0);
	} else if (!strncmp(input_bank->name, "HITZ", 4)) {
	    expand_bank(input_data, input_size, output_event,
		        "HITS", (expand_fcn) hits_expand, 0);
	} else if (!strncmp(input_bank->name, "CTPC", 4)) {
	    expand_bank(input_data, input_size, output_event,
		        "STCK", (expand_fcn) stck_expand, 0);
	} else {
	    // Otherwise: if the input and output events are not the same,
            // just copy the bank whole.
            if(input_event != output_event) {
	      char *output_data;
	      bk_create(output_event, input_bank->name, input_bank->type,
			&output_data);
	      memcpy(output_data, input_data, input_size);
	      bk_close(output_event, output_data + input_size);
	    }
	}
    }
}

void compress_load_all()
{
  tdc400_load();
  fadc_load();
  n2fadc_tpc_load();
  n2fadc_ndet_load();
  caen_load();
  comp_load();
  hits_load();
  stck_load();
}

void compress_optimize_all()
{
  tdc400_optimize();
  fadc_optimize();
  n2fadc_tpc_optimize();
  n2fadc_ndet_optimize();
  caen_optimize();
  comp_optimize();
}

void compress_optimize_skim()
{
  hits_optimize();
  stck_optimize();
}


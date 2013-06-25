#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <byteswap.h>
#include "midas.h"

#include "mucap_structures.h"
#include "mucap_compress.h"

HNDLE hDB;

int main(int argc, char *argv[])
{
    char host_name[256], exp_name[32];

    cm_get_environment(host_name, sizeof(host_name), exp_name,
		       sizeof(exp_name));
    cm_connect_experiment(host_name, exp_name, "mucap_compress", NULL);
    cm_get_experiment_database(&hDB, NULL);

    compress_load_all();

    if(argc != 3) {
      printf("Usage: mucap_compress uncompressed_input.mid compressed_output.mid\n");
      exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
      printf("Unable to open %s\n", argv[1]);
      exit(1);
    }

    FILE *output_fp = fopen(argv[2], "w");
    if (!fp) {
      printf("Unable to open %s\n", argv[2]);
      exit(1);
    }

    char *input_event = new char[MAX_EVENT_SIZE];
    char *output_event = new char[MAX_EVENT_SIZE];

    while (read_midas_event(fp, input_event) == SUCCESS) {

        EVENT_HEADER header = *((EVENT_HEADER *) input_event);
        int event_id = header.event_id;

        if (event_id == EVENTID_BOR || event_id == EVENTID_EOR ||
            event_id == EVENTID_MESSAGE) {

           // For non-ordinary events, just write out the raw data
           fwrite(input_event, 1, sizeof(EVENT_HEADER) + header.data_size, 
                  output_fp);
        } else {

          bk_init32(output_event);
          compress_event(input_event + sizeof(EVENT_HEADER), output_event);
          header.data_size = bk_size(output_event);

          fwrite(&header, sizeof(EVENT_HEADER), 1, output_fp);
          fwrite(output_event, 1, header.data_size, output_fp);
       }
    }

    fclose(fp);
    fclose(output_fp);

    cm_disconnect_experiment();
}

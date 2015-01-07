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
    cm_connect_experiment(host_name, exp_name, "Compression", NULL);
    cm_get_experiment_database(&hDB, NULL);

    compress_load_all();

    if(argc != 2) {
      printf("Usage: mucap_optimize uncompressed_input.mid\n"); 
      exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
      printf("Unable to open %s\n", argv[1]);
      exit(1);
    }

    char *input_event = new char[MAX_EVENT_SIZE];
    char *output_event = new char[MAX_EVENT_SIZE];

    while (read_midas_event(fp, input_event) == SUCCESS) {
        EVENT_HEADER header = *((EVENT_HEADER *) input_event);

        int event_id = header.event_id;

        if (event_id == EVENTID_BOR || event_id == EVENTID_EOR ||
            event_id == EVENTID_MESSAGE) {

          continue;
        } 

        bk_init32(output_event);
        compress_event(input_event + sizeof(EVENT_HEADER), output_event);
    }

    compress_optimize_all();

    fclose(fp);

    cm_disconnect_experiment();
}

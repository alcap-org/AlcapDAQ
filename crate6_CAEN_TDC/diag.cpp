#include <stdio.h>
#include <stdarg.h>

#include "midas.h"

#include "odb_wrapper.h"
#include "crate.h"
#include "diag.h"

int diag_print_threshold = -1;
int diag_cm_msg_threshold = -1;

time_t last_message_time = 0; 
int num_messages_this_second = 0;

/* ******************************************************************* */
/*
 * diag_print_setup
 *
 * Reads threshold for diagnostic printout from ODB.
 */
void diag_print_setup()
{
  // Read the diagnostic print thresholds from the ODB
  diag_print_threshold = 
     odb_get_int("/Equipment/Crate %d/Settings/Diagnostic Print Level", crate_number);
  diag_cm_msg_threshold = 
     odb_get_int("/Equipment/Crate %d/Settings/Operator Message Level", crate_number);
}

/* ******************************************************************* */
/*
 * 
 */
void diag_print_impl(int level, char *format, va_list argp)
{
  // Format the message.
  char buffer[4096];
  vsnprintf(buffer, sizeof(buffer), format, argp);

  // Print on standard output
  // if (level <= diag_print_threshold) {
    printf("%s", buffer);
    fflush(stdout);
  // }

  // Send to cm_msg
  if(level <= diag_cm_msg_threshold) {

    // Be careful to avoid sending too many messages...let's limit ourselves
    // to 10 per second. 
    
    time_t now = time(NULL);
    if(last_message_time == now) {
      num_messages_this_second++;
      if(num_messages_this_second < 1) {
        cm_msg(MINFO, frontend_name, buffer); 
      }
    } else {
      num_messages_this_second = 0;
    }

    last_message_time = now;
  }
}

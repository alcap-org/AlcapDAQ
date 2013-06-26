#include <stdarg.h>

extern int diag_print_threshold;
extern int diag_cm_msg_threshold;

void diag_print_setup();
void diag_print_impl(int level, char *format, va_list ap);

inline void diag_print(int level, char *format, ...)
{
  if(diag_print_threshold < 0) {
    diag_print_setup();
  }
                                                                                
  // Return immediately if we don't even need to format the message.
  if(level > diag_print_threshold && level > diag_cm_msg_threshold) {
    return;
  }

  va_list argp;
  va_start(argp, format);
  diag_print_impl(level, format, argp);
  va_end(argp);

}

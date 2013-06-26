#include "midas.h"
#include "mcstd.h"
#include "camac_wrapper.h"

static BOOL camac_inited = FALSE;

void cam_init_once()
{
  if(!camac_inited) {
    cam_init();
    cam_inhibit_clear(0);
  } 
}

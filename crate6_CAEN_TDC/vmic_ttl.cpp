#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/io.h>

#include "midas.h"

#include "crate.h"
#include "diag.h"
#include "odb_wrapper.h"
#include "vme.h"

INT vmic_ttl_init();
INT vmic_ttl_start_block();
INT vmic_ttl_stop_block();
INT vmic_ttl_poll_live();

struct readout_module vmic_ttl_module = {
  vmic_ttl_init,        // init
  NULL,                 // exit
  NULL,                 // pre_bor
  NULL,                 // bor
  NULL,                 // eor
  vmic_ttl_poll_live,   // poll_live
  NULL,                 // poll_dead
  vmic_ttl_start_block, // start_block
  vmic_ttl_stop_block,  // stop_block
  NULL,                 // read
};

/*
 * VMIC registers.
 */
#define VMIC_PORT12 0x12
#define VMIC_PORT34 0x10
#define VMIC_PORT56 0x16
#define VMIC_PORT78 0x14
#define VMIC_CSRU   0x20
#define VMIC_CSRL   0x21

/* 
 * VMIC outputs
 */ 
#define VMIC_OUTPUT_1      0x00004000
#define VMIC_OUTPUT_2      0x00000800
#define VMIC_OUTPUT_3      0x00000100
#define VMIC_OUTPUT_4      0x00000030
#define VMIC_OUTPUT_5      0x00000004
#define VMIC_OUTPUT_6      0x00800000
#define VMIC_OUTPUT_7      0x00100000
#define VMIC_OUTPUT_8      0x00020000

/*
 * ...and their aliases for muCap:
 */
#define VMIC_OUTPUT_EW1          VMIC_OUTPUT_1
#define VMIC_OUTPUT_EW2          VMIC_OUTPUT_2
#define VMIC_OUTPUT_LATCH_RESET  VMIC_OUTPUT_3
#define VMIC_OUTPUT_CAENandCOMP  VMIC_OUTPUT_4
#define VMIC_OUTPUT_CRATE4       VMIC_OUTPUT_5
#define VMIC_OUTPUT_SOFTSTOP     VMIC_OUTPUT_6
#define VMIC_OUTPUT_CLOCK_RESET  VMIC_OUTPUT_7
#define VMIC_OUTPUT_START1       (VMIC_OUTPUT_1 | VMIC_OUTPUT_4)
#define VMIC_OUTPUT_START2       (VMIC_OUTPUT_2 | VMIC_OUTPUT_4)

/*
 * VMIC input channels.
 */
#define VMIC_INPUT_END_EVENT    62
#define VMIC_INPUT_BLOCK        59

/*
 *
 */
static WORD vmic_vme_base;
static struct vme_handle *vmic_handle;
static WORD vmic_current_low;
static WORD vmic_current_high;

static BOOL vmic_enabled;

static BOOL not_really_started;

extern INT cycle_ram();

/*
 *
 */
void vmic_set(DWORD new_value)
{
  WORD low = new_value & 0xffff;
  WORD high = (new_value >> 16) & 0xffff;

  if(low != vmic_current_low) { 
    vme_write_d16(vmic_handle, vmic_vme_base | VMIC_PORT12, low);
    vmic_current_low = low;
  }
 
  if(high != vmic_current_high) { 
    vme_write_d16(vmic_handle, vmic_vme_base | VMIC_PORT34, high);
    vmic_current_high = high;
  }
}

/*
 *
 */
void vmic_turn_on(DWORD new_channels)
{
  WORD low = vmic_current_low | (new_channels & 0xffff);
  WORD high = vmic_current_high | ((new_channels >> 16) & 0xffff);
  vmic_set((high << 16) | low);
}

INT vmic_read_input(int channel)
{
  short status, mask;
  
  int reg;
 
  // decide which port and bit we're reading
  if ((channel >= 0) && (channel < 16)) {
    reg = VMIC_PORT56;
    mask = (1 << channel);
  } else if ((channel >= 16) && (channel < 32)) {
    reg = VMIC_PORT78;
    mask = (1 << (channel-16));
  } else if ((channel >= 32) && (channel < 48)) {
    reg = VMIC_PORT12;
    mask = (1 << (channel-32));
  } else if ((channel >= 48) && (channel < 64)) {
    reg = VMIC_PORT34;
    mask = (1 << (channel-48));
  }

  status = vme_read_d16(vmic_handle, vmic_vme_base | reg); 
 
  if ((status & mask)==0) {
    return 0;
  } else {
    return 1;
  }
}

/*
 * vmic_ttl_init
 *
 */
INT vmic_ttl_init()
{
  vmic_enabled = 
    (odb_find_key("/Equipment/Crate %d/Settings/VMIC", crate_number) != 0);

  if(!vmic_enabled) {
    return SUCCESS;
  }

  diag_print(4,"vmic_ttl_init()\n");

  vmic_vme_base = 
    odb_get_word("/Equipment/Crate %d/Settings/VMIC/vme address", crate_number);

  struct vme_mapping_ctrl mapping = {
          VMEMAP_DWIDTH_16,
          VMEMAP_ASPACE_A16,
          VMEMAP_SUPUSERAM_SUPER,
          VMEMAP_PRGDATAAM_DATA
  };

  vmic_handle = vme_open(vmic_vme_base, mapping, 0x100, 0);
  if(vmic_handle == NULL) {
    diag_print(0, "Unable to open handle for VMIC TTL I/O register\n");
    return FE_ERR_HW;
  }

  // Set control register: port 4 as input, all others as output
  int status = 
    vme_write_d16_checked(vmic_handle, vmic_vme_base | VMIC_CSRU, 0xefff);
  if(status != SUCCESS) {
    diag_print(0, "Unable to write CSRU value to VMIC TTL I/O register\n");
    return FE_ERR_HW;
  }

  // Wait a moment to allow this setting to take effect
  ss_sleep(400);

  // Turn off all outputs
/*
  vme_write_d16(vmic_handle, vmic_vme_base | VMIC_PORT12, 0);
  vmic_current_high = 0;
  vme_write_d16(vmic_handle, vmic_vme_base | VMIC_PORT34, 0);
  vmic_current_low = 0;
*/
  vmic_set(0);

  not_really_started = 0;

  return SUCCESS;
}

/*
 * vmic_ttl_start_block
 *
 */
INT vmic_ttl_start_block()
{
  if(!vmic_enabled) {
    return SUCCESS;
  }

  diag_print(4,"vmic_ttl_start_block()\n");

  // Is crate 4 going to be part of the cycle?
  DWORD maybe_crate4 = 0;
  if(crate_is_participating(4)) {
    maybe_crate4 = VMIC_OUTPUT_CRATE4;
  } 

  // If there is already a block in progress, refuse to start another one.
  int block_input = vmic_read_input(VMIC_INPUT_BLOCK);
  if(block_input != 0) {
    diag_print(0, "BLOCK is high when it should be low!\n");
    vmic_set(0);
    ss_sleep(10);
    vmic_set(VMIC_OUTPUT_LATCH_RESET);
    ss_sleep(10);
    vmic_set(0);
    ss_sleep(100);
    not_really_started = 1;
    return FE_ERR_HW; 
  }

  // First send a pulse on the latch reset line:
  vmic_set(0);
  vmic_set(VMIC_OUTPUT_LATCH_RESET | maybe_crate4);
//  vmic_set(0 | maybe_crate4);

#if 1
  // Now check whether the reset was effective--did it make the EndEvent
  // signal clear?
  if(vmic_read_input(VMIC_INPUT_END_EVENT) != 0) {
    diag_print(0, "END_EVENT signal not clearing with reset\n");
    not_really_started = 1;
    return FE_ERR_HW;
  }
#endif

#if 0
  // Try another reset...going straight into the start of the cycle
  // so that the compressor timing matches the CAENs.
  vmic_set(VMIC_OUTPUT_LATCH_RESET | maybe_crate4);
#endif

  int ram = cycle_ram();

  diag_print(2, "Starting RAM %d cycle\n", ram);

  //
  // Finally, start the cycle.  
  // 
  if(ram == 1) {
    vmic_set(VMIC_OUTPUT_START1 | maybe_crate4);
    vmic_set(VMIC_OUTPUT_EW1 | maybe_crate4);
  } else {
    vmic_set(VMIC_OUTPUT_START2 | maybe_crate4);
    vmic_set(VMIC_OUTPUT_EW2 | maybe_crate4);
  }

  // We don't currently have any way of testing that the cycle started 
  // correctly.  Perhaps something to think about adding...
  not_really_started = 0;
  return SUCCESS;
}

/*
 * vmic_ttl_stop_block
 *
 */
INT vmic_ttl_stop_block()
{
  if(!vmic_enabled) {
    return SUCCESS;
  }

  diag_print(4,"vmic_ttl_stop_block()\n");

  // set SOFTSTOP output
  vmic_turn_on(VMIC_OUTPUT_SOFTSTOP);
}

/*
 * vmic_ttl_poll_live()
 *
 * Called periodically while a block is active; performs active readout.
 *
 * Returns:
 * - ordinarily 0,
 * - a request for a "soft stop" end-of-block, or
 * - an error code
 */
INT vmic_ttl_poll_live()
{
  if(!vmic_enabled) {
    return SUCCESS;
  }

  //diag_print(4,"vmic_ttl_poll_live()\n");

  INT end_event = vmic_read_input(VMIC_INPUT_END_EVENT);

  if(end_event || not_really_started) {
    vmic_set(0); 
    return FE_END_BLOCK;
  } else {
    return SUCCESS;
  }
}


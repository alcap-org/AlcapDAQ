#include <map>
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <linux/if_packet.h>

#include "midas.h"

#include "crate.h"
#include "vme.h"
#include "odb_wrapper.h"
#include "diag.h"

INT new_fadc_bor();
INT new_fadc_eor();
INT new_fadc_read(char *pevent);

const int TP_BLOCK_SIZE = 8192;
const int TP_NUM_BLOCKS = 1024;
const int FRAME_SIZE = 2048;
const int NUM_FRAMES = 4096;

unsigned char *packet_space = 0;
bool frame_examined[NUM_FRAMES];

struct readout_module new_fadc_module = {
  NULL,                 // init
  NULL,                 // exit
  NULL,                 // pre_bor
  new_fadc_bor,          // bor
  new_fadc_eor,          // eor
  NULL,                  // poll_live
  NULL,                  // poll_dead
  NULL,                 // start_cycle
  NULL,                 // stop_cycle
  new_fadc_read,         // read
};

bool allPacketsReceived(bool diag = false);

int packet_socket;

// packet format, including Ethernet-level headers
struct fadc_packet
{
  unsigned char dest_addr[6];       // 0 
  unsigned char src_addr[6];        // 6
  unsigned short protocol;          // 12
  unsigned short content_length;    // 14
  unsigned char buffer_number;      // 16
  unsigned char admin_message;      // 17
  unsigned short packet_serial;     // 18
  unsigned long be_debug;           // 20
  // unsigned char fe_debug[8];        // 24
  // OR
  // unsigned char fe_debug[9];        // 24
  // unsigned char frontend_number;    // 32 or 33
  // unsigned char data[];             // 33 or 34
};

// state associated with each board
const int max_boards = 256;
struct fadc_board 
{
  bool enabled;
  INT  interface;
  map<int, fadc_packet *> packets;
  int start_packet;
  int stop_packet;
  int first_packet;
  double first_packet_time;
  int last_packet;
  double last_packet_time;
  bool start_packet_seen;
  bool stop_packet_seen;
};
struct fadc_board board[max_boards];

void receivePackets()
{
  for(int i = 0; i < NUM_FRAMES; i++) {
    if(frame_examined[i]) continue;  

    unsigned char *frame = packet_space + i*FRAME_SIZE;
    struct tpacket_hdr *tph = (struct tpacket_hdr *) frame;
    if(!(tph->tp_status & TP_STATUS_USER)) continue;

    struct fadc_packet *pkt = (struct fadc_packet *) (frame + tph->tp_mac);
    int board_number = pkt->src_addr[5];
    if(!board[board_number].enabled) continue;

    int content_length = ntohs(pkt->content_length);
    int buffer_number = pkt->buffer_number;
    int admin_message = pkt->admin_message;
    int packet_serial = ntohs(pkt->packet_serial);

    board[board_number].packets[packet_serial] = pkt;
    if(admin_message & 0x1) {
      board[board_number].start_packet = packet_serial;
      board[board_number].start_packet_seen = true;
    }
   if(admin_message & 0x2) {
     board[board_number].stop_packet = packet_serial;
     board[board_number].stop_packet_seen = true;
   }

   double packet_time = tph->tp_sec*1e6 + tph->tp_usec;
   double fpt = board[board_number].first_packet_time;
   if(packet_time < fpt || fpt < 0) {
     board[board_number].first_packet = packet_serial;
     board[board_number].first_packet_time = packet_time;
   }
   double lpt = board[board_number].last_packet_time;
   if(packet_time > lpt || lpt < 0) {
     board[board_number].last_packet = packet_serial;
     board[board_number].last_packet_time = packet_time;
   }

   frame_examined[i] = true;
  }
}

void forgetPackets(bool reallyAll = false)
{
  for(int i = 0; i < max_boards; i++) {
    if(board[i].enabled) {
      board[i].packets.clear();
      board[i].start_packet_seen = false;
      board[i].stop_packet_seen = false;
      board[i].first_packet_time = -1;
    }
  }

  // release all frame buffers back to the kernel
  for(int i = 0; i < NUM_FRAMES; i++) {
    if(frame_examined[i] || reallyAll) {
      unsigned char *frame = packet_space + i*FRAME_SIZE;
      struct tpacket_hdr *tph = (struct tpacket_hdr *) frame;
      tph->tp_status = TP_STATUS_KERNEL;
      frame_examined[i] = false;
    }
  }
}

bool allPacketsReceived(bool diag)
{
  // check whether we have a start packet, a stop packet, and a stream of
  // continuous serial numbers in between.

  bool missing = false;

  for(int i = 0; i < max_boards; i++) {
    if(!board[i].enabled) continue;
    if(!board[i].start_packet_seen) {
      board[i].start_packet = board[i].first_packet;
      if(diag) {
        printf("-- board 0x%02x waiting on start packet\n", i);
#if 0
        missing = true;
#endif
      } else {
#if 0
        return false;
#endif
      }
      if(diag) {
        printf("-- board 0x%02x workaround for missing start packet\n", i);
      }
    }
    if(!board[i].stop_packet_seen) {
      board[i].stop_packet = board[i].last_packet;
      if(diag) {
        printf("-- board 0x%02x waiting on stop packet\n", i);
#if 0
        missing = true;
#endif
      } else {
        return false;
      }
    }
  }
  for(int i = 0; i < max_boards; i++) {
    if(!board[i].enabled) continue;
    int num_packets = board[i].stop_packet - board[i].start_packet;
    if(num_packets < 0) {
      num_packets += 0x10000;
    }

    int num_missing = 0; 
#if 0
    if(diag) {
      printf("Start=%d Stop=%d\n", board[i].start_packet, board[i].stop_packet);
    }
#endif
    for(int j = num_packets; j >= 0; j--) {
      int pn = (board[i].start_packet + j) & 0xffff;
      if(board[i].packets[pn] == NULL) {
        if(diag) {
          printf("-- board 0x%02x waiting on packet %d\n", i, pn);
          num_missing++;
          missing = true;
        } else {
          return false; 
        }
      }
    }
    if(diag && num_missing != 0) {
      printf("-- board 0x%02x waiting on %d/%d packets\n", i, 
        num_missing, num_packets);
    }
  }
  return !missing;
}


void setReg(char *if_name, int board, int fadc, int reg,
  unsigned long long value, bool acquire)
{
  // look up the index of the interface
  int if_index = if_nametoindex(if_name);
  if(if_index < 0) {
    printf("Unknown interface %s\n", if_name);
    return;
  }

  struct ifreq req;
  strcpy(req.ifr_name, if_name);
  if(ioctl(packet_socket, SIOCGIFHWADDR, &req) < 0) {
    perror("SIOCGIFHWADDR");
    return;
  }
  unsigned char src[ETH_ALEN];
  memcpy(src, req.ifr_hwaddr.sa_data, ETH_ALEN);

  unsigned char dst[ETH_ALEN] = { 0x00, 0x12, 0x6d, 0x12, 0x34, 0xff};
  dst[5] = board;
  struct sockaddr_ll dstaddr;
  dstaddr.sll_family = AF_PACKET;
  dstaddr.sll_protocol = htons(ETH_P_ALL);
  dstaddr.sll_ifindex = if_index;
  dstaddr.sll_hatype = 0;
  dstaddr.sll_pkttype = 0;
  dstaddr.sll_halen = ETH_ALEN;
  memcpy(dstaddr.sll_addr, dst, ETH_ALEN);

  // format a packet
  unsigned char packet[512];
  memcpy(&packet[0], dst, ETH_ALEN);
  memcpy(&packet[6], src, ETH_ALEN);

  // protocol
  packet[12] = 0x0b;
  packet[13] = 0x04;

  // the content
  packet[14] = (!acquire << 7) |(fadc << 5) | reg;
  packet[15] = (value >> 56) & 0xff;
  packet[16] = (value >> 48) & 0xff;
  packet[17] = (value >> 40) & 0xff;
  packet[18] = (value >> 32) & 0xff;
  packet[19] = (value >> 24) & 0xff;
  packet[20] = (value >> 16) & 0xff;
  packet[21] = (value >>  8) & 0xff;
  packet[22] = (value      ) & 0xff;

  int packet_length = 512;

  if(sendto(packet_socket, packet, packet_length, 0, (const sockaddr *) &dstaddr, sizeof(dstaddr)) < 0) {
    perror("sendto");
  }

  // wait for completion
  usleep(2000);
}

void setupRegs()
{
  for(int i = 0; i < max_boards; i++) {

    // If no settings exist for the board, disable it.
    if(!odb_find_key("/Equipment/Crate %d/Settings/NFADC %02x", 
                     crate_number, i)) {
      board[i].enabled = false;
      continue;
    } 
    printf("Found board %02x\n", i);

    bool enabled = 
      odb_get_bool("/Equipment/Crate %d/Settings/NFADC %02x/Enabled",
                   crate_number, i);
    board[i].enabled = enabled;

    const char *yn = board[i].enabled ? "YES" : "NO";
    printf("FADC board 0x%02x: enabled: %s\n", i, yn);

    board[i].interface = odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Interface", crate_number, i);

    for(int chan = 0; chan < 8; chan++) {
      int led_mode = 
        odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Channel %d/LED Mode",
                    crate_number, i, chan);

      int lower_threshold = 
        odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Channel %d/Lower threshold", 
                     crate_number, i, chan);
      int upper_threshold = 
        odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Channel %d/Upper threshold", 
                    crate_number, i, chan);
      int pulser_period = 
        odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Channel %d/Pulser period", 
                    crate_number, i, chan);
      int trigger_mask = 
        odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Channel %d/Trigger mask", 
                    crate_number, i, chan);
      int presamples = 
        odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Channel %d/Presamples", 
                    crate_number, i, chan);
      int stretch_samples = 
        odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Channel %d/Stretch samples", 
                  crate_number, i, chan);
      int dcm_phase =
        odb_get_int("/Equipment/Crate %d/Settings/NFADC %02x/Channel %d/DCM phase", 
                  crate_number, i, chan);

      if(enabled) {
        unsigned long long r0 =
               (lower_threshold & 0xfff) |
               ((upper_threshold & 0xfff) << 12) |
               ((trigger_mask & 0xf) << 24) |
               ((presamples & 0xfff) << 28) |
               (((unsigned long long) (stretch_samples & 0xfff)) << 40);
        unsigned long long r1 =
           (pulser_period & 0xffffffff) |
           (((unsigned long long) (led_mode & 0xf)) << 32); 
        unsigned long long r2 = dcm_phase;

        int frontend = chan/2;

#if 0
        if(i == 0xff) {
          if(frontend == 1) {
            frontend = 2;
          } else if(frontend == 2) {
            frontend = 1;
          }
        }
#endif

	char interface[8];
	sprintf(interface,"eth%i",board[i].interface);
	//printf("Board %i interface %i (%s)\n",i,board[i].interface,interface);

        if(chan % 2 == 0) { 
          setReg(interface, i, frontend, 0, r0, 0);
          setReg(interface, i, frontend, 1, r1, 0);
          setReg(interface, i, frontend, 4, r2, 1);
        } else {
          setReg(interface, i, frontend, 2, r0, 0);
          setReg(interface, i, frontend, 3, r1, 0);
          setReg(interface, i, frontend, 5, r2, 1);
        }
      }
    } 
  }
}

INT new_fadc_bor()
{
    // open a socket to receive packets for our protocol
    packet_socket = socket(PF_PACKET, SOCK_RAW, htons(0x0b04));

    if(packet_socket < 0) {
      perror("socket");
      return FE_ERR_HW;
    }

    // request that the packets be placed in a memory-mapped buffer
    struct tpacket_req req;
    req.tp_block_size = TP_BLOCK_SIZE;
    req.tp_block_nr = TP_NUM_BLOCKS;
    req.tp_frame_size = FRAME_SIZE;
    req.tp_frame_nr = NUM_FRAMES;
    int status = setsockopt(packet_socket, SOL_PACKET, PACKET_RX_RING, 
       (void *) &req, sizeof(req));
    if(status < 0) {
      perror("setsockopt PACKET_RX_RING");
      return FE_ERR_HW;
    }

    // map the buffer into our process address space
    packet_space = (unsigned char *) mmap(0, TP_BLOCK_SIZE*TP_NUM_BLOCKS, 
      PROT_READ|PROT_WRITE, MAP_SHARED, packet_socket, 0);
    if(packet_space == MAP_FAILED) {
      perror("mmap"); 
      return FE_ERR_HW;
    }

    setupRegs();
    forgetPackets(true);

    return SUCCESS;
}

INT new_fadc_eor()
{
    if(packet_space != 0) {
      munmap(packet_space, TP_BLOCK_SIZE*TP_NUM_BLOCKS);
      packet_space = 0;
    }
    close(packet_socket);
    return SUCCESS;
}

static struct timeval tv_start;

INT new_fadc_read(char *pevent)
{
  printf("new_fadc_read\n");

  char bk_name[80];
  char *pdata;

  bool timed_out = false;
  struct timeval t1, t2;
  gettimeofday(&t1,0);

  receivePackets();

  while(!allPacketsReceived()) {
    sched_yield(); 
    receivePackets();

    gettimeofday(&t2,0);
    double tdiff = (t2.tv_usec-t1.tv_usec) + 1000000*(t2.tv_sec-t1.tv_sec);
    if(tdiff > 200000) {
      printf("Timed out:\n");
      allPacketsReceived(true);  // for diagnostic printout
      timed_out = true;
      break;
    }
  } 

  for(int i = 0; i < max_boards; i++) {
    if(board[i].enabled) {
      printf("Board 0x%02x: start %d (%c) - stop %d (%c)\n", i,
        board[i].start_packet, board[i].start_packet_seen ? '-' :  'X',
        board[i].stop_packet, board[i].stop_packet_seen ? '-' :  'X');
    }
  }

  bk_init32(pevent);

//  if(!timed_out) {
//    printf("Block OK\n");
    for(int i = 0; i < max_boards; i++) {
      if(!board[i].enabled) continue;

      // values for TPC personality
      int word_length = 9;
      int fadc_number_offset = 32;
      int data_offset = 33;
      if(i >= 128) {  
        // values for neutron detector personality
        word_length = 10;
        fadc_number_offset = 33;
        data_offset = 34;
      }

      for(int j = 0; j < 8; j++) {
        sprintf(bk_name, "N%c%02x", 'a' + j, i);
        bk_create(pevent, bk_name, TID_BYTE, &pdata);

        int num_packets = board[i].stop_packet - board[i].start_packet + 1;
        if(num_packets < 0) {
          num_packets += 0x10000;
        }
 
        for(int k = 0; k < num_packets; k++) {
          int pn = (board[i].start_packet + k) & 0xffff;
          struct fadc_packet *pkt = board[i].packets[pn];
          if(pkt) {
            int fadc_number = (int) *(((unsigned char *) pkt) + fadc_number_offset);
            if(fadc_number != 0 && fadc_number != 1) 
              printf("board %d: fadc_number = %d !\n", fadc_number);
            int frontend = ((pkt->buffer_number)/4)*2 + fadc_number;
            if(frontend == j) { 
              int len = ntohs(pkt->content_length) - word_length;
              if((len % word_length) != 0) 
                printf("board %d: content_length = %d !\n", i, len);
              else if(len > 0) {
                char *p = ((char *) pkt) + data_offset;
                memcpy(pdata, p, len);
                pdata += len;
              }
            }
          }
        }

        bk_close(pevent, pdata);
      }
    }
// }

  forgetPackets();
      
  return SUCCESS;
}

/* ******************************************************************* */

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
#include <netpacket/packet.h>

#include "midas.h"

#include "crate.h"
#include "vme.h"
#include "odb_wrapper.h"
#include "diag.h"

INT new_fadc_bor();
INT new_fadc_eor();
INT new_fadc_poll_live();
INT new_fadc_read(char *pevent);

const int fadc_buffer_size = 256*1024 * sizeof(int);

struct readout_module new_fadc_module = {
  NULL,                 // init
  NULL,                 // exit
  NULL,                 // pre_bor
  new_fadc_bor,          // bor
  new_fadc_eor,          // eor
  new_fadc_poll_live,    // poll_live
  NULL,                 // poll_dead
  NULL,                 // start_cycle
  NULL,                 // stop_cycle
  new_fadc_read,         // read
};

bool allPacketsReceived(bool diag = false);

int packet_socket;

// packet format, including Ethernet-level headers
struct fadc_packet
{
  unsigned char dest_addr[6];
  unsigned char src_addr[6];
  unsigned short protocol;
  unsigned short content_length;
  unsigned char buffer_number;
  unsigned char admin_message;
  unsigned short packet_serial;
};

// state associated with each board
const int max_boards = 256;
struct fadc_board 
{
  bool enabled;
  map<int, fadc_packet *> packets;
  int start_packet;
  int stop_packet;
  bool start_packet_seen;
  bool stop_packet_seen;
};
struct fadc_board board[max_boards];
char *buf = NULL;
char *bufp; 

void receivePackets()
{
  while(1) {
    int status = recv(packet_socket, bufp, 1518, 0); 
    if(status < 0) {
      if(errno != EWOULDBLOCK && errno != EAGAIN) {
        perror("receivePackets");
      }
      break;
    }

#if 0
    for(int i = 0; i < status; i++) {
      printf("%02x ", 0xff & bufp[i]);
    }
    printf("\n");
#endif

    struct fadc_packet *pkt = (struct fadc_packet *) bufp;
    int board_number = pkt->src_addr[5];
    int content_length = ntohs(pkt->content_length);
    int buffer_number = pkt->buffer_number;
    int admin_message = pkt->admin_message;
    int packet_serial = ntohs(pkt->packet_serial);

    if(content_length > 1500) {
      printf("board %d length %d buffer %d admin %d serial %d\n", 
         board_number, content_length, buffer_number, 
         admin_message, packet_serial);
    }

    board[board_number].packets[packet_serial] = pkt;
    if(admin_message & 0x1) {
      board[board_number].start_packet = packet_serial;
      board[board_number].start_packet_seen = true;
    }
    if(admin_message & 0x2) {
      board[board_number].stop_packet = packet_serial;
      board[board_number].stop_packet_seen = true;
    }
 
    bufp += status;

    if(allPacketsReceived()) {
       break;
    }

  }
}

void forgetPackets()
{
  for(int i = 0; i < max_boards; i++) {
    if(board[i].enabled) {
      board[i].packets.clear();
      board[i].start_packet_seen = false;
      board[i].stop_packet_seen = false;
    }
  }
  bufp = buf;
}

bool allPacketsReceived(bool diag = false)
{
  // check whether we have a start packet, a stop packet, and a stream of
  // continuous serial numbers in between.

  bool missing = false;

  for(int i = 0; i < max_boards; i++) {
    if(!board[i].enabled) continue;
    if(!board[i].start_packet_seen) {
      if(diag) {
        printf("-- board 0x%02x waiting on start packet\n", i);
        missing = true;
      } else {
        return false;
      }
    }
    if(!board[i].stop_packet_seen) {
      if(diag) {
        printf("-- board 0x%02x waiting on stop packet\n", i);
        missing = true;
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
 
    for(int j = num_packets; j >= 0; j--) {
      int pn = (board[i].start_packet + j) & 0xffff;
      if(board[i].packets[pn] == NULL) {
        if(diag) {
          printf("-- board 0x%02x waiting on packet 0x%04x\n", i, pn);
          missing = true;
        } else {
          return false; 
        }
      }
    }
  }
  return !missing;
}


void setReg(char *if_name, int board, int fadc, int reg,
  unsigned long long value)
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
  packet[14] = (fadc << 5) | reg;
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
          ((presamples & 0xf) << 28) |
          (((unsigned long long) (stretch_samples & 0xff)) << 32);
         unsigned long long r1 =
           (pulser_period & 0xffffffff) |
           (((unsigned long long) (led_mode & 0xf)) << 32); 
         unsigned long long r2 = dcm_phase;

        int frontend = chan/2;
        if(frontend == 1) {
          frontend = 2;
        } else if(frontend == 2) {
          frontend = 1;
        }

        if(chan % 2 == 0) { 
          setReg("eth0", i, frontend, 0, r0);
          setReg("eth0", i, frontend, 1, r1);
          setReg("eth0", i, frontend, 4, r2);
        } else {
          setReg("eth0", i, frontend, 2, r0);
          setReg("eth0", i, frontend, 3, r1);
          setReg("eth0", i, frontend, 5, r2);
        }
      }
    }
  }
}

INT new_fadc_bor()
{
    packet_socket = socket(PF_PACKET, SOCK_RAW, htons(0x0b04));

    if(packet_socket < 0) {
      perror("socket");
    }

    buf = new char[fadc_buffer_size];

#if 0
    // set up a timeout so that recv() calls return after ~1 ms even if
    // there is no packet.
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;
    if(setsockopt(packet_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
      perror("setsockopt");
    }
#endif
#if 1
    // make socket non-blocking
    if(fcntl(packet_socket, F_SETFL, O_NONBLOCK)) {
      perror("fcntl O_NONBLOCK");
    }
#endif

    setupRegs();
    forgetPackets();

    return SUCCESS;
}

INT new_fadc_eor()
{
    if(buf != NULL) {
      delete[] buf;
      buf = NULL;
    }
    close(packet_socket);
    return SUCCESS;
}

INT new_fadc_poll_live()
{
  receivePackets();
  return SUCCESS;
}

INT new_fadc_read(char *pevent)
{
  char bk_name[80];
  char *pdata;

  bool timed_out = false;
  struct timeval t1, t2;
  gettimeofday(&t1,0);
  int loop_count = 0;
  while(!allPacketsReceived()) {
//    printf("Waiting to read...\n");
    sched_yield();
    receivePackets();

    gettimeofday(&t2,0);
    double tdiff = (t2.tv_usec-t1.tv_usec) + 1000000*(t2.tv_sec-t1.tv_sec);
    if(tdiff > 10000 && loop_count > 10) {
      printf("Timed out:\n");
      allPacketsReceived(true);  // for diagnostic printout
      timed_out = true;
      break;
    }
    loop_count++;
  } 

  bk_init32(pevent);

  if(!timed_out) {
    for(int i = 0; i < max_boards; i++) {
      if(!board[i].enabled) continue;

      for(int j = 0; j < 4; j++) {
        sprintf(bk_name, "N%c%02x", 'a' + j, i);
        bk_create(pevent, bk_name, TID_BYTE, &pdata);

        int num_packets = board[i].stop_packet - board[i].start_packet;
        if(num_packets < 0) {
          num_packets += 0x10000;
        }
 
        for(int k = 0; k < num_packets; k++) {
          int pn = (board[i].start_packet + k) & 0xffff;
          int frontend = (board[i].packets[pn]->buffer_number - 1) / 3;
          if(frontend == j) { 
            int len = ntohs(board[i].packets[pn]->content_length);
            char *p = ((char *) (board[i].packets[pn])) + 20;
            memcpy(pdata, p, len);
            pdata += len;
          }
        }

        bk_close(pevent, pdata);
      }
    }
  }

  forgetPackets();
      
  return SUCCESS;
}

/* ******************************************************************* */

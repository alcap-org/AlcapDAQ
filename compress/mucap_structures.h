typedef struct tdc400_word {

  unsigned char bytes1[4];
  unsigned short time;
  unsigned char bytes2[2];

} tdc400_word;

static inline unsigned char & tdc400_byte(tdc400_word & w, int i) {
  if(i < 4) {
    return w.bytes1[3-i];
  } else {
    return w.bytes2[5-i];
  }
}

typedef struct caen_word {
  unsigned int time:20, edge:1, r2:2, start:1, channel:7, r1:1;
} caen_word;

typedef struct channel_hit {
  double time;
  int    parameter; 
} channel_hit;

typedef union comp_word {
  struct {
    unsigned int time:24, fpga:6, a:1, b:1;
  } r1;

  unsigned char bytes[4];

  unsigned int  word;
} comp_word;

typedef struct fadc_word {
  unsigned char channel[4];
} fadc_word;

typedef struct td_elt {
  double time;
  unsigned short word[3];
} td_elt;

typedef struct track_elt {
  double time;
  unsigned short word[6];
} track_elt;

typedef struct sniff_elt {
  long entranceLindex, entranceRindex;
  double entranceLtime, entranceRtime;
  long endptLindex, endptRindex;
  double endptLtime, endptRtime;
  long endptanode, ngood, ngood5;
} sniff_elt;

typedef struct TPC_Anode_Map_elt {
  int tdc400;
  int word;
  unsigned short bitmask;
  int bitshift;
} TPC_Anode_Map_elt;

typedef struct thresh_bits {
  unsigned short Aword[6]; // anode sections
  unsigned short Sword[3]; // strip sections
} thresh_bits;

typedef struct stack_elt {
  double time;
  bool thrbits[3];
  thresh_bits thr[3];
} stack_elt;

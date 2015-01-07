// io_buffer.cpp ----------------------------------------

typedef struct io_buffer {
  unsigned int w;
  int w_bits;
  unsigned int *start;
  unsigned int *p;
  int num_codes;
} io_buffer;

void start_output_buffer(io_buffer * buffer, unsigned char *p);
int flush_output_buffer(io_buffer * buffer);
void io_buffer_put(io_buffer * buffer, unsigned int code_bits, 
  int code_length);
void start_input_buffer(io_buffer * buffer, unsigned char *p);
unsigned int io_buffer_get(io_buffer * buffer, int num_bits);

// huffman.cpp ----------------------------------------

typedef struct huffman_node huffman_node;

typedef struct huffman_table {
  int num_symbols;
  int *frequency;
  unsigned int *code_bits;
  int *code_length;
  huffman_node *tree;
  int *decode_symbols;
} huffman_table;

void huffman_put_symbol(huffman_table * table, io_buffer * buffer, 
  int symbol);
int huffman_get_symbol(huffman_table * table, io_buffer * buffer);
BOOL huffman_optimize_tree(huffman_table * table);
void huffman_init_default(huffman_table * table, int num_symbols);
BOOL save_huffman(char *key_dir, huffman_table * table);
BOOL load_huffman(char *key_dir, huffman_table * table);

// rle.cpp ----------------------------------------

typedef struct rle_state {
  int running_digit;
  int run_length;
  int huffman_length[2];
  int use_extension_word;
  huffman_table *huffman;
} rle_state;

void rle_bits_setup();
void rle_put(unsigned char b, rle_state * s, io_buffer * output);
int flush_rle(rle_state * s, io_buffer * output);
int rle_get(io_buffer * input, rle_state * s);

// tdc400_compress.cpp ----------------------------------------

typedef struct tdc400_word tdc400_word;

void tdc400_load();
void tdc400_optimize();
int tdc400_compress(tdc400_word * input, int input_size,
                    unsigned char *output, int userParam);
int tdc400_expand(unsigned char *input, int input_size,
                  tdc400_word * output, int userParam);

extern huffman_table time_huffman, bits_huffman[2];
extern BOOL should_compress_tdc400;

// stck_compress.cpp ----------------------------------------

typedef struct stack_elt stack_elt;

void stck_load();
void stck_optimize();
int stck_compress(stack_elt * input, int input_size,
                    unsigned char *output, int userParam);
int stck_expand(unsigned char *input, int input_size,
                    stack_elt * output, int userParam);

// caen_compress.cpp ----------------------------------------

typedef struct caen_word caen_word;

void comp_load();
void comp_optimize();
int caen_compress(caen_word * input, int input_size, unsigned char *output, int userParam);
int caen_expand(unsigned char *input, int input_size, caen_word * output, int userParam);

extern huffman_table caen_channel_huffman;
extern BOOL should_compress_caen;

// hits_compress.cpp ----------------------------------------

typedef struct channel_hit channel_hit;

void hits_load();
void hits_optimize();
int hits_compress(channel_hit * input, int input_size, unsigned char *output, int userParam);
int hits_expand(unsigned char *input, int input_size, channel_hit * output, int userParam);

// comp_compress.cpp ----------------------------------------

typedef union comp_word comp_word;

void caen_load();
void caen_optimize();
int cmp_compress(comp_word * input, int input_size, unsigned char *output, int userParam);
int cmp_expand(unsigned char *input, int input_size, comp_word * output, int userParam);

extern huffman_table cmp_channel_huffman, cmp_bits_huffman[2];
extern BOOL should_compress_comp;

// fadc_compress.cpp ----------------------------------------

typedef struct fadc_word fadc_word;

void fadc_load();
void fadc_optimize();
int fadc_compress(fadc_word * input, int input_size, unsigned char *output, int userParam);
int fadc_expand(unsigned char *input, int input_size, fadc_word * output, int userParam);

extern huffman_table fadc_run_huffman, fadc_small_huffman;
extern BOOL should_compress_fadc;

// nfadc_compress.cpp ----------------------------------------

typedef struct nfadc_word nfadc_word;

void nfadc_load();
void nfadc_optimize();
int nfadc_compress(unsigned char * input, int input_size, unsigned char *output, int userParam);
int nfadc_expand(unsigned char *input, int input_size, unsigned char * output, int userParam);

extern BOOL should_compress_nfadc;

/* Beginning of run11 additions */
// Run 11 tpc definition
// n2fadc_tpc_compress.cpp ----------------------------------------

typedef struct n2fadc_tpc_word n2fadc_tpc_word;

void n2fadc_tpc_load();
void n2fadc_tpc_optimize();
int n2fadc_tpc_compress(unsigned char * input, int input_size, unsigned char *output, int userParam);
int n2fadc_tpc_expand(unsigned char *input, int input_size, unsigned char * output, int userParam);

extern BOOL should_compress_n2fadc_tpc;

// n2fadc_ndet_compress.cpp ----------------------------------------

typedef struct n2fadc_ndet_word n2fadc_ndet_word;

void n2fadc_ndet_load();
void n2fadc_ndet_optimize();
int n2fadc_ndet_compress(unsigned char * input, int input_size, unsigned char *output, int userParam);
int n2fadc_ndet_expand(unsigned char *input, int input_size, unsigned char * output, int userParam);

extern BOOL should_compress_n2fadc_ndet;

/* end of run 11 definitions */

// overall.cpp ----------------------------------------

extern HNDLE hDB;
typedef int (*expand_fcn)(unsigned char *input, int input_size, void *output, int userParam);

int read_midas_event(FILE * fp, char *pevent);
void compress_event(void *input_event, void *output_event, bool USE_OLD_COMPRESSION=false);
void compress_event_skim(void *input_event, void *output_event, bool keep_etc, 
			 bool USE_OLD_COMPRESSION=false);
void expand_event(void *input_event, void *output_event, bool USE_OLD_COMPRESSION=false);
void expand_bank(unsigned char *input_data, int input_size, void *output_event,
		   char *output_name, expand_fcn f, int userParam);
void compress_load_all(bool USE_OLD_COMPRESSION=false);

void compress_optimize_all(bool USE_OLD_COMPRESSION=false);


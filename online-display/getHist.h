#ifndef getHist_h
#define getHist_h
class TFile;
class TSocket;
extern TFile   *openRootFile(const char *filename);
extern TSocket *openSocket(const char *hostname, const int port_nr=9090);
extern void     closeSource();
extern TObject *getObject(const char *name);
//extern void    *getPointer(const char *Name);
extern void     clear_recv_buffer();
extern int      get_status_code();
extern ULong_t  get_run_number();
extern void     reset_error_code();

// error codes
/// cannot connect to the server 
#define RDISP_ERR_NO_CON         1

/// Bad response from the server
#define RDISP_ERR_BAD_RESPONSE   2

///  No response from the server
#define RDISP_ERR_NO_RESPONSE    3

///  No sourses open
#define RDISP_ERR_NO_SOURCE      4

/// Broken pipe
#define RDISP_ERR_BROKEN_PIPE    5

/// Cannot open file
#define RDISP_ERR_NO_FILE        6

/// Cannot load root folder from file
#define RDISP_ERR_NO_FOLDER      7


#endif

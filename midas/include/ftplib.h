#ifndef EXPRT
#define EXPRT
#endif

#define MAX_ANSWERS 10          /* Number of known goodest answers for reqest */
#define QUIT 0
#define Ctrl(x) ((x) - '@')
#define FREE(x) memset ( &x , '\0' , sizeof x )
#define CUT(x) ((x)&0xff)

typedef struct {
   int sock;
   int data;
   int err_no;
} FTP_CON;

#define ftp_account(ftp,acc)       ftp_command(ftp,"ACCT %s",acc,230,EOF)
#define ftp_user(ftp,user)         ftp_command(ftp,"USER %s",user,230,331,332,EOF)
#define ftp_password(ftp,pas)      ftp_command(ftp,"PASS %s",pas,230,332,EOF)
#define ftp_type(ftp,type)         ftp_command(ftp,"TYPE %s",type,200,EOF)
#define ftp_chdir(ftp,dir)         ftp_command(ftp,"CWD %s",dir,200,250,EOF)
#define ftp_mkdir(ftp,dir)         ftp_command(ftp,"MKD %s",dir,200,257,EOF)
#define ftp_rm(ftp,dir)            ftp_command(ftp,"DELE %s",dir,200,250,EOF)
#define ftp_ascii(ftp)             ftp_type(ftp,"A")
#define ftp_binary(ftp)            ftp_type(ftp,"I")
#define ftp_open_read(ftp,file)    ftp_data(ftp,"RETR %s",file)
#define ftp_open_write(ftp,file)   ftp_data(ftp,"STOR %s",file)
#define ftp_open_append(ftp,file)  ftp_data(ftp,"APPE %s",file)

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif


   int EXPRT ftp_bye(FTP_CON * con);
   int EXPRT ftp_close(FTP_CON * con);
   int EXPRT ftp_connect(FTP_CON ** con, char *host_name, unsigned short port);
   int EXPRT ftp_login(FTP_CON ** con, char *host, unsigned short port, char *user,
                       char *pass, char *acct);
   int EXPRT ftp_move(FTP_CON * con, char *old_name, char *new_name);
   int EXPRT ftp_data(FTP_CON * con, char *command, char *param);
   int EXPRT ftp_port(FTP_CON * con, int, int, int, int, int, int);
   int EXPRT ftp_get(FTP_CON * con, char *local_name, char *remote_name);
   int EXPRT ftp_put(FTP_CON * con, char *local_name, char *remote_name);
   int EXPRT ftp_send(int sock, char *buffer, int n_bytes);
   int EXPRT ftp_receive(int sock, char *buffer, int bsize);
   int EXPRT ftp_send_message(FTP_CON * con, char *message);
   int EXPRT ftp_get_message(FTP_CON * con, char *message);
   BOOL EXPRT ftp_good(int number, ...);
   int EXPRT ftp_command(FTP_CON * con, char *command, char *param, ...);
   int EXPRT ftp_dir(FTP_CON * con, char *file);
   char EXPRT *ftp_pwd(FTP_CON * con);
   void EXPRT ftp_debug(int (*debug_func) (char *message),
                        int (*error_func) (char *message));

#ifdef __cplusplus
}
#endif

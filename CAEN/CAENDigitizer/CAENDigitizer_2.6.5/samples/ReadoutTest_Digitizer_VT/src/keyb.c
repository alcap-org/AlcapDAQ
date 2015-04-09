/*
kbhit() and getch() for Linux/UNIX
Chris Giese <geezer@execpc.com>	http://my.execpc.com/~geezer
*/

#ifdef linux
    #include <sys/time.h> /* struct timeval, select() */
    #include <termios.h> /* tcgetattr(), tcsetattr() */
    #include <stdlib.h> /* atexit(), exit() */
    #include <unistd.h> /* read() */
    #include <stdio.h> /* printf() */
    #include <string.h> /* memcpy() */

#define CLEARSCR "clear"

static struct termios g_old_kbd_mode;

/*****************************************************************************/
static void cooked(void)
{
	tcsetattr(0, TCSANOW, &g_old_kbd_mode);
}

static void raw(void)
{
	static char init;
/**/
	struct termios new_kbd_mode;

	if(init)
		return;
/* put keyboard (stdin, actually) in raw, unbuffered mode */
	tcgetattr(0, &g_old_kbd_mode);
	memcpy(&new_kbd_mode, &g_old_kbd_mode, sizeof(struct termios));
	new_kbd_mode.c_lflag &= ~(ICANON | ECHO);
	new_kbd_mode.c_cc[VTIME] = 0;
	new_kbd_mode.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &new_kbd_mode);
/* when we exit, go back to normal, "cooked" mode */
	atexit(cooked);

	init = 1;
}

/*****************************************************************************/
/*  SLEEP  */
/*****************************************************************************/
void Sleep(int t) {
    usleep( t*1000 );
}

/*****************************************************************************/
/*  GETCH  */
/*****************************************************************************/
int getch(void)
{
	unsigned char temp;

	raw();
    /* stdin = fd 0 */
	if(read(0, &temp, 1) != 1)
		return 0;
	return temp;

}


/*****************************************************************************/
/*  KBHIT  */
/*****************************************************************************/
int kbhit()
{

	struct timeval timeout;
	fd_set read_handles;
	int status;

	raw();
    /* check stdin (fd 0) for activity */
	FD_ZERO(&read_handles);
	FD_SET(0, &read_handles);
	timeout.tv_sec = timeout.tv_usec = 0;
	status = select(0 + 1, &read_handles, NULL, NULL, &timeout);
	if(status < 0)
	{
		printf("select() failed in kbhit()\n");
		exit(1);
	}
    return (status);
}


#else  // Windows

    #include <conio.h>
#define CLEARSCR "cls"
#endif


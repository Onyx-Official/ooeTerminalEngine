#ifndef _OOE_TERMINAL_ENGINE_H_
#define _OOE_TERMINAL_ENGINE_H_

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <limits.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <signal.h>
#include <memory.h>
#include <stdlib.h>

#define NUL	0
#define DONT_CARE  55995 
#define CTRL_KEY(k)	((k) & 0x1F)
#define SAFE_ALLOC_ADDITIONAL	33
#define INVERSE_COLOR           500

typedef char        	BYTE;
typedef char		GLYPH;
typedef struct VBUF	Vbuf;

struct termios 	orig_termIOS;	// Original State of the Terminal

typedef struct
{
    GLYPH	glyph;
    int		bg;
    int		fg;
} CHAR_T;

struct terminalEngine
{
    int		    m_nScreenWidth;		    // Width of the terminal
    int		    m_nScreenHeight;		// Height of the terminal
    CHAR_T*         m_bufScreen;            // Contents to draw to screen
    float           fElapsedTime;           // Time between frames
    int             key;                    // Input
};

enum KEYS
{
    KEY_ESC 		= 0x1B,			// ^[	0x1B	27
    KEY_BACKSPACE	= 0x7F,
    KEY_ARROW_UP	= 1000,
    KEY_ARROW_DOWN,
    KEY_ARROW_RIGHT,
    KEY_ARROW_LEFT,
    HOME_KEY,
    END_KEY,
    INSERT_KEY,
    DEL_KEY,
    PAGE_UP,
    PAGE_DOWN,
    F1_KEY,
    F2_KEY,
    F3_KEY,
    F4_KEY,
    F5_KEY,
    F6_KEY,
    F7_KEY,
    F8_KEY,
    F9_KEY,
    F10_KEY,
    F11_KEY,
    F12_KEY
};

/*** Variable Buffer ***/
struct VBUF
{
    char* data;
    size_t size;
    size_t cap;
};

Vbuf vbuf_init()
{
    Vbuf vbuf = {NULL, 0, SAFE_ALLOC_ADDITIONAL};
    vbuf.data = malloc(SAFE_ALLOC_ADDITIONAL);

    return vbuf;
}

static void vbuf_Realloc(Vbuf* vbuf_ptr, size_t newCapacity)
{
    if (newCapacity < vbuf_ptr->size)
        vbuf_ptr->size = newCapacity;

    char* data = malloc(newCapacity);

    // Copy data frmo vbuf_ptr->data to vbuf.data
    memcpy(vbuf_ptr->data, data, vbuf_ptr->size);

    vbuf_ptr->data = data;
    vbuf_ptr->cap = newCapacity;
}

void vbuf_append(Vbuf* vbuf_ptr, const char *const data, const size_t data_size)
{
    if (data_size >= vbuf_ptr->cap)
        vbuf_Realloc(vbuf_ptr, (vbuf_ptr->cap + data_size + SAFE_ALLOC_ADDITIONAL));

    memcpy((char*) data, &vbuf_ptr->data[vbuf_ptr->size - 1], data_size);
}

void vbuf_free(Vbuf* vbuf_ptr)
{
    free(vbuf_ptr->data);
    *vbuf_ptr = (Vbuf){NULL, 0, 0};
}

// Set terminal into raw mode
void tE_cfmakeraw(t)
    struct termios* t;
{
    t->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    t->c_oflag &= ~OPOST;
    t->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    t->c_cflag &= ~(CSIZE|PARENB);
	t->c_cflag |= CS8;
}

// Read in terminal input
int tE_tty_input()
{
    // Read in key from the terminal
    BYTE byte = NUL;	// Want to read in ONE BYTE to differentiaite between ANSI ESC CODE & printable character
    int nread = NUL;
    while ((nread = read(STDIN_FILENO, &byte, 1)) != 1)	// Keep on checking for the return value of read...
        if (nread == -1 && errno != EAGAIN)	return -1;

    // Check if it is an ESC sequence or just ESC key
    if (byte == 0x1B)	// Equal to '\x1b'/0x1B/27
    {
        fd_set fd;
        struct timeval timeout;
        FD_ZERO(&fd);
        FD_SET(STDIN_FILENO, &fd);
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int selret = select(1, &fd, NULL, NULL, &timeout);

        if (selret == 1)	// Input available, possible sequence
        {
            BYTE seq[4];	// Rest of sequence for ANSI ESC code
            // More bytes would have most likely been read in, read() forward
            if (read(STDIN_FILENO, &seq[0], 1) != 1)	return '\x1b';	// Byte was not sucessfully read, just return ESC
            if (read(STDIN_FILENO, &seq[1], 1) != 1)	return '\x1b';

            // If it contains '[' then it is an ANSI ESC SEQUENCE
            if (seq[0] == '[')
            {
                // Detect F5-F12 keys and HOME, END, PGUP, PGDWN keys
                if (seq[1] >= '0' && seq[1] <= '9')
                {
                    if (read(STDIN_FILENO, &seq[2], 1) != 1)	return '\x1b';
                    if (seq[2] == '~')
                    {
                        switch (seq[1])
                        {
                        case '1':	return HOME_KEY;	// ^[[1~
                        case '2':	return INSERT_KEY;	// ^[[2~
                        case '3':	return DEL_KEY;		// ^[[3~ || BACKSPACE || ^H
                        case '4':	return END_KEY;		// ^[[4~
                        case '5':	return PAGE_UP;		// ^[[5~
                        case '6':	return PAGE_DOWN;	// ^[[6~
                        case '7':	return HOME_KEY;	// ^[[7~
                        case '8':	return END_KEY;		// ^[[8~
                        }
                    } else if (seq[2] >= '0' && seq[2] <= '9') {
                        if (read(STDIN_FILENO, &seq[3], 1) != 1)	return '\x1b';
                        if (seq[3] == '~')
                            switch (seq[1])
                            {
                            case '1':
                                switch(seq[2])
                                {
                                case '5':	return F5_KEY;	// ^[[15~
                                case '7':	return F6_KEY;	// ^[[17~
                                case '8':	return F7_KEY;	// ^[[18~
                                case '9':	return F8_KEY;	// ^[[19~
                                }
                                break;
                            case '2':
                                switch (seq[2])
                                {
                                case '0':	return F9_KEY;	// ^[[20~
                                case '1':	return F10_KEY;	// ^[[21~
                                case '3':	return F11_KEY;	// ^[[23~
                                case '4':	return F12_KEY;	// ^[[24~
                                }
                                break;
                            }
                    }
                }
                switch (seq[1])	// Soft keys
                {
                case 'A':	return KEY_ARROW_UP;	// ^[[A
                case 'B':	return KEY_ARROW_DOWN;	// ^[[B
                case 'C':	return KEY_ARROW_RIGHT;	// ^[[C
                case 'D':	return KEY_ARROW_LEFT;	// ^[[D
                case 'H':	return HOME_KEY;	// ^[[H
                case 'F':	return END_KEY;		// ^[[F
                }
            } else if (seq[0] == 'O') {
                switch (seq[1])
                {
                case 'H':	return HOME_KEY;	// ^[OH
                case 'F':	return END_KEY;		// ^[OF
                case 'P':	return F1_KEY;		// ^[OP
                case 'Q':	return F2_KEY;		// ^[OQ
                case 'R':	return F3_KEY;		// ^[OR
                case 'S':	return F4_KEY;		// ^[OS
                }
            }
            return '\x1b';
        } else if (selret == -1)    {
            return -1;
        } else {
            return KEY_ESC;
        }
    } else {
        return byte;
    }
}

int tE_getCursorPosition(int* row, int* col)
{
	char buf[32];
  	unsigned int i = 0;
  	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
  	while (i < sizeof(buf) - 1)
	  {
    	if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    	if (buf[i] == 'R') break;
    	i++;
  	}
  	buf[i] = '\0';
  	if (buf[0] != '\x1b' || buf[1] != '[') return -1;
 	if (sscanf(&buf[2], "%d;%d", row, col) != 2) return -1;
  	return 0;
}

int tE_getTerminalWindowSize(int* row, int* col)
{
    struct winsize ws;
    if (1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return tE_getCursorPosition(row, col);
    } else {
        *col = ws.ws_col;
        *row = ws.ws_row;
        return 0;
    }
}

int tE_setTerminalRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termIOS);
    struct termios raw;	tE_cfmakeraw(&raw);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    return tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void tE_disableTerminalRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termIOS);
}

void Draw(struct terminalEngine* tE_ptr, int x, int y, GLYPH glyph, int bg, int fg)
{
    tE_ptr->m_bufScreen[y * tE_ptr->m_nScreenWidth + x] = (CHAR_T){glyph, bg, fg};
}

void Clip(int* x, int* y, struct terminalEngine tE)
{
    if (*x < 0)                     *x = 0;
    if (*x >= tE.m_nScreenWidth)    *x = tE.m_nScreenWidth;
    if (*y < 0)                     *y = 0;
    if (*y >= tE.m_nScreenHeight)    *y = tE.m_nScreenHeight;
}

void Fill(struct terminalEngine* tE_ptr, int x1, int y1, int x2, int y2, GLYPH glyph, int bg, int fg)
{
    Clip(&x1, &y1, *tE_ptr);
    Clip(&x2, &y2, *tE_ptr);

    for (int x = x1; x < x2; x++)
        for (int y = y1; y < y2; y++)
            Draw(tE_ptr, x, y, glyph, bg, fg);
}

void WriteConsoleOutput(struct terminalEngine* tE_ptr)
{
    write(STDOUT_FILENO, "\x1b[?25l", 6);	// Hide terminal cursor
    for (int i = 0; i < tE_ptr->m_nScreenWidth * tE_ptr->m_nScreenHeight; i++)
    {
        char buf[50];
        int size = snprintf(buf, sizeof(buf), "\x1b[38;5;%dm\x1b[48;5;%dm%c\x1b[m", tE_ptr->m_bufScreen[i].fg, tE_ptr->m_bufScreen[i].bg, tE_ptr->m_bufScreen[i].glyph);
        write(STDOUT_FILENO, buf, size);
    }
    write(STDOUT_FILENO, "\x1b[?25h", 6);
}

#endif
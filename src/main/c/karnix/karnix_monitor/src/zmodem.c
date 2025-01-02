/*
 * This implementation of ZModem protocol is old, buggy and non-linear, but still usable.
 * So I left code almost without modifications.
 *
 * Original code was written by Chuck Forsberg from Omen Technology Inc. in 05-23-87. Thank you Chuck!
 *
 * by Ruslan Zalata, 2025, Tyumen, Russia.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <alloca.h>

#include "uart.h"
#include "soc.h"
#include "utils.h"
#include "zmodem.h"

#define OK 0
#define FALSE 0
#define TRUE 1
#define ERROR (-1)

/*
 * Max value for HOWMANY is 255.
 *   A larger value reduces system overhead but may evoke kernel bugs.
 *   133 corresponds to an XMODEM/CRC sector
 */
#define HOWMANY 133

#define CANBREAK 1

#define DEBUGZ	1

int Zmodem=1;		/* ZMODEM protocol requested */
int Nozmodem = 0;	/* If invoked as "rb" */
unsigned Baudrate = 115200;

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

void flushmo(void);





/* Ward Christensen / CP/M parameters - Don't change these! */
#define ENQ 005
#define CAN ('X'&037)
#define XOFF ('s'&037)
#define XON ('q'&037)
#define SOH 1
#define STX 2
#define EOT 4
#define ACK 6
#define NAK 025
#define CPMEOF 032
#define WANTCRC 0103	/* send C not NAK to get crc not checksum */
#define TIMEOUT (-2)
#define RCDO (-3)
#define ERRORMAX 5
#define RETRYMAX 5
#define WCEOT (-10)
#define SECSIZ 128	/* cp/m's Magic Number record size */
#define PATHLEN 257	/* ready for 4.2 bsd ? */
#define KSIZE 1024	/* record size with k option */
#define UNIXFILE 0x8000	/* happens to the the S_IFREG file mask bit for stat */

int Lastrx;
int Crcflg;
int Firstsec;
int Eofseen;		/* indicates cpm eof (^Z) has been received */
int Errors = 0;
int Restricted=0;	/* restricted; no /.. or ../ in filenames */
/* Sorry, Regulus and some others don't work right in raw mode! */
int Readnum = HOWMANY;	/* Number of bytes to ask for in read() from modem */

#define DEFBYTL 2000000000L	/* default rx file size */
long ZModemBytesleft;		/* number of bytes of incoming file left */
long Modtime;		/* Unix style mod time for incoming file */
short Filemode;		/* Unix style mode for incoming file */
char *ZModemPathname;
long ZModemRxBytes;
long ZModemMaxBytes;
int ZModemRxOffset;
char *Progname;		/* the name by which we were called */

int Batch=0;
int Wcsmask=0377;
int Topipe=0;
int MakeLCPathname=TRUE;	/* make received pathname lower case */
int Verbose=0;
int Quiet=0;		/* overrides logic that would otherwise set verbose */
int Nflag = 0;		/* Don't really transfer files */
int Rxbinary=TRUE;	/* receive all files in bin mode */
int Rxascii=FALSE;	/* receive files in ascii (translate) mode */
int Thisbinary;		/* current file is to be received in bin mode */
int Blklen;		/* record length of received packets */
char *secbuf;
char *linbuf;
int Lleft;		/* number of characters in linbuf */
char Lzmanag;		/* Local file management request */
char zconv;		/* ZMODEM file conversion request */
char zmanag;		/* ZMODEM file management request */
char ztrans;		/* ZMODEM file transport request */
int Zctlesc;		/* Encode control characters */
int Zrwindow = 1400;	/* RX window size (controls garbage count) */
int lastsent;
int tryzhdrtype=ZRINIT;	/* Header type to send corresponding to Last rx close */


/*
 * Routine to calculate the free bytes on the current file system
 *  ~0 means many free bytes (unknown)
 */
long getfree()
{
	return(ZModemMaxBytes);	/* many free bytes ... */
}

/*
 *  Debugging information output interface routine
 */
/* VARARGS1 */
/*
void vfile(char *f, ...)
{
	va_list args;
	va_start(args, f);
	if (Verbose > 2) {
		swd_print(f, args);
		swd_print("\n");
	}

	va_end(args);


}
*/
#define vfile(f, ...) if(Verbose>1){printf(f, ## __VA_ARGS__); printf("\r\n");}


/*
 * Log an error
 */
/*VARARGS1*/
void zperr(s,p,u)
char *s, *p, *u;
{
	if (Verbose <= 0)
		return;
	printf("Retry %d: ", Errors);
	printf(s, p, u);
	printf("\r\n");
}




/*
 *   Z M . C
 *    ZMODEM protocol primitives
 *    07-28-87  Chuck Forsberg Omen Technology Inc
 *
 * Entry point Functions:
 *	zsbhdr(type, hdr) send binary header
 *	zshhdr(type, hdr) send hex header
 *	zgethdr(hdr, eflag) receive header - binary or hex
 *	zsdata(buf, len, frameend) send data
 *	zrdata(buf, len) receive data
 *	stohdr(pos) store position data in Txhdr
 *	long rclhdr(hdr) recover position offset from header
 */

int Rxtimeout = 1;		/* Seconds to wait for something */

#ifndef UNSL
#define UNSL unsigned
#endif


/* Globals used by ZMODEM functions */
int Rxframeind;		/* ZBIN ZBIN32, or ZHEX type of frame received */
int Rxtype;		/* Type of header received */
int Rxcount;		/* Count of data bytes received */
char Rxhdr[4];		/* Received header */
char Txhdr[4];		/* Transmitted header */
long Rxpos;		/* Received file position */
long Txpos;		/* Transmitted file position */
int Txfcs32;		/* TURE means send binary frames with 32 bit FCS */
int Crc32t;		/* Display flag indicating 32 bit CRC being sent */
int Crc32;		/* Display flag indicating 32 bit CRC being received */
int Znulls;		/* Number of nulls to send at beginning of ZDATA hdr */
char *Attn;		/* Attention string rx sends to tx on err */

void zsendline(int c);
void xsendline(int c);
void sendline(int c);
int zsbh32(char *hdr, int type);
void zputhex(int c);
int zsda32(char* buf, int length, int frameend);
int zrdat32(char* buf, int length);
int zdlread(void);
int noxrd7(void);
int readline(int timeout);
void bttyout(char c);
int zrbhdr(char* hdr);
int zrbhdr32(char* hdr);
int zrhhdr(char* hdr);
int zgethex(void);
int zgeth1(void);
int tryz(int);


static char *frametypes[] = {
	"Carrier Lost",		/* -3 */
	"TIMEOUT",		/* -2 */
	"ERROR",		/* -1 */
#define FTOFFSET 3
	"ZRQINIT",
	"ZRINIT",
	"ZSINIT",
	"ZACK",
	"ZFILE",
	"ZSKIP",
	"ZNAK",
	"ZABORT",
	"ZFIN",
	"ZRPOS",
	"ZDATA",
	"ZEOF",
	"ZFERR",
	"ZCRC",
	"ZCHALLENGE",
	"ZCOMPL",
	"ZCAN",
	"ZFREECNT",
	"ZCOMMAND",
	"ZSTDERR",
	"xxxxx"
#define FRTYPES 22	/* Total number of frame types in this array */
			/*  not including psuedo negative entries */
};


void sendbrk(void)
{
	uart_sendBreak(UART0);
}

/*
 *  Return non 0 iff something to read from io descriptor f
 */
int rdchk(void *f)
{
	return uart_readOccupancy(UART0);
	
}

char checked = '\0' ;


/* crctab calculated by Mark G. Mendel, Network Systems Corporation */
static unsigned short crctab[256] = {
    0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
    0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
    0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
    0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
    0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
    0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
    0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
    0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
    0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
    0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
    0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
    0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
    0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
    0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
    0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
    0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
    0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
    0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
    0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
    0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
    0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
    0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
    0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
    0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
    0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
    0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
    0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
    0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
    0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
    0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
    0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
    0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
};

#define updcrc(cp, crc) ( crctab[((crc >> 8) & 255)] ^ (crc << 8) ^ cp)

static long cr3tab[] = { /* CRC polynomial 0xedb88320 */
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#ifdef NFGM
long UPDC32(char b, long c)
{
	return (cr3tab[((int)c ^ b) & 0xff] ^ ((c >> 8) & 0x00FFFFFF));
}

#else

#define UPDC32(b, c) (cr3tab[((int)c ^ b) & 0xff] ^ ((c >> 8) & 0x00FFFFFF))
#endif





/* Send ZMODEM binary header hdr of type type */
int zsbhdr(int type, register char* hdr)
{
	register int n;
	register unsigned short crc;

	vfile("zsbhdr: %s %lx", frametypes[type+FTOFFSET], rclhdr(hdr));
	if (type == ZDATA)
		for (n = Znulls; --n >=0; )
			zsendline(0);

	xsendline(ZPAD); xsendline(ZDLE);

	if (Crc32t=Txfcs32)
		zsbh32(hdr, type);
	else {
		xsendline(ZBIN); zsendline(type); crc = updcrc(type, 0);

		for (n=4; --n >= 0; ++hdr) {
			zsendline(*hdr);
			crc = updcrc((0377& *hdr), crc);
		}
		crc = updcrc(0,updcrc(0,crc));
		zsendline(crc>>8);
		zsendline(crc);
	}
	if (type != ZDATA)
		flushmo();
}


/* Send ZMODEM binary header hdr of type type */
int zsbh32(char *hdr, int type)
{
	register int n;
	register UNSL long crc;

	xsendline(ZBIN32);  zsendline(type);
	crc = 0xFFFFFFFFL; crc = UPDC32(type, crc);

	for (n=4; --n >= 0; ++hdr) {
		crc = UPDC32((0377 & *hdr), crc);
		zsendline(*hdr);
	}
	crc = ~crc;
	for (n=4; --n >= 0;) {
		zsendline((int)crc);
		crc >>= 8;
	}
}

/* Send ZMODEM HEX header hdr of type type */
int zshhdr(int type, char *hdr)
{
	register int n;
	register unsigned short crc;

	vfile("zshhdr: %s %lx", frametypes[type+FTOFFSET], rclhdr(hdr));
	sendline(ZPAD); sendline(ZPAD); sendline(ZDLE); sendline(ZHEX);
	zputhex(type);
	Crc32t = 0;

	crc = updcrc(type, 0);
	for (n=4; --n >= 0; ++hdr) {
		zputhex(*hdr); crc = updcrc((0377 & *hdr), crc);
	}
	crc = updcrc(0,updcrc(0,crc));
	zputhex(crc>>8); zputhex(crc);

	/* Make it printable on remote machine */
	//sendline(015); sendline(012);
	sendline(015); sendline(0212);
	/*
	 * Uncork the remote in case a fake XOFF has stopped data flow
	 */
	if (type != ZFIN && type != ZACK)
		sendline(021);
	flushmo();
}

/*
 * Send binary array buf of length length, with ending ZDLE sequence frameend
 */
int zsdata(char* buf, int length, int frameend)
{
	register unsigned short crc;

	vfile("zsdata: length=%d end=%x", length, frameend);
	if (Crc32t)
		zsda32(buf, length, frameend);
	else {
		crc = 0;
		for (;--length >= 0; ++buf) {
			zsendline(*buf); crc = updcrc((0377 & *buf), crc);
		}
		xsendline(ZDLE); xsendline(frameend);
		crc = updcrc(frameend, crc);

		crc = updcrc(0,updcrc(0,crc));
		zsendline(crc>>8); zsendline(crc);
	}
	if (frameend == ZCRCW) {
		xsendline(XON);  flushmo();
	}
}

int zsda32(char* buf, int length, int frameend)
{
	unsigned long crc;

	crc = 0xFFFFFFFFL;
	for (;--length >= 0;++buf) {
		crc = UPDC32((0377 & *buf), crc);
		zsendline(*buf);
	}
	xsendline(ZDLE); xsendline(frameend);
	crc = UPDC32(frameend, crc);

	crc = ~crc;
	for (length=4; --length >= 0;) {
		zsendline((int)crc);  crc >>= 8;
	}
}

/*
 * Receive array buf of max length with ending ZDLE sequence
 *  and CRC.  Returns the ending character or error code.
 *  NB: On errors may store length+1 bytes!
 */
int zrdata(char* buf, int length)
{
	register int c;
	register unsigned short crc;
	register char *end;
	register int d;

	if (Rxframeind == ZBIN32)
		return zrdat32(buf, length);

	crc = Rxcount = 0;  end = buf + length;
	while (buf <= end) {
		if ((c = zdlread()) & ~0377) {
crcfoo:
			switch (c) {
			case GOTCRCE:
			case GOTCRCG:
			case GOTCRCQ:
			case GOTCRCW:
				crc = updcrc((d=c)&0377, crc);
				if ((c = zdlread()) & ~0377)
					goto crcfoo;
				crc = updcrc(c, crc);
				if ((c = zdlread()) & ~0377)
					goto crcfoo;
				crc = updcrc(c, crc);
				if (crc & 0xFFFF) {
					zperr("Bad data CRC");
					return ERROR;
				}
				Rxcount = length - (end - buf);
				vfile("zrdata: cnt = %d ret = %x", Rxcount, d);
				return d;
			case GOTCAN:
				zperr("Sender Canceled");
				return ZCAN;
			case TIMEOUT:
				zperr("TIMEOUT");
				return c;
			default:
				zperr("Bad data subpacket");
				return c;
			}
		}
		*buf++ = c;
		crc = updcrc(c, crc);
	}
	zperr("Data subpacket too long");
	return ERROR;
}

int zrdat32(char* buf, int length)
{
	register int c;
	register unsigned long crc;
	register char *end;
	register int d;

	crc = 0xFFFFFFFFL;  Rxcount = 0;  end = buf + length;
	while (buf <= end) {
		if ((c = zdlread()) & ~0377) {
crcfoo:
			switch (c) {
			case GOTCRCE:
			case GOTCRCG:
			case GOTCRCQ:
			case GOTCRCW:
				d = c;  c &= 0377;
				crc = UPDC32(c, crc);
				if ((c = zdlread()) & ~0377)
					goto crcfoo;
				crc = UPDC32(c, crc);
				if ((c = zdlread()) & ~0377)
					goto crcfoo;
				crc = UPDC32(c, crc);
				if ((c = zdlread()) & ~0377)
					goto crcfoo;
				crc = UPDC32(c, crc);
				if ((c = zdlread()) & ~0377)
					goto crcfoo;
				crc = UPDC32(c, crc);
				if (crc != 0xDEBB20E3) {
					zperr("Bad data CRC");
					return ERROR;
				}
				Rxcount = length - (end - buf);
				vfile("zrdat32: cnt = %d ret = %x", Rxcount, d);
				return d;
			case GOTCAN:
				zperr("Sender Canceled");
				return ZCAN;
			case TIMEOUT:
				zperr("TIMEOUT");
				return c;
			default:
				zperr("Bad data subpacket");
				return c;
			}
		}
		*buf++ = c;
		crc = UPDC32(c, crc);
	}
	zperr("Data subpacket too long");
	return ERROR;
}


/*
 * Read a ZMODEM header to hdr, either binary or hex.
 *  eflag controls local display of non zmodem characters:
 *	0:  no display
 *	1:  display printing characters only
 *	2:  display all non ZMODEM characters
 *  On success, set Zmodem to 1, set Rxpos and return type of header.
 *   Otherwise return negative on error.
 *   Return ERROR instantly if ZCRCW sequence, for fast error recovery.
 */
int zgethdr(char* hdr, int eflag)
{
	register int c, n, cancount;

	n = Zrwindow + Baudrate;	/* Max bytes before start of frame */
	Rxframeind = Rxtype = 0;

startover:
	cancount = 5;
again:
	/* Return immediate ERROR if ZCRCW sequence seen */
	switch (c = readline(Rxtimeout)) {
	case RCDO:
	case TIMEOUT:
		goto fifi;
	case CAN:
gotcan:
		if (--cancount <= 0) {
			c = ZCAN; goto fifi;
		}
		switch (c = readline(1)) {
		case TIMEOUT:
			goto again;
		case ZCRCW:
			c = ERROR;
		/* **** FALL THRU TO **** */
		case RCDO:
			goto fifi;
		default:
			break;
		case CAN:
			if (--cancount <= 0) {
				c = ZCAN; goto fifi;
			}
			goto again;
		}
	/* **** FALL THRU TO **** */
	default:
agn2:
		if ( --n == 0) {
			zperr("Garbage count exceeded");
			return(ERROR);
		}
		if (eflag && ((c &= 0177) & 0140))
			bttyout(c);
		else if (eflag > 1)
			bttyout(c);
		goto startover;
	case ZPAD|0200:		/* This is what we want. */
	case ZPAD:		/* This is what we want. */
		break;
	}
	cancount = 5;
splat:
	switch (c = noxrd7()) {
	case ZPAD:
		goto splat;
	case RCDO:
	case TIMEOUT:
		goto fifi;
	default:
		goto agn2;
	case ZDLE:		/* This is what we want. */
		break;
	}

	switch (c = noxrd7()) {
	case RCDO:
	case TIMEOUT:
		goto fifi;
	case ZBIN:
		Rxframeind = ZBIN;  Crc32 = FALSE;
		c =  zrbhdr(hdr);
		break;
	case ZBIN32:
		Crc32 = Rxframeind = ZBIN32;
		c =  zrbhdr32(hdr);
		break;
	case ZHEX:
		Rxframeind = ZHEX;  Crc32 = FALSE;
		c =  zrhhdr(hdr);
		break;
	case CAN:
		goto gotcan;
	default:
		goto agn2;
	}
	Rxpos = hdr[ZP3] & 0377;
	Rxpos = (Rxpos<<8) + (hdr[ZP2] & 0377);
	Rxpos = (Rxpos<<8) + (hdr[ZP1] & 0377);
	Rxpos = (Rxpos<<8) + (hdr[ZP0] & 0377);
fifi:
	switch (c) {
	case GOTCAN:
		c = ZCAN;
	/* **** FALL THRU TO **** */
	case ZNAK:
	case ZCAN:
	case ERROR:
	case TIMEOUT:
	case RCDO:
		zperr("Got %s", frametypes[c+FTOFFSET]);
	/* **** FALL THRU TO **** */
	default:
		if (c >= -3 && c <= FRTYPES) {
			vfile("zgethdr: %s %lx", frametypes[c+FTOFFSET], Rxpos);
		} else {
			vfile("zgethdr: %d %lx", c, Rxpos);
		}
	}
	return c;
}

/* Receive a binary style header (type and position) */
int zrbhdr(char* hdr)
{
	register int c, n;
	register unsigned short crc;

	if ((c = zdlread()) & ~0377)
		return c;
	Rxtype = c;
	crc = updcrc(c, 0);

	for (n=4; --n >= 0; ++hdr) {
		if ((c = zdlread()) & ~0377)
			return c;
		crc = updcrc(c, crc);
		*hdr = c;
	}
	if ((c = zdlread()) & ~0377)
		return c;
	crc = updcrc(c, crc);
	if ((c = zdlread()) & ~0377)
		return c;
	crc = updcrc(c, crc);
	if (crc & 0xFFFF) {
		zperr("Bad Header CRC"); return ERROR;
	}
	Zmodem = 1;
	return Rxtype;
}

/* Receive a binary style header (type and position) with 32 bit FCS */
int zrbhdr32(char* hdr)
{
	register int c, n;
	register unsigned  long crc;

	if ((c = zdlread()) & ~0377)
		return c;
	Rxtype = c;
	crc = 0xFFFFFFFFL; crc = UPDC32(c, crc);
#ifdef DEBUGZ
	vfile("zrbhdr32 c=%X  crc=%lX", c, crc);
#endif

	for (n=4; --n >= 0; ++hdr) {
		if ((c = zdlread()) & ~0377)
			return c;
		crc = UPDC32(c, crc);
		*hdr = c;
#ifdef DEBUGZ
		vfile("zrbhdr32 c=%X  crc=%lX", c, crc);
#endif
	}
	for (n=4; --n >= 0;) {
		if ((c = zdlread()) & ~0377)
			return c;
		crc = UPDC32(c, crc);
#ifdef DEBUGZ
		vfile("zrbhdr32 c=%X  crc=%lX", c, crc);
#endif
	}
	if (crc != 0xDEBB20E3) {
		zperr("Bad Header CRC"); return ERROR;
	}
	Zmodem = 1;
	return Rxtype;
}


/* Receive a hex style header (type and position) */
int zrhhdr(char* hdr)
{
	register int c;
	register unsigned short crc;
	register int n;

	if ((c = zgethex()) < 0)
		return c;
	Rxtype = c;
	crc = updcrc(c, 0);

	for (n=4; --n >= 0; ++hdr) {
		if ((c = zgethex()) < 0)
			return c;
		crc = updcrc(c, crc);
		*hdr = c;
	}
	if ((c = zgethex()) < 0)
		return c;
	crc = updcrc(c, crc);
	if ((c = zgethex()) < 0)
		return c;
	crc = updcrc(c, crc);
	if (crc & 0xFFFF) {
		zperr("Bad Header CRC"); return ERROR;
	}
	if (readline(1) == '\r')	/* Throw away possible cr/lf */
		readline(1);
	Zmodem = 1; return Rxtype;
}

/* Send a byte as two hex digits */
void zputhex(int c)
{
	static char	digits[]	= "0123456789abcdef";

	if (Verbose>8)
		vfile("zputhex: %02X", c);
	sendline(digits[(c&0xF0)>>4]);
	sendline(digits[(c)&0xF]);
}

/*
 * Send character c with ZMODEM escape sequence encoding.
 *  Escape XON, XOFF. Escape CR following @ (Telenet net escape)
 */
void zsendline(int c)
{

	switch (c &= 0377) {
	case ZDLE:
		xsendline(ZDLE);
		xsendline (lastsent = (c ^= 0100));
		break;
	case 015:
	case 0215:
		if (!Zctlesc && (lastsent & 0177) != '@')
			goto sendit;
	/* **** FALL THRU TO **** */
	case 020:
	case 021:
	case 023:
	case 0220:
	case 0221:
	case 0223:
		xsendline(ZDLE);
		c ^= 0100;
sendit:
		xsendline(lastsent = c);
		break;
	default:
		if (Zctlesc && ! (c & 0140)) {
			xsendline(ZDLE);
			c ^= 0100;
		}
		xsendline(lastsent = c);
	}
}

/* Decode two lower case hex digits into an 8 bit byte value */
int zgethex(void)
{
	register int c;

	c = zgeth1();
	if (Verbose>8)
		vfile("zgethex: %02X", c);
	return c;
}
int zgeth1(void)
{
	register int c, n;

	if ((c = noxrd7()) < 0)
		return c;
	n = c - '0';
	if (n > 9)
		n -= ('a' - ':');
	if (n & ~0xF)
		return ERROR;
	if ((c = noxrd7()) < 0)
		return c;
	c -= '0';
	if (c > 9)
		c -= ('a' - ':');
	if (c & ~0xF)
		return ERROR;
	c += (n<<4);
	return c;
}

/*
 * Read a byte, checking for ZMODEM escape encoding
 *  including CAN*5 which represents a quick abort
 */
int zdlread(void)
{
	register int c;

again:
	switch (c = readline(Rxtimeout)) {
	case ZDLE:
		break;
	case 023:
	case 0223:
	case 021:
	case 0221:
		goto again;
	default:
		if (Zctlesc && !(c & 0140)) {
			goto again;
		}
		return c;
	}
again2:
	if ((c = readline(Rxtimeout)) < 0)
		return c;
	if (c == CAN && (c = readline(Rxtimeout)) < 0)
		return c;
	if (c == CAN && (c = readline(Rxtimeout)) < 0)
		return c;
	if (c == CAN && (c = readline(Rxtimeout)) < 0)
		return c;
	switch (c) {
	case CAN:
		return GOTCAN;
	case ZCRCE:
	case ZCRCG:
	case ZCRCQ:
	case ZCRCW:
		return (c | GOTOR);
	case ZRUB0:
		return 0177;
	case ZRUB1:
		return 0377;
	case 023:
	case 0223:
	case 021:
	case 0221:
		goto again2;
	default:
		if (Zctlesc && ! (c & 0140)) {
			goto again2;
		}
		if ((c & 0140) ==  0100)
			return (c ^ 0100);
		break;
	}
	zperr("Bad escape sequence %x", c);
	return ERROR;
}

/*
 * Read a character from the modem line with timeout.
 *  Eat parity, XON and XOFF characters.
 */
int noxrd7(void)
{
	register int c;

	for (;;) {
		if ((c = readline(Rxtimeout)) < 0)
			return c;
		switch (c &= 0177) {
		case XON:
		case XOFF:
			continue;
		default:
			if (Zctlesc && !(c & 0140))
				continue;
		case '\r':
		case '\n':
		case ZDLE:
			return c;
		}
	}
}

/* Store long integer pos in Txhdr */
void stohdr(long pos)
{
	Txhdr[ZP0] = pos;
	Txhdr[ZP1] = pos>>8;
	Txhdr[ZP2] = pos>>16;
	Txhdr[ZP3] = pos>>24;
}

/* Recover a long integer from a header */
long rclhdr(char* hdr)
{
	register long l;

	l = (hdr[ZP3] & 0377);
	//printf("rclhdr: %p\r\n", l);
	l = (l << 8) | (hdr[ZP2] & 0377);
	//printf("rclhdr: %p\r\n", l);
	l = (l << 8) | (hdr[ZP1] & 0377);
	//printf("rclhdr: %p\r\n", l);
	l = (l << 8) | (hdr[ZP0] & 0377);
	//printf("rclhdr: %p\r\n", l);
	return l;
}



/*
 * Let's receive something already.
 */


int wcreceive(int attempts, int maxbytes, char *filename)
{
	ZModemMaxBytes = maxbytes;
	ZModemPathname = filename;

	// Allocate buffers on stack
	secbuf = alloca(KSIZE+1);
	linbuf = alloca(HOWMANY);
	Attn = alloca(ZATTNLEN+1);

	// Clear context
	Lastrx = 0;
	Crcflg = 0;
	Firstsec = 0;
	Eofseen = 0;
	Zmodem = 0;
	Errors = 0;
	Rxtimeout = 1;
	ZModemRxBytes = 0;
	ZModemBytesleft = 0;
	Rxbinary=TRUE;
	Rxascii=FALSE;
	Thisbinary = 0;
	Blklen = 0;
	Lleft=0;
	Lzmanag = 0;
	zconv = 0;
	zmanag = 0;
	ztrans = 0;
	Zctlesc = 0;
	Zrwindow = 1400;
	lastsent = 0;
	tryzhdrtype=ZRINIT;
	ZModemRxOffset = 0;

	memset(secbuf, 0, KSIZE+1);
	
	purgeline();


	register int c;


		Crcflg=(Wcsmask==0377);

		// Try to handshake
		c=tryz(attempts);

		if (c) {
			if (c == ZCOMPL)
				return OK;
			if (c == ERROR)
				goto fubar;
			if (c == TIMEOUT)
				return TIMEOUT;

			// Start receiving file
			c = rzfiles(attempts);
			if (c)
				goto fubar;
		} 

		return OK;
/*
		else {
			for (;;) {
				if (wcrxpn(secbuf)== ERROR)
					goto fubar;
				if (secbuf[0]==0)
					return OK;
				if (procheader(secbuf) == ERROR)
					goto fubar;
				if (wcrx()==ERROR)
					goto fubar;
			}
		}
*/


fubar:
	canit();

	return ERROR;
}


/*
 * Fetch a pathname from the other end as a C ctyle ASCIZ string.
 * Length is indeterminate as long as less than Blklen
 * A null string represents no more files (YMODEM)
 */
int wcrxpn(char* rpn) 	/* receive a pathname */
{
	register int c;

#ifdef NFGVMIN
	readline(1);
#else
	purgeline();
#endif

et_tu:
	Firstsec=TRUE;  Eofseen=FALSE;
	sendline(Crcflg?WANTCRC:NAK);
	Lleft=0;	/* Do read next time ... */
	while ((c = wcgetsec(rpn, 10)) != 0) {
		if (c == WCEOT) {
			zperr( "Pathname fetch returned erro %d", c);
			sendline(ACK);
			Lleft=0;	/* Do read next time ... */
			readline(1);
			goto et_tu;
		}
		return ERROR;
	}
	sendline(ACK);
	return OK;
}

/*
 * Adapted from CMODEM13.C, written by
 * Jack M. Wierda and Roderick W. Hart
 */

int wcrx(void)
{
	register int sectnum, sectcurr;
	register char sendchar;
	register char *p;
	int cblklen;			/* bytes to dump this block */

	Firstsec=TRUE;sectnum=0; Eofseen=FALSE;
	sendchar=Crcflg?WANTCRC:NAK;

	for (;;) {
		sendline(sendchar);	/* send it now, we're ready! */
		Lleft=0;	/* Do read next time ... */
		sectcurr=wcgetsec(secbuf, (sectnum&0177)?5:10);
		report(sectcurr);
		if (sectcurr==(sectnum+1 &Wcsmask)) {
			sectnum++;
			cblklen = ZModemBytesleft>Blklen ? Blklen:ZModemBytesleft;
			if (putsec(secbuf, cblklen)==ERROR)
				return ERROR;
			if ((ZModemBytesleft-=cblklen) < 0)
				ZModemBytesleft = 0;
			sendchar=ACK;
		}
		else if (sectcurr==(sectnum&Wcsmask)) {
			zperr( "Received dup Sector");
			sendchar=ACK;
		}
		else if (sectcurr==WCEOT) {
			if (closeit())
				return ERROR;
			sendline(ACK);
			Lleft=0;	/* Do read next time ... */
			return OK;
		}
		else if (sectcurr==ERROR)
			return ERROR;
		else {
			zperr( "Sync Error");
			return ERROR;
		}
	}
}

/*
 * Wcgetsec fetches a Ward Christensen type sector.
 * Returns sector number encountered or ERROR if valid sector not received,
 * or CAN CAN received
 * or WCEOT if eot sector
 * time is timeout for first char, set to 4 seconds thereafter
 ***************** NO ACK IS SENT IF SECTOR IS RECEIVED OK **************
 *    (Caller must do that when he is good and ready to get next sector)
 */

int wcgetsec(char* rxbuf, int maxtime)
{
	register int checksum, wcj, firstch;
	register unsigned short oldcrc;
	register char *p;
	int sectcurr;

	for (Lastrx=Errors=0; Errors<RETRYMAX; Errors++) {

		if ((firstch=readline(maxtime))==STX) {
			Blklen=KSIZE; goto get2;
		}
		if (firstch==SOH) {
			Blklen=SECSIZ;
get2:
			sectcurr=readline(1);
			if ((sectcurr+(oldcrc=readline(1)))==Wcsmask) {
				oldcrc=checksum=0;
				for (p=rxbuf,wcj=Blklen; --wcj>=0; ) {
					if ((firstch=readline(1)) < 0)
						goto bilge;
					oldcrc=updcrc(firstch, oldcrc);
					checksum += (*p++ = firstch);
				}
				if ((firstch=readline(1)) < 0)
					goto bilge;
				if (Crcflg) {
					oldcrc=updcrc(firstch, oldcrc);
					if ((firstch=readline(1)) < 0)
						goto bilge;
					oldcrc=updcrc(firstch, oldcrc);
					if (oldcrc & 0xFFFF)
						zperr( "CRC");
					else {
						Firstsec=FALSE;
						return sectcurr;
					}
				}
				else if (((checksum-firstch)&Wcsmask)==0) {
					Firstsec=FALSE;
					return sectcurr;
				}
				else
					zperr( "Checksum");
			}
			else
				zperr("Sector number garbled");
		}
		/* make sure eot really is eot and not just mixmash */
#ifdef NFGVMIN
		else if (firstch==EOT && readline(1)==TIMEOUT)
			return WCEOT;
#else
		else if (firstch==EOT && Lleft==0)
			return WCEOT;
#endif
		else if (firstch==CAN) {
			if (Lastrx==CAN) {
				zperr( "Sender CANcelled");
				return ERROR;
			} else {
				Lastrx=CAN;
				continue;
			}
		}
		else if (firstch==TIMEOUT) {
			if (Firstsec)
				goto humbug;
bilge:
			zperr( "TIMEOUT");
		}
		else
			zperr( "Got 0%o sector header", firstch);

humbug:
		Lastrx=0;
		while(readline(1)!=TIMEOUT)
			;
		if (Firstsec) {
			sendline(Crcflg?WANTCRC:NAK);
			Lleft=0;	/* Do read next time ... */
		} else {
			maxtime=40; sendline(NAK);
			Lleft=0;	/* Do read next time ... */
		}
	}
	/* try to stop the bubble machine. */
	canit();
	return ERROR;
}

/*
 * This version of readline is reasoably well suited for
 * reading many characters.
 *  (except, currently, for the Regulus version!)
 *
 * timeout is in seconds
 */
int readline(int timeout)
{
	int n;
	static char *cdq;	// pointer for removing chars from linbuf 

	if (--Lleft >= 0) {
		if (Verbose > 8) {
			printf("%02x ", *cdq&0377);
		}
		return (*cdq++ & Wcsmask);
	}

	cdq = linbuf;

	//printf("_read() before: %d\r\n", timeout);

	Lleft = uart_read(UART0, cdq, Readnum, timeout*1000);

	//printf("_read() = %d\n", Lleft);


	if(Lleft > 0) {
		--Lleft;
		return (*cdq++ & Wcsmask);

	} else {
		if (Verbose>1)
			printf("ZModem: readline() TIMEOUT = %d\n", timeout);
		return TIMEOUT;
	}

}



/*
 * Purge the modem input queue of all characters
 */
void purgeline(void)
{
	Lleft = 0;
	volatile char c;
	while(uart_readOccupancy(UART0))
		c = UART0->DATA;
}


/*
 * Process incoming file information header
 */
int procheader(char *name)
{
	char *p, **pp;

	Thisbinary = (!Rxascii) || Rxbinary;
	if (Lzmanag)
		zmanag = Lzmanag;

	/*
	 *  Process ZMODEM remote file management requests
	 */
	if (!Rxbinary && zconv == ZCNL)	/* Remote ASCII override */
		Thisbinary = 0;
	if (zconv == ZCBIN)	/* Remote Binary override */
		Thisbinary = TRUE;

#ifndef BIX
	/* ZMPROT check for existing file */
	//if (zmanag == ZMPROT && 1 /* check existing file */) {
	//	return ERROR;
	//}
#endif

	ZModemBytesleft = DEFBYTL; Filemode = 0777; Modtime = 0L;

	p = name + 1 + strlen(name);
	if (*p) {	/* file coming from Unix or DOS system */
		sscanf(p, "%ld%lo%o", &ZModemBytesleft, &Modtime, &Filemode);
		if (Filemode & UNIXFILE)
			++Thisbinary;
		if (Verbose) {
			printf("Incoming: %s %ld %lo %o\n",
			  name, ZModemBytesleft, Modtime, Filemode);
		}
	} else {		/* File coming from CP/M system */
		for (p=name; *p; ++p)		/* change / to _ */
			if ( *p == '/')
				*p = '_';

		if ( *--p == '.')		/* zap trailing period */
			*p = 0;
	}

	if (!Zmodem && MakeLCPathname && !IsAnyLower(name))
		uncaps(name);
	if (Topipe) {
		sprintf(ZModemPathname, "%s %s", Progname+2, name);
		if (Verbose)
			printf("Topipe: %s %s\n",
			  ZModemPathname, Thisbinary?"BIN":"ASCII");
	} else {
		strcpy(ZModemPathname, name);
		if (Verbose) {
			printf("Receiving %s %s\n",
			  name, Thisbinary?"BIN":"ASCII");
		}
		//checkpath(name);
	}

	if(ZModemBytesleft > ZModemMaxBytes) {
		printf("File too long, %d bytes is max allowed!\r\n", ZModemMaxBytes); 
		return ERROR;
	}

	return OK;
}

/*
 * Putsec writes the n characters of buf to receive file fout.
 *  If not in binary mode, carriage returns, and all characters
 *  starting with CPMEOF are discarded.
 */
int putsec(char* buf, int n)
{
	register char *p;

	if (Thisbinary) {
		//printf("=== Received binary: %d bytes\n", n);
		memcpy(ZModemWriteAddress + ZModemRxOffset, buf, n);
		ZModemRxOffset += n;
	} else {
		if (Eofseen)
			return OK;
		for (p=buf; --n>=0; ++p ) {
			if ( *p == '\r')
				continue;
			if (*p == CPMEOF) {
				Eofseen=TRUE; return OK;
			}
			//printf("=== Received text: %d bytes\n", n);
			memcpy(ZModemWriteAddress + ZModemRxOffset, buf, n);
			ZModemRxOffset += n;
		}
	}
	return OK;
}

/*
 *  Send a character to modem.  Small is beautiful.
 */
void sendline(int c)
{
	uart_write(UART0, c);
}

void xsendline(register int c)
{
	sendline(c);
}

void flushmo(void) 
{
	//_flush(ZModemUSART);
}




/* make string s lower case */
void uncaps(char* s)
{
	for ( ; *s; ++s)
		if (isupper(*s))
			*s = tolower(*s);
}
/*
 * IsAnyLower returns TRUE if string s has lower case letters.
 */
int IsAnyLower(char* s)
{
	for ( ; *s; ++s)
		if (islower(*s))
			return TRUE;
	return FALSE;
}

/*
 * substr(string, token) searches for token in string s
 * returns pointer to token within string if found, NULL otherwise
 */
char * substr(register char *s, char *t)
{
	register char *ss,*tt;
	/* search for first char of token */
	for (ss=s; *s; s++)
		if (*s == *t)
			/* compare token with substring */
			for (ss=s,tt=t; ;) {
				if (*tt == 0)
					return s;
				if (*ss++ != *tt++)
					break;
			}
	return NULL;
}

/* send cancel string to get the other end to shut up */
void canit(void)
{
	static char canistr[] = {
	 24,24,24,24,24,24,24,24,24,24,8,8,8,8,8,8,8,8,8,8,0
	};

	for(int i = 0; i < sizeof(canistr); i++)
		uart_write(UART0, canistr[i]);

	Lleft=0;	/* Do read next time ... */
}


/*
 * Return 1 iff stdout and stderr are different devices
 *  indicating this program operating with a modem on a
 *  different line
 */
int fromcu()
{
	return 0;
}

void report(int sct)
{
	if (Verbose>1)
		printf("Progress: %03d%c",sct,sct%10? ' ' : '\r');
}


/*
 * Initialize for Zmodem receive attempt, try to activate Zmodem sender
 *  Handles ZSINIT frame
 *  Return ZFILE if Zmodem filename received, -1 on error,
 *   ZCOMPL if transaction finished,  else 0
 */
int tryz(int attempts)
{
	int c, n;
	int cmdzack1flg;

	//if (Nozmodem)		/* Check for "rb" program name */
	//	return 0;


	for (n=attempts; --n>=0; ) {
		/* Set buffer length (0) and capability flags */
		stohdr(0L);
#ifdef CANBREAK
		Txhdr[ZF0] = CANFC32|CANFDX|CANOVIO|CANBRK;
#else
		Txhdr[ZF0] = CANFC32|CANFDX|CANOVIO;
#endif
		if (Zctlesc)
			Txhdr[ZF0] |= TESCCTL;
		zshhdr(tryzhdrtype, Txhdr);
		if (tryzhdrtype == ZSKIP)	/* Don't skip too far */
			tryzhdrtype = ZRINIT;	/* CAF 8-21-87 */
again:
		switch (zgethdr(Rxhdr, 0)) {
		case ZRQINIT:
			continue;
		case ZEOF:
			continue;
		case TIMEOUT:
			continue;
		case ZFILE:
			zconv = Rxhdr[ZF0];
			zmanag = Rxhdr[ZF1];
			ztrans = Rxhdr[ZF2];
			tryzhdrtype = ZRINIT;
			c = zrdata(secbuf, KSIZE);
			//mode(3);
			if (c == GOTCRCW)
				return ZFILE;
			zshhdr(ZNAK, Txhdr);
			goto again;
		case ZSINIT:
			Zctlesc = TESCCTL & Rxhdr[ZF0];
			if (zrdata(Attn, ZATTNLEN) == GOTCRCW) {
				zshhdr(ZACK, Txhdr);
				goto again;
			}
			zshhdr(ZNAK, Txhdr);
			goto again;
		case ZFREECNT:
			stohdr(getfree());
			zshhdr(ZACK, Txhdr);
			goto again;
		case ZCOMMAND:
			cmdzack1flg = Rxhdr[ZF0];
			if (zrdata(secbuf, KSIZE) == GOTCRCW) {
				if (cmdzack1flg & ZCACK1)
					stohdr(0L);
				else
					//stohdr((long)sys2(secbuf));
					stohdr(0L);
				purgeline();	/* dump impatient questions */
				do {
					zshhdr(ZCOMPL, Txhdr);
				}
				while (++Errors<20 && zgethdr(Rxhdr,1) != ZFIN);
				ackbibi();
				//if (cmdzack1flg & ZCACK1)
				//	exec2(secbuf);
				return ZCOMPL;
			}
			zshhdr(ZNAK, Txhdr); goto again;
		case ZCOMPL:
			goto again;
		default:
			continue;
		case ZFIN:
			ackbibi(); return ZCOMPL;
		case ZCAN:
			return ERROR;
		}
	}
	return TIMEOUT;
}

/*
 * Receive 1 or more files with ZMODEM protocol
 */
int rzfiles(int attempts)
{
	register int c;

	int i;

	for (i = 0; i < 5; i++) {
		c = rzfile();
		switch (c) {
		case ZEOF:
		case ZSKIP:
			switch (tryz(attempts)) {
			case ZCOMPL:
				return OK;
			default:
				return ERROR;
			case ZFILE:
				break;
			}
			continue;
		case ZABORT:
			return ERROR;
		default:
			return c;
		case ERROR:
			return ERROR;
		}
	}

	return ERROR;
}

/*
 * Receive a file with ZMODEM protocol
 *  Assumes file name frame is in secbuf
 */
int rzfile(void)
{
	int c, n;


	Eofseen=FALSE;

	if (procheader(secbuf) == ERROR) {
		//return (tryzhdrtype = ZSKIP);
		return (tryzhdrtype = ZABORT);
	}

	n = 20; 

	for (;;) {
		stohdr(ZModemRxBytes);
		zshhdr(ZRPOS, Txhdr);
nxthdr:
		switch (c = zgethdr(Rxhdr, 0)) {
		default:
			vfile("rzfile: zgethdr returned %d", c);
			return ERROR;
		case ZNAK:
		case TIMEOUT:
			if ( --n < 0) {
				vfile("rzfile: zgethdr returned %d", c);
				return ERROR;
			}
		case ZFILE:
			zrdata(secbuf, KSIZE);
			continue;
		case ZEOF:
			if (rclhdr(Rxhdr) != ZModemRxBytes) {
				/*
				 * Ignore eof if it's at wrong place - force
				 *  a timeout because the eof might have gone
				 *  out before we sent our zrpos.
				 */
				Errors = 0;  goto nxthdr;
			}
			if (closeit()) {
				tryzhdrtype = ZFERR;
				vfile("rzfile: closeit returned <> 0");
				return ERROR;
			}
			vfile("rzfile: normal EOF");
			return c;
		case ERROR:	/* Too much garbage in header search error */
			if ( --n < 0) {
				vfile("rzfile: zgethdr returned %d", c);
				return ERROR;
			}
			zmputs(Attn);
			continue;
		case ZDATA:
			if (rclhdr(Rxhdr) != ZModemRxBytes) {
				if ( --n < 0) {
					return ERROR;
				}
				zmputs(Attn);  continue;
			}
moredata:
			if (Verbose>1)
				printf("\r%7ld ZMODEM%s    ",
				  ZModemRxBytes, Crc32?" CRC-32":"");
			switch (c = zrdata(secbuf, KSIZE)) {
			case ZCAN:
				vfile("rzfile: zgethdr returned %d", c);
				return ERROR;
			case ERROR:	/* CRC error */
				if ( --n < 0) {
					vfile("rzfile: zgethdr returned %d", c);
					return ERROR;
				}
				zmputs(Attn);
				continue;
			case TIMEOUT:
				if ( --n < 0) {
					vfile("rzfile: zgethdr returned %d", c);
					return ERROR;
				}
				continue;
			case GOTCRCW:
				n = 20;
				putsec(secbuf, Rxcount);
				ZModemRxBytes += Rxcount;
				stohdr(ZModemRxBytes);
				zshhdr(ZACK, Txhdr);
				sendline(XON);
				goto nxthdr;
			case GOTCRCQ:
				n = 20;
				putsec(secbuf, Rxcount);
				ZModemRxBytes += Rxcount;
				stohdr(ZModemRxBytes);
				zshhdr(ZACK, Txhdr);
				goto moredata;
			case GOTCRCG:
				n = 20;
				putsec(secbuf, Rxcount);
				ZModemRxBytes += Rxcount;
				goto moredata;
			case GOTCRCE:
				n = 20;
				putsec(secbuf, Rxcount);
				ZModemRxBytes += Rxcount;
				goto nxthdr;
			}
		}
	}
}

/*
 * Send a string to the modem, processing for \336 (sleep 1 sec)
 *   and \335 (break signal)
 */
void zmputs(char *s)
{
	int c;

	while (*s) {
		switch (c = *s++) {
		case '\336':
			delay_us(1000000); continue;
		case '\335':
			sendbrk(); continue;
		default:
			sendline(c);
		}
	}
}

/*
 * Close the receive dataset, return OK or ERROR
 */
int closeit(void)
{
	return OK;
}

/*
 * Ack a ZFIN packet, let byegones be byegones
 */
void ackbibi(void)
{
	int n;

	vfile("ackbibi:");
	Readnum = 1;
	stohdr(0L);
	for (n=3; --n>=0; ) {
		purgeline();
		zshhdr(ZFIN, Txhdr);
		switch (readline(3)) {
		case 'O':
			readline(1);	/* Discard 2nd 'O' */
			vfile("ackbibi complete");
			return;
		case RCDO:
			return;
		case TIMEOUT:
		default:
			break;
		}
	}
}



/*
 * Local console output simulation
 */
void bttyout(char c)
{
	//putc(c, stderr);
}

/* End of rz.c */


/*************************************************************************
*
*	@(#)toapple2.c	1.2
*	8/17/88
*
*************************************************************************/

/* send a file to the Apple II. This prog transmits a file via a serial
   port to the AppleII's 'Super Serial Card'.
   Due to some hassle with receiving data from the SSC, this proggy
   expects a fixed response in reply to its checksum rather than
   the remote system's idea of the checksum.
   A byte is sent as 2 4-bit bytes due to Apple's inherant protocols.
   0's are 'doubled' so that any zero transmitted is sent as 0 0
   except for packet headers which consist of a 0 followed by the
   packet number.
   XON/XOFF signals from Apple are coped with but are sent at known
   points in the transfer of packets.
   See 'getimage2.asm' which is the receiving end running on the AppleII
   for further description of protocol etc.

   Paul 16/8/88

*/

#include <stdio.h>
#include <sgtty.h>
#include <sys/file.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>

struct sgttyb oldtty, newtty;
int out;
FILE *out_fp;
int packetnumber;
int checktotal=0;
static jmp_buf sjbuf;

#define _send_char(c)	putc((c), out_fp)
#define send_char(c)	{ _send_char(c & 0x0f); _send_char((c>>4) & 0x0f); }

main(argc, argv)
int argc;
char **argv;
{
    int i, fd, packetsize;
    unsigned char buf[256];
    unsigned char Rxcksumlo, Rxcksumhi;
    int checksum,RxChecksum;
    int timera();
    int timerb();

    if (argc != 3) {
	printf("Usage: toapple2 file /dev/ttyxx\n");
	exit(-1);
    }
    out = open(argv[2], O_RDWR);
    if (out == -1) {
	perror(argv[2]);
	exit(-1);
    }
    out_fp = fdopen(out, "w");
    if (!out_fp) {
	fprintf(stderr, "help! fdopen returned NULL\n");
	exit(-1);
    }
    gtty(out, &oldtty);
    newtty = oldtty;
    newtty.sg_flags |= CBREAK;
    newtty.sg_flags &= ~ECHO;
    stty(out, &newtty);
    signal(SIGALRM, timera);
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
	perror(argv[1]);
	exit(-1);
    }
    packetsize = 256;
    for (packetnumber = 1; packetsize == 256; packetnumber++) {
	if ((packetnumber&0xff)==0)
		packetnumber=1;

	packetsize = read(fd, buf, 256);
	if (packetsize==0) {
		_send_char(0x7f);			/* indicate party over */
    	close(fd);
    	stty(out, &oldtty);
		printf("Transfer complete. Checksum is %x\n",checktotal&0xffff);
    	exit(0);
			}

retry:
	printf ("Sending packet: %d\n",packetnumber&0xff);
	send_char(0);						/* packet hdr */
	send_char(packetnumber&0xff);		/*		      */

	for (i=0, checksum=0; i<packetsize; i++)
	    checksum += buf[i];				/* calc chksum */
		checktotal+=checksum;
	for (i=0; i<packetsize; i++) {
	    send_char(buf[i]);				/* send data packet */
		if (buf[i]==0)
			send_char(0);
			}

	send_char(checksum&0xff);
	if ((checksum & 0xff)==0)
		send_char(0);
	send_char(((checksum >> 8) & 0xff));
	if (((checksum>>8)&0xff)==0)
		send_char(0);

	fflush(out_fp);		/* make sure the block is sent */

	Rxcksumlo=ttinc(1);
	Rxcksumhi=ttinc(1);

	if ((Rxcksumlo!=0x3) || (Rxcksumhi!=0x17)) {
		goto retry;
		}
/*
	RxChecksum=(Rxcksumlo|((Rxcksumhi << 8)&0xff00));
	if (RxChecksum!=checksum) {
		printf("Checksum error: %x expected, %x received\n",
	       checksum, RxChecksum);
	       goto retry;
		   }
*/

    }

	_send_char(0x7f);			/* indicate party over */
    close(fd);
    stty(out, &oldtty);
	printf("Transfer complete. Checksum is %x\n",checktotal&0xffff);
    exit(0);
}

setsignal(usec)
int usec;
{
    struct itimerval t;
    t.it_value.tv_sec = 0;
    t.it_value.tv_usec = 1000000/usec;	/* NB Vax resolution is 10usecs */
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &t, 0);
}

/*  T T I N C --  Read a character from the communication line  */
/*		  and wait n useconds before giving up.	*/

ttinc(timo) int timo; {
    int n = 0;
    unsigned char ch = 0;
    int timerb();

    if (timo <= 0) {			/* Untimed. */
	while ((n = read(out,&ch,1)) == 0) ; /* Wait for a character. */
	return( (n > 0) ? (ch & 0377) : n );
    }

    signal(SIGALRM,timerb);		/* Timed, set up timer. */
	alarm(timo);			/* timo seconds from now */
/*	setsignal(timo);*/		/* timo useconds from now */
    if (setjmp(sjbuf)) {		/* stash so we can get back */
	n = -1;
    } else {
    	n = read(out,&ch,1);		/* Otherwise call the system. */

    }
    alarm(0);				/* Turn off timer, */
    signal(SIGALRM,SIG_DFL);		/* and interrupt. */
    return( (n > 0) ? (ch & 0377) : n ); /* Return char or -1. */
}


timera()
{}

timerb() {
    longjmp(sjbuf,1);
}


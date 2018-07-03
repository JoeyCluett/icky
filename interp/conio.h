#ifndef __iccc_conio_hpp__
#define __iccc_conio_hpp__

#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/time.h>

/* reads from keypress, doesn't echo */
int getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

/* reads from keypress, echoes */
int getche(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

unsigned long getUsecTimestamp(void) {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + 1000000L*tv.tv_sec;
}

#endif // __iccc_conio_hpp__

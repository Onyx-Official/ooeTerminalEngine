#include "ooeTerminalEngine.h"

int main()
{
    tE_setTerminalRawMode();
    atexit(tE_disableTerminalRawMode);

    //
    struct terminalEngine tE;
    //tE.m_nScreenWidth = 20;   tE.m_nScreenHeight = 20;
    tE_getTerminalWindowSize(&tE.m_nScreenHeight, &tE.m_nScreenWidth);
    tE.m_bufScreen = malloc(sizeof(CHAR_T) * (tE.m_nScreenWidth * tE.m_nScreenHeight));
    write(STDOUT_FILENO, "\x1b[2J\x1b[H", 7);
    Fill(&tE, 0, 0, tE.m_nScreenWidth, tE.m_nScreenHeight, ' ', 20, 0);
    WriteConsoleOutput(&tE);
    sleep(10);
    free(tE.m_bufScreen);

    return 0;
}
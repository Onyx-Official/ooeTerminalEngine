#include "ooeTerminalEngine.h"

void WriteCOUT(CHAR_T* char_t_ptr, size_t size)
{
    Vbuf vbuf = vbuf_init();

    for (size_t i = 0; i < size; i++)
        vbuf_append(&vbuf, &char_t_ptr[i].glyph, 1);

    int    prev_bg = char_t_ptr[0].bg + 1,
           prev_bg_index = 0,
           prev_fg = char_t_ptr[0].fg + 1,
           prev_fg_index = 0;

    for (size_t i = 0; i < size; i++)
    {
        if (char_t_ptr[i].bg != prev_bg)
        {
            char buf[10];	int size = 0;
/*            
            if (prev_bg == INVERSE_COLOR) {
                size = snprintf(buf, sizeof(buf), "\x1b[7m");
                vbuf_insert_cstring(&vbuf, buf, size, prev_bg_index);
            } else {*/
                size = snprintf(buf, sizeof(buf), "\x1b[38;5;%dm", char_t_ptr[prev_bg_index].bg);
                vbuf_insert_cstring(&vbuf, buf, size, prev_bg_index);
            //}

            //printf("Background -> index: %2d\tPrevious Index: %2d\tvalue: %3d\n\n", i, prev_bg_index, tE_ptr->m_bufScreen[i].attrib.bg);
            prev_bg = char_t_ptr[i].bg;
            prev_bg_index = i;
        }
/*        if (char_t_ptr[i].fg != prev_fg)
        {
            //printf("Foreground -> index: %2d\tPrevious Index: %2d\tvalue: %3d\n\n", i, prev_fg_index,tE_ptr->m_bufScreen[i].attrib.fg);
            prev_fg = char_t_ptr[i].fg;
            prev_fg_index = i;
        }*/
    }
    write(STDOUT_FILENO, vbuf.data, vbuf.size);
    vbuf_free(&vbuf);
}

void printf_vbuf(Vbuf* vbuf_ptr)
{
    for (size_t i = 0; i < vbuf_ptr->size; i++)
        printf("%c", vbuf_ptr->data[i]);
    printf("\n");
}

int main()
{
/*    tE_setTerminalRawMode();
    atexit(tE_disableTerminalRawMode);

    //
    struct terminalEngine tE;
    //tE.m_nScreenWidth = 20;   tE.m_nScreenHeight = 20;
    tE_getTerminalWindowSize(&tE.m_nScreenHeight, &tE.m_nScreenWidth);
    tE.m_bufScreen = malloc(sizeof(CHAR_T) * (tE.m_nScreenWidth * tE.m_nScreenHeight));
    write(STDOUT_FILENO, "\x1b[2J\x1b[H", 7);
    Fill(&tE, 0, 0, tE.m_nScreenWidth, tE.m_nScreenHeight, ' ', 0, 0);
    Fill(&tE, 0, 0, 5, 5, ' ', 20, 0);
    WriteConsoleOutput(&tE);
    sleep(1);
    free(tE.m_bufScreen);*/
/*
    CHAR_T char_t[10];
    char_t[0].glyph = 'a';
    char_t[0].bg  = 1;
    char_t[0].fg = 3;

    char_t[1].glyph = 'a';
    char_t[1].bg  = 1;
    char_t[1].fg = 3;
    
    char_t[2].glyph = 'a';
    char_t[2].bg  = 2;
    char_t[2].fg = 4;
    
    char_t[3].glyph = 'a';
    char_t[3].bg  = 2;
    char_t[3].fg = 4;
    
    char_t[4].glyph = 'a';
    char_t[4].bg  = 3;
    char_t[4].fg = 5;
    
    char_t[5].glyph = 'a';
    char_t[5].bg  = 3;
    char_t[5].fg = 5;
    
    char_t[6].glyph = 'a';
    char_t[6].bg  = 4;
    char_t[6].fg = 6;
    
    char_t[7].glyph = 'a';
    char_t[7].bg  = 4;
    char_t[7].fg = 6;
    
    char_t[8].glyph = 'a';
    char_t[8].bg  = 5;
    char_t[8].fg = 7;
    
    char_t[9].glyph = 'a';
    char_t[9].bg  = 5;
    char_t[9].fg = 7;
    WriteCOUT(char_t, 10);*/

    Vbuf vbuf = vbuf_init();
    vbuf_append(&vbuf, "helloworld", 10);
    printf_vbuf(&vbuf);
    vbuf_insert_cstring(&vbuf, " ", 1, 0);
    printf_vbuf(&vbuf);
    vbuf_free(&vbuf);
    return 0;
}
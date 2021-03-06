/* 
** $Id: hebrew_test.c,v 1.41 2007-12-27 04:14:30 houhuihua Exp $
**
** Listing 2.1
**
** hebrew_test.c: test hebrew text show demo, use it follow:
** ./hebrew_test hebrew_text.txt
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

//#define ARABIC_DEBUG

#ifdef ARABIC_DEBUG
    #define DBGLOG(s)      do { if (1) { fprintf(stderr, s); } } while (0)
    #define DBGLOG2(s, t1)  do { if (1) { fprintf(stderr, s, t1); } } while (0)
    #define DBGLOG3(s, t1,t2)  do { if (1) { fprintf(stderr, s, t1,t2); } } while (0)
#else
    #define DBGLOG(s)
    #define DBGLOG2(s, t1)
    #define DBGLOG3(s, t1, t2)
#endif


char linebuf[4096];
char filebuf[4096*4];
char filename[256];
char fontname[50];

BOOL textout = TRUE;
RECT arabic_rc = {0, 0, 640, 480};
int line_num = 0;

static char msg_text [256] = "Show text with TextOut.";

static FILE *fp = NULL;
static PLOGFONT logfont_iso8859_8;
//static char arabic_text[] = {0xc7,0xe4,0xe1,0xd1,0xea,0xe2,0xa0,0xc7,0xe4,0xca,0xe2,0xe6,0xea,0x00};

static int reset_fp_pos()
{
    arabic_rc.left   = 0;
    arabic_rc.top    = 0;
    arabic_rc.right  = g_rcScr.right - 24;
    arabic_rc.bottom = g_rcScr.bottom;
    if(!fp){
        DBGLOG2("============================= arabic_test %s===============================\n", filename);
        fp = fopen(filename, "rb");
        if (!fp) {
            DBGLOG2("cannot open %s\n", filename);
            return 0;
        }
    }
    if(fp){
        fseek(fp, 0, SEEK_SET);
    }
    line_num = 0;
    return 1;
}

static void print_hexstr(const unsigned char* str, int len)
{
    int m = 0;
    DBGLOG2("\n========================%d============================\n", ++line_num);
    DBGLOG("Read one line from file start.\n");
    DBGLOG("   ");
    for(m = 0; m < len; m++){
        if(m && !(m%16)) DBGLOG("\n   ");
        DBGLOG2("0x%02x ", str[m]);
    }
    DBGLOG("\nRead one line from file end.\n");
    DBGLOG("====================================================\n");
}

static char* read_one_line(BOOL* read_over)
{
    int i = 0;
    char c = 0;
    if(fp == NULL){
        *read_over = TRUE;
        return NULL;
    }
    memset(linebuf, 0, 4096);
    while(!(*read_over = feof(fp))) {
        c = fgetc(fp);
        if (c == 0xa || c == 0xd) {
            break;
        }
        linebuf[i] = c;
        i++;
    }
    if(i > 0) {
        print_hexstr((BYTE*)linebuf, i);
        return linebuf;
    }
    else{
        return NULL;
    }
}

void read_total_file( void )
{
    FILE* fp = fopen(filename, "rb");
    long pos;
    fseek (fp, 0, SEEK_END);
    pos = ftell(fp);
    fseek (fp, 0, SEEK_SET);
    fread (filebuf, pos, 1, fp);
    fclose (fp);
}

static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_CREATE:
            logfont_iso8859_8 = CreateLogFont("vbf", fontname, "ISO8859-8", FONT_WEIGHT_REGULAR,
                    FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                    FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE,
                    FONT_STRUCKOUT_NONE, 18, 0);
            strcat(msg_text, filename);
            break;
        case MSG_LBUTTONDOWN:
            textout = TRUE;
            strcpy (msg_text, "Show text with TextOut.");
            strcat(msg_text, filename);
            InvalidateRect (hWnd, NULL, TRUE);
            break;

        case MSG_MOUSEMOVE:
            {
                /*
                int x_pos = LOSWORD (lParam);
                int y_pos = HISWORD (lParam);
                DBGLOG3("mouse in <%d, %d>\n", x_pos, y_pos);
                */
            }
            break;
        case MSG_RBUTTONDOWN:
            textout = FALSE;
            strcpy (msg_text, "Show text with DrawText.");
            strcat(msg_text, filename);
            InvalidateRect (hWnd, NULL, TRUE);
            break;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            SetWindowCaption(hWnd, msg_text);
#if 1
            //SetBkColor (hdc, COLOR_green);
            SelectFont(hdc, logfont_iso8859_8);
            SetTextAlign(hdc, TA_RIGHT);
            //SetTextAlign(hdc, TA_LEFT);
            reset_fp_pos();
            //if(textout == TRUE)
            {
                char* pline = NULL;
                int i = 0;
                int line_height = GetFontHeight(hdc);
                BOOL read_over = FALSE;
                while(1){
                    pline = read_one_line(&read_over);
                    if(read_over) break;
                    if(!pline) continue;
                    
                    if(textout == TRUE) {
                        if(GetTextAlign(hdc) == TA_RIGHT)
                            TextOut(hdc, arabic_rc.right, i*line_height, pline);
                        else
                            TextOut(hdc, arabic_rc.left, i*line_height, pline);
                    }
                    else{
                        int height = DrawText(hdc, pline, -1, &arabic_rc, DT_RIGHT);
                        arabic_rc.top += height;
                    }
                    i++;
                }
            }
            /*
            else{
                read_total_file();
                DrawText(hdc, filebuf, -1, &arabic_rc, DT_RIGHT);
            }
            */
#endif
            EndPaint (hWnd, hdc);
            return 0;
        case MSG_CLOSE:
            if(fp) fclose(fp);
            DestroyMainWindow (hWnd);
            DestroyLogFont(logfont_iso8859_8);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif

    if(argc >= 2){
        strcpy(filename, argv[1]);
        if(!reset_fp_pos()) return -1;
    }
    else{
        fprintf(stderr, "usage:./hebrew_test hebrew_text.txt\n");
        return -1;
    }
    if(argc >= 3){
        if(argv[2])
            strcpy(fontname, argv[2]);
    }
    else
        strcpy(fontname, "Naskhi18");

    CreateInfo.dwStyle = 
        WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    CreateInfo.dwExStyle = WS_EX_NONE;
    //CreateInfo.spCaption = "Hello, world!";
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = HelloWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif


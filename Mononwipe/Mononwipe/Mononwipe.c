#include <windows.h>
#include <math.h>

#define M_PI 3.14159265358979323846

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msimg32.lib")

typedef union _CUSTOM_RGBQUAD {
    COLORREF rgb;
    struct { BYTE b; BYTE g; BYTE r; BYTE unused; };
} CUSTOM_RGBQUAD;

/* RNG & Hue */
static DWORD xs = 123456789;
void SeedXorshift32(DWORD seed) { xs = seed; }
DWORD Xorshift32(void) {
    xs ^= xs << 13; xs ^= xs << 17; xs ^= xs << 5; return xs;
}

static int redv=0, greenv=0, bluev=0; static BOOL ifblue=FALSE;
COLORREF Hue(int len) {
    if (redv < len) { redv++; return ifblue ? RGB(redv,0,len) : RGB(redv,0,0); }
    else if (greenv < len) { greenv++; return RGB(len,greenv,0); }
    else if (bluev < len) { bluev++; return RGB(0,len,bluev); }
    else { redv=greenv=bluev=0; ifblue=TRUE; return RGB(0,0,0); }
}
/* ==================== Effects ==================== */

DWORD WINAPI Shader1(LPVOID lp) {
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
    CUSTOM_RGBQUAD* data = (CUSTOM_RGBQUAD*)VirtualAlloc(0, (SIZE_T)w*h*sizeof(CUSTOM_RGBQUAD), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    while(1) {
        HDC d=GetDC(0), m=CreateCompatibleDC(d);
        HBITMAP b=CreateBitmap(w,h,1,32,data);
        SelectObject(m,b); BitBlt(m,0,0,w,h,d,0,0,SRCCOPY); GetBitmapBits(b,w*h*4,data);
        for(int i=0; i<w*h; i++) {
            int x = i % w;
            data[i].rgb ^= (x * (i/w)) ^ Xorshift32();
        }
        SetBitmapBits(b,w*h*4,data); BitBlt(d,0,0,w,h,m,0,0,SRCCOPY);
        DeleteObject(b); DeleteDC(m); ReleaseDC(0,d); Sleep(8);
    }
    return 0;
}

DWORD WINAPI FastMelt(LPVOID lp) {
    int w = GetSystemMetrics(SM_CXSCREEN);
    while(1) {
        HDC h=GetDC(0);
        int x = rand() % w;
        BitBlt(h, x, 0, 40, GetSystemMetrics(SM_CYSCREEN), h, x, -(rand()%15 + 5), SRCCOPY);
        ReleaseDC(0,h);
        Sleep(1);
    }
    return 0;
}

DWORD WINAPI BounceBig(LPVOID lp) {
    int w=GetSystemMetrics(SM_CXSCREEN), h=GetSystemMetrics(SM_CYSCREEN);
    int x=100, y=100, sx=1, sy=1;
    while(1) {
        HDC dc=GetDC(0);
        HBRUSH br=CreateSolidBrush(Hue(600));
        SelectObject(dc,br);
        Ellipse(dc, x, y, x+280, y+280);
        DeleteObject(br); ReleaseDC(0,dc);
        x += 26*sx; y += 26*sy;
        if(x<0 || x>w-280) sx=-sx;
        if(y<0 || y>h-280) sy=-sy;
        Sleep(5);
    }
    return 0;
}

DWORD WINAPI TextSpam(LPVOID lp) {
    LPCWSTR t[] = {L"Mononwipe.exe", L"Hax", L"GDI", L"WARNING"};
    while(1) {
        HDC dc=GetDC(0);
        SetTextColor(dc, RGB(rand()%255,rand()%255,rand()%255));
        SetBkMode(dc, TRANSPARENT);
        TextOutW(dc, rand()%GetSystemMetrics(SM_CXSCREEN), rand()%GetSystemMetrics(SM_CYSCREEN), t[rand()%4], wcslen(t[rand()%4]));
        ReleaseDC(0,dc);
        Sleep(7);
    }
    return 0;
}

DWORD WINAPI PatBltFast(LPVOID lp) {
    while(1) {
        HDC dc=GetDC(0);
        HBRUSH br=CreateSolidBrush(RGB(rand()%255,rand()%255,rand()%255));
        SelectObject(dc,br);
        PatBlt(dc,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),PATINVERT);
        DeleteObject(br); ReleaseDC(0,dc);
        Sleep(25);
    }
    return 0;
}

DWORD WINAPI InvertScreen(LPVOID lp) {
    while(1) {
        HDC dc=GetDC(0);
        BitBlt(dc,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),dc,0,0,NOTSRCCOPY);
        ReleaseDC(0,dc);
        Sleep(80);
    }
    return 0;
}

DWORD WINAPI SineWave(LPVOID lp) {
    int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
    double a = 0.0;
    while(1) {
        HDC dc=GetDC(0);
        for(int i=0; i<sw; i+=6) {
            int offset = (int)(sin(a + i*0.03) * 28);
            BitBlt(dc, i, 0, 6, sh, dc, i, offset, SRCCOPY);
        }
        a += 0.35;
        ReleaseDC(0,dc);
        Sleep(10);
    }
    return 0;
}

DWORD WINAPI RandomNoise(LPVOID lp) {
    int w=GetSystemMetrics(SM_CXSCREEN), h=GetSystemMetrics(SM_CYSCREEN);
    while(1) {
        HDC dc=GetDC(0);
        for(int i=0; i<1200; i++) {
            int x = rand() % w;
            int y = rand() % h;
            SetPixel(dc, x, y, rand() % 0xFFFFFF);
        }
        ReleaseDC(0,dc);
        Sleep(6);
    }
    return 0;
}

DWORD WINAPI CircleSpam(LPVOID lp) {
    int w=GetSystemMetrics(SM_CXSCREEN), h=GetSystemMetrics(SM_CYSCREEN);
    while(1) {
        HDC dc=GetDC(0);
        HBRUSH br=CreateSolidBrush(RGB(rand()%255,rand()%255,rand()%255));
        SelectObject(dc,br);
        Ellipse(dc, rand()%w, rand()%h, rand()%w, rand()%h);
        DeleteObject(br);
        ReleaseDC(0,dc);
        Sleep(12);
    }
    return 0;
}

/* Stage Manager */
static HANDLE threads[12] = {0};
static int currentStage = 0;

void KillAllThreads() {
    for(int i=0; i<12; i++) if(threads[i]) {
        TerminateThread(threads[i],0);
        CloseHandle(threads[i]);
        threads[i] = NULL;
    }
    InvalidateRect(0,NULL,TRUE);
}

void StartStage(int stage) {
    KillAllThreads();
    currentStage = stage % 9;

    switch(currentStage) {
        case 0: threads[0]=CreateThread(0,0,Shader1,NULL,0,0);     threads[1]=CreateThread(0,0,FastMelt,NULL,0,0); break;
        case 1: threads[0]=CreateThread(0,0,BounceBig,NULL,0,0);   threads[1]=CreateThread(0,0,TextSpam,NULL,0,0); break;
        case 2: threads[0]=CreateThread(0,0,PatBltFast,NULL,0,0);  threads[1]=CreateThread(0,0,InvertScreen,NULL,0,0); break;
        case 3: threads[0]=CreateThread(0,0,SineWave,NULL,0,0);    threads[1]=CreateThread(0,0,RandomNoise,NULL,0,0); break;
        case 4: threads[0]=CreateThread(0,0,CircleSpam,NULL,0,0);  threads[1]=CreateThread(0,0,FastMelt,NULL,0,0); break;
        case 5: threads[0]=CreateThread(0,0,Shader1,NULL,0,0);     threads[1]=CreateThread(0,0,BounceBig,NULL,0,0); break;
        case 6: threads[0]=CreateThread(0,0,InvertScreen,NULL,0,0);threads[1]=CreateThread(0,0,TextSpam,NULL,0,0); break;
        case 7: threads[0]=CreateThread(0,0,SineWave,NULL,0,0);    threads[1]=CreateThread(0,0,PatBltFast,NULL,0,0); break;
        case 8: threads[0]=CreateThread(0,0,RandomNoise,NULL,0,0); threads[1]=CreateThread(0,0,CircleSpam,NULL,0,0); break;
    }
}

/* WinMain */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    if(MessageBoxW(NULL,L"Run Intense GDI Payloads",L"Mononwipe.exe",MB_YESNO|MB_ICONEXCLAMATION)==IDNO) return 0;
    if(MessageBoxW(NULL,L"FINAL WARNING!\nExtremely chaotic effects ahead.\nIf you want to continue press yes to begin",L"Mononwipe.exe",MB_YESNO|MB_ICONWARNING)==IDNO) return 0;

    SeedXorshift32(GetTickCount());
    StartStage(0);

    while(1) {
        Sleep(5000);   // Change every 5 seconds
        StartStage(currentStage + 1);
    }
    return 0;
}
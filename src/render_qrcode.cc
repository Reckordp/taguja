#include <jenneire.h>
#include "render_qrcode.h"

#define NAMA_KELAS_QRCODE (L"Whatsapp Muti Device")
#define NAMA_WINDOW_QRCODE (L"Login QRCode")
#define UKURAN_WINDOW_QRCODE (640)

HWND mainWindow = NULL;
std::vector<std::vector<bool>> texture{};

void render_lukis_texture(HDC dc) {
  std::vector<bool> baris;
  bool bagian;
  HBRUSH spray;
  RECT kotak;
  spray = WHITE_BRUSH;
  kotak.left = 0;
  kotak.top = 0;
  kotak.right = UKURAN_WINDOW_QRCODE;
  kotak.bottom = UKURAN_WINDOW_QRCODE;
  FillRect(dc, &kotak, spray);

  spray = (HBRUSH)BLACK_BRUSH;
  for (size_t y = 0; y < texture.size(); y++) {
    baris = texture[y];
    kotak.top = 55 + y * 10;
    kotak.bottom = kotak.top + 10;
    for (size_t x = 0; x < baris.size(); x++) {
      kotak.left = 55 + x * 10;
      kotak.right = kotak.left + 10;
      bagian = baris[x];
      if (!bagian) continue;
      // spray = bagian ? BLACK_BRUSH : WHITE_BRUSH;
      FillRect(dc, &kotak, spray);
    }
  }
}

LRESULT CALLBACK QRCODEProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hdc;

  switch(uMsg) {
    case WM_CREATE:
      return 0;

    case WM_CLOSE:
      DestroyWindow(hwnd);
      return 0;
    
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    
    case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      render_lukis_texture(hdc);
      EndPaint(hwnd, &ps);
      return 0;
    
    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

static DWORD putaran_jendela(void* void_ptr) {
  HINSTANCE inst;
  WNDCLASSEX wc;

  inst = *reinterpret_cast<HINSTANCE*>(void_ptr);

  ZeroMemory(&wc, sizeof(wc));
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = QRCODEProc;
  wc.hInstance = inst;
  wc.lpszClassName = NAMA_KELAS_QRCODE;
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon = LoadIcon(inst, IDI_APPLICATION);
  int screen_width = GetSystemMetrics(SM_CXSCREEN);
  int screen_height = GetSystemMetrics(SM_CYSCREEN);

  RegisterClassEx(&wc);
  mainWindow = CreateWindow(
    NAMA_KELAS_QRCODE, 
    NAMA_WINDOW_QRCODE, 
    WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_SIZEBOX), 
    screen_width / 2 - UKURAN_WINDOW_QRCODE / 2, 
    screen_height / 2 - UKURAN_WINDOW_QRCODE / 2, 
    UKURAN_WINDOW_QRCODE, UKURAN_WINDOW_QRCODE, 
    NULL, NULL, inst, NULL);
  
  letakkan_penunjang(mainWindow);
  UpdateWindow(mainWindow);
  ShowWindow(mainWindow, SW_MINIMIZE);

  MSG msg;
  while(GetMessage(&msg, mainWindow, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  mainWindow = NULL;
  return NULL;
}

void RenderInit(HINSTANCE* pinst) {
  HANDLE event_loop;
  DWORD loop_id;
  event_loop = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)putaran_jendela, pinst, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION, &loop_id);
  ResumeThread(event_loop);
}

void RenderAngkat(int ukuran, std::vector<std::vector<bool>> t) {
  if (!mainWindow) return;
  texture = t;
  InvalidateRect(mainWindow, NULL, FALSE);
  if (GetActiveWindow() != mainWindow) {
    penunjang_angkat();
  }
}
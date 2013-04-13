#include <windows.h>

#define STAGE_X 10
#define STAGE_Y 20

HINSTANCE hInstance;
HWND hMainWindow;

HDC hMemDC, hBlockDC;
HBITMAP hMemPrev, hBlockPrev;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch(msg) {
  case WM_CREATE: {
    HDC hdc = GetDC(hWnd);

    hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 24 * 10, 24 * 20);
    hMemPrev = (HBITMAP)SelectObject(hMemDC, hBitmap);

    hBlockDC = CreateCompatibleDC(hdc);
    hBitmap = LoadBitmap(hInstance, "BLOCKS");
    hBlockPrev = (HBITMAP)SelectObject(hBlockDC, hBitmap);

    // debug
    BitBlt(hMemDC, 0, 0, 24, 24, hBlockDC, 0, 0, SRCCOPY);

    ReleaseDC (hWnd, hdc);
    break;
  }
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    BitBlt(hdc, 0, 0, 24*10, 24*20, hMemDC, 0, 0, SRCCOPY);
    EndPaint(hWnd, &ps);
    break;
  }
  case WM_DESTROY: {
    HBITMAP hBitmap = (HBITMAP)SelectObject(hMemDC, hMemPrev);
    DeleteObject(hBitmap);
    DeleteObject(hMemDC);
    
    hBitmap = (HBITMAP)SelectObject(hBlockDC, hBlockPrev);
    DeleteObject(hBitmap);
    DeleteObject(hBlockDC);
    PostQuitMessage(0);
    break;
  }
  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}

int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {
  hInstance = hInst;
  
  WNDCLASSEX wc;
  static LPSTR pClassName = "WinTetris";
  
  /* �E�B���h�E�N���X���`���� */
  wc.hInstance = hInst;                   // ���̃C���X�^���X�ւ̃n���h��
  wc.lpszClassName = pClassName;               // �E�B���h�E�N���X��
  wc.lpfnWndProc = (WNDPROC)WndProc;                   // �E�B���h�E�֐�
  wc.style = CS_HREDRAW | CS_VREDRAW;                               // �E�B���h�E�X�^�C���i�f�t�H���g�j
  wc.cbSize = sizeof(WNDCLASSEX);             // WNDCLASSEX�\���̂̃T�C�Y��ݒ�
  wc.hIcon = NULL; // ���[�W�A�C�R��
  wc.hIconSm = NULL;   // �X���[���A�C�R��
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);   // �J�[�\���X�^�C��
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // �E�B���h�E�̔w�i�i���ɐݒ�j
  wc.lpszMenuName = NULL;                     // ���j���[�i�Ȃ��ɐݒ�j
  wc.cbClsExtra = 0;                          // �G�L�X�g���i�Ȃ��ɐݒ�j
  wc.cbWndExtra = 0;                          // �K�v�ȏ��i�Ȃ��ɐݒ�j
  
  
  /* �E�B���h�E�N���X��o�^���� */
  if (!RegisterClassEx(&wc))
    return (FALSE);

  RECT r;
  r.left = r.top = 0;
  r.right = 24 * STAGE_X;
  r.bottom = 24 * STAGE_Y;
  AdjustWindowRectEx(&r, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION, false, 0);
  
  hMainWindow = CreateWindow(pClassName, "WinTetris", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             r.right - r.left, r.bottom - r.top, NULL, NULL, hInst, NULL);
  ShowWindow(hMainWindow, SW_SHOW);

  MSG msg;
  while(GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  
  return 0;
}
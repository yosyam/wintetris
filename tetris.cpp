#include <windows.h>

HINSTANCE hInstance;
HWND hMainWindow;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch(msg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}

int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {
  hInstance = hInst;
  
  WNDCLASSEX wc;
  static LPSTR pClassName = "WinTetris";
  
  /* ウィンドウクラスを定義する */
  wc.hInstance = hInst;                   // このインスタンスへのハンドル
  wc.lpszClassName = pClassName;               // ウィンドウクラス名
  wc.lpfnWndProc = (WNDPROC)WndProc;                   // ウィンドウ関数
  wc.style = CS_HREDRAW | CS_VREDRAW;                               // ウィンドウスタイル（デフォルト）
  wc.cbSize = sizeof(WNDCLASSEX);             // WNDCLASSEX構造体のサイズを設定
  wc.hIcon = NULL; // ラージアイコン
  wc.hIconSm = NULL;   // スモールアイコン
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);   // カーソルスタイル
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // ウィンドウの背景（黒に設定）
  wc.lpszMenuName = NULL;                     // メニュー（なしに設定）
  wc.cbClsExtra = 0;                          // エキストラ（なしに設定）
  wc.cbWndExtra = 0;                          // 必要な情報（なしに設定）
  
  
  /* ウィンドウクラスを登録する */
  if (!RegisterClassEx(&wc))
    return (FALSE);

  RECT r;
  r.left = r.top = 0;
  r.right = 24 * 10;
  r.bottom = 24 * 20;
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
#include <windows.h>

#define STAGE_X 10
#define STAGE_Y 20

HINSTANCE hInstance;
HWND hMainWindow;

HDC hMemDC, hBlockDC;
HBITMAP hMemPrev, hBlockPrev;

int board[12][25];

typedef struct _TAG_POSITION {
  int x;
  int y;
} POSITION;

typedef struct _TAG_BLOCK {
  int rotate;
  POSITION p[3];
} BLOCK;

BLOCK block[8] = {
  {1, {{0, 0}, {0, 0}, {0, 0}}}, // null
  {2, {{0, -1}, {0, 1}, {0, 2}}}, // tetris
  {4, {{0, -1}, {0, 1}, {1, 1}}}, // L1
  {4, {{0, -1}, {0, 1}, {-1, 1}}}, // L2
  {2, {{0, -1}, {1, 0}, {1, 1}}}, // key1
  {2, {{0, -1}, {-1, 0}, {-1, 1}}}, // key2
  {1, {{0, 1}, {1, 0}, {1, 1}}}, // square
  {4, {{0, -1}, {1, 0}, {-1, 0}}}, // t
};

typedef struct _TAG_STATUS {
  int x;
  int y;
  int type;
  int rotate;
} STATUS;

STATUS current;

int random(int max) {
  return (int) ((rand() / (RAND_MAX + 1.0) * max));
}

bool putBlock(STATUS s, bool action = false) {
  if(board[s.x][s.y] != 0) {
    return false;
  }
  if (action) {
    board[s.x][s.y] = s.type;
  }
  for (int i = 0; i < 3; i++) {
    int dx = block[s.type].p[i].x;
    int dy = block[s.type].p[i].y;
    int r = s.rotate % block[s.type].rotate;
    for (int j = 0; j < r; j++) {
      int nx = dx, ny = dy;
      dx = ny; dy = -nx;
    }
    if (board[s.x + dx][s.y + dy] != 0){
      return false;
    }
    if (action) {
      board[s.x + dx][s.y + dy] = s.type;
    }
  }
  if (!action) {
    putBlock(s, true);
  }
  return true;
}

bool deleteBlock(STATUS s) {
  board[s.x][s.y] = 0;
  for (int i = 0; i < 3; i++) {
    int dx = block[s.type].p[i].x;
    int dy = block[s.type].p[i].y;
    int r = s.rotate % block[s.type].rotate;
    for (int j = 0; j < r; j++) {
      int nx = dx, ny = dy;
      dx = ny; dy = -nx;
    }
    board[s.x + dx][s.y + dy] = 0;
  }
  return true;
}


void showBoard(){
  for (int x = 1; x <= 10; x++){
    for(int y = 1; y < 25; y++){
      BitBlt(hMemDC, (x - 1) * 24, (20 - y) * 24, 24, 24, hBlockDC, 0, board[x][y] * 24, SRCCOPY);
    }
  }

}

void processInput() {
  STATUS n = current;
  if (GetAsyncKeyState(VK_LEFT)) {
    n.x--;
  } else if (GetAsyncKeyState(VK_RIGHT)) {
    n.x++;
  } else if (GetAsyncKeyState(VK_UP)) {
    n.rotate++;
  } else if (GetAsyncKeyState(VK_DOWN)) {
    n.y--;
  }
  
  if(n.x != current.x || n.y != current.y || n.rotate != current.rotate) {
    deleteBlock(current);
    if(putBlock(n)){
      current = n;
    } else {
      putBlock(current);
    }
  }
}

void gameOver() {
  KillTimer(hMainWindow, 100);
  for (int x = 1; x <= 10; x++) {
    for (int y = 1; y < 20; y++) {
      if(board[x][y] != 0) {
        board[x][y] = 1;
      }
    }
  }
  InvalidateRect(hMainWindow, NULL, false);
}

void blockDown(){
  deleteBlock(current);
  current.y--;
  if(!putBlock(current)) {
    current.y++;
    putBlock(current);

    // new block
    current.x = 5;
    current.y = 21;
    current.type = random(7) + 1;
    current.rotate = random(4);
    if (!putBlock(current)) {
      gameOver();
    }
  }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch(msg) {
  case WM_CREATE: {
    for(int x = 0; x < 12; x++){
      for(int y = 0; y < 25; y++){
        if (x == 0 || x == 11 || y == 0) {
          board[x][y] = 1;
        } else {
          board[x][y] = 0;
        }
      }
    }

    current.x = 5;
    current.y = 21;
    current.type = random(7)+1;
    current.rotate = random(4);
    putBlock(current);
        
    HDC hdc = GetDC(hWnd);

    hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 24 * 10, 24 * 20);
    hMemPrev = (HBITMAP)SelectObject(hMemDC, hBitmap);

    hBlockDC = CreateCompatibleDC(hdc);
    hBitmap = LoadBitmap(hInstance, "BLOCKS");
    hBlockPrev = (HBITMAP)SelectObject(hBlockDC, hBitmap);

    // debug
    //BitBlt(hMemDC, 0, 0, 24, 24, hBlockDC, 0, 0, SRCCOPY);

    ReleaseDC (hWnd, hdc);
    break;
  }
  case WM_TIMER: {
    processInput();
    blockDown();
    InvalidateRect(hWnd, NULL, false);
    break;
  }
  case WM_PAINT: {
    showBoard();
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    BitBlt(hdc, 0, 0, 24 * 10, 24 * 20, hMemDC, 0, 0, SRCCOPY);
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
  static LPSTR pClassName = (LPSTR) "WinTetris";
  
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
  r.right = 24 * STAGE_X;
  r.bottom = 24 * STAGE_Y;
  AdjustWindowRectEx(&r, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION, false, 0);
  
  hMainWindow = CreateWindow(pClassName, "WinTetris", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             r.right - r.left, r.bottom - r.top, NULL, NULL, hInst, NULL);
  ShowWindow(hMainWindow, SW_SHOW);

  SetTimer(hMainWindow, 100, 1000/30, NULL);
  
  MSG msg;
  while(GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  KillTimer(hMainWindow, 100);
  
  return 0;
}
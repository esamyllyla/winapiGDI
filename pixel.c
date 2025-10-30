#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "rendering.c"

static bool quit = false;
static float dt = 0;

HWND hEdit = NULL;
HWND hButton = NULL;
HWND hStatic = NULL;

Frame frame = {0};

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;

static char InputBuffer[256];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
    const char window_class_name[] = "My Window Class";
    static WNDCLASS window_class = { 0 };
    window_class.lpfnWndProc = WindowProcessMessage;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = window_class_name;
    RegisterClass(&window_class);

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    static HWND window_handle;
    window_handle = CreateWindow(window_class_name, "Drawing Pixels", 
    							 WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
                                 640, 300, 880, 680, NULL, NULL, hInstance, NULL);
    if(window_handle == NULL) { return -1; }

    while(!quit) {
        static MSG message = { 0 };
        while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
        { 
        	DispatchMessage(&message); 
            TranslateMessage(&message);
        }
        
        render(&frame, dt);
        
        InvalidateRect(window_handle, NULL, FALSE);
        UpdateWindow(window_handle);
        
        dt += 0.25f;
    }
    return 0;
}


LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    	case WM_CREATE:
        {
        	//hEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            //	5, 35, 150, 30, window_handle, NULL, NULL, NULL);
            
            hButton = CreateWindow("BUTTON", "Enter",
            	WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                155, 35, 60, 30, window_handle, 
                NULL, (HINSTANCE)GetWindowLongPtr(window_handle, GWLP_HINSTANCE),
                NULL);
            
            //hStatic = CreateWindow("EDIT", "",
            //	WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
            //    5, 95, 150, 30, window_handle, NULL, NULL, NULL);
        } break;
    
    	case WM_COMMAND:
        {
        	switch(wParam)
            {
            	case BN_CLICKED:
                {
                	char sum[256];
                	char buff[64];
                    
                    GetWindowText(hEdit, buff, 128);
                    SetWindowText(hEdit, "");
                    
                    strcpy(sum, InputBuffer);
                    strcat(sum, "\r\n");
                    strcat(sum, buff);
                    strcpy(InputBuffer, sum);
                    SetWindowText(hStatic, sum);
                    
					SendMessageA(hStatic, EM_LINESCROLL, 0, 9999);
                } break;
            }
        } break;
    
        case WM_QUIT:
        case WM_DESTROY: {
            quit = true;
        } break;

        case WM_PAINT: {
            static PAINTSTRUCT paint;
            static HDC device_context;
            device_context = BeginPaint(window_handle, &paint);
            BitBlt(device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
                   frame_device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   SRCCOPY);
            EndPaint(window_handle, &paint);
        } break;

        case WM_SIZE: {
            frame_bitmap_info.bmiHeader.biWidth  = LOWORD(lParam);
            frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

            if(frame_bitmap) DeleteObject(frame_bitmap);
            frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
            SelectObject(frame_device_context, frame_bitmap);

            frame.width =  LOWORD(lParam);
            frame.height = HIWORD(lParam);
            
            MoveWindow(hEdit, 0, frame.height - 30, frame.width - 60, 30, TRUE);
            MoveWindow(hButton, frame.width - 60, frame.height - 30, 60, 30, TRUE);
            MoveWindow(hStatic, 0, frame.height - 90, frame.width, 60, TRUE);
        } break;

        default: {
            return DefWindowProc(window_handle, message, wParam, lParam);
        }
    }
    return 0;
}

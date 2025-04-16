#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

// Global variables
HWND hwndEdit;
HINSTANCE hInst;
WCHAR szFileName[MAX_PATH] = L"";
WCHAR szAppName[] = L"SimpleTextEditor";
BOOL bFileChanged = FALSE;

// Function prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitApplication(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
void DoFileOpen(HWND);
void DoFileSave(HWND);
void DoFileSaveAs(HWND);
void DoFileNew(HWND);
BOOL SaveFile(HWND, LPCWSTR);
BOOL LoadFile(HWND, LPCWSTR);
void SetWindowTitle(HWND, LPCWSTR);
BOOL AskSaveChanges(HWND);

// Menu IDs
#define IDM_NEW     1001
#define IDM_OPEN    1002
#define IDM_SAVE    1003
#define IDM_SAVEAS  1004
#define IDM_EXIT    1005
#define IDM_CUT     1101
#define IDM_COPY    1102
#define IDM_PASTE   1103
#define IDM_SELECTALL 1104
#define IDM_WORDWRAP  1201
#define IDM_ABOUT   1301

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG msg;
    
    // Initialize application
    if (!InitApplication(hInstance))
        return FALSE;
    
    // Initialize this instance
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;
    
    // Message loop
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}

BOOL InitApplication(HINSTANCE hInstance) {
    WNDCLASS wc;
    
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szAppName;
    
    return RegisterClass(&wc);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    HWND hwnd;
    HMENU hMenu, hSubMenu;
    
    hInst = hInstance;
    
    // Create main menu
    hMenu = CreateMenu();
    
    // File menu
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, IDM_NEW, L"&New");
    AppendMenu(hSubMenu, MF_STRING, IDM_OPEN, L"&Open...");
    AppendMenu(hSubMenu, MF_STRING, IDM_SAVE, L"&Save");
    AppendMenu(hSubMenu, MF_STRING, IDM_SAVEAS, L"Save &As...");
    AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hSubMenu, MF_STRING, IDM_EXIT, L"E&xit");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"&File");
    
    // Edit menu
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, IDM_CUT, L"Cu&t");
    AppendMenu(hSubMenu, MF_STRING, IDM_COPY, L"&Copy");
    AppendMenu(hSubMenu, MF_STRING, IDM_PASTE, L"&Paste");
    AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hSubMenu, MF_STRING, IDM_SELECTALL, L"Select &All");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"&Edit");
    
    // Format menu
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, IDM_WORDWRAP, L"&Word Wrap");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"F&ormat");
    
    // Help menu
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, IDM_ABOUT, L"&About...");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"&Help");
    
    // Create the main window
    hwnd = CreateWindow(
        szAppName,
        L"Simple Text Editor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        NULL,
        hMenu,
        hInstance,
        NULL
    );
    
    if (!hwnd)
        return FALSE;
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            // Create text edit control
            hwndEdit = CreateWindowW(
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                hwnd,
                (HMENU)1,
                hInst,
                NULL
            );
            
            // Set edit control font
            SendMessage(hwndEdit, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
            
            // Initialize default window title
            SetWindowTitle(hwnd, NULL);
            break;
            
        case WM_SIZE:
            // Resize edit control to fill client area
            MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            break;
            
        case WM_SETFOCUS:
            SetFocus(hwndEdit);
            break;
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_NEW:
                    DoFileNew(hwnd);
                    break;
                    
                case IDM_OPEN:
                    DoFileOpen(hwnd);
                    break;
                    
                case IDM_SAVE:
                    DoFileSave(hwnd);
                    break;
                    
                case IDM_SAVEAS:
                    DoFileSaveAs(hwnd);
                    break;
                    
                case IDM_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                    
                case IDM_CUT:
                    SendMessage(hwndEdit, WM_CUT, 0, 0);
                    break;
                    
                case IDM_COPY:
                    SendMessage(hwndEdit, WM_COPY, 0, 0);
                    break;
                    
                case IDM_PASTE:
                    SendMessage(hwndEdit, WM_PASTE, 0, 0);
                    break;
                    
                case IDM_SELECTALL:
                    SendMessage(hwndEdit, EM_SETSEL, 0, -1);
                    break;
                    
                case IDM_WORDWRAP:
                    {
                        DWORD dwStyle = GetWindowLong(hwndEdit, GWL_STYLE);
                        HMENU hMenu = GetMenu(hwnd);
                        
                        if (dwStyle & ES_AUTOHSCROLL) {
                            // Turn on word wrap (remove horizontal scrollbar)
                            dwStyle &= ~WS_HSCROLL;
                            dwStyle &= ~ES_AUTOHSCROLL;
                            CheckMenuItem(hMenu, IDM_WORDWRAP, MF_CHECKED);
                        } else {
                            // Turn off word wrap (add horizontal scrollbar)
                            dwStyle |= WS_HSCROLL | ES_AUTOHSCROLL;
                            CheckMenuItem(hMenu, IDM_WORDWRAP, MF_UNCHECKED);
                        }
                        
                        SetWindowLong(hwndEdit, GWL_STYLE, dwStyle);
                        
                        // Force the edit control to recalculate its layout
                        SetWindowPos(hwndEdit, NULL, 0, 0, 0, 0, 
                                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                    }
                    break;
                    
                case IDM_ABOUT:
                    MessageBox(hwnd, 
                              L"Simple Text Editor\n"
                              L"Created with Windows API and C", 
                              L"About Simple Text Editor", 
                              MB_OK | MB_ICONINFORMATION);
                    break;
                
                // Detect text changes
                case 1:  // Edit control ID
                    if (HIWORD(wParam) == EN_CHANGE) {
                        bFileChanged = TRUE;
                    }
                    break;
            }
            break;
            
        case WM_CLOSE:
            if (bFileChanged) {
                if (!AskSaveChanges(hwnd)) {
                    // User canceled the close operation
                    return 0;
                }
            }
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void DoFileNew(HWND hwnd) {
    if (bFileChanged) {
        if (!AskSaveChanges(hwnd)) {
            return;  // User canceled the operation
        }
    }
    
    SetWindowText(hwndEdit, L"");
    szFileName[0] = '\0';
    SetWindowTitle(hwnd, NULL);
    bFileChanged = FALSE;
}

void DoFileOpen(HWND hwnd) {
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = L"";
    
    if (bFileChanged) {
        if (!AskSaveChanges(hwnd)) {
            return;  // User canceled the operation
        }
    }
    
    // Initialize OPENFILENAME structure
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameW(&ofn)) {
        if (LoadFile(hwnd, ofn.lpstrFile)) {
            lstrcpyW(szFileName, ofn.lpstrFile);
            SetWindowTitle(hwnd, szFileName);
            bFileChanged = FALSE;
        }
    }
}

void DoFileSave(HWND hwnd) {
    if (szFileName[0] == '\0') {
        DoFileSaveAs(hwnd);
    } else {
        if (SaveFile(hwnd, szFileName)) {
            bFileChanged = FALSE;
        }
    }
}

void DoFileSaveAs(HWND hwnd) {
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = L"";
    
    if (szFileName[0] != L'\0') {
        lstrcpyW(szFile, szFileName);
    }
    
    // Initialize OPENFILENAME structure
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileNameW(&ofn)) {
        if (SaveFile(hwnd, ofn.lpstrFile)) {
            lstrcpyW(szFileName, ofn.lpstrFile);
            SetWindowTitle(hwnd, szFileName);
            bFileChanged = FALSE;
        }
    }
}

BOOL LoadFile(HWND hwnd, LPCWSTR pszFileName) {
    HANDLE hFile;
    DWORD dwFileSize, dwBytesRead;
    LPWSTR pszFileText;
    BOOL bSuccess = FALSE;
    
    hFile = CreateFileW(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                      
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBoxW(hwnd, L"Failed to open file", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    
    dwFileSize = GetFileSize(hFile, NULL);
    
    if (dwFileSize == INVALID_FILE_SIZE) {
        MessageBoxW(hwnd, L"Failed to get file size", L"Error", MB_OK | MB_ICONERROR);
        CloseHandle(hFile);
        return FALSE;
    }
    
    // Allocate memory for UTF-16 text (each character is 2 bytes)
    // Plus extra space for a null terminator
    pszFileText = (LPWSTR)GlobalAlloc(GPTR, (dwFileSize + 2));
    
    if (pszFileText == NULL) {
        MessageBoxW(hwnd, L"Failed to allocate memory", L"Error", MB_OK | MB_ICONERROR);
        CloseHandle(hFile);
        return FALSE;
    }
    
    if (ReadFile(hFile, pszFileText, dwFileSize, &dwBytesRead, NULL)) {
        // Calculate number of characters in the buffer
        DWORD charCount = dwBytesRead / sizeof(WCHAR);
        // Ensure the string is properly null-terminated
        pszFileText[charCount] = L'\0';
        
        // Set text in edit control
        SetWindowTextW(hwndEdit, pszFileText);
        bSuccess = TRUE;
    } else {
        MessageBoxW(hwnd, L"Failed to read file", L"Error", MB_OK | MB_ICONERROR);
    }
    
    GlobalFree(pszFileText);
    CloseHandle(hFile);
    
    return bSuccess;
}

BOOL SaveFile(HWND hwnd, LPCWSTR pszFileName) {
    HANDLE hFile;
    DWORD dwTextLength, dwBytesWritten;
    LPWSTR pszText;
    BOOL bSuccess = FALSE;
    
    hFile = CreateFileW(pszFileName, GENERIC_WRITE, 0, NULL, 
                      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                      
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBoxW(hwnd, L"Failed to create file", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    
    // Get text length
    dwTextLength = GetWindowTextLengthW(hwndEdit);
    
    if (dwTextLength > 0) {
        pszText = (LPWSTR)GlobalAlloc(GPTR, (dwTextLength + 1) * sizeof(WCHAR));
        
        if (pszText != NULL) {
            if (GetWindowTextW(hwndEdit, pszText, dwTextLength + 1)) {
                // Calculate byte length for UTF-16 text
                DWORD byteLength = dwTextLength * sizeof(WCHAR);
                
                if (WriteFile(hFile, pszText, byteLength, &dwBytesWritten, NULL)) {
                    bSuccess = TRUE;
                } else {
                    MessageBoxW(hwnd, L"Failed to write to file", L"Error", MB_OK | MB_ICONERROR);
                }
            } else {
                MessageBoxW(hwnd, L"Failed to get text from edit control", L"Error", MB_OK | MB_ICONERROR);
            }
            
            GlobalFree(pszText);
        } else {
            MessageBoxW(hwnd, L"Failed to allocate memory", L"Error", MB_OK | MB_ICONERROR);
        }
    } else {
        // Empty file - still considered a success
        bSuccess = TRUE;
    }
    
    CloseHandle(hFile);
    
    return bSuccess;
}

void SetWindowTitle(HWND hwnd, LPCWSTR pszFileName) {
    WCHAR szTitle[MAX_PATH + 20];
    
    if (pszFileName == NULL || pszFileName[0] == L'\0') {
        lstrcpyW(szTitle, L"Simple Text Editor - [Untitled]");
    } else {
        wsprintfW(szTitle, L"Simple Text Editor - [%s]", pszFileName);
    }
    
    SetWindowText(hwnd, szTitle);
}

BOOL AskSaveChanges(HWND hwnd) {
    int nResult;
    
    nResult = MessageBoxW(hwnd, 
                        L"The text has been changed.\nDo you want to save changes?", 
                        L"Simple Text Editor", 
                        MB_YESNOCANCEL | MB_ICONQUESTION);
                        
    switch (nResult) {
        case IDYES:
            DoFileSave(hwnd);
            // Only continue if save was successful
            return !bFileChanged;
            
        case IDNO:
            return TRUE;  // Continue without saving
            
        case IDCANCEL:
            return FALSE;  // Cancel operation
            
        default:
            return FALSE;
    }
}
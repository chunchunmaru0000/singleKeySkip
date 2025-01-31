#include <windows.h>
#include <iostream>


HHOOK hHook;
bool winKeyDown = false;
int messagesWasWhileDown_VK_LWIN = 0;

// https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-kbdllhookstruct
        // lParam [in] Type: LPARAM A pointer to a KBDLLHOOKSTRUCT structure.
        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        MSG* msg = (MSG*)lParam;

        // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
        DWORD key = kbStruct->vkCode;
        std::cout << key << ' ' << kbStruct->dwExtraInfo << '\n';
        
        if (key == VK_LWIN) 
        {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
            {
                winKeyDown = true;
                messagesWasWhileDown_VK_LWIN = 0;
            } 
            else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) 
            {
                winKeyDown = false;
                
                if (messagesWasWhileDown_VK_LWIN > 0)
                    std::cout << "combination" << '\n';
                else 
                {
                    std::cout << "just single press" << '\n';
                    //keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
                    return 1;
                }
            }
        }

        if (winKeyDown && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && key != VK_LWIN)
            messagesWasWhileDown_VK_LWIN++;

        std::cout << (winKeyDown ? "lwin down" : "lwin up") << '\n';
    }
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-callnexthookex
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main()
{
    /*
    INPUT input[6] = {
        {INPUT_KEYBOARD},
        {INPUT_KEYBOARD},
        {INPUT_KEYBOARD},
        {INPUT_KEYBOARD},
        {INPUT_KEYBOARD},
        {INPUT_KEYBOARD},
    };

    input[0].ki.wVk = VK_LCONTROL;
    input[1].ki.wVk = VK_LSHIFT;
    input[2].ki.wVk = VK_ESCAPE;

    input[3].ki.wVk = VK_LCONTROL;
    input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    input[4].ki.wVk = VK_LSHIFT;
    input[4].ki.dwFlags = KEYEVENTF_KEYUP;
    input[5].ki.wVk = VK_ESCAPE;
    input[5].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(_countof(input), input, sizeof(INPUT));
    */

    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    return 0;
}

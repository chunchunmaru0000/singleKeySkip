#include <windows.h>
#include <iostream>

HHOOK hHook;

bool winKeyDown = false;
int messagesWasWhileDown_VK_LWIN = 0;

bool altLeftKeyDown = false;
int messagesWasWhileDown_VK_LALT = 0;

/* Ъ УНРЕК ЯДЕКЮРЭ ЕЫЕ ОПНЫЕ МН СУНДХР Б ЖХЙК Х ЯДЕКЮРЭ Б ХРНЦЕ ЙНПНВЕ МЕ ОНКСВХКНЯЭ, ЩРН ЛНФМН ОПНЯРН СДЮКХРЭ
struct KeyData {
    DWORD key;
    const char* view;
    mutable bool isDown;
    int otherPressesCounter;

    void print() {
        std::cout << "Key: " << key << ", Name: " << view
            << ", Pressed: " << (isDown ? "Yes" : "No")
            << ", Counter: " << otherPressesCounter << std::endl;
    }
};

KeyData checkKeys[2] = {
    {VK_LWIN, "L_WIN", false, 0},
    {VK_LMENU, "L_ALT", false, 0},
}; ДН ЯЧДЮ
*/

int SkipKey(DWORD key, const char* keyView, bool* checkerDown, int* counter, WPARAM* wParam)
{
    if (*wParam == WM_KEYDOWN || *wParam == WM_SYSKEYDOWN)
    {
        *checkerDown = true;
        *counter = 0;
    }
    else if (*wParam == WM_KEYUP || *wParam == WM_SYSKEYUP)
    {
        *checkerDown = false;

        if (*counter > 0)
            std::cout << "combination" << '\n';
        else
        {
            std::cout << "just single press" << '\n';

            INPUT keyUpButDoNothing[4] = {
                {INPUT_KEYBOARD},
                {INPUT_KEYBOARD},
                {INPUT_KEYBOARD},
                {INPUT_KEYBOARD},
            };

            BYTE COMBINATION_SECOND_KEY = VK_F13;

            keyUpButDoNothing[0].ki.wVk = key;
            keyUpButDoNothing[1].ki.wVk = COMBINATION_SECOND_KEY;

            keyUpButDoNothing[2].ki.wVk = key;
            keyUpButDoNothing[2].ki.dwFlags = KEYEVENTF_KEYUP;
            keyUpButDoNothing[3].ki.wVk = COMBINATION_SECOND_KEY;
            keyUpButDoNothing[3].ki.dwFlags = KEYEVENTF_KEYUP;

            std::cout << "##### here does [" << keyView << " + F13] to release[" << keyView << "] with combination that does nothing\n";
            SendInput(_countof(keyUpButDoNothing), keyUpButDoNothing, sizeof(INPUT));
            std::cout << "##### here ends [" << keyView << " + F13]\n";

            return 1;
        }
    }
    return 0;
}


void AfterAnyKeyDown(DWORD key, DWORD anyKey, const char* keyView, bool* checkerDown, int* counter, WPARAM* wParam)
{
    if (*checkerDown && (*wParam == WM_KEYDOWN || *wParam == WM_SYSKEYDOWN) && anyKey != key)
        *counter = *counter + 1;

    std::cout << keyView << (*checkerDown ? " down" : " up") << '\n';
    std::cout << "keys pressed while " << keyView << " is down" << *counter << '\n';
}


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
        DWORD transKb = (kbStruct->flags >> 7) & 1; // is key up or down // doesnt have meaning because there is just wParam == WM_KEYDOWN
        std::cout << key << ' ' << (transKb == 0 ? "down" : "up") << ' ';

        //
        // б опнжедспе SkipKey еярэ 5 оюпюлерпнб:
        // 1: яюлю йкюбхью йюй гдеяэ https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
        // 2: опнярн рейярнбши бхд йкюбхьх дкъ нрнапюфемхъ б йнмянкх, лнфмн бшпегюрэ блеяре я бшбнднл, еякх ме рпеасеряъ
        // 3: сйюгюрекэ мю bool оепелеммсч, мю йюфдсч йкюбхьс ябнъ нрдекэмюъ, 
        // 4: сйюгюрекэ мю int оепелеммсч, мю йюфдсч йкюбхьс ябнъ нрдекэмюъ
        //    щрн явервхй, яйнкэйн йкюбхь ме рюйху йюй б оюпюлерпе 1 ашкн мюфюрн онйю
        //    ашкю гюфюрю йкюбхью хг оюпюлерпю 1
        //    б бхмде ъ гюлерхк, врн йнлахмюжхъ щрн рнцдю йнцдю дн нрфюрхъ йкюбхьх ашкх гюфюрш еые х дпсцхе
        //    онщрнлс рпеасеряъ нрякефхбюмхе яйнкэйн пюг ашкх мюфюрш дпсцхе йкюбхьх
        // 
        //    ю рюйфе йнцдю йкюбхью гюфюрю врнаш ее нрфюрэ лнфмн хяонкэгнбюрэ кчасч йнлахмюжхч йкюбхь
        //    йнрнпюъ асдер хяонкэгнбюрэ щрс
        // 5: сйюгюрекэ мю ярпсйрспс WPARAM, днйслемрюжхъ гдеяэ https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-kbdllhookstruct
        // 
        // б опнжедспе AfterAnyKeyDown 6 оюпюлерпнб, бяе хг йнрнпшу рюйхе фе йюй х б оепбни тсмйжхх мн рнкэй0
        // 1: 1 оюпюлерп щрн рю фе йкюбхью 
        // 2: ю 2 оюпюлерп бннаые бяе йкюбхьх
        // 
        // оепед бшонкмемхел оепбни опнжедспш хдер опнбепйю if (key == VK_LWIN)
        // онщрнлс оепбши оюпюлерп оепедюммши б SkipKey бяецдю х асдер гюубюршбюелни йкюбхьеи
        // х бшундхр врн опнжедспю SkipKey бшгшбюеряъ рнкэйн ндхм пюг опх нрфюрхх, 
        // цде SkipKey(...) == 1 нгмювюер нрюрхе, ю == 0 ме свхршбюеряъ
        // ю опнжедспю AfterAnyKeyDown днкфмю бшгшбюрэяъ бяецдю
        // 
        // врнаш сапюрэ гюубюр йкюбхьх WIN мюдн сдюкхрэ нр ячдю
        // L_WIN
        if (key == VK_LWIN)
            if (SkipKey(key, "L_WIN", &winKeyDown, &messagesWasWhileDown_VK_LWIN, &wParam) == 1)
                return 1;
        AfterAnyKeyDown(VK_LWIN, key, "L_WIN", &winKeyDown, &messagesWasWhileDown_VK_LWIN, &wParam);
        // L_WIN
        // дн ячдю
        // 


        //
        // врнаш сапюрэ гюубюр йкюбхьх кебши юкэр мюдн сдюкхрэ нр ячдю
        // L_ALT
        if (key == VK_LMENU)
            if (SkipKey(key, "L_ALT", &altLeftKeyDown, &messagesWasWhileDown_VK_LALT, &wParam) == 1)
                return 1;
        AfterAnyKeyDown(VK_LMENU, key, "L_ALT", &altLeftKeyDown, &messagesWasWhileDown_VK_LALT, &wParam);
        // L_ALT
        // дн ячдю
        // 
    }
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-callnexthookex
    return CallNextHookEx(NULL, nCode, wParam, lParam);
    //return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main()
{
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

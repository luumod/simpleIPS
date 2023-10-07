#include "../include/assist/NativeEventFilter.h"
#include "../include/widget.h"
#include <windows.h>
#include <iostream>

NativeEventFilter::NativeEventFilter(const unsigned int& mod, const unsigned int& key)
{
    this->mod = mod;
    this->key = key;
}

bool NativeEventFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
    Widget* widget = Widget::getInstance();

    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        const quint32 keycode = HIWORD(msg->lParam);
        const quint32 modifiers = LOWORD(msg->lParam);

        if (keycode == key && mod == modifiers) {
              //成功！
            widget->on_action_capture_triggered();
        }
    }

    return false;
}

void ShortcutTest::registerShortcut(const NativeEventFilter& filter)
{
    int id = filter.key ^ filter.mod;
    BOOL ok = RegisterHotKey(0, id, filter.mod, filter.key);
    if (!ok)
        std::cerr << "register shortcut failed!" << std::endl;
}
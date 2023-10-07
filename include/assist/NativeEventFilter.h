#ifndef NATIVEEVENTFILTER_H_
#define NATIVEEVENTFILTER_H_

#include <QAbstractNativeEventFilter>
#include <QObject>

class NativeEventFilter :public QAbstractNativeEventFilter {
public:
    NativeEventFilter(const unsigned int& mod, const unsigned int& key);
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)override;
    unsigned int mod, key;
};

class ShortcutTest {
public:
    ShortcutTest() {};
    ~ShortcutTest() {};
public:
    void registerShortcut(const NativeEventFilter& filter);

};

#endif
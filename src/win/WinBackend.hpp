#pragma once
#include <stdint.h>
#include <windows.h>
#include "core/Platform.hpp"

namespace WinBackend {
    void init ( );
    void shutdown ( );

    void setTargetWindow (WindowHandle w);
    void lockInput ( );
    void unlockInput ( );

    bool processEvents ( );
    void sendVirtualKey (uint32_t vk);
}
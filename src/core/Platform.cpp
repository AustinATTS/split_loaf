#ifdef _WIN32
// Separate code for Windows devices.
#include "Platform.h"
#include "win/WinBackend.h"

namespace Platform {

    void init ( ) {
        WinBackend::init(); // Start.
    }

    void shutdown ( ) {
        WinBackend::shutdown(); // End.
    }

    void setTargetWindow (WindowHandle w) {
        WinBackend::setTargetWindow(w); // Assign target window.
    }

    void lockInput ( ) {
        WinBackend::lockInput(); // Lock the keyboard to the targeted window.
    }

    void unlockInput ( ) {
        WinBackend::unlockInput(); // Unlock the keyboard.
    }

    bool processEvents ( ) {
        return WinBackend::processEvents(); // Process events.
    }

}
#else
// Currently do nothing for non Windows devices.
#endif

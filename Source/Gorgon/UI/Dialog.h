#pragma once

#include "../Containers/Collection.h"
#include "../Widgets/DialogWindow.h"

namespace Gorgon { namespace UI {
    
    /// Displays the given message in a message window
    void ShowMessage(const std::string &title, const std::string &message);
    
    /// Displays the given message in a message window, calls the given function 
    /// when the window is closed
    void ShowMessage(const std::string &title, const std::string &message, std::function<void()> onclose);
    
    /// Displays the given message in a message window
    inline void ShowMessage(const std::string &message) {
        ShowMessage("", message);
    }
    
    /// Displays the given message in a message window, calls the given function 
    /// when the window is closed
    inline void ShowMessage(const std::string &message, std::function<void()> onclose) {
        ShowMessage("", message, onclose);
    }
    
    /// Changes the text of the close button. This change will effect only future
    /// dialogs.
    void SetCloseText(const std::string &value);

} }

#pragma once

#include "../Containers/Collection.h"
#include "../Widgets/DialogWindow.h"
#include "../Enum.h"

namespace Gorgon { namespace UI {

    /// Allows specifying how the dialog can be closed without supplying a
    /// response. 
    enum class CloseOption {
        None,
        Close,
        Cancel
    };

    /// Displays the given message in a message window, calls the given function 
    /// when the window is closed
    void ShowMessage(const std::string &title, const std::string &message, std::function<void()> onclose = {}, const std::string &buttontext = "");

    /// Displays the given message in a message window, calls the given function 
    /// when the window is closed
    inline void ShowMessage(const std::string &title, const std::string &message, const std::string &buttontext) {
        ShowMessage(title, message, {}, buttontext);
    }

    /// Displays the given message in a message window, calls the given function 
    /// when the window is closed
    inline void ShowMessage(const std::string &message, std::function<void()> onclose = {}) {
        ShowMessage("", message, onclose);
    }

    /// Asks a multiple choice question to the user with an optional close/cancel button.
    /// Useful for a few options
    void MultipleChoiceIndex(
        const std::string &title, const std::string &message,
        const std::vector<std::string> &options, std::function<void(int)> onselect,
        CloseOption close = CloseOption::None
    );

    /// Asks a multiple choice question to the user with an optional close/cancel button.
    /// Useful for a few options. Selection option index is passed to the onselect function.
    /// If close/cancel option is selected, -1 is passed to onselect. options are not
    /// needed after the function call and can be destroyed.
    inline void MultipleChoiceIndex(
        const std::string &message,
        const std::vector<std::string> &options, std::function<void(int)> onselect,
        CloseOption close = CloseOption::None
    ) {
        MultipleChoiceIndex("", message, options, onselect, close);
    }
    
    /// Asks a multiple choice question to the user with an optional close/cancel button
    /// Useful for a few options. Option value is passed to onselect function. options must
    /// outlive the dialog. Use MultipleChoiceIndex if this is not practical.
    template<class T_>
    void MultipleChoice(
        const std::string &title, const std::string &message,
        const std::vector<T_> &options, std::function<void(T_)> onselect,
        CloseOption close = CloseOption::None, std::function<void(void)> onclose = {}
    ) {
        std::vector<std::string> opts;
        for(auto o : options) {
            opts.push_back(String::From(o));
        }
        MultipleChoiceIndex(title, message, options, [options, onselect, onclose](int ind) {
            if(ind == -1) {
                if(onclose)
                    onclose();
            }
            else {
                if(onselect)
                    onselect(options[ind]);
            }
        }, close);
    }


    /// Asks a multiple choice question to the user with an optional close/cancel button
    /// Useful for a few options. Option value is passed to onselect function. options must
    /// outlive the dialog. Use MultipleChoiceIndex if this is not practical.
    template<class T_>
    void MultipleChoice(
        const std::string &message, 
        const std::vector<T_> &options, std::function<void(T_)> onselect, 
        CloseOption close = CloseOption::None, std::function<void(void)> onclose = {}
    ) {
        MultipleChoice<T_>("", message, options, onselect, close, onclose);
    }

    //disallow passing temporary vectors
    template<class T_>
    void MultipleChoice(
        const std::string &title, const std::string &message,
        const std::vector<T_> &&options, std::function<void(T_)> onselect,
        CloseOption close = CloseOption::None, std::function<void(void)> onclose = {}
    ) = delete;

    //disallow passing temporary vectors
    template<class T_>
    void MultipleChoice(
        const std::string &message,
        const std::vector<T_> &&options, std::function<void(T_)> onselect,
        CloseOption close = CloseOption::None, std::function<void(void)> onclose = {}
    ) = delete;

    /// Asks a multiple choice question to the user with an optional close/cancel button
    /// using the values from enumeration. Useful for a few options.
    template<class T_>
    typename std::enable_if<decltype(gorgon__enum_tr_loc(T_()))::isupgradedenum, void>::type 
    MultipleChoice(
        const std::string &title, const std::string &message, 
        std::function<void(T_)> onselect, CloseOption close = CloseOption::None, 
        std::function<void(void)> onclose = {}
    ) {
        std::vector<std::string> options;
        for(auto e : Enumerate<T_>()) {
            options.push_back(String::From(e));
        }
        MultipleChoiceIndex(title, message, options, [onselect, onclose](int ind) {
            if(ind == -1) {
                if(onclose) 
                    onclose();
            }
            else {
                if(onselect)
                    onselect(*(begin(Enumerate<T_>())+ind));
            }
        }, close);
    }

    /// Asks a multiple choice question to the user with an optional close/cancel button
    /// Useful for few options
    template<class T_>
    typename std::enable_if<decltype(gorgon__enum_tr_loc(T_()))::isupgradedenum, void>::type 
    MultipleChoice(
        const std::string &message, 
        std::function<void(T_)> onselect, CloseOption close = CloseOption::None, 
        std::function<void(void)> onclose = {}
    ) {
        MultipleChoice<T_>("", message, onselect, close, onclose);
    }


    /// Asks user to respond with yes or no, optionally with cancel
    void AskYesNo(
        const std::string &title, const std::string &message,
        std::function<void()> onyes, std::function<void()> onno = {},
        CloseOption close = CloseOption::None, std::function<void()> onclose = {}
    );

    /// Asks user to respond with yes or no, optionally with cancel
    inline void AskYesNo(
        const std::string &message,
        std::function<void()> onyes, std::function<void()> onno = {},
        CloseOption close = CloseOption::None, std::function<void()> onclose = {}
    ) {
        AskYesNo("", message, onyes, onno, close, onclose);
    }

    /// Changes the text of the close button. This change will effect only future
    /// dialogs.
    void SetCloseText(const std::string &value);

    /// Changes the text of the yes and no buttons. This change will effect only 
    /// future dialogs.
    void SetYesNoText(const std::string &yes, const std::string &no);

    /// Changes the text of the cancel button. This change will effect only future
    /// dialogs.
    void SetCancelText(const std::string &value);

} }

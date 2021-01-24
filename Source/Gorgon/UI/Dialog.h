#pragma once

#include "../Containers/Collection.h"
#include "../Widgets/DialogWindow.h"
#include "../Widgets/Inputbox.h"
#include "../Widgets/Label.h"
#include "../Enum.h"

namespace Gorgon { namespace UI {
    
    /// @cond internal
    namespace internal {
        void init();
        bool handledialogcopy(Input::Key key, float state, const std::string &message);
        void attachdialogcopy(Widgets::DialogWindow *diag, const std::string &title, const std::string &message);
        void closethis(Widgets::DialogWindow *diag);
        Geometry::Size negotiatesize(Widgets::DialogWindow *diag, Widget *text, bool allowshrink = true);
        void place(Widgets::DialogWindow *diag);
    }
    /// @endcond
    
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
    
    /// Asks user to confirm an action. There will be a cancel button to
    /// continue without confirming this dialog.
    void Confirm(
        const std::string &title, const std::string &message, 
        std::function<void()> onconfirm, 
        const std::string &confirmtext = "", const std::string &canceltext = ""
    );
    
    /// Asks user to confirm an action. There will be a cancel button to
    /// continue without confirming this dialog.
    inline void Confirm(
        const std::string &message, 
        std::function<void()> onconfirm, 
        const std::string &confirmtext = "", const std::string &canceltext = ""
    ) {
        Confirm("", message, onconfirm, confirmtext, canceltext);
    }

    /// Changes the text of the close button. This change will effect only future
    /// dialogs.
    void SetCloseText(const std::string &value);

    /// Changes the text of the yes and no buttons. This change will effect only 
    /// future dialogs.
    void SetYesNoText(const std::string &yes, const std::string &no);

    /// Changes the text of the ok button. This change will effect only future
    /// dialogs.
    void SetOkText(const std::string &value);

    /// Returns the text of the ok button.
    std::string GetOkText();

    /// Changes the text of the cancel button. This change will effect only future
    /// dialogs.
    void SetCancelText(const std::string &value);

    /// Returns the text of the cancel button.
    std::string GetCancelText();

    /// Requests an input from the user
    template<class T_>
    void Input(
        const std::string &title, const std::string &message, 
        const std::string &label, const T_ &def, 
        std::function<void(const T_ &)> onconfirm, 
        bool allowcancel = true,
        std::function<void()> oncancel = {}, 
        const std::string &confirmtext = "", const std::string &canceltext = ""
    ) {
        using namespace internal;
        
        init();
        
        auto diag = new Widgets::DialogWindow(title);
        diag->HideCloseButton(); //not to confuse user
        
        if(allowcancel) {
            auto &close = diag->AddButton(canceltext != "" ? canceltext : GetCancelText(), [diag, oncancel]() {
                closethis(diag);
                if(oncancel)
                    oncancel();
            });
            
            diag->SetCancel(close);
        }
        
        auto inp = new Widgets::Inputbox<T_>(def);
        inp->SelectAll();
        diag->Own(*inp);
        diag->Add(*inp);
        
        auto &confirm = diag->AddButton(confirmtext != "" ? confirmtext : GetOkText(), [diag, onconfirm, inp]() {
            closethis(diag);
            
            onconfirm(*inp);
        });
        
        diag->SetDefault(confirm);
        
        auto text = new Widgets::Label(message);
        text->SetAutosize(Autosize::Automatic, Autosize::Automatic);
        diag->Add(*text);
        diag->Own(*text);
        inp->Move(0, text->GetBounds().Bottom + diag->GetSpacing());
        
        Widgets::Label *l = nullptr;
        
        if(!label.empty()) {
            l = new Widgets::Label(label);
            l->SetHorizonalAutosize(Autosize::Automatic);
            l->Move(0, text->GetBounds().Bottom + diag->GetSpacing());
            l->SetHeight(inp->GetHeight());
            inp->Location.X = l->GetBounds().Right + diag->GetSpacing()*2;
            
            diag->Add(*l);
            diag->Own(*l);
        }
        
        diag->ResizeInterior({inp->GetBounds().Right, inp->GetBounds().Bottom});
        
        attachdialogcopy(diag, title, message);
        
        diag->ResizeInterior(negotiatesize(diag, text, false));
        
        if(l) {
            l->Move(text->Location.X, text->GetBounds().Bottom + text->Location.Y);
            l->SetHeight(inp->GetHeight());
            inp->Move(l->GetBounds().Right + diag->GetSpacing()*2, l->Location.Y);
            inp->SetWidth(diag->GetInteriorSize().Width - inp->Location.X - text->Location.X);
        }
        else {
            inp->Move(text->Location.X, text->GetBounds().Bottom + text->Location.Y);
            inp->SetWidth(diag->GetInteriorSize().Width - text->Location.X*2);
        }
        
        diag->ResizeInterior({diag->GetInteriorSize().Width, inp->GetBounds().Bottom});
        
        place(diag);
        diag->Center(); //input dialogs will be centered
    }
    
} }

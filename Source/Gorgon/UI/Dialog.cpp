#include "Dialog.h"

#include "../Widgets/Label.h"
#include "../Main.h"
#include "../Window.h"
#include "../Containers/GarbageCollection.h"

using namespace std::placeholders;

namespace Gorgon { namespace Widgets {
    
    bool ShouldBeCollected(Widgets::DialogWindow &wind) {
        return !wind.IsVisible();
    }
    
} }

namespace Gorgon { namespace UI {
    
namespace {
    std::string closetext = "Close";
    std::string canceltext = "Cancel";
    std::string oktext = "Ok";
    std::string yestext = "Yes";
    std::string notext = "No";
}
    

namespace internal {
    bool ready = false;
    
    Containers::GarbageCollection<Widgets::DialogWindow> dialogs;
    
    void init() {
        if(ready)
            return;
        
        BeforeFrameEvent.Register([] {
            dialogs.Collect();
        });
    }
    
    bool handledialogcopy(Input::Key key, float state, const std::string &message) {
        if(state == 1 && key == Input::Keyboard::Keycodes::C && Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Ctrl) {
            WindowManager::SetClipboardText(message);
            
            return true;
        }
        
        return false;
    }
    
    void attachdialogcopy(Widgets::DialogWindow *diag, const std::string &title, const std::string &message) {
        if(title.empty()) {
            diag->KeyEvent.Register(
                static_cast<std::function<bool(Input::Key, float)>>(
                    std::bind(&handledialogcopy, _1, _2, message)
                )
            );
        }
        else {
            diag->KeyEvent.Register(
                static_cast<std::function<bool(Input::Key, float)>>(
                    std::bind(&handledialogcopy, _1, _2, title + "\n---\n" + message)
                )
            );
        }
    }
    
    
    void closethis(Widgets::DialogWindow *diag) {
        if(internal::dialogs.GetCount() > 1) {
            if(internal::dialogs.Last().CurrentPtr() == diag) {
                (internal::dialogs.Last()-1)->Focus();
            }
            else {
                internal::dialogs.Last()->Focus();
            }
        }

        diag->Close();
    }

    Geometry::Size negotiatesize(Widgets::DialogWindow *diag, Widget *text, bool allowshrink) {
        text->SetWidth(diag->GetInteriorSize().Width);

        Geometry::Size sz;
        int maxw = int(diag->GetParent().GetInteriorSize().Width * 0.9);

        bool compact = false;
        
        for(int i=0; i<5; i++) { //maximum 5 iterations
            sz = text->GetSize();

            if(!allowshrink)
                sz.Width = diag->GetInteriorSize().Width;

            if(sz.Width < diag->GetUnitWidth() * 6) {
                //too small, use 4units at least
                sz.Width = diag->GetUnitWidth() * 6;
                break;
            }
            else if(sz.Width <= maxw && sz.Height > diag->GetHeight()) {
                //too high, increase width
                sz.Width = int(sz.Width * 1.5);
                if(sz.Width >= maxw) {
                    sz.Width = maxw;
                    return sz;
                }
                else {
                    diag->SetWidth(sz.Width);
                    text->SetWidth(diag->GetInteriorSize().Width);
                }
                
                compact = true;
            }
            else {
                break;
            }
        }
        
        if(compact) {
            text->Move(diag->GetSpacing(), diag->GetSpacing());
            sz.Width  += diag->GetSpacing()*2;
            sz.Height += diag->GetSpacing()*2;
        }
        else {
            text->Move(diag->GetSpacing()*4, diag->GetSpacing()*4);
            sz.Width  += diag->GetSpacing()*8;
            sz.Height += diag->GetSpacing()*8;
        }

        return sz;
    }

    //move, focus, add to dialogs
    void place(Widgets::DialogWindow *diag) {
        auto parent = &diag->GetParent();
        
        Geometry::Point location = {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
        
        if(dynamic_cast<LayerAdapter*>(parent)) {
            auto &layer = dynamic_cast<LayerAdapter*>(parent)->GetLayer();
            auto lp = dynamic_cast<Gorgon::Window*>(&layer.GetTopLevel());
            
            if(lp)
                location = lp->GetMouseLocation() - Geometry::Point(diag->GetSize()/2) - layer.GetEffectiveBounds().TopLeft();
        }
        if(dynamic_cast<Gorgon::Window*>(parent)) {
            location = dynamic_cast<Gorgon::Window*>(parent)->GetMouseLocation() - Geometry::Point(diag->GetSize()/2);
        }
        
        if(location != Geometry::Point{std::numeric_limits<int>::min(), std::numeric_limits<int>::min()}) {
            auto psz = diag->GetParent().GetInteriorSize();
            auto sz  = diag->GetSize();
            
            if(location.X < 0)
                location.X = 0;
            if(location.Y < 0)
                location.Y = 0;
            if(location.X + sz.Width > psz.Width)
                location.X = psz.Width - sz.Width;
            if(location.Y + sz.Height > psz.Height)
                location.Y = psz.Height - sz.Height;
            
            diag->Move(location);
        }
        else {
            auto sz = diag->GetParent().GetInteriorSize() - diag->GetSize();
            sz /= 10;
            diag->Move(Geometry::Point(sz * std::min(10, (int)internal::dialogs.GetCount()+3)));
        }
        
        diag->Focus();
        internal::dialogs.Add(diag);

    }
    
}

    using namespace internal;
    
    /**
     * @page uidialog UI dialog system
     * Dialog system contains commonly used UI dialogboxes. These dialogs have their lifetime 
     * managed internally. They can be created using a simple function call.
     * 
     * @see ShowMessage
     */

    void ShowMessage(const std::string &title, const std::string &message, std::function<void()> onclose, const std::string &buttontext) {
        internal::init();
        
        auto diag = new Widgets::DialogWindow(title);
        diag->HideCloseButton(); //not to confuse user
        
        auto &close = diag->AddButton(buttontext != "" ? buttontext : closetext, [diag]() {
            closethis(diag);
        });

        if(onclose)
            diag->ClosedEvent.Register(onclose);
        
        diag->SetCancel(close);
        diag->SetDefault(close);
        attachdialogcopy(diag, title, message);
        
        auto text = new Widgets::Label(message);
        text->SetAutosize(Autosize::Automatic, Autosize::Automatic);
        diag->Add(*text);
        diag->Own(*text);
        
        diag->ResizeInterior(negotiatesize(diag, text));
        place(diag);
    }

    void MultipleChoiceIndex(
        const std::string &title, const std::string &message,
        const std::vector<std::string> &options, std::function<void(int)> onselect,
        CloseOption close
    ) {
        internal::init();

        auto diag = new Widgets::DialogWindow(title);
        diag->HideCloseButton(); //not to confuse user

        
        auto text = new Widgets::Label(message);
        text->SetAutosize(Autosize::Automatic, Autosize::Automatic);
        diag->Add(*text);
        diag->Own(*text);

        std::string buttontexts = "";
        auto &btnsarea = diag->ButtonAreaOrganizer().GetAttached();
        int index = 0;
        for(auto opt : options) {
            auto &btn = diag->AddButton(opt, [index, onselect, diag]() {
                closethis(diag);
                onselect(index);
            });
            btn.SetWidthInUnits(10); //allow up to 10 units
            btn.SetHorizonalAutosize(Autosize::Automatic);
            
            buttontexts += opt + " | ";

            index++;
        }

        if(close != CloseOption::None) {
            auto &closebtn = diag->AddButton(close == CloseOption::Close ? closetext : canceltext, [diag, onselect]() {
                closethis(diag);
                onselect(-1);
            });

            diag->SetCancel(closebtn);
            closebtn.SetHorizonalAutosize(Autosize::Automatic);
            diag->Own(closebtn);
            
            buttontexts += (close == CloseOption::Close ? closetext : canceltext) + " | ";
        }
        
        if(buttontexts.empty())
                attachdialogcopy(diag, title, message);
        else
                attachdialogcopy(diag, title, message + "\n---\n" + buttontexts.substr(0, buttontexts.length()-3));

        int totw = 0;
        for(auto &w : btnsarea) {
            totw += w.GetWidth();
        }
        totw += diag->ButtonAreaOrganizer().GetSpacing() * (btnsarea.GetCount()-1);

        if(totw > diag->GetWidth())
            diag->ResizeInterior({totw, diag->GetInteriorSize().Height});

        diag->ResizeInterior(negotiatesize(diag, text, false));

        diag->ButtonAreaOrganizer().Reorganize();

        place(diag);
    }


    /// Asks user to respond with yes or no, optionally with cancel
    void AskYesNo(const std::string &title, const std::string &message, std::function<void()> onyes, std::function<void()> onno, CloseOption close, std::function<void()> onclose) {
        MultipleChoiceIndex(title, message, {notext, yestext}, [onyes, onno, onclose](int ind) {
            switch(ind) {
            case -1:
                if(onclose)
                    onclose();
                break;
            case 0:
                if(onno)
                    onno();
                break;
            case 1:
                if(onyes)
                    onyes();
                break;
            }
        }, close);
    }
    
    void Confirm(
        const std::string &title, const std::string &message, 
        std::function<void()> onconfirm, 
        const std::string &confirm, const std::string &cancel
    ) {
        MultipleChoiceIndex(title, message, {cancel== "" ? canceltext : cancel, confirm== "" ? oktext : confirm}, [onconfirm](int ind) {
            switch(ind) {
            case 1:
                if(onconfirm)
                    onconfirm();
                break;
            }
        });
    }

    void SetCloseText(const std::string &value) {
        closetext = value;
    }

    void SetOkText(const std::string &value) {
        oktext = value;
    }

    void SetCancelText(const std::string &value) {
        canceltext = value;
    }

    void SetYesNoText(const std::string &yes, const std::string &no) {
        yestext = yes;
        notext = no;
    }
    
    std::string GetOkText() {
        return oktext;
    }
    
    std::string GetCancelText() {
        return canceltext;
    }
    
    
} }

#include "Dialog.h"

#include "../Widgets/Label.h"
#include "../Main.h"
#include "../Containers/GarbageCollection.h"


namespace Gorgon { namespace Widgets {
    
    bool ShouldBeCollected(Widgets::DialogWindow &wind) {
        return !wind.IsVisible();
    }
    
} }

namespace Gorgon { namespace UI {
    
namespace {
    std::string closetext = "Close";
    std::string canceltext = "Cancel";
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
    
}
    /**
     * @page uidialog UI dialog system
     * Dialog system contains commonly used UI dialogboxes. These dialogs have their lifetime 
     * managed internally. They can be created using a simple function call.
     * 
     * @see ShowMessage
     */
    
    static void closethis(Widgets::DialogWindow *diag) {
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

    static Geometry::Size negotiatesize(Widgets::DialogWindow *diag, Widget *text, bool allowshrink = true) {
        text->SetWidth(diag->GetInteriorSize().Width);

        Geometry::Size sz;
        int maxw = int(diag->GetParent().GetInteriorSize().Width * 0.9);

        for(int i=0; i<5; i++) { //maximum 5 iterations
            sz = text->GetSize();

            if(!allowshrink)
                sz.Width = diag->GetInteriorSize().Width;

            if(sz.Width < diag->GetUnitWidth() * 4) {
                //too small, use 4units at least
                sz.Width = diag->GetUnitWidth() * 4;
                return sz;
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
            }
            else {
                return sz;
            }
        }

        return sz;
    }

    //move, focus, add to dialogs
    static void place(Widgets::DialogWindow *diag) {
        auto sz = diag->GetParent().GetInteriorSize() - diag->GetSize();
        sz /= 10;
        diag->Move(Geometry::Point(sz * std::min(10, (int)internal::dialogs.GetCount()+3)));

        diag->Focus();
        internal::dialogs.Add(diag);

    }
   
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
        
        auto text = new Widgets::Label(message);
        text->SetAutosize(Autosize::Automatic, Autosize::Automatic);
        diag->Add(*text);
        
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

        int cnt = int(options.size()) + (close != CloseOption::None);
        auto &btnsarea = diag->ButtonAreaOrganizer().GetAttached();
        int index = 0;
        for(auto opt : options) {
            diag->AddButton(opt, [index, onselect, diag]() {
                closethis(diag);
                onselect(index);
            }).SetHorizonalAutosize(Autosize::Automatic);

            index++;
        }

        if(close != CloseOption::None) {
            auto &closebtn = diag->AddButton(close == CloseOption::Close ? closetext : canceltext, [diag, onselect]() {
                closethis(diag);
                onselect(-1);
            });

            diag->SetCancel(closebtn);
            closebtn.SetHorizonalAutosize(Autosize::Automatic);
        }

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

    void SetCloseText(const std::string &value) {
        closetext = value;
    }

    void SetCancelText(const std::string &value) {
        canceltext = value;
    }

    void SetYesNoText(const std::string &yes, const std::string &no) {
        yestext = yes;
        notext = no;
    }
    
    
} }

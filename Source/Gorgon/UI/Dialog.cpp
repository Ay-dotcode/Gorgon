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
    
    void ShowMessage(const std::string &title, const std::string &message) {
        internal::init();
        
        auto diag = new Widgets::DialogWindow(title);
        diag->HideCloseButton();
        
        auto &close = diag->AddButton(closetext, [diag]() {
            if(internal::dialogs.GetCount() > 1) {
                if(internal::dialogs.Last().CurrentPtr() == diag) {
                    (internal::dialogs.Last()-1)->Focus();
                }
                else {
                    internal::dialogs.Last()->Focus();
                }
            }
            
            diag->Close();
        });
        
        diag->SetCancel(close);
        diag->SetDefault(close);
        
        auto text = new Widgets::Label(message);
        text->SetWidth(diag->GetInteriorSize().Width);
        text->SetAutosize(Autosize::Automatic, Autosize::Automatic);
        diag->Add(*text);
        
        //size agreement
        Geometry::Size sz;
        int maxw = diag->GetParent().GetInteriorSize().Width * 0.9;
        
        bool done = false;
        while(!done) {
            sz = text->GetSize();
            
            if(sz.Width < diag->GetUnitWidth() * 4) {
                //too small, use 4units at least
                sz.Width = diag->GetUnitWidth() * 4;
                done = true;
            }
            else if(sz.Width <= maxw && sz.Height > diag->GetHeight()) {
                //too high, increase width
                sz.Width *= 1.5;
                if(sz.Width >= maxw) {
                    sz.Width = maxw;
                    done = true;
                }
                else {
                    diag->SetWidth(sz.Width);
                    text->SetWidth(diag->GetInteriorSize().Width);
                }
            }
            else {
                done = true;
            }
        }
        
        diag->ResizeInterior(sz);
        
        sz = diag->GetParent().GetInteriorSize() - diag->GetSize();
        sz /= 10;
        diag->Move(Geometry::Point(sz * std::min(10, (int)internal::dialogs.GetCount()+3)));
        
        diag->Focus();
        internal::dialogs.Add(diag);
    }

    void ShowMessage(const std::string &title, const std::string &message, std::function<void()> onclose) {
        ShowMessage(title, message);
        internal::dialogs.Last()->ClosedEvent.Register(onclose);
    }
    
    void SetCloseText(const std::string &value) {
        closetext = value;
    }
    
    
    
} }

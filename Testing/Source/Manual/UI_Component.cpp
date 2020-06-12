#include "GraphicsHelper.h"

#include <Gorgon/UI/Template.h>
#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Graphics/EmptyImage.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/Graphics/Rectangle.h>
#include <Gorgon/Graphics/TintedObject.h>


std::string helptext = 
    "Key list:\n"
    "d\tToggle disabled\n"
    "left, right\tPrevious, next test\n"
    "1-8\tModify value of the stack [not active yet]\n"
    "esc\tClose\n"
;

using namespace Gorgon::UI;
namespace Color = Gorgon::Graphics::Color;

Graphics::RectangleProvider &coloredrect(Graphics::RGBA color) {
    //this will not leak
    auto &img = *new Graphics::BlankImage(10, 10, {color, 0.5});
    
    //as long as rectangle is destroyed
    auto &ret = *new Graphics::RectangleProvider(img);
    ret.OwnProviders();
    
    return ret;
}

Graphics::RectangleProvider &cyanrect() {
    static auto &rect = coloredrect(Graphics::Color::Cyan);
    
    return rect;
}

Graphics::RectangleProvider &redrect() {
    static auto &rect = coloredrect(Graphics::Color::Red);
    
    return rect;
}

Graphics::RectangleProvider &greenrect() {
    static auto &rect = coloredrect(Graphics::Color::Green);
    
    return rect;
}

Graphics::BlankImage &blankimage(Graphics::RGBA color) {
    auto &img = *new Graphics::BlankImage(10, 10, {color, 0.5});
    
    return img;
}


Graphics::BlankImage &whiteimg() {
    static auto &img = blankimage(Graphics::Color::White);
    
    return img;
}

Graphics::BlankImage &blackimg() {
    static auto &img = blankimage(Graphics::Color::Black);
    
    return img;
}


/***********
 * Tests
 * These functions will create their objects on the heap. They
 * will live until the program ends. Create the function and
 * add it to the tests vector.
 ***********/

struct TestData {
    std::string name, description;
    ComponentStack &stack;
};

TestData test_setborder(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetPosition(0,0);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set border", "White background (50x50) and a green border (starting from 10,10, size of 30x30). Result should be concentric squares, 10px size difference, white, green, white.", stack};
}


TestData test_setpadding(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetPosition(0,0);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set padding", "Two empty squares inside each other. Cyan on the outside, red inside. There should be 10x10 empty square at the middle. Borders should be 10px in size and should touch each other.", stack};
}



std::vector<std::function<TestData(Layer &)>> tests = {
    &test_setborder,
    &test_setpadding
};



Containers::Collection<Layer> layers;
Containers::Collection<ComponentStack> stacks;
std::vector<std::pair<std::string, std::string>> info;
    
int main() {
    Application app("uitest", "UI Component Test", helptext, 10, 8);

    Graphics::Layer grid;
    app.wind.Add(grid);
    
    Graphics::Layer textlayer;
    app.wind.Add(textlayer);
    
    int xs = 50, ys = 20, w = 500, h = 400;
    grid.Draw(xs-20, ys-2, 20, 2, Graphics::Color::White);
    grid.Draw(xs-2, ys-20, 2, 20, Graphics::Color::White);
    
    grid.Draw(w+xs, ys-2, 20, 2, Graphics::Color::White);
    grid.Draw(w+xs, ys-20, 2, 20, Graphics::Color::White);
    
    grid.Draw(xs-20, ys+h, 20, 2, Graphics::Color::White);
    grid.Draw(xs-2, ys+h, 2, 20, Graphics::Color::White);
    
    grid.Draw(w+xs, ys+h, 20, 2, Graphics::Color::White);
    grid.Draw(w+xs, ys+h, 2, 20, Graphics::Color::White);
    
    for(int x=1; x<w/100;x++) {
        grid.Draw(xs-1+x*100, ys-10, 2, 10, Graphics::Color::White);
        grid.Draw(xs-1+x*100, ys+h, 2, 10, Graphics::Color::White);
    }
    
    for(int x=1; x<w/50;x+=2) {
        grid.Draw(xs-1+x*50, ys-5, 2, 5, Graphics::Color::White);
        grid.Draw(xs-1+x*50, ys+h, 2, 5, Graphics::Color::White);
    }
    
    for(int y=1; y<h/100;y++) {
        grid.Draw(xs-10,ys-1+y*100, 10, 2, Graphics::Color::White);
        grid.Draw(xs+w, ys-1+y*100, 10, 2, Graphics::Color::White);
    }
    
    for(int y=1; y<h/50;y+=2) {
        grid.Draw(xs-5, ys-1+y*50, 5, 2, Graphics::Color::White);
        grid.Draw(xs+w, ys-1+y*50, 5, 2, Graphics::Color::White);
    }
    
    textlayer.Move(xs, ys+h+20);
    textlayer.Resize(w, 600-ys+h+20);

    
    Layer *activelayer = nullptr;
    ComponentStack *activestack = nullptr;
    
    for(auto &fn : tests) {
        if(activelayer)
            activelayer->Hide();
        
        activelayer = &layers.AddNew();
        
        activelayer->Move(xs, ys);
        activelayer->Resize(w, h);
        app.wind.Add(activelayer);
        
        auto data = fn(*activelayer);
        activestack = &data.stack;
        info.push_back({data.name, data.description});
        
        stacks.Push(activestack);
    }
    
    int ind = layers.GetSize() - 1;
    
    auto displaytext = [&]() {
        textlayer.Clear();
        
        app.stylarge.Print(textlayer, info[ind].first, 0,0, w);
        app.sty.Print(textlayer, info[ind].second, 0, app.stylarge.GetSize(info[ind].first, w).Height, w);
    };
    
    displaytext();
    
    app.wind.KeyEvent.Register([&](Gorgon::Input::Key key, float amount) {
        namespace Keycodes = Gorgon::Input::Keyboard::Keycodes;
        if(amount) {
            switch(key) {
            case Keycodes::Left:
                if(ind > 0) {
                    ind--;
                    activelayer->Hide();
                    activelayer = &layers[ind];
                    activelayer->Show();
                    
                    activestack = &stacks[ind];
                    
                    displaytext();
                }
                break;
                
            case Keycodes::Right:
                if(ind < layers.GetSize() - 1) {
                    ind++;
                    activelayer->Hide();
                    activelayer = &layers[ind];
                    activelayer->Show();
                    
                    activestack = &stacks[ind];
                    
                    displaytext();
                }
                break;
                
            case Keycodes::Escape:
                exit(0);
                break;
            }
        }
        
        return true;
    });

    while(true) {
        Gorgon::NextFrame();
        
    }

    return 0;
}


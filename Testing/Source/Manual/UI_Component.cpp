#include "GraphicsHelper.h"

#include <Gorgon/UI/Template.h>
#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Graphics/EmptyImage.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/Graphics/Rectangle.h>
#include <Gorgon/Graphics/TintedObject.h>
#include <Gorgon/Input/KeyRepeater.h>

std::string helptext = 
    "Key list:\n"
    "d\tToggle disabled\n"
    "left, right\tPrevious, next test\n"
    "c\tCopy title of the active test\n"
    "1234\tIncrease the value of the stack\n"
    "qwer\tDecrease the value of the stack\n"
    "Shift\tSlower value change\n"
    "Ctrl\tFaster value change, snaps to nearest 20%\n"
    "esc\tClose\n"
;

using namespace Gorgon::UI;
namespace Color = Gorgon::Graphics::Color;

//BEGIN Helpers

Graphics::RectangleProvider &coloredrect(Graphics::RGBA color) {
    //this will not leak
    auto &img = *new Graphics::BlankImage(10, 10, {color, 0.6});
    
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

Graphics::BlankImage &blankimage(Graphics::RGBA color, Geometry::Size size = {1, 1}) {
    auto &img = *new Graphics::BlankImage(size * 10, {color, 0.6});
    
    return img;
}


Graphics::BlankImage &whiteimg() {
    static auto &img = blankimage(Graphics::Color::White);
    
    return img;
}

Graphics::BlankImage &greenimg2x1() {
    static auto &img = blankimage(Graphics::Color::Green, {2, 1});
    
    return img;
}

Graphics::BlankImage &greenimg1x2() {
    static auto &img = blankimage(Graphics::Color::Green, {1, 2});
    
    return img;
}

Graphics::BlankImage &greenimg2x2() {
    static auto &img = blankimage(Graphics::Color::Green, {1, 2});
    
    return img;
}

Graphics::BlankImage &greenimg() {
    static auto &img = blankimage(Graphics::Color::Green);
    
    return img;
}

Graphics::BlankImage &redimg() {
    static auto &img = blankimage(Graphics::Color::Red);
    
    return img;
}

Graphics::BlankImage &blackimg() {
    static auto &img = blankimage(Graphics::Color::Black);
    
    return img;
}

Graphics::BlankImage &brownimg() {
    static auto &img = blankimage(Graphics::Color::Brown);
    
    return img;
}


Graphics::BlankImage &blueimg() {
    static auto &img = blankimage(Graphics::Color::Blue);
    
    return img;
}

Graphics::BlankImage &yellowimg() {
    static auto &img = blankimage(Graphics::Color::Yellow);
    
    return img;
}

Graphics::BlankImage &purpleimg() {
    static auto &img = blankimage(Graphics::Color::Purple);
    
    return img;
}

Graphics::BlankImage &grayimg() {
    static auto &img = blankimage(Graphics::Color::Grey);
    
    return img;
}

Graphics::BlankImage &orangeimg() {
    static auto &img = blankimage(Graphics::Color::Orange);
    
    return img;
}

//END helpers


//BEGIN tests

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

TestData test_setsize(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(20, 30);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set size", "20x30 green object on a 50x50 white background, should be aligned to top left.", stack};
}

TestData test_setsizepercent(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(5000, 3333, Gorgon::UI::Dimension::BasisPoint);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Set size percent", "30x20 green object on a 60x60 white background, should be aligned to top left.", stack};
}

TestData test_setsizepercent_percent(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(3333, 3333, Gorgon::UI::Dimension::BasisPoint);

    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(redimg());
    cont3.SetSize(50, 50, Gorgon::UI::Dimension::Percent);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Set size percent x2", "Size 20x20 and 30x30 objects on a 60x60 white background, should be aligned to top left. Objects are green and red and should be touching.", stack};
}

TestData test_setsizepercent_rel(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont2.SetPosition(0, 0);

    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(redimg());
    cont3.SetSize(50, 50, Gorgon::UI::Dimension::Percent);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Set size percent relative", "Size 20x20 and 20x30 objects on a 60x60 white background, should be aligned to top left. Objects are green and red and should be touching.", stack};
}

TestData test_sizepercent_center(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(50, 50, Gorgon::UI::Dimension::Percent);
    cont2.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::TopLeft);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Set size percent from center", "20x20 green object on a 80x80 white background, should be aligned to center from its top left.", stack};
}

TestData test_sizepercent_centerabs(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(50, 50, Gorgon::UI::Dimension::Percent);
    cont2.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::TopLeft);
    cont2.SetPositioning(cont2.Absolute);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Set size percent from center absolute", "20x20 green object on a 80x80 white background, should be aligned to center from its top left.", stack};
}

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
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set border", "White background (50x50) and a green border (starting from 10,10, size of 30x30). Result should be concentric squares, 10px size difference, white, green, white.", stack};
}

TestData test_setborder2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(70, 70);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetBorderSize(10,20,30,40);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set border", "This has a white border around a green rectangle 30x10 in size. Border size should be left: 10, top: 20, right: 30, bottom: 40", stack};
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
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set padding", "Two empty squares inside each other. Cyan on the outside, red inside. There should be 10x10 empty square at the middle. Borders should be 10px in size and should touch each other.", stack};
}

TestData test_setpadding2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(70, 70);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(20, 10, 30, 40);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set padding", "This has a cyan border around a green rectangle 20x20 in size. Padding should be left: 20, top: 10, right: 30, bottom: 40.", stack};
}

TestData test_setpadding_percent(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(100, 100);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(20, 10, 30, 40, Gorgon::UI::Dimension::Percent);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redimg());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set padding percent", "This has a cyan border around a red rectangle 50x50 in size. Padding should be left: 20, top: 10, right: 30, bottom: 40.", stack};
}

TestData test_setpadding_border(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(120, 120);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    cont1.SetPadding(20, 10, 30, 40, Gorgon::UI::Dimension::Percent);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set border and padding", "This has a cyan border around a red rectangle 50x50 in size. There is 10px border as well as padding. Padding should be left: 20, top: 10, right: 30, bottom: 40.", stack};
}

TestData test_setmargin_parent(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetMargin(10);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set margin", "Two empty squares inside each other. Cyan on the outside, red inside. There should be 10x10 empty square at the middle. Borders should be 10px in size and should touch each other.", stack};
}

TestData test_setmargin_parent_percent(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetMargin(20, Gorgon::UI::Dimension::Percent);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set margin percent", "Two empty squares inside each other. Cyan on the outside, red inside. There should be 10x10 empty square at the middle. Borders should be 10px in size and should touch each other.", stack};
}

TestData test_margin_parent_padding(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 60);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(10,20,10,10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetMargin(10);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Margin padding collapse", "Two empty squares inside each other. Cyan on the outside, red inside. There should be 10x10 empty square at the middle. Borders should be 10px in size and should touch each other apart from the top where it should have 10px space between two borders..", stack};
}

TestData test_setindent(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetIndent(10);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set indent", "Two empty squares inside each other. Cyan on the outside, red inside. There should be 10x10 empty square at the middle. Borders should be 10px in size and should touch each other.", stack};
}

TestData test_indent_padding(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetIndent(10);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set indent", "Two empty squares inside each other. Cyan on the outside, red inside. Inner border should be 10x10 and there should be 10px space between borders", stack};
}

TestData test_setpadding_negative(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(0, -10, 0, 0);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetMargin(10);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set padding negative", "Two empty squares inside each other. Cyan on the outside, red inside. There should be 10x20 empty square at the middle. Borders should be 10px in size and should touch each other apart from top which they should be on top of each other.", stack};
}

TestData test_setmargin_negative(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetMargin(-10, 0, -10, 0);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Set margin negative", "Two empty squares inside each other. Cyan on the outside, red inside. There should be 30x10 empty square at the middle. Borders should be 10px in size and should touch each other apart from left and right which they should be on top of each other.", stack};
}

TestData test_border_padding_margin_size(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(120, 120);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    cont1.SetPadding(20, 10, 10, 40, Gorgon::UI::Dimension::Percent);

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redimg());
    cont2.SetSize({50, Gorgon::UI::Dimension::Percent}, 40);
    cont2.SetMargin(10, 20, 30, -10, Gorgon::UI::Dimension::Pixel);

    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(greenimg());
    cont3.SetSize(20, 60, Gorgon::UI::Dimension::Pixel);
    cont3.SetMargin(30, -10, 0, 0, Gorgon::UI::Dimension::Pixel);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Set border, padding, margin and check relative sizing", "This has a cyan border around a 10x40 red object and 20x60 green object. There should be 30px between object. Red object should be 20px from top border, green object should be aligned to the red object from top.", stack};
}

TestData test_absanch_samepoint(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(70, 70);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    for(int i=1; i<=9; i++) 
        cont1.AddIndex(i);
    
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(10);
    
    Containers::Collection<Graphics::BlankImage> images = {
        redimg(), yellowimg(), orangeimg(),
        blueimg(), purpleimg(), brownimg(),
        greenimg(), grayimg(), whiteimg(),
    };
    
    for(int i=1; i<=9; i++) {
        auto &cont = temp.AddContainer(i, Gorgon::UI::ComponentCondition::Always);
        cont.Background.SetAnimation(images[i-1]);
        cont.SetPositioning(Gorgon::UI::ComponentTemplate::Absolute);
        cont.SetAnchor(Gorgon::UI::Anchor(i), Gorgon::UI::Anchor(i), Gorgon::UI::Anchor(i));
        cont.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
        cont.SetPosition(0,0);
    }
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Absolute anchoring", "9 components will be anchored in a cyan rectangle. Colors at top from left: red, yellow, orange; middle: blue, purple, brown; bottom: green, gray, white. Apart from the center component, all component should touch cyan border. No component should touch (not even corner to corner) each other (10px space around all).", stack};
}

TestData test_paranch_samepoint(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(70, 70);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    for(int i=1; i<=9; i++) 
        cont1.AddIndex(i);
    
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(10);
    
    Containers::Collection<Graphics::BlankImage> images = {
        redimg(), yellowimg(), orangeimg(),
        blueimg(), purpleimg(), brownimg(),
        greenimg(), grayimg(), whiteimg(),
    };
    
    for(int i=1; i<=9; i++) {
        auto &cont = temp.AddContainer(i, Gorgon::UI::ComponentCondition::Always);
        cont.Background.SetAnimation(images[i-1]);
        cont.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor(i), Gorgon::UI::Anchor(i));
        cont.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
        cont.SetPosition(0,0);
    }
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Parent anchoring", "9 components will be anchored in a cyan rectangle. Colors at top from left: red, yellow, orange; middle: blue, purple, brown; bottom: green, gray, white. Apart from the center component, all component should touch cyan border. No component should touch (not even corner to corner) each other (10px space around all).", stack};
}

TestData test_absanch_centertopoint(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    for(int i=1; i<=9; i++) 
        cont1.AddIndex(i);
    
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetPadding(10);
    
    Containers::Collection<Graphics::BlankImage> images = {
        redimg(), yellowimg(), orangeimg(),
        blueimg(), purpleimg(), brownimg(),
        greenimg(), grayimg(), whiteimg(),
    };
    
    for(int i=1; i<=9; i++) {
        auto &cont = temp.AddContainer(i, Gorgon::UI::ComponentCondition::Always);
        cont.Background.SetAnimation(images[i-1]);
        cont.SetPositioning(Gorgon::UI::ComponentTemplate::Absolute);
        cont.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor(i), Gorgon::UI::Anchor::MiddleCenter);
        cont.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
        cont.SetPosition(0,0);
    }
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Absolute anchoring center to anchor", "9 components will be anchored in a cyan rectangle. Colors at top from left: red, yellow, orange; middle: blue, purple, brown; bottom: green, gray, white. Apart from the center component, all component should overlap cyan border. No component should touch (not even corner to corner) each other (10px space around all).", stack};
}

TestData test_absanchoff(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(blueimg());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont2.SetPosition(10, 20);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Absolute anchoring from top-left", "Blue object of size 20x20 on white background with 10, 20 pixel from top.", stack};
}

TestData test_absanchoffsize(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetPositioning(UI::ComponentTemplate::Absolute);
    cont2.SetPosition(10, 20);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Absolute anchoring from top-left", "Green object in a cyan rectangle with 10, 20 pixel from border. It should fill the rest of the space.", stack};
}

TestData test_absanchoffrev(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(blueimg());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont2.SetAnchor(UI::Anchor::None, UI::Anchor::BottomRight, UI::Anchor::BottomRight);
    cont2.SetPositioning(UI::ComponentTemplate::Absolute);
    cont2.SetPosition(10, 20);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Absolute anchoring from bottom-right", "Blue object of size 20x20 on white background with 10, 20 pixel from bottom right.", stack};
}

TestData test_relanch(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);

    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(redimg());
    cont3.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Relative anchoring", "Size 20x20 and 20x20 objects on a 60x60 white background, first one should be aligned to top left, second should be left of first one. Objects are green and red and should be touching.", stack};
}

TestData test_relanchvert(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.SetOrientation(Gorgon::Graphics::Orientation::Vertical);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);

    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(redimg());
    cont3.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont3.SetAnchor(Gorgon::UI::Anchor::BottomLeft, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Relative anchoring vertical", "Size 20x20 and 20x20 objects on a 60x60 white background, first one should be aligned to top left, second should be under the first one. Objects are green and red and should be touching.", stack};
}

TestData test_relanch2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);

    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(redimg());
    cont3.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont3.SetAnchor(UI::Anchor::BottomRight, UI::Anchor::BottomLeft, UI::Anchor::TopLeft);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Relative anchoring", "Size 20x20 and 20x20 objects on a 60x60 white background, first should be aligned to top left. Objects are green and red and should be touching from the corners.", stack};
}

TestData test_anchbaseline(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.AddIndex(3);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont2.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::FirstBaselineLeft, UI::Anchor::FirstBaselineLeft);

    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(redimg());
    cont3.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont3.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::FirstBaselineLeft, UI::Anchor::FirstBaselineLeft);
    
    static Graphics::BitmapFont fnt;
    fnt.SetBaseline(20);
    auto &cont4 = temp.AddTextholder(3, Gorgon::UI::ComponentCondition::Always);
    cont4.SetRenderer(fnt);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Baseline anchoring", "Size 20x20 and 20x20 objects on a 60x60 white background, first should be aligned to top left. Objects are green and red and should be touching from the corners.", stack};
}

TestData test_anchsetbaseline(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.AddIndex(3);
    cont1.SetBaseline(30);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(greenimg());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont2.SetBaseline(10);
    cont2.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::FirstBaselineLeft, UI::Anchor::FirstBaselineLeft);

    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(redimg());
    cont3.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont3.SetBaseline(20);
    cont3.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::FirstBaselineLeft, UI::Anchor::FirstBaselineLeft);
    
    static Graphics::BitmapFont fnt;
    fnt.SetBaseline(20);
    auto &cont4 = temp.AddTextholder(3, Gorgon::UI::ComponentCondition::Always);
    cont4.SetRenderer(fnt);

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Baseline anchoring using set baseline", "Size 20x20 and 20x20 objects on a 60x60 white background, first should be aligned to left, 20px from the top; second should be touching first object, should be from 10px from the top border. Objects are green and red.", stack};
}

TestData test_anchbaseline2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(60, 60);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.SetBaseline(30);
    cont1.Background.SetAnimation(whiteimg());

    static Graphics::BitmapFont fnt;
    fnt.AddGlyph(' ', redimg());
    fnt.SetBaseline(10);
    
    static Graphics::BitmapFont fnt2;
    fnt2.AddGlyph(' ', greenimg1x2());
    fnt2.SetBaseline(20);
    
    auto &cont2 = temp.AddTextholder(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetRenderer(fnt);
    cont2.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::FirstBaselineLeft, UI::Anchor::FirstBaselineLeft);
    cont2.SetText(" ");
    cont2.SetSize(10,10);
    
    auto &cont3 = temp.AddTextholder(2, Gorgon::UI::ComponentCondition::Always);
    cont3.SetRenderer(fnt2);
    cont3.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::FirstBaselineLeft, UI::Anchor::FirstBaselineLeft);
    cont3.SetText(" ");
    cont3.SetSize(10,10);
    

    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();

    layer.Add(stack);

    return {"Baseline anchoring between textholders", "Size 10x10 and 10x20 objects on a 60x60 white background, first should be aligned to left, 20px from the top; second should be touching first object, should be from 10px from the top border. Objects are red and green.", stack};
}

TestData test_abssliding(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(20, 0, Gorgon::UI::Dimension::Percent);
    
    
    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(greenrect());
    cont3.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont3.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont3.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont3.SetMargin(10, 0, 10, 0);
    cont3.SetPosition(3333, 10000, Gorgon::UI::Dimension::BasisPoint);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"AbsoluteSliding", "80x40 cyan rectangle contains 10x10 red and green rectangle. Red rectangle should be touching cyan from top, it should be 10px from the left border. Green rectangle should be touching to the bottom of cyan border, 20px from the left. Inner rectangles should be touching on the corners.", stack};
}

TestData test_abspolar(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(100, 60);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::PolarAbsolute);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::MiddleCenter, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(10, 90);
    
    
    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(greenrect());
    cont3.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont3.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::MiddleCenter, Gorgon::UI::Anchor::TopLeft);
    cont3.SetPositioning(Gorgon::UI::ComponentTemplate::PolarAbsolute);
    cont3.SetMargin(10, 0, 10, 0);
    cont3.SetPosition(50, 50, Gorgon::UI::Dimension::Percent);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
        
    layer.Add(stack);
    
    return {"PolarAbsolute", "100x60 white background contains 20x20 red and green rectangle. Red rectangle should be 10px from top, centered. Green rectangle should be at the middle 30px from the left.", stack};
}

TestData test_abspolar2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(120, 70);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.SetCenter(50, 0, Gorgon::UI::Dimension::Percent);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::PolarAbsolute);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopCenter, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition({40, UI::Dimension::Percent}, {200, UI::Dimension::EM});
    cont2.SetCenter(50, 0, Gorgon::UI::Dimension::Percent);
    
    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Background.SetAnimation(greenrect());
    cont3.SetSize(20, 20, Gorgon::UI::Dimension::Pixel);
    cont3.SetPositioning(Gorgon::UI::ComponentTemplate::PolarAbsolute);
    cont3.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::BottomCenter, Gorgon::UI::Anchor::TopLeft);
    cont3.SetPosition(100, 100, Gorgon::UI::Dimension::Percent);
    cont3.SetCenter(50, 100, Gorgon::UI::Dimension::Percent);
    cont3.SetMargin(0, 100, Gorgon::UI::Dimension::Percent);
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
        
    layer.Add(stack);
    
    return {"PolarAbsolute 2", "100x60 white background contains 20x20 red and green rectangle. Red rectangle should be 20px from top, in centered. Green rectangle should be at bottom left corner.", stack};
}



TestData test_modify_position_sliding(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify position sliding", "80x40 cyan rectangle contains 10x10 red rectangle. It should move from left to right with the change with the value of channel 1.", stack};
}

TestData test_modify_position_slidingvert(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(40, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    cont1.SetOrientation(Gorgon::Graphics::Orientation::Vertical);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify position sliding vertical", "40x80 cyan rectangle contains 10x10 red rectangle. It should move from left to right with the change with the value of channel 1.", stack};
}

TestData test_modify_rev(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    cont2.SetValueRange(0, 1, 0);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Reverse modify", "80x40 cyan rectangle contains 10x10 red rectangle. It should move from right to left with the change with the value of channel 1.", stack};
}

TestData test_modify_minmax(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    cont2.SetValueRange(0, 0.2, 0.8);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Value range", "80x40 cyan rectangle contains 10x10 red rectangle. It should move from 10px left to 10px to right with the change with the value of channel 1.", stack};
}

TestData test_modify_chmap2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    cont2.SetValueOrdering(1, 0, 0, 0);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Channel mapping 2", "80x40 cyan rectangle contains 10x10 red rectangle. It should move from left to right with the change with the value of channel 2.", stack};
}

TestData test_modify_chmap3(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    cont2.SetValueOrdering(2, 0, 0, 0);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Channel mapping 3", "80x40 cyan rectangle contains 10x10 red rectangle. It should move from left to right with the change with the value of channel 3.", stack};
}

TestData test_modify_chmap4(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    cont2.SetValueOrdering(3, 0, 0, 0);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Channel mapping 4", "80x40 cyan rectangle contains 10x10 red rectangle. It should move from left to right with the change with the value of channel 4.", stack};
}

TestData test_modify_ch4_minmax(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    cont2.SetValueOrdering(3, 0, 0, 0);
    cont2.SetValueRange(0, 0.2, 0.8);
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Channel map value range", "80x40 cyan rectangle contains 10x10 red rectangle. It should move from 10px left to 10px to right with the change with the value of channel 1.", stack};
}

TestData test_modify_position_2ch(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    cont2.SetValueSource(cont2.UseXY);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify XY position sliding", "80x80 cyan rectangle contains 10x10 red rectangle. It should move from left to right with the change with the value of channel 1, top to bottom with 2.", stack};
}

TestData test_modify_position_absolute(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(70, 70);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::Absolute);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition);
    cont2.SetValueSource(cont2.UseXY);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify XY position absolute", "70x70 cyan rectangle contains 10x10 red rectangle. It should move from left to right with the change with the value of channel 1, top to bottom with 2. At the right and bottom, component should pass the boundaries.", stack};
}

TestData test_modify_x(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyX);
    cont2.SetValueSource(cont2.UseX);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify X", "80x80 cyan rectangle contains 10x10 red rectangle. It should move from left to right with the change with the value of channel 1. Other channel should not have any effect.", stack};
}

TestData test_modify_y(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redrect());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyY);
    cont2.SetValueSource(cont2.UseX);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify Y", "80x80 cyan rectangle contains 10x10 red rectangle. It should move from top to bottom with the change with the value of channel 1. Other channel should not have any effect.", stack};
}

TestData test_modify_size(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redimg());
    cont2.SetSize(0, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifySize);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify size", "80x40 cyan rectangle contains 10px high red rectangle. It should scale from left to right with the change with the value of channel 1.", stack};
}

TestData test_modify_sizevert(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(40, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetOrientation(Gorgon::Graphics::Orientation::Vertical);
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redimg());
    cont2.SetSize(10, 0, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifySize);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify size vertical", "40x80 cyan rectangle contains 10px wide red rectangle. It should scale from top to bottom with the change with the value of channel 1.", stack};
}

TestData test_modify_sizemin(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redimg());
    cont2.SetSize(10, 10, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifySize);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify size set minimum", "80x40 cyan rectangle contains 10px high red rectangle. It should scale from left to right with the change with the value of channel 1 but it should have minimum of 10px.", stack};
}

TestData test_modify_sizexy(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 40);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redimg());
    cont2.SetSize(0, 0, Gorgon::UI::Dimension::Pixel);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetAnchor(Gorgon::UI::Anchor::None, Gorgon::UI::Anchor::TopLeft, Gorgon::UI::Anchor::TopLeft);
    cont2.SetPosition(0, 0, Gorgon::UI::Dimension::Percent);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifySize);
    cont2.SetValueSource(Gorgon::UI::ComponentTemplate::UseSize);
    
    
    
    auto &stack = *new ComponentStack(temp);
    stack.HandleMouse();
    
    layer.Add(stack);
    
    return {"Modify size width & height", "80x40 cyan rectangle contains red rectangle. It should scale from left to right with the change with the value of channel 1 and top to bottom with the value of channel 2.", stack};
}


std::vector<std::function<TestData(Layer &)>> tests = {
    &test_setsize,
    &test_setsizepercent,
    &test_setsizepercent_percent,
    &test_setsizepercent_rel,
    &test_sizepercent_center,
    &test_sizepercent_centerabs,

    &test_setborder,
    &test_setborder2,
    &test_setpadding,
    &test_setpadding2,
    &test_setpadding_percent,
    &test_setpadding_border,
    &test_setmargin_parent,
    &test_setmargin_parent_percent,
    &test_margin_parent_padding,
    &test_setindent,
    &test_indent_padding,
    &test_setpadding_negative,
    &test_setmargin_negative,
    &test_border_padding_margin_size,

    &test_absanch_samepoint,
    &test_paranch_samepoint,
    &test_absanch_centertopoint,
    &test_absanchoff,
    &test_absanchoffsize,
    &test_absanchoffrev,
    
    &test_relanch,
    &test_relanch2,
    &test_relanchvert,
    
    &test_anchbaseline,
    &test_anchsetbaseline,
    &test_anchbaseline2,
    
    &test_abssliding,
    &test_abspolar,
    &test_abspolar2,
    
    &test_modify_position_sliding,
    &test_modify_position_slidingvert,
    &test_modify_rev,
    &test_modify_minmax,
    &test_modify_chmap2,
    &test_modify_chmap3,
    &test_modify_chmap4,
    &test_modify_ch4_minmax,
    &test_modify_position_2ch,    
    &test_modify_position_absolute,
    &test_modify_x,
    &test_modify_y,
    
    &test_modify_size,
    &test_modify_sizevert,
    &test_modify_sizemin,
    &test_modify_sizexy,
};

//END tests

Containers::Collection<Layer> layers;
Containers::Collection<ComponentStack> stacks;
std::vector<std::pair<std::string, std::string>> info;
Input::KeyRepeater repeater;
    
int main() {
    helptext += String::Concat("\nTotal tests:\t", tests.size());
    Application app("uitest", "UI Component Test", helptext, 10, 8);

    int xs = 50, ys = 20, w = 500, h = 400;
    
    Graphics::Layer grid;
    app.wind.Add(grid);
    
    Graphics::Layer textlayer;
    app.wind.Add(textlayer);
    textlayer.Move(xs, ys+h+20);
    textlayer.Resize(w, 600-ys+h+20);
    
    Graphics::Layer datalayer;
    app.wind.Add(datalayer);
    datalayer.Move(w + xs *2, h-50);
    datalayer.Resize(app.wind.GetWidth() - w - xs*2 - 10, 50);
    
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
    
    auto displayvalue = [&]() {
        datalayer.Clear();
        if(activestack)
            app.stylarge.Print(datalayer, 
                String::Concat(
                    "Value 1:\t", int(activestack->GetValue()[0]*100),"\n",
                    "Value 2:\t", int(activestack->GetValue()[1]*100),"\n",
                    "Value 3:\t", int(activestack->GetValue()[2]*100),"\n",
                    "Value 4:\t", int(activestack->GetValue()[3]*100),"\n"
                ),
                0,0, datalayer.GetWidth(), Gorgon::Graphics::TextAlignment::Left
            );
    };
    
    auto displaytext = [&]() {
        textlayer.Clear();
        
        app.stylarge.Print(textlayer, info[ind].first, 0,0, w);
        app.sty.Print(textlayer, info[ind].second, 0, app.stylarge.GetSize(info[ind].first, w).Height, w);
        
        displayvalue();
    };
    
    displaytext();
    
    namespace Keycodes = Gorgon::Input::Keyboard::Keycodes;
    repeater.Register(
        Keycodes::Number_1,
        Keycodes::Number_2,
        Keycodes::Number_3,
        Keycodes::Number_4,
        Keycodes::Q,
        Keycodes::W,
        Keycodes::E,
        Keycodes::R
    );
    repeater.SetInitialDelay(200);
    repeater.SetDelay(50);
    
    repeater.Repeat.Register([&](Gorgon::Input::Key key) {
        if(!activestack)
            return;
        
        auto v = activestack->GetValue();
        
        float p = 100.f;
        float ch = 0.01f;
        
        if(Input::Keyboard::CurrentModifier & Input::Keyboard::Modifier::Ctrl) {
            p = 10.f;
            ch = 0.1f;
        }
        
        switch(key) {
        case Keycodes::Number_1:
            activestack->SetValue(std::round(Clamp(v[0]+ch, 0.0f, 1.0f)*p)/p);
            break;
        case Keycodes::Number_2:
            activestack->SetValue(v[0], std::round(Clamp(v[1]+ch, 0.0f, 1.0f)*p)/p);
            break;
        case Keycodes::Number_3:
            activestack->SetValue(v[0], v[1], std::round(Clamp(v[2]+ch, 0.0f, 1.0f)*p)/p);
            break;
        case Keycodes::Number_4:
            activestack->SetValue(v[0], v[1], v[2], std::round(Clamp(v[3]+ch, 0.0f, 1.0f)*p)/p);
            break;
        case Keycodes::Q:
            activestack->SetValue(std::round(Clamp(v[0]-ch, 0.0f, 1.0f)*p)/p);
            break;
        case Keycodes::W:
            activestack->SetValue(v[0], std::round(Clamp(v[1]-ch, 0.0f, 1.0f)*p)/p);
            break;
        case Keycodes::E:
            activestack->SetValue(v[0], v[1], std::round(Clamp(v[2]-ch, 0.0f, 1.0f)*p)/p);
            break;
        case Keycodes::R:
            activestack->SetValue(v[0], v[1], v[2], std::round(Clamp(v[3]-ch, 0.0f, 1.0f)*p)/p);
            break;
        }
        
        displayvalue();
    });
    
    app.wind.KeyEvent.Register([&](Gorgon::Input::Key key, float amount) {
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
                
            case Keycodes::C:
                WindowManager::SetClipboardText(info[ind].first);
                break;

            case Keycodes::Escape:
                exit(0);
                break;
                
            case Keycodes::Shift:
                repeater.SetInitialDelay(500);
                repeater.SetDelay(500);
                break;
            }
        }
        else {
            switch(key) {
            case Keycodes::Shift:
                repeater.SetInitialDelay(200);
                repeater.SetDelay(50);
                break;
            }
        }
        
        repeater.KeyEvent(key, amount);
        
        return true;
    });

    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}


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
    "left\tPrevious\n"
    "right\tNext test\n"
    "c\tCopy title of the active test\n"
    "1234\tIncrease the value of the stack\n"
    "qwer\tDecrease the value of the stack\n"
    "Shift\tSlower value change\n"
    "Ctrl\tFaster value change, snaps to nearest 20%\n"
    "56\tChange text, title\n"
    "78\tChange icon1, icon2\n"
    "esc\tClose\n"
;

using namespace Gorgon::UI;
namespace Color = Gorgon::Graphics::Color;

struct TestData {
    std::string name, description;
    ComponentStack &stack;
};

extern std::vector<std::function<TestData(Layer &)>> tests;

Application &getapp() {
    auto help = helptext + String::Concat("\nTotal tests:\t", tests.size());
    static Application app("uitest", "UI Component Test", help, 10, 8);
    
    return app;
}

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
    static auto &img = blankimage(Graphics::Color::Green, {2, 2});
    
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

Graphics::BitmapAnimationProvider &makecoloranim() {
    auto &anim = *new Graphics::BitmapAnimationProvider;
    
    static decltype(Color::Red) colors[] = {Color::Red, Color::Orange, Color::Yellow, Color::Green, Color::Cyan, Color::Blue, Color::Magenta, Color::White, Color::Grey, Color::Black};
    
    for(auto color : colors) {
        auto &img = *new Graphics::Bitmap(10, 10, Gorgon::Graphics::ColorMode::RGBA);
        
        img.ForAllPixels([&](int x, int y) {
            img.SetRGBAAt(x, y, color);
        });
        
        img.Prepare();
        anim.Add(img, 100);
        anim.Own(img);
    }
    
    return anim;
}

Graphics::BitmapAnimationProvider &coloranim() {
    static auto &anim = makecoloranim();
    
    return anim;
}


const std::string teststrings[] = {
    "Hi",
    "Hello",
    "Hello world!",
    "This is a very long text."
};

const Graphics::BlankImage *testimages[] = {
    &orangeimg(),
    &greenimg1x2(),
    &greenimg2x1(),
    &greenimg2x2(),
    &redimg()
};

//END helpers


//BEGIN tests

/***********
 * Tests
 * These functions will create their objects on the heap. They
 * will live until the program ends. Create the function and
 * add it to the tests vector.
 ***********/

TestData test_graphic(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Graphic component", "10x10 green object on a 50x50 white background, should be aligned to top left.", stack};
}

TestData test_text(Layer &layer) {
    auto &app = getapp();

    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddTextholder(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetText("Hello");
    cont2.SetColor(Color::Brown);
    cont2.SetRenderer(app.fntlarge);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Text component", "Brown Hello on 50x50 white background, should be aligned to top left.", stack};
}

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
    
    layer.Add(stack);
    
    return {"Set border", "This has a white border around a green rectangle 30x10 in size. Border size should be left: 10, top: 20, right: 30, bottom: 40", stack};
}

TestData test_setoverlay(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetOverlayExtent(10);
    cont1.SetBorderSize(10);
    cont1.Overlay.SetAnimation(greenrect());
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Set overlay", "White background (50x50) and a green border (starting from 10,10, size of 30x30). Result should be concentric squares, 10px size difference, white, green, white.", stack};
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
    
    layer.Add(stack);
    
    return {"Channel map value range", "80x40 cyan rectangle contains 10x10 red rectangle. It should move from 10px left to 10px to right with the change with the value of channel 4.", stack};
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
    
    layer.Add(stack);
    
    return {"Modify size width & height", "80x40 cyan rectangle contains red rectangle. It should scale from left to right with the change with the value of channel 1 and top to bottom with the value of channel 2.", stack};
}

TestData test_modify_alpha(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Background.SetAnimation(redimg());
    cont2.SetSize(60, 60, Gorgon::UI::Dimension::Pixel);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyAlpha);
    cont2.SetValueSource(cont2.UseFirst);
    
    
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Modify blend", "80x80 cyan rectangle contains 60x60 red rectangle. It should appear with the change with the value of channel 1.", stack};
}

TestData test_modify_blend1(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(whiteimg());
    cont2.SetSize(60, 60, Gorgon::UI::Dimension::Pixel);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::BlendColor);
    cont2.SetValueSource(cont2.UseFirst);
    cont2.SetColor(Color::Red);
    cont2.SetTargetColor(Color::AppleGreen);
    
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Blend", "80x80 cyan rectangle contains 60x60 red rectangle. Its color should change to green with the change with the value of channel 1.", stack};
}

TestData test_modify_blend2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(whiteimg());
    cont2.SetSize(60, 60, Gorgon::UI::Dimension::Pixel);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::BlendColor);
    cont2.SetValueSource(cont2.UseRA);
    cont2.SetColor(Color::Red);
    cont2.SetTargetColor({Color::AppleGreen, 0.f});
    
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Blend color & alpha", "80x80 cyan rectangle contains 60x60 red rectangle. Its color should change to green with the change with the value of channel 1, alpha will be modified from 60% to 0% with the value of channel 4.", stack};
}

TestData test_modify_blend4(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(whiteimg());
    cont2.SetSize(60, 60, Gorgon::UI::Dimension::Pixel);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::BlendColor);
    cont2.SetValueSource(cont2.UseRGBA);
    cont2.SetColor(Color::Black);
    cont2.SetTargetColor({Color::White, 0.f});
    
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Blend color & alpha", "80x80 cyan rectangle contains 60x60 black rectangle. Its color should be controllable using all channels. Alpha is reversed.", stack};
}

TestData test_modify_animation(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(cyanrect());
    cont1.SetBorderSize(10);
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(coloranim());
    cont2.SetSize(60, 60, Gorgon::UI::Dimension::Pixel);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyAnimation);
    cont2.SetValueSource(cont2.UseFirst);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Animation", "80x80 cyan rectangle contains a 60x60 rectangle. It is an animation with 10 colors (red, orange, yellow, green, cyan, blue, magenta, white, grey, black), channel 1 can be used to control the animation frame. 0-10 should be red, orange starts from 11, yellow starts from 21 and so on.", stack};
}

TestData test_data_settext(Layer &layer) {
    auto &app = getapp();

    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddTextholder(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetDataEffect(Gorgon::UI::ComponentTemplate::Text);
    cont2.SetColor(Color::Brown);
    cont2.SetRenderer(app.fntlarge);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Set text data", "Brown text obtained from text data on 50x50 white background, should be aligned to top left. Extra text should overflow.", stack};
}

TestData test_data_textwrap(Layer &layer) {
    auto &app = getapp();

    auto &temp = *new Template;
    temp.SetSize(70, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddTextholder(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetDataEffect(Gorgon::UI::ComponentTemplate::Text);
    cont2.SetColor(Color::Brown);
    cont2.SetRenderer(app.fntlarge);
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Text wrap", "Brown text obtained from text data on 70x50 white background, should be aligned to top left. Extra text should wrap but completely visible.", stack};
}

TestData test_data_textclip(Layer &layer) {
    auto &app = getapp();

    auto &temp = *new Template;
    temp.SetSize(70, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddTextholder(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetDataEffect(Gorgon::UI::ComponentTemplate::Text);
    cont2.SetColor(Color::Brown);
    cont2.SetRenderer(app.fntlarge);
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetClip(true);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Text clipping", "Brown text obtained from text data on 70x50 white background, should be aligned to top left. Extra text should wrap and clipped.", stack};
}

TestData test_data_settitle(Layer &layer) {
    auto &app = getapp();

    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddTextholder(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetDataEffect(Gorgon::UI::ComponentTemplate::Title);
    cont2.SetColor(Color::Brown);
    cont2.SetRenderer(app.fntlarge);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Set title data", "Brown text obtained from title data on 50x50 white background, should be aligned to top left. Extra text should overflow.", stack};
}

TestData test_data_settexttitle(Layer &layer) {
    auto &app = getapp();

    auto &temp = *new Template;
    temp.SetSize(120, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddTextholder(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetDataEffect(Gorgon::UI::ComponentTemplate::Title);
    cont2.SetColor(Color::Brown);
    cont2.SetRenderer(app.fntlarge);
    
    auto &cont3 = temp.AddTextholder(2, Gorgon::UI::ComponentCondition::Always);
    cont3.SetDataEffect(Gorgon::UI::ComponentTemplate::Text);
    cont3.SetColor(Color::DarkGreen);
    cont3.SetRenderer(app.fnt);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Set text data, title", "Brown text obtained from title data followed by small green text obtained from text on 120x50 white background, should be aligned to top left. Extra text should overflow. Two text components should be aligned from the baseline.", stack};
}

TestData test_data_setimage(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddPlaceholder(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetDataEffect(Gorgon::UI::ComponentTemplate::Icon1);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Set image data", "Selected image obtained from icon 1 data on 50x50 white background, should be aligned to top left.", stack};
}

TestData test_data_setimagegt(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.SetDataEffect(Gorgon::UI::ComponentTemplate::Icon1);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Set image data graphics", "Selected image obtained from icon 1 data on 50x50 white background, should be aligned to top left.", stack};
}

TestData test_data_setimage2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());
    
    auto &cont2 = temp.AddPlaceholder(1, Gorgon::UI::ComponentCondition::Icon1IsSet);
    cont2.SetDataEffect(Gorgon::UI::ComponentTemplate::Icon1);
    
    auto &cont3 = temp.AddPlaceholder(2, Gorgon::UI::ComponentCondition::Always);
    cont3.SetDataEffect(Gorgon::UI::ComponentTemplate::Icon2);
    cont3.SetAnchor(Gorgon::UI::Anchor::BottomRight, Gorgon::UI::Anchor::BottomLeft, Gorgon::UI::Anchor::BottomLeft);
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    return {"Set image data 2", "Selected image obtained from icon 1 data and icon 2 data on 50x50 white background, should be aligned to top left. Second image should be aligned to bottom right of the first one. If the first one is empty, it should anchor to the bottom left of its parent.", stack};
}

TestData test_data_setstate(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(90, 30);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetPadding(0, 10);
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyX);
    cont2.SetSize(10, 10);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &cont3 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::State2);
    cont3.Content.SetAnimation(blueimg());
    cont3.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyX);
    cont3.SetSize(10, 10);
    cont3.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont3.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &stack = *new ComponentStack(temp);
    
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.25);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.5);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.75);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 1);
    
    stack.SetConditionOf(Gorgon::UI::ComponentTemplate::XTick, 1, Gorgon::UI::ComponentCondition::State2);
    
    layer.Add(stack);
    
    return {"Repeat set state", "On a white background there should be 5 10x10 rectangles with 10px spacing, all but second should be green.", stack};
}

TestData test_repeat_pos(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(90, 30);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetPadding(0, 10);
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyX);
    cont2.SetSize(10, 10);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &stack = *new ComponentStack(temp);
    
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.25);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.5);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.75);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 1);
    
    layer.Add(stack);
    
    return {"Repeat position", "On a white background there should be 5 10x10 green rectangles with 10px spacing.", stack};
}

TestData test_repeat_size(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(90, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetPadding(0, 10);
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(redimg());
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyHeight);
    cont2.SetSize(10, 10);
    cont2.SetMargin(10, 0, 0, 0);
    cont2.SetIndent(-10, 0, 0, 0);
    cont2.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &stack = *new ComponentStack(temp);
    
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.25);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.5);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.75);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 1);
    
    layer.Add(stack);
    
    return {"Repeat size", "On a white background there should be 5 10px wide red rectangles with 10px spacing. Sizes should start from 10 and should go up by 5px.", stack};
}

TestData test_repeat_alpha(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(90, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetPadding(0, 10);
    
    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(redimg());
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyAlpha);
    cont2.SetSize(10, 10);
    cont2.SetMargin(10, 0, 0, 0);
    cont2.SetIndent(-10, 0, 0, 0);
    cont2.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &stack = *new ComponentStack(temp);
    
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.25);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.5);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.75);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 1);
    
    layer.Add(stack);
    
    return {"Repeat alpha", "On a white background there should be 5 10x10 red rectangles with 10px spacing. First rectangle should be completely transparent while opacity increases with every step.", stack};
}

TestData test_repeat_sizealpha(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(90, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetPadding(0, 10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.AddIndex(2);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyHeight);
    cont2.SetSize(10, 10);
    cont2.SetMargin(10, 0, 0, 0);
    cont2.SetIndent(-10, 0, 0, 0);
    cont2.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &cont3 = temp.AddGraphics(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Content.SetAnimation(redimg());
    cont3.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyAlpha);
    cont3.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont3.SetSizing(Gorgon::UI::ComponentTemplate::Fixed);
    cont3.SetMargin(0);
    cont3.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &stack = *new ComponentStack(temp);
    
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.25);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.5);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.75);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 1);
    
    layer.Add(stack);
    
    return {"Repeat size and alpha", "On a white background there should be 5 10px wide red rectangles with 10px spacing. First rectangle should be completely transparent while opacity increases with every step while the height should start from 10px and increases 5px every step.", stack};
}

TestData test_repeat_possizealpha(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(90, 50);
    
    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());
    cont1.SetPadding(0, 10);
    
    auto &cont2 = temp.AddContainer(1, Gorgon::UI::ComponentCondition::Always);
    cont2.AddIndex(2);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyX);
    cont2.SetSize(10, {100, Gorgon::UI::Dimension::Percent});
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &cont3 = temp.AddContainer(2, Gorgon::UI::ComponentCondition::Always);
    cont3.AddIndex(3);
    cont3.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyHeight);
    cont3.SetSize(10, 10);
    cont3.SetPosition(0, 0);
    cont3.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &cont4 = temp.AddGraphics(3, Gorgon::UI::ComponentCondition::Always);
    cont4.Content.SetAnimation(redimg());
    cont4.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyAlpha);
    cont4.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont4.SetSizing(Gorgon::UI::ComponentTemplate::Fixed);
    cont4.SetMargin(0);
    cont4.SetRepeatMode(Gorgon::UI::ComponentTemplate::XTick);
    
    auto &stack = *new ComponentStack(temp);
    
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.25);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.5);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 0.75);
    stack.AddRepeat(Gorgon::UI::ComponentTemplate::XTick, 1);
    
    layer.Add(stack);
    
    return {"Repeat size and alpha", "On a white background there should be 5 10px wide red rectangles with 10px spacing. First rectangle should be completely transparent while opacity increases with every step while the height should start from 10px and increases 5px every step.", stack};
}

TestData test_gettagbounds1(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);

    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::LeftTag);

    return {"Get tag bounds 1", String::Concat((b.operator ==({0,0,10,10}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(0, 0) - (10, 10)]"), stack};
}

TestData test_gettagbounds2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetSizing(Gorgon::UI::ComponentTemplate::Fixed);
    cont2.SetSize(100, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);

    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::LeftTag);

    return {"Get tag bounds 2", String::Concat((b.operator ==({0,0,50,50}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(0, 0) - (50, 50)]"), stack};
}

TestData test_gettagbounds3(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetSizing(Gorgon::UI::ComponentTemplate::Fixed);
    cont2.SetSize(50, 100, Gorgon::UI::Dimension::Percent);
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);

    auto &cont3 = temp.AddGraphics(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Content.SetAnimation(redimg());
    cont3.SetTag(Gorgon::UI::ComponentTemplate::RightTag);

    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::RightTag);

    return {"Get tag bounds 3", String::Concat((b.operator ==({20,0,30,10}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(20, 0) - (30, 50)]"), stack};
}

TestData test_gettagbounds_modifysize(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(70, 80);
    
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
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifySize, Gorgon::UI::ComponentTemplate::UseSize);
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);
    
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    stack.SetValue(0.2, 0.5);
    
    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::LeftTag);
    

    return {"Get tag bounds modify size", String::Concat((b.operator ==({0, 0, 10, 35}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(0, 0) - (10, 35)]"), stack};
}

TestData test_gettagbounds_modifypos(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(80, 80);
    
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
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition, Gorgon::UI::ComponentTemplate::UseXY);
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);
    
    
    auto &stack = *new ComponentStack(temp);
    
    layer.Add(stack);
    
    stack.SetValue(0.2, 0.5);
    
    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::LeftTag);
    

    return {"Get tag bounds modify position", String::Concat((b.operator ==({10, 25, 20, 35}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(10, 25) - (20, 35)]"), stack};
}

TestData test_indexoftag(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);

    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    auto i = stack.IndexOfTag(Gorgon::UI::ComponentTemplate::LeftTag);

    return {"Get index of tag", String::Concat((i==1) ? "Passed" : "Failed", ". Returned index = ", i, " should be 1"), stack};
}

TestData test_haslayer(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());

    auto &cont3 = temp.AddGraphics(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Content.SetAnimation(redimg());
    cont3.SetClip(true);
 
    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    return {"Has layer", String::Concat((!stack.HasLayer(1) && stack.HasLayer(2)) ? "Passed" : "Failed"), stack};
}

TestData test_componentat1(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());

    auto &cont3 = temp.AddGraphics(2, Gorgon::UI::ComponentCondition::Always);
    cont3.Content.SetAnimation(redimg());
 
    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    return {"Component at 1", String::Concat((
            stack.ComponentAt({1, 1}) == 1 && 
            stack.ComponentAt({10, 0}) == 2 && 
            stack.ComponentAt({20, 0}) == 0 && 
            stack.ComponentAt({50, 0}) == -1
        ) ? "Passed" : "Failed", ". ",
        stack.ComponentAt({1, 1}), " = 1, ", 
        stack.ComponentAt({10, 0}), " = 2, ", 
        stack.ComponentAt({20, 0}), " = 0, ", 
        stack.ComponentAt({50, 0}), " = -1, "
    ), stack};
}

TestData test_componentat2(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());

    auto &cont3 = temp.AddGraphics(2, Gorgon::UI::ComponentCondition::Always);
    cont3.SetMargin(-5, 0, 0, 0);
    cont3.Content.SetAnimation(redimg());
 
    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);
    
    Geometry::Bounds b;
    int ind = stack.ComponentAt({5, 0}, b);

    return {"ComponentAt stacking", String::Concat((
            stack.ComponentAt({1, 1}) == 1 && 
            ind == 2 &&
            b == Geometry::Bounds(5, 0, 15, 10)
        ) ? "Passed" : "Failed", ". ",
        stack.ComponentAt({1, 1}), " = 1, ", 
        stack.ComponentAt({5, 0}), " = 2, ",
        b," = [(5, 0) - (15, 10)]"
    ), stack};
}

TestData test_componentexists(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.AddIndex(2);
    cont1.AddIndex(3);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());

    auto &cont3 = temp.AddGraphics(2, Gorgon::UI::ComponentCondition::Always);
    cont3.SetMargin(-5, 0, 0, 0);
    cont3.Content.SetAnimation(redimg());
 
    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    return {"ComponentExists", String::Concat((
            stack.ComponentExists(1) && 
            stack.ComponentExists(2) && 
            !stack.ComponentExists(3) && 
            !stack.ComponentExists(4)
        ) ? "Passed" : "Failed"
    ), stack};
}

TestData test_settagpos(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);

    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    stack.SetTagLocation(Gorgon::UI::ComponentTemplate::LeftTag, {10, 10});
    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::LeftTag);

    return {"Set tag position", String::Concat((b.operator ==({10,10,20,20}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(10, 10) - (20, 20)]. Green rectangle should be 10, 10 from top left."), stack};
}

TestData test_settagposabs(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::Absolute);
    cont2.SetPosition(10, 10);
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);

    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    stack.SetTagLocation(Gorgon::UI::ComponentTemplate::LeftTag, {10, 10});
    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::LeftTag);

    return {"Set tag position absolute", String::Concat((b.operator ==({20,20,30,30}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(20, 20) - (30, 30)]. Green rectangle should be 20, 20 from top left."), stack};
}

TestData test_settagposval(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(70, 70);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);
    cont2.SetPositioning(Gorgon::UI::ComponentTemplate::AbsoluteSliding);
    cont2.SetValueModification(Gorgon::UI::ComponentTemplate::ModifyPosition, Gorgon::UI::ComponentTemplate::UseXY);

    auto &stack = *new ComponentStack(temp);
    stack.SetValue(0.50, 0.50);

    layer.Add(stack);

    stack.SetTagLocation(Gorgon::UI::ComponentTemplate::LeftTag, {10, 10});
    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::LeftTag);

    return {"Set tag position value", String::Concat((b.operator ==({35,35,45,45}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(35, 35) - (45, 45)]. Green rectangle should be 35, 35 from top left."), stack};
}

TestData test_settagposrem(Layer &layer) {
    auto &temp = *new Template;
    temp.SetSize(50, 50);

    auto &cont1 = temp.AddContainer(0, Gorgon::UI::ComponentCondition::Always);
    cont1.AddIndex(1);
    cont1.Background.SetAnimation(whiteimg());

    auto &cont2 = temp.AddGraphics(1, Gorgon::UI::ComponentCondition::Always);
    cont2.Content.SetAnimation(greenimg());
    cont2.SetTag(Gorgon::UI::ComponentTemplate::LeftTag);

    auto &stack = *new ComponentStack(temp);

    layer.Add(stack);

    stack.SetTagLocation(Gorgon::UI::ComponentTemplate::LeftTag, {10, 10});
    stack.RemoveTagLocation(Gorgon::UI::ComponentTemplate::LeftTag);
    auto b = stack.TagBounds(Gorgon::UI::ComponentTemplate::LeftTag);

    return {"Set tag position and remove", String::Concat((b.operator ==({0,0,10,10}) ? "Passed" : "Failed"), ". Returned bounds = ", b, " should be [(0, 0) - (10, 10)]. Green rectangle should be at top left."), stack};
}


std::vector<std::function<TestData(Layer &)>> tests = {
    &test_graphic,
    &test_text,
    
    &test_setsize,
    &test_setsizepercent,
    &test_setsizepercent_percent,
    &test_setsizepercent_rel,
    &test_sizepercent_center,
    &test_sizepercent_centerabs,

    &test_setborder,
    &test_setborder2,
    &test_setoverlay,
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
    
    &test_modify_alpha,
    &test_modify_blend1,
    &test_modify_blend2,
    &test_modify_blend4,
    
    &test_modify_animation,
    
    &test_data_settext,
    &test_data_textwrap,
    &test_data_textclip,
    &test_data_settitle,
    &test_data_settexttitle,
    
    &test_data_setimage,
    &test_data_setimagegt,
    &test_data_setimage2,
    
    &test_repeat_pos,
    &test_data_setstate,
    &test_repeat_size,
    &test_repeat_alpha,
    &test_repeat_sizealpha,
    &test_repeat_possizealpha,
    
    &test_gettagbounds1,
    &test_gettagbounds2,
    &test_gettagbounds3,
    &test_gettagbounds_modifysize,
    &test_gettagbounds_modifypos,
    &test_indexoftag,
    &test_haslayer,
    &test_componentat1,
    &test_componentat2,
    &test_componentexists,
    &test_settagpos,
    &test_settagposabs,
    &test_settagposval,
    &test_settagposrem,
};

//END tests

Containers::Collection<Layer> layers;
Containers::Collection<ComponentStack> stacks;
std::vector<std::pair<std::string, std::string>> info;
Input::KeyRepeater repeater;
    
int main() {
    bool quit = false;
    auto &app = getapp();

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
                    "Value 4:\t", int(activestack->GetValue()[3]*100),"\n",
                    "Text   :\t", activestack->GetTextData(Gorgon::UI::ComponentTemplate::Text), "\n",
                    "Title  :\t", activestack->GetTextData(Gorgon::UI::ComponentTemplate::Title), "\n"
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
                quit = true;
                break;
                
            case Keycodes::Number_5: {
                std::string text = activestack->GetTextData(Gorgon::UI::ComponentTemplate::Text);
                
                switch(text.length()) {
                case 0:
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Text, teststrings[0]);
                    break;
                case 2:
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Text, teststrings[1]);
                    break;
                case 5:
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Text, teststrings[2]);
                    break;
                case 12:
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Text, teststrings[3]);
                    break;
                default:
                    activestack->RemoveData(Gorgon::UI::ComponentTemplate::Text);
                    break;
                }
                
                displayvalue();
                
                break;
            }
            
            case Keycodes::Number_6: {
                std::string text = activestack->GetTextData(Gorgon::UI::ComponentTemplate::Title);
                
                switch(text.length()) {
                case 0:
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Title, teststrings[0]);
                    break;
                case 2:
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Title, teststrings[1]);
                    break;
                case 5:
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Title, teststrings[2]);
                    break;
                case 12:
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Title, teststrings[3]);
                    break;
                default:
                    activestack->RemoveData(Gorgon::UI::ComponentTemplate::Title);
                    break;
                }
                
                displayvalue();
                
                break;
            }
                
            case Keycodes::Number_7: {
                auto img = activestack->GetImageData(Gorgon::UI::ComponentTemplate::Icon1);
                
                if(img == testimages[4]) {
                    activestack->RemoveData(Gorgon::UI::ComponentTemplate::Icon1);
                }
                else if(img == testimages[0]) {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon1, *testimages[1]);
                }
                else if(img == testimages[1]) {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon1, *testimages[2]);
                }
                else if(img == testimages[2]) {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon1, *testimages[3]);
                }
                else if(img == testimages[3]) {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon1, *testimages[4]);
                }
                else {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon1, *testimages[0]);
                }
                
                break;
            }
            
            case Keycodes::Number_8: {
                auto img = activestack->GetImageData(Gorgon::UI::ComponentTemplate::Icon2);
                
                if(img == testimages[4]) {
                    activestack->RemoveData(Gorgon::UI::ComponentTemplate::Icon2);
                }
                else if(img == testimages[0]) {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon2, *testimages[1]);
                }
                else if(img == testimages[1]) {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon2, *testimages[2]);
                }
                else if(img == testimages[2]) {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon2, *testimages[3]);
                }
                else if(img == testimages[3]) {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon2, *testimages[4]);
                }
                else {
                    activestack->SetData(Gorgon::UI::ComponentTemplate::Icon2, *testimages[0]);
                }
                
                break;
            }
            
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

    while(!quit) {
        Gorgon::NextFrame();
    }

    return 0;
}


#include "GraphicsHelper.h"

#include <Gorgon/Scene.h>
#include "Gorgon/Graphics/TintedObject.h"
#include "Gorgon/Graphics/StackedObject.h"
#include "Gorgon/Graphics/Pointer.h"
#include <Gorgon/Graphics/FreeType.h>


void Init();

std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;

enum Scenes {
	SCENE_INIT,
	NEXT_SCENE
};

class InitScene : public Gorgon::Scene {
public:
	InitScene(Gorgon::SceneManager &parent, SceneID id,
			  const Graphics::Bitmap &bg, Graphics::StyledRenderer &renderer, Graphics::RectangularAnimationProvider &prov) :
		Scene(parent, id),
		bg(bg), renderer(renderer), prov(prov) {

		auto &cursorhead = *new Gorgon::Graphics::Bitmap(Triangle1(12, 12));
		cursorhead.Prepare();
		auto cursortail_img = Triangle1(8, 8);
		cursortail_img.Prepare();
		Gorgon::Graphics::TintedBitmapProvider &cursortail = *new Gorgon::Graphics::TintedBitmapProvider(std::move(cursortail_img), 0xff000000);
		cursor = Gorgon::Graphics::StackedObjectProvider{Gorgon::AssumeOwnership, cursorhead, cursortail, {1, 4}};


		ptr={Gorgon::AssumeOwnership, cursor.CreateAnimation(), 0, 0};

		parent.Pointers.Add(Graphics::PointerType::Arrow, ptr);
	}

	virtual bool RequiresKeyInput() const override {
		return true;
	}
	
	Graphics::Layer &GetGraphics() {
        return graphics;
    }


	virtual void KeyEvent(Gorgon::Input::Key key, float status) override {
		namespace Keycodes = Gorgon::Input::Keyboard::Keycodes;
		if(key == Keycodes::Escape && status) {
			parent->Quit();
		}
		else if(key == Keycodes::N && status) {
			parent->SwitchScene(NEXT_SCENE);
		}
	}

protected:
    
    virtual void first_activation() override {
        anim = prov.CreateAnimation(true);
    }
    
	virtual void activate() override {
		parent->SwitchToLocalPointers();
	}

	virtual void deactivate() override {
		parent->SwitchToWMPointers();
	}


	virtual void doframe(unsigned int delta) override {

	}


	virtual void render() override {
		graphics.Clear();

		bg.DrawIn(graphics);
        anim.Draw(graphics, 400, 0);
		renderer.Print(graphics, helptext + "n\tNext scene\n");
	}

	const Graphics::Bitmap &bg;
	Graphics::StyledRenderer &renderer;
	Gorgon::Graphics::StackedObjectProvider cursor;
	Graphics::DrawablePointer ptr;
    Graphics::Instance anim;
    Graphics::RectangularAnimationStorage prov;
};

class NextScene : public Gorgon::Scene {
public:
	NextScene(Gorgon::SceneManager &parent, SceneID id,
			  const Graphics::Bitmap &bg, Graphics::StyledRenderer &renderer) :
		Scene(parent, id),
		bg(bg), renderer(renderer) 
	{
		mouse.SetClick(this, &NextScene::Click);
	}

	void Click(Gorgon::Input::Mouse::Button button) {
		parent->SwitchScene(SCENE_INIT);
	}

	virtual bool RequiresKeyInput() const override {
		return true;
	}


	virtual void KeyEvent(Gorgon::Input::Key key, float status) override {
		namespace Keycodes = Gorgon::Input::Keyboard::Keycodes;
		if(key == Keycodes::Escape && status) {
			parent->Quit();
		}
	}

protected:
	virtual void activate() override {
        graphics.GetParent().Add(dynamic_cast<InitScene&>(parent->GetScene(SCENE_INIT)).GetGraphics());
        graphics.PlaceToTop();
	}


	virtual void doframe(unsigned int delta) override {

	}


	virtual void render() override {
		graphics.Clear();

		//bg.DrawIn(graphics);
		renderer.Print(graphics, helptext + "click\tInitial scene\n", 0, 200);
	}

	const Graphics::Bitmap &bg;
	Graphics::StyledRenderer &renderer;
};

int main() {
	std::cout<<"Current working directory: ";
#ifdef WIN32
	system("cd");
#else
	system("pwd");
#endif
	std::cout<<std::endl;

	Gorgon::Initialize("scenetest");
	SceneManager wind({800, 600}, "Scene test");

	Bitmap bgimage;

	bgimage = BGImage(25, 25);
	bgimage.Prepare();
    
    Graphics::FreeType f;
#ifdef WIN32
    f.LoadFile("C:/Windows/Fonts/tahoma.ttf");
#else
	f.LoadFile("/usr/share/fonts/gnu-free/FreeSans.ttf");
#endif
    f.LoadMetrics(16);
    f.Pack();

	Graphics::StyledRenderer sty ={f};
	sty.UseFlatShadow({0.f, 1.0f}, {1.f, 1.f});
	sty.SetColor({0.6f, 1.f, 1.f});
	sty.JustifyLeft();

	sty.SetTabWidthInLetters(4.f);
	sty.SetParagraphSpacing(2);
    
    Graphics::BitmapAnimationProvider animprov;

    for(int i=0; i<25; i++) {
        Graphics::Bitmap bmp({25, i + 1}, Graphics::ColorMode::Alpha);
        bmp.Clear();
        for(int y = 0; y<i; y++) {
            for(int x = 0; x<25; x++) {
                bmp(x, y, 0) = 255;
            }
        }
        
        animprov.Add(std::move(bmp), 30+i*5);
    }
    
    animprov.Prepare();

	wind.NewScene<InitScene>(SCENE_INIT, bgimage, sty, animprov);
	wind.NewScene<NextScene>(NEXT_SCENE, bgimage, sty);
	wind.SwitchScene(SCENE_INIT);

	wind.Run();

	return 0;
}


void Init() {

}

#include "GraphicsHelper.h"

#include <Gorgon/Scene.h>
#include "Gorgon/Graphics/TintedObject.h"
#include "Gorgon/Graphics/StackedObject.h"
#include "Gorgon/Graphics/Pointer.h"


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
			  const Graphics::Bitmap &bg, Graphics::StyledRenderer &renderer) :
		Scene(parent, id),
		bg(bg), renderer(renderer) {

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
		renderer.Print(graphics, helptext + "n\tNext scene\n");
	}

	const Graphics::Bitmap &bg;
	Graphics::StyledRenderer &renderer;
	Gorgon::Graphics::StackedObjectProvider cursor;
	Graphics::DrawablePointer ptr;
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

	}


	virtual void doframe(unsigned int delta) override {

	}


	virtual void render() override {
		graphics.Clear();

		bg.DrawIn(graphics);
		renderer.Print(graphics, helptext + "click\tInitial scene\n");
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

	Graphics::BitmapFont fnt;
	Graphics::StyledRenderer sty ={fnt};
	Bitmap bgimage;

	bgimage = BGImage(25, 25);
	bgimage.Prepare();

	fnt.ImportFolder("Victoria", Graphics::BitmapFont::Automatic, 0, "", -1, true, false, false);
	fnt.Pack();

	sty.UseFlatShadow({0.f, 1.0f}, {1.f, 1.f});
	sty.SetColor({0.6f, 1.f, 1.f});
	sty.JustifyLeft();

	sty.SetTabWidthInLetters(6.f);
	sty.SetParagraphSpacing(2);

	wind.NewScene<InitScene>(SCENE_INIT, bgimage, sty);
	wind.NewScene<NextScene>(NEXT_SCENE, bgimage, sty);
	wind.SwitchScene(SCENE_INIT);

	wind.Run();

	return 0;
}


void Init() {

}

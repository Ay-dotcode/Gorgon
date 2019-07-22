#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../Property.h"

namespace Gorgon { namespace Widgets {
   
	class Button : public UI::ComponentStackWidget {
	private:
		void seticon(const Graphics::Animation *value);

		const Graphics::Animation *geticon() const;
	public:

		Button() = default;

		Button(const Button &) = delete;

		explicit Button(std::string title = "");

		explicit Button(std::function<void()> clickfn);

		Button(std::string title, std::function<void()> clickfn);

		void SetText(const std::string &value);

		std::string GetText() const;

		void SetIcon(const Graphics::Animation &value);

		const Graphics::Animation &GetIcon() const;

		bool HasIcon() const;

		void OwnIcon();

		void OwnIcon(const Graphics::Animation &value);


		TextualProperty<Button, std::string, &Button::GetText, &Button::SetText> Text;

		ObjectProperty<Button, const Graphics::Animation, &Button::GetIcon, &Button::SetIcon> Icon;

		Event<Button> ClickEvent;

	};
    
} }

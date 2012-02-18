#pragma once

#include "../Listbox.h"
#include "../Checkbox.h"
#include "ComboboxBlueprint.h"
#include "../Basic/PetContainer.h"
#include "../Basic/PetTextbox.h"


namespace gge { namespace widgets {
	namespace combobox {


		template<class T_>
		void CastToString(const T_ &v, std::string &str) {
			str=(std::string)v;
		}

		template<class T_>
		void CastFromString(T_ &v, const std::string &str) {
			v=(T_)str;
		}

		template<class T_, void(*CS_)(const T_ &, std::string &)=CastFromString<T_> >
		class Base : public WidgetBase {
		public:

			Base(const T_ &value=T_()) : bp(NULL),  controls(*this), isextended(false)
			{
				controls.AddWidget(dropbutton);
				controls.AddWidget(textbox);
				controls.AddWidget(listbox);

				listbox.SetIsExtender(true);
				listbox.ItemClickedEvent.RegisterLambda([&](){
					setvalue(this->listbox.GetValue());
					shrink();
				});
				listbox.AutoHeight=true;
				listbox.Hide();

				textbox.NoSelection=true;
				textbox.Readonly=true;
				textbox.MouseEventOccured.Register(this, &Base::textbox_mouse);
				//textbox.ChangeEvent().RegisterLambda([&](){setvalue(this->textbox.Text);});

				dropbutton.Autosize=AutosizeModes::None;
				dropbutton.ChangeEvent().RegisterLambda([&](){
					if(dropbutton)
						extend();
					else
						shrink();
				});
				dropbutton.GotFocus.Register(this, &Base::dropbutton_focus);
			}


			virtual void Enable() {
				controls.Enable();
				WidgetBase::Enable();
			}

			virtual void Disable() {
				controls.Disable();
				WidgetBase::Disable();
			}

			virtual bool Focus() {
				if(!IsFocused()) {
					WidgetBase::Focus();

					textbox.Focus();
				}
				return true;
			}

			using WidgetBase::SetBlueprint;
			virtual void SetBlueprint(const widgets::Blueprint &bp);

			virtual void Resize(utils::Size Size) {
				WidgetBase::Resize(Size);

				textbox.Resize(Size);
				Bounds b=textbox.GetBounds();
				dropbutton.SetX(b.Right-dropbutton.GetWidth());
				if(this->bp)
					listbox.SetY(b.Bottom+this->bp->ListMargins.Top);
				else
					listbox.SetY(b.Bottom);
				if(GetWidth()>listbox.GetWidth()) {
					if(this->bp)
						listbox.SetWidth(GetWidth()-this->bp	->ListMargins.TotalX());
					else
						listbox.SetWidth(GetWidth());
				}
			}

			//!check
			virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
				if(event==input::keyboard::Event::Char) {
					if(isextended) {
						if(!input::keyboard::Modifier::IsModified()) {
							if(Key==input::keyboard::KeyCodes::Enter || Key==input::keyboard::KeyCodes::Escape) {
								shrink();
								return true;
							}
						}
					}
					else {
						if(!input::keyboard::Modifier::IsModified()) {
							if(Key==input::keyboard::KeyCodes::Enter) {
								extend();
								return true;
							}
						}
					}
				}

				return textbox.KeyboardEvent(event, Key);
			}


		protected:

			virtual void add(ListItem<T_, CS_> &item) {
				listbox.Add(item);
			}

			virtual void insert(ListItem<T_, CS_> &item, ListItem<T_, CS_> *before) {
				listbox.Insert(item,before);
			}

			virtual void draw() {}

			virtual bool loosefocus(bool force) {
				shrink();

				if(force) {
					textbox.ForceRemoveFocus();
					return true;
				}
				else
					return textbox.RemoveFocus();
			}

			virtual bool detach(ContainerBase *container) {
				controls.AttachTo(NULL, NULL);
				listbox.Detach();

				return true;
			}

			bool textbox_mouse(input::mouse::Event event) {
				return dropbutton.MouseEvent(event.event, event.location, event.amount);
			}

			void dropbutton_focus() {
				if(isextended)
					listbox.Focus();
				else
					textbox.Focus();
			}

			virtual void containerenabledchanged(bool state) {
				controls.InformEnabledChange(state);
			}

			virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
				WidgetBase::located(container, w, Order);

				if(container)
					controls.AttachTo(BaseLayer, &container->CreateExtenderLayer());
				else
					controls.AttachTo(NULL, NULL);
			}

			virtual void extend() {
				if(!isextended) {
					isextended=true;
					if(auto li=listbox.Find(value))
						li->Signal();
					else
						listbox.ClearSelection();

					listbox.Show(true);
					if(!dropbutton)
						dropbutton=true;
				}
			}

			virtual bool IsExtended() {
				return isextended;
			}

			virtual void shrink() {
				if(isextended) {
					isextended=false;
					
					listbox.Hide();
					if(dropbutton)
						dropbutton=false;
					if(IsFocused())
						textbox.Focus();
				}
			}

			virtual void valuechanged() {}

			void setvalue(const T_ &value) {
				if(this->value!=value) {
					this->value = value;

					if(isextended) {
						if(auto li=listbox.Find(value))
							li->Signal();
						else
							listbox.ClearSelection();
					}
					
					std::string str;
					CS_(value, str);
					textbox.Text=str;
					valuechanged();
				}
			}
			T_ getvalue() const {
				return value;
			}

			const Blueprint *bp;

			ExtendedPetContiner<Base> controls;

			bool isextended;
			
			PetTextbox textbox;
			Listbox<T_, CS_> listbox;
			Checkbox dropbutton;

			T_ value;
		};

		template<class T_, void(*CS_)(const T_ &, std::string &)>
		void Base<T_, CS_>::SetBlueprint(const widgets::Blueprint &bp) {
			if(this->bp==&bp)
				return;

			this->bp=static_cast<const Blueprint*>(&bp);
			if(!this->bp)
				return;

			if(this->bp->Textbox)
				textbox.SetBlueprint(*this->bp->Textbox);

			if(this->bp->Listbox)
				listbox.SetBlueprint(*this->bp->Listbox);

			if(this->bp->Dropbutton)
				dropbutton.SetBlueprint(*this->bp->Dropbutton);

			if(WidgetBase::size.Width==0)
				SetWidth(this->bp->DefaultSize.Width);

			if(WidgetBase::size.Height==0)
				SetHeight(this->bp->DefaultSize.Height);

			listbox.Move(this->bp->ListMargins.Left,this->GetBounds().Bottom+this->bp->ListMargins.Top);
			if(this->bp->Listbox) {
				listbox.Resize(this->bp->Listbox->DefaultSize);
				if(GetWidth()>listbox.GetWidth())
					listbox.SetWidth(GetWidth()-this->bp->ListMargins.TotalX());
			}
			
			textbox.SetPointer(Pointer::None);
		}


	}
}}

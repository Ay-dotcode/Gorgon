#pragma once

#include "../Listbox.h"
#include "../Checkbox.h"
#include "ComboboxBlueprint.h"
#include "../Basic/PetContainer.h"
#include "../Basic/PetTextbox.h"


namespace gge { namespace widgets {
	class ComboboxType {

	};

	namespace combobox {


		template<class T_>
		void CastToString(const T_ &v, std::string &str) {
			str=(std::string)v;
		}

		template<class T_>
		void CastFromString(T_ &v, const std::string &str) {
			v=(T_)str;
		}

		template<class T_, class A_, class L_, void(*CS_)(const T_ &, std::string &)=CastFromString<T_> >
		class Base : public WidgetBase, public ComboboxType {
		public:

			Base() : bp(NULL),  controls(*this), isextended(false),
				blueprintmodified(false),
				INIT_PROPERTY(Base, AutoUpdate)
			{
				controls.AddWidget(dropbutton);
				controls.AddWidget(textbox);
				controls.AddWidget(listbox);

				listbox.SetIsExtender(true);
				listbox.ItemClickEvent.RegisterLambda([&](){
					this->setvalue(this->listbox.Get());
					this->shrink();
				});
				listbox.AutoHeight=true;
				listbox.Hide();

				textbox.NoSelection=true;
				textbox.Readonly=true;
				textbox.MouseEventOccured.Register(this, &Base::textbox_mouse);
				//textbox.ChangeEvent().RegisterLambda([&](){setvalue(this->textbox.Text);});

				dropbutton.Autosize=AutosizeModes::None;
				dropbutton.ChangeEvent.RegisterLambda([&](){
					if(dropbutton)
						this->extend();
					else
						this->shrink();
				});
				dropbutton.GotFocus.Register(this, &Base::dropbutton_focus);
				WR.LoadedEvent.Register(this, &Base::wr_loaded);
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
			virtual void SetBlueprint(const widgets::Blueprint &bp) {
				blueprintmodified=true;
				setblueprint(bp);
			}

			virtual void Resize(utils::Size Size) {
				WidgetBase::Resize(Size);

				if(Size.Width==0 && bp)
					Size.Width=bp->DefaultSize.Width;
				if(Size.Height==0 && bp)
					Size.Height=bp->DefaultSize.Height;

				if(BaseLayer)
					BaseLayer->Resize(Size);

 				controls.Resize(Size);

				textbox.Resize(Size);
				utils::Bounds b=textbox.GetBounds();
				dropbutton.SetX(b.Right-dropbutton.GetWidth());
				if(this->bp)
					listbox.SetY(b.Bottom+this->bp->ListMargins.Top);
				else
					listbox.SetY(b.Bottom);
				listbox.SetWidth(0);
				if(GetWidth()>listbox.GetWidth()) {
					if(this->bp)
						listbox.SetWidth(GetWidth()-this->bp	->ListMargins.TotalX());
					else
						listbox.SetWidth(GetWidth());
				}
			}

			virtual void Move(utils::Point Location) {
				WidgetBase::Move(Location);

				if(controls.extenderbase)
					controls.extenderbase->Move(Location);
			}

			virtual utils::Size GetSize() {
				if(!bp)
					return size;

				return utils::Size(size.Width ? size.Width : bp->DefaultSize.Width, size.Height ? size.Height : bp->DefaultSize.Height);
			}

			//!check
			virtual bool KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key key) {
				using namespace input::keyboard;

				if(!Modifier::Check() && event==Event::Down) {
					if(isextended && (key==KeyCodes::Enter || key==KeyCodes::Escape)) {
						shrink();
						return true;
					}
					if(!isextended && key==KeyCodes::Enter) {
						extend();
						return true;
					}
					if(key==KeyCodes::Down) {
						if(listbox.ActiveIndex()<listbox.GetCount()-1 && listbox.GetCount()) {
							listbox.Select(listbox.ActiveIndex()+1);
							listbox.EnsureVisible();
							this->setvalue(this->listbox.Get());
						}

						return true;
					}
					if(key==KeyCodes::Up) {
						if(listbox.ActiveIndex()>0) {
							listbox.Select(listbox.ActiveIndex()-1);
							listbox.EnsureVisible();
							this->setvalue(this->listbox.Get());
						}

						return true;
					}
					if(key==KeyCodes::PageDown) {
						if(listbox.GetCount()==0) return true;

						int target=listbox.ActiveIndex()+5;
						if(target>=listbox.GetCount()) {
							target=listbox.GetCount()-1;
						}
						listbox.Select(target);
						listbox.EnsureVisible();
						this->setvalue(this->listbox.Get());

						return true;
					}
					if(key==KeyCodes::PageUp) {
						if(listbox.GetCount()==0) return true;

						int target=listbox.ActiveIndex()-5;
						if(target<0) {
							target=0;
						}
						listbox.Select(target);
						listbox.EnsureVisible();
						this->setvalue(this->listbox.Get());

						return true;
					}
					if(key==KeyCodes::Home) {
						if(listbox.GetCount()) {
							listbox.Select(0);
							listbox.EnsureVisible();
							this->setvalue(this->listbox.Get());
						}
						return true;
					}
					if(key==KeyCodes::End) {
						if(listbox.GetCount()) {
							listbox.Select(listbox.GetCount()-1);
							listbox.EnsureVisible();
							this->setvalue(this->listbox.Get());
						}
						return true;
					}
				}

				return false;


				return textbox.KeyboardHandler(event, key);
			}

			virtual bool IsExtended() {
				return isextended;
			}

			virtual ~Base() {
			}

			utils::BooleanProperty<Base> AutoUpdate;

		protected:
			virtual void setblueprint(const widgets::Blueprint &bp);

			bool blueprintmodified;

			virtual void wr_loaded() {

			}

			virtual void add(typename A_::paramtype item) {
				listbox.Add(item);
			}

			virtual void insert(typename A_::paramtype item, unsigned before) {
				listbox.Insert(item,before);
			}

			virtual void remove(unsigned before) {
				listbox.Remove(before);
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
				container->ScrollingEvent.Unregister(this, &Base::container_scrolling);
				controls.AttachTo(NULL, NULL);
				listbox.Detach();

				return true;
			}

			bool textbox_mouse(input::mouse::Event event) {
				return dropbutton.MouseHandler(event.event, event.location, event.amount);
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

				if(BaseLayer)
					BaseLayer->Resize(controls.GetSize());

				if(container) {
					controls.AttachTo(BaseLayer, &container->CreateExtenderLayer());
					controls.extenderbase->Move(location);
					container->ScrollingEvent.Register(this, &Base::container_scrolling);
					containerenabledchanged(container->IsEnabled());
				}
				else
					controls.AttachTo(NULL, NULL);
			}

			virtual void extend() {
				if(!isextended) {
					isextended=true;

					listbox.Value=value;
					listbox.CenterItem();

					listbox.Show(true);
					if(!dropbutton)
						dropbutton=true;
				}
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

			void setvalue(typename A_::paramtype &value) {
				if(this->value!=value) {
					this->value = A_::ParamToStorageType(value);

					if(isextended) {
						listbox.Value=value;
						listbox.EnsureVisible();
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

			void container_scrolling(bool &allow) {
				if(isextended)
					allow=false;
			}

			void setAutoUpdate(const bool &value) {
				listbox.AutoUpdate=value;
			}
			bool getAutoUpdate() const {
				return listbox.AutoUpdate;
			}

			const Blueprint *bp;

			ExtendedPetContiner<Base> controls;

			bool isextended;
			
			PetTextbox textbox;
			L_ listbox;
			Checkbox dropbutton;

			typename A_::storagetype value;
		};

		template<class T_, class A_, class L_, void(*CS_)(const T_ &, std::string &)>
		void Base<T_, A_, L_, CS_>::setblueprint(const widgets::Blueprint &bp) {
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

			listbox.Move(location+utils::Point(this->bp->ListMargins.Left,this->GetBounds().Bottom+this->bp->ListMargins.Top));
			if(this->bp->Listbox) {
				listbox.Resize(this->bp->Listbox->DefaultSize);
				if(GetWidth()>listbox.GetWidth())
					listbox.SetWidth(GetWidth()-this->bp->ListMargins.TotalX());
			}

			textbox.SetPointer(Pointer::None);

			Resize(size);
		}


	}
}}

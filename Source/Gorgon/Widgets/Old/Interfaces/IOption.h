#pragma once


#include <string>
#include "../Base/Widget.h"
#include "../../Utils/Property.h"
#include "../../Utils/EventChain.h"
#include "../Base/Container.h"
#include "../../Engine/Graphic2D.h"



namespace gge { namespace widgets {


	template<class T_>
	class IOptionGroup;

	template<class T_>
	class IOption {
		template<class T2_, class O_>
		friend class OptionGroup;
	public:

		IOption() : ChangeEvent("ChangeEvent", this),
			INIT_PROPERTY(IOption,Text),
			INIT_PROPERTY(IOption,State),
			owned(NULL), parent(NULL)
		{ }

		utils::TextualProperty<IOption> Text;
		utils::BooleanProperty<IOption> State;

		void Check() {
			if(!State)
				State=true;
		}
		operator bool() const {
			return State;
		}
		operator T_() const {
			return Value;
		}
		virtual void Uncheck() {
			if(State)
				State=false;
		}

		utils::EventChain<IOption> ChangeEvent;

		T_ Value;

	protected:

		void setgroup(IOptionGroup<T_> *grp);

		//REQUIRED
		virtual void setState(const bool &state) = 0;
		virtual bool getState() const = 0;
		virtual void setText(const std::string &text) = 0;
		virtual std::string getText() const = 0;

		bool owned;
		IOptionGroup<T_> *parent;


	};

	template<class T_>
	class IOptionGroup {
	public:
		virtual void Remove(IOption<T_> *) = 0;
		virtual void SetToNext()=0;
		virtual void SetToPrev()=0;
	};

	//O_ should always be covariant of IOptionGroup
	template<class T_, class O_>
	class OptionGroup : public IOptionGroup<T_> {
	public:

		OptionGroup() : ChangeEvent("ChangedEvent", this), currentoption(NULL) {

		}

		virtual void Add(O_ &option) {
			Options.Add(option);
			option.ChangeEvent.Register(*this, &OptionGroup::option_changed);
			option.setgroup(this);
		}
		void Add(O_ *option) {
			if(option)
				Add(*option);
		}
		O_ &Add(const std::string &Text, T_ Value=T_()) {
			O_ *option=new O_(Text);
			option->owned=this;

			Add(option);

			return *option;
		}

		virtual void Remove(O_ &option) {
			if(&option==currentoption)
				currentoption=NULL;

			option.ChangeEvent.Unregister(*this, &OptionGroup::option_changed);

			if(option.parent==this && option.owned)
				Options.Delete(option);
			else {
				Options.Remove(option);
				option.parent=NULL;
			}
		}
		void Remove(O_ *option) {
			if(option)
				Remove(*option);
		}
		void Remove(IOption<T_> *option) {
			if(option)
				Remove(*option);
		}
		void Remove(const T_ &value) {
			O_ *option=FindOption(value);
			if(option)
				Remove(*option);
		}

		//may throw std::runtime_error("No value selected")
		operator T_() const { return Get(); }
		OptionGroup &operator = (const T_ &value) { Set(value); }

		//allows * to be overloaded to make sure you are allowed to set it NULL
		virtual void Set(O_ *option) {
			currentoption=option;

			for(auto i=Options.First();i.IsValid();i.Next()) {
				if(i.CurrentPtr()!=currentoption)
					i->Uncheck();
			}

			if(currentoption)
				currentoption->Check();

			if(option)
				ChangeEvent(option->Value);
			else
				ChangeEvent(T_());
		}
		void Set(O_ &option) {
			Set(&option);
		}
		void Set(const T_ &value) {
			Set(FindOption(value));
		}
		virtual void SetToNext() {
			if(currentoption==NULL) {
				if(Options.GetCount())
					Set(*Options.First());
			}
			else {
				auto it=Options.Find(currentoption);
				if(it.IsValid())
					it.Next();

				if(it.IsValid())
					Set(*it);
				else
					Set(*Options.First());
			}

			if(dynamic_cast<WidgetBase*>(currentoption))
				dynamic_cast<WidgetBase*>(currentoption)->Focus();
		}
		virtual void SetToPrev() {
			if(currentoption==NULL) {
				if(Options.GetCount())
					Set(*Options.Last());
			}
			else {
				auto it=Options.Find(currentoption);
				if(it.IsValid())
					it.Previous();

				if(it.IsValid())
					Set(*it);
				else
					Set(*Options.Last());
			}

			if(dynamic_cast<WidgetBase*>(currentoption))
				dynamic_cast<WidgetBase*>(currentoption)->Focus();
		}

		//Checks if there is a selected option, can be used to prevent exceptions
		virtual bool HasSelected() const {
			return currentoption!=NULL;
		}

		//may throw std::runtime_error("No value selected")
		virtual O_ &GetOption() {
			if(!currentoption)
				throw std::runtime_error("No value selected");

			return *currentoption;
		}
		//may throw std::runtime_error("No value selected")
		virtual const O_ &GetOption() const {
			if(!currentoption)
				throw std::runtime_error("No value selected");

			return *currentoption;
		}
		//may throw std::runtime_error("No value selected")
		virtual T_ Get() const {
			if(!currentoption)
				throw std::runtime_error("No value selected");

			return currentoption->Value;
		}

		virtual O_ *FindOption(const T_ &value) {
			for(auto i=Options.First();i.IsValid();i.Next()) {
				if(i->Value==value)
					return i.CurrentPtr();
			}

			return NULL;
		}
		virtual const O_ *FindOption(const T_ &value) const {
			for(auto i=Options.First();i.IsValid();i.Next()) {
				if(i->Value==value)
					return i.CurrentPtr();
			}

			return NULL;
		}

		typename utils::Collection<O_>::Iterator First() {
			return Options.First();
		}

		typename utils::Collection<O_>::Iterator Last() {
			return Options.Last();
		}

		typename utils::Collection<O_>::Iterator begin() {
			return Options.begin();
		}

		typename utils::Collection<O_>::Iterator end() {
			return Options.end();
		}

		typename utils::Collection<O_>::ConstIterator First() const {
			return Options.First();
		}

		typename utils::Collection<O_>::ConstIterator Last() const {
			return Options.Last();
		}

		typename utils::Collection<O_>::ConstIterator begin() const {
			return Options.begin();
		}

		typename utils::Collection<O_>::ConstIterator end() const {
			return Options.end();
		}

		utils::EventChain<OptionGroup, T_> ChangeEvent;

	protected:
		utils::Collection<O_> Options;


		void clearall() {
			for(auto i=Options.First();i.IsValid();i.Next()) {
				i->Uncheck();
			}
		}

		void option_changed(O_ &option) {
 			if(option.State)
 				Set(option);
		}
		

		O_ *currentoption;
	};

//TODO support: replace with c++0x partial specialization typedef
	template<class T_>
	void gge::widgets::IOption<T_>::setgroup(IOptionGroup<T_> *grp) {
		IOptionGroup<T_> *pparent=parent;
		//must be set to new group, so if ownership belongs to group it wont be destroyed
		parent=grp;
		if(pparent)
			pparent->Remove(this);

		//prev line might set parent to null
		parent=grp;
	}


} }

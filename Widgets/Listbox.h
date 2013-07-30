#pragma once


#include "Listbox/ListboxBase.h"
#include "../Utils/Property.h"
#include "Listbox/ListItem.h"
#include "../Utils/OrderedCollection.h"


namespace gge { namespace widgets {

	//multi select, direction keys

	namespace listbox {
#pragma warning(disable:4068)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
		template<class T_>
		graphics::RectangularGraphic2D *NullIcon(const T_ &) {
			return nullptr;
		}
		template <typename T>
		class has_GetIcon
		{
			typedef char one;
			struct two {
				char a[2];
			};

			template <typename C> static one test( decltype(((C*)(nullptr))->GetIcon()) aa ) {;}
			template <typename C> static two test(...){;}

		public:
			enum { value = sizeof(test<T>(""))==sizeof(char) };
		};

		template <typename T>
		class has_HasIcon
		{
			typedef char one;
			struct two {
				char a[2];
			};

			template <typename C> static one test( decltype(((C*)(nullptr))->HasIcon()) aa ) {;}
			template <typename C> static two test(...){;}

		public:
			enum { value = sizeof(test<T>(""))==sizeof(char) };
		};

		template<class T_>
		typename std::enable_if<has_GetIcon<T_>::value && has_HasIcon<T_>::value, graphics::RectangularGraphic2D *>::type GetIcon(const T_ &item) {
			if(item.HasIcon())
				return &item.GetIcon();
			else
				return nullptr;
		}

		template<class T_>
		typename std::enable_if<has_GetIcon<T_>::value && !has_HasIcon<T_>::value, graphics::RectangularGraphic2D *>::type GetIcon(const T_ &item) {
			return item.GetIcon();
		}

		
		template<class T_>
		typename std::enable_if<!has_GetIcon<T_>::value, graphics::RectangularGraphic2D *>::type GetIcon(const T_ &item) {
			return nullptr;
		}
#pragma GCC diagnostic pop
	}

	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_>,  graphics::RectangularGraphic2D*(*GetIcon)(const T_&)=listbox::GetIcon<T_> >
	class Collectionbox;

	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_>,  graphics::RectangularGraphic2D*(*GetIcon)(const T_&)=listbox::GetIcon<T_> >
	class Listbox;

	namespace listbox {
		template<class T_>
		class ItemData {
		public:
			T_ item;
			bool selected;

			ItemData(T_ item=T_(), bool selected=false) : item(item), selected(selected) { }

			bool operator ==(const ItemData &l) const {
				return item==l.item;
			}

			bool operator !=(const ItemData &l) const {
				return item==l.item;
			}

			bool operator <(const ItemData &l) const {
				return item<l.item;
			}

			bool operator >(const ItemData &l) const {
				return item>l.item;
			}

			T_ Get() const {
				return item;
			}
		};

		template<class T_, class P_>
		struct collectionboxelementcompare {
			collectionboxelementcompare(const P_ &predicate) : predicate(predicate) {}

			bool operator()(const ItemData<T_*> &l, const ItemData<T_*> &r) {
				return predicate(*l.item,*r.item);
			}

			const P_ &predicate;
		};

		template<class T_, class P_>
		struct listboxelementcompare {
			listboxelementcompare(const P_ &predicate) : predicate(predicate) {}

			bool operator()(const ItemData<T_> &l, const ItemData<T_> &r) {
				return predicate(l.item,r.item);
			}

			const P_ &predicate;
		};

		template<class T_>
		struct valueaccessor {
			typedef T_ storagetype;
			typedef T_ returntype;
			typedef ItemData<T_> datatype;
			typedef const T_ paramtype;

			static T_ Get(const ItemData<T_> data) {
				return data.item;
			}

			static T_ StorageToReturnType(T_ storage) {
				return storage;
			}

			static T_ &StorageToModifyable(T_ &storage) {
				return storage;
			}

			static const T_ &StorageToModifyable(const T_ &storage) {
				return storage;
			}

			static T_ ParamToStorageType(T_ storage) {
				return storage;
			}

			static void Set(ItemData<T_> &data, const T_ &v) {
				data.item=v;
			}

			static ItemData<T_> New(const T_ &v) {
				return ItemData<T_>(v);
			}
		};

		template<class T_>
		struct ptraccessor {
			typedef T_ *storagetype;
			typedef T_  returntype;
			typedef ItemData<T_*> datatype;
			typedef T_ paramtype;

			static T_ &Get(const ItemData<T_> data) {
				return *data.item;
			}

			static T_&StorageToReturnType(T_ *storage) {
				return *storage;
			}

			static T_&StorageToModifyable(T_ *storage) {
				return *storage;
			}

			static const T_&StorageToModifyable(const T_ *storage) {
				return *storage;
			}

			static T_ *ParamToStorageType(T_ &storage) {
				return &storage;
			}

			static void Set(ItemData<T_*> &data, T_ &v) {
				data.item=&v;
			}

			static ItemData<T_*> New(T_ &v) {
				return ItemData<T_*>(&v);
			}
		};

		template<class T_, class A_, void(*CF_)(const T_ &, std::string &), graphics::RectangularGraphic2D*(*GetIcon)(const T_&)>
		class Basic : public listbox::Base<ListItem> {
		public:

			enum SelectionTypes {
				SingleSelect,
				ToggleSelect, //click toggles
				MultiSelect, //control click adds to selected, support for shift multi select and drag multi select
			};

			template<class Ty_, class Ac_>
			class SelectionIterator;

			template<class Ty_, class Ac_>
			class Iterator : public gge::utils::IteratorBase<Iterator<Ty_, Ac_>, Ty_> {
				friend class Basic;
				friend class gge::utils::IteratorBase<Iterator<Ty_, Ac_>, Ty_>;
			public:
				Iterator(const Iterator &it) : listbox(it->listbox), index(it->index) {
				}

				Iterator() : listbox(NULL), index(0) {
				}

				Iterator(const SelectionIterator<Ty_, Ac_> &it) : listbox(it.listbox), index(it.index) {
				}

				operator unsigned int() const {
					return index;
				}

			private:
				Iterator(Basic *listbox, int index) : listbox(listbox), index(index) {

				}

				Basic *listbox;
				int index;

			protected:
				typename Ac_::returntype& current() const {
#ifdef _DEBUG
					if(!listbox || index<0 || index>=listbox->GetCount())
						throw std::runtime_error("Iterator is invalid state and cannot be read from.");
#endif

					return (*listbox)[index];
				}
				bool moveby(int amount) {
#ifdef _DEBUG
					if(!listbox)
						throw std::runtime_error("Iterator is not initialized.");
#endif
					index+=amount;
					if(index<0) {
						index=-1;
						return false;
					}
					else if(index>listbox->GetCount()) {
						index=listbox->GetCount();
						return false;
					}
					else
						return true;
				}
				bool isvalid() const {
					return listbox && index<listbox->GetCount() && index>=0;
				}
				bool compare(const Iterator &it) const {
					return it.listbox==listbox && it.index==index;
				}
				void set(const Iterator &it) {
					listbox=it.listbox;
					index=it.index;
				}
				int distance(const Iterator &it) const {
#ifdef _DEBUG
					if(!listbox)
						throw std::runtime_error("Iterator is not initialized.");

					if(listbox!=it.listbox)
						throw std::runtime_error("Iterators are not from the same container.");
#endif
					return index-it.index;
				}
				bool isbefore(const Iterator &it) const {
					return distance(it)<0;
				}
			};


			template<class Ty_, class Ac_>
			class SelectionIterator : public gge::utils::IteratorBase<SelectionIterator<Ty_, Ac_>, Ty_> {
				friend class Basic;
				friend class gge::utils::IteratorBase<SelectionIterator<Ty_, Ac_>, Ty_>;
			public:
				SelectionIterator(const SelectionIterator &it) : listbox(it.listbox), index(it.index) {
				}

				SelectionIterator() : listbox(NULL), index(0) {
				}

				operator Iterator<Ty_, Ac_>() const {
					return Iterator<Ty_, Ac_>(*this);
				}

				operator unsigned int() const {
					return index;
				}

			private:
				SelectionIterator(Basic *listbox, int index) : listbox(listbox), index(index) {
					if(!listbox->IsSelected(index))
						moveby(1);
				}

				Basic *listbox;
				int index;

			protected:
				typename Ac_::returntype& current() const {
#ifdef _DEBUG
					if(!listbox || index<0 || index>=listbox->GetCount())
						throw std::runtime_error("Iterator is invalid state and cannot be read from.");
#endif

					return (*listbox)[index];
				}
				bool moveby(int amount) {
#ifdef _DEBUG
					if(!listbox)
						throw std::runtime_error("Iterator is not initialized.");
#endif

					if(amount>0) {
						while(index<listbox->GetCount()-1 && amount) {
							index++;
							if(listbox->IsSelected(index))
								amount--;
						}
						if(amount>0)
							index=listbox->GetCount();
					}
					else if(amount<0) {
						while(index>0 && amount) {
							index--;
							if(listbox->IsSelected(index))
								amount++;
						}
						if(amount<0)
							index=-1;
					}

					if(index<0) {
						index=-1;
						return false;
					}
					else if(index>=listbox->GetCount()) {
						index=listbox->GetCount();
						return false;
					}
					else {
						return true;
					}
				}
				bool isvalid() const {
					return listbox && index<listbox->GetCount() && index>=0;
				}
				bool compare(const SelectionIterator &it) const {
					return it.listbox==listbox && it.index==index;
				}
				void set(const SelectionIterator &it) {
					listbox=it.listbox;
					index=it.index;
				}
				int distance(const SelectionIterator &it) const {
#ifdef _DEBUG
					if(!listbox)
						throw std::runtime_error("Iterator is not initialized.");

					if(listbox!=it.listbox)
						throw std::runtime_error("Iterators are not from the same container.");
#endif
					int ind=index, c=0;

					if(ind>it.index) {
						while(ind>it.index) {
							ind++;
							if(listbox->IsSelected(ind))
								c++;
						}
					}
					else if(ind<it.index) {
						while(ind<it.index) {
							ind--;
							if(listbox->IsSelected(ind))
								c++;
						}
					}
					return c;
				}
				bool isbefore(const SelectionIterator &it) const {
					return distance(it)<0;
				}
			};

			Basic() : selectiontype(SingleSelect),
				INIT_PROPERTY(Basic, SelectionType),
				INIT_PROPERTY(Basic, AutoHeight),
				INIT_PROPERTY(Basic, Columns),
				INIT_PROPERTY(Basic, AllowReorder),
				INIT_PROPERTY(Basic, ItemHeight),
				INIT_PROPERTY(Basic, AutoUpdate),
				itemheight(0), 
				autoupdate(false),
				initemcountqueue(false),
				activeindex(-1),
				selectedcount(0)
			{
				Main.BeforeRenderEvent.RegisterLambda([&]{ this->doadjust(); });
			}

			void Clear() {
				this->activeindex=-1;
				selectedcount=0;

				items.clear();
				delaysetitemcount(items.size());
			}

			int GetCount() const {
				return items.size();
			}

			template<class I_>
			void AddRange(const I_ &begin, const I_ &end) {
				for(auto it=begin;it!=end;++it)
					add(*it);
			}

			template<class C_>
			void AddAll(const C_ &container) {
				for(auto it=container.begin();it!=container.end();++it)
					add(*it);
			}

			//Call this function if not in auto update mode and you have changed
			//the items externally
			void Refresh() {
				adjustitems();
			}

			const T_ &operator[](unsigned index) const {
				return A_::StorageToModifyable(items[index].item);
			}

			//returns active item, may not be selected if multiselect is on
			const T_ &Get() const {
				if(activeindex==-1)
					throw std::runtime_error("Nothing active");

				return A_::StorageToModifyable(items[activeindex].item);
			}

			const T_ &Get(unsigned index) const {
				return A_::StorageToReturnType(items[index].item);
			}

			T_ &operator[](unsigned index)  {
				return A_::StorageToModifyable(items[index].item);
			}

			//returns active item, may not be selected if multiselect is on
			T_ &Get()  {
				if(activeindex==-1)
					throw std::runtime_error("Nothing active");

				return A_::StorageToModifyable(items[activeindex].item);
			}

			T_ &Get(unsigned index)  {
				return A_::StorageToReturnType(items[index].item);
			}

			bool HasSelection() const {
				if(selectiontype==SingleSelect) {
					return activeindex!=-1;
				}
				else {
					return selectedcount>0;
				}
			}

			//returns active index, may not be selected if multiselect is on
			int ActiveIndex() const {
				return activeindex;
			}

			void SetIndex(unsigned index) {
				if(selectiontype==SingleSelect) {
					if(index!=activeindex) {
						activeindex=index;
						items[activeindex].selected=true;

						selectedcount=1;

						adjustitems();
					}
				}
				else {
					ClearSelection();

					activeindex=index;
					items[activeindex].selected=true;

					selectedcount=1;

					adjustitems();
				}
			}

			void Select(unsigned index) {
				if(selectiontype==SingleSelect) {
					if(activeindex!=index) {
						if(activeindex!=-1) {
							items[activeindex].selected=false;
						}

						activeindex=index;
						items[activeindex].selected=true;
						adjustitems();
					}
				}
				else {
					if(!items[index].selected) {
						items[index].selected=true;
						selectedcount++;
						adjustitems();
					}
				}
			}

			void SelectAll() {
				for(auto it=items.begin();it!=items.end();++it) {
					it->selected=true;
				}
				selectedcount=items.size();
			}

			void Deselect() {
				ClearSelection();
			}

			void Deselect(unsigned index) {
				if(selectiontype==SingleSelect) {
					if(items[index].selected) {
						activeindex=-1;
						items[index].selected=false;
						selectedcount--;
						adjustitems();
					}
				}
				else {
					if(items[index].selected) {
						items[index].selected=false;
						selectedcount--;
						adjustitems();
					}
				}
			}

			void ClearSelection() {
				for(auto it=items.begin();it!=items.end();++it) {
					it->selected=false;
				}
				selectedcount=0;
			}

			bool IsSelected(unsigned index) const {
				return items[index].selected;
			}

			Iterator<T_, A_> begin() {
				return Iterator<T_, A_>(this, 0);
			}

			Iterator<T_, A_> end() {
				return Iterator<T_, A_>(this, GetCount());
			}

			Iterator<T_, A_> First() {
				return begin();
			}

			Iterator<T_, A_> Last() {
				return Iterator<T_, A_>(this, GetCount()-1);
			}

			SelectionIterator<T_, A_> selbegin() {
				return SelectionIterator<T_, A_>(this, 0);
			}

			SelectionIterator<T_, A_> selend() {
				return SelectionIterator<T_, A_>(this, GetCount());
			}

			SelectionIterator<T_, A_> FirstSelected() {
				return selbegin();
			}

			SelectionIterator<T_, A_> LastSelected() {
				return SelectionIterator<T_, A_>(this, GetCount()-1);
			}


			using WidgetBase::SetBlueprint;

			virtual void SetBlueprint(const widgets::Blueprint &bp) {
				if(&bp==static_cast<const widgets::Blueprint*>(this->bp))
					return;

				listbox::Base<ListItem>::SetBlueprint(bp);
			}

			virtual bool KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key key) {
				using namespace input::keyboard;

				if(!Modifier::Check() && event==Event::Down) {
					if(key==KeyCodes::Down && activeindex<(int)items.size()-1) {
						if(selectiontype==SingleSelect) {
							Select(activeindex+1);
						}
						else {
							activeindex++;
						}
						EnsureVisible();
						adjustitems();

						return true;
					}
					if(key==KeyCodes::Up && activeindex>0) {
						if(selectiontype==SingleSelect) {
							Select(activeindex-1);
						}
						else {
							activeindex--;
						}
						EnsureVisible();
						adjustitems();

						return true;
					}
					if(key==KeyCodes::PageDown) {
						int target=activeindex+panel.Widgets.GetCount()-1;
						if(target>=(int)items.size()) {
							target=items.size()-1;
						}
						if(selectiontype==SingleSelect) {
							Select(target);
						}
						else {
							activeindex=target;
						}
						EnsureVisible();
						adjustitems();

						return true;
					}
					if(key==KeyCodes::PageUp) {
						int target=activeindex-panel.Widgets.GetCount()+1;
						if(target<0) {
							target=0;
						}
						if(selectiontype==SingleSelect) {
							Select(target);
						}
						else {
							activeindex=target;
						}
						EnsureVisible(target);
						adjustitems();

						return true;
					}
				}
				if(!Modifier::Check() && event==Event::Up) {
					if(key==KeyCodes::Home && items.size()) {
						if(selectiontype==SingleSelect) {
							Select(0);
						}
						else {
							activeindex=0;
						}
						EnsureVisible();
						adjustitems();

						return true;
					}
					if(key==KeyCodes::End) {
						if(selectiontype==SingleSelect) {
							Select(items.size()-1);
						}
						else {
							activeindex=items.size()-1;
						}
						EnsureVisible();
						adjustitems();

						return true;
					}
				}

				return false;
			}

			void EnsureVisible() {
				if(activeindex>-1)
					EnsureVisible(activeindex);
			}
			void EnsureVisible(int index) {
				flushitemcount();
				if(index>organizer.GetTop()+panel.Widgets.GetCount()-2) {
					panel.SetVScroll(organizer.GetLogicalLocation(index+1).y-panel.GetUsableHeight());
				}
				else if(index<organizer.GetTop()) {
					panel.SetVScroll(organizer.GetLogicalLocation(index).y);
				}
			}
			void CenterItem() {
				if(activeindex>-1)
					CenterItem(activeindex);
			}

			void CenterItem(int index) {
				flushitemcount();
				if(panel.Widgets.GetCount())
					panel.SetVScroll(organizer.GetLogicalLocation(index).y-panel.GetUsableHeight()/2+panel.Widgets.First()->GetHeight()/2);
			}

			utils::Property<Basic, SelectionTypes> SelectionType;
			utils::NumericProperty<Basic, int> ItemHeight;
			utils::BooleanProperty<Basic> AutoHeight;
			utils::NumericProperty<Basic, int> Columns;
			utils::BooleanProperty<Basic> AllowReorder;
			utils::BooleanProperty<Basic> AutoUpdate;



		protected:
			void setSelectionType(const SelectionTypes &value) {
				if(selectiontype==MultiSelect && value!=MultiSelect) {
					this->ClearSelection();
				}
				selectiontype=value;
			}
			SelectionTypes getSelectionType() const {
				return selectiontype;
			}

			void setItemHeight(const int &value) {
				if(itemheight!=value) {
					itemheight=value;
					for(auto widget=this->representations.begin();widget.IsValid();widget.Next()) {
						widget->SetHeight(value);
					}
					itemheightchanged();
				}
			}
			int getItemHeight() const {
				return itemheight;
			}

			void setAutoUpdate(const bool &value) {
				autoupdate = value;
			}
			bool getAutoUpdate() const {
				return autoupdate;
			}

			virtual void checkdelete() {

			}

			virtual int elementheight() {
				if(itemheight!=0)
					return itemheight;
				else if(bp)
					return bp->Item->DefaultSize.Height;
				else
					return 25;
			}

			virtual void adjustitems() {
				updaterequired=true;
			}

			void flushitemcount() {
				if(initemcountqueue) {
					setitemcount(delayedcount);
					initemcountqueue=false;
				}
			}

			void doadjust() {
				if(!autoupdate && !updaterequired) return;

				updaterequired=false;

				if(initemcountqueue) {
					this->setitemcount(delayedcount);
					initemcountqueue=false;
				}

				unsigned target=organizer.GetTop()+panel.Widgets.GetCount();
				if(target>items.size())
					target=items.size();

				int j=0;
				for(unsigned i=organizer.GetTop();i<target;i++) {
					std::string str;
					CF_(A_::StorageToReturnType(items[i].item), str);
					representations[j].SetText(str);

					graphics::RectangularGraphic2D *icon;
					icon=GetIcon(A_::StorageToReturnType(items[i].item));
					representations[j].SetIcon(icon);

					if(items[i].selected) 
						representations[j].Select();
					else
						representations[j].Deselect();

					j++;
				}
			}

			virtual void trigger(ListItem &element, int index) {
				bool notifyselection=false;
				using namespace input::keyboard;

				auto &item=items[index];

				if(selectiontype==SingleSelect) {
					if(activeindex!=-1) {
						items[activeindex].selected=false;
						item.selected=true;
					}
					if(index!=activeindex) {
						notifyselection=true;
					}
					selectedcount=1;
				}
				else if(selectiontype==ToggleSelect) {
					item.selected=!item.selected;

					if(item.selected)
						selectedcount++;
					else
						selectedcount--;

					notifyselection=true;
				}
				else if(selectiontype==MultiSelect) {
					if(Modifier::Current==Modifier::Ctrl) {
						item.selected=!item.selected;

						if(item.selected)
							selectedcount++;
						else
							selectedcount--;

						notifyselection=true;
					}
					else if(Modifier::Current==Modifier::Shift) {
						int from=activeindex;
						int to=index;

						if(from>to) std::swap(from, to);

						for(int i=from;i<=to;i++) {
							if(!items[i].selected) {
								items[i].selected=true;
								notifyselection=true;
								selectedcount++;
							}
						}
					}
					else {
						if(selectedcount!=1 || !items[index].selected) {
							notifyselection=true;
							ClearSelection();
							items[index].selected=true;
							selectedcount=1;
						}
					}
				}

				activeindex=index;

				if(notifyselection) {
					selectionchanged();
				}

				clicked();
				adjustitems();
			}

			void setAutoHeight(const bool &value) {
				this->setautoheight(value);
			}
			bool getAutoHeight() const {
				return this->getautoheight();
			}
			void setColumns(const int &value) {
				this->setcolumns(value);
			}
			int getColumns() const {
				return this->getcolumns();
			}
			void setAllowReorder(const bool &value) {
				this->setallowreorder(value);
			}
			bool getAllowReorder() const {
				return this->getallowreorder();
			}

			virtual void wr_loaded() {
				if(WR.Listbox && !this->blueprintmodified)
					this->setblueprint(*WR.Listbox);
			}
			virtual void itemadded(typename A_::paramtype &, int index) {}
			virtual void itemremoving(int index) { }

			std::vector<typename A_::datatype> items;

			SelectionTypes selectiontype;

			int activeindex;
			int itemheight;
			int selectedcount;
			bool updaterequired;
			bool autoupdate;

		private:
			void delaysetitemcount(int count) {
				delayedcount=count;
				initemcountqueue=true;
			}

			int delayedcount;
			bool initemcountqueue;


		protected:
			virtual void add(typename A_::paramtype &v) {
				items.push_back(A_::New(v));
				delaysetitemcount(items.size());
				itemadded(v,items.size()-1);
			}
			virtual void insert(typename A_::paramtype &v, unsigned before) {
				items.insert(items.begin()+before, A_::ParamToStorageType(v));
				if(activeindex>=(int)before && activeindex!=-1)
					activeindex++;

				setitemcount(items.size());
				itemadded(v,std::min(before,items.size()-1));
			}
			virtual void movebefore(unsigned index, unsigned before) {
				if(before>=items.size()) {
					auto temp=items[index];
					items.erase(items.begin()+index);
					items.push_back(temp);

					if(activeindex!=-1) {
						if(activeindex==index) {
							activeindex=items.size()-1;
						}
						else if(activeindex<(int)index) {
							activeindex--;
						}
					}
				}
				else if(before==index || before==index+1) {
					return;
				}
				else if(before<index) {
					auto temp=items[index];
					for(unsigned i=index;i>before;i--) {
						items[i]=items[i-1];
					}
					items[before]=temp;

					if(activeindex!=-1) {
						if(activeindex==index) {
							activeindex=before;
						}
						else if(activeindex<(int)index && activeindex>=(int)before) {
							activeindex++;
						}
					}
				}
				else {
					auto temp=items[index];
					for(unsigned i=index;i<before-1;i++) {
						items[i]=items[i+1];
					}
					items[before-1]=temp;

					if(activeindex!=-1) {
						if(activeindex==index) {
							activeindex=before-1;
						}
						else if(activeindex>(int)index && activeindex<(int)before) {
							activeindex--;
						}
					}
				}

				this->adjustitems();

				reordered();
			}
			virtual bool remove(unsigned index) {
				typename A_::storagetype item=items[index].item;

				itemremoving(index);

				if(activeindex==index) {
					activeindex=-1;
				}
				else if(activeindex>(int)index) {
					activeindex--;
				}
				if(items[index].selected) {
					selectedcount--;
				}
				items.erase(items.begin()+index);

				delaysetitemcount(items.size());

				return true;
			}

			virtual void selectionchanged() = 0;
			virtual void reordered() = 0;
			virtual void clicked()=0;

		};
	}

	//This type of listbox is for reference typed objects, unless specified otherwise
	//these objects will not be duplicated or destroyed
	//Index based operations are in O(1) complexity.
	template<class T_, void(*CF_)(const T_ &, std::string &), graphics::RectangularGraphic2D*(*GetIcon)(const T_&) >
	class Collectionbox : public listbox::Basic<T_, listbox::ptraccessor<T_>, CF_, GetIcon> {
	public:


		typedef ListItem RepresentationType;
		typedef listbox::ItemData<T_*> DataType;
		
		typedef typename listbox::Basic<T_, listbox::ptraccessor<T_>, CF_, GetIcon>::template Iterator<T_, listbox::ptraccessor<T_> > Iterator;
		typedef typename listbox::Basic<T_, listbox::ptraccessor<T_>, CF_, GetIcon>::template Iterator<T_, listbox::ptraccessor<T_> > SelectedIterator;

		Collectionbox() : 
			ChangeEvent(this),
			ReorderEvent(this),
			ItemClickEvent(this),
			INIT_PROPERTY(Collectionbox, Value)
		{
			if(WR.Listbox)
				this->setblueprint(*WR.Listbox);
		}

		virtual ~Collectionbox() {

		}

		void Add(T_ &value) {
			add(value);
		}

		//nullptrs are ignored
		void Add(T_ *value) {
			if(value)
				Add(*value);
		}

		T_ &AddNew() {
			T_ &t=*new T_;
			add(t);

			return t;
		}

		void Insert(T_ &value, const  T_ *before) {
			insert(value, Find(before));
		}

		void Insert(T_ &value, const  T_ &before) {
			Insert(value, Find(before));
		}

		void Insert(T_ &value, unsigned before) {
			insert(value,before);
		}

		void Insert(T_ *value, const  T_ *before) {
			if(value)
				Insert(*value, before);
		}

		void Insert(T_ *value, const  T_ &before) {
			if(value)
				Insert(*value, &before);
		}

		void Insert(T_ *value, unsigned before) {
			if(value)
				Insert(*value, before);
		}

		void MoveBefore(T_ &value, const T_ &before) {
			this->movebefore(Find(value), Find(before));
		}

		void MoveBefore(T_ &value, unsigned before) {
			this->movebefore(Find(value), before);
		}

		void MoveBefore(unsigned index, const T_ &before) {
			this->movebefore(index, Find(before));
		}

		void MoveBefore(unsigned index, unsigned before) {
			this->movebefore(index,before);
		}

		void Remove(T_ &item) {
			this->Remove(&item);
		}

		void Remove(T_ *item) {
			this->remove(Find(item));
		}

		void Remove(unsigned index) {
			this->remove(index);
		}

		void Delete(T_ &item) {
			Delete(Find(item));
		}

		void Delete(T_ *item) {
			if(!item) return;

			Delete(Find(item));
		}

		void Delete(unsigned index) {
			T_ *item=&this->Get(index);

			this->remove(index);

			delete item;
		}

		void DeleteAll() {
			Destroy();
		}

		void Destroy() {
			for(auto it=this->items.begin();it!=this->items.end();++it) {
				delete it->item;
			}

			this->Clear();
		}

		void operator +=(T_ &value) {
			this->add(value);
		}

		void operator +=(T_ *value) {
			this->add(*value);
		}

		void SetSelected(T_ &item) {
			this->SetIndex(Find(item));
		}

		unsigned Find(const T_& item) {
			int index=0;
			for(auto it=this->items.begin(); it!=this->items.end(); ++it) {
				if(it->item==&item) return index;
				index++;
			}

			return -1;
		}

		unsigned Find(const T_ *item) {
			if(item) return Find(*item);

			return -1;
		}

		template<class P_>
		void Sort(P_ predicate=std::less<T_>()) {
			std::sort(this->items.begin(), this->items.end(), listbox::collectionboxelementcompare<T_, P_>(predicate));

			this->adjustitems();
		}

		void Sort() {
			std::sort(this->items.begin(), this->items.end(), listbox::collectionboxelementcompare<T_, std::less<T_> >(std::less<T_>()));

			this->adjustitems();
		}

		utils::EventChain<Collectionbox> ChangeEvent;
		utils::EventChain<Collectionbox> ReorderEvent;
		utils::EventChain<Collectionbox> ItemClickEvent;

		utils::ReferenceProperty<Collectionbox, T_> Value;


	protected:

		void setValue(T_ *value) {
			if(value)
				SetSelected(*value);
		}
		T_ *getValue() const {
			if(this->HasSelection())
				return this->items[this->ActiveIndex()].item;
			else
				return nullptr;
		}

		virtual void selectionchanged() {
			ChangeEvent();
		}

		virtual void reordered() {
			ReorderEvent();
		}

		virtual void clicked() {
			ItemClickEvent();
		}
	};


	//This type of listbox is for value typed objects, these items should be copyable, assignable and comparable
	template<class T_, void(*CF_)(const T_ &, std::string &), graphics::RectangularGraphic2D*(*GetIcon)(const T_&) >
	class Listbox : public listbox::Basic<T_, listbox::valueaccessor<T_>, CF_, GetIcon> {
	public:


		typedef ListItem RepresentationType;
		typedef listbox::ItemData<T_> DataType;
		
		typedef typename listbox::Basic<T_, listbox::valueaccessor<T_>, CF_, GetIcon>::template Iterator<T_, listbox::valueaccessor<T_> > Iterator;
		typedef typename listbox::Basic<T_, listbox::valueaccessor<T_>, CF_, GetIcon>::template SelectionIterator<T_, listbox::valueaccessor<T_> > SelectionIterator;

		Listbox() : 
			ChangeEvent(this),
			ReorderEvent(this),
			ItemClickEvent(this),
			INIT_PROPERTY(Listbox, Value)
		{
			if(WR.Listbox)
				this->setblueprint(*WR.Listbox);
		}

		virtual ~Listbox() {

		}

		void Add(const T_ &value) {
			this->add(value);
		}

		void Add() {
			this->add(T_());
		}

		void Insert(const T_ &value, unsigned before) {
			this->insert(value,before);
		}

		void MoveBefore(unsigned index, unsigned before) {
			this->movebefore(index,before);
		}

		void RemoveAll(const T_ &item) {
			for(int i=0;i<this->items.size();i++) {
				if(this->items[i].item==item)
					this->remove(i);
			}
		}

		void Remove(unsigned index) {
			this->remove(index);
		}

		void operator +=(const T_ &value) {
			this->add(value);
		}

		template<class P_>
		void Sort(P_ predicate=std::less<T_>()) {
			std::sort(this->items.begin(), this->items.end(), listbox::listboxelementcompare<T_, P_>(predicate));

			this->adjustitems();
		}

		void Sort() {
			std::sort(this->items.begin(), this->items.end(), listbox::listboxelementcompare<T_, std::less<T_> >(std::less<T_>()));

			this->adjustitems();
		}

		utils::EventChain<Listbox> ChangeEvent;
		utils::EventChain<Listbox> ReorderEvent;
		utils::EventChain<Listbox> ItemClickEvent;

		//If there are more than one items having this value
		//if listbox is in single select, it will select first one
		//if multiselect is possible, it will select all
		utils::Property<Listbox, T_> Value;


	protected:

		void setValue(const T_ &value) {
			if(this->selectiontype==listbox::Basic<T_, listbox::valueaccessor<T_>, CF_, GetIcon>::SingleSelect) {
				for(unsigned i=0;i<this->items.size();i++) {
					if(this->items[i].item==value) {
						this->Select(i);
						return;
					}
				}
			}
			else {
				for(unsigned i=0;i<this->items.size();i++) {
					if(this->items[i].item==value) {
						this->Select(i);
						return;
					}
					else {
						this->Deselect(i);
					}
				}
			}
		}
		T_ getValue() const {
			if(this->HasSelection())
				return this->Get();
			else
				return T_();
		}

		virtual void selectionchanged() {
			ChangeEvent();
		}

		virtual void reordered() {
			ReorderEvent();
		}

		virtual void clicked() {
			ItemClickEvent();
		}
	};


}}

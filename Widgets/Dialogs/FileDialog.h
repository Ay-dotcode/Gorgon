#pragma once

#include "../DialogWindow.h"
#include "../Listbox.h"
#include "../Textbox.h"
#include <Utils/CaptionValue.h>
#include "Message.h"
#include "Query.h"

namespace gge { namespace widgets { namespace dialog {



	class File : public DialogWindow {
	public:
		File() :
		  RepliedEvent(this),
		  INIT_PROPERTY(File, AskOverwrite),
		  INIT_PROPERTY(File, OnlyExisting),
		  INIT_PROPERTY(File, DirectoriesOnly),
		  INIT_PROPERTY(File, CurrentDirectory),
		  INIT_PROPERTY(File, Mask),
		  INIT_PROPERTY(File, Columns),
		  askoverwrite(false),
		  onlyexisting(true),
		  directoriesonly(false),
		  columns(2),
		  curdir("."),
		  mask("*.*")
		{
			curdir=os::filesystem::CanonizePath(curdir);
			init();
		}


		void RefreshPaths() {
			Paths.DeleteAll();
			std::vector<os::filesystem::EntryPoint> paths=os::filesystem::EntryPoints();

			for(auto it=paths.begin();it!=paths.end();++it) {
				if(!it->Readable) continue;
				if(it->Name=="") {
					Paths.Add(utils::CaptionValue<std::string>(it->Path, it->Path));
				}
				else if(it->Path.length()<10) {
					Paths.Add(utils::CaptionValue<std::string>(it->Path, it->Name+" ["+it->Path+"]"));
				}
				else {
					Paths.Add(utils::CaptionValue<std::string>(it->Path, it->Name));
				}
			}
		}

		void RefreshFiles() {
			Files.DeleteAll();

			std::string updir=os::filesystem::CanonizePath(curdir+"/..");
			Up.SetEnabled(updir!=curdir && os::filesystem::IsDirectoryExists(updir));

			if(!directoriesonly && Filename.Text=="")
				Ok.Disable();

			//Directories
			for(auto it=os::filesystem::DirectoryIterator(curdir);it!=os::filesystem::EndOfDirectory;++it) {
				if(*it!="." && *it!=".." && os::filesystem::IsDirectoryExists(curdir+"/"+*it) && !os::filesystem::IsPathHidden(curdir+"/"+*it)) {
					Files.Add(utils::CaptionValue<std::string>("/"+os::filesystem::CanonizePath(curdir+"/"+*it),"["+*it+"]"));
				}
			}

			if(directoriesonly) return;

			//Files
			for(auto it=os::filesystem::DirectoryIterator(curdir, mask);it!=os::filesystem::EndOfDirectory;++it) {
				if(*it!="." && *it!=".." && os::filesystem::IsFileExists(curdir+"/"+*it) && !os::filesystem::IsPathHidden(curdir+"/"+*it)) {
					Files.Add(utils::CaptionValue<std::string>(os::filesystem::CanonizePath(curdir+"/"+*it),*it));
				}
			}
		}

		void Show(bool focus=true) {
			DialogWindow::Show(focus);
			Filename.Focus();
		}

		utils::EventChain<File, std::string> RepliedEvent;

		utils::BooleanProperty<File> AskOverwrite;
		utils::BooleanProperty<File> OnlyExisting;
		utils::BooleanProperty<File> DirectoriesOnly;
		utils::TextualProperty<File> CurrentDirectory;
		utils::TextualProperty<File> Mask;
		utils::NumericProperty<File, int> Columns;


	protected:

		void init() {
			if(activevp)
				SetContainer(activevp);
			else
				SetContainer(TopLevel);

			SetBlueprint(WR.Panels.DialogWindow);


			Up.Text=" Up ";
			Up.Autosize=AutosizeModes::Autosize;
			Up.SetContainer(this);
			Up.SetBlueprint(WR.Buttons.Navigation);
			Up.ClickEvent.RegisterLambda([&]{
				curdir=os::filesystem::CanonizePath(curdir+"/..");
				RefreshFiles();
			});

			Paths.ItemClickedEvent.Register(this,&File::path_click);
			Paths.SetContainer(this);



			//Files.SetHeight(Files.GetHeight()-Files.GetY());
			Files.ItemClickedEvent.Register(this,&File::file_click);
			Files.SetContainer(this);

			Filename.SetContainer(this);
			Filename.ChangeEvent.RegisterLambda([&]{
				Ok.SetEnabled(directoriesonly || Filename.Text!="");
			});


			dialogbuttons.Add(Cancel);
			placedialogbutton(Cancel);
			Cancel.Text="Cancel";
			this->SetCancel(Cancel);
			Cancel.Autosize=AutosizeModes::GrowOnly;
			Cancel.ClickEvent.RegisterLambda([&]{ Close(); });

			dialogbuttons.Add(Ok);
			placedialogbutton(Ok);
			Ok.Text="OK";
			Ok.SetEnabled(directoriesonly);
			this->SetDefault(Ok);
			Ok.Autosize=AutosizeModes::GrowOnly;
			Ok.ClickEvent.Register(this, &File::Accept);

			RefreshPaths();
			RefreshFiles();

			relocate();
		}

		void relocate() {
			Paths.SetY(Up.GetHeight()+WR.WidgetSpacing.y);
			Paths.SetHeight(WR.Listbox->DefaultSize.Height-Paths.GetY());

			Up.SetX(Paths.GetWidth()-Up.GetWidth());

			Files.SetWidth(WR.Listbox->DefaultSize.Width*columns);
			Files.Columns=columns;
			Files.Move(Paths.GetWidth()+WR.WidgetSpacing.x, 0);

			Filename.SetWidth(Files.GetBounds().Right);
			Filename.SetY(Files.GetBounds().Bottom+WR.WidgetSpacing.y);

			this->SetWidth(Files.GetBounds().Right+this->GetOverheadMargins().TotalX());
			this->SetHeight(Filename.GetBounds().Bottom+this->GetOverheadMargins().TotalY());


			MoveToCenter();
			this->SetHeight(Filename.GetBounds().Bottom+this->GetOverheadMargins().TotalY()+1);

		}

		void Accept() { 
			std::string f=curdir+"/"+std::string(Filename.Text);
			if(directoriesonly && os::filesystem::IsFileExists(f)) {
				ShowMessage("Please select a directory.", Title);
				return;
			}
			if(!directoriesonly && os::filesystem::IsDirectoryExists(f)) {
				ShowMessage("Please select a file.", Title);
				return;
			}
			if(onlyexisting) {
				if(directoriesonly) {
					if(!os::filesystem::IsDirectoryExists(f)) {
						ShowMessage("Directory not found: "+Filename.Text, Title);
						return;
					}
				}
				else {
					if(!os::filesystem::IsFileExists(f)) {
						ShowMessage("File not found: "+Filename.Text, Title);
						return;
					}
				}
			}
			if(askoverwrite) {
				if(os::filesystem::IsDirectoryExists(f)) {
					AskConfirm("The directory exists, do you still wish to continue?",Title).RepliedEvent
					.RegisterLambda([&](bool reply){
						if(reply) {
							std::string f=curdir+"/"+Filename.Text;
							this->RepliedEvent(f);
							Close();
						}
					});
					return;
				}
				else if(os::filesystem::IsFileExists(f)) {
					AskConfirm("The file exists, do you still wish to continue?",Title).RepliedEvent
					.RegisterLambda([&](bool reply){
						if(reply) {
							std::string f=curdir+"/"+Filename.Text;
							this->RepliedEvent(f);
							Close();
						}
					});
					return;
				}
			}
			RepliedEvent(f);
			Close();
		}

		void path_click(Listbox<utils::CaptionValue<std::string> >::ItemType &item) {
			curdir=utils::CaptionValue<std::string>(item.Value).value;
			RefreshFiles();
		}

		void file_click(Listbox<utils::CaptionValue<std::string> >::ItemType &item) {
			std::string v=utils::CaptionValue<std::string>(item.Value).value;

			if(v==selectedfile) {
				if(v[0]=='/') {
					v=v.substr(1);
					if(v==curdir) return;
					if(!os::filesystem::IsDirectoryExists(v)) {
						ShowMessage("The directory "+v+" is vanished.","Folder not found!");
						RefreshFiles();
						return;
					}
					else {
						curdir=v;
						RefreshFiles();
						Paths.ClearSelection();
						selectedfile="";
						if(!directoriesonly) {
							Ok.Disable();
						}
					}
				}
				else {
					RepliedEvent(selectedfile);
					Close();
				}
			}
			else {
				selectedfile=v;
				if( selectedfile[0]!='/' || directoriesonly ) {
					Filename.Text=utils::CaptionValue<std::string>(item.Value).caption;
					Ok.Enable();
				}
			}
		}

		void setAskOverwrite(const bool &value) {
			askoverwrite = value;
		}
		bool getAskOverwrite() const {
			return askoverwrite;
		}

		void setOnlyExisting(const bool &value) {
			onlyexisting = value;
		}
		bool getOnlyExisting() const {
			return onlyexisting;
		}

		void setDirectoriesOnly(const bool &value) {
			if(directoriesonly!=value) {
				directoriesonly = value;
				if(value)
					Ok.Enable();
				else {
					if(!directoriesonly && Filename.Text=="") {
						Ok.Disable();
					}
				}
				RefreshFiles();
			}
		}
		bool getDirectoriesOnly() const {
			return directoriesonly;
		}

		void setCurrentDirectory(const std::string &value) {
			std::string d=os::filesystem::CanonizePath(value);
			if(curdir!=d) {
				curdir = d;
				RefreshFiles();
			}
		}
		std::string getCurrentDirectory() const {
			return curdir;
		}

		void setMask(const std::string &value) {
			if(mask!=value) {
				mask = value;
				RefreshFiles();
			}
		}
		std::string getMask() const {
			return mask;
		}

		void setColumns(const int &value) {
			if(columns!=value) {
				columns = value;
				relocate();
			}
		}
		int getColumns() const {
			return columns;
		}

		std::string curdir;
		std::string mask;

		bool askoverwrite;
		bool onlyexisting;
		bool directoriesonly;

		int columns;


		std::string selectedfile;

		Listbox<utils::CaptionValue<std::string> > Paths;
		Listbox<utils::CaptionValue<std::string> > Files;
		Textbox Filename;
		Button Ok, Cancel;
		Button Up;
	};


} } }

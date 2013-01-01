#pragma once

#include "../DialogWindow.h"
#include "../Listbox.h"
#include "../Textbox.h"
#include <Utils/CaptionValue.h>
#include "Message.h"

namespace gge { namespace widgets { namespace dialog {



	class File : public DialogWindow {
	public:
		File() {
			curdir=os::filesystem::CanonizePath(".");
			mask="*.*";
			init();
		}


		void RefreshPaths() {
			Paths.DeleteAll();
			std::vector<os::filesystem::EntryPoint> paths=os::filesystem::EntryPoints();

			for(auto it=paths.begin();it!=paths.end();++it) {
				if(!it->Readable) continue;
				if(it->Name!="") {
					Paths.Add(utils::CaptionValue<std::string>(it->Path, it->Name+" ["+it->Path+"]"));
				}
				else {
					Paths.Add(utils::CaptionValue<std::string>(it->Path, it->Path));
				}
			}
		}

		void RefreshFiles() {
			Files.DeleteAll();

			//Directories
			for(auto it=os::filesystem::DirectoryIterator(curdir);it!=os::filesystem::EndOfDirectory;++it) {
				if(*it!="." && *it!=".." && os::filesystem::IsDirectoryExists(curdir+"/"+*it) && !os::filesystem::IsPathHidden(curdir+"/"+*it)) {
					Files.Add(utils::CaptionValue<std::string>("*"+os::filesystem::CanonizePath(curdir+"/"+*it),"["+*it+"]"));
				}
			}

			//Files
			for(auto it=os::filesystem::DirectoryIterator(curdir, mask);it!=os::filesystem::EndOfDirectory;++it) {
				if(*it!="." && *it!=".." && os::filesystem::IsFileExists(curdir+"/"+*it) && !os::filesystem::IsPathHidden(curdir+"/"+*it)) {
					Files.Add(utils::CaptionValue<std::string>(os::filesystem::CanonizePath(curdir+"/"+*it),*it));
				}
			}
		}



	protected:

		void init() {
			if(activevp)
				SetContainer(activevp);
			else
				SetContainer(TopLevel);

			SetBlueprint(WR.Panels.DialogWindow);


			Paths.ItemClickedEvent.Register(this,&File::path_click);
			this->AddWidget(Paths);

			Files.SetWidth(Files.GetWidth()*2);
			Files.Columns=2;
			Files.SetX(Paths.GetWidth()+WR.WidgetSpacing.x);
			Files.ItemClickedEvent.Register(this,&File::file_click);
			this->AddWidget(Files);


			this->SetWidth(Files.GetBounds().Right+this->GetOverheadMargins().TotalX());


			RefreshPaths();
			RefreshFiles();
		}

		void path_click(Listbox<utils::CaptionValue<std::string> >::ItemType &item) {
			curdir=utils::CaptionValue<std::string>(item.Value).value;
			RefreshFiles();
		}

		void file_click(Listbox<utils::CaptionValue<std::string> >::ItemType &item) {
			std::string v=utils::CaptionValue<std::string>(item.Value).value;

			if(v==selectedfile) {
				if(v[0]=='*') {
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
					}
				}
				else {
					//select file and return
				}
			}
			else {
				selectedfile=v;
			}
		}


		std::string curdir;
		std::string mask;

		std::string selectedfile;

		Listbox<utils::CaptionValue<std::string> > Paths;
		Listbox<utils::CaptionValue<std::string> > Files;
	};


} } }

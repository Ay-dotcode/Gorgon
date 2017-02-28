#include "Query.h"

using std::string;

namespace gge { namespace widgets { namespace dialog {

	std::vector<Query*> Queries;




	void Query::SetIcon(const std::string &icon) {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();

		Icon=WR.Icons(icon);
		iconowner=true;
	}

	void Query::RemoveIcon() {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
		Icon=NULL;
	}

	Query::~Query() {
		if(iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
	}

	void Query::resize() {
		if(GetUsableWidth()>0 && autosize) {
			SetHeight(Main.BoundingBox.Height());
			SetHeight(query.GetHeight()+GetHeight()-GetUsableHeight()+(*Padding).Bottom);

			autosize=true;

			if(autocenter)
				MoveToCenter();
		}
	}

	Query &AskConfirm(const string &query, const string &Title) {
		Query *m=NULL;
		for(auto it=Queries.begin();it!=Queries.end();++it) {
			if(!(*it)->IsVisible()) {
				m=*it;
			}
		}

		if(!m) {
			m=new Query;
		}

		m->reset();

		m->Title=Title;
		m->QueryText=query;
		m->Show(true);
		

		return *m;
	}

}}}

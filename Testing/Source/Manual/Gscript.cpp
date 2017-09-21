#include <Gorgon/Scripting.h>
#include <Gorgon/Scripting/VirtualMachine.h>
#include <Gorgon/Scripting/Embedding.h>
#include <thread>
#include <iostream>
#include <Gorgon/Scripting/VirtualMachine.h>
#include <Gorgon/Scripting/Compilers.h>
#include <Gorgon/Filesystem/Iterator.h>
#include <Gorgon/Filesystem.h>
#include <Gorgon/Enum.h>

using namespace Gorgon::Scripting;

const std::string source = "";

namespace Gorgon { namespace Scripting { namespace Compilers {
	extern bool showsvg__;
} } }

void togglesvg() {
	Compilers::showsvg__=!Compilers::showsvg__;
}

void testfill(std::string &s) {
	s+="abcdefg";
}

void testset(int &c) {
	c='5';
}

void dumprefs() {
	VirtualMachine::Get().References.list();
}

class NullTest {
};

NullTest *thisisnull() { return nullptr; }

void nonull(NullTest &n) {
	std::cout<<"Cannot be null :"<<(&n)<<std::endl;
}

void allownull(NullTest *n) {
	std::cout<<"Can be null :"<<n<<std::endl;
}

std::ostream &operator<<(std::ostream &out, const NullTest &n) {
	out<<(&n);
	return out;
}

namespace Gorgon { namespace Geometry {
	extern Scripting::Library LibGeometry;
	void init_scripting();
	
} }

void test() {
	std::string scriptdir="../Source/Unit/Scripts";
	Gorgon::Filesystem::Iterator dir(scriptdir);
		
	for( ; dir.IsValid(); dir.Next()) {
		std::ifstream file(scriptdir+"/"+dir.Current());
		std::cout<<*dir<<std::endl;

		std::string line;
		bool outputmode=false;
		std::string code;
		std::string output;
		
		while( std::getline(file, line) ) {
			if(outputmode) {
				output+=line+"\n";
			}
			else if(Gorgon::String::Trim(line)=="## OUTPUT ##") {
				outputmode=true;
			}
			else {
				code+=line+"\n";
			}
		}
		std::cout<<output<<std::endl;
		if(output!="") output=output.substr(0, output.length()-1);
		
		std::stringstream codestream(code), outputstream(output), scriptoutput;
		Gorgon::Scripting::StreamInput input(codestream, Gorgon::Scripting::InputProvider::Programming, dir.Current());
		Gorgon::Scripting::VirtualMachine &vm=Gorgon::Scripting::VirtualMachine::Get();
		
		//vm.SetOutput(scriptoutput);
		vm.Begin(input);
		vm.Run();
		
		std::string line2;
		while(std::getline(outputstream, line),std::getline(scriptoutput, line2)) {
			//REQUIRE(line==line2);
		}
		
		//REQUIRE(outputstream.rdstate()==scriptoutput.rdstate());
	}
}

std::vector<int> v(40);

Data SpecHandler(char c, std::string var) {
	if(c=='!') {
		return {Gorgon::Scripting::Types::String(), var};
	}
	else if(c=='%') {
		if(Gorgon::String::To<int>(var)<40 && Gorgon::String::To<int>(var)>0) {
			return {Gorgon::Scripting::Types::Int(), &v[Gorgon::String::To<int>(var)], true, false};
		}
	}
	
	return {};
}

class evttest {
public:
	evttest() : ev(*this) { }
	evttest(const evttest &) = delete;
	
	Gorgon::Event<evttest, int> ev;
};

enum class Day {
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};

DefineEnumStrings(Day,
	{Day::Monday, "Monday"},
	{Day::Tuesday, "Tuesday"},
	{Day::Wednesday, "Wednesday"},
	{Day::Thursday, "Thursday"},
	{Day::Friday, "Friday"},
	{Day::Saturday, "Saturday"},
	{Day::Sunday, "Sunday"}
);


enum class Direction {
	None  = 0,
	In    = 1,
	Out   = 2,
	InOut = 3,
};

DefineEnumStrings(Direction,
	{Direction::None, "None"},
	{Direction::In,   "In"},
	{Direction::Out,  "Out"},
	{Direction::InOut,"InOut"}
);

class imtest {
public:
	imtest() = default;
	imtest(std::string) { }
	
	operator std::string() const { return Gorgon::String::Concat(a, ",", b); }
	
	int a=0;
	int b=0;
	
	int getb() const { return b; }
	void setb(int v) { b=v; }
	
	bool operator ==(imtest l) const { return a==l.a && b==l.b; }
};

int main() {
	VirtualMachine vm;
	Gorgon::Geometry::init_scripting();
	vm.AddLibrary(Gorgon::Geometry::LibGeometry);
	vm.SetSpecialIdentifierHandler(SpecHandler);
    
    auto console = Gorgon::Utils::StdConsole();
	
	auto reftyp=new Gorgon::Scripting::MappedReferenceType<evttest, ToEmptyString<evttest>>("reftype", "");
	auto eventtype=new MappedEventType<
		Gorgon::Event<evttest, int>, evttest, void, int
	>(
		"evt", "help", {
			Parameter("int", "int", Types::Int())
		}
	);
	
	reftyp->MapConstructor<>({});
	reftyp->AddMembers({
		new MappedROInstanceMember<evttest*, Gorgon::Event<evttest, int>>(&evttest::ev, "event", "", eventtype)
	});
	
	evttest a;
	
	a.ev.Register([](int v){ std::cout<<"OK: "<<v<<std::endl; });
	
	
	
	//test();

	std::stringstream ss(source);
	ConsoleInput streaminput={InputProvider::Programming};


	Library mylib("GTTest", "");
	
	auto mystr = new MappedValueType<std::string>("MyStr", "");

	auto StringToMyStrConvert=[&mystr](Data data) { 
		return Data{mystr, data.GetValue<std::string>()}; 
	};

	auto MyStrToStringConvert=[&mystr](Data data) {
		return Data{Types::String(), data.GetValue<std::string>()}; 
	};
	mystr->AddInheritance(Types::String(), StringToMyStrConvert, MyStrToStringConvert);
	
	auto nulltype=new MappedReferenceType<NullTest>("NullTest", "");
	
	auto myregenum = new MappedStringEnum<Day>("Day", "");
	auto mybinenum = new MappedStringEnum<Direction>("Direction", "", Direction::In, true);
	auto imtester  = new MappedValueType<imtest>("imtest", "");
	imtester->MapConstructor<>({});
	imtester->AddMembers({
		new MappedInstanceMember<imtest, int>(
			&imtest::a, "a", "", Types::Int(), false, false
		),
		new MappedInstanceMember_Function(
			&imtest::getb, &imtest::setb, "b", "", Types::Int(), imtester, true, false, false, true
		)
	});
	
	mylib.AddMembers({
		nulltype, reftyp, myregenum, mybinenum, imtester
	});

	mylib.AddMembers({
		new Function("geta", "", nullptr,
			{
				MapFunction([&]{
					return &a;
				}, reftyp, {})
			}
		),
		new Function("svg", "", nullptr,
			{
				MapFunction(&togglesvg, nullptr, {})
			},
			KeywordTag
		),
		
		new Function("thisisnull", "", nullptr, 
			{
				MapFunction(&thisisnull, nulltype, {})
			}
		),
		
		new Function("nonull", "", nullptr, 
			{
				MapFunction(&nonull, nullptr, {
					Parameter("v", "", nulltype)
				})
			}
		),		
			
		new Function("allownull", "", nullptr, 
			{
				MapFunction(&allownull, nullptr, {
					Parameter("v", "", nulltype, AllowNullTag)
				})
			}
		),		
	
		new Function("testfill", "", nullptr, 
			{
				MapFunction(
					testfill, nullptr,
					{
						Parameter("a", "", Types::String(), Gorgon::Scripting::ReferenceTag)
					}
				)
			}
		),
		
		new Function("gen", "", nullptr,
			{
				MapFunction(
					[]{ 
						return std::string("abc"); 	
					}, mystr, 
					{ }
				), 
			}
		),
		
		new Function("testset", "", nullptr,
			{
				MapFunction(
					testset, nullptr, 
					{
						Parameter("c","",Types::Int(), Gorgon::Scripting::ReferenceTag)
					}
				), 
			}
		),

		new Function("dumprefs", "", nullptr,
			{
				MapFunction(
					dumprefs, nullptr, 
					{ }
				), 
			}, KeywordTag
		),
	});
	
	vm.AddLibrary(mylib);

	std::cout<<std::endl<<std::endl;
	console.SetBold();
	console.SetColor(console.Yellow);
	std::cout<<"OUTPUT: "<<std::endl;
	console.Reset();
	
	vm.Begin(streaminput);
	vm.UsingNamespace(mylib);
	vm.UsingNamespace(Reflection);
	//Compilers::Disassemble(input, std::cout);
	
	std::cout<<std::endl<<std::endl;
	

	while(vm.GetScopeInstanceCount()) {
		try {
			vm.Run();
		}
		catch(const Gorgon::Scripting::ParseError &ex) {
			
			
			std::cout<<std::endl<<std::endl;
			console.SetBold();
			console.SetColor(console.Yellow);
			std::cout<<"ERROR: "<<std::endl;
			console.Reset();
			
			console.SetBold();
			std::cout<<"At line "<<ex.GetLine()<<":"<<ex.Char+1;
			console.SetBold(false);
			console.SetColor(console.Red);
			std::cout<<": "<<ex.GetType();
			console.SetColor(console.Default);
			std::cout<<": "<<ex.GetMessage()<<std::endl;
			if(ex.GetDetails()!="") {
				std::cout<<" > "<<ex.GetDetails()<<std::endl;
			}
			
		}
		catch(const Exception &ex) {

			std::cout<<std::endl<<std::endl;
			console.SetBold();
			console.SetColor(console.Yellow);
			std::cout<<"SOURCE CODE: "<<std::endl;
			console.Reset();

			std::stringstream ss2(source);
			std::string line;
			int linenr=0;
			while(std::getline(ss2, line)) {
				linenr++;
				if(linenr==ex.GetLine())
					console.SetColor(console.Red);

				console.SetBold();
				std::cout<<std::setw(3)<<linenr<<" ";
				console.SetBold(false);
				std::cout<<line<<std::endl;
				console.Reset();
			}
			std::cout<<std::endl<<std::endl;


			std::cout<<std::endl<<std::endl;
			console.SetBold();
			console.SetColor(console.Yellow);
			std::cout<<"ERROR: "<<std::endl;
			console.Reset();

			console.SetBold();
			std::cout<<"At line "<<ex.GetLine();
			console.SetBold(false);
			console.SetColor(console.Red);
			std::cout<<": "<<ex.GetType();
			console.SetColor(console.Default);
			std::cout<<": "<<ex.GetMessage()<<std::endl;
			if(ex.GetDetails()!="") {
				std::cout<<" > "<<ex.GetDetails()<<std::endl;
			}
		}
		catch(const std::exception &ex) {
			console.SetColor(console.Red);
			console.SetBold();
			std::cout<<ex.what()<<std::endl;
			console.Reset();
			std::cout<<std::endl<<std::endl;
		}
	}

	std::cout<<std::endl<<std::endl;
	console.SetBold();
	console.SetColor(console.Yellow);
	std::cout<<"SOURCE CODE: "<<std::endl;
	console.Reset();

	std::stringstream ss2(source);
	std::string line;
	int linenr=0;
	while(std::getline(ss2, line)) {
		linenr++;
		console.SetBold();
		std::cout<<std::setw(3)<<linenr<<" ";
		console.SetBold(false);
		std::cout<<line<<std::endl;
		console.Reset();
	}
	std::cout<<std::endl<<std::endl;

	std::cout<<std::endl<<std::endl;
	console.SetColor(console.Green);
	std::cout<<"Execution successful."<<std::endl<<std::endl;
	
	Data ret=vm.GetReturnValue();
	if(ret.IsValid()) {
		std::cout<<"Return value: "<<ret<<std::endl;
	}
	console.Reset();

	//std::cout<<vm.GetVariable("a")<<std::endl;

	return 0;
}

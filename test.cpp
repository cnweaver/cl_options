#include <iostream>
#include <unistd.h>
#include "cl_options.h"

//Launder access to private class members for testing
class cl_options_test_access{
public:
	template<typename C>
	using TokenIterator=OptionParser::TokenIterator<C>;
};

void require(bool cond, const char* cond_s, unsigned int line){
	if(!cond)
		throw std::runtime_error("FAILURE: Line " + std::to_string(line) + ", " + cond_s);
}

#define REQUIRE(expr) \
require(expr,#expr,__LINE__);

void test_short_flag(){
	OptionParser op;
	bool fSet=false;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	const char* args[]={"program","-f"};
	std::vector<std::string> positionals;
	//deliberately omit the option
	positionals=op.parseArgs(1,args);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(!fSet);
	op.parseArgs(2,args);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(fSet);
}

void test_short_set_value(){
	OptionParser op;
	int i=0;
	op.addOption('i',i,"Set an integer");
	const char* args[]={"program","-i=52"};
	const char* badArgs[]={"program","-i"};
	const char* badArgs2[]={"program","-i="};
	//deliberately omit the option
	op.parseArgs(1,args);
	REQUIRE(i==0);
	
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,badArgs);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	try{
		op.parseArgs(2,badArgs2);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	
	op.parseArgs(2,args);
	REQUIRE(i==52);
}

void test_short_set_value2(){
	OptionParser op;
	int i=0;
	op.addOption('i',i,"Set an integer");
	const char* args[]={"program","-i","52"};
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,args);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	op.parseArgs(3,args);
	REQUIRE(i==52);
}

void test_short_value_no_equals(){
	OptionParser op;
	op.allowsShortValueWithoutEquals(true);
	int i=0;
	op.addOption('i',i,"Set an integer");
	const char* args[]={"program","-i52"};
	const char* badArgs[]={"program","-i"};
	const char* badArgs2[]={"program","-i="};
	const char* argsVerbose[]={"program","-i=52"};
	//deliberately omit the option
	op.parseArgs(1,args);
	REQUIRE(i==0);
	
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,badArgs);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	try{
		op.parseArgs(2,badArgs2);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	
	op.parseArgs(2,args);
	REQUIRE(i==52);
	
	//continuing to use an equals sign should still work
	i=0;
	op.parseArgs(2,argsVerbose);
	REQUIRE(i==52);
}

void test_short_set_value_callback(){
	OptionParser op;
	int i=0;
	op.addOption<int>('i',[&](int i_){i=i_;},"Set an integer");
	const char* args[]={"program","-i=52"};
	const char* badArgs[]={"program","-i"};
	//deliberately omit the option
	op.parseArgs(1,args);
	REQUIRE(i==0);
	
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,badArgs);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	
	op.parseArgs(2,args);
	REQUIRE(i==52);
}

void test_short_set_value_callback2(){
	OptionParser op;
	int i=0;
	op.addOption<int>('i',[&](int i_){i=i_;},"Set an integer");
	const char* args[]={"program","-i","52"};
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,args);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	op.parseArgs(3,args);
	REQUIRE(i==52);
}

void test_combined_short_options(){
	OptionParser op;
	op.allowsShortOptionCombination(true);
	bool fSet=false, gSet=false;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption('g',[&]{gSet=true;},"Set another flag");
	const char* args[]={"program","-fg"};
	const char* badArgs[]={"program","-fgx"};
	std::vector<std::string> positionals;
	//deliberately omit the option
	positionals=op.parseArgs(1,args);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(!fSet);
	REQUIRE(!gSet);
	positionals=op.parseArgs(2,args);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(fSet);
	REQUIRE(gSet);
	//including a nonsense option among valid ones should be caught
	fSet=gSet=false;
	try{
		op.parseArgs(2,badArgs);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(fSet);
	REQUIRE(gSet);
	//disable support and check that an error results
	op.allowsShortOptionCombination(false);
	fSet=gSet=false;
	try{
		op.parseArgs(2,args);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(!fSet);
	REQUIRE(!gSet);
}

void test_combined_short_options_with_value(){
	OptionParser op;
	op.allowsShortOptionCombination(true);
	bool fSet=false, gSet=false;
	int i=0;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption('g',[&]{gSet=true;},"Set another flag");
	op.addOption('i',i,"Set an integer");
	const char* args[]={"program","-fgi=17"};
	const char* args2[]={"program","-fgi","22"};
	const char* args3[]={"program","-fgi17"};
	std::vector<std::string> positionals;
	positionals=op.parseArgs(2,args);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(fSet);
	REQUIRE(gSet);
	REQUIRE(i==17);
	fSet=gSet=false;
	i=0;
	op.parseArgs(3,args2);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(fSet);
	REQUIRE(gSet);
	REQUIRE(i==22);
	//this should also play together with no-equals mode
	op.allowsShortValueWithoutEquals(true);
	fSet=gSet=false;
	i=0;
	positionals=op.parseArgs(2,args3);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(fSet);
	REQUIRE(gSet);
	REQUIRE(i==17);
}

void test_combined_short_options_with_value2(){
	//Trying to combine multiple short options which take values cannot work
	OptionParser op;
	op.allowsShortOptionCombination(true);
	std::string name;
	int i=0;
	op.addOption('i',i,"Set an integer");
	op.addOption('n',name,"A name");
	//justa single option with a value is fine
	const char* args[]={"program","-n=Edgar"};
	op.parseArgs(2,args);
	REQUIRE(name=="Edgar");
	//trying to jam two together does nothing useful
	const char* args2[]={"program","-n=Edgari=60"};
	op.parseArgs(2,args2);
	REQUIRE(name=="Edgari=60");
}


void test_long_flag(){
	OptionParser op;
	bool fSet=false;
	op.addOption("flag",[&]{fSet=true;},"Set a flag");
	const char* args[]={"program","--flag"};
	std::vector<std::string> positionals;
	//deliberately omit the option
	positionals=op.parseArgs(1,args);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(!fSet);
	positionals=op.parseArgs(2,args);
	REQUIRE(positionals.size()==1 && positionals[0]=="program");
	REQUIRE(fSet);
}

void test_long_set_value(){
	OptionParser op;
	int i=0;
	op.addOption("integer",i,"Set an integer");
	const char* args[]={"program","--integer=52"};
	const char* badArgs[]={"program","--integer"};
	//deliberately omit the option
	op.parseArgs(1,args);
	REQUIRE(i==0);
	
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,badArgs);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	
	op.parseArgs(2,args);
	REQUIRE(i==52);
}

void test_long_set_value2(){
	OptionParser op;
	int i=0;
	op.addOption("integer",i,"Set an integer");
	const char* args[]={"program","--integer","52"};
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,args);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	op.parseArgs(3,args);
	REQUIRE(i==52);
}

void test_long_set_value_callback(){
	OptionParser op;
	int i=0;
	op.addOption<int>("integer",[&i](int i_){i=i_;},"Set an integer");
	const char* args[]={"program","--integer=52"};
	const char* badArgs[]={"program","--integer"};
	//deliberately omit the option
	op.parseArgs(1,args);
	REQUIRE(i==0);
	
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,badArgs);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	
	op.parseArgs(2,args);
	REQUIRE(i==52);
}

void test_long_set_value_callback2(){
	OptionParser op;
	int i=0;
	op.addOption<int>("integer",[&i](int i_){i=i_;},"Set an integer");
	const char* args[]={"program","--integer","52"};
	//deliberately omit the value for the option
	try{
		op.parseArgs(2,args);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::runtime_error& err){}
	REQUIRE(i==0);
	op.parseArgs(3,args);
	REQUIRE(i==52);
}

void test_positionals(){
	OptionParser op;
	const char* args[]={"program","foo","bar","baz quux"};
	std::vector<std::string> positionals=op.parseArgs(4,args);
	REQUIRE(positionals.size()==4);
	for(unsigned int i=0; i<4; i++)
		REQUIRE(positionals[i]==args[i]);
}

void test_positionals_and_options(){
	bool fSet=false;
	int number=0;
	OptionParser op;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption("integer",number,"Set an integer");
	const char* args[]={"program","foo","-f","bar","--integer","17","baz quux"};
	std::vector<std::string> positionals=op.parseArgs(7,args);
	REQUIRE(positionals.size()==4);
	REQUIRE(fSet);
	REQUIRE(number==17);
}

void test_option_terminator(){
	OptionParser op;
	bool fSet=false, gSet=false;
	int i=0;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption('g',[&]{gSet=true;},"Set another flag");
	op.allowsOptionTerminator(true);
	const char* args[]={"program","foo","-g","bar","--","baz","-f"};
	std::vector<std::string> positionals=op.parseArgs(7,args);
	REQUIRE(positionals.size()==5);
	REQUIRE(gSet);
	//option-like arguments after the option terminator should be passed through
	//as positionals
	REQUIRE(!fSet);
	REQUIRE(positionals.back()=="-f");
}

void test_token_iterator(){
	std::string rawInput=R"(foo "bar baz" quux\ xen
	'"hom" \drel')";
	std::istringstream iss(rawInput);
	using CharIterator=std::istreambuf_iterator<char>;
	using TokenIterator=cl_options_test_access::TokenIterator<CharIterator>;
	CharIterator cit(iss), cend;
	TokenIterator it(cit,cend), end(cend,cend);
	REQUIRE(it!=end);
	REQUIRE(*it=="foo");
	
	it++;
	REQUIRE(it!=end);
	REQUIRE(*it=="bar baz");
	
	++it;
	REQUIRE(it!=end);
	REQUIRE(*it=="quux xen");
	
	++it;
	REQUIRE(it!=end);
	REQUIRE(*it=="\"hom\" \\drel");
}

void test_positionals_and_options_from_stream(){
	bool fSet=false;
	int number=0;
	OptionParser op;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption("integer",number,"Set an integer");
	std::string args="program foo -f bar --integer 17 'baz quux'";
	std::istringstream iss(args);
	std::vector<std::string> positionals=op.parseArgsFromStream(iss);
	REQUIRE(positionals.size()==4);
	REQUIRE(fSet);
	REQUIRE(number==17);
}

void test_config_file_parsing_short_option(){
	bool fSet=false;
	int number=0;
	OptionParser op;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption("integer",number,"Set an integer");
	op.addConfigFileOption('c',"Read config from a file");
	{
		std::ofstream o1(".test_c1");
		o1 << "-f -c .test_c2";
		std::ofstream o2(".test_c2");
		o2 << "--integer 17";
	}
	const char* args[]={"program","foo","-c",".test_c1","bar","baz quux"};
	std::vector<std::string> positionals=op.parseArgs(6,args);
	unlink(".test_c1");
	unlink(".test_c2");
	REQUIRE(positionals.size()==4);
	REQUIRE(fSet);
	REQUIRE(number==17);
}

void test_config_file_parsing_long_option(){
	bool fSet=false;
	int number=0;
	OptionParser op;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption("integer",number,"Set an integer");
	op.addConfigFileOption("config","Read config from a file");
	{
		std::ofstream o1(".test_c1");
		o1 << "-f --config .test_c2";
		std::ofstream o2(".test_c2");
		o2 << "--integer 17";
	}
	const char* args[]={"program","foo","--config",".test_c1","bar","baz quux"};
	std::vector<std::string> positionals=op.parseArgs(6,args);
	unlink(".test_c1");
	unlink(".test_c2");
	REQUIRE(positionals.size()==4);
	REQUIRE(fSet);
	REQUIRE(number==17);
}

void test_config_file_parsing_short_and_long_options(){
	bool fSet=false;
	int number=0;
	OptionParser op;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption("integer",number,"Set an integer");
	op.addConfigFileOption({"c","config"},"Read config from a file");
	{
		std::ofstream o1(".test_c1");
		o1 << "-f --config .test_c2";
		std::ofstream o2(".test_c2");
		o2 << "--integer 17";
	}
	const char* args[]={"program","foo","-c",".test_c1","bar","baz quux"};
	std::vector<std::string> positionals=op.parseArgs(6,args);
	unlink(".test_c1");
	unlink(".test_c2");
	REQUIRE(positionals.size()==4);
	REQUIRE(fSet);
	REQUIRE(number==17);
}

void test_config_file_parsing_loop(){
	bool fSet=false;
	int number=0;
	OptionParser op;
	op.addOption('f',[&]{fSet=true;},"Set a flag");
	op.addOption("integer",number,"Set an integer");
	op.addConfigFileOption("config","Read config from a file");
	{
		std::ofstream o1(".test_c1");
		o1 << "-f --config .test_c2";
		std::ofstream o2(".test_c2");
		o2 << "--integer 17 --config .test_c1";
	}
	const char* args[]={"program","foo","--config",".test_c1","bar","baz quux"};
	try{
		std::vector<std::string> positionals=op.parseArgs(6,args);
		REQUIRE(false && "An exception should be thrown");
	}catch(std::exception& ex){
		std::string err=ex.what();
		REQUIRE(err.find("Configuration file loop")!=std::string::npos);
	}
	unlink(".test_c1");
	unlink(".test_c2");
}

#define DO_TEST(test) \
	do{ \
	try{ \
		test(); \
	}catch(std::out_of_range& ex){ \
		std::cout << #test << ":\n\tOut of Range: " << ex.what() << std::endl; \
		failures++; \
	}catch(std::exception& ex){ \
		std::cout << #test << ":\n\t" << ex.what() << std::endl; \
		failures++; \
	}catch(...){ \
		failures++; \
	} \
	}while(0)

int main(){
	unsigned int failures=0;
	
	DO_TEST(test_short_flag);
	DO_TEST(test_short_set_value);
	DO_TEST(test_short_set_value2);
	DO_TEST(test_short_value_no_equals);
	DO_TEST(test_short_set_value_callback);
	DO_TEST(test_short_set_value_callback2);
	DO_TEST(test_combined_short_options);
	DO_TEST(test_combined_short_options_with_value);
	DO_TEST(test_combined_short_options_with_value2);
	DO_TEST(test_long_flag);
	DO_TEST(test_long_set_value);
	DO_TEST(test_long_set_value2);
	DO_TEST(test_long_set_value_callback);
	DO_TEST(test_long_set_value_callback2);
	DO_TEST(test_positionals);
	DO_TEST(test_positionals_and_options);
	DO_TEST(test_option_terminator);
	DO_TEST(test_token_iterator);
	DO_TEST(test_positionals_and_options_from_stream);
	DO_TEST(test_config_file_parsing_short_option);
	DO_TEST(test_config_file_parsing_long_option);
	DO_TEST(test_config_file_parsing_short_and_long_options);
	DO_TEST(test_config_file_parsing_loop);
	
	if(!failures)
		std::cout << "Test successful" << std::endl;
	else
		std::cout << failures << (failures==1?" failure":" failures") << std::endl;
}

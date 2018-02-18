#include <iostream>
#include "cl_options.h"

#define REQUIRE(expr) \
	require(expr,#expr,__LINE__);

void require(bool cond, const char* cond_s, unsigned int line){
	if(!cond)
		throw std::runtime_error("FAILURE: Line " + std::to_string(line) + ", " + cond_s);
}

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
	op.parseArgs(2,args);
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
	DO_TEST(test_long_flag);
	DO_TEST(test_long_set_value);
	DO_TEST(test_long_set_value2);
	DO_TEST(test_long_set_value_callback);
	DO_TEST(test_long_set_value_callback2);
	DO_TEST(test_positionals);
	DO_TEST(test_positionals_and_options);
	
	if(!failures)
		std::cout << "Test successful" << std::endl;
	else
		std::cout << failures << (failures==1?" failure":" failures") << std::endl;
}

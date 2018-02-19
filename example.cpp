///This simple example demonstrates implementing a subset of the `tar` utility's
///option parsing. 

#include <iostream>
#include "cl_options.h"

int main(int argc, char* argv[]){
	//Set up options
	OptionParser op;
	op.setBaseUsage("This is toy example of the tar interface");
	bool create=false, extract=false;
	op.addOption('c', [&]{create=true;}, 
	  "Create a new archive containing the specified items.");
	op.addOption('x', [&]{extract=true;}, 
	  "Extract to disk from the archive.");
	std::string archiveFile;
	op.addOption('f', archiveFile, 
	  "Read the archive from or write the archive to the specified file.",
	  "archive");
	op.allowsShortOptionCombination(true);
	op.allowsOptionTerminator(true);
	auto positionals = op.parseArgs(argc, argv);
	
	//If the user asked for the help message there is probably nothing else 
	//useful to do
	if(op.didPrintUsage())
		return(0);
	//Check that the specified options make sense
	if(create && extract){
		std::cerr << "-c and -x cannot be specified at the same time" << std::endl;
		return(1);
	}
	if(!create && !extract){
		std::cerr << "Either -c or -x must be specified" << std::endl;
		return(1);
	}
	if(archiveFile.empty()){
		std::cerr << "No archive file specified" << std::endl;
		return(1);
	}
	//Demonstrate what effect the options had
	if(create){
		if(positionals.size()==1){
			std::cerr << "No input files were specified from which to create the archive" << std::endl;
			return(1);
		}
		std::cout << "If this were a real implementation of tar it would create "
		  << archiveFile << " from ";
		for(size_t i=1; i<positionals.size(); i++){
			if(i>1){
				if(i+1==positionals.size())
					std::cout << " and ";
				else
					std::cout << ", ";
			}
			std::cout << positionals[i];
		}
		std::cout << std::endl;
	}
	if(extract)
		std::cout << "If this were a real implementation of tar it would extract "
		  << archiveFile << std::endl;
	return(0);
}
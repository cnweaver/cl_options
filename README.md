General
-------

This is a simple, header-only library for parsing command line options. Many others exist; this one happens to suit the author's personal taste. It has no dependencies except the C++ standard library. C++11 or newer is required. 

As the library itself is a single header file there is nothing which must be compiled. However, included are an example program demonstrating usage and a test program which runs through a very basic set of unit tests. The provided makefile will compile these. The makefile can also install (and uninstall) the library. By default the install prefix is /usr/local but this can be changed using the PREFIX variable:

	make install PREFIX=/preferred/location

Code Documentation
------------------

A simple utility for parsing command line options. 

Assuming an object of this class:

	OptionParser op;

Options may be flags (like `ls -l`) which are handled via a callback function:

	bool longFormat=false;
	op.addOption('l', [&]{longFormat=true;}, "List in long format.");

or may take values, which can be directly stored in a variable 
(like `tar -f archive.tar`):

	std::string archiveFile;
	op.addOption('f', archiveFile, 
	     "Read the archive from or write the archive to the specified file.",
	     "archive");

Here, the option is also given a description ("Read the archive. . . ") and a 
name for the value it takes ("archive"). These are used to provide help
messages for users (see below). A description is required, and though optional,
a value name is strongly recommended.

Options taking values may also do so via a callback, although in this case
it is unfortunately necessary to explicit specify the datatype expected by
the callback:

	std::string archiveFile;
	op.addOption<std::string>('f',
	     [&archiveFile](std::string file){ archiveFile=file; },
	     "Read the archive from or write the archive to the specified file.",
	     "archive");

Note that options which take value may do so either by consuming the 
following argument, or may have the value speficier 'inline' with an equals 
sign so that both of the following invocations of a program with the above 
option are equivalent:

	tar -f archive.tar
	tar -f=archive.tar

Options may be either short ('-l') or long ('--long'). A single option may
have multiple synonymous forms:

	bool longFormat=false;
	op.addOption({"l","long","long-listing"}, 
	     [&]{longFormat=true;}, "List in long format.");

Positional arguments are also supported and are separated out from options
and their values. A more complete 'tar' example:

	OptionParser op;
	bool create=false, extract=false;
	op.addOption('c', [&]{create=true}, 
	     "Create a new archive containing the specified items.");
	op.addOption('x', [&]{extract=true}, 
	     "Extract to disk from the archive.");
	std::string archiveFile;
	op.addOption('f', archiveFile, 
	     "Read the archive from or write the archive to the specified file.",
	     "archive");
	auto positionals = op.parseArgs(argc, argv);

With this code in place one could invoke the hypothetical 'tar' program as

	tar -c -f my_files.tar file1 file2

and `create` would be set to `true`, while `positionals` would be an
`std::vector<std::string>` containing `{"tar","file1","file2"}`. 

A common shortcut to reduce typing is to allow multiple short options to be
run together in a single argument. This is not enabled by default, but can
be turned on:

	op.allowsShortOptionCombination(true);

Inserting this into the example makes it allow

	tar -cf my_files.tar file1 file2

Any number of flags may be combined in this way, but at most one option taking 
a value may appear, and it must be the last in the combined argument. Otherwise,
there would be no way for the parser to know in general where the value for an
option ended and where additional option packed into the same argument might
begin. 

Since we have taken `tar` as an example, it should also be noted that even
when short option combining is allowed, the BSD-style 'bundled flags' first
positional argument is not supported. (It is not exactly forbidden, as this
implementation will simply treat it as a positional argument and pass it as
such back to the calling code, but no automatic parsing is directly provided.)

This class also supports generating simple usage information, and by default
the options '-h', '-?', '--help', and '--usage' will write it to standard 
output. (These default options may be suppressed when the OptionParser is 
constructed.) Each option is summarized using the description string provided 
when it is added, and custom introductory text can be added before this. 
Adding to the above example:

	op.setBaseUsage("This is toy example of the tar interface");

Running this program with any of the built-in help options produces:

	This is toy example of the tar interface
	 -c: Create a new archive containing the specified items.
	 -x: Extract to disk from the archive.
	 -f archive: Read the archive from or write the archive to the specified file.

The generation of this help message is currently very simple, and makes no 
effort to reflow text, so users are advised to format description strings 
themselves. 

For short options, it may be desirable to support accepting a value as part of the 
same argument, without an intervening equals sign, as in

	# run make with 8 jobs
	make -j8
	# compile foo.c, linking against libbar and libbaz
	$CC foo.c -lbar -lbaz -o foo

This behavior is supported, but turned off by default. To activate it use

	op.allowsShortValueWithoutEquals(true);

This covers short options only, as supporting long options would require both more
complex parsing and could lead to ambiguities. (If 'foo' were an option taking a 
value and 'foobar' were also an option, how should `--foobar` be interpreted?)

Another feature which is sometimes important is providing a way to terminate option
parsing and treat all remaining arguments as positional. This is relevant for 
allowing programs to accept filenames beginning with dashes as arguments, as well as
debuggers and other wrapper programs, which may have many options of their own, but 
also wish to take a series of arguments defining another program to be run including 
its own options and arguments. This is optionally supported by the special option 
'--', which can be used to indicate that all following arguments should be passed 
through without further interpretation. This feature is enabled by

	op.allowsOptionTerminator(true);

Because of the simplistic way that the help text is accumulated currently, disabling
this feature after enabling it does not remove the automatically added entry in the 
help text, and enabling it multiple times will result in multiple copies of that text. 

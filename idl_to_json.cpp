// --------------------------------------------------------------
//
//                        idl_to_json.cpp
//
//        Author: Noah Mendelsohn
//
//        Syntax:
//
//               idl_to_json idlfile1
//  
//        This program reads the file named on the command line
//        and parses the file into a Declarations parse tree.
//        The results are then formatted on standard output as JSON.
//
//        Results:
//
//        If successful, JSON is written to stdout and exit code is EXIT_SUCCESS(0).
//
//        If there are errors, messages are written to stderr and
//        exit code is EXIT_FAILURE. In most error cases no output is written
//        to stdout, but that is not guaranteed.
//
//        NEEDSWORK:
//
//        This test program and the parsing framework were written in some haste.
//        Some of the functions are long, etc. 
//
//        Function_names are sometimesInconsistent in capitalization.
//
//        It's known that the parsing framework does not free dynamically
//        allocated structures -- valgrind will report leaks.
//
//       Copyright: 2012 & 2016 Noah Mendelsohn
//     
// --------------------------------------------------------------



#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "c150exceptions.h"
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"

using namespace std;

// forward function declarations

void processIDLFile(const char fileName[]);
void indent_it(int ind);
void print_arglist_or_members(vector<Arg_or_Member_Declaration *>& members, 
			      int indent);
void print_type(TypeDeclaration *typep, int indent);
void print_function(FunctionDeclaration *functionp, int indent, 
		    bool writeCommaFunction);
void print_type_of_type(const char * type_of_type);

const int INDENT = 4;            // Amount to indent each block of JSON


// --------------------------------------------------------------
//
//                       main
//
// --------------------------------------------------------------

int 
main(int argc, char *argv[])
{

    //
    // Check argument count
    //
    if (argc != 2) {
      fprintf(stderr, "Syntax: %s  idlfile1\n", argv[0]);
      exit (8);
    };

    //
    //  Process the named file
    //

    try {
	    processIDLFile(argv[1]);
    } 

    catch (C150Exception e) {
	    //
	    // If any of the exceptions that are created using the COMP 150
	    // framework are thrown, they come with a detailed explanation.
	    // Print it, then give up.
	    //
	    fprintf(stderr,"%s: caught C150Exception: %s\n", argv[0],  
		    e.formattedExplanation().c_str());
	    fprintf(stderr, "...Giving up on file %s...\n",argv[1]);
	    exit(EXIT_FAILURE);
    }

    return 0;
}

// -----------------------------------------------------
//
//                  processIDLFile
//
//  Use the IDLTokenizer class to tokenize one file.
//  Then format the JSON results on stdout.
//
// -----------------------------------------------------

void
processIDLFile(const char fileName[]) {

  int indent = 0;                  // Current JSON indent amount
  bool writeCommaType = false;     // control comma in a,b,c type lists
  bool writeCommaFunction = false; // control comma in a,b,c function lists


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Parse the IDL file
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  //
  // Open the file
  //
  ifstream idlFile(fileName);        // open 

  if (!idlFile.is_open()) {
    stringstream ss;
    ss << "Could not open IDL file: " << fileName;
    throw C150Exception(ss.str());
  }
  // The following line does all the work parsing the file into
  // the variable parseTree

  Declarations parseTree(idlFile);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print outer { for whole JSON file
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  printf("{\n");
  indent += INDENT;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print out all Types
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  //
  //  print "types" : {
  //
  indent_it(indent);
  indent += INDENT;
  printf("\"types\" : {");               // start types json key

  // This iterator will return pairs of <string, TypeDeclaration*>
  // If this doesn't make sense, look on the Web for information
  // about iterating over C++ maps. 

  // Note to those learning about the Declarations parse tree.  
  //
  // For your convenience, the C++ [] operator is overloaded so
  // you could also reference a particular type with
  //
  //      parseTree.types["yourtypename"]
  //
  //  First, you should see whether the type exists by calling: 
  //
  //      parseTree.typeExists("yourtypename")
  //
  //  Below, instead, we just iterate through all types

  std::map<std::string, TypeDeclaration*>::iterator iter;  
  TypeDeclaration *typep;

  //
  //  Loop printing JSON for each type
  //
  for (iter = parseTree.types.begin(); iter != parseTree.types.end(); ++iter) {

    //
    // Iter actually is a pair, with members first (the key) and
    // second (the value) from the map. Our keys are the type
    // names, and the values are the actual type declaration structures.
    //
    // iter -> second gets you the type decl, 
    //
    // [ By the way, We can actually get the type name from two places:
    // 1) From the map at iter -> first, or 2)from the TypeDeclaration
    // itself by doing typep->getName() ]
    // 

    typep = iter -> second;             // ptr to type declaration

    printf("%s\n", 
	   writeCommaType ? "," : "");  // prefix with comma if needed + \n
    writeCommaType = true;              // ...next time we WILL need a comma
    print_type(typep, indent);          // write all the JSON for this type

  }                                     // end loop through all types 


  //
  //  Write }, to close dict for all types
  //

  putchar('\n');
  indent -= INDENT;
  indent_it(indent);
  printf("},\n");


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print out all Functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  //
  //  print "functions" : {
  //
  indent_it(indent);
  indent += INDENT;
  printf("\"functions\" : {");               // start types json key

  // This iterator will return pairs of <string, FunctionDeclaration*>
  std::map<std::string, FunctionDeclaration*>::iterator fiter;  

  // Note to those learning about the Declarations parse tree.  
  //
  // For your convenience, the C++ [] operator is overloaded so
  // you could also reference a particular function with
  //
  //      parseTree.functions["yourfunctionname"]
  //
  //  First, you should see whether the function exists by calling: 
  //
  //      parseTree.functionExists("yourfunctionname")
  //
  //  Below, instead, we just iterate through all functions


  //
  //  Loop once for each function declaration
  //

  FunctionDeclaration *functionp;    // Declare FunctionDelcaration pointer
  for (fiter = parseTree.functions.begin(); fiter != parseTree.functions.end(); 
       ++fiter) {

    //
    // fiter actually is a pair, with members first (the key) and
    // second (the value) from the map. Our keys are the function
    // names, and the values are the actual function declaration structures.
    // 
    functionp = fiter -> second;
    print_function(functionp, indent, writeCommaFunction);
    writeCommaFunction = true;
  }

  //
  //  Write } to close dict for all functions
  //

  putchar('\n');
  indent -= INDENT;
  indent_it(indent);
  printf("}\n");


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print outer } for whole JSON file
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  indent -= INDENT;
  indent_it(indent);
  printf("}\n");
    
} 


// ----------------------------------------------------------
//
//                   print_type
//
//     Prints the JSON for a type declaration 
//
// ----------------------------------------------------------

void print_type(TypeDeclaration *typep, int indent)
{

    //
    //  "typename" : {
    //

    indent_it(indent);                  
    printf("\"%s\" : ", 
	   typep -> getName().c_str()); // write name of type

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    //    Put out JSON for struct, array or builtin as appropriate
    //
    //    Note that even though the { was written above, the }
    //    is handled separately for each case. Reason: builtins
    //    are all on one line but structs and arrays have
    //    } on its own line.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

    //
    // It's a struct
    //
    if (typep -> isStruct()) {
	    //
	    // Write start of dict for type (note might close on same line for builtin)
	    //
	    
	    indent += INDENT;
	    putchar('{');

	    print_type_of_type("struct");  // write the output    
	    //
	    //set up for member list
	    printf(",\n");
	    //
	    indent_it(indent);
	    printf("\"members\" : ");
	    //
	    // print members of struct
	    //
	    print_arglist_or_members(typep -> getStructMembers(), indent);
	    //
	    // close struct type
	    //
	    putchar('\n');
	    indent -= INDENT;
	    indent_it(indent);
	    putchar('}');
    }
    //
    // It's an array
    //
    else if (typep -> isArray()) { 
	    //
	    // Write start of dict for type (note might close on same line for builtin)
	    //
	    
	    indent += INDENT;
	    putchar('{');

	    print_type_of_type("array");  // write the output    
	    //
	    //  member_type
	    //
	    printf(",\n");
	    indent_it(indent);
	    printf("\"member_type\" : \"%s\"", 
		   (typep -> getArrayMemberType())->getName().c_str());
	    //
	    //  element_count
	    //
	    printf(",\n");
	    indent_it(indent);
	    printf("\"element_count\" : %u", typep -> getArrayBound());
	    //
	    // close array type
	    //
	    putchar('\n');
	    indent -= INDENT;
	    indent_it(indent);
	    putchar('}');

    //
    // Neither struct nor array: must be a built-in type
    //
    } else {
	    //
	    // Builtin type
	    //
	    putchar('{');
	    print_type_of_type("builtin");  // write the output    
	    putchar('}');
    }

}

// ----------------------------------------------------------
//
//                   print_function
//
//     Prints the json for a function (quite similar to a struct)
//
// ----------------------------------------------------------

void print_function(FunctionDeclaration *functionp, int indent, 
		    bool writeCommaFunction)
{

    //
    //  "functionname" : {
    //
    printf("%s\n", 
	   writeCommaFunction ? "," : "");  // write the output
    indent_it(indent);
    printf("\"%s\" : {\n", 
	   functionp -> getName().c_str());  // write the output
    indent += INDENT;

    //
    //  "return_type" : "type_name",
    //
    indent_it(indent);
    printf("\"return_type\" : \"%s\",\n", 
	   functionp->getReturnType()->getName().c_str());

    //
    //  "arguments" " : {
    //
    indent_it(indent);
    printf("\"arguments\" : ");

    //

    //
    // print JSON for function arguments
    //

    // Get a C++ reference to the vector with the argument list
    ArgumentVector& args = functionp -> getArgumentVector();
    print_arglist_or_members(args, indent);

    putchar('\n');
    indent -= INDENT;
    indent_it(indent);
    putchar('}');
}

// ----------------------------------------------------------
//
//                   print_arglist_or_members
//
//     The same internal structures are used to track argument lists
//     and structure members. Either way, print the list
//     bracketed by [ ] with each item being a two entry dict.
//
//     The opening { is printed is the next character...the }
//     is indented on its own line.
//
// ----------------------------------------------------------

void print_arglist_or_members(vector<Arg_or_Member_Declaration *>& members, int indent)
{
  bool writeCommaStruct = false;
  unsigned memberNum;

  //
  //  Opening { and indent
  //
  printf("[");               // start types json key
  indent_it(indent);
  indent += INDENT;
	    
  //
  // The members are in a vector, not a map, so we
  // access them by position, not name
  //
  for(memberNum=0; memberNum < members.size();memberNum++) {
	  Arg_or_Member_Declaration* memp = members[memberNum];
	  printf("%s\n",
		 writeCommaStruct ? "," : "") ;
	  indent_it(indent);
	  printf("{\"name\" : \"%s\", \"type\" : \"%s\"}",
		 memp->getName().c_str(), 
		 memp->getType()->getName().c_str()) ;
	  writeCommaStruct = true;
  }

  //
  // Write closing '}'
  //
  putchar('\n');
  indent -= INDENT;
  indent_it(indent);
  printf("]");
}


// ----------------------------------------------------------
//
//                   print_type_of_type
//
//     Write "type_of_type" : "ARG"
//
// ----------------------------------------------------------

void print_type_of_type(const char * type_of_type)
{
	printf("\"type_of_type\" : \"%s\"", type_of_type);
}


// ----------------------------------------------------------
//
//                   indent
//
//     Indent stdout by amount given
//
// ----------------------------------------------------------

void indent_it(int ind)
{
  while (ind--) {
	  putc(' ', stdout);
  }
}

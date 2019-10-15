#pragma once
#include<string>
#include<map>

using namespace std;

string keyword[] =
{ "auto", "break", "case", "char", "const", "continue", "default" "do",
"double", "else", "enum", "extern", "float", "for", "goto", "if",
"inline", "int", "long", "register", "restrict", "return", "short", "signed",
"sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void",
"volatile", "while", "_Bool", "_Complex", "_Imaginary" };

class Location
{
public:
	Location() :line(1), column(0) {}
	int line;
	int column;
};

extern void mapInit();
map<string, string> punctuatorType;
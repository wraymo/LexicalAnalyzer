#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<cstdlib>
#include<algorithm>

using namespace std;
map<string, string> punctuatorType;

string keyword[] =
	{"auto", "break", "case", "char", "const", "continue", "default" "do",
	"double", "else", "enum", "extern", "float", "for", "goto", "if",
	"inline", "int", "long", "register", "restrict", "return", "short", "signed",
	"sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void",
	"volatile", "while", "_Bool", "_Complex", "_Imaginary"};

class Location
{
public:
	Location() :line(1), column(0){}
	int line;
	int column;
};

class Token 
{
private:

public:
	string type;
	string subtype;
	string value;
	Location location;
	Token(string _type, string _subtype, string _value, Location _location) :
		type(_type), subtype(_subtype), value(_value), location(_location) {}
	Token(string _type, string _value, Location _location):
		type(_type), subtype(""), value(_value), location(_location){}
	Token(string _type, Location _location):
		type(_type), subtype(""), value(""), location(_location){}
};

ostream& operator<< (ostream& of, const Token& token)
{
	of << "Token type: ";
	if (token.subtype != "")
		of << token.type << " (" << token.subtype << ")" << endl;
	else
		of << token.type << endl;
	of << "Value: " << token.value << endl;
	return of;
}

class FileHandler
{
private:
	ifstream file;
	Location location;
	bool eof;
	bool flag;
	bool isPreprocessor;
	int charCount;
	string buffer;
	string fileName;
public:
	FileHandler(string _fileName):flag(true), fileName(_fileName)
	{
		file.open(_fileName, ios::in);
		eof = file.eof();
		status = file.fail() || eof? false : true;
	}

	Location getLocation()
	{
		return location;
	}

	void printFileInfo(ofstream & of)
	{
		of << "File Name: " << fileName << endl;
		of << "Total Characters: " << charCount << endl;
		of << "Total Lines: " << location.line << endl;
	}

	char getChar()
	{
		if (isEOF())
			return 0;
		else if (buffer.empty())
			readChar();
		char c = buffer.front();
		buffer.erase(buffer.begin());
		charCount++;
		if (c != '\t')
			location.column++;
		else
			location.column += 4;
		isPreprocessor = (c == '#' && flag) ? true : false;
		if (c == '\n')
		{
			location.line++;
			location.column = 0;
			flag = true;
		}
		else if (c != '\t')
			flag = false;
		return c;
	}

	string getNChar(int n)
	{
		string temp = "";
		char c;
		for (int i = 0; i < n; i++)
		{
			if ((c = getChar()) == 0)
				break;
			temp += c;
		}
		return temp;
	}

	bool isPre()
	{
		return isPreprocessor;
	}

	void readChar()
	{
		if (!eof)
		{
			buffer += file.get();
			eof = file.eof();
		}
	}

	char peek(int offset)
	{
		while (!isEOF() && buffer.size() < offset)
			readChar();
		if (buffer.size() < offset)
		{
			cout << "There is something wrong with the operation" << endl;
			return 0;
		}			
		else
			return buffer[offset - 1];
	}

	bool isEOF()
	{
		return eof && buffer.empty();
	}

	bool status;
};

class Analyzer
{
private:
	FileHandler fileHandler;
	ofstream result;
	string buffer;
	string fileName;
	Location currentLoc;
	int keywordCnt;
	int identifierCnt;
	int constantCnt;
	int stringLiteralCnt;
	int punctuatorCnt;
	int preprocessorCnt;
	int unknownCnt;
	int errorCnt;
public:
	Analyzer(const string _fileName, const int i) 
		:fileName(_fileName), fileHandler(_fileName), no(i), buffer(""),
		keywordCnt(0), identifierCnt(0), constantCnt(0), stringLiteralCnt(0),
		punctuatorCnt(0), preprocessorCnt(0), unknownCnt(0), errorCnt(0)
	{
		if (!fileHandler.status)
			status = false;
		else
		{
			status = true;
			int pos = fileName.rfind('/');
			stringstream ss;
			ss << i;
			string s = "result" + ss.str() + ".txt";
			string resultName = (pos == string::npos) ? s : fileName.substr(0, pos + 1) + s;
			result.open(resultName, ios::out);
		}
	}

	void skipLineComments()
	{
		while (!fileHandler.isEOF() && fileHandler.getChar() != '\n');
	}

	void skipBlockComments()
	{
		while (!fileHandler.isEOF()) 
		{
			if (fileHandler.getChar() == '*' && fileHandler.peek(1) == '/') 
			{
				fileHandler.getChar();
				return;
			}
		}
	}

	friend ostream & operator << (ostream& of, const Token& token);

	Token getToken()
	{
		buffer = "";
		char next;
		if (fileHandler.isEOF())
			return Token("EOF", fileHandler.getLocation());

		char c = fileHandler.getChar();
		while (c == ' ' || c == '\t' || c == '\n')
		{
			if (fileHandler.isEOF())
				return Token("EOF", fileHandler.getLocation());
			c = fileHandler.getChar();
		}

		currentLoc = fileHandler.getLocation();

		switch (c)
		{
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
		case 'I': case 'J': case 'K': /*case 'L': */case 'M': case 'N': case 'O': case 'P':
		case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
		case 'Y': case 'Z': case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v':
		case 'w': case 'x': case 'y': case 'z': case '_':
			buffer += c;
			return getIdentifier();

		case 'L':
			next = fileHandler.peek(1);
			if (next == '\'' || next == '\"')
				return getToken();
			else
				buffer += c;
				return getIdentifier();

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			buffer += c;
			return getNumericConstant();

		case '[': case ']': case '{': case '}': case '(':
		case ')': case ',': case ';': case '?': case '~':
			buffer += c;
			punctuatorCnt++;
			return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);

		case '+': case '|': case '&': case '=':
		case '-': case '*': case '!': case '^':
			buffer += c;
			punctuatorCnt++;
			next = fileHandler.peek(1);
			if (next == '=' || (next == c && c != '*' && c != '!' && c != '^') || (c == '-' && next == '>'))
			{
				buffer += fileHandler.getChar();
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
			else
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);

		case '/':
			if (fileHandler.peek(1) == '/')
			{
				fileHandler.getChar();
				skipLineComments();
				return getToken();
			}
			else if (fileHandler.peek(1) == '*')
			{
				fileHandler.getChar();
				skipBlockComments();
				return getToken();
			}
			else if (fileHandler.peek(1) == '=')
			{
				buffer += c;
				buffer += fileHandler.getChar();
				punctuatorCnt++;
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
			else
			{
				buffer += c;
				punctuatorCnt++;
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}

		case '%':
			buffer += c;
			punctuatorCnt++;
			if (fileHandler.peek(1) == '=' || fileHandler.peek(1) == '>')
			{
				buffer += fileHandler.getChar();
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
			else if (fileHandler.peek(1) == ':')
			{
				if (fileHandler.peek(2) == '%' && fileHandler.peek(3) == ':')
				{
					buffer += fileHandler.getNChar(3);
					return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
				}
				else
				{
					buffer += fileHandler.getChar();
					return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
				}
			}
			else
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);

		case '<': case '>':
			buffer += c;
			punctuatorCnt++;
			if (fileHandler.peek(1) == c)
			{
				buffer += fileHandler.getChar();
				if (fileHandler.peek(2) == '=')
				{
					buffer += fileHandler.getChar();
					return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
				}
				else
					return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
			else if (fileHandler.peek(1) == '=' || (fileHandler.peek(1) == ':' ||
				fileHandler.peek(1) == '%' && c == '<'))
			{
				buffer += fileHandler.getChar();
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
			else
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);

		case '.':
			buffer += c;
			if (fileHandler.peek(1) == '.' && fileHandler.peek(2) == '.')
			{
				buffer += fileHandler.getNChar(2);
				punctuatorCnt++;
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
			else if (isdigit(fileHandler.peek(1)))
				return getNumericConstant();
			else
			{
				punctuatorCnt++;
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
				

		case '#':
			buffer += c;
			if (fileHandler.peek(1) == '#')
			{
				buffer += fileHandler.getChar();
				punctuatorCnt++;
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
			else if (fileHandler.isPre())
			{
				while (!fileHandler.isEOF() && (fileHandler.peek(1) != '\n' || c == '\\')) 
					buffer += fileHandler.getChar();
				preprocessorCnt++;
				return Token("Preprocessor", buffer, currentLoc);
			}
			else
			{
				punctuatorCnt++;
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}

		case ':':
			buffer += c;
			punctuatorCnt++;
			if (fileHandler.peek(1) == '>')
			{
				buffer += fileHandler.getChar();
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);
			}
			else
				return Token("Punctuator", punctuatorType[buffer], buffer, currentLoc);

		case '\'':
			buffer += c;
			return getCharacterConstant();

		case '\"':
			buffer += c;
			return getStringLiteral();

		default:
			buffer += c;
			unknownCnt++;
			return Token("Unknown", buffer, currentLoc);
		}
	}

	Token getStringLiteral()
	{
		while (!fileHandler.isEOF())
		{
			if (fileHandler.peek(1) != '\\' && fileHandler.peek(1) != '\"')
				buffer += fileHandler.getChar();
			else if (fileHandler.peek(1) == '\\')
			{
				if (isSimpleEscape(fileHandler.peek(2)))
					buffer += fileHandler.getNChar(2);
				else if (fileHandler.peek(2) >= '0' && fileHandler.peek(2) <= '7'
					&& fileHandler.peek(3) >= '0' && fileHandler.peek(3) <= '7'
					&& fileHandler.peek(4) >= '0' && fileHandler.peek(4) <= '7')
					buffer += fileHandler.getNChar(4);
				else if (fileHandler.peek(2) >= '0' && fileHandler.peek(2) <= '7'
					&& fileHandler.peek(3) >= '0' && fileHandler.peek(3) <= '7')
					buffer += fileHandler.getNChar(3);
				else if (fileHandler.peek(2) >= '0' && fileHandler.peek(2) <= '7')
					buffer += fileHandler.getNChar(2);
				else if (fileHandler.peek(2) == 'x')
				{
					if (!isHexaDigit(fileHandler.peek(3)))
					{
						buffer += fileHandler.getNChar(3);
						while (!fileHandler.isEOF() && fileHandler.peek(1) != '\"')
							buffer += fileHandler.getChar();
						if (fileHandler.peek(1) != '\"')
							buffer += fileHandler.getChar();
						errorCnt++;
						return Token("StringLiteral", "Error", buffer, currentLoc);
					}
					else
					{
						buffer += fileHandler.getNChar(3);
						while (!fileHandler.isEOF() && isHexaDigit(fileHandler.peek(1)))
							buffer += fileHandler.getChar();
						continue;
					}
				}
				else
				{
					buffer += fileHandler.getNChar(2);
					while (!fileHandler.isEOF() && fileHandler.peek(1) != '\"')
						buffer += fileHandler.getChar();
					errorCnt++;
					if (fileHandler.peek(1) == '\"')
						buffer += fileHandler.getChar();
					return Token("StringLiteral", "Error", buffer, currentLoc);
				}
			}
			else
			{
				stringLiteralCnt++;
				buffer += fileHandler.getChar();
				return Token("StringLiteral", buffer, currentLoc);
			}
		}
		errorCnt++;
		return Token("StringLiteral", "Error", buffer, currentLoc);
	}

	bool isSimpleEscape(char c)
	{
		return c == '\'' || c == '?' || c == '\"' || c == '\\' || c == 'a'
			|| c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't' || c == 'v';
	}

	bool isHexaDigit(char c)
	{
		return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
	}

	Token getCharacterConstant()
	{
		while (!fileHandler.isEOF())
		{
			if (fileHandler.peek(1) != '\\' && fileHandler.peek(1) != '\'')
				buffer += fileHandler.getChar();
			else if (fileHandler.peek(1) == '\\')
			{
				if (isSimpleEscape(fileHandler.peek(2)))
					buffer += fileHandler.getNChar(2);
				else if (fileHandler.peek(2) >= '0' && fileHandler.peek(2) <= '7'
					&& fileHandler.peek(3) >= '0' && fileHandler.peek(3) <= '7'
					&& fileHandler.peek(4) >= '0' && fileHandler.peek(4) <= '7')
					buffer += fileHandler.getNChar(4);
				else if (fileHandler.peek(2) >= '0' && fileHandler.peek(2) <= '7'
					&& fileHandler.peek(3) >= '0' && fileHandler.peek(3) <= '7')
					buffer += fileHandler.getNChar(3);
				else if (fileHandler.peek(2) >= '0' && fileHandler.peek(2) <= '7')
					buffer += fileHandler.getNChar(2);
				else if (fileHandler.peek(2) == 'x')
				{
					if (!isHexaDigit(fileHandler.peek(3)))
					{
						
						buffer += fileHandler.getNChar(3);
						while (!fileHandler.isEOF() && fileHandler.peek(1) != '\'')
							buffer += fileHandler.getChar();
						errorCnt++;
						if (fileHandler.peek(1) == '\'')
							buffer += fileHandler.getChar();
						return Token("Constant", "Error", buffer, currentLoc);
					}
					else
					{
						buffer += fileHandler.getNChar(3);
						while (!fileHandler.isEOF() && isHexaDigit(fileHandler.peek(1)))
							buffer += fileHandler.getChar();
						continue;
					}
				}
				else
				{
					buffer += fileHandler.getNChar(2);
					while (!fileHandler.isEOF() && fileHandler.peek(1) != '\'')
						buffer += fileHandler.getChar();
					if (fileHandler.peek(1) != '\'')
						buffer += fileHandler.getChar();
					errorCnt++;
					return Token("Constant",  "Error", buffer, currentLoc);
				}
			}
			else
			{
				constantCnt++;
				buffer += fileHandler.getChar();
				return Token("Constant", "CharacterConstant", buffer, currentLoc);
			}
		}
		errorCnt++;
		return Token("Constant", "Error", buffer, currentLoc);
	}

	Token getIdentifier()
	{
		char temp;
		string* p;
		while (!fileHandler.isEOF() && (isalnum(temp = (fileHandler.peek(1))) || temp == '_'))
			buffer += fileHandler.getChar();
		if ((p = find(keyword, keyword + 36, buffer)) != keyword + 36)
		{
			keywordCnt++;
			return Token("Keyword", buffer, currentLoc);
		}
		else
		{
			identifierCnt++;
			return Token("Identifier", buffer, currentLoc);
		}
	}

	bool isConstantChar(char c)
	{
		return isdigit(c) || c == '.' || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || 
			c == 'x' || c == 'X' || c == 'e' || c == 'E' || c == 'p' || c == 'P"' || 
			c == 'u' || c == 'U' || c == 'l' || c == 'L';
	}

	bool isSuffix(size_t pos)
	{
		bool zero = buffer.size() == pos;
		bool one = (buffer.size() == pos + 1) &&
			((buffer[pos] == 'u' || buffer[pos] == 'U' && buffer.find_first_of(".eEpP") == string::npos) 
			|| buffer[pos] == 'l' || buffer[pos] == 'L' 
			|| (buffer[pos] == 'f' || buffer[pos] == 'F' && buffer.find_first_of(".eEpP") != string::npos));
		bool two = (buffer.size() == pos + 2) &&
			((buffer[pos] == 'u' && buffer[pos+1] == 'l') || (buffer[pos] == 'U' && buffer[pos+1] == 'l')
			|| (buffer[pos] == 'u' && buffer[pos+1] == 'L') || (buffer[pos] == 'U' && buffer[pos+1] == 'L'));
		return zero || one || two;
	}

	Token getNumericConstant()
	{
		while (!fileHandler.isEOF() && isConstantChar(fileHandler.peek(1)))
			buffer += fileHandler.getChar();
		size_t pos;
		stof(buffer,&pos);
		if (isSuffix(pos))
		{
			constantCnt++;
			if (buffer.find_first_of(".eEpP") != string::npos)
				return Token("Constant", "FloatingConstant", buffer, currentLoc);
			else
				return Token("Constant", "IntegerConstant", buffer, currentLoc);
		}
		else
			return Token("Constant", "Error", buffer, currentLoc);
	}

	bool printToken(Token token)
	{
		result << token;
		result << "Location: " << fileName << ":" << token.location.line 
			<< ":" << token.location.column << endl << endl;
		return fileHandler.isEOF()? true: false;
	}

	void printInfo()
	{
		result << "Result:" << endl;
		fileHandler.printFileInfo(result);
		result << "Keyword: " << keywordCnt << endl;
		result << "Identifier: " << identifierCnt << endl;
		result << "Punctuator: " << punctuatorCnt << endl;
		result << "Constant: " << constantCnt << endl;
		result << "String Literal: " << stringLiteralCnt << endl;
		result << "Preprocessor: " << preprocessorCnt << endl;
		result << "Unknown: " << unknownCnt << endl;
		result << "Error: " << errorCnt << endl;
	}

	bool status;
	int no;
};

void mapInit()
{
	punctuatorType.insert(pair<string, string>("[", "OpenBracket"));
	punctuatorType.insert(pair<string, string>("]", "CloseBracket"));
	punctuatorType.insert(pair<string, string>("(", "OpenParen"));
	punctuatorType.insert(pair<string, string>(")", "CloseParen"));
	punctuatorType.insert(pair<string, string>("{", "OpenBrace"));
	punctuatorType.insert(pair<string, string>("}", "CloseBrace"));
	punctuatorType.insert(pair<string, string>(".", "Dot"));
	punctuatorType.insert(pair<string, string>("->", "AccessThroughPointer"));
	punctuatorType.insert(pair<string, string>("+", "Plus"));
	punctuatorType.insert(pair<string, string>("-", "Minus"));
	punctuatorType.insert(pair<string, string>("&", "BitwiseAnd"));
	punctuatorType.insert(pair<string, string>("|", "BitwideOr"));
	punctuatorType.insert(pair<string, string>("~", "BitwideNot"));
	punctuatorType.insert(pair<string, string>("++", "Increment"));
	punctuatorType.insert(pair<string, string>("--", "Decrement"));
	punctuatorType.insert(pair<string, string>("*", "Star"));
	punctuatorType.insert(pair<string, string>("!", "LogicalNot"));
	punctuatorType.insert(pair<string, string>("/", "Slash"));
	punctuatorType.insert(pair<string, string>("%", "Percent"));
	punctuatorType.insert(pair<string, string>("<<", "LeftShift"));
	punctuatorType.insert(pair<string, string>(">>", "RightShift"));
	punctuatorType.insert(pair<string, string>("<", "Less"));
	punctuatorType.insert(pair<string, string>(">", "More"));
	punctuatorType.insert(pair<string, string>("<=", "LessOrEqual"));
	punctuatorType.insert(pair<string, string>(">=", "MoreOrEqual"));
	punctuatorType.insert(pair<string, string>("==", "DoubleEqual"));
	punctuatorType.insert(pair<string, string>("!=", "NotEqual"));
	punctuatorType.insert(pair<string, string>("^", "Caret"));
	punctuatorType.insert(pair<string, string>("&&", "LogicalAnd"));
	punctuatorType.insert(pair<string, string>("||", "LogicalOr"));
	punctuatorType.insert(pair<string, string>("?", "Question"));
	punctuatorType.insert(pair<string, string>(":", "Colon"));
	punctuatorType.insert(pair<string, string>(";", "Semicolon"));
	punctuatorType.insert(pair<string, string>("...", "Ellipsis"));
	punctuatorType.insert(pair<string, string>("=", "Equal"));
	punctuatorType.insert(pair<string, string>("*=", "MultiplyEqual"));
	punctuatorType.insert(pair<string, string>("/=", "DivideEqual"));
	punctuatorType.insert(pair<string, string>("%=", "ModuleEqual"));
	punctuatorType.insert(pair<string, string>("+=", "RightShift"));
	punctuatorType.insert(pair<string, string>("-=", "MinusEqual"));
	punctuatorType.insert(pair<string, string>("<<=", "LeftShiftEqual"));
	punctuatorType.insert(pair<string, string>(">>=", "RightShiftEqual"));
	punctuatorType.insert(pair<string, string>("&=", "BitwiseAndEqual"));
	punctuatorType.insert(pair<string, string>("^=", "BitwiseXorEqual"));
	punctuatorType.insert(pair<string, string>("|=", "BitwiseOrEqual"));
	punctuatorType.insert(pair<string, string>("#", "Pound"));
	punctuatorType.insert(pair<string, string>("##", "DoublePound"));
	punctuatorType.insert(pair<string, string>("<:", "LessColon"));
	punctuatorType.insert(pair<string, string>(":>", "ColonMore"));
	punctuatorType.insert(pair<string, string>("<%", "LessPercent"));
	punctuatorType.insert(pair<string, string>("%>", "PercentMore"));
	punctuatorType.insert(pair<string, string>("%:", "PercentColon"));
	punctuatorType.insert(pair<string, string>("%:%:", "DoublePercentColon"));
}

int main(int argc, char** argv)
{
	if (argc <= 1)
		throw "Please input at least one file you want to analyze";
	
	vector<Analyzer*> analyzeList;
	mapInit();

	for (int i = 0; i < argc - 1; i++)
	{
		Analyzer* temp = new Analyzer(argv[i + 1], i + 1);
		if (!temp->status)
		{
			cout << "File " << i + 1 << " is not valid!" << endl;
			delete temp;
		}
		else
			analyzeList.push_back(temp);
	}
	
	for (int i = 0; i < analyzeList.size(); i++)
	{
		bool flag = true;
		do
		{
			Token token = analyzeList[i]->getToken();
			if (flag = analyzeList[i]->printToken(token))
				break;
		} 
		while(true);
		if (!flag)
			cout << "There is something wrong with file " << analyzeList[i]->no;
		analyzeList[i]->printInfo();
	}
}

/*
Sergio Munguia 


Mini Compiler -  Parser Project
Lexical and Syntax Analyzers


    prog        ->  begin stmt_list end

    stmt_list   ->  stmt ; stmt_list
                 |  stmt
				 |	stmt;

    stmt        ->  var = expr

    var         ->  A | B | C

    expr        ->  var + expr
                 |  var - expr
                 |  var

*/
#include    <iostream>
#include    <fstream>
#include    <cctype>

#include    "token.h"
#include    "functions.h"

#include	<cstring>
#include    <stack>

using namespace std;

ifstream ifs;                   // input file stream used by lexan
SymTab   symtab;                // global symbol table
Token    token;                 // global token
int      lookahead = 0;         // no look ahead token yet
int		 lookvalue = 0;       

int      dbg = 1;               // debut is ON

								// get weight of operators as per precedence
								// higher weight given to operators with higher precedence
								// for non operators, return 0 
int getWeight(char ch) {
	switch (ch) {
	case '+': return 3;
	case '-': return 3;
	case '=': return 2;
	case ';': return 1;
	default: return 0;
	}
}

// convert infix expression to postfix using a stack
void infix2postfix(char infix[], char postfix[], int size) {
	stack<char> s;
	int weight;
	int i = 0;
	int k = 0;
	char ch;
	// iterate over the infix expression   
	while (i < size) {
		ch = infix[i];
		if (ch == '(') {
			// simply push the opening parenthesis
			s.push(ch);
			i++;
			continue;
		}
		if (ch == ')') {
			// if we see a closing parenthesis,
			// pop of all the elements and append it to
			// the postfix expression till we encounter
			// a opening parenthesis
			while (!s.empty() && s.top() != '(') {
				postfix[k++] = s.top();
				s.pop();

			}
			// pop off the opening parenthesis also
			if (!s.empty()) {
				s.pop();
			}
			i++;
			continue;
		}
		weight = getWeight(ch);
		if (weight == 0) {
			// we saw an operand
			// simply append it to postfix expression
			postfix[k++] = ch;

		}
		else {
			// we saw an operator
			if (s.empty()) {
				// simply push the operator onto stack if
				// stack is empty
				s.push(ch);
			}
			else {
				// pop of all the operators from the stack and
				// append it to the postfix expression till we
				// see an operator with a lower precedence that
				// the current operator
				while (!s.empty() && s.top() != '(' &&
					weight <= getWeight(s.top())) {
					postfix[k++] = s.top();
					s.pop();

				}
				// push the current operator onto stack
				s.push(ch);
			}
		}
		i++;
	}
	// pop of the remaining operators present in the stack
	// and append it to postfix expression 
	while (!s.empty()) {
		postfix[k++] = s.top();
		s.pop();
	}
	postfix[k] = 0; // null terminate the postfix expression 
}

int main( )
{
	// Function to take the previous tokens emitted to emitted_tokens.txt & Convert them into Post Fix Expression
	int i = 0;
	char infix[100];
	ifstream file("emitted_tokens.txt");
	if (file.is_open())
	{
		while (!file.eof()) {
			file >> infix[i];
			++i;
			infix[i] = '\0';
		}
	}
	ofstream ofile;
	ofile.open("outfile.txt");
	int size = strlen(infix);
	char postfix[100];
	infix2postfix(infix, postfix, size);
	cout << "\nPrevious Emitted Infix Expression :: " << infix;
	cout << "\nConverted Postfix Expression :: " << postfix;
	ofile << postfix;
	cout << "\n*saved into outfile.txt*";
	cout << endl;
	ofile.close();
	file.close();

    ifs = get_ifs( );           // open an input file stream w/ the program
    init_kws( );                // initialize keywords in the symtab
	
    match( lookahead );         // get the first input token
    prog( );

    return 0;
}

// your methods...

void var(int CurTokenValue, int CurTokenType) {
	//cout << CurTokenType << " " << CurTokenValue << "\n"; 
	// if Var is A | B | C  and its .toktype is 257 then Accept its in the grammar
	if (token.toktype() == DONE)
	{
		exit(1);
	}
	if (CurTokenValue == 2 | 3 | 4   && CurTokenType == 257  ) {
		cout << "Var_Accepted\n";
	}
	// if Var is not in the grammar , reject and throw the syntax error on the current token
	else {
		cout << "Var_Not Accepted\n";
		error(0);
	}
}
void equal(int lookahead, int lookvalue) {
	// if Equal checks the token of the next token and its type is 61 and its value is -1 accept
	if (lookahead == 61 && lookvalue == -1) {
		cout << "Equal_Accepted\n";
	}
	// if Equal checks the token of the next token after Var and its type is not correct
	// throw syntax error on the token and reject
	else {
		cout << "Equal_Not Accepted\n";
		error(0);
	}
}

void expr(int CurTokenValue, int CurTokenType, int lookahead, int lookvalue) {
	// checks if the variable is followed by a - or a + and if so accepts
	// then calls var() to check the variable is also accepted
	if (token.toktype() == DONE)
	{
		exit(1);
	}
	if (CurTokenType == 43 | 45  && CurTokenValue - 1 ) {
		var(lookvalue, lookahead);
		cout << "Expr_Accepted\n";

	}
	else {
		cout << "Expr_Not Accepted\n";
		error(0);
	}
}


// utility methods

void emit(int t)
{
	switch (t)
	{
	case '+': case '-': case '=':
		cout << char(t) << ' ';
		break;

	case ';':
		cout << ";\n";
		break;

	case '\n':
		cout << "\n";
		break;

	case ID:
	case KW:
	case UID:
		cout << symtab.tokstr(token.tokvalue()) << ' ';
		break;

	default:
		cout << "'token " << t << ", tokvalue "
			<< token.tokvalue() << "' ";
		break;
	}
}

void error(int t, int expt, const string &str)
{
	cerr << "\nunexpected token '";
	if (lookahead == DONE)
	{
		cerr << "EOF";
	}
	else
	{
		cerr << token.tokstr();
	}
	cerr << "' of type " << lookahead;

	switch (expt)
	{
	case 0:         // default value; nothing to do
		break;

	case ID:
		cout << " while looking for an ID";
		break;

	case KW:
		cout << " while looking for KW '" << str << "'";
		break;

	default:
		cout << " while looking for '" << char(expt) << "'";
		break;
	}

	cerr << "\nsyntax error.\n";

	exit(1);
}

void match( int t )
{
    if( lookahead == t )
    {
        token = lexan( );               // get next token
        lookahead = token.toktype( );   // lookahed contains the tok type
		lookvalue = token.tokvalue( ); // lookahead tok value
    }
    else
    {
        error( t );
    }
}

void prog() {
	// uses a switch made out of if statements for the line number its currently reading. 
	// so it can repeat the calls of var() followed by expr() recursively as long as its on same line
	// when changing lines , it checks the variable is followed by an equal sign only 1 time.
	// calls the methods to begin 
	//cout << token.lineno() << "<- This is the original lineno \n";
	ofstream myfile;
	myfile.open("emitted_tokens.txt");
	bool Process = true;
	try {
		while (Process == true) {
			if (token.lineno() == 1) {
				token = lexan();
// for begining of the file call lexan on first token , which is begin, its on line 1
			}
			if (token.lineno() == 2) {
				cout << "this is token : " << token.tokstr() << "\n";
				int CurTokenValue = token.tokvalue();
				int CurTokenType = token.toktype(); 
				//cout << CurTokenType <<" "<< CurTokenValue << "\n";
				var(CurTokenValue, CurTokenType); 

				streambuf *oldbuf = cout.rdbuf(); // save
				cout.rdbuf(myfile.rdbuf());
				emit(CurTokenType); // saved emit output to file
				cout.rdbuf(oldbuf); // reset back to standard input

// calls match to lookahead and grab .tokvalue and toktype stores them in lookahead and lookvalue
// does this by calling lexan() and moving along the global token
				match(lookahead);

				//cout <<"2nd set "<< CurTokenType << " " << CurTokenValue << "\n";
				for (static bool first = true; first; first = false) { // run only once on equal sign
					equal(lookahead, lookvalue); 
				}
				//emit(token.toktype());
				if (token.toktype() != 258) {
					streambuf *oldbuf2 = cout.rdbuf(); // save
					cout.rdbuf(myfile.rdbuf());
					emit(token.toktype()); // saved emit output to file
					cout.rdbuf(oldbuf2); // reset back to standard input
				}
				match(lookahead);
				int CurTokenValue3 = token.tokvalue();
				int CurTokenType3 = token.toktype(); 
				//cout << "3rd set " << CurTokenType << " " << CurTokenValue << "\n";
				expr(CurTokenValue3, CurTokenType3, lookahead, lookvalue); 
				cout << "this is lookahead : " << lookahead << "\n";
			}
			if (token.lineno() == 3) {
				//cout << "this is token : " << token.tokstr() << "\n"; // token
				//match(lookahead);
				//cout << "this is lookahead : " << lookahead << "\n"; // next token
				cout << "this is token : " << token.tokstr() << "\n";
				int CurTokenValue = token.tokvalue();
				int CurTokenType = token.toktype();

				var(CurTokenValue, CurTokenType); 
				streambuf *oldbuf = cout.rdbuf(); // save
				cout.rdbuf(myfile.rdbuf());
				emit(CurTokenType); // saved emit output to file
				cout.rdbuf(oldbuf); // reset back to standard input

				match(lookahead);

				for (static bool first = true; first; first = false) { 
					equal(lookahead, lookvalue); 
				}
				if (token.toktype() != 258) {
					streambuf *oldbuf2 = cout.rdbuf(); // save
					cout.rdbuf(myfile.rdbuf());
					emit(token.toktype()); // saved emit output to file
					cout.rdbuf(oldbuf2); // reset back to standard input
				}

				match(lookahead);
				int CurTokenValue3 = token.tokvalue();
				int CurTokenType3 = token.toktype();

				expr(CurTokenValue3, CurTokenType3, lookahead, lookvalue);
				cout << "this is lookahead : " << lookahead << "\n";
				
			}
			if (token.lineno() == 4) {
				cout << "this is token : " << token.tokstr() << "\n";
				int CurTokenValue = token.tokvalue();
				int CurTokenType = token.toktype();

				var(CurTokenValue, CurTokenType);
				streambuf *oldbuf = cout.rdbuf(); // save
				cout.rdbuf(myfile.rdbuf());
				emit(CurTokenType); // saved emit output to file
				cout.rdbuf(oldbuf); // reset back to standard input

				match(lookahead);

				for (static bool first = true; first; first = false) { 
					equal(lookahead, lookvalue);
				}

				if (token.toktype() != 258) {
					streambuf *oldbuf2 = cout.rdbuf(); // save
					cout.rdbuf(myfile.rdbuf());
					emit(token.toktype()); // saved emit output to file
					cout.rdbuf(oldbuf2); // reset back to standard input
				}

				match(lookahead);
				int CurTokenValue3 = token.tokvalue();
				int CurTokenType3 = token.toktype();

				expr(CurTokenValue3, CurTokenType3, lookahead, lookvalue);
				cout << "this is lookahead : " << lookahead << "\n";
			}
			if (token.toktype() == DONE)
			{
				break;
			}
		}
	}
	catch (...) {
		cout << "Error Incorrect Grammar";
	}
	myfile.close();
}


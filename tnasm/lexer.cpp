#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

#include <cstdlib>

#include "../teenyat.h"
#include "token.h"

/*
 * Uncomment the definition below to trace the lexer
 */
#define DEBUG_TRACE

using namespace std;


struct Token_Regex {
	regex expr;
	Token_Type id;
};


void read_file(const char *path, vector <string> &asm_lines);
void initialize_lexical_regex(vector <Token_Regex> &patterns);
Token get_token(const vector <Token_Regex>& patterns, const string& s, int line_no);
void tokenize_line(
	string asm_line,
	Token_Line &token_line,
	vector <Token_Regex> &patterns,
	int line_no
);
void tokenize_all_lines(
	Token_Lines &token_lines,
	vector <string> &asm_lines,
	vector <Token_Regex> &patterns
);
void debug_print_lexed_input(Token_Lines &token_lines, vector <string> &asm_lines);

int main(int argc, char *argv[]) {
	if(argc != 2) {
		cerr << "Usage:   tnasm <file>" << endl;
		exit(EXIT_FAILURE);
	}

	vector <string> asm_lines;
	read_file(argv[1], asm_lines);

	vector <Token_Regex> patterns;
	initialize_lexical_regex(patterns);

	Token_Lines token_lines;
	tokenize_all_lines(token_lines, asm_lines, patterns);
	#ifdef DEBUG_TRACE
	cerr << "\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n";
	debug_print_lexed_input(token_lines, asm_lines);
	#endif /* DEBUG_TRACE */

	return EXIT_SUCCESS;
}


void read_file(const char *path, vector <string> &asm_lines) {
	ifstream f(path);
	string s;
	while(getline(f, s)) {
		asm_lines.push_back(s);
	}
	f.close();

	return;
}


Token_Regex regex_token(string s, Token_Type id) {
	Token_Regex tr;
	tr.expr = regex(s, regex_constants::icase);
	tr.id = id;

	return tr;
}

void initialize_lexical_regex(vector <Token_Regex> &patterns) {
    patterns.push_back(regex_token("[ \\t\\b\\v\\f\\r]+", T_IGNORE));
	patterns.push_back(regex_token(";.*", T_IGNORE));
	patterns.push_back(regex_token("![^ \\t\\b\\v\\r\\n;]+", T_LABEL));
	patterns.push_back(regex_token("\\.const(ant)?", T_CONSTANT));
	patterns.push_back(regex_token("\\.var(iable)?", T_VARIABLE));
	patterns.push_back(regex_token("(PC)|(SP)|(rZ)", T_REGISTER));
	patterns.push_back(regex_token("r[A-E0-7]", T_REGISTER));
	patterns.push_back(regex_token("set", T_SET));
	patterns.push_back(regex_token("lod", T_LOD));
	patterns.push_back(regex_token("str", T_STR));
	patterns.push_back(regex_token("psh", T_PSH));
	patterns.push_back(regex_token("pop", T_POP));
	patterns.push_back(regex_token("bts", T_BTS));
	patterns.push_back(regex_token("btc", T_BTC));
	patterns.push_back(regex_token("btf", T_BTF));
	patterns.push_back(regex_token("cal", T_CAL));
	patterns.push_back(regex_token("add", T_ADD));
	patterns.push_back(regex_token("sub", T_SUB));
	patterns.push_back(regex_token("mpy", T_MPY));
	patterns.push_back(regex_token("div", T_DIV));
	patterns.push_back(regex_token("mod", T_MOD));
	patterns.push_back(regex_token("and", T_AND));
	patterns.push_back(regex_token("or", T_OR));
	patterns.push_back(regex_token("xor", T_XOR));
	patterns.push_back(regex_token("shf", T_SHF));
	patterns.push_back(regex_token("rot", T_ROT));
	patterns.push_back(regex_token("neg", T_NEG));
	patterns.push_back(regex_token("cmp", T_CMP));
	patterns.push_back(regex_token("jmp", T_JMP));
	patterns.push_back(regex_token("djz", T_DJZ));
	patterns.push_back(regex_token("inc", T_INC));
	patterns.push_back(regex_token("dec", T_DEC));
	patterns.push_back(regex_token("ret", T_RET));
	patterns.push_back(regex_token("[a-z][a-z0-9]*", T_IDENTIFIER));
	patterns.push_back(regex_token("[0-9]+", T_NUMBER));
	patterns.push_back(regex_token("0x[0-9a-f]+", T_NUMBER));
	patterns.push_back(regex_token("0b[01]+", T_NUMBER));
	patterns.push_back(regex_token("\\+", T_PLUS));
	patterns.push_back(regex_token("-", T_MINUS));
	patterns.push_back(regex_token(",", T_COMMA));
	patterns.push_back(regex_token("\\[", T_LBRACKET));
	patterns.push_back(regex_token("\\]", T_RBRACKET));
	patterns.push_back(regex_token(".", T_BAD));

	return;
}


Token get_token(const vector <Token_Regex>& patterns, const string& s, int line_no) {
	Token token;
	token.line_no = line_no;
	int longest = 0;

	#ifdef DEBUG_TRACE
	string tok = "NO MATCHING TOKEN"; // should NEVER see this
	#endif /* DEBUG_TRACE */

	for (const auto &pattern : patterns) {
		smatch match;
		if (regex_search(s, match, pattern.expr, regex_constants::match_continuous)) {
			int length = match.length();
			if (length > longest) {
				longest = length;
				token.id = pattern.id;
				token.token_str = s.substr(0, longest);
				#ifdef DEBUG_TRACE
				tok = tstr[pattern.id];
				#endif /* DEBUG_TRACE */
			}
		}
	}

	#ifdef DEBUG_TRACE
	cerr << "Line " << line_no << ": " << tok << " matched \"";
	if(s.length() <= 10) {
		cerr << s;
	}
	else {
		cerr << s.substr(0, 10) << "...";
	}
	cerr << "\"" << " as \"" << token.token_str << "\"" << endl;
	#endif /* DEBUG_TRACE */

	return token;
}


void tokenize_line(
	string asm_line,
	Token_Line &token_line,
	vector <Token_Regex> &patterns,
	int line_no
) {
	string s(asm_line);
	while(s.length() > 0) {
        Token token = get_token(patterns, s, line_no);
		if(token.id != T_IGNORE && token.id != T_BAD) {
			token_line.push_back(token);
        }
        else if(token.id == T_BAD) {
			cerr << " Line " << line_no << ": Unexpected character '";
			cerr << token.token_str << "'" << endl;
			exit(EXIT_FAILURE);
		}

		/* Advance the string just past the token */
		s = s.substr(token.token_str.length());
	}

	return;
}


void tokenize_all_lines(
	Token_Lines &token_lines,
	vector <string> &asm_lines,
	vector <Token_Regex> &patterns
) {
		for(int i = 0; i < asm_lines.size(); i++) {
		Token_Line token_line;
		tokenize_line(asm_lines[i], token_line, patterns, i + 1);
		if(token_line.size() > 0) {
			Token t;
			t.id = T_EOL;
			t.line_no = i + 1;
			token_lines.push_back(token_line);
		}
	}

	return;
}


void debug_print_lexed_input(Token_Lines &token_lines, vector <string> &asm_lines) {
	int current_i = 1;
	for(int i = 0; i < token_lines.size(); i++) {
		Token_Line line = token_lines[i];
		/* catch up current line to wherever this token was */
		while(current_i < line[0].line_no) {
			cerr << current_i << ": " << asm_lines[current_i - 1] << endl;
			current_i++;
		}

		cerr << current_i << ":";
		for(auto t: line) {
			cerr << "  " << tstr[t.id];
		}
		cerr << endl;
		current_i++;
	}
	return;
}

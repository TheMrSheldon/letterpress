grammar jointparser;

@lexer::header {
    #include <map>
}

@members {
    std::map<std::string, int> activeSequences;

    bool isActiveSequence() noexcept {
        auto x = reinterpret_cast<antlr4::CommonTokenStream*>(getInputStream())->LT(1)->getText();
        auto y = reinterpret_cast<antlr4::CommonTokenStream*>(getInputStream())->LT(2)->getText();
        if (x + y == "\n\n") {
            return true;
        }
        return false;
    }
}

file: (space* preamble_expr space*)* end_preamble content;
end_preamble: '==' '='* '\n';
preamble_expr: p_import_expr | p_doctype_expr;
p_import_expr: 'import' space+ name = ident;
p_doctype_expr:
	'doctype' space+ name = ident space* (args = dict)?;
content: content_part*;
content_part: active_seq | command | space+ | character;
active_seq:
	{isActiveSequence()}? . { getInputStream()->consume(); };
character: .;
command:
	'\\' name = ident ('{' space* params += value space* '}')+;
space: ' ' | '\r' | '\t' | '\n';

/** Value Types: **/
value: array | dict | number | text;
text:
	~('"' | ' ' | '\r' | '\t' | '\n') .+?
	| '"' (~'"' | '\\"')* '"';
number: DIGIT+ | DIGIT* '.' DIGIT+;
array:
	'[' (
		(space* entries += value space* ',')* space* entries += value
	)? space* ']';
dict:
	'{' (
		(space* entries += dict_entry space* ',')* space* entries += dict_entry
	)? space* '}';
dict_entry: key = ident space* ':' space* val = value;
ident: ALPHA (ALPHA | DIGIT)*;

DIGIT: '0' .. '9';
ALPHA: 'A' .. 'Z' | 'a' .. 'z';
UNICODE: ('\u0000' ..'\uFFFF');
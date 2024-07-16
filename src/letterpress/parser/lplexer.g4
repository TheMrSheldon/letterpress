lexer grammar lplexer;

// default mode: preamble
BLOCK_COMMENT       : '/*' .*? '*/' -> channel(HIDDEN) ;
LINE_COMMENT        : '//' ~[\r\n]* -> channel(HIDDEN) ;
DOCTYPE             : 'doctype' ;
IMPORT              : 'import' ;
END_PREAMBLE        : '==' '='* '\n' -> mode(M_CONTENT) ;
IDENT               : [A-Za-z] [A-Za-z0-9]+ ;
LBRACE              : '{' -> pushMode(M_ARGS_KEY) ;
SPACE               : [ \n\r\t] -> channel(HIDDEN) ;

mode M_ARGS_KEY;
ARGS_BLOCK_COMMENT  : '/*' .*? '*/' -> channel(HIDDEN) ;
ARGS_LINE_COMMENT   : '//' ~[\r\n]* -> channel(HIDDEN) ;
ARG_SPACE           : SPACE -> channel(HIDDEN) ;
ARG_IDENT           : [A-Za-z0-9_]+ -> type(IDENT) ;
COLON               : ':' -> mode(M_ARGS_VALUE) ;

mode M_ARGS_VALUE;
VAL_BLOCK_COMMENT   : BLOCK_COMMENT -> channel(HIDDEN) ;
VAL_LINE_COMMENT    : LINE_COMMENT -> channel(HIDDEN) ;
VAL_SPACE           : SPACE -> channel(HIDDEN) ;
BEG_ARRAY           : '[' -> pushMode(M_ARRAY) ;
STRING              : '"' (~'"' | '\\' .)* '"' ;
TEXT                : ~[,/[" \r\n\t}] (~[,}])* ;
AVAL_COMMA          : COMMA -> mode(M_ARGS_KEY), type(COMMA) ;
RBRACE              : '}' -> popMode ;

mode M_ARRAY;
ARR_BLOCK_COMMENT   : BLOCK_COMMENT -> channel(HIDDEN) ;
ARR_LINE_COMMENT    : LINE_COMMENT -> channel(HIDDEN) ;
ARR_SPACE           : SPACE -> channel(HIDDEN) ;
COMMA               : ',' ;
END_ARRAY           : ']' -> popMode ;
ARR_STRING          : STRING -> type(STRING) ;
ARR_BEG_ARRAY       : '[' -> type(BEG_ARRAY), pushMode(M_ARRAY) ;
ARR_TEXT            : ~[,/[" \r\n\t\]] (~[,\]])* -> type(TEXT) ;

mode M_CONTENT;
CNT_BLOCK_COMMENT   : '/*' .*? '*/' -> channel(HIDDEN) ;
CNT_LINE_COMMENT    : '//' ~[\r\n]* -> channel(HIDDEN) ;
COMMAND             : '\\' [A-Za-z0-9]+ ;
PAR                 : '\n\n' ;
CNT_SPACE           : SPACE -> type(SPACE) ;
BEG_INLINE_MATH     : '$' -> pushMode(M_MATH) ;
BEG_DISPLAY_MATH    : '$$' -> pushMode(M_MATH) ;
CNT_LBRACE              : '{' -> type(LBRACE) ;
CNT_RBRACE              : '}' -> type(RBRACE) ;
CHARACTER           : . ;

mode M_MATH;
END_INLINE_MATH     : '$' -> popMode ;
END_DISPLAY_MATH    : '$$' -> popMode ;
M_COMMAND           : '\\' [A-Za-z0-9]+ -> type(COMMAND) ;
M_TEXT              : '"' (~'"' | '\\' .)* '"' ;
SUBSCRIPT           : '_' ;
SUPSCRIPT           : '^' ;
NUMBER              : [0-9]+ | ([0-9]?.[0-9]+) ;
WORD                : [A-Za-z]+ ;
M_SPACE             : SPACE -> type(SPACE) ;
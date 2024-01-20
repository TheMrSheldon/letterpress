parser grammar lpparser;
options { tokenVocab=lplexer; }

file            : preamble_expr* END_PREAMBLE content
                ;
preamble_expr   : p_import_expr
                | p_doctype_expr
                ;
p_import_expr   : IMPORT IDENT
                ;
p_doctype_expr  : DOCTYPE IDENT (LBRACE p_args RBRACE)?
                ;
p_args          : ((p_arg COMMA)* p_arg)?
                ;
p_arg           : IDENT COLON p_arg_val
                ;
p_arg_val       : STRING
                | TEXT
                | array
                ;
array           : BEG_ARRAY (p_arg_val COMMA)* p_arg_val END_ARRAY
                ;
content         : content_part*
                ;
content_part    : c_math
                | COMMAND
                | PAR (SPACE | PAR)*
                | SPACE+
                | .+?       /* Should be removed in practice. Exists now to avoid syntax errors while implementing the grammar further*/
                ;
c_math          : BEG_DISPLAY_MATH c_math_content* END_DISPLAY_MATH
                | BEG_INLINE_MATH c_math_content* END_INLINE_MATH
                ;
c_math_content  : M_TEXT
                | COMMAND
                | WORD
                | NUMBER
                ;
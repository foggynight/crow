# Context-Free Grammar

    SEXP -> ATOM | ' SEXP | ( SEXP TAIL )

    ATOM -> SYMBOL | BOOL | NUMBER | CHARACTER | STRING
    TAIL -> SEXP TAIL | epsilon

    SYMBOL -> ...
    NUMBER -> [0-9]+ | -[0-9]+
    CHARACTER -> #\[ASCII]
    STRING -> ...

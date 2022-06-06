# Context-Free Grammar

    SEXP -> ATOM | ' SEXP | ( REST )

    ATOM -> SYMBOL | BOOL | NUMBER | CHARACTER | STRING
    REST -> SEXP REST | epsilon

    SYMBOL -> ...
    NUMBER -> [0-9]+ | -[0-9]+
    CHARACTER -> #\[ASCII]
    STRING -> ...

# Context-Free Grammar

    SEXP -> ATOM | ' SEXP | ( XS )

    ATOM -> SYMBOL | BOOL | NUMBER | CHARACTER | STRING
    XS -> SEXP XS | epsilon

    SYMBOL -> ...
    NUMBER -> [0-9]+ | -[0-9]+
    CHARACTER -> #\[ASCII]
    STRING -> ...

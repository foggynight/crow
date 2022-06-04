# Context-Free Grammar

    NIL -> ()
    SYMBOL -> ...
    NUMBER -> [0-9]+ | -[0-9]+
    CHARACTER -> #\[ASCII]
    STRING -> ...

    ATOM -> SYMBOL | NUMBER | CHARACTER | STRING
    SEXP -> ATOM | ( SEXP* )

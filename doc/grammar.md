# Context-Free Grammar

    SEXP -> ATOM | ' SEXP | ( REST )

    ATOM -> SYM | BOOL | NUM | CHAR | STR
    REST -> SEXP REST | epsilon

    SYM  -> [^'()]+
    BOOL -> #t | #f
    NUM  -> [0-9]+ | -[0-9]+
    CHAR -> #\[ASCII]
    STR  -> " ... "

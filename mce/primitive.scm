;; primitive.scm - CROW primitive definitions

(define (primitives) `(
  ;; environment
  (toplevel . ,(lambda () toplevel))

  ;; eval / apply
  (eval . ,crow-eval)
  (apply . ,crow-apply)

  ;; logic
  (not . ,(lambda (x) (if (null? x) 't '())))

  ;; symbol
  (sym? . ,(compose bool->atom symbol?))
  (sym->str . ,symbol->string)

  ;; number
  (num? . ,(compose bool->atom number?))
  (+ . ,+)
  (- . ,-)
  (* . ,*)
  (/ . ,/)
  (// . ,(lambda args ((compose floor inexact->exact)
                       (apply / args))))
  (= . ,(lambda args (bool->atom (apply = args))))
  (<> . ,(lambda args (bool->atom (not (apply = args)))))
  (> . ,(lambda args (bool->atom (apply > args))))
  (< . ,(lambda args (bool->atom (apply < args))))
  (>= . ,(lambda args (bool->atom (apply >= args))))
  (<= . ,(lambda args (bool->atom (apply <= args))))
  (floor . ,(compose floor inexact->exact))
  (ceil . ,(compose ceiling inexact->exact))
  (trunc . ,(compose truncate inexact->exact))
  (float . ,exact->inexact)

  ;; atom
  (atom? . ,(compose bool->atom atom?))

  ;; cons
  (cons . ,cons)
  (cons? . ,(compose bool->atom pair?))
  (car . ,car)
  (cdr . ,cdr)

  ;; list
  (list . ,list)
  (null? . ,(compose bool->atom null?))
  (list? . ,(compose bool->atom list?))
  (length . ,length)
  (list-ref . ,list-ref)
  (list->vec . ,list->vector)
  (list->str . ,list->string)

  ;; vector
  (vec . ,vector)
  (make-vec . ,(lambda (size . fill)
                 (make-vector size (if (null? fill) '() (car fill)))))
  (vec? . ,(compose bool->atom vector?))
  (vec-len . ,vector-length)
  (vec-ref . ,vector-ref)
  (vec-set! . ,vector-set!)
  (vec->list . ,vector->list)

  ;; character
  (char? . ,(compose bool->atom char?))
  (char=? . ,(compose bool->atom char=?))
  (char<? . ,(compose bool->atom char<?))
  (char>? . ,(compose bool->atom char>?))
  (char<=? . ,(compose bool->atom char<=?))
  (char>=? . ,(compose bool->atom char>=?))
  (char->int . ,char->integer)
  (int->char . ,integer->char)

  ;; string
  (str . ,string)
  (str? . ,(compose bool->atom string?))
  (str-len . ,string-length)
  (str-ref . ,string-ref)
  (str-set! . ,string-set!)
  (str->sym . ,string->symbol)
  (str->list . ,string->list)
  (str->num . ,(lambda args
                 ((lambda (x)
                    (if x x (bool->atom x)))
                  (apply string->number args))))

  ;; ports
  (in-port? . ,(compose bool->atom input-port?))
  (out-port? . ,(compose bool->atom output-port?))
  (curr-in-port . ,current-input-port)
  (curr-out-port . ,current-output-port)
  (open-in-file . ,open-input-file)
  (open-out-file . ,open-output-file)
  (close-in-port . ,close-input-port)
  (close-out-port . ,close-output-port)

  ;; input
  (eof? . ,(compose bool->atom eof-object?))
  (ready? ,(compose bool->atom char-ready?))
  (read . ,read)
  (read-char . ,read-char)
  (peek-char . ,peek-char)

  ;; output
  (write . ,write)
  (display . ,display)
  (print . ,print)
  (newline . ,newline)
  (write-char . ,write-char)

  ;; misc
  (args . ,command-line-arguments)
  (load . ,(lambda (name) (crow-load name) '()))
  (exit . ,exit)
  (error . ,crow-error)
  (void . ,void)
)) ; primitives

#!/usr/bin/env -S csi -q

;; crow.scm - CROW metacircular evaluator and REPL in CHICKEN Scheme
;;
;; This will be used to test ideas for CROW, and will be replaced by the
;; interpreter written in C.
;;
;; INTERPRET
;;   ./crow.scm
;;
;; COMPILE
;;   csc crow.scm
;;   ./crow [FILE...]
;;
;; NOTE: Filenames can only be passed as arguments when the program is compiled,
;; otherwise arguments to the CHICKEN interpreter will be included.

(import chicken.process-context)

;; misc ------------------------------------------------------------------------

(define true? (compose not null?))
(define false? null?)
(define (bool->atom bool) (if bool 't '()))

(define (list-cars lst) (map car lst))
(define (list-cadrs lst) (map cadr lst))

(define (zip keys dats)
  (map (lambda (x y) (cons x y)) keys dats))

;; environment -----------------------------------------------------------------

(define (lookup sym env)
  (if (null? env)
      (error 'lookup "unbound symbol" sym)
      ((lambda (cell)
         (if cell
             (cdr cell)
             (lookup sym (cdr env))))
       (assq sym (car env)))))

(define (bind keys dats env)
  (cons (zip keys dats) env))

(define (env-insert! env par)
  (set-car! env (cons par (car env))))

;; closure ---------------------------------------------------------------------
;;
;; Closures represent procedures with zero or more free variables bound within
;; an environment, they are represented by lists of the form:
;;
;;   ('closure (args body) env)
;;
;; Where args and body are the arguments and body of the lambda expression used
;; to create the closure, and env is the environment of that lambda expression.

(define (closure exp env) (list 'closure (cdr exp) env))
(define (closure? exp) (eq? (car exp) 'closure))

(define closure-args caadr)
(define closure-body cadadr)
(define closure-env caddr)

;; evaluator -------------------------------------------------------------------

(define (evlist lst env)
  (map (lambda (x) (crow-eval x env)) lst))

(define (evcond clauses env)
  (cond ((null? clauses) '())
        ((eq? (caar clauses) 'else) (crow-eval (cadar clauses) env))
        ((false? (crow-eval (caar clauses) env)) (evcond (cdr clauses) env))
        (else (crow-eval (cadar clauses) env))))

(define (evif exp env)
  (define pred (car exp))
  (define body-true (cadr exp))
  (define body-false (if (null? (cddr exp)) '() (caddr exp)))
  (if (true? (crow-eval pred env))
      (crow-eval body-true env)
      (if (false? body-false)
          '()
          (crow-eval body-false env))))

(define (evand exp last env)
  (if (null? exp)
      last
      (let ((val (crow-eval (car exp) env)))
        (if (false? val)
            '()
            (evand (cdr exp) val env)))))

(define (evor exp last env)
  (if (null? exp)
      last
      (let ((val (crow-eval (car exp) env)))
        (if (true? val)
            val
            (evor (cdr exp) val env)))))

(define (evlet exp env)
  (define binds (car exp))
  (define body (cadr exp))
  (crow-eval body
             (bind (list-cars binds)
                   (evlist (list-cadrs binds) env)
                   env)))

(define (evdef def env)
  (cond ((null? def) (error 'evdef "invalid definition"))
        ((symbol? (car def)) (cons (car def) (crow-eval (cadr def) env)))
        (else (cons (caar def)
                    (crow-eval (cons 'lambda (cons (cdar def) (cdr def)))
                               env)))))

(define (evset! exp env)
  (if (null? env)
      (error 'evset! "unbound symbol" (car exp))
      (let ((par (assq (car exp) (car env))))
        (if par
            (set-cdr! par (crow-eval (cadr exp) env))
            (evset! exp (cdr env))))))

(define (evspec exp env toplvl)
  (case (car exp)
    ((quote) (cadr exp))
    ((% lambda) (closure exp env))
    ((cond) (evcond (cdr exp) env))
    ((if) (evif (cdr exp) env))
    ((and) (evand (cdr exp) 't env))
    ((or) (evor (cdr exp) '() env))
    ((let) (evlet (cdr exp) env))
    ((def define)
     (if toplvl
         (env-insert! env (evdef (cdr exp) env))
         (error 'crow-eval "definition outside toplevel")))
    ((set!) (evset! (cdr exp) env))
    (else #f)))

(define (crow-eval exp env #!optional toplvl)
  (cond ((or (number? exp) (char? exp) (string? exp)) exp)
        ((symbol? exp) (lookup exp env))
        ((evspec exp env toplvl)) ; returns false when exp is not a special form
        (else (crow-apply (crow-eval (car exp) env)
                          (evlist (cdr exp) env)))))

(define (crow-apply proc args)
  (cond ((primitive? proc) (papply proc args))
        ((closure? proc) (crow-eval (closure-body proc)
                                    (bind (closure-args proc)
                                          args
                                          (closure-env proc))))
        (else (error 'crow-apply "not a primitive or closure" exp))))

;; primitive -------------------------------------------------------------------

(define primitives `(
  ;; environment
  (toplevel . ,(lambda () toplevel))

  ;; eval / apply
  (eval . ,crow-eval)
  (apply . ,crow-apply)

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
  (>= . ,(lambda args (bool->atom (apply > args))))
  (<= . ,(lambda args (bool->atom (apply < args))))
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

  ;; vector
  (vec . ,vector)
  (make-vec . ,(lambda (size . fill)
                 (make-vector size (if (null? fill) '() (car fill)))))
  (vec? . ,(compose bool->atom vector?))
  (vec-len . ,vector-length)
  (vec-ref . ,vector-ref)
  (vec-set! . ,vector-set!)
  (vec->list . ,vector->list)

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

  ;; misc
  (exit . ,exit)
)) ; primitives

(define (primitive? proc) (procedure? proc))
(define (papply proc args) (apply proc args))

;; repl ------------------------------------------------------------------------

(define toplevel (list '() primitives)) ; toplevel environment

(define (display-prompt)
  (display "> "))

(define (crow-repl #!optional prompt)
  (when prompt (display-prompt))
  (let ((exp (read)))
    (if (eof-object? exp)
        (when prompt (newline) (exit))
        (begin ((lambda (x)
                  (unless (eq? x (void))
                    (write x)
                    (newline)))
                (crow-eval exp toplevel #t))
               (crow-repl prompt)))))

(let ((args (command-line-arguments)))
  (unless (or (null? args)
              (string=? (car args) "-q"))
    (for-each (lambda (arg)
                (with-input-from-file arg
                  crow-repl))
              args)))
(crow-repl #t)

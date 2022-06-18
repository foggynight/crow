#!/usr/bin/env -S csi -q

;; crow.scm - CROW Metacircular Evaluator and REPL in CHICKEN Scheme
;;
;; This will be used to test ideas for CROW, and will be replaced by the
;; interpreter written in C.
;;
;; INTERPRET
;;   ./crow.scm
;;
;; COMPILE
;;   make mce
;;   ./crow [FILE...]
;;
;; NOTE: Filenames can only be passed as arguments when the program is compiled,
;; otherwise arguments to the CHICKEN interpreter will be included.

(import chicken.format
        chicken.process-context
        simple-exceptions)

;; misc ------------------------------------------------------------------------

(define true? (compose not null?))
(define false? null?)
(define (bool->atom bool) (if bool 't '()))

(define (list-cars lst) (map car lst))
(define (list-cadrs lst) (map cadr lst))

(define (zip keys dats)
  (map (lambda (x y) (cons x y)) keys dats))

;; error -----------------------------------------------------------------------

(define (crow-error . args)
  (define fmt (apply string-append (cons "error"
                                         (map (lambda (a) ": ~A") args))))
  (error (apply sprintf (cons fmt args))))

;; environment -----------------------------------------------------------------

(define (fetch sym env)
  (if (null? env)
      (crow-error 'fetch "unbound symbol" sym)
      ((lambda (cell)
         (if cell cell (fetch sym (cdr env))))
       (assq sym (car env)))))

(define (lookup sym env)
  (cdr (fetch sym env)))

(define (bind keys dats env)
  (cons (zip keys dats) env))

(define (env-insert! env par)
  (set-car! env (cons par (car env))))

;; closure ---------------------------------------------------------------------
;;
;; Closures represent procedures with zero or more free variables bound within
;; an environment, they are represented by lists of the form:
;;
;;   ('closure ((args*) ('body exp*)) env)
;;
;; Where args, body, and env are from the lambda expression used to create the
;; closure.

;; Create a closure from EXP bound in ENV. EXP should be an expression of the
;; form: ((args*) sexp*).
(define (closure exp env)
  `(closure (,(car exp) (body . ,(cdr exp))) ,env))

(define (closure? exp)
  (and (pair? exp) (eq? (car exp) 'closure)))

(define closure-args caadr)
(define closure-body cadadr)
(define closure-env caddr)

;; evaluator -------------------------------------------------------------------

(define (evlist lst env)
  (map (lambda (x) (crow-eval x env)) lst))

(define (evcond clauses env)
  (cond ((null? clauses) '())
        ((eq? (caar clauses) 'else) (crow-eval (cons 'body (cdar clauses)) env))
        ((false? (crow-eval (caar clauses) env)) (evcond (cdr clauses) env))
        (else (crow-eval (cons 'body (cdar clauses)) env))))

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
  (define body (cons 'body (cdr exp)))
  (crow-eval body
             (bind (list-cars binds)
                   (evlist (list-cadrs binds) env)
                   env)))

(define (evdef def env)
  (cond ((null? def) (crow-error 'evdef "invalid definition"))
        ((symbol? (car def)) (cons (car def) (crow-eval (cadr def) env)))
        (else (cons (caar def)
                    (crow-eval (cons 'lambda (cons (cdar def) (cdr def)))
                               env)))))

(define (evset! exp env)
  (define par (fetch (car exp) env))
  (set-cdr! par (crow-eval (cadr exp) env)))

(define (evbody exp env toplvl)
  (if (null? exp)
      '()
      (let ((val (crow-eval (car exp) env toplvl)))
        (if (null? (cdr exp))
            val
            (evbody (cdr exp) env toplvl)))))

(define (evspec exp env toplvl)
  (case (car exp)
    ((quote) (cadr exp))
    ((% lambda) (closure (cdr exp) env))
    ((cond) (evcond (cdr exp) env))
    ((if) (evif (cdr exp) env))
    ((and) (evand (cdr exp) 't env))
    ((or) (evor (cdr exp) '() env))
    ((let) (evlet (cdr exp) env))
    ((def define)
     (if toplvl
         (begin (env-insert! env (evdef (cdr exp) env)) '())
         (crow-error 'evspec "definition outside toplevel")))
    ((set!) (evset! (cdr exp) env))
    ((body) (evbody (cdr exp) env toplvl))
    (else #f)))

(define (crow-eval exp env #!optional toplvl)
  (cond ((symbol? exp) (lookup exp env))
        ((or (number? exp) (char? exp) (string? exp)) exp)
        ((list? exp) (let ((val (evspec exp env toplvl)))
                       (if val val (crow-apply (crow-eval (car exp) env)
                                               (evlist (cdr exp) env)))))
        (else (crow-error 'crow-eval "invalid expression" exp))))

(define (crow-apply proc args)
  (cond ((primitive? proc) (papply proc args))
        ((closure? proc) (crow-eval (closure-body proc)
                                    (bind (closure-args proc)
                                          args
                                          (closure-env proc))))
        (else (crow-error 'crow-apply "invalid procedure" proc))))

;; primitive -------------------------------------------------------------------

(define (primitive? proc) (procedure? proc))
(define (papply proc args) (apply proc args))

(include "primitive.scm")

;; repl ------------------------------------------------------------------------

(define (display-banner)
  (printf "CROW v0.0.0~%~
           (C) 2022 Robert Coffey~%"))
(define (display-prompt) (display "> "))

(define (crow-repl ip #!optional prompt)
  (when prompt (display-prompt))
  (let ((exp (read ip)))
    (if (eof-object? exp)
        (when prompt (newline) (exit))
        (begin ((lambda (x)
                  (when prompt (write x) (newline)))
                (crow-eval exp toplevel #t))
               (crow-repl ip prompt)))))

(define (crow-load name)
  (call-with-input-file name crow-repl))

;; main ------------------------------------------------------------------------

(define toplevel (list '() (primitives))) ; toplevel environment

(define (main-load #!optional (args (command-line-arguments)))
  (unless (or (null? args) (string=? (car args) "-q"))
    (crow-load (car args)))
  void) ; Must return a procedure to avoid error in WITH-EXN-HANDLER.

(define (main-repl) (crow-repl (current-input-port) #t))

(display-banner)
(letrec ((h-load (lambda (exn)
                   (print (message exn))
                   (exit)))
         (h-repl (lambda (exn)
                   (print (message exn))
                   (with-exn-handler h-repl main-repl))))
  (with-exn-handler h-load main-load)
  (with-exn-handler h-repl main-repl))
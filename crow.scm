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
;; otherwise arguments to the CHICKEN interpreter will be included in args.

(import chicken.format
        chicken.process-context)

;; misc ------------------------------------------------------------------------

(define (alist-copy lst)
  (if (null? lst)
      '()
      (cons (cons (caar lst) (cdar lst))
            (alist-copy (cdr lst)))))

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

(define (env-copy env)
  (if (null? env)
      '()
      (cons (alist-copy (car env)) (env-copy (cdr env)))))

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

(define (closure exp env) (list 'closure (cdr exp) (env-copy env)))
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
        ((null? (crow-eval (caar clauses) env)) (evcond (cdr clauses) env))
        (else (crow-eval (cadar clauses) env))))

(define (evdef def env)
  (cond ((null? def) (error 'evdef "invalid definition"))
        ((symbol? (car def)) (cons (car def) (crow-eval (cadr def) env)))
        (else (cons (caar def)
                    (crow-eval (cons 'lambda (cons (cdar def) (cdr def)))
                               env)))))

(define (crow-eval exp env #!optional toplvl)
  (cond ((number? exp) exp)
        ((symbol? exp) (lookup exp env))
        (else (case (car exp)
                ((quote) (cadr exp))
                ((% lambda) (closure exp env))
                ((cond) (evcond (cdr exp) env))
                ((def define)
                 (if toplvl
                     (let* ((par (evdef (cdr exp) env))
                            (key (car par))
                            (dat (cdr par)))
                       (when (closure? dat)   ; insert self into own environment
                         (env-insert! (closure-env dat) par))
                       (env-insert! env par)) ; insert into toplevel environment
                     (error 'crow-eval "definition outside toplevel")))
                (else (crow-apply (crow-eval (car exp) env)
                                  (evlist (cdr exp) env)))))))

(define (crow-apply proc args)
  (cond ((primitive? proc) (papply proc args))
        ((closure? proc) (crow-eval (closure-body proc)
                                    (bind (closure-args proc)
                                          args
                                          (closure-env proc))))
        (else (error 'crow-apply "not a primitive or closure" exp))))

;; primitive -------------------------------------------------------------------

(define primitives
  `((toplevel . ,(lambda () toplevel))
    (+ . ,(lambda args (apply + args)))
    (- . ,(lambda args (apply - args)))
    (* . ,(lambda args (apply * args)))
    (/ . ,(lambda args (apply / args)))
    (= . ,(lambda args (if (apply = args) 't '())))
    (> . ,(lambda args (if (apply > args) 't '())))
    (< . ,(lambda args (if (apply < args) 't '())))))

(define (primitive? proc) (procedure? proc))
(define (papply proc args) (apply proc args))

;; repl ------------------------------------------------------------------------

(define toplevel (list '() primitives)) ; toplevel environment

(define (crow-repl)
  (define exp (read))
  (unless (eof-object? exp)
    ((lambda (x)
       (unless (eq? x (void))
         (print x)))
     (crow-eval exp toplevel #t))
    (crow-repl)))

(let ((args (command-line-arguments)))
  (cond ((or (null? args)
             (string=? (car args) "-q"))
         (crow-repl))
        (else (for-each (lambda (arg)
                          (with-input-from-file arg
                            crow-repl))
                        args)
              (crow-repl))))

(exit)

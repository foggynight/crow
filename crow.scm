#!/usr/bin/env -S csi -q

;; crow.scm - CROW metacircular evaluator and REPL in CHICKEN Scheme
;;
;; This will be used to test ideas for CROW, and will be replaced by the
;; interpreter written in C.

(import chicken.format)

;; misc ------------------------------------------------------------------------

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

;; primitive -------------------------------------------------------------------

(define primitives
  `((+ . ,(lambda args (apply + args)))
    (- . ,(lambda args (apply - args)))
    (* . ,(lambda args (apply * args)))
    (/ . ,(lambda args (apply / args)))))

(define (primitive? proc) (procedure? proc))
(define (papply proc args) (apply proc args))

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

(define toplevel (list primitives)) ; toplevel environment

(define (evlist lst env) (map (lambda (x) (crow-eval x env)) lst))
(define (evcond clauses env)
  (cond ((null? clauses) '())
        ((eq? (caar clauses) 'else) (crow-eval (cadar clauses) env))
        ((null? (crow-eval (caar clauses) env)) (evcond (cdr clauses) env))
        (else (crow-eval (cadar clauses) env))))

(define (crow-eval exp env)
  (cond ((number? exp) exp)
        ((symbol? exp) (lookup exp env))
        (else (case (car exp)
                ((quote) (cadr exp))
                ((lambda %) (closure exp env))
                ((cond) (evcond (cdr exp) env))
                (else (crow-apply (crow-eval (car exp) env)
                                  (evlist (cdr exp) env)))))))

(define (crow-apply proc args)
  (cond ((primitive? proc) (papply proc args))
        ((closure? proc) (crow-eval (closure-body proc)
                                    (bind (closure-args proc)
                                          args
                                          (closure-env proc))))
        (else (error 'crow-apply "not a primitive or closure" exp))))

;; repl ------------------------------------------------------------------------

(define (crow-repl)
  (do () (#f)
    (print (crow-eval (read) toplevel))))

(crow-repl)

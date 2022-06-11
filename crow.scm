;; crow.scm - CROW evaluator and REPL in CHICKEN Scheme
;;
;; This will be used to test ideas for CROW, and will be replaced by the
;; interpreter written in C.

(import chicken.format)

;; Environments ----------------------------------------------------------------

(define (lookup) (void))
(define (bind) (void))

;; Primitives ------------------------------------------------------------------

(define (primitive?) (void))
(define (papply proc args) (void))

;; Closures --------------------------------------------------------------------
;;
;; Closures represent procedures with zero or more free variables bound within
;; an environment, they are represented lists of the form:
;;
;;   ('closure (args body) env)
;;
;; Where args and body are the arguments and body of the lambda expression used
;; to create the closure, and env is the environment of that lambda expression.

(define (closure exp env) (list 'closure (cdr exp) env))
(define (closure? exp) (eq? (car exp) 'closure))

(define closure-args caadr)
(define closure-proc cadadr)
(define closure-env caddr)

;; Meta-Circular Evaluator -----------------------------------------------------

(define (crow-eval exp env)
  (cond ((number? exp) exp)
        ((symbol? exp) (lookup exp env))
        (else (case (car exp)
                ((quote) (cadr exp))
                ((lambda %) (closure exp env))
                ((cond) (evcond (cdr exp) env))
                (else (crow-apply (crow-eval (car exp) env)
                                  (crow-evlist (cdr exp) env)))))))

(define (crow-apply proc args)
  (cond ((primitive? proc) (papply proc args))
        ((closure? proc) (eval (closure-proc exp)
                               (bind (closure-args exp)
                                     args
                                     (closure-env exp))))
        (else (printf "not a primitive or closure: ~A~%" exp))))

;; REPL ------------------------------------------------------------------------

(define (crow-repl)
  (do () (#f)
    (print (crow-eval (read) '()))))

(crow-repl)

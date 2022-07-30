(define lf (lambda (x) x))
(define lg (lambda (x . y) (cons x y)))
(define lh (lambda lst lst))

(define (df x) x)
(define (dg x . y) (cons x y))

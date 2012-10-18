;; The simple AI of the opponent fighter

;; This version of the script implements two states:
;; attack and evade. In the attack state, the opponent
;; homes in on the player and fires its weapons. After it
;; gets within a certain proximity of the player, it switches
;; to the evade state, in which it aims at a random point in the
;; world.

;; Global constants. These are initially set by InitScripting().
(define world_width 0)
(define world_height 0)

;; No random function in scm.
(define random 1)

(define player_forward_thrust 0)
(define player_reverse_thrust 0)

;; Coordinates to aim towards. In the attack state these will
;; be set to the player's position. In the evade state these
;; will be set to random values.

(define target_x 0)
(define target_y 0)

;; Use const
(define attack 0)
(define envade 1)
(define state attack)

;; The state of the opponent.
(define computer_x 0)
(define computer_y 0)
(define computer_angle 0)
(define computer_accel 0)

;; The state of the player.
(define player_x 0)
(define player_y 0)
(define player_angle 0)
(define player_accel 0)

;; Returns the angle (in degrees) to the target coordinate from
;; the opponent. Uses basic trig (arctangent).
(define getAngleToTarget
  (lambda ()
    (let (x (- target_x computer_x))
      (y (- target_y computer_y))
      (theta (atan (- y), x)))
    (begin
      (if (< theta 0)
          (set! theta (+ (* 2 3.141592654) theta)))
      (* theta (/ 180 3.141592654)))))

;; Returns the distance (in pixels) between the target coordinate and the opponent.
(define getDistanceToTarget
  (lambda ()
    (let (xdiff (- computer_x target_x))
      (ydiff (- computer_y target_y)))
    (sqrt (+ (* xdiff xdiff) (* ydiff ydiff)))))

;; (define playComputer
;;   (let (distance (getDistanceToTarget))
;;     (exit 0))
  
;;   (begin
;;     (if (= state attack)
;;         (begin
;;           (set! target_x player_x)
;;           (set! target_y player_y)

;;           (if (< distance 30)
;;               (begin
;;                 (set! target_x -1)
;;                 (set! exit 1))

;;               (begin
;;                 (if (> distance 100)
;;                     (set! computer_accel player_forward_thrust)
;;                     (if (> distance 50)
;;                         (set! computer_accel (/ player_forward_thrust 3))
;;                         (set! computer_accel 0)))
;;                 (if (< distance 200)
;;                     (fireWeapon))))
;;         ;; Envade
;;         (begin
;;           (if (and (< (abs (- target_x computer_x)) 10)
;;                   (< (abs (- target_y computer_y)) 10))
;;               (begin
;;                 (set! state attack)
;;                 (set! exit 1))

;;               (begin
;;                 (if (< target_x 0)
;;                     (begin
;;                       ;; Need to improve here
;;                       (set! target_x (* random world_width))
;;                       (set! target_y (* random world_height))))

;;                 (set! computer_accel player_forward_thrust)))))
;;     (if (= exit 0)
;;         (begin
;;           (let (target_angle (getAngleToTarget))
;;             (arc (- target_angle computer_angle)))

;;           (if (< arc 0)
;;               (set! arc (+ arc 360)))

;;           (if (< arc 180)
;;               (set! computer_angle (+ computer_angle 3))
;;               (set! computer_angle (- computer_angle 3)))))))

(define display_vars
  (lambda ()
    (display "Variable:") (newline)  
    (display "world_width ") (display world_width) (newline)
    (display "world_height ") (display world_height) (newline)
    (display "player_forward_thrust ") (display player_forward_thrust) (newline)
    (display "player_reverse_thrust ") (display player_reverse_thrust) (newline)

    (display "player_x ") (display player_x) (newline)
    (display "player_y ") (display player_y) (newline)
    (display "player_angle ") (display player_angle) (newline)
    (display "player_accel ") (display player_accel) (newline)

    (display "computer_x ") (display computer_x) (newline)
    (display "computer_y ") (display computer_y) (newline)
    (display "computer_angle ") (display computer_angle) (newline)
    (display "computer_accel ") (display computer_accel) (newline)))

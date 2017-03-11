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
;; define in c
;; (define random 1)
;; (define fireWeapon 1)

(define player_forward_thrust 0)
(define player_reverse_thrust 0)

;; Coordinates to aim towards. In the attack state these will
;; be set to the player's position. In the evade state these
;; will be set to random values.

(define target_x 0)
(define target_y 0)

(define ATTACK 0)
(define ENVADE 1)
(define state ATTACK)

;; The state of the opponent.
(define opponent_x 0)
(define opponent_y 0)
(define opponent_angle 0)
(define opponent_accel 0)

;; The state of the player.
(define player_x 0)
(define player_y 0)
(define player_angle 0)
(define player_accel 0)

;; Returns the angle (in degrees) to the target coordinate from
;; the opponent. Uses basic trig (arctangent).
(define getAngleToTarget
  (lambda ()
    (let* ((x (- target_x opponent_x))
           (y (- target_y opponent_y))
           (theta (atan (- y) x)))
      (begin
        (if (< theta 0)
            (set! theta (+ (* 2 3.141592654) theta)))
        (* theta (/ 180 3.141592654))))))

;; Returns the distance (in pixels) between the target coordinate and the opponent.
(define getDistanceToTarget
  (lambda ()
    (let ((xdiff (- opponent_x target_x))
          (ydiff (- opponent_y target_y)))
      (sqrt (+ (* xdiff xdiff) (* ydiff ydiff))))))

(define playOpponent
  (lambda ()
    (begin
      (if (= state ATTACK)
          (let ((distance 0))
            (set! target_x player_x)
            (set! target_y player_y)
            (set! distance (getDistanceToTarget))

            (if (< distance 30)
                (begin
                  (set! state ENVADE)
                  (set! target_x -1))
                (begin
                  (if (> distance 100)
                      (set! opponent_accel player_forward_thrust)
                      (if (> distance 50)
                          (set! opponent_accel (/ player_forward_thrust 3))
                          (set! opponent_accel 0)))
                  (if (< distance 200)
                      (fireWeapon)))))
            ;; ENVADE
            (begin
              (if (and (< (abs (- target_x opponent_x)) 10)
                       (< (abs (- target_y opponent_y)) 10))
                  (begin
                    (set! state ATTACK))
                  (begin
                    (if (< target_x 0)
                        (begin
                          (set! target_x (* (random) world_width))
                          (set! target_y (* (random) world_height))))
                    (set! opponent_accel player_forward_thrust)))))

      (let* ((target_angle (getAngleToTarget))
            (arc (- target_angle opponent_angle)))

        (if (< arc 0)
            (set! arc (+ arc 360)))

        (if (< arc 180)
            (set! opponent_angle (+ opponent_angle 3))
            (set! opponent_angle (- opponent_angle 3)))))))

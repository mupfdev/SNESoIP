; Deactivate pseudo-random number generator

; Note: The items you get from containers vary by the values you store
; at $9F:0024 and $9F:0025.  A static value of '0' on both addresses is
; resulting in a squirt gun (default weapon) in every container.

org $809D39
    SEP #$20
    LDA #$00
    STA $0024
    LDA #$00
    STA $0025
    REP #$20
    AND #$00FF
    RTL

; Skip Konami-Logo

org $838000
    RTL

; Accelerate Lucas Arts-Logo and make it unskippable

org $809330
    SEI

; Remove screen-transitions

org $809748 ; Main menu
    NOP
    NOP
org $8099C1 ; Character select
    NOP
    NOP

; Skip wave-effect

org $809527
    BMI $E2

; Skip main menu

org $80974F
    BCC $03
org $809751
    JMP $96C7

; Start with 7 lives cheat

org $808880
    db $06

; Make sure that both players have selected a character

; Note: If a players presses start, the counter at $7E:0C5E is set to
; '780' once.  But because of the removed INC operation at $80:99C1, the
; final condition is never reached.  The solution was to change the
; condition to '2' (instead of '900') and increment it by '1' when the
; first or the second player press start.

org $809958 ; Player 1
    INC $5E
org $80998F ; Player 2
    INC $5E
org $8099C5 ; Condition
    CMP #$0002

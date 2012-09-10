; Note: This file is recreated by the project wizard whenever the MCU is
;       changed and should not be edited by hand

; include derivative specific macros
            INCLUDE 'MC68HC908LJ12.inc'


; RAM/ROM definitions


; Watchdog feed macro
feed_watchdog: MACRO
            STA COPCTL           ; feed the watchdog
          ENDM

# Test

## 10 Fragen: Lektionen 1-3

Der ATmega ist ein RISC Prozessor. Wofür steht RISC?

- Reduced Instruction Set Computer

Worauf ist bei der Verwendung Rest-Operators `%` zu achten?

- Der Rest Operator entspricht  in ANSI C nicht der mathematischen Modulo Operation, da er vorzeichenbehaftet ist. Der `%` Operator is nicht in jeder Programmiersprache gleich ausprogrammiert und kann daher zu Problemen bei der Portierung auf andere Systeme führen.

Nennen Sie den maximalen Wertebereich für eine signed short Variable.

- Wertebereich: `-32'768 ... +32767`.

Nennen Sie den maximalen Wertebereich für eine unsigned char Variable.

- Wertebereich: `0 ... 255`.

Von welcher Adresse startet der uC ATmega2560 nach einem Reset?

- `0x0000` (p. 101)

Wie nennt man diese Adresse?

- Reset Vector (p. 101)

Nennen Sie drei mögliche Reset-Quellen des ATmega2560?

- Power on Reset
- Watchdog Reset
- Brown out Reset

Wozu dient ein Watchdog Timer?

- Er zählt wie lange ein Programm läuft.

Über welche Register verfügt jeder IO Ports des ATmega2560?

- Data Direction `DDRx`
- Data Register `PORTx`
- Input Pin Register `PINx`

Welchen Reset Wert haben die Register der IO Ports und warum?

- Die Register werden mit `0x00` initialisiert. Damit sind alle Ports Pins als Tri-State (hochohmig) konfiguriert und es kann keine angeschlossene HW zerstört werden.

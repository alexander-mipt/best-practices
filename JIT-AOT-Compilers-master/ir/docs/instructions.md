# IR ISA Description

### CMP
* Typed
* Arguments: [ConditionCode, lhs, rhs]
* Stores comparison result in accumulator
* Must be immediately followed by `JCMP` instruction

### JCMP
* Untyped
* Arguments: []
* Jumps into target instruction if accumulator's value is true, proceeds linear execution otherwise
* Must immediately preceed a `CMP` instruction

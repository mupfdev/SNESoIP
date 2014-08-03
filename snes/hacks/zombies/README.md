# Zombies Ate The Internet #

A SNESoIP optimised version of Zombies Ate My Neighbours.

```
ROM: Zombies Ate My Neighbors (U) [!].smc
MD5: 23c2af7897d9384c4791189b68c142eb
```


## Hacklog ##

### Fix pseudorandom number generator ###

Note: The items you get from containers vary by the values you store at
$9F:0024 and $9F:0025. A static value of '0' on both addresses is
resulting in a squirt gun (default weapon) in every container.

#### Original ####

```
$80/9D39 E2 20       SEP #$20
$80/9D3B AD 24 00    LDA $0024
$80/9D3E 2E 24 00    ROL $0024
$80/9D41 4D 24 00    EOR $0024
$80/9D44 6E 24 00    ROR $0024
$80/9D47 EE 25 00    INC $0025
$80/9D4A 6D 25 00    ADC $0025
$80/9D4D 50 03       BVC $03
$80/9D52 8D 24 00    STA $0024
$80/9D55 C2 20       REP #$20
$80/9D57 29 FF 00    AND #$00FF
$80/9D5A 6B          RTL
```

#### Modified ####

```
$80/9D39 E2 20       SEP #$20
$80/9D3B A9 00       LDA #$00
$80/9D3D 8D 24 00    STA $0024
$80/9D40 A9 00       LDA #$00
$80/9D42 8D 25 00    STA $0025
$80/9D45 C2 20       REP #$20
$80/9D47 29 FF 00    AND #$00FF
$80/9D4A 6B          RTL
```

### Skip Konami-Logo ###

#### Original ####

```
$80/9140 22 00 80 83 JSL
$83/8000 8B          PHB
```

#### Modified ####

```
$80/9140 22 00 80 83 JSL
$83/8000 6B          RTL
```

### Accelerate Lucas Arts-Logo and make it unskippable ###

#### Original ####

```
$80/9330 CB          WAI
```

#### Modified ####

```
$80/9330 78          SEI
```

### Remove screen-transitions ###

Note: The counter value is stored at $7E:0C5E. Thanks to Piranhaplant
for pointing it out.

#### Original ####

Main menu:
```
$80/9748 E6 5E       INC $5E
```

Character select:
```
$80/99C1 E6 5E       INC $5E
```

#### Modified #####

Main menu:
```
$80/9748 EA          NOP
$80/9749 EA          NOP
```

Character select:
```
$80/99C1 EA          NOP
$80/99C2 EA          NOP
```

### Skip wave-effect ###

#### Original ####

```
$80/9527 10 E2       BPL $E2
```

#### Modified ####

```
$80/9527 30 E2       BMI $E2
```

### Skip main menu ###

#### Original ####

```
$80/974F B0 03       BCS $03
$80/9751 4C C7 96    JMP $96C7
```

#### Modified ####

```
$80/974F 90 03       BCC $03
$80/9751 4C C7 96    JMP $96C7
```

### Make sure that both players have selected a character ###

Note: If a players presses start, the counter at $7E:0C5E is set to
'780' once. But because of the removed INC operation at $80:99C1, the
final condition is never reached. The solution was to change the
condition to '2' (instead of '900') and increment it by '1' when the
first or the second player press start.

#### Original ####

Player 1:
```
$80/9955 A9 0C 03    LDA #$030C
$80/9958 85 5E       STA $5E
```

Player 2:
```
$80/998C A9 0C 03    LDA #$030C
$80/998F 85 5E       STA $5E
```

Condition:
```
$80/974A A5 5E       LDA $5E
$80/974C C9 84 03    CMP #$0384
```

#### Modified ####

Player 1:
```
$80/9958 E6 5E       INC $5E
```

Player 2:
```
$80/998F E6 5E       INC $5E
```

Condition:
```
$80/99C3 A5 5E       LDA $5E
$80/99C5 C9 02 00    CMP #$0002
```

### 7 Lives ###

#### Game Genie ####

Note: Patched using GGGuy.

```
D166-4DD4
```

## Special thanks goes to ##

- [Piranhaplant](mailto:piranhaplant1@gmail.com) for his incredible
knowledge about the game and his great level editor,
[zamn-editor](https://code.google.com/p/zamn-editor/), which was used,
among other things, to remove the random-effect potions from the levels,

- Kyle for releasing
  [GGGuy](http://www.romhacking.net/forum/index.php?topic=17595.0).

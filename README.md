# MT-recursive-parser

Рассмотрим грамматику для арифметических выражений со сложением, вычитанием и унарным минусом:
```
E -> E+T
E -> E-T
E -> T
T -> T*F
T -> F
F -> -F
F -> P
P -> n
P -> (E)
```
* E - арифметическое выражение
* Т - обобщённое слагаемое
* F - обобщённый множитель c унарным минусом
* P - обобщённый множитель без унарного минуса с учётом скобок

Заметим, что в все операции в ней имеют левую ассоциативность, что собенно важно для `-`. Устраним левую рекурсию:
```
E -> TX
X -> +TX
X -> -TX
X -> ε
T -> FY
Y -> *FY
Y -> ε
F -> -F
F -> P
P -> n
P -> (E)
```

Построим для этой грамматики множества **FIRST** и **FOLLOW**:

| Нетерминал | FIRST       | FOLLOW              |
| ---------- |------------ | ------------------- |
| E          | `(` `-` `n` | `$` `)`             |
| X          | `ε` `+` `-` | `$` `)`             |
| T          | `(` `-` `n` | `$` `)` `+` `-`     |
| Y          | `ε` `*`     | `$` `)` `+` `-`     |
| F          | `(` `-` `n` | `$` `)` `*` `+` `-` |
| P          | `(` `n`     | `$` `)` `*` `+` `-` |

Заметим, что выполняются условия теоремы связывающие **LL(1)** грамматики с множествами **FIRST** и **FOLLOW**.

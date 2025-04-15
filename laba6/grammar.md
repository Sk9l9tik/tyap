lab3 grammar

S → I=E;
E → E'|'Y|E&Y|Y
Y → Y<K|Y>K|Y==K|K
K → K+T|K-T|T 
T → T*F|T/F|F
F → M|G(E)
M → (E)|-M|!M|I|C
G → sin|cos|sqr|sqrt
I → AK|A
K → AK|DK|A|D 
C → DC|D|.R
R → DR|D
A → a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_
D → 0|1|2|3|4|5|6|7|8|9

----

lab6 grammar
V = {0..9, a..z, _, |, &, <, >, ==, +, -, *, /, (, ), !, sin, cos, sqr, sqrt}

S` → #S#
S → I=E;

E → E'|'Y | E&Y | Y
Y → Y<K | Y>K | Y==K | K
K → K+T | K-T | T 
T → T*F | T/F | F

F → ~~==M | G(E)==~~ (E) | -F | !F | I | C sin(E) | cos(E) | sqr(E) | sqrt(E)
~~==M → (E) | -M | !M | I | C==~~
~~==G → sin | cos | sqr | sqrt==~~

I - терминал
С - терминал

----

__=__

#S S#
I= =E E;
E| |Y E& &Y
Y< <K Y> >K Y== ==K
K+ +T K- -T
T* *F T/ /F
sin( cos( sqr( sqrt( (E E) -F !F



__<__ (Вопрос, функции раскрываются вместе со скобками?)

#S          #I
=E          `=E` =Y =K =T =F =M `=sin( =cos( =sqr( =sqrt(` =( =- =! =I =C
|Y &Y       `|Y` `&Y` |K |T |F |M `|sin( |cos( |sqr( |sqrt(` |( |- |! |I |C &K &T &F &M `&sin( &cos( &sqr( &sqrt(` &( &- &! &I &C
<K >K ==K `<K` `>K` `==K` <T <F <M `<sin( <cos( <sqr( <sqrt(` <( <- <! <I <C >T >F >M `>sin( >cos( >sqr( >sqrt(` >( >- >! >I ==C ==T ==F ==M `==sin( ==cos( ==sqr( ==sqrt(` ==( ==- ==! ==I ==C
+T -T       +T -T +F +M `+sin( +cos( +sq(r +sqrt(` +( +- +! +I +C -F -M `-sin( -cos( -sqr( -sqrt(` -( -- -! -I -C
*F /F       `*F` `/F` *M `*sin( *cos( *sqr( *sqrt(` *( *- *! *I *C /M `/sin( /cos( /sqr( /sqrt(` /( /- /! /I /C
sin(E cos(E sqr(E sqrt(E (E -F !F        `sin(E cos(E sqr(E sqrt(E` sin(Y sin(K sin(T sin(F sin(M sin(sin( sin(- sin(! sin(I sin(C cos(Y cos(K cos(T cos(F cos(M cos(cos( cos(- cos(! cos(I cos(C sqr(Y sqr(K sqr(T sqr(F sqr(M sqr(sqr( sqr(- sqr(! sqr(I sqr(C sqrt(Y sqrt(K sqrt(T sqrt(F sqrt(M sqrt(sqrt( sqrt(- sqrt(! sqrt(I sqrt(C  `(E` (Y (K (T (F `(sin (cos (sqr (sqrt` (( (- (! (I (C -( -- -I -C

~~==(E -M !M        `(E` (Y (K (T (F (M `(sin (cos (sqr (sqrt` (( (- (! (I (C -( -- -I -C==~~
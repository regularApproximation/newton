<PP> ::= 1.0 <P> <NP>;
<Det> ::= 0.8 | 0.2;
<N> ::= 0.5 | 0.5;
<VP> ::= 0.1 <VP> <PP> | 0.7 <V> <NP> | 0.2 <V>;
<P> ::= 0.61 | 0.39;
<S> ::= 1.0 <NP> <VP>;
<V> ::= 0.35 | 0.65;
<NP> ::= 0.5 <Det> <N> | 0.25 <NP> <PP> | 0.1 | 0.15;

#pragma once

#include <string>
#include "../../libraries/augeas/src/fa.h"

#include "regular_language.h"

class FiniteAutomaton : public RegularLanguage<FiniteAutomaton> {
public:

    /*
     * Builds a finite automaton from a POSIX regular expression.
     * Use | for choice, () for grouping, * for Kleene star; don't use spaces,
     * also concatenation does not require a symbol.
     */
    FiniteAutomaton(std::string regularExpression) {
        fa_compile(regularExpression.c_str(), regularExpression.size(), &automaton);
    }

    bool empty() const {
        return fa_equals(automaton, empty_language) == 1;
    }

    FiniteAutomaton minimize() const {
        struct fa* automatonClone = automaton;
        fa_minimize(automatonClone);

        return FiniteAutomaton(automatonClone);
    }

    FiniteAutomaton complement() const {
        return FiniteAutomaton(fa_complement(automaton));
    }

    FiniteAutomaton intersectionWith(FiniteAutomaton other) const {
            return FiniteAutomaton(fa_intersect(automaton, other.automaton));
    }

    FiniteAutomaton unionWith(FiniteAutomaton other) const {
            return FiniteAutomaton(fa_union(automaton, other.automaton));
    }

    FiniteAutomaton concatenate(FiniteAutomaton other) const {
        return FiniteAutomaton(fa_concat(automaton, other.automaton));
    }

    FiniteAutomaton kleeneStar() const {
        return FiniteAutomaton(fa_iter(automaton, 0, -1));
    }

    std::string string() const {
        char *regularExpression;
        size_t size;
        fa_as_regexp(automaton, &regularExpression, &size);
        std::string str(regularExpression);
        return str;
    }

    static const FiniteAutomaton emptyAutomaton;
    static const FiniteAutomaton epsilonAutomaton;
    static const FiniteAutomaton universeAutomaton;
private:
    FiniteAutomaton(struct fa *FA) {
        automaton = FA;
    }

    static const struct fa* empty_language;

    const struct fa* automaton;
};
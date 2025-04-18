#include <iostream>
#include <ostream>


bool is_ident(char x) { return ((x <= 'z' && x >= 'a') || (x >= 'a' && x <= 'z') || (x == '_') || (x >= '0' && x <= '9')); }
bool is_ident_fst(char x) { return ((x <= 'z' && x >= 'a') || (x >= 'a' && x <= 'z') || (x == '_')); }
bool is_digit(char x) { return (x >= '0' && x <= '9'); }
bool is_double_digit(char x ) { return (x >= '0' && x <= '9' && x == '.'); }
bool is_op(std::string_view x) { return (x == "+" || x == "-" || x == "|" || x == "&" || x == "*" || x == "/" || x == "<" || x == ">" || x == "==" || x == "!" || x == "="); }
bool is_func(std::string_view x) { return (x == "sin" || x == "cos" || x == "sqr" || x == "sqrt"); }


struct triade_t {
    std::string op;
    std::string lo;
    std::string ro;
    int pos;
    bool m = 0;
    
    // triade_t(std::string op_, std::string lo_, std::string ro_, int pos_) : op(std::forward<std::string>(op_)), lo(std::forward<std::string>(lo_)), ro(std::forward<std::string>(ro_)), pos(std::forward<int>(pos_)) {};
    triade_t(std::string op_, std::string lo_, std::string ro_, int pos_) : op(op_), lo(lo_), ro(ro_), pos(pos_) {};

    std::string to_string() { return op + "(" + lo + "," + ro + ")"; }

    void print(){ std::cout << op << "(" << lo << "," << ro << ")";}
    void print(std::ostream& out) { out << op << "(" << lo << "," << ro << ")"; }
};

struct symbol_t{
    char type;
    char relation;
    int number;
    std::string value;
};

class Lexer {

};

class Parser{
public:

};

int main(){

    return 0;
}
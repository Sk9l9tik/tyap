#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <string_view>
#include <variant>
#include <unordered_map>
#include <vector>

#include "StringSwitch.h"

#define error(x) std::cout << "Error " << x << '\n';

/**
* @brief The structure of the triade
*/
struct triade_t {
    std::string op; ///< operation between lo and ro
    std::string lo; ///< right operand
    std::string ro; ///< left operand
   
    triade_t(std::string op_, std::string lo_, std::string ro_) : op(op_), lo(lo_), ro(ro_) {};

    std::string to_string() { return op + "(" + lo + "," + ro + ")"; }

    void print(){ std::cout << op << "(" << lo << "," << ro << ")";}
    void print(std::ostream& out) { out << op << "(" << lo << "," << ro << ")"; }
};

/**
* @brief The structure of the symbol
*/
struct symbol_t{
    std::string type; ///< terminal/netermial (e.g. S, E, I, ...)
    char relation; ///< relation between symbols (e.g. Y`<`=)
    int number; ///< triade number (1, 2, ...)
    std::string value; ///< symbol value (e.g. in triade a = 2)

    symbol_t() : type(0), relation(0), number(0), value(0) {}
    symbol_t(std::string type_, char relation_, int number_, std::string value_) : type(type_), relation(relation_), number(number_), value(value_) {}
};

using type_t = std::variant<int32_t, double>; ///< var can be int or double
std::unordered_map<std::string, type_t> sym_table;
std::vector<triade_t> triades; 

std::string to_string(type_t x){
    if (x.index() == 0) return std::to_string(std::get<0>(x));
    else return std::to_string(std::get<1>(x));
}

class Parser{

    using token_type = type_t;

    bool is_ident(char x) { return ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || (x == '_') || (x >= '0' && x <= '9')); }
    bool is_ident_fst(char x) { return ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || (x == '_')); }
    bool is_digit(char x) { return (x >= '0' && x <= '9'); }
    bool is_double_digit(char x ) { return ((x >= '0' && x <= '9') || x == '.'); }
    bool is_op(std::string_view x) { return (x == "+" || x == "-" || x == "|" || x == "&" || x == "*" || x == "/" || x == "<" || x == ">" || x == "==" || x == "!" || x == "="); }
    bool is_func(std::string_view x) { return (x == "sin" || x == "cos" || x == "sqr" || x == "sqrt"); }
    bool is_alf(char x) {return (is_ident(x) || is_op(std::to_string(x)) || is_double_digit(x) || x == ' ');}

    void get(){
        prev_symbol_ = current_symbol_;
        if (input_file_ >> std::noskipws >> current_symbol_){
            if (!is_alf(current_symbol_)){ error("Symbol is not in alpahbet!\n");}
            else if (current_symbol_ == '\n') {
                ++current_number_line_;
                posintion_ = 0;
            }
            else ++posintion_;
        }
    }
        
    void get_del(){
        prev_symbol_ = current_symbol_;
        if (input_file_ >> std::noskipws >> current_symbol_){
            while(current_symbol_ > 0 && current_symbol_ <= ' ') input_file_ >> std::noskipws >> current_symbol_;

            if (!is_alf(current_symbol_)){ error("Symbol is not in alpahbet!\n");}
            else if (current_symbol_ == '\n') {
                ++current_number_line_;
                posintion_ = 0;
            }
            else ++posintion_;
        }
        else
            return;
    }
    
    token_type get_const() {
        type_t val;
        int32_t xf = 0;
        double xl = 0.0;
        bool has_decimal = false;
        double decimal_place = 0.1;

        if (!is_double_digit(current_symbol_)) { 
            error("Is not a number!!\n");
            return 0;
        }

        current_lex_ = current_symbol_;
        while(is_digit(current_symbol_) || current_symbol_ == '.') {
            if (current_symbol_ == '.') {
                if (has_decimal) {
                    error("Multiple decimal points in number!\n");
                    break;
                }
                has_decimal = true;
                get();
                continue;
            }
            
            if (!has_decimal) {
                xf *= 10;
                xf += current_symbol_ - '0';
            } else {
                xl += (current_symbol_ - '0') * decimal_place;
                decimal_place *= 0.1;
            }
            
            get();
            current_lex_ += current_symbol_;
        }

        if (has_decimal) {
            val = static_cast<double>(xf) + xl;
        } else {
            val = xf;
        }
        
        return val;
    }

    std::string get_var(){
        std::string var;
        if (!is_ident_fst(current_symbol_)) { error("First symbol cannot be NAN!\n");}

        while(is_ident(current_symbol_)){
            var+= current_symbol_;
            get();
        }
        return var;
    }

    int index(std::string_view x){ return StringSwitch<int>(x)
        .Case("L", 0)
        .Case("S", 1)
        .Case("L", 0)
        .Case("S", 1)
        .Case("E", 2)
        .Case("Y", 3)
        .Case("K", 4)
        .Case("T", 5)
        .Case("F", 6)
        .Case("=", 7)
        .Case(";", 8)
        .Case("|", 9)
        .Case("&", 10)
        .Case("<", 11)
        .Case(">", 12)
        .Case("==", 13)
        .Case("*", 14)
        .Case("/", 15)
        .Case("+", 16)
        .Case("-", 17)
        .Case("!", 18)
        .Case("(", 19)
        .Case(")", 20)
        .Case("G", 21)
        .Case("I", 22)
        .Case("C", 23)
        .Case("#", 24)
        .Default(-1);
    }
    // int nttoidx(char c){
    //     switch(c){
    //         case 'L': return 0;
    //         case 'S': return 1;
    //         case 'E': return 2;
    //         case 'Y': return 3;
    //         case 'K': return 4;
    //         case 'T': return 5;
    //         case 'F': return 6;
    //         case '=': return 7;
    //         case ';': return 8;
    //         case '|': return 9;
    //         case '&': return 10;
    //         case '<': return 11;
    //         case '>': return 12;
    //         case '~': return 13;
    //         case '*': return 14;
    //         case '/': return 15;
    //         case '+': return 16;
    //         case '-': return 17;
    //         case '!': return 18;
    //         case '(': return 19;
    //         case ')': return 20;
    //         case 'G': return 21;
    //         case 'I': return 22;
    //         case 'C': return 23;
    //         case '#': return 24;
    //         default: return -1;
    //     }
    // }

    int get_relations(symbol_t a, symbol_t b){
        const char relations[25][25] = {
            // L     S     E     Y     K     T     F     =     ;     |     &     <     >    ==     *     /     +     -     !     (     )     G     I     C     #
            {   0,  '=',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',    0,  '='},  // L
            {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '>',    0,  '>'},  // S
            {   0,    0,    0,    0,    0,    0,    0,    0,  '=',  '=',  '=',    0,    0,    0,    0,    0,    0,    0,    0,    0,  '=',    0,    0,    0,  '>'},  // E
            {   0,    0,    0,    0,    0,    0,    0,    0,  '>',  '>',  '>',  '=',  '=',  '=',    0,    0,    0,    0,    0,    0,  '>',    0,    0,    0,    0},  // Y
            {   0,    0,    0,    0,    0,    0,    0,    0,  '>',  '>',  '>',  '>',  '>',  '>',    0,    0,  '=',  '=',    0,    0,  '>',    0,    0,    0,    0},  // K
            {   0,    0,    0,    0,    0,    0,    0,    0,  '>',  '>',  '>',  '>',  '>',  '>',  '=',  '=',  '>',  '>',    0,    0,  '>',    0,    0,    0,    0},  // T
            {   0,    0,    0,    0,    0,    0,    0,    0,  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',    0,    0,  '>',    0,    0,    0,    0},  // F
            {   0,    0, '%',  '<',  '<',  '<',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // =
            {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '>',    0,  '>'},  // ;
            {   0,    0,    0, '%',  '<',  '<',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // |
            {   0,    0,    0, '%',  '<',  '<',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // &
            {   0,    0,    0,    0, '%',  '<',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // <
            {   0,    0,    0,    0, '%',  '<',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // >
            {   0,    0,    0,    0, '%',  '<',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // ==
            {   0,    0,    0,    0,    0,    0,  '=',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // *
            {   0,    0,    0,    0,    0,    0,  '=',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // /
            {   0,    0,    0,    0,    0, '%',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',  '<',    0,  '<',  '<',  '<',    0},  // +
            {   0,    0,  '<',  '<',  '<', '%', '%',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',    0,    0,    0,    0,    0},  // -
            {   0,    0,  '<',  '<',  '<',  '<',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',    0,  '<',  '<',  '<',    0},  // !
            {   0,    0, '%',  '<',  '<',  '<',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',  '<',    0,  '<',  '<',  '<',    0},  // (
            {   0,    0,    0,    0,    0,    0,    0,    0,  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',    0,    0,  '>',    0,    0,    0,    0},  // )
            {   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '=',    0,    0,    0,    0,    0},  // G
            {   0,    0,    0,    0,    0,    0,    0,  '=',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',    0,  '>',    0,    0,    0,    0},  // I
            {   0,    0,    0,    0,    0,    0,    0,    0,  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',  '>',    0,  '>',    0,    0,    0,    0},  // C
            {'%',  '<',    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  '<',    0,    0}   // #
        };

        char res;
        int x = index(a.value);
        int y = index(b.value);

        if (x < 0 || y < 0) res = 0;
        else res = relations[x][y];

        return (!res) ? '0' : res; 
    }

    symbol_t get_smbol(){
        symbol_t res;

        if (is_double_digit(current_symbol_)){
            res.value = to_string(get_const());
            res.type = "C";
            res.number = triade_count++;
            triades.emplace_back("C", res.value, "0");
        }
        else if (is_ident_fst(current_symbol_)){
            res.value = get_var();
            res.type = "V";
            res.number = triade_count++;
            triades.emplace_back("V", res.value, "0");
        }
        else if (current_symbol_ == EOF){
            res.type = "#";
            res.value = "0";
        }
        else{
            res.type = std::string(1, current_symbol_);
            if (current_symbol_ == '=') {
                get();
                if (current_symbol_ == '=') {
                    res.type = "==";
                    get();
                }
            }
            res.value = "0";
            get();

        }
        return res;
    }

    void shift(){

    }   
    
    void reduce(){

    }

public:
    Parser(const std::string& input) : input_(input), input_size_(input.size()), posintion_(0), current_number_line_(0) {}
    Parser(const std::string_view& input_file, const std::string_view& output_file) : posintion_(0), current_number_line_(0), file_name_(input_file) {           
        input_file_.open(input_file.data());
        output_file_.open(output_file.data());

        if (!input_file_.is_open()) { throw std::runtime_error("Can't open in file!\n"); }
        else if (!output_file_.is_open()) { throw std::runtime_error("Can't open out file!\n"); }
        // std::string s;
        // while(std::getline(input_file_, s)){
        //     input_ += s + '\n';
        // }
        // input_size_ = input_.size();
    }
    Parser() = delete;
    Parser(Parser& other) = delete;
    Parser(Parser&& other) = delete;
    ~Parser(){
        if (input_file_.is_open()) input_file_.close();
        if (output_file_.is_open()) output_file_.close();
    }

private:
    std::stack<symbol_t> sym_stack_;

    std::string input_;
    std::ifstream input_file_;
    std::ofstream output_file_;
    std::string file_name_;
    
    size_t input_size_;
    size_t posintion_; ///< The position of symbol in the string.
    char current_symbol_; ///< The symbol of the current reading.
    char prev_symbol_;    ///< The symbol of the previous reafding.
    std::string current_lex_; ///< The lexem of the currend reading.
    size_t current_number_line_; ///< The currnet file line number of the file.

    int triade_count = 0;
};

int main(int argc, char** argv){

    if (argc != 3) {std::cerr << "Error input!\n"; return 3;}

    Parser parser(argv[1], argv[2]);

    // std::cout << parser.nttoidx('~');

    return 0;
}
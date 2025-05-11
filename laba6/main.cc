#include <algorithm>
#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <string_view>
#include <variant>
#include <unordered_map>
#include <vector>
 
#include "StringSwitch.h"
 
#define error(x) std::cout << "Error " << x << '\n'; abort();
 
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
 
    symbol_t() : type(""), relation(0), number(0), value("") {}
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
    bool is_alf(char x) {
        return (is_ident(x) || is_double_digit(x) || x == ' ' || x == '=' || x == ';' || x == '+' || x == '-' || x == '*' || x == '/' || x == '<' || x == '>' || x == '!' || x == '(' || x == ')');
    }
 
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
 
            if (!is_alf(current_symbol_)){ 
                error("Symbol is not in alphabet: " + std::string(1, current_symbol_) + "\n");
            }
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
 
    int index(std::string_view x){ 
        return StringSwitch<int>(x)
        .Case("#", 0)
        .Case("L", 1)
        .Case("S", 2)
        .Case("I", 3)
        .Case("=", 4)
        .Case("E", 5)
        .Case(";", 6)
        .Case("-", 7)
        .Case("+", 8)
        .Case("F", 9)
        .Case("(", 10)
        .Case(")", 11)
        .Case("G", 12)
        .Case("C", 13)
        .Default(-1);
    }
 
    char get_relations(symbol_t a, symbol_t b){
        const char relations[14][14] = {
            //#	   L	S	 I	  =	   E	;	 +	  -	   F	(	 )	  G	   C 
            {'0', '<', '0', '<', '0', '0', '0', '0', '0', '0', '0', '0', '0', '<'},
            {'=', '0', '<', '<', '0', '0', '>', '0', '0', '0', '0', '>', '0', '<'},
            {'>', '0', '0', '0', '0', '0', '>', '0', '0', '0', '0', '>', '0', '0'},
            {'0', '0', '0', '0', '=', '0', '>', '>', '>', '0', '0', '>', '0', '0'},
            {'0', '0', '0', '0', '0', '<', '0', '<', '<', '<', '<', '0', '<', '<'},
            {'0', '0', '0', '0', '0', '0', '=', '=', '=', '0', '0', '>', '0', '0'},
            {'>', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '<', '0', '>', '>', '<', '<', '>', '<', '<'},
            {'0', '0', '0', '0', '0', '<', '0', '>', '>', '<', '<', '>', '<', '<'},
            {'0', '0', '0', '0', '0', '0', '>', '>', '>', '0', '0', '>', '0', '0'},
            {'0', '0', '0', '0', '0', '<', '0', '<', '<', '<', '<', '=', '<', '<'},
            {'0', '0', '0', '0', '0', '0', '>', '>', '>', '0', '0', '>', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '<', '0', '0', '0'},
            {'0', '0', '0', '0', '0', '0', '>', '>', '>', '0', '0', '>', '0', '0'}
        };

        char res;
        int x = index(a.type);
        int y = index(b.type);

        if (x < 0 || y < 0) {
            res = 0;
        }
        else {
            res = relations[x][y];
        }

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
            std::string name = get_var();
            if (is_func(name)) {
                res.type = "G";
                res.value = name;
                res.number = triade_count++;
                triades.emplace_back(name, "0", "0");
            } else {
                res.type = "I";
                res.value = name;
                res.number = triade_count++;
                triades.emplace_back("V", res.value, "0");
            }
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
            get_del();
 
        }
        return res;
    }
 
    void shift(symbol_t s){
        sym_stack_.push(s);
    }   
 
    void reduce() {
        std::string rule = "";
        symbol_t bufferI, bufferC, bufferE, bufferF, bufferS, bufferL, bufferG;
        symbol_t new_symbol;

        while (sym_stack_.top().relation == '=') {
            rule = sym_stack_.top().type + rule;
            if (sym_stack_.top().type == "I") bufferI = sym_stack_.top();
            if (sym_stack_.top().type == "C") bufferC = sym_stack_.top();
            if (sym_stack_.top().type == "E") bufferE = sym_stack_.top();
            if (sym_stack_.top().type == "F") bufferF = sym_stack_.top();
            if (sym_stack_.top().type == "S") bufferS = sym_stack_.top();
            if (sym_stack_.top().type == "L") bufferL = sym_stack_.top();
            if (sym_stack_.top().type == "G") bufferG = sym_stack_.top();
            sym_stack_.pop();
        }
        rule = sym_stack_.top().type + rule;
        if (sym_stack_.top().type == "I") bufferI = sym_stack_.top();
        if (sym_stack_.top().type == "C") bufferC = sym_stack_.top();
        if (sym_stack_.top().type == "E") bufferE = sym_stack_.top();
        if (sym_stack_.top().type == "F") bufferF = sym_stack_.top();
        if (sym_stack_.top().type == "S") bufferS = sym_stack_.top();
        if (sym_stack_.top().type == "L") bufferL = sym_stack_.top();
        if (sym_stack_.top().type == "G") bufferG = sym_stack_.top();
        sym_stack_.pop();

        if (rule == "I" || rule == "C") {
            new_symbol.type = "F";
            if (rule == "I") {
                if (sym_table.find(bufferI.value) != sym_table.end()) {
                    new_symbol.value = bufferI.value;
                    new_symbol.number = bufferI.number;
                } else {
                    triades.pop_back();
                    error("Undefined variable: " + bufferI.value);
                }
            } else {
                new_symbol.value = bufferC.value;
                new_symbol.number = bufferC.number;
            }
        } else if (rule == "I=E;") {
            new_symbol.type = "S";
            new_symbol.number = triade_count++;
            if (sym_table.find(bufferI.value) == sym_table.end()) {
                sym_table[bufferI.value] = 0;
            }
            triades.emplace_back("=", 
                               "^" + std::to_string(bufferI.number),
                               "^" + std::to_string(bufferE.number));
        } else if (rule == "E+F") {
            new_symbol.type = "E";
            new_symbol.number = triade_count++;
            triades.emplace_back("+",
                               "^" + std::to_string(bufferE.number),
                               "^" + std::to_string(bufferF.number));
        } else if (rule == "E-F") {
            new_symbol.type = "E";
            new_symbol.number = triade_count++;
            triades.emplace_back("-",
                               "^" + std::to_string(bufferE.number),
                               "^" + std::to_string(bufferF.number));
        } else if (rule == "(E)") {
            new_symbol.type = "F";
            new_symbol.number = bufferE.number;
        } else if (rule == "G(E)") {
            new_symbol.type = "F";
            new_symbol.number = triade_count++;
            triades.emplace_back(bufferG.value,
                               "^" + std::to_string(bufferE.number),
                               "0");
        } else if (rule == "-F") {
            new_symbol.type = "F";
            new_symbol.number = triade_count++;
            triades.emplace_back("-",
                               "^" + std::to_string(bufferF.number),
                               "0");
        } else if (rule == "S") {
            new_symbol.type = "L";
            new_symbol.number = bufferS.number;
        } else if (rule == "LS") {
            new_symbol.type = "L";
            new_symbol.number = bufferL.number;
        } else if (rule == "F") {
            new_symbol.type = "E";
            new_symbol.number = bufferF.number;
        } else {
            error("Unknown rule: " + rule);
        }

        new_symbol.relation = get_relations(sym_stack_.top(), new_symbol);
        std::cout << "reduce <" << rule << "> to <" << new_symbol.type 
                 << "> triade: " << new_symbol.number << "\n";
        sym_stack_.push(new_symbol);
    }
 
    void print_tiades(){
        for(auto& a : triades){
            if (!a.op.empty())
                output_file_ << a.op << "(" << a.lo << "," << a.ro << ")\n";
        }
    }
 
public:
    Parser(const std::string& input) : input_(input), input_size_(input.size()), posintion_(0), current_number_line_(0) {}
    Parser(const std::string_view& input_file, const std::string_view& output_file)
    :  file_name_(input_file), posintion_(0), current_number_line_(1), triade_count(1) {           
        input_file_.open(input_file.data());
        output_file_.open(output_file.data());
 
        if (!input_file_.is_open()) { throw std::runtime_error("Can't open in file!\n"); }
        else if (!output_file_.is_open()) { throw std::runtime_error("Can't open out file!\n"); }
 
        triades.emplace_back("", "", "");
        sym_stack_.emplace("#", 0, 0, "0");
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
 
    void parse(){
        symbol_t new_symbol{};

        get_del();

        new_symbol = get_smbol();
        while(true){
            if (new_symbol.type == "#" || sym_stack_.top().type == "L"){
                std::cout << "End parse\n";
                break;
            }

            new_symbol.relation = get_relations(sym_stack_.top(), new_symbol);

            if (new_symbol.relation == '<' || new_symbol.relation == '=' || new_symbol.relation == '%'){
                shift(new_symbol);
                std::cout << "shift <" << new_symbol.type << ">";
                if (new_symbol.type == "C" || new_symbol.type == "I"){
                    std::cout << " triade N:" << new_symbol.number;
                }                
                std::cout << "\n";

                if(current_symbol_ == ' ') get_del();

                new_symbol = get_smbol();
            }
            else if (new_symbol.relation == '>'){
                reduce();
            }
            else {
                auto s = sym_stack_;
                while(!s.empty()){
                    std::cout << s.top().type << " ";
                    s.pop();
                }
                std::cout << "\n";
                error("Wrong relation!\n");
                break;
            }
        }

        print_tiades();
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
 
    parser.parse();
 
    return 0;
}
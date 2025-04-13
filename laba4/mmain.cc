#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <cmath>
#include <fstream>

#define DEBUG
#undef DEBUG


#include "Log.hh"

// S → I=E;
// E → E<K|E>K|E==K|T
// K → K+T|K-T|T 
// T → T*F|T/F|F
// F → M|G(E)
// M → (E)|-M|!M|I|C
// G → sin|cos|sqr|sqrt
// I → AK|A
// K → AK|DK|A|D 
// C → DC|D|.R
// R → DR|D
// A → a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_
// D → 0|1|2|3|4|5|6|7|8|9

#define IS_IDENT_FIRST_SYMBOL(x) (((x) >= 'a' && (x) <= 'z') || (x) == '_')
#define IS_IDENT(x) (((x) >= 'a' && (x) <= 'z') || (x) == '_' || ((x) >= '0' && (x) <= '9'))

std::unordered_map<std::string, double> VarTable;
std::vector<std::pair<std::string, double>> Ops;
std::vector<std::string> funcs;

class Parser {
public:
    Parser(const std::string& input) : input_(input), input_size_(input.size()), posintion_(0), current_number_line_(0), t_count_(0), file_(0) {}

    Parser(const std::string_view& input_file, const std::string_view& output_file) : file_(1), posintion_(0), current_number_line_(0), file_name_(input_file), t_count_(0) {
        
        input_file_.open(input_file.data());
        output_file_.open(output_file.data());

        if (!input_file_.is_open()) { throw std::runtime_error("Can't open in file!\n"); }
        else if (!output_file_.is_open()) { throw std::runtime_error("Can't open out file!\n"); }
        std::string s;
        while(std::getline(input_file_, s)){
            input_ += s + '\n';
        }
        input_size_ = input_.size();
    }

    Parser() = delete;
    Parser(Parser& other) = delete;
    Parser(Parser&& other) = delete;
    
    ~Parser(){
        input_file_.close();
        output_file_.close();
    }

    void Parse(){
        LOG_TRACE        
                // size_t i = 0;
        
                if (file_)
                    output_file_ << "START PARSE\n";
        
                std::cout << "START PARSE\n";
        
        
                get_next_token_del();
        
                while(posintion_ <= input_size_ && current_symbol_ != ' '){
                    if (current_symbol_ == '\n')
                        ++current_number_line_;
                    while(current_symbol_ >= 0 && current_symbol_ <= ' ')
                        get_next_token();
                    
                    if (current_symbol_ == EOF)
                        return;
                    
                    std::string var;
                    try{
                        var = ProcS();
                    }
                    catch(const std::logic_error& e){
                        return;
                    }
    
                    get_next_token_del();
                }
                if (file_){
                    output_file_ << "END PARSE\n";
                }
                std::cout << "END PARSE\n ";
            }
private:

    int print_triade(std::string op, std::string op1, std::string op2 = ""){
        if (file_){
            if (op2 == "")
                output_file_ << ++t_count_ << ": " << op << '(' << op1 << ',' << "null" << ")\n";
            else
                output_file_ << ++t_count_ << ": " << op << '(' << op1 << ',' << op2 << ")\n";
            return t_count_;
        }
        else{
            if (op2 == "")
                std::cout << ++t_count_ << ": " << op << '(' << op1 << ',' << "null" << ")\n";
            else
                std::cout << ++t_count_ << ": " << op << '(' << op1 << ',' << op2 << ")\n";
            return t_count_;
        }
    }

    void error(const std::string& error_name, const std::string& param = "", const std::string& param2 = "") {
        if (file_){
            if (param.empty())
                output_file_ << file_name_ << ":" << current_number_line_ << ":" << posintion_ << ": \e[1;31merror:\e[0m " << error_name << "\n";
            else if (!param.empty()){
                output_file_ << file_name_ << ":" << current_number_line_ << ":" << posintion_ << ": \e[1;31merror:\e[0m " << error_name << " '"<< param << "'\n";
            }
            else if (!param.empty() && !param2.empty()){
                output_file_ << file_name_ << ":" << current_number_line_ << ":" << posintion_ << ": \e[1;31merror:\e[0m " << error_name << " '"<< param << "'\n" << " '" << param2 << "'\n";
            }
            output_file_ << "END PARSE\n";
        }
        else{
            if (param.empty())
                std::cout << __FILE__ << ":" << current_number_line_ << ":" << posintion_ << ": \e[1;31merror:\e[0m " << error_name << "\n";
            else if (!param.empty()){
                std::cout << __FILE__ << ":" << current_number_line_ << ":" << posintion_ << ": \e[1;31merror:\e[0m " << error_name << " '"<< param << "'\n";
            }
            else if (!param.empty() && !param2.empty()){
                std::cout << __FILE__ << ":" << current_number_line_ << ":" << posintion_ << ": \e[1;31merror:\e[0m " << error_name << " '"<< param << "'\n" << " '" << param2 << "'\n";
            }
        }
        std::cout << "END PARSE\n";
        throw std::logic_error("");
    }


    void print_all() {
        if (file_){
            auto& out{output_file_};
            out << "Count vars: " << Ops.size() << '\n';

            std::for_each(Ops.begin(), Ops.end(), [&out](const auto& el){
                out << el.first << " = " << el.second << '\n';
            });
        }
        else{
            std::cout << "Count vars: " << Ops.size() << '\n';
            std::for_each(Ops.begin(), Ops.end(), [](const auto& el){
                std::cout << el.first << " = " << el.second << '\n';
            });
        }
    }

    void print_var(const std::string& var_name) {
        std::cout << var_name << " = " << get_value(var_name) << '\n';
    }

    double get_value(const std::string& var_name) {
        if (VarTable.find(var_name) != VarTable.end()){
            return VarTable[var_name];
        }
        else {
            error("Use of undeclared identifier", var_name);
            return -1;
        }
    }

    int ProcI(){
LOG_TRACE
        std::string var;
        while(IS_IDENT(current_symbol_)){
            var+= current_symbol_;
            get_next_token();
        }
        return print_triade("V", var);
        // return get_value(var);
    }

    double ProcC(){
LOG_TRACE
        double xf = 0;
        double xl = 0;
        while((current_symbol_ >= '0' && current_symbol_ <= '9')){
            xf *= 10;
            xf += current_symbol_ - '0';
            get_next_token();
        }
        if (current_symbol_ == '.'){
            get_next_token();
            size_t l = 1;
            while((current_symbol_ >= '0' && current_symbol_ <= '9')){
                l*=10;
                xl += current_symbol_ -'0';
                get_next_token();
            }
            if (current_symbol_ > 0 && current_symbol_ <= ' ')
            xl /= l;
        }
        if (xl == 0){
            if (current_symbol_ > 0 && current_symbol_ <= ' ')
                get_next_token_del();
            return xf;        
        }
        else{
            return (xf + xl);
        }
        
    }

    int ProcM(){
LOG_TRACE
        int x;
        if (current_symbol_ == '('){
            get_next_token_del();
            x = ProcE();
            if (current_symbol_ != ')')
                error("Missing", ")");
            get_next_token_del();
        }
        else {
            if (current_symbol_ == '-'){
                get_next_token_del();
                x = ProcM();
                print_triade("-", "^" + std::to_string(x));
            }
            else if (current_symbol_ == '!'){
                get_next_token_del();
                // x = !static_cast<bool>(ProcM());
                x = ProcM();
                print_triade("!", "^" + std::to_string(x));
            }
            else{
                if (current_symbol_ >= '0' && current_symbol_ <= '9'){
                    x = ProcC();
                    x = print_triade("C", std::to_string(x));
                }
                else{
                    if ((current_symbol_ >= 'a' && current_symbol_ <= 'z') || current_symbol_ == '_')
                        x = ProcI();
                    else{
                        error("Syntax error", ""); 
                    }
                }
            }
        }
        
        return x;
    }

    int ProcF(){
LOG_TRACE
        int x;
        std::string s;
        if (posintion_ + 3 < input_size_ && \
       current_symbol_ == 's' && input_[posintion_] == 'q' && input_[posintion_ + 1] == 'r' && input_[posintion_ + 2] == 't'){
            s = ""; 
            for(int i = 0; i <= 3; ++i){
                s+= current_symbol_;
                get_next_token_del();
            }
            if (s == "sqrt"){
                funcs.push_back(s);
                if (current_symbol_ == '('){
                    get_next_token_del();   
                    x = ProcE();
                }
                else {
                    error("Syntax error, missing: ", "(");
                }
                if (current_symbol_  != ')'){
                    error("Syntax error, missing: ", ")");
                }
                s = funcs.back();
                funcs.pop_back();
                x = print_triade("sqrt", "^" + std::to_string(x));//std::sqrt(x);
                get_next_token_del();
            }
            else{
                x = ProcM();
            }
        }
        else if (posintion_ + 2 < input_size_ ){
            s = ""; 
            s += current_symbol_;
            for(int i = 0; i < 2; ++i)
                s += input_[posintion_ + i];

            if (s == "sin" || s == "cos" || s == "sqr"){
                for(int i = 0; i < 3; ++i) get_next_token();
                funcs.push_back(s);

                if (current_symbol_ == '('){
                    get_next_token();
                    x = ProcE();
                }
                else {
                    error("Syntax error, missing: ", "(", std::to_string(current_symbol_));
                }
                if (current_symbol_  != ')'){
                    error("Syntax error, missing: ", ")");
                }

                s = funcs.back();
                funcs.pop_back();
                if (s == "sin") x = print_triade("sin", "^" + std::to_string(x));//sin(x);
                else if (s == "cos") x = print_triade("cos", "^" + std::to_string(x));//cos(x);
                else x = print_triade("sqr", "^" + std::to_string(x));//x * x;
                get_next_token_del();
            }
            else{
                x = ProcM();
            }
        }
        else{
            x = ProcM();
        }
        
        return x;
    }

    int ProcT(){
LOG_TRACE
        int lx = ProcF();
        while(current_symbol_ == '*' || current_symbol_ == '/'){
            char op = current_symbol_;
            get_next_token_del();
            if (op == '*'){
                int rx = ProcT();
                lx = print_triade("*", "^" + std::to_string(lx), "^" + std::to_string(rx));
            }
            else{
                int rx = ProcT();
                lx = print_triade("/", "^" + std::to_string(lx), "^" + std::to_string(rx));
            }
        }
        
        return lx;
    }

    int ProcK(){
        int lx = ProcT();
        while(current_symbol_ == '+' || current_symbol_ == '-'){
            char op = current_symbol_;
            get_next_token();
            if (op == '+'){
                int rx = ProcT();
                lx = print_triade("+", "^" + std::to_string(lx), "^" + std::to_string(rx));
            }
            else{
                int rx = ProcT();
                lx = print_triade("-", "^" + std::to_string(lx), "^" + std::to_string(rx));
            }
        }
        
        return lx;
    }

    int ProcE(){
LOG_TRACE
        int lx = ProcK();
        while(current_symbol_ == '<' || current_symbol_ == '>' || (current_symbol_ == '=')){
            char op = current_symbol_;
            get_next_token();
            if (op == '<'){
                int rx = ProcK();
                lx = print_triade("<", "^" + std::to_string(lx), "^" + std::to_string(rx));
            }
            else if (op == '>'){
                int rx = ProcK();
                lx = print_triade(">", "^" + std::to_string(lx), "^" + std::to_string(rx));
            }
            else{
                if (current_symbol_ == '='){
                    get_next_token_del();
                    int rx = ProcK();
                    lx = print_triade("==", "^" + std::to_string(lx), "^" + std::to_string(rx));
                    // x = (fabs(x - ProcK()) < 1e-9);
                }
                else
                    error("Is not assingable");
            }
        }
        
        return lx;
    }

    std::string ProcIL(){
LOG_TRACE //TODO: Сделать проверку на зарезервированные ключевые слова: sin, .... v
        std::string var;
        if (!IS_IDENT_FIRST_SYMBOL(current_symbol_))
            error("Expression is not assignable", std::to_string(current_symbol_ - '0'));
        while(IS_IDENT(current_symbol_)){
            var += current_symbol_;
            get_next_token();
        }
        if (var == "sin" || var == "cos" || var == "sqr" || var == "sqrt")
            error("Expected unqualified-id");
        
        if (current_symbol_ > 0 && current_symbol_ <= ' ')
            get_next_token();
        return var;
    }
    
    std::string ProcS() {
LOG_TRACE
    std::string left = ProcIL();   
    if (current_symbol_ != '=')
        error("Missing", "=");

    std::string var = "^" + std::to_string(print_triade("V", left));
    get_next_token_del();

    int right = ProcE();

    if (current_symbol_ != ';')
        error("Missing", ";");

    print_triade("=", var , "^" + std::to_string(right));

    VarTable[left] = right;

    return left;
}

    void get_next_token(){
LOG_TRACE
        if (posintion_ < input_size_) {
            // while(input_[posintion_] > 0 && input_[posintion_] <= ' ') // ignore spaces
            //     ++posintion_;
            current_symbol_ = input_[posintion_++];

            #ifdef DEBUG
            std::cout << current_symbol_ << '\n';
            #endif
        }
    }

    void get_next_token_del(){
LOG_TRACE
        if (posintion_ < input_size_) {
            while(input_[posintion_] > 0 && input_[posintion_] <= ' ') // ignore spaces
                ++posintion_;
            current_symbol_ = input_[posintion_++];
            #ifdef DEBUG
            std::cout << current_symbol_  << ' ' << posintion_ << '\n';
            #endif
        }
        else {
            current_symbol_ = ' ';
        }
    }
            
    std::string input_;
    std::ifstream input_file_;
    std::ofstream output_file_;
    std::string file_name_;

    size_t input_size_;
    size_t posintion_;
    char current_symbol_;
    size_t current_number_line_;
    const bool file_;
    size_t t_count_;
};

int main(int argc, char** argv){
LOG_TRACE 
    if (argc != 3) {std::cerr << "Error input!\n"; return 3;}

    Parser Parser(argv[1], argv[2]);

    //Parser Parser("./tests/br-expr.txt", "out.txt");
    //Parser Parser("c=5;c=10;c=c+2;");
    //Parser Parser("cc=191+2147483647/65536*(-5*-5+40000000000000000000000000);");
    //Parser Parser("a=-(2147483647-214748364*10);");
    //Parser Parser("a=sqrt(sqrt(sqrt(sqrt(sqrt(sqrt(2))))));");
    //Parser Parser("a=sin(sin(sin(sin(sin(sin(2 | (5 + 1)))))));");
    //Parser Parser("f=2;z=2;b=-((f-z)*10);");
    //Parser Parser("z=214748364;f=2147483647;b=1; c=2;d=c*-2;z=81/9/3;b=-(f-z*10);c=c+1;c=c+1;c=c+1;c=c+1;c=c+1;c=c+1;cc=c+f/65536*(d*d+b);abc=cc/100;f=cc-100*abc;z=-z;c=c-c;");
    //Parser Parser("a=sin;");
    Parser.Parse();
    return 0;
}

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

#define DEBUG
#undef DEBUG


#include "Log.hh"

// S → I=E;

// E → E+T|E-T|T
// T → T*F|T/F|F
// F → M|G(E)
// M → (E)|-M|I|C
// G → sin|cos|sqr|sqrt

// I → AK|A
// K → AK|DK|A|D 

// C → DC|D|.R
// R → DR|D

// A → a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_

// D → 0|1|2|3|4|5|6|7|8|9

#define IS_IDENT(x) (((x) >= 'a' && (x) <= 'z') || (x) == '_' || ((x) >= '0' && (x) <= '9'))
#define IS_IDENT_FIRST_SYMBOL(x) (((x) >= 'a' && (x) <= 'z') || (x) == '_')

std::unordered_map<std::string, double> VarTable;

std::vector<std::pair<std::string, double>> Ops;

std::vector<std::string> funcs;

//TODO: Add error line and error symbol
class Parser {
public:
    Parser(const std::string& input) : input_(input), input_size_(input.size()), posintion_(0), current_number_line_(0), t_count_(0) {}

    Parser(std::istream& input) : input_size_(0), posintion_(0), current_number_line_(0) {
        std::string line;
        while (std::getline(input, line)) {
            input_ += line + '\n';
        }
        input_size_ = input_.size();
    }

    Parser() = delete;
    Parser(Parser& other) = delete;
    Parser(Parser&& other) = delete;
    
    ~Parser() = default;

    void Parse(){
LOG_TRACE        
        std::cout << "START PARSE\n";
        size_t i = 0;

        get_next_token();
        while(posintion_ < input_size_){
            if (current_symbol_ == '\n')
                ++current_number_line_;
            while (current_symbol_ >= 0 && current_symbol_ <= ' ')
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

            // std::cout << "Operator " << i++ << ": ";
            
            // const auto it = std::find_if(Ops.begin(), Ops.end(), 
            // [&var](std::pair<std::string, double> el){return el.first == var;});
            // if (it == Ops.end())
            //     Ops.emplace_back(var, get_value(var));
            // else {
            //     it->second = get_value(var);
            // }

            // print_var(var);
            get_next_token();
        }
        
        std::cout << "END PARSE\n";
        print_all();
        
    }
    
private:
    int print_triade(std::string op, std::string op1, std::string op2 = ""){
        if (op2 == "")
            std::cout << ++t_count_ << ": " << op << '(' << op1 << ',' << "null" << ")\n";
        else
            std::cout << ++t_count_ << ": " << op << '(' << op1 << ',' << op2 << ")\n";
        return t_count_;
    }

    void error(const std::string& error_name, const std::string& param = "") const {
        
        if (param.empty())
            printf("%s:%ld:%ld: \e[1;31merror:\e[0m %s\n", __FILE__, current_number_line_, posintion_, error_name.c_str());
        else if (!param.empty()){
            printf("%s:%ld:%ld: \e[1;31merror:\e[0m %s \'%s\'\n", __FILE__, current_number_line_, posintion_, error_name.c_str(), param.c_str());
        }
        throw std::logic_error("");
    }

    void print_all()const noexcept {
        std::cout << "Count vars: " << Ops.size() << '\n';
        std::for_each(Ops.begin(), Ops.end(), [](const auto& el){
            std::cout << el.first << " = " << el.second << '\n'; 
        });
    }

    void print_var(const std::string& var_name) const {
        std::cout << var_name << " = " << get_value(var_name) << '\n';
    }

    double get_value(const std::string& var_name) const {
        if (VarTable.find(var_name) != VarTable.end()){
            return VarTable[var_name];
        }
        else {
            error("use of undeclared identifier", var_name);
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
            xl /= l;
        }
        return (xf + xl);
    }

    int ProcM(){
LOG_TRACE
        int x;
        if (current_symbol_ == '('){
            get_next_token();
            x = ProcE();
            if (current_symbol_ != ')')
                error("Missing", ")");
            get_next_token();
        }
        else {
            if (current_symbol_ == '-'){
                get_next_token();
                x = ProcM();
                print_triade("-", "^" + std::to_string(x));
            }
            else{
                if (current_symbol_ >= '0' && current_symbol_ <= '9'){
                    x = ProcC();
                    return print_triade("C", std::to_string(x));
                }
                else{
                    if ((current_symbol_ >= 'a' && current_symbol_ <= 'z') || current_symbol_ == '_'){
                        return ProcI();
                    }
                    else{
                        error("Syntax error !!!", ""); 
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
                get_next_token();
            }
            std::cout << s << "=\n";
            if (s == "sqrt"){
                funcs.push_back(s);
                get_next_token();
                x = ProcE();
                s = funcs.back();
                funcs.pop_back();
                x = print_triade("sqrt", "^" + std::to_string(x));//std::sqrt(x);
                get_next_token();
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
                get_next_token();
                get_next_token();
                get_next_token();
                get_next_token();
                funcs.push_back(s);
                x = ProcE();
                s = funcs.back();
                funcs.pop_back();
                if (s == "sin") x = print_triade("sin", "^" + std::to_string(x));//sin(x);
                else if (s == "cos") x = print_triade("cos", "^" + std::to_string(x));//cos(x);
                else x = print_triade("sqr", "^" + std::to_string(x));//x * x;
                get_next_token();
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
            get_next_token();
            if (op == '*'){
                int rx = ProcF();
                return print_triade("*", "^" + std::to_string(lx), "^" + std::to_string(rx));
            }
            else{
                int rx = ProcF();
                return print_triade("/", "^" + std::to_string(lx), "^" + std::to_string(rx));
            }
        }
        
        return lx;
    }

    int ProcE(){
LOG_TRACE
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
        return var;
    }
    
    std::string ProcS() {
LOG_TRACE
        std::string left = ProcIL();
        if (current_symbol_ != '=')
            error("Missing", "=");
        
        std::string var = "^" + std::to_string(print_triade("V", left));
        get_next_token();
    
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
            current_symbol_ = input_[posintion_++];
            #ifdef DEBUG
            std::cout << current_symbol_ << '\n';
            #endif
        }
    }

    std::string input_;
    size_t input_size_;
    size_t posintion_;
    char current_symbol_;
    size_t current_number_line_;
    size_t t_count_;
};

int main(){
LOG_TRACE 

    // Parser Parser("a=2;b=3;c=(2+3)*5-2;d=(c-2)+((((c+3))));");
    //cc=c+f/65536*(d*d+b)
    // Parser Parser("cc=191+2147483647/65536*(-5*-5+40000000000000000000000000);");
    // Parser Parser("a=-(2147483647-214748364*10);");
    // Parser Parser("a=sqrt(sqrt(sqrt(sqrt(sqrt(sqrt(2))))));");
    // Parser Parser("a=sin(sin(sin(sin(sin(sin(2))))));");
    // Parser Parser("f=2;z=2;b=-((f-z)*10);");
    // Parser Parser("z=214748364;f=2147483647;b=1;c=2;d=c*-2;z=81/9/3;b=-(f-z*10);c=c+1;c=c+1;c=c+1;c=c+1;c=c+1;c=c+1;cc=c+f/65536*(d*d+b);abc=cc/100;f=cc-100*abc;z=-z;c=c-c;");
    Parser Parser("a=1;b=2+3+4+5+6+7+8+9+0;");
    Parser.Parse();
    return 0;
}

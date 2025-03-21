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

//TODO: Add error line and error symbol v
class Parser {
public:
    Parser(const std::string& input) : input_(input), input_size_(input.size()), posintion_(0), current_number_line_(0), current_line_("") {}

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
                std::cout << "START PARSE\n";
                var = ProcS();
            }
            catch(const std::logic_error& e){
                return;
            }

            std::cout << "Operator " << i++ << ": ";
            
            const auto it = std::find_if(Ops.begin(), Ops.end(), 
            [&var](std::pair<std::string, double> el){return el.first == var;});
            if (it == Ops.end())
                Ops.emplace_back(var, get_value(var));
            else {
                it->second = get_value(var);
            }

            print_var(var);
            get_next_token();
        }
        
        std::cout << "END PARSE\n";
        print_all();
        
    }
    
private:
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
            error("Use of undeclared identifier", var_name);
            return -1;
        }
    }

    double ProcI(){
LOG_TRACE
        std::string var;
        while(IS_IDENT(current_symbol_)){
            var+= current_symbol_;
            get_next_token();
        }
        
        return get_value(var);
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

    double ProcM(){
LOG_TRACE
        double x;
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
                x = -ProcM();
            }
            else if (current_symbol_ == '!'){
                get_next_token();
                x = !static_cast<bool>(ProcM());
            }
            else{
                if (current_symbol_ >= '0' && current_symbol_ <= '9'){
                    x = ProcC();
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

    double ProcF(){
LOG_TRACE
        double x;
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
                x = std::sqrt(x);
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
                //get_next_token();
                x = ProcE();
                s = funcs.back();
                funcs.pop_back();
                if (s == "sin") x = sin(x);
                else if (s == "cos") x = cos(x);
                else x = x * x;
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

    double ProcT(){
LOG_TRACE
        double x = ProcF();
        while(current_symbol_ == '*' || current_symbol_ == '/'){
            char op = current_symbol_;
            get_next_token();
            if (op == '*')
                x *= ProcF();
            else
                x /= ProcF();
        }
        
        return x;
    }

    double ProcK(){
        double x = ProcT();
        while(current_symbol_ == '+' || current_symbol_ == '-'){
            char op = current_symbol_;
            get_next_token();
            if (op == '+')
                x += ProcT();
            else
                x -= ProcT();
        }
        
        return x;
    }

    double ProcE(){
LOG_TRACE
        double x = ProcK();
        while(current_symbol_ == '<' || current_symbol_ == '>' || (current_symbol_ == '=')){
            char op = current_symbol_;
            get_next_token();
            if (op == '<')
                x = x < ProcK();
            else if (op == '>')
                x = x > ProcK();
            else{
                if (current_symbol_ == '=' && op == '='){
                    get_next_token();
                    x = (fabs(x - ProcK()) < 1e-9);
                }
                else
                    error("Is not assingable");
            }
        }
        
        return x;
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
        
        get_next_token();
    
        double right = ProcE();
        
        if (current_symbol_ != ';')
            error("Missing", ";");
        
        VarTable[left] = right;

        return left;
    }

    void get_next_token(){
LOG_TRACE
        if (posintion_ < input_size_) {
            while(input_[posintion_] > 0 && input_[posintion_] <= ' ') // ignore spaces
                ++posintion_;
            current_symbol_ = input_[posintion_++];
            current_line_ += std::to_string(current_symbol_ - '0');
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
    std::string current_line_;
};

int main(){
LOG_TRACE 
    //Parser Parser("c=5;c=10;c=c+2;");
    //cc=c+f/65536*(d*d+b)
    //Parser Parser("cc=191+2147483647/65536*(-5*-5+40000000000000000000000000);");
    //Parser Parser("a=-(2147483647-214748364*10);");
    //Parser Parser("a=sqrt(sqrt(sqrt(sqrt(sqrt(sqrt(2))))));");
    //Parser Parser("a=sin(sin(sin(sin(sin(sin(2))))));");
    //Parser Parser("f=2;z=2;b=-((f-z)*10);");
    //Parser Parser("z=214748364;f=2147483647;b=1; c=2;d=c*-2;z=81/9/3;b=-(f-z*10);c=c+1;c=c+1;c=c+1;c=c+1;c=c+1;c=c+1;cc=c+f/65536*(d*d+b);abc=cc/100;f=cc-100*abc;z=-z;c=c-c;");
    Parser Parser("___ = (3<(2+1))");
    Parser.Parse();
    return 0;
}

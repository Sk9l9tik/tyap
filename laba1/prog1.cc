#include <iostream>
#include <fstream>
#include <string>


enum class States {
    Normal,
    Slash,
    StartMultyComment,
    EndComment
};

void first_auto_deleter(std::fstream& in, std::fstream& out) {

    int c = 0;
    States State = States::Normal;

    while ((c = in.get()) != EOF) {
        switch (State) {
        case States::Normal:
            if (c == '/')
                State = States::Slash;
            else
                out << static_cast<char>(c);
            break;

        case States::Slash:
            if (c == '*')
                State = States::StartMultyComment;
            else if (c == '/'){
                State = States::Slash;
                out << '/';
            }
            else {
                State = States::Normal;
                out << '/' << static_cast<char>(c);
            }
            break;

        case States::StartMultyComment:
            if (c == '*')
                State = States::EndComment;
            break;

        case States::EndComment:
            if (c == '/') {
                State = States::Normal;
                out << ' ';
            }
            else if (c != '*')
                State = States::StartMultyComment;
            break;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Error input!\n";
        return 3;
    }

    std::fstream in(argv[1]);
    std::fstream out(argv[2]);

    if (!in.is_open()) {
        std::cerr << "Can't open in file!\n ";
        return 1;
    }
    else if (!out.is_open()) {
        std::cerr << "Can't open out file!\n ";
        in.close();
        return 2;
    }

    first_auto_deleter(in, out);

    std::cout << "Comments are deleted!\n";

    in.close();
    out.close();

    return 0;
} 
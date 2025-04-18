#include <iostream>
#include <fstream>


enum class States {
    Normal,
    Slash,
    Literal,
    CharLiteral,
    BackSlash,
    CharBackSlash,
    StartMultyComment,
    StartOneComment,
    EndComment
};

void second_auto_deleter(std::fstream& in, std::fstream& out) {

	int c = 0;
	States State = States::Normal;

	while ((c = in.get()) != EOF) {
		switch (State) {
		case States::Normal:
			if (c == '/')
				State = States::Slash;
			else if (c == '"') {
				State = States::Literal;
				out << '"';
			}
			else if (c == '\'') {
				State = States::CharLiteral;
				out << '\'';
			}
			else
				out << static_cast<char>(c);
			break;

		case States::Literal:
			if (c == '\\') {
				State = States::BackSlash;
			}
			else if (c == '"') {
				State = States::Normal;
			}
			out << static_cast<char>(c);
			break;

		case States::BackSlash:
			out.put(c);
			State = States::Literal;
			break;

		case States::CharBackSlash:
			out.put(c);
			State = States::CharLiteral;
			break;

		case States::CharLiteral:
			if (c == '\\') {
				State = States::CharBackSlash;
			}
			else if (c == '\'') {
				State = States::Normal;
			}
			out.put(c);
			break;

		case States::Slash:
			if (c == '/')
				State = States::StartOneComment;
			else if (c == '*')
				State = States::StartMultyComment;
			else {
				State = States::Normal;
				out << '/' << static_cast<char>(c);
			}
			break;

		case States::StartOneComment:
			if (c == '\n') {
				State = States::Normal;
				out << '\n';
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

    second_auto_deleter(in, out);

    std::cout << "Comments are deleted!\n";

    in.close();
    out.close();

    return 0;
} 
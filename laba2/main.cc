#include <iostream>
#include <fstream>
#include <string_view>

enum class States {
	Normal,

	IntText,

	Slash,
	Literal,
	CharLiteral,
	BackSlash,
	CharBackSlash,
	StartMultyComment,
	StartOneComment,
	EndComment,

	Decimal,
	OctHexBin,
	Octal,
	HexT,
	Hex,
	BinaryT,
	Binary,

	Long,
	Unsigned,

	Long_Long,
	Unsingned_Long,
	Long_Unsigned,
	Unsigned_Long_Long,

	ErrorRead
};

std::string_view digit1{ "123456789" };
std::string_view digit{ "0123456789" };
std::string_view hex_digit{ "0123456789AaBbCcDdEeFf" };
std::string_view oct_digit{ "01234567" };

bool is_text(const int& c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}
bool is_text_ordigit(const int& c){
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || digit.find(c) != std::string_view::npos);
}

States end_int_state(const int& c){
	if (c == '/') return States::Slash;
	else if (c == '"') return States::Literal;
	else if (c == '\'') return States::CharLiteral;
	else return States::Normal;
}

void const_detector(std::ifstream& in, std::ofstream& out) {
	int c = 0;
	States State = States::Normal;

	while (/*(c = in.get()) != EOF*/ 1) {
		c = in.get();
		switch (State) {
		case States::Normal:
			if (c == '/') State = States::Slash;
			else if (c == '"') State = States::Literal;
			else if (c == '\'') State = States::CharLiteral;
			else if (c == '0') {
				State = States::OctHexBin;
				out.put(c);
			}
			else if (digit1.find(c) != std::string_view::npos) {
				out.put(c);
				State = States::Decimal;
			}
			else if (is_text(c)) State = States::IntText;
			else if (c == EOF) return;
			break;
		case States::IntText:
			if (is_text_ordigit(c))
				State = States::IntText;
			else 
				State = end_int_state(c);
			break;
		case States::Literal:
			if (c == '\\')
				State = States::BackSlash;
			else if (c == '"')
				State = States::Normal;
			break;

		case States::BackSlash:
			State = States::Literal;
			break;
		case States::CharBackSlash:
			State = States::CharLiteral;
			break;
		case States::CharLiteral:
			if (c == '\\')
				State = States::CharBackSlash;
			else if (c == '\'')
				State = States::Normal;
			break;
		case States::Slash:
			if (c == '/')
				State = States::StartOneComment;
			else if (c == '*')
				State = States::StartMultyComment;
			else if (c == '"') State = States::Literal;
			else if (c == '0') {
				out.put(c);
				State = States::OctHexBin;
			}
			else if (digit1.find(c) == std::string_view::npos) {
				out.put(c);
				State = States::Decimal;
			}
			else if (is_text(c)) State = States::IntText;
			else
				State = States::Normal;
			break;
		case States::StartOneComment:
			if (c == '\n')
				State = States::Normal;
			else if (c == EOF) return;
			break;

		case States::StartMultyComment:
			if (c == '*')
				State = States::EndComment;
			break;
		case States::EndComment:
			if (c == '/')
				State = States::Normal;
			else if (c != '*')
				State = States::StartMultyComment;
			else if (c == EOF) return;
			break;

		case States::ErrorRead:
			if (!is_text(c)) {
				State = States::Normal;
				out << "\t ERROR\n";
				if (c == EOF) return;
			}
			break;
		case States::Decimal:
			if (digit.find(c) != std::string_view::npos)
				out.put(c);
			else if (c == 'l' || c == 'L') {
				out.put(c);
				State = States::Long;
			}
			else if (c == 'u' || c == 'U') {
				out.put(c);
				State = States::Unsigned;
			}
			else if (is_text(c)) {
				out.put(c);
				State = States::ErrorRead;
			}
			else {
				out << "\t int\n";
				State = end_int_state(c);
				if (c == EOF) return;
			}
			break;
		case States::OctHexBin:
			if (c == 'x' || c == 'X') {
				out.put(c);
				State = States::HexT;
			}
			else if (c == 'b' || c == 'B') {
				out.put(c);
				State = States::BinaryT;
			}
			else if (oct_digit.find(c) != std::string_view::npos) {
				out.put(c);
				State = States::Octal;
			}
			else {
				out.put(c);
				State = States::ErrorRead;
			}
			break;
		case States::HexT:
			if (hex_digit.find(c) != std::string_view::npos){
				out.put(c);
				State = States::Hex;
			}
			else if(is_text(c)){
				out.put(c);
				State = States::ErrorRead;
			}
			else{
				out << "\t Error\n";
				State = end_int_state(c);
			}
			break;
		case States::Hex:
			if (hex_digit.find(c) != std::string_view::npos) {
				out.put(c);
			}
			else if (c == 'l' || c == 'L') {
				out.put(c);
				State = States::Long;
			}
			else if (c == 'u' || c == 'U') {
				out.put(c);
				State = States::Unsigned;
			}
			else if (is_text(c)) {
				out.put(c);
				State = States::ErrorRead;
			}
			else {
				State = end_int_state(c);
				out << "\t int\n";
				if (c == EOF) return;
			}
			break;
		case States::Octal:
			if (oct_digit.find(c) != std::string_view::npos) {
				out.put(c);
			}
			else if (c == '8' || c == '9'){
				out.put(c);
				State = States::ErrorRead;
			}
			else if (c == 'l' || c == 'L') {
				out.put(c);
				State = States::Long;
			}
			else if (c == 'u' || c == 'U') {
				out.put(c);
				State = States::Unsigned;
			}
			else if (is_text(c)) {
				out.put(c);
				State = States::ErrorRead;
			}
			else {
				State = end_int_state(c);
				out << "\t int\n";
				if (c == EOF) return;
			}
			break;
		case States::BinaryT:
			if (c == '0' || c == '1'){
				out.put(c);
				State = States::Binary;
			}
			else if(is_text_ordigit(c)){
				out.put(c);
				State = States::ErrorRead;
			}
			else{
				out << "\t Error\n";
				State = end_int_state(c);
			}
			break;
		case States::Binary:
			if (c == '0' || c == '1') {
				out.put(c);
			}
			else if (c == 'l' || c == 'L') {
				out.put(c);
				State = States::Long;
			}
			else if (c == 'u' || c == 'U') {
				out.put(c);
				State = States::Unsigned;
			}
			else if (is_text_ordigit(c)) {
				out.put(c);
				State = States::ErrorRead;
			}
			else {
				State = end_int_state(c);
				out << "\t int\n";
				if (c == EOF) return;
			}
			break;
		case States::Long:
			if (c == 'l' || c == 'L') {
				out.put(c);
				State = States::Long_Long;
			}
			else if (c == 'u' || c == 'U') {
				out.put(c);
				State = States::Long_Unsigned;
			}
			else if (is_text_ordigit(c)) {
				out.put(c);
				State = States::ErrorRead;
			}
			else {
				out << "\t long int\n";
				State = end_int_state(c);;
				if (c == EOF) return;
			}
			break;
		case States::Unsigned:
			if (c == 'l' || c == 'L') {
				out.put(c);
				State = States::Unsingned_Long;
			}
			else if (is_text_ordigit(c)) {
				out.put(c);
				State = States::ErrorRead;
			}
			else {
				out << "\t unsigned int\n";
				State = end_int_state(c);
				if (c == EOF) return;
			}
			break;
		case States::Long_Long:
			if (c == 'u' || c == 'U'){
				out.put(c);
				State = States::Unsigned_Long_Long;
			}
			else if (!is_text_ordigit(c)) {
				out << "\t long long int\n";
				State = end_int_state(c);;
			}
			else {
				out.put(c);
				State = States::ErrorRead;
				if (c == EOF) return;
			}
			break;
		case States::Long_Unsigned:
			if (!is_text_ordigit(c)){
				out << "\t unsigned long int\n";
				State = end_int_state(c);
				if(c == EOF) return;
			}
			else{
				out.put(c);
				State = States::ErrorRead;
				if (c == EOF) return;
			}
		break;
		case States::Unsingned_Long:
			if (c == 'l' || c == 'L'){
				State = States::Unsigned_Long_Long;
				out.put(c);
			}
			else if (!is_text_ordigit(c)) {
				out << "\t unsigned long int\n";
				State = end_int_state(c);;
				if (c == EOF) return;
			}
			else {
				out.put(c);
				State = States::ErrorRead;
				if (c == EOF) return;
			}
			break;
		case States::Unsigned_Long_Long:
			if (!is_text_ordigit(c)){
				out << "\t unsigned long int\n";
				State = end_int_state(c);;
				if (c == EOF) return;
			}
			else{
				out.put(c);
				State = States::ErrorRead;
				if (c == EOF) return;
			}
		}
	}
}


int main(int argc, char** argv) {
	if (argc != 3) {std::cerr << "Error input!\n"; return 3;}
	
	std::ifstream in(argv[1]);
	std::ofstream out(argv[2]);

	if (!in.is_open()) {std::cerr << "Can't open in file!\n"; return 1;}
	else if (!out.is_open()) {std::cerr << "Can't open out file!\n"; in.close(); return 2;}

	const_detector(in, out);

	std::cout << "Consts are detected\n";

	in.close();
	out.close();

	return 0;
}

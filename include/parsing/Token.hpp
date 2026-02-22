#ifndef TOKEN_HPP
#define TOKEN_HPP

class Token
{
private:

public:
	enum Tokens {
		LEFT_BRACE,
		RIGHT_BRACE,
		SEMI_COLON,
		WORD,
	};
	Token();
	Token(const Token &obj);
	Token &operator=(const Token &obj);
	~Token();

	void tokenize();

};

#endif
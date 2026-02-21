#ifndef TOKEN_HPP
#define TOKEN_HPP

class Token
{
private:

public:
	Token();
	Token(const Token &obj);
	Token &operator=(const Token &obj);
	~Token();

	void tokenize();

};

#endif
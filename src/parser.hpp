#ifndef GRAPHCALC_PARSER_HPP
#define GRAPHCALC_PARSER_HPP

#include <string>
#include <vector>
#include <map>

enum class TokenType {
    Number,
    Variable,
    Operator,
    Function,
    Lparen,
    Rparen
};

struct Token {
    TokenType type;
    std::string value;
};

class MathParser {
public:
    MathParser();
    bool parse(const std::string& expression);
    double evaluate(double x, double y = 0.0, double t = 0.0, double a = 1.0, double b = 1.0, double c = 0.0) const;
    const std::string& getLastError() const { return lastError; }
    bool isValid() const { return valid; }

private:
    std::vector<Token> infixTokens;
    std::vector<Token> postfixTokens;
    mutable std::string lastError;
    bool valid;

    std::vector<Token> tokenize(const std::string& expr);
    bool shuntingYard();
};

#endif // GRAPHCALC_PARSER_HPP

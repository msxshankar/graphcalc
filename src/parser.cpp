#include "parser.hpp"
#include <iostream>
#include <sstream>
#include <cmath>
#include <stack>
#include <cctype>
#include <algorithm>

MathParser::MathParser() : valid(false) {}

bool MathParser::parse(const std::string& expression) {
    valid = false;
    lastError = "";
    if (expression == "mandelbrot" || expression == "julia" || expression == "plasma" || expression == "lorenz" || expression == "sierpinski" || expression == "clifford" || expression == "barnsley" || expression == "aizawa") {
        valid = true;
        return true;
    }
    infixTokens = tokenize(expression);
    if (infixTokens.empty()) {
        lastError = "Empty expression";
        return false;
    }
    if (!shuntingYard()) {
        return false;
    }
    valid = true;
    return true;
}

std::vector<Token> MathParser::tokenize(const std::string& expr) {
    std::vector<Token> tokens;
    size_t i = 0;
    size_t len = expr.length();

    auto is_operator = [](char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
    };

    while (i < len) {
        if (std::isspace(expr[i])) {
            i++;
            continue;
        }

        // 1. Numbers
        if (std::isdigit(expr[i]) || expr[i] == '.') {
            std::string numStr = "";
            bool hasDot = false;
            while (i < len && (std::isdigit(expr[i]) || expr[i] == '.')) {
                if (expr[i] == '.') {
                    if (hasDot) break;
                    hasDot = true;
                }
                numStr += expr[i];
                i++;
            }
            tokens.push_back({TokenType::Number, numStr});
            continue;
        }

        // 2. Parentheses
        if (expr[i] == '(') {
            tokens.push_back({TokenType::Lparen, "("});
            i++;
            continue;
        }
        if (expr[i] == ')') {
            tokens.push_back({TokenType::Rparen, ")"});
            i++;
            continue;
        }

        // 3. Operators
        if (is_operator(expr[i])) {
            tokens.push_back({TokenType::Operator, std::string(1, expr[i])});
            i++;
            continue;
        }

        // 4. Words (Variables, Functions)
        if (std::isalpha(expr[i])) {
            std::string word = "";
            while (i < len && std::isalpha(expr[i])) {
                word += expr[i];
                i++;
            }

            // Check if it's a function
            if (word == "sin" || word == "cos" || word == "tan" || 
                word == "sqrt" || word == "exp" || word == "log" || 
                word == "abs" || word == "asin" || word == "acos" || 
                word == "atan") {
                tokens.push_back({TokenType::Function, word});
            } else {
                // Treat each letter as a separate variable/constant if it's not a function?
                // Actually, let's treat variables as 'x', 'y', 't', 'a', 'b', 'c'.
                // If the word contains multiple characters that are not a function, we can split it,
                // or just treat the whole word as a variable.
                // To support standard algebraic syntax like "ax", let's split it if it has length > 1
                // and contains letters like x, y, t, a, b, c.
                if (word.length() == 1) {
                    tokens.push_back({TokenType::Variable, word});
                } else {
                    for (char c : word) {
                        tokens.push_back({TokenType::Variable, std::string(1, c)});
                    }
                }
            }
            continue;
        }

        // Unknown character
        lastError = "Unknown character: " + std::string(1, expr[i]);
        return {}; // Return empty to trigger parsing failure
    }

    // Insert implicit multiplication (*) where necessary
    std::vector<Token> finalTokens;
    for (size_t k = 0; k < tokens.size(); k++) {
        finalTokens.push_back(tokens[k]);
        if (k + 1 < tokens.size()) {
            const auto& t1 = tokens[k];
            const auto& t2 = tokens[k + 1];

            bool t1EndsValue = (t1.type == TokenType::Number || 
                                t1.type == TokenType::Variable || 
                                t1.type == TokenType::Rparen);
            bool t2StartsValue = (t2.type == TokenType::Variable || 
                                  t2.type == TokenType::Function || 
                                  t2.type == TokenType::Lparen || 
                                  t2.type == TokenType::Number);

            if (t1EndsValue && t2StartsValue) {
                finalTokens.push_back({TokenType::Operator, "*"});
            }
        }
    }

    return finalTokens;
}

bool MathParser::shuntingYard() {
    postfixTokens.clear();
    std::stack<Token> opStack;

    auto getPrecedence = [](const std::string& op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        if (op == "u-") return 3; // Unary minus
        if (op == "^") return 4;
        return 0;
    };

    auto isRightAssociative = [](const std::string& op) {
        return op == "^" || op == "u-";
    };

    bool nextCanBeUnary = true;

    for (const auto& token : infixTokens) {
        if (token.type == TokenType::Number || token.type == TokenType::Variable) {
            postfixTokens.push_back(token);
            nextCanBeUnary = false;
        } 
        else if (token.type == TokenType::Function) {
            opStack.push(token);
            nextCanBeUnary = true;
        } 
        else if (token.type == TokenType::Lparen) {
            opStack.push(token);
            nextCanBeUnary = true;
        } 
        else if (token.type == TokenType::Rparen) {
            bool foundLparen = false;
            while (!opStack.empty()) {
                if (opStack.top().type == TokenType::Lparen) {
                    opStack.pop();
                    foundLparen = true;
                    break;
                }
                postfixTokens.push_back(opStack.top());
                opStack.pop();
            }
            if (!foundLparen) {
                lastError = "Mismatched parentheses";
                return false;
            }
            if (!opStack.empty() && opStack.top().type == TokenType::Function) {
                postfixTokens.push_back(opStack.top());
                opStack.pop();
            }
            nextCanBeUnary = false;
        } 
        else if (token.type == TokenType::Operator) {
            std::string opVal = token.value;
            if (nextCanBeUnary && opVal == "-") {
                opVal = "u-";
            }

            while (!opStack.empty() && (opStack.top().type == TokenType::Operator || opStack.top().type == TokenType::Function)) {
                std::string topOp = opStack.top().value;
                int p1 = getPrecedence(opVal);
                int p2 = getPrecedence(topOp);
                
                // If it is a function, it has higher precedence than normal operators
                if (opStack.top().type == TokenType::Function) {
                    postfixTokens.push_back(opStack.top());
                    opStack.pop();
                    continue;
                }

                if ((!isRightAssociative(opVal) && p1 <= p2) || (isRightAssociative(opVal) && p1 < p2)) {
                    postfixTokens.push_back(opStack.top());
                    opStack.pop();
                } else {
                    break;
                }
            }
            opStack.push({TokenType::Operator, opVal});
            nextCanBeUnary = true;
        }
    }

    while (!opStack.empty()) {
        if (opStack.top().type == TokenType::Lparen) {
            lastError = "Mismatched parentheses";
            return false;
        }
        postfixTokens.push_back(opStack.top());
        opStack.pop();
    }

    return true;
}

double MathParser::evaluate(double x, double y, double t, double a, double b, double c) const {
    if (!valid) return 0.0;
    std::stack<double> evalStack;

    for (const auto& token : postfixTokens) {
        if (token.type == TokenType::Number) {
            try {
                evalStack.push(std::stod(token.value));
            } catch (...) {
                evalStack.push(0.0);
            }
        } 
        else if (token.type == TokenType::Variable) {
            char var = token.value[0];
            if (var == 'x') evalStack.push(x);
            else if (var == 'y') evalStack.push(y);
            else if (var == 't') evalStack.push(t);
            else if (var == 'a') evalStack.push(a);
            else if (var == 'b') evalStack.push(b);
            else if (var == 'c') evalStack.push(c);
            else evalStack.push(0.0);
        } 
        else if (token.type == TokenType::Operator) {
            if (token.value == "u-") {
                if (evalStack.empty()) return 0.0;
                double val = evalStack.top(); evalStack.pop();
                evalStack.push(-val);
            } else {
                if (evalStack.size() < 2) return 0.0;
                double rVal = evalStack.top(); evalStack.pop();
                double lVal = evalStack.top(); evalStack.pop();
                
                if (token.value == "+") evalStack.push(lVal + rVal);
                else if (token.value == "-") evalStack.push(lVal - rVal);
                else if (token.value == "*") evalStack.push(lVal * rVal);
                else if (token.value == "/") evalStack.push(rVal == 0.0 ? 0.0 : lVal / rVal);
                else if (token.value == "^") evalStack.push(std::pow(lVal, rVal));
            }
        } 
        else if (token.type == TokenType::Function) {
            if (evalStack.empty()) return 0.0;
            double val = evalStack.top(); evalStack.pop();
            
            if (token.value == "sin") evalStack.push(std::sin(val));
            else if (token.value == "cos") evalStack.push(std::cos(val));
            else if (token.value == "tan") evalStack.push(std::tan(val));
            else if (token.value == "sqrt") evalStack.push(val < 0.0 ? 0.0 : std::sqrt(val));
            else if (token.value == "exp") evalStack.push(std::exp(val));
            else if (token.value == "log") evalStack.push(val <= 0.0 ? 0.0 : std::log(val));
            else if (token.value == "abs") evalStack.push(std::abs(val));
            else if (token.value == "asin") evalStack.push(std::asin(val));
            else if (token.value == "acos") evalStack.push(std::acos(val));
            else if (token.value == "atan") evalStack.push(std::atan(val));
        }
    }

    if (evalStack.empty()) return 0.0;
    return evalStack.top();
}

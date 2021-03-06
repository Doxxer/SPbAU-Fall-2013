#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "lexer.hpp"

Lexer::Lexer(std::string const &sourceFileName)
{
    std::ifstream inputFile_(sourceFileName.c_str(), std::ifstream::in);

    size_t lineNumber = 1;
    for (std::string line; std::getline(inputFile_, line); ++lineNumber) {       
        if (line.length() != 0)
            processLine(line, lineNumber);        
    }
    content_.push_back(Token(Token::tt_eof, "", lineNumber));
    position_ = content_.begin();
}

void Lexer::getOperator(std::string::const_iterator &it, Token &currentToken)
{
    switch (*it) {
        case ':':
            currentToken.type = Token::tt_colon;
            break;
        case ',':
            currentToken.type = Token::tt_comma;
            break;
        case '+':
            currentToken.type = Token::tt_operation_plus;
            break;
        case '-':
            currentToken.type = Token::tt_operation_minus;
            break;
        case '*':
            currentToken.type = Token::tt_operation_mult;
            break;
        case '/':
            currentToken.type = Token::tt_operation_div;
            break;
        case '(':
            currentToken.type = Token::tt_opening_bracket;
            break;
        case ')':
            currentToken.type = Token::tt_closing_bracket;
            break;
        case '<':
        case '>':
            currentToken.name = *it;
            currentToken.type = Token::tt_inequality;
            if (*(it + 1) == '=') {
                currentToken.name += '=';
                ++it;
            }
            break;
        case '=':
            currentToken.name = *it;
            if (*(it + 1) == '=') {
                currentToken.name += '=';
                currentToken.type = Token::tt_inequality;
                ++it;
            } else
                currentToken.type = Token::tt_operation_eq;
            break;
        case '!':
            currentToken.name = *it;
            if (*(it + 1) == '=') {
                currentToken.name += '=';
                currentToken.type = Token::tt_inequality;
                ++it;
            } else
                throw LexerError(currentToken.lineNumber, "expected '=' after '!'");
            break;
    }
}

void Lexer::getIdentifier(std::string::const_iterator &it, Token &currentToken)
{
    if (isalpha(*it)) {
        std::string identifier;

        while (isalpha(*it) || isdigit(*it) || *it == '_')
            identifier += *(it++);
        --it;

        if (identifier == "def")
            currentToken.type = Token::tt_def;
        else if (identifier == "end")
            currentToken.type = Token::tt_end;
        else if (identifier == "while")
            currentToken.type = Token::tt_while;
        else if (identifier == "if")
            currentToken.type = Token::tt_if;
        else if (identifier == "return")
            currentToken.type = Token::tt_return;
        else if (identifier == "print")
            currentToken.type = Token::tt_print;
        else if (identifier == "read")
            currentToken.type = Token::tt_read;
        else {
            currentToken.type = Token::tt_identifier;
            currentToken.name = identifier;
        }
    }
}

void Lexer::getNumber(std::string::const_iterator &it, Token &currentToken)
{
    if (isdigit(*it)) {
        std::string number;
        while (isdigit(*it))
            number += *(it++);

        if (isalpha(*it) || *it == '_')
            throw LexerError(currentToken.lineNumber, "not a number");

        --it;
        currentToken.type = Token::tt_number;
        currentToken.name = number;
    }
}

void Lexer::processLine(std::string const &str, size_t lineNumber)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (*it == '#')
            break;
        if (isspace(*it))
            continue;

        Token currentToken(Token::tt_unknown, "", lineNumber);

        getOperator(it, currentToken);
        if (currentToken.type == Token::tt_unknown)
            getIdentifier(it, currentToken);
        if (currentToken.type == Token::tt_unknown)
            getNumber(it, currentToken);

        if (currentToken.type == Token::tt_unknown) {
            std::stringstream ss;
            ss << "unknown symbol <" << *it << ">";
            throw LexerError(lineNumber, ss.str());
        }
        content_.push_back(currentToken);
    }
    content_.push_back(Token(Token::tt_cr, "", lineNumber));
}

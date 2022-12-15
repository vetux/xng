/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "xng/headertool/tokenizer.hpp"

#include <iterator>

namespace xng {
    enum Scope {
        SCOPE_NONE,
        SCOPE_IDENTIFIER,
        SCOPE_STRING_LITERAL,
        SCOPE_NUMERIC_LITERAL,
        SCOPE_COMMENT_SINGLE_LINE,
        SCOPE_COMMENT_MULTI_LINE
    };

    bool isNumber(char c) {
        return c >= '0' && c <= '9';
    }

    bool isTerminator(char c) {
        return c == ' '
               || c == '\r'
               || c == '\n'
               || c == '\t'
               || c == ',';
    }

    void finishScope(std::vector<Token> &ret, std::string &accumulator, Scope &currentScope) {
        switch (currentScope) {
            case SCOPE_NONE:
                break;
            case SCOPE_IDENTIFIER:
                ret.emplace_back(Token(Token::IDENTIFIER, accumulator));
                break;
            case SCOPE_STRING_LITERAL:
                ret.emplace_back(Token(Token::LITERAL_STRING, accumulator));
                break;
            case SCOPE_NUMERIC_LITERAL:
                ret.emplace_back(Token(Token::LITERAL_NUMERIC, accumulator));
                break;
            case SCOPE_COMMENT_SINGLE_LINE:
            case SCOPE_COMMENT_MULTI_LINE:
                ret.emplace_back(Token::COMMENT, accumulator);
                break;
        }
        currentScope = SCOPE_NONE;
        accumulator.clear();
    }

    std::vector<Token> Tokenizer::tokenize(std::istream &source) {
        std::string data;
        char c;
        while (source.get(c)) {
            data += c;
        }

        std::vector<Token> ret;
        std::string accumulator;
        Scope currentScope = SCOPE_NONE;
        bool stringEscape = false;

        for (auto i = 0; i < data.size(); i++) {
            auto c = data.at(i);

            if (currentScope != SCOPE_STRING_LITERAL
                && currentScope != SCOPE_COMMENT_SINGLE_LINE
                && currentScope != SCOPE_COMMENT_MULTI_LINE) {
                if (c == '(') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::BRACKET_OPEN));
                    continue;
                } else if (c == '[') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::SQUARE_BRACKET_OPEN));
                    continue;
                } else if (c == ']') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::SQUARE_BRACKET_CLOSE));
                    continue;
                } else if (c == ')') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::BRACKET_CLOSE));
                    continue;
                } else if (c == '{') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::CURLY_BRACKET_OPEN));
                    continue;
                } else if (c == '}') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::CURLY_BRACKET_CLOSE));
                    continue;
                } else if (c == '*') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::ASTERISK));
                    continue;
                } else if (c == '&') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::AMPERSAND));
                    continue;
                } else if (c == ';') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::SEMICOLON));
                    continue;
                } else if (c == '<') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::LESS_THAN));
                    continue;
                } else if (c == '>') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::GREATER_THAN));
                    continue;
                } else if (c == '=') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::EQUAL_SIGN));
                    continue;
                } else if (c == ',') {
                    finishScope(ret, accumulator, currentScope);
                    ret.emplace_back(Token(Token::COMMA));
                    continue;
                }
            }
            switch (currentScope) {
                case SCOPE_NONE:
                    if (isTerminator(c)) {
                        continue;
                    } else if (isNumber(c)
                               || c == '-'
                               || c == '+') {
                        currentScope = SCOPE_NUMERIC_LITERAL;
                        accumulator += c;
                    } else if (c == '"') {
                        currentScope = SCOPE_STRING_LITERAL;
                        accumulator += c;
                    } else if (c == '/'
                               && i + 1 < data.size()
                               && data.at(i + 1) == '/') {
                        currentScope = SCOPE_COMMENT_SINGLE_LINE;
                        accumulator += c;
                    } else if (c == '/'
                               && i + 1 < data.size()
                               && data.at(i + 1) == '*') {
                        currentScope = SCOPE_COMMENT_MULTI_LINE;
                        accumulator += c;
                    } else {
                        currentScope = SCOPE_IDENTIFIER;
                        accumulator += c;
                    }
                    break;
                case SCOPE_IDENTIFIER:
                    if (isTerminator(c)) {
                        finishScope(ret, accumulator, currentScope);
                    } else {
                        accumulator += c;
                    }
                    break;
                case SCOPE_STRING_LITERAL:
                    accumulator += c;
                    if (c == '\\' && !stringEscape) {
                        stringEscape = true;
                    } else if (c == '"' && !stringEscape) {
                        finishScope(ret, accumulator, currentScope);
                    } else {
                        stringEscape = false;
                    }
                    break;
                case SCOPE_NUMERIC_LITERAL:
                    if (isTerminator(c)) {
                        finishScope(ret, accumulator, currentScope);
                    } else {
                        accumulator += c;
                    }
                    break;
                case SCOPE_COMMENT_SINGLE_LINE:
                    if (c == '\n') {
                        finishScope(ret, accumulator, currentScope);
                    } else {
                        accumulator += c;
                    }
                    break;
                case SCOPE_COMMENT_MULTI_LINE:
                    accumulator += c;
                    if (c == '*'
                        && i + 1 < data.size()
                        && data.at(i + 1) == '/') {
                        accumulator += data.at(i + 1);
                        finishScope(ret, accumulator, currentScope);
                        i++;
                    }
                    break;
            }
        }

        return ret;
    }
}

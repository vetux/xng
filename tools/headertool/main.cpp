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

#include <fstream>
#include <iostream>

#include "xng/xng.hpp"

void printUsage() {

}

void printToken(const xng::Token &token) {
    switch (token.type) {
        case xng::Token::BRACKET_OPEN:
            std::cout << "BRACKET_OPEN";
            break;
        case xng::Token::BRACKET_CLOSE:
            std::cout << "BRACKET_CLOSE";
            break;
        case xng::Token::SQUARE_BRACKET_OPEN:
            std::cout << "SQUARE_BRACKET_OPEN";
            break;
        case xng::Token::SQUARE_BRACKET_CLOSE:
            std::cout << "SQUARE_BRACKET_CLOSE";
            break;
        case xng::Token::CURLY_BRACKET_OPEN:
            std::cout << "CURLY_BRACKET_OPEN";
            break;
        case xng::Token::CURLY_BRACKET_CLOSE:
            std::cout << "CURLY_BRACKET_CLOSE";
            break;
        case xng::Token::ASTERISK:
            std::cout << "ASTERISK";
            break;
        case xng::Token::AMPERSAND:
            std::cout << "AMPERSAND";
            break;
        case xng::Token::SEMICOLON:
            std::cout << "SEMICOLON";
            break;
        case xng::Token::LESS_THAN:
            std::cout << "LESS_THAN";
            break;
        case xng::Token::GREATER_THAN:
            std::cout << "GREATER_THAN";
            break;
        case xng::Token::EQUAL_SIGN:
            std::cout << "EQUAL_SIGN";
            break;
        case xng::Token::COMMA:
            std::cout << "COMMA";
            break;
        case xng::Token::IDENTIFIER:
            std::cout << "IDENTIFIER\t\t" + token.value;
            break;
        case xng::Token::LITERAL_STRING:
            std::cout << "LSTRING   \t\t" + token.value;
            break;
        case xng::Token::LITERAL_NUMERIC:
            std::cout << "LNUM      \t\t" + token.value;
            break;
        case xng::Token::COMMENT:
            std::cout << "COMMENT   \t\t" + token.value;
            break;
    }
    std::cout << "\t" << token.lineNumber << "\n";
}

void printMetadata(const xng::ComponentMetadata &metadata) {
    std::cout << metadata.typeName << "(" << metadata.category << ")" << "\n";
    for (auto &mem: metadata.members) {
        std::cout << "\t"
                  << mem.typeMetadata.fullTypeName()
                  << " "
                  << mem.instanceName
                  << "("
                  << mem.displayName
                  << ", "
                  << mem.description
                  << ") = "
                  << mem.defaultValue
                  << " min:"
                  << mem.minimum.value
                  << " max:"
                  << mem.maximum.value
                  << "\n";
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage();
        return 0;
    }

    std::string sourceFile(argv[1]);
    std::ifstream fs(sourceFile);

    if (!fs) {
        std::cout << "Failed to open file at " + sourceFile;
        return 0;
    }

    xng::Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(fs);

    std::cout << "--------- Lexer ----------\n";

    for (auto &token: tokens) {
        printToken(token);
    }

    std::cout << "--------- Parser/Generator ----------\n";

    xng::HeaderGenerator generator;
    xng::HeaderParser parser;
    auto metadata = parser.parseTokens(sourceFile, tokens);

    for(auto &m : metadata){
        printMetadata(m);
        auto hdr = generator.generateHeader(m);
        std::cout << "Generated Header:\n" << hdr << "\n";
    }

    std::cout << "--------- Generator ----------\n";


    return 0;
}
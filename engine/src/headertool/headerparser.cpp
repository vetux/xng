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

#include "xng/headertool/headerparser.hpp"

#include <stdexcept>
#include <vector>
#include <string>

namespace xng {
    size_t findToken(size_t begin,
                     size_t end,
                     const std::vector<Token> &tokens,
                     Token::TokenType type,
                     const std::string &value) {
        for (auto it = begin; it < end; it++) {
            auto token = tokens.at(it);
            if (token.type == type && token.value == value) {
                return it;
            }
        }
        return end;
    }

    size_t findToken(size_t begin,
                     size_t end,
                     const std::vector<Token> &tokens,
                     Token::TokenType type) {
        for (auto it = begin; it < end; it++) {
            auto token = tokens.at(it);
            if (token.type == type) {
                return it;
            }
        }
        return end;
    }

    /**
     * Argument values must be either string or numeric literals.
     *
     * Eg. MyArgument="MyValue"
     *          lhs  |  rhs
     */
    struct MacroArgument {
        Token lhs;
        Token rhs;

        MacroArgument() = default;

        MacroArgument(Token lhs, Token rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    };

    void syntaxErrorCallback(const std::string &fileName, size_t lineNumber, const std::string &msg) {
        throw SyntaxException(fileName, lineNumber, msg);
    }

    static const char *ERROR_TOKEN_TYPE = "Token type error";
    static const char *ERROR_EOF = "Unexpected EOF";

    size_t parseMacro(const std::string &fileName,
                      const std::vector<Token> &tokens,
                      size_t begin,
                      std::vector<MacroArgument> &arguments) {
        auto bracketOpen = begin + 1;
        auto bracketClose = findToken(bracketOpen + 1, tokens.size(), tokens, Token::BRACKET_CLOSE);

        auto diff = bracketClose - bracketOpen - 1;
        if (diff > 3) {
            // Iterate commas
            size_t lastComma = 0;
            for (auto comma = findToken(bracketOpen, bracketClose, tokens, Token::COMMA);
                 comma < bracketClose;
                 comma += 1,
                         comma = findToken(comma, bracketClose, tokens, Token::COMMA)) {
                lastComma = comma;

                if (comma <= bracketOpen + 3) {
                    continue;
                }

                auto ident = tokens.at(comma - 3);
                auto equals = tokens.at(comma - 2);
                auto val = tokens.at(comma - 1);

                if (ident.type != Token::IDENTIFIER
                    || equals.type != Token::EQUAL_SIGN
                    || (val.type != Token::LITERAL_STRING
                        && val.type != Token::LITERAL_NUMERIC)) {
                    syntaxErrorCallback(fileName,
                                        ident.lineNumber,
                                        ERROR_TOKEN_TYPE);
                }

                arguments.emplace_back(MacroArgument(ident, val));
            }

            auto ident = tokens.at(lastComma + 1);
            auto equals = tokens.at(lastComma + 2);
            auto val = tokens.at(lastComma + 3);

            if (ident.type != Token::IDENTIFIER
                || equals.type != Token::EQUAL_SIGN
                || (val.type != Token::LITERAL_STRING
                    && val.type != Token::LITERAL_NUMERIC)) {
                syntaxErrorCallback(fileName,
                                    ident.lineNumber,
                                    ERROR_TOKEN_TYPE);
            }

            arguments.emplace_back(MacroArgument(ident, val));
        } else if (diff > 1) {
            auto ident = tokens.at(bracketOpen + 1);
            auto equals = tokens.at(bracketOpen + 2);
            auto val = tokens.at(bracketOpen + 3);

            if (ident.type != Token::IDENTIFIER
                || equals.type != Token::EQUAL_SIGN
                || (val.type != Token::LITERAL_STRING
                    && val.type != Token::LITERAL_NUMERIC)) {
                syntaxErrorCallback(fileName,
                                    ident.lineNumber,
                                    ERROR_TOKEN_TYPE);
            }

            arguments.emplace_back(MacroArgument(ident, val));
        }
        return bracketClose;
    }

    /**
     * Parse a type with optional template specification.
     *
     * @param fileName
     * @param tokens
     * @param begin
     * @param typeName
     * @param arguments
     * @return The index of the last token belonging to the type
     */
    size_t parseType(const std::string &fileName,
                     size_t begin,
                     size_t end,
                     const std::vector<Token> &tokens,
                     ComponentMetadata::TypeMetadata &typeMetadata) {
        if (begin >= tokens.size())
            return begin;

        // Parse top level typename
        auto typeToken = tokens.at(begin);

        if (typeToken.type != Token::IDENTIFIER) {
            syntaxErrorCallback(fileName,
                                typeToken.lineNumber,
                                ERROR_TOKEN_TYPE);
        }

        typeMetadata.typeName = typeToken.value;

        auto tempOpen = begin + 1;
        auto tempClose = tempOpen;

        if (tempOpen <= tokens.size()
            && tokens.at(tempOpen).type == Token::LESS_THAN) {
            // Type is template type
            auto scope = 0;
            for (auto i = tempOpen + 1; i < end; i++) {
                if (tokens.at(i).type == Token::LESS_THAN) {
                    scope++;
                } else if (tokens.at(i).type == Token::GREATER_THAN) {
                    if (scope == 0) {
                        tempClose = i + 1;
                        break;
                    } else {
                        scope--;
                    }
                }
            }

            auto tempTokens = tempClose - tempOpen - 2;

            if (tempTokens == 1) {
                // Single argument
                ComponentMetadata::TypeMetadata tempMetadata;

                // Parse typename
                auto tempTypeToken = tokens.at(tempOpen + 1);

                if (tempTypeToken.type != Token::IDENTIFIER) {
                    syntaxErrorCallback(fileName,
                                        tempTypeToken.lineNumber,
                                        ERROR_TOKEN_TYPE);
                }

                tempMetadata.typeName = tempTypeToken.value;
                typeMetadata.templateArguments.emplace_back(tempMetadata);
            } else if (tempTokens > 1) {
                // Multiple arguments
                // Iterate commas between brackets
                size_t lastComma = tempOpen;
                for (auto comma = findToken(tempOpen, tempClose, tokens, Token::COMMA);
                     comma < tempClose;
                     lastComma = comma,
                     comma += 1,
                     comma = findToken(comma, tempClose, tokens, Token::COMMA)) {
                    // Parse the type between the commas
                    ComponentMetadata::TypeMetadata tempMetadata;
                    parseType(fileName, lastComma + 1, comma, tokens, tempMetadata);
                    typeMetadata.templateArguments.emplace_back(tempMetadata);
                }
                // Parse the type between the last comma and tempClose
                ComponentMetadata::TypeMetadata tempMetadata;
                parseType(fileName, lastComma + 1, tempClose, tokens, tempMetadata);
                typeMetadata.templateArguments.emplace_back(tempMetadata);
            }
        }

        return tempClose;
    }

    std::vector<ComponentMetadata> HeaderParser::parseTokens(const std::string &fileName,
                                                             const std::vector<Token> &tokens) {
        std::vector<ComponentMetadata> ret;

        // Iterate all XCOMPONENT identifiers
        for (auto componentBegin = findToken(0, tokens.size(), tokens, Token::IDENTIFIER, "XCOMPONENT");
             componentBegin < tokens.size();
             componentBegin += 1,
                     componentBegin = findToken(componentBegin,
                                                tokens.size(),
                                                tokens,
                                                Token::IDENTIFIER,
                                                "XCOMPONENT")) {
            ComponentMetadata componentMetadata;

            // Parse XCOMPONENT macro arguments
            std::vector<MacroArgument> macroArgs;
            auto bracketClose = parseMacro(fileName, tokens, componentBegin, macroArgs);
            if (!macroArgs.empty()) {
                auto arg = macroArgs.at(0);
                if (arg.lhs.value == "Category") {
                    componentMetadata.category = arg.rhs.value;
                }
            }

            // Parse typename
            auto typeName = bracketClose + 2;
            if (typeName >= tokens.size()) {
                syntaxErrorCallback(fileName,
                                    tokens.at(componentBegin).lineNumber,
                                    ERROR_EOF);
            } else if (tokens.at(typeName).type == Token::IDENTIFIER) {
                componentMetadata.typeName = tokens.at(typeName).value;
            } else {
                syntaxErrorCallback(fileName,
                                    tokens.at(componentBegin).lineNumber,
                                    ERROR_TOKEN_TYPE);
            }

            // Find the component type closing brackets
            auto componentEnd = typeName + 1;
            auto scope = 0;
            for (auto &i = componentEnd; i < tokens.size(); i++) {
                if (tokens.at(i).type == Token::CURLY_BRACKET_OPEN) {
                    scope++;
                } else if (tokens.at(i).type == Token::CURLY_BRACKET_CLOSE) {
                    scope--;
                    if (scope == 0) {
                        componentEnd = i;
                        break;
                    }
                }
            }

            // Iterate all XVARIABLE identifiers in the component range
            for (auto varBegin = findToken(componentBegin, componentEnd, tokens, Token::IDENTIFIER, "XVARIABLE");
                 varBegin < componentEnd;
                 varBegin += 1,
                         varBegin = findToken(varBegin,
                                              componentEnd,
                                              tokens,
                                              Token::IDENTIFIER,
                                              "XVARIABLE")) {
                ComponentMetadata::MemberMetadata member;

                std::vector<MacroArgument> varMacroArgs;
                auto varBracketCloseIndex = parseMacro(fileName, tokens, varBegin, varMacroArgs);

                for (auto &arg: varMacroArgs) {
                    if (arg.lhs.value == "Name") {
                        member.displayName = arg.rhs.value;
                    } else if (arg.lhs.value == "Description") {
                        member.description = arg.rhs.value;
                    } else if (arg.lhs.value == "Minimum") {
                        member.minimum = arg.rhs;
                    } else if (arg.lhs.value == "Maximum") {
                        member.maximum = arg.rhs;
                    }
                }

                // Parse variable type
                auto varTypeBegin = varBracketCloseIndex + 1;

                if (varBracketCloseIndex >= tokens.size()
                    || varTypeBegin >= tokens.size()) {
                    syntaxErrorCallback(fileName,
                                        tokens.at(varBegin).lineNumber,
                                        ERROR_EOF);
                }

                ComponentMetadata::TypeMetadata typeMetadata;
                auto varTypeEnd = parseType(fileName, varTypeBegin, componentEnd, tokens, typeMetadata);

                member.typeMetadata = typeMetadata;

                // Check instance name
                auto varInstanceName = varTypeEnd;
                if (varInstanceName < componentEnd && tokens.at(varInstanceName).type == Token::IDENTIFIER) {
                    member.instanceName = tokens.at(varInstanceName).value;

                    // Check assignment
                    auto varAssign = varInstanceName + 1;
                    if (varAssign < componentEnd && tokens.at(varAssign).type == Token::EQUAL_SIGN) {
                        // Parse default value
                        auto assignmentBegin = varAssign + 1;
                        auto assignmentEnd = findToken(assignmentBegin, componentEnd, tokens, Token::SEMICOLON);
                        std::string defVal;
                        for (auto i = assignmentBegin; i < assignmentEnd; i++) {
                            defVal += tokens.at(i).value;
                        }
                        member.defaultValue = defVal;
                    }

                    componentMetadata.members.emplace_back(member);
                }
            }

            ret.emplace_back(componentMetadata);
        }

        return ret;
    }
}
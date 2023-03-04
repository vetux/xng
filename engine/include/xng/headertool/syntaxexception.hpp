/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_SYNTAXEXCEPTION_HPP
#define XENGINE_SYNTAXEXCEPTION_HPP

#include <exception>
#include <string>

namespace xng {
    /**
     * Thrown when the HeaderParser encounters a syntax error
     */
    class SyntaxException : public std::exception {
    public:
        static std::string formatMessage(const std::string &fileName,
                                         size_t lineNumber,
                                         const std::string &syntaxError) {
            return "Syntax Error at ["
                   + fileName
                   + "]+"
                   + std::to_string(lineNumber)
                   + "\n"
                   + syntaxError;
        }

        SyntaxException(const std::string &fileName,
                        size_t lineNumber,
                        const std::string &syntaxError)
                : fileName(fileName),
                  lineNumber(lineNumber),
                  syntaxError(syntaxError) {
            formattedMessage = formatMessage(fileName, lineNumber, syntaxError);
        }

        const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return formattedMessage.c_str();
        }

        const std::string &getFormattedMessage() const {
            return formattedMessage;
        }

        const std::string &getFileName() const {
            return fileName;
        }

        size_t getLineNumber() const {
            return lineNumber;
        }

        const std::string &getSyntaxError() const {
            return syntaxError;
        }

    private:
        std::string formattedMessage;
        std::string fileName;
        size_t lineNumber;
        std::string syntaxError;
    };
}
#endif //XENGINE_SYNTAXEXCEPTION_HPP

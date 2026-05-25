/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef GRID_HPP
#define GRID_HPP

#include <cassert>
#include <vector>

#include "xng/math/rectangle.hpp"
#include "xng/math/vector2.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    class XENGINE_EXPORT Grid : public Messageable {
    public:
        int nCol, nRow;
        Vec2i size;

        Grid(int nColumns, int nRows, Vec2i size)
                : nCol(nColumns), nRow(nRows), size(size) {
            assert(nColumns > 0);
            assert(nRows > 0);
            assert(size.x > 0);
            assert(size.y > 0);
            assert(size.x % nColumns == 0);
            assert(size.y % nRows == 0);
        }

        Grid()
                : nCol(0), nRow(0), size(0) {
        }

        Vec2i getCellSize() const {
            return {size.x / nCol, size.y / nRow};
        }

        Recti getCell(int column, int row) const {
            assert(column < nCol);
            assert(row < nRow);

            return {Vec2i(column * (size.x / nCol), row * (size.y / nRow)),
                    Vec2i(size.x / nCol, size.y / nRow)};
        }

        std::vector<Recti> getColumn(int column, int numberOfRows = 0) const {
            assert(column < nCol);
            assert(numberOfRows <= nRow);

            if (numberOfRows <= 0)
                numberOfRows = nRow;
            std::vector<Recti> ret;
            for (int row = 0; row < numberOfRows; row++) {
                ret.push_back(getCell(column, row));
            }
            return ret;
        }

        std::vector<Recti> getRow(int row, int numberOfColumns = 0) const {
            assert(row < nRow);
            assert(numberOfColumns <= nCol);

            if (numberOfColumns <= 0)
                numberOfColumns = nCol;
            std::vector<Recti> ret;
            for (int col = 0; col < numberOfColumns; col++) {
                ret.push_back(getCell(col, row));
            }
            return ret;
        }

        Messageable &operator<<(const Message &message) override {
            message.value("nCol", nCol);
            message.value("nRow", nRow);
            size << message.getMessage("size");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["nCol"] = nCol;
            message["nRow"] = nRow;
            size >> message["size"];
            return message;
        }
    };
}

#endif //GRID_HPP
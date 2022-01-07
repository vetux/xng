#ifndef GRID_HPP
#define GRID_HPP

#include <cassert>
#include <vector>

#include "rectangle.hpp"
#include "math/vector2.hpp"

namespace xengine {
    class XENGINE_EXPORT Grid {
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
    };
}

#endif //GRID_HPP
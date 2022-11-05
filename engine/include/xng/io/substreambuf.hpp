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

#ifndef XENGINE_SUBSTREAMBUF_HPP
#define XENGINE_SUBSTREAMBUF_HPP

#include <cassert>
#include <streambuf>
#include <iostream>

//https://stackoverflow.com/a/7657367

class substreambuf : public std::streambuf
{
public:

    substreambuf(std::streambuf *sbuf, std::size_t start, std::size_t len) : m_sbuf(sbuf), m_start(start), m_len(len), m_pos(0)
    {
        std::streampos p = m_sbuf->pubseekpos(start);
        assert(p != std::streampos(-1));
        setbuf(NULL, 0);
    }

protected:

    int underflow()
    {
        if (m_pos + std::streamsize(1) >= m_len)
            return traits_type::eof();
        return m_sbuf->sgetc();
    }

    int uflow()
    {
        if (m_pos + std::streamsize(1) > m_len)
            return traits_type::eof();
        m_pos += std::streamsize(1);
        return m_sbuf->sbumpc();
    }

    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
    {
        std::streampos cursor;

        if (way == std::ios_base::beg)
            cursor = off;
        else if (way == std::ios_base::cur)
            cursor = m_pos + off;
        else if (way == std::ios_base::end)
            cursor = m_len - off;

        if (cursor < 0 || cursor >= m_len)
            return std::streampos(-1);
        m_pos = cursor;
        if (m_sbuf->pubseekpos(m_start + m_pos) == std::streampos(-1))
            return std::streampos(-1);

        return m_pos;
    }

    std::streampos seekpos(std::streampos sp, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
    {
        if (sp < 0 || sp >= m_len)
            return std::streampos(-1);
        m_pos = sp;
        if (m_sbuf->pubseekpos(m_start + m_pos) == std::streampos(-1))
            return std::streampos(-1);
        return m_pos;
    }

private:
    std::streambuf *m_sbuf;
    std::streampos m_start;
    std::streamsize m_len;
    std::streampos m_pos;
};

#endif //XENGINE_SUBSTREAMBUF_HPP

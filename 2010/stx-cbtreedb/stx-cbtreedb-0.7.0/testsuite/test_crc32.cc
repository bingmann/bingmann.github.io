// -*- mode: c++; fill-column: 79 -*-
// $Id: test_crc32.cc 2 2010-04-14 07:34:58Z tb $

/*
 * STX Constant B-Tree Database Template Classes v0.7.0
 * Copyright (C) 2010 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * Test CRC32 class by calculating and comparing a few checksums.
 */

#define CBTREEDB_SELF_VERIFY
#include "stx-cbtreedb.h"

#include <assert.h>

// simple method to access the protected CRC32 class
class CBTreeDBTest : public stx::CBTreeDB<>
{
public:

    static uint32_t CRC32_digest(const std::string& str)
    {
	return CRC32::digest(str);
    }
};

int main()
{
    assert( CBTreeDBTest::CRC32_digest("") == 0x00000000 );

    assert( CBTreeDBTest::CRC32_digest("test string") == 0x13471545 );

    assert( CBTreeDBTest::CRC32_digest("0123456789012345678901234567890123456789") == 0xA6463C49 );

    assert( CBTreeDBTest::CRC32_digest("wYHemLvD4RCdRZJc0ac42WAL1SIjaRdd8OxLeAjtOc") == 0x1CFCEA07 );

    return 0;
}

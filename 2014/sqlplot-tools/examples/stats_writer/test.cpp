/*******************************************************************************
 * examples/stats_writer/test.cc
 *
 * Example for stats_writer
 *
 *******************************************************************************
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "stats_writer.h"

#include <iostream>

int main(int argc, char *argv[])
{
    stats_writer sw;

    sw >> "keyA" << "value " << 5
       >> "keyB" << 42
       >> "keyC" << 101.5
       >> "argc" << argc
       >> "argv[0]" << argv[0];

    std::cout << sw;
    
    return 0;
}

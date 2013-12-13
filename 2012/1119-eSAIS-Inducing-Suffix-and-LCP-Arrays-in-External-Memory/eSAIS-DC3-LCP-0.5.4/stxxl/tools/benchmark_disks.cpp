/***************************************************************************
 *  tools/benchmark_disks.cpp
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2009 Johannes Singler <singler@ira.uka.de>
 *  Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

/*
  This programm will benchmark the disks configured via .stxxl disk
  configuration files. The block manager is used to read and write blocks using
  the different allocation strategies.
*/

/*
   example gnuplot command for the output of this program:
   (x-axis: offset in GiB, y-axis: bandwidth in MiB/s)

   plot \
        "disk.log" using ($2/1024):($7) w l title "read", \
        "disk.log" using ($2/1024):($4)  w l title "write"
 */

#include <iomanip>
#include <vector>

#include <stxxl/io>
#include <stxxl/mng>
#include <stxxl/bits/common/cmdline.h>

using stxxl::timestamp;

#ifdef BLOCK_ALIGN
 #undef BLOCK_ALIGN
#endif

#define BLOCK_ALIGN  4096

#define POLL_DELAY 1000

#define CHECK_AFTER_READ 0

#define MB (1024 * 1024)

template <typename AllocStrategy>
int benchmark_disks_alloc(stxxl::uint64 length, stxxl::uint64 batch_size,
                          std::string optrw)
{
    stxxl::uint64 offset = 0, endpos = offset + length;

    if (length == 0)
        endpos = std::numeric_limits<stxxl::uint64>::max();

    bool do_read = (optrw.find('r') != std::string::npos);
    bool do_write = (optrw.find('w') != std::string::npos);

    // construct block type

    const unsigned raw_block_size = 8 * MB;
    const unsigned block_size = raw_block_size / sizeof(int);

    typedef stxxl::typed_block<raw_block_size, unsigned> block_type;
    typedef stxxl::BID<raw_block_size> BID_type;

    if (batch_size == 0)
        batch_size = stxxl::config::get_instance()->disks_number();

    // calculate total bytes processed in a batch
    batch_size = raw_block_size * batch_size;

    stxxl::uint64 num_blocks_per_batch = stxxl::div_ceil(batch_size, raw_block_size);
    batch_size = num_blocks_per_batch * raw_block_size;

    block_type* buffer = new block_type[num_blocks_per_batch];
    stxxl::request_ptr* reqs = new stxxl::request_ptr[num_blocks_per_batch];
    std::vector<BID_type> blocks;
    double totaltimeread = 0, totaltimewrite = 0;
    stxxl::int64 totalsizeread = 0, totalsizewrite = 0;

    std::cout << "# Batch size: "
              << stxxl::add_IEC_binary_multiplier(batch_size, "B") << " ("
              << num_blocks_per_batch << " blocks of "
              << stxxl::add_IEC_binary_multiplier(raw_block_size, "B") << ")"
              << " using " << AllocStrategy().name()
              << std::endl;

    // touch data, so it is actually allcoated
    for (unsigned j = 0; j < num_blocks_per_batch; ++j)
        for (unsigned i = 0; i < block_size; ++i)
            buffer[j][i] = j * block_size + i;

    try {
        AllocStrategy alloc;
        while (offset < endpos)
        {
            const stxxl::int64 current_batch_size = std::min<stxxl::int64>(batch_size, endpos - offset);
#if CHECK_AFTER_READ
            const stxxl::int64 current_batch_size_int = current_batch_size / sizeof(int);
#endif
            const stxxl::uint64 current_num_blocks_per_batch = stxxl::div_ceil(current_batch_size, raw_block_size);

            std::cout << "Offset    " << std::setw(7) << offset / MB << " MiB: " << std::fixed;

            stxxl::unsigned_type num_total_blocks = blocks.size();
            blocks.resize(num_total_blocks + current_num_blocks_per_batch);
            stxxl::block_manager::get_instance()->new_blocks(alloc, blocks.begin() + num_total_blocks, blocks.end());

            double begin = timestamp(), end, elapsed;

            if (do_write)
            {
                for (unsigned j = 0; j < current_num_blocks_per_batch; j++)
                    reqs[j] = buffer[j].write(blocks[num_total_blocks + j]);

                wait_all(reqs, current_num_blocks_per_batch);

                end = timestamp();
                elapsed = end - begin;
                totalsizewrite += current_batch_size;
                totaltimewrite += elapsed;
            }
            else
                elapsed = 0.0;

            std::cout << std::setw(5) << std::setprecision(1) << (double(current_batch_size) / MB / elapsed) << " MiB/s write, ";


            begin = timestamp();

            if (do_read)
            {
                for (unsigned j = 0; j < current_num_blocks_per_batch; j++)
                    reqs[j] = buffer[j].read(blocks[num_total_blocks + j]);

                wait_all(reqs, current_num_blocks_per_batch);

                end = timestamp();
                elapsed = end - begin;
                totalsizeread += current_batch_size;
                totaltimeread += elapsed;
            }
            else
                elapsed = 0.0;

            std::cout << std::setw(5) << std::setprecision(1) << (double(current_batch_size) / MB / elapsed) << " MiB/s read" << std::endl;

#if CHECK_AFTER_READ
            for (unsigned j = 0; j < current_num_blocks_per_batch; j++)
            {
                for (unsigned i = 0; i < block_size; i++)
                {
                    if (buffer[j][i] != j * block_size + i)
                    {
                        int ibuf = i / current_batch_size_int;
                        int pos = i % current_batch_size_int;

                        std::cout << "Error on disk " << ibuf << " position " << std::hex << std::setw(8) << offset + pos * sizeof(int)
                                  << "  got: " << std::hex << std::setw(8) << buffer[j][i] << " wanted: " << std::hex << std::setw(8) << (j * block_size + i)
                                  << std::dec << std::endl;

                        i = (ibuf + 1) * current_batch_size_int; // jump to next
                    }
                }
            }
#endif

            offset += current_batch_size;
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << std::endl;
        STXXL_ERRMSG(ex.what());
    }

    std::cout << "=============================================================================================" << std::endl;
    std::cout << "# Average over " << std::setw(7) << totalsizewrite / MB << " MiB: ";
    std::cout << std::setw(5) << std::setprecision(1) << (double(totalsizewrite) / MB / totaltimewrite) << " MiB/s write, ";
    std::cout << std::setw(5) << std::setprecision(1) << (double(totalsizeread) / MB / totaltimeread) << " MiB/s read" << std::endl;

    delete[] reqs;
    delete[] buffer;

    return 0;
}

int benchmark_disks(int argc, char* argv[])
{
    // parse command line

    stxxl::cmdline_parser cp;

    stxxl::uint64 length = 0;
    unsigned int batch_size = 0;
    std::string optrw = "rw", allocstr;

    cp.add_param_bytes("size", "Amount of data to write/read from disks (e.g. 10GiB)", length);
    cp.add_opt_param_string("r|w", "Only read or write blocks (default: both write and read)", optrw);
    cp.add_opt_param_string("alloc", "Block allocation strategy: RC, SR, FR, striping. (default: RC)", allocstr);

    cp.add_uint('b', "batch", "Number of blocks written/read in one batch (default: D * 8MiB)", batch_size);

    cp.set_description(
        "This program will benchmark the disks configured by the standard "
        ".stxxl disk configuration files mechanism. Blocks of 8 MiB are "
        "written and/or read in sequence using the block manager. The batch "
        "size describes how many blocks are written/read in one batch. The "
        "are taken from block_manager using given the specified allocation "
        "strategy. If size == 0, then writing/reading operation are done "
        "until an error occurs. "
        );

    if (!cp.process(argc, argv))
        return -1;

    if (allocstr.size())
    {
        if (allocstr == "RC")
            return benchmark_disks_alloc<stxxl::RC>(length, batch_size, optrw);
        if (allocstr == "SR")
            return benchmark_disks_alloc<stxxl::SR>(length, batch_size, optrw);
        if (allocstr == "FR")
            return benchmark_disks_alloc<stxxl::FR>(length, batch_size, optrw);
        if (allocstr == "striping")
            return benchmark_disks_alloc<stxxl::striping>(length, batch_size, optrw);

        std::cout << "Unknown allocation strategy '" << allocstr << "'" << std::endl;
        cp.print_usage();
        return -1;
    }

    return benchmark_disks_alloc<STXXL_DEFAULT_ALLOC_STRATEGY>(length, batch_size, optrw);
}

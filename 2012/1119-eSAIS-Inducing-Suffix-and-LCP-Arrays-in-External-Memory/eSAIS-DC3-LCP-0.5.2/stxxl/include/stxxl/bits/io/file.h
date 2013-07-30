/***************************************************************************
 *  include/stxxl/bits/io/file.h
 *
 *  Part of the STXXL. See http://stxxl.sourceforge.net
 *
 *  Copyright (C) 2002 Roman Dementiev <dementiev@mpi-sb.mpg.de>
 *  Copyright (C) 2008, 2010 Andreas Beckmann <beckmann@cs.uni-frankfurt.de>
 *  Copyright (C) 2008, 2009 Johannes Singler <singler@ira.uka.de>
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 **************************************************************************/

#ifndef STXXL_IO_FILE_HEADER
#define STXXL_IO_FILE_HEADER

#ifdef STXXL_BOOST_CONFIG
 #include <boost/config.hpp>
#endif

#if defined (__linux__)
 #define STXXL_CHECK_BLOCK_ALIGNING
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef BOOST_MSVC
// this is not stxxl/bits/io/io.h !
 #include <io.h>
#else
 #include <unistd.h>
 #include <sys/resource.h>
 #include <sys/wait.h>
#endif


//#ifdef __sun__
//#define O_DIRECT 0
//#endif

#ifndef O_SYNC
 #define O_SYNC 0
#endif
#ifndef O_RSYNC
 #define O_RSYNC 0
#endif
#ifndef O_DSYNC
 #define O_DSYNC 0
#endif

#if defined (__linux__)
 #if ! defined(O_DIRECT)
  #error O_DIRECT is not defined while __linux__ is - PLEASE REPORT THIS BUG
 #endif
//#include <asm/fcntl.h>
// FIXME: In which conditions is this not defined? Why only i386 and alpha? Why not amd64?
 #if !defined (O_DIRECT) && (defined (__alpha__) || defined (__i386__))
  #define O_DIRECT 040000       /* direct disk access */
 #endif
#endif

#ifndef O_DIRECT
 #define O_DIRECT O_SYNC
#endif


#include <cassert>

#include <stxxl/bits/libstxxl.h>
#include <stxxl/bits/namespace.h>
#include <stxxl/bits/noncopyable.h>
#include <stxxl/bits/common/exceptions.h>
#include <stxxl/bits/common/mutex.h>
#include <stxxl/bits/io/request.h>
#include <stxxl/bits/io/request_ptr.h>


__STXXL_BEGIN_NAMESPACE

//! \addtogroup iolayer
//! \{

//! \brief Defines interface of file

//! It is a base class for different implementations that might
//! base on various file systems or even remote storage interfaces
class file : private noncopyable
{
    mutex request_ref_cnt_mutex;
    int request_ref_cnt;

protected:
    //! \brief Initializes file object
    //! \param _id file identifier
    //! \remark Called in implementations of file
    file() : request_ref_cnt(0) { }

public:
    // the offset of a request, also the size of the file
    typedef request::offset_type offset_type;
    // the size of a request
    typedef request::size_type size_type;

    //! \brief Definition of acceptable file open modes

    //! Various open modes in a file system must be
    //! converted to this set of acceptable modes
    enum open_mode
    {
        RDONLY = 1,                         //!< only reading of the file is allowed
        WRONLY = 2,                         //!< only writing of the file is allowed
        RDWR = 4,                           //!< read and write of the file are allowed
        CREAT = 8,                          //!< in case file does not exist no error occurs and file is newly created
        DIRECT = 16,                        //!< I/Os proceed bypassing file system buffers, i.e. unbuffered I/O
        TRUNC = 32,                         //!< once file is opened its length becomes zero
        SYNC = 64,                          //!< open the file with O_SYNC | O_DSYNC | O_RSYNC flags set
        NO_LOCK = 128,                      //!< do not aquire an exclusive lock by default
    };

    static const int DEFAULT_QUEUE = -1;
    static const int NO_QUEUE = -2;
    static const int NO_ALLOCATOR = -1;

    //! \brief Schedules an asynchronous read request to the file
    //! \param buffer pointer to memory buffer to read into
    //! \param pos file position to start read from
    //! \param bytes number of bytes to transfer
    //! \param on_cmpl I/O completion handler
    //! \return \c request_ptr request object, which can be used to track the status of the operation
    virtual request_ptr aread(void * buffer, offset_type pos, size_type bytes,
                              const completion_handler & on_cmpl) = 0;

    //! \brief Schedules an asynchronous write request to the file
    //! \param buffer pointer to memory buffer to write from
    //! \param pos starting file position to write
    //! \param bytes number of bytes to transfer
    //! \param on_cmpl I/O completion handler
    //! \return \c request_ptr request object, which can be used to track the status of the operation
    virtual request_ptr awrite(void * buffer, offset_type pos, size_type bytes,
                               const completion_handler & on_cmpl) = 0;

    virtual void serve(const request * req) throw (io_error) = 0;

    void add_request_ref()
    {
        scoped_mutex_lock Lock(request_ref_cnt_mutex);
        ++request_ref_cnt;
    }

    void delete_request_ref()
    {
        scoped_mutex_lock Lock(request_ref_cnt_mutex);
        assert(request_ref_cnt > 0);
        --request_ref_cnt;
    }

    int get_request_nref()
    {
        scoped_mutex_lock Lock(request_ref_cnt_mutex);
        return request_ref_cnt;
    }

    //! \brief Changes the size of the file
    //! \param newsize new file size
    virtual void set_size(offset_type newsize) = 0;
    //! \brief Returns size of the file
    //! \return file size in bytes
    virtual offset_type size() = 0;
    //! \brief Returns the identifier of the file's queue
    //! \remark Files allocated on the same physical device usually share the same queue
    //! \return queue number
    virtual int get_queue_id() const = 0;
    //! \brief Returns the file's allocator
    //! \return allocator number
    virtual int get_allocator_id() const = 0;

    virtual int get_physical_device_id() const
    {
        return get_queue_id();
    }

    //! \brief Locks file for reading and writing (acquires a lock in the file system)
    virtual void lock() = 0;

    //! \brief Discard a region of the file (mark it unused)
    //! some specialized file types may need to know freed regions
    virtual void discard(offset_type offset, offset_type size)
    {
        STXXL_UNUSED(offset);
        STXXL_UNUSED(size);
    }

    virtual void export_files(offset_type offset, offset_type length, std::string prefix)
    {
        STXXL_UNUSED(offset);
        STXXL_UNUSED(length);
        STXXL_UNUSED(prefix);
    }

    virtual void remove() { }

    virtual ~file()
    {
        int nr = get_request_nref();
        if (nr != 0)
            STXXL_ERRMSG("stxxl::file is being deleted while there are still " << nr << " (unfinished) requests referencing it");
    }

    //! \brief Identifies the type of I/O implementation
    //! \return pointer to null terminated string of characters, containing the name of I/O implementation
    virtual const char * io_type() const
    {
        return "none";
    }
};

//! \}

__STXXL_END_NAMESPACE

#endif // !STXXL_IO_FILE_HEADER
// vim: et:ts=4:sw=4

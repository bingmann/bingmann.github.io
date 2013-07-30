/*! \mainpage Documentation for STXXL library
 *
 *  \image html layer_diagram.png
 *
 * <br><br>
 * The core of \c S<small>TXXL</small> is an implementation of the C++
 * standard template library STL for external memory (out-of-core)
 * computations, i.e., \c S<small>TXXL</small> implements containers and algorithms
 * that can process huge volumes of data that only fit on
 * disks. While the compatibility to the STL supports
 * ease of use and compatibility with existing applications,
 * another design priority is high performance.
 * Here is a selection of \c S<small>TXXL</small> performance features:
 * - transparent support of multiple disks
 * - variable block lengths
 * - overlapping of I/O and computation
 * - prevention of OS file buffering overhead
 * - algorithm pipelining
 * - utilization of multiple processor cores for internal computation
 *
 *
 * \section platforms Supported Operating Systems
 * - Linux (kernel >= 2.4.18)
 * - Mac OS X
 * - FreeBSD
 * - other POSIX compatible systems should work, but have not been tested
 * - Windows 2000/XP/Vista/7
 *
 *
 * \section compilers Supported Compilers
 *
 * The following compilers have been tested in different
 * \c S<small>TXXL</small> configurations.
 * Other compilers might work, too, but we don't have the resources
 * (systems, compilers or time) to test them.
 * Feedback is welcome.
 *
 * The compilers marked with '*' are the developer's favorite choices
 * and are most thoroughly tested.
 *
 * \verbatim
                |         parallel            parallel
                |  stxxl   stxxl     stxxl     stxxl
  compiler      |                   + boost   + boost
----------------+----------------------------------------
  GCC 4.6 c++0x |    x     PMODE       x       PMODE 
  GCC 4.6       |    x     PMODE       x       PMODE 
  GCC 4.5 c++0x |    x     PMODE       x       PMODE 
  GCC 4.5       |    x     PMODE       x       PMODE 
* GCC 4.4 c++0x |    x     PMODE       x       PMODE 
  GCC 4.4       |    x     PMODE       x       PMODE 
  GCC 4.3 c++0x |    x     PMODE²      x       PMODE²
  GCC 4.3       |    x     PMODE²      x       PMODE²
  GCC 4.2       |    x     MCSTL       x       MCSTL
  GCC 4.1       |    x       -         x         -
  GCC 4.0       |    x       -         x         -
  GCC 3.4       |    x       -         x         -
  GCC 3.3       |    o       -         o         -
  GCC 2.95      |    -       -         -         -
  ICPC 12.0.191 |    x¹    PMODE¹²     x¹      PMODE¹²
  ICPC 12.0.191 |    x¹    MCSTL¹      x¹      MCSTL¹
* ICPC 11.1.075 |    x¹    MCSTL¹      x¹      MCSTL¹
  ICPC 11.0.084 |    x¹    MCSTL¹      x¹      MCSTL¹
  ICPC 10.1.026 |    x¹    MCSTL¹      x¹      MCSTL¹
  ICPC 10.0.026 |    x¹    MCSTL¹      x¹      MCSTL¹
  ICPC 9.1.053  |    x¹      -         x¹        -
  ICPC 9.0.032  |    x¹      -         x¹        -
  clang++ 2.9   |    x       -         x         -
  MSVC 2010 10.0|    -       -         x         -
* MSVC 2008 9.0 |    -       -         x         -
  MSVC 2005 8.0 |    -       -         x         -

 x   = full support
 o   = partial support
 -   = unsupported
 ?   = untested
 PMODE = supports parallelization using libstdc++ parallel mode
 MCSTL = supports parallelization using the MCSTL library (superseded by
       PMODE, introduced in gcc 4.3)
 ¹   = you may have to add a -gcc-name=<gcc-x.y> option if the system default
       gcc does not come in the correct version:
       icpc 9.0: use with gcc 3.x
       icpc 9.1: use with gcc before 4.2
       icpc 10.x, 11.x, 12.0 with mcstl support: use with gcc 4.2
       icpc 12.0 with pmode support: use with gcc 4.3
 ²   = gcc 4.3 only provides partial support for the libstdc++ parallel mode,
       full support requires gcc 4.4 or later
\endverbatim
 *
 *
 * \section boost Supported BOOST versions
 *
 * The <a href="http://www.boost.org">Boost</a> libraries are required on
 * Windows platforms using MSVC compiler and optional on other platforms.
 *
 * \c S<small>TXXL</small> has been tested with Boost 1.40.0, 1.42.0 and 1.46.1.
 * Other versions may work, too, but older versions will not get support.
 *
 *
 * \section installation Instructions on installation, usage, configuration
 *
 * - \link installation_linux_gcc Installation, usage, configuration (Linux/Unix &ndash; g++/icpc/clang++) \endlink
 * - \link installation_msvc Installation, usage, configuration (Windows &ndash; Microsoft Visual C++) \endlink
 *
 * - \link install-svn Installing from subversion \endlink
 *
 *
 * \section questions Questions
 *
 * - Questions concerning use and development of the \c S<small>TXXL</small>
 * library should be posted to the
 * <b><a href="http://sourceforge.net/projects/stxxl/forums">FORUMS</a></b>.
 * Please search the forum before posting,
 * your question may have been answered before.
 *
 * \section bugreports Bug Reports
 *
 * - Bugs should be reported in the 
 *   <b><a href="https://stxxl.ae.cs.uni-frankfurt.de/bugs/">Bugzilla Bug Tracker</a></b>
 *
 * - \link FAQ FAQ - Frequently Asked Questions \endlink
 *
 *
 * \section license License
 *
 * \c S<small>TXXL</small> is distributed under the Boost Software License, Version 1.0.<br>
 * You can find a copy of the license in the accompanying file \c LICENSE_1_0.txt or online at
 * <a href="http://www.boost.org/LICENSE_1_0.txt">http://www.boost.org/LICENSE_1_0.txt</a>.
 */


/*!
 * \page FAQ FAQ - Frequently Asked Questions
 *
 * \section FAQ-latest Latest version of this FAQ
 * The most recent version of this FAQ can always be found
 * <a href="http://algo2.iti.kit.edu/stxxl/trunk/FAQ.html">here</a>.
 *
 *
 * \section q1 References to Elements in External Memory Data Structures
 *
 * You should not pass or store references to elements in an external memory
 * data structure. When the reference is used, the block that contains the
 * element may be no longer in internal memory.<br>
 * Use/pass an iterator (reference) instead.<br>
 * For an \c stxxl::vector with \c n pages and LRU replacement strategy, it
 * can be guaranteed that the last \c n references
 * obtained using \c stxxl::vector::operator[] or dereferencing
 * an iterator are valid.
 * However, if \c n is 1, even a single innocent-looking line like
 * \verbatim std::cout << v[0] << " " << v[1000000] << std::endl; \endverbatim can lead to
 * inconsistent results.
 * <br>
 *
 *
 * \section q2 Parameterizing STXXL Containers
 *
 * STXXL container types like stxxl::vector can be parameterized only with a value type that is a
 * <a href="http://en.wikipedia.org/wiki/Plain_old_data_structures">POD</a>
 * (i. e. no virtual functions, no user-defined copy assignment/destructor, etc.)
 * and does not contain references (including pointers) to internal memory.
 * Usually, "complex" data types do not satisfy this requirements.
 *
 * This is why stxxl::vector<std::vector<T> > and stxxl::vector<stxxl::vector<T> > are invalid.
 * If appropriate, use std::vector<stxxl::vector<T> >, or emulate a two-dimensional array by
 * doing index calculation.
 *
 *
 * \section q3 Thread-Safety
 *
 * The I/O and block management layers are thread-safe (since release 1.1.1).
 * The user layer data structures are not thread-safe.<br>
 * I.e. you may access <b>different</b> \c S<small>TXXL</small> data structures from concurrent threads without problems,
 * but you should not share a data structure between threads (without implementing proper locking yourself).<br>
 * This is a design choice, having the data structures thread-safe would mean a significant performance loss.
 *
 *
 * \section q4 I have configured several disks to use with STXXL. Why does STXXL fail complaining about the lack of space? According to my calclulations, the space on the disks should be sufficient.
 *
 * This may happen if the disks have different size. With the default parameters \c S<small>TXXL</small> containers use randomized block-to-disk allocation strategies
 * that distribute data evenly between the disks but ignore the availability of free space on them. 
 *
 *
 * \section q5 STXXL in a Microsoft CLR Library
 *
 * From STXXL user Christian, posted in the <a href="https://sourceforge.net/projects/stxxl/forums/forum/446474/topic/3407329">forum</a>:
 *
 * Precondition: I use STXXL in a Microsoft CLR Library (a special DLL). That means that managed code and native code (e.g. STXXL) have to co-exist in your library.
 *
 * Symptom: Application crashes at process exit, when the DLL is unloaded.
 *
 * Cause: STXXL's singleton classes use the \c atexit() function to destruct themselves at process exit. The exit handling will cause the process to crash at exit (still unclear if it's a bug or a feature of the MS runtime).
 *
 * Solution:
 *
 * 1.) Compiled STXXL static library with \c STXXL_NON_DEFAULT_EXIT_HANDLER defined.
 *
 * 2.) For cleanup, \c stxxl::run_exit_handlers() has now to be called manually. To get this done automatically:
 *
 * Defined a CLI singleton class "Controller":
 *
 * \verbatim
public ref class Controller {
private: 
    static Controller^ instance = gcnew Controller;
    Controller();
};
\endverbatim
 *
 *     Registered my own cleanup function in Controller's constructor which will manage to call \c stxxl::run_exit_handlers():
 *
 * \verbatim
#pragma managed(push, off)
static int myexitfn()
{
    stxxl::run_exit_handlers();
    return 0;
}
#pragma managed(pop)

Controller::Controller()
{
    onexit(myexitfn);
}
\endverbatim
 *
 *
 * \section q6 How can I credit STXXL, and thus foster its development?
 *
 * - For all users:
 *   - Sign up at Ohloh and add yourself as an STXXL user / rate STXXL: http://www.ohloh.net/p/stxxl
 *   - Rate STXXL at heise Software-Verzeichnis (German): http://www.heise.de/software/download/stxxl/76072
 *   - Rate STXXL at SourceForge: https://sourceforge.net/projects/stxxl/
 *
 * - For scientific work:  Cite the papers mentioned here: http://stxxl.sourceforge.net/
 *
 * - For industrial users:  Tell us the name of your company, so we can use it as a reference.
 *
 */


/*!
 * \page install Instructions on Installation, usage, configuration
 * - \link installation_linux_gcc Installation, usage, configuration (Linux/Unix &ndash; g++/icpc/clang++) \endlink
 * - \link installation_msvc Installation, usage, configuration (Windows &ndash; Microsoft Visual C++) \endlink
 *
 * - \link install-svn Installing from subversion \endlink
 */


/*!
 * \page installation_linux_gcc Installation, usage, configuration (Linux/Unix &ndash; g++/icpc/clang++)
 *
 * \section download_linux_gcc Download and Extraction
 *
 * - Download the latest gzipped tarball from
 *   <a href="http://sourceforge.net/projects/stxxl/files/stxxl/">SourceForge</a>
 * - Unpack in some directory executing: \c tar \c zfxv \c stxxl-x.y.z.tgz
 * - Change to \c stxxl directory: \c cd \c stxxl-x.y.z
 *
 * \section library_compilation_linux_gcc Library Compilation
 *
 * - Run: \verbatim make config_gnu \endverbatim to create a template \c make.settings.local file.
 *   Note: this will produce some warnings and abort with an error, which is intended.
 * - (optionally) change the \c make.settings.local file according to your system configuration:
 *   - (optionally) set the \c STXXL_ROOT variable to \c S<small>TXXL</small> root directory
 *     ( \c directory_where_you_unpacked_the_tar_ball/stxxl-x.y.z )
 *   - if you want \c S<small>TXXL</small> to use <a href="http://www.boost.org">Boost</a> libraries
 *     (you should have the Boost libraries already installed)
 *     - change the \c USE_BOOST variable to \c yes
 *     - change the \c BOOST_ROOT variable to the Boost root path, unless Boost is installed in system default search paths.
 *   - if you want \c S<small>TXXL</small> to use the <a href="http://gcc.gnu.org/onlinedocs/libstdc++/manual/parallel_mode.html">libstdc++ parallel mode</a>
 *     - use GCC version 4.3 or later
 *     - use the targets \c library_g++_pmode and \c tests_g++_pmode instead of the ones listed below
 *   - if you want \c S<small>TXXL</small> to use the <a href="http://algo2.iti.kit.edu/singler/mcstl/">MCSTL</a>
 *     library (you should have the MCSTL library already installed)
 *     - change the \c MCSTL_ROOT variable to the MCSTL root path
 *     - use the targets \c library_g++_mcstl and \c tests_g++_mcstl instead of the ones listed below
 *   - (optionally) set the \c OPT variable to \c -O3 or other g++ optimization level you like (default: \c -O3 )
 *   - (optionally) set the \c DEBUG variable to \c -g or other g++ debugging option
 *     if you want to produce a debug version of the Stxxl library or Stxxl examples (default: not set)
 *   - for more variables to tune take a look at \c make.settings.gnu ,
 *     they are usually overridden by settings in \c make.settings.local,
 *     using \c CPPFLAGS and \c LDFLAGS, for example, you can add arbitrary compiler and linker options
 * - Run: \verbatim make library_g++ \endverbatim
 * - Run: \verbatim make tests_g++ \endverbatim (optional, if you want to compile and run some test programs)
 *
 *
 * \section build_apps Building an Application
 *
 * After compiling the library, some Makefile variables are written to
 * \c stxxl.mk (\c pmstxxl.mk if you have built with parallel mode, \c mcstxxl.mk if you have built with MCSTL) in your
 * \c STXXL_ROOT directory. This file should be included from your
 * application's Makefile.
 *
 * The following variables can be used:
 * - \c STXXL_CXX - the compiler used to build the \c S<small>TXXL</small>
 *      library, it's recommended to use the same to build your applications
 * - \c STXXL_CPPFLAGS - add these flags to the compile commands
 * - \c STXXL_LDLIBS - add these libraries to the link commands
 *
 * An example Makefile for an application using \c S<small>TXXL</small>:
 * \verbatim
STXXL_ROOT      ?= .../stxxl
STXXL_CONFIG    ?= stxxl.mk
include $(STXXL_ROOT)/$(STXXL_CONFIG)

# use the variables from stxxl.mk
CXX              = $(STXXL_CXX)
CPPFLAGS        += $(STXXL_CPPFLAGS)
LDLIBS          += $(STXXL_LDLIBS)

# add your own optimization, warning, debug, ... flags
# (these are *not* set in stxxl.mk)
CPPFLAGS        += -O3 -Wall -g -DFOO=BAR

# build your application
# (my_example.o is generated from my_example.cpp automatically)
my_example.bin: my_example.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) my_example.o -o $@ $(LDLIBS)
\endverbatim
 *
 * \section parallel Enabling parallel execution
 *
 * To enable (shared-memory-)parallel execution of internal computation (in fact, sorting and merging, and random shuffling),
 * you have several options depending on the \ref compilers "compiler version" used:
 * - Enable the g++ parallel mode specifically for STXXL,
 *   by defining \c STXXL_PARALLEL_MODE_EXPLICIT and enabling OpenMP (\c -DSTXXL_PARALLEL_MODE_EXPLICIT \c -fopenmp)
 *   during compilation and linkage of your program.
 *   Compiling the library binary with this flag enabled is not really necessary,
 *   since the most time-consuming operations are called by the generic routines and thus contained in the header files.
 * - Enable the g++ parallel mode for your program globally,
 *   by defining \c _GLIBCXX_PARALLEL and enabling OpenMP (\c -D_GLIBCXX_PARALLEL \c -fopenmp).
 *   This has the implication that STL algorithms in your program will also be executed in parallel,
 *   which may have undesired side effects.
 *   These options are automatically used when you built STXXL using the \c *_pmode target,
 *   and your Makefile includes \c pmstxxl.mk.
 * - Enable MCSTL in your program by setting the include path appropriately.
 *   This implies that STL algorithms in your program will also be executed in parallel,
 *   which may have undesired side effects.
 *   These options are automatically used when you built STXXL using the \c *_mcstl target,
 *   and your Makefile includes \c mcstxxl.mk.
 *
 * We recommend to try the first option at first.
 *
 * The number of threads to be used can be set by the environment variable OMP_NUM_THREADS or
 * by calling omp_set_num_threads.
 * Detailed tuning can be achieved as described
 * <a href="http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt03ch18s04.html#parallel_mode.design.tuning">here</a>.
 *
 *
 * \section space Disk space
 *
 * Before you try to run one of the \c S<small>TXXL</small> examples
 * (or your own \c S<small>TXXL</small> program) you must configure the disk
 * space that will be used as external memory for the library.
 *
 * To get best performance with \c S<small>TXXL</small> you should assign separate disks to it.
 * These disks should be used by the library only.
 * Since \c S<small>TXXL</small> is developed to exploit disk parallelism, the performance of your
 * external memory application will increase if you use more than one disk.
 * But from how many disks your application can benefit depends on how "I/O bound" it is.
 * With modern disk bandwidths
 * of about 50-75 MiB/s most of applications are I/O bound for one disk. This means that if you add another disk
 * the running time will be halved. Adding more disks might also increase performance significantly.
 *
 *
 * \section filesystem Recommended file system
 *
 * The library benefits from direct transfers from user memory to disk, which saves superfluous copies.
 * We recommend to use the
 * \c <a href="http://xfs.org">XFS</a> file system,
 * which gives good read and write performance for large files.
 * Note that file creation speed of \c XFS is a bit slower,
 * so that disk files should be precreated for optimal performance.
 *
 * If the filesystems only use is to store one large \c S<small>TXXL</small> disk file,
 * we also recommend to add the following options to the \c mkfs.xfs command to gain maximum performance:
 * \verbatim -d agcount=1 -l size=512b \endverbatim
 *
 * The following filesystems have been reported not to support direct I/O: \c tmpfs , \c glusterfs .
 * Since direct I/O is enabled by default, you may recompile \c S<small>TXXL</small>
 * with \c STXXL_DIRECT_IO_OFF defined to access files on these file systems.
 *
 *
 * \section configuration Disk configuration file
 *
 * You must define the disk configuration for an
 * \c S<small>TXXL</small> program in a file named \c '.stxxl' that must reside
 * in the same directory where you execute the program.
 * You can change the default file name for the configuration
 * file by setting the environment variable \c STXXLCFG .
 *
 * Each line of the configuration file describes a disk.
 * A disk description uses the following format:<br>
 * \c disk=full_disk_filename,capacity,access_method
 *
 * Description of the parameters:
 * - \c full_disk_filename : full disk filename. In order to access disks S<small>TXXL</small> uses file
 *   access methods. Each disk is represented as a file. If you have a disk that is mounted in Unix
 *   to the path /mnt/disk0/, then the correct value for the \c full_disk_filename would be
 *   \c /mnt/disk0/some_file_name ,
 * - \c capacity : maximum capacity of the disk in megabytes
 *   (0 means autogrow, file will be deleted afterwards)
 * - \c access_method : \c S<small>TXXL</small> has a number of different
 *   file access implementations for POSIX systems, choose one of them:
 *   - \c syscall : use \c read and \c write system calls which perform disk transfers directly
 *     on user memory pages without superfluous copying (currently the fastest method)
 *   - \c mmap : \c use \c mmap and \c munmap system calls
 *   - \c boostfd : access the file using a Boost file descriptor
 *   - \c fileperblock_syscall, \c fileperblock_mmap, \c fileperblock_boostfd :
 *     same as above, but take a single file per block, using full_disk_filename as file name prefix.
 *     Usually provide worse performance than the standard variants,
 *     but release freed blocks to the file system immediately.
 *   - \c simdisk : simulates timings of the IBM IC35L080AVVA07 disk, full_disk_filename must point
 *     to a file on a RAM disk partition with sufficient space
 *   - \c memory : keeps all data in RAM, for quicker testing
 *   - \c wbtl : library-based write-combining (good for writing small blocks onto SSDs),
 *     based on \c syscall
 *
 * See also the example configuration file \c 'config_example' included in the tarball.
 *
 *
 * \section logfiles Log files
 *
 * \c S<small>TXXL</small> produces two kinds of log files, a message and an error log.
 * By setting the environment variables \c STXXLLOGFILE and \c STXXLERRLOGFILE, you can configure
 * the location of these files.
 * The default values are \c stxxl.log and \c stxxl.errlog, respectively.
 *
 *
 * \section excreation Precreating external memory files
 *
 * In order to get the maximum performance one should precreate disk files described in the configuration file,
 * before running \c S<small>TXXL</small> applications.
 *
 * The precreation utility is included in the set of \c S<small>TXXL</small>
 * utilities ( \c utils/createdisks.bin ). Run this utility
 * for each disk you have defined in the disk configuration file:
 * \verbatim utils/createdisks.bin capacity full_disk_filename... \endverbatim
 *
 * */


/*!
 * \page installation_msvc Installation, usage, configuration (Windows &ndash; Microsoft Visual C++)
 *
 * \section download_msvc Download and Extraction
 *
 * - Install the <a href="http://www.boost.org">Boost</a> libraries (required).
 * - Download the latest \c Stxxl zip file from
 *   <a href="http://sourceforge.net/projects/stxxl/files/stxxl/">SourceForge</a>
 * - Unpack the zip file in some directory (e.&nbsp;g. \c 'C:\\' )
 * - Change to \c stxxl base directory: \c cd \c stxxl-x.y.z
 *
 * \section library_compilation_msvc Library Compilation
 *
 * - Create \c make.settings.local in the base directory according to your system configuration:
 *   - set \c BOOST_ROOT variable according to the Boost root path, e.&nbsp;g.
 *     BOOST_ROOT = "C:\Program Files (x86)\boost\boost_1_40_0"#
 *   - (optionally) set \c STXXL_ROOT variable to \c S<small>TXXL</small> root directory
 *   - (optionally) set \c OPT variable to \c /O2 or any other VC++ optimization level you like,
 *   add -D_SECURE_SCL=0 to switch off iterator checking, which improves performance
 *   - (optionally) set \c DEBUG_COMPILER=/MDd /Zi and DEBUG_LINKER=/DEBUG enable debugging
 * - Open the \c stxxl.vcproj file (VS Solution Object) in Visual Studio.
 *   The file is located in the \c STXXL_ROOT directory
 *   Press F7 to build the library.
 *   The library file (libstxxl.lib) should appear in \c STXXL_ROOT\\lib directory
 *   Or build the library and the S<small>TXXL</small> test programs by pressing Ctrl-Alt-F7
 *   (or choosing from 'Build' drop-down menu Rebuild Solution)
 * - (alternatively) Compile the library by executing \c nmake \c library_msvc
 *   and the tests by executing \c nmake \c tests_msvc,
 *   with all the appropriate environment set (e.&nbsp;g. by using the VS Command Shell)
 *
 *
 * \section build_apps Building an application
 *
 * Programs using Stxxl can be compiled using options from \c compiler.options
 * file (in the \c STXXL_ROOT directory). The linking options for the VC++
 * linker you can find in \c linker.options file. In order to accomplish this
 * do the following:
 * - Open project property pages (menu Project->Properties)
 * - Choose C/C++->Command Line page.
 * - In the 'Additional Options' field insert the contents of the \c compiler.options file.
 * Make sure that the Runtime libraries/debug options (/MDd or /MD or /MT or /MTd) of
 * the \c Stxxl library (see above) do not conflict with the options of your project.
 * Use the same options in the \c Stxxl and your project.
 * - Choose Linker->Command Line page.
 * - In the 'Additional Options' field insert the contents of the \c linker.options file.
 *
 * <br>
 * If you use make files you can
 * include the \c make.settings file in your make files and use \c STXXL_COMPILER_OPTIONS and
 * \c STXXL_LINKER_OPTIONS variables, defined therein.
 *
 * For example: <br>
 * \verbatim cl -c my_example.cpp $(STXXL_COMPILER_OPTIONS) \endverbatim <br>
 * \verbatim link my_example.obj /out:my_example.exe $(STXXL_LINKER_OPTIONS) \endverbatim
 *
 * <br>
 * The \c STXXL_ROOT\\test\\WinGUI directory contains an example MFC GUI project
 * that uses \c Stxxl. In order to compile it open the WinGUI.vcproj file in
 * Visual Studio .NET. Change if needed the Compiler and Linker Options of the project
 * (see above).
 *
 * Before you try to run one of the \c S<small>TXXL</small> examples
 * (or your own \c S<small>TXXL</small> program) you must configure the disk
 * space that will be used as external memory for the library. For instructions how to do that,
 * see the next section.
 *
 *
 * \section space Disk space
 *
 * To get best performance with \c S<small>TXXL</small> you should assign separate disks to it.
 * These disks should be used by the library only.
 * Since \c S<small>TXXL</small> is developed to exploit disk parallelism, the performance of your
 * external memory application will increase if you use more than one disk.
 * But from how many disks your application can benefit depends on how "I/O bound" it is.
 * With modern disk bandwidths
 * of about 50-75 MiB/s most of applications are I/O bound for one disk. This means that if you add another disk
 * the running time will be halved. Adding more disks might also increase performance significantly.
 *
 *
 * \section configuration Disk configuration file
 *
 * You must define the disk configuration for an
 * \c S<small>TXXL</small> program in a file named \c '.stxxl' that must reside
 * in the same directory where you execute the program.
 * You can change the default file name for the configuration
 * file by setting the environment variable \c STXXLCFG .
 *
 * Each line of the configuration file describes a disk.
 * A disk description uses the following format:<br>
 * \c disk=full_disk_filename,capacity,access_method
 *
 * Description of the parameters:
 * - \c full_disk_filename : full disk filename. In order to access disks S<small>TXXL</small> uses file
 *   access methods. Each disk is represented as a file. If you have a disk called \c e:
 *   then the correct value for the \c full_disk_filename would be
 *   \c e:\\some_file_name ,
 * - \c capacity : maximum capacity of the disk in megabytes
 * - \c access_method : \c S<small>TXXL</small> has a number of different
 *   file access implementations for WINDOWS, choose one of them:
 *   - \c syscall : use \c read and \c write POSIX system calls (slow)
 *   - \c wincall: performs disks transfers using \c ReadFile and \c WriteFile WinAPI calls
 *     This method supports direct I/O that avoids superfluous copying of data pages
 *     in the Windows kernel. This is the best (and default) method in Stxxl for Windows.
 *   - \c boostfd : access the file using a Boost file descriptor
 *   - \c fileperblock_syscall, \c fileperblock_wincall, \c fileperblock_boostfd :
 *     same as above, but take a single file per block, using full_disk_filename as file name prefix.
 *     Usually provide worse performance than the standard variants,
 *     but release freed blocks to the file system immediately.
 *   - \c memory : keeps all data in RAM, for quicker testing
 *   - \c wbtl : library-based write-combining (good for writing small blocks onto SSDs),
 *     based on \c syscall
 *
 * See also the example configuration file \c 'config_example_win' included in the archive.
 *
 *
 * \section excreation Precreating external memory files
 *
 * In order to get the maximum performance one should precreate disk files described in the configuration file,
 * before running \c S<small>TXXL</small> applications.
 *
 * The precreation utility is included in the set of \c S<small>TXXL</small>
 * utilities ( \c utils\\createdisks.exe ). Run this utility
 * for each disk you have defined in the disk configuration file:
 * \verbatim utils\createdisks.exe capacity full_disk_filename... \endverbatim
 *
 * */

/*!
 * \page install-svn Installing from subversion
 *
 * \section checkout Retrieving the source from subversion
 *
 * The \c S<small>TXXL</small> source code is available in a subversion repository on sourceforge.net.<br>
 * To learn more about subversion and (command line and graphical) subversion clients
 * visit <a href="http://subversion.tigris.org/">http://subversion.tigris.org/</a>.
 *
 * The main development line (in subversion called the "trunk") is located at
 * \c https://stxxl.svn.sourceforge.net/svnroot/stxxl/trunk
 * <br>Alternatively you might use a branch where a new feature is being developed.
 * Branches have URLs like
 * \c https://stxxl.svn.sourceforge.net/svnroot/stxxl/branches/foobar
 *
 * For the following example let us assume you want to download the latest trunk version
 * using the command line client and store it in a directory called \c stxxl-trunk
 * (which should not exist, yet).
 * Otherwise replace URL and path to your needs.
 *
 * Run: \verbatim svn checkout https://stxxl.svn.sourceforge.net/svnroot/stxxl/trunk stxxl-trunk \endverbatim
 * Change to stxxl directory: \verbatim cd stxxl-trunk \endverbatim
 *
 * \section svn_continue_installation Continue as Usual
 *
 * Now follow the regular installation and usage instructions,
 * starting from "Library Compilation":
 * - \ref library_compilation_linux_gcc "Linux/Unix &ndash; g++/icpc/clang++"
 * - \ref library_compilation_msvc "Windows &ndash; Microsoft Visual C++"
 *
 * \section update Updating an existing subversion checkout
 *
 * Once you have checked out the source code you can easily update it to the latest version later on.
 *
 * In the S<small>TXXL</small> directory, run
 * \verbatim svn update \endverbatim
 * and rebuild.
 * */

/*!
  
\page intro-stream Introduction to the Stream Package

\author Timo Bingmann (2012-06-11)

This page gives a short introduction into the stream package. First the main
abstractions are discussed and then some examples on how to utilize the
existing algorithms are developed.

All example code can be found in stream/test_intro1.cpp

\section stream1 Abstraction, Interface and a Simple Example

The stream package is built around the abstract notion of an object being able
to produce a sequence of output values. Only three simple operations are necessary:
- Retrieval of the current value: prefix * operator
- Advance to the next value in the sequence: prefix ++ operator
- Indication of the sequence's end: empty() function

The most common place object that fits easily into this abstraction is the
random generator. Actually, a random generator only requires two operations: it
can be queried for its current value and be instructed to calculate/advance to
new value. Of course the random sequence should be unbounded, so an empty()
function would always be false. Nevertheless, this common-place example
illustrates the purpose of the stream interface pretty well.

All stream objects must support the three operations above, they form the
stream algorithm concept. In C++ a class conforms to this concept if it
implements the following interface:

\code
struct stream_object
{
    // Type of the values in the output sequence.
    typedef output_type value_type;

    // Retrieval prefix * operator (like dereferencing a pointer or iterator).
    const value_type& operator* () const;

    // Prefix increment ++ operator, which advances the stream to the next value.
    stream_object& operator++ ();

    // Empty indicator. True if the last ++ operation could not fetch a value.
    bool empty() const;
};
\endcode

A very simple stream object that produces the sequence 1,2,3,4,....,1000 is shown in the following snippet:

\code
struct counter_object
{
    // This stream produces a sequence of integers.
    typedef int         value_type;

private:
    // A class attribute to save the current value.
    int                 m_current_value;

public:
    // A constructor to set the initial value to 1.
    counter_object()
        : m_current_value(1)
    {
    }

    // The retrieve operator returning the current value.
    const value_type& operator* () const
    {
        return m_current_value;
    }

    // Increment operator advancing to the next integer.
    counter_object& operator++ ()
    {
        ++m_current_value;
        return *this;
    }

    // Empty indicator, which in this case can check the current value.
    bool empty() const
    {
        return (m_current_value > 1000);
    }
};
\endcode

After this verbose interface definition, the actual iteration over a stream object can be done as follows:

\code
counter_object counter;

while (!counter.empty())
{
    std::cout << *counter << " ";
    ++counter;
}
std::cout << std::endl;
\endcode

For those who like to shorten everything into fewer lines, the above can also be expressed as a for loop:

\code
for (counter_object cnt; !cnt.empty(); ++cnt)
{
    std::cout << *cnt << " ";
}
std::cout << std::endl;
\endcode

Both loops will print the following output:
\verbatim
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 [...] 995 996 997 998 999 1000 
\endverbatim

\section stream2 Pipelining: Plugging Stream Objects Together

The stream interface is so very useful for external memory algorithms because it represents the concept of sequential access to a stream of individual values. While the simple example above only works with integers, the value_type of streams will more often contain complex tuple structs with multiple components.

A stream algorithm can then be constructed from multiple stream objects that pass data from one to another. This notion of "plugging together" stream objects is used in the following example to calculate the square of each value of an integer sequence:

\code
template <typename InputStream>
struct squaring_object
{
    // This stream produces a sequence of integers.
    typedef int         value_type;

private:
    // A reference to another stream of integers, which are our input.
    InputStream&        m_input_stream;

    // A temporary value buffer to hold the current square for retrieval.
    value_type          m_current_value;

public:
    // A constructor taking another stream of integers as input.
    squaring_object(InputStream& input_stream)
        : m_input_stream(input_stream)
    {
        if (!m_input_stream.empty())
        {
            m_current_value = *m_input_stream;
            m_current_value = m_current_value * m_current_value;
        }
    }

    // The retrieve operator returning the square of the input stream.
    const value_type& operator* () const
    {
        return m_current_value;
    }

    // Increment operator: handled by incrementing the input stream.
    squaring_object& operator++ ()
    {
        ++m_input_stream;
        if (!m_input_stream.empty())
        {
            m_current_value = *m_input_stream;
            m_current_value = m_current_value * m_current_value;
        }
        return *this;
    }

    // Empty indicator: this stream is empty when the input stream is.
    bool empty() const
    {
        return m_input_stream.empty();
    }
};
\endcode

For a beginner in stream object programming, the squaring example contains multiple unexpected, verbose complications.

- We wish to allow many different integer sequences as input streams to the squaring class. For this we use template meta-programming and define squaring to take any class as InputStream template parameter. As yet, in C++ we cannot syntactically define which concepts the template parameters must fulfill, in this case one would require InputStream to implement the stream interface.

- After defining the input stream class, one will usually need an instantiated object of that class inside the new stream class. Most common practice is to define references to other streams as class attributes, and have the actual objects be passed to the constructor of the new stream object. <br> In the case of the squaring class, any InputStream object is accepted by the constructor and a reference is saved into m_input_stream.

- As second attribute, the squaring class contains m_current_value. The additional temporary value is required in this case because operator*() must return a const-reference, so the square must actually be stored in a variable after it is calculated. Now note that the squaring operation in this version is implemented at two places: in the constructor and the operator++(). <br> This is necessary, because the stream concept requires that the first value be <em>immediately available after construction</em>! Therefore it must be calculated in the constructor, and this code is usually a duplicate to the action done in operator++(). A real implementation would probably combine the calculation code into a process() function and also do additional allocation work in the constructor.

An instance of the counter_object can be plugged into a squaring_object as done in the following example:

\code
counter_object counter;
squaring_object<counter_object> squares(counter);

while (!squares.empty())
{
    std::cout << *squares << " ";
    ++squares;
}
std::cout << std::endl;
\endcode

The example outputs:

\verbatim
1 4 9 16 25 36 49 64 81 100 121 144 169 [...] 986049 988036 990025 992016 994009 996004 998001 1000000
\endverbatim

\section stream3 Miscellaneous Utilities Provided by the Stream Package

The above examples are pure C++ interface manipulations and do not even require stxxl. However, when writing stream algorithms you can take advantage of the utilities provided by the stream package to create complex algorithms. Probably the most useful is the pair of sorting classes, which will be discussed after a few preliminaries.

More complex algorithms will most often use tuples as values passed from one stream to another. These tuples wrap all information fields of a specific piece of data. Simple tuples can be created using std::pair, tuples with larger number of components can use Boost.Tuple or just plain structs with multiple fields. (In the tuple case, the temporary value inside the stream struct can mostly be avoided.)

The stream package contains utilities to plug stream classes together to form complex algorithms. The following few examples are very basic algorithms:

Very often the input to a sequence of stream classes comes from an array or other container. In this case one requires an input stream object, which iterates through the container and outputs each element once. Stxxl provides iterator2stream for this common purpose:
\code
std::vector<int> intvector;
// (fill intvector)

// define stream class iterating over an integer vector
typedef stxxl::stream::iterator2stream< std::vector<int>::const_iterator > intstream_type;

// instantiate the stream object, iterate from begin to end of intvector.
intstream_type intstream (intvector.begin(), intvector.end());

// plug in squaring object after vector iterator stream.
squaring_object<intstream_type> squares(intstream);
\endcode

Most important: if the input container is a stxxl::vector, then one should use vector_iterator2stream, because this class will prefetch additional blocks from the vector while processing the stream.
\code
stxxl::vector<int> intvector;
// (fill intvector)

// define stream class iterating over an integer STXXL vector
typedef stxxl::stream::vector_iterator2stream< stxxl::vector<int>::const_iterator > intstream_type;

// instantiate the stream object, iterate from begin to end of intvector using prefetching
intstream_type intstream (intvector.begin(), intvector.end());

// plug in squaring object after vector iterator stream.
squaring_object<intstream_type> squares(intstream);
\endcode

The opposite to iterator2stream is to collect the output of a sequence of stream objects into a container or stxxl vector. This operation is called materialize and also comes in the general version and a special version for the STXXL-vector, which uses asynchronous writes.

This example shows how to materialize a stream into a usual STL vector.
\code
// construct the squared counter stream
counter_object counter;
squaring_object<counter_object> squares(counter);

// allocate vector of 100 integers
std::vector<int> intvector (100);

// materialize 100 integers from stream and put into vector
stxxl::stream::materialize(squares, intvector.begin(), intvector.end());
\endcode

And the only modification needed to support larger data sets is to materialize to an STXXL vector:
\code
// construct the squared counter stream
counter_object counter;
squaring_object<counter_object> squares(counter);

// allocate STXXL vector of 100 integers
stxxl::vector<int> intvector (100);

// materialize 100 integers from stream and put into STXXL vector
stxxl::stream::materialize(squares, intvector.begin(), intvector.end());
\endcode

\section stream4 Sorting As Provided by the Stream Package

Maybe the most important set of tools in the stream package is the pairs of sorter classes runs_creator and runs_merger. The general way to sort a sequential input stream is to first consolidate a large number of input items in an internal memory buffer. Then when the buffer is full, it can be sorted in internal memory and subsequently written out to disk. This sorted sequence is then called a run. When the input stream is finished and the sorted output must be produced, theses sorted sequences can efficiently be merged using a tournament tree or similar multi-way comparison structure.

STXXL implements this using two stream classes: runs_creator and runs_merger.

The following examples shows how to sort the integer sequence 1,2,...,1000 first by the right-most decimal digit, then by its absolute value (yes a somewhat constructed example, but it serves its purpose well.) For all sorters a comparator object is required which tells the sorter which of two objects is the smaller one. This is similar to the requirements of the usual STL, however, the STXXL sorters need to additional functions: min_value() and max_value() which are used as padding sentinels. These functions return the smallest and highest possible values of the given data type.
\code
// define comparator class: compare right-most decimal and then absolute value
struct CompareMod10
{
    // comparison operator() returning true if (a < b)
    inline bool operator() (int a, int b) const
    {
        if ((a % 10) == (b % 10))
            return a < b;
        else
            return (a % 10) < (b % 10);
    }

    // smallest possible integer value
    int min_value() const { return INT_MIN; }
    // largest possible integer value
    int max_value() const { return INT_MAX; }
};
\endcode

All sorters steps require an internal memory buffer. This size can be fixed using a parameter to runs_creator and runs_merger.
The following example code instantiates a counter object, plugs this into a runs_creator which is followed by a runs_merger.

\code
static const int ram_use = 10*1024*1024;   // amount of memory to use in runs creation

counter_object  counter;        // the counter stream from first examples

// define a runs sorter for the counter stream which order by CompareMod10 object.
typedef stxxl::stream::runs_creator<counter_object, CompareMod10> rc_counter_type;

// instance of CompareMod10 comparator class
CompareMod10    comparemod10;

// instance of runs_creator which reads the counter stream.
rc_counter_type rc_counter (counter, comparemod10, ram_use);

// define a runs merger for the sorted runs from rc_counter.
typedef stxxl::stream::runs_merger<rc_counter_type::sorted_runs_type, CompareMod10> rm_counter_type;

// instance of runs_merger which merges sorted runs from rc_counter.
rm_counter_type rm_counter (rc_counter.result(), comparemod10, ram_use);

// read sorted stream: runs_merger also conforms to the stream interface.
while (!rm_counter.empty())
{
    std::cout << *rm_counter << " ";
    ++rm_counter;
}
std::cout << std::endl;
\endcode
The output of the code above is:
\verbatim
10 20 30 40 50 60 70 80 [...] 990 1000 1 11 21 31 41 51 61 [...] 909 919 929 939 949 959 969 979 989 999
\endverbatim

Note that in the above example the input of the runs_creator is itself a stream. If however the data is not naturally available as a stream, one can use a variant of runs_creator which accepts input via a push() function. This is more useful when using an imperative programming style. Note that the runs_merger does not change.
\code
static const int ram_use = 10*1024*1024;   // amount of memory to use in runs creation

// define a runs sorter which accepts imperative push()s and orders by CompareMod10 object.
typedef stxxl::stream::runs_creator<stxxl::stream::use_push<int>, CompareMod10> rc_counter_type;

// instance of CompareMod10 comparator class.
CompareMod10    comparemod10;

// instance of runs_creator which waits for input.
rc_counter_type rc_counter (comparemod10, ram_use);

// write sequence of integers into runs
for (int i = 1; i <= 1000; ++i)
    rc_counter.push(i);

// define a runs merger for the sorted runs from rc_counter.
typedef stxxl::stream::runs_merger<rc_counter_type::sorted_runs_type, CompareMod10> rm_counter_type;

// instance of runs_merger which merges sorted runs from rc_counter.
rm_counter_type rm_counter (rc_counter.result(), comparemod10, ram_use);

// read sorted stream: runs_merger also conforms to the stream interface.
while (!rm_counter.empty())
{
    std::cout << *rm_counter << " ";
    ++rm_counter;
}
std::cout << std::endl;
\endcode

And as the last example in this tutorial we show how to use stxxl::sorter, which combines runs_creator and runs_merger into one object. The sorter has two states: input and output. During input, new elements can be sorted using push(). Then to switch to output state, the function sort() is called, after which the sorter can be queried using the usual stream interface.
\code
static const int ram_use = 10*1024*1024;   // amount of memory to use in runs creation

// define a runs sorter which accepts imperative push()s and orders by CompareMod10 object.
typedef stxxl::sorter<int, CompareMod10> sr_counter_type;

// instance of CompareMod10 comparator class.
CompareMod10    comparemod10;

// instance of sorter which waits for input.
sr_counter_type sr_counter (comparemod10, ram_use);

// write sequence of integers into sorter, which creates sorted runs
for (int i = 1; i <= 1000; ++i)
    sr_counter.push(i);

// signal sorter that the input stream is finished and switch to output mode.
sr_counter.sort();

// read sorted stream: sorter also conforms to the stream interface.
while (!sr_counter.empty())
{
    std::cout << *sr_counter << " ";
    ++sr_counter;
}
std::cout << std::endl;
\endcode

All three examples have the same output.

 */

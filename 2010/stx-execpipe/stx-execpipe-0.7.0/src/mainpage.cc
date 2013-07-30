
The STX ExecPipe library provides a convenient C++ interface to execute child
programs connected via pipes. It is a front-end to the system calls fork(),
pipe(), select() and execv() and hides all the complexity of these low-level
functions. It allows a program to build a sequence of connected children
programs with input and output of the pipe sequence redirected to a file,
string or file descriptor. The library also allows custom asynchronous data
processing classes to be inserted into the pipe or placed at source or sink of
the sequence.

An execution pipe consists of an input stream, a number of pipe stages and an
output stream. The input and output streams can be a plain file descriptor, a
file, a std::string or a special processing class. Each pipe stage is either an
executed child program or an intermediate function class. At the junction
between each stage in the pipeline the following program's stdin is connected
to the preceding stage's stdout. The input and output streams are connected to
the start and end of the pipe line.

<table style="margin: 0 auto; border: 0px"><tr><td>
<pre>
 Input Stream                   Pipe Stages                   Output Stream
     none    |                                                |    none
      fd     |                 exec()                         |     fd
     file    |--> stage -->      or      --> stage --> ... -->|    file
    string   |              PipeFunction                      |   string
  PipeSource |                                                |  PipeSink
</pre>
</td></tr></table>

All this functionality is wrapped into a flexible C++ class, which can be used
in an application to construct complex sequences of external programs similar
to shell piping. Some common operations would be calls of mkisofs or tar
coupled with gzip or gpg and possibly send the output to a remote host via ssh
or ncftpput.

\section sec_usage Library Usage Tutorial

The following tutorial shows some simple examples on how an execution pipe can
be set up.

To use the library a program must

\code
#include "stx-execpipe.h"
\endcode

and later link against libstx-execpipe.a or include the corresponding .o / .cc
in the project's dependencies.

To run a sequence of programs you must first initialize a new ExecPipe
object. The ExecPipe object is referenced counted so you can easily pass it
around without deep-duplicating the object.

\code
stx::ExecPipe ep;	      	// creates new pipe

stx::ExecPipe ep_ref1 = ep;   	// reference to the same pipe.
\endcode

Once created the input stream source can be set using one of the four
set_input_*() functions. Note that these are mutually exclusive, you must call
at most one of the following functions!

\code
// you can designate an existing file as input stream
ep.set_input_file("/path/to/file");

// or directly assign an already opened file descriptor
int fd = ...;
ep.set_input_fd(fd);

// or pass the contents of a std::string as input
std::string str = ...;
ep.set_input_string(&str);

// or attach a data generating source class (details later).
PipeSource source;
ep.set_input_source(&source);
\endcode

The input stream objects are _not_ copied. The fd, string or source object must
still exist when calling run().

After setting up the input you specify the individual stages in the pipe by
adding children programs to exec() or function classes. The stx::ExecPipe
provides different variants of add_exec*(), which are derived from the exec*()
system call variants.

\code
// add simple exec() call with full path.
ep.add_exec("/bin/cat");

// add exec() call with up to three direct parameters.
ep.add_exec("/bin/echo", "one", "two", "three");

// add exec() call with many parameters. the vector is _not_ copied.
std::vector<std::string> tarargs;
tarargs.push_back("/bin/tar");
tarargs.push_back("--create");
tarargs.push_back("--verbose");
tarargs.push_back("--gzip");
tarargs.push_back("--file");
tarargs.push_back("/path/to/file");
ep.add_exec(&tarargs);

// add execp() call which searches $PATH. see man 3 execvp.
ep.add_execp("cat");

// same with up to three parameters.
ep.add_execp("echo", "one", "two", "three");

// and also works with a vector of arguments.
ep.add_execp(&tarargs);

// most versatile function: call execve() with program name, argv[] arguments
// and a set of environment variables.
std::vector<std::string> gzipargs;
gzipargs.push_back("gunzip");		// this changes argv[0]

std::vector<std::string> gzipenvs;	// set environment variable
gzipenvs.push_back("GZIP=-d --name");

ep.add_exece("/bin/gzip", &gzipargs, &gzipenvs);

// insert an intermediate data processing class into the pipe (details later).
PipeFunction function;
ep.add_function(&function);
\endcode

After configuring the pipe stages the user program can redirect the pipe's
output using one of the four set_output_*() functions. These correspond directly
the to input functions.

\code
// designate a file as output, it will be over-written,
ep.set_output_file("/path/to/file");

// or directly assign an already opened file descriptor
int fd = ...;
ep.set_output_fd(fd);

// or save output in a std::string object
std::string str = ...;
ep.set_output_string(&str);

// or attach a sink class (details later).
PipeSink sink;
ep.set_output_sink(&sink);
\endcode

The three steps above can be done in any order. Once the pipeline is configured
as required, a call to run() will set up the input and output file descriptors,
launch all children programs, wait until these finish and concurrently process
data passed between parent and children.

If any system calls fail while running the pipe, the run() function will
throw() a std::runtime_error exception. So wrap run() in a try-catch block.

\code
try {
    ep.run();
}
catch (std::runtime_error &e) {
    std::cerr << "Pipe execution failed: " << e.what() << std::endl;
}
\endcode

After running all children their return status should be checked. These can be
inspected using the following functions. The integer parameter specifies the
exec stage in the pipe sequence.

\code
// get plain return status as indicated by wait().
int rs = ep.get_return_status(0)

// get return code for normally terminated program.
int rc = ep.get_return_code(1);

// get signal for abnormally terminated program (like segfault).
int rg = ep.get_return_signal(1);
\endcode

Most program have a return code of 0 when no error occurred. Therefore, a
convenience function is available which checks whether all program stages
returned zero. This is what would usually be used.

\code
// check all that program returned zero
if (ep.all_return_codes_zero()) {
    // run was ok.
}
else {
    // error handling.
}
\endcode

After checking the return error codes the pipe's results can be used.

The tarball contains three simple examples of using the different exec()
variants and input/output redirections. See \ref simple1.cc
"examples/simple1.cc", \ref simple2.cc "examples/simple2.cc" or \ref simple3.cc
"examples/simple3.cc". More a more elaborate example using data processing
classes see the continued tutorial below.

\section sec_functionals Data Processing Classes

One of the big features of the STX ExecPipe classes is the ability to insert
intermediate asynchronous data processing classes into the pipe sequence. The
data of the pipe line is returned to the parent process and, after arbitrary
computations, can be sent on to the following execution stages. Besides
intermediate processing, the input and output stream can be attached to source
or sink classes.

This feature can be used to generate input data, e.g. binary data or file
listing, or peek at the data flowing between stages, e.g. to compute a SHA1
digest, or to directly processes output data while the children are running.

The data processing classes must be derived from one of the three abstract
classes: stx::PipeSource for generating input streams, stx::PipeFunction for
intermediate processing between stages or stx::PipeSink for receiving output.

For generating an input stream a class must derive from stx::PipeSource and
implement the \ref stx::PipeSource::poll "poll()" function. This function is
called when new data can be pushed into the pipe. When \ref
stx::PipeSource::poll "poll()" is called, new data must be generated and
delivered via the \ref stx::PipeSource::write "write()" function of
stx::PipeSource. If more data is available \ref stx::PipeSource::poll "poll()"
must return true, otherwise the input stream is terminated.

Intermediate data processing classes must derive from stx::PipeFunction and
implement the two pure virtual function \ref stx::PipeFunction::process
"process()" and \ref stx::PipeFunction::eof "eof()". As the name suggests, data
is delivered to the class via the \ref stx::PipeFunction::process "process()"
function. After processing the data it may be forwarded to the next pipe stage
via the inherited\ref stx::PipeFunction::write "write()" function. Note that
the library does not automatically forward data, so if you forget to write()
data, then the following stage does not receive anything. When the preceding
processing stage closes its data stream the function \ref
stx::PipeFunction::eof "eof()" is called.

To receive the output stream a class must derive from stx::PipeSink. Similar to
stx::PipeFunction, an output sink must implement the two pure virtual function
\ref stx::PipeFunction::process "process()" and \ref stx::PipeFunction::eof
"eof()". However, different from an intermediate class the stx::PipeSink does
not provide a write() function, so no data can be forwarded.

For a full example of using stx::PipeSource to iterate through a file list and
stx::PipeFunction to compute an intermediate SHA1 digest see \ref functions1.cc
"examples/functions1.cc".

*/

/**
 * \example simple1.cc
 *
 * Shows a simple setup which calls "echo" and saves the output to a string.
 */

/**
 * \example simple2.cc
 *
 * Shows a simple setup which writes a string to "sha1sum" and saves the output
 * to a string.
 */

/**
 * \example simple3.cc
 *
 * Shows a simple setup which lists "/bin", pipes the outputs to "grep" and
 * "sort" and saves the output to a string.
 */

/**
 * \example functions1.cc
 *
 * This example shows how to use stx::PipeSource to tar a list of file names
 * and inserts an intermediate processing class, which calculates the SHA1
 * digest of the uncompressed tarball before gzipping it..
 */

//  LocalWords:  redirections

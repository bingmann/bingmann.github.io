/*************************************************
* Module Factory Source File                     *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#include "botan-1.6/include/modules.h"
#include "botan-1.6/include/defalloc.h"
#include "botan-1.6/include/def_char.h"
#include "botan-1.6/include/eng_def.h"
#include "botan-1.6/include/es_file.h"
#include "botan-1.6/include/timers.h"

#if defined(BOTAN_EXT_MUTEX_PTHREAD)
  #include "botan-1.6/include/mux_pthr.h"
#elif defined(BOTAN_EXT_MUTEX_WIN32)
  #include "botan-1.6/include/mux_win32.h"
#elif defined(BOTAN_EXT_MUTEX_QT)
  #include "botan-1.6/include/mux_qt.h"
#endif

#if defined(BOTAN_EXT_ALLOC_MMAP)
  #include "botan-1.6/include/mmap_mem.h"
#endif

#if defined(BOTAN_EXT_TIMER_HARDWARE)
  #include "botan-1.6/include/tm_hard.h"
#elif defined(BOTAN_EXT_TIMER_POSIX)
  #include "botan-1.6/include/tm_posix.h"
#elif defined(BOTAN_EXT_TIMER_UNIX)
  #include "botan-1.6/include/tm_unix.h"
#elif defined(BOTAN_EXT_TIMER_WIN32)
  #include "botan-1.6/include/tm_win32.h"
#endif

#if defined(BOTAN_EXT_ENGINE_AEP)
  #include "botan-1.6/include/eng_aep.h"
#endif

#if defined(BOTAN_EXT_ENGINE_GNU_MP)
  #include "botan-1.6/include/eng_gmp.h"
#endif

#if defined(BOTAN_EXT_ENGINE_OPENSSL)
  #include "botan-1.6/include/eng_ossl.h"
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_AEP)
  #include "botan-1.6/include/es_aep.h"
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_EGD)
  #include "botan-1.6/include/es_egd.h"
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_UNIX)
  #include "botan-1.6/include/es_unix.h"
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_BEOS)
  #include "botan-1.6/include/es_beos.h"
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_CAPI)
  #include "botan-1.6/include/es_capi.h"
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_WIN32)
  #include "botan-1.6/include/es_win32.h"
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_FTW)
  #include "botan-1.6/include/es_ftw.h"
#endif

namespace Enctain {
namespace Botan {

/*************************************************
* Return a mutex factory, if available           *
*************************************************/
Mutex_Factory* Builtin_Modules::mutex_factory() const
   {
#if defined(BOTAN_EXT_MUTEX_PTHREAD)
   return new Pthread_Mutex_Factory;
#elif defined(BOTAN_EXT_MUTEX_WIN32)
   return new Win32_Mutex_Factory;
#elif defined(BOTAN_EXT_MUTEX_QT)
   return new Qt_Mutex_Factory;
#else
   return 0;
#endif
   }

/*************************************************
* Find a high resolution timer, if possible      *
*************************************************/
Timer* Builtin_Modules::timer() const
   {
#if defined(BOTAN_EXT_TIMER_HARDWARE)
   return new Hardware_Timer;
#elif defined(BOTAN_EXT_TIMER_POSIX)
   return new POSIX_Timer;
#elif defined(BOTAN_EXT_TIMER_UNIX)
   return new Unix_Timer;
#elif defined(BOTAN_EXT_TIMER_WIN32)
   return new Win32_Timer;
#else
   return new Timer;
#endif
   }

/*************************************************
* Find any usable allocators                     *
*************************************************/
std::vector<Allocator*> Builtin_Modules::allocators() const
   {
   std::vector<Allocator*> allocators;

#if defined(BOTAN_EXT_ALLOC_MMAP)
   allocators.push_back(new MemoryMapping_Allocator);
#endif

   allocators.push_back(new Locking_Allocator);
   allocators.push_back(new Malloc_Allocator);

   return allocators;
   }

/*************************************************
* Return the default allocator                   *
*************************************************/
std::string Builtin_Modules::default_allocator() const
   {
   if(should_lock)
      {
#if defined(BOTAN_EXT_ALLOC_MMAP)
      return "mmap";
#else
      return "locking";
#endif
      }
   else
      return "malloc";
   }

/*************************************************
* Register any usable entropy sources            *
*************************************************/
std::vector<EntropySource*> Builtin_Modules::entropy_sources() const
   {
   std::vector<EntropySource*> sources;

   sources.push_back(new File_EntropySource);

#if defined(BOTAN_EXT_ENTROPY_SRC_AEP)
   sources.push_back(new AEP_EntropySource);
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_EGD)
   sources.push_back(new EGD_EntropySource);
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_CAPI)
   sources.push_back(new Win32_CAPI_EntropySource);
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_WIN32)
   sources.push_back(new Win32_EntropySource);
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_UNIX)
   sources.push_back(new Unix_EntropySource);
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_BEOS)
   sources.push_back(new BeOS_EntropySource);
#endif

#if defined(BOTAN_EXT_ENTROPY_SRC_FTW)
   sources.push_back(new FTW_EntropySource);
#endif

   return sources;
   }

/*************************************************
* Find any usable engines                        *
*************************************************/
std::vector<Engine*> Builtin_Modules::engines() const
   {
   std::vector<Engine*> engines;

   if(use_engines)
      {
#if defined(BOTAN_EXT_ENGINE_AEP)
      engines.push_back(new AEP_Engine);
#endif

#if defined(BOTAN_EXT_ENGINE_GNU_MP)
      engines.push_back(new GMP_Engine);
#endif

#if defined(BOTAN_EXT_ENGINE_OPENSSL)
      engines.push_back(new OpenSSL_Engine);
#endif
      }

   engines.push_back(new Default_Engine);

   return engines;
   }

/*************************************************
* Find the best transcoder option                *
*************************************************/
Charset_Transcoder* Builtin_Modules::transcoder() const
   {
   return new Default_Charset_Transcoder;
   }

/*************************************************
* Builtin_Modules Constructor                    *
*************************************************/
Builtin_Modules::Builtin_Modules(const InitializerOptions& args) :
   should_lock(args.secure_memory()),
   use_engines(args.use_engines())
   {
   }

}
}

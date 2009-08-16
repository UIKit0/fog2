// [Fog/Core Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_LOCK_H
#define _FOG_CORE_LOCK_H

// [Dependencies]
#include <Fog/Build/Build.h>

#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/TypeInfo.h>
#include <Fog/Core/Constants.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
#include <pthread.h>
#endif // FOG_OS_POSIX

//! @addtogroup Fog_Core
//! @{

#if defined(FOG_OS_POSIX)
FOG_CVAR_EXTERN pthread_mutexattr_t lock_recursive_attrs;
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ThreadCondition;

// ============================================================================
// [Fog::Lock]
// ============================================================================

struct FOG_HIDDEN Lock
{
#if defined(FOG_OS_WINDOWS)
  typedef CRITICAL_SECTION Handle;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  typedef pthread_mutex_t Handle;
#endif // FOG_OS_POSIX

private:
  Handle _handle;

public:
  FOG_INLINE Lock()
  {
#if defined(FOG_OS_WINDOWS)
    InitializeCriticalSection(&_handle);
    // InitializeCriticalSectionAndSpinCount(&_handle, 2000);
#endif // FOG_OS_WINDOWS
#if defined(FOG_OS_POSIX)
    pthread_mutex_init(&_handle, &lock_recursive_attrs);
#endif // FOG_OS_POSIX
  }

  FOG_INLINE ~Lock()
  {
#if defined(FOG_OS_WINDOWS)
    DeleteCriticalSection(&_handle);
#endif // FOG_OS_WINDOWS
#if defined(FOG_OS_POSIX)
    pthread_mutex_destroy(&_handle);
#endif // FOG_OS_POSIX
  }

  FOG_INLINE Handle& getHandle()
  {
    return _handle;
  }

  FOG_INLINE const Handle& getHandle() const
  {
    return _handle;
  }

  FOG_INLINE void lock()
  { 
#if defined(FOG_OS_WINDOWS)
    EnterCriticalSection(&_handle);
#endif // FOG_OS_WINDOWS
#if defined(FOG_OS_POSIX)
    pthread_mutex_lock(&_handle);
#endif // FOG_OS_POSIX
  }

  FOG_INLINE void unlock()
  {
#if defined(FOG_OS_WINDOWS)
    LeaveCriticalSection(&_handle);
#endif // FOG_OS_WINDOWS
#if defined(FOG_OS_POSIX)
    pthread_mutex_unlock(&_handle);
#endif // FOG_OS_POSIX
  }

  FOG_INLINE bool tryLock()
  {
#if defined(FOG_OS_WINDOWS)
    return TryEnterCriticalSection(&_handle) != 0;
#endif // FOG_OS_WINDOWS
#if defined(FOG_OS_POSIX)
    return pthread_mutex_trylock(&_handle) != 0;
#endif // FOG_OS_POSIX
  }

private:
  friend struct ThreadCondition;

  FOG_DISABLE_COPY(Lock)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_LOCK_H

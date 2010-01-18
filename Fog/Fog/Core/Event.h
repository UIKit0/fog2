// [Fog/Core Library - Public API]
//
// [Licence]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_EVENT_H
#define _FOG_CORE_EVENT_H

// [Dependencies]
#include <Fog/Core/String.h>
#include <Fog/Core/Task.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Object;
struct Timer;

// ============================================================================
// [Fog::Event]
// ============================================================================

//! @brief Structure that contains information about event.
//!
//! Used together with Fog::Object event model.
struct FOG_API Event : Task
{
  // [Event allocation]

  //! @brief Event constructor.
  Event(uint32_t code = 0, uint32_t flags = 0);

  //! @brief Internal run method that's inherited from task. This method
  //! is only called when event is posted into task queue to call onEvent
  //! method in object, so it's not called by Object::sendEvent().
  virtual void run();

  //! @brief Returns cloned event.
  //!
  //! Cloned events are used when sending events to different
  //! threads.
  virtual Event* clone() const;

  //! @brief Enumeration used for @c _flags member.
  enum FLAGS
  {
    //! @brief @c true if event is dynamically allocated (all posted events).
    IS_DYNAMIC = (1 << 0),
    //! @brief @c true if event is generated by system.
    IS_SYSTEM = (1 << 1),
    //! @brief Event was posted to queue.
    IS_POSTED = (1 << 2),
    //! @brief @c true if event was accepted.
    IS_ACCEPTED = (1 << 3),
    //! @brief @c true if event was propagated.
    IS_PROPAGATED = (1 << 4)
  };

  //! @brief Returns the event code, see @c Code_Enum for possible codes.
  FOG_INLINE uint32_t getCode() const { return _code; }
  //! @brief Returns the event flags.
  FOG_INLINE uint32_t getFlags() const { return _flags; }

  //! @brief Returns @c true if event was allocated on the heap.
  FOG_INLINE bool isDynamic() const { return (_flags & IS_DYNAMIC) != 0; }
  //! @brief Returns @c true if event was generated by operating or windowing system.
  FOG_INLINE bool isSystem() const { return (_flags & IS_SYSTEM) != 0; }
  //! @brief Returns @c true if event was posted.
  FOG_INLINE bool isPosted() const { return (_flags &IS_POSTED) != 0; }
  //! @brief Returns @c true if event was accepted.
  FOG_INLINE bool isAccepted() const { return (_flags & IS_ACCEPTED) != 0; }
  //! @brief Returns @c true if event was propagated.
  FOG_INLINE bool isPropagated() const { return (_flags & IS_PROPAGATED) != 0; }

  //! @brief Accept event.
  FOG_INLINE void accept() { _flags |= IS_ACCEPTED; }

  //! @brief Ignore event.
  FOG_INLINE void ignore() { _flags &= ~IS_ACCEPTED; }

  //! @brief Sets propagated bit to @c true.
  FOG_INLINE void propagate() { _flags |= IS_PROPAGATED; }

  //! @brief Event receiver.
  FOG_INLINE Object* getReceiver() const { return _receiver; }

  FOG_INLINE uint32_t wasDeleted() const { return _wasDeleted; }

  //! @brief Generates uniue event id.
  static uint32_t uid();

protected:
  FOG_INLINE Event* _cloned(Object* receiver)
  {
    _receiver = receiver;
    return this;
  }

  FOG_INLINE void setCode(uint32_t code) { _code = code; }
  FOG_INLINE void setReceiver(Object* receiver) { _receiver = receiver; }

  // [Event members]

public:
  //! @brief Event code.
  uint32_t _code;
  //! @brief Event flags.
  uint32_t _flags;

  //! @brief Object that will receive the event.
  Object* _receiver;

private:
  //! @brief Link to previous receiver event.
  Event* _prev;
  //! @brief Link to next receiver event in queue.
  Event* _next;

protected:
  //! @brief Receiver was deleted. 
  //!
  //! If this flag is true, the event will be discarded instead of dispatched.
  //! This flag is set by receiver object destructor.
  uint32_t _wasDeleted;

public:
  friend struct Object;
  friend struct Thread;
  friend struct Timer;
};

FOG_INLINE Event::Event(uint32_t code, uint32_t flags)
{
  _code = code;
  _flags = flags;
  _receiver = NULL;
  _prev = NULL;
  _next = NULL;
  _wasDeleted = 0;
}

// ============================================================================
// [Fog::CreateEvent]
// ============================================================================

// CreateEvent conflicts with WinAPI function "CreateEvent" that's
// redefined to CreateEventA or CreateEventW
#if defined CreateEvent
#undef CreateEvent
#endif // CreateEvent

struct FOG_API CreateEvent : public Event
{
  CreateEvent();
};

// ============================================================================
// [Fog::DestroyEvent]
// ============================================================================

struct FOG_API DestroyEvent : public Event
{
  DestroyEvent();

  virtual void run();
};

// ============================================================================
// [Fog::TimerEvent]
// ============================================================================

struct FOG_API TimerEvent : public Event
{
  TimerEvent(Timer* timer = NULL);

  FOG_INLINE Timer* getTimer() const { return _timer; }

  virtual Event* clone();

  Timer* _timer;

private:
  friend struct Timer;
};

// ============================================================================
// [Fog::PropertyChangeEvent]
// ============================================================================

struct FOG_API PropertyChangedEvent : public Event
{
public:
  PropertyChangedEvent(const String& name);
  virtual ~PropertyChangedEvent();
  virtual Event* clone() const;

  FOG_INLINE const String& getName() const { return _name; }

private:
  String _name;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_EVENT_H

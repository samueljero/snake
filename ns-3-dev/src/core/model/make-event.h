#ifndef MAKE_EVENT_H
#define MAKE_EVENT_H

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

namespace ns3 {

class EventImpl;

template <typename MEM, typename OBJ>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj);

template <typename MEM, typename OBJ,
          typename T1>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj, T1 a1);

template <typename MEM, typename OBJ,
          typename T1, typename T2>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj, T1 a1, T2 a2);

template <typename MEM, typename OBJ>
EventImpl * MakeSpecialEvent (MEM mem_ptr, OBJ obj, uint8_t* buf, ssize_t len);

template <typename MEM, typename OBJ,
          typename T1, typename T2, typename T3>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj, T1 a1, T2 a2, T3 a3);

template <typename MEM, typename OBJ,
          typename T1, typename T2, typename T3, typename T4>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj, T1 a1, T2 a2, T3 a3, T4 a4);

template <typename MEM, typename OBJ,
          typename T1, typename T2, typename T3, typename T4, typename T5>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj,
                       T1 a1, T2 a2, T3 a3, T4 a4, T5 a5);

EventImpl * MakeEvent (void (*f)(void));
template <typename U1,
          typename T1>
EventImpl * MakeEvent (void (*f)(U1), T1 a1);

template <typename U1, typename U2,
          typename T1, typename T2>
EventImpl * MakeEvent (void (*f)(U1,U2), T1 a1, T2 a2);

template <typename U1, typename U2>
EventImpl * MakeSpecialEvent (void (*f)(U1,U2), uint8_t *buf, ssize_t len);

template <typename U1, typename U2, typename U3,
          typename T1, typename T2, typename T3>
EventImpl * MakeEvent (void (*f)(U1,U2,U3), T1 a1, T2 a2, T3 a3);

template <typename U1, typename U2, typename U3, typename U4,
          typename T1, typename T2, typename T3, typename T4>
EventImpl * MakeEvent (void (*f)(U1,U2,U3,U4), T1 a1, T2 a2, T3 a3, T4 a4);

template <typename U1, typename U2, typename U3, typename U4, typename U5,
          typename T1, typename T2, typename T3, typename T4, typename T5>
EventImpl * MakeEvent (void (*f)(U1,U2,U3,U4,U5), T1 a1, T2 a2, T3 a3, T4 a4, T5 a5);

} // namespace ns3

/********************************************************************
   Implementation of templates defined above
 ********************************************************************/

#include "event-impl.h"
#include "type-traits.h"

namespace ns3 {

template <typename T>
struct EventMemberImplObjTraits;

template <typename T>
struct EventMemberImplObjTraits<T *>
{
  static T &GetReference (T *p)
  {
    return *p;
  }
};

template <typename MEM, typename OBJ>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj)
{
  // zero argument version
  class EventMemberImpl0 : public EventImpl
  {
public:
    EventMemberImpl0 (OBJ obj, MEM function)
      : m_obj (obj),
        m_function (function)
    {
    }
    virtual ~EventMemberImpl0 ()
    {
    }
		EventMemberImpl0* clone() { return new EventMemberImpl0(*this);}

private:
    virtual void Notify (void)
    {
      (EventMemberImplObjTraits<OBJ>::GetReference (m_obj).*m_function)();
    }
    OBJ m_obj;
    MEM m_function;
  } *ev = new EventMemberImpl0 (obj, mem_ptr);
  return ev;
}


template <typename MEM, typename OBJ,
          typename T1>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj, T1 a1)
{
  // one argument version
  class EventMemberImpl1 : public EventImpl
  {
public:
    EventMemberImpl1 (OBJ obj, MEM function, T1 a1)
      : m_obj (obj),
        m_function (function),
        m_a1 (a1)
    {
    }
    EventMemberImpl1* clone() {  return new EventMemberImpl1(*this);}
protected:
    virtual ~EventMemberImpl1 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (EventMemberImplObjTraits<OBJ>::GetReference (m_obj).*m_function)(m_a1);
    }
    OBJ m_obj;
    MEM m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
  } *ev = new EventMemberImpl1 (obj, mem_ptr, a1);
  return ev;
}

template <typename MEM, typename OBJ,
          typename T1, typename T2>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj, T1 a1, T2 a2)
{
  // two argument version
  class EventMemberImpl2 : public EventImpl
  {
public:
    EventMemberImpl2 (OBJ obj, MEM function, T1 a1, T2 a2)
      : m_obj (obj),
        m_function (function),
        m_a1 (a1),
        m_a2 (a2)
    {
    }
    EventMemberImpl2* clone() {
	std::cout << "clone for EventMemberImpl2" << std::endl;
	//EventMemberImpl2 newEvent;
	    return new EventMemberImpl2(m_obj, m_function, m_a1, m_a2);
    }
protected:
    virtual ~EventMemberImpl2 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (EventMemberImplObjTraits<OBJ>::GetReference (m_obj).*m_function)(m_a1, m_a2);
    }
    OBJ m_obj;
    MEM m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
    typename TypeTraits<T2>::ReferencedType m_a2;
  } *ev = new EventMemberImpl2 (obj, mem_ptr, a1, a2);
  return ev;
}

template <typename MEM, typename OBJ>
EventImpl * MakeSpecialEvent (MEM mem_ptr, OBJ obj, uint8_t* buf, ssize_t len)
{
  // two argument version
  class EventMemberSpecial : public EventImpl
  {
public:
    EventMemberSpecial (OBJ obj, MEM function, uint8_t* buf, ssize_t len)
      : m_obj (obj),
        m_function (function),
        m_a1 (buf),
        m_a2 (len)
    {
	    m_a1 = new uint8_t[m_a2];
	    memcpy(m_a1, buf, m_a2);
    }
    EventMemberSpecial* clone() {
	    return new EventMemberSpecial(m_obj, m_function, m_a1, m_a2);
    }
protected:
    virtual ~EventMemberSpecial ()
    {
	if (m_a1 != NULL) free(m_a1);
	m_a1 = NULL;
    }
private:
    virtual void Notify (void)
    {
      (EventMemberImplObjTraits<OBJ>::GetReference (m_obj).*m_function)(m_a1, m_a2);
    }
    OBJ m_obj;
    MEM m_function;
    uint8_t* m_a1;
    uint8_t* new_buf;
    ssize_t m_a2;
  } *ev = new EventMemberSpecial (obj, mem_ptr, buf, len);
  return ev;
}

template <typename MEM, typename OBJ,
          typename T1, typename T2, typename T3>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj, T1 a1, T2 a2, T3 a3)
{
  // three argument version
  class EventMemberImpl3 : public EventImpl
  {
public:
    EventMemberImpl3 (OBJ obj, MEM function, T1 a1, T2 a2, T3 a3)
      : m_obj (obj),
        m_function (function),
        m_a1 (a1),
        m_a2 (a2),
        m_a3 (a3)
    {
    }
		EventMemberImpl3* clone() { return new EventMemberImpl3(*this);}
protected:
    virtual ~EventMemberImpl3 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (EventMemberImplObjTraits<OBJ>::GetReference (m_obj).*m_function)(m_a1, m_a2, m_a3);
    }
    OBJ m_obj;
    MEM m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
    typename TypeTraits<T2>::ReferencedType m_a2;
    typename TypeTraits<T3>::ReferencedType m_a3;
  } *ev = new EventMemberImpl3 (obj, mem_ptr, a1, a2, a3);
  return ev;
}

template <typename MEM, typename OBJ,
          typename T1, typename T2, typename T3, typename T4>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj, T1 a1, T2 a2, T3 a3, T4 a4)
{
  // four argument version
  class EventMemberImpl4 : public EventImpl
  {
public:
    EventMemberImpl4 (OBJ obj, MEM function, T1 a1, T2 a2, T3 a3, T4 a4)
      : m_obj (obj),
        m_function (function),
        m_a1 (a1),
        m_a2 (a2),
        m_a3 (a3),
        m_a4 (a4)
    {
    }
		EventMemberImpl4* clone() { return new EventMemberImpl4(*this);}
protected:
    virtual ~EventMemberImpl4 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (EventMemberImplObjTraits<OBJ>::GetReference (m_obj).*m_function)(m_a1, m_a2, m_a3, m_a4);
    }
    OBJ m_obj;
    MEM m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
    typename TypeTraits<T2>::ReferencedType m_a2;
    typename TypeTraits<T3>::ReferencedType m_a3;
    typename TypeTraits<T4>::ReferencedType m_a4;
  } *ev = new EventMemberImpl4 (obj, mem_ptr, a1, a2, a3, a4);
  return ev;
}

template <typename MEM, typename OBJ,
          typename T1, typename T2, typename T3, typename T4, typename T5>
EventImpl * MakeEvent (MEM mem_ptr, OBJ obj,
                       T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
{
  // five argument version
  class EventMemberImpl5 : public EventImpl
  {
public:
    EventMemberImpl5 (OBJ obj, MEM function, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
      : m_obj (obj),
        m_function (function),
        m_a1 (a1),
        m_a2 (a2),
        m_a3 (a3),
        m_a4 (a4),
        m_a5 (a5)
    {
    }
		EventMemberImpl5* clone() { return new EventMemberImpl5(*this);}
protected:
    virtual ~EventMemberImpl5 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (EventMemberImplObjTraits<OBJ>::GetReference (m_obj).*m_function)(m_a1, m_a2, m_a3, m_a4, m_a5);
    }
    OBJ m_obj;
    MEM m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
    typename TypeTraits<T2>::ReferencedType m_a2;
    typename TypeTraits<T3>::ReferencedType m_a3;
    typename TypeTraits<T4>::ReferencedType m_a4;
    typename TypeTraits<T5>::ReferencedType m_a5;
  } *ev = new EventMemberImpl5 (obj, mem_ptr, a1, a2, a3, a4, a5);
  return ev;
}

template <typename U1, typename T1>
EventImpl * MakeEvent (void (*f)(U1), T1 a1)
{
  // one arg version
  class EventFunctionImpl1 : public EventImpl
  {
public:
    typedef void (*F)(U1);

    EventFunctionImpl1 (F function, T1 a1)
      : m_function (function),
        m_a1 (a1)
    {
    }
		EventFunctionImpl1* clone() { return new EventFunctionImpl1(*this);}
protected:
    virtual ~EventFunctionImpl1 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (*m_function)(m_a1);
    }
    F m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
  } *ev = new EventFunctionImpl1 (f, a1);
  return ev;
}

template <typename U1, typename U2, typename T1, typename T2>
EventImpl * MakeEvent (void (*f)(U1,U2), T1 a1, T2 a2)
{
  // two arg version
  class EventFunctionImpl2 : public EventImpl
  {
public:
    typedef void (*F)(U1, U2);

    EventFunctionImpl2 (F function, T1 a1, T2 a2)
      : m_function (function),
        m_a1 (a1),
        m_a2 (a2)
    {
    }
		EventFunctionImpl2* clone() { return new EventFunctionImpl2(*this);}
protected:
    virtual ~EventFunctionImpl2 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (*m_function)(m_a1, m_a2);
    }
    F m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
    typename TypeTraits<T2>::ReferencedType m_a2;
  } *ev = new EventFunctionImpl2 (f, a1, a2);
  return ev;
}

template <typename U1, typename U2, typename U3,
          typename T1, typename T2, typename T3>
EventImpl * MakeEvent (void (*f)(U1,U2,U3), T1 a1, T2 a2, T3 a3)
{
  // three arg version
  class EventFunctionImpl3 : public EventImpl
  {
public:
    typedef void (*F)(U1, U2, U3);

    EventFunctionImpl3 (F function, T1 a1, T2 a2, T3 a3)
      : m_function (function),
        m_a1 (a1),
        m_a2 (a2),
        m_a3 (a3)
    {
    }
		EventFunctionImpl3* clone() { return new EventFunctionImpl3(*this);}
protected:
    virtual ~EventFunctionImpl3 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (*m_function)(m_a1, m_a2, m_a3);
    }
    F m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
    typename TypeTraits<T2>::ReferencedType m_a2;
    typename TypeTraits<T3>::ReferencedType m_a3;
  } *ev = new EventFunctionImpl3 (f, a1, a2, a3);
  return ev;
}

template <typename U1, typename U2, typename U3, typename U4,
          typename T1, typename T2, typename T3, typename T4>
EventImpl * MakeEvent (void (*f)(U1,U2,U3,U4), T1 a1, T2 a2, T3 a3, T4 a4)
{
  // four arg version
  class EventFunctionImpl4 : public EventImpl
  {
public:
    typedef void (*F)(U1, U2, U3, U4);

    EventFunctionImpl4 (F function, T1 a1, T2 a2, T3 a3, T4 a4)
      : m_function (function),
        m_a1 (a1),
        m_a2 (a2),
        m_a3 (a3),
        m_a4 (a4)
    {
    }
		EventFunctionImpl4* clone() { return new EventFunctionImpl4(*this);}
protected:
    virtual ~EventFunctionImpl4 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (*m_function)(m_a1, m_a2, m_a3, m_a4);
    }
    F m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
    typename TypeTraits<T2>::ReferencedType m_a2;
    typename TypeTraits<T3>::ReferencedType m_a3;
    typename TypeTraits<T4>::ReferencedType m_a4;
  } *ev = new EventFunctionImpl4 (f, a1, a2, a3, a4);
  return ev;
}

template <typename U1, typename U2, typename U3, typename U4, typename U5,
          typename T1, typename T2, typename T3, typename T4, typename T5>
EventImpl * MakeEvent (void (*f)(U1,U2,U3,U4,U5), T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
{
  // five arg version
  class EventFunctionImpl5 : public EventImpl
  {
public:
    typedef void (*F)(U1,U2,U3,U4,U5);

    EventFunctionImpl5 (F function, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
      : m_function (function),
        m_a1 (a1),
        m_a2 (a2),
        m_a3 (a3),
        m_a4 (a4),
        m_a5 (a5)
    {
    }
		EventFunctionImpl5* clone() { return new EventFunctionImpl5(*this);}
protected:
    virtual ~EventFunctionImpl5 ()
    {
    }
private:
    virtual void Notify (void)
    {
      (*m_function)(m_a1, m_a2, m_a3, m_a4, m_a5);
    }
    F m_function;
    typename TypeTraits<T1>::ReferencedType m_a1;
    typename TypeTraits<T2>::ReferencedType m_a2;
    typename TypeTraits<T3>::ReferencedType m_a3;
    typename TypeTraits<T4>::ReferencedType m_a4;
    typename TypeTraits<T5>::ReferencedType m_a5;
  } *ev = new EventFunctionImpl5 (f, a1, a2, a3, a4, a5);
  return ev;
}

} // namespace ns3

#endif /* MAKE_EVENT_H */

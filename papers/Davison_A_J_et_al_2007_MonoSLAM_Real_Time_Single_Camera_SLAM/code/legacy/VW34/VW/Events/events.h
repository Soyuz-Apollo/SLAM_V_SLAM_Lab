/** @file events.h
 *
 *              (c) Copyright University of Oxford
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/**
 * @namespace VWEvents
 * @Author    Keith Connor <krc@robots.ox.ac.uk>
 * @Date      20/06/03
 *
 * A cunning system for setting up connections to general functions.
 * Supports functions and member functions.
 *
 * e.g., you write a library that uses Events, then others can make
 * your library call their class member functions -- Cool!
 *
 * An idea stolen from libsigc, but is less general, simpler and
 * fits into one header.
 *
 * Semantics:
 *         functions are attached to Events.
 *
 * Syntax:
 *         Provider of library:
 *           Event0<void> event; // to set up a Event
 *           event(); // to execute the event handler
 *         User of library:
 *           if(you_like_functions)
 *             event.Attach(&myfunction);
 *           else if(you_like_member_functions)
 *             event.Attach(&myclass,&MyClass::Member);
 *           else if(you_want_to_chain_events)
 *             event.Attach(&other_event);
 *           else if(youve_finished_with_the_Event)
 *             event.Detach();
 *
*/


#ifndef _vw_events_h_
#define _vw_events_h_

namespace VWEvents {


  /** A base class from which the user object should derrive, if one
* wishes to be able to attach events to its member functions.
*/
  class EventHandler
  {
  public:
    virtual ~EventHandler()
    { }
  };

  
  /** The base class for all Events ( the common bits of the interface )
*/
  class Event : public EventHandler
  {
  public:
    bool IsAttached() const
    {
      return _attached;
    }
    
    void Detach()
    {
      _attached = false;
    }
    
  protected:
    Event()
      : _attached(false)
    { }
    
    bool _attached;
  };
  
  
  /** Event0 is for calling member functions taking no parameters. */
  template<typename P0>
  class Event0 : public Event
  {
    typedef EventHandler* ObjectPtr;
    typedef P0 (EventHandler::*MemberFuncPtr)(void);
    typedef P0 (*GlobalFuncPtr)(void);

  public:
    Event0()
      : _pobj(0), _pmem(0), _pfun(0)
    { }
    
    template<typename T>
    void Attach(ObjectPtr po, P0 (T::*pm)(void))
    {
      _pobj = po;
      _pmem = static_cast<MemberFuncPtr>(pm);
      _pfun = 0;
      _attached = true;
    }
    
    void Attach(GlobalFuncPtr pf)
    {
      _pobj = 0;
      _pmem = 0;
      _pfun = pf;
      _attached = true;
    }

   void Attach(Event0* e)
    {
      _pobj = e;
      _pmem = static_cast<MemberFuncPtr>(e->operator());
      _pfun = 0;
      _attached = true;
    }
    
    P0 operator()()
    {
      if(_attached) {
	if(_pobj)
	  return (_pobj->*_pmem)();
	else if(_pfun)
	  return _pfun();
      }
    }
    
  private:
    ObjectPtr _pobj;
    MemberFuncPtr _pmem;
    GlobalFuncPtr _pfun;
  };
  

  /** Event1 is for calling member functions taking one parameter. */
  template<typename P0, typename P1>
  class Event1 : public Event
  {
    typedef EventHandler* ObjectPtr;
    typedef P0 (EventHandler::*MemberFuncPtr)(P1);
    typedef P0 (*GlobalFuncPtr)(P1);
    
  public:
    Event1()
      : _pobj(0), _pmem(0), _pfun(0)
    { }
    
    template<typename T>
    void Attach(ObjectPtr po, P0 (T::*pm)(P1))
    {
      _pobj = po;
      _pmem = static_cast<MemberFuncPtr>(pm);
      _pfun = 0;
      _attached = true;
    }
    
    void Attach(GlobalFuncPtr pf)
    {
      _pobj = 0;
      _pmem = 0;
      _pfun = pf;
      _attached = true;
    }

   void Attach(Event1* e)
    {
      _pobj = e;
      _pmem = static_cast<MemberFuncPtr>(e->operator());
      _pfun = 0;
      _attached = true;
    }

    P0 operator()(P1 p1)
    {
      if(_attached) {
	if(_pobj)
	  return (_pobj->*_pmem)(p1);
	else if(_pfun)
	  return _pfun(p1);
      }
    }
    
  private:
    ObjectPtr _pobj;
    MemberFuncPtr _pmem;
    GlobalFuncPtr _pfun;
  };
  
  
  /** Event2 is for Eventing onto member functions taking two parameters. */
  template<typename P0, typename P1, typename P2>
  class Event2 : public Event
  {
    typedef EventHandler* ObjectPtr;
    typedef P0 (EventHandler::*MemberFuncPtr)(P1,P2);
    typedef P0 (*GlobalFuncPtr)(P1,P2);

  public:
    Event2()
      : _pobj(0), _pmem(0), _pfun(0)
    { }
    
    template<typename T>
    void Attach(ObjectPtr po, P0 (T::*pm)(P1,P2))
    {
      _pobj = po;
      _pmem = static_cast<MemberFuncPtr>(pm);
      _pfun = 0;
      _attached = true;
    }

    void Attach(GlobalFuncPtr pf)
    {
      _pobj = 0;
      _pmem = 0;
      _pfun = pf;
      _attached = true;
    }
    
   void Attach(Event2* e)
    {
      _pobj = e;
      _pmem = static_cast<MemberFuncPtr>(e->operator());
      _pfun = 0;
      _attached = true;
    }

    P0 operator()(P1 p1, P2 p2)
    {
      if(_attached) {
	if(_pobj)
	  return (_pobj->*_pmem)(p1,p2);
	else if(_pfun)
	  return _pfun(p1,p2);
      }
    }
    
  private:
    ObjectPtr _pobj;
    MemberFuncPtr _pmem;
    GlobalFuncPtr _pfun;
  };


  /** Event3 is for calling member functions taking three parameters. */
  template<typename P0, typename P1, typename P2, typename P3>
  class Event3 : public Event
  {
    typedef EventHandler* ObjectPtr;
    typedef P0 (EventHandler::*MemberFuncPtr)(P1,P2,P3);
    typedef P0 (*GlobalFuncPtr)(P1,P2,P3);

  public:
    Event3()
      : _pobj(0), _pmem(0), _pfun(0)
    { }
    
    template<typename T>
    void Attach(ObjectPtr po, P0 (T::*pm)(P1,P2,P3))
    {
      _pobj = po;
      _pmem = static_cast<MemberFuncPtr>(pm);
      _pfun = 0;
      _attached = true;
    }

    void Attach(GlobalFuncPtr pf)
    {
      _pobj = 0;
      _pmem = 0;
      _pfun = pf;
      _attached = true;
    }
    

   void Attach(Event3* e)
    {
      _pobj = e;
      _pmem = static_cast<MemberFuncPtr>(e->operator());
      _pfun = 0;
      _attached = true;
    }

    P0 operator()(P1 p1, P2 p2, P3 p3)
    {
      if(_attached) {
	if(_pobj)
	  return (_pobj->*_pmem)(p1,p2,p3);
	else if(_pfun)
	  return _pfun(p1,p2,p3);
      }
    }
    
  private:
    ObjectPtr _pobj;
    MemberFuncPtr _pmem;
    GlobalFuncPtr _pfun;
  };


  /** Event4 is for calling member functions taking four parameters. */
  template<typename P0, typename P1, typename P2, typename P3, typename P4>
  class Event4 : public Event
  {
    typedef EventHandler* ObjectPtr;
    typedef P0 (EventHandler::*MemberFuncPtr)(P1,P2,P3,P4);
    typedef P0 (*GlobalFuncPtr)(P1,P2,P3,P4);

  public:
    Event4()
      : _pobj(0), _pmem(0), _pfun(0)
    { }
    
    template<typename T>
    void Attach(ObjectPtr po, P0 (T::*pm)(P1,P2,P3,P4))
    {
      _pobj = po;
      _pmem = static_cast<MemberFuncPtr>(pm);
      _pfun = 0;
      _attached = true;
    }
    
    void Attach(GlobalFuncPtr pf)
    {
      _pobj = 0;
      _pmem = 0;
      _pfun = pf;
      _attached = true;
    }
    
   void Attach(Event4* e)
    {
      _pobj = e;
      _pmem = static_cast<MemberFuncPtr>(e->operator());
      _pfun = 0;
      _attached = true;
    }

    P0 operator()(P1 p1, P2 p2, P3 p3, P4 p4)
    {
      if(_attached) {
	if(_pobj)
	  return (_pobj->*_pmem)(p1,p2,p3,p4);
	else if(_pfun)
	  return _pfun(p1,p2,p3,p4);
      }
    }
    
  private:
    ObjectPtr _pobj;
    MemberFuncPtr _pmem;
    GlobalFuncPtr _pfun;
  };


  /** Event5 is for Eventing onto member functions taking five parameters. */
  template<typename P0, typename P1, typename P2, typename P3, typename P4,
	   typename P5>
  class Event5 : public Event
  {
    typedef EventHandler* ObjectPtr;
    typedef P0 (EventHandler::*MemberFuncPtr)(P1,P2,P3,P4,P5);
    typedef P0 (*GlobalFuncPtr)(P1,P2,P3,P4,P5);
    
  public:
    Event5()
      : _pobj(0), _pmem(0), _pfun(0)
    { }
    
    template<typename T>
    void Attach(ObjectPtr po, P0 (T::*pm)(P1,P2,P3,P4,P5))
    {
      _pobj = po;
      _pmem = static_cast<MemberFuncPtr>(pm);
      _pfun = 0;
      _attached = true;
    }
    
    void Attach(GlobalFuncPtr pf)
    {
      _pobj = 0;
      _pmem = 0;
      _pfun = pf;
      _attached = true;
    }
    
   void Attach(Event5* e)
    {
      _pobj = e;
      _pmem = static_cast<MemberFuncPtr>(e->operator());
      _pfun = 0;
      _attached = true;
    }

    P0 operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
    {
      if(_attached) {
	if(_pobj)
	  return (_pobj->*_pmem)(p1,p2,p3,p4,p5);
	else if(_pfun)
	  return _pfun(p1,p2,p3,p4,p5);
      }
    }
    
  private:
    ObjectPtr _pobj;
    MemberFuncPtr _pmem;
    GlobalFuncPtr _pfun;
  };

  
} // namespace VWEvents


#endif // _vwevents_h_

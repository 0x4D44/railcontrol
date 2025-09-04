//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// Singleton template (to implement the Singleton pattern from Design
// Patterns)
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#ifndef __OWLEXT_SINGLETN_H
#define __OWLEXT_SINGLETN_H

OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                  Singleton<T>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
class Singleton
{
    // Accessors
    //
public:
    static T& Instance()
    {
        static T theInstance;
        return theInstance;
    }

    // DISALLOWED METHODS
    //
private:
    Singleton()
        { }
    Singleton(Singleton<T>&)
        { }
    ~Singleton()
        { }
    void operator=(Singleton<T>&)
        { }
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif

/*
 * USAGE:

#include <iostream.h>

class Foo
{
public:
    Foo()
        { cout << "Foo::Foo()" << endl; }
    ~Foo()
        { cout << "Foo::~Foo()" << endl; }
        
    void DoIt()
        { cout << "Foo::DoIt()" << endl; }
};
class Foo2
{
public:
    Foo2()
        { cout << "Foo2::Foo2()" << endl; }
    ~Foo2()
        { cout << "Foo2::~Foo2()" << endl; }
        
    void DoIt()
        { cout << "Foo2::DoIt()" << endl; }
};

void main()
{
    Singleton<Foo>::Instance().DoIt();
    Singleton<Foo>::Instance().DoIt();
    Singleton<Foo2>::Instance().DoIt();
    Singleton<Foo2>::Instance().DoIt();
}

*/
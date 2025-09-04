//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// A "busy" cursor.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:26 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_BUSY_H)
#define __OWLEXT_BUSY_H


// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif


OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                   TBusyCursor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TBusyHook;

class OWLEXTCLASS TBusyCursor {
  // Friendship declarations
  //
	public:
  	friend class TBusyHook;

  // Object lifetime methods
  //
	public:
		explicit TBusyCursor(owl_string message);
		TBusyCursor();
		~TBusyCursor();

	// Accessors
	//
	public:
		bool Active() const;
		owl_string Message() const;
		static bool GloballyActive ();

	// Mutators
	//
	public:
		void Active(bool active);
		void Message(owl_string message);
	protected:
		virtual void UpdateMessage (LPCTSTR);
		// Override this to change the text other than in the app's MainWindow's
		// status bar:
		void UpdateMessage(const owl_string& s);

	// Internal methods
	//
	private:
		void Init();
		void Activate();
		void Deactivate();

	// Internal data
	//
	private:
		static TBusyCursor* sTop;
		TBusyCursor* mNext;
		HCURSOR mBusyCursor;
		owl_string mMessage;
		bool mActive;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations

inline bool TBusyCursor::Active() const
{ return mActive; }

inline owl_string TBusyCursor::Message() const
{ return mMessage; }

inline bool TBusyCursor::GloballyActive ()
{ return sTop ? sTop->Active() : false; }

inline void TBusyCursor::UpdateMessage(const owl_string& s)
{ UpdateMessage (s.length() > 0 ? s.c_str() : 0); }

OWLEXT_END_NAMESPACE

#endif  // __OWLEXT_BUSY_H


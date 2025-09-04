//-------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// Copyright(c) 1996 by Manic Software.
// All rights reserved.
//
// TTabWindow
// TWin95TabWindow
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-19 16:49:53 $
//-------------------------------------------------------------------
#if !defined (__OWLEXT_TABWIN_H)
#define __OWLEXT_TABWIN_H

#ifndef __OWLEXT_CORE_H
#  include <owlext/core.h>              // required for all OWLEXT headers
#endif


#include <owl/template.h>

#ifndef OWL_TABCTRL_H
# include <owl/tabctrl.h>
#endif


OWLEXT_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                    TTabWindow
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TTabWindow : public __OWL TWindow {
    // Object lifetime methods
    //
public:
    TTabWindow(__OWL TWindow* parent, __OWL TWindow* array[] = 0);
    TTabWindow(__OWL TWindow* parent, uint32 X, uint32 Y, uint32 W, uint32 H, __OWL TWindow* array[] = 0);
private:
    TTabWindow();                                 // DISALLOWED METHOD
    TTabWindow(const TTabWindow&);                // DISALLOWED METHOD
    TTabWindow& operator=(const TTabWindow& src); // DISALLOWED METHOD

    // Tabbed-window interface: adding windows, removing windows
    //
public:
    virtual void Attach(__OWL TWindow* ptr, LPCTSTR title = 0);
    virtual void Attach(__OWL TWindow* windowArray[]);
    virtual __OWL TWindow* Detach(int32 index) = 0;
    virtual __OWL TWindow* Detach(__OWL TWindow* ptr)
        { return Detach(Retrieve(ptr)); }
    virtual __OWL TWindow* Retrieve(int32 index)
        { return tabbedWindowArray[index]; }
    virtual int32 Retrieve(__OWL TWindow* ptr);
    virtual __OWL TWindow* operator[](int32 index)
        { return tabbedWindowArray[index]; }
    virtual int32 operator[](__OWL TWindow* win)
        { return Retrieve(win); }

    virtual int32 GetSelectedTab() = 0;
    virtual void SetSelectedTab(int32 index) = 0;
    virtual int32 GetWindowCount()
        { return tabbedWindowArray.GetItemsInContainer(); }

protected:
    enum { ID_TABCTRL = 100 };
    virtual __OWL TWindow* GetTabCtrl() = 0;

    // OWL methods
    //
protected:
    DECLARE_RESPONSE_TABLE(TTabWindow);

    // Internal Data
    //
protected:      // so deriveds can see it more easily
    __OWL TPtrArray<__OWL TWindow*> tabbedWindowArray;
};



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                               TWin95TabWindow
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class OWLEXTCLASS TWin95TabWindow : public TTabWindow {
    // Ctors & Dtors
    //
public:
    TWin95TabWindow(__OWL TWindow* parent, __OWL TWindow* array[] = 0);
    TWin95TabWindow(__OWL TWindow* parent, uint32 X, uint32 Y, uint32 W, uint32 H, __OWL TWindow* array[] = 0);
    virtual ~TWin95TabWindow();

    // OWL Methods
    //
protected:
    virtual void SetupWindow();
    DECLARE_RESPONSE_TABLE(TWin95TabWindow);
    void EvSize(uint sizeType, __OWL TSize& size);
    void TabSelChange(__OWL TNotify &);
    bool TabSelChanging(__OWL TNotify &);
    void TabKeyDown(__OWL TTabKeyDown &);

    // Tabbed-window interface: adding windows, removing windows
    //
public:
    virtual void Attach(__OWL TWindow* ptr, LPCTSTR title = 0);
    virtual void Attach(__OWL TWindow* windowArray[])
      { TTabWindow::Attach(windowArray); }
    virtual __OWL TWindow* Detach(int32 index);
    virtual __OWL TWindow* Detach(__OWL TWindow* ptr)
      { return TTabWindow::Detach(ptr); }
    virtual int32 GetSelectedTab()
      { return m_tabCtrl->GetSel(); }
    virtual void SetSelectedTab(int32 index);
    virtual int32 GetWindowCount();
protected:
#if defined(BI_NO_COVAR_RET)
    virtual __OWL TWindow* 			GetTabCtrl()
#else
    virtual __OWL TTabControl* 	GetTabCtrl()
#endif
      { return m_tabCtrl; }

    virtual void AdjustTabClient();

    // Internal data
    //
private:
    __OWL TTabControl* m_tabCtrl;
};


/*
 * By arranging the code this way, we allow for other tabbed-window schemes to
 * customize the actual window-arrangement; for example, the class declaration
 * below would allow us to use the TNoteTab control instead of the TTabControl
 * control to manage the window-flipping.
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                                TNoteTabWindow
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
#include <owl/notetab.h>

class OWLEXTCLASS TNoteTabWindow : public TTabWindow
{
    // Ctors & Dtors
    //
public:
    TNoteTabWindow(TWindow* parent, TWindow* array[] = 0);
    TNoteTabWindow(TWindow* parent, uint32 X, uint32 Y, uint32 W, uint32 H, TWindow* array[] = 0);
    virtual ~TNoteTabWindow();

    // OWL Methods
    //
protected:
    virtual void SetupWindow();
    DECLARE_RESPONSE_TABLE(TNoteTabWindow);
    void EvSize(uint sizeType, TSize& size);
    void TabSelChange(TNotify &);
    bool TabSelChanging(TNotify &);
    void TabKeyDown(TTabKeyDown &);

    // Tabbed-window interface: adding windows, removing windows
    //
public:
    virtual void Attach(TWindow* ptr, const char* title = 0);
    virtual void Attach(TWindow* windowArray[])
    { TTabWindow::Attach(windowArray); }
    virtual TWindow* Detach(int32 index);
    virtual TWindow* Detach(TWindow* ptr)
    { return TTabWindow::Detach(ptr); }
    virtual int32 GetSelectedTab()
    { return m_tabCtrl->GetSel(); }
    virtual void SetSelectedTab(int32 index)
    { m_tabCtrl->SetSel(index); }
    virtual int32 GetWindowCount()
    {
        if (m_tabCtrl->GetCount() != tabbedWindowArray.GetItemsInContainer())
            TRACE("This is a problem--tabCtrl->GetCount() != tabbedWindowArray");
        return TTabWindow::GetWindowCount();
    }
protected:
    virtual TNoteTab* GetTabCtrl()
    { return m_tabCtrl; }
    virtual void AdjustTabClient();

private:
    TNoteTab* m_tabCtrl;
};
*/

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif

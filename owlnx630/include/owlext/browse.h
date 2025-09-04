//------------------------------------------------------------------------------
// OWL Extensions (OWLEXT) Class Library
// BROWSE.H
//
// Header file for TBrowse Class
//
// Original code by Daniel BERMAN (dberman@inge-com.fr)
//
//$Revision: 1.1.4.3 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:43 $
//------------------------------------------------------------------------------
#if !defined(__OWLEXT_BROWSE_H)
#define __OWLEXT_BROWSE_H

// TBrowse styles
#define TBS_MULTIPLESEL		0x0040L
#define TBS_EXTENDEDSEL		0x0080L

#ifndef RC_INVOKED

// Grab the core OWLEXT file we need
//
#if !defined (__OWLEXT_CORE_H) && !defined (__OWLEXT_ALL_H)
# include <owlext/core.h>
#endif

#include <owl/color.h>
#include <owl/control.h>
#include <owl/listbox.h>
#include <owl/geometry.h>
#include <owl/gdiobjec.h>

OWLEXT_BEGIN_NAMESPACE

class TBrowse;
class TBrowseList;

// Those functions should return true if str1 < str2
typedef bool (*TCompareFunc)(int colId, LPCTSTR str1, LPCTSTR str2);
typedef bool (TBrowse::*TCompareMemFunc)(int colId, LPCTSTR str1, LPCTSTR str2);

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
//
#include <owl/preclass.h>


class OWLEXTCLASS THeader {
	public:
		enum TSortOrder {Undefined, Ascending, Descending};

		THeader();
	  THeader(int id, const owl_string& title, int width, bool visible, bool exactWidth);

	  bool operator == (const THeader& header) const {return header.Id == Id;}

	  int 					Id;
		owl_string		Title;
	  bool 					Visible;
	  int 					Width;
	  bool 					ExactWidth;
		__OWL TRect 	Rect;
		uint16 				TextFormat;
		__OWL TColor 	Color;
		int 					StyleNo;
	  bool 					Pressed;
		TCompareFunc 	CompareFunc;
		TSortOrder    SortOrder;
	  TCompareMemFunc CompareMemFunc;
};
typedef __OWL TIPtrArray<THeader*> THeaders;
typedef __OWL TIPtrArray<THeader*>::Iterator THeadersIterator;


class OWLEXTCLASS TStyle {
	public:
		TStyle();
		TStyle(int id);
		~TStyle();

		TStyle(const TStyle& style);
		void operator = (const TStyle& style);

		bool operator == (const TStyle& style) const { return style.Id == Id;}

		void SetStyle(__OWL TDC &dc, UINT itemState);

		int 						Id;
	  __OWL TColor 		TextColor;
	  __OWL TColor 	  BkColor;
	  __OWL TColor 	  HilightTextColor;
	  __OWL TColor 	  HilightBkColor;
	  __OWL TBitmap*	IconImage;
	  __OWL TBitmap*	IconMask;
	  uint16 					TextAlignment;
};
typedef __OWL TIPtrArray<TStyle*> TStyles;
typedef __OWL TIPtrArray<TStyle*>::Iterator TStylesIterator;


class OWLEXTCLASS TBrowse : public __OWL TControl{
	public:
		enum TUnits {Chars, Pixels, Percent};

		TBrowse(__OWL TWindow* parent, int id, int x, int y, int w, int h, __OWL TModule* module = 0);
		TBrowse(__OWL TWindow* parent, int resourceId, __OWL TModule* module = 0);
		virtual ~TBrowse();

		virtual void	AddColumn(int id, LPCTSTR title, int width = -1,
									 					bool visible = true, TUnits units = Chars);
		virtual int		AddRow();
		virtual void	AddStyle(int id);
	  virtual int		InsertRow(int index);
		virtual void	ClearList();
		virtual int		DeleteRow(int index);
		virtual bool	EnableWindow(bool enable);
		bool					GetColumnText(int id, owl_string& text, int index);
		bool					GetColumnWidth(int id, int& width, bool& exactWidth);
	  virtual int		GetCount() const;
		bool					GetHeaderColor(int id, __OWL TColor& color);
		int						GetHeadersHeight() {return HeadersHeight;}
	  int						GetNumCols();
		bool					GetSel(int index) const;
		int						GetSelCount() const;
		int						GetSelIndex() const;
		int						GetSelIndexes(int* indexes, int maxCount) const;
		int						GetTopIndex() const;
		bool					IsColumnVisible(int id);
		virtual bool	ModifyColumnText(int id, LPCTSTR text, int index);
		void					Reset();
		bool				  SetColumnStyle(int id, int styleNo, int index = -1);
		virtual bool	SetColumnText(int id, LPCTSTR text, int index = -1);
		virtual bool	SetColumnText(int id, LPCTSTR text, int styleNo, int index);
		bool					SetColumnWidth(int id, int width, TUnits units = Chars);
		bool				  SetCompareItemProc(int colId, TCompareFunc compareFunc);
		bool				  SetCompareItemProc(int colId, TCompareMemFunc compareMemFunc);
		bool					SetDefaultStyle(int id, int styleNo);
		bool					SetHeaderColor(int id, __OWL TColor color);
		bool					SetHeaderTextFormat(int id, uint16 uiTextFormat);
		void					SetHeadersHeight(int height);
		int						SetSel(int index, bool select);
		virtual int		SetSelIndex(int index);
		bool					SetStyleColor(int id, __OWL TColor textColor, __OWL TColor bkColor);
		bool					SetStyleSelectedColor(int id, __OWL TColor textColor, __OWL TColor bkColor);
		bool					SetStyleImage(int id, __OWL TDib& iconImageAndMask );
		bool					SetStyleImage(int id, __OWL TDib& iconImage, __OWL TColor& faceColor);
		bool					SetStyleTextAlignment(int id, uint16 uiTextAlignment);
		int						SetTopIndex(int index);
		bool					ShowColumn(int id, int visible = true);
		virtual void	SortItems(int colId, bool ascending);
		virtual uint	Transfer(void *buffer, __OWL TTransferDirection direction);

	protected:

		virtual void		CalculateHeadersSize();
		virtual void		DeleteItem(DELETEITEMSTRUCT & deleteInfo);
		virtual void		DrawHeaderRect(__OWL TDC& dc, __OWL TRect& rect, bool down);
		virtual void		DrawHeaderText(__OWL TDC& dc, THeader* header);
		virtual void		DrawItem(DRAWITEMSTRUCT & drawInfo);
		virtual void		DrawSizingLine(__OWL TPoint& point);
		void						EvDblClk();
		void						EvLButtonDown(uint modKeys, __OWL TPoint& point);
		void						EvLButtonUp(uint modKeys, __OWL TPoint& point);
		void						EvMouseMove(uint modKeys, __OWL TPoint& point);
		uint						EvNCHitTest(__OWL TPoint& point);
		void						EvRButtonDown(uint modKeys, __OWL TPoint& point);
		void					  EvRButtonUp(uint modKeys, __OWL TPoint& point);
		void						EvSelChange();
		void						EvSetFocus(HWND hWndLostFocus);
		void					  EvSize(uint sizeType, __OWL TSize& size);
		void						EvSysColorChange();
		virtual THeader*	GetHeaderFromPoint(__OWL TPoint& point, bool& onBorder);
		virtual void		MeasureItem(MEASUREITEMSTRUCT & measureInfo);
		virtual void		Paint(__OWL TDC& dc, bool erase, __OWL TRect& rect);
		virtual void		SetupWindow();
		virtual void		StartDrawingSizingLine(__OWL TPoint& point);
		virtual void		StopDrawingSizingLine(__OWL TPoint& point);

	private:

		__OWL TFont* 	NormalFont;
		__OWL TFont* 	BoldFont;
		int 					HeadersHeight;
		int 					BoldCharWidth;
		int 					HorzExtent;
		int 					ViewportOrgX;
		int 					RowsHeight;

		THeader* 			Capture;
		bool 					ReSizing;
		__OWL TRect 	PrevSizingRect;

		THeaders* 		Headers;
		TBrowseList* 	List;
		TStyles* 			Styles;

		void Init();

		DECLARE_RESPONSE_TABLE(TBrowse);
};

typedef __OWL TIPtrArray<uint32*> TRowArray;

class OWLEXTCLASS TRowArrayIterator : public __OWL TPtrArrayIterator<uint32*,__OWL TIPtrArray<uint32*> >{
	public:
		TRowArrayIterator(TRowArray& array);
		bool GetColumnText(int id, owl_string& text);
};


class OWLEXTCLASS TBrowseData {
	public:
		TBrowseData();
		TBrowseData(int numCols);

		void				AddRow(bool isSelected = false);
		void				Clear() {ItemDatas.Flush(); ResetSelections();}
		int					GetNumRows() {return ItemDatas.GetItemsInContainer();}
		TRowArray&	GetItemDatas() {return ItemDatas;}
		int					GetNumCols(int numCols) {return NumCols;}
		int					GetSelCount() const {return SelIndices.GetItemsInContainer();}
		__OWL TIntArray&	GetSelIndices() {return SelIndices;}
		void				ResetSelections() {SelIndices.Flush();}
		void				Select(int index);
		void				SetColumnText(int id, LPCTSTR text);
		void				SetColumnStyle(int id, int StyleNo);
		void				SetNumCols(int numCols) {NumCols = numCols;}


	protected:

		int 			NumCols;
		TRowArray ItemDatas;
		__OWL TIntArray SelIndices;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

OWLEXT_END_NAMESPACE

#endif	// RC_INVOKED

#endif	// __OWLEXT_BROWSE_H

#ifndef EDITOR_H
#define EDITOR_H

#include <owl/editview.h>
#include <owl/richedv.h>

class TPlainEditor
  : public owl::TEditView
{
public:

  TPlainEditor(owl::TDocument& doc, owl::TWindow* parent = 0);

protected:

  virtual void Paint(owl::TDC& dc, bool erase, owl::TRect& rect); // override

  void EvGetMinMaxInfo(MINMAXINFO& minmaxinfo);

  DECLARE_RESPONSE_TABLE(TPlainEditor);
};


class TRichEditor
  : public owl::TRichEditView
{
public:

  TRichEditor(owl::TDocument& doc, owl::TWindow* parent = 0);

protected:

#if defined(UNICODE)
    void SetupWindow()
    {
        TRichEditView::SetupWindow();

        owl::TResult res = SendMessage(EM_GETEDITSTYLE);
        res |= SES_USECTF | SES_CTFALLOWEMBED | SES_CTFALLOWSMARTTAG | SES_CTFALLOWPROOFING;
        res = SendMessage(EM_SETEDITSTYLE, res, res);

        int opt = GetLangOptions();
        opt |= IMF_SPELLCHECKING | IMF_IMEUIINTEGRATION;
        SetLangOptions(opt);
    }
#endif

  void EvGetMinMaxInfo(MINMAXINFO& minmaxinfo);

  DECLARE_RESPONSE_TABLE(TRichEditor);
};

#endif

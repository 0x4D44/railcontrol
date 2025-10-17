//---------------------------------------------------------------------------

#ifndef opendlgH
#define opendlgH
//---------------------------------------------------------------------------
#include <owl/opensave.h>
#include <owl/listbox.h>

class TOpenDialog : public owl::TFileOpenDialog
{
  private:
   int _share;

  protected:
    virtual int ShareViolation();

  public:
    TOpenDialog(owl::TWindow*                parent,
                owl::TFileOpenDialog::TData& data,
                int                          share,
                owl::TResId                  templateId = 0,
                LPCTSTR                      title = 0,
                owl::TModule*                module = 0);
};


class TMyFrame : public owl::TListBox
{
public:
  TMyFrame(owl::TWindow* parent);
  virtual ~TMyFrame();

protected:
  virtual void SetupWindow(); // TWindow override
  virtual void CleanupWindow(); // TWindow override

  void CmAbout();

  void CmFileOpen();

  void CmShareWarn();
  void CmShareNoWarn();
  void CmShareFallThrough();

  void CmExplorer();

  void CeShareWarn(owl::TCommandEnabler& ce);
  void CeShareNoWarn(owl::TCommandEnabler& ce);
  void CeShareFallThrough(owl::TCommandEnabler& ce);

  void CeExplorer(owl::TCommandEnabler& ce);

private:
  void ShowMessage(const owl::tstring& message);

  int share;
  bool explorer;

  DECLARE_RESPONSE_TABLE(TMyFrame);
};

#endif

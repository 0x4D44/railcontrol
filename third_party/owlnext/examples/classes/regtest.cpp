#include "pch.h"
#include "client.h"
#include <owl/registry.h>
#include <owl/time.h>

using namespace owl;
using namespace std;
using namespace std::string_literals;

void TClassesMDIClient::CmRegistry()
{
  const auto messageTitle = _T("Registry classes test");
  try
  {
    // Do some registry query tests.
    //
    if (const auto key = TRegKey::GetCurrentUser().GetSubkey(_T("Software\\TortoiseMerge")))
    {
      const auto hasFont = key->HasValue(_T("FontName"));
      const auto fontValue = key->GetValue(_T("FontName"));
      const auto font = fontValue ? *fontValue : _T("DefaultFont");

      const auto fontSize = key->HasValue(_T("FontSize")) ? *key->GetValue(_T("FontSize")) : 0ul;
      const auto fontSize2 = key->HasValue(_T("FontSize")) ? TRegValue{*key, _T("FontSize")} : 0ul;

      const auto hasNonsense = key->HasValue(_T("Nonsense"));
      const auto nonsenseValue = key->GetValue(_T("Nonsense"));
      const auto nonsense = nonsenseValue ? *nonsenseValue : _T("DefaultNonsense");

      //const auto font2 = key->GetValueOrDefault(_T("FontName"), _T("DefaultFont")); // Compile error: Default value cannot be pointer.
      const auto font2 = key->GetValueOrDefault(_T("FontName"), tstring{_T("DefaultFont")});
      const auto font3 = key->GetValueOrDefault(_T("FontName"), _T("DefaultFont"s));
      const auto font4 = key->GetValueOrDefault<tstring>(_T("FontName"), _T("DefaultFont"));

      //const auto fontSize3 = key->GetValueOrDefault(_T("FontSize"), 0); // Compile error: Ambigious user-defined conversion.
      //const auto fontSize3 = key->GetValueOrDefault(_T("FontSize"), 0ull); // TXRegistry: incompatible registry value type conversion (uint64).
      const auto fontSize3 = key->GetValueOrDefault(_T("FontSize"), uint32{0});
      const auto fontSize4 = key->GetValueOrDefault(_T("FontSize"), 0ul);
      const auto fontSize5 = key->GetValueOrDefault<uint32>(_T("FontSize"), 0);
    }

    const auto testKeyName = _T("OWLNext_RegTest"s);

    // Test key and value creation.
    {
      auto testKey = TRegKey{HKEY_CURRENT_USER, testKeyName};
      if (!testKey.GetHandle()) throw TXOwl{_T("Failed to create registry key HKEY_CURRENT_USER\\") + testKeyName};

      if (testKey.SetValue(_T(""), _T("DefaultString")) != ERROR_SUCCESS) throw TXOwl{_T("Failed to create default string for registry key HKEY_CURRENT)USER\\") + testKeyName};
      if (testKey.SetValue(_T("Uint32Value"), 32ul) != ERROR_SUCCESS) throw TXOwl{_T("Failed to create uint32 value for registry key HKEY_CURRENT)USER\\") + testKeyName};
      if (testKey.SetValue(_T("Uint64Value"), 64ull) != ERROR_SUCCESS) throw TXOwl{_T("Failed to create uint64 value for registry key HKEY_CURRENT)USER\\") + testKeyName};
      if (testKey.SetValue(_T("StringValue"), _T("StringValueString")) != ERROR_SUCCESS) throw TXOwl{_T("Failed to create string value for registry key HKEY_CURRENT)USER\\") + testKeyName};

      TRegValue{testKey, _T("")} = _T("DefaultStringModified");
      TRegValue{testKey, _T("Uint32Value")} = 33ul;
      TRegValue{testKey, _T("Uint64Value")} = 65ull;
      TRegValue{testKey, _T("StringValue")} = _T("StringValueStringModified");

      TRegValue{testKey, _T("AnotherUint32Value")} = 32ul;
      TRegValue{testKey, _T("AnotherUint64Value")} = 64ull;
      TRegValue{testKey, _T("AnotherStringValue")} = _T("AnotherStringValueString");

      // Test key information retrieval (RegQueryInfoKey).
      //
      const auto i = testKey.QueryInfo();
      const auto lastWriteTime = TTime{i.LastWriteTime}.AsString();
      TRACE(_T("Info for registry key HKEY_CURRENT_USER\\") << testKeyName << _T(": {") <<
        _T("Class: \"") << i.Class << _T("\", ") <<
        _T("SubKeyCount: ") << i.SubkeyCount << _T(", ") <<
        _T("MaxSubkeyNameSize: ") << i.MaxSubkeyNameSize << _T(", ") <<
        _T("MaxSubkeyClassSize: ") << i.MaxSubkeyClassSize << _T(", ") <<
        _T("ValueCount: ") << i.ValueCount << _T(", ") <<
        _T("MaxValueNameSize: ") << i.MaxValueNameSize << _T(", ") <<
        _T("MaxValueDataSize: ") << i.MaxValueDataSize << _T(", ") <<
        _T("SecurityDescriptorSize: ") << i.SecurityDescriptorSize << _T(", ") <<
        _T("LastWriteTime: \"") << lastWriteTime << _T("\"}")
      );
      auto lwt = FILETIME{};
      const auto r = testKey.QueryInfo(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &lwt);
      if (r != ERROR_SUCCESS) throw TXOwl{_T("QueryInfo failed for registry key HKEY_CURRENT_USER\\") + testKeyName};
      CHECK(TTime{lwt}.AsString() == lastWriteTime);
    }

    // Test deletion.
    {
      const auto r = TRegKey::GetCurrentUser().DeleteKey(testKeyName);
      if (r != ERROR_SUCCESS) throw TXOwl{_T("Failed to delete registry key HKEY_CURRENT_USER\\") + testKeyName};
    }

    // Test Update and Validate.
    {
      auto s = tstringstream{};
      if constexpr (false) // This simple test should work for 6.44.9 and earlier.
      {
        s << testKeyName << _T("\\Key1=Key1DefaultText\n")
          << testKeyName << _T("\\Key2 = Key2DefaultText\n")
          << testKeyName << _T("\\Key3  =  Key3DefaultText\n");
      }
      else // This tougher test fails on 6.44.9 and earlier.
      {
        s << testKeyName << _T("=DefaultText\n")
          << testKeyName << _T("|Value1=Text1\n")
          << testKeyName << _T("|Value2 = Text2\n")
          << testKeyName << _T("|Value3  =  Text3\n")
          << testKeyName << _T("\\Key1  =  Key1DefaultText\n")
          << testKeyName << _T("\\Key1|Key1Value1=Key1Text1\n")
          << testKeyName << _T("\\Key1|Key1Value2 = Key1Text2\n")
          << testKeyName << _T("\\Key1|Key1Value3  =  Key1Text3\n");
      }
      auto testKey = TRegKey{HKEY_CURRENT_USER, testKeyName};
      TRegistry::Update(testKey, s);
      s.clear();
      s.seekg(0);
      const auto diff = TRegistry::Validate(testKey, s);
      if (diff != 0) throw TXOwl{_T("Failed to validate registry key HKEY_CURRENT_USER\\") + testKeyName};
    }

    // Test security attributes access and modification.
    {
      const auto test = [](TRegKey& k, SECURITY_INFORMATION i, const tstring& iDescription)
      {
        const auto fmtOpt = [](auto opt) -> tstring
        {
          if (!opt) return _T("(not present)");
          auto s = tostringstream{};
          s << *opt;
          return s.str();
        };
        try
        {
          TRACE(_T("Testing key ") << k.GetName() << _T(" with security information ") << iDescription);
          auto s = k.GetSecurity(i);
          const auto data = s.GetData();
          TRACE(_T("TSecurityDescriptor::GetData: ") << data);
          const auto dataSize = s.GetDataSize();
          TRACE(_T("TSecurityDescriptor::GetDataSize: ") << dataSize);
          const auto valid = s.IsValid();
          TRACE(_T("TSecurityDescriptor::IsValid: ") << boolalpha << valid);
          const auto length = s.GetLength();
          TRACE(_T("TSecurityDescriptor::GetLength: ") << length << _T(" (SECURITY_DESCRIPTOR_MIN_LENGTH == ") << SECURITY_DESCRIPTOR_MIN_LENGTH << _T(')'));
          const auto control = s.GetControl();
          TRACE(_T("TSecurityDescriptor::GetControl: ") << control);
          const auto revision = s.GetRevision();
          TRACE(_T("TSecurityDescriptor::GetRevision: ") << revision);

          TRACE(_T("TSecurityDescriptor::IsAbsolute: ") << boolalpha << s.IsAbsolute());
          TRACE(_T("TSecurityDescriptor::IsDaclAutoInheritRequired: ") << boolalpha << s.IsDaclAutoInheritRequired());
          TRACE(_T("TSecurityDescriptor::IsDaclAutoInherited: ") << boolalpha << s.IsDaclAutoInherited());
          TRACE(_T("TSecurityDescriptor::IsDaclDefaulted: ") << boolalpha << s.IsDaclDefaulted());
          TRACE(_T("TSecurityDescriptor::IsDaclPresent: ") << boolalpha << s.IsDaclPresent());
          TRACE(_T("TSecurityDescriptor::IsDaclProtected: ") << boolalpha << s.IsDaclProtected());
          TRACE(_T("TSecurityDescriptor::IsGroupDefaulted: ") << boolalpha << s.IsGroupDefaulted());
          TRACE(_T("TSecurityDescriptor::IsOwnerDefaulted: ") << boolalpha << s.IsOwnerDefaulted());
          TRACE(_T("TSecurityDescriptor::IsRmControlValid: ") << boolalpha << s.IsRmControlValid());
          TRACE(_T("TSecurityDescriptor::IsSaclAutoInheritRequired: ") << boolalpha << s.IsSaclAutoInheritRequired());
          TRACE(_T("TSecurityDescriptor::IsSaclAutoInherited: ") << boolalpha << s.IsSaclAutoInherited());
          TRACE(_T("TSecurityDescriptor::IsSaclDefaulted: ") << boolalpha << s.IsSaclDefaulted());
          TRACE(_T("TSecurityDescriptor::IsSaclPresent: ") << boolalpha << s.IsSaclPresent());
          TRACE(_T("TSecurityDescriptor::IsSaclProtected: ") << boolalpha << s.IsSaclProtected());
          TRACE(_T("TSecurityDescriptor::IsSelfRelative: ") << boolalpha << s.IsSelfRelative());

          const auto group = s.GetGroup();
          TRACE(_T("TSecurityDescriptor::GetGroup: ") << group);
          const auto owner = s.GetOwner();
          TRACE(_T("TSecurityDescriptor::GetOwner: ") << owner);
          const auto dacl = s.GetDacl();
          TRACE(_T("TSecurityDescriptor::GetDacl: ") << fmtOpt(dacl));
          const auto sacl = s.GetSacl();
          TRACE(_T("TSecurityDescriptor::GetSacl: ") << fmtOpt(sacl));
          const auto rmControl = s.GetRmControl();
          TRACE(_T("TSecurityDescriptor::GetRmControl: ") << fmtOpt(rmControl));

          s.Initialize();
          WARN(!s.IsValid(), _T("TSecurityDescriptor::Initialize invalidated the security descriptor."));

          // Test removing and setting group and owner.
          //
          s.RemoveGroup();
          CHECK(!s.GetGroup() && !s.IsGroupDefaulted());
          auto newGroup = SID{SID_REVISION};
          s.SetGroup(&newGroup, true);
          CHECK(s.GetGroup() == &newGroup && s.IsGroupDefaulted());
          s.RemoveOwner();
          CHECK(!s.GetOwner() && !s.IsOwnerDefaulted());
          auto newOwner = SID{SID_REVISION};
          s.SetOwner(&newOwner, true);
          CHECK(s.GetOwner() == &newOwner && s.IsOwnerDefaulted());

          // Test removing and setting DACL and SACL.
          //
          s.RemoveDacl();
          CHECK(!s.IsDaclPresent() && !s.IsDaclDefaulted());
          auto newDacl = ACL{ACL_REVISION};
          s.SetDacl(&newDacl, true);
          CHECK(s.IsDaclPresent() && (*s.GetDacl()) == &newDacl && s.IsDaclDefaulted());
          s.RemoveSacl();
          CHECK(!s.IsSaclPresent() && !s.IsSaclDefaulted());
          auto newSacl = ACL{ACL_REVISION};
          s.SetSacl(&newSacl, true);
          CHECK(s.IsSaclPresent() && (*s.GetSacl()) == &newSacl && s.IsSaclDefaulted());

          // Test setting, clearing and removing resource manager control bits.
          //
          s.SetRmControl(255);
          CHECK(s.IsRmControlValid());
          const auto rmControl1 = s.GetRmControl();
          CHECK(rmControl1 && *rmControl1 == 255);
          s.SetRmControl(0);
          CHECK(s.IsRmControlValid());
          const auto rmControl2 = s.GetRmControl();
          CHECK(rmControl2 && *rmControl2 == 0);
          s.RemoveRmControl();
          CHECK(!s.IsRmControlValid());
          const auto rmControl3 = s.GetRmControl();
          CHECK(!rmControl3);
        }
        catch ([[maybe_unused]] const TXRegistry& x)
        {
          WARN(true, _T("Unhandled TXRegistry for key ") << k.GetName() << _T(": ")
            << to_tstring(x.what())
            << _T(", Error code: ") << x.GetErrorCode()
          );
        }
        catch ([[maybe_unused]] const TXOwl& x)
        {
          WARN(true, _T("Unhandled TXOwl for key ") << k.GetName() << _T(": ")
            << to_tstring(x.what()));
        }
        catch ([[maybe_unused]] const exception& x)
        {
          WARN(true, _T("Unhandled exception for key ") << k.GetName() << _T(": ")
            << to_tstring(x.what()));
        }
      };
      auto testKey = TRegKey{HKEY_CURRENT_USER, testKeyName};
      test(testKey, ATTRIBUTE_SECURITY_INFORMATION, _T("ATTRIBUTE_SECURITY_INFORMATION"));
      test(testKey, BACKUP_SECURITY_INFORMATION, _T("BACKUP_SECURITY_INFORMATION"));
      test(testKey, DACL_SECURITY_INFORMATION, _T("DACL_SECURITY_INFORMATION"));
      test(testKey, GROUP_SECURITY_INFORMATION, _T("GROUP_SECURITY_INFORMATION"));
      test(testKey, LABEL_SECURITY_INFORMATION, _T("LABEL_SECURITY_INFORMATION"));
      test(testKey, OWNER_SECURITY_INFORMATION, _T("OWNER_SECURITY_INFORMATION"));
      test(testKey, PROTECTED_DACL_SECURITY_INFORMATION, _T("PROTECTED_DACL_SECURITY_INFORMATION"));
      test(testKey, PROTECTED_SACL_SECURITY_INFORMATION, _T("PROTECTED_SACL_SECURITY_INFORMATION"));
      test(testKey, SACL_SECURITY_INFORMATION, _T("SACL_SECURITY_INFORMATION"));
      test(testKey, SCOPE_SECURITY_INFORMATION, _T("SCOPE_SECURITY_INFORMATION"));
      test(testKey, UNPROTECTED_DACL_SECURITY_INFORMATION, _T("UNPROTECTED_DACL_SECURITY_INFORMATION"));
      test(testKey, UNPROTECTED_SACL_SECURITY_INFORMATION, _T("UNPROTECTED_SACL_SECURITY_INFORMATION"));
      test(testKey,
        ATTRIBUTE_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION | SCOPE_SECURITY_INFORMATION,
        _T("ATTRIBUTE_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION | SCOPE_SECURITY_INFORMATION"));
      test(testKey,
        DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION,
        _T("DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION"));


      test(TRegKey::GetCurrentUser(), ATTRIBUTE_SECURITY_INFORMATION, _T("ATTRIBUTE_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), BACKUP_SECURITY_INFORMATION, _T("BACKUP_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), DACL_SECURITY_INFORMATION, _T("DACL_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), GROUP_SECURITY_INFORMATION, _T("GROUP_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), LABEL_SECURITY_INFORMATION, _T("LABEL_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), OWNER_SECURITY_INFORMATION, _T("OWNER_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), PROTECTED_DACL_SECURITY_INFORMATION, _T("PROTECTED_DACL_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), PROTECTED_SACL_SECURITY_INFORMATION, _T("PROTECTED_SACL_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), SACL_SECURITY_INFORMATION, _T("SACL_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), SCOPE_SECURITY_INFORMATION, _T("SCOPE_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), UNPROTECTED_DACL_SECURITY_INFORMATION, _T("UNPROTECTED_DACL_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(), UNPROTECTED_SACL_SECURITY_INFORMATION, _T("UNPROTECTED_SACL_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(),
        ATTRIBUTE_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION | SCOPE_SECURITY_INFORMATION,
        _T("ATTRIBUTE_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION | SCOPE_SECURITY_INFORMATION"));
      test(TRegKey::GetCurrentUser(),
        DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION,
        _T("DACL_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION"));
    }

    // Clean up by testing NukeKey.
    {
      const auto r = TRegKey::GetCurrentUser().NukeKey(testKeyName);
      if (r != ERROR_SUCCESS) throw TXOwl{_T("Failed to nuke registry key HKEY_CURRENT_USER\\") + testKeyName};
    }

    // We're done!
    //
    MessageBox(_T("All tests succeeded!"), messageTitle, MB_ICONINFORMATION);
  }
  catch (const exception& x)
  {
    MessageBox(_T("Some test failed:\n\n") + to_tstring(x.what()), messageTitle, MB_ICONERROR);
  }
}


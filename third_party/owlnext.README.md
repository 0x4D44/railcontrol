# OWLNext Source Placeholder

Stage 1 requires cloning the latest OWLNext (v7.x) sources into this repository so the MSVC toolchain can link against locally built static libraries.

## Acquisition Steps (to run once Visual Studio/MSVC is installed)
Source archive is hosted on SourceForge. Download the latest 7.x release:

```powershell
cd third_party
Invoke-WebRequest -Uri https://downloads.sourceforge.net/project/owlnext/OWLNExt%207.0/owlnext_7_0_20.zip -OutFile owlnext.zip
Expand-Archive owlnext.zip -DestinationPath owlnext
Remove-Item owlnext.zip
```

After cloning, document the exact commit/tag in `docs/environment_readiness.md` and archive any required build outputs under `third_party/owlnext/msvc/`.

- Use `build\msvc\build_owlnext_msvc.bat` to build the static Win32 libraries (COMPAT=5) once Visual Studio is installed; outputs land in `third_party\owlnext\lib`.

_Created: 17 Oct 2025 (updated same day)_

[Setup]
AppName=TCA++
AppVersion=1.0.0
DefaultDirName={pf}\TCAPP
DefaultGroupName=TCAPP
OutputBaseFilename=TCA++_Installer
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "build\windows\x86\tca++.exe"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: ignoreversion
Source: "libs\x86\*.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: ignoreversion
Source: "build\windows\x64\tca++.exe"; DestDir: "{app}"; Check: Is64BitInstallMode; Flags: ignoreversion
Source: "libs\x64\*.dll"; DestDir: "{app}"; Check: Is64BitInstallMode; Flags: ignoreversion

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Flags: preservestringtype

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

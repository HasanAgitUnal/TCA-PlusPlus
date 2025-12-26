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
ChangesEnvironment=yes

[Files]
Source: "build\windows\x86\tca++.exe"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: ignoreversion
Source: "libs\x86\*.dll"; DestDir: "{app}"; Check: not Is64BitInstallMode; Flags: ignoreversion
Source: "build\windows\x64\tca++.exe"; DestDir: "{app}"; Check: Is64BitInstallMode; Flags: ignoreversion
Source: "libs\x64\*.dll"; DestDir: "{app}"; Check: Is64BitInstallMode; Flags: ignoreversion

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueName: "Path"; ValueType: expandsz; ValueData: "{olddata};{app}"; Flags: preservestringtype; \
    Check: NeedsAddPath('{app}')

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Code]
function NeedsAddPath(Param: string): boolean;
var
  ExistingPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', ExistingPath)
  then begin
    Result := True;
    exit;
  end;
  // Look for the path with ";" as a separator
  Result := Pos(';' + Param + ';', ';' + ExistingPath + ';') = 0;
  // If not found, look for it without the separator at the end
  if Result then
    Result := Pos(Param + ';', ExistingPath + ';') <> 1;
end;

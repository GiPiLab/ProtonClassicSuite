; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{3AC6F3EF-BE1C-4918-8DD1-010DD0A423A0}
AppName=ProtonClassicSuite
AppVersion=1.2.1
;AppVerName=ProtonClassicSuite 060115
AppPublisher=Laboratoire de Recherche pour le D�veloppement Local
AppPublisherURL=https://protonclassic.com
AppSupportURL=https://protonclassic.com
AppUpdatesURL=https://protonclassic.com
DefaultDirName={pf}\ProtonClassicSuite
DefaultGroupName=ProtonClassicSuite
OutputDir=C:\Proton\Forwin
OutputBaseFilename=ProtonClassicSuite_1.2.1_setup
SolidCompression=yes
UninstallDisplayName=ProtonClassicSuite
ShowLanguageDialog=auto
Compression=lzma2/ultra
InternalCompressLevel=ultra
LicenseFile=c:\Proton\Forwin\EULA.txt
Encryption=False
AppCopyright=Laboratoire de Recherche pour le D�veloppement Local
MinVersion=0,10.0
ArchitecturesInstallIn64BitMode=x64
;ArchitecturesAllowed=x64
WizardStyle=Modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Proton\Forwin\ProtonClassicSuite_win64.exe"; DestDir: "{app}"; Flags: ignoreversion; Check: Is64BitInstallMode
Source: "C:\Proton\Forwin\ProtonClassicSuite_win32.exe"; DestDir: "{app}"; Flags: ignoreversion; Check: not Is64BitInstallMode
Source: "C:\Proton\Forwin\deployed_win32\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs ;Check: not Is64BitInstallMode
Source: "C:\Proton\Forwin\deployed_win64\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs ;Check: Is64BitInstallMode
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\ProtonClassicSuite"; Filename: "{app}\ProtonClassicSuite_win32.exe";
Name: "{commondesktop}\ProtonClassicSuite"; Filename: "{app}\ProtonClassicSuite_win32.exe"; Tasks: desktopicon
Name: "{group}\ProtonClassicSuite"; Filename: "{app}\ProtonClassicSuite_win64.exe";
Name: "{commondesktop}\ProtonClassicSuite"; Filename: "{app}\ProtonClassicSuite_win64.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\ProtonClassicSuite_win32.exe"; Description: "{cm:LaunchProgram,ProtonClassicSuite}"; Flags: nowait postinstall skipifsilent;Check: not Is64BitInstallMode
Filename: "{app}\ProtonClassicSuite_win64.exe"; Description: "{cm:LaunchProgram,ProtonClassicSuite}"; Flags: nowait postinstall skipifsilent;Check: Is64BitInstallMode

#pragma once

#include <string>

class CDosUtil {
	TCHAR dirname[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	std::wstring ProgrmDir;
	std::wstring LocalCfg;
	std::wstring LocalSys;

	std::wstring _GetProgramDir();
	std::wstring _GetLocalSysFile();
	std::wstring _GetLocalCfgFile();

public:

	double ReadCfgINI(const TCHAR* sectionconst, const TCHAR* name, double defvalue);
	float ReadCfgINI(const TCHAR* section, const  TCHAR* name, float defvalue);
	int ReadCfgINI(const TCHAR* section, const  TCHAR* name, int defvalue);
	std::wstring ReadCfgINI(const TCHAR* section, const TCHAR* name, TCHAR* str);
	ULONG ReadCfgINI(const TCHAR* section, const  TCHAR* name, ULONG defvalue);

	//void WriteCfgINI(const TCHAR* section, const  TCHAR* key, const TCHAR* str);
	void WriteCfgINI(const TCHAR* section, const  TCHAR* key, const TCHAR* str);
	void WriteCfgINI(const TCHAR* section, const  TCHAR* key, double val);
	void WriteCfgINI(const TCHAR* section, const  TCHAR* key, float val);
	void WriteCfgINI(const TCHAR* section, const  TCHAR* key, int val);
	void WriteCfgINI(const TCHAR* section, const  TCHAR* key, UINT val);
	void WriteCfgINI(const TCHAR* section, const  TCHAR* key, ULONG val);

	std::wstring GetMotorFile();
	std::wstring GetRecipeDir();
	std::wstring GetResultDir();
	std::wstring GetFilename(const TCHAR* fname, const TCHAR* ext);

	std::wstring GetFSMAppFile();
	std::wstring GetLocalSysFile();
	std::wstring GetLocalCfgFile();
	std::wstring GetProgramDir();
	BOOL GetProgramDir(CString& mdir);
	std::wstring RemoveExtension(const TCHAR* filename);
	std::wstring ExtractFilename(const TCHAR* filename);
	std::wstring ExtractDrive(TCHAR* filename);
	std::wstring ExtractFilenameExt(TCHAR* filename);
	std::wstring MakeFullPathName(TCHAR* path, TCHAR* name, TCHAR* ext);
	void AddExtension(TCHAR* filename, TCHAR* ex);
	BOOL MakeFileNameFromModule(TCHAR* filename, TCHAR* name, TCHAR* extn);

	//08142024 - START
	// int ReadINI(const wchar_t* section, const wchar_t* name, int defvalue, CString& inifile);
	int ReadINI(LPCTSTR section, LPCTSTR name, int defvalue, LPCTSTR inifile);
	ULONG ReadINI(const wchar_t* section, const wchar_t* name, ULONG defvalue, CString& inifile);
	float ReadINI(const wchar_t* section, const wchar_t* name, float defvalue, CString& inifile);
	double ReadINI(const wchar_t* section, const wchar_t* name, double defvalue, CString& inifile);
	CString ReadINI(const wchar_t* section, const wchar_t* name, CString& str, CString& inifile);

	void WriteINI(const wchar_t* section, const wchar_t* key, int val, CString& inifile);
	void WriteINI(const wchar_t* section, const wchar_t* key, ULONG val, CString& inifile);
	void WriteINI(const wchar_t* section, const wchar_t* key, UINT val, CString& inifile);
	void WriteINI(const wchar_t* section, const wchar_t* key, const wchar_t* str, CString& inifile);
	void WriteINI(const wchar_t* section, const wchar_t* key, CString& str, CString& inifile);
	void WriteINI(const wchar_t* section, const wchar_t* key, double val, CString& inifile);
	void WriteINI(const wchar_t* section, const wchar_t* key, float val, CString& inifile);
	//08142024 - END

	CDosUtil();
	virtual ~CDosUtil();
};

extern CDosUtil DosUtil;

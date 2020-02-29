#include "CppUnitTest.h"
#include "../CppPy.h"

#include <cassert>
#include <filesystem>
#pragma warning(disable : 4996)
#include <cstdlib>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CppPyMethodTest
{
	void initilize() {
		if (CppPy::System::IsInitlized())return;

		const auto strPyhonHome = std::string{ std::getenv("ANACONDA_HOME") };
		CppPy::System::SetPythonHome(strPyhonHome);

		CppPy::System::Initilize();

		Assert::IsTrue(CppPy::System::IsInitlized());
	}
	void finalize() {
		CppPy::System::Finalize();

		Assert::IsFalse(CppPy::System::IsInitlized());
	}

	TEST_CLASS(PreInitlize_GetTest)
	{
		 
	public:
		
		TEST_METHOD(GetVersion)
		{
			auto temp = CppPy::System::GetVersion();
			Assert::IsFalse(temp.empty());
		}

		TEST_METHOD(GetBuildInfo)
		{
			auto temp = CppPy::System::GetBuildInfo();
			Assert::IsFalse(temp.empty());
		}

		TEST_METHOD(GetPlatform)
		{
			 auto temp = CppPy::System::GetPlatform();
			Assert::IsFalse(temp.empty());
		}

		TEST_METHOD(GetCompiler)
		{
			auto temp = CppPy::System::GetCompiler();
			Assert::IsFalse(temp.empty());
		}
		
		TEST_METHOD(GetCopyringht)
		{
			auto temp = CppPy::System::GetCopyringht();
			Assert::IsFalse(temp.empty());
		}

		TEST_METHOD(GetPath)
		{
			auto temp = CppPy::System::GetPath();
			Assert::IsFalse(temp.empty());
		}

		TEST_METHOD(GetProgramName)
		{
			auto temp = CppPy::System::GetProgramName();
			Assert::IsTrue(temp == "python");
		}

	};

	TEST_CLASS(PreInitlize_SetTest)
	{
		CppPy::System api;
	public:
		TEST_METHOD(SetPythonHome)
		{
			const auto home = std::string{ std::getenv("ANACONDA_HOME") };
			CppPy::System::SetPythonHome(home);

			auto temp = CppPy::System::GetPythonHome();
			Assert::AreEqual(temp.empty(), false);

			Assert::IsTrue(temp == home);
		}

		TEST_METHOD(SetPath)
		{
			const auto pythonPath = std::string{ std::getenv("ANACONDA_HOME") };

			auto oldPath = CppPy::System::GetPath();
			Assert::AreEqual(oldPath.empty(), false);

			auto seq = std::string{ ";" };
			auto path = std::string{};
			
			path += pythonPath + seq;
			path += pythonPath + "\\python37.zip" + seq;
			path += pythonPath + "\\DLLs" + seq;
			path += pythonPath + "\\lib" + seq;
			path += pythonPath + "\\lib\\site-packages" + seq;
			path += pythonPath + "\\lib\\site-packages\\win32" + seq;
			path += pythonPath + "\\lib\\site-packages\\win32\\lib" + seq;
			path += pythonPath + "\\lib\\site-packages\\Pythonwin" + seq;
			path += std::filesystem::current_path().string() + seq;

			CppPy::System::SetPath(path);
			auto newPath = CppPy::System::GetPath();
			Assert::IsTrue(newPath == path);

		}

		TEST_METHOD(SetProgramName) {
			const auto n1 = "n1";
			CppPy::System::SetProgramName(n1);
			const auto n1get =CppPy::System::GetProgramName();
			Assert::IsTrue(n1 == n1get);

		}

		TEST_METHOD(SetIODefaultEncoding) {
			const auto enc = "";
			const auto err = "";
			const auto result = CppPy::System::SetIODefaultEncoding(enc,err);
			Assert::IsTrue(result);
		}

	};

#if !DEBUG/*	Debug実行でテストを行いた時はpython37_d.libが必要*/

	TEST_CLASS(Initilize_Test) {
		TEST_METHOD(IsInitlized) {
			const auto isPreInit = CppPy::System::IsInitlized();
			Assert::IsFalse(isPreInit);
		}
		TEST_METHOD(Initlized) {
			Initlized();
			Assert::IsTrue(CppPy::System::IsInitlized());
			finalize();
		}
	};


	TEST_CLASS(Object_MethodTesd)
	{
		std::string scriptPath = "";

		TEST_METHOD(objectValueConstructors) {
			initilize();

			long lvalue = 15l;
			long long llvalue = 102ll;
			unsigned long ulvalue = 57ul;
			unsigned long long ullvalue = 3892ull;
			double dvalue = 2.3;

			CppPy::object lObject{ lvalue };
			CppPy::object llObject{ llvalue };
			CppPy::object ulObject{ ulvalue };
			CppPy::object ullObject{ ullvalue };
			CppPy::object dObject{ dvalue };

			Assert::AreEqual((long)lObject, lvalue);
			Assert::AreEqual((long long)llObject, llvalue);
			Assert::AreEqual((unsigned long)ulObject, ulvalue);
			Assert::AreEqual((unsigned long long)ullObject, ullvalue);
			Assert::AreEqual((double)dObject, dvalue);

			finalize();
		}

		TEST_METHOD(objectString) {
			initilize();
			const std::string test{ "STRING" };
			const std::wstring wtest{ L"WSTRING" };

			CppPy::object strObject{ test };
			Assert::IsTrue(CppPy::object::isString(strObject));

			const auto str = static_cast<std::string>(strObject);
			Assert::IsTrue(str == test);


			//wchar_tの場合は一度変換
			const auto wStrTest = CppPy::System::ToString(wtest);

			CppPy::object wstrObject{ wStrTest };
			Assert::IsTrue(CppPy::object::isString(wstrObject));

			const auto wstr = static_cast<std::string>(wstrObject);
			Assert::IsTrue(wstr == wStrTest);

			finalize();
		}


		TEST_METHOD(objectDir) {
			initilize();
			const std::vector<std::string> sysTemplateDir{
				"__breakpointhook__",	"__displayhook__",	"__doc__","__excepthook__",	"__interactivehook__",	"__loader__","__name__",
				"__package__","__spec__","__stderr__",	"__stdin__",	"__stdout__","_base_executable",	"_clear_type_cache",	"_current_frames",
				"_debugmallocstats",	"_enablelegacywindowsfsencoding",	"_framework",	"_getframe","_git","_home","_xoptions",	"api_version",
				"argv",	"base_exec_prefix",	"base_prefix",	"breakpointhook","builtin_module_names","byteorder","call_tracing",	"callstats",	"copyright",
				"displayhook",	"dllhandle", 	"dont_write_bytecode","exc_info",	"excepthook",	"exec_prefix",	"executable",	"exit",	"flags",
				"float_info", "float_repr_style", "get_asyncgen_hooks", "get_coroutine_origin_tracking_depth", "get_coroutine_wrapper",
				"getallocatedblocks", "getcheckinterval", "getdefaultencoding", "getfilesystemencodeerrors", "getfilesystemencoding",
				"getprofile", "getrecursionlimit", "getrefcount", "getsizeof", "getswitchinterval", "gettrace", "getwindowsversion",
				"hash_info", "hexversion", "implementation", "int_info", "intern", "is_finalizing", "last_traceback", "last_type", "last_value",
				"maxsize", "maxunicode", "meta_path", "modules", "path", "path_hooks", "path_importer_cache", "platform", "prefix", "ps1",
				"ps2", "set_asyncgen_hooks", "set_coroutine_origin_tracking_depth", "set_coroutine_wrapper", "setcheckinterval", "setprofile",
				"setrecursionlimit", "setswitchinterval", "settrace", "stderr", "stdin", "stdout", "thread_info", "version", "version_info", "warnoptions", "winver" };

			CppPy::object sys = CppPy::object::import("sys");
			const auto sysDir = sys.dir().getInstance();

			Assert::IsTrue(CppPy::object::isList(sysDir));

			finalize();
		}
	};
#endif

}

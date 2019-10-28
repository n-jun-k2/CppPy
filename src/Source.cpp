
#define PY_SSIZE_T_CLEAN //length は int 型ではなく Py_ssize_t 型。
#include <Python.h>

#include <iostream>
#include <string>
#include <vector>

#pragma warning(disable : 4996)
#include <cstdlib>

void PyCheck(PyObject* obj) {
	if (obj != nullptr)return;

	PyObject* errObj, * errData, * errTraceback;
	PyErr_Fetch(&errObj, &errData, &errTraceback);

	if (errObj == nullptr)return;

	if (errTraceback == nullptr) {
		auto errmsg = std::string {PyUnicode_AsUTF8(errObj)};
		std::cout << "Error :" << errmsg << std::endl;
	}
	
	Py_DECREF(errObj);
	Py_DECREF(errData);
	Py_DECREF(errTraceback);

	throw std::runtime_error("");

}

std::vector<std::string> PyListToVextor(PyObject *obj) {
	const auto size = static_cast<int>(PyList_Size(obj));
	std::vector<std::string> result;
	result.resize(size);
	for (int i = 0; i < size; ++i) {
		auto item = PyList_GetItem(obj, i);
		result[i] = std::string{ PyUnicode_AsUTF8(item) };
		Py_DECREF(item);
	}
	return result;
}

int main()
{
	const std::string LINE_BLOCK = "_/_/_/_/_/_/_/_/_/_/";
	const std::string ANACONDA_HOME = std::getenv("ANACONDA_HOME");

	char* err;
	size_t decSize;

	//0. pythonホームを設定する。
	decSize = ANACONDA_HOME.size();
	Py_SetPythonHome(Py_DecodeLocale(ANACONDA_HOME.c_str(), &decSize));

	auto pyVersion		= Py_GetVersion();
	auto pyPlatform		= Py_GetPlatform();
	auto pyCompiler		= Py_GetCompiler();
	auto pyCopyright		= Py_GetCopyright();
	auto pyHomePath	= Py_EncodeLocale(Py_GetPythonHome(),&decSize);

	std::cout << LINE_BLOCK + "初期化前の情報取得" + LINE_BLOCK << std::endl;
	{
		std::cout << pyVersion << std::endl;
		std::cout << pyPlatform << std::endl;
		std::cout << pyCompiler << std::endl;
		std::cout << pyCopyright << std::endl;
		std::cout << pyHomePath << std::endl;
	}

	int errCode;
	Py_Initialize();
	{
		//1. sys.pathにこれからインポートするファイルのディレクトリを指定する。
		auto sysPath = PySys_GetObject("path");
		errCode = PyList_Append(sysPath, PyUnicode_DecodeFSDefault("srcripts"));
	
		//2. ファイルを取得。
		auto sample = PyImport_ImportModule("sample");
		PyCheck(sample);
		
		//3.  __dir___()
		std::cout << LINE_BLOCK + "__dir__" + LINE_BLOCK << std::endl;
		{
			auto dir = PyObject_Dir(sample);
			PyCheck(dir);
			// is list?
			if (PyList_Check(dir))
			{
				std::cout << "__dir__ {" << std::endl;
				//アイテムを取得・表示
				auto _dir = PyListToVextor(dir);
				for (const auto& d  :  _dir) {
					std::cout << d << std::endl;
				}
				std::cout << "}" << std::endl;
			}
			Py_DECREF(dir);
		}

		//4. 関数の取得・実行。
		std::cout << LINE_BLOCK + "関数の処理" + LINE_BLOCK << std::endl;
		{
			{
				//モジュールの属性(関数)を取得
				auto func = PyObject_GetAttrString(sample, "sampleFunc");
				//引数を作成。引数がない場合はタプルのサイズは０にする。
				auto args = PyTuple_New(0);
				//関数を実行・戻り値を取得。
				auto objReturn = PyObject_CallObject(func, args);

				Py_DECREF(func);
				Py_DECREF(args);
				Py_XDECREF(objReturn);
			}

			{
				//モジュールの属性を取得.
				auto func = PyObject_GetAttrString(sample, "sampleFunc2");
				//引数を作成。
				auto args = PyTuple_New(1);
				//値を作成。
				auto value = PyFloat_FromDouble(8.3);
				//引数のタプルに値を追加。
				auto isSuccess = PyTuple_SetItem(args, 0, value);
				//関数を実行・戻り値を取得。
				auto objReturn = PyObject_CallObject(func, args);
				PyCheck(objReturn);
				Py_DECREF(func);
				Py_DECREF(args);
				Py_DECREF(value);
				Py_XDECREF(objReturn);

			}

			//5. クラスの取得・メソッドの実行
			std::cout << LINE_BLOCK + "クラスの処理" + LINE_BLOCK << std::endl;
			{
				//クラスを取得
				auto type = PyObject_GetAttrString(sample, "SampleClass");
				//インスタンスを作成. __Init__の呼出し
				auto instance = PyObject_CallObject(type, nullptr);
				//__call__のメソッドを取得
				auto call = PyObject_GetAttrString(instance, "__call__");
				//処理を呼び出す。
				auto callReturn = PyObject_CallObject(call, nullptr);
				//funcメソッドを取得
				auto func = PyObject_GetAttrString(instance, "Func");
				//処理を呼び出す。
				auto funcReturn = PyObject_CallObject(func, nullptr);

				Py_DECREF(type);
				Py_DECREF(instance);
				Py_DECREF(call);
				Py_XDECREF(callReturn);
				Py_DECREF(func);
				Py_XDECREF(funcReturn);
			}


			//6. クラスの取得・メソッドの実行
			std::cout << LINE_BLOCK + "クラスの処理 2 " + LINE_BLOCK << std::endl;
			{
				auto type = PyObject_GetAttrString(sample, "SampleClass2");

				auto instance = PyObject_CallObject(type, nullptr);

				Py_DECREF(type);
				Py_DECREF(instance);
			}

		}


		Py_DECREF(sample);
		Py_DECREF(sysPath);
	}
	Py_Finalize();


}
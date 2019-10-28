
#define PY_SSIZE_T_CLEAN //length �� int �^�ł͂Ȃ� Py_ssize_t �^�B
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

	//0. python�z�[����ݒ肷��B
	decSize = ANACONDA_HOME.size();
	Py_SetPythonHome(Py_DecodeLocale(ANACONDA_HOME.c_str(), &decSize));

	auto pyVersion		= Py_GetVersion();
	auto pyPlatform		= Py_GetPlatform();
	auto pyCompiler		= Py_GetCompiler();
	auto pyCopyright		= Py_GetCopyright();
	auto pyHomePath	= Py_EncodeLocale(Py_GetPythonHome(),&decSize);

	std::cout << LINE_BLOCK + "�������O�̏��擾" + LINE_BLOCK << std::endl;
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
		//1. sys.path�ɂ��ꂩ��C���|�[�g����t�@�C���̃f�B���N�g�����w�肷��B
		auto sysPath = PySys_GetObject("path");
		errCode = PyList_Append(sysPath, PyUnicode_DecodeFSDefault("srcripts"));
	
		//2. �t�@�C�����擾�B
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
				//�A�C�e�����擾�E�\��
				auto _dir = PyListToVextor(dir);
				for (const auto& d  :  _dir) {
					std::cout << d << std::endl;
				}
				std::cout << "}" << std::endl;
			}
			Py_DECREF(dir);
		}

		//4. �֐��̎擾�E���s�B
		std::cout << LINE_BLOCK + "�֐��̏���" + LINE_BLOCK << std::endl;
		{
			{
				//���W���[���̑���(�֐�)���擾
				auto func = PyObject_GetAttrString(sample, "sampleFunc");
				//�������쐬�B�������Ȃ��ꍇ�̓^�v���̃T�C�Y�͂O�ɂ���B
				auto args = PyTuple_New(0);
				//�֐������s�E�߂�l���擾�B
				auto objReturn = PyObject_CallObject(func, args);

				Py_DECREF(func);
				Py_DECREF(args);
				Py_XDECREF(objReturn);
			}

			{
				//���W���[���̑������擾.
				auto func = PyObject_GetAttrString(sample, "sampleFunc2");
				//�������쐬�B
				auto args = PyTuple_New(1);
				//�l���쐬�B
				auto value = PyFloat_FromDouble(8.3);
				//�����̃^�v���ɒl��ǉ��B
				auto isSuccess = PyTuple_SetItem(args, 0, value);
				//�֐������s�E�߂�l���擾�B
				auto objReturn = PyObject_CallObject(func, args);
				PyCheck(objReturn);
				Py_DECREF(func);
				Py_DECREF(args);
				Py_DECREF(value);
				Py_XDECREF(objReturn);

			}

			//5. �N���X�̎擾�E���\�b�h�̎��s
			std::cout << LINE_BLOCK + "�N���X�̏���" + LINE_BLOCK << std::endl;
			{
				//�N���X���擾
				auto type = PyObject_GetAttrString(sample, "SampleClass");
				//�C���X�^���X���쐬. __Init__�̌ďo��
				auto instance = PyObject_CallObject(type, nullptr);
				//__call__�̃��\�b�h���擾
				auto call = PyObject_GetAttrString(instance, "__call__");
				//�������Ăяo���B
				auto callReturn = PyObject_CallObject(call, nullptr);
				//func���\�b�h���擾
				auto func = PyObject_GetAttrString(instance, "Func");
				//�������Ăяo���B
				auto funcReturn = PyObject_CallObject(func, nullptr);

				Py_DECREF(type);
				Py_DECREF(instance);
				Py_DECREF(call);
				Py_XDECREF(callReturn);
				Py_DECREF(func);
				Py_XDECREF(funcReturn);
			}


			//6. �N���X�̎擾�E���\�b�h�̎��s
			std::cout << LINE_BLOCK + "�N���X�̏��� 2 " + LINE_BLOCK << std::endl;
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
#pragma once

#define PY_SSIZE_T_CLEAN //length は int 型ではなく Py_ssize_t 型。
#include <Python.h>

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <memory>


namespace CppPy {

	/* C Python API のオブジェクトポインター */
	using pyPtr = std::shared_ptr<PyObject>;

	/*　コードジェネレーター用	*/
	template <class t, class... Args>
	struct builder {
		/* Todo:メソッド名修正*/
		auto getInstance(Args&&... args) {
			return t{ args... };
		}
	};

	/*　コードジェネレーター用	*/
	template <class t, class _rt,class... Args>
	struct builder<t, _rt(Args...), Args...> {		
		builder(std::function<_rt(Args...)>&& make,Args... args):value(make(args...)){}
		/* Todo:メソッド名修正*/
		t getInstance() {return t{value};}
		_rt value;
	};

	/* Python API　チェック関数 (未完) */
	static PyObject* check(PyObject* obj, const std::string& msg, std::ostream& ostr = std::cout) {
		//pythonのオブジェクトが取得できている場合エラーでない。
		if (obj != nullptr)return obj;

		PyObject* errObj, * errData, * errTraceback;
		PyErr_Fetch(&errObj, &errData, &errTraceback);
		//pythonのインタプリタのエラー出力を取得。
		if (errObj == nullptr)return obj;
		//例外メッセージ出力フォーマット
		if (errTraceback == nullptr) {
			auto errmsg = std::string{ PyUnicode_AsUTF8(errObj) };
			ostr << msg << errmsg << std::endl;
		}
		//不要な局所オブジェクトの参照カウントを減らす。
		Py_DECREF(errObj);
		Py_DECREF(errData);
		Py_DECREF(errTraceback);
		
		return nullptr;
	}
	

	/*ADL firewall object 先行宣言*/
	namespace __object__ {
		struct object;
	}using object = __object__::object;
	namespace __tuple__ {
		struct tuple;
	}using tuple = __tuple__::tuple;
	namespace __iterator__{
		struct iterator;
	}using iterator = __iterator__::iterator;
	namespace __list__ {
		struct list;
	}using list = __list__::list;

	using ListBuilder = builder<list,PyObject*(PyObject*),PyObject*>;


	/*
	インタプリタ制御
	*/
	class System {
	public:
		System(const System&) = delete;
		System& operator=(const System&) = delete;
		System(System&&) = delete;
		System& operator = (System&&) = delete;

		System() {
			
		}
		~System() {
		
		}
		static  std::string GetVersion()  { 
			return std::string{ Py_GetVersion() };
		}
		static  std::string GetPlatform()  {
			return std::string{ Py_GetPlatform() };
		}
		static  std::string GetCompiler()  {
			return std::string{ Py_GetCompiler() };
		}
		static  std::string GetBuildInfo() {
			return std::string{ Py_GetBuildInfo() };
		}
		static  std::string GetCopyringht() {
			return std::string{ Py_GetCopyright() };
		}
		static  std::string GetPythonHome() {
			return ToString(Py_GetPythonHome());
		}
		static  std::string  GetPath()  {
			return ToString(Py_GetPath());
		}
		static std::string GetProgramName() {
			return ToString( Py_GetProgramName() );
		}
		static  std::string ToString(const std::wstring& msg) 
		{
			size_t size;
			return std::string{ Py_EncodeLocale(msg.c_str(),&size) };
		}
		static  std::wstring ToString(const std::string& msg)
		{
			size_t size;
			return std::wstring{ Py_DecodeLocale(msg.c_str(),&size) };
		}
		static void SetPythonHome(const std::string& path) 
		{
			size_t e;
			Py_SetPythonHome(Py_DecodeLocale(path.c_str(), &e));
		}
		static void SetPythonHome(const std::wstring& path)
		{
			Py_SetPythonHome(path.c_str());
		}
		static void SetPath(const std::string& path) 
		{
			size_t e;
			Py_SetPath(Py_DecodeLocale(path.c_str(), &e));
		}
		static void SetPath(const std::wstring& path)
		{
			Py_SetPath(path.c_str());
		}
		static void SetProgramName(const std::string& name) {
			return Py_SetProgramName(ToString(name).c_str());
		}
		static void SetProgramName(const std::wstring& name) {
			return Py_SetProgramName(name.c_str());
		}
		static bool SetIODefaultEncoding(const std::string& enc,const std::string& err) {
			return Py_SetStandardStreamEncoding(enc.c_str(),err.c_str()) == 0;
		}
		/*初期化済みの場合True*/
		static bool IsInitlized() {
			return Py_IsInitialized() > 0;
		}
		static void Initilize(const int ex = 1) {
			return Py_InitializeEx(ex);
		}
		static void Finalize() {
			Py_FinalizeEx();
		}
	};
	  
	//ADL firewall
	namespace __object__ {
		struct object {
			PyObject* _ptr;

			virtual ~object() {	Py_XDECREF(_ptr);}
			object(PyObject* o) : _ptr(o) {}
			object(const double& value) { _ptr = PyFloat_FromDouble(value); };
			object(const long& value) { _ptr = PyLong_FromLong(value); };
			object(const long long& value) { _ptr = PyLong_FromLongLong(value); }
			object(const unsigned long& value) { _ptr = PyLong_FromUnsignedLong(value); }
			object(const unsigned long long& value) { _ptr = PyLong_FromUnsignedLongLong(value); }
			object(const std::string& value) { _ptr = PyByteArray_FromStringAndSize(value.data(), static_cast<Py_ssize_t>(value.size())); };

			operator double() const { return PyFloat_AsDouble(_ptr); }
			operator long() const { return PyLong_AsLong(_ptr); }
			operator long long() const { return PyLong_AsLongLong(_ptr); }
			operator unsigned long() const { return PyLong_AsUnsignedLong(_ptr); }
			operator unsigned long long() const { return PyLong_AsUnsignedLongLong(_ptr); }
			operator std::string() const { return std::string{ PyByteArray_AsString(_ptr) }; }

			static object getSysObject(const std::string& name) { return object{ PySys_GetObject(name.c_str()) }; }
			static bool isByte(const object& o) {
				return PyBytes_Check(o._ptr) == 0;
			}
			static bool isByteArray(const object& o) {
				return PyByteArray_Check(o._ptr) == 0;
			}
			static bool isString(const object& o) {
				return std::strcmp(o._ptr->ob_type->tp_name,"str") == 0;
			}
			static bool isBuffer(const object& o) {
				return std::strcmp( o._ptr->ob_type->tp_name,"buffer") == 0;
			}
			static bool isList(const object& o) {
				return std::strcmp (o._ptr->ob_type->tp_name,"list") == 0;
			}
			static bool isTuple(const object& o) {
				return std::strcmp( o._ptr->ob_type->tp_name,"tuple") == 0;
			}
			static bool isModule(const object& o) {
				return std::strcmp(o._ptr->ob_type->tp_name,"module") == 0;
			}

			static object concat(const object& left, const object& right) { return object{ PyByteArray_Concat(left._ptr,right._ptr) }; }
			static object import(const std::string& name) { return object{ PyImport_ImportModule(name.c_str()) }; }

			ListBuilder dir() { return  ListBuilder{PyObject_Dir, _ptr}; }
			const object& attr(const std::string& name) { return object{ PyObject_GetAttrString(_ptr,name.c_str()) }; }
			const object& call(const object& tuple) { return object{ PyObject_CallObject(_ptr,tuple._ptr) }; }
			const size_t size() { return static_cast<size_t>(PyObject_Size(_ptr)); }
		};
	}
	
	namespace __tuple__ {
		struct tuple : public  object {
			tuple(PyObject* _ptr) : object(_ptr) {}
			tuple(const size_t& size) :object(PyTuple_New(size)) {	}
			template<class... Args>tuple(Args... arg) : object(PyTuple_Puck(sizeof...(arg), arg...)) {}
			~tuple() {}
			tuple slice(const uint32_t& start, const uint32_t& end) { return tuple{ PyTuple_GetSlice(_ptr, static_cast<Py_ssize_t>(start),static_cast<Py_ssize_t>(end)) }; }
			object getItem(uint32_t index) { return object{ PyTuple_GetItem(_ptr,index) }; }
			tuple& setItem(uint32_t index, const object& ob) { PyTuple_SetItem(_ptr, static_cast<Py_ssize_t>(index), ob._ptr); return *this; }
			size_t length() { return static_cast<size_t>(PyTuple_Size(_ptr)); }
		};
	}
	
	namespace __iterator__{
		struct iterator : public object {
			PyObject* item;
			iterator():object(nullptr),item(nullptr){}
			iterator(PyObject* obj) : object(PyObject_GetIter(obj)),item(nullptr){}
			const object& operator*(){
				return object{ item };
			}
			iterator& operator++() {
				item = PyIter_Next(_ptr);
				return *this;
			}
			bool operator!=(const iterator& v){
				return item == v.item;
			}
		};
	}

	namespace __list__ {

		struct list : public  object {
			list(PyObject* o) : object(o) {}
			list(const size_t len) : object(PyList_New(len)) {	}
			~list() {}
			const list& append(const object& item) {
				PyList_Append(_ptr, item._ptr);
				return *this;
			}
			const list& insert(const object& item, const uint32_t& index) {
				PyList_Insert(_ptr, index, item._ptr);
				return *this;
			}
			const size_t size() {
				return static_cast<size_t>(PyList_Size(_ptr));
			}
			const list& slice(const uint32_t& row, const uint32_t& heigth) {
				return  list{ PyList_GetSlice(_ptr, row, heigth) };
			}
			const list& reverse() {
				PyList_Reverse(_ptr);
				return *this;
			}
			const object& operator[](uint32_t index) { 
				return object{ PyList_GetItem(_ptr,index) };
			}

			iterator begin() {
				return iterator(_ptr);
			}

			iterator end(){
				return iterator();
			}
			
		};
	}
	
}
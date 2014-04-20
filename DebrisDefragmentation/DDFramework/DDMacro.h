#pragma once

#define WM_SOCKET 104
#define ALIGNMENT_SIZE 16

template <typename T>
inline void SafeDelete( T* &p )
{
	if ( p != nullptr )
	{
		delete p;
		p = nullptr;
	}
}

template <typename T>
inline void SafeArrayDelete( T* &p )
{
	if ( p != nullptr )
	{
		delete[] p;
		p = nullptr;
	}
}
// 
// template<typename T>
// struct mallocDeleter
// {
// 	void operator() ( T*& ptr )
// 	{
// 		if ( ptr )
// 		{
// 			ptr->~T(); 
// 			_aligned_free(ptr); 
// 		}
// 	}
// };

// #define CREATE_FUNC(CLASS_NAME) \
// 	static CLASS_NAME* Create() \
// { \
// 	CLASS_NAME* pInstance = new CLASS_NAME(); \
// 	return pInstance; \
// }
// 
// // string입력 받는 버전
// #define CREATE_FUNC_WSTRING(CLASS_NAME, str) \
// 	static CLASS_NAME* Create(wchar_t* str) \
// { \
// 	CLASS_NAME* pInstance = new CLASS_NAME(str); \
// 	return pInstance; \
// }
// 
#define CREATE_FUNC(CLASS_NAME) \
	static CLASS_NAME* Create() \
{ \
	CLASS_NAME* pInstance = new (_aligned_malloc(sizeof(CLASS_NAME), ALIGNMENT_SIZE)) CLASS_NAME(); \
	return pInstance; \
}

// string입력 받는 버전
#define CREATE_FUNC_WSTRING(CLASS_NAME, str) \
	static CLASS_NAME* Create(std::wstring str) \
{ \
	CLASS_NAME* pInstance = new (_aligned_malloc(sizeof(CLASS_NAME), ALIGNMENT_SIZE)) CLASS_NAME(str); \
	return pInstance; \
}

#define CREATE_FUNC_INTEGER(CLASS_NAME, integer) \
	static CLASS_NAME* Create(int integer) \
{ \
	CLASS_NAME* pInstance = new (_aligned_malloc(sizeof(CLASS_NAME), ALIGNMENT_SIZE)) CLASS_NAME(integer); \
	return pInstance; \
}

// 
// #define CREATE_FUNC(CLASS_NAME) \
// 	static CLASS_NAME* CreateClass() \
// { \
// 	CLASS_NAME* pInstance = (CLASS_NAME*)_aligned_malloc( sizeof( CLASS_NAME ), ALIGNMENT_SIZE ); \
// 	new (pInstance)CLASS_NAME(); \
// 	return pInstance; \
// }
// 
// // string입력 받는 버전
// #define CREATE_FUNC_WSTRING(CLASS_NAME, str) \
// 	static CLASS_NAME* CreateClass(wchar_t* str) \
// { \
// 	CLASS_NAME* pInstance = (CLASS_NAME*)_aligned_malloc( sizeof( CLASS_NAME ), ALIGNMENT_SIZE ); \
// 	new (pInstance)CLASS_NAME(str); \
// 	return pInstance; \
// }
// 
// #define DESTROY_FUNC(CLASS_NAME) \
// 	void DestroyClass() \
// { \
// 	this->~CLASS_NAME(); \
// 	_aligned_free(this); \	
// } 


// 
// template<typename T>
// struct AlignedObjectDeleter
// {
// 	m_pInstance->~T();
//	_aligned_free( m_pInstance );	
// };


template <class T>
class Singleton
{
protected:
	Singleton( void ) {}
	virtual ~Singleton( void ) {}
public:
	static T* GetInstance( void )
	{
		if ( !m_pInstance )
		{
			m_pInstance = (T*)_aligned_malloc( sizeof( T ), ALIGNMENT_SIZE );
			new (m_pInstance)T();
		}
		return m_pInstance;
	}

	static void ReleaseInstance( void )
	{
		m_pInstance->~T();
		_aligned_free( m_pInstance );
	}

private:
	static T*	m_pInstance;
};
template <typename T>
T* Singleton<T>::m_pInstance = nullptr;
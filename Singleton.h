#pragma once
#include "head.h"


template <typename T>
class CSingleton
{
	public:
		static T* GetSingleObjPtr();
	private:
		static T* m_pSingleObj;
	protected:
		CSingleton(){}
};

template <typename T>
T* CSingleton<T>:: m_pSingleObj=nullptr;

template <typename T>
T* CSingleton<T>::GetSingleObjPtr()
{
	if(!m_pSingleObj)
		m_pSingleObj=new T;

	return m_pSingleObj;
}
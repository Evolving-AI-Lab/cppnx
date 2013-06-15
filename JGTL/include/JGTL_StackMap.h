#ifndef STACKMAP_H_INCLUDED
#define STACKMAP_H_INCLUDED

#ifndef DEBUG_STACK_MAP
#define DEBUG_STACK_MAP (0)
#endif

#include "JGTL_MapInterface.h"

#include <utility>
#include <stdexcept>
#include <cstdlib>

#if DEBUG_STACK_MAP
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class StackMap
	* @brief The StackMap Class is a fixed, array-based, sorted key structure
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Key,class Data,int MAX_ELEMENTS=8>
	class StackMap : public MapInterface<Key,Data>
	{
	public:
		using MapInterface<Key,Data>::dataList;
		using MapInterface<Key,Data>::clear;

	protected:
		using MapInterface<Key,Data>::numElements;
		using MapInterface<Key,Data>::maxElements;

		unsigned char data[MAX_ELEMENTS*sizeof(std::pair<Key,Data>)];


	public:
		/*
		* Constructor
		*/
		StackMap()
			:
		MapInterface<Key,Data>()
		{
			maxElements = MAX_ELEMENTS;
			dataList = (std::pair<Key,Data>*)data;
		}

		/*
		* Copy Constructor
		*/
		StackMap(const StackMap &other)
		{
			copyFrom(other);
		}

		/*
		* Assignment Operator
		*/
		const StackMap &operator=(const StackMap &other)
		{
            clear();
			copyFrom(other);

			return *this;
		}

		/*
		* Destructor Operator
		*/
		virtual ~StackMap()
		{
			clear();
		}

		/*
		* Resizes the capacity of the map
		* @return True if the map was resized, false if the resize failed
		*/
		virtual bool resize(int newSize)
		{
			//Don't support resizing.
			return false;
		}

	protected:
		virtual void copyFrom(const StackMap &other)
		{
			numElements = other.numElements;
			maxElements = MAX_ELEMENTS;

            if(numElements>maxElements)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
            }

			dataList = (std::pair<Key,Data>*)data;

			for (int a=0;a<numElements;a++)
			{
				new(dataList+a) std::pair<Key,Data>(other.dataList[a]);
			}
		}

	};

}

#endif // STACKMAP_H_INCLUDED

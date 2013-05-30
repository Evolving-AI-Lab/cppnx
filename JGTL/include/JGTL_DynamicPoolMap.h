#ifndef DYNAMICPOOLMAP_H_INCLUDED
#define DYNAMICPOOLMAP_H_INCLUDED

#ifndef DEBUG_DYNAMIC_POOL_MAP
#define DEBUG_DYNAMIC_POOL_MAP (0)
#endif

#include "JGTL_MapInterface.h"

#include <utility>
#include <cstdlib>

#include "JGTL_LocatedException.h"

#if DEBUG_DYNAMIC_POOL_MAP
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class DynamicPoolMap
	* @brief The DynamicPoolMap Class is a resizable array-based 
	* associative map structure.
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Key,class Data>
	class DynamicPoolMap : public MapInterface<Key,Data>
	{
	public:
		using MapInterface<Key,Data>::dataList;
		using MapInterface<Key,Data>::clear;

	protected:
		using MapInterface<Key,Data>::numElements;
		using MapInterface<Key,Data>::maxElements;

	public:
		/*
		* Constructor
		*/
		DynamicPoolMap()
			:
		MapInterface<Key,Data>()
		{}

		/*
		* Constructor
		* @param _capacity Sets the initial capacity of the array
		*/
		DynamicPoolMap(int _capacity)
			:
		MapInterface<Key,Data>()
		{
			resize(_capacity);
		}

		/*
		* Copy Constructor
		*/
		DynamicPoolMap(const DynamicPoolMap &other)
		{
			copyFrom(other);
		}

		/*
		* Assignment Operator
		*/
		const DynamicPoolMap &operator=(const DynamicPoolMap &other)
		{
            clear();

			copyFrom(other);

			return *this;
		}

		/*
		* Destructor Operator
		*/
		virtual ~DynamicPoolMap()
		{
			clear();

			if (dataList)
			{
				free(dataList);
			}
		}

		/*
		* Resizes the capacity of the map
		* @return True if the map was resized, false if the resize failed
		*/
		virtual bool resize(int newSize)
		{
			if (newSize<=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: TRIED TO RESERVE LESS SPACE THAN WHAT IS CURRENTLY NEEDED!");
			}

			maxElements = newSize;
			dataList = (std::pair<Key,Data>*)realloc(dataList,sizeof(std::pair<Key,Data>)*maxElements);

			return true;
		}

	protected:
		virtual void copyFrom(const DynamicPoolMap &other)
		{
			numElements = other.numElements;
			maxElements = other.maxElements;

			if (maxElements)
			{
				dataList = (std::pair<Key,Data>*)realloc( 
						dataList,
						sizeof(std::pair<Key,Data>)*maxElements
					);
			}
			else
			{
				dataList = NULL;
			}

			for (int a=0;a<numElements;a++)
			{
				new(dataList+a) std::pair<Key,Data>(other.dataList[a]);
			}
		}

	};

}

#endif // DYNAMICPOOLMAP_H_INCLUDED

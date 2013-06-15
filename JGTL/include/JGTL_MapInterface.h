#ifndef MAPINTERFACE_H_INCLUDED
#define MAPINTERFACE_H_INCLUDED

#ifndef DEBUG_MAP_INTERFACE
#define DEBUG_MAP_INTERFACE (0)
#endif

#include <utility>
#include <cstdlib>
#include "JGTL_LocatedException.h"

#if DEBUG_MAP_INTERFACE
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class MapInterface
	* @brief This class acts as a base class for the Map construct
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Key,class Data>
	class MapInterface
	{
	public:
		typedef std::pair<Key,Data>* iterator;
		typedef const std::pair<Key,Data>* const_iterator;

	protected:
		int numElements,maxElements;

		//dataList pointer.  This is used by the derivatives of this class.
		std::pair<Key,Data> *dataList;

	public:
		/*
		* Constructor
		*/
		MapInterface()
			:
		numElements(0),
			maxElements(0),
			dataList(NULL)
		{
			/*NOTE:
			The derived class is responsible for allocating the memory
			*/
		}

		/*
		* Destructor
		*/
		virtual ~MapInterface()
		{
			/*NOTE:
			The derived class is responsible for freeing the memory
			*/
		}

		/*
		* Tests each element of two maps for equality
		*/
		bool operator==(const MapInterface &other) const
		{
			if (numElements != other.numElements)
			{
				return false;
			}

			for (int a=0;a<numElements;a++)
			{
				if (dataList[a]!=other.dataList[a])
				{
					return false;
				}
			}

			return true;
		}

		/*
		* Resizes the capacity of the map
		* @return True if the map was resized, false if the resize failed
		*/
		virtual bool resize(int newSize) = 0;

		/*
		* Inserts a new item into the map
		*/
		virtual void insert(const Key &key,const Data &data)
		{
			if (!numElements)
			{
				if (!maxElements)
				{
					if (!this->resize(16))
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add too many elements to a sorted list!");
					}
				}

				new(dataList) std::pair<Key,Data>(key,data);

				++numElements;

				return;
			}

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
				{
					//The key exists, replace the data
					dataList[i].second = data;
					return;
				}
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
			{
				//The key exists, replace the data
				dataList[high].second = data;
				return;
			}
			else
			{
				if (numElements==maxElements)
				{
					int newSize = maxElements;

					if (newSize==0)
						newSize = 16;
					else
						newSize *= 2;

					if (!this->resize(newSize))
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add too many elements to a sorted list!");
					}
				}

				if ( key < dataList[high].first )
				{
					//The key does not exist, insert before high

					memmove(
						dataList+high+1,
						dataList+high,
						sizeof(std::pair<Key,Data>)*(numElements-high)
						);

					new(dataList+high) std::pair<Key,Data>(key,data);

					++numElements;
					return;
				}
				else //if ( key > dataList[high].first )
				{
					//The key does not exist, insert after high
					high++;

					memmove(
						dataList+high+1,
						dataList+high,
						sizeof(std::pair<Key,Data>)*(numElements-high)
						);

					new(dataList+high) std::pair<Key,Data>(key,data);

					++numElements;
					return;
				}
			}

#if DEBUG_MAP_INTERFACE
			cout << "DEBUG:\n";
			for (int a=0;a<numElements;a++)
			{
				cout << dataList[a].first << ',' << dataList[a].second << endl;
			}
#endif
		}

		/*
		* Retrieves the number of elements in the map
		*/
		inline int size() const
		{
			return numElements;
		}

		/*
		* Returns whether or not the map is empty
		*/
		inline bool empty() const
		{
			return numElements==0;
		}

		/*
		* Deletes all items from the map
		*/
		void clear()
		{
			using namespace std; //std::~pair<Key,Data>() does not work

			for (int a=0;a<numElements;a++)
				(dataList+a)->~pair<Key,Data>();

			numElements=0;
		}

		/*
		* Returns an iterator to the first item in the map
		*/
		iterator begin()
		{
			return dataList;
		}

		/*
		* Returns a const iterator to the first item in the map
		*/
		const_iterator begin() const
		{
			return dataList;
		}

		/*
		* Returns an iterator to the location just 
		* past the end of the map
		*/
		iterator end()
		{
			return dataList+numElements;
		}

		/*
		* Returns a const iterator to the location just 
		* past the end of the map
		*/
		const_iterator end() const
		{
			return dataList+numElements;
		}

		/*
		* Returns whether or not the key exists in a map
		* @param key The key to check for
		*/
		bool hasKey(const Key &key) const
		{
			if (!numElements)
				return false;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
					return true;
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
				return( true );
			else
				return( false );
		}

		/*
		* Returns an iterator to an item by its key
		* @param key The key to look up
		*/
		iterator find(const Key &key)
		{
			if (!numElements)
				return end();

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if ( key < dataList[i].first )
					high = i;
				else if ( dataList[i].first < key )
					low  = i;
				else //they are equal
					return dataList+i;
			}
			if ( key == dataList[high].first )
				return( dataList + high ); //They are equal
			else
				return( end() );
		}

		/*
		* Returns a const iterator to an item by its key
		* @param key The key to look up
		*/
		const_iterator find(const Key &key) const
		{
			if (!numElements)
				return end();

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if ( key < dataList[i].first )
					high = i;
				else if ( dataList[i].first < key )
					low  = i;
				else //they are equal
					return dataList+i;
			}
			if ( key == dataList[high].first )
				return( dataList + high ); //They are equal
			else
				return( end() );
		}

		/*
		* Returns a pointer to an item by its key
		* @param key The key to look up
		*/
		Data *getData(const Key &key)
		{
			if (!numElements)
				return NULL;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
					return &(dataList[i].second);
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
				return( &(dataList[high].second) );
			else
			{
#if DEBUG_MAP_INTERFACE
				for (int a=0;a<numElements;a++)
				{
					if (dataList[a].first==key)
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Binary search failed!");
					}
				}
#endif
				return NULL;
			}
		}

		/*
		* Returns a const pointer to an item by its key
		* @param key The key to look up
		*/
		const Data *getData(const Key &key) const
		{
			if (!numElements)
				return NULL;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
					return &(dataList[i].second);
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
				return( &(dataList[high].second) );
			else
				return( NULL );
		}

		/*
		* Returns a reference to an item by its key
		* @param key The key to look up
		*/
		Data &getDataRef(const Key &key)
		{
			if (!numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
					return (dataList[i].second);
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
			{
				return( (dataList[high].second) );
			}
			else
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}
		}

		/*
		* Returns a const reference to an item by its key
		* @param key The key to look up
		*/
		const Data &getDataRef(const Key &key) const
		{
			if (!numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
					return (dataList[i].second);
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
			{
				return( (dataList[high].second) );
			}
			else
			{
                std::cerr << "could not find dataList[high].first: " << dataList[high].first << std::endl;
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}
		}

		/*
		* Erases an item from the map
		* @param iter An iterator to an item in the map
		*/
		void erase(const_iterator iter)
		{
			if(iter==end())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to erase an invalid iterator!");
			}

			using namespace std;

			int index = (iter-dataList);
			dataList[index].~pair<Key,Data>();

			memmove(
				dataList+index,
				dataList+index+1,
				sizeof(std::pair<Key,Data>)*(numElements-(index+1))
				);

			numElements--;
		}

		/*
		* Erases an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		void eraseIndex(int index)
		{
			erase(begin()+index);
		}

		/*
		* Erases an item from the map given it's index
		* @param iter An index in the map
		*/
		inline const Data &getIndexData(int index) const
		{
			return dataList[index].second;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline Data *getIndexDataPtr(int index)
		{
			return &dataList[index].second;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline const Data *getIndexDataPtr(int index) const
		{
			return &dataList[index].second;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline const std::pair<Key,Data> &getIndex(int index) const
		{
			return dataList[index];
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline iterator getIndexPtr(int index)
		{
			return &dataList[index];
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline const_iterator getIndexPtr(int index) const
		{
			return &dataList[index];
		}

	protected:

	};

}

#endif // MAPINTERFACE_H_INCLUDED


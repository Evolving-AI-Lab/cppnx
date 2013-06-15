#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

#include <iostream>
#include <string>
using namespace std;

#include "JGTL_Vector2.h"

#include "boost/pool/pool.hpp"

namespace JGTL
{

	template<class T>
	class QuadTreeNode
	{
	protected:
	public:
		QuadTreeNode()
		{}

		virtual ~QuadTreeNode<T>()
		{
			cout << string("You must use the custom memory manager for QuadTrees!");
			string line;
			getline(cin,line);
		}

		virtual T getValue(Vector2<int> topLeftVector2,int size,const Vector2<int> &location) const = 0;

		virtual T getValue() const = 0;

		virtual bool setValue(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			Vector2<int> topLeftVector2,
			int size,
			const Vector2<int> &location,
			const T &value
			) = 0;

		virtual bool isStub() const
		{
			return false;
		}

		virtual void destroy(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool
			)
		{}

		virtual void display(int level) const = 0;
	};

	template<class T>
	class QuadTreeStub : public QuadTreeNode<T>
	{
	protected:
		T value;

	public:
		QuadTreeStub(T _value)
			:
		QuadTreeNode<T>(),
			value(_value)
		{}

		virtual ~QuadTreeStub()
		{
			cout << string("You must use the custom memory manager for QuadTrees!");
			string line;
			getline(cin,line);
		}

		virtual T getValue(Vector2<int> topLeftVector2,int size,const Vector2<int> &location) const
		{
			return value;
		}

		virtual T getValue() const
		{
			return value;
		}

		virtual bool setValue(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			Vector2<int> topLeftVector2,
			int size,
			const Vector2<int> &location,
			const T &value
			)
		{
			this->value = value;

			return false;
		}

		virtual bool setValue(const T &_value)
		{
			value = _value;

			return false;
		}

		virtual bool isStub() const
		{
			return true;
		}


		virtual void display(int level) const
		{
			for (int a=0;a<level;a++)
				printf(">");

			if (value)
				printf("+\n");
			else
				printf(" \n");
		}
	};

	template<class T>
	class QuadTreeBranch : public QuadTreeNode<T>
	{
		QuadTreeNode<T> *topLeft,*topRight,*bottomLeft,*bottomRight;

	public:
		/*QuadTreeBranch(QuadTreeStub<T> *stub)
		:
		QuadTreeNode<T>()
		{
		if (!stub)
		{
		topLeft = topRight = bottomLeft = bottomRight = NULL;
		}
		else
		{
		topLeft = new QuadTreeStub<T>(*stub);
		topRight = new QuadTreeStub<T>(*stub);
		bottomLeft = new QuadTreeStub<T>(*stub);
		bottomRight = new QuadTreeStub<T>(*stub);
		}

		}*/

		QuadTreeBranch(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			const T &value
			)
			:
		QuadTreeNode<T>()
		{
			topLeft = new(stubPool.malloc()) QuadTreeStub<T>(value);
			topRight = new(stubPool.malloc()) QuadTreeStub<T>(value);
			bottomLeft = new(stubPool.malloc()) QuadTreeStub<T>(value);
			bottomRight = new(stubPool.malloc()) QuadTreeStub<T>(value);
		}

		/*
		QuadTreeBranch()
		:
		QuadTreeNode<T>()
		{
		topLeft = topRight = bottomLeft = bottomRight = NULL;
		}
		*/

		virtual ~QuadTreeBranch()
		{
			cout << string("You must use the custom memory manager for QuadTrees!");
			string line;
			getline(cin,line);
		}

		virtual void destroy(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool
			)
		{
			if (topLeft)
			{
				topLeft->destroy(branchPool,stubPool);
				if (topLeft->isStub())
				{
					//((QuadTreeStub<T>*)topLeft)->~QuadTreeStub<T>();
					stubPool.free(topLeft);
				}
				else
				{
					//((QuadTreeBranch<T>*)topLeft)->~QuadTreeBranch<T>();
					branchPool.free(topLeft);
				}

				topRight->destroy(branchPool,stubPool);
				if (topRight->isStub())
				{
					//((QuadTreeStub<T>*)topRight)->~QuadTreeStub<T>();
					stubPool.free(topRight);
				}
				else
				{
					//((QuadTreeBranch<T>*)topRight)->~QuadTreeBranch<T>();
					branchPool.free(topRight);
				}

				bottomRight->destroy(branchPool,stubPool);
				if (bottomRight->isStub())
				{
					//((QuadTreeStub<T>*)bottomRight)->~QuadTreeStub<T>();
					stubPool.free(bottomRight);
				}
				else
				{
					//((QuadTreeBranch<T>*)bottomRight)->~QuadTreeBranch<T>();
					branchPool.free(bottomRight);
				}

				bottomLeft->destroy(branchPool,stubPool);
				if (bottomLeft->isStub())
				{
					//((QuadTreeStub<T>*)bottomLeft)->~QuadTreeStub<T>();
					stubPool.free(bottomLeft);
				}
				else
				{
					//((QuadTreeBranch<T>*)bottomLeft)->~QuadTreeBranch<T>();
					branchPool.free(bottomLeft);
				}
			}
		}

		virtual T getValue(Vector2<int> topLeftVector2,int size,const Vector2<int> &location) const
		{
			Vector2<int> center = topLeftVector2;

			size/=2;

			center.x += size;
			center.y += size;

			/*if(location.x<center.x)
			{
			if(location.y<center.y)
			{
			return topLeft->getValue(topLeftVector2,size,location);
			}
			else
			{
			return bottomLeft->getValue(topLeftVector2,size,location);
			}
			}
			else
			{
			if(location.y<center.y)
			{
			return topRight->getValue(topLeftVector2,size,location);
			}
			else
			{
			return bottomRight->getValue(topLeftVector2,size,location);
			}
			}*/

			QuadTreeNode<T> *node;
			Vector2<int> tmpTopLeftVector2;

			if (location.x<center.x)
			{
				if (location.y<center.y)
				{
					node = topLeft;
					tmpTopLeftVector2 = topLeftVector2;
				}
				else
				{
					node = bottomLeft;
					tmpTopLeftVector2 = Vector2<int>(topLeftVector2.x,center.y);
				}
			}
			else
			{
				if (location.y<center.y)
				{
					node = topRight;
					tmpTopLeftVector2 = Vector2<int>(center.x,topLeftVector2.y);
				}
				else
				{
					node = bottomRight;
					tmpTopLeftVector2 = Vector2<int>(center.x,center.y);
				}
			}

			return node->getValue(tmpTopLeftVector2,size,location);
		}

		virtual T getValue() const
		{
			return topLeft->getValue();
		}

		void setAll(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			T &value
			)
		{
			destroy(branchPool,stubPool);

			topLeft = new(stubPool.malloc()) QuadTreeStub<T>(value);
			topRight = new(stubPool.malloc()) QuadTreeStub<T>(value);
			bottomLeft = new(stubPool.malloc()) QuadTreeStub<T>(value);
			bottomRight = new(stubPool.malloc()) QuadTreeStub<T>(value);
		}

		//Returns 'true' if we need to collapse this branch
		virtual bool setValue(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			Vector2<int> topLeftVector2,
			int size,
			const Vector2<int> &location,
			const T &value
			)
		{
			Vector2<int> center = topLeftVector2;

			size /=2;

			center.x+=size;
			center.y+=size;

			QuadTreeNode<T> *node;
			Vector2<int> tmpTopLeftVector2;

			if (location.x<center.x)
			{
				if (location.y<center.y)
				{
					node = topLeft;
					tmpTopLeftVector2 = topLeftVector2;
				}
				else
				{
					node = bottomLeft;
					tmpTopLeftVector2 = Vector2<int>(topLeftVector2.x,center.y);
				}
			}
			else
			{
				if (location.y<center.y)
				{
					node = topRight;
					tmpTopLeftVector2 = Vector2<int>(center.x,topLeftVector2.y);
				}
				else
				{
					node = bottomRight;
					tmpTopLeftVector2 = Vector2<int>(center.x,center.y);
				}
			}

			if (node->isStub())
			{
				QuadTreeStub<T> *stubNode = static_cast<QuadTreeStub<T> *>(node);

				if (size==1) //replace the stub
				{
					stubNode->setValue(value);
				}
				else if (node->getValue()!=value) //This stub needs to be expanded to a branch
				{
					QuadTreeBranch<T> *tmpBranch =
						new(branchPool.malloc()) QuadTreeBranch<T>(branchPool,stubPool,stubNode->getValue());

					if (node==topLeft)
						topLeft = tmpBranch;
					if (node==topRight)
						topRight = tmpBranch;
					if (node==bottomLeft)
						bottomLeft = tmpBranch;
					if (node==bottomRight)
						bottomRight = tmpBranch;

					node->destroy(branchPool,stubPool);
					stubPool.free(node);

					tmpBranch->setValue(branchPool,stubPool,tmpTopLeftVector2,size,location,value);
				}
			}
			else
			{
				//node is a branch

				if (node->setValue(branchPool,stubPool,tmpTopLeftVector2,size,location,value))
				{
					//We need to collapse (convert a branch to a stub)

					//First, get a value from the branch (all values should be the same)
					T value = node->getValue();

					//Then, create a stub in the appropriate location
					if (node==topLeft)
						topLeft = new(stubPool.malloc()) QuadTreeStub<T>(value);
					else if (node==topRight)
						topRight = new(stubPool.malloc()) QuadTreeStub<T>(value);
					else if (node==bottomLeft)
						bottomLeft = new(stubPool.malloc()) QuadTreeStub<T>(value);
					else if (node==bottomRight)
						bottomRight = new(stubPool.malloc()) QuadTreeStub<T>(value);

					//Then delete the original branch
					node->destroy(branchPool,stubPool);
					branchPool.free(node);
				}
			}

			//check if we need to collapse

			if (
				topRight->isStub() &&
				bottomRight->isStub() &&
				bottomLeft->isStub() &&
				topLeft->isStub()
				)
			{
				//All branches are stubs, maybe a chance for a collapse?

				if (
					((QuadTreeStub<T> *)topLeft)->getValue() == ((QuadTreeStub<T> *)topRight)->getValue() &&
					((QuadTreeStub<T> *)topRight)->getValue() == ((QuadTreeStub<T> *)bottomRight)->getValue() &&
					((QuadTreeStub<T> *)bottomRight)->getValue() == ((QuadTreeStub<T> *)bottomLeft)->getValue()
					)
				{
					//all branches are equal, we need to collapse

					return true;
				}
			}

			return false;
		}

		virtual void display(int level) const
		{
			level++;
			topLeft->display(level);
			topRight->display(level);
			bottomLeft->display(level);
			bottomRight->display(level);
		}
	};

	template<class T>
	class QuadTree
	{
		int size;
		QuadTreeBranch<T> *root;

		boost::pool<> branchPool;
		boost::pool<> stubPool;

	public:
		QuadTree(int _size=16,T defaultValue=(T)0)
			:
		size(_size),
			branchPool(sizeof(QuadTreeBranch<T>)),
			stubPool(sizeof(QuadTreeStub<T>))
		{
			root = new(branchPool.malloc()) QuadTreeBranch<T>(branchPool,stubPool,defaultValue);
		}

		QuadTree(const QuadTree<T> &other)
			:
		branchPool(sizeof(QuadTreeBranch<T>)),
			stubPool(sizeof(QuadTreeStub<T>))
		{
			copyFrom(other);
		}

		~QuadTree()
		{
			root->destroy(branchPool,stubPool);
			//((QuadTreeBranch<T>*)root)->~QuadTreeBranch<T>();
			branchPool.free(root);
		}

		QuadTree<T>& operator=(const QuadTree<T> &other)
		{
			if (this==&other)
			{
				return *this;
			}

			copyFrom(other);

			return *this;
		}

		void copyFrom(const QuadTree<T> &other)
		{
			size = other.size;

			root = new(branchPool.malloc()) QuadTreeBranch<T>(branchPool,stubPool,(T)0);

			for (int y=0;y<size;y++)
			{
				for (int x=0;x<size;x++)
				{
					setValue(x,y,other.getValue(x,y));
				}
			}

		}

		inline T getValue(const Vector2<int> &location) const
		{
			Vector2<int> tmpVec(0,0);
			return root->getValue(tmpVec,size,location);
		}

		inline T getValue(int x,int y) const
		{
			Vector2<int> tmpVec(x,y);
			return getValue(tmpVec);
		}

		inline void setValue(const Vector2<int> &location,T value)
		{
			root->setValue(branchPool,stubPool,Vector2<int>(0,0),size,location,value);
		}

		inline void setValue(int x,int y,T value)
		{
			Vector2<int> tmpVec(x,y);
			setValue(tmpVec,value);
		}

		inline void setAll(T value)
		{
			root->setAll(branchPool,stubPool,value);
		}

		inline void display() const
		{
			root->display(0);
		}

		inline T operator()(const Vector2<int> &location) const
		{
			return getValue(location);
		}

		inline T operator()(int x,int y) const
		{
			Vector2<int> tmpVec(x,y);
			return getValue(tmpVec);
		}

	protected:
	};

}

#endif // QUADTREE_H_INCLUDED

#ifndef VARIANT_H_INCLUDED
#define VARIANT_H_INCLUDED

#include <iostream>
#include "JGTL_LocatedException.h"

namespace JGTL
{

	template <class Type,bool condition, Type Then, Type Else>
	struct TYPEIF
	{
		static const Type RESULT=Then;
	};

	template <class Type,Type Then, Type Else>
	struct TYPEIF<Type,false, Then, Else>
	{
		static const Type RESULT=Else;
	};

	template <
		class One,
		class Two = One,
		class Three = One,
		class Four = One,
		class Five = One,
		class Six = One,
		class Seven = One,
		class Eight = One,
		class Nine = One,
		class Ten = One
	>
	struct STATIC_MAX_SIZE
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(Two) > sizeof(Three)),
			STATIC_MAX_SIZE<One,One,Two,Four,Five,Six,Seven,Eight,Nine,Ten>::RESULT,
			STATIC_MAX_SIZE<One,One,Three,Four,Five,Six,Seven,Eight,Nine,Ten>::RESULT
			>::RESULT;
	};
	template <
		class One,
		class Two,
		class Three,
		class Four,
		class Five,
		class Six,
		class Seven,
		class Eight,
		class Nine
	>
	struct STATIC_MAX_SIZE<One,One,Two,Three,Four,Five,Six,Seven,Eight,Nine>
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(Two) > sizeof(Three)),
			STATIC_MAX_SIZE<One,One,One,Two,Four,Five,Six,Seven,Eight,Nine>::RESULT,
			STATIC_MAX_SIZE<One,One,One,Three,Four,Five,Six,Seven,Eight,Nine>::RESULT
			>::RESULT;
	};
	template <
		class One,
		class Two,
		class Three,
		class Four,
		class Five,
		class Six,
		class Seven,
		class Eight
	>
	struct STATIC_MAX_SIZE<One,One,One,Two,Three,Four,Five,Six,Seven,Eight>
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(Two) > sizeof(Three)),
			STATIC_MAX_SIZE<One,One,One,One,Two,Four,Five,Six,Seven,Eight>::RESULT,
			STATIC_MAX_SIZE<One,One,One,One,Three,Four,Five,Six,Seven,Eight>::RESULT
			>::RESULT;
	};
	template <
		class One,
		class Two,
		class Three,
		class Four,
		class Five,
		class Six,
		class Seven
	>
	struct STATIC_MAX_SIZE<One,One,One,One,Two,Three,Four,Five,Six,Seven>
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(Two) > sizeof(Three)),
			STATIC_MAX_SIZE<One,One,One,One,One,Two,Four,Five,Six,Seven>::RESULT,
			STATIC_MAX_SIZE<One,One,One,One,One,Three,Four,Five,Six,Seven>::RESULT
			>::RESULT;
	};
	template <
		class One,
		class Two,
		class Three,
		class Four,
		class Five,
		class Six
	>
	struct STATIC_MAX_SIZE<One,One,One,One,One,Two,Three,Four,Five,Six>
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(Two) > sizeof(Three)),
			STATIC_MAX_SIZE<One,One,One,One,One,One,Two,Four,Five,Six>::RESULT,
			STATIC_MAX_SIZE<One,One,One,One,One,One,Three,Four,Five,Six>::RESULT
			>::RESULT;
	};
	template <class One, class Two, class Three, class Four, class Five>
	struct STATIC_MAX_SIZE<One,One,One,One,One,One,Two,Three,Four,Five>
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(Two) > sizeof(Three)),
			STATIC_MAX_SIZE<One,One,One,One,One,One,One,Two,Four,Five>::RESULT,
			STATIC_MAX_SIZE<One,One,One,One,One,One,One,Three,Four,Five>::RESULT
			>::RESULT;
	};
	template <class One, class Two, class Three, class Four>
	struct STATIC_MAX_SIZE<One,One,One,One,One,One,One,Two,Three,Four>
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(Two) > sizeof(Three)),
			STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,Two,Four>::RESULT,
			STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,Three,Four>::RESULT
			>::RESULT;
	};
	template <class One, class Two, class Three>
	struct STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,Two,Three>
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(Two) > sizeof(Three)),
			STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,One,Two>::RESULT,
			STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,One,Three>::RESULT
			>::RESULT;
	};
	template <class One, class Two>
	struct STATIC_MAX_SIZE<One,One,One,One,One,One,One,One,One,Two>
	{
		const static int RESULT =
			TYPEIF<
			int,
			(sizeof(One) > sizeof(Two)),
			sizeof(One),
			sizeof(Two)
			>::RESULT;
	};
	class NullVariantClass
	{};
	template<
		class Class1,
		class Class2=NullVariantClass,
		class Class3=NullVariantClass,
		class Class4=NullVariantClass,
		class Class5=NullVariantClass,
		class Class6=NullVariantClass,
		class Class7=NullVariantClass,
		class Class8=NullVariantClass,
		class Class9=NullVariantClass,
		class Class10=NullVariantClass
	>
	class Variant
	{
	protected:
		char data[
			STATIC_MAX_SIZE<
				Class1,
				Class2,
				Class3,
				Class4,
				Class5,
				Class6,
				Class7,
				Class8,
				Class9,
				Class10
			>::RESULT
		];
		unsigned char typeOfData;
	public:
		Variant()
			:
		typeOfData(0)
		{}

		Variant(const Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10> &other)
			:
		typeOfData(0)
		{
			switch(typeOfData)
			{
			case 0:
				break;
			case 1:
				setValue(other.getValue<Class1>());
				break;
			case 2:
				setValue(other.getValue<Class2>());
				break;
			case 3:
				setValue(other.getValue<Class3>());
				break;
			default:
				break;
			}
		}

		template<class T>
		bool isOfType()
		{
			switch(typeOfData)
			{
			case 0:
				return false;
			case 1:
				if(typeid(Class1) != typeid(T))
					return false;
				break;
			case 2:
				if(typeid(Class2) != typeid(T))
					return false;
				break;
			case 3:
				if(typeid(Class3) != typeid(T))
					return false;
				break;
			default:
				return false;
				break;
			}

			return true;

		}

		template<class ReturnValueClass>
		ReturnValueClass getValue() const
		{
			const ReturnValueClass* ptr = getValuePtr<ReturnValueClass>();

			if(ptr==NULL)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to get the value of a variant which was assigned to a different type");
			}

			return *ptr;
		}

		template<class ReturnValueClass>
		ReturnValueClass &getValueRef()
		{
			ReturnValueClass* ptr = getValuePtr<ReturnValueClass>();

			if(ptr==NULL)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to get the reference of a variant which was assigned to a different type");
			}

			return *ptr;
		}

		template<class ReturnValueClass>
		const ReturnValueClass &getValueRef() const
		{
			const ReturnValueClass* ptr = getValuePtr<ReturnValueClass>();

			if(ptr==NULL)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to get the reference of a variant which was assigned to a different type");
			}

			return *ptr;
		}

		template<class ReturnValueClass>
		ReturnValueClass* getValuePtr()
		{
			if(!typeOfData)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}

			switch(typeOfData)
			{
			case 1:
				if(typeid(Class1) != typeid(ReturnValueClass))
					return NULL;
				break;
			case 2:
				if(typeid(Class2) != typeid(ReturnValueClass))
					return NULL;
				break;
			case 3:
				if(typeid(Class3) != typeid(ReturnValueClass))
					return NULL;
				break;
			default:
				return NULL;
				break;
			}

			return ((ReturnValueClass*)data);
		}

		template<class ReturnValueClass>
		const ReturnValueClass* getValuePtr() const
		{
			if(!typeOfData)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}

			switch(typeOfData)
			{
			case 1:
				if(typeid(Class1) != typeid(ReturnValueClass))
					return NULL;
				break;
			case 2:
				if(typeid(Class2) != typeid(ReturnValueClass))
					return NULL;
				break;
			case 3:
				if(typeid(Class3) != typeid(ReturnValueClass))
					return NULL;
				break;
			default:
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				return ((const ReturnValueClass*)data);
				break;
			}

			return ((const ReturnValueClass*)data);
		}

		void clearValue()
		{
			if (typeOfData)
			{
				switch(typeOfData)
				{
				case 1:
					((Class1*)data)->~Class1();
					break;
				case 2:
					((Class2*)data)->~Class2();
					break;
				case 3:
					((Class3*)data)->~Class3();
					break;
				default:
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
					break;
				}
				typeOfData=0;
			}

			memset(data,0,STATIC_MAX_SIZE<
				Class1,
				Class2,
				Class3,
				Class4,
				Class5,
				Class6,
				Class7,
				Class8,
				Class9,
				Class10
			>::RESULT );
		}

		template<class ValueToSet>
		void setValue(const ValueToSet &newValue)
		{
			clearValue();
			new(data) ValueToSet(newValue);
			if(typeid(ValueToSet).name() == typeid(Class1).name())
			{
				typeOfData = 1;
			}
			else if(typeid(ValueToSet).name() == typeid(Class2).name())
			{
				typeOfData = 2;
			}
			else if(typeid(ValueToSet).name() == typeid(Class3).name())
			{
				typeOfData = 3;
			}
			else
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Oops, tried to set the value of a variant with a type that is not supported");
			}
		}
	};

}

#endif // VARIANT_H_INCLUDED

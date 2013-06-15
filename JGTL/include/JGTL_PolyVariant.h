#ifndef POLY_VARIANT_H_INCLUDED
#define POLY_VARIANT_H_INCLUDED

#include <iostream>
#include "JGTL_LocatedException.h"

#include "JGTL_Variant.h"

namespace JGTL
{

	template<
		class BaseClass,
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
	class PolyVariant : public Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>
	{
	protected:
		BaseClass* base;

		using Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>::data;
		using Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>::typeOfData;

	public:
		using Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>::clearValue;

		PolyVariant()
			:
		Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>()
		{
		}

		PolyVariant(const PolyVariant<BaseClass,Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10> &other)
			:
				Variant<Class1,Class2,Class3,Class4,Class5,Class6,Class7,Class8,Class9,Class10>(other),
				base( (BaseClass*)data )
		{
		}

		BaseClass* operator-> () const
		{
			return base;
		}

		template<class ValueToSet>
		void setValue(const ValueToSet &newValue)
		{
			clearValue();
			new(data) ValueToSet(newValue);
			base = (BaseClass*)((ValueToSet*)data);
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

#endif // POLY_VARIANT_H_INCLUDED

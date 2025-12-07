#include "Features/SMLFeatureTestsFunctionThunks.h"

#include "Reflection/FunctionThunkGenerator.h"

UE_DISABLE_OPTIMIZATION_SHIP

namespace
{

/// Creates a thunk that can forward to member functions.
template<auto FuncPtr>
struct GenerateThunkForwarder
{
	consteval operator std::remove_pointer_t<FNativeFuncPtr>&() const
	{
		return *static_cast<FNativeFuncPtr>(
			&TFunctionThunkGenerator<decltype(FuncPtr)>::template Thunk<&Forwarder<decltype(FuncPtr)>::Impl>);
	}

private:
	template<typename FuncPtrType>
	struct Forwarder;

	// Static function.
	template<typename Ret, typename... ArgTypes>
	struct Forwarder<Ret(*)(ArgTypes...)>
	{
		static Ret Impl(ArgTypes... Args)
		{
			return FuncPtr(Args...);
		}
	};

	// Non-const member function.
	template<typename Ret, typename Cls, typename... ArgTypes>
	struct Forwarder<Ret(Cls::*)(ArgTypes...)>
	{
		static Ret Impl(Cls* Obj, ArgTypes... Args)
		{
			return (Obj->*FuncPtr)(Args...);
		}
	};

	// Const member function.
	template<typename Ret, typename Cls, typename... ArgTypes>
	struct Forwarder<Ret(Cls::*)(ArgTypes...) const>
	{
		static Ret Impl(const Cls* Obj, ArgTypes... Args)
		{
			return (Obj->*FuncPtr)(Args...);
		}
	};
};

} // namespace

#define DEFINE_TEST_THUNK(Name) \
	constexpr std::remove_pointer_t<FNativeFuncPtr>& USMLFeatureTestsFunctionThunks::exec##Name = \
		GenerateThunkForwarder<&USMLFeatureTestsFunctionThunks::Name>()
DEFINE_TEST_THUNK(NoParameters);
DEFINE_TEST_THUNK(ReturnConstant);
DEFINE_TEST_THUNK(ReturnArray);
DEFINE_TEST_THUNK(AddToInParam);
DEFINE_TEST_THUNK(ParamValues);
DEFINE_TEST_THUNK(ParamRefs);
#undef DEFINE_TEST_THUNK

void USMLFeatureTestsFunctionThunks::RunTest()
{
	TestNoParameters();
	TestReturnConstant();
	TestReturnArray();
	TestAddToInParam();
	TestParamValues();
	TestParamRefs();
}

void USMLFeatureTestsFunctionThunks::TestNoParameters()
{
	UFunction* NoParametersFunction = FindFunctionChecked(TEXT("NoParameters"));
	check(NoParametersCallCount == 0);
	ProcessEvent(NoParametersFunction, nullptr);
	check(NoParametersCallCount == 1);
	ProcessEvent(NoParametersFunction, nullptr);
	check(NoParametersCallCount == 2);
	ProcessEvent(NoParametersFunction, nullptr);
	check(NoParametersCallCount == 3);
	NoParametersCallCount = 0;
}

void USMLFeatureTestsFunctionThunks::TestReturnConstant()
{
	auto InvokeReturnConstant = [this, Function = FindFunctionChecked(TEXT("ReturnConstant"))]
	{
		int Result;
		ProcessEvent(Function, &Result);
		return Result;
	};
	ConstantToReturn = 123;
	check(InvokeReturnConstant() == 123);
	ConstantToReturn = 456;
	check(InvokeReturnConstant() == 456);
}

void USMLFeatureTestsFunctionThunks::TestReturnArray()
{
	auto InvokeReturnArray = [this, Function = FindFunctionChecked(TEXT("ReturnArray"))](int Value1, int Value2)
	{
		struct { int Value1; int Value2; TArray<int> Result; } Params{Value1, Value2};
		ProcessEvent(Function, &Params);
		return Params.Result;
	};
	check(InvokeReturnArray(1, 2) == (TArray<int>{1, 2}));
	check(InvokeReturnArray(3, 4) == (TArray<int>{3, 4}));
}

void USMLFeatureTestsFunctionThunks::TestAddToInParam()
{
	auto InvokeAddToInParam = [this, Function = FindFunctionChecked(TEXT("AddToInParam"))](int& Param, int AmountToAdd)
	{
		struct { int Param; int AmountToAdd; } Params{Param, AmountToAdd};
		ProcessEvent(Function, &Params);
		Param = Params.Param;
	};
	int Value = 5;
	InvokeAddToInParam(Value, 10);
	check(Value == 15);
	InvokeAddToInParam(Value, 100);
	check(Value == 115);
}

void USMLFeatureTestsFunctionThunks::TestParamValues()
{
	bool Called = false;

	struct
	{
		int8 Int8 = -0x12;
		int16 Int16 = -0x1234;
		int32 Int32 = -0x12345678;
		int64 Int64 = -0x123456789abcdef0;
		uint8 Uint8 = 0x12;
		uint16 Uint16 = 0x1234;
		uint32 Uint32 = 0x12345678;
		uint64 Uint64 = 0x123456789abcdef0;
		bool Bool1 = false;
		bool Bool2 = true;
		TArray<int> Array = { 1, 2, 3, 4 };
		TMap<int, int> Map = { {1, 2}, {3, 4} };
		TSet<int> Set = { 1, 2, 3, 4 };
		USMLFeatureTestsFunctionThunks* ObjectPtr = nullptr;
		TSubclassOf<USMLFeatureTestsFunctionThunks> ClassPtr = StaticClass();
		TScriptInterface<ISLMFeatureTestsFunctionThunkInterface> Interface = nullptr;
		FSMLFeatureTestsFunctionThunkStructParameter Struct = { .Value1 = 1, .Value2 = 2 };
		ESMLFeatureTestsFunctionThunkEnumParameter Enum = ESMLFeatureTestsFunctionThunkEnumParameter::Value3;
	} Params;
	Params.ObjectPtr = this;
	Params.Interface = this;

	// Compares values.
	ParamValuesCallback = [&](
		int8 Int8,
		int16 Int16,
		int32 Int32,
		int64 Int64,
		uint8 Uint8,
		uint16 Uint16,
		uint32 Uint32,
		uint64 Uint64,
		bool Bool1,
		bool Bool2,
		TArray<int> Array,
		TMap<int, int> Map,
		TSet<int> Set,
		USMLFeatureTestsFunctionThunks* ObjectPtr,
		TSubclassOf<USMLFeatureTestsFunctionThunks> ClassPtr,
		TScriptInterface<ISLMFeatureTestsFunctionThunkInterface> Interface,
		FSMLFeatureTestsFunctionThunkStructParameter Struct,
		ESMLFeatureTestsFunctionThunkEnumParameter Enum)
	{
		Called = true;
		check(Int8 == Params.Int8);
		check(Int16 == Params.Int16);
		check(Int32 == Params.Int32);
		check(Int64 == Params.Int64);
		check(Uint8 == Params.Uint8);
		check(Uint16 == Params.Uint16);
		check(Uint32 == Params.Uint32);
		check(Uint64 == Params.Uint64);
		check(Bool1 == Params.Bool1);
		check(Bool2 == Params.Bool2);
		check(Array == Params.Array);
		check(Map.OrderIndependentCompareEqual(Params.Map));
		check(Set.Num() == Params.Set.Num() && Set.Includes(Params.Set));
		check(ObjectPtr == this);
		check(ClassPtr == StaticClass());
		check(Interface == this);
		check(Struct == Params.Struct);
		check(Enum == Params.Enum);
	};

	ProcessEvent(FindFunctionChecked(TEXT("ParamValues")), &Params);
	check(Called);
	ParamValuesCallback.Reset();
}

void USMLFeatureTestsFunctionThunks::TestParamRefs()
{
	bool Called = false;

	struct
	{
		int8 Int8;
		int16 Int16;
		int32 Int32;
		int64 Int64;
		uint8 Uint8;
		uint16 Uint16;
		uint32 Uint32;
		uint64 Uint64;
		bool Bool;
		TArray<int> Array;
		TMap<int, int> Map;
		TSet<int> Set;
		FSMLFeatureTestsFunctionThunkStructParameter Struct;
		ESMLFeatureTestsFunctionThunkEnumParameter Enum;
	} Params = {};

	// Compares addresses.
	ParamRefsCallback = [&](
		const int8& Int8,
		const int16& Int16,
		const int32& Int32,
		const int64& Int64,
		const uint8& Uint8,
		const uint16& Uint16,
		const uint32& Uint32,
		const uint64& Uint64,
		const bool& Bool,
		const TArray<int>& Array,
		const TMap<int, int>& Map,
		const TSet<int>& Set,
		const FSMLFeatureTestsFunctionThunkStructParameter& Struct,
		const ESMLFeatureTestsFunctionThunkEnumParameter& Enum)
	{
		Called = true;
		check(&Int8 == &Params.Int8);
		check(&Int16 == &Params.Int16);
		check(&Int32 == &Params.Int32);
		check(&Int64 == &Params.Int64);
		check(&Uint8 == &Params.Uint8);
		check(&Uint16 == &Params.Uint16);
		check(&Uint32 == &Params.Uint32);
		check(&Uint64 == &Params.Uint64);
		check(&Bool == &Params.Bool);
		check(&Array == &Params.Array);
		check(&Map == &Params.Map);
		check(&Set == &Params.Set);
		check(&Struct == &Params.Struct);
		check(&Enum == &Params.Enum);
	};

	ProcessEvent(FindFunctionChecked(TEXT("ParamRefs")), &Params);
	check(Called);
	ParamRefsCallback.Reset();
}

void USMLFeatureTestsFunctionThunks::NoParameters()
{
	++NoParametersCallCount;
}

int USMLFeatureTestsFunctionThunks::ReturnConstant() const
{
	return ConstantToReturn;
}

TArray<int> USMLFeatureTestsFunctionThunks::ReturnArray(int Value1, int Value2)
{
	return { Value1, Value2 };
}

void USMLFeatureTestsFunctionThunks::AddToInParam(int& Param, int AmountToAdd)
{
	Param += AmountToAdd;
}

void USMLFeatureTestsFunctionThunks::ParamValues(
	int8 Int8,
	int16 Int16,
	int32 Int32,
	int64 Int64,
	uint8 Uint8,
	uint16 Uint16,
	uint32 Uint32,
	uint64 Uint64,
	bool Bool1,
	bool Bool2,
	TArray<int> Array,
	TMap<int, int> Map,
	TSet<int> Set,
	USMLFeatureTestsFunctionThunks* ObjectPtr,
	TSubclassOf<USMLFeatureTestsFunctionThunks> ClassPtr,
	TScriptInterface<ISLMFeatureTestsFunctionThunkInterface> Interface,
	FSMLFeatureTestsFunctionThunkStructParameter Struct,
	ESMLFeatureTestsFunctionThunkEnumParameter Enum)
{
	if (ParamValuesCallback == nullptr)
		return;

	ParamValuesCallback(
		Int8,
		Int16,
		Int32,
		Int64,
		Uint8,
		Uint16,
		Uint32,
		Uint64,
		Bool1,
		Bool2,
		Array,
		Map,
		Set,
		ObjectPtr,
		ClassPtr,
		Interface,
		Struct,
		Enum);
}

void USMLFeatureTestsFunctionThunks::ParamRefs(
	const int8& Int8,
	const int16& Int16,
	const int32& Int32,
	const int64& Int64,
	const uint8& Uint8,
	const uint16& Uint16,
	const uint32& Uint32,
	const uint64& Uint64,
	const bool& Bool,
	const TArray<int>& Array,
	const TMap<int, int>& Map,
	const TSet<int>& Set,
	const FSMLFeatureTestsFunctionThunkStructParameter& Struct,
	const ESMLFeatureTestsFunctionThunkEnumParameter& Enum)
{
	if (ParamRefsCallback == nullptr)
		return;

	ParamRefsCallback(
		Int8,
		Int16,
		Int32,
		Int64,
		Uint8,
		Uint16,
		Uint32,
		Uint64,
		Bool,
		Array,
		Map,
		Set,
		Struct,
		Enum);
}

UE_ENABLE_OPTIMIZATION_SHIP

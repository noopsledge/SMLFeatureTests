#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SMLFeatureTestsFunctionThunks.generated.h"

UINTERFACE()
class USLMFeatureTestsFunctionThunkInterface : public UInterface
{
	GENERATED_BODY()
};

class ISLMFeatureTestsFunctionThunkInterface
{
	GENERATED_BODY()
};

USTRUCT()
struct FSMLFeatureTestsFunctionThunkStructParameter
{
	GENERATED_BODY()

	UPROPERTY()
	int Value1;

	UPROPERTY()
	int Value2;

	bool operator==(const FSMLFeatureTestsFunctionThunkStructParameter&) const = default;
};

UENUM()
enum class ESMLFeatureTestsFunctionThunkEnumParameter : uint8
{
	Value0,
	Value1,
	Value2,
	Value3,
	Value4,
};

/**
 * Tests for FunctionThunkGenerator
 */
UCLASS()
class USMLFeatureTestsFunctionThunks : public UObject, public ISLMFeatureTestsFunctionThunkInterface
{
	GENERATED_BODY()

public:
	void RunTest();

protected:
	UFUNCTION(CustomThunk)
	void NoParameters();

	UFUNCTION(CustomThunk)
	int ReturnConstant() const;

	UFUNCTION(CustomThunk)
	static TArray<int> ReturnArray(int Value1, int Value2);

	UFUNCTION(CustomThunk)
	static void AddToInParam(int& Param, int AmountToAdd);

	UFUNCTION(CustomThunk)
	void ParamValues(
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
		ESMLFeatureTestsFunctionThunkEnumParameter Enum);

	UFUNCTION(CustomThunk)
	void ParamRefs(
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
		const ESMLFeatureTestsFunctionThunkEnumParameter& Enum);

private:
	using ParamValuesFunctionType = void(
		int8,
		int16,
		int32,
		int64,
		uint8,
		uint16,
		uint32,
		uint64,
		bool,
		bool,
		TArray<int>,
		TMap<int, int>,
		TSet<int>,
		USMLFeatureTestsFunctionThunks*,
		TSubclassOf<USMLFeatureTestsFunctionThunks>,
		TScriptInterface<ISLMFeatureTestsFunctionThunkInterface>,
		FSMLFeatureTestsFunctionThunkStructParameter,
		ESMLFeatureTestsFunctionThunkEnumParameter);

	using ParamRefsFunctionType = void(
		const int8&,
		const int16&,
		const int32&,
		const int64&,
		const uint8&,
		const uint16&,
		const uint32&,
		const uint64&,
		const bool&,
		const TArray<int>&,
		const TMap<int, int>&,
		const TSet<int>&,
		const FSMLFeatureTestsFunctionThunkStructParameter&,
		const ESMLFeatureTestsFunctionThunkEnumParameter&);

	void TestNoParameters();
	void TestReturnConstant();
	void TestReturnArray();
	void TestAddToInParam();
	void TestParamValues();
	void TestParamRefs();

	// Custom thunks.
	static std::remove_pointer_t<FNativeFuncPtr>& execNoParameters;
	static std::remove_pointer_t<FNativeFuncPtr>& execReturnConstant;
	static std::remove_pointer_t<FNativeFuncPtr>& execReturnArray;
	static std::remove_pointer_t<FNativeFuncPtr>& execAddToInParam;
	static std::remove_pointer_t<FNativeFuncPtr>& execParamValues;
	static std::remove_pointer_t<FNativeFuncPtr>& execParamRefs;

	int NoParametersCallCount = 0;
	int ConstantToReturn = 0;
	TFunction<ParamValuesFunctionType> ParamValuesCallback;
	TFunction<ParamRefsFunctionType> ParamRefsCallback;
};

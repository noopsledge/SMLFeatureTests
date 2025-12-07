#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SMLFeatureTestsNativeHooking.generated.h"

/**
 * Tests for NativeHookManager
 */
UCLASS()
class USMLFeatureTestsNativeHooking : public UObject
{
	GENERATED_BODY()

public:
	void RunTest();

protected:
	enum { DEFAULT_VALUE = 12345, MODDED_VALUE = 54321 };
	struct SmallStruct { int Value; };
	struct LargeStruct { unsigned char Prefix[60]; int Value; };

	UFUNCTION()
	static int GetValueStatic(int AmountToAdd);
	UFUNCTION()
	int GetValueMember(int AmountToAdd) const;
	UFUNCTION()
	virtual int GetValueVirtual(int AmountToAdd) const;

	// Tests that we conform to the ABI When it comes to returning user-defined types.
	static SmallStruct GetSmallStructStatic(int AmountToAdd);
	static LargeStruct GetLargeStructStatic(int AmountToAdd);
	SmallStruct GetSmallStructMember(int AmountToAdd) const;
	LargeStruct GetLargeStructMember(int AmountToAdd) const;

private:
	void TestStandardHooks();
	void TestAfterHooks();
	void TestMultiHooks();
	void TestVtableHooks();
	void TestUFunctionHooks();
};

#include "Features/SMLFeatureTestsNativeHooking.h"

#include "Patching/NativeHookManager.h"

UE_DISABLE_OPTIMIZATION_SHIP

int USMLFeatureTestsNativeHooking::GetValueStatic(int AmountToAdd) { return DEFAULT_VALUE + AmountToAdd; }
int USMLFeatureTestsNativeHooking::GetValueMember(int AmountToAdd) const { return DEFAULT_VALUE + AmountToAdd; }
int USMLFeatureTestsNativeHooking::GetValueVirtual(int AmountToAdd) const { return DEFAULT_VALUE + AmountToAdd; }
auto USMLFeatureTestsNativeHooking::GetSmallStructStatic(int AmountToAdd) -> SmallStruct { return { .Value = DEFAULT_VALUE + AmountToAdd }; }
auto USMLFeatureTestsNativeHooking::GetLargeStructStatic(int AmountToAdd) -> LargeStruct { return { .Value = DEFAULT_VALUE + AmountToAdd }; }
auto USMLFeatureTestsNativeHooking::GetSmallStructMember(int AmountToAdd) const -> SmallStruct { return { .Value = DEFAULT_VALUE + AmountToAdd }; }
auto USMLFeatureTestsNativeHooking::GetLargeStructMember(int AmountToAdd) const -> LargeStruct { return { .Value = DEFAULT_VALUE + AmountToAdd }; }

void USMLFeatureTestsNativeHooking::RunTest()
{
	// These tests are run multiple times to ensure that there're no lingering issues with unhooking.
	for (int i = 0; i < 3; ++i)
	{
		TestStandardHooks();
		TestAfterHooks();
		TestMultiHooks();
	}
}

void USMLFeatureTestsNativeHooking::TestStandardHooks()
{
	// Static function.
	{
		const FDelegateHandle Handler = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic,
			[](auto& Scope, int AmountToAdd)
			{
				Scope.Override(MODDED_VALUE + AmountToAdd);
			});

		check(GetValueStatic(8) == MODDED_VALUE + 8);
		check(GetValueStatic(9) == MODDED_VALUE + 9);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic, Handler);

		check(GetValueStatic(8) == DEFAULT_VALUE + 8);
		check(GetValueStatic(9) == DEFAULT_VALUE + 9);
	}

	// Member function.
	{
		const FDelegateHandle Handler = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueMember,
			[this](auto& Scope, const USMLFeatureTestsNativeHooking* Self, int AmountToAdd)
			{
				check(Self == this);
				Scope.Override(MODDED_VALUE + AmountToAdd);
			});

		check(GetValueMember(3) == MODDED_VALUE + 3);
		check(GetValueMember(4) == MODDED_VALUE + 4);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueMember, Handler);

		check(GetValueMember(3) == DEFAULT_VALUE + 3);
		check(GetValueMember(4) == DEFAULT_VALUE + 4);
	}

	// Virtual function.
	{
		const FDelegateHandle Handler = SUBSCRIBE_METHOD_VIRTUAL(USMLFeatureTestsNativeHooking::GetValueVirtual,
			this,
			[this](auto& Scope, const USMLFeatureTestsNativeHooking* Self, int AmountToAdd)
			{
				check(Self == this);
				Scope.Override(MODDED_VALUE + AmountToAdd);
			});

		check(GetValueVirtual(6) == MODDED_VALUE + 6);
		check(GetValueVirtual(7) == MODDED_VALUE + 7);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueVirtual, Handler);

		check(GetValueVirtual(6) == DEFAULT_VALUE + 6);
		check(GetValueVirtual(7) == DEFAULT_VALUE + 7);
	}

	// Virtual function on UObject.
	{
		const FDelegateHandle Handler = SUBSCRIBE_UOBJECT_METHOD(USMLFeatureTestsNativeHooking, GetValueVirtual,
			[this](auto& Scope, const USMLFeatureTestsNativeHooking* Self, int AmountToAdd)
			{
				check(Self == this);
				Scope.Override(MODDED_VALUE + AmountToAdd);
			});

		check(GetValueVirtual(8) == MODDED_VALUE + 8);
		check(GetValueVirtual(9) == MODDED_VALUE + 9);

		UNSUBSCRIBE_UOBJECT_METHOD(USMLFeatureTestsNativeHooking, GetValueVirtual, Handler);

		check(GetValueVirtual(8) == DEFAULT_VALUE + 8);
		check(GetValueVirtual(9) == DEFAULT_VALUE + 9);
	}

	// Small struct from static function.
	{
		const FDelegateHandle Handler = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetSmallStructStatic,
			[](auto& Scope, int AmountToAdd)
			{
				Scope.Override({ .Value = MODDED_VALUE + AmountToAdd });
			});

		check(GetSmallStructStatic(12).Value == MODDED_VALUE + 12);
		check(GetSmallStructStatic(13).Value == MODDED_VALUE + 13);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetSmallStructStatic, Handler);

		check(GetSmallStructStatic(12).Value == DEFAULT_VALUE + 12);
		check(GetSmallStructStatic(13).Value == DEFAULT_VALUE + 13);
	}

	// Small struct from member function.
	{
		const FDelegateHandle Handler = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetSmallStructMember,
			[this](auto& Scope, const USMLFeatureTestsNativeHooking* Self, int AmountToAdd)
			{
				check(Self == this);
				Scope.Override({ .Value = MODDED_VALUE + AmountToAdd });
			});

		check(GetSmallStructMember(12).Value == MODDED_VALUE + 12);
		check(GetSmallStructMember(13).Value == MODDED_VALUE + 13);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetSmallStructMember, Handler);

		check(GetSmallStructMember(12).Value == DEFAULT_VALUE + 12);
		check(GetSmallStructMember(13).Value == DEFAULT_VALUE + 13);
	}

	// Large struct from static function.
	{
		const FDelegateHandle Handler = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetLargeStructStatic,
			[](auto& Scope, int AmountToAdd)
			{
				Scope.Override({ .Value = MODDED_VALUE + AmountToAdd });
			});

		check(GetLargeStructStatic(12).Value == MODDED_VALUE + 12);
		check(GetLargeStructStatic(13).Value == MODDED_VALUE + 13);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetLargeStructStatic, Handler);

		check(GetLargeStructStatic(12).Value == DEFAULT_VALUE + 12);
		check(GetLargeStructStatic(13).Value == DEFAULT_VALUE + 13);
	}

	// Large struct from member function.
	{
		const FDelegateHandle Handler = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetLargeStructMember,
			[this](auto& Scope, const USMLFeatureTestsNativeHooking* Self, int AmountToAdd)
			{
				check(Self == this);
				Scope.Override({ .Value = MODDED_VALUE + AmountToAdd });
			});

		check(GetLargeStructMember(12).Value == MODDED_VALUE + 12);
		check(GetLargeStructMember(13).Value == MODDED_VALUE + 13);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetLargeStructMember, Handler);

		check(GetLargeStructMember(12).Value == DEFAULT_VALUE + 12);
		check(GetLargeStructMember(13).Value == DEFAULT_VALUE + 13);
	}
}

void USMLFeatureTestsNativeHooking::TestAfterHooks()
{
	// After static function.
	{
		unsigned CalledHandler = 0;
		int ExpectedResult = -1;
		int ExpectedAmountToAdd = -1;

		const FDelegateHandle Handler = SUBSCRIBE_METHOD_AFTER(USMLFeatureTestsNativeHooking::GetValueStatic,
			[&](int Result, int AmountToAdd)
			{
				++CalledHandler;
				checkf(Result == ExpectedResult,
					TEXT("Expected %u, got %u"), ExpectedResult, Result);
				checkf(AmountToAdd == ExpectedAmountToAdd,
					TEXT("Expected %u, got %u"), ExpectedAmountToAdd, AmountToAdd);
			});

		auto DoTest = [&](int AmountToAdd, bool IsHooked)
		{
			ExpectedAmountToAdd = AmountToAdd;
			ExpectedResult = DEFAULT_VALUE + AmountToAdd;
			check(GetValueStatic(AmountToAdd) == ExpectedResult);
			check(CalledHandler == static_cast<unsigned>(IsHooked));
			CalledHandler = 0;
		};

		DoTest(101, true);
		DoTest(102, true);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic, Handler);

		DoTest(101, false);
		DoTest(102, false);
	}

	// After member function.
	{
		unsigned CalledHandler = 0;
		int ExpectedResult = -1;
		int ExpectedAmountToAdd = -1;

		const FDelegateHandle Handler = SUBSCRIBE_METHOD_AFTER(USMLFeatureTestsNativeHooking::GetValueMember,
			[&](int Result, const USMLFeatureTestsNativeHooking* Self, int AmountToAdd)
			{
				++CalledHandler;
				check(Self == this);
				checkf(Result == ExpectedResult,
					TEXT("Expected %u, got %u"), ExpectedResult, Result);
				checkf(AmountToAdd == ExpectedAmountToAdd,
					TEXT("Expected %u, got %u"), ExpectedAmountToAdd, AmountToAdd);
			});

		auto DoTest = [&](int AmountToAdd, bool IsHooked)
		{
			ExpectedAmountToAdd = AmountToAdd;
			ExpectedResult = DEFAULT_VALUE + AmountToAdd;
			check(GetValueMember(AmountToAdd) == ExpectedResult);
			check(CalledHandler == static_cast<unsigned>(IsHooked));
			CalledHandler = 0;
		};

		DoTest(101, true);
		DoTest(102, true);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueMember, Handler);

		DoTest(101, false);
		DoTest(102, false);
	}
}

void USMLFeatureTestsNativeHooking::TestMultiHooks()
{
	// Two hooks that should both run.
	{
		unsigned CalledHandler1 = 0;
		unsigned CalledHandler2 = 0;

		const FDelegateHandle Handler1 = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic,
			[&](auto& Scope, int AmountToAdd)
			{
				++CalledHandler1;
				Scope.Override(Scope(AmountToAdd) * 2);
			});
		const FDelegateHandle Handler2 = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic,
			[&](auto& Scope, int AmountToAdd)
			{
				++CalledHandler2;
				Scope.Override(Scope(AmountToAdd) * 3);
			});

		// Both handlers should multiply the result.
		check(GetValueStatic(10) == (DEFAULT_VALUE + 10) * 6);
		check(CalledHandler1 == 1);
		check(CalledHandler2 == 1);
		CalledHandler1 = CalledHandler2 = 0;
		check(GetValueStatic(21) == (DEFAULT_VALUE + 21) * 6);
		check(CalledHandler1 == 1);
		check(CalledHandler2 == 1);
		CalledHandler1 = CalledHandler2 = 0;

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic, Handler1);

		// One handler is unregistered, only one of them multiplies the result.
		check(GetValueStatic(10) == (DEFAULT_VALUE + 10) * 3);
		check(CalledHandler1 == 0);
		check(CalledHandler2 == 1);
		CalledHandler1 = CalledHandler2 = 0;
		check(GetValueStatic(21) == (DEFAULT_VALUE + 21) * 3);
		check(CalledHandler1 == 0);
		check(CalledHandler2 == 1);
		CalledHandler1 = CalledHandler2 = 0;

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic, Handler2);

		// All unregistered, everything should be back to normal.
		check(GetValueStatic(10) == DEFAULT_VALUE + 10);
		check(CalledHandler1 == 0);
		check(CalledHandler2 == 0);
		check(GetValueStatic(21) == DEFAULT_VALUE + 21);
		check(CalledHandler1 == 0);
		check(CalledHandler2 == 0);
	}

	// Hook that never calls the second handler.
	{
		const FDelegateHandle Handler1 = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic,
			[&](auto& Scope, int AmountToAdd)
			{
				Scope.Override(MODDED_VALUE + AmountToAdd);
			});
		const FDelegateHandle Handler2 = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic,
			[&](auto& Scope, int AmountToAdd)
			{
				check(false);
			});

		check(GetValueStatic(5) == MODDED_VALUE + 5);
		check(GetValueStatic(50) == MODDED_VALUE + 50);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic, Handler1);
		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic, Handler2);

		check(GetValueStatic(5) == DEFAULT_VALUE + 5);
		check(GetValueStatic(50) == DEFAULT_VALUE + 50);
	}

	// Hook that changes a parameter and passes that down the chain.
	{
		const FDelegateHandle Handler1 = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic,
			[&](auto& Scope, int AmountToAdd) { Scope.Override(Scope(AmountToAdd + 1) * 5); });
		const FDelegateHandle Handler2 = SUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic,
			[&](auto& Scope, int AmountToAdd) { Scope.Override(Scope(AmountToAdd) * 6); });

		check(GetValueStatic(25) == (DEFAULT_VALUE + 26) * 30);
		check(GetValueStatic(22) == (DEFAULT_VALUE + 23) * 30);

		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic, Handler1);
		UNSUBSCRIBE_METHOD(USMLFeatureTestsNativeHooking::GetValueStatic, Handler2);

		check(GetValueStatic(25) == DEFAULT_VALUE + 25);
		check(GetValueStatic(22) == DEFAULT_VALUE + 22);
	}
}

UE_ENABLE_OPTIMIZATION_SHIP

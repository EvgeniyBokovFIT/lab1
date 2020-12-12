#include "pch.h"
#include "Tritset.h"

TEST(Tritset, Capacity)
{
	Tritset set(200);
	size_t AllocLength = set.capacity();
	EXPECT_TRUE(AllocLength >= 200 * 2 / 8 / sizeof(uint));

}
TEST(Tritset, DontAllocMemoryForUnknown)
{
	Tritset set(200);
	size_t PrevCapacity = set.capacity();
	set[1000] = TritState::UNKNOWN;
	EXPECT_TRUE(PrevCapacity == set.capacity());

}
TEST(Tritset,AllocMemory)
{
	Tritset set(200);
	size_t PrevCapacity = set.capacity();
	set[1000] = TritState::TRUE;
	EXPECT_TRUE(PrevCapacity < set.capacity());

}
TEST(Tritset, Length)
{
	Tritset set(200);
	size_t Length = set.GetLengthInTrits();
	EXPECT_TRUE(Length == 200);

}
TEST(Tritset, TritAndStateEqual)
{

	Tritset set(100);
	set[15] = TritState::TRUE;
	EXPECT_TRUE(set[15] == TritState::TRUE);
}
TEST(Tritset, TritAndStateNotEqual)
{

	Tritset set(100);
	set[15] = TritState::TRUE;
	EXPECT_TRUE(set[15] != TritState::FALSE);
}

TEST(Tritset, TritsEqual)
{
	Tritset set(100);
	set[15] = TritState::TRUE;
	set[16] = TritState::TRUE;
	EXPECT_TRUE(set[15] == set[16]);
}

TEST(Tritset, AssignAndIndexOperators)
{
	Tritset set(100);
	set[14] = set[15] = TritState::TRUE;
	EXPECT_TRUE(set[15] == set[14]);
}

TEST(Tritset, TritsNotEqual)
{
	Tritset set(100);
	set[15] = TritState::TRUE;
	set[16] = TritState::FALSE;
	EXPECT_TRUE(set[15] != set[16]);
}

TEST(Tritset, Shrink)
{
	Tritset set(100);
	set[10] = TritState::TRUE;
	size_t allocLength = set.capacity();
	set.shrink();
	EXPECT_TRUE(allocLength > set.capacity());

}

TEST(Tritset, AND)
{
	Tritset setA(100);
	setA[15] = TritState::TRUE;
	setA[16] = TritState::FALSE;
	Tritset setB(50);
	setB[15] = TritState::TRUE;
	setB[16] = TritState::TRUE;
	Tritset setC = setA & setB;
	Tritset setAAfterOperation = setA;
	for (size_t i = 0; i < setAAfterOperation.GetLengthInTrits(); i++)
	{
		EXPECT_TRUE(setAAfterOperation[i] == setA[i]);
	}
	Tritset setBAfterOperation = setB;
	for (size_t i = 0; i < setBAfterOperation.GetLengthInTrits(); i++)
	{
		EXPECT_TRUE(setBAfterOperation[i] == setB[i]);
	}
	EXPECT_TRUE(setC[15] == TritState::TRUE);
	EXPECT_TRUE(setC[16] == TritState::FALSE);
	EXPECT_TRUE(setC[50] == TritState::UNKNOWN);
}

TEST(Tritset, OR)
{
	Tritset setA(100);
	setA[15] = TritState::TRUE;
	setA[16] = TritState::FALSE;
	Tritset setB(50);
	setB[15] = TritState::TRUE;
	setB[16] = TritState::TRUE;
	Tritset setC = setA || setB;
	Tritset setAAfterOperation = setA;
	for (size_t i = 0; i < setAAfterOperation.GetLengthInTrits(); i++)
	{
		EXPECT_TRUE(setAAfterOperation[i] == setA[i]);
	}
	Tritset setBAfterOperation = setB;
	for (size_t i = 0; i < setBAfterOperation.GetLengthInTrits(); i++)
	{
		EXPECT_TRUE(setBAfterOperation[i] == setB[i]);
	}
	EXPECT_TRUE(setC[15] == TritState::TRUE);
	EXPECT_TRUE(setC[16] == TritState::TRUE);
	EXPECT_TRUE(setC[50] == TritState::UNKNOWN);
}

TEST(Tritset, NOT)
{
	Tritset setA(100);
	setA[15] = TritState::TRUE;
	setA[16] = TritState::FALSE;
	Tritset setB = ~setA;
	Tritset setAAfterOperation = setA;
	for (size_t i = 0; i < setAAfterOperation.GetLengthInTrits(); i++)
	{
		EXPECT_TRUE(setAAfterOperation[i] == setA[i]);
	}
	EXPECT_TRUE(setB[15] == TritState::FALSE);
	EXPECT_TRUE(setB[16] == TritState::TRUE);
	EXPECT_TRUE(setB[50] == TritState::UNKNOWN);
	EXPECT_TRUE(setB[1000] == TritState::UNKNOWN);
}

TEST(Tritset, AssignTritsetToTritset)
{
	Tritset setA(100);
	for (size_t i = 0; i < 10; i++)
	{
		setA[i] = TritState::TRUE;
	}
	for (size_t i = 10; i < 50; i++)
	{
		setA[i] = TritState::FALSE;
	}
	Tritset setB = setA;
	for (size_t i = 0; i < 100; i++)
	{
		EXPECT_TRUE(setB[i] == setA[i]);
	}
}

TEST(Tritset, Cardinality)
{
	Tritset set(100);
	for (size_t i = 1; i < 10; i++)
	{
		set[i] = TritState::TRUE;
	}
	for (size_t i = 10; i < 30; i++)
	{
		set[i] = TritState::FALSE;
	}

	EXPECT_TRUE(set.cardinality(TritState::TRUE) == 9);
	EXPECT_TRUE(set.cardinality(TritState::FALSE) == 20);
	EXPECT_TRUE(set.cardinality(TritState::UNKNOWN) == 1);
	
}

TEST(Tritset, CardinalityMap)
{
	Tritset set(100);
	set[5] = TritState::FALSE;
	set[7] = TritState::TRUE;
	set[10] = TritState::TRUE;

	auto map = set.cardinality();

	EXPECT_EQ(map[TritState::TRUE], 2);
	EXPECT_EQ(map[TritState::FALSE], 1);
	EXPECT_EQ(map[TritState::UNKNOWN], 8);
}

TEST(Tritset, Trim)
{
	Tritset set(100);
	for (size_t i = 1; i < 50; i++)
	{
		set[i] = TritState::TRUE;
	}
	set.trim(30);

	for (size_t i = 30; i < 100; i++)
	{
		EXPECT_TRUE(set[i] == TritState::UNKNOWN);
	}

}

TEST(Tritset, LogicalLength)
{
	Tritset set(100);
	for (size_t i = 1; i < 50; i++)
	{
		set[i] = TritState::TRUE;
	}
	EXPECT_TRUE(set.LogicalLength() == 50);

	set[70] = TritState::FALSE;
	EXPECT_TRUE(set.LogicalLength() == 71);

	set[70] = TritState::UNKNOWN;
	EXPECT_TRUE(set.LogicalLength() == 50);
}

TEST(Tritset, TaskFullTest)
{
	Tritset set(1000);

	size_t AllocLength = set.capacity();
	EXPECT_TRUE(AllocLength >= 200 * 2 / 8 / sizeof(uint));

	set[10000000] = TritState::UNKNOWN;
	EXPECT_TRUE(AllocLength == set.capacity());

	if (set[20000000] == TritState::TRUE)
	{

	}
	EXPECT_TRUE(AllocLength == set.capacity());

	set[10000000] = TritState::TRUE;
	EXPECT_TRUE(AllocLength < set.capacity());

	AllocLength = set.capacity();
	set[10000000] = TritState::UNKNOWN;
	set[100000] = TritState::FALSE;
	EXPECT_TRUE(AllocLength == set.capacity());

	set.shrink();
	EXPECT_TRUE(AllocLength > set.capacity());

	Tritset setA(1000);
	Tritset setB(2000);
	Tritset setC = setA & setB;
	EXPECT_TRUE(setC.capacity() == setB.capacity());

}

TEST(Tritset, ForAuto)
{
	Tritset set(100);
	for (auto& trit : set)
	{
		trit = TritState::TRUE;
	}
	for (size_t i = 0; i < 100; i++)
	{
		EXPECT_TRUE(set[i] == TritState::TRUE);
	}
}

#include "Tritset.h"

Tritset::Tritset()
{
	this->lengthInTrits = 0;
}

Tritset::Tritset(size_t length)
{
	this->lengthInTrits = length;

	size_t lengthInUints = (size_t)ceil(double(lengthInTrits) * 2 / 8.0 / sizeof(uint));

	uint Shift = sizeof(uint) * 8 - 2;

	uint TritValue = (uint)TritState::UNKNOWN;

	uint FullUnknownUintValue = TritValue;

	for (size_t j = Shift; j > 1; j -= 2)
	{
		FullUnknownUintValue += (TritValue << j);
	}

	this->Trits.resize(lengthInUints, FullUnknownUintValue); //assign unknown to every trit
}

Tritset::Tritset(const Tritset& other)
{
	this->lengthInTrits = other.lengthInTrits;

	this->Trits = other.Trits;
}

Tritset::TritsetProxy::TritsetProxy(Tritset& other, size_t index) : set(other)
{
	TritIndex = index;
	CurUintIndex = size_t(TritIndex * 2 / 8 / sizeof(uint));
}

Tritset Tritset::TritsetProxy::GetSet()
{
	return set;
}

size_t Tritset::TritsetProxy::GetTritIndex()
{
	return this->TritIndex;
}

TritState Tritset::TritsetProxy::operator = (TritState value)
{
	if ((this->TritIndex >= set.lengthInTrits) && (value != TritState::UNKNOWN))
	{

		uint Shift = sizeof(uint) * 8 - 2;

		size_t PrevSize = set.Trits.size();

		uint TritValue = (uint)TritState::UNKNOWN;

		uint FullUnknownUintValue = TritValue;

		for (size_t j = Shift; j > 1; j -= 2)
		{
			FullUnknownUintValue += (TritValue << j);
		}

		set.Trits.resize(CurUintIndex + 1, FullUnknownUintValue);

		set.lengthInTrits = this->TritIndex + 1;

	}

	if (CurUintIndex < set.Trits.size())
	{
		uint UintValue = set.Trits[CurUintIndex];

		size_t TritIndexInCurUint = (TritIndex * 2 - CurUintIndex * sizeof(uint) * 8) / 2;

		uint tritValue = (uint)value;
		tritValue = tritValue << ((sizeof(uint) * 8 - 2) / 2 - TritIndexInCurUint) * 2;

		uint LeftValues = 0;
		if (TritIndexInCurUint > 0) //to don't shift uint on sizeof(uint) * 8 bits because it has undefined behavior
		{
			LeftValues = UintValue >> (sizeof(uint) * 8 - TritIndexInCurUint * 2);
			LeftValues = LeftValues << (sizeof(uint) * 8 - TritIndexInCurUint * 2);
		}

		uint RightValues = 0;
		if (TritIndexInCurUint < (sizeof(uint) * 8 / 2 - 1)) //to don't shift uint on sizeof(uint) * 8 bits because it has undefined behavior
		{
			RightValues = UintValue << (TritIndexInCurUint * 2 + 2);
			RightValues = RightValues >> (TritIndexInCurUint * 2 + 2);
		}

		UintValue = LeftValues | tritValue | RightValues;

		set.Trits[CurUintIndex] = UintValue;
	}

	return value;
}

bool Tritset::TritsetProxy::operator ==(const TritsetProxy& other)
{
	if (this->set.GetValueByIndex(this->TritIndex, this->set.lengthInTrits) ==
		other.set.GetValueByIndex(other.TritIndex, other.set.lengthInTrits) &&
		&(this->set) == &(other.set) && this->TritIndex == other.TritIndex)
	{
		return true;
	}
	return false;
}

bool Tritset::TritsetProxy::operator !=(const TritsetProxy& other)
{
	if (*this == other)
	{
		return false;
	}
	return true;
}

bool Tritset::TritsetProxy::operator ==(TritState value)
{
	return this->set.GetValueByIndex(this->TritIndex, this->set.lengthInTrits) == value;
}

bool Tritset::TritsetProxy::operator !=(TritState value)
{
	return !(this->set.GetValueByIndex(this->TritIndex, this->set.lengthInTrits) == value);
}

void Tritset::TritsetProxy::operator ++()
{
	this->TritIndex++;
	CurUintIndex = size_t(TritIndex * 2 / 8 / sizeof(uint));
}

Tritset::TritsetProxy& Tritset::TritsetProxy::operator *()
{
	return *this;
}

size_t Tritset::GetLengthInTrits()
{
	return this->lengthInTrits;
}

Tritset::TritsetProxy Tritset::operator [] (const size_t i)
{
	TritsetProxy proxyObj(*this, i);
	return proxyObj;
}

size_t Tritset::capacity()
{
	return this->Trits.capacity();
}

TritState Tritset::GetValueByIndex(size_t TritIndex, size_t TritLength) const
{
	if (TritIndex >= TritLength)
	{
		return TritState::UNKNOWN;
	}
	size_t CurUintIndex = size_t(TritIndex * 2 / 8 / sizeof(uint));

	size_t TritIndexInCurUint = size_t((TritIndex * 2 - CurUintIndex * sizeof(uint) * 8) / 2);

	uint UintValue = this->Trits[CurUintIndex] << TritIndexInCurUint * 2;
	UintValue = UintValue >> (sizeof(uint) * 8 - 2);

	TritState value = TritState(UintValue);
	return value;
}

void Tritset::shrink()
{
	size_t RightTritIndex = this->lengthInTrits - 1;
	size_t NewSize = size_t(ceil(double(this->lengthInTrits) * 2.0 / 8.0 / sizeof(uint)));
	while (this->GetValueByIndex(RightTritIndex, this->lengthInTrits) == TritState::UNKNOWN)
	{
		RightTritIndex--;
		if ((RightTritIndex + 1) % (sizeof(uint) * 8 / 2) == 0)
		{
			NewSize--;
		}
	}
	this->Trits.resize(NewSize);
	this->Trits.shrink_to_fit();
	this->lengthInTrits = RightTritIndex + 1;
}

bool Tritset::operator == (const Tritset& other)const
{
	size_t MaxSize = 0;
	if (this->lengthInTrits > other.lengthInTrits)
	{
		MaxSize = this->lengthInTrits;
    }
	else
	{
		MaxSize = other.lengthInTrits;
	}
	for (size_t i = 0; i < MaxSize; i++)
	{
		if (this->GetValueByIndex(i, this->lengthInTrits) != other.GetValueByIndex(i, this->lengthInTrits))
		{
			return false;
		}
	}
	return true;
}

bool Tritset::operator != (const Tritset& other)const
{
	if (*this == other)
	{
		return false;
	}
	return true;
}

Tritset Tritset::operator & (const Tritset& other)const
{
	size_t TritIndex = 0;
	size_t MaxSize = 0;

	if (this->lengthInTrits > other.lengthInTrits)
	{
		MaxSize = this->lengthInTrits;
	}
	else
	{
		MaxSize = other.lengthInTrits;
	}
	Tritset result(MaxSize);

	size_t ResultLengthInUints = size_t(ceil(double(result.lengthInTrits) * 2.0 / 8.0 / sizeof(uint)));

	for (size_t i = 0; i < ResultLengthInUints; i++)
	{
		result.Trits[i] = 0;

		for (size_t j = 0; j < sizeof(uint) * 8 / 2; j++)
		{
			if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::TRUE)
			{
				if (other.GetValueByIndex(TritIndex, other.lengthInTrits) == TritState::TRUE)
				{
					uint ResultValue = uint(TritState::TRUE);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}
				else if (other.GetValueByIndex(TritIndex, other.lengthInTrits) == TritState::FALSE)
				{
					uint ResultValue = uint(TritState::FALSE);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}
				else if (other.GetValueByIndex(TritIndex, other.lengthInTrits) == TritState::UNKNOWN)
				{
					uint ResultValue = uint(TritState::UNKNOWN);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}
			}
			else if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::FALSE)
			{
				uint ResultValue = uint(TritState::FALSE);
				ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
				result.Trits[i] += ResultValue;
			}
			else if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::UNKNOWN)
			{
				if (other.GetValueByIndex(TritIndex, other.lengthInTrits) == TritState::FALSE)
				{
					uint ResultValue = uint(TritState::FALSE);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}

				else
				{
					uint ResultValue = uint(TritState::UNKNOWN);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}
			}
			TritIndex++;
		}
	}
	return result;
}

Tritset Tritset::operator || (const Tritset& other)const
{
	size_t TritIndex = 0;
	size_t MaxSize = 0;

	if (this->lengthInTrits > other.lengthInTrits)
	{
		MaxSize = this->lengthInTrits;
	}
	else
	{
		MaxSize = other.lengthInTrits;
	}
	Tritset result(MaxSize);

	size_t ResultLengthInUints = size_t(ceil(double(result.lengthInTrits) * 2.0 / 8.0 / sizeof(uint)));

	for (size_t i = 0; i < ResultLengthInUints; i++)
	{
		result.Trits[i] = 0;
		for (size_t j = 0; j < sizeof(uint) * 8 / 2; j++)
		{
			if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::TRUE)
			{
				uint ResultValue = uint(TritState::TRUE);
				ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
				result.Trits[i] += ResultValue;

			}
			else if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::FALSE)
			{
				if (other.GetValueByIndex(TritIndex, other.lengthInTrits) == TritState::TRUE)
				{
					uint ResultValue = uint(TritState::TRUE);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}
				else if (other.GetValueByIndex(TritIndex, other.lengthInTrits) == TritState::FALSE)
				{
					uint ResultValue = uint(TritState::FALSE);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}
				else if (other.GetValueByIndex(TritIndex, other.lengthInTrits) == TritState::UNKNOWN)
				{
					uint ResultValue = uint(TritState::UNKNOWN);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}
			}
			else if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::UNKNOWN)
			{
				if (other.GetValueByIndex(TritIndex, other.lengthInTrits) == TritState::TRUE)
				{
					uint ResultValue = uint(TritState::TRUE);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}

				else
				{
					uint ResultValue = uint(TritState::UNKNOWN);
					ResultValue = ResultValue << (sizeof(uint) * 8 / 2 - 1 - j) * 2;
					result.Trits[i] += ResultValue;
				}
			}
			TritIndex++;
		}
	}
	return result;
}

Tritset Tritset::operator ~() const
{
	Tritset result(this->lengthInTrits);
	size_t LengthInUints = size_t(ceil(double(this->lengthInTrits) * 2.0 / 8.0 / sizeof(uint)));
	size_t TritIndex = 0;

	for (size_t i = 0; i < LengthInUints; i++)
	{
		result.Trits[i] = 0;

		for (size_t j = 0; j < sizeof(uint) * 8 / 2; j++)
		{
			if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::TRUE)
			{
				uint NewValue = uint(TritState::FALSE);
				NewValue = NewValue << ((sizeof(uint) * 8 / 2 - 1 - j) * 2);
				result.Trits[i] += NewValue;
			}
			else if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::FALSE)
			{
				uint NewValue = uint(TritState::TRUE);
				NewValue = NewValue << ((sizeof(uint) * 8 / 2 - 1 - j) * 2);
				result.Trits[i] += NewValue;
			}
			else if (this->GetValueByIndex(TritIndex, this->lengthInTrits) == TritState::UNKNOWN)
			{
				uint NewValue = uint(TritState::UNKNOWN);
				NewValue = NewValue << ((sizeof(uint) * 8 / 2 - 1 - j) * 2);
				result.Trits[i] += NewValue;
			}
			TritIndex++;
		}
	}
	return result;
}

Tritset& Tritset::operator = (const Tritset& other)
{
	this->lengthInTrits = other.lengthInTrits;

	this->Trits = other.Trits;

	return *this;
}

size_t Tritset::cardinality(TritState value)
{
	size_t LastTritIndex = this->lengthInTrits - 1;
	while (this->GetValueByIndex(LastTritIndex, this->lengthInTrits) == TritState::UNKNOWN)
	{
		LastTritIndex--;
	}
	size_t count = 0;
	for (size_t i = 0; i <= LastTritIndex; i++)
	{
		if (this->GetValueByIndex(i, this->lengthInTrits) == value)
		{
			count++;
		}
	}
	return count;
}

unordered_map<TritState, int> Tritset::cardinality()
{
	unordered_map<TritState, int> result;

	size_t LastTritIndex = this->lengthInTrits - 1;
	while (this->GetValueByIndex(LastTritIndex, this->lengthInTrits) == TritState::UNKNOWN)
	{
		LastTritIndex--;
	}
	
	for (size_t i = 0; i <= LastTritIndex; i++)
	{
		TritState value = this->GetValueByIndex(i, this->lengthInTrits);
		result[value]++;
	}
	return result;
}

void Tritset::trim(size_t LastIndex)
{
	size_t LastUintIndex = size_t(double(LastIndex) * 2.0 / 8.0 / sizeof(uint));

	size_t TritIndexInCurUint = size_t((LastIndex * 2 - LastUintIndex * sizeof(uint) * 8) / 2);

	this->Trits.resize(LastUintIndex + 1);

	uint LastUintValue = Trits[LastUintIndex];
	LastUintValue = LastUintValue >> ((sizeof(uint) * 8 / 2 - 1 - TritIndexInCurUint) * 2);
	LastUintValue = LastUintValue << ((sizeof(uint) * 8 / 2 - 1 - TritIndexInCurUint) * 2);

	uint UnknownValue = uint(TritState::UNKNOWN);
	size_t LeftShift = size_t((sizeof(uint) * 8 / 2 - 1 - TritIndexInCurUint) * 2);
	for (int i = LeftShift; i > -1; i -= 2)
	{
		LastUintValue += (UnknownValue << i);
	}
	Trits[LastUintIndex] = LastUintValue;

	this->lengthInTrits = LastIndex;
}

size_t Tritset::LogicalLength()
{
	size_t LastIndex = this->lengthInTrits - 1;
	
	while (this->GetValueByIndex(LastIndex, this->lengthInTrits) == TritState::UNKNOWN && LastIndex > 0)
	{
		LastIndex--;
	}
	if (LastIndex > 0)
		return LastIndex + 1;
	else
		return 0;
}

Tritset::TritsetProxy Tritset::begin()
{
	TritsetProxy obj(*this, 0);
	return obj;
}

Tritset::TritsetProxy Tritset::end()
{
	TritsetProxy obj(*this, this->lengthInTrits);
	return obj;
}

ostream& operator << (ostream& out, const TritState& value)
{
	if (value == TritState::TRUE)
	{
		out << "TRUE";
		return out;
	}
	else if (value == TritState::FALSE)
	{
		out << "FALSE";
		return out;
	}
	else
	{
		out << "UNKNOWN";
		return out;
	}
}

ostream& operator << (ostream& out, Tritset::TritsetProxy ProxyObj)
{
	Tritset set = ProxyObj.GetSet();
	size_t TritIndex = ProxyObj.GetTritIndex();
	size_t LengthInTrits = set.GetLengthInTrits();
	out << set.GetValueByIndex(TritIndex, LengthInTrits);
	return out;
}

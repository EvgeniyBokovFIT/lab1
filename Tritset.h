#define _CRT_SECURE_NO_WARNINGS
typedef unsigned int uint;
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>

using namespace std;

enum class TritState
{
	FALSE = 0,
	TRUE = 1,
	UNKNOWN = 2
};

class Tritset
{
private:
	size_t lengthInTrits;

	vector <uint> Trits;

	class TritsetProxy
	{
	private:
		size_t TritIndex;

		Tritset& set;

		size_t CurUintIndex;	

		Tritset GetSet();

		size_t GetTritIndex();

	public:
		TritsetProxy(Tritset& other, size_t index);

		friend ostream& operator << (ostream& out, Tritset::TritsetProxy ProxyObj);	

		TritState operator = (TritState value);

		bool operator ==(const TritsetProxy& other);

		bool operator ==(TritState value);

		bool operator !=(const TritsetProxy& other);

		bool operator !=(TritState value);
	};

public:

	Tritset();

	Tritset(size_t length);

	Tritset(const Tritset& other);
	
	size_t GetLengthInTrits();

	TritsetProxy operator [] (const size_t i);

	size_t capacity();

	friend ostream& operator << (ostream& out, Tritset::TritsetProxy ProxyObj);

	TritState GetValueByIndex(size_t TritIndex, size_t TritLength);

	void shrink();

	Tritset operator & (Tritset& other);

	Tritset operator || (Tritset& other);

	Tritset operator ~();

	Tritset& operator = (const Tritset& other);

	size_t cardinality(TritState value);

	unordered_map<TritState, int>  cardinality();

	void trim(size_t LastIndex);

	size_t LogicalLength();

	~Tritset()
	{

	}

};







#include <iostream>

namespace mutils{

	struct Frequency
	{
		void Print() const;
		const unsigned long long hertz;
		constexpr Frequency operator+(const Frequency& fr) const{
			return Frequency{fr.hertz + hertz};
		}
		constexpr Frequency operator*(const Frequency& fr) const{
			return Frequency{fr.hertz * hertz};
		}
	};
	
	constexpr Frequency operator"" _Hz(unsigned long long hz)
	{
		return Frequency{hz};
	}
	constexpr Frequency operator"" _kHz(long double khz)
	{
		return Frequency{((unsigned long long)khz) * 1000};
	}
	constexpr Frequency operator"" _kHz(unsigned long long khz)
	{
		return Frequency{khz * 1000};
	}

	constexpr Frequency as_hertz(unsigned long long hz){
		return Frequency{hz};
	}
	
	std::ostream& operator<<(std::ostream& os, const Frequency &freq);

}



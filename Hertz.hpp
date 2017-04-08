#pragma once
#include <iostream>

namespace mutils{

	struct Frequency
	{
		void Print() const;
		unsigned long long hertz;
		constexpr Frequency operator+(const Frequency& fr) const{
			return Frequency{fr.hertz + hertz};
		}

		constexpr Frequency operator*(const unsigned long long& factor) const{
			return Frequency{factor * hertz};
		}

		template<typename Rep, typename Period>
		constexpr std::size_t operator*(std::chrono::duration<Rep,Period> time) const{
			using namespace std;
			using namespace chrono;
			return duration_cast<seconds>(time).count()*hertz;
		}
		
		constexpr bool operator<(const Frequency &rhs) const{
			return hertz < rhs.hertz;
		}
		constexpr bool operator>(const Frequency &rhs) const{
			return hertz > rhs.hertz;
		}
		constexpr bool operator==(const Frequency &rhs) const{
			return hertz == rhs.hertz;
		}
		constexpr bool operator!=(const Frequency &rhs) const{
			return hertz != rhs.hertz;
		}
		constexpr bool operator<=(const Frequency &rhs) const{
			return hertz <= rhs.hertz;
		}
		constexpr bool operator>=(const Frequency &rhs) const{
			return hertz >= rhs.hertz;
		}
		Frequency operator=(const Frequency &rhs){
			return Frequency{hertz = rhs.hertz};
		}
		Frequency operator+=(const Frequency &rhs){
			return (*this) = (*this) + rhs;
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
	std::istream& operator>>(std::istream& os, Frequency &freq);

}



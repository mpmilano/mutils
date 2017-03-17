#pragma once

namespace mutils{
		struct CexprString {
			char str[1024]{0};
			operator std::string(){
				return std::string{str};
			}

			constexpr CexprString operator=(const CexprString& other){
				for (int i = 0; i < 1024; ++i){
					str[i] = other.str[i];
					if (other.str[i] == 0) break;
				}
				return *this;
			}
		};

		constexpr int roundTo10s(unsigned long number)
		{
			int digits = (number ? 1 : 0);
			number /= 10;
			while (number) {
				number /= 10;
				digits*=10;
			}
			return digits;
		}
		
		constexpr CexprString to_cexprstring(unsigned int number){
			CexprString ret;
			//0 won't enter the loop
			ret.str[0] = '0';
			ret.str[1] = 0;
			
			for (int i = 0; i < 1023; ++i){
				int tens = roundTo10s(number);
				if (!tens) {
					//null-terminate
					ret.str[i] =0;
					break;
				}
				ret.str[i] = '0' + (number / tens);
				number -= (tens * (number / tens));
			}
			//if the string was too long
			ret.str[1023] = 0;
			return ret;
		}

	static_assert(to_cexprstring(5).str[0] == '5',"Testing to_ststring");
	static_assert(to_cexprstring(53).str[1] == '3',"Testing to_ststring");
	static_assert(to_cexprstring(521).str[1] == '2',"Testing to_ststring");
	static_assert(to_cexprstring(1235).str[2] == '3',"Testing to_ststring");
	static_assert(to_cexprstring(849274).str[4] == '7',"Testing to_ststring");
	static_assert(to_cexprstring(2948672).str[3] == '8',"Testing to_ststring");
		
		static constexpr CexprString operator+(const CexprString &ths, const CexprString& other){
			CexprString newstr;
			int index = 0;
			for (;ths.str[index] != 0;++index) {
				newstr.str[index] = ths.str[index];
			}
			for (int i = 0; index < 1023 && other.str[i] != 0; ++i,++index){
				newstr.str[index] = other.str[i];
			}
			newstr.str[index] = 0;
			return newstr;
		}
		
		static constexpr CexprString operator+(const CexprString &ths, char const * const other){
				CexprString newstr;
				int index = 0;
				for (;ths.str[index] != 0;++index) {
					newstr.str[index] = ths.str[index];
				}
				for (int i = 0; index < 1023 && other[i] != 0; ++i,++index){
					newstr.str[index] = other[i];
				}
				newstr.str[index] = 0;
				return newstr;
			}
		
		static constexpr CexprString operator+(const CexprString &ths, int _other){
			return ths + to_cexprstring(_other);
		}

	static_assert((CexprString{} + "=").str[0] == '=',"");
	static_assert((CexprString{} + "=").str[1] == 0,"");
	static_assert((to_cexprstring(5) + "=").str[0] == '5',"");
	static_assert((to_cexprstring(5) + "=").str[1] == '=',"");
	static_assert((to_cexprstring(5) + "=").str[2] == 0,"");
	static_assert((CexprString{} + 3 +"=").str[1] == '=' ,"");

}

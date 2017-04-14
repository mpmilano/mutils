#include "mutils.hpp"
#include <arpa/inet.h>
#include "Hertz.hpp"
#include "string.h"

namespace mutils{

	std::list<std::string> split(const std::string &s, char c, std::size_t pos){
		if (pos == std::string::npos) return {};
		auto index = s.find(c,pos);
		if (index == std::string::npos) return {s.substr(pos)};
		auto ret = split(s,c,index+1);
		ret.push_front(s.substr(pos,index));
		return ret;
	}


	std::nullptr_t choose_non_np(std::nullptr_t, std::nullptr_t){
		return nullptr;
	}

	std::nullptr_t dref_np(std::nullptr_t*){
		return nullptr;
	}

	unsigned int gensym() {
		static std::mutex m;
		std::unique_lock<std::mutex> lock{m};
		static unsigned  counter = 0;
		assert(counter < (std::numeric_limits<int>::max() - 1));
		return ++counter;
	}

	int get_thread_id(){
		static int id {0};
		thread_local int tid = id++;
		return tid;
	}

	std::vector<std::string> read_dir(const std::string &name){

		std::vector<std::string> ret;
	
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir (name.c_str()))) {
			/* print all the files and directories within directory */
			while ((ent = readdir (dir))) {
				std::string maybe(ent->d_name);
				if (maybe == "." || maybe == "..") continue;
				ret.push_back(std::string(maybe));
			}
			closedir (dir);
		} else {
			/* could not open directory */
			perror ("");
			assert(false && "Could not open dir.");
			struct io_exn : public std::exception{
				const char* what() const noexcept{
					return "io exception on read_dir: could not open directory";
				}
			}; throw io_exn();
		}

		return ret;
	}

	void break_here(){}

	
	bool init_rand(int seed = 0){
		static std::mutex m;
		static bool init = [&](){
			std::unique_lock<std::mutex> lock{m};
			timespec ts;
			clock_gettime(CLOCK_REALTIME,&ts);
			if (seed) srand48(seed);
			else srand48(ts.tv_nsec);
			return true;}();
		return init;
	}
	
	double better_rand(){
		discard(init_rand());
		return drand48();
	}

	long int long_rand() {
		return int_rand();
	}

	unsigned int int_rand(){
		discard(init_rand());
		return lrand48();
	}

	int decode_ip(const std::string &static_addr){
		int ret = 0;
		char* iparr = (char*) &ret;
		int a, b, c, d;
		std::stringstream s(static_addr);
		char ch; //to temporarily store the '.'
		s >> a >> ch >> b >> ch >> c >> ch >> d;
		iparr[0] = a, iparr[1] = b, iparr[2] = c, iparr[3] = d;
#ifndef NDEBUG
		{
			int dbg = decode_ip(static_addr.c_str());
			if (ret != dbg){
				std::cerr << string_of_ip(ret) << std::endl;
				std::cerr << static_addr << std::endl;
				std::cerr << string_of_ip(dbg) << std::endl;
			}
		}
		if (string_of_ip(ret) != static_addr){
			std::cerr << string_of_ip(ret) << std::endl;
			std::cerr << static_addr << std::endl;
		}
		assert(string_of_ip(ret) == static_addr);
#endif
		return ret;
	}

	std::string string_of_ip(unsigned int i){
		if (i == 0) return "127.0.0.1";
		else {
			in_addr a;
			char str[INET_ADDRSTRLEN];
			a.s_addr = i;
			assert(a.s_addr == i);
			inet_ntop(AF_INET,&a,str,INET_ADDRSTRLEN);
			return std::string(str);
		}
	}

	type_id type_id_counter(bool increment){
		static type_id counter = 0;
		if (increment) ++counter;
		return counter;
	}

	void copy_into(std::size_t how_many,std::size_t const * const sizes,void ** bufs, char* src){
		for (std::size_t i = 0; i < how_many; ++i){
			memcpy(bufs[i],src,sizes[i]);
			src += sizes[i];
		}
	}

}
	//Frequency stuff
	
namespace mutils{
	
	void Frequency::Print() const { std::cout << hertz << "Hz\n"; }

	std::ostream& operator<<(std::ostream& os, const Frequency &freq){
		return os << freq.hertz << "_Hz";
	}

	std::istream& operator>>(std::istream& os, Frequency &freq){
		char str[256];
		bzero(str,256);
		for (std::size_t i = 0; os.peek() != '_'; ++i) os.get(str[i]);
		char suffix[] = {0,0,0,0};
		os >> suffix[0] >> suffix[1] >> suffix[2];
		assert(std::string(suffix) == "_Hz");
		freq.hertz = atoi(str);
		return os;
	}

	comma_is_space::comma_is_space() : std::ctype<char>(get_table()) {}

	typename std::ctype_base::mask const* comma_is_space::get_table()
	{
		static mask rc[table_size];
		rc[(int)','] = std::ctype_base::space;
		rc[(int)'\n'] = std::ctype_base::space;
		return &rc[0];
	}

	std::ostream& operator<<(std::ostream& o, const mutils::comma_space&){
		return o << ", ";
	}

	std::istream& operator>>(std::istream& o, const mutils::comma_space&){
		if (o.peek() == ',')
		{
			char cma;
			o.get(cma);
			assert(cma == ',');
		}
		if (o.peek() == ' ')
		{
			char spc;
			o.get(spc);
			assert(spc == ' ');
		}
		return o;
	}
	
}

namespace std{
	std::ostream & operator<<(std::ostream &os, const std::vector<char>& so){
		os << "<[";
		for (auto &e : so){
			os << +e << ",";
		}
		return os << "]>";
	}

	std::ostream & operator<<(std::ostream &os, const std::vector<unsigned char>& so){
		os << "<[";
		for (auto &e : so){
			os << +e << ",";
		}
		return os << "]>";
	}
}

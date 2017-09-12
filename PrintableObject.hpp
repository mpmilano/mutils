#pragma once
#include <type_traits>
#include <sstream>
#include <cxxabi.h>
#include <unistd.h>
#include "type_utils.hpp"
#include "../mutils-serialization/SerializationSupport.hpp"
#include "Name.hpp"

namespace mutils{

	namespace printable_object{
		
		template<typename, typename> struct Field;
		
		template<typename Type, char... str>
		struct Field<Name<str...>, Type>{
			constexpr Field() = default;
			using Name = Name<str...>;
			using type = Type;
			type value;
			static constexpr Field const * const p{nullptr};
		};
		template<typename Type, char... str>
		constexpr Field<Name<str...>, Type> const * const Field<Name<str...>, Type>::p;

		template<typename> struct FieldRef;
		
		template<typename Type, char... str>
		struct FieldRef<Field<Name<str...>, Type> >{
			using Name = Name<str...>;
			using type = Type;
			static constexpr type& getField(Name const * const, Field<Name,type>& f) {return f.value;}
		protected: ~FieldRef() = default;
		};

		
		template<typename... > struct AllFields;
		template<> struct AllFields<>{
			static constexpr void getField(){}
			AllFields() = delete;
			~AllFields() = delete;
		};
		template<typename Field, typename... Rest>
		struct AllFields<Field, Rest...> :
			public FieldRef<Field>, public AllFields<Rest...>{
			using AllFields<Rest...>::getField;
			using FieldRef<Field>::getField;
			AllFields() = delete;
			~AllFields() = delete;
		};
		

	}

	template<typename Name, typename... Fields>
	struct PrintableObject : public Name, public Fields...{
		static_assert(is_name<Name>::value,"error: supply struct Name<...>");
		using Name::name;
		using Name::name_length;
		constexpr PrintableObject() = default;
		template<typename FieldName>
			constexpr auto& getField(FieldName* n){
			return printable_object::AllFields<Fields...>::getField(n,*this);
		}
	};

	template<typename>
	struct is_printable_object : public std::false_type {};
	template<typename Name, typename... Fields>
	struct is_printable_object<PrintableObject<Name,Fields...> > : public std::true_type {};
	
	namespace printable_object{

		template<typename Type, char... str>
		std::ostream & operator<<(std::ostream &os, const Field<Name<str...>, Type> &po){
			constexpr std::size_t size = 256;
			char demangled[size];
			bzero(demangled,size);
			auto _size = size;
			int _status{0};
			abi::__cxa_demangle(typeid(Type).name(),demangled,&_size,&_status);
			return os << "/*" << Name<str...>::name << "*/ " << demangled << "{" << po.value << "}";
		}

		template<typename Type, char... str>
		std::ostream & declare_field(std::ostream &os, Field<Name<str...>, Type> const * const){
			constexpr std::size_t size = 256;
			char demangled[size];
			bzero(demangled,size);
			auto _size = size;
			int _status{0};
			abi::__cxa_demangle(typeid(Type).name(),demangled,&_size,&_status);
			return os << demangled << " " << Name<str...>::name;
		}

		template<typename Field>
		std::ostream & declare_field(std::ostream &os){
			Field* field{nullptr};
			return os << declare_field(os,field);
		}
		

		template<typename PO>
		std::ostream& print_field_declarations(std::ostream &os){
			return os;
		}
		
		template<typename Field1, typename... Fields>
		std::ostream& print_field_declarations(std::ostream &os){
			return print_field_declarations<Fields...>(declare_field<Field1>(os) << (sizeof...(Fields) == 0 ? "" : "; ") );
		}

		template<typename PO>
		std::ostream& print_instance(std::ostream &os, const PO &){
			return os;
		}

		template<typename PO, typename Field1, typename... Fields>
		std::ostream& print_instance(std::ostream &os, const PO &po){
			const Field1& field = po;
			return print_instance<PO,Fields...>(os << field << (sizeof...(Fields) == 0 ? "" : ", "),po);
		}

	}

	template<typename Name, typename... Fields>
	std::ostream & print_declaration(std::ostream &os, PrintableObject<Name, Fields...> const * const){
		os << "struct " << Name::name << " {" << " ";
		printable_object::template print_field_declarations<Fields...>(os);
		return os << "};";
	}

	template<typename PrintableObject>
	std::ostream & print_declaration(std::ostream &os){
		PrintableObject * po{nullptr};
		return print_declaration(os,po);
	}
	
	template<typename Name, typename... Fields>
	std::ostream & operator<<(std::ostream &os, const PrintableObject<Name, Fields...> &po){
		os << Name::name << "{" << " ";
		printable_object::template print_instance<PrintableObject<Name, Fields...>, Fields...>(os,po);
		return os << "};";
	}

	template<typename F, typename PrintableObject, typename... Args>
	std::enable_if_t<is_printable_object<PrintableObject>::value>
	post_object_as_string(const F& f, const PrintableObject& br, Args&&... args){
		using namespace std;
		stringstream ss;
		ss << br;
		auto&& str = ss.str();
		f(forward<Args>(args)...,str.c_str(),str.size());
	}
	
	template<typename F, typename PrintableObject, typename... Args>
	std::enable_if_t<is_printable_object<PrintableObject>::value>
	post_object(const F& f, const PrintableObject& br, Args&&... args){
		return post_object_as_string<F,PrintableObject,Args...>(f,br,std::forward<Args>(args)...);
	}

	template<typename Name, typename... Fields>
	std::size_t to_bytes(const PrintableObject<Name,Fields...>& br, char *v){
		std::size_t index{0};
		return post_object(post_to_buffer(index,v),br);
	}

	namespace printable_object{

		template<typename PO>
		void set_fields(DeserializationManager*, PO&, char const * const, int){
		}
		
		template<typename PO, typename Field1, typename... Fields>
		void set_fields(DeserializationManager* d, PO& po, char const * const str, int in_between_space){
			Field1& f1 = po;
			int start = 0;
			int end = 0;
			for (int i = 0; ; ++i){
				if (str[i] == '{') {
					start = i+1;
					break;
				}
			}
			{
				int stack_depth = 0;
				for (int i = start; ; ++i){
					if (str[i] == '{') ++stack_depth;
					if (str[i] == '}'){
						if (stack_depth) --stack_depth;
						else {
							end = i;
							break;
						}
					}
				}
			}

			auto size = end - start;
			auto field_str = str + start;
			f1.value = *from_string<typename Field1::type>(d,field_str,size);
			set_fields<PO,Fields...>(d,po,str + end + 1 + in_between_space,in_between_space);
		}
		
		template<typename Name, typename... Fields>
		std::unique_ptr<PrintableObject<Name,Fields...> >	PrintableObject_from_string(DeserializationManager* d,
																																								 PrintableObject<Name,Fields...> const * const,
																																								 char const * const v){
			using namespace std;
			assert(strncmp(v,Name::name,Name::name_length) == 0);
			auto ret = std::make_unique<PrintableObject<Name,Fields...> >();
			set_fields<PrintableObject<Name,Fields...>, Fields...>(d,*ret,v + Name::name_length+1,2);
			return ret;
		}
	}

	template<typename T>
	std::unique_ptr<type_check<is_printable_object,T> >	from_string(DeserializationManager* d, char const * const v, std::size_t){
		constexpr T const * const t{nullptr};
		return PrintableObject_from_string(d,t,v);
	}

	template<typename T>
	std::unique_ptr<type_check<is_printable_object,T> >	from_bytes(DeserializationManager* d, char const * const v){
		return from_string<T>(d,v);
	}
}

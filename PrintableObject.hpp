#pragma once
#include <type_traits>
#include <sstream>
#include <cxxabi.h>
#include <unistd.h>

namespace mutils{

	namespace printable_object{

		template<char... str> struct Name {
			constexpr Name() = default;
			static const constexpr char name [] = {str...,0};
		};
		template<char... str>
		const char Name<str...>::name[];

		template<typename> struct is_name : public std::false_type{};
		template<char... str> struct is_name<Name<str...> > : public std::true_type{};

		template<typename, typename> struct Field;
		
		template<typename Type, char... str>
		struct Field<Name<str...>, Type>{
			constexpr Field() = default;
			using Name = Name<str...>;
			using type = Type;
			type value;
		};

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
		static_assert(printable_object::is_name<Name>::value,"error: supply struct Name<...>");
		using Name::name;
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
	post_object(const F& f, const PrintableObject& br, Args&&... args){
		using namespace std;
		stringstream ss;
		ss << br;
		auto&& str = ss.str();
		f(forward<Args>(args)...,str.c_str(),str.size());
	}
}

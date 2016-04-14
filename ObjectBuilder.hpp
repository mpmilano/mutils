#pragma once
#include <array>
#include <map>
#include "filter-varargs.hpp"
#include "type_utils.hpp"
#include "Profiler.hpp"

struct TemporarilySuspendProfiling {
	TemporarilySuspendProfiling(const TemporarilySuspendProfiling& = delete){}

	TemporarilySuspendProfiling(){
		
	}
	
};

/**
 * I intend to use this to print out valid C++ source code containing logging information. 
 * The idea is that I can then write C++ programs which analyze the logged information. 
 * Let's see how well this works.
 */
namespace mutils{
	template<typename _StructNameEnum, _StructNameEnum _StructName, typename _StructFields>
	struct StructType{
		using StructNameEnum = _StructNameEnum;
		using StructName = std::integral_constant<StructNameEnum,_StructName>;
		using StructFields = _StructFields;
		
	};
	
	struct abs_StructBuilder{
		const std::string name;
		abs_StructBuilder(const std::string name)
			:name(name){}

		abs_StructBuilder(const abs_StructBuilder&) = delete;
				
		virtual abs_StructBuilder& addField_impl(int name, std::string data) = 0;
		virtual const std::string& getField_impl(int name) = 0;

		template<typename FNameEnum>
		abs_StructBuilder& addField(FNameEnum Name, const std::string& data){
			Profiler
			std::stringstream ss;
			ss << "\"" << data << "\"";
			return addField_impl(static_cast<int>(Name),ss.str());
		}

		template<typename FNameEnum, typename T>
		abs_StructBuilder& addField(FNameEnum Name, const std::initializer_list<T>& data){
			std::stringstream ss;
			ss << "{";
			int count;
			for (auto& datum : data){
				ss << datum;
				if (count < data.size()) ss << ",";
				++count;
			}
			ss << "}";
			return addField_impl(static_cast<int>(Name),ss.str());
		}

		template<typename FNameEnum>
		const std::string& getField(FNameEnum Name) {
			return getField_impl(static_cast<int>(Name));
		}

		template<typename FNameEnum>
		void incrementIntField(FNameEnum Name) {
			addField(Name,1 + std::stoi(getField(Name)));
		}
		
		template<typename FNameEnum, typename T>
		std::enable_if_t<!std::is_same<std::decay_t<T>, std::string>::value,
						 abs_StructBuilder&> addField(FNameEnum Name, const T& data){
			std::stringstream ss;
			ss << data;
			return addField_impl(static_cast<int>(Name),ss.str());
		}

		virtual std::string single() const = 0;
		
		virtual std::string print_data() const = 0;
		virtual ~abs_StructBuilder(){}
	};
	
	template<typename StructNameEnum, typename... StructType>
	struct ObjectBuilder{
		static_assert(forall_nt(std::is_same<typename StructType::StructNameEnum, StructNameEnum>::value...), "Error: Name mismatch" );
		
		std::map<std::pair<int,int>, std::string> declaration_strings;
		
		template<typename NameEnum>
		const std::string& lookup_declaration_string(NameEnum name) const {
			auto key = std::pair<int,int>{get_type_id<NameEnum>(),
										  static_cast<int>(name)};
			if (declaration_strings.count(key) == 0){
				std::cerr << "looking for a key that's not there!" << std::endl;
				std::cerr << "NameEnum: " << type_name<NameEnum>() << std::endl;
				std::cerr << "Name: " << name << std::endl;
			}
			return declaration_strings.at(key);
		}

		std::map<std::pair<int,int>, std::string> default_strings;

		template<typename NameEnum>
		const std::string& lookup_default(NameEnum name) const {
			auto key = std::pair<int,int>{get_type_id<NameEnum>(),
										  static_cast<int>(name)};
			assert(declaration_strings.count(key) > 0);
			return default_strings.at(key);
				
		}
		
		
		auto init_all_strings() const {
			return std::pair<decltype(declaration_strings),decltype(default_strings)>{};
		}
		
		template<typename NameEnum, typename... Rest>
		auto init_all_strings(const NameEnum name, const std::string &print_name, const std::string &default_val, const Rest&... rst) const {
			auto partial_maps = init_all_strings(rst...);
			auto index = std::pair<int,int>{get_type_id<NameEnum>(), static_cast<int>(name)};
			partial_maps.first[index] = print_name;
			partial_maps.second[index] = default_val;
			return partial_maps;
		}

		template<typename FNameEnum>
		struct ctr_sanity_check_inner{
			const ObjectBuilder &ob;
			
			template<FNameEnum name>
			auto fun(const std::string &accum) const {
				assert(ob.lookup_declaration_string(name) != ob.lookup_default(name));
				return accum;
			}
		};

		struct ctr_sanity_check_outer {
			const ObjectBuilder &ob;
			
			template<StructNameEnum name>
			auto fun(const std::string &accum) const {
				assert(ob.lookup_declaration_string(name) != ob.lookup_default(name));
				using FNameEnum = typename Lookup<name>::StructFields;
				return enum_fold<FNameEnum >(ctr_sanity_check_inner<FNameEnum>{ob},accum);
			}
		};

		ObjectBuilder(const std::pair<std::map<std::pair<int,int>, std::string>,std::map<std::pair<int,int>, std::string> > &pair)
			:declaration_strings(pair.first),default_strings(pair.second){
			enum_fold<StructNameEnum>(ctr_sanity_check_outer{*this},"");
		}
		
		template<typename... name_string_list>
		ObjectBuilder(const name_string_list&... nsl)
			:ObjectBuilder(init_all_strings(nsl...))
			{}
		
		ObjectBuilder(const ObjectBuilder&) = delete;

		template<StructNameEnum StructName>
		struct Lookup_str{
			template<typename FStructType>
			using pred = typename std::integral_constant<bool, StructName == FStructType::StructName::value>::type;
			using type = First<filter_t<pred, StructType...> >;
		};
		
		template<StructNameEnum name>
		using Lookup = typename Lookup_str<name>::type;
		
	
		template<StructNameEnum Name>
		struct StructBuilder : public abs_StructBuilder{
			
			using FNameEnum = typename Lookup<Name>::StructFields;
			
			std::array<std::string, static_cast<int>(FNameEnum::MAX)> field_data;
			
			ObjectBuilder &parent;
			StructBuilder(ObjectBuilder& parent)
				:abs_StructBuilder(parent.lookup_declaration_string(Name) ),parent(parent){}

			std::string& getField_impl(int _name) {
				if (field_data.at(_name).length() == 0)
					field_data[_name] = parent.lookup_default((FNameEnum)_name);
				return field_data.at(_name);
			}
			
			StructBuilder& addField_impl(int _name, std::string data){
				field_data[_name] = data;
				return *this;
			}

			std::string print_data() const {
				std::stringstream out;
				auto data_str = [&](int i){
					return (field_data.at(i).size() == 0
							? parent.lookup_default(static_cast<FNameEnum>(i))
							: field_data.at(i));
				};
				
				for (int i = 0; i < field_data.size() -1; ++i){
					out << data_str(i) << ", ";
				}
				auto last_entry = field_data.size() - 1;
				out << data_str(last_entry);
				return out.str();
			}

			std::string single() const {
				std::stringstream out;
				out << "[]() -> " << this->name << " {" << this->name << " ret {" << print_data() << "}; return ret; }()";
				return out.str();
			}
			
			StructBuilder(const StructBuilder& sb) = delete;
		};
		
		template<StructNameEnum Name>
		std::unique_ptr<StructBuilder<Name> > beginStruct(){
			return std::make_unique<StructBuilder<Name> >(*this);
		}
		
		
		//output!
		
		template<typename NameEnum>
		std::string printWithData(NameEnum name, std::string data) const {
			std::stringstream ss;
			ss << lookup_declaration_string(name) << " { " << data << " }; " << std::endl;
			return ss.str();
		}
		
		template<typename FNameEnum>
		std::string listMembers() const {
			std::stringstream accum;
			for (auto i = 0; i < static_cast<int>(FNameEnum::MAX); ++i){
				accum << "const " << lookup_declaration_string(static_cast<FNameEnum>(i)) << "; ";
			}
			return accum.str();
		}

		struct fold_fun{
			const ObjectBuilder &ob;
			
			template<StructNameEnum name>
			auto fun(const std::string &accum) const {
				return accum + ob.printWithData(name,ob.listMembers<typename Lookup<name>::StructFields >());
			}
		};
		std::string declarations() const {
			return enum_fold<StructNameEnum>(fold_fun{*this},"");
		}
		
	};
}

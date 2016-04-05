#pragma once
#include <array>
#include <map>
#include "filter-varargs.hpp"
#include "type_utils.hpp"

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
	
	template<typename StructNameEnum, typename... StructType>
	struct ObjectBuilder{
		static_assert(forall_nt(std::is_same<typename StructType::StructNameEnum, StructNameEnum>::value...), "Error: Name mismatch" );
		
		std::map<std::pair<int,int>, std::string> declaration_strings;
		
		template<typename NameEnum>
		const std::string& lookup_declaration_string(NameEnum name) const {
			return declaration_strings.at(
				std::pair<int,int>{get_type_id<NameEnum>(),
						static_cast<int>(name)});
		}
		
		decltype(declaration_strings) init_declaration_strings() const {
			return decltype(declaration_strings){};
		}
		
		template<typename NameEnum, typename... Rest>
		decltype(declaration_strings) init_declaration_strings(const NameEnum name, const std::string &print_name, const Rest&... rst) const {
			auto partial_map = init_declaration_strings(rst...);
			partial_map[std::pair<int,int>{get_type_id<NameEnum>(), static_cast<int>(name)}] = print_name;
			return partial_map;
		}
		
		template<typename... name_string_list>
		ObjectBuilder(const name_string_list&... nsl):declaration_strings(init_declaration_strings(nsl...))
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
		
		struct abs_StructBuilder{
			const StructNameEnum name;
			abs_StructBuilder(const StructNameEnum name)
				:name(name){}
			abs_StructBuilder(const abs_StructBuilder &asb):name(asb.name){}

			virtual abs_StructBuilder& addField_impl(int name, std::string data) = 0;
			
			template<typename FNameEnum, typename T>
			abs_StructBuilder& addField(FNameEnum Name, const T& data){
				std::stringstream ss;
				ss << data;
				return addField_impl(static_cast<int>(Name),ss.str());
			}
			
			virtual std::string print_data() const = 0;
			virtual ~abs_StructBuilder(){}
		};
		
		std::vector<std::unique_ptr<abs_StructBuilder> > instances;
		
		template<StructNameEnum name>
		struct StructBuilder : public abs_StructBuilder{
			
			using FNameEnum = typename Lookup<name>::StructFields;
			
			std::array<std::string, static_cast<int>(FNameEnum::MAX)> field_data;
			
			ObjectBuilder &parent;
			StructBuilder(ObjectBuilder& parent)
				:abs_StructBuilder(name),parent(parent){}
			
			StructBuilder& addField_impl(int _name, std::string data){
				field_data[_name] = data;
				return *this;
			}
			
			std::string print_data() const {
				std::stringstream out;
				for (int i = 0; i < field_data.size() -1; ++i){
					out << field_data.at(i) << ", ";
				}
				out << field_data.at(field_data.size() -1);
				return out.str();
			}
			
			StructBuilder(const StructBuilder& sb) = delete;
		};
		
		template<StructNameEnum Name>
		StructBuilder<Name>& beginStruct(){
			auto ptr = std::make_unique<StructBuilder<Name> >(*this);
			auto &ret = *ptr;
			instances.emplace_back(std::move(ptr));
			return ret;
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
		
		std::string objects() const {
			int i = 0;
			std::stringstream out;
			for (auto &ob_p : instances){
				auto &ob = *ob_p;
				out << "const " << lookup_declaration_string(ob.name) << " o" << i++ << "{" << ob.print_data() << "};" << std::endl;
			}
			return out.str();
		}
	};
}

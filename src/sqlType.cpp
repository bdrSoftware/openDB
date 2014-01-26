/* Copyright 2013-2014 Salvatore Barone <salvator.barone@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "sqlType.hpp"
#include "common.hpp"
#include <stdexcept>
#include <typeinfo>
using namespace openDB;
using namespace sqlType;

const std::string boolean::type_name = "boolean";
const std::string boolean::udt_name = "bool";
const std::string boolean::true_default = "true";
const std::string boolean::false_default = "false";
const std::list<std::string> boolean::true_value = {true_default, "TRUE", "t", "y", "yes", "on", "1"};
const std::list<std::string> boolean::false_value = {false_default, "FALSE", "f", "n", "no", "off", "0"};
const std::string date::type_name = "date";
const std::string date::udt_name = "date";
const std::string date::separator = "/-";
const std::string date::qt4_format = "dd/MM/yyyy";
const std::string time::type_name = "time";
const std::string time::udt_name = "time";
const std::string time::separator = ":.";
const std::string time::qt4_format = "hh:mm:ss";
const std::string character::type_name = "character";
const std::string character::udt_name = "bpchar";
const std::string varchar::type_name = "varchar";
const std::string varchar::udt_name = "varchar";
const std::string smallint::type_name = "smallint";
const std::string smallint::udt_name = "int2";
const std::string integer::type_name = "integer";
const std::string integer::udt_name = "int4";
const long int integer::min = std::numeric_limits<int>::min();
const long int integer::max = std::numeric_limits<int>::max();
const std::string bigint::type_name = "bigint";
const std::string bigint::udt_name = "int8";
const long long bigint::min = std::numeric_limits<long long>::min();
const long long bigint::max = std::numeric_limits<long long>::max();
const std::string real::type_name = "real";
const std::string real::udt_name = "float4";
const float real::min = std::numeric_limits<float>::lowest();
const float real::max = std::numeric_limits<float>::max();
const std::string double_precision::type_name = "double precision";
const std::string double_precision::udt_name = "float8";
const long double double_precision::min = std::numeric_limits<long double>::lowest();
const long double double_precision::max = std::numeric_limits<long double>::max();
const std::string numeric::type_name = "numeric";
const std::string numeric::udt_name = "numeric";


std::string boolean::validate_value(std::string value) const throw(data_exception&) {
	for (std::list<std::string>::const_iterator it = true_value.begin(); it != true_value.end(); it++)
		if (value == *it)
			return true_default;
	for (std::list<std::string>::const_iterator it = false_value.begin(); it != false_value.end(); it++)
		if (value == *it)
			return false_default;

	throw invalid_argument("Invalid argument for type 'boolean': " + value + " isn't permitted.");
}

std::string boolean::prepare_value(std::string value) const throw () {
	for (std::list<std::string>::const_iterator it = true_value.begin(); it != true_value.end(); it++)
		if (value == *it)
			return true_default;
	return false_default;
}

std::string date::validate_value(std::string value) const throw(data_exception&) {
	date_integer _date = convert(value);
	if (!validate(_date))
		throw invalid_date("Invalid date: " + value + " is invalid.");
	return ((_date.day<10) ? "0" : "") + std::to_string(_date.day) + "/" + ((_date.month<10) ? "0" : "") + std::to_string(_date.month) + "/" + std::to_string(_date.year);
}

date::date_integer date::convert (std::string __value) const throw (data_exception&) {
	std::unique_ptr<std::list<std::string>> token = tokenize(__value, separator);

	if (token->size() != 3)
		throw invalid_argument("Invalid argument for type 'date': " + __value + " isn't permitted.");

	std::list<std::string>::iterator token_it = token->begin();
	date_integer _date;
	try {
		if (token_it->size() == 4) { //la data è nel formato yyyy/mm/dd
			_date.year = std::stoul(*token_it++);
			_date.month = std::stoul(*token_it++);
			_date.day = std::stoul(*token_it);
		}
		else
		if (token_it->size() >= 1) { 						//la data è nel formato dd/mm/yyyy ?
			_date.day = std::stoul(*token_it++);
			_date.month = std::stoul(*token_it++);
			if (token_it->size() == 4)						//la data è nel formato dd/mm/yyyy
				_date.year = std::stoul(*token_it);
			else											//la data è ambigua.
				throw ambiguous_value("Ambiguous value for type 'date': " + __value + " is ambiguous.");
		}
	}
	catch (std::out_of_range&) {throw invalid_argument("Invalid argument for type 'date': " + __value + " isn't permitted.");}
	catch (std::invalid_argument&) {throw invalid_argument("Invalid argument for type 'date': " + __value + " isn't permitted.");}
	return _date;
}

bool date::validate (date::date_integer __date) const throw () {
	switch (__date.month) {
	case 1: case 3: case 5:	case 7:	case 8:	case 10:	case 12:
		if (__date.day >= 1 && __date.day <= 31)
			return true;
		else
			return false;

	case 4:	case 6:	case 9:	case 11:
		if (__date.day >= 1 && __date.day <= 31)
			return true;
		else
			return false;

	case 2:
		if (__date.day >= 1 && __date.day <= 28)
			return true;
		else {
			if (__date.day == 29 && ((__date.year%4 == 0 && __date.year%100 !=0) || __date.year%400 == 0))
				return true;
			else
				return false;
		}
	default : return false;
	}
}

std::string time::validate_value(std::string value) const throw(data_exception&) {
	time_integer _time = convert(value);
	if (!validate(_time))
		throw invalid_time("Invalid time: " + value + " is invalid.");
	return ((_time.hour < 10) ? "0" : "") + std::to_string(_time.hour) + ":" + ((_time.minute < 10) ? "0" : "") + std::to_string(_time.minute) + ":" + ((_time.second < 10) ? "0" : "") + std::to_string(_time.second);
}

time::time_integer time::convert (std::string __value) const throw (data_exception&) {
	std::unique_ptr<std::list<std::string>> token = tokenize(__value, separator);

	if (token->size() != 3 && token->size() != 2)
		throw invalid_argument("Invalid argument for type 'date': " + __value + " isn't permitted.");

	std::list<std::string>::iterator token_it = token->begin();
	time_integer _time;
	try {
		_time.hour = std::stoul(*token_it++);
		_time.minute = std::stoul(*token_it++);
		if (token->size() == 2)
			_time.second = 0;
		else
			_time.second = std::stoul(*token_it);
	}
	catch (std::out_of_range&) {throw invalid_argument("Invalid argument for type 'time': " + __value + " isn't permitted.");}
	catch (std::invalid_argument&) {throw invalid_argument("Invalid argument for type 'time': " + __value + " isn't permitted.");}
	return _time;
}

bool time::validate (time::time_integer __time) const throw () {
	if ( __time.hour <= 23 && __time.minute <= 59 && __time.second <= 59)
		return true;
	else
		return false;
}


std::string smallint::validate_value(std::string value) const throw(data_exception&) {
	int _value;
	try {_value = std::stoi(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for smallint data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for smallint data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for smallint data type.");
	return value;
}

std::string integer::validate_value(std::string value) const throw(data_exception&) {
	long _value;
	try {_value = std::stol(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for integer data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for integer data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for integer data type.");
	return value;
}

std::string bigint::validate_value(std::string value) const throw(data_exception&) {
	long long _value;
	try {_value = std::stoll(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for bigint data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for bigint data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for bigint data type.");
	return value;
}

std::string real::validate_value(std::string value) const throw(data_exception&) {
	float _value;
	try {_value = std::stof(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for real data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for real data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for real data type.");
	return value;
}


std::string double_precision::validate_value(std::string value) const throw(data_exception&) {
	long double _value;
	try {_value = std::stold(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for double precision data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for double precision data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for double precision data type.");
	return value;
}

std::string numeric::validate_value(std::string value) const throw(data_exception&) {
	std::unique_ptr<std::list<std::string>> token = tokenize(value, '.');
	if (token->size() > 2)
		throw invalid_argument(value + " isn't valid for numeric type.");

	std::list<std::string>::iterator token_it = token->begin();
	unsigned __precision = token_it++ -> size();
	unsigned __scale = 0;
	if (token_it != token->end())
		__scale = token_it->size();
	__precision += __scale;

	if (__precision > precision)
		throw out_of_boud(value + " exceeds the allowable precision.");

	if (__scale > scale)
		throw out_of_boud(value + " exceeds the allowable scale.");
	return value;
}

type_info::type_info() : type_name(), numeric_precision(0), numeric_scale(0), vchar_length(0) {}

struct openDB::sqlType::type_info boolean::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info date::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info time::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info varchar::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	info.vchar_length = length;
	return info;
}

struct openDB::sqlType::type_info character::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.vchar_length = length;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info smallint::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info integer::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info bigint::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info real::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info double_precision::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}

struct openDB::sqlType::type_info numeric::get_type_info() const throw () {
	type_info info;
	info.type_name = type_name;
	info.udt_name = udt_name;
	return info;
}




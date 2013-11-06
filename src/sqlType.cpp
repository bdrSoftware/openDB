/* Copyright 2013 Salvatore Barone <salvator.barone@gmail.com>
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
using namespace openDB;
using namespace sqlType;


const std::string boolean::true_value[] 	= {"TRUE", "t", "true", "y", "yes", "on", "1"};
const std::string boolean::false_value[] = {"FALSE", "f", "false", "n", "no", "off", "0"};

void boolean::validate_value(std::string value) const throw(data_exception&) {
	bool conform = false;
	unsigned i = 0;
	while (i < value_number && !conform)
		if (value == true_value[i] || value == false_value[i])
			conform = true;
		else i++;
	if (!conform)
		throw invalid_argument("Invalid argument for type 'boolean': " + value + " isn't permitted.");
}

std::string boolean::prepare_value(std::string value) const throw () {
	bool _true = false;
	unsigned i = 0;
	while (i < value_number && !_true)
		if (value == true_value[i] )
			_true = true;
		else i++;
	if (_true)
		return "true";
	else
		return "false";
}





void date::validate_value(std::string value) const throw(data_exception&) {
	date_integer _date = convert(value);
	if (!validate(_date))
		throw invalid_date("Invalid date: " + value + " is invalid.");
}

std::string date::prepare_value(std::string value) const throw () {
	date_integer _date = convert(value);
	return "'" + std::to_string(_date.year) + "-" + std::to_string(_date.month) + "-" + std::to_string(_date.day) + "'";
}

date::date_integer date::convert (std::string __value) const throw (data_exception&) {
	std::unique_ptr<std::list<std::string>> token = tokenize(__value, '/');

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





void time::validate_value(std::string value) const throw(data_exception&) {
	time_integer _time = convert(value);
	if (!validate(_time))
		throw invalid_time("Invalid time: " + value + " is invalid.");
}

std::string time::prepare_value(std::string value) const throw () {
	time_integer _time= convert(value);
	return "'" + std::to_string(_time.hour) + ":" + std::to_string(_time.minute) + ":" + (_time.second == 0 ? "00" : std::to_string(_time.second)) + "'";
}

time::time_integer time::convert (std::string __value) const throw (data_exception&) {
	std::unique_ptr<std::list<std::string>> token = tokenize(__value, ':');

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


void smallint::validate_value(std::string value) const throw(data_exception&) {
	int _value;
	try {_value = std::stoi(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for smallint data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for smallint data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for smallint data type.");
}

const long int integer::min = std::numeric_limits<int>::min();	/*	limite superiore del bound dei valori	*/
const long int integer::max = std::numeric_limits<int>::max();	/*	limite inferiore del bound dei valori	*/


void integer::validate_value(std::string value) const throw(data_exception&) {
	long _value;
	try {_value = std::stol(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for integer data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for integer data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for integer data type.");
}

const long long bigint::min = std::numeric_limits<long long>::min();	/*	limite superiore del bound dei valori	*/
const long long bigint::max = std::numeric_limits<long long>::max();	/*	limite inferiore del bound dei valori	*/


void bigint::validate_value(std::string value) const throw(data_exception&) {
	long long _value;
	try {_value = std::stoll(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for bigint data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for bigint data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for bigint data type.");
}

const float real::min = std::numeric_limits<float>::lowest();	/*	limite superiore del bound dei valori	*/
const float real::max = std::numeric_limits<float>::max();		/*	limite inferiore del bound dei valori	*/


void real::validate_value(std::string value) const throw(data_exception&) {
	float _value;
	try {_value = std::stof(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for real data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for real data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for real data type.");
}

const long double double_precision::min = std::numeric_limits<long double>::lowest();		/*	limite superiore del bound dei valori	*/
const long double double_precision::max = std::numeric_limits<long double>::max();		/*	limite inferiore del bound dei valori	*/

void double_precision::validate_value(std::string value) const throw(data_exception&) {
	long double _value;
	try {_value = std::stold(value);}
	catch (std::out_of_range& e) {throw out_of_boud(value + " is out of range for double precision data type.");}
	catch (std::invalid_argument& e) {throw invalid_argument(value + " isn't valid for double precision data type.");}

	if (!(_value >= min && _value <= max))
		throw out_of_boud(value + " is out of range for double precision data type.");
}


void numeric::validate_value(std::string value) const throw(data_exception&) {
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
}








/*
 * CX_InvalidCaseException.hpp
 *
 *  Created on: Jul 17, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_INVALIDCASEEXCEPTION_HPP_
#define CPPN_X_INCLUDE_CX_INVALIDCASEEXCEPTION_HPP_

class InvalidCaseException : public std::exception
{
    std::string text;

public:

    InvalidCaseException(const std::string &_reason){
        text = _reason;
    }

    ~InvalidCaseException() throw (){

    }

    virtual const char* what() const throw(){
        return text.c_str();
    }
};



#endif /* CPPN_X_INCLUDE_CX_INVALIDCASEEXCEPTION_HPP_ */

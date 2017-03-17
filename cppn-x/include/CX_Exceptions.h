/*
 * CX_Exceptions.h
 *
 *  Created on: Apr 11, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_EXCEPTIONS_H_
#define CPPN_X_INCLUDE_CX_EXCEPTIONS_H_

class ModeException : public std::exception
{
    std::string text;

public:

    ModeException(const std::string &_reason){
        text = _reason;
    }

    ~ModeException() throw (){

    }

    virtual const char* what() const throw(){
        return text.c_str();
    }
};



#endif /* CPPN_X_INCLUDE_CX_EXCEPTIONS_H_ */

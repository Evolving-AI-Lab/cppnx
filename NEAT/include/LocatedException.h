#ifndef LOCATEDEXCEPTION_H_INCLUDED
#define LOCATEDEXCEPTION_H_INCLUDED

#include <string>

#define CREATE_LOCATEDEXCEPTION_INFO(X) LocatedException( (X) ,__FILE__,__LINE__)

#define CREATE_PAUSE(X) {cout << X << "\nPress enter to continue" << endl;string line;getline(cin,line);}

class LocatedException : public std::exception
{
    char text[4096];

public:
    LocatedException(const char *_reason,const char *_fileName,const long _lineNumber)
    {
        sprintf(
            text,
            "%s ;\n %s ;\n %ld ;\n",
            _reason,
            _fileName,
            _lineNumber
        );
    }

    LocatedException(const std::string &_reason,const char *_fileName,const long _lineNumber)
    {
        sprintf(
            text,
            "%s ;\n %s ;\n %ld ;\n",
            _reason.c_str(),
            _fileName,
            _lineNumber
        );
    }

    char const* what() const throw()
    {
        return text;
    }
};

#endif // LOCATEDEXCEPTION_H_INCLUDED

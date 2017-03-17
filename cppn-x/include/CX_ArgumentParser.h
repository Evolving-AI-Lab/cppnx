/*
 * CX_ArgumentParser.h
 *
 *  Created on: Mar 28, 2015
 *      Author: Joost Huizinga
 */

#ifndef CPPN_X_INCLUDE_CX_ARGUMENTPARSER_H_
#define CPPN_X_INCLUDE_CX_ARGUMENTPARSER_H_

//Standard includes
#include <map>

//Qt includes
#include <QApplication>
#include <QRegExp>

//static const int zeroOrMore = -2;
//static const int oneOrMore = -1;

static bool shortOption(QString option){
    static QRegExp shortOption("-.");
    return shortOption.exactMatch(option);
}

static bool longOption(QString option){
    static QRegExp longOption("--.*");
    return longOption.exactMatch(option);
}

class Option{
public:
    static const int infinite = -1;

    Option(QString shortName, QString longName, bool positional = false){
        setLongName(longName);
        setShortName(shortName);
        setStore();
        _minNbOfArguments = 0;
        _maxNbOfArguments = infinite;
//        _nbOfArguments = zeroOrMore;
        _append = true;
        _positional = positional;
    }

    Option(QString name, bool positional = false){
        setLongName(name);
        setShortName(name);
        setStore();
        _minNbOfArguments = 0;
        _maxNbOfArguments = infinite;
        _append = true;
        _positional = positional;
    }

    void setLongName(QString longName){
        if(longOption(longName)){
            _longName = longName;
        } else {
            QString candidateName = "--" + longName;
            if(longOption(candidateName)){
                _longName = candidateName;
            }
        }
    }

    void setShortName(QString shortName){
        if(shortOption(shortName)){
            _shortName = shortName;
        } else {
            QString candidateName = "-" + shortName;
            if(shortOption(candidateName)){
                _shortName = candidateName;
            }
        }
    }

    void setStore(){
        if(_longName.size()>2){
            _store = _longName.right(_longName.size()-2);
        } else if(_shortName.size() == 2){
            _store = _shortName[1];
        } else {
            _store = "---Invalid name---";
        }

        _optionNames.push_back(_store.toUpper());
    }

    QString getStore(){
        return _store;
    }

    QString getShortName(){
        return _shortName;
    }

    QString getLongName(){
        return _longName;
    }

    int getMinNbOfArguments(){
        return _minNbOfArguments;
    }

    void setMinNbOfArguments(int minNbOfArguments){
        _minNbOfArguments = minNbOfArguments;
    }

    int getMaxNbOfArguments(){
        return _maxNbOfArguments;
    }

    void setMaxNbOfArguments(int maxNbOfArguments){
        _maxNbOfArguments = maxNbOfArguments;
    }

    bool exceedsMaxNbOfArguments(int nbOfArguments){
        if(_maxNbOfArguments == infinite){
            return false;
        } else {
            return nbOfArguments > _maxNbOfArguments;
        }
    }


    bool getAppend(){
        return _append;
    }

    QStringList getOptionNames(){
        return _optionNames;
    }

    QString getHelp(){
        return _help;
    }

    void setHelp(QString help){
        _help = help;
    }

    bool getPositional(){
        return _positional;
    }

    QString optionName(int i){
        if(i < _optionNames.size()){
            return _optionNames[i];
        } else {
            return _optionNames.back();
        }
    }

private:
    QString _store;
    QString _shortName;
    QString _longName;
    int _minNbOfArguments;
    int _maxNbOfArguments;
    bool _append;
    QStringList _optionNames;
    QString _help;
    bool _positional;
};

class ArgumentParser{
public:
    static const size_t rightAlign = 30;

    ArgumentParser(){
        _nbOfArgumentsProvided = 0;
        addOption("-h", "--help");
        getLastOption()->setMaxNbOfArguments(0);
        getLastOption()->setHelp("show this help message and exit");
        addOption("version");
        getLastOption()->setMaxNbOfArguments(0);
        getLastOption()->setHelp("show program's version number and exit");
        addOption("--");
        getLastOption()->setMaxNbOfArguments(0);
        _version = "unknown";
    }

    ~ArgumentParser(){
        _arguments.clear();
        _shortOptions.clear();
        _longOptions.clear();
        _positionalOptions.clear();
        foreach(Option* option, _options){
            delete option;
        }
        _options.clear();
    }

    void setVersion(QString version){
        _version = version;
    }

    QMap<QString, QStringList> parse(QStringList args){
        QFileInfo info(args[0]);
        _programName = info.baseName();
        args.pop_front(); //We do not care about the name of the application

//        _parsing = "";
//        _nbOfArgumentsProvided = 0;
        _currentOption = 0;
//        _arguments[""] = QStringList();
        _positionalOptionIndex = 0;
        _nbOfPositionalArgumentsProvided = 0;

        foreach(QString argument, args){
            if(shortOption(argument)){
                parseOption(_shortOptions, argument);
            } else if(longOption(argument)){
                parseOption(_longOptions, argument);
            } else if(_currentOption){
                parseArgument(argument);
            } else {
                parsePositionalOption(argument);
            }
        }

        checkNbOfArguments();
        if(_arguments.count("help")){
            printHelp();
        }
        if(_arguments.count("version")){
            printVersion();
        }

        return _arguments;
    }

    void parseArgument(const QString& argument){
        QString store = _currentOption->getStore();
        _arguments[store].push_back(argument);
        ++_nbOfArgumentsProvided;
        if(_currentOption->exceedsMaxNbOfArguments(_nbOfArgumentsProvided)){
            _currentOption = 0;
        }

//        if(_arguments[store].size() > _currentOption->getMaxNbOfArguments()){
//            std::cout << "Too many arguments provided for option " << store.toStdString() << std::endl;
//            printHelp();
//        }
    }

    void parseOption(QMap<QString, Option*>& map, QString& key){
        if(map.count(key) == 1){
            switchCurrentOption(map[key]);
            if(!_currentOption->getAppend() || _arguments.count(_currentOption->getStore()) == 0){
                _arguments[_currentOption->getStore()] = QStringList();
            }
            if(_currentOption->getMaxNbOfArguments() == 0){
                _currentOption = 0;
            }
        } else {
            std::cout << "Invalid option: " << key.toStdString() << std::endl;
            printHelp();
        }
    }

    void parsePositionalOption(QString& argument){
        if(_positionalOptions.size() > _positionalOptionIndex){
            Option* option = _positionalOptions[_positionalOptionIndex];
            _arguments[option->getStore()].push_back(argument);
            ++_nbOfPositionalArgumentsProvided;

            if(option->exceedsMaxNbOfArguments(_nbOfPositionalArgumentsProvided)){
                _nbOfPositionalArgumentsProvided = 0;
                ++_positionalOptionIndex;
            }
        } else {
            std::cout << "Unexpected argument: " << argument.toStdString() << std::endl;
            printHelp();
        }
    }

    void switchCurrentOption(Option* newOption){
        checkNbOfArguments();
        _currentOption = newOption;
        _nbOfArgumentsProvided = 0;
    }

    void addOption(Option* option){
        _options.push_back(option);
        if(option->getPositional()){
            _positionalOptions.push_back(option);
            return;
        }
        if(option->getShortName().size()>0){
            _shortOptions[option->getShortName()] = option;
        }
        if(option->getLongName().size()>0){
            _longOptions[option->getLongName()] = option;
        }
    }

    void addOption(QString name){
        addOption(new Option(name));
    }

    void addOption(QString shortName, QString longName){
        addOption(new Option(shortName, longName));
    }

    void addPositionalOption(QString name){
        addOption(new Option(name, true));
    }

    Option* getLastOption(){
        return _options.back();
    }



    void checkNbOfArguments(){
        if(_currentOption){
            if(_nbOfArgumentsProvided < _currentOption->getMinNbOfArguments()){
                std::cout << "Not enough arguments provided for option " << _currentOption->getStore().toStdString() << "." << std::endl;
                std::cout << "Minimum  number of arguments required is " << _currentOption->getMinNbOfArguments() << std::endl;
                printHelp();
            }
        }
    }

    void checkNbOfPositionalArguments(){
        if(_positionalOptions.size() <= _positionalOptionIndex){
            //We have provided arguments for all positional options, we are good.
            return;
        }

        if(_positionalOptions[_positionalOptionIndex]->getMinNbOfArguments() > _nbOfPositionalArgumentsProvided){
            std::cout << "Not enough positional arguments provided" << std::endl;
            printHelp();
        }
    }

    void printHelpChars(QString string, size_t& charsWritten){
        for(int i=0; i<string.size(); ++i){
            if(string[i] == '\n'){
                charsWritten = 0;
            } else {
                ++charsWritten;
            }
        }
        std::cout << string.toStdString();
//        charsWritten += string.size();
    }

    void printVersion(){
        std::cout << _version.toStdString() << std::endl;
        exit(0);
    }

    void printHelp(int exitStatus = 1){
        //Gather printable options
        QList<Option*> printableOptions;
        QList<Option*> printablePositionalOptions;
        foreach(Option* option, _options){
            if(option->getPositional()){
                if(option->getStore().size() == 0){
                    continue;
                }
                printablePositionalOptions.append(option);
            } else {
                if(option->getShortName().size() == 0 && option->getLongName().size() == 0){
                    continue;
                }
                if(option->getShortName() == "--" && option->getLongName() == "--"){
                    continue;
                }
                printableOptions.append(option);
            }
        }

        //Print usage line
        size_t charsWritten = 0;
        printHelpChars("Usage: " + _programName, charsWritten);
        foreach(Option* option, printablePositionalOptions){
            printOptionArguments(option, charsWritten);
        }
        printHelpChars(" [OPTIONS]\n", charsWritten);

        //Print help
        if(_help.size() > 0){
            printHelpChars("\n", charsWritten);
            printHelpChars(_help, charsWritten);
        }

        //Print positional arguments
        if(!printablePositionalOptions.empty()){
            printHelpChars("\n", charsWritten);
            printHelpChars("Positional arguments:\n", charsWritten);
            foreach(Option* option, printablePositionalOptions){
                printPositionOptionName(option, charsWritten);
                align(charsWritten);
                printHelpChars(option->getHelp(), charsWritten);
                printHelpChars("\n", charsWritten);
            }
        }

        //Print optional arguments
        if(!printableOptions.empty()){
            printHelpChars("\n", charsWritten);
            printHelpChars("Optional arguments:\n", charsWritten);
            foreach(Option* option, printableOptions){
                printOptionName(option, charsWritten);
                printOptionArguments(option, charsWritten);
                align(charsWritten);
                printHelpChars(option->getHelp(), charsWritten);
                printHelpChars("\n", charsWritten);
            }
        }

        exit(exitStatus);
    }

    void align(size_t& charsWritten){
        if(charsWritten > rightAlign){
            printHelpChars("\n", charsWritten);
//            charsWritten = 0;
        }

        while(charsWritten < rightAlign){
            printHelpChars(" ", charsWritten);
        }
    }

    void printPositionOptionName(Option* option, size_t& charsWritten){
        printHelpChars("  ", charsWritten);
        printHelpChars(option->getStore(), charsWritten);
    }


    void printOptionName(Option* option, size_t& charsWritten){
        printHelpChars("  ", charsWritten);
        if(option->getShortName().size() > 0){
            printHelpChars(option->getShortName(), charsWritten);
            if(option->getLongName().size() > 0){
                printHelpChars(", ", charsWritten);
            }
        }
        if(option->getLongName().size() > 0){
            printHelpChars(option->getLongName(), charsWritten);
        }
    }

    void printOptionArguments(Option* option, size_t& charsWritten){
        for(int i=0; i<option->getMinNbOfArguments(); ++i){
            printHelpChars(" " + option->optionName(i), charsWritten);
        }
        if(option->getMaxNbOfArguments() != Option::infinite){
            for(int i=option->getMinNbOfArguments(); i<option->getMaxNbOfArguments(); ++i){
                printHelpChars(" [" + option->optionName(i) + "]", charsWritten);
            }
        } else {
            printHelpChars(" [" + option->getOptionNames().back() + " ...]", charsWritten);
        }
    }

    void printArguments(){
        QList<QString> keys = _arguments.keys();
        foreach(QString key, keys){
            QStringList arguments = _arguments[key];
            std::cout << key.toStdString() << ":";
            foreach(QString argument, arguments){
                std::cout << argument.toStdString() << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    QMap<QString, QStringList> _arguments;
    QMap<QString, Option*> _shortOptions;
    QMap<QString, Option*> _longOptions;
    QVector<Option*> _positionalOptions;
    QVector<Option*> _options;
    Option* _currentOption;
//    QString _parsing;
    QString _help;
    QString _version;
    QString _programName;
    int _nbOfArgumentsProvided;
    int _positionalOptionIndex;
    int _nbOfPositionalArgumentsProvided;
};



#endif /* CPPN_X_INCLUDE_CX_ARGUMENTPARSER_H_ */

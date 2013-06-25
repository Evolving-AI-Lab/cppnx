#include "CE_Window.h"
#include "CE_ActivationFunctions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <QApplication>
#include <QWidget>
#include <QtGui>

#include <zip.h>



int main(int argc, char **argv) {
    //Open the ZIP archive
//    int err = 0;
//    zip *z = zip_open("/Users/joost/firefly.zip", 0, &err);
//
//    //Search for the file of given name
//    const char *name = "a";
//    struct zip_stat st;
//    zip_stat_init(&st);
//    zip_stat(z, name, 0, &st);
//
//    //Alloc memory for its uncompressed contents
//    char *contents = new char[st.size];
//
//    //Read the compressed file
//    zip_file *f = zip_fopen(z, "a", 0);
//    zip_fread(f, contents, st.size);
//    zip_fclose(f);
//
//    std::cout << contents <<std::endl;
//
//    //And close the archive
//    zip_close(z);
//
//	return 0;
	act_functions::initializeActivationFunctions();
    QApplication app(argc, argv);
    Window dialog;

    dialog.show();


    return app.exec();
}

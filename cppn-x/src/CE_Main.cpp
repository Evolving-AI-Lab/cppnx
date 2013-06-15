#include "CE_Window.h"
#include "CE_ActivationFunctions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <QApplication>
#include <QWidget>
#include <QtGui>




//IMPLEMENT_APP(MainApp)


int main(int argc, char **argv) {
//	std::string examinerSourceFile = std::string(__FILE__);
//	size_t found = examinerSourceFile.find_last_of("/\\");
//	if(found == std::string::npos){
//		std::cout << "Sources not correctly compiled, __FILE__ macro failed to provide path to source-file\n Please clean and rebuild the CPPN Examiner." << std::endl;
//		exit(1);
//	}



//
//	std::string root_dir = examinerSourceFile.substr(0,found-4);
//	NEAT::Globals::init(root_dir + "/settings.dat");

	act_functions::initializeActivationFunctions();
    QApplication app(argc, argv);
    Window dialog;
//    dialog.connect(&app, SIGNAL(aboutToQuit()), &dialog, SLOT(onExit()));

    dialog.show();


    return app.exec();



//	ofstream dotFile;
//	dotFile.open("cppn.dot");
//
//	cppn->printDot(dotFile);
//	dotFile.close();
//
//	NEAT::FastNetwork<double> cppn_phen = cppn->spawnFastPhenotypeStack<double> ();
//
//	double res =2;
//	int width = 256;
//	int heigth = 256;
//
//	Bitmap picture(width,heigth);
//
//	for(int updates=1; updates < 40; updates++){
//		//	for(double scale = 1; scale <2; scale+=0.1){
//		//		for(double dscale = 1; dscale <2; dscale+=0.1){
//		double scale=1;
//		double dscale=1.4;
//		for(unsigned long x=0; x<width; x++){
//			for(unsigned long y=0; y<heigth; y++){
//				double xv = (double(x)/(double(width)/res) - (res/2))*scale;
//				double yv = (double(y)/(double(heigth)/res) - (res/2))*scale;
//
//				cppn_phen.reinitialize();
//				cppn_phen.setValue(x_input_id, xv);
//				cppn_phen.setValue(y_input_id, yv);
//				cppn_phen.setValue(bias_input_id, scale);
//				cppn_phen.setValue(d_input_id, sqrt(float(xv*xv+yv*yv))*dscale);
//				cppn_phen.update(updates);
//
//				char grey = char(std::min(abs(cppn_phen.getValue(output_id)), 1.0)*255);
//
//				picture.setPixel(x, (heigth-1)-y, grey, grey, grey);
//
//				//std::cout << x << " " << y << " " << cppn_phen.getValue(output_id) << " " << int(grey) << std::endl;
//
//
//			}
//		}
//		picture.write(boost::lexical_cast<std::string>(scale).substr(0,5)+"d"+boost::lexical_cast<std::string>(dscale).substr(0,5)+"u"+boost::lexical_cast<std::string>(updates)+".bmp");
//		//	}
//		//	}
//	}
}

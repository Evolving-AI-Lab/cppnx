/*
 * CX_Matrix.h
 *
 *  Created on: Jul 30, 2013
 *      Author: joost
 */

#ifndef CX_MATRIX_H_
#define CX_MATRIX_H_

class Matrix {
public:
	Matrix(size_t x, size_t y){
		matrix = new double[x*y];
		width = x;
		height = y;
	}

	~Matrix(){
		delete[] matrix;
	}

	double get(size_t index) const{
		return matrix[index];
	}

	double get(size_t x, size_t y) const{
		return matrix[y*width+x];
	}

	void set(size_t x, size_t y, double value){
		matrix[y*width+x] = value;
	}

	void set(size_t index, double value){
		matrix[index] = value;
	}

	Matrix operator*(const Matrix &other) const{
		Matrix result(width, height);

		for(size_t row =0; row<height; row++){
			for(size_t column =0; column<width; column++){
				double temp = 0;
				for(size_t i=0; i<width; i++){
					temp+=get(i, column)*other.get(row, i);
				}
			}
		}

		return result;
	}

	Matrix operator+(const Matrix &other) const{
		Matrix result(width, height);
		for(size_t i=0; i<width*height; i++){
			result.set(i, matrix[i]+other.matrix[i]);
		}
		return result;
	}

private:
	double* matrix;
	size_t width;
	size_t height;
};


#endif /* CX_MATRIX_H_ */

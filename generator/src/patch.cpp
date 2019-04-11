#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <vector>
#include <algorithm>

#include "patch.h"

using namespace std;

std::vector<int> fillArrayInt(int size, std::ifstream& file) {
	vector<int> result;
	std::string delimiter = ", ";
	std::string line;

	for (int i = 0; i < size; i++) {
		size_t pos = 0;
		std::string token;
		getline(file, line);

		while ((pos = line.find(delimiter)) != std::string::npos) {
			token = line.substr(0, pos);
			int ind = stoi(token);
			result.push_back(ind);
			line.erase(0, pos + delimiter.length());
			//cout << token << "\n";
		}
		int ind = stoi(line);
		result.push_back(ind);
		//cout << line << "\n";
	}
	return result;
}

std::vector<double> fillArrayDouble(int size, std::ifstream& file) {
	vector<double> result;
	std::string delimiter = ", ";
	std::string line;

	for (int i = 0; i < size; i++) {
		size_t pos = 0;
		std::string token;
		getline(file, line);

		//line.erase(std::remove(line.begin(), line.end(), ','), line.end());

		while ((pos = line.find(delimiter)) != std::string::npos) {
			token = line.substr(0, pos);
			double ind = stod(token);
			result.push_back(ind);
			line.erase(0, pos + delimiter.length());
			//cout << token << "\n";
		}
		double ind = stod(line);
		result.push_back(ind);
		//cout << line << "\n";
	}
	return result;
}

void multLinMatrix4x4(double l[4], double m[4][4], double r[4]) {
	for (int i = 0; i < 4; i++) {
		r[i] = 0;
		for(int j = 0; j < 4; j++) {
			r[i] += m[j][i] * l[j];
		}
	}
}

void bezierPoint(double controlPoints[3][4][4], double u, double v, double point[3]) {

	double b[4][4] = {
			{-1, 3, 3, 1},
			{ 3,-6,-3, 0},
			{-3, 3, 0, 0},
			{ 1, 0, 0, 0}
	}; // b == M, M == M^T

	double u2 = u * u;
	double u3 = u * u2;
	double v2 = v * v;
	double v3 = v * v2;
	double us[4] = {u3, u2, u, 1}; // [u^3 u^2 u 1]
	double vs[4] = {v3, v2, v, 1}; // [v^3 v^2 v 1]^T

	double temp1[3][4];
	double temp2[3][4];
	double temp3[3][4];
	for(int i = 0; i < 3; i++) {
		multLinMatrix4x4(us, b, temp1[i]); // [u^3 u^2 u 1] * M
		multLinMatrix4x4(temp1[i], controlPoints[i], temp2[i]); // [u^3 u^2 u 1] * M * P
		multLinMatrix4x4(temp2[i], b, temp3[i]); // [u^3 u^2 u 1] * M * P * M^T
		point[i] = 0;
		for (int j = 0; j < 4; ++j) {
			point[i] += temp3[i][j] * vs[j]; // [u^3 u^2 u 1] * M * P * M^T * [v^3 v^2 v 1]^T
		}
	}
}

void parsePatches(const char* name, const char* cpFile, int tesselations) {
	FILE *fp = fopen(name, "w+");
	std::string line;
	std::ifstream file;
	file.open(cpFile);

	if (file.is_open()) {
		getline(file, line);
		int nPatches = stoi(line);
		vector<int> indexes = fillArrayInt(nPatches, file);

		getline(file, line);
		int nCPoints = stoi(line);
        vector<double> points = fillArrayDouble(nCPoints, file);

		//Coisas
        for (int l = 0; l < nPatches; ++l) {
            int patchOffset = 16 * l;
            double controlPoints[3][4][4];
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 3; ++k) {
                        int indexOffset = patchOffset + (j * 4 + i);
                        // Insere na matrix por coluna
                        controlPoints[k][j][i] = points[indexes[indexOffset] * 3 + k];
                    }
                    // printf("%d %d (%f, %f, %f)\n", l, indexes[j * 4 + i], controlPoints[0][i][j], controlPoints[1][i][j], controlPoints[2][i][j]);
                }
            }
            // Loop que divide as curvas de bezier
			float parts = 1.0f / tesselations;
            for (int ui = 0; ui < tesselations; ++ui) {
				double u = (double) ui / tesselations;
                for (int vi = 0; vi < tesselations; ++vi) {
					double v = (double) vi / tesselations;
					double point[3];
                    bezierPoint(controlPoints, u, v, point); 
                    fprintf(fp,"%f %f %f\n", point[0], point[1], point[2]);
					bezierPoint(controlPoints, (u + parts), (v + parts), point);
					fprintf(fp,"%f %f %f\n", point[0], point[1], point[2]);
					bezierPoint(controlPoints, u, (v + parts), point);
					fprintf(fp,"%f %f %f\n", point[0], point[1], point[2]);
				
					bezierPoint(controlPoints, (u + parts), v, point);
					fprintf(fp,"%f %f %f\n", point[0], point[1], point[2]);
					bezierPoint(controlPoints, (u + parts), (v + parts), point);
					fprintf(fp,"%f %f %f\n", point[0], point[1], point[2]);
					bezierPoint(controlPoints, u, v, point);
					fprintf(fp,"%f %f %f\n", point[0], point[1], point[2]);
                }
            }
        }

		file.close();

	}
	else std::cout << "Unable to open bezier patches file\n";

	if(fp) fclose(fp);
}

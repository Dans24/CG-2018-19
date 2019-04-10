#include "Model3D.h"

using namespace std;


const void Model3D::draw() {
	// TODO: VBOs
	glutSolidTeapot(10);
		/*
	glBegin(GL_TRIANGLES);
		for (int i = 0; i < size; i++) {
			glVertex3f(points[i].x, points[i].y, points[i].z);
		}
	glEnd();
	*/
}

Model3D::Model3D() {
}

int Model3D::parse(const char* filename) {
	string line;
	ifstream file;
	file.open(filename);

	if (file.is_open()) {
		getline(file, line);
		int count = stoi(line);
		points = new Point3D[count];


		for (int i = 0; getline(file, line); i++) {
			Point3D p;

			const string delimiter = " ";

			size_t pos = 0;
			std::string token;

			pos = line.find(delimiter);
			token = line.substr(0, pos);
			p.x = stof(token);
			line.erase(0, pos + delimiter.length());

			pos = line.find(delimiter);
			token = line.substr(0, pos);
			p.y = stof(token);
			line.erase(0, pos + delimiter.length());

			p.z = stof(line);


			points[i] = p;
			size++;
		}
		file.close();
	}
	else fprintf(stderr, "%s: %s\n", strerror(errno), filename);


	file.close();

	return 0;
}

Model3D::~Model3D() {
	delete points;
}


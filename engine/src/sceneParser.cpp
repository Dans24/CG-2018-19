#include "tinyxml2.h"
#include <unordered_map>
#include "Model3D.h"
#include "sceneParser.h"

using namespace tinyxml2;

void loadmodel(const char*s) {
	printf("model: %s\n",s);
}

int sceneParser(char* filename,Scene s) {
	XMLDocument doc;
	XMLError eresult = doc.LoadFile(filename);
	if (eresult != XML_SUCCESS)
	{
		printf("Erro a abrir o fichero");
		return -1;
	}
	XMLNode* n = doc.FirstChild();
	if (!n) {
		printf("Ficheiro não tem nodos");
		return -2;
	}
	
	XMLElement* el = n->ToElement();
	if (strcmp("scene", el->Name())) {
		printf("Wrong file Format");
	}
	XMLElement* child = el->FirstChildElement();
	while (child)
	{
		const char * f = child->Attribute("file");
		if (!s->modelos[f]) {
			Model3D* m = new Model3D(f);
			s->modelos[f] = m;
			loadmodel(f);
		}
		s->objetos.push_back(f);
		child = child->NextSiblingElement();
	}
	return 0;
}
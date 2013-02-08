#include <cassert>
#include <iostream>

#include "Library.hpp"

Library::Library() {
	handle = nullptr;
	loaded = false;
}

Library::Library(const std::string &fileName) {
	handle = nullptr;
	loaded = false;
	setFileName(fileName);
}

Library::~Library() {
	if(loaded)
		dlclose(handle);
}

void Library::setFileName(const std::string &fileName) {
	this->fileName = fileName;
}

bool Library::load() {
	handle = dlopen(fileName.c_str(), RTLD_NOW);
	loaded = handle;
	
	return loaded;
}

bool Library::unload() {
	if(dlclose(handle)) {
		loaded = false;
		return true;
	} else
		return false;
}

void *Library::resolve(const char* symbol) {
	void *sym;

	assert(loaded);
	
	sym = dlsym(handle, symbol);

	if(!sym) {
		unload();
		assert(sym);
	}
	
	// std::cout << "Resolved " << symbol << ": " << fileName << " (" << sym << ")" << std::endl;
	
	return sym;
}

bool Library::isLoaded() const {
	return loaded;
}

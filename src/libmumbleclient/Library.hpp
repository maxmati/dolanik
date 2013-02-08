#ifndef _LIBMUMBLECLIENTLIBRARY_H_
#define _LIBMUMBLECLIENTLIBRARY_H_

#include <string>

#include <dlfcn.h>

class Library {
public:
	Library();
	Library(const std::string &fileName);
	virtual ~Library();
	
	bool load();
	bool unload();
	bool isLoaded() const;
	
	void setFileName(const std::string &fileName);

	void *resolve(const char *symbol);
	
private:
	void *handle;
	std::string fileName;
	bool loaded;
};

#endif

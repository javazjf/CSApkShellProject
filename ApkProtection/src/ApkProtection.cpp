//============================================================================
// Name        : ApkProtection.cpp
// Author      : maxzhang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "utils/ApkFile.h"
using namespace std;

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		puts("pls input apk file");
		return 0;
	}
	string path = string(argv[1]);
	ApkFile apk(path);
	apk.unzip();

	if(apk.getUncompressPath() == ""){
		cout<<"Uncompress APK file error!"<<endl;
		return 0;
	}
	cout<<"Uncompress APK file success!"<<endl;

	apk.decodeAndroidManifest();
	return 0;
}


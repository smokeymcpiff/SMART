/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART Minimizing Autoing Resource Thing (SMART)
 *
 *  SMART is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SMART is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SMART. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ClassLoader.h"
#include "jni.h"
#include "Main.h"
#include <cstring>
#include <iostream>

using namespace std;

#include "classes.data" //Autogen

extern JNIEnv* jre;
jclass* classdefs = 0;

inline void decrypt(char* memblock, long length) {
    int times = 41;
    char rotate[] = {-119, 107, -41, 127, -80, 117, 58, 88, -21, -41, -80, 58, -21, -100, 78};
    int i;
    for (i = 0; i < times * length; i++) {
        memblock[i%length] -= rotate[i%15];
    }
}

void loadClasses() {
    if (!classdefs) {
        classdefs = new jclass[numClasses];
        for (int i = 0; i < (numClasses*3); i += 3) {
            jsize size = *(int*)classes[i+2];
            jbyte* bytes = new jbyte[size];
            cout << "reading";
            memcpy(bytes,classes[i+1],size);
            cout << " decrypting";
            decrypt((char*)bytes, (long)size);
            cout << " defining: ";
            cout << (char*)classes[i] << " = ";
            jclass temp = jre->DefineClass((char*)classes[i], NULL, bytes, size);
            delete bytes;
            cout << (void*)temp << '\n';
            classdefs[i/3] = (jclass) jre->NewGlobalRef(temp);
            jre->DeleteLocalRef(temp);
        }
    }
}

void unloadClasses() {
    if (classdefs) {
        for (int i = 0; i < numClasses; i++) {
            jre->DeleteGlobalRef(classdefs[i]);
        }
        delete classdefs;
        classdefs = 0;
    }
}
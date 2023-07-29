//κώδικα απο https://www.youtube.com/watch?v=greXpRqCTKs&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-&index=5
#pragma once

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID;
	Shader(const char* vertexFile, const char* fragmentFile);

	void Activate();
	void Delete();
};


//κώδικα απο https://www.youtube.com/watch?v=greXpRqCTKs&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-&index=5
#pragma once

#include <glad/glad.h>

class EBO
{
	GLuint ID;
public:
	EBO(GLuint* indices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();
};


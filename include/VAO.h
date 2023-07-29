//κώδικα απο https://www.youtube.com/watch?v=greXpRqCTKs&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-&index=5
#pragma once

#include <glad/glad.h>

#include "VBO.h"

class VAO
{
	GLuint ID;
	public:
		VAO();

		void LinkVBO(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset);
		void Bind();
		void Unbind();
		void Delete();
};


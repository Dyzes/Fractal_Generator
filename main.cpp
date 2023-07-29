#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "ShaderClass.h"

float dt{0}; //μεταβλητή για διαδικασίες που θέλω να μένουν σταθερές στον χρόνο
int width{ 1920 }; //διαστάσεις παραθήρου
int height{ 1080 };
float zoom_level{ 1 };
float speed{ 2.5f };
float pos[]{ -0.75, 0.1 };

//φτιάχνω ένα τετράγωνο προστατευτικά από την οθόνη και το επεξεργάζομαι με shaders.
GLfloat vertices[] = 
{
	 1.0f, 1.0f,
	-1.0f, 1.0f,
	-1.0f,-1.0f,
	 1.0f,-1.0f
};

GLuint indices[] = 
{
	0, 1, 2,
	0, 2, 3
};

//ενημέρωση παραθήρου
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//επεξεργασία των Inputs
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS && zoom_level > 0.1f)
	{
		zoom_level -= speed * dt/4;
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		zoom_level += speed * dt/4;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		pos[1] += dt * speed/ (2*pow(zoom_level, zoom_level));
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		pos[1] -= dt * speed / (2*pow(zoom_level, zoom_level));
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		pos[0] += dt * speed / (2*pow(zoom_level, zoom_level));
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		pos[0] -= dt * speed / (2*pow(zoom_level, zoom_level));
	}
}

int main()
{
	//ανήγω glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//φτιαχνω παράθηρο και τσεκάρω αν όλα πήγαν σωστά
	GLFWwindow* window = glfwCreateWindow(width, height, "Fractal Generator 3.5", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//ανήγω glad και κοιτάω για errors
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//φτιάχνω το viewport και ενημερώνω την οθόνη
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	//φτίαχνω το shaderprogram, το VAO,το VBO, EBO
	Shader shaderProgram("default.vert", "default.frag");
	VAO VAO1;
	VAO1.Bind();
	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));
	VAO1.LinkVBO(VBO1, 0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	//ετειμάζω την επικοινωνία με το fragment shader
	GLuint posID = glGetUniformLocation(shaderProgram.ID, "pos");
	GLuint zoomID = glGetUniformLocation(shaderProgram.ID, "zoom");
	GLuint resID = glGetUniformLocation(shaderProgram.ID, "resolution");
	GLuint upscaleID = glGetUniformLocation(shaderProgram.ID, "upscale");
	GLuint gammaID = glGetUniformLocation(shaderProgram.ID, "gamma");
	GLuint powerID = glGetUniformLocation(shaderProgram.ID, "power");
	GLuint julisID = glGetUniformLocation(shaderProgram.ID, "julius_status");
	GLuint cID = glGetUniformLocation(shaderProgram.ID, "inpt_c");
	GLuint color_insideID = glGetUniformLocation(shaderProgram.ID, "color_inside");

	//ανήγω imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	//ετοιμάζω τις μεταβλητές που θα χρειαστώ
	float p_time{glfwGetTime()};
	int c_frame_rate {0};
	bool julius{false};
	bool animate_c{false};
	bool only_boundary{true};
	bool vs{true};
	int quality{ 50 };
	float brt{ 2.75f };
	float power[]{ 2,0 };
	float c_x {0.0};
	float c_y {0.0};
	float pow_animation_speed[]{0,0};
	float c_animation_speed{1};
	float c_angle{0};

	//το render loop
	while (!glfwWindowShouldClose(window))
	{
		//όλα σχετικά με τον χρόνο ανάμεσα σε frames
		float c_time = glfwGetTime();
		dt = c_time - p_time;
		if(static_cast<int>(glfwGetTime()*100) % 10 == 0)
		{
			c_frame_rate = static_cast<int>(1/dt);
		}

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);//οριζμώ το background χρώμα
		glClear(GL_COLOR_BUFFER_BIT); //καθαρίζει το color buffer

		//ενημερώνω το imgui ότι αρχίζω καινούργιο frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		//ζωγραφίζω το τετράγωνο
		shaderProgram.Activate();
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		//όλα τα GUI που θα χρειαστώ
		ImGui::Begin("Settings");
		ImGui::Text("Frame rate: %d", c_frame_rate);
		ImGui::Checkbox("VSync", &vs);
		if(!vs)
		{
			glfwSwapInterval(0);
		}
		else
		{
			glfwSwapInterval(1);
		}
		ImGui::Checkbox("Color Only Boundery", &only_boundary);
		ImGui::InputFloat2("Position (wasd on keyboars)", &pos[0] , "%.3f", 0);
		ImGui::InputFloat("Zoom (- and + on keyboard)", &zoom_level, 0.1, 1, "%.2f", 0);
		ImGui::SliderInt("Quality (Iterations)", &quality, 1, 500);
		ImGui::SliderFloat("Brightness", &brt, 0.0f, 20.0f);
		ImGui::SliderFloat("Speed (zoom and movement)", &speed, 0.5f, 10.0f);
		ImGui::InputFloat2("Power", &power[0], "%.3f", 0);
		ImGui::InputFloat2("Power Animation Speed", &pow_animation_speed[0], "%.3f", 0);
		power[0] += dt*pow_animation_speed[0]/4;
		power[1] += dt*pow_animation_speed[1]/4;
		ImGui::Checkbox("Julius Set", &julius);
		if (julius)
		{
			ImGui::InputFloat("Real C", &c_x, 0.05, 0.5, "%.4f", 0);
			ImGui::InputFloat("Imgagounary C", &c_y, 0.05, 0.5, "%.4f", 0);
			ImGui::Checkbox("Animate C", &animate_c);
			if (animate_c) //e^(it)
			{
				ImGui::SliderFloat("Animation Speed", &c_animation_speed, -2.0f, 2.0f);
				c_angle += c_animation_speed *dt/2;
				c_x = cos(c_angle);
				c_y = sin(c_angle);
			}
		}
		ImGui::End();

		//τελικές εντολές για το GUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		processInput(window);

		//επικοινωνία με το fragment shader
        glfwGetFramebufferSize(window, &width, &height);
		glUniform2f(resID, width, height);
		glUniform2f(posID, pos[0], pos[1]);
		glUniform1f(gammaID, 1/brt);
		glUniform1i(upscaleID, quality);
		glUniform2f(powerID, power[0], power[1]);
		glUniform1f(julisID, julius);
		glUniform2f(cID, c_x, c_y);
		glUniform1i(color_insideID, !only_boundary);
		if (zoom_level > 1) // οποιό smooth τρόπος που βρήκα
		{
			glUniform1f(zoomID, pow(zoom_level, zoom_level));
		}
		else
		{
			glUniform1f(zoomID, zoom_level);
		}

		glfwSwapBuffers(window);//αλλαγή buffer
		glfwPollEvents();//επεξεργασία <<γεγονότων>>
		p_time = c_time;
	}

	//τελική δήλωση 
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


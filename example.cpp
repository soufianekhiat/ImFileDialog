//#include <SDL2/SDL.h>
//#include <imgui.h>
//#include <backends/imgui_impl_sdl.h>
//#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <time.h>

//#ifdef _WIN32
//#include <windows.h>
//#endif
//
//#include <GL/glew.h>
//
//#if defined(__APPLE__)
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif
//#pragma comment(lib, "opengl32.lib")
//
#define TINYEXR_IMPLEMENTATION
#include "ImFileDialog.h"
//
//// SDL defines main
//#undef main

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int argc, char* argv[])
{
	bool run = true;
	srand(time(NULL));
	
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	int wndWidth = 1200, wndHeight = 720;
	GLFWwindow* window = glfwCreateWindow(wndWidth, wndHeight, "File Dialog", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	//
	//// init glew
	//glewExperimental = true;
	//if (glewInit() != GLEW_OK) {
	//	printf("Failed to initialize GLEW\n");
	//	return 0;
	//}

	// imgui
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//ImGui::StyleColorsLight();

	//ImGui_ImplSDL2_InitForOpenGL(wnd, glContext);
	//ImGui_ImplOpenGL3_Init();

	// ImFileDialog requires you to set the CreateTexture and DeleteTexture
	ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt, int channel, bool isFloat) -> void* {
		GLuint tex;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		//if (!isFloat)
		//{
			//if (fmt == 0)
			//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
			//else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, isFloat ? GL_FLOAT : GL_UNSIGNED_BYTE, data);
		//}
		//else
		//{
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, isFloat ? GL_UNSIGNED_BYTE, data);
		//}
		//glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return (void*)tex;
	};
	ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
		GLuint texID = (GLuint)((uintptr_t)tex);
		glDeleteTextures(1, &texID);
	};


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (!run) break;

		// Simple window
		ImGui::Begin("Control Panel");
		if (ImGui::Button("Open file"))
			ifd::FileDialog::Instance().Open("ShaderOpenDialog", "Open a shader", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga,.exr},.*", true);
		if (ImGui::Button("Open directory"))
			ifd::FileDialog::Instance().Open("DirectoryOpenDialog", "Open a directory", "");
		if (ImGui::Button("Save file"))
			ifd::FileDialog::Instance().Save("ShaderSaveDialog", "Save a shader", "*.sprj {.sprj}");
		ImGui::End();

		// file dialogs
		if (ifd::FileDialog::Instance().IsDone("ShaderOpenDialog")) {
			if (ifd::FileDialog::Instance().HasResult()) {
				const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
				for (const auto& r : res) // ShaderOpenDialog supports multiselection
					printf("OPEN[%s]\n", r.u8string().c_str());
			}
			ifd::FileDialog::Instance().Close();
		}
		if (ifd::FileDialog::Instance().IsDone("DirectoryOpenDialog")) {
			if (ifd::FileDialog::Instance().HasResult()) {
				std::string res = ifd::FileDialog::Instance().GetResult().u8string();
				printf("DIRECTORY[%s]\n", res.c_str());
			}
			ifd::FileDialog::Instance().Close();
		}
		if (ifd::FileDialog::Instance().IsDone("ShaderSaveDialog")) {
			if (ifd::FileDialog::Instance().HasResult()) {
				std::string res = ifd::FileDialog::Instance().GetResult().u8string();
				printf("SAVE[%s]\n", res.c_str());
			}
			ifd::FileDialog::Instance().Close();
		}

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, wndWidth, wndHeight);

		//int display_w, display_h;
		//glfwGetFramebufferSize(window, &display_w, &display_h);
		//glViewport(0, 0, display_w, display_h);
		//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, wndWidth, wndHeight);

		// render Dear ImGui
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

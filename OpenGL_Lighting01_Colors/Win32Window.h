
//#define GLEW_STATIC
//#include <GL\glew.h>
//
//#include <GLFW\glfw3.h>


#ifndef WIN32WINDOW_H
#define WIN32WINDOW_H

class Win32Window
{
public:
	Win32Window() {}

	/*
	*  This function creates a window and its associated OpenGL context and set it as the current context
	*  then returns a pointer to the created window.
	*  If the creation fails, then the returned value would be nullptr
	*
	*  @param[in] width The desired width, in screen coordinates, of the window.
	*  This must be greater than zero.
	*  @param[in] height The desired height, in screen coordinates, of the window.
	*  This must be greater than zero.
	*  @param[in] title The initial, UTF-8 encoded window title.
	*/
	static GLFWwindow *CreateWindow(GLint width, GLint height, const char* title);

	/*
	*  This function sets up glew and starts off OpenGL game loop.
	*  then breaks up when the desired event or key is thrown
	*
	*  @param[in] The desired window context.
	*/
	static void Run(GLFWwindow* window);

	// Returns the actual or the accurate representation of the 
	// window width relative to the screen
	static GLint GetScreenWidth();
	// Returns the actual or the accurate representation of the 
	// window height relative to the screen
	static GLint GetScreenHeight();

private:
	// The actual window dimensions relative to our screen
	static GLint m_ScreenWidth, m_ScreenHeight;
	static const char* m_Title;
};
#endif // !WIN32WINDOW_H


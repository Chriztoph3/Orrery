#include "RendererUtility.h"
#include "CommonIncludes.h"

using namespace std;
using namespace glm;

void RendererUtility::QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool RendererUtility::CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

void RendererUtility::ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

//Load shader sources, compile and create shader objects
bool RendererUtility::InitializeShader(Shader *shader, const string &vertSource, const string &fragSource)
{
	// load shader source from files
	string vertexSource = LoadSource(vertSource);
	string fragmentSource = LoadSource(fragSource);
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	shader->program = LinkProgram(shader->vertex, shader->fragment);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

//Intitialize and activate buffers for geometry, returning true if successful
bool RendererUtility::InitializeGeometry(Geometry *geometry)
{
	// create a vertex array object encapsulating all vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// create an array buffer object for storing positions
	glGenBuffers(1, &geometry->positionBuffer);

	// create another one for storing colors
	glGenBuffers(1, &geometry->colorBuffer);

	// create another one for storing normals
	glGenBuffers(1, &geometry->normalBuffer);

	// create another one for storing texcoords
	glGenBuffers(1, &geometry->texCoordBuffer);

	// create another for storing indices
	glGenBuffers(1, &geometry->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->indexBuffer);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// check for OpenGL errors and return false if error occurred
	return !RendererUtility::CheckGLErrors();
}

// reads a text file with the given name into a string
string RendererUtility::LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint RendererUtility::CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint RendererUtility::LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}
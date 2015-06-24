#include <shader.h>

//shader used is determined by what is passed in here.


shader::shader(GLuint Shader)
{
   source = Shader;
   std::cout << "Shader: "<< Shader << std::endl;
}

// opens a shader
bool shader::open(std::string filename)
{
   int l2;
   name = filename;
   std::string filestring (std::istreambuf_iterator<char>(std::ifstream(filename).rdbuf()), std::istreambuf_iterator<char>());
   std::cout << "SHADER: "<< filename << std::endl;
   std::string str = filestring;
   char writable[ str.size() + 1];
   std::copy(str.begin(), str.end(), writable);
   writable[str.size()] = '\0'; // don't forget the terminating 0

   const char* shader = &writable[0];
   glShaderSource(source, 1, &shader, NULL);
   glGetShaderiv(source, GL_SHADER_SOURCE_LENGTH, &l2);
   std::cout << "Length of shader" << l2 << std::endl;
   return filestring.compare("");
}

// compiles shader
bool shader::compile()
{
   
   glCompileShader(source);
   GLint shader_status = 0;
   glGetShaderiv(source,GL_COMPILE_STATUS, &shader_status);
   char logs[1000];
   int length=1000;
   
   //glGetShaderiv(source, GL_SHADER_SOURCE_LENGTH, &l2);
   if(!shader_status)
   {
      std::cerr << "[F] FAILED TO COMPILE SHADER!" << std::endl;
      glGetShaderInfoLog(source, 1000, &length, &logs[0]);
      std::cerr <<"REASON: "<< logs << std::endl;
      return false;
   }
   return true;
}

//links shader
bool shader::link(GLuint& program)
{
   std::cout << "SOURCE: "<< source << std::endl;
   GLint shader_status = 0;
   glAttachShader(program,source);
   //glLinkProgram(program);

   return true;
}

void shader::cleanup()
{
   glDeleteShader(source);
}

const GLuint shader::getSource() const
{
   const GLuint r = source;
   return r;
}

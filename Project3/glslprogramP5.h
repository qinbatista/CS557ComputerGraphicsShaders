#ifndef GLSLPROGRAM_P5_H
#define GLSLPROGRAM_P5_H

#include <ctype.h>
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

// if on a Mac, don't need glew.h and the <GL/ should be <OpenGL/

//#include "glew.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "glut.h"
#include <map>
#include <stdarg.h>

inline int GetOSU( int flag )
{
	int i;
	glGetIntegerv( flag, &i );
	return i;
}


void	CheckGlErrors( const char* );



class GLSLProgram
{
  private:
	std::map<char *, int>	AttributeLocs;
	char *			Ffile;
	unsigned int		Fshader;
	bool			IncludeGstap;
	GLuint			Program;
	std::map<char *, int>	UniformLocs;
	bool			Valid;
	char *			Vfile;
	GLuint			Vshader;
	bool			Verbose;

	static int		CurrentProgram;

	void	AttachShader( GLuint );
	bool	CanDoFragmentShaders;
	bool	CanDoVertexShaders;
	int	CompileShader( GLuint );
	bool	CreateHelper( char *, ... );
	int	GetAttributeLocation( char * );
	int	GetUniformLocation( char * );


  public:
		GLSLProgram( );

	bool	Create( char *, char * = NULL, char * = NULL, char * = NULL, char * = NULL, char * = NULL );
	void	DisableVertexAttribArray( const char * );
	void	EnableVertexAttribArray( const char * );
	bool	IsExtensionSupported( const char * );
	bool	IsNotValid( );
	bool	IsValid( );
	void	SetAttributePointer3fv( char *, float * );
	void	SetAttributeVariable( char *, int );
	void	SetAttributeVariable( char *, float );
	void	SetAttributeVariable( char *, float, float, float );
	void	SetAttributeVariable( char *, float[3] );
	void	VertexAttrib3f( const char *, float, float, float );
	void	SetUniformVariable( char *, int );
	void	SetUniformVariable( char *, float );
	void	SetUniformVariable( char *, float, float, float );
	void	SetUniformVariable( char *, float[3] );
	void	SetVerbose( bool );
	void	UnUse( );
	void	Use( );
	void	Use( GLuint );
	void	UseFixedFunction( );
};

#endif		// #ifndef GLSLPROGRAMP5_H

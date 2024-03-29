/**
 * @file gui.h
 * @author Erik Sandrén
 * @date 14-08-2016
 * @brief GUI or top left hud
 */

#ifndef __GUI_H__
#define __GUI_H__

#include "glm/glm.hpp"
#include <GL/glew.h>

#include "shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

struct Character {
	GLuint textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLint64 advance;
};

class GUI {
public:
	GUI();
	bool initialize();
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale,
									glm::vec3 color = glm::vec3(1.0, 1.0, 1.0));

private:
	void loadCharacters();
	float mFontSize;
	FT_Library mLibrary;
	FT_Face mFace;
	std::map<GLchar, Character> mCharacters;
	GLuint mVAO;
	GLuint mVBO;
};

#endif

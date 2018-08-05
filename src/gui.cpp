/**
* @file gui.cpp
* @author Erik Sandrén
* @date 14-08-2016
* @brief [Description Goes Here]
*/



#include <GL/glew.h>
#include "glm/gtc/matrix_transform.hpp"

#include "gui.h"
#include "shaderManager.h"

GUI::GUI(): mFontSize(22.0f) { }

bool GUI::initialize() {
	if(FT_Init_FreeType(&mLibrary))
		return false;

	if(FT_New_Face(mLibrary, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0, &mFace))
		return false;

	FT_Set_Pixel_Sizes(mFace, 0, mFontSize);

	this->loadCharacters();

	Shader* guiVS = new Shader("../shaders/font.vert", GL_VERTEX_SHADER);
	Shader* guiFS = new Shader("../shaders/font.frag", GL_FRAGMENT_SHADER);

	Program* guiProgram = new Program("font");

	guiProgram->attach(guiVS);
	guiProgram->attach(guiFS);
	guiProgram->linkProgram();
	ShaderManager::getInstance()->addShader(guiProgram);

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void GUI::loadCharacters() {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for(GLubyte c = 0; c < 128; c++) {
		FT_Load_Char(mFace, c, FT_LOAD_RENDER);

		GLuint texture;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				mFace->glyph->bitmap.width,
				mFace->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				mFace->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = {
			texture,
			glm::ivec2(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows),
			glm::ivec2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
			mFace->glyph->advance.x
		};
		mCharacters.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(mFace);
	FT_Done_FreeType(mLibrary);
}

void GUI::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {

	Program* guiShader = ShaderManager::getInstance()->getShader("font");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);

	glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);

	guiShader->useProgram();
	glUniform3f(glGetUniformLocation(guiShader->getProgram(), "textColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(guiShader->getProgram(), "projection"), 1, GL_FALSE, &projection[0][0]);
	glBindVertexArray(mVAO);

	std::string::const_iterator c;

	for (c = text.begin(); c != text.end(); c++) {
		Character ch = mCharacters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;

		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos, ypos,			0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },

			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 0.0 },
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x +=(ch.advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

#pragma once
#include <GL/glew.h>

#include <vector>
#include <memory>
#include <string>

struct VertexBuffer;

struct VertexArray
{
	public:
		VertexArray();
		VertexArray(std::string path);
		void setBuffer(int location, std::shared_ptr<VertexBuffer> buffer);
		GLuint getId();
		size_t getVertCount();

	private:
		std::vector<std::shared_ptr<VertexBuffer> > buffers;
		bool dirty;
		GLuint id;
		size_t vertCount;
};
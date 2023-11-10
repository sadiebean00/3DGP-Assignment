/******************************************************
	 * Class for Vertex Array
	 * This is where all of the vertex buffers are bound together
	 ******************************************************/
#include <VertexArray.h>
#include <VertexBuffer.h>
#include <bugl.h>

#include <exception>

/******************************************************
	 * Create the load path for the model
	 ******************************************************/
VertexArray::VertexArray(std::string path)
{
	id = buLoadModel(path, &vertCount);
	dirty = false;
}
/******************************************************
	 * Resize the buffers and the vertices count
	 ******************************************************/
VertexArray::VertexArray()
{
	glGenVertexArrays(1, &id);

	if (!id)
	{
		throw std::exception();
	}

	dirty = true;
	buffers.resize(20);
	vertCount = 0;
}
/******************************************************
	 * Get the vertices count for the model loaded
	 ******************************************************/
size_t VertexArray::getVertCount()
{
	return vertCount;
}
/******************************************************
	 * Set the buffer from within the location told
	 ******************************************************/
void VertexArray::setBuffer(int location, std::shared_ptr<VertexBuffer> buffer)
{
	buffers.at(location) = buffer;
	dirty = true;
}
/******************************************************
	 * Get the ID for the vertex array with all components
	 ******************************************************/
GLuint VertexArray::getId()
{
	if (dirty)
	{
		glBindVertexArray(id);

		for (size_t i = 0; i < buffers.size(); i++)
		{
			if (!buffers.at(i)) continue;

			glBindBuffer(GL_ARRAY_BUFFER, buffers.at(i)->getId());

			glVertexAttribPointer(i, buffers.at(i)->getComponents(), GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(i);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		glBindVertexArray(0);
		dirty = false;
	}
	return id;
}
/******************************************************
	 * Class for the Vertex Buffer
	 * Stores all of the data, inc. points of vertexes
	 * Much more efficient
	 ******************************************************/
#include <VertexBuffer.h>

#include <exception>
/******************************************************
	 * Checks errors for the vertex buffer
	 ******************************************************/
VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &id);

	if (!id)
	{
		throw std::exception();
	}

	dirty = true;
	components = 0;
}
/******************************************************
	 * Cleans up the vertex buffer
	 ******************************************************/
VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &id);
}
/******************************************************
	 * Gets the components for the vertex buffer
	 ******************************************************/
int VertexBuffer::getComponents()
{
	return components;
}
/******************************************************
	 * Adds data as a push back to the vertex buffer as a vec3
	 ******************************************************/
void VertexBuffer::add(glm::vec3 value)
{
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);

	components = 3;
	dirty = true;
}
/******************************************************
	 * Adds data as a push back to the vertex buffer as a vec4
	 ******************************************************/
void VertexBuffer::add(glm::vec4 value)
{
	data.push_back(value.x);
	data.push_back(value.y);
	data.push_back(value.z);
	data.push_back(value.w);

	components = 4;
	dirty = true;
}
/******************************************************
	 * Adds data as a push back to the vertex buffer as a vec2
	 ******************************************************/
void VertexBuffer::add(glm::vec2 value)
{
	data.push_back(value.x);
	data.push_back(value.y);

	components = 2;
	dirty = true;
}
/******************************************************
	 * Gets the ID for the vertex buffer
	 ******************************************************/
GLuint VertexBuffer::getId()
{
	if (dirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, id);

		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data.at(0)), &data.at(0), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		dirty = false;
	}

	return id;
}
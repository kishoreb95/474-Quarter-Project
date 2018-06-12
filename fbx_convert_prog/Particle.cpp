#include "GLSL.h"
#include <iostream>
#include <glad/glad.h>
#include "Program.h"
#include "Particle.h"
using namespace std;
using namespace glm;
ParticleGenerator::ParticleGenerator(shared_ptr<Program> prog)
{
	program = prog;
	init();
}


int ParticleGenerator::firstUnusedParticle()
{
	//Shortcut to speed it up
	for (int i = lastUsedParticle; i < particles.size(); i++)
	{
		if (particles[i].life <= 0.001f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (particles[i].life <= 0.001f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, vec2 pos, glm::vec2 offset)
{
	vec2 random = vec2( (rand() % 100 - 50) / 10.0f, (rand() % 100 - 50) / 10.0f);
	float bright = 0.5 + ((rand() % 100) / 100.0f);
	particle.pos = pos + (0.01f * random) + offset;
	particle.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);// glm::vec4(bright, bright, bright, 1.0f);
	particle.life = 1.0f;
	particle.vel = 0.00025f * random + offset;
}

void ParticleGenerator::Update(vec2 pos, int newParticles, float dt, glm::vec2 offset)
{
	// Add new particles
	for (int i = 0; i < newParticles; i++)
	{
		int unusedParticle = firstUnusedParticle();
		respawnParticle(particles[unusedParticle], pos, offset);
	}
	// Update all particles
	for (int i = 0; i < num; i++)
	{
		Particle &p = particles[i];
		p.life -= dt; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.pos -= p.vel * dt;
			p.color.a -= dt * 2.5;
		}
	}
}

// Render all particles
void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindVertexArray(VAO);
	program->bind();
	for (Particle particle : this->particles)
	{
		if (particle.life > 0.0f)
		{
			glUniform2fv(program->getUniform("offset"), 1, (GLfloat *)(&particle.pos));
			glUniform4fv(program->getUniform("color"), 1, (GLfloat *)(&particle.color));

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	program->unbind();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < num; ++i)
		this->particles.push_back(Particle());
}
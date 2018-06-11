#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <memory>
#include "Program.h"

#include <glm/glm.hpp>

using namespace glm;
//One particle
struct Particle {
	vec2 pos, vel;
	vec4 color;
	float life;

	Particle() : pos(0.0f), vel(0.0f), color(1.0f), life(0.0f) { }
};

// Container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
	// Constructor
	ParticleGenerator(std::shared_ptr<Program> prog);
	// Update all particles
	void Update(float dt, vec2 pos, int newParticles, vec2 offset = vec2(0.0f, 0.0f));
	// Render all particles
	void Draw();
	std::vector<Particle> particles;
	int num = 500;
private:
	int lastUsedParticle = 0;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	int firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle, vec2 pos, vec2 offset = vec2(0.0f, 0.0f));
	std::shared_ptr<Program> program;
	GLuint VAO;
};
#endif
//
//  Noise.h
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/19/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//


#pragma once

#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <vector>

class PerlinNoise {
	// The permutation vector
	std::vector<int> p;
public:
	// Initialize with the reference values for the permutation vector
	PerlinNoise();

	// Generate a new permutation vector based on the value of seed
	PerlinNoise(unsigned int seed);

	// Get a noise value, for 2D images z can have any value
	double noise(double x, double y, double z);

private:
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);

};

#endif
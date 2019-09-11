#include "crossover.h"

#include <cstdlib>
#include <vector>
#include <algorithm>

#include <iostream>

Network DoCrossover(NEAT &neat, Network &a, Network &b)
{
	int maxInnovationNumber = -1;

	Network result = neat.CreateStandardNetwork();
	result.GetGenome()->connectionGenes.clear();

	for (auto g : a.GetGenome()->connectionGenes)
		if (g.innovation > maxInnovationNumber)
			maxInnovationNumber = g.innovation;

	for (auto g : b.GetGenome()->connectionGenes)
		if (g.innovation > maxInnovationNumber)
			maxInnovationNumber = g.innovation;

	if (maxInnovationNumber == -1)
		return result;

	for (int i = 0; i <= maxInnovationNumber; i++)
	{
		bool aHasGene = false;
		Connection aGene;
		bool bHasGene = false;
		Connection bGene;

		for (auto g : b.GetGenome()->connectionGenes)
		{
			if (g.innovation == i)
			{
				bHasGene = true;
				bGene = g;
				break;
			}
		}

		for (auto g : a.GetGenome()->connectionGenes)
		{
			if (g.innovation == i)
			{
				aHasGene = true;
				aGene = g;
				break;
			}
		}

		if (aHasGene && bHasGene)
		{
			Connection connection = rand() % 2 ? aGene : bGene;
			connection.enabled = rand() % 4 ? connection.enabled : true; //specified chance of enabling connection otherwise inherit if enabled

			std::cout << "Innovation: " << i << "; Selected gene: " << connection << std::endl;

			result.GetGenome()->connectionGenes.push_back(connection);
		}
		else if (a.GetFitness() == b.GetFitness())
		{
			if (rand() % 2)
			{
				Connection connection = aHasGene ? aGene : bGene;
				connection.enabled = rand() % 4 ? connection.enabled : true; //specified chance of enabling connection otherwise inherit if enabled
				result.GetGenome()->connectionGenes.push_back(connection);
			}
		}
		else if (a.GetFitness() > b.GetFitness())
		{
			if (aHasGene)
			{
				Connection connection = aGene;
				connection.enabled = rand() % 4 ? connection.enabled : true; //specified chance of enabling connection otherwise inherit if enabled
				result.GetGenome()->connectionGenes.push_back(connection);
			}
		}
		else if (a.GetFitness() < b.GetFitness())
		{
			if (bHasGene)
			{
				Connection connection = bGene;
				connection.enabled = rand() % 4 ? connection.enabled : true; //specified chance of enabling connection otherwise inherit if enabled
				result.GetGenome()->connectionGenes.push_back(connection);
			}
		}
	}

	std::vector<Node> nodes = DeduceNodesFromConnections(*result.GetGenome());

	result.GetGenome()->nodeGenes = nodes;

	return result;
}

float DistanceFunction(NEAT &neat, Network &a, Network &b)
{
	int maxInnovationA = -1;
	int maxInnovationB = -1;
	int maxInnovation = -1;

	for (auto g : a.GetGenome()->connectionGenes)
		if (g.innovation > maxInnovationA)
			maxInnovationA = g.innovation;

	for (auto g : b.GetGenome()->connectionGenes)
		if (g.innovation > maxInnovationB)
			maxInnovationB = g.innovation;

	if (maxInnovationB == -1 && maxInnovationA == -1)
		return 0;

	maxInnovation = maxInnovationA > maxInnovationB ? maxInnovationA : maxInnovationB;

	int numberOfGenes = maxInnovationB > maxInnovationA ? (int)b.GetGenome()->connectionGenes.size() : (int)a.GetGenome()->connectionGenes.size();
	if (numberOfGenes < neat.nThreshold)
		numberOfGenes = 1;

	int excess = 0;
	int disjoint = 0;
	float sharedweightSum = 0;
	int sharedWeightNum = 0;

	for (int i = 0; i <= maxInnovation; i++)
	{
		bool aHasGene = false;
		Connection aGene;
		bool bHasGene = false;
		Connection bGene;

		for (auto g : b.GetGenome()->connectionGenes)
			if (g.innovation == i)
			{
				bHasGene = true;
				bGene = g;
				break;
			}

		for (auto g : a.GetGenome()->connectionGenes)
			if (g.innovation == i)
			{
				aHasGene = true;
				aGene = g;
				break;
			}

		if (aHasGene && bHasGene)
		{
			sharedWeightNum++;
			sharedweightSum += (aGene.weight - bGene.weight);
		}
		else
		{
			if (aHasGene)
			{
				if (maxInnovationB < maxInnovationA)
					excess++;
				else
					disjoint++;
			}
			else
			{
				if (maxInnovationA < maxInnovationB)
					excess++;
				else
					disjoint++;
			}
		}
	}

	return (neat.c1 * excess) / numberOfGenes + (neat.c2 * disjoint) / numberOfGenes + neat.c3 * (sharedweightSum / sharedWeightNum);
}
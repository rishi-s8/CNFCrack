#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

inline int var(int lit)
{
	return (lit > 0) ? lit : -lit;
}

using namespace std;

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	os << "[";
	for (auto it = v.begin(); it != v.end(); ++it) {
		if (it != v.begin()) os << ", ";
		os << *it;
	}
	return os << "]";
}

std::vector<std::vector<int>> parse_dimacs(std::string filename) {
	std::vector<std::vector<int>> clauses;
	std::ifstream in(filename);
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		if (line == "") continue;
		if (line.front() == 'c') continue;
		if (line.front() == 'p') continue;
		clauses.emplace_back();
		const char* begin = line.c_str();
		char* end = nullptr;
		while (true) {
			auto id = std::strtoll(begin, &end, 10);
			begin = end;
			if (id == 0) break;
			clauses.back().emplace_back(id);
		}
		if (clauses.back().empty()) clauses.pop_back();
	}
	return clauses;
}

class trailElement{
	public:
	int assignment;
	bool flipped;	
	bool decision;
	trailElement(int assign)
	{
		assignment = assign;
		decision = true;
		flipped = false;
	}
	trailElement(int assign, bool dec)
	{
		assignment = assign;
		decision = dec;
		flipped = false;
	}
};
using Satisfiability =  const std::string;

int nVars, nClauses;
stack<trailElement> trail;
vector<int> Assignments;
std::vector<std::vector<int> > clauses;
Satisfiability SAT = "sat";
Satisfiability UNSAT = "unsat";
int decideVar;

bool isConflict()
{
	for(auto clause: clauses)
	{
		int nFalse = 0;
		for(auto literal: clause)
			if(Assignments[var(literal)]==-literal)
				++nFalse;
		if(nFalse == clause.size())
		{
			// cout << "Reached Conflict.." << endl;
			// cout << vector<vector<int>>(1, clause) << endl;
			return true;
		}
	}
	return false;
}

bool backtrack()
{
	// cout << "Backtracking.." << endl;
	while(!trail.empty())
	{
		trailElement current = trail.top();
		if(current.decision && !current.flipped)
		{
			// cout << "Flipping Decision to: " << -current.assignment << endl;
			decideVar = var(current.assignment);
			current.assignment = -current.assignment;
			current.flipped = true;
			Assignments[var(current.assignment)] = current.assignment;
			trail.pop();
			trail.push(current);
			return true;
		}
		else
		{
			// cout << "Unassigning: " << current.assignment << endl;
			Assignments[var(current.assignment)] = 0;
			trail.pop();
		}
	}
	// cout << "Decisions could\'nt be flipped.. UNSAT" << endl;
	return false;
}

bool BCP()
{
	for(int i=0; i<clauses.size(); ++i)
	{
		vector<int> &clause = clauses[i];
		bool alreadyTrue = false;
		int nFalse = 0, unassignedVar = 0;
		for(auto literal: clause)
			if(Assignments[var(literal)]==-literal) ++nFalse;
			else if(!Assignments[var(literal)])
				unassignedVar = literal;
			else
			{
				alreadyTrue = true;
				break;
			}
			
		if(!alreadyTrue && clause.size()-nFalse==1)
		{
			// cout << "Propagating in clause: " << i << " var: " << unassignedVar << endl;
			Assignments[var(unassignedVar)] = unassignedVar;
			trail.push(trailElement(unassignedVar, false));
			if(isConflict())
				return false;
			i=-1;
		}
	}
	return true;
}

bool decide()
{
	while(decideVar<=nVars && Assignments[decideVar]) ++decideVar;
	if(decideVar>nVars) return false;
	// cout << "Deciding: " << -decideVar << endl;
	trail.push(trailElement(-decideVar));
	Assignments[decideVar] = -decideVar;
	return true;
}

Satisfiability solve()
{
	if(!BCP())
		return UNSAT;
	while(true)
	{
		if(!decide()) return SAT;
		while(!BCP())
			if(!backtrack()) return UNSAT;
	}
}


int main(int argc, char* argv[]) {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	decideVar = 1;
	trail = stack<trailElement>();
	Assignments.clear();

	assert(argc == 2);
	clauses = parse_dimacs(argv[1]);
	nClauses=clauses.size();
	for(auto clause: clauses)
	{
		for(auto literal: clause)
		{
			nVars = max(nVars, var(literal));
		}
	}
	Assignments.resize(nVars+1);
	// cout << Assignments.size() << endl;
	// for(auto z: Assignments)
	// 	cout << z << " ";
	// cout << endl;
	// std::cout << clauses << std::endl;
	if(solve()==SAT)
	{
		std::cout << SAT << std::endl;
		for(int i=1; i<=nVars; ++i)
			cout << Assignments[i] << " ";
		cout << endl;
		return 10;
	}
	else
	{
		std::cout << UNSAT << std::endl;
		return 20;
	}
	
}
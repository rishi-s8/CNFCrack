#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <queue>
#include <math.h>
#include <algorithm>

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
unordered_map<int,vector<int>> watchList;
queue<int> falseAssignments;
vector<pair<double,int>> variableOrdering;

void generateWatchList()
{
	for(int i=0; i<clauses.size(); ++i)
	{
		auto &clause = clauses[i];
		int maxRange = (clause.size() < 2) ? clause.size() : 2;
		for(int j=0; j<maxRange; ++j)
			watchList[clause[j]].push_back(i);
	}
}

bool backtrack()
{
	falseAssignments = queue<int>();
	while(!trail.empty())
	{
		trailElement current = trail.top();
		if(current.decision && !current.flipped)
		{
			falseAssignments.push(current.assignment);
			current.assignment = -current.assignment;
			current.flipped = true;
			Assignments[var(current.assignment)] = current.assignment;
			trail.pop();
			trail.push(current);
			return true;
		}
		else
		{
			Assignments[var(current.assignment)] = 0;
			trail.pop();
		}
	}
	return false;
}

bool BCP()
{
	while(!falseAssignments.empty())
	{
		int current = falseAssignments.front();
		falseAssignments.pop();
		for(int lookingIndex=0; lookingIndex<watchList[current].size(); ++lookingIndex)
		{
			int clauseIndex = watchList[current][lookingIndex];
			auto &clause = clauses[clauseIndex];
			int watchable = 0;
			int otherWatch = 0;
			for(auto literal: clause)
			{
				if(literal != current)
				{
					if(!otherWatch)
						for(auto z: watchList[literal])
							if(z==clauseIndex) 
							{
								otherWatch = literal;
								break;
							}
					if(otherWatch != literal && (!Assignments[var(literal)] || Assignments[var(literal)]==literal))
						watchable = literal;
					if(otherWatch && watchable)
						break;
				}
			}
			if(!otherWatch)
				return false;
			else if(Assignments[var(otherWatch)]==otherWatch)
				continue;
			else if(watchable)
			{
				watchList[current].erase(watchList[current].begin()+lookingIndex);
				watchList[watchable].push_back(clauseIndex);
				--lookingIndex;
			}
			else if(!Assignments[var(otherWatch)])
			{
				Assignments[var(otherWatch)] = otherWatch;
				trail.push(trailElement(otherWatch, false));
				falseAssignments.push(-otherWatch);
			}
			else
				return false;
		}
	}
	return true;
}

bool decide()
{

	int decideVar = 0;
	for(; decideVar<variableOrdering.size() && Assignments[var(variableOrdering[decideVar].second)]; ++decideVar);
	if(decideVar>=variableOrdering.size()) return false;
	trail.push(trailElement(variableOrdering[decideVar].second));
	Assignments[var(variableOrdering[decideVar].second)] = variableOrdering[decideVar].second;
	falseAssignments.push(-variableOrdering[decideVar].second);
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
	trail = stack<trailElement>();
	falseAssignments = queue<int>();
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

	variableOrdering.resize(nVars*2);
	for(auto clause: clauses)
	{
		for(auto literal: clause)
		{
			if(literal<0)
			{
				variableOrdering[2*(-literal)-1].second = literal;
				variableOrdering[2*(-literal)-1].first += pow(2, -(int)clause.size());
			}
		}
	}
	sort(variableOrdering.rbegin(), variableOrdering.rend());
	for(int j=2*nVars-1; variableOrdering[j].second==0; --j) variableOrdering.pop_back();

	generateWatchList();

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
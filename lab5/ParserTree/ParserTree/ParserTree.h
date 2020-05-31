#pragma once
#if not defined PARSERTREE_H
#define PARSERTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

typedef struct SelAttr {
	string attr_name;
	string attr_operator;
	string attr_value;
}SelAttr;

typedef struct Select {
	vector<SelAttr> sellist;
}Select;

typedef struct Project
{
	vector<string> attr_list;
}Project;

typedef struct Join
{
	string attr1;
	string attr2;
}Join;

typedef struct TreeNode
{
	int type;
	Select query1;
	Project query2;
	Join query3;
	vector<TreeNode> branch1;
	vector<TreeNode> branch2;
	string attr;
}TreeNode;

void Optimization(vector<TreeNode> *treenodes);
void UpdateSelect(vector<TreeNode> *newtreenodes, vector<SelAttr> *sellist, string sel_name);
void UpdateProject(vector<TreeNode> *newtreenodes, vector<string> *attrlist, string pro_name);
void LogicalTree(vector<TreeNode> *treenodes,int style);
void Parser(vector<string> *query, vector<TreeNode> *treenodes);
void SplitSql(string sql, vector<string> *query);
#endif

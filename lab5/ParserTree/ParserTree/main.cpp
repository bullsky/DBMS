#include "ParserTree.h"
int main(int argc,char **argv)
{
	string sql;
	cout << "Please input the sql:";
	//cin >> sql;
	getline(cin, sql);

	vector<string> query;
	SplitSql(sql, &query);

	vector<TreeNode> treenodes;
	Parser(&query, &treenodes);
	cout << "Logical Query Plan Tree" << endl;
	LogicalTree(&treenodes,0);
	cout << endl;
	Optimization(&treenodes);
	cout << "Physical Query Plan Tree" << endl;
	LogicalTree(&treenodes, 0);

	return 0;
}
void UpdateProject(vector<TreeNode> *newtreenodes, vector<string> *attrlist, string pro_name)
{
	Project newpro;
	for (int j = 0; j < attrlist->size(); j++)
	{
		if (attrlist->at(j) == pro_name)
		{
			newpro.attr_list.push_back(attrlist->at(j));
		}
	}
	TreeNode pro_node;
	pro_node.type = 2;
	pro_node.query2 = newpro;
	newtreenodes->push_back(pro_node);
}
void UpdateSelect(vector<TreeNode> *newtreenodes, vector<SelAttr> *sellist, string sel_name)
{
	Select newsel;
	for (int j = 0; j < sellist->size(); j++)
	{
		if (sellist->at(j).attr_name == sel_name)
		{
			newsel.sellist.push_back(sellist->at(j));
		}
	}
	TreeNode sel_node, pro_node;
	sel_node.type = 1;
	sel_node.query1 = newsel;
	newtreenodes->push_back(sel_node);
}
void Optimization(vector<TreeNode> *treenodes)
{
	int sel_index = -1;
	int pro_index = -1;
	vector<SelAttr> sellist;
	vector<string> attrlist;
	for (int i = 0; i < treenodes->size(); i++)
	{
		if (treenodes->at(i).type == 1)
		{
			sel_index = i;
			sellist = treenodes->at(i).query1.sellist;
		}
		if (treenodes->at(i).type == 2)
		{
			pro_index = i;
			attrlist = treenodes->at(i).query2.attr_list;
		}
	}
	for (int i = 0; i < treenodes->size(); i++)
	{
		if (treenodes->at(i).type == 3)
		{
			if (sel_index >= 0 && pro_index >= 0)
			{
				string attr1, attr2;
				attr1 = treenodes->at(i).query3.attr1;
				attr2 = treenodes->at(i).query3.attr2;
				if (attr1 == "EMPLOYEE") {
					vector<TreeNode> newtreenodes;
					UpdateSelect(&newtreenodes, &sellist, "ENAME");
					TreeNode tnode;
					tnode.type = 4;
					tnode.attr = attr1;
					newtreenodes.push_back(tnode);
					treenodes->at(i).query3.attr1 = "";
					treenodes->at(i).branch1 = newtreenodes;
				}
				else if (attr1 == "WORKS_ON")
				{
					vector<TreeNode> newtreenodes;
					//UpdateProject(&newtreenodes, &attrlist, "ESSN");
					UpdateSelect(&newtreenodes, &sellist, "ESSN");
					TreeNode tnode;
					tnode.type = 4;
					tnode.attr = attr1;
					newtreenodes.push_back(tnode);
					treenodes->at(i).query3.attr1 = "";
					treenodes->at(i).branch1 = newtreenodes;
				}
				if (attr2 == "DEPARTMENT")
				{
					vector<TreeNode> newtreenodes;
					//UpdateProject(&newtreenodes, &attrlist, "BDATE");
					UpdateSelect(&newtreenodes, &sellist, "DNAME");
					TreeNode tnode;
					tnode.type = 4;
					tnode.attr = attr2;
					newtreenodes.push_back(tnode);
					treenodes->at(i).query3.attr2 = "";
					treenodes->at(i).branch2 = newtreenodes;
				}
				//else if (attr2 == "PROJECT")
				//{
				//	vector<TreeNode> newtreenodes;
				//	UpdateProject(&newtreenodes, &attrlist, "PNAME");
				//	TreeNode tnode;
				//	tnode.type = 4;
				//	tnode.attr = attr2;
				//	newtreenodes.push_back(tnode);
				//	treenodes->at(i).query3.attr2 = "";
				//	treenodes->at(i).branch2 = newtreenodes;
				//}
				vector<TreeNode>::iterator it = treenodes->begin() + sel_index ;
				treenodes->erase(it);
			}
			else if (sel_index >= 0)
			{
				string attr1, attr2;
				attr1 = treenodes->at(i).query3.attr1;
				attr2 = treenodes->at(i).query3.attr2;
				if (attr1 == "EMPLOYEE") {
					vector<TreeNode> newtreenodes;
					UpdateSelect(&newtreenodes, &sellist, "ENAME");
					TreeNode tnode;
					tnode.type = 4;
					tnode.attr = attr1;
					newtreenodes.push_back(tnode);
					treenodes->at(i).query3.attr1 = "";
					treenodes->at(i).branch1 = newtreenodes;
				}
				if (attr2 == "DEPARTMENT")
				{
					vector<TreeNode> newtreenodes;
					UpdateSelect(&newtreenodes, &sellist, "DNAME");
					TreeNode tnode;
					tnode.type = 4;
					tnode.attr = attr2;
					newtreenodes.push_back(tnode);
					treenodes->at(i).query3.attr2 = "";
					treenodes->at(i).branch2 = newtreenodes;
				}
				vector<TreeNode>::iterator it = treenodes->begin() + sel_index;
				treenodes->erase(it);
			}
		}
	}
}
void LogicalTree(vector<TreeNode> *treenodes , int style)
{
	//cout << endl;
	for (int i = 0; i < treenodes->size(); i++)
	{
		if (treenodes->at(i).type == 1)
		{
			cout << "  选择 ";
			vector<SelAttr> list;
			list = treenodes->at(i).query1.sellist;
			for (int j = 0; j < list.size(); j++)
			{
				cout << list.at(j).attr_name << list.at(j).attr_operator << list.at(j).attr_value << " ";
			}
			if (style == 0)
			{
				cout << endl << "    |" << endl << "    |" << endl;
			}
			else if (style == 1)
			{
				cout << "  ——" ;
			}
			
		}
		if (treenodes->at(i).type == 2)
		{
			cout << "  投影 ";
			vector<string> attrlist;
			attrlist = treenodes->at(i).query2.attr_list;
			for (int j = 0; j < attrlist.size(); j++)
			{
				cout << attrlist.at(j) << " ";
			}
			if (style == 0)
			{
				cout << endl << "    |" << endl << "    |" << endl;
			}
			else
			{
				cout << "——";
			}

		}
		if (treenodes->at(i).type == 3)
		{
			cout << "  连接";
			if (treenodes->at(i).query3.attr2 != "")
			{
				cout << "  ——  " << treenodes->at(i).query3.attr2 << endl;
			}
			else
			{
				cout << "  ——";
				vector<TreeNode> newtreenodes = treenodes->at(i).branch2;
				LogicalTree(&newtreenodes, 1);
				cout << endl;
			}
			if (treenodes->at(i).query3.attr1 != "")
			{
				cout << "    |" << endl << "    |" << endl << "  " << treenodes->at(i).query3.attr1;
			}
			else
			{
				cout << "    |" << endl << "    |" << endl;
				vector<TreeNode> newtreenodes = treenodes->at(i).branch1;
				LogicalTree(&newtreenodes, 0);
			}
		}
		if (treenodes->at(i).type == 4)
		{
			cout << "  " << treenodes->at(i).attr;
		}
	}
}
void Parser(vector<string> *query, vector<TreeNode> *treenodes)
{
	string select = "SELECT";
	string project = "PROJECTION";
	string join = "JOIN";
	for (int i = 0; i < query->size(); i++)
	{
		if (query->at(i) == select)
		{
			i++;
			if (query->at(i) != "[")
			{
				cout << "Sql syntax error" << endl;
			}
			i++;
			vector<SelAttr> sellist;
			while (query->at(i) != "]")
			{
				SelAttr attr;
				attr.attr_name = query->at(i);
				attr.attr_operator = query->at(i + 1);
				attr.attr_value = query->at(i + 2);
				sellist.push_back(attr);
				i += 3;
				if (query->at(i) == "&")
				{
					i++;
				}
			}
			i++;
			Select select;
			select.sellist = sellist;
			TreeNode tnode;
			tnode.type = 1;
			tnode.query1 = select;
			treenodes->push_back(tnode);
		}
		else if (query->at(i) == project)
		{
			i++;
			if (query->at(i) != "[")
			{
				cout << "Sql syntax error" << endl;
			}
			i++;
			vector<string> attrlist;
			while (query->at(i) != "]")
			{
				attrlist.push_back(query->at(i));
				i++;
				if (query->at(i) == ",")
				{
					i++;
				}
			}
			i++;
			Project project;
			project.attr_list = attrlist;
			TreeNode tnode;
			tnode.type = 2;
			tnode.query2 = project;
			treenodes->push_back(tnode);
		}
		else if(query->at(i) == "(")
		{
			i++;
			vector<string> tmp_query;
			while (query->at(i) != ")")
			{
				tmp_query.push_back(query->at(i));
			}
			Parser(&tmp_query, treenodes);
			i++;
		}
		else if (query->at(i) == ")")
		{
			i++;
		}
		else if (query->at(i + 1) == join)
		{
			//if ((query->at(i+2) == "EMPLOYEE") || (query->at(i + 2) == "DEPARTMENT") || (query->at(i + 2) == "WORKS_ON") || (query->at(i + 2) == "PROJECT"))
			//{
			Join newjoin;
			newjoin.attr1 = query->at(i);
			newjoin.attr2 = query->at(i + 2);
			TreeNode tnode;
			tnode.type = 3;
			tnode.query3 = newjoin;
			treenodes->push_back(tnode);
			i += 3;
			//}
			//Join newjoin;
			//newjoin.attr1 = query->at(i);
			//newjoin.attr2 = to_string(treenodes->size() + 2);
			//i += 2;
			//TreeNode tnode;
			//tnode.type = 3;
			//tnode.query3 = newjoin;
			//treenodes->push_back(tnode);
			//vector<string> tmp_query;
			//for (; i < query->size(); i++)
			//{
			//	tmp_query.push_back(query->at(i));
			//}
			//Parser(&tmp_query, treenodes);
		}
	}
}
void SplitSql(string sql, vector<string> *query)
{
	string str = "";
	for (int i = 0; i < sql.size(); i++)
	{
		if (sql[i] == ' ')
		{
			if (str.size() > 0)
			{
				query->push_back(str);
				str = "";
			}
		}
		else
		{
			str += sql[i];
		}
	}
	if (str.size() > 0)
	{
		query->push_back(str);
	}
}
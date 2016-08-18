
#include "AVL.h"
#include <iostream>
#include <stdlib.h>

using namespace std; 	//introduces namespace std
int main( void )
{
	int i;
	AVL<short, short> junk;

	cout << "adding items\n";

	for (i = 0; i < 20; ++i)
	{
		junk.Insert(i, i + 1);
	}

	cout << "creating iterator\n";

	AVL<short, short>::Iterator<short, short> iter(&junk);

	bool more;
	short key = 3, item;

	more = iter.Find(key, item);
	cout << "Finding 3\n";
	cout << "key - " << key << " item - " << item << "\n";

	iter.GetFirst(key, item);

	cout << "Tree contains:\n";
	
	more = iter.GetFirst(key, item);
	while (more)
	{
		cout << "key - " << key << " item - " << item << "\n";
		more = iter.GetNext(key, item);
	}

	cout << "removing even keys\n";

	for (i = 0; i < 20; i += 2)
	{
		junk.Remove(i);
	}
	
	more = iter.GetFirst(key, item);
	while (more)
	{
		cout << "key - " << key << " item - " << item << "\n";
		more = iter.GetNext(key, item);
	}

	cout << "removing odd keys\n";

	for (i = 1; i < 20; i += 2)
	{
		junk.Remove(i);
	}

	more = iter.GetFirst(key, item);
	while (more)
	{
		cout << "key - " << key << " item - " << item << "\n";
		more = iter.GetNext(key, item);
	}

	cout << "done\n";

	return 0;
}

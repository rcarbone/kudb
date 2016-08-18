
/*==========================================================================

	   _____                .__          .__          
	  /     \   ____   _____|  |__  __ __|  | _____   
	 /  \ /  \_/ __ \ /  ___/  |  \|  |  \  | \__  \  
	/    Y    \  ___/ \___ \|   Y  \  |  /  |__/ __ \_
	\____|__  /\___  >____  >___|  /____/|____(____  /
	        \/     \/     \/     \/                \/ 
	   _________   ____.____      ___________                      
	  /  _  \   \ /   /|    |     \__    ___/______   ____   ____  
	 /  /_\  \   Y   / |    |       |    |  \_  __ \_/ __ \_/ __ \ 
	/    |    \     /  |    |___    |    |   |  | \/\  ___/\  ___/ 
	\____|__  /\___/   |_______ \   |____|   |__|    \___  >\___  >
	        \/                 \/                        \/     \/ 

	with thanks to -
		Greig McLachlan of http://www.learntoprogram.com
			for providing a clean implementation of Insert which I used as a starting point
		Jason F Boxall of CSC330
			who posted his assignment on the web, I double checked my Remove implementation against his.
		R Levow of http://www.cse.fau.edu/~roy/cop3530.98f/prog5-BSTIterator.html
			who posted an algorithm for creating binary tree iterators

	http://meshula.artistnation.com
										2002

	The template is free for use, no warrantee or suitability for any particular 
	purpose is expressed or implied. Verify that it works the way you expect before you 
	incorporate it into your own project.

	public interface -
	
		AVL();
		~AVL();
		bool  Insert(KeyType key, ItemType item);
		bool  Remove(KeyType key);
		bool  Find(KeyType key, ItemType& item);
		short GetDepth()
		
		Iterator(Iterator*)
		Iterator(AVL*)
		~Iterator()
		
			// all return true for success, false for failure or end of tree

			bool GetFirst(KeyType& key, ItemType& item)
			bool GetNext(KeyType& key, ItemType& item)
			bool GetCurrent(KeyType& key, ItemType& item)
			bool Find(KeyType key, ItemType& item)

  ==========================================================================*/


/*--------------------------------------------------------------------------------------------
	    ___     ___           _           _                 _   _             
	   / \ \   / / |       __| | ___  ___| | __ _ _ __ __ _| |_(_) ___  _ __  
	  / _ \ \ / /| |      / _` |/ _ \/ __| |/ _` | '__/ _` | __| |/ _ \| '_ \ 
	 / ___ \ V / | |___  | (_| |  __/ (__| | (_| | | | (_| | |_| | (_) | | | |
	/_/   \_\_/  |_____|  \__,_|\___|\___|_|\__,_|_|  \__,_|\__|_|\___/|_| |_|
  --------------------------------------------------------------------------------------------*/

template<class KeyType, class ItemType>
class AVL
{
protected:
	template<class KeyType, class ItemType>
	class AVLNode
	{
	public:
		AVLNode(KeyType key, ItemType item) :
			m_Balance(0), m_Depth(0),
			m_Key(key), m_Data(item),
			m_pLeft(0), m_pRight(0)
		{
		}
		
		short		m_Balance;
		short		m_Depth;
		
		KeyType		m_Key;
		ItemType	m_Data;

		AVLNode*	m_pLeft;
		AVLNode*	m_pRight;
	};

	AVLNode<KeyType, ItemType>* m_pRoot;

public:

	/*------------------------------------------------------------------------
		             _     _ _        _       _             __                
		 _ __  _   _| |__ | (_) ___  (_)_ __ | |_ ___ _ __ / _| __ _  ___ ___ 
		| '_ \| | | | '_ \| | |/ __| | | '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \
		| |_) | |_| | |_) | | | (__  | | | | | ||  __/ |  |  _| (_| | (_|  __/
		| .__/ \__,_|_.__/|_|_|\___| |_|_| |_|\__\___|_|  |_|  \__,_|\___\___|
		|_|                                                                    */

	AVL() : m_pRoot(0) { }
	~AVL() { }
	
	void 	Insert(KeyType key, ItemType item);
	void 	Remove(KeyType key);
	bool 	Find  (KeyType key, ItemType& item);
	
	short	GetDepth() const {	return (m_pRoot ? m_pRoot->m_Depth : 0);	}

	/*------------------------------------------------------------------------
		 _ _                 _             
		(_) |_ ___ _ __ __ _| |_ ___  _ __ 
		| | __/ _ \ '__/ _` | __/ _ \| '__|
		| | ||  __/ | | (_| | || (_) | |   
		|_|\__\___|_|  \__,_|\__\___/|_|   
																				*/

	template<class KeyType, class ItemType>
	class Iterator
	{
	#define kMaxAVLDepth 32
	
	public:
		Iterator(Iterator* pCopyMe) : 
			mpAVL(pCopyMe->mpAVL), mpCurr(pCopyMe->mpCurr)
		{
			mTraversalStackIndex = pCopyMe->mTraversalStackIndex;

			for (int i = 0; i < mTraversalStackIndex; ++i)
				mTraversalStack[i] = pCopyMe->mTraversalStack[i];
		}
		
		Iterator(AVL<KeyType, ItemType>* pTree) : mpAVL(pTree)
		{
			KeyType key;
			ItemType item;
			GetFirst(key, item);
		}

		~Iterator() { }

		bool GetCurrent(KeyType& key, ItemType& item)
		{
			if (mpCurr)
			{
				key = mpCurr->m_Key;
				item = mpCurr->m_Data;
				return true;
			}
			else
				return false;
		}

		// returns false if the tree is empty
		bool GetFirst (KeyType& key, ItemType& item)
		{
			mTraversalStackIndex = 0;
			
			if (!mpAVL->m_pRoot)
			{
				mpCurr = 0;
				item = 0;
				return false;
			}
			else
			{
				AVLNode<ItemType, KeyType>* pCurr = mpAVL->m_pRoot;
				AVLNode<ItemType, KeyType>* pPrev = pCurr;
				while (pCurr)
				{
					pPrev = pCurr;

					if (pCurr->m_pLeft)
						mTraversalStack[mTraversalStackIndex++] = pCurr;

					pCurr = pCurr->m_pLeft;
				}
			
				mpCurr = pPrev;
				key = mpCurr->m_Key;
				item = mpCurr->m_Data;
				return true;
			}
		}

		bool GetNext  (KeyType& key, ItemType& item)
		{
			if (!mpCurr)	// already done?
			{
				item = 0;
				return false;
			}
			else
			{
				AVLNode<KeyType, ItemType>* pCurr = mpCurr->m_pRight;	// start looking to the right

				while (true)	// this while forces a traversal as far left as possible
				{
					if (pCurr)	// if we have a pcurr, push it and go left, and repeat.
					{
						mTraversalStack[mTraversalStackIndex++] = pCurr;
						pCurr = pCurr->m_pLeft;
					}
					else	// backtrack
					{
						if (mTraversalStackIndex > 0)
						{
							AVLNode<KeyType, ItemType>* pCandidate = mTraversalStack[--mTraversalStackIndex];

							// did we backtrack up a right branch?
							if (mpCurr == pCandidate->m_pRight)
							{
								// if there is a parent, return the parent.
								if (mTraversalStackIndex > 0)
								{
									mpCurr = mTraversalStack[--mTraversalStackIndex];
									key = mpCurr->m_Key;
									item = mpCurr->m_Data;
									return true;
								}
								else	// if up a right branch, and no parent, traversal finished
								{
									mpCurr = 0;
									item = 0;
									return false;
								}
							}
							else	// up a left branch, done.
							{
								mpCurr = pCandidate;
								key = mpCurr->m_Key;
								item = mpCurr->m_Data;
								return true;
							}
						}
						else	// totally done
						{
							mpCurr = 0;
							item = 0;
							return false;
						}
					}
				}
			}
		}

		bool Find     (KeyType  key, ItemType& item)
		{
			AVLNode<KeyType, ItemType>* pCurr = mpAVL->m_pRoot;
			mTraversalStackIndex = 0;

			while (true)
			{
				AVLNode<KeyType, ItemType>* pPushMe = pCurr;
				if (pCurr->m_Key == key)	// already done?
				{
					mpCurr = pCurr;
					item = mpCurr->m_Data;
					return true;
				}
					
				if (pCurr->m_Key > key)
					pCurr = pCurr->m_pLeft;
				else
					pCurr = pCurr->m_pRight;

				if (pCurr)	// maintain the stack so that GetNext will work.
				{
					mTraversalStack[mTraversalStackIndex++] = pPushMe;
				}
				else	// couldn't find it.
				{
					mpCurr = 0;
					mTraversalStackIndex = 0;
					return false;
				}
			}
			
			return true;
		}

	protected:
		AVL*						mpAVL;		
		AVLNode<KeyType, ItemType>*	mTraversalStack[kMaxAVLDepth];
		short						mTraversalStackIndex;
		AVLNode<KeyType, ItemType>*	mpCurr;				// for iteration
	};

	friend class AVL::Iterator<KeyType, ItemType>;

protected:
	void _Insert        		(KeyType key, ItemType item, AVLNode<KeyType, ItemType>*& root);
	void _Remove				(AVLNode<KeyType, ItemType>*& root, KeyType key, bool& delOK);
	void _RemoveBothChildren	(AVLNode<KeyType, ItemType>*& root, AVLNode<KeyType, ItemType>*& curr, bool& delOK);
	bool _Find					(KeyType key, ItemType& item, AVLNode<KeyType, ItemType>* root);
	void ComputeBalance 		(AVLNode<KeyType, ItemType>*  root);
	void Balance        		(AVLNode<KeyType, ItemType>*& root);
	void BalanceRight			(AVLNode<KeyType, ItemType>*& root);
	void BalanceLeft			(AVLNode<KeyType, ItemType>*& root);
	void RotateLeft				(AVLNode<KeyType, ItemType>*& root);
	void RotateRight			(AVLNode<KeyType, ItemType>*& root);
};


/*--------------------------------------------------------------------------------------------
	 _                     _   _             
	(_)_ __  ___  ___ _ __| |_(_) ___  _ __  
	| | '_ \/ __|/ _ \ '__| __| |/ _ \| '_ \ 
	| | | | \__ \  __/ |  | |_| | (_) | | | |
	|_|_| |_|___/\___|_|   \__|_|\___/|_| |_|
  --------------------------------------------------------------------------------------------*/

template<class KeyType, class ItemType>
void AVL<KeyType,ItemType>::Insert(KeyType key, ItemType item)
{
	if (m_pRoot == 0)
	{
		m_pRoot = new AVLNode<KeyType, ItemType>(key, item);
	}
	else
		_Insert(key, item, m_pRoot);
}

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::_Insert(KeyType key, ItemType item, AVLNode<KeyType, ItemType>*& root)
{
	if (key < root->m_Key)
	{
		if (root->m_pLeft)
			_Insert(key, item, root->m_pLeft);
		else
			root->m_pLeft = new AVLNode<KeyType, ItemType>(key, item);
	}
	else if (key > root->m_Key)
	{
		if (root->m_pRight)
			_Insert(key, item, root->m_pRight);
		else
			root->m_pRight = new AVLNode<KeyType, ItemType>(key, item);
	}
	else
	{
		// error - can't have duplicate keys.
		// if duplicate keys are okay, change key < to key <= above
	}
	
	ComputeBalance(root);
	Balance(root);
}

/*--------------------------------------------------------------------------------------------
	                                    _ 
	 _ __ ___ _ __ ___   _____   ____ _| |
	| '__/ _ \ '_ ` _ \ / _ \ \ / / _` | |
	| | |  __/ | | | | | (_) \ V / (_| | |
	|_|  \___|_| |_| |_|\___/ \_/ \__,_|_|
  --------------------------------------------------------------------------------------------*/
                                      
template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::Remove(KeyType key)
{
	bool delOK = false;
	_Remove(m_pRoot, key, delOK);
}

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::_Remove(AVLNode<KeyType, ItemType>*& root, KeyType key, bool& delOK)
{
	if (!root)
	{
		delOK = false;
	}
	else if (root->m_Key > key)	// go to left subtree
	{
		_Remove(root->m_pLeft, key, delOK);

		if (delOK)
		{
			ComputeBalance(root);
			BalanceRight(root);
		}
	}
	else if (root->m_Key < key) // go to right subtree
	{
		_Remove(root->m_pRight, key, delOK);

		if (delOK)
		{
			ComputeBalance(root);
			BalanceLeft(root);
		}
	}
	else	// node found!
	{
		AVLNode<KeyType, ItemType>* pMe = root;
		
		if (!root->m_pRight)
		{
			root = root->m_pLeft;
			delOK = true;
			delete pMe;
		}
		else if (!root->m_pLeft)
		{
			root = root->m_pRight;
			delOK = true;
			delete pMe;
		}
		else
		{
			_RemoveBothChildren(root, root->m_pLeft, delOK);
			if (delOK)
			{
				ComputeBalance(root);
				Balance(root);
			}
				
			delOK = true;
		}
	}
}

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::_RemoveBothChildren(AVLNode<KeyType, ItemType>*& root, AVLNode<KeyType, ItemType>*& curr, bool& delOK)
{
	if (!curr->m_pRight)
	{
		root->m_Key = curr->m_Key;
		root->m_Data = curr->m_Data;
		AVLNode<KeyType, ItemType>* pMe = curr;
		curr = curr->m_pLeft;
		delete pMe;
		delOK = true;
	}
	else
	{
		_RemoveBothChildren(root, curr->m_pRight, delOK);
		if (delOK)
		{
			ComputeBalance(root);
			Balance(root);
		}
	}
}

/*--------------------------------------------------------------------------------------------
	                         _     _             
	 ___  ___  __ _ _ __ ___| |__ (_)_ __   __ _ 
	/ __|/ _ \/ _` | '__/ __| '_ \| | '_ \ / _` |
	\__ \  __/ (_| | | | (__| | | | | | | | (_| |
	|___/\___|\__,_|_|  \___|_| |_|_|_| |_|\__, |
	                                       |___/ 
  --------------------------------------------------------------------------------------------*/

template<class KeyType, class ItemType>
bool AVL<KeyType, ItemType>::Find(KeyType key, ItemType& item)
{
	return _Find(key, item, m_pRoot);
}

template<class KeyType, class ItemType>
bool AVL<KeyType, ItemType>::_Find(KeyType key, ItemType& item, AVLNode<KeyType, ItemType>* root)
{
	if (root)
	{
		if (root->m_Key == key)
		{
			item = root->m_Data;
			return true;
		}
		
		if (key < root->m_Key)
			return _Find(key, item, root->m_pLeft);
		else
			return _Find(key, item, root->m_pRight);
	}
	else
	{
		return false;
	}
}


/*--------------------------------------------------------------------------------------------
	 _           _                  _             
	| |__   __ _| | __ _ _ __   ___(_)_ __   __ _ 
	| '_ \ / _` | |/ _` | '_ \ / __| | '_ \ / _` |
	| |_) | (_| | | (_| | | | | (__| | | | | (_| |
	|_.__/ \__,_|_|\__,_|_| |_|\___|_|_| |_|\__, |
	                                        |___/ 
  --------------------------------------------------------------------------------------------*/

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::ComputeBalance(AVLNode<KeyType, ItemType>* root)
{
	if (root)
	{
		short leftDepth  = root->m_pLeft  ? root->m_pLeft->m_Depth  : 0;
		short rightDepth = root->m_pRight ? root->m_pRight->m_Depth : 0;

		root->m_Depth = 1 + ((leftDepth > rightDepth) ? leftDepth : rightDepth);
		root->m_Balance = rightDepth - leftDepth;
	}
}

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::Balance(AVLNode<KeyType, ItemType>*& root)
{
	// AVL trees have the property that no branch is more than 1 longer than its sibling

	if (root->m_Balance > 1)
		BalanceRight(root);
		
	if (root->m_Balance < -1)
		BalanceLeft(root);
}

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::BalanceRight(AVLNode<KeyType, ItemType>*& root)
{
	if (root->m_pRight)
	{
		if (root->m_pRight->m_Balance > 0)
		{
			RotateLeft(root);
		}
		else if (root->m_pRight->m_Balance < 0)
		{
			RotateRight(root->m_pRight);
			RotateLeft(root);
		}
	}
}

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::BalanceLeft(AVLNode<KeyType, ItemType>*& root)
{
	if (root->m_pLeft)
	{
		if (root->m_pLeft->m_Balance < 0)
		{
			RotateRight(root);
		}
		else if (root->m_pLeft->m_Balance > 0)
		{
			RotateLeft(root->m_pLeft);
			RotateRight(root);
		}
	}
}

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::RotateLeft(AVLNode<KeyType, ItemType>*& root)
{
	AVLNode<KeyType, ItemType>* pTemp = root;
	root = root->m_pRight;
	pTemp->m_pRight = root->m_pLeft;
	root->m_pLeft = pTemp;

	ComputeBalance(root->m_pLeft);
	ComputeBalance(root->m_pRight);
	ComputeBalance(root);
}

template<class KeyType, class ItemType>
void AVL<KeyType, ItemType>::RotateRight(AVLNode<KeyType, ItemType>*& root)
{
	AVLNode<KeyType, ItemType>* pTemp = root;
	root = root->m_pLeft;
	pTemp->m_pLeft = root->m_pRight;
	root->m_pRight = pTemp;

	ComputeBalance(root->m_pLeft);
	ComputeBalance(root->m_pRight);
	ComputeBalance(root);
}

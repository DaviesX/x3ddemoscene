/* algorithm: Some commonly used algorithm which is implemented in the header
 * to avoid function pointer */
#ifndef ALGORITHM_H_INCLUDED
#define ALGORITHM_H_INCLUDED

// Swap any type of variables
#define SwapVariableType(_a, _b, _TYPE)	\
{\
	_TYPE _t;\
	memcpy ( &(_t), (_a), sizeof ( _TYPE ) );\
	memcpy ( (_a), (_b), sizeof ( _TYPE ) );\
	memcpy ( (_b), &(_t), sizeof ( _TYPE ) );\
}



// Copy any type of variables
#define CopyVariableType(_a, _b, _TYPE )	\
{\
	memcpy ( (_b), (_a), sizeof ( _TYPE ) );\
}



// Swap integer index
#define SwapIndex(_a, _b)	\
{\
	int _t = (_a)->index;\
	(_a)->index = (_b)->index;\
	(_b)->index = (_t);\
}// End Function SwapIndex


// Copy index list element
#define CopyIndex( _a, _b )		((_b)->index = (_a)->index)

// Access list through index list
#define GetListIndex( _indexList, _index )		((_indexList)[(_index)].index)


// Sorting an array list with bubble sort algorithm
#define SortArrayBubble( _list, _length, _TYPE, _info, _CompareFunction )	\
{\
	int _i = (_length) - 1;\
    while ( _i > 0 ) {\
        int _last = 0;\
        for ( int _j = 0; _j < _i; _j ++ ) {\
            if ( _CompareFunction ( &(_list)[_j + 1], &(_list)[_j] ), (_info) ) {\
                SwapVariableType ( &(_list)[_j], &(_list)[_j + 1], _TYPE );\
                _last = _j;\
            }\
        }\
        _i = _last;\
    }\
}// End Function SortArrayBubble



// Sorting an array list with insertion sort algorithm
#define SortArrayInsertion( _list, _length, _TYPE, _info, _CompareFunction )	\
{\
	int _iSample;\
	for ( _iSample = 0; _iSample < (_length); _iSample ++ ) {\
		if ( _CompareFunction ( &(_list)[_iSample], &(_list)[_iSample - 1], (_info) ) ) {\
			_TYPE _exception;\
			CopyVariableType ( &(_list)[_iSample], &_exception, _TYPE );\
			int _iCurr;\
			for ( _iCurr = _iSample; _iCurr > 0; _iCurr -- ) {\
				if ( _CompareFunction ( &_exception, &(_list)[_iCurr - 1], (_info) ) ) {\
					CopyVariableType ( &(_list)[_iCurr - 1], &(_list)[_iCurr], _TYPE );\
				} else {\
					break;\
				}\
			}\
			CopyVariableType ( &_exception, &(_list)[_iCurr], _TYPE );\
		}\
	}\
}// End Function SortArrayInsertion



// Select a number cloest to the middle value
#define SelectPivotVariable( _list, _left, _right, _pivot, _info, _CompareFunction)	\
{\
	int _mid = ((_left) + (_right)) >> 1;\
	if ( !_CompareFunction ( &(_list)[(_left)], &(_list)[_mid], (_info) ) ) {\
		if ( !_CompareFunction ( &(_list)[_mid], &(_list)[(_right)], (_info) ) ) {\
			(_pivot) = _mid;\
		} else {\
			(_pivot) = (_left);\
		}\
	} else {\
		if ( _CompareFunction ( &(_list)[_mid], &(_list)[(_right)], (_info) ) ) {\
			(_pivot) = _mid;\
		} else {\
			(_pivot) = (_right);\
		}\
	}\
}// End Function SelectPivotVariable


// Storage for stack information of QuickSort function
struct PARTITION_BOUNDARY {
	int right;
	int pivotIndex;
};

// Helper functions for SortArrayListQuick
#define PushStackSortArrayQuick( _stack, _index, _right, _pivot )	\
{\
    (_stack)[(_index)].right = (_right);\
    (_stack)[(_index)].pivotIndex = (_pivot);\
    (_index) ++;\
}// End Function PushStackSortArrayQuick



#define PopStackSortArrayQuick( _stack, _index, _left, _right )	\
{\
    (_index) --;\
    (_left) = (_stack)[(_index)].pivotIndex + 1;\
    (_right) = (_stack)[(_index)].right;\
}// End Function PopStackSortArrayQuick


#define MAX_ITERATIONS		64
#define CUT_OFF_SIZE		16


// Sorting an array list with quick sort algorithm
#define SortArrayQuick( _list, _length, _TYPE, _info, _CompareFunction )	\
{\
	struct PARTITION_BOUNDARY partBoundary[MAX_ITERATIONS];\
\
    int _stackIndex = 0;\
    int _left = 0,\
        _right = (_length) - 1;\
    const int _boundary = _right;\
    int _pivotIndex;\
	SelectPivotVariable ( (_list), 0, _right, _pivotIndex, (_info), _CompareFunction );\
\
    while ( 1 ) {\
		if ( _right - _left < CUT_OFF_SIZE ) {\
			if ( _boundary - _left < CUT_OFF_SIZE ) {\
				SortArrayInsertion ( &(_list)[_left], _boundary - _left + 1,\
									_TYPE, (_info), _CompareFunction );\
                break;\
            }\
            SortArrayInsertion ( &(_list)[_left], _right - _left + 1,\
							_TYPE, (_info), _CompareFunction );\
            PopStackSortArrayQuick ( partBoundary, _stackIndex, _left, _right );\
            SelectPivotVariable ( (_list), _left, _right, _pivotIndex, (_info), _CompareFunction );\
            continue;\
		} else {\
            _TYPE _pivotValue;\
            CopyVariableType ( &(_list)[_pivotIndex], &_pivotValue, _TYPE );\
            SwapVariableType ( &(_list)[_pivotIndex], &(_list)[_right], _TYPE );\
            int _newPivotIndex = _left;\
\
		int _i;\
            for ( _i = _left; _i < _right; _i ++ ) {\
                if ( _CompareFunction ( &(_list)[_i], &_pivotValue, _info ) ) {\
                    SwapVariableType ( &(_list)[_newPivotIndex], &(_list)[_i], _TYPE );\
                    _newPivotIndex ++;\
                }\
            }\
\
            SwapVariableType ( &(_list)[_newPivotIndex], &(_list)[_right], _TYPE );\
\
            PushStackSortArrayQuick ( partBoundary, _stackIndex, _right, _newPivotIndex );\
            _right = _newPivotIndex;\
            SelectPivotVariable ( (_list), _left, _right, _pivotIndex, (_info), _CompareFunction );\
		}\
    }\
}// End Function SortArrayQuick



// Partition an array list in-place according to the given mid-value
#define PartitionArrayMidValue( _list, _length, _TYPE, _midValue, _CompareFunction, _midIndex )	\
{\
	int _newPivotIndex = 0;\
	int _i;\
    for ( _i = 0; _i < (_length); _i ++ ) {\
        if ( _CompareFunction ( &(_list)[_i], (_midValue) ) ) {\
            SwapVariableType ( &(_list)[_newPivotIndex], &(_list)[_i], _TYPE );\
            _newPivotIndex ++;\
        }\
    }\
    (_midIndex) = _newPivotIndex;\
}// End Function PartitionArrayMidValue

#define partition_alg( _array, _length, _info, _compare, _imiddle )	\
{\
	int _pivot = 0;\
	int _i;\
	for ( _i = 0; _i < (_length); _i ++ ) {\
		if ( _compare ( &(_array)[_i], _info ) ) {\
			/* Swap */\
			typeof (*(_array)) t = (_array)[_i];\
			(_array)[_i] = (_array)[_pivot];\
			(_array)[_pivot] = t;\
			_pivot ++;\
		}\
	}\
	*(_imiddle) = _pivot;\
}

// Partition an array list in-place without a given mid-value
#define PartitionArray( _list, _length, _TYPE, _info, _CompareFunction, midIndex )	\
{\
	int _newPivotIndex = 0;\
    for ( int _i = 0; _i < (_length); _i ++ ) {\
        if ( _CompareFunction ( &(_list)[_i], (_info) ) ) {\
            SwapVariableType ( &(_list)[_newPivotIndex], &(_list)[_i], _TYPE );\
            _newPivotIndex ++;\
        }\
    }\
    (_midIndex) = _newPivotIndex;\
}// End Function PartitionArray



// Find the nth element in the array list
#define FindNthElementArray( _list, _length, _nthPos, _TYPE, _info, _CompareFunction )	\
{\
	int _left = 0,\
		_right = (_length) - 1;\
	_TYPE _pivotValue;\
	int _pivotIndex = 0;\
	SelectPivotVariable ( (_list), 0, _right, _pivotIndex, (_info), _CompareFunction );\
	CopyVariableType ( &(_list)[_pivotIndex], &_pivotValue, _TYPE );\
\
	while ( 1 ) {\
		int _partPos = _left;\
\
		SwapVariableType ( &(_list)[_pivotIndex], &(_list)[_right], _TYPE );\
		int _index;\
		for ( _index = _left; _index < _right; _index ++ ) {\
			if ( _CompareFunction ( &(_list)[_index], &_pivotValue, (_info) ) ) {\
				SwapVariableType ( &(_list)[_partPos], &(_list)[_index], _TYPE );\
				_partPos ++;\
			}\
		}\
		SwapVariableType ( &(_list)[_right], &(_list)[_partPos], _TYPE );\
\
		if ( _partPos > _nthPos ) {\
			_right = _partPos - 1;\
		} else if ( _partPos < _nthPos ) {\
			_left = _partPos + 1;\
			_pivotIndex = _left;\
		} else {\
			break;\
		}\
\
		if ( _right - _left <= CUT_OFF_SIZE ) {\
			SortArrayInsertion ( &(_list)[_left], _right - _left + 1,\
								_TYPE, (_info), _CompareFunction );\
			break;\
		}\
\
		SelectPivotVariable ( (_list), _left, _right, _pivotIndex, (_info), _CompareFunction );\
		CopyVariableType ( &(_list)[_pivotIndex], &_pivotValue, _TYPE );\
	}\
}// End Function FindNthElementArray

#define split_i_alg( _array, _length, _i, _info, _compare )	\
{\
	int _left = 0,\
		_right = (_length) - 1;\
	typeof (*(_array)) _pivotValue;\
	int _pivotIndex = 0;\
	SelectPivotVariable ( (_array), 0, _right, _pivotIndex, (_info), _compare );\
	_pivotValue = (_array)[_pivotIndex];\
\
	while ( 1 ) {\
		int _partPos = _left;\
\
		typeof (*(_array)) _t;\
		_t = (_array)[_pivotIndex];\
		(_array)[_pivotIndex] = (_array)[_right];\
		(_array)[_right] = _t;\
\
		int _index;\
		for ( _index = _left; _index < _right; _index ++ ) {\
			if ( _compare ( &(_array)[_index], &_pivotValue, (_info) ) ) {\
				_t = (_array)[_partPos];\
				(_array)[_partPos] = (_array)[_index];\
				(_array)[_index] = _t;\
				_partPos ++;\
			}\
		}\
		_t = (_array)[_right];\
		(_array)[_right] = (_array)[_partPos];\
		(_array)[_partPos] = _t;\
\
		if ( _partPos > _i ) {\
			_right = _partPos - 1;\
		} else if ( _partPos < _i ) {\
			_left = _partPos + 1;\
			_pivotIndex = _left;\
		} else {\
			break;\
		}\
\
		if ( _right - _left <= CUT_OFF_SIZE ) {/** FIXEME, NO TYPE ! **/\
			SortArrayInsertion ( &(_array)[_left], _right - _left + 1,\
								typeof (*(_array)), (_info), _compare );\
			break;\
		}\
\
		SelectPivotVariable ( (_array), _left, _right, _pivotIndex, (_info), _compare );\
		_pivotValue = (_array)[_pivotIndex];\
	}\
}

// Create a indirect index list for different list-series algorithm
#define CreateIndexList( _indexList, _length )	\
{\
	*(_indexList) = alloc_fix ( sizeof ( ALG_INDEX_LIST ), (_length) );\
	int _i;\
	for ( _i = 0; _i < (_length); _i ++ ) {\
		(*(_indexList))[_i].index = _i;\
	}\
}// End Function CreateIndexList



// Rearrage the whole array list according to the ordered index list
#define ProcessIndexListToArray( _list, _indexList, _length, _TYPE, _newList )	\
{\
	int _i;\
	for ( _i = 0; _i < (_length); _i ++ ) {\
		CopyVariableType ( &(_list)[GetListIndex ( (_indexList), _i )],\
							&(_newList)[_i], _TYPE );\
	}\
	free_fix ( (_indexList) );\
}// End Function ProcessArrayIndexList



// Create a indirect bi-index list for different list-series algorithm
#define CreateBiindexList( _indexList, _length )	\
{\
	*(_indexList) = alloc_fix ( sizeof ( ALG_BIINDEX_LIST ), (_length) );\
	int _i;\
	for ( _i = 0; _i < (_length); _i ++ ) {\
		(*(_indexList))[_i].index = _i;\
	}\
}// End Function CreateBiindexList



// Rearrage the whole array list according to the ordered bi-index list
#define ProcessBiindexList( _list, _indexList, _length, _TYPE )	\
{\
	int _i;\
	for ( _i = 0; _i < (_length); _i ++ ) {\
		(_indexList)[(_indexList)[_i].index].origin = _i;\
	}\
	_TYPE temp[2];\
	CopyVariableType ( &(_list)[0], &temp[0], _TYPE );\
	int _iterIndex = 0;\
	int _iElement = 0, _switcher = 0;\
	while ( _iElement < (_length) ) {\
		if ( _iterIndex != (_indexList)[_iterIndex].index ) {\
			(_indexList)[_iterIndex].index = _iterIndex;\
			_iterIndex = (_indexList)[_iterIndex].origin;\
			CopyVariableType ( &(_list)[_iterIndex], &temp[(_switcher + 1) & 1], _TYPE );\
			CopyVariableType ( &temp[_switcher & 1], &(_list)[_iterIndex], _TYPE );\
			_switcher ++;\
		} else {\
			do {\
				_iterIndex ++;\
				_iElement ++;\
			} while ( _iterIndex == (_indexList)[_iterIndex].index );\
			CopyVariableType ( &(_list)[_iterIndex], &temp[_switcher & 1], _TYPE );\
		}\
	}\
	free_fix ( (_indexList) );\
}// End Function ProcessBiindexList



// Sorting an index list with bubble sort algorithm
#define SortIndexBubble( _list, _indexList, _length, _TYPE, _info, _CompareFunction )	\
{\
	int _i = (_length) - 1;\
    while ( _i > 0 ) {\
        int _last = 0;\
        for ( int _j = 0; _j < _i; _j ++ ) {\
            if ( _CompareFunction ( &(_list)[(_indexList)[_j + 1]], \
									  &(_list)[(_indexList)[_j]], (_info) ) ) {\
				SwapIndex ( &(_indexList)[_j], &(_indexList)[_j + 1] );\
                _last = _j;\
            }\
        }\
        _i = _last;\
    }\
}// End Function SortIndexBubble



// Sorting an index list with insertion sort algorithm
#define SortIndexInsertion( _list, _indexList, _length, _TYPE, _info, _CompareFunction )	\
{\
	int _iSample;\
	for ( _iSample = 0; _iSample < (_length); _iSample ++ ) {\
		if ( _CompareFunction ( &(_list)[GetListIndex ( (_indexList), _iSample )],\
								&(_list)[GetListIndex ( (_indexList), _iSample - 1 )], (_info) ) ) {\
			ALG_BIINDEX_LIST _exception;\
			CopyIndex ( &(_indexList)[_iSample], &_exception );\
			int _iCurr;\
			for ( _iCurr = _iSample; _iCurr > 0; _iCurr -- ) {\
				if ( _CompareFunction ( &(_list)[GetListIndex ( &_exception, 0 )],\
										&(_list)[GetListIndex ( (_indexList), _iCurr - 1 )], (_info) ) ) {\
					CopyIndex ( &(_indexList)[_iCurr - 1], &(_indexList)[_iCurr] );\
				} else {\
					break;\
				}\
			}\
			CopyIndex ( &_exception, &(_indexList)[_iCurr] );\
		}\
	}\
}// End Function SortIndexInsertion



// Select a number cloest to the middle value
#define SelectPivotIndex( _list, _indexList, _left, _right, _pivot, _info, _CompareFunction )	\
{\
	int _mid = ((_left) + (_right)) >> 1;\
	if ( !_CompareFunction ( &(_list)[GetListIndex ( (_indexList), (_left) )],\
							 &(_list)[GetListIndex ( (_indexList), _mid )], (_info) ) ) {\
		if ( !_CompareFunction ( &(_list)[GetListIndex ( (_indexList), _mid )],\
								 &(_list)[GetListIndex ( (_indexList), (_right) )], (_info) ) ) {\
			(_pivot) = _mid;\
		} else {\
			(_pivot) = (_left);\
		}\
	} else {\
		if ( _CompareFunction ( &(_list)[GetListIndex ( (_indexList), _mid )],\
								&(_list)[GetListIndex ( (_indexList), (_right) )], (_info) ) ) {\
			(_pivot) = _mid;\
		} else {\
			(_pivot) = (_right);\
		}\
	}\
}// End Function SelectPivotIndex



// Sorting an index list with quick sort algorithm
#define SortIndexQuick( _list, _indexList, _length, _TYPE, _info, _CompareFunction )	\
{\
	struct PARTITION_BOUNDARY partBoundary[MAX_ITERATIONS];\
\
    int _stackIndex = 0;\
    int _left = 0,\
        _right = (_length) - 1;\
    const int _boundary = _right;\
    int _pivotIndex;\
	SelectPivotIndex ( (_list), (_indexList),\
					0, _right, _pivotIndex, (_info), _CompareFunction );\
\
    while ( 1 ) {\
		if ( _right - _left < CUT_OFF_SIZE ) {\
			if ( _boundary - _left < CUT_OFF_SIZE ) {\
				SortIndexInsertion ( (_list), &(_indexList)[_left],\
								_boundary - _left + 1, _TYPE,\
								(_info), _CompareFunction );\
                break;\
            }\
            SortIndexInsertion ( (_list), &(_indexList)[_left],\
								_right - _left + 1, _TYPE,\
								(_info), _CompareFunction );\
            PopStackSortArrayQuick ( partBoundary, _stackIndex, _left, _right );\
            SelectPivotIndex ( (_list), (_indexList),\
							_left, _right, _pivotIndex, (_info), _CompareFunction );\
            continue;\
		} else {\
            _TYPE *_pivotValue = &(_list)[GetListIndex ( (_indexList), _pivotIndex )];\
            SwapIndex ( &(_indexList)[_pivotIndex], &(_indexList)[_right] );\
            int _newPivotIndex = _left;\
\
		int _i;\
            for ( _i = _left; _i < _right; _i ++ ) {\
                if ( _CompareFunction ( &(_list)[GetListIndex ( (_indexList), _i )],\
									_pivotValue, (_info) ) ) {\
                    SwapIndex ( &_indexList[_newPivotIndex], &_indexList[_i] );\
                    _newPivotIndex ++;\
                }\
            }\
\
			SwapIndex ( &_indexList[_newPivotIndex], &_indexList[_right] );\
\
            PushStackSortArrayQuick ( partBoundary, _stackIndex, _right, _newPivotIndex );\
            _right = _newPivotIndex;\
            SelectPivotIndex ( (_list), (_indexList),\
							_left, _right, _pivotIndex, (_info), _CompareFunction );\
		}\
    }\
}// End Function SortIndexQuick



// Partition an index list in-place according to the given mid-value
#define PartitionIndexMidValue( _list, _indexList, _length, _midValue, _CompareFunction, _midIndex )	\
{\
	int _newPivotIndex = 0;\
    for ( int _i = 0; _i < (_length); _i ++ ) {\
        if ( _CompareFunction ( &(_list)[GetListIndex ( (_indexList), _i )],\
							(_midValue) ) ) {\
            SwapIndex ( &(_indexList)[_newPivotIndex], &(_indexList)[_i] );\
            _newPivotIndex ++;\
        }\
    }\
    (_midIndex) = _newPivotIndex;\
}// End Function PartitionIndexMidValue



// Partition an index list in-place without a given mid-value
#define PartitionIndex( _list, _indexList, _length, _TYPE, _info, _CompareFunction, _midIndex )	\
{\
	int _newPivotIndex = 0;\
    for ( int _i = 0; _i < (_length); _i ++ ) {\
        if ( _CompareFunction ( &(_list)[GetListIndex ( (_indexList), _i)], (_info) ) ) {\
            SwapIndex ( &(_indexList)[_newPivotIndex], &(_indexList)[_i] );\
            _newPivotIndex ++;\
        }\
    }\
    (_midIndex) = _newPivotIndex;\
}// End Function PartitionIndex



// Find the nth element in a index list
#define FindNthElementIndex( _list, _indexList, _length, _nthPos, _TYPE, _info, _CompareFunction )	\
{\
	int _left = 0,\
		_right = (_length) - 1;\
	_TYPE *_pivotValue;\
	int _pivotIndex = 0;\
	SelectPivotIndex ( (_list), (_indexList), 0, _right, _pivotIndex, _info, _CompareFunction );\
	_pivotValue = &(_list)[GetListIndex ( (_indexList), _pivotIndex )];\
\
	while ( 1 ) {\
		int _partPos = _left;\
\
		SwapIndex ( &(_indexList)[_pivotIndex], &(_indexList)[_right] );\
		int _index;\
		for ( _index = _left; _index < _right; _index ++ ) {\
			if ( _CompareFunction ( &(_list)[GetListIndex ( (_indexList), _index )],\
									_pivotValue, (_info) ) ) {\
				SwapIndex ( &(_indexList)[_partPos], &(_indexList)[_index] );\
				_partPos ++;\
			}\
		}\
		SwapIndex ( &(_indexList)[_right], &(_indexList)[_partPos] );\
\
		if ( _partPos > _nthPos ) {\
			_right = _partPos - 1;\
		} else if ( _partPos < _nthPos ) {\
			_left = _partPos + 1;\
			_pivotIndex = _left;\
		} else {\
			break;\
		}\
\
		if ( _right - _left <= CUT_OFF_SIZE ) {\
			SortIndexInsertion ( (_list), &(_indexList)[_left], _right - _left + 1,\
								_TYPE, _info, _CompareFunction );\
			break;\
		}\
\
		SelectPivotIndex ( (_list), (_indexList), _left, _right, _pivotIndex, (_info), _CompareFunction );\
		_pivotValue = &(_list)[GetListIndex ( (_indexList), _pivotIndex )];\
	}\
}// End Function FindNthElementIndex


// Generate a 32 bit hash key for 32 and 64 bits value
#define GenHash32( _value )				((*(unsigned int *) &(_value))*0x9e370001UL)
#define GenHash64( _value )				((*(uint64_t *) &(_value))*0x9e37fffffffc0001UL)

#define HASH_NULL_KEY		-1


// Initialize and create the hash table
#define InitHashTable( _header_len, _link_list_len, _hash_table )	\
{\
	int _power = log2_int_c ( (_header_len) );\
	int _table_len = 1 << _power;\
	(_hash_table)->hash_table = alloc_var ( sizeof(int), _table_len + (_link_list_len) );\
	(_hash_table)->shift_bits = 32 - (_power);\
	(_hash_table)->size_bits = (_power) - 1; \
	(_hash_table)->header_len = _table_len;\
	(_hash_table)->link_list_len = (_link_list_len);\
	int _i;\
	for ( _i = 0; _i < _table_len + (_link_list_len); _i ++ ) {\
		(_hash_table)->hash_table[_i] = HASH_NULL_KEY;\
	}\
}



// Release allocated memory in the hash table
#define ReleaseHashTable( _hash_table )	\
{\
	free_var ( (_hash_table)->hash_table );\
	memset ( (_hash_table), 0, sizeof ( *(_hash_table) ) );\
\
}// End Function ReleaseHashTableStatic



// Empty all the elements from hash table
#define FlushHashTable( _hash_table )	\
{ \
	flush_var ( (_hash_table)->hash_table ); \
	int totalCount = (_hash_table)->header_len + \
			 (_hash_table)->link_list_len; \
	int _i; \
	for ( _i = 0; _i < totalCount; _i ++ ) { \
		(_hash_table)->hash_table[_i] = HASH_NULL_KEY; \
	} \
}// End Function FlushHashTableStatic


// Expand the memory if needs a dynamic hash table
#define StaticHash( _hash_table, _dataIndex )
#define DynamicHash( _hash_table, _dataIndex )	{\
	const int _newCount = (_dataIndex) + 1;\
	if ( _newCount > (_hash_table)->link_list_len ) {\
		(_hash_table)->hash_table = expand2_var ( (_hash_table)->hash_table, \
							  (_hash_table)->header_len + _newCount );\
		int *tempHash = &(_hash_table)->hash_table[(_hash_table)->header_len];\
		for ( ; (_hash_table)->link_list_len < _newCount;\
				(_hash_table)->link_list_len ++ ) {\
			tempHash[(_hash_table)->link_list_len] = HASH_NULL_KEY;\
		}\
	}\
}

// Insert an element to the dynamic hash table
#define InsertHash( _hash_table, _dataIndex, _hashKey, _UpdateFunction )	\
{\
	int _keyPosition = (unsigned int) _hashKey & (_hash_table)->size_bits;\
	if ( (_hash_table)->hash_table[_keyPosition] == HASH_NULL_KEY ) {\
		(_hash_table)->hash_table[_keyPosition] = (_dataIndex);\
		_UpdateFunction ( _hash_table, _dataIndex );\
	} else {\
		int _currentIndex = (_hash_table)->hash_table[_keyPosition];\
		int *_dataZone = &(_hash_table)->hash_table[(_hash_table)->header_len];\
		while ( _dataZone[_currentIndex] != HASH_NULL_KEY ) {\
			_currentIndex = _dataZone[_currentIndex];\
		}/* End While */\
		_dataZone[_currentIndex] = (_dataIndex);\
		_UpdateFunction ( _hash_table, _dataIndex );\
	}/* End If ( Key exists ? ) */\
\
}// End Function InsertHashVar



// Find first occurred matched element position from hash table
#define FindFirstHash( _hash_table, _hashKey, _foundPos, _info, _CompareFunction )	\
{\
	int _keyPosition = (unsigned int) _hashKey & (_hash_table)->size_bits;\
    if ( (_hash_table)->hash_table[_keyPosition] == HASH_NULL_KEY ) {\
        (_foundPos) = HASH_NULL_KEY;\
    } else {\
        int _currentIndex = (_hash_table)->hash_table[_keyPosition];\
        const int *_dataZone = &(_hash_table)->hash_table[(_hash_table)->header_len];\
        while ( 1 ) {\
            if ( _CompareFunction ( _currentIndex, (_info) ) ) {\
                (_foundPos) = _currentIndex;\
                break;\
            } else {\
				if ( _dataZone[_currentIndex] != HASH_NULL_KEY ) {\
					/* Go on to the next element */\
					_currentIndex = _dataZone[_currentIndex];\
				} else {\
					(_foundPos) = HASH_NULL_KEY;\
					break;\
				}/* End If ( End of the list ? ) */\
            }/* End If ( Matches ? ) */\
        }/* End While */\
    }/* End If ( Key exists ? ) */\
\
}// End Function FindFirstHash



// Find next matched element position from hash table
#define FindNextHash( _hash_table, _foundPos, _info, _CompareFunction )	\
{\
	const int *_dataZone = &(_hash_table)->hash_table[(_hash_table)->header_len];\
	int _currentIndex = (_foundPos);\
	while ( 1 ) {\
		if ( _dataZone[_currentIndex] != HASH_NULL_KEY ) {\
			/* Go on to the next element */\
			_currentIndex = _dataZone[_currentIndex];\
			if ( _CompareFunction ( _currentIndex, (_info) ) ) {\
				(_foundPos) = _currentIndex;\
				break;\
			}/* End If ( Matches ? ) */\
		} else {\
			(_foundPos) = HASH_NULL_KEY;\
			break;\
		}/* End If ( End of the list ? ) */\
	}/* End While */\
\
}// End Function FindNextHash



// Look up if the data has been inserted into the table, if not, inserts it
#define FindInsertHash( _hash_table, _dataIndex, _hashKey, _foundPos, _info, _CompareFunction, _UpdateFunction )	\
{\
    int _keyPosition = (unsigned int) _hashKey & (_hash_table)->size_bits;\
    if ( (_hash_table)->hash_table[_keyPosition] == HASH_NULL_KEY ) {\
		(_hash_table)->hash_table[_keyPosition] = (_dataIndex);\
		_UpdateFunction ( _hash_table, _dataIndex );\
        (_foundPos) = HASH_NULL_KEY;\
    } else {\
        int _currentIndex = (_hash_table)->hash_table[_keyPosition];\
        int *_dataZone = &(_hash_table)->hash_table[(_hash_table)->header_len];\
        while ( 1 ) {\
            if ( _CompareFunction ( _currentIndex, (_info) ) ) {\
				(_foundPos) = _currentIndex;\
				_UpdateFunction ( _hash_table, _dataIndex );\
                break;\
            } else {\
                if ( _dataZone[_currentIndex] == HASH_NULL_KEY ) {\
					_dataZone[_currentIndex] = (_dataIndex);\
					_UpdateFunction ( _hash_table, _dataIndex );\
					(_foundPos) = HASH_NULL_KEY;\
                    break;\
                } else {\
                    _currentIndex = _dataZone[_currentIndex];\
                }/* End If ( No more element ? ) */\
            }/* End If ( Matches ? ) */\
        }/* End While */\
    }/* End If ( Key exists ? ) */\
\
}// End Function FindInsertHash



// Look up if the data has been inserted into the table, if not, inserts it,
// if so, it replace the one that has equal comparing result
#define FindInsertHashReplace( _hashTable, _dataIndex, _hashKey, _foundPos, _info, _CompareFunction, _UpdateFunction )	\
{\
    int _keyPosition = (unsigned int) (_hashKey) & (_hashTable)->size_bits;\
    if ( (_hashTable)->hash_table[_keyPosition] == HASH_NULL_KEY ) {\
        (_hashTable)->hash_table[_keyPosition] = (_dataIndex);\
        (_foundPos) = HASH_NULL_KEY;\
    } else {\
	int _lastIndex = -((_hashTable)->header_len - _keyPosition);\
        int _currentIndex = (_hashTable)->hash_table[_keyPosition];\
        int *_dataZone = &(_hashTable)->hash_table[(_hashTable)->header_len];\
        while ( 1 ) {\
            if ( _CompareFunction ( _currentIndex, (_info) ) ) {\
				_UpdateFunction ( _hashTable, _dataIndex );\
				_dataZone = &(_hashTable)->hash_table[(_hashTable)->header_len];\
				/* Modify the pointer to make higher cache performance */\
				_dataZone[_lastIndex] = _dataIndex;\
				_dataZone[_dataIndex] = _dataZone[_currentIndex];\
                (_foundPos) = _currentIndex;\
                break;\
            } else {\
                if ( _dataZone[_currentIndex] == HASH_NULL_KEY ) {\
					_dataZone[_currentIndex] = (_dataIndex);\
					_UpdateFunction ( _hashTable, _dataIndex );\
					(_foundPos) = HASH_NULL_KEY;\
                    break;\
                } else {\
					_lastIndex = _currentIndex;\
                    _currentIndex = _dataZone[_currentIndex];\
                }/* End If ( No more element ? ) */\
            }/* End If ( Matches ? ) */\
        }/* End While */\
    }/* End If ( Key exists ? ) */\
\
}// End Function FindAddElementHashStatic

/** list containers **/
#define list_elm( _list, _i )			(&(_list)->list[(_i)*(_list)->elm_size])

#define find_elm_alg_list( _list, _info, _ret_pos, _compare_func )	\
{\
	*(_ret_pos) = -1; \
	int _i; \
	for ( _i = 0; _i < (_list)->num_elm; _i ++ ) { \
		if ( _compare_func ( (_info), list_elm(_list, _i) ) ) { \
			*(_ret_pos) = _i; \
			break; \
		} \
	} \
}

#define remove_elm_alg_list( _list, _info, _compare_func ) \
{ \
	int _i; \
	for ( _i = 0; _i < (_list)->num_elm; _i ++ ) { \
		if ( _compare_func ( (_info), list_elm(_list, _i) ) ) { \
			break; \
		} \
	} \
	if ( _i != (_list)->num_elm ) { \
		if ( (_list)->ops.destroy_callback ) { \
			(_list)->ops.destroy_callback ( list_elm( _list, _i ) ); \
		} \
		(_list)->num_elm --; \
		memcpy ( &(_list)->list[_i], &(_list)->list[(_list)->num_elm], \
			(_list)->elm_size ); \
	} \
}

#define alg_llist_find( _llist, _info, _f_Compare ) \
{ \
}

#define alg_llist_remove ( _llist, _info, _f_Compare ) \
{ \
}

#endif // ALGORITHM_H_INCLUDED
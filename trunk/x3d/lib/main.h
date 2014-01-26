#ifndef X3DTESTUTILITIES_H_INCLUDED
#define X3DTESTUTILITIES_H_INCLUDED


/*
 * Functions' declaration
 */

void variable_memory_test0 ( struct alg_named_params *param );
void variable_memory_test1 ( struct alg_named_params *param );

// Test the error report code in X3dLogOutput
void ReportErrorTest ( void );

// Test the SortArrayQuick function in X3dAlgorithm
void SortArrayQuickTest ( void );

// Test the PartitionArray function in X3dAlgorithm
void PartitionArrayTest ( void );

// Test the SortListQuick function in X3dAlgorithm
void SortListQuickTest ( void );

// Test the FindNthElementArray function in X3dAlgorithm
void FindNthElementArrayTest ( void );

// Test the FindNthElementIndex function in X3dAlgorithm
void FindNthElementIndexTest ( void );

// Test the FindAddElementHashStatic in X3dAlgorithm
void HashLookupTest ( void );


#endif // X3DTESTUTILITIES_H_INCLUDED

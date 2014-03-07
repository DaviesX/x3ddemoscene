/* main.c: unit tests of loader functions go here */
#include "main.h"


#if 0
/* Test to load a couple model file */
void load_model_from_file_test ( struct alg_named_params *np )
{
#define NUM_FILES		3
        char sampleFiles[NUM_FILES][64] = {
                "../Data/Scenes/diamond.obj",
                "../Data/Scenes/teapot.obj",
                "../Data/Scenes/Sponza/sponza.obj"
        };

        // Loading the sample files
        struct resource_piler rp;
        init_resource_piler ( &rp );
        int iCase;
        for ( iCase = 0; iCase < 3; iCase ++ ) {
                int modelId;
                int sceneId = 0X12345678;
                if ( MODEL_ID_INVALID !=
                     (modelId = load_model_from_file ( sampleFiles[iCase], sceneId, &rp )) ) {
                        log_normal ( "Case %d successful ! - %s", iCase, sampleFiles[iCase] );
                } else {
                        log_severe_err ( "Case %d failed ! - ", iCase, sampleFiles[iCase] );
                }
        }
        free_resource_piler ( &rp );
//	dbg_finish ();
#undef NUM_FILES
}

/* Test to load a scene file */
void load_scene_from_file_test ( struct alg_named_params *np )
{
#define NUM_FILES		3
        char sampleFiles[NUM_FILES][64] = {
                "../Data/Scenes/diamond.obj",
                "../Data/Scenes/teapot.obj",
                "../Data/Scenes/Sponza/sponza.obj"
        };

        // Loading the sample files
        struct resource_piler rp;
        init_resource_piler ( &rp );
        int sceneIds[3];
        int iCase;
        for ( iCase = 0; iCase < 3; iCase ++ ) {
                int sceneId;
                if ( sceneIds[iCase] = load_scene_from_file ( sampleFiles[iCase], &rp ) ) {
                        log_normal ( "Case %d successful ! - %s", iCase, sampleFiles[iCase] );
                } else {
                        log_severe_err ( "Case %d failed ! - ", iCase, sampleFiles[iCase] );
                }
        }
        free_resource_piler ( &rp );
//	dbg_finish ();
#undef NUM_FILES
}

/* Test on the resource piler structure */
void resource_piler_test ( struct alg_named_params *np )
{
        struct resource_piler rp;
        init_resource_piler ( &rp );
        res_record_t *rec = create_res_record ( &rp );
        int c0 = begin_entry_resource_piler ( "RootInfo", rec );
        push_data_resource_piler_c ( 0X7FFFFFFF, 4, rec );
        {
                int c1 = begin_entry_resource_piler ( "SubInfo", rec );
                float k = 1.0f;
                push_data_resource_piler ( &k, 4, rec );
                push_data_resource_piler_c ( 2.0f, 4, rec );
                push_data_resource_piler_c ( 1.5f, 4, rec );
                push_data_resource_piler_c ( 100.0f, 4, rec );
                end_entry_resource_piler ( c1, rec );
        }

        {
                int c2 = begin_entry_resource_piler ( "SubInfo", rec );
                push_data_resource_piler_c ( 'c', 1, rec );
                push_data_resource_piler_c ( 'g', 1, rec );
                push_data_resource_piler_c ( 'e', 1, rec );
                push_data_resource_piler_c ( 'f', 1, rec );
                end_entry_resource_piler ( c2, rec );
        }
        end_entry_resource_piler ( c0, rec );

        enter_root_resource_piler ( "RootInfo", rec );
        enter_sub_resource_piler ( "SubInfo", rec );
        {
                set_layer_first_resource_piler ( rec );
                float fdata[4];
                fdata[0] = pop_data_resource_piler_c ( float, rec );
                fdata[1] = pop_data_resource_piler_c ( float, rec );
                fdata[2] = pop_data_resource_piler_c ( float, rec );
                fdata[3] = pop_data_resource_piler_c ( float, rec );
                printf ( "%f\n", fdata[0] );
                printf ( "%f\n", fdata[1] );
                printf ( "%f\n", fdata[2] );
                printf ( "%f\n", fdata[3] );
        }

        {
                set_layer_next_resource_piler ( rec );
                char cdata[4];
                cdata[0] = pop_data_resource_piler_c ( char, rec );
                cdata[1] = pop_data_resource_piler_c ( char, rec );
                cdata[2] = pop_data_resource_piler_c ( char, rec );
                cdata[3] = pop_data_resource_piler_c ( char, rec );
                printf ( "%c\n", cdata[0] );
                printf ( "%c\n", cdata[1] );
                printf ( "%c\n", cdata[2] );
                printf ( "%c\n", cdata[3] );
        }

        leave_node_resource_piler ( rec );
        set_layer_first_resource_piler ( rec );
        int idata = pop_data_resource_piler_c ( int, rec );
        printf ( "%x\n", idata );

        printf ( "memory instances: %d\n", query_mem_inst () );
        printf ( "memory usage: %d bytes\n", query_mem_usage () );
        puts ( "free memory..." );
        free_res_record_s ( rec );
        free_resource_piler ( &rp );
        printf ( "memory instances: %d\n", query_mem_inst () );
        printf ( "memory usage: %d bytes\n", query_mem_usage () );
//	dbg_finish ();
}
#endif

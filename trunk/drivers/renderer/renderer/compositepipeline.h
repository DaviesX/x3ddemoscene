#ifndef COMPOSITEPIPELINE_H_INCLUDED
#define COMPOSITEPIPELINE_H_INCLUDED

#include "fbo.h"


struct compo_pipeline {
};

/*
 * functions' declaration
 */
void compo_colorize ( struct compo_pipeline* pipeline, struct fbo* target );


#endif // COMPOSITEPIPELINE_H_INCLUDED

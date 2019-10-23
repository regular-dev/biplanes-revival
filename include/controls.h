#ifndef CONTROLS_H
#define CONTROLS_H

#include "structures.h"
#include "plane.h"

void collect_local_input();
void collect_opponent_input();
void process_local_controls( Plane&, Controls& );


#endif //CONTROLS_H

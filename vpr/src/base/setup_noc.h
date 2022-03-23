#ifndef SETUP_NOC
#define SETUP_NOC

#include <iostream>
#include <string>
#include <vector>


#include "physical_types.h"
#include "device_grid.h"
#include "globals.h"
#include "noc_storage.h"
#include "vpr_error.h"

#define PHYSICAL_ROUTER_NOT_ASSIGNED -1
#define INVALID_PHYSICAL_ROUTER_INDEX -1



// a data structure to store the position information of a noc router in the FPGA device
struct t_noc_router_tile_position {

    int grid_width_position;
    int grid_height_position;
    
    double tile_centroid_x;
    double tile_centroid_y;

};


void setup_noc(const t_arch& arch);


#endif
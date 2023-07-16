#ifndef PASS_FILTER_H
#define PASS_FILTER_H

#include "Filter.h"


class Pass_filter : public Filter{

    private:

    public:
        Pass_filter(Controller * controller);

        void update_pos(uint16_t pos_uint);

        void update_angle(double phi);

        void recieve_velo(double velo);

};

#endif
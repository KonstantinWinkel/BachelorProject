#include "Controller.h"

class Eigenbewegung : public Controller{
    public:
    Eigenbewegung(){};
    void calculate_u(double x_hat[4],double& u);
};
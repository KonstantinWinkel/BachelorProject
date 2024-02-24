#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller{
	protected:
		double target_state[4] = {0,0,0,0};

	public:
		virtual void update_target_state(double new_target[4]){
			for(int i = 0;i<4;i++)
				target_state[i] = new_target[i];
		}

		//
		virtual void calculate_u(double x_hat[4], double& u) = 0;
};

#endif //CONTROLLER_H


#define FOR_AUTO_LANDING 0

#ifdef FOR_AUTO_LANDING
typedef struct _AUTO_FLIGHT_cValue{
	uint16_t pitch;
	uint16_t roll;
	uint16_t yaw;
	uint16_t vertical;
}AUTO_FLIGHT_cValue;
#endif

#ifdef FOR_AUTO_LANDING
#define AUTO 1
#define HAND_OPERATION 0
#define GOAL_DISTANCE 3
#define GOAL_HEIGHT_HOLDING_TIME_CNT 3000
#endif

int main(void)
{
#ifdef FOR_AUTO_LANDING
	AUTO_FLIGHT_cValue cValue;
	uint8_t distance_to_gnd;
	uint8_t arrive_flag = 0;
	uint8_t landing_flag = 0;
	uint16_t prev_vertical_cValue;
	uint16_t height_holding_time_cnt  = 0;
	uint16_t motor_speed_desc_value = 10500;
#endif

#ifdef FOR_AUTO_LANDING
	while(1)
	{
		if(arrive_flag == 1)
		  {
		  	height_holding_time_cnt++;
		  	
		  	// Height Holding Control //
			// ~~~~~~~~~~~~~~~~~~~~~~ //
			////////////////////////////
			
			if(height_holding_time_cnt >= GOAL_HEIGHT_HOLDING_TIME_CNT)
			{
				arrive_flag = 0;
				height_holding_time_cnt = 0;
				landing_flag = 1;
				prev_vertical_cValue = cValue.vertical; // throttle
			}
		  }
		  
		  /////////////// Auto Landing Control ///////////////
		  if(landing_flag == 1)
		  {
		  	if(distance_to_gnd < 2000)
		  	{
		  		cValue.vertical = prev_vertical_cValue - ???;
		  		prev_vertical_cValue = cValue.vertical;
			}
			else if(distance_to_gnd < 1000)
			{
				cValue.vertical = prev_vertical_cValue - ???;
		  		prev_vertical_cValue = cValue.vertical;
			}
			else if(distance_to_gnd < 500)
			{
				cValue.vertical = prev_vertical_cValue - ???;
		  		prev_vertical_cValue = cValue.vertical;
			}
			else if(distance_to_gnd <= GOAL_DISTANCE)
		  	{
		  		motor_arming_flag = 0;
		  		landing_flag = 2;
			}
		  }
		  ////////////////////////////////////////////////////	
	}
#endif
}
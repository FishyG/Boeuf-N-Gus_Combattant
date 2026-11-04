void Goblet() {
	
	int Pulse_tourne_LEFT;
	int Pulse_tourne_RIGHT;
	int Pulse_recule_LEFT;
	int Pulse_recule_RIGHT;
	int Pulse_retourne_LEFT;
	int Pulse_retourne_RIGHT;
	int Pulse_avance_LEFT;
	int Pulse_avance_RIGHT;
	int distance;
	ENCODER_Reset(LEFT);
	ENCODER_Reset(RIGHT);

	MOTOR_SetSpeed(LEFT,0.2);
	MOTOR_SetSpeed(RIGHT,-0.2);
	
	while ( distance < 200) {
		
		Pulse_tourne_LEFT = ENCODER_Read(LEFT);
		Pulse_tourne_RIGHT = ENCODER_Read(RIGHT);
		distance = AnalogRead(/*Banane*/);
	}
	
	ENCODER_Reset(LEFT);
	ENCODER_Reset(RIGHT);
	MOTOR_SetSpeed(LEFT,-0.1);
	MOTOR_SetSpeed(RIGHT,-0.1);
	
	while (distance > 600){
		
		Pulse_recule_LEFT = ENCODER_Read(LEFT);
		Pulse_recule_RIGHT = ENCODER_Read(RIGHT);
		distance = AnalogRead(/*Banane*/);

	}
	
	/*recule_difference*/
	
	/*fermer_Pince*/
	
	/*avance_difference*/
	
	ENCODER_Reset(LEFT);
	ENCODER_Reset(RIGHT);
	MOTOR_SetSpeed(LEFT,0.1);
	MOTOR_SetSpeed(RIGHT,0.1);
	
	while (Pulse_avance_LEFT < Pulse_recule_LEFT && Pulse_avance_RIGHT < Pulse_recule_RIGHT) {
	
	Pulse_avance_LEFT = ENCODER_Read(LEFT);
	Pulse_avance_RIGHT = ENCODER_Read(RIGHT);
	}
	
	ENCODER_Reset(LEFT);
	ENCODER_Reset(RIGHT);
	MOTOR_SetSpeed(LEFT,-0.2);
	MOTOR_SetSpeed(RIGHT,0.2);
	
	while (Pulse_retourne_LEFT < Pulse_tourne_LEFT && Pulse_retourne_RIGHT < Pulse_tourne_RIGHT) {
	
	Pulse_retourne_LEFT = ENCODER_Read(LEFT);
	Pulse_retourne_RIGHT = ENCODER_Read(RIGHT);
	}	
}

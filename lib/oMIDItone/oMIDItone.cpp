/*
This is the oMIDItone library, to send commands to a pair of MCP4151 digital
pots to change the tone on an otamatone.

I have set it up so there is one 100k pot, and 1 50k pot. The first 512 steps 
oscilate the 50k pot between 0 and 1 so that I maintain the higher resolution,
and the final 256 steps are directly modifying the 50k pot. Make sure the
first pot pin is the 100k and the second is the 50k.

It also has a pin for turning the sound on and off, and a pin for
analog readings to correct the frequency being played.

The oMIDItone class objects will be able to be set to output a specific 
frequency that is within the range measured during their initialization, and 
will try to maintain that frequency as long as pitch correction is enabled and 
they have not been requested to turn off the note.

The class object will also keep track of the lighting animation state (even 
though the animation triggers will be controlled externally), as well 
as control the servos that open and close the mouth whenever the sound output
is set to be on or off.

Copyright 2019 - kiyoshigawa - tim@twa.ninja
*/
/*
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <oMIDItone.h>

oMIDItone::oMIDItone(uint16_t signal_enable_optoisolator, uint16_t speaker_disable_optoisolator, uint16_t cs1, uint16_t cs2, uint16_t feedback, uint16_t servo_l_channel, uint16_t servo_r_channel, uint16_t servo_l_min, uint16_t servo_l_max, uint16_t servo_r_min, uint16_t servo_r_max, uint16_t led_head_array[OM_NUM_LEDS_PER_HEAD], Animation * head_animation)
{
	//Declare default values for variables:
	had_successful_init = false;
	pitch_correction_is_enabled = OM_FREQ_CORRECTION_DEFAULT_ENABLE_STATE;
	servo_is_enabled = OM_SERVO_DEFAULT_ENABLE_STATE;
	smallest_freq = 1000000U; //larger than MIDI note 0 by an order of magnitude
	largest_freq = 0;
	for(int i=0; i<OM_NUM_RESISTANCE_STEPS; i++){
		measured_freqs[i] = 0;
	}
	for(int i=0; i<OM_NUM_FREQ_READINGS; i++){
		recent_freqs[i] = 0;
	}
	freq_reading_index = 0;
	current_freq = OM_NO_FREQ;
	current_desired_freq = OM_NO_FREQ;
	last_analog_read = 1024;
	current_resistance = 0;
	pitch_correction_has_been_compromised = false;
	new_note_dropped = false;

	//set pin variables based on constructor inputs:
	signal_enable_optoisolator_pin = signal_enable_optoisolator;
	speaker_disable_optoisolator_pin = speaker_disable_optoisolator;
	cs1_pin = cs1;
	cs2_pin = cs2;
	analog_feedback_pin = feedback;

	//set up an instanced ADC object for use in the code.
	ADC *adc = new ADC();
	//this line makes the unused variable go away, since we never otherwise directly interact with the adc, only the pointer to it.
	(void)adc;

	//Servo channels
	l_channel = servo_l_channel;
	r_channel = servo_r_channel;

	//Servo position values
	l_min = servo_l_min;
	l_max = servo_l_max;
	r_min = servo_r_min;
	r_max = servo_r_max;

	//set the led array variable for use by the lighting functions
	led_position_array = led_head_array;

	//Set the animation pointer:
	animation = head_animation;
}

/* ----- PUBLIC FUNCTIONS BELOW ----- */

void oMIDItone::init(void)
{
	//start timers.
	last_rising_edge = 0;
	last_servo_update = 0;

	//enable servo outputs:
	pinMode(OM_PCA9685_OE_PIN, OUTPUT);
	digitalWrite(OM_PCA9685_OE_PIN, LOW);

	//Init Servo Library
	servo_controller.begin();
	//change frequency to get ~1us per step:
	servo_controller.setPWMFreq(OM_PCA9685_FREQ);
	//need to wait a bit for the frequency change before continuing:
	delay(10);
	//set default values:
	servo_controller.setPWM(l_channel, 0, l_min);
	servo_controller.setPWM(r_channel, 0, r_min);

	#ifdef OMIDITONE_DEBUG
		//init Serial to allow for manual note setting in debug mode:
		Serial.print("Debug is enabled for oMIDItone on relay pin ");
		Serial.print(signal_enable_optoisolator_pin);
		Serial.println(".");
	#endif

	//set pin modes
	pinMode(cs1_pin, OUTPUT);
	pinMode(cs2_pin, OUTPUT);
	pinMode(signal_enable_optoisolator_pin, OUTPUT);
	pinMode(speaker_disable_optoisolator_pin, OUTPUT);
	pinMode(analog_feedback_pin, INPUT);

	//set up ADC:
	adc->setAveraging(0);
	adc->setResolution(8);
	adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
	adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);

	//turn off relay and all CS pins
	digitalWrite(cs1_pin, HIGH);
	digitalWrite(cs2_pin, HIGH);
	digitalWrite(signal_enable_optoisolator_pin, LOW);
	digitalWrite(speaker_disable_optoisolator_pin, HIGH); //defaults to the speakers being enabled. The startup test will disable them, and then re-enable them when complete.

	//init SPI
	SPI.begin();

	//Play startup tone and save initial resistance to note values.
	bool startup_succeeded = startup_test();
	if(!startup_succeeded){
		#ifdef OMIDITONE_DEBUG
			Serial.print("Init for oMIDItone on relay pin ");
			Serial.print(signal_enable_optoisolator_pin);
			Serial.println(" failed.");
		#endif
	}
}

void oMIDItone::update(void)
{
	//if no note is set, disable the relay and stop checking the current frequency.
	//don't bother with any of the rest if the head can't play the current_note
	if(!can_play_freq(current_desired_freq)){
		//turn off the noise.
		digitalWrite(signal_enable_optoisolator_pin, LOW);
	} else {
		//turn on the noise.
		if(note_start_time > OM_NOTE_WAIT_TIME){
			digitalWrite(signal_enable_optoisolator_pin, HIGH);
		}
		//continuously measure the current frequency and adjust the resistance as needed.
		if(pitch_correction_is_enabled){
			measure_freq();
		}
		//and set the resistance to a jittered value based on the adjusted current_resistance.
		set_jitter_resistance(current_resistance, OM_JITTER);
	}

	//Update servos at the specified animation update rate:
	if(last_servo_update > OM_MIN_TIME_BETWEEN_SERVO_MOVEMENTS){
		//update the face-grabber servos:
		if(servo_is_enabled){
			servo_update();
		}

		//reset the timer to keep updates at the right frequency
		last_servo_update = 0;
	}
}

bool oMIDItone::play_freq(uint32_t freq)
{
	if(can_play_freq(freq)){
		note_start_time = 0;
		set_freq(freq);
		return true;
	} else{
		current_desired_freq = OM_NO_FREQ;
		return false;
	}
}

bool oMIDItone::update_freq(uint32_t freq)
{
	if(can_play_freq(freq)){
		set_freq(freq);
		return true;
	} else {
		current_desired_freq = OM_NO_FREQ;
		return false;
	}
}
void oMIDItone::sound_off(void)
{
	current_desired_freq = OM_NO_FREQ;
	digitalWrite(signal_enable_optoisolator_pin, LOW);
}

void oMIDItone::set_servos(uint16_t position)
{
	uint16_t l_servo_value = map(position, 0, 127, l_min, l_max);
	uint16_t r_servo_value = map(position, 0, 127, r_min, r_max);
	servo_controller.setPWM(l_channel, 0, l_servo_value);
	servo_controller.setPWM(r_channel, 0, r_servo_value);
	pitch_correction_has_been_compromised = true;
}

void oMIDItone::cancel_pitch_correction(void)
{
	pitch_correction_has_been_compromised = true;
}

bool oMIDItone::is_ready(void)
{
	if(had_successful_init && current_desired_freq == OM_NO_FREQ){
		return true;
	} else {
		return false;
	}
}

void oMIDItone::enable_pitch_correction(void)
{
	pitch_correction_is_enabled = true;
}

void oMIDItone::disable_pitch_correction(void)
{
	pitch_correction_is_enabled = false;
}

void oMIDItone::enable_servos(void)
{
	servo_is_enabled = true;
}

void oMIDItone::disable_servos(void)
{
	//disable the servos in the open position:
	servo_is_enabled = false;
}

bool oMIDItone::note_was_dropped(void){
	if(new_note_dropped){
		new_note_dropped = false;
		return true;
	} else {
		return false;
	}
}

/* ----- END PUBLIC FUNCTIONS ----- */
/* ----- PRIVATE FUNCTIONS BELOW ----- */

bool oMIDItone::startup_test(void)
{

	//the first part is all manual control of the resistance value and the signal_enable_optoisolator_pin.

	#ifdef OMIDITONE_DEBUG
		Serial.println("Startup Test Beginning:");
	#endif

	//Turn on the relay to generate sounds:
	digitalWrite(signal_enable_optoisolator_pin, HIGH);

	//Turn off the speaker output to keep your sanity:
	digitalWrite(speaker_disable_optoisolator_pin, LOW);

	//Start counting microseconds since a rising edge to calculate frequencies:
	last_stabilize_time = 0;

	//run a stabilization note for a minute.
	while(last_stabilize_time < OM_TIME_TO_WAIT_FOR_STARTUP_TEST_SOUND){
		set_jitter_resistance(OM_JITTER, OM_JITTER);
	}

	//Confirm the first rising edge before the timeout to make sure we are getting good data.
	last_rising_edge = 0;
	elapsedMillis startup_start_time = 0;

	while(1){
		if(is_rising_edge()){
			last_rising_edge = 0;
			last_freq_measurement = 0;
			break;
		}
		//If it doesn't detect a first rising edge in time, return false so the rest of the controller can continue to function.
		if(startup_start_time > OM_TIME_TO_WAIT_FOR_INIT){
			digitalWrite(signal_enable_optoisolator_pin, LOW);
			current_freq = OM_NO_FREQ;
			return false;
		}
	}

	startup_start_time = 0;
	//iterate through all frequencies with jitter to determine the average frequency for that resistance.
	for(uint16_t resistance = OM_JITTER; resistance <= OM_NUM_RESISTANCE_STEPS-OM_JITTER; resistance++){
		current_resistance = resistance;
		set_jitter_resistance(current_resistance, OM_JITTER);

		//measure the frequency OM_NUM_FREQ_READINGS times:
		//wait for OM_NUM_FREQ_READINGS*OM_INIT_MULTIPLIER rising edge before beginning:
		int num_rising_edges = 0;
		while(num_rising_edges < OM_NUM_FREQ_READINGS*OM_INIT_MULTIPLIER){
			if(is_rising_edge()){
				num_rising_edges++;
				last_rising_edge = 0;
			}
			//If it doesn't detect a rising edge in time mid frequency checking, set the value to the previous value and continue.
			if(startup_start_time > OM_TIME_TO_WAIT_FOR_INIT){
				measured_freqs[current_resistance] = measured_freqs[current_resistance-1];
				break;
			}
		}
		freq_reading_index = 0;
		while(1){
			set_jitter_resistance(current_resistance, OM_JITTER);
			if(is_rising_edge()){
				recent_freqs[freq_reading_index] = last_rising_edge;
				last_rising_edge = 0;
				freq_reading_index++;
			}
			if(freq_reading_index >= OM_NUM_FREQ_READINGS){
				measured_freqs[current_resistance] = average(recent_freqs, OM_NUM_FREQ_READINGS);
				//This will break out of the for loop if the measured frequency is higher than OM_SMALLEST_VIABLE_FREQ (less us)
				if(measured_freqs[current_resistance] < OM_SMALLEST_VIABLE_FREQ){
					//Set resistance to a failure condition to stop iterating through the for loop.
					resistance = OM_NUM_RESISTANCE_STEPS;
					//fill in the rest of the array with the OM_SMALLEST_VIABLE_FREQ to keep the rest of the code working.
					for(int i=resistance; i<OM_NUM_RESISTANCE_STEPS-OM_JITTER; i++){
						measured_freqs[current_resistance] = OM_SMALLEST_VIABLE_FREQ;
					}
				}

				//This is a check to see if the frequency is unreasonably large, and should be thrown out.
				if(current_resistance > 5){ //skip the first few, as there's nothing to compare it to.
					if(measured_freqs[current_resistance] > OM_UNREASONABLY_LARGE_MULTIPLIER*measured_freqs[current_resistance-1]){
						measured_freqs[current_resistance] = measured_freqs[current_resistance-1];
					}
				}

				#ifdef OM_STARTUP_PITCH_MEASUREMENT_DEBUG
					Serial.print("Res->Freq::");
					Serial.print(current_resistance);
					Serial.print("->");
					Serial.println(measured_freqs[current_resistance]);
				#endif

				//reset the timeout when a new frequency measurement has occurred.
				last_freq_measurement = 0;
				break; //break the while loop for this specific resistance value
			}
			//If it doesn't detect a rising edge in time mid frequency checking, set the value to the previous value and continue.
			if(last_freq_measurement > OM_NOTE_TIMEOUT || last_rising_edge > OM_NOTE_TIMEOUT*1000){
				measured_freqs[current_resistance] = measured_freqs[current_resistance-1];
				//reset the timeout counter when breaking a loop for timeout.
				last_freq_measurement = 0;
				break; //break the while loop for this specific resistance value
			}
		} //while (1)
	} //for resistance step loop
	digitalWrite(signal_enable_optoisolator_pin, LOW);

	//End manual control of the signal_enable_optoisolator_pin and resistance number - from here on in, use note_on() and note_off()

	//Set the max_note and min_note variables based on the frequencies measured:
	for(uint16_t i = OM_JITTER; i <= OM_NUM_RESISTANCE_STEPS-OM_JITTER; i++){
		if(measured_freqs[i] > largest_freq){
			largest_freq = measured_freqs[i];
		}
		if(measured_freqs[i] < smallest_freq){
			smallest_freq = measured_freqs[i];
		}
	}
	#ifdef OMIDITONE_DEBUG
		Serial.print("Min Measured Freq in us: ");
		Serial.println(smallest_freq);
		Serial.print("Max Measured Freq in us: ");
		Serial.println(largest_freq);
	#endif

	//if it makes it here the init was successful, and the allowable frequency range is established
	if(smallest_freq <= largest_freq){
		had_successful_init = true;
	} else {
		//only continue when the min and max note values make sense.
		return false;
	}

	//This will only happen if nothing went wrong above and the oMIDItone is ready for use.
	//Turn the speaker output back on now that it's ready to work:
	digitalWrite(speaker_disable_optoisolator_pin, HIGH);
	#ifdef OMIDITONE_DEBUG
		Serial.println("Startup test was successful!");
	#endif
	//and finally return true.
	return true;
}

void oMIDItone::set_freq(uint32_t freq)
{
	//set the current_note:
	current_desired_freq = freq;
	freq_reading_index = 0;
	//set the averaging function to the new frequency in anticipation of the change:
	for(int i=0; i<OM_NUM_FREQ_READINGS; i++){
		recent_freqs[i] = current_desired_freq;
	}
	//set the current_resistance to a value that was previously measured as close to the desired note's frequency.
	current_resistance = freq_to_resistance(current_desired_freq);
}

void oMIDItone::measure_freq(void)
{
	//this first bit is calculating the average continuously and storing it in current_freq
	if(is_rising_edge()){
		//sanity check on the reading - it should never be more than OM_ALLOWABLE_FREQ_READING_VARIANCE percent off of the desired frequency.
		uint32_t low_bound = current_desired_freq*(100-OM_ALLOWABLE_FREQ_READING_VARIANCE)/100;
		uint32_t high_bound = current_desired_freq*(100+OM_ALLOWABLE_FREQ_READING_VARIANCE)/100;
		if((last_rising_edge > low_bound) && (last_rising_edge < high_bound)){
			//if things are compromised, reset the last_rising_edge and start over:
			if(pitch_correction_has_been_compromised){
				last_rising_edge = 0;
				//reset the flag so pitch correction can continue until it is interrupted again.
				pitch_correction_has_been_compromised = false;
				#ifdef OM_PITCH_DEBUG
						Serial.println("Pitch Correction Compromised.");
				#endif
			} else {
				recent_freqs[freq_reading_index] = last_rising_edge;
				last_rising_edge = 0;
				#ifdef OM_PITCH_DEBUG
					Serial.print("Frequency Successfully measured: ");
					Serial.println(recent_freqs[freq_reading_index]);
				#endif
				freq_reading_index++;
			}
		} else {
			//take action as if things are compromised and reset the last_rising_edge to start over:
			last_rising_edge = 0;
			//reset pitch correction flag so the next reading can be used.
			pitch_correction_has_been_compromised = false;
			#ifdef OM_PITCH_DEBUG
				Serial.println("Last_rising_edge out of valid ranges.");
			#endif
		}
	}
	if(freq_reading_index >= OM_NUM_FREQ_READINGS){
		//calculate a new average frequency
		current_freq = average(recent_freqs, OM_NUM_FREQ_READINGS);
		//and reset the counter
		freq_reading_index = 0;
		//only when you've had a valid reading should the frequency be adjusted
		if(current_desired_freq != OM_NO_FREQ){
			adjust_freq();
		}
		//also update the measured_freqs array to be correct for the current resistasnce.
		//TIM: Leaving this commented out for now, seems to prevent drifting over time, but requires occasional hard resets
		//measured_freqs[current_resistance] = current_freq;
	}
}

void oMIDItone::adjust_freq(void)
{
	//the next bit will adjust the current jittered resistance value up or down depending on how close the current_freq is to the desired frequency of the current_note, and store it in the midi_to_resistance array
	if(last_adjust_time > OM_TIME_BETWEEN_FREQ_CORRECTIONS){
		//this determines the allowable range that the frequency can be in to avoid triggering a retune:

		//this is the range of frequencies acceptable for the current pitch-bent note being played.
		uint32_t max_allowable_freq = current_desired_freq*(100-OM_ALLOWABLE_NOTE_ERROR)/100;
		uint32_t min_allowable_freq = current_desired_freq*(100+OM_ALLOWABLE_NOTE_ERROR)/100;

		if(current_freq >= max_allowable_freq && current_freq <= min_allowable_freq){
			//Don't adjust anything.
		} else if(current_freq < max_allowable_freq){
			current_resistance--;
			//Only correct if the resistance is too low
			if(current_resistance < OM_JITTER){
				//prevent the value from overflowing:
				current_resistance = OM_JITTER;
				//if it's bottoming out, increase the largest freq.
				largest_freq = current_freq;
				#ifdef OMIDITONE_DEBUG
					Serial.print("Inverted frequency ");
					Serial.print(current_freq);
					Serial.print(" bottomed out on oMIDItone on relay pin");
					Serial.println(signal_enable_optoisolator_pin);
				#endif
				//stop playing the note if it bottoms out
				current_desired_freq = OM_NO_FREQ;
				//set the new_note_dropped flag:
				new_note_dropped = true;
			}
			last_adjust_time = 0;
			#ifdef OM_PITCH_DEBUG_VERBOSE
				Serial.print("Inverted frequency ");
				Serial.print(current_freq);
				Serial.print(" resistance adjusted to ");
				Serial.println(current_resistance);
			#endif
		} else if(current_freq > min_allowable_freq){
			current_resistance++;
			//Only correct if the resistance is too high
			if(current_resistance > (OM_NUM_RESISTANCE_STEPS-OM_JITTER)){
				//prevent the value from overflowing:
				current_resistance = OM_NUM_RESISTANCE_STEPS-OM_JITTER;
				//TIM: Disabled this because it was wrong often enough to detune the whole thing after a few incidents.
				//the oMIDItone rarely drifts too high anyway, so it sounds better with this off.
				//smallest_freq = current_freq;
				#ifdef OMIDITONE_DEBUG
					Serial.print("Inverted frequency ");
					Serial.print(current_freq);
					Serial.print(" topped out on oMIDItone on relay pin ");
					Serial.println(signal_enable_optoisolator_pin);
				#endif
				//stop playing the note if it topped out
				current_desired_freq = OM_NO_FREQ;
				//set the new_note_dropped flag:
				new_note_dropped = true;
			}
			last_adjust_time = 0;
			#ifdef OM_PITCH_DEBUG_VERBOSE
				Serial.print("Inverted frequency ");
				Serial.print(current_freq);
				Serial.print(" resistance adjusted to ");
				Serial.println(current_resistance);
			#endif
		}
	}//if(last_adjustment_time > MIN_TIME_BETWEEN_FREQUENCY_CORRECTIONS)
}

bool oMIDItone::can_play_freq(uint32_t freq)
{
	//some initial conditions to return false immediately before doing the pitch adjusted frequency calculation to save time
	if(!had_successful_init){
		return false;
	}
	if(freq == OM_NO_FREQ){
		return false;
	}
	if(freq < largest_freq && freq > smallest_freq){
		return true;
	}
	return false;
}

void oMIDItone::servo_update(void)
{
	//If the current_desired_freq is not OM_NO_FREQ, open the mouth to the max position:
	if(current_desired_freq != OM_NO_FREQ){
		servo_controller.setPWM(l_channel, 0, l_max);
		servo_controller.setPWM(r_channel, 0, r_max);
		pitch_correction_has_been_compromised = true;
	} else {
		//Set both channels to the min value, closing the mouth.
		servo_controller.setPWM(l_channel, 0, l_min);
		servo_controller.setPWM(r_channel, 0, r_min);
		pitch_correction_has_been_compromised = true;
	}
}

bool oMIDItone::is_rising_edge(void)
{
	if(last_rising_edge > OM_MIN_TIME_BETWEEN_RISING_EDGE_MEASUREMENTS){
	uint16_t current_analog_read = adc->analogRead(analog_feedback_pin);
		if( current_analog_read > OM_RISING_EDGE_THRESHOLD && last_analog_read < OM_RISING_EDGE_THRESHOLD){
			last_analog_read = current_analog_read;
			return true;
		} else {
			last_analog_read = current_analog_read;
			return false;
		}
	} else {
		return false;
	}
}

uint32_t oMIDItone::average(uint32_t * array, uint16_t num_elements)
{
	uint32_t total = 0;
	for(uint16_t i=0; i<num_elements; i++){
		total = total + array[i];
	}
	return total/num_elements;
}

uint16_t oMIDItone::freq_to_resistance(uint16_t freq)
{
	//iterate through the measured_freqs array and check for when the frequency has gone over the desired frequency by one step.
	for(int i=OM_JITTER+2; i<OM_NUM_RESISTANCE_STEPS-OM_JITTER-2; i++){
		//If the frequency if higher than the current note (less us) then set the midi_to_resistance value and increment the note:
		if(measured_freqs[i] < freq){
			return i;
		}
	}
	//if none of the above matched, return the max value.
	return OM_NUM_RESISTANCE_STEPS-OM_JITTER;
}

void oMIDItone::set_jitter_resistance(uint16_t resistance, uint16_t jitter)
{
	uint16_t current_jitter = random(jitter);
	uint16_t positive = random(1);
	if(positive){
		set_resistance(resistance + current_jitter);
	} else {
		set_resistance(resistance - current_jitter);
	}
}

void oMIDItone::set_resistance(uint16_t resistance)
{
	//The case where we need to oscillate the 50k pot to increase resolution:
	if(resistance >= 0 && resistance <= 512){
		//this is divided by 2, so it returns a number between 0 and 256.
		set_pot(cs1_pin, resistance/2);
		//this sets to either 0 or 1 depending on the modulus with 2.
		if(resistance % 2){
			set_pot(cs2_pin, 0);
		} else {
			set_pot(cs2_pin, 1);
		}
	} else if(resistance > 512 && resistance <= OM_NUM_RESISTANCE_STEPS) {
		//The case where the steps are above 512 means to set the 100k pot to 256 and the other to the current resistance value - 512.
		set_pot(cs1_pin, 256);
		set_pot(cs2_pin, resistance-512);
	} else if(resistance < 0){
		set_pot(cs1_pin, 0);
		set_pot(cs2_pin, 0);
	} else {
		set_pot(cs1_pin, 256);
		set_pot(cs2_pin, 256);
	}
}

void oMIDItone::set_pot(uint16_t CS_pin, uint16_t command_byte)
{
	digitalWrite(CS_pin, LOW); //select chip
	uint16_t byte_high = command_byte >> 8;
	uint16_t byte_low = command_byte & 0xff;
	SPI.transfer(byte_high); //send command first
	SPI.transfer(byte_low); //send value second
	digitalWrite(CS_pin, HIGH); //de-select chip when done
}

/* ----- END PRIVATE FUNCTIONS ----- */
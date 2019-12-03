/*
This is the oMIDItone library, to send commands to a pair of MCP4151 digital
pots to change the tone on an otamatone.

I have set it up so there is one 100k pot, and 1 50k pot. The first 512 steps oscilate
the 50k pot between 0 and 1 so that I maintain the higher resolution,
and the final 256 steps are directly modifying the 50k pot. Make sure the
first pot pin is the 100k and the second is the 50k.

It also has a pin for turning the sound on and off, and a pin for
analog readings to correct the frequency being played.

I should probably implement channel controls in the future.
update: 2019-09-26: channels are implemented for pitch bending, and current_channel is
tracked for the current note.

As of 2019-09-24, I am adding the ability for an oMIDItone head to control servos
using an Adafruit PCA9685 pwm controller, as well as the ability to control lighting
using WS2812 addressable LEDs. These will be set up on a per-head basis, but can share
a single servo controller or WS2812 lighting strip.
Update 2019-09-26: basic lighting and servos implemented, now I want to implement a proper
lighting controller and maybe some servo animations. Changes are ongoing. Expect great things.

I should also add in the ability to handle MIDI control in the future messages for changing the
lighting modes and servo animation modes, as well as for direct control if I want it.

*/

#ifndef OMIDITONE_H
#define OMIDITONE_H

#include <Arduino.h>

//We will need the SPI library to communicate with the MCP4151 chips.
#include <SPI.h>

//We will also use the fancy faster ADC library from pedvide for Teensy:
#include <ADC.h>

//This is for the PCA9685 Servo controller.
#include <i2c_t3.h>
#include <Adafruit_PWMServoDriver.h>

//this is for the LED lighting so the animation for the head can be stored on the head.
#include <lighting_control.h>

//This prints out general debug messages for the head.
//this includes startup test messages and messages that let you know when notes are measured out of range
#define OMIDITONE_DEBUG

//comment this out to disable pitch debug messages
//this will turn on or off output messages about the frequency measurements in the measure_frequency() function:
//#define PITCH_DEBUG

//comment this out to disable verbose frequency adjustment messages for every change in frequency
//#define PITCH_VERBOSE_DEBUG

//comment this out to disable startup test in-depth frequency printouts:
//#define STARTUP_PITCH_MEASUREMENT_DEBUG

//this controls default state of frequency correction
#define FREQUENCY_CORRECTION_DEFAULT_ENABLE_STATE true

//this controls default state of servos
#define SERVO_DEFAULT_ENABLE_STATE true

//number of oMIDItones on this controller - Update as needed.
#define NUM_OMIDITONES 6

//This is a non-valid note number for MIDI to designate no note should be played.
#define NO_NOTE 128

//This is a non-valid channel number for MIDI to designate no current channel.
#define NO_CHANNEL 17

//This is a non-valid resistance value to denote that the note should not be played.
#define NO_RESISTANCE -1

//This is the frequency of the note A - typically 440 Hz.
#define NOTE_A 440

//This is how many MIDI notes there are. It should always be 127+1 for NO_NOTE
#define NUM_MIDI_NOTES 128

//This is the number of MIDI channels. Should always be 16, unless MIDI changed sometime in the last decade.
#define NUM_MIDI_CHANNELS 16

//this is how many resistance steps can be used with the digital pots. The current hardware has 2 digital pots with 256 steps each, for a total of 512.
#define NUM_RESISTANCE_STEPS 765

//This is the analog read threshold for a rising edge to count the frequency.
#define RISING_EDGE_THRESHOLD 50

//this is the % difference that a note can be off to trigger correction, as a number from 0-100
#define ALLOWABLE_NOTE_ERROR 1

//these are the default values for pitch bend range:
#define DEFAULT_MIDI_PITCH_BEND_SEMITONES 2
#define DEFAULT_MIDI_PITCH_BEND_CENTS 0
#define MAX_PITCH_BEND_SEMITONES 24
#define MAX_PITCH_BEND_CENTS 99

//This is the % off that a frequency reading can be before it's determined to be invalid and thrown out, as a number from 0-100:
#define ALLOWABLE_FREQUENCY_READING_VARIANCE 50

//this is to make sure that the rising edge isn't measured too often (in us):
#define MIN_TIME_BETWEEN_RISING_EDGE_MEASUREMENTS 1

//Time to wait between receiving a note and starting to play that note (in ms).
#define NOTE_WAIT_TIME 10

//this is to make sure frequency corrections are not too frequent (in us):
#define TIME_BETWEEN_FREQUENCY_CORRECTIONS 20

//this is a jitter value to randomize the resistance in an attempt to counter the frequency variation around a specific resistance value. it is measured in resistance steps
#define JITTER 1

//This is the number of rising edges to read before computing a new current average frequency.
#define NUM_FREQ_READINGS 5

//This forces the init to run for INIT_MULTIPLIER*NUM_FREQ_READINGS of rising edges before taking the frequency reading on init.
//Hopefully this will reduce or remove the need for the STABILIZATION_TIME startup testing.
#define INIT_MULTIPLIER 17

//This is how long to wait for initial frequency readings on init before declaring failure and marking the object as unavailable in ms.
//if it sounds like things should be working, but it keeps timing out, you may need to increase this value.
#define TIME_TO_WAIT_FOR_INIT 2000

//THis is how long to play an initial note before the startup_test sets midi_freqs. in ms
#define TIME_TO_WAIT_FOR_STARTUP_TEST_SOUND 100

//This is how long to wait for a single reading to timeout. Increase if low notes are reading highter than they should. in ms
#define NOTE_TIMEOUT 2000

//this is a multiplier number to check for unreasonably large frequency measurements.
#define UNREASONABLY_LARGE_MULTIPLIER 2

//this is the center value for pitch bending - if the pitch_bend vlaue is set to this, it will play the note without any pitch bending.
//Teensy MIDI sends a number form -8192 to 8191, so I am using 0 as the center here. Typical MIDI center is 8192, adjust as needed.
#define CENTER_PITCH_BEND 0

//If a frequency is measured at this or higher, it will stop incrementing resistances, as it's too high to measure.
#define MAX_FREQ 300

//This is the address of the Adafruit PCA9685 servo controller used by all the servos on the project.
//If yours has a different I2C adderess bit set, you will need to change it here.
//Note that this code assumed a maximum of 16 servos, with only one controller that will work for all oMIDItone heads.
#define PCA9685_ADDRESS 0x41

//This is for setting the base frequency of the PCA9685. By default it is 240Hz, which results in a ~1us per step pulse length
//Don't mess with this unless you've got a very good reason for needing something other than ~1us per step.
//All the servo max and min pulse length values assume this is set to 240Hz resulting in ~1us pulse steps.
#define PCA9685_FREQUENCY 240

//This is the OE pin for the controller board. One pin controls all the servos, so it's defined at the top of the class instead of per head.
#define PCA9685_OE_PIN 20

//This is the number of LEDs on the WS2812 strip used for the head, including in front, back-top, and back-bottom:
#define NUM_LEDS_PER_HEAD 18

//This is how often lighting and servo updates can be sent in us. (About 60Hz)
#define TIME_BETWEEN_SERVO_MOVEMENTS 16

//this is how much I multiplied the cent frequency ratios by to make things work with integer math
#define CENT_FREQUENCY_RATIO_MULTIPLIER 1000000

//This is an array of MIDI notes and the frequency they correspond to. Turns out it is not needed.
//const float midi_Hz_freqs[NUM_MIDI_NOTES] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};

//This is an array that has converted the midi_freqs_Hz array into an array of integers representing us between rising edges for the note frequencies
const uint32_t midi_freqs[NUM_MIDI_NOTES] = {122309, 115446, 108968, 102848, 97077, 91633, 86490, 81632, 77053, 72727, 68643, 64792, 61154, 57723, 54484, 51427, 48538, 45814, 43243, 40816, 38525, 36363, 34322, 32396, 30578, 28861, 27242, 25712, 24270, 22907, 21622, 20408, 19262, 18181, 17161, 16198, 15289, 14430, 13621, 12856, 12134, 11453, 10810, 10204, 9631, 9090, 8580, 8099, 7644, 7215, 6810, 6428, 6067, 5726, 5405, 5102, 4815, 4545, 4290, 4049, 3822, 3607, 3405, 3214, 3033, 2863, 2702, 2551, 2407, 2272, 2145, 2024, 1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275, 1203, 1136, 1072, 1012, 955, 901, 851, 803, 758, 715, 675, 637, 601, 568, 536, 506, 477, 450, 425, 401, 379, 357, 337, 318, 300, 284, 268, 253, 238, 225, 212, 200, 189, 178, 168, 159, 150, 142, 134, 126, 119, 112, 106, 100, 94, 89, 84, 79};

//this is the ratio that frequencies change from -99 cents to 99 cents, multiplied by CENT_FREQUENCY_RATIO_MULTIPLIER, in this case 1,000,000.
const uint32_t cent_frequency_ratios[199] = {1058851, 1058240, 1057629, 1057018, 1056408, 1055798, 1055188, 1054579, 1053970, 1053361, 1052753, 1052145, 1051537, 1050930, 1050323, 1049717, 1049111, 1048505, 1047899, 1047294, 1046689, 1046085, 1045481, 1044877, 1044274, 1043671, 1043068, 1042466, 1041864, 1041262, 1040661, 1040060, 1039459, 1038859, 1038259, 1037660, 1037060, 1036462, 1035863, 1035265, 1034667, 1034070, 1033472, 1032876, 1032279, 1031683, 1031087, 1030492, 1029897, 1029302, 1028708, 1028114, 1027520, 1026927, 1026334, 1025741, 1025149, 1024557, 1023965, 1023374, 1022783, 1022192, 1021602, 1021012, 1020423, 1019833, 1019244, 1018656, 1018068, 1017480, 1016892, 1016305, 1015718, 1015132, 1014545, 1013959, 1013374, 1012789, 1012204, 1011619, 1011035, 1010451, 1009868, 1009285, 1008702, 1008120, 1007537, 1006956, 1006374, 1005793, 1005212, 1004632, 1004052, 1003472, 1002892, 1002313, 1001734, 1001156, 1000578, 1000000, 999423, 998845, 998269, 997692, 997116, 996540, 995965, 995390, 994815, 994240, 993666, 993092, 992519, 991946, 991373, 990801, 990228, 989657, 989085, 988514, 987943, 987373, 986803, 986233, 985663, 985094, 984525, 983957, 983388, 982821, 982253, 981686, 981119, 980552, 979986, 979420, 978855, 978289, 977725, 977160, 976596, 976032, 975468, 974905, 974342, 973779, 973217, 972655, 972093, 971532, 970971, 970410, 969850, 969290, 968730, 968171, 967612, 967053, 966494, 965936, 965379, 964821, 964264, 963707, 963151, 962594, 962039, 961483, 960928, 960373, 959818, 959264, 958710, 958157, 957603, 957050, 956498, 955945, 955393, 954842, 954290, 953739, 953188, 952638, 952088, 951538, 950989, 950439, 949891, 949342, 948794, 948246, 947698, 947151, 946604, 946058, 945511, 944965, 944420};

class oMIDItone {
	public:
		//constructor function
		oMIDItone(uint16_t signal_enable_optoisolator, uint16_t speaker_disable_optoisolator, uint16_t cs1, uint16_t cs2, uint16_t feedback, uint16_t servo_l_channel, uint16_t servo_r_channel, uint16_t servo_l_min, uint16_t servo_l_max, uint16_t servo_r_min, uint16_t servo_r_max, uint16_t led_head_array[NUM_LEDS_PER_HEAD], Animation * head_animation);

		//this will init the pin modes and set up Serial if it's not already running.
		void init();

		//This should be called during the loop, and it will update the note frequencies and play notes as needed.
		void update();

		//This will tell the oMIDItone to play the note. The note will continue to play until changed or until set to off.
		//note is a MIDI note number. 
		//If the note is out of the init value oMIDItone range, it will not play anything and return false
		//if the note can be played, it will begin playing immediately and return true
		bool note_on(uint16_t note, uint16_t velocity, uint16_t channel);

		//This will set the oMIDItone to stop playing all notes.
		void note_off(uint16_t note);

		//this will set the pitch bend value. This will apply to any notes played on the oMIDItone.
		void set_pitch_bend(int16_t pitch_bend_value, uint8_t pitch_bend_channel);

		//this sets the max pitch bend for the oMIDItone, used when calculating all pitch bends:
		//pitch will bend to a max of semitones + cents above or below the currently_playing_note frequency based on currently_playing_pitch_bend
		void set_max_pitch_bend(uint8_t semitones, uint8_t cents);

		//this will cancel the current pitch correction testing if anything disruptive happens during the testing to avoid incorrect corrections
		void cancel_pitch_correction();

		//this allows manual setting of servos when they are disabled by the above function:
		//position is a value between 0 and 127, 0 being closed, 127 being open.
		void set_servos(uint16_t position);

		//This will return true if init was successful AND there is no current note playing.
		bool is_running();

		//This returns the note number that is currently playing or NO_NOTE if no note is currently playing.
		uint16_t currently_playing_note();

		//this returns the current pitch bend value
		uint16_t currently_playing_pitch_bend(uint16_t channel);

		//these enable and disable frequency correction:
		void enable_pitch_correction();
		void disable_pitch_correction();

		//these enable and disable servos:
		void enable_servos();
		void disable_servos();

		//this stores the lighting animation info for the head:
		Animation * animation;

	private:
		//This will play from 0 resistance value to 512 resistance value and note which resistances
		//correspond to which notes in the note matrix:
		//Returns true if init was successful and false if it was not.
		bool startup_test();

		//this takes the frequency averaging code and puts it into a function to clean up the update function:
		void measure_frequency();

		//This is a function that will change the current_resistance to a different value if it is too far off from the current_frequency.
		void adjust_frequency();

		//This will check if a frequency can be played by an initialized oMIDItone object.
		bool can_play_note(uint8_t note, int16_t pitch_bend);

		//This adjusts a frequency to a pitch-bent value from the base note.
		//note will vary as a proportion of pitch_bend from -8192 to +8192 up to max_pitch_bend_offset cents cents max away from the note frequency.
		uint32_t pitch_adjusted_frequency(uint8_t note, int16_t pitch_bend);

		//This function will open and close the mouth based on the current note valocity
		void servo_update();

		//This will constantly read the analog input and return true when it detects a rising edge signal.
		//It also updates the current and last rising edge time values.
		bool is_rising_edge();

		//a simple averaging function:
		uint32_t average(unsigned long * array, uint16_t num_elements);

		//this function will find a resistance value that was measured as being very near the desired frequency.
		uint16_t frequency_to_resistance(uint16_t frequency);

		//this introduces jittered resistance settings, and should be called every loop to keep the jitter working:
		void set_jitter_resistance(uint16_t resistance, uint16_t jitter);

		//this will take a uint16_t number and set the total resistance value to between 0 and 767 on the board.
		void set_resistance(uint16_t resistance);

		//this will set the CS_pin digital pot's wiper to a value based on byte 1 and byte 2
		void set_pot(uint16_t CS_pin, uint16_t command_byte);

		//This will be set to true if the startup_test was successful:
		bool had_successful_init;

		//this is a variable that controls whether or not frequency correction is enabled:
		bool pitch_correction_is_enabled;

		//this is a variable that controls whether or not servos are enabled
		bool servo_is_enabled;

		//current_note is the number of the MIDI note value that is being played.
		uint16_t current_note;

		//current_velocity is the velocity value associated with the current note.
		uint16_t current_velocity;

		//current_channel is the channel value associated with the current note.
		uint16_t current_channel;

		//this will be set during the startup test to the lowest note registered.
		uint16_t min_note;

		//this will be set during the startup test to the highest note registered.
		uint16_t max_note;

		//this is an array of the most recent measured rising edge average times in us that correspond to a resistance
		uint32_t measured_freqs[NUM_RESISTANCE_STEPS];

		//This is an array of the last NUM_FREQ_READINGS frequency readings for averaging purposes.
		uint32_t recent_freqs[NUM_FREQ_READINGS];

		//this is an index that will count up to NUM_FREQ_READINGS and reset to 0, triggering a frequency measurement average.
		uint16_t freq_reading_index;

		//This is a variable for storing the most recent frequency reading based on the average of the last NUM_FREQ_READINGS readings
		uint32_t current_freq;

		//this is a variable that stores the current desired frequency including pitch bend modifications
		//it cuts down on calculating it every time, since pitch bend uses floating point math, which is much slower than the rest of the code
		uint32_t current_desired_freq;

		//this is the current max offset from the current note in cents when a max pitch bend is reached.
		uint32_t max_pitch_bend_offset;

		//this is to measure the frequency of rising edges produced by the output sound wave
		elapsedMicros last_rising_edge;

		//this is to make sure the frequency correction isn't happening faster than the digital pots can be set
		elapsedMillis last_adjust_time;

		//this is to make sure the frequency correction isn't happening faster than the digital pots can be set
		elapsedMillis last_stabilize_time;

		//This is a time tracker that will force a short pause between notes.
		elapsedMillis note_start_time;

		//this is to allow the oMIDItone to play a note for a bit before it sets the initial midi_freqs
		elapsedMillis startup_stabilization_began;

		//this is to allow the oMIDItone to play a note for a bit before it sets the initial midi_freqs
		elapsedMillis last_frequency_measurement;

		//this is a variable for globally storing the most recent analog reading
		uint16_t last_analog_read;

		//this tracks the current pitch bend for notes on a per-channel basis.
		int16_t current_oMIDItone_pitch_bend[NUM_MIDI_CHANNELS];

		//variable for saving the current resistance value of the digital pots.
		uint16_t current_resistance;

		//pin number tracking:
		uint16_t signal_enable_optoisolator_pin;
		uint16_t speaker_disable_optoisolator_pin;
		uint16_t cs1_pin;
		uint16_t cs2_pin;
		uint16_t analog_feedback_pin;

		//set up an instanced ADC object for use in the code.
		ADC *adc = new ADC();

		//Servo channels - these are the channel for the left and right servo for this head on the servo controller
		uint16_t l_channel;
		uint16_t r_channel;

		//Servo Position values - these set the max and min extent of servo pulse width in us
		uint16_t l_min;
		uint16_t l_max;
		uint16_t r_min;
		uint16_t r_max;

		//this is the servo controller object that will run all the servos:
		Adafruit_PWMServoDriver servo_controller = Adafruit_PWMServoDriver(PCA9685_ADDRESS);

		//This tracks when the previous servo update ran on this head
		elapsedMillis last_servo_update;

		//this is used to cancel a pitch correction if an event occurs that would disturb the timing
		bool pitch_correction_has_been_compromised;

		//this stores the order of the led positions on the lighting controller that correspond to the oMIDItone head.
		uint16_t * led_position_array;
};

#endif
